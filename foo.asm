# MIPS CODE GENERATED FOR CS370, 2023

.data

_L0: .asciiz	"Enter the number of terms in the sequence (0-25): "		# String declaration for writing
_L1: .asciiz	"\n\nFibonacci series:"		# String declaration for writing
_L2: .asciiz	" "		# String declaration for writing

.align 2

A: .space 100		# Global variable declaration

.text


.globl main


fibonacci:			# Function definition


	move $a1, $sp		# Carve out activation record, copy SP
	subi $a1, $a1, 40		# Carve out activation record, copy function size
	sw $ra, ($a1)		# Store return address in memory
	sw $sp 4($a1)		# Store the old stack pointer
	move $sp, $a1		# Make SP the current activation record


	sw $t0, 8($sp)		# Parameter store start of function


			# IF conditional
	move $a0, $sp		# Var LOCAL make a copy of stack pointer
	addi $a0, $a0, 8		# Var LOCAL stack pointer plus offset


	lw $a0, ($a0)		# Expression is a variable
	sw $a0, 12($sp)		# expression store LHS temporarily
	li $a0, 1		# Expression is a constant
	move $a1, $a0		# right hand side needs to be a1
	lw $a0, 12($sp)		# expression restore LHS from memory
	slt $t2, $a0, $a1		# EQUAL expression
	slt $t3, $a1, $a0		# EQUAL expression
	nor $a0, $t2, $t3		# EQUAL expression
	andi $a0, 1		# EQUAL expression
	beq $a0, $0, _L3		# IF branch out
			# IF positive branch
	move $a0, $sp		# Var LOCAL make a copy of stack pointer
	addi $a0, $a0, 8		# Var LOCAL stack pointer plus offset


	lw $a0, ($a0)		# Expression is a variable
	lw $ra ($sp)		# Restore old environment RA
	lw $sp 4($sp)		# Return from function store SP

	jr $ra		# return to the caller
	j _L4		# IF END
_L3:			# End IF / start of ELSE
			# The else if there is an else,
			# otherwise just these lines
_L4:			# End conditional


			# IF conditional
	move $a0, $sp		# Var LOCAL make a copy of stack pointer
	addi $a0, $a0, 8		# Var LOCAL stack pointer plus offset


	lw $a0, ($a0)		# Expression is a variable
	sw $a0, 16($sp)		# expression store LHS temporarily
	li $a0, 0		# Expression is a constant
	move $a1, $a0		# right hand side needs to be a1
	lw $a0, 16($sp)		# expression restore LHS from memory
	slt $t2, $a0, $a1		# EQUAL expression
	slt $t3, $a1, $a0		# EQUAL expression
	nor $a0, $t2, $t3		# EQUAL expression
	andi $a0, 1		# EQUAL expression
	beq $a0, $0, _L5		# IF branch out
			# IF positive branch
	move $a0, $sp		# Var LOCAL make a copy of stack pointer
	addi $a0, $a0, 8		# Var LOCAL stack pointer plus offset


	lw $a0, ($a0)		# Expression is a variable
	lw $ra ($sp)		# Restore old environment RA
	lw $sp 4($sp)		# Return from function store SP

	jr $ra		# return to the caller
	j _L6		# IF END
_L5:			# End IF / start of ELSE
			# The else if there is an else,
			# otherwise just these lines
			# ELSE main body
			# Setting Up Function Call
			# Evaluating function parameters
	move $a0, $sp		# Var LOCAL make a copy of stack pointer
	addi $a0, $a0, 8		# Var LOCAL stack pointer plus offset


	lw $a0, ($a0)		# Expression is a variable
	sw $a0, 20($sp)		# expression store LHS temporarily
	li $a0, 1		# Expression is a constant
	move $a1, $a0		# right hand side needs to be a1
	lw $a0, 20($sp)		# expression restore LHS from memory
	sub $a0, $a0, $a1		# SUBTRACT expression and store in $a0
	sw $a0, 24($sp)		# Store CALL Arg temporarily


			# Placing arguments into T registers
	lw $a0, 24($sp)		# pull out stored Arg
	move $t0, $a0		# Move CALL argument into temp register


	jal fibonacci		# Call the function


	sw $a0, 36($sp)		# expression store LHS temporarily
			# Setting Up Function Call
			# Evaluating function parameters
	move $a0, $sp		# Var LOCAL make a copy of stack pointer
	addi $a0, $a0, 8		# Var LOCAL stack pointer plus offset


	lw $a0, ($a0)		# Expression is a variable
	sw $a0, 28($sp)		# expression store LHS temporarily
	li $a0, 2		# Expression is a constant
	move $a1, $a0		# right hand side needs to be a1
	lw $a0, 28($sp)		# expression restore LHS from memory
	sub $a0, $a0, $a1		# SUBTRACT expression and store in $a0
	sw $a0, 32($sp)		# Store CALL Arg temporarily


			# Placing arguments into T registers
	lw $a0, 32($sp)		# pull out stored Arg
	move $t0, $a0		# Move CALL argument into temp register


	jal fibonacci		# Call the function


	move $a1, $a0		# right hand side needs to be a1
	lw $a0, 36($sp)		# expression restore LHS from memory
	add $a0, $a0, $a1		# ADD expression and store in $a0
	lw $ra ($sp)		# Restore old environment RA
	lw $sp 4($sp)		# Return from function store SP

	jr $ra		# return to the caller
