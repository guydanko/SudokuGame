#include "parser.h"
#include "string.h"
#include <stdio.h>
#include "ctype.h"
#include "SudokuGame.h"

/*Private functions declarations*/
int isLegalInt(char *str);
int isLegalFloat(char *str);
int wordCount(char *str);
/*Parsing functions*/
int parseMarkErrors(Command *command, char *str);
int parseSet(Command *command, char *str, int maxValue);
int parseGuess(Command *command, char *str);
int parseGenerate(Command *command, char *str, int maxValue);
int parseGuessHint(Command *command, char *str, int maxValue);
int parseHint(Command *command, char *str, int maxValue);
/*Set-to-command-template functions*/
int setFileTypeCommand(Command *command, char *str);
int setIntTypeCommand(Command *command, char *str, int boardLen);
int setNoParamCommand(char *str);


/**  parseCommandParams:
* @brief Parses the command parameters if they are correct
* @param Command* command - a pointer to command
* @param char* str - a pointer to string input of parameters
* @param int maxVal - upper bound to the parameters values
* @return int - 1 if parse is successful, 0 o/w
*/
int parseCommandParams(Command *command, char *str, int maxValue) {

    if (command->commandType == solve || command->commandType == edit || command->commandType == save) {
        return (setFileTypeCommand(command, str));
    }
    if (command->commandType == mark_errors || command->commandType == set || command->commandType == guess ||
        command->commandType == generate || command->commandType == hint || command->commandType == guess_hint) {
        return (setIntTypeCommand(command, str, maxValue));
    }
    return setNoParamCommand(str);

}

/**  setCommandName:
* @brief Parses the command name of a command
* @param Command* command - a pointer to command
* @param char* str - a pointer to string input
* @return int - 1 if parse is successful, 0 o/w
*/
int setCommandName(Command *command, char str[]) {
    char *token;

    token = strtok(str, " \n\r\t");

    if (strcmp("solve", token) == 0) {
        command->commandType = solve;
        return 1;
    }
    if (strcmp("edit", token) == 0) {
        command->commandType = edit;
        return 1;
    }
    if (strcmp("mark_errors", token) == 0) {
        command->commandType = mark_errors;
        return 1;
    }
    if (strcmp("print_board", token) == 0) {
        command->commandType = printBoard;
        return 1;
    }
    if (strcmp("set", token) == 0) {
        command->commandType = set;
        return 1;
    }
    if (strcmp("validate", token) == 0) {
        command->commandType = validate;
        return 1;
    }
    if (strcmp("guess", token) == 0) {
        command->commandType = guess;
        return 1;
    }
    if (strcmp("generate", token) == 0) {
        command->commandType = generate;
        return 1;
    }
    if (strcmp("undo", token) == 0) {
        command->commandType = undo;
        return 1;
    }
    if (strcmp("redo", token) == 0) {
        command->commandType = redo;
        return 1;
    }
    if (strcmp("save", token) == 0) {
        command->commandType = save;
        return 1;
    }
    if (strcmp("hint", token) == 0) {
        command->commandType = hint;
        return 1;
    }
    if (strcmp("guess_hint", token) == 0) {
        command->commandType = guess_hint;
        return 1;
    }
    if (strcmp("num_solutions", token) == 0) {
        command->commandType = num_solutions;
        return 1;
    }
    if (strcmp("autofill", token) == 0) {
        command->commandType = autofill;
        return 1;
    }
    if (strcmp("reset", token) == 0) {
        command->commandType = reset;
        return 1;
    }
    if (strcmp("exit", token) == 0) {
        command->commandType = ex;
        return 1;
    }
    printf("Error: Command does not exist\n");
    return 0;
}


/*determines if input represents a legal integer*/
int isLegalInt(char *str) {
    int val;
    char ch;

    if (sscanf(str, "%d%c", &val, &ch) == 2) {
        if (ch == '\0' || isspace(ch)) {
            if (val >= 0) {
                return 1;
            }
        }
    }
    return 0;
}

/*determines if input represents a legal float*/
int isLegalFloat(char *str) {
    float val;
    char ch;

    if (sscanf(str, "%f%c", &val, &ch) == 2) {
        if (ch == '\0' || isspace(ch)) {
            if (val >= 0) {
                return 1;
            }
        }
    }
    return 0;
}

/*counts num of words in a string*/
int wordCount(char *str) {
    int wordsRead = 0;
    char temp[256];
    char *token;

    strcpy(temp, str);

    token = strtok(temp, " \t\n\r");


    while (token != NULL) {
        wordsRead++;
        token = strtok(NULL, " \t\r\n");
    }

    return wordsRead;
}

