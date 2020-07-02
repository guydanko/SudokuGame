#include <stdio.h>
#include <stdlib.h>
#include "SudokuBoard.h"

/*Private functions declarations, all are auxiliary to setCell*/
void writeCell(sudokuBoard *sBoard,int row,int col,int block,int value);
void clearCell(sudokuBoard *sBoard,int row,int col,int block,int value);
void markErrors(sudokuBoard *sBoard,int index,int value,char type);
void rowClearCleanup(sudokuBoard *sBoard,int row,int prevVal);
void colClearCleanup(sudokuBoard *sBoard,int col,int prevVal);
void blockClearCleanup(sudokuBoard *sBoard,int block,int prevVal);
/*Debug Methods*/
void printInvalids(sudokuBoard *sBoard,char type);

/**  print_board:
* @brief Prints a sudoku board in the required format
* @param sudokuBoard *pBoard - a pointer to the board needed to be printed
* @param int markErrors - indicator whether to mark erroneous cells on print
* @return void
*/
void print_board(sudokuBoard *pBoard,int markErrors) {
    int i, j;
    int len = pBoard->length;
    int sep_len = 4*(len)+pBoard->rowsInBlock+1;
    char *seperator;
    seperator = (char *)malloc((sep_len+1)*sizeof(char));
    if(seperator==NULL){
        puts("Error: memory allocation has failed in print_board, Exiting...\n");
        exit(0);
    }
    puts("");
    for(i = 0;i<sep_len;i++){
        seperator[i] = '-';
    }
    seperator[sep_len] = '\0';
    for (i = 0; i < len; ++i) {
        if ((i) % pBoard->rowsInBlock == 0) {
            printf("%s\n",seperator);
        }
        for (j = 0; j <len; ++j) {
            if ((j % pBoard->colsInBlock) == 0) {
                printf("|");
            }
            printf(" ");
            if (pBoard->board[i][j].fixed == 1) {
                printf("%2d.", pBoard->board[i][j].value);
            } else {
                if (pBoard->board[i][j].value != 0) {
                    printf("%2d", pBoard->board[i][j].value);
                    if(markErrors && pBoard->board[i][j].errorFlag){
                        printf("*");
                    } else{
                        printf(" ");
                    }
                } else {
                    printf("   ");
                }
            }
        }
        printf("|\n");
    }
    printf("%s\n",seperator);
    free(seperator);
    puts("");
    return;
}

/**  get_empty_board:
* @brief Initializes a new sudoku board to be empty
* @param sudokuBoard *sBoard - a pointer to the board needed to be initialized
* @param int m - represents row subgrid (block) size
* @param int m - represents column subgrid (block) size
* @return void
*/
void get_empty_board(sudokuBoard *sBoard,int m, int n) {
    int i, j;
    int len = m*n;
    sBoard->board = (Cell **) calloc(len,sizeof(Cell *));
    sBoard->invalidRows = (int **) calloc(len,sizeof(int *));
    sBoard->invalidCols = (int **) calloc(len,sizeof(int *));
    sBoard->invalidBlocks = (int **) calloc(len,sizeof(int *));
    if (sBoard->board == NULL || sBoard->invalidRows == NULL || sBoard->invalidCols == NULL || sBoard->invalidBlocks == NULL) {
        puts("Error: empty board malloc has failed in get_empty_board, Exiting...");
        exit(0);
    }
    for (i = 0; i <len ; i++) {
        sBoard->board[i] = calloc(len,sizeof(Cell));
        sBoard->invalidRows[i] = calloc(len,sizeof(int));
        sBoard->invalidCols[i] = calloc(len,sizeof(int));
        sBoard->invalidBlocks[i] = calloc(len,sizeof(int));
        if (sBoard->board[i] == NULL || sBoard->invalidRows[i] == NULL || sBoard->invalidCols[i] == NULL || sBoard->invalidBlocks[i] == NULL) {
            puts("Error: empty board malloc has failed in get_empty_board, Exiting...");
            exit(0);
        }
    }
    for (i = 0; i < len; i++) {
        for (j = 0; j < len; j++) {
            sBoard->board[i][j].value = 0;
            sBoard->board[i][j].fixed = 0;
            sBoard->board[i][j].errorFlag = 0;
            sBoard->board[i][j].userMod = 0;
        }
    }
    sBoard->colsInBlock = n;
    sBoard->rowsInBlock = m;
    sBoard->length = len;
}