_L6:			# End conditional


	li $a0, 0		# RETURN has no specified value set to 0
	lw $ra, ($sp)		# Restore old RA
	lw $sp, 4($sp)		# Restore old SP

	jr $ra		# Return to the caller


main:			# Function definition


	move $a1, $sp		# Carve out activation record, copy SP
	subi $a1, $a1, 40		# Carve out activation record, copy function size
	sw $ra, ($a1)		# Store return address in memory
	sw $sp 4($a1)		# Store the old stack pointer
	move $sp, $a1		# Make SP the current activation record




	la $a0, _L0		# Loading label address into $a0
	li $v0, 4		# About to write a string
	syscall			# Call write string


	move $a0, $sp		# Var LOCAL make a copy of stack pointer
	addi $a0, $a0, 8		# Var LOCAL stack pointer plus offset


	li $v0, 5		# Preparing to read something
	syscall			# read in value to $v0
	sw $v0, ($a0)		# Store value into memory


	li $a0, 0		# Expression is a constant
	sw $a0 16($sp)		# ASSIGN store RHS temporarily
	move $a0, $sp		# Var LOCAL make a copy of stack pointer
	addi $a0, $a0, 12		# Var LOCAL stack pointer plus offset


	lw $a1 16($sp)		# ASSIGN get RHS temporarily
	sw $a1 ($a0)		# ASSIGN place RHS into memory
_L7:			# WHILE TOP target
	move $a0, $sp		# Var LOCAL make a copy of stack pointer
	addi $a0, $a0, 12		# Var LOCAL stack pointer plus offset


	lw $a0, ($a0)		# Expression is a variable
	sw $a0, 20($sp)		# expression store LHS temporarily
	move $a0, $sp		# Var LOCAL make a copy of stack pointer
	addi $a0, $a0, 8		# Var LOCAL stack pointer plus offset


	lw $a0, ($a0)		# Expression is a variable
	move $a1, $a0		# right hand side needs to be a1
	lw $a0, 20($sp)		# expression restore LHS from memory
	slt $a0, $a0, $a1		# LESSTHAN expression
	beq $a0, $0, _L8		# WHILE branch out
			# Setting Up Function Call
			# Evaluating function parameters
	move $a0, $sp		# Var LOCAL make a copy of stack pointer
	addi $a0, $a0, 12		# Var LOCAL stack pointer plus offset


	lw $a0, ($a0)		# Expression is a variable
	sw $a0, 24($sp)		# Store CALL Arg temporarily


			# Placing arguments into T registers
	lw $a0, 24($sp)		# pull out stored Arg
	move $t0, $a0		# Move CALL argument into temp register


	jal fibonacci		# Call the function


	sw $a0 28($sp)		# ASSIGN store RHS temporarily
	move $a0, $sp		# Var LOCAL make a copy of stack pointer
	addi $a0, $a0, 12		# Var LOCAL stack pointer plus offset


	lw $a0, ($a0)		# Expression is a variable
	move $a1, $a0		# ARRAY move expression to $a1
	sll $a1, $a1, 2		# ARRAY shift left by LOG_WSIZE
	la $a0, A		# EMIT Var global variable
	add $a0, $a0, $a1		# ARRAY add internal offset


	lw $a1 28($sp)		# ASSIGN get RHS temporarily
	sw $a1 ($a0)		# ASSIGN place RHS into memory
	move $a0, $sp		# Var LOCAL make a copy of stack pointer
	addi $a0, $a0, 12		# Var LOCAL stack pointer plus offset


	lw $a0, ($a0)		# Expression is a variable
	sw $a0, 32($sp)		# expression store LHS temporarily
	li $a0, 1		# Expression is a constant
	move $a1, $a0		# right hand side needs to be a1
	lw $a0, 32($sp)		# expression restore LHS from memory
	add $a0, $a0, $a1		# ADD expression and store in $a0
	sw $a0 36($sp)		# ASSIGN store RHS temporarily
	move $a0, $sp		# Var LOCAL make a copy of stack pointer
	addi $a0, $a0, 12		# Var LOCAL stack pointer plus offset


	lw $a1 36($sp)		# ASSIGN get RHS temporarily
	sw $a1 ($a0)		# ASSIGN place RHS into memory


	j _L7		# WHILE jump back
