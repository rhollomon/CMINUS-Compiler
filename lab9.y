%{
/*

NAME:      Ruby Hollomon
LAB TITLE: Lab 9
DATE:      April 18, 2023
PURPOSE:   YACC code for Shaun Cooper's CS370 Compilers & Automata Theory. Checks syntax and semantics of given CMINUS+
           code according to production rules.

*/


	/* begin specs */
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "ast.h"
#include "symtable.h"
#include "emit.h"

ASTnode *PROGRAM = NULL;

int yylex();

extern int mydebug;
extern int lineno;

// Helps us know how deep we are
int LEVEL = 0;
// Global variable for accumulating needed runtime space
int OFFSET = 0;
// Global variable for accumulation global variable offset
int GOFFSET = 0;
// The largest offset needed for the current function
int MAXOFFSET = 0;

void yyerror (s)
     char *s;
{
  printf ("YACC PARSE ERROR: %s on line %d\n", s, lineno);
  //have it print line number of error
}


%}

%union {
      int value;
      char * string;
      ASTnode * node; // Tree node
      enum AST_MY_DATA_TYPE input_type;
      enum AST_OPERATORS operator_type;
}

%start Program;

%token <value> T_NUM
%token  <string> T_ID T_STRING
%token T_INT T_VOID T_IF T_ELSE T_RETURN T_WHILE T_READ T_WRITE T_LE T_GE T_LESS T_GREATER T_EQUIV T_NOTEQUIV

%type <node> Var_List Var_Declaration Declaration Declaration_List Fun_Declaration Params Compound_Stmt Local_Declarations
%type <node> Statement_List Statement Write_Stmt Expression Simple_Expression Additive_Expression Term Factor Var Call
%type <node> Param_List Param Return_Stmt Read_Stmt Expression_Stmt Args Arg_List Iteration_Stmt Assignment_Stmt Selection_Stmt

%type <input_type> Type_Specifier
%type <operator_type> Addop Relop Multop

%left '|'
%left '&'
%left '+' '-'
%left '*' '/' '%'
%left UMINUS


%%	/* end specs, begin rules */

Program             :   Declaration_List  { PROGRAM = $1; }
                    ;


Declaration_List    : Declaration {$$ = $1; }
					| Declaration Declaration_List
                      {
                        $$ = $1;
                        $$->next = $2;
                      }
				    ;


Declaration         : Var_Declaration  { $$ = $1; }
                    | Fun_Declaration  { $$ = $1; }
                    ;


Var_Declaration     : Type_Specifier Var_List ';'
                      { // add type to all elements in the list
                         ASTnode *p;
                         p = $2;
                         while(p != NULL){ // make sure everything in Var_List is same data type
                              p->my_data_type = $1;

                              // Check each variable in list to see if it's been defined at current level
                              if(Search(p->name, LEVEL, 0) != NULL)
                              { yyerror(p->name);
                                yyerror("Symbol already defined");
                                exit(1);
                              }
                              // Not in symtable, so:
                              // All scalars are 1 byte long, if value is not -1 it isn't an array
                              if(p->value == 0){
                                p->symbol = Insert(p->name, p->my_data_type, SYM_SCALAR, LEVEL, 1, OFFSET);
                                OFFSET = OFFSET + 1;
                              }
                              else // array not a scalar
                              {
                                p->symbol = Insert(p->name, p->my_data_type, SYM_ARRAY, LEVEL, p->value, OFFSET);
                                OFFSET = OFFSET + p->value;
                              }

                              p = p->s1;

                         } // end while

                         $$ = $2;
                      }
                    ;


Var_List            : T_ID
                    { $$ = ASTCreateNode(A_VARDEC);
                      $$->name = $1;
                    }

                    | T_ID ',' Var_List
                    { $$ = ASTCreateNode(A_VARDEC);
                      $$->name = $1;
                      $$->s1 = $3;
                    }

                    | T_ID '[' T_NUM ']'
                    { $$ = ASTCreateNode(A_VARARRAYDEC);
                      $$->name = $1;
                      $$->s2 = ASTCreateNode(A_NUM);
                      $$->value = $3;
                      $$->s2->value = $3;
                    }

                    | T_ID '[' T_NUM ']' ',' Var_List
                    { $$ = ASTCreateNode(A_VARARRAYDEC);
                      $$->name = $1;
                      $$->s2 = ASTCreateNode(A_NUM);
                      $$->value = $3;
                      $$->s2->value = $3;
                      $$->s1 = $6;
                    }
                    ;


Type_Specifier      : T_INT { $$ = A_INTTYPE; }
                    | T_VOID { $$ = A_VOIDTYPE; }
				    ;


