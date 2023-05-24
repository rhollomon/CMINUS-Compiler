/*

NAME:      Ruby Hollomon
LAB TITLE: Lab 9
DATE:      April 18, 2023
PURPOSE:   Main file for MIPs code generation from CMINUS+ input code

GRADING LEVEL: e

*/


#include <string.h>
#include <stdlib.h>
#include "emit.h"



/**
 * PRECONDITION: File pointer; possible label, command, comment
 * POSTCONDITION: Formatted output printed to file
 *
 * Emits formatted lines of MIPS code to file
 */
void emit(FILE *fp, char * label, char * command, char * comment){

    if(strcmp("", comment) == 0){
        if(strcmp("", label) == 0)      // if comment is "" and label is ""
            fprintf(fp, "\t%s\t\t\n", command);
        else                            // if comment is ""
            fprintf(fp, "%s:\t%s\t\t\n", label, command);
    } else if(strcmp("", command) == 0 && strcmp("", label) == 0) // if it's just a comment
        fprintf(fp, "\t\t\t# %s\n", comment);
    else if(strcmp("", label) == 0)  // if label is ""
        fprintf(fp, "\t%s\t\t# %s\n", command, comment);
    else                                // if none are ""
        fprintf(fp, "%s:\t%s\t\t# %s\n", label, command, comment);

} // end emit




/**
 * PRECONDITION: Pointer to top of AST, pointer to FILE to print to
 * POSTCONDITION: prints MIPS code into file using helper methods
 *
 * Writes MIPS code header section to file
 */
void EMIT(ASTnode *p, FILE *fp){

    if(p == NULL) return;
    if(fp == NULL) return;

    fprintf(fp, "# MIPS CODE GENERATED FOR CS370, 2023\n\n");
    fprintf(fp, ".data\n\n");

    EMIT_STRINGS(p, fp);

    fprintf(fp,"\n.align 2\n\n");

    EMIT_GLOBALS(p, fp);

    fprintf(fp,"\n.text\n\n");
    fprintf(fp,"\n.globl main\n\n\n");

    EMIT_AST(fp, p);

} // end EMIT




/**
 * PRECONDITION: Pointer to top of AST, pointer to FILE to print to
 * POSTCONDITION: prints MIPS based global variables into file
 *
 * Write global variables to .data section of MIPS file
 */
void EMIT_GLOBALS(ASTnode *p, FILE *fp){

    // If we've reached the end of the tree
    if(p == NULL) return;

    // If we've found a global variable, print to file
    if((p->type == A_VARDEC || p->type == A_VARARRAYDEC) && p->symbol->level == 0)
        fprintf(fp, "%s: .space %d\t\t# Global variable declaration\n", p->name, p->symbol->mysize*WSIZE);

    // Cruise rest of the tree
    EMIT_GLOBALS(p->next, fp);

} // end EMIT_GLOBALS




/**
 * PRECONDITION: Pointer to top of AST, pointer to FILE to print to
 * POSTCONDITION: Adds a label into AST for using strings in write statements
 * POSTCONDITION: Prints MIPS based string into file
 *
 * Adds labels for string into .data section of output file
 */
void EMIT_STRINGS(ASTnode *p, FILE *fp){

    // If we've reached the end of the tree
    if(p == NULL) return;

    // If we've found a string write statement, add label & print to file
    if(p->type == A_WRITE && p->name != NULL){
        p->label = CreateLabel();
        fprintf(fp, "%s: .asciiz\t%s\t\t# String declaration for writing\n", p->label, p->name);
    } // end if

    // Cruise rest of the tree
    EMIT_STRINGS(p->s1, fp);
    EMIT_STRINGS(p->s2, fp);
    EMIT_STRINGS(p->next, fp);

} // end EMIT_STRINGS




/**
 * PRECONDITION: ASTnode pointer to A_FUNCTIONDEC
 * POSTCONDITION: MIPS code generated for a function
 *
 * Writes assembly code for function into file; carves out stack, copies parameters,
 * generates compound statement, and restores RA and SP
 */