_L8:			# End of WHILE

	li $a0, 0		# Expression is a constant
	sw $a0 24($sp)		# ASSIGN store RHS temporarily
	move $a0, $sp		# Var LOCAL make a copy of stack pointer
	addi $a0, $a0, 12		# Var LOCAL stack pointer plus offset


	lw $a1 24($sp)		# ASSIGN get RHS temporarily
	sw $a1 ($a0)		# ASSIGN place RHS into memory
	la $a0, _L1		# Loading label address into $a0
	li $v0, 4		# About to write a string
	syscall			# Call write string


_L9:			# WHILE TOP target
	move $a0, $sp		# Var LOCAL make a copy of stack pointer
	addi $a0, $a0, 12		# Var LOCAL stack pointer plus offset


	lw $a0, ($a0)		# Expression is a variable
	sw $a0, 28($sp)		# expression store LHS temporarily
	move $a0, $sp		# Var LOCAL make a copy of stack pointer
	addi $a0, $a0, 8		# Var LOCAL stack pointer plus offset


	lw $a0, ($a0)		# Expression is a variable
	move $a1, $a0		# right hand side needs to be a1
	lw $a0, 28($sp)		# expression restore LHS from memory
	slt $a0, $a0, $a1		# LESSTHAN expression
	beq $a0, $0, _L10		# WHILE branch out
	la $a0, _L2		# Loading label address into $a0
	li $v0, 4		# About to write a string
	syscall			# Call write string


	move $a0, $sp		# Var LOCAL make a copy of stack pointer
	addi $a0, $a0, 12		# Var LOCAL stack pointer plus offset


	lw $a0, ($a0)		# Expression is a variable
	move $a1, $a0		# ARRAY move expression to $a1
	sll $a1, $a1, 2		# ARRAY shift left by LOG_WSIZE
	la $a0, A		# EMIT Var global variable
	add $a0, $a0, $a1		# ARRAY add internal offset


	lw $a0, ($a0)		# Expression is a variable array
	li $v0, 1		# About to write a number
	syscall			# Call write number


	move $a0, $sp		# Var LOCAL make a copy of stack pointer
	addi $a0, $a0, 12		# Var LOCAL stack pointer plus offset


	lw $a0, ($a0)		# Expression is a variable
	sw $a0, 32($sp)		# expression store LHS temporarily
	li $a0, 1		# Expression is a constant
	move $a1, $a0		# right hand side needs to be a1
	lw $a0, 32($sp)		# expression restore LHS from memory
	add $a0, $a0, $a1		# ADD expression and store in $a0
	sw $a0 36($sp)		# ASSIGN store RHS temporarily
	move $a0, $sp		# Var LOCAL make a copy of stack pointer
	addi $a0, $a0, 12		# Var LOCAL stack pointer plus offset


	lw $a1 36($sp)		# ASSIGN get RHS temporarily
	sw $a1 ($a0)		# ASSIGN place RHS into memory


	j _L9		# WHILE jump back
_L10:			# End of WHILE

	li $a0, 0		# RETURN has no specified value set to 0
	lw $ra, ($sp)		# Restore old RA
	lw $sp, 4($sp)		# Restore old SP

	li $v0, 10		# Exit from main
	syscall			# EXIT everything


			# END OF FUNCTION
