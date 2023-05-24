# TTILE:   Lab 9 CS370
# NAME:    Ruby Hollomon
# PURPOSE: Makefile for LEX, YACC, ast, emit, & symtable built to read and syntax/semantics check given input CMINUS+ code
# DATE:    April 18, 2023

all: lab9

lab9: lab9.y lab9.l ast.c ast.h symtable.c symtable.h emit.c emit.h
	yacc -d lab9.y
	lex lab9.l
	gcc y.tab.c lex.yy.c ast.c symtable.c emit.c -o lab9

clean:
	rm -f lab9
