/*

NAME:      Ruby Hollomon
LAB TITLE: Lab 9
DATE:      April 18, 2023
PURPOSE:   C code for a compiler's abstract syntax tree. Made for Shaun Cooper's CS370

*/

#include<stdio.h>
#include<malloc.h>
#include "ast.h" 


/* uses malloc to create an ASTnode and passes back the heap address of the newley created node */
//  PRE:  Ast Node Type
//  POST: PTR To heap memory and ASTnode set and all other pointers set to NULL
ASTnode *ASTCreateNode(enum ASTtype mytype)

{
    ASTnode *p;
    if (mydebug) fprintf(stderr,"Creating AST Node \n");
    p=(ASTnode *)malloc(sizeof(ASTnode));
    p->type=mytype;
    p->s1=NULL;
    p->s2=NULL;
    p->next=NULL;
    p->value=0;
    return(p);
}



/*  Helper function to print tabbing */
//PRE:  Number of spaces desired
//POST:  Number of spaces printed on standard output
void PT(int howmany)
{
   // Print indent of AST
   int i;
   for(i = 0; i < howmany; i++) printf(" ");

} // end PT



//  PRE:  A declaration type
//  POST:  A character string that is the name of the type
//          Typically used in formatted printing
char * ASTtypeToString(enum ASTtype mytype)
{
    switch(mytype){

        case A_INTTYPE:
            return "INT";
            break;

        case A_VOIDTYPE:
            return "VOID";
            break;

        default: printf("ASTtypeToString UNKNOWN type FIX FIX FIX FIX\n");
                 return "UNKNOWN TYPE";
    }
    // Missing

} // end ASTtypeToSring



//  PRE:  An operator type
//  POST:  A character string that is the name of the operator
//          Typically used in formatted printing
char * ASToperatorToString(enum AST_OPERATORS myoperator)
{
    switch(myoperator){

        case A_PLUS:
            return "operator PLUS";
            break;

        case A_MINUS:
            return "operator MINUS";
            break;

        case A_TIMES:
            return "operator TIMES";
            break;

        case A_DIVIDE:
            return "operator DIVIDE";
            break;

        case A_MOD:
            return "operator MODULO";
            break;

        case A_UNARYMINUS:
            return "operator UNARY_MINUS";
            break;

        case A_LE:
            return "operator <=";
            break;

        case A_LESS:
            return "operator <";
            break;

        case A_GE:
            return "operator >=";
            break;

        case A_GREATER:
            return "operator >";
            break;

        case A_EQUIV:
            return "operator ==";
            break;

        case A_NOTEQUIV:
            return "operator !=";
            break;

        default:
            return "STATEMENT";
            break;
    }

} // end ASTtypeToSring



/*  Print out the abstract syntax tree */
// PRE:   PRT to an ASTtree
// POST:  indented output using AST order printing with indentation