Fun_Declaration     : Type_Specifier T_ID
                    { // check if function has been defined
                      if(Search($2, LEVEL, 0) != NULL) {
                        // ID already used
                        yyerror($2);
                        yyerror("Function name already in use");
                        exit(1);
                      } // end if

                      // Not in synbol table, so add it in
                      Insert($2, $1, SYM_FUNCTION, LEVEL, 0, 0);

                      GOFFSET = OFFSET;
                      OFFSET = 2;
                      MAXOFFSET = OFFSET;
                    }
                    '(' Params ')'
                    { struct SymbTab *p;
                      p = Search($2, LEVEL, 0); // not search $2, 0, 0?
                      p->fparams = $5;
                    }
                    Compound_Stmt
                    {

                      $$ = ASTCreateNode(A_FUNCTIONDEC);
                      $$->name = $2;
                      $$->my_data_type = $1;
                      $$->s1 = $5;
                      $$->s2 = $8;

                      $$->symbol = Search($2, LEVEL, 0);
                      $$->symbol->offset = MAXOFFSET;

                      OFFSET = GOFFSET; // resets offset for global variables
                    }
                    ;


Params              : T_VOID
                    {
                      $$ = NULL;
                    }

                    | Param_List {$$ = $1;}
					;


Param_List          : Param { $$ = $1; }

                    | Param ',' Param_List
                    { $$ = $1;
                      $$->next = $3;
                    }
                    ;


Param               : Type_Specifier T_ID '[' ']'
                    {
                      // make sure parameter symbol is not used
                      if(Search($2, LEVEL+1, 0) != NULL){
                        yyerror($2);
                        yyerror("Parameter already used");
                        exit(1);
                      }

                      $$ = ASTCreateNode(A_PARAMARR);
                      $$->my_data_type = $1;
                      $$->name = $2;

                      $$->symbol = Insert($2, $1, SYM_ARRAY, LEVEL+1, 1, OFFSET);
					  OFFSET = OFFSET+1;
                    }

					| Type_Specifier T_ID
					{ // make sure parameter symbol is not used
                      if(Search($2, LEVEL+1, 0) != NULL){
                        yyerror($2);
                        yyerror("Parameter already used");
                        exit(1);
                      }

                      $$ = ASTCreateNode(A_PARAM);
					  $$->my_data_type = $1;
					  $$->name = $2;

					  $$->symbol = Insert($2, $1, SYM_SCALAR, LEVEL+1, 1, OFFSET);
					  OFFSET = OFFSET+1;
					}
                    ;


Compound_Stmt       : '{' {LEVEL++;} Local_Declarations Statement_List '}'
                    { $$ = ASTCreateNode(A_COMPOUND);
                      $$->s1 = $3;
                      $$->s2 = $4;
                      if(mydebug) Display();
                      // Set new maximum offset
                      if(OFFSET > MAXOFFSET){
                        MAXOFFSET = OFFSET;
                      }
                      // Delete everything on current level of symtable
                      // and decrease offset by # of deleted items
                      OFFSET -= Delete(LEVEL);
                      LEVEL--;
                    }
                    ;


Local_Declarations  : /*empty*/ {$$ = NULL;}
                    | Var_Declaration Local_Declarations
                    { $$ = $1;
                      $$->next = $2;
                    }
                    ;


Statement_List      : /*EMPTY*/ {$$ = NULL;}
                    | Statement Statement_List
                    { $$ = $1;
                      $$->next = $2;
                    }
                    ;


Statement           : Expression_Stmt {$$ = $1;}
				    | Compound_Stmt {$$ = $1;}
				    | Selection_Stmt {$$ = $1;}
				    | Iteration_Stmt {$$ = $1;}
				    | Assignment_Stmt {$$ = $1;}
				    | Return_Stmt {$$ = $1;}
				    | Read_Stmt {$$ = $1;}
				    | Write_Stmt {$$ = $1;}
				    ;


Expression_Stmt     : Expression ';'
                    { $$ = ASTCreateNode(A_EXPRSTMT);
                      $$->s1 = $1;
                    }

                    | ';'
                    { $$ = ASTCreateNode(A_EXPRSTMT);
                    }
                    ;


Selection_Stmt      : T_IF '(' Expression ')' Statement
                    { $$ = ASTCreateNode(A_IF);
                      $$->s1 = $3;
                      $$->s2 = $5;
                    }

                    | T_IF '(' Expression ')' Statement T_ELSE Statement
                    { $$ = ASTCreateNode(A_IFELSE);
                      $$->s1 = ASTCreateNode(A_IF);
                      $$->s1->s1 = $3;
                      $$->s1->s2 = $5;
                      $$->s2 = $7;
                    }
                    ;


Expression          : Simple_Expression {$$ = $1;}
					;


