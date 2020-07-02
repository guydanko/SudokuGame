#include <stdio.h>
#include <stdlib.h>
#include "SudokuGame.h"

/*Private functions declarations*/
void undoCommands(sudokuGame *game, int printMessage);
void redoCommands(sudokuGame *game);

/**  initNewGame:
* @brief Initializes a new instance of a sudoku game
* @param sudokuGame* game - a pointer to an allocated sudokuGame to update with member values
* @return void
*/
void initNewGame(sudokuGame *sGame) {
    sGame->mode = init;
    sGame->currentMove = (MoveNode *) malloc(sizeof(MoveNode));
    if (sGame->currentMove == NULL) {
        printf("Error: memory allocation has failed in initNewGame, Exiting...\n");
        exit(0);
    }
    sGame->currBoard = NULL;
    sGame -> currSol = NULL;
    initMoveList(sGame->currentMove);
    sGame->markErrors = 1;
}

/**  setMarkErrors:
* @brief Sets the game's markErrors indicator on/off
* @param sudokuGame* game - a pointer to the current sudoku game
* @param int x - updates the markErrors member of game to x
* @return void
*/
void setMarkErrors(sudokuGame *game, int x) {
    game->markErrors = x;
}

/**  printGameBoard:
* @brief Prints the current board of a sudoku game depending on mode and mark errors value
* @param sudokuGame* game - a pointer to the current sudoku game
* @return void
*/
void printGameBoard(sudokuGame *game) {
    print_board(game->currBoard, game->markErrors);
}

/**  clearAllGameMoves:
* @brief Clears all moves of a current sudoku game
* @param sudokuGame* game - a pointer to the current sudoku game
* @return void
*/
void clearAllGameMoves(sudokuGame *game) {
    /*clear all moves*/
    clearAllMoves(game->currentMove);
    /*create new first head move*/
    game->currentMove = (MoveNode *) malloc(sizeof(MoveNode));
    if (game->currentMove == NULL) {
        printf("Error: memory allocation has failed in clearAllGameMoves, Exiting...\n");
        exit(0);
    }
    initMoveList(game->currentMove);
}

/**  gameSetCell:
* @brief Sets value of cell in the current game board
* @param sudokuGame* game - a pointer to the current sudoku game
* @param int row - value of row to of changed cell
* @param int col - value of col to of changed cell
* @param int value - value to change cell value to
* @param int isMultiSet - 1 if it is a multiple set command (autofill, generate, guess), 0 o\w
* @return 1 if successful, 0 o\w
*/
int gameSetCell(sudokuGame *sudokuGame, int row, int col, int val, int isMultiSet) {
    int didSet;
    int prevVal;
    Command *command;
    CommandNode *commandNode;

    prevVal = sudokuGame->currBoard->board[row][col].value;

    didSet = setCell(sudokuGame->currBoard, row, col, val);

    if (didSet) {
        commandNode = (CommandNode *) malloc(sizeof(CommandNode));
        if (commandNode == NULL) {
            printf("Error: memory allocation has failed in gameSetCell, Exiting...\n");
            exit(0);
        }
        command = (Command *) malloc(sizeof(Command));
        if (command == NULL) {
            printf("Error: memory allocation has failed in gameSetCell, Exiting...\n");
            exit(0);
        }
        command->x = col + 1;
        command->y = row + 1;
        command->z = val;
        setCommandNode(commandNode, command, prevVal);

        /*push new move if it is a regular set*/
        if (!isMultiSet) {
            pushGameMove(sudokuGame);
            clearNextMoves(sudokuGame->currentMove);
        }

        addCommandToMove(sudokuGame->currentMove, commandNode);

        return 1;
    }
    return 0;
}

/**  pushGameMove:
* @brief Pushes a new move into sudokuGame
* @param sudokuGame *game - a pointer to the current sudoku game
* @return void
*/
void pushGameMove(sudokuGame *game) {
    pushMove(game->currentMove);
    game->currentMove = game->currentMove->next;
}

/**  undoMove:
* @brief Undo the current move in the game
* @param sudokuGame* game - a pointer to the current sudoku game
* @param int printMessage - 1 if we want to print the change made, 0 o\w
* @return 1 if successful, 0 o\w
*/
int undoMove(sudokuGame *game, int printMessage) {
    if (game->currentMove->isHead) {
        printf("Error: No more operations left to undo\n");
        return 0;
    }
    undoCommands(game, printMessage);
    game->currentMove = game->currentMove->prev;
    return 1;
}

