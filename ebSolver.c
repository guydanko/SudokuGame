#include <stdio.h>
#include <stdlib.h>
#include "ebSolver.h"

/*Private Stack functions declarations*/
struct StackNode* newNode(int value,int cellIndex);
int isEmpty(struct StackNode* root);
void push(struct StackNode** root, int value,int cellIndex);
void pop(struct StackNode** root);

/*Private Solver functions declarations*/
int solverStack(sudokuBoard *sBoard,int* currValids,struct StackNode* stackTop);
int updateValids(sudokuBoard *sBoard,int* currValids,int row,int col,int block,int minValue);
void changeBoard(sudokuBoard *sBoard,int row,int col,int block,int index,int mod,int value);
int findMinValid(int *currValids,int minVal,int N);


/**  newNode:
* @brief Creates a new StackNode
* @param int value - value to define the new StackNode
* @param int cellIndex - cellIndex to define the new StackNode
* @return struct StackNode* - a new instance of a StackNode with value and cellIndex members
*/
struct StackNode* newNode(int value,int cellIndex)
{
    struct StackNode* stackNode;
    stackNode = (struct StackNode*)malloc(sizeof(struct StackNode));
    stackNode->value = value;
    stackNode->cellIndex = cellIndex;
    stackNode->next = NULL;
    return stackNode;
}

/**  isEmpty:
* @brief Checks if the stack is empty
* @param struct StackNode* - a pointer to the root/top of the stack
* @return int - 1 if the stack is empty , 0 o\w
*/
int isEmpty(struct StackNode* root)
{
    return !root;
}

/**  push:
* @brief Adds a new node to the top of the stack
* @param struct StackNode** root - a pointer to the root/top-of-the-stack pointer
* @param int value - value to define the new StackNode being pushed
* @param int cellIndex - cellIndex to define the new StackNode being pushed
* @return void
*/
void push(struct StackNode** root, int value ,int cellIndex)
{
    struct StackNode* stackNode;
    stackNode = newNode(value,cellIndex);
    stackNode->next = *root;
    *root = stackNode;
}

/**  pop:
* @brief Removes a node from the top of the stack
* @param struct StackNode** root - a pointer to the root/top-of-the-stack pointer
* @return void
*/
void pop(struct StackNode** root)
{
    if (isEmpty(*root)){
        printf("Error: pop from empty stack");
        exit(0);
    }else{
        struct StackNode* temp = *root;
        *root = (*root)->next;
        free(temp);
    }
    return ;
}


/**  solveStackEnv:
* @brief The main function for game command num_solutions, serves as an envelope function for solveStack
* @param sudokuBoard *sBoard - a pointer to the sudoku board being checked
* @return int - the number of different valid solutions to the board
*/
int solveStackEnv(sudokuBoard *sBoard){
    struct StackNode* stackTop = NULL;
    int *currValids;
    int count;
    Index_Pair ip;
    /*Check if board is full*/
    ip = find_empty_cell(sBoard);
    if (ip.x == -1 && ip.y==-1)
        return 1;/*Solved*/
    /*init for solveStack*/
    currValids = (int*)calloc(sBoard->length,sizeof(int));
    if(!currValids){
        printf("Error: memory allocation failure, Exiting...");
        exit(0);
    }
    push(&stackTop,0,0); /*Set a bottom marker for the stack*/
    count = solverStack(sBoard,currValids,stackTop);
    free(currValids);
    free(stackTop); /*maybe add check that stack is empty*/
    return count;
}