Simple_Expression   : Additive_Expression {$$ = $1;}

                    | Additive_Expression Relop Additive_Expression
                    { // makes sure both sides of the expression match
                      if($1->my_data_type != $3->my_data_type){
                        yyerror("Type mismatch");
                        exit(1);
                      }

                      $$ = ASTCreateNode(A_EXPR);
                      $$->operator = $2;
                      $$->s1 = $1;
                      $$->s2 = $3;

                      $$->name = CreateTemp();
					  $$->symbol = Insert($$->name, $1->my_data_type, SYM_SCALAR, LEVEL, 1, OFFSET);
					  OFFSET = OFFSET+1;

                      $$->my_data_type = $$->s1->my_data_type;
                    }
					;


Iteration_Stmt      : T_WHILE '(' Expression ')' Statement
                    { $$ = ASTCreateNode(A_WHILE);
                      $$->s1 = $3;
                      $$->s2 = $5;
                    }
                    ;


Return_Stmt         : T_RETURN ';'
                    { $$ = ASTCreateNode(A_RETURN);
                    }

                    | T_RETURN Expression ';'
                    { $$ = ASTCreateNode(A_RETURN);
                      $$->s1 = $2;
                    }
					;


Read_Stmt           : T_READ Var ';'
                    { $$ = ASTCreateNode(A_READ);
                      $$->s1 = $2;
                    }
                    ;


Write_Stmt          : T_WRITE Expression ';'
                    { $$ = ASTCreateNode(A_WRITE);
                      $$->s1 = $2;
                    }

                    | T_WRITE T_STRING ';'
                    { $$ = ASTCreateNode(A_WRITE);
                      $$->name = $2;
                    }
                    ;


Assignment_Stmt     : Var '=' Simple_Expression ';'
                    { // makes sure both sides of the expression match
                      if($1->my_data_type != $3->my_data_type){
                        yyerror("Type mismatch");
                        exit(1);
                      }

                      $$ = ASTCreateNode(A_ASSIGN);
                      $$->s1 = $1;
                      $$->s2 = $3;

                      $$->name = CreateTemp();
					  $$->symbol = Insert($$->name, $1->my_data_type, SYM_SCALAR, LEVEL, 1, OFFSET);
					  OFFSET = OFFSET+1;

					  $$->my_data_type = $1->my_data_type;
                    }
                    ;


Var                 : T_ID '[' Expression ']'
                    { struct SymbTab *p;
                      p = Search($1, LEVEL, 1);

                      // Make sure variable is defined in table
                      if(p == NULL){
                        // Reference variable not in symbol table
                        yyerror($1);
                        yyerror("Symbol used but not defined");
                        exit(1);
                      } // end if

                      // Make sure variable is an array
                      if(p->SubType != SYM_ARRAY){
                        // Reference to a non SCALAR variable
                        yyerror($1);
                        yyerror("Symbol used not of proper ARRAY type");
                        exit(1);
                      } // end if

                      $$ = ASTCreateNode(A_VARARRAY);
                      $$->name = $1;
                      $$->s1 = $3;

                      $$->symbol = p;
                      $$->my_data_type = p->Declared_Type;
                    }


                    | T_ID
                    { struct SymbTab *p;
                      p = Search($1, LEVEL, 1);

                      // Make sure variable is defined in table
                      if(p == NULL){
                        // Reference variable not in symbol table
                        yyerror($1);
                        yyerror("Symbol used but not defined");
                        exit(1);
                      } // end if

                      // Make sure variable is a scalar
                      if(p->SubType != SYM_SCALAR){
                        // Reference to a non SCALAR variable
                        yyerror($1);
                        yyerror("Symbol used not of proper SCALAR type");
                        exit(1);
                      } // end if

                      $$ = ASTCreateNode(A_VAR);
                      $$->name = $1;

                      $$->symbol = p;
                      $$->my_data_type = p->Declared_Type;
                    }
                    ;


Relop               : T_LE         {$$ = A_LE;}
                    | T_LESS       {$$ = A_LESS;}
                    | T_GREATER    {$$ = A_GREATER;}
                    | T_GE         {$$ = A_GE;}
                    | T_EQUIV      {$$ = A_EQUIV;}
                    | T_NOTEQUIV   {$$ = A_NOTEQUIV;}
                    ;


Additive_Expression : Term {$$ = $1;}

					| Additive_Expression Addop Term
					{ // makes sure both sides of the expression match
                      if($1->my_data_type != $3->my_data_type){
                        yyerror("Type mismatch");
                        exit(1);
                      }

                      $$ = ASTCreateNode(A_EXPR);

					  $$->operator = $2;
					  $$->s1 = $1;
					  $$->s2 = $3;

					  $$->name = CreateTemp();
					  $$->symbol = Insert($$->name, $3->my_data_type, SYM_SCALAR, LEVEL, 1, OFFSET);
					  OFFSET = OFFSET+1;

					  $$->my_data_type = $$->s2->my_data_type;
					}
					;


