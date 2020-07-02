#include <stdio.h>
#include <stdlib.h>
#include "ctype.h"
#include "string.h"
#include "gameLogic.h"

/*Private functions declarations*/
int loadToGameBoard(sudokuGame *game, char *fileName);
int canSolveSudoku(char *fileName);
int canEditSudoku(char *fileName);
int isInputEmpty(char *input);
int checkMode(sudokuGame *game, Command *command);
int isPrintTypeCommand(Command *command);

/**  solveCommand:
* @brief Loads a board from a file to the current board of the sudoku game in solve mode
* @param sudokuGame* game - a pointer to a sudoku game
* @param char* fileName - a string representing the name of file to read a sudoku board from
* @return 1 if successful, 0 o/w
*/
int solveCommand(sudokuGame *game, char *fileName) {
    if (canSolveSudoku(fileName)) {
        game->mode = so;
        loadToGameBoard(game, fileName);
        clearAllGameMoves(game);
        return 1;
    }
    return 0;
}

/*loads a file to the current board of the game*/
int loadToGameBoard(sudokuGame *game, char *fileName) {

    freeSolution(game);
    freeBoard(game->currBoard);

    game->currBoard = (sudokuBoard *) malloc(sizeof(sudokuBoard));
    if(game->currBoard==NULL){
        printf("Error: memory allocation has failed in loadToGameBoard,Exiting...\n");
        exit(0);
    }

    fileToSudoku(game->currBoard, fileName);

    get_empty_solution(game);

    if (game->mode == ed) {
        clearAllFixedCells(game->currBoard);
    }

    return 1;
}

/*checks if sudoku from file is solvable*/
int canSolveSudoku(char *fileName) {
    int isSuccess;
    sudokuBoard *checkBoard = (sudokuBoard *) malloc(sizeof(sudokuBoard));

    if(checkBoard==NULL){
        printf("Error: memory allocation has failed in canSolveSudoku,Exiting...\n");
        exit(0);
    }
    isSuccess=fileToSudoku(checkBoard, fileName);
    if (isSuccess==-1) {
        free(checkBoard);
        return 0;
    }
    else if(isSuccess==0){
        freeBoard(checkBoard);
        return 0;
    }

    if (!isFixedCellsLegal(checkBoard)) {
        printf("Error: board that was attempted to be loaded has illegal fixed values\n");
        freeBoard(checkBoard);
        return 0;
    }

    freeBoard(checkBoard);
    return 1;
}

/**  editCommand:
* @brief Loads a board from a file to the current board of the sudoku game in edit mode
* @param sudokuGame* game - a pointer to a sudoku game
* @param char* fileName - a string representing the name of file to edit sudoku from (can be NULL for new board)
* @return 1 if successful, 0 o/w
*/
int editCommand(sudokuGame *game, char *fileName) {
    if (fileName != NULL) {
        if (canEditSudoku(fileName)) {
            game->mode = ed;
            loadToGameBoard(game, fileName);
            clearAllGameMoves(game);
            return 1;
        } else {
            return 0;
        }
    } else {
        freeSolution(game);
        freeBoard(game->currBoard);

        game->currBoard = (sudokuBoard *) malloc(sizeof(sudokuBoard));
        if(game->currBoard==NULL){
            printf("Error: memory allocation has failed in editCommand,Exiting...\n");
            exit(0);
        }

        get_empty_board(game->currBoard, 3, 3);
        get_empty_solution(game);
        game->mode = ed;
        clearAllGameMoves(game);
        return 1;
    }

}

/*checks if sudoku from file can be loaded for edit*/
int canEditSudoku(char *fileName) {
    int isSuccess;

    sudokuBoard *checkBoard = (sudokuBoard *) malloc(sizeof(sudokuBoard));

    if(checkBoard==NULL){
        printf("Error: memory allocation has failed in canSolveSudoku,Exiting...\n");
        exit(0);
    }
    isSuccess=fileToSudoku(checkBoard, fileName);

    if (isSuccess==-1) {
        free(checkBoard);
        return 0;
    }
    else if(isSuccess==0){
        freeBoard(checkBoard);
        return 0;
    }

    freeBoard(checkBoard);
    return 1;
}

