#ifndef FINALPROJECT_PARSER_H
#define FINALPROJECT_PARSER_H

#include "command.h"

/*Public functions declarations*/
int parseCommandParams(Command *command, char *str, int maxValue);
int setCommandName(Command *command, char *str);

#endif