/**  solveStack:
* @brief The algorithmic function for num_solutions
* @param sudokuBoard *sBoard - a pointer to the sudoku board being checked
* @param int *currValids - a boolean array that represents valid options to a value and cell being checked
* @param struct StackNode* stackTop - a pointer to the top StackNode of the recursion stack
* @return int - the number of different valid solutions to the board
*/
int solverStack(sudokuBoard *sBoard,int* currValids,struct StackNode* stackTop){
    int count=0,N=sBoard->length;
    int row,col,block,valids,pushFlag=1,ROWS,COLS;
    ROWS=sBoard->rowsInBlock,COLS=sBoard->colsInBlock;
    do{
        if(pushFlag==1){/*Pushing Mode*/
            /*Work with current cell*/
            row = (stackTop->cellIndex)/N;
            col = (stackTop->cellIndex) - (row*N);
            push(&stackTop,sBoard->board[row][col].value,(stackTop->cellIndex)+1);
        }
        else{/*Popping Mode*/
            pop(&stackTop);
            if(stackTop->cellIndex<1)
                break; /*Reached bottom marker*/
            /*Work with previous cell*/
            row = (stackTop->cellIndex-1) / N;
            col = (stackTop->cellIndex-1) - (row*N);
        }
        if((sBoard->board[row][col].userMod==0)&&(sBoard->board[row][col].fixed==0)){
            /*Cell in need of solving: not fixed and not user-modified*/
            block = ROWS*(row/ROWS)+(col/COLS);
            valids = updateValids(sBoard,currValids,row,col,block,stackTop->value);
            if(valids<=0){/*No valid values for this cell*/
                if(pushFlag==0){/*Popping rollout*/
                    changeBoard(sBoard,row,col,block,stackTop->value,0,0); /* #define CLEAR_CELL*/
                }
                pushFlag=0;/*turn to Popping Mode*/
            }
            else{/*There are valid values for this cell*/
                if(pushFlag==1){/*Pushing rollout*/
                    stackTop->value=findMinValid(currValids,0,N);/*enter any valid*/
                }
                else{/*Stop popping get ready for pushing*/
                    changeBoard(sBoard,row,col,block,stackTop->value,0,0);
                    stackTop->value=findMinValid(currValids,stackTop->value+1,N);/*enter valid greater than top value*/
                }
                if((row==(N-1))&&(col==(N-1))){ /*End of board*/
                    count++;
                    pushFlag=0;
                }
                else{
                    changeBoard(sBoard,row,col,block,stackTop->value,1,stackTop->value+1);/*#define SET_CELL*/
                    pushFlag=1;
                }
            }
        }
        else{
            /*Cell is in no need to solve: either fixed or was user-modified*/
            if((row==(N-1))&&(col==(N-1))){ /*End of board*/
                count++;
                pushFlag=0;
            }
        }
    }while(stackTop->cellIndex!=0);
    return count;
}

/**  updateValids:
* @brief Updates the currValids boolean array for a board cell
* @param sudokuBoard *sBoard - a pointer to the sudoku board being checked
* @param int *currValids - a boolean array that represents valid options to a value and cell being checked
* @param int row / col / block - indices to represent the location of a cell within the board
* @param int minValue - the minimal valid value to search for this cell
* @return int - the number of different valid values for the cell (can be in range 0 to N)
*/
int updateValids(sudokuBoard *sBoard,int* currValids,int row,int col,int block,int minValue){
    int i,N=sBoard->length;
    int valids = N - minValue;
    for(i=minValue;i<N;i++){
        if((sBoard->invalidRows[row][i]==1)||(sBoard->invalidCols[col][i]==1)||(sBoard->invalidBlocks[block][i]==1)){
            /*For any reason why i might not be a valid value*/
            currValids[i]=0;
            valids--;
        }
        else{
            currValids[i]=1;
        }
    }
    return valids;
}

/**  updateValids:
* @brief Updates the sudoku board members to align with solveStack
* @param sudokuBoard *sBoard - a pointer to the sudoku board being checked
* @param int row / col / block - indices to represent the location of a cell within the board
* @param int index - index for value within the invalidsArrays
* @param int mod - changes to the invalidsArrays ranged 0 to 1
* @param int value - value to assign for cell
* @return void
*/
void changeBoard(sudokuBoard *sBoard,int row,int col,int block,int index,int mod,int value){
    sBoard->board[row][col].value=value;
    sBoard->invalidRows[row][index]=mod;
    sBoard->invalidCols[col][index]=mod;
    sBoard->invalidBlocks[block][index]=mod;
    return;
}

/**  findMinValid:
* @brief Finds the minimal valid value for a cell (via currValids)
* @param int *currValids - a boolean array that represents valid options to a value and cell being checked
* @param int minVal - lower bound for minimal valid value
* @param int N - upper bound for minimal valid value
* @return int - minimal valid value found , or -1 if none found
*/
int findMinValid(int *currValids,int minVal,int N){
    int i;
    for(i=minVal;i<N;i++){
        if(currValids[i]==1)
            return i;
    }
    return -1;
}