/**  markErrorsCommand:
* @brief Sets  the mark error flag of sudokuGame
* @param sudokuGame* game - a pointer to a sudoku game
* @param int x - the new value of mark_errors of a sudoku game (ranged 0-1)
* @return 1 if successful, 0 o/w
*/
int markErrorsCommand(sudokuGame *game, int x) {
    setMarkErrors(game, x);
    return 1;

}

/**  printBoardCommand:
* @brief Prints the current board of the game
* @param sudokuGame* game - a pointer to a sudoku game
* @return 1 if successful, 0 o/w
*/
int printBoardCommand(sudokuGame *game) {
    if (game->mode == ed) {
        print_board(game->currBoard, 1);
    } else {
        printGameBoard(game);
    }
    return 1;
}

/**  setGameCommand:
* @brief Sets cell of current board to val
* @param sudokuGame* game - a pointer to a sudoku game
* @param int row -  row of changed cell
* @param int col - col of changed cell
* @param int val - value to change cell to
* @return 1 if successful, 0 o/w
*/
int setGameCommand(sudokuGame *game, int row, int col, int val) {
    return (gameSetCell(game, row, col, val, 0));
}

/**  undoCommand:
* @brief Undo the current move of the game
* @param sudokuGame* game - a pointer to a sudoku game
* @return 1 if successful, 0 o/w
*/
int undoCommand(sudokuGame *game) {
    return undoMove(game, 1);
}

/**  redoCommand:
* @brief Redo the current move of the game
* @param sudokuGame* game - a pointer to a sudoku game
* @return 1 if successful, 0 o/w
*/
int redoCommand(sudokuGame *game) {
    return redoMove(game);
}

/**  saveCommand:
* @brief Saves current sudoku board into a file
* @param sudokuGame* game - a pointer to a sudoku game
* @param char* fileName - a string representing the name of file to write sudoku to
* @return 1 if successful, 0 o/w
*/
int saveCommand(sudokuGame *game, char *fileName) {
    if (game->mode == ed) {
        if (isBoardError(game->currBoard)) {
            printf("Error: save is not available in edit mode while board is erroneous\n");
            return 0;
        }
        if (ILPSolver(game)) {
            return sudokuToFile(game->currBoard, fileName, 1);
        } else {
            printf("Error: save is not available, board has no solution\n");
            return 0;
        }
    }
    return sudokuToFile(game->currBoard, fileName, 0);
}

/**  guessCommand:
* @brief Guess a solution to the current board with threshold x
* @param sudokuGame* game - a pointer to a sudoku game
* @param float x - the threshold of which to perform the guess
* @return 1 if successful, 0 o/w
*/
int guessCommand(sudokuGame *game, float x) {
    if (isBoardError(game->currBoard)) {
        printf("Error: guess is not available while board is erroneous\n");
        return 0;
    } else {
        if (!LPSolverGuess(game, x)) {
            printf("Error: LP solver has crashed\n");
            return 0;
        }
    }
    return 1;
}

/**  guessHintCommand:
* @brief Guess a hint to a cell of current sudoku board
* @param sudokuGame* game - a pointer to a sudoku game
* @param int row - value of row of changed cell
* @param int col - value of col of changed cell
* @return 1 if successful, 0 o/w
*/
int guessHintCommand(sudokuGame *game, int row, int col) {
    if (isBoardError(game->currBoard)) {
        printf("Error: guess is not available while board is erroneous\n");
        return 0;
    } else if (game->currBoard->board[row][col].fixed == 1) {
        printf("Error: can't guess hint a fixed cell\n");
        return 0;
    } else if (game->currBoard->board[row][col].userMod == 1) {
        printf("Error: can't guess hint a user modified cell\n");
        return 0;
    } else {
        if (!LPSolverGuessHint(game, row, col)) {
            printf("Error: LP solver has crashed\n");
            return 0;
        }
    }
    return 1;
}