void emit_function(FILE * fp, ASTnode * p){

    char s[100];

    emit(fp, p->name,"","Function definition");
    fprintf(fp, "\n\n");

    // Carve out stack for activation record
    emit(fp, "","move $a1, $sp","Carve out activation record, copy SP");
    sprintf(s,"subi $a1, $a1, %d", p->symbol->offset*WSIZE);
    emit(fp,"",s,"Carve out activation record, copy function size");
    emit(fp,"","sw $ra, ($a1)","Store return address in memory");
    sprintf(s,"sw $sp %d($a1)", WSIZE);
    emit(fp,"",s,"Store the old stack pointer");
    emit(fp,"","move $sp, $a1","Make SP the current activation record");
    fprintf(fp, "\n\n");

    // Copy parameters to the formal from registers $t0, etc.
    emit_params(fp, p->s1);
    fprintf(fp, "\n\n");

    // Generate compound statement
    EMIT_AST(fp, p->s2);

    // Restore RA and SP before we return
    emit(fp, "", "li $a0, 0", "RETURN has no specified value set to 0");
    emit(fp, "", "lw $ra, ($sp)", "Restore old RA");
    sprintf(s,"lw $sp, %d($sp)", WSIZE);
    emit(fp, "", s, "Restore old SP");
    fprintf(fp, "\n");

    // Generate implicit return depending on if we're in main
    if(strcmp(p->name, "main") == 0){ // exit the system

        emit(fp,"","li $v0, 10","Exit from main");
        emit(fp,"","syscall\t","EXIT everything");
        fprintf(fp, "\n\n");
        emit(fp, "", "", "END OF FUNCTION");

    } else { // jump back to the caller
        emit(fp, "", "jr $ra", "Return to the caller");
        fprintf(fp, "\n\n");
    } // end else

} // end emit_function




/**
 * PRECONDITION: ASTnode pointer to A_PARAM
 * POSTCONDITION: MIPS code generated for loading param values INSIDE FUNCTION
 *
 * Loads the actual values of a parameter inside a function when they are passed in by CALL
 */
void emit_params(FILE * fp, ASTnode * p){

    char s[100];
    ASTnode * paramPointer = p;
    int i = 0;

    // Iterating through param list and loading in the values
    while(paramPointer != NULL){

        sprintf(s, "sw $t%d, %d($sp)", i, paramPointer->symbol->offset * WSIZE);
        emit(fp, "", s, "Parameter store start of function");

        paramPointer = paramPointer->next;
        i++;
    } // end while

} // end emit_params




/**
 * PRECONDITION: ASTnode pointer to A_CALL
 * POSTCONDITION: MIPS code generated for given function call
 *
 * Generate MIPS code for CALL; evaluates and loads params into temp registers
 * before jumping inside function
 */
void emit_call(FILE * fp, ASTnode * p){

    char s[100];

    emit(fp, "", "", "Setting Up Function Call");

    // Evaluate & temporarily store arguments
    emit(fp, "", "", "Evaluating function parameters");
    emit_storeargs(fp, p->s1);

    // Load arguments into $t0... $t7
    emit(fp, "", "", "Placing arguments into T registers");
    emit_loadargs(fp, p->s1);

    // Call function
    sprintf(s, "jal %s", p->name);
    emit(fp, "", s, "Call the function");
    fprintf(fp, "\n\n");

} // end emit_call




/**
 * PRECONDITION: ASTnode pointer to A_RETURN
 * POSTCONDITION: MIPS code generated for function return statement
 * POSTCONDITION: Result of return stored in $a0; 0 in $a0 if no return parameter
 *
 * Generates MIPS code for function returns, either evaluating expression result or
 * returning 0 by default
 */
void emit_return(FILE * fp, ASTnode * p){

    char s[100];

    // If we have no return parameters
    if(p->s1 == NULL) {
        emit(fp, "", "li $a0, 0", "RETURN has no specified value set to 0");
    } else{ // if we do have a return parameter
        emit_expr(fp, p->s1);
    } // end else

    emit(fp, "", "lw $ra ($sp)", "Restore old environment RA");
    sprintf(s, "lw $sp %d($sp)", WSIZE);
    emit(fp, "", s, "Return from function store SP\n");
    emit(fp, "", "jr $ra", "return to the caller");

} // end emit_return