/*all functions that parse correct command if params are correct, return 1 if parse is successful, 0 otherwise*/
int parseMarkErrors(Command *command, char *str) {
    int val;

    if (wordCount(str) != 1) {
        printf("Error: Wrong amount of parameters for mark_errors: should be 1\n");
        return 0;
    }

    if (sscanf(str, "%d", &val) == 1) {
        if (isLegalInt(str) && (val == 0 || val == 1)) {
            command->x = val;
            return 1;
        } else {
            printf("Error: Wrong parameter for mark_errors - must be 0 or 1\n");
        }
    } else {
        printf("Error: Wrong parameter for mark_errors - must be 0 or 1\n");
    }
    return 0;

}

int parseSet(Command *command, char *str, int maxValue) {
    int x, y, z;
    int stringsRead;
    int bytesRead = 0;
    int offset = 0;

    stringsRead = wordCount(str);

    if (stringsRead != 3) {
        printf("Error: Number of parameters is wrong - should be 3 for set\n");
        return 0;
    }

    if (sscanf(str + offset, "%d%n", &x, &bytesRead) == 1) {
        if (isLegalInt(str + offset)) {
            if (x > maxValue || x <= 0) {
                printf("Error: 1st param must be an integer between 1 and %d for set\n", maxValue);
                return 0;
            }
            command->x = x;
        } else {
            printf("Error: 1st param must be an integer between 1 and %d for set\n", maxValue);
            return 0;
        }
        offset = offset + bytesRead;
    } else {
        printf("Error: 1st param must be an integer between 1 and %d for set\n", maxValue);
        return 0;
    }

    if (sscanf(str + offset, "%d%n", &y, &bytesRead) == 1) {
        if (isLegalInt(str + offset)) {
            if (y > maxValue || y <= 0) {
                printf("Error: 2nd param must be an integer between 1 and %d for set\n", maxValue);
                return 0;
            }
            command->y = y;
        } else {
            printf("Error: 2nd param must be an integer between 1 and %d for set\n", maxValue);
            return 0;
        }
        offset = offset + bytesRead;
    } else {
        printf("Error: 2nd param must be an integer between 1 and %d for set\n", maxValue);
        return 0;
    }

    if (sscanf(str + offset, "%d", &z) == 1) {
        if (isLegalInt(str + offset)) {
            if (z > maxValue || z < 0) {
                printf("Error: 3rd param must be an integer between 0 and %d for set\n", maxValue);
                return 0;
            }
            command->z = z;
        } else {
            printf("Error: 3rd param must be an integer between 0 and %d for set\n", maxValue);
            return 0;
        }
    } else {
        printf("Error: 3rd param must be an integer between 0 and %d for set\n", maxValue);
        return 0;
    }
    return 1;
}

int parseGuess(Command *command, char *str) {
    float val;
    int stringsRead;

    stringsRead = wordCount(str);

    if (stringsRead != 1) {
        printf(" Error:Number of parameters is wrong - should be 1 for guess\n");
        return 0;
    }

    if (sscanf(str, "%f", &val) == 1) {
        if (isLegalFloat(str) && (val >= 0 && val <= 1)) {
            command->val = val;
            return 1;
        } else {
            printf("Error: Param for guess must be float between 0 and 1\n");
        }
    } else {
        printf("Error: Param for guess must be float between 0 and 1\n");
    }
    return 0;


}

int parseGenerate(Command *command, char *str, int maxValue) {
    int x, y;
    int stringsRead;
    int bytesRead = 0;
    int offset = 0;

    stringsRead = wordCount(str);

    if (stringsRead != 2) {
        printf("Error: Number of parameters is wrong - should be 2 for generate\n");
        return 0;
    }

    if (sscanf(str + offset, "%d%n", &x, &bytesRead) == 1) {
        if (isLegalInt(str + offset)) {
            if (x < 0) {
                printf("Error: 1st param must be an integer between 0 and %d for generate\n", maxValue);
                return 0;
            }
            command->x = x;
        } else {
            printf("Error: 1st param must be an integer between 0 and %d for generate\n", maxValue);
            return 0;
        }
        offset = offset + bytesRead;
    } else {
        printf("Error: 1st param must be an integer between 0 and %d for generate\n", maxValue);
        return 0;
    }

    if (sscanf(str + offset, "%d%n", &y, &bytesRead) == 1) {
        if (isLegalInt(str + offset)) {
            if (y > maxValue || y <= 0) {
                printf("Error: 2nd param must be an integer between 1 and %d for generate\n", maxValue);
                return 0;
            }
            command->y = y;
        } else {
            printf("Error: 2nd param must be an integer between 1 and %d for generate\n", maxValue);
            return 0;
        }
    } else {
        printf("Error: 2nd param must be an integer between 1 and %d for generate\n", maxValue);
        return 0;
    }
    return 1;

}