/**  validateCommand:
* @brief Validates if the current board is solvable or not
* @param sudokuGame* game - a pointer to a sudoku game
* @return 1 if successful, 0 otherwise
*/
int validateCommand(sudokuGame *game) {
    if (isBoardError(game->currBoard)) {
        printf("Error: validate is not available while board is erroneous\n");
        return 0;
    }
    if (ILPSolver(game))
        printf("Board is solvable\n");
    else
        printf("Board is unsolvable\n");
    return 1;
}

/**  numOfSolutionsCommand:
* @brief Calculates the number of solutions to the current board and prints it to the user
* @param sudokuGame* game - a pointer to a sudoku game
* @return 1 if successful, 0 o/w
*/
int numOfSolutionsCommand(sudokuGame *game) {
    if (isBoardError(game->currBoard)) {
        printf("Error: num_solutions is not available while board is erroneous\n");
        return 0;
    }
    printf("The board has %d different solutions\n", solveStackEnv(game->currBoard));
    return 1;
}

/**  generateCommand:
* @brief Generates a new sudoku board to the sudoku game
* @param sudokuGame* game - a pointer to a sudoku game
* @param int x - number of legal values to add to the board before solving (randomization effect)
* @param int y - number of cells filled for the newly generated sudoku board
* @return 1 if successful, 0 o/w
*/
int generateCommand(sudokuGame *game, int x, int y) {
    int emptyCells, row, col, N, xCount, yCount, i, j, value;
    sudokuBoard *tempBoard, *swapBoard;
    N = game->currBoard->length;
    tempBoard = (sudokuBoard *) malloc((sizeof(sudokuBoard)));
    if(tempBoard == NULL){
        printf("Error: failed to allocate memory in generateCommand, Exiting...\n");
        exit(0);
    }
    get_empty_board(tempBoard, game->currBoard->rowsInBlock, game->currBoard->colsInBlock);
    copy_boards(game->currBoard, tempBoard);
    emptyCells = countEmptyCells(game->currBoard);
    yCount = (N * N) - y;
    if (emptyCells < x) {
        printf("Error: not enough empty cells for parameter X:%d\n", x);
        return 0;
    } else if (isBoardError(game->currBoard)) {
        printf("Error: generate is not available when board is erroneous\n");
        return 0;
    } else {
        for (i = 0; i < 1000; i++) {
            xCount = x;
            while (xCount > 0) {
                row = rand() % N;
                col = rand() % N;
                if (game->currBoard->board[row][col].fixed == 0 && game->currBoard->board[row][col].value == 0) {
                    value = randomizeLegalValue(game->currBoard, row, col);
                    if (value == -1) {
                        copy_boards(tempBoard, game->currBoard);
                        break;
                    }
                    setCell(game->currBoard, row, col, value);
                    xCount--;
                }
            }
            if (ILPSolver(game)) {
                for (i = 0; i < N; i++) {
                    for (j = 0; j < N; j++) {
                        if (game->currBoard->board[i][j].fixed == 0 && game->currBoard->board[i][j].value == 0) {
                            setCell(game->currBoard, i, j, game->currSol[i][j]);
                        }
                    }
                }
                break;
            } else {
                copy_boards(tempBoard, game->currBoard);
            }
        }
        if (i == 1000) {
            printf("Error: after 1000 iteration - generate has failed\n");
            return 0;
        }
        /*All X cells got legal values and ILPSolver was successful*/
        while (yCount > 0) {
            row = rand() % N;
            col = rand() % N;
            if (game->currBoard->board[row][col].value != 0) {
                /*FORMER FIXED CHECK*/
                if (game->currBoard->board[row][col].fixed == 1) {
                    game->currBoard->board[row][col].fixed = 0;
                }
                setCell(game->currBoard, row, col, 0);
                yCount--;
            }
        }
        /*SWAP BOARDS*/
        swapBoard = game->currBoard;
        game->currBoard = tempBoard;
        tempBoard = swapBoard;
        pushGameMove(game);
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                if (game->currBoard->board[i][j].value != tempBoard->board[i][j].value)
                    gameSetCell(game, i, j, tempBoard->board[i][j].value, 1);
            }
        }
    }
    freeBoard(tempBoard);
    return 1;
}

