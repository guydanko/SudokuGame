#ifndef FILEHANDLER_H_INCLUDED
#define FILEHANDLER_H_INCLUDED
#include "SudokuBoard.h"


/*Public functions declarations*/
int fileToSudoku(sudokuBoard * sudokuBoard,char * fileName);
int sudokuToFile(sudokuBoard* sudokuBoard, char * fileName, int fixAll);

#endif