int parseGuessHint(Command *command, char *str, int maxValue) {
    int x, y;
    int stringsRead;
    int bytesRead = 0;
    int offset = 0;

    stringsRead = wordCount(str);

    if (stringsRead != 2) {
        printf("Error: Number of parameters is wrong - should be 2 for guess_hint\n");
        return 0;
    }

    if (sscanf(str + offset, "%d%n", &x, &bytesRead) == 1) {
        if (isLegalInt(str + offset)) {
            if (x > maxValue || x <= 0) {
                printf("Error: 1st param must be an integer between 1 and %d for guess_hint\n", maxValue);
                return 0;
            }
            command->x = x;
        } else {
            printf("Error: 1st param must be an integer between 1 and %d for guess_hint\n", maxValue);
            return 0;
        }
        offset = offset + bytesRead;
    } else {
        printf("Error: 1st param must be an integer between 1 and %d for guess_hint\n", maxValue);
        return 0;
    }

    if (sscanf(str + offset, "%d%n", &y, &bytesRead) == 1) {
        if (isLegalInt(str + offset)) {
            if (y > maxValue || y <= 0) {
                printf("Error: 2nd param must be an integer between 1 and %d for guess_hint\n", maxValue);
                return 0;
            }
            command->y = y;
        } else {
            printf("Error: 2nd param must be an integer between 1 and %d for guess_hint\n", maxValue);
            return 0;
        }
    } else {
        printf("Error: 2nd param must be an integer between 1 and %d for guess_hint\n", maxValue);
        return 0;
    }
    return 1;

}

int parseHint(Command *command, char *str, int maxValue) {
    int x, y;
    int stringsRead;
    int bytesRead = 0;
    int offset = 0;

    stringsRead = wordCount(str);

    if (stringsRead != 2) {
        printf("Error: Number of parameters is wrong - should be 2 for hint\n");
        return 0;
    }

    if (sscanf(str + offset, "%d%n", &x, &bytesRead) == 1) {
        if (isLegalInt(str + offset)) {
            if (x > maxValue || x <= 0) {
                printf("Error: 1st param must be an integer between 1 and %d for hint\n", maxValue);
                return 0;
            }
            command->x = x;
        } else {
            printf("Error: 1st param must be an integer between 1 and %d for hint\n", maxValue);
            return 0;
        }
        offset = offset + bytesRead;
    } else {
        printf("Error: 1st param must be an integer between 1 and %d for hint\n", maxValue);
        return 0;
    }

    if (sscanf(str + offset, "%d%n", &y, &bytesRead) == 1) {
        if (isLegalInt(str + offset)) {
            if (y > maxValue || y <= 0) {
                printf("Error: 2nd param must be an integer between 1 and %d for hint\n", maxValue);
                return 0;
            }
            command->y = y;
        } else {
            printf("Error: 2nd param must be an integer between 1 and %d for hint\n", maxValue);
            return 0;
        }
    } else {
        printf("Error: 2nd param must be an integer between 1 and %d for hint\n", maxValue);
        return 0;
    }
    return 1;

}

int setFileTypeCommand(Command *command, char *str) {
    char *fileName;
    int wordsRead;

    wordsRead = wordCount(str);
    if (wordsRead == 0) {
        if (command->commandType == edit) {
            command->fileName = NULL;
            return 1;
        }
        printf("Error: Wrong amount of parameters for command - should be 1 file name\n");
        return 0;
    }

    if (wordsRead != 1) {
        printf("Error: Wrong amount of parameters for command - should be 1 file name\n");
        return 0;
    }

    fileName = strtok(str, " \n\r\t");
    command->fileName = fileName;
    return 1;
}

int setIntTypeCommand(Command *command, char *str, int boardLen) {
    if (command->commandType == mark_errors) {
        return parseMarkErrors(command, str);
    }
    if (command->commandType == set) {
        return parseSet(command, str, boardLen);
    }
    if (command->commandType == guess) {
        return parseGuess(command, str);
    }
    if (command->commandType == generate) {
        return parseGenerate(command, str, boardLen * boardLen);
    }
    if (command->commandType == hint) {
        return parseHint(command, str, boardLen);
    }
    if (command->commandType == guess_hint) {
        return parseGuessHint(command, str, boardLen);
    }


    return 0;
}

int setNoParamCommand(char *str) {
    if (wordCount(str) != 0) {
        printf("Error: Wrong number of parameters for command - should be 0\n");
        return 0;
    }
    return 1;
}