/**  freeBoard:
* @brief Frees all memory used by the sudoku board
* @param sudokuBoard *sBoard - a pointer to the board needed to be freed
* @return void
*/
void freeBoard(sudokuBoard *sBoard) {
    int i;
    if(sBoard == NULL){
        return;
    }
    for (i = 0; i < sBoard->length; i++) {
        free(sBoard->board[i]);
        free(sBoard->invalidRows[i]);
        free(sBoard->invalidCols[i]);
        free(sBoard->invalidBlocks[i]);
    }
    free(sBoard->invalidBlocks);
    free(sBoard->invalidCols);
    free(sBoard->invalidRows);
    free(sBoard->board);
    free(sBoard);
}

/**  isLegalValue:
* @brief Checks if a value is valid for a cell in the sudoku board
* @param sudokuBoard *sBoard - a pointer to the board played
* @param int row/col - indicates the cell's location in the board (0-based)
* @param int val - the value being check for validity (0-based)
* @return int - 1 if the value is valid , 0 o\w
*/
int isLegalValue(sudokuBoard *sBoard,int row,int col,int val){
    int block;
    block = (sBoard->rowsInBlock)*(row/sBoard->rowsInBlock)+(col/sBoard->colsInBlock);
    if(sBoard->board[row][col].fixed==1 || sBoard->board[row][col].userMod==1)
        return 0;
    if(sBoard->invalidRows[row][val]==1)
        return 0;
    if(sBoard->invalidCols[col][val]==1)
        return 0;
    if(sBoard->invalidBlocks[block][val]==1)
        return 0;
    return 1;
}

/**  isBoardError:
* @brief Checks if the game board is erroneous
* @param sudokuBoard *sBoard - a pointer to the board played
* @return int - 1 if the board is erroneous , 0 o\w
*/
int isBoardError(sudokuBoard *sBoard){
    int i,j,len;
    len=sBoard->length;
    for(i=0;i<len;i++){
        for(j=0;j<len;j++){
            if(sBoard->board[i][j].errorFlag==1)
                return 1;
        }
    }
    return 0;
}

/**  isFixedCellsLegal:
* @brief Checks if all of the game board's fixed cells are not erroneous
* @param sudokuBoard *sBoard - a pointer to the board played
* @return int - 1 if the board is erroneous , 0 o\w
*/
int isFixedCellsLegal(sudokuBoard *sBoard){
    int i,j,len;
    len=sBoard->length;
    for(i=0;i<len;i++){
        for(j=0;j<len;j++){
            if(sBoard->board[i][j].errorFlag==1 && sBoard->board[i][j].fixed ==1 )
                return 0;
        }
    }
    return 1;
}

/**  clearAllFixedCells:
* @brief Unfixes all cells of the game board
* @param sudokuBoard *sBoard - a pointer to the board played
* @return void
*/
void clearAllFixedCells(sudokuBoard *sBoard){
    int i,j,len;
    len=sBoard->length;
    for(i=0;i<len;i++){
        for(j=0;j<len;j++){
            sBoard->board[i][j].fixed=0;
        }
    }
}

/**  countEmptyCells:
* @brief Counts the number of empty cells in the game board
* @param sudokuBoard *sBoard - a pointer to the board played
* @return int - specified count
*/
int countEmptyCells(sudokuBoard *sBoard){
    int count=0,i,j;
    for(i=0;i<sBoard->length;i++){
        for(j=0;j<sBoard->length;j++){
            if(sBoard->board[i][j].fixed==0 && sBoard->board[i][j].value==0)
                count++;
        }
    }
    return count;
}

/**  randomizeLegalValue:
* @brief Randomizes a legal value for a cell in the game board from all valid values
* @param sudokuBoard *sBoard - a pointer to the board played
* @param int row/col - indicates the cell's location in the board (0-based)
* @return int - the randomized legal value (1-based) or -1 if there are none
*/
int randomizeLegalValue(sudokuBoard *sBoard,int row,int col){
    int *values, N,v,count=0;
    N=sBoard->length;
    values = (int*)malloc(N*sizeof(int));
    if(values==NULL){
        printf("Error: memory allocation has failed in randomizeLegalValue,Exiting...\n");
        exit(0);
    }
    for(v=0;v<N;v++){
        if(isLegalValue(sBoard,row,col,v)){
            values[v]=1;
            count++;
        }
        else{
            values[v]=0;
        }
    }
    if(count==0){/*No legal values available*/
        free(values);
        return -1;
    }
    else{
        while(1){
            v = rand()%N;
            if(values[v]==1){
                free(values);
                return (v+1);
            }
        }
    }
    free(values);
    return -1;
}