/**
 * PRECONDITION: ASTnode pointer to A_ARGS
 * POSTCONDITION: MIPS code generated for temporarily storing CALL arguments (used in CALL)
 *
 * Temporarily stores call arguments in memory (used in emit_call)
 */
void emit_storeargs(FILE * fp, ASTnode * p){

    char s[100];
    ASTnode * argPointer = p;

    // Evaluate and temporarily store arguments
    while(argPointer != NULL){

        // Get result of argument expression into $a0
        emit_expr(fp, argPointer->s1);

        sprintf(s, "sw $a0, %d($sp)", argPointer->symbol->offset * WSIZE);
        emit(fp, "", s, "Store CALL Arg temporarily");

        argPointer = argPointer->next;
    } // end while

    fprintf(fp, "\n\n");

} // end emit_storeargs




/**
 * PRECONDITION: ASTnode pointer to A_ARGS
 * POSTCONDITION: MIPS code generated for loading arg values (used in CALL)
 *
 * Loads call arguments into temp variables, throws error if there are too many
 * to store (used in emit_call)
 */
void emit_loadargs(FILE * fp, ASTnode * p){

    char s[100];

    int i = 0;
    ASTnode * argPointer = p;

    // Load arguments into registers $t0... $t7
    while(argPointer != NULL){

        if(i >= 8) { // if we have too many arguments to fit registers
            printf("ERROR: emit_call has been passed too many arguments\n");
            exit(1);
        } // end if

        sprintf(s, "lw $a0, %d($sp)", argPointer->symbol->offset * WSIZE);
        emit(fp, "", s, "pull out stored Arg");
        sprintf(s, "move $t%d, $a0", i);
        emit(fp, "", s, "Move CALL argument into temp register");

        argPointer = argPointer->next;
        i++;
    } // end while

    fprintf(fp, "\n\n");

} // end emit_loadargs





/**
 * PRECONDITION: ASTnode pointer to A_VAR or A_VARARRAY
 * POSTCONDITION: Given address of var stored into $a0
 *
 * Generates MIPS code for var into file for arrays and scalars
 */
void emit_var(FILE * fp, ASTnode * p){

    char s[100];

    // If we have an array, gets the index
    if(p->type == A_VARARRAY) {
        emit_expr(fp, p->s1);
        emit(fp, "", "move $a1, $a0", "ARRAY move expression to $a1");
        sprintf(s, "sll $a1, $a1, %d", LOG_WSIZE);
        emit(fp, "", s, "ARRAY shift left by LOG_WSIZE");
    } // end if

    // we run these for both arrays and scalars
    if(p->symbol->level == 0) { // if we have a global variable
        sprintf(s, "la $a0, %s", p->name); // Load in variable name
        emit(fp, "", s, "EMIT Var global variable");
    } else { // if we have a local variable
        emit(fp, "", "move $a0, $sp", "Var LOCAL make a copy of stack pointer");
        sprintf(s, "addi $a0, $a0, %d", p->symbol->offset * WSIZE);
        emit(fp, "", s, "Var LOCAL stack pointer plus offset");
    } // end else

    // If we have an array, add on to the index
    if(p->type == A_VARARRAY) {
        emit(fp, "", "add $a0, $a0, $a1", "ARRAY add internal offset");
    } // end if

    fprintf(fp, "\n\n");

} // end emit_var




/**
 * PRECONDITION: ASTnode pointer to A_ASSIGN
 * POSTCONDITION: Given value stored into memory
 *
 * Generates MIPS code into file for assigning values to array and scalar variables
 */
