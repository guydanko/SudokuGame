CC = gcc
OBJS = main.o ebSolver.o fileHandler.o gameLogic.o SudokuBoard.o SudokuGame.o parser.o CommandNode.o MoveNode.o LPSolver.o
EXEC = sudoku-console
COMP_FLAG = -ansi -Wall -Wextra -Werror -pedantic-errors
GUROBI_COMP = -I/usr/local/lib/gurobi563/include
GUROBI_LIB = -L/usr/local/lib/gurobi563/lib -lgurobi56

all : $(EXEC)
$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(GUROBI_LIB) -o $@ -lm
main.o: main.c SudokuBoard.h SudokuGame.h gameLogic.h fileHandler.h ebSolver.h SudokuGame.h parser.h command.h CommandNode.h MoveNode.h LPSolver.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
ebSolver.o: ebSolver.c ebSolver.h
	$(CC) $(COMP_FLAG) -c $*.c
fileHandler.o: fileHandler.c fileHandler.h
	$(CC) $(COMP_FLAG) -c $*.c
gameLogic.o: gameLogic.c gameLogic.h
	$(CC) $(COMP_FLAG) -c $*.c
SudokuBoard.o: SudokuBoard.c SudokuBoard.h
	$(CC) $(COMP_FLAG) -c $*.c
SudokuGame.o: SudokuGame.c SudokuGame.h
	$(CC) $(COMP_FLAG) -c $*.c
parser.o: parser.c parser.h
	$(CC) $(COMP_FLAG) -c $*.c
CommandNode.o: CommandNode.c CommandNode.h
	$(CC) $(COMP_FLAG) -c $*.c
MoveNode.o: MoveNode.c MoveNode.h
	$(CC) $(COMP_FLAG) -c $*.c
LPSolver.o: LPSolver.c LPSolver.h
	$(CC) $(COMP_FLAG) -c $*.c
clean:
	rm -f $(OBJS) $(EXEC)