/**  hintCommand:
* @brief Gives a hint for value a cell of current sudoku board
* @param sudokuGame* game - a pointer to a sudoku game
* @param int row - value of row of changed cell
* @param int col - value of col of changed cell
* @return 1 if successful, 0 o/w
*/
int hintCommand(sudokuGame *game, int row, int col) {
    if (isBoardError(game->currBoard)) {
        printf("Error: guess is not available while board is erroneous\n");
        return 0;
    } else if (game->currBoard->board[row][col].fixed == 1) {
        printf("Error: can't guess hint a fixed cell\n");
        return 0;
    } else if (game->currBoard->board[row][col].userMod == 1) {
        printf("Error: can't guess hint a user modified cell\n");
        return 0;
    } else {
        if (!ILPSolver(game)) {
            printf("Error: ILP solver has crashed\n");
            return 0;
        }
        printf("Hinted value for cell <%d,%d> is %d\n", col + 1, row + 1, game->currSol[row][col]);
    }
    return 1;
}

/**  autofillCommand:
* @brief Fills values of all cells in current board which have a single legal value
* @param sudokuGame* game - a pointer to a sudoku game
* @return 1 if successful, 0 o/w
*/
int autofillCommand(sudokuGame *game) {
    int **values, i, j, v, count, len;
    len = game->currBoard->length;
    if (isBoardError(game->currBoard)) {
        printf("Error: autofill is not available when the board is erronoues\n");
        return 0;
    }
    pushGameMove(game);
    /*INIT VALUES*/
    values = (int **) malloc(len * sizeof(int *));
    if (values == NULL) {
        puts("Error: memory allocation has failed in autofillCommand, Exiting...");
        exit(0);
    }
    for (i = 0; i < len; i++) {
        values[i] = (int *) calloc(len, sizeof(int));
        if (values[i] == NULL) {
            puts("Error: memory allocation has failed in autofillCommand, Exiting...");
            exit(0);
        }
    }
    for (i = 0; i < len; i++) {
        for (j = 0; j < len; j++) { /*For every cell*/
            count = 0;
            for (v = 0; v < len; v++) {
            /*Check all legal values and count them , remember the last one*/
                if (isLegalValue(game->currBoard, i, j, v)) {
                    values[i][j] = v + 1;
                    count++;
                }
            }
            if (count > 1) { /*If there's more than 1 legal value - mark cell to not change*/
                values[i][j] = -1;
            }
        }
    }

    for (i = 0; i < len; i++) {
        for (j = 0; j < len; j++) {
            if (values[i][j] != -1 && game->currBoard->board[i][j].fixed == 0 &&
                game->currBoard->board[i][j].value == 0) {
                gameSetCell(game, i, j, values[i][j], 1);
            }
        }
    }
    /*FREE VALUES*/
    for (i = 0; i < len; i++) {
        free(values[i]);
    }
    free(values);
    return 1;
}

/**  resetCommand:
* @brief Undo all moves made since loading the game ,i.e reset to the original state of the game
* @param sudokuGame* game - a pointer to a sudoku game
* @return 1 if successful, 0 o/w
*/
int resetCommand(sudokuGame *game) {
    undoAllMoves(game);
    return 1;
}

/**  exitCommand:
* @brief Exits from sudokuGame, frees all dynamic memory allocated
* @param sudokuGame* game - a pointer to a sudoku game
* @return 1 if successful, 0 o/w
*/
int exitCommand(sudokuGame *game) {
    freeSudokuGame(game);
    printf("Thanks for playings The Guy's Sudoku, exiting...\n");
    return 1;
}

/*checks whether the input of the user is empty (all whitespace)*/
int isInputEmpty(char *input) {
    while (*input) {
        if (!isspace(*input)) {
            return 0;
        }
        input++;
    }
    return 1;
}