/**  copy_boards:
* @brief Copies all content of a board to a different board
* @param sudokuBoard *oldBoard - a pointer to the board getting a new copy
* @param sudokuBoard *newBoard - a pointer to the board who will hold the copy of oldBoard
* @return void
*/
void copy_boards(sudokuBoard *oldBoard , sudokuBoard *newBoard){
    int i,j,N;
    N=oldBoard->length;
    for(i=0;i<N;i++){
        for(j=0;j<N;j++){
            newBoard->board[i][j].value=oldBoard->board[i][j].value;
            newBoard->board[i][j].fixed=oldBoard->board[i][j].fixed;
            newBoard->board[i][j].errorFlag=oldBoard->board[i][j].errorFlag;
            newBoard->board[i][j].userMod=oldBoard->board[i][j].userMod;
            newBoard->invalidRows[i][j]=oldBoard->invalidRows[i][j];
            newBoard->invalidCols[i][j]=oldBoard->invalidCols[i][j];
            newBoard->invalidBlocks[i][j]=oldBoard->invalidBlocks[i][j];
        }
    }
    return;
}

/**  find_empty_cell:
* @brief Finds the first empty cell of the game board (if any)
* @param sudokuBoard *sBoard - a pointer to the board played
* @return Index_Pair - the coordinates of the cell's location in the game board , or <-1,-1> if there's none
*/
Index_Pair find_empty_cell(sudokuBoard *sBoard){
    int i,j,len;
    Index_Pair ip;
    len = sBoard->length;
    ip.x=-1,ip.y=-1;
    for(i=0;i<len;i++){
        for(j=0;j<len;j++){
            if ((sBoard->board[i][j].fixed==0) && (sBoard->board[i][j].value==0)){
                ip.x = i;
                ip.y = j;
                return ip;
            }
        }
    }
    return ip;
}

/**  setCell:
* @brief Sets a cell in the game board to a value , and updates all auxiliary members of the board
* @param sudokuBoard *sBoard - a pointer to the board played
* @param int row/col - indicates the cell's location in the board (0-based)
* @param int val - the value being check for validity (1-based)
* @return int - 1 if the set was successful , 0 o\w
*/
int setCell(sudokuBoard *sBoard,int row,int col,int value){
    int block,prevVal, ROWS=sBoard->rowsInBlock,COLS=sBoard->colsInBlock;
    block = ROWS*(row/ROWS) + (col/COLS);
    prevVal = sBoard->board[row][col].value;
    if(sBoard->board[row][col].fixed==1){
        printf("Error: can't set a fixed cell\n");
        return 0;
    }
    if(prevVal==0 && value==0){
        return 1;
    }
    else if(prevVal==0 && value!=0){
        writeCell(sBoard,row,col,block,value);
    }
    else if(prevVal!=0 && value==0){
        clearCell(sBoard,row,col,block,prevVal);
    }
    else if(prevVal==value){
        return 1;
    }
    else{ /*both prevVal,value!=0 | overwrite*/
        clearCell(sBoard,row,col,block,prevVal);
        writeCell(sBoard,row,col,block,value);
    }
    return 1;
}

/**  writeCell:
* @brief Changes a cell in the game board from 0 to value (ranged 1-N)
* @param sudokuBoard *sBoard - a pointer to the board played
* @param int row/col - indicates the cell's location in the board (0-based)
* @param int val - the value being check for validity (1-based)
* @return void
*/
void writeCell(sudokuBoard *sBoard,int row,int col,int block,int value){
    /*MUSTS*/
    sBoard->board[row][col].value=value;
    sBoard->board[row][col].userMod=1;
    /*OPTIONAL*/
    /*Row check*/
    if(sBoard->invalidRows[row][value-1]==1){
        markErrors(sBoard,row,value,'R');
    }else{sBoard->invalidRows[row][value-1]=1;}
    /*Column check*/
    if(sBoard->invalidCols[col][value-1]==1){
        markErrors(sBoard,col,value,'C');
    }else{sBoard->invalidCols[col][value-1]=1;}
    /*Block check*/
    if(sBoard->invalidBlocks[block][value-1]==1){
        markErrors(sBoard,block,value,'B');
    }else{sBoard->invalidBlocks[block][value-1]=1;}
}

