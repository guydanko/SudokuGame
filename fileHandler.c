#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "fileHandler.h"

int fileToSudoku(sudokuBoard *newBoard, char *fileName) {
    FILE *file;
    int n, m;
    int rowCount = 0;
    int val, colcount = 0;
    char ch;
    int validFile = 1;

    file = fopen(fileName, "r");

    if (file == NULL) {
        printf("Error: failed to open file: %s \n", fileName);
        return -1;
    }

    /*checks m n format of first line*/
    if (fscanf(file, "%d", &m) != 1 || fscanf(file, "%d", &n) != 1) {
        printf("Error: Wrong file format\n");
        fclose(file);
        return -1;
    }


    if( m > 0 && n >0){
        get_empty_board(newBoard, m, n);
    } else{
        printf("Error: Wrong file format\n");
        fclose(file);
        return -1;
    }


    /*input values into board*/
    while (validFile && rowCount != newBoard->length) {
        colcount = 0;
        while (validFile && colcount != newBoard->length) {
            if (fscanf(file, "%d", &val) == 1) {
                fscanf(file, "%c", &ch);
                if (val == 0 && ch == '.') {
                    printf("Error: Invalid format of file - 0 cannot be a fixed value\n");
                    validFile = 0;
                }
                if (val >= 0 && val <= newBoard->length) {
                    setCell(newBoard, rowCount, colcount, val);
                    if (ch == '.') {
                        newBoard->board[rowCount][colcount].fixed = 1;
                        fscanf(file, "%c", &ch);
                        if(!isspace(ch)){
                            printf("Error: Invalid value in file\n");
                            validFile = 0;
                        }
                    } else if (ch != '\0' && !isspace(ch)) {
                        printf("Error: Invalid format of file\n");
                        validFile = 0;
                    }
                    colcount++;
                } else {
                    printf("Error: Invalid value in file\n");
                    validFile = 0;
                }
            } else {
                printf("Error: Not enough values in file\n");
                validFile = 0;
            }
        }
        rowCount++;
    }


    if (!validFile) {
        fclose(file);
        return 0;
    }

    /*check that file is empty after reading all values*/
    while (validFile && fscanf(file, "%ch", &ch) != EOF) {
        if (!isspace(ch)) {
            printf("Error: Too many values in file\n");
            validFile = 0;
        }
    }

    fclose(file);

    if (validFile) {
        return 1;
    }

    return 0;


}

int sudokuToFile(sudokuBoard *sBoard, char *fileName, int fixAll) {

    int i, j;
    FILE *file;
    file = fopen(fileName, "w");

    if (file == NULL) {
        printf("Error: failed to open file: %s\n", fileName);
        return 0;
    }

    /*write first line to file*/
    fprintf(file, "%d %d\n", sBoard->rowsInBlock, sBoard->colsInBlock);

    /*write all values into file*/
    for (i = 0; i < sBoard->length; i++) {
        for (j = 0; j < sBoard->length; j++) {
            fprintf(file, "%d", sBoard->board[i][j].value);
            if (fixAll) {
                if (sBoard->board[i][j].value != 0) {
                    fprintf(file, ".");
                }
            } else {
                if (sBoard->board[i][j].fixed) {
                    fprintf(file, ".");
                }
            }
            if (j != sBoard->length - 1) {
                fprintf(file, " ");
            }
        }
        fprintf(file, "\n");
    }

    fclose(file);
    return 1;

}
