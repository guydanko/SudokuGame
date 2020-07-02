#ifndef FINALPROJECT_COMMAND_H
#define FINALPROJECT_COMMAND_H

typedef enum commandType{solve,edit,mark_errors, set,printBoard,validate,guess,generate,undo,redo,save,hint,guess_hint,num_solutions,autofill,reset,ex }COMMAND_TYPE;

typedef struct command{
    COMMAND_TYPE commandType;
    int x;
    int y;
    int z;
    float val;
    char * fileName;
}Command;

#endif