void ASTprint(int level,ASTnode *p){

   // Base case
   int i;
   if (p == NULL ) return;

   // Recursive case
   else{

       switch (p->type) {
        case A_VARDEC : 
                     PT(level);
                     printf("Variable declr. %s %s LEVEL %d OFFSET %d\n", ASTtypeToString(p->my_data_type),
                                                                                          p->name,
                                                                                          p->symbol->level,
                                                                                          p->symbol->offset); //put variable type/name, use ASTtypeToString

                     ASTprint(level,p->s1); //For if there are multiple var decls on the same line
                     ASTprint(level,p->next);
                     break;

        case A_VARARRAYDEC:
                    PT(level);
                    printf("Variable decl %s %s LEVEL %d OFFSET %d\n", ASTtypeToString(p->my_data_type),
                                                                                          p->name,
                                                                                          p->symbol->level,
                                                                                          p->symbol->offset);
                    PT(level+1);
                    printf("[\n");
                    ASTprint(level+2, p->s2);
                    PT(level+1);
                    printf("]\n");

                    ASTprint(level+1, p->s1); // for multiple var decls on line
                    ASTprint(level,p->next);
                    break;

        case A_FUNCTIONDEC :  
                     PT(level);
                     printf("Function %s %s LEVEL %d OFFSET %d\n", ASTtypeToString(p->my_data_type),
                                                                                          p->name,
                                                                                          p->symbol->level,
                                                                                          p->symbol->offset);

                     ASTprint(level+1, p->s1); // Parameters
                     ASTprint(level+1, p->s2); // Compound
                     ASTprint(level, p->next);
                     break;

        case A_COMPOUND:
                    PT(level);
                     printf("Compound Statement\n");

                     ASTprint(level+1, p->s1); // Local Decs
                     ASTprint(level+1, p->s2); // Statement list
                     ASTprint(level, p->next);
                    break;

        case A_WRITE:
                    PT(level);
                    if(p->name != NULL){ // If writing string
                        printf("WRITE string %s\n", p->name);
                    }
                    else{ // If writing expression
                        printf("WRITE expression\n");
                        ASTprint(level+1, p->s1);
                    }

                    ASTprint(level, p->next);
                    break;

        case A_NUM:
                    PT(level);
                    printf("NUMBER value %d \n", p->value);

                    //ASTprint(level, p->next);
                    break;

        case A_PARAM:
                    PT(level);
                    if(p->name != NULL) // if parameter is not empty
                        printf("Parameter %s %s LEVEL %d OFFSET %d\n", ASTtypeToString(p->my_data_type),
                                                                                          p->name,
                                                                                        p->symbol->level,
                                                                                        p->symbol->offset);
                    else // If parameter is void
                        printf("Parameter %s\n", ASTtypeToString(p->my_data_type));

                    ASTprint(level, p->next);
                    break;

        case A_PARAMARR:
                    PT(level);
                    if(p->name != NULL) // if parameter is not empty
                        printf("Parameter %s %s[]\n", ASTtypeToString(p->my_data_type), p->name);
                    else // if parameter is void
                        printf("Parameter %s[]\n", ASTtypeToString(p->my_data_type));

                    ASTprint(level, p->next);
                    break;

        case A_RETURN:
                    PT(level);
                    printf("RETURN statement\n");
                    ASTprint(level+1, p->s1); // Expression
                    ASTprint(level, p->next);
                    break;

        case A_READ:
                    PT(level);
                    printf("READ STATEMENT\n");
                    ASTprint(level+1, p->s1); //Var
                    ASTprint(level, p->next);
                    break;

        case A_VAR:
                    PT(level);
                    printf("VARIABLE %s LEVEL %d OFFSET %d\n", p->name,
                                                               p->symbol->level,
                                                               p->symbol->offset);
                    break;

        case A_VARARRAY:
                    PT(level);
                    printf("VARIABLE %s LEVEL %d OFFSET %d\n", p->name,
                                                               p->symbol->level,
                                                               p->symbol->offset);
                    PT(level+1);
                    printf("[\n");
                    ASTprint(level+2, p->s1); // Expression
                    PT(level+1);
                    printf("]\n");
                    break;

        case A_EXPR:
                    PT(level);
                    // Gives operator if operator exists
                    printf("EXPRESSION %s\n", ASToperatorToString(p->operator));

                    ASTprint(level+1, p->s1);
                    ASTprint(level+1, p->s2);
                    //ASTprint(level, p->next);
                    break;

        case A_EXPRSTMT:
                    PT(level);
                    printf("EXPRESSION STATEMENT\n");

                    ASTprint(level+1, p->s1);
                    break;

        case A_CALL:
                    PT(level);
                    printf("CALL STATEMENT of function %s\n", p->name);
                    PT(level+1);
                    printf("(\n");

                    ASTprint(level+2, p->s1); //Args

                    PT(level+1);
                    printf(")\n");
                    break;

        case A_ARGS:
                    PT(level);
                    printf("CALL argument\n");
                    ASTprint(level+1, p->s1); // Arg_List
                    ASTprint(level, p->next);
                    break;

        case A_WHILE:
                    PT(level);
                    printf("WHILE STATEMENT\n");
                    PT(level+1);
                    printf("WHILE expression\n");
                    ASTprint(level+2, p->s1); // expression
                    PT(level+1);
                    printf("WHILE body\n");
                    ASTprint(level+2, p->s2); //statement
                    ASTprint(level, p->next);
                    break;

        case A_ASSIGN:
                    PT(level);
                    printf("ASSIGNMENT STATEMENT\n");
                    ASTprint(level+1, p->s1); // Variable
                    PT(level);
                    printf("is assigned\n");
                    ASTprint(level+1, p->s2); // Simple expression
                    ASTprint(level, p->next);
                    break;

        case A_IF:
                    PT(level);
                    printf("IF STATEMENT\n");
                    PT(level+1);
                    printf("IF expression\n"); // Conditions for IF
                    ASTprint(level+2, p->s1);
                    PT(level+1);
                    printf("IF body\n"); // Body of if statement
                    ASTprint(level+2, p->s2);
                    ASTprint(level, p->next);
                    break;

        case A_IFELSE:
                    ASTprint(level, p->s1);
                    PT(level);
                    printf("ELSE body\n"); // body of else statement
                    ASTprint(level+1, p->s2);
                    //ASTprint(level, p->next);
                    break;

        default: printf("unknown AST Node type %d in ASTprint\n", p->type);


       } // end switch
     } // end else

} // end ASTprint



/*
 * Pre: Pointers to formal and actual parameter lists
 * Post: 0 if parameters do not match type & length, else 1 if they match
 */
int check_params(ASTnode *actualParams, ASTnode *formalParams){

    // Iterate through and check parameter types match
    while(actualParams != NULL && formalParams != NULL){

        // Check if parameters are not the same
        if(actualParams->my_data_type != formalParams->my_data_type)
            return 0;

        actualParams = actualParams->next;
        formalParams = formalParams->next;
    }

    // if lengths are not different
    if(actualParams == NULL && formalParams == NULL){
        return 1;
    }

    // if lengths are different
    return 0;
} // end check_params



/* dummy main program so I can compile for syntax error independently
int main(){
}
/* */
