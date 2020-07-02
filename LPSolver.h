#ifndef LPSOLVER_H_INCLUDED
#define LPSOLVER_H_INCLUDED
#include "SudokuGame.h"

/*Public functions declarations*/
int ILPSolver(sudokuGame *sGame);
int LPSolverGuess(sudokuGame *sGame,double x);
int LPSolverGuessHint(sudokuGame *sGame,int row,int col);

#endif