Addop               : '+'
                    { $$ = A_PLUS; }

                    | '-'
                    { $$ = A_MINUS; }
                    ;


Term                : Factor { $$ = $1; }

                    | Term Multop Factor
                    { // makes sure both sides of the expression match
                      if($1->my_data_type != $3->my_data_type){
                        yyerror("Type mismatch");
                        exit(1);
                      }

                      $$ = ASTCreateNode(A_EXPR);
                      $$->operator = $2;
                      $$->s1 = $1;
                      $$->s2 = $3;

                      $$->name = CreateTemp();
					  $$->symbol = Insert($$->name, $3->my_data_type, SYM_SCALAR, LEVEL, 1, OFFSET);
					  OFFSET = OFFSET+1;

                      $$->my_data_type = $$->s2->my_data_type;
                    }
                    ;


Multop              : '*' { $$ = A_TIMES; }
                    | '/' { $$ = A_DIVIDE; }
                    | '%' { $$ = A_MOD; }
                    ;


Factor              : '(' Expression ')' { $$ = $2; }

                    | T_NUM
                    { $$ = ASTCreateNode(A_NUM);
                      $$->my_data_type = A_INTTYPE;
                      $$->value = $1;
                    }

                    | Var
                    { $$ = $1; }

                    | Call
                    { $$ = $1; }

                    | '-' Factor
                    { $$ = ASTCreateNode(A_EXPR);
                      $$->operator = A_UNARYMINUS;
                      $$->s1 = $2;

                      $$->name = CreateTemp();
					  $$->symbol = Insert($$->name, $2->my_data_type, SYM_SCALAR, LEVEL, 1, OFFSET);
					  OFFSET = OFFSET+1;

                      $$->my_data_type = $$->s1->my_data_type;
                    }
                    ;


Call                : T_ID '(' Args ')'
                    { struct SymbTab *p;
                      p = Search($1, 0, 0);

                      // Make sure name is in table
                      if(p == NULL){
                        yyerror($1);
                        yyerror("Function name not defined");
                        exit(1);
                      }

                      // Name is there; is it a function?
                      if(p->SubType != SYM_FUNCTION){
                        yyerror($1);
                        yyerror("Name is not defined as a function");
                        exit(1);
                      }

                      // Ensure CALL arguments and defined parameters match
                      if(check_params($3, p->fparams) == 0){
                        yyerror($1);
                        yyerror("Actual and formal parameters do not match");
                        exit(1);
                      }

                      $$ = ASTCreateNode(A_CALL);
                      $$->my_data_type = p->Declared_Type;
                      $$->name = $1;
                      $$->s1 = $3;
                    }
                    ;


Args                : Arg_List  {$$ = $1;}
                    | /*empty*/ {$$ = NULL;}
                    ;


Arg_List            : Expression
                    { $$ = ASTCreateNode(A_ARGS);
                      $$->s1 = $1;

                      $$->name = CreateTemp();
					  $$->symbol = Insert($$->name, $1->my_data_type, SYM_SCALAR, LEVEL, 1, OFFSET);
					  OFFSET = OFFSET+1;

                      $$->my_data_type = $$->s1->my_data_type;
                    }

                    | Expression ',' Arg_List
                    { $$ = ASTCreateNode(A_ARGS);
                      $$->s1 = $1;

                      $$->name = CreateTemp();
					  $$->symbol = Insert($$->name, $1->my_data_type, SYM_SCALAR, LEVEL, 1, OFFSET);
					  OFFSET = OFFSET+1;

                      $$->my_data_type = $$->s1->my_data_type;

                      $$->next = $3;
                    }
                    ;




%%	/* end of rules, start of program */

int main(int argc, char *argv[])
{ FILE *fp;
  // Read our input arguments

  int i;
  char s[100];

  // Option -d enable debug mode
  // Option -o next argument is our output file name
  for(i = 0; i < argc; i++){
    if(strcmp(argv[i], "-d") == 0) mydebug = 1; // enable debug

    if(strcmp(argv[i], "-o") == 0){ // store name of output file
      strcpy(s, argv[i+1]);
      strcat(s, ".asm");
    }
  }

  // Open file references by s
  fp = fopen(s, "w");
  if(fp == NULL) {
    printf("Cannot open file %s\n", s);
    exit(1);
  }

  yyparse();
  if(mydebug) printf("Parsing Complete\n\n");
  if(mydebug) Display();// Shows our global variables and functions
  if(mydebug) printf("\n\n\nAST PRINT\n\n\n");
  if(mydebug) ASTprint(0, PROGRAM);

  EMIT(PROGRAM, fp);
}
