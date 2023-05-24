/*

NAME:      Ruby Hollomon
LAB TITLE: Lab 9
DATE:      April 18, 2023
PURPOSE:   Header file for MIPs code generation from CMINUS+ input code

*/

#ifndef EMIT_H
#define EMIT_H
#include "ast.h"

// Preprocessor definitions for space allocation
#define WSIZE 4
#define LOG_WSIZE 2

// Write MIPS code to file from AST
void EMIT(ASTnode *p, FILE *fp);

// Write global variables in MIPS
void EMIT_GLOBALS(ASTnode *p, FILE *fp);

// Adds label into AST for printing; prints MIPS based string into file
void EMIT_STRINGS(ASTnode *p, FILE *fp);

// Generate MIPS code from AST
void EMIT_AST(FILE * fp, ASTnode * p);

// Generate MIPS code for function declaration
void emit_function(FILE * fp, ASTnode * p);

// Generate MIPS code for loading params INSIDE FUNCTION
void emit_params(FILE * fp, ASTnode * p);

// Generate MIPS code for function returns
void emit_return(FILE * fp, ASTnode * p);

// Generate MIPS code for CALL arguments (storage)
void emit_storeargs(FILE * fp, ASTnode * p);

// Generate MIPS code for CALL arguments (loading)
void emit_loadargs(FILE * fp, ASTnode * p);

// Generate MIPS code for function call
void emit_call(FILE * fp, ASTnode * p);

// Generate MIPS code for assigning value to a variable
void emit_assign(FILE * fp, ASTnode * p);

// Generate MIPS code for expression
void emit_expr(FILE * fp, ASTnode * p);

// Generate MIPS code for variable
void emit_var(FILE * fp, ASTnode * p);

// Generate MIPS code for read expression
void emit_read(FILE * fp, ASTnode * p);

// Generate MIPS code for while expression
void emit_while(FILE * fp, ASTnode * p);

// Generate MIPS code for if expression
void emit_if(FILE * fp, ASTnode * p);

// Calls emit_expr
void emit_expressionstmt(FILE * fp, ASTnode * p);

// Helper method for formatting MIPS code
void emit(FILE *fp, char * label, char * command, char * comment);

#endif