/**  redoMove:
* @brief Redo the next move in the game
* @param sudokuGame* game - a pointer to the current sudoku game
* @return 1 if successful, 0 o\w
*/
int redoMove(sudokuGame *game) {
    if (game->currentMove->next == NULL) {
        printf("Error: No more operations left to redo\n");
        return 0;
    }
    game->currentMove = game->currentMove->next;
    redoCommands(game);
    return 1;

}

/**  undoAllMoves:
* @brief Undo all the previous moves in the game
* @param sudokuGame* game - a pointer to the current sudoku game
* @return void
*/
void undoAllMoves(sudokuGame *game) {
    MoveNode *tempMove = game->currentMove;
    while (!tempMove->isHead) {
        undoMove(game, 0);
        tempMove = game->currentMove;
    }
}

/**  freeSudokuGame:
* @brief Frees all memory allocated for the sudoku game
* @param sudokuGame* game - a pointer to the current sudoku game
* @return void
*/
void freeSudokuGame(sudokuGame *game) {
    freeSolution(game);
    freeBoard(game->currBoard);
    clearAllMoves(game->currentMove);
    free(game);
}

/**  get_empty_solution:
* @brief Initialize the current solution of the sudoku game
* @param sudokuGame* game - a pointer to the current sudoku game
* @return void
*/
void get_empty_solution(sudokuGame *sGame) {
    int i, j, len;
    len = sGame->currBoard->length;
    sGame->currSol = (int **) malloc(len * sizeof(int *));
    if (sGame->currSol == NULL) {
        puts("Error: memory allocation has failed in get_empty_solution, Exiting...");
        exit(0);
    }
    for (i = 0; i < len; i++) {
        sGame->currSol[i] = (int *) calloc(len, sizeof(int));
        if (sGame->currSol[i] == NULL) {
            puts("Error: memory allocation has failed in get_empty_solution, Exiting...");
            exit(0);
        }
    }
    for (i = 0; i < len; i++) {
        for (j = 0; j < len; j++) {
            sGame->currSol[i][j] = 0;
        }
    }
    return;
}

/**  freeSolution:
* @brief Frees all memory allocated to the current solution of the game
* @param sudokuGame* game - a pointer to the current sudoku game
* @return void
*/
void freeSolution(sudokuGame *sGame) {
    int i,len = 0;
    if (sGame->currSol == NULL) {
        return;
    }
    if(sGame->currBoard!=NULL)
        len = sGame->currBoard->length;
    for (i = 0; i < len; i++) {
        free(sGame->currSol[i]);
    }
    free(sGame->currSol);
    return;
}

/**  undoCommands:
* @brief Undo all the commands in the game's current move
* @param sudokuGame* game - a pointer to the current sudoku game
* @param int printMessage - 1 if we want to print the changes made, 0 o\w
* @return void
*/
void undoCommands(sudokuGame *game, int printMessage) {
    CommandNode *head = game->currentMove->lastCommandNode;
    int row, col, val, prevVal;
    while (head != NULL) {
        row = head->command->y - 1;
        col = head->command->x - 1;
        val = head->command->z;
        prevVal = head->prevVal;
        if (printMessage) {
            printf("Changing cell <%d,%d> from:(%d) back to:(%d)\n", col + 1, row + 1, val, prevVal);
        }
        setCell(game->currBoard, row, col, prevVal);
        head = head->next;
    }
}

/**  redoCommands:
* @brief Redo all the commands in the game's current move
* @param sudokuGame* game - a pointer to the current sudoku game
* @return void
*/
void redoCommands(sudokuGame *game) {
    CommandNode *head = game->currentMove->firstCommandNode;
    int row, col, val, prevVal;
    while (head != NULL) {
        row = head->command->y - 1;
        col = head->command->x - 1;
        val = head->command->z;
        prevVal = head->prevVal;
        printf("Changing cell <%d,%d> back to:(%d) from:(%d)\n", col + 1, row + 1, prevVal, val);
        setCell(game->currBoard, row, col, val);
        head = head->prev;
    }
}
/*Debug Methods*/
void printSolution(sudokuGame *sGame) {
    int i, j, len;
    len = sGame->currBoard->length;
    for (i = 0; i < len; i++) {
        puts("");
        for (j = 0; j < len; j++) {
            printf("%d ", sGame->currSol[i][j]);
        }
    }
    puts("");
    return;
}
