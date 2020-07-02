#ifndef FINALPROJECT_MOVENODE_H
#define FINALPROJECT_MOVENODE_H

#include "CommandNode.h"

/* A structure to represent a doubly linked list of moves performed by the user */
/* Member: CommandNode* firstCommandNode - a pointer to the first command in the list of commands performed by move */
/* Member: CommandNode* lastCommandNode - a pointer to the last command in the list of commands performed by move */
/* Member: MoveNode* next - a pointer to the next move in the list */
/* Member: MovedNode* prev - a pointer to the next move in the list*/
/* Member: int isHead - indicator for the "fake-head-move" that is the first in the list and represent it's beginning*/
typedef struct MoveNode{
    CommandNode* firstCommandNode;
    CommandNode* lastCommandNode;
    struct MoveNode * next;
    struct MoveNode * prev;
    int isHead;
}MoveNode;

/*Public functions declarations*/
void initMoveList(MoveNode* head);
void pushMove(MoveNode *currentMove);
void clearNextMoves(MoveNode *currentMove);
void clearAllMoves(MoveNode *currentMove);
void addCommandToMove(MoveNode* currentMove, CommandNode *commandNode);
#endif
