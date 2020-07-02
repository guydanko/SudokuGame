#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gameLogic.h"
#include "LPSolver.h"


int main() {
    int isRunning = 1;
    sudokuGame *sGame;
    sGame = (sudokuGame *) malloc(sizeof(sudokuGame));
    if(sGame==NULL){
        printf("Error: memory allocation has failed,Exiting...\n");
        exit(0);
    }
    srand(time(NULL));
    initNewGame(sGame);

    printf("Welcome to the Guy's sudoku!\n");

    while (isRunning) {
        gameTurn(sGame);
    }


    return 0;
}