/**  markErrors:
* @brief After a write to a cell , mark all new erroneous cells (if any)
* @param sudokuBoard *sBoard - a pointer to the board played
* @param int index - index of the row/column/block needed checked
* @param char type - differs for checking 'R' - row , 'C' - column or 'B' - block
* @return void
*/
void markErrors(sudokuBoard *sBoard,int index,int value,char type){
    int i,ROWS,COLS,N=sBoard->length;
    int rowStart,rowEnd,colStart,colEnd,j;
    ROWS = sBoard->rowsInBlock,COLS = sBoard->colsInBlock;
    switch (type){
    case 'R':
        for(i=0;i<N;i++){
            if(sBoard->board[index][i].value==value){
                sBoard->board[index][i].errorFlag=1;
            }
        }
        break;
    case 'C':
        for(i=0;i<N;i++){
            if(sBoard->board[i][index].value==value){
                sBoard->board[i][index].errorFlag=1;
            }
        }
        break;
    case 'B':
        rowStart = (index/ROWS)*ROWS , rowEnd = (((index/ROWS)+1)*ROWS)-1;
        colStart = (index%ROWS)*COLS , colEnd = (((index%ROWS)+1)*COLS)-1;
        for(i=rowStart;i<=rowEnd;i++){
            for(j=colStart;j<=colEnd;j++){
                if(sBoard->board[i][j].value==value){
                    sBoard->board[i][j].errorFlag=1;
                }
            }
        }
        break;
    default:
        printf("Error: no such type -%c- while marking errors\n",type);
    }
    return;
}

/**  clearCell:
* @brief Changes a cell in the game board from a value to 0
* @param sudokuBoard *sBoard - a pointer to the board played
* @param int row/col/block - indicates the cell's location in the board (0-based)
* @param int prevVal - the original value of the cell that is being cleared
* @return void
*/
void clearCell(sudokuBoard *sBoard,int row,int col,int block,int prevVal){
    /*MUSTS*/
    sBoard->board[row][col].value=0;
    sBoard->board[row][col].userMod=0;
    /*OPTIONAL*/
    if(sBoard->board[row][col].errorFlag==0){
        /*Cell is not errored*/
        sBoard->invalidRows[row][prevVal-1]=0;
        sBoard->invalidCols[col][prevVal-1]=0;
        sBoard->invalidBlocks[block][prevVal-1]=0;
    }
    else{/*Cell is errored*/
        sBoard->board[row][col].errorFlag=0;
        /*NOTE: check if there's need to be an if statement : sBoard->invalidRows/Cols/Blocks[relevent index][prevVal-1]==1*/
        rowClearCleanup(sBoard,row,prevVal);
        colClearCleanup(sBoard,col,prevVal);
        blockClearCleanup(sBoard,block,prevVal);

    }
}

/**  rowClearCleanup:
* @brief After a clear, update previously erroneous cells to non-erroneous (if any) in the cleared cell's row
* @param sudokuBoard *sBoard - a pointer to the board played
* @param int row- indicates the cell's row in the board (0-based)
* @param int prevVal - the original value of the cell that is being cleared
* @return void
*/
void rowClearCleanup(sudokuBoard *sBoard,int row,int prevVal){
    int count,sec_row,sec_col,i,j,noChange=0;
    int ROWS=sBoard->rowsInBlock,COLS=sBoard->colsInBlock;
    int rowStart,rowEnd,colStart,colEnd;
    /*Check Row*/
    count=0;
    for(i=0;i<sBoard->length;i++){
        if(sBoard->board[row][i].value==prevVal){
            count++;
            sec_row=row;sec_col=i;
        }
    }
    if(count==0){
        sBoard->invalidRows[row][prevVal-1]=0;
    }
    rowStart = (sec_row/ROWS)*ROWS , rowEnd =((sec_row/ROWS)*ROWS)+ROWS-1 ;
    colStart = (sec_col/COLS)*COLS , colEnd = ((sec_col/COLS)*COLS)+COLS-1;
    if(count==1){
        /*NEED TO CHECK COL AND BLOCK*/
        for(i=0;i<sBoard->length;i++){
            if(sBoard->board[i][sec_col].value==prevVal && i!=sec_row){
                noChange=1;
            }
        }
        for(i=rowStart;i<=rowEnd;i++){
            for(j=colStart;j<=colEnd;j++){
                if(sBoard->board[i][j].value==prevVal && i!=sec_row && j!=sec_col)
                    noChange=1;
            }
        }
        if(noChange!=1)
            sBoard->board[sec_row][sec_col].errorFlag=0;
    }
}

