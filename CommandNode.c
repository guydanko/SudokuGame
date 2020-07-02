#include "CommandNode.h"
#include <stdlib.h>
#include <stdio.h>

/**  setCommandNode:
* @brief Initialize a new CommandNode instance
* @param commandNode* commandNode - an allocated commandNode to update with member values
* @param Command* command - pointer to the new Command
* @param int prevVal - previous value of the cell that command was performed on
* @return void
*/
void setCommandNode(CommandNode *commandNode, Command *command, int prevVal) {
    commandNode->command = command;
    commandNode->prevVal = prevVal;
    commandNode->next = NULL;
    commandNode->prev = NULL;
}

/**  freeCommandNodes:
* @brief Frees memory of all commandNodes in the list
* @param commandNode* head - pointer to first commandNode in the list
* @return void
*/
void freeCommandNodes(CommandNode *head) {
    CommandNode *temp = head;
    while (head != NULL) {
        head = head->next;
        free(temp->command);
        free(temp);
        temp = head;
    }
}

