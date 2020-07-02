#ifndef SUDOKUGAME_H_INCLUDED
#define SUDOKUGAME_H_INCLUDED

#include "MoveNode.h"
#include "SudokuBoard.h"


typedef enum gameMode {
    init, ed, so
} MODE;

/* A structure to represent a sudoku game */
/* Member: Mode mode - an enum (defined above) that represents the current mode of the game*/
/* Member: MoveNode* currentMove - a pointer to the current move performed in the game*/
/* Member: sudokuBoard* currBoard - represents the current sudoku board that the user is playing with*/
/* Member: int** currSol - represents the current solution of the board, if necessary*/
/* Member: int markErrors - 1 if user wants to print errors, 0 otherwise*/
typedef struct sudoku_game {
    MODE mode;
    MoveNode *currentMove;
    sudokuBoard *currBoard;
    int **currSol;
    int markErrors;
} sudokuGame;

/*Public functions declarations*/
void initNewGame(sudokuGame *game);
void setMarkErrors(sudokuGame *game, int x);
void printGameBoard(sudokuGame *game);
void clearAllGameMoves(sudokuGame *game);
int gameSetCell(sudokuGame *sudokuGame, int row, int col, int val, int isMultiSet);
void pushGameMove(sudokuGame *game);
int undoMove(sudokuGame *game, int printMessage);
int redoMove(sudokuGame *game);
void undoAllMoves(sudokuGame *game);
void freeSudokuGame(sudokuGame *game);
void get_empty_solution(sudokuGame *sGame);
void freeSolution(sudokuGame *sGame);

/*Debug Methods*/
void printSolution(sudokuGame *sGame);

#endif