/**  checkMode:
* @brief Checks that the command type matches the current mode of the game
* @param sudokuGame* game - a pointer to a sudoku game
* @param Command* command - receives a command with command type set
* @return 1 if command can be performed in current mode, 0 o\w
*/
int checkMode(sudokuGame *game, Command *command) {
    switch (command->commandType) {
        case solve:
            return 1;
        case edit:
            return 1;
        case mark_errors:
            if (game->mode == init) {
                puts("Error: mark_errors is unavailable in INIT mode, only on SOLVE mode");
                return 0;
            } else if (game->mode == ed) {
                puts("Error: mark_errors is unavailable in EDIT mode, only on SOLVE mode");
                return 0;
            } else {
                return 1;
            }
        case printBoard:
            if (game->mode == init) {
                puts("Error: print_board is unavailable in INIT mode, only on SOLVE and EDIT mode");
                return 0;
            }
            return 1;
        case set:
            if (game->mode == init) {
                puts("Error: set is unavailable in INIT mode, only on SOLVE and EDIT mode");
                return 0;
            }
            return 1;
        case validate:
            if (game->mode == init) {
                puts("Error: validate is unavailable in INIT mode, only on SOLVE and EDIT mode");
                return 0;
            }
            return 1;
        case guess:
            if (game->mode == init) {
                puts("Error: guess is unavailable in INIT mode, only on SOLVE mode");
                return 0;
            } else if (game->mode == ed) {
                puts("Error: guess is unavailable in EDIT mode, only on SOLVE mode");
                return 0;
            } else {
                return 1;
            }
        case generate:
            if (game->mode == init) {
                puts("Error: generate is unavailable in INIT mode, only on EDIT mode");
                return 0;
            } else if (game->mode == so) {
                puts("Error: generate is unavailable in SOLVE mode, only on EDIT mode");
                return 0;
            } else {
                return 1;
            }
        case undo:
            if (game->mode == init) {
                puts("Error: undo is unavailable in INIT mode, only on SOLVE and EDIT mode");
                return 0;
            }
            return 1;
        case redo:
            if (game->mode == init) {
                puts("Error: redo is unavailable in INIT mode, only on SOLVE and EDIT mode");
                return 0;
            }
            return 1;
        case save:
            if (game->mode == init) {
                puts("Error: save is unavailable in INIT mode, only on SOLVE and EDIT mode");
                return 0;
            }
            return 1;
        case hint:
            if (game->mode == init) {
                puts("Error: hint is unavailable in INIT mode, only on SOLVE mode");
                return 0;
            } else if (game->mode == ed) {
                puts("Error: hint is unavailable in EDIT mode, only on SOLVE mode");
                return 0;
            } else {
                return 1;
            }
        case guess_hint:
            if (game->mode == init) {
                puts("Error: guess_hint is unavailable in INIT mode, only on SOLVE mode");
                return 0;
            } else if (game->mode == ed) {
                puts("Error: guess_hint is unavailable in EDIT mode, only on SOLVE mode");
                return 0;
            } else {
                return 1;
            }
        case num_solutions:
            if (game->mode == init) {
                puts("Error: num_solutions is unavailable in INIT mode, only on SOLVE and EDIT mode");
                return 0;
            }
            return 1;
        case autofill:
            if (game->mode == init) {
                puts("Error: autofill is unavailable in INIT mode, only on SOLVE mode");
                return 0;
            } else if (game->mode == ed) {
                puts("Error: autofill is unavailable in EDIT mode, only on SOLVE mode");
                return 0;
            } else {
                return 1;
            }
        case reset:
            if (game->mode == init) {
                puts("Error: reset is unavailable in INIT mode, only on SOLVE and EDIT mode");
                return 0;
            }
            return 1;
        case ex:
            return 1;
    }
    return 0; /*Command not found*/

}

/*checks whether the command of the user is a type that requires print if successful*/
int isPrintTypeCommand(Command *command) {
    if (command->commandType == edit || command->commandType == solve || command->commandType == set ||
        command->commandType == autofill || command->commandType == redo || command->commandType == undo ||
        command->commandType == generate || command->commandType == guess || command->commandType == reset) {
        return 1;
    }
    return 0;

}

