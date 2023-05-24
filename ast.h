/*

NAME:      Ruby Hollomon
LAB TITLE: Lab 9
DATE:      April 18, 2023
PURPOSE:   Header file for a compiler's abstract syntax tree. Made for Shaun Cooper's CS370

*/

#include<stdio.h>
#include<malloc.h>
#include "symtable.h"

#ifndef AST_H
#define AST_H
int mydebug;

/* define the enumerated types for the AST.  THis is used to tell us what 
sort of production rule we came across */

enum ASTtype {
   A_FUNCTIONDEC,
   A_VARDEC,
   A_WRITE,
   A_COMPOUND,
   A_NUM,
   A_PARAM,
   A_PARAMARR,
   A_RETURN,
   A_READ,
   A_VAR,
   A_VARARRAY,
   A_VARARRAYDEC,
   A_EXPR,
   A_CALL,
   A_ARGS,
   A_WHILE,
   A_ASSIGN,
   A_IF,
   A_IFELSE,
   A_EXPRSTMT
};



// Math Operators
enum AST_OPERATORS {
   A_NONE,
   A_PLUS,
   A_MINUS,
   A_TIMES,
   A_DIVIDE,
   A_MOD,
   A_UNARYMINUS,
   A_LE,
   A_LESS,
   A_GE,
   A_GREATER,
   A_EQUIV,
   A_NOTEQUIV
};



enum AST_MY_DATA_TYPE {
   A_BOOLTYPE,
   A_VOIDTYPE,
   A_INTTYPE
};



/* define a type AST node which will hold pointers to AST structs that will
   allow us to represent the parsed code 
*/
typedef struct ASTnodetype
{
     enum ASTtype type;
     enum AST_OPERATORS operator;
     char * name;
     char * label; /* for use in write statements */
     int value;
     enum AST_MY_DATA_TYPE my_data_type;
     struct SymbTab *symbol;
     struct ASTnodetype *s1,*s2, *next ; /* used for holding IF and WHILE components -- not very descriptive */
} ASTnode;


/* uses malloc to create an ASTnode and passes back the heap address of the newley created node */
ASTnode *ASTCreateNode(enum ASTtype mytype);

// For comparing formal and actual parameters of function call
int check_params(ASTnode *actualParams, ASTnode *formalParams);


/*helper for ASTprint, prints # of spaces passed by argument*/
void PT(int howmany);


/*
 * Preorder print of AST
 */
void ASTprint(int level,ASTnode *p);

#endif // of AST_H
