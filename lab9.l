/*

NAME:      Ruby Hollomon
LAB TITLE: Lab 9
DATE:      April 18, 2023
PURPOSE:   LEX code for Shaun Cooper's CS370 Compilers & Automata Theory. Tokenizes given CMINUS+
           input for use by YACC.

*/

%{

int mydebug=0;
int lineno = 1;
#include "ast.h"
#include "symtable.h"
#include "emit.h"
#include "y.tab.h"
%}


%%
int                         { return(T_INT);}

void                        { return (T_VOID); }

if                          { return(T_IF);}

else                        { return(T_ELSE);}

while                       { return(T_WHILE);}

return                      { return(T_RETURN);}

read                        { return(T_READ);}

write                       { return(T_WRITE);}

\".*\"                      {yylval.string=strdup(yytext);
                             return(T_STRING);}

"<="                          { return(T_LE);}

">="                          { return(T_GE);}

"<"                           { return(T_LESS);}

">"                           { return(T_GREATER);}

"=="                          { return(T_EQUIV);}

"!="                          { return(T_NOTEQUIV);}

[a-zA-Z]+[a-zA-Z0-9_]*		{ yylval.string=strdup(yytext);
                              return(T_ID);}

[0-9][0-9]*	                { yylval.value=atoi((const char *)yytext);
                              return(T_NUM);}

[ \t]		                {}

[;=\-+*/\%&|(),\[\]{}<>] 	{ return (*yytext);}

\n 	                   	    {
                              lineno++;
                            }

\/\/.*\n                    { lineno++; }

%%

int yywrap(void)
{ return 1;}