/**  gameTurn:
* @brief Peforms a single turn in a sudoku game
* @param sudokuGame* game - a pointer to a sudoku game
* @return 1 if successful, 0 otherwise
*/
int gameTurn(sudokuGame *game) {
    char *input;
    char temp[258];
    char *token;
    int ch;
    Command *command;
    int isSuccess = 0;
    int boardLen = 0;

    if(game->currBoard != NULL){
        boardLen = game->currBoard->length;
    }
    printf("Enter command:");

    if (feof(stdin)) {
        exitCommand(game);
        exit(0);
    }
    input = malloc(258 * sizeof(char));
    /*Request user command*/
    if (input == NULL) {
        printf("Error: memory allocation has failed in gameTurn, Exiting...\n");
        exit(0);
    }

    fgets(input, 258, stdin);

    /*Basic input checks (length + is empty)*/
    if (strlen(input) >= 257) {
        printf("Error: command must be less 256 characters or less\n");
        free(input);
        while ( (ch = getchar()) != '\n' && ch != EOF ) {
        }
        return 0;
    }

    if (isInputEmpty(input)) {
        free(input);
        return 1;
    }

    strcpy(temp, input);

    token = strtok(temp, " \n\r\t");

    command = (Command *) malloc(sizeof(Command));
    if (command == NULL) {
        printf("Error: memory allocation has failed in gameTurn, Exiting...\n");
        exit(0);
    }
    /*Checks valid command name and mode*/
    if (!setCommandName(command, token)) {
        free(input);
        free(command);
        return 0;
    }
    if (!checkMode(game, command)) {
        free(input);
        free(command);
        return 0;
    }
    /*Checks valid params for each command*/
    if (!parseCommandParams(command, input + strlen(token), boardLen)) {
        free(input);
        free(command);
        return 0;;
    }
    /*Perform command on board*/
    switch (command->commandType) {
        case solve: {
            isSuccess = solveCommand(game, command->fileName);
            break;
        }
        case edit: {
            isSuccess = editCommand(game, command->fileName);
            break;
        }
        case mark_errors: {
            isSuccess = markErrorsCommand(game, command->x);
            break;
        }
        case printBoard: {
            isSuccess = printBoardCommand(game);
            break;
        }
        case set: {
            isSuccess = setGameCommand(game, command->y - 1, command->x - 1, command->z);
            break;
        }
        case undo: {
            isSuccess = undoCommand(game);
            break;
        }
        case redo: {
            isSuccess = redoCommand(game);
            break;
        }
        case validate: {
            isSuccess = validateCommand(game);
            break;
        }
        case guess: {
            isSuccess = guessCommand(game, command->val);
            break;
        }
        case generate: {
            isSuccess = generateCommand(game, command->x, command->y);
            break;
        }
        case save: {
            isSuccess = saveCommand(game, command->fileName);
            break;
        }
        case hint: {
            isSuccess = hintCommand(game, command->y - 1, command->x - 1);
            break;
        }
        case guess_hint: {
            isSuccess = guessHintCommand(game, command->y - 1, command->x - 1);
            break;
        }
        case num_solutions: {
            isSuccess = numOfSolutionsCommand(game);
            break;
        }
        case autofill: {
            isSuccess = autofillCommand(game);
            break;
        }

        case reset: {
            isSuccess = resetCommand(game);
            break;
        }
        case ex: {
            exitCommand(game);
            free(input);
            free(command);
            exit(0);
        }
    }

    /*Decides whether to print board after successful command*/
    if (isSuccess && isPrintTypeCommand(command)) {
        printBoardCommand(game);
    }
    free(input);
    free(command);

    /*Checks if board is solved*/
    if (game->mode == so && countEmptyCells(game->currBoard) == 0) {
            if (!isBoardError(game->currBoard)) {
                printf("Congratulations! The puzzle was solved succesfully\n");
                game->mode = init;
            } else {
                printf("The solution is not correct\n");
            }
        }
    return isSuccess;
}