void emit_assign(FILE * fp, ASTnode * p){

    char s[100];

    // Emit expression of assignment; stored in $a0
    emit_expr(fp, p->s2);

    sprintf(s, "sw $a0 %d($sp)", p->symbol->offset*WSIZE);
    emit(fp, "", s, "ASSIGN store RHS temporarily");

    // Emit code for var
    emit_var(fp, p->s1);

    sprintf(s, "lw $a1 %d($sp)", p->symbol->offset*WSIZE);
    emit(fp, "", s, "ASSIGN get RHS temporarily");

    emit(fp, "", "sw $a1 ($a0)", "ASSIGN place RHS into memory");

}// end emit_assign




/**
 * PRECONDITION: ASTnode pointer to A_WRITE
 * POSTCONDITION: MIPS code generated for writing string or number
 *
 * Generates MIPS code into file for writing strings and expressions
 */
void emit_write(FILE * fp, ASTnode * p){

    char s[100];

    if(p->name != NULL){ // if we're writing a string

        // Load address of label into $a0
        sprintf(s,"la $a0, %s", p->label);
        emit(fp, "", s, "Loading label address into $a0");
        emit(fp, "", "li $v0, 4", "About to write a string");
        emit(fp, "", "syscall\t", "Call write string");
        fprintf(fp, "\n\n");

    } else{ // if we're writing an expression
        emit_expr(fp, p->s1);
        // Pull expression result out of a0 and write it
        emit(fp, "", "li $v0, 1", "About to write a number");
        emit(fp, "", "syscall\t", "Call write number");
        fprintf(fp, "\n\n");
    } // end else

} // end emit_write




/**
 * PRECONDITION: ASTnode pointer to expression family or NULL
 * POSTCONDITION: Result of expression stored in register a0
 *
 * Generates MIPS code for expression evaluation into file
 */
void emit_expr(FILE * fp, ASTnode * p){

    char s[100];

    // For unary minus
    if(p == NULL) return;

    // Base cases: depends on type of expression
    switch(p->type){
        case A_NUM:
            sprintf(s,"li $a0, %d", p->value);
            emit(fp, "", s, "Expression is a constant");
            return;
            break;

        case A_VAR:
            emit_var(fp, p);
            emit(fp, "", "lw $a0, ($a0)", "Expression is a variable");
            return;
            break;

        case A_VARARRAY:
            emit_var(fp, p);
            emit(fp, "", "lw $a0, ($a0)", "Expression is a variable array");
            return;
            break;

        case A_CALL: // call sets $a0
            emit_call(fp, p);
            return;
            break;

        case A_EXPR:
            break; // handled after switch

        default: // Should never be here
            printf("ERROR inside emit_expr FIX FIX FIX");
            exit(1);
            break;
    } // end switch

    // We have an expression

    emit_expr(fp, p->s1);
    // store $a0 in the offset + stack pointer of p->symbol->offset
    sprintf(s, "sw $a0, %d($sp)", p->symbol->offset*WSIZE);
    emit(fp, "", s, "expression store LHS temporarily");
    emit_expr(fp, p->s2); // this overwrites $a0
    emit(fp, "", "move $a1, $a0", "right hand side needs to be a1");
    // load $a0 with stack pointer plus p->symbol->offset
    sprintf(s, "lw $a0, %d($sp)", p->symbol->offset*WSIZE);
    emit(fp, "", s, "expression restore LHS from memory");

    // Now we have the left operaand in $a0 and the right operand in $a1

    // Case statement for operator
    switch(p->operator){

        case A_PLUS:
            emit(fp, "", "add $a0, $a0, $a1", "ADD expression and store in $a0");
            return;
            break;

        case A_MINUS:
            emit(fp, "", "sub $a0, $a0, $a1", "SUBTRACT expression and store in $a0");
            return;
            break;

        case A_TIMES:
            emit(fp, "", "mult $a0, $a1", "MULTIPLY expression");
            emit(fp, "", "mflo $a0", "MULTIPLY expression");
            return;
            break;

        case A_DIVIDE:
            emit(fp, "", "div $a0, $a1", "DIVIDE expression");
            emit(fp, "", "mflo $a0", "DIVIDE expression");
            return;
            break;

        case A_MOD:
            emit(fp, "", "div $a0, $a1", "MOD expression");
            emit(fp, "", "mfhi $a0", "MOD expression");
            return;
            break;

        case A_UNARYMINUS:
            emit(fp, "", "sub $a0, $0, $a0", "UNARY MINUS expression");
            return;
            break;

        case A_LE:
            emit(fp, "", "add $a1 ,$a1, 1", "EXPR LE add one to compare");
            emit(fp, "", "slt $a0 ,$a0, $a1", "LE expression");
            return;
            break;

        case A_LESS:
            emit(fp, "", "slt $a0, $a0, $a1", "LESSTHAN expression");
            return;
            break;

        case A_GE:
            emit(fp, "", "add $a0, $a0, 1", "EXPR GE add one to compare");
            emit(fp, "", "slt $a0, $a1, $a0", "GE expression");
            return;
            break;

        case A_GREATER:
            emit(fp, "", "slt $a0, $a1, $a0", "GREATERTHAN expression");
            return;
            break;

        case A_EQUIV:
            emit(fp, "", "slt $t2, $a0, $a1", "EQUAL expression");
            emit(fp, "", "slt $t3, $a1, $a0", "EQUAL expression");
            emit(fp, "", "nor $a0, $t2, $t3", "EQUAL expression");
            emit(fp, "", "andi $a0, 1", "EQUAL expression");
            return;
            break;

        case A_NOTEQUIV:
            emit(fp, "", "slt $t2, $a0, $a1", "NOTEQUAL expression");
            emit(fp, "", "slt $t3, $a1, $a0", "NOTEQUAL expression");
            emit(fp, "", "or $a0, $t2, $t3", "NOTEQUAL expression");
            return;
            break;

        default: // Should never be here
            printf("ERROR unknown operator for emit_expr FIX FIX FIX");
            exit(1);
            break;

    } // end switch

} // end emit_expr




