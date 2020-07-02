#ifndef EBSOLVER_H_INCLUDED
#define EBSOLVER_H_INCLUDED
#include "SudokuBoard.h"

/* A structure to represent a stack to simulate recursion */
/* Member: int value - represents the currently checked sudoku board value (for this stackNode)*/
/* Member: int cellIndex - represents the currently checked sudoku board cell */
/* Member: StackNode* next - a pointer to the next Node in the stack*/
struct StackNode {
	int value;
	int cellIndex;
	struct StackNode* next;
};

/*Public functions*/
int solveStackEnv(sudokuBoard *sBoard);

#endif
