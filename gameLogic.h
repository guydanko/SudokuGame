#ifndef GAMELOGIC_H_INCLUDED
#define GAMELOGIC_H_INCLUDED

#include "SudokuGame.h"
#include "MoveNode.h"
#include "parser.h"
#include "fileHandler.h"
#include "LPSolver.h"
#include "ebSolver.h"


/*Public functions declarations*/
int solveCommand(sudokuGame *game, char *fileName);
int editCommand(sudokuGame *game, char *fileName);
int markErrorsCommand(sudokuGame *game, int x);
int printBoardCommand(sudokuGame *game);
int setGameCommand(sudokuGame *game, int row, int col, int val);
int undoCommand(sudokuGame *game);
int redoCommand(sudokuGame *game);
int saveCommand(sudokuGame *game, char *fileName);
int guessCommand(sudokuGame *game, float x);
int guessHintCommand(sudokuGame *game, int row, int col);
int validateCommand(sudokuGame *game);
int numOfSolutionsCommand(sudokuGame *game);
int generateCommand(sudokuGame *game, int x, int y);
int hintCommand(sudokuGame *game, int row, int col);
int autofillCommand(sudokuGame *game);
int resetCommand(sudokuGame *game);
int exitCommand(sudokuGame *game);
int gameTurn(sudokuGame *game);


#endif
