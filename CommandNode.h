#ifndef FINALPROJECT_COMMANDNODE_H
#define FINALPROJECT_COMMANDNODE_H

#include "command.h"

/* A structure to represent a doubly linked list of commands entered by the user */
/* Member: Command* command - a pointer to a command entered by the user*/
/* Member: CommandNode* next - a pointer to the next command in the list */
/* Member: CommandNode* prev - a pointer to the previous command in the list */
/* Member: int prevVal - holds the previous value of the cell before the change*/
typedef struct CommandNode {
    Command *command;
    struct CommandNode *next;
    struct CommandNode *prev;
    int prevVal;
} CommandNode;

/*Public functions declarations*/
void setCommandNode(CommandNode *commandNode, Command *command, int prevVal);
void freeCommandNodes(CommandNode *head);

#endif

