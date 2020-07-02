#include "MoveNode.h"
#include <stdlib.h>
#include <stdio.h>

/**  initMoveList:
* @brief Starts a new move list - initiating head to "first fake" move
* @param MoveNode* head - an allocated MoveNode to be set as head
* @return void
*/
void initMoveList(MoveNode* head) {
    head->firstCommandNode = NULL;
    head->lastCommandNode = NULL;
    head->prev = NULL;
    head->next = NULL;
    head->isHead = 1;
}

/**  pushMove:
* @brief Pushes a new successful move into the move list
* @param MoveNode* currentMove - a pointer to the current move of the game
* @return void
*/
void pushMove(MoveNode* currentMove){
    MoveNode *newMove, *nextMove;
    nextMove = currentMove->next;
    /*creates new moveNode for nextMove*/
    newMove = (MoveNode *) malloc(sizeof(MoveNode));
    if(newMove == NULL){
        printf("Error: memory allocation has failed in pushMove, Exiting...\n");
        exit(0);
    }
    /*push new move into list*/
    newMove->firstCommandNode = NULL;
    newMove->lastCommandNode = NULL;
    newMove->isHead = 0;
    newMove->next = nextMove;
    if(nextMove!=NULL){
        nextMove->prev = newMove;
    }
    newMove->prev = currentMove;
    currentMove->next = newMove;
}

/**  clearNextMoves:
* @brief Clears all the next moves in the game's move list performed after the current move
* @param MoveNode* currentMove - a pointer to the current move of the game
* @return void
*/
void clearNextMoves(MoveNode *currentMove) {
    MoveNode *temp;
    if (currentMove->next == NULL) {
        return;
    }
    /*free all moves after, including commandNodes of each Move*/
    temp = currentMove->next;
    currentMove->next = NULL;
    currentMove = temp;
    while (temp != NULL) {
        temp = temp->next;
        freeCommandNodes(currentMove->lastCommandNode);
        free(currentMove);
        currentMove = temp;
    }
}

/**  clearAllMoves:
* @brief Clears all the moves in the game's move list
* @param MoveNode* currentMove - a pointer to the current move of the game
* @return void
*/
void clearAllMoves(MoveNode *currentMove) {
    MoveNode *temp;
    temp = currentMove;
    clearNextMoves(currentMove);
    currentMove = temp;
    /*clear all previous moves from current move including itself*/
    while (temp!=NULL) {
        temp = temp->prev;
        freeCommandNodes(currentMove->lastCommandNode);
        free(currentMove);
        currentMove = temp;
    }
}

/**  addCommandToMove:
* @brief Adds a Command to the Move's Commands list
* @param MoveNode* currentMove - a pointer to the current move of the game
* @param CommandNode *commandNode - a pointer to the commandNode that needs to be added
* @return void
*/
void addCommandToMove(MoveNode* currentMove, CommandNode *commandNode) {
    CommandNode *currentCommand = currentMove->lastCommandNode;
    /*if empty command list*/
    if (currentCommand == NULL) {
        currentMove->firstCommandNode = commandNode;
        currentMove->lastCommandNode = commandNode;
        return;
    }
    currentMove->lastCommandNode = commandNode;
    commandNode->next = currentCommand;
    if (currentCommand != NULL) {
        currentCommand->prev = commandNode;
    }
}