/**
 * PRECONDITION: ASTnode pointer to A_READ
 * POSTCONDITION: MIPS code to read in a value and store in the VAR of read
 *
 * Generates MIPS code for reading input into a variable
 */
void emit_read(FILE * fp, ASTnode * p){

    char s[100];

    emit_var(fp, p->s1); // $a0 is location of value
    emit(fp, "", "li $v0, 5", "Preparing to read something");
    emit(fp, "", "syscall\t", "read in value to $v0");
    emit(fp, "", "sw $v0, ($a0)", "Store value into memory");
    fprintf(fp, "\n\n");

} // end read_expr




/**
 * PRECONDITION: ASTnode pointer to A_WHILE
 * POSTCONDITION: MIPS code for a while loop structure
 *
 * Generates MIPS code for while loop into file
 */
void emit_while(FILE * fp, ASTnode * p){
    char s[100];

    char * L1 = CreateLabel();
    char * L2 = CreateLabel();

    emit(fp, L1, "", "WHILE TOP target");

    emit_expr(fp, p->s1); // generate code for expression

    // Result is in $a0 so see if we jump to L2 or not
    sprintf(s, "beq $a0, $0, %s", L2);
    emit(fp, "", s, "WHILE branch out");

    // Emit while body in p->s2
    EMIT_AST(fp, p->s2);
    fprintf(fp, "\n\n");

    // Return to start of while
    sprintf(s, "j %s", L1);
    emit(fp, "", s, "WHILE jump back");

    emit(fp, L2, "", "End of WHILE");
    fprintf(fp, "\n");

} // end emit_while




/**
 * PRECONDITION: ASTnode pointer to A_IF or A_IFELSE
 * POSTCONDITION: MIPS code for an if (and potentially else) structure
 *
 * Generates MIPS code for if and if/else statement into file
 */