/**  colClearCleanup:
* @brief After a clear, update previously erroneous cells to non-erroneous (if any) in the cleared cell's column
* @param sudokuBoard *sBoard - a pointer to the board played
* @param int col- indicates the cell's column in the board (0-based)
* @param int prevVal - the original value of the cell that is being cleared
* @return void
*/
void colClearCleanup(sudokuBoard *sBoard,int col,int prevVal){
    int count,sec_row,sec_col,i,j,noChange=0;
    int ROWS=sBoard->rowsInBlock,COLS=sBoard->colsInBlock;
    int rowStart,rowEnd,colStart,colEnd;
    /*Check Col*/
    count=0;
    for(i=0;i<sBoard->length;i++){
        if(sBoard->board[i][col].value==prevVal){
            count++;
            sec_row=i;sec_col=col;
        }
    }
    if(count==0){
        sBoard->invalidCols[col][prevVal-1]=0;
    }
    rowStart = (sec_row/ROWS)*ROWS , rowEnd =((sec_row/ROWS)*ROWS)+ROWS-1 ;
    colStart = (sec_col/COLS)*COLS , colEnd = ((sec_col/COLS)*COLS)+COLS-1;
    if(count==1){
        /*NEED TO CHECK ROW AND BLOCK*/
        for(i=0;i<sBoard->length;i++){
            if(sBoard->board[sec_row][i].value==prevVal && i!=sec_col){
                noChange=1;
            }
        }
        for(i=rowStart;i<=rowEnd;i++){
            for(j=colStart;j<=colEnd;j++){
                if(sBoard->board[i][j].value==prevVal && i!=sec_row && j!=sec_col){
                    noChange=1;
                }
            }
        }
        if(noChange!=1)
            sBoard->board[sec_row][sec_col].errorFlag=0;
    }
}
/**  blockClearCleanup:
* @brief After a clear, update previously erroneous cells to non-erroneous (if any) in the cleared cell's block
* @param sudokuBoard *sBoard - a pointer to the board played
* @param int block- indicates the cell's block in the board (0-based)
* @param int prevVal - the original value of the cell that is being cleared
* @return void
*/
void blockClearCleanup(sudokuBoard *sBoard,int block,int prevVal){
    int count,sec_row,sec_col,i,j,noChange=0;
    int ROWS=sBoard->rowsInBlock,COLS=sBoard->colsInBlock;
    int rowStart,rowEnd,colStart,colEnd;
    rowStart = (block/ROWS)*ROWS , rowEnd = (((block/ROWS)+1)*ROWS)-1;
    colStart = (block%ROWS)*COLS , colEnd = (((block%ROWS)+1)*COLS)-1;
    /*Check Block*/
    count=0;
    for(i=rowStart;i<=rowEnd;i++){
        for(j=colStart;j<=colEnd;j++){
            if(sBoard->board[i][j].value==prevVal){
                count++;
                sec_row=i;sec_col=j;
            }
        }
    }
    if(count==0){
        sBoard->invalidBlocks[block][prevVal-1]=0;
    }
    if(count==1){
        /*NEED TO CHECK ROW AND COL*/
        for(i=0;i<sBoard->length;i++){
            if(sBoard->board[sec_row][i].value==prevVal && i!=sec_col)
                noChange=1;
        }
        for(i=0;i<sBoard->length;i++){
            if(sBoard->board[i][sec_col].value==prevVal && i!=sec_row){
                noChange=1;
            }
        }
        if(noChange!=1)
            sBoard->board[sec_row][sec_col].errorFlag=0;
    }
}

/*Debug Methods*/
/*Prints an auxiliary array of sBoard*/
/*Types : 'R' - invalidRows , 'C' - invalidCols , 'B' - invalidBlocks*/
/*'E' - errorFlags , 'U' - userMods*/

void printInvalids(sudokuBoard *sBoard,char type){
    int i,j;
    switch(type){
    case 'R':
        puts("InvalidRows:");
        for(i=0;i<sBoard->length;i++){
            puts("");
            for(j=0;j<sBoard->length;j++){
                printf("%d ",sBoard->invalidRows[i][j]);
            }
        }
        puts("");
        break;
    case 'C':
        puts("InvalidCols:");
        for(i=0;i<sBoard->length;i++){
            puts("");
            for(j=0;j<sBoard->length;j++){
                printf("%d ",sBoard->invalidCols[i][j]);
            }
        }
        puts("");
        break;
    case 'B':
        puts("InvalidBlocks:");
        for(i=0;i<sBoard->length;i++){
            puts("");
            for(j=0;j<sBoard->length;j++){
                printf("%d ",sBoard->invalidBlocks[i][j]);
            }
        }
        puts("");
        break;
    case 'E':
        puts("ErrorFlags:");
        for(i=0;i<sBoard->length;i++){
            puts("");
            for(j=0;j<sBoard->length;j++){
                printf("%d ",sBoard->board[i][j].errorFlag);
            }
        }
        puts("");
        break;
    case 'U':
        puts("UserMods:");
        for(i=0;i<sBoard->length;i++){
            puts("");
            for(j=0;j<sBoard->length;j++){
                printf("%d ",sBoard->board[i][j].userMod);
            }
        }
        puts("");
        break;
    }
}
