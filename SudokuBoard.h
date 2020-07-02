#ifndef SUDOKUBOARD_H_INCLUDED
#define SUDOKUBOARD_H_INCLUDED


/* A helping structure to represent a row/column location of a cell in a sudoko board */
/* Member: int x - represents the column of a cell*/
/* Member: int y - represents the row of a cell */
typedef struct index_pair{
    int x;
    int y;
}Index_Pair;

/* A structure to represent a cell of a sudoku board */
/* Member: int value - the cell's value (ranges from 1-N , and 0 is empty)*/
/* Member: int fixed - indicator if the cell is fixed (can't be changed by user) */
/* Member: int errorFlag - indicator if the cell is erroneous */
/* Member: int userMod - indicator if the cell is user-modified (non-empty, non-fixed cells) */
typedef struct cell{
    int value;
    int fixed;
    int errorFlag;
    int userMod;
}Cell;

/* A structure to represent a sudoku board */
/* Member: Cell** board - a 2D array of cells the represents the game board */
/* Member: int rowsInBlock - row subgrid (block) size */
/* Member: int colsInBlock - column subgrid (block) size */
/* Member: int length - size of the board's sides */
/* Member: int** invalidRows - a 2D boolean array that represents which values are already set in a board's row */
/* Member: int** invalidCols - a 2D boolean array that represents which values are already set in a board's column */
/* Member: int** invalidBlocks - a 2D boolean array that represents which values are already set in a board's block */
typedef struct sudoku_board{
    /*solvedFlag and errorFlag deleted*/
    Cell** board;
    int rowsInBlock;
    int colsInBlock;
    int length;
    int** invalidRows;
    int** invalidCols;
    int** invalidBlocks;
}sudokuBoard;


/*Public functions declarations*/

void print_board(sudokuBoard *pBoard,int markErrors);
void get_empty_board(sudokuBoard *sBoard,int m,int n);
void freeBoard(sudokuBoard *game_board);
int isLegalValue(sudokuBoard *sBoard,int row,int col,int val);
int isBoardError(sudokuBoard *sBoard);
int isFixedCellsLegal(sudokuBoard *sBoard);
int countEmptyCells(sudokuBoard *sBoard);
void clearAllFixedCells(sudokuBoard *sBoard);
void copy_boards(sudokuBoard *oldBoard , sudokuBoard *newBoard);
Index_Pair find_empty_cell(sudokuBoard *sBoard);
int randomizeLegalValue(sudokuBoard *sBoard,int row,int col);
int setCell(sudokuBoard *sBoard,int row,int col,int value);


#endif