void emit_if(FILE * fp, ASTnode * p){
    char s[100];

    char * L1 = CreateLabel();
    char * L2 = CreateLabel();

    // Emit main IF expression (only if IF)
    emit(fp, "", "", "IF conditional");
    if(p->type == A_IF)
        emit_expr(fp, p->s1);
    else // if type is A_IFELSE
        emit_expr(fp, p->s1->s1);

    // See if we jump out of IF or not
    sprintf(s, "beq $a0, $0, %s", L1);
    emit(fp, "", s, "IF branch out");

    // Emit IF body
    emit(fp, "", "", "IF positive branch");
    if(p->type == A_IF) // if type is A_IF
        EMIT_AST(fp, p->s2);
    else // if type is A_IFELSE
        EMIT_AST(fp, p->s1->s2);

    sprintf(s, "j %s", L2);
    emit(fp, "", s, "IF END");

    // Else
    emit(fp, L1, "", "End IF / start of ELSE");
    emit(fp, "", "", "The else if there is an else,");
    emit(fp, "", "", "otherwise just these lines");

    // emit ELSE body or nothing
    if(p->type == A_IFELSE){
        emit(fp, "", "", "ELSE main body");
        EMIT_AST(fp, p->s2);
    }

    emit(fp, L2, "", "End conditional");
    fprintf(fp, "\n\n");

} // end emit_if




/**
 * PRECONDITION: Pointer to ASTnode A_EXPRSTMT
 * POSTCONDITION: EMIT_AST called on A_EXPR or NULL
 *
 * Feed expression node into tree
 */
void emit_expressionstmt(FILE * fp, ASTnode * p){

    EMIT_AST(fp, p->s1);

} // end emit_expressionstmt




/**
 * PRECONDITION: Pointer to ASTnode or NULL
 * POSTCONDITION: MIPS code generated from tree into output file
 *
 * Reads through AST and calls relevant code generation functions on each node type
 */
void EMIT_AST(FILE * fp, ASTnode * p){

    // Base case
    if(p == NULL) return;

    // Recursive case
    switch (p->type) {
        case A_VARDEC: // No real action
                     EMIT_AST(fp,p->next);
                     break;

        case A_VARARRAYDEC: // No real action
                    EMIT_AST(fp,p->next);
                    break;

        case A_FUNCTIONDEC:
                     emit_function(fp, p);
                     EMIT_AST(fp, p->next);
                     break;

        case A_COMPOUND:
                        // No real action for s1, vardecs already dealt with
                     EMIT_AST(fp, p->s2);
                     EMIT_AST(fp, p->next);
                    break;

        case A_WRITE:
                    emit_write(fp, p);

                    EMIT_AST(fp, p->next);
                    break;

        case A_NUM:
                    EMIT_AST(fp, p->next);
                    break;

        case A_PARAM:
                    EMIT_AST(fp, p->next);
                    break;

        case A_PARAMARR:
                    EMIT_AST(fp, p->next);
                    break;

        case A_RETURN:
                    emit_return(fp, p);
                    EMIT_AST(fp, p->next);
                    break;

        case A_READ:
                    emit_read(fp, p);
                    EMIT_AST(fp, p->next);
                    break;

        case A_VAR:
                    EMIT_AST(fp, p->next);
                    break;

        case A_VARARRAY:
                    EMIT_AST(fp, p->next);
                    break;

        case A_EXPR:
                    EMIT_AST(fp, p->next);
                    break;

        case A_EXPRSTMT:
                    emit_expressionstmt(fp, p);
                    EMIT_AST(fp, p->next);
                    break;

        case A_CALL:
                    emit_call(fp, p);
                    EMIT_AST(fp, p->next);
                    break;

        case A_ARGS:
                    EMIT_AST(fp, p->next);
                    break;

        case A_WHILE:
                    emit_while(fp, p);
                    EMIT_AST(fp, p->next);
                    break;

        case A_ASSIGN:
                    emit_assign(fp, p);
                    EMIT_AST(fp, p->next);
                    break;

        case A_IF:
                    emit_if(fp, p);
                    EMIT_AST(fp, p->next);
                    break;

        case A_IFELSE:
                    emit_if(fp, p);
                    EMIT_AST(fp, p->next);
                    break;

        default: printf("ERROR: Case %d not implemented in EMIT_AST\n", p->type);
                 exit(1);


       } // end switch
} // end EMIT_AST
