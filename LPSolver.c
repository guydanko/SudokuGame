#include <stdlib.h>
#include <stdio.h>
#include "gurobi_c.h"
#include "LPSolver.h"


/*private functions declarations*/
void allocateMemoryGRB(int DIM,double **lb,double **val, double **resBoard,char **vtype,int **ind,double **obj,char type);
void freeMemoryGRB(int *ind,char *vtype,double *resBoard,double *val,double *lb,double *obj,char type);
int addConstraints(GRBmodel *model,int *ind,double *val,int DIM,int SUBDIMROW,int SUBDIMCOL,char type);
int getILPSolution(GRBenv *env, GRBmodel *model, double* resBoard, sudokuGame* sGame, int DIM);
int getGuess(GRBenv *env, GRBmodel *model, double* resBoard, sudokuGame* sGame, int DIM,double x);
int getGuessHint(GRBenv *env, GRBmodel *model, double* resBoard, sudokuGame* sGame, int DIM,double x);
void randomizeCoefs(double *obj,int DIM);


/**  ILPSolver:
* @brief Generates a solution to a sudoku game board using Integer-Linear-Programming tools
* @param sudokuGame *sGame - a pointer to the sudoku game being played
* @return int - 1 if the board was solved and currSol member of the game was updated, 0 o/w
*/
int ILPSolver(sudokuGame *sGame){
    int SUBDIMROW,SUBDIMCOL,DIM;
    GRBenv *env = NULL;
    GRBmodel *model = NULL;
    double *lb=NULL,*val=NULL,*resBoard=NULL,*obj=NULL,objval;
    char *vtype=NULL;
    int i,j,v,error=0,optimstatus, *ind = NULL;
    SUBDIMROW = sGame->currBoard->rowsInBlock , SUBDIMCOL = sGame->currBoard->colsInBlock;
    DIM = sGame->currBoard->length;

    /*allocate memory for all auxiliary arrays*/
    allocateMemoryGRB(DIM,&lb,&val,&resBoard,&vtype,&ind,&obj,'I');

    /* Create an empty model */
    for (i = 0; i < DIM; i++) {
        for (j = 0; j < DIM; j++) {
            for (v = 0; v < DIM; v++) {
                if (sGame->currBoard->board[i][j].value == v+1)
                    lb[i*DIM*DIM+j*DIM+v] = 1;
                else
                    lb[i*DIM*DIM+j*DIM+v] = 0;
                vtype[i*DIM*DIM+j*DIM+v] = GRB_BINARY;
            }
        }
    }
    /* Create environment */
    error = GRBloadenv(&env, "sudokuILP.log");
    if (error) goto QUIT;
    error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
    if (error) goto QUIT;

    /* Create new model */
    error = GRBnewmodel(env, &model, "sudoku", DIM*DIM*DIM, NULL, lb, NULL,vtype, NULL);
    if (error) goto QUIT;



    /*Add sudokuBoard constraints*/
    error = addConstraints(model,ind,val,DIM,SUBDIMROW,SUBDIMCOL,'I');
    if (error) goto QUIT;

    /* Optimize model */
    error = GRBoptimize(model);
    if (error) goto QUIT;

    /* Write model to 'sudoku.lp' */
    error = GRBwrite(model, "sudokuILP.lp");
    if (error) goto QUIT;

    /* Capture solution information */
    error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
    if (error) goto QUIT;
    error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
    if (error) goto QUIT;

    error = getILPSolution(env,model,resBoard,sGame,DIM);
    if (error) goto QUIT;
    QUIT: /* Error reporting */
    if (error){
        freeMemoryGRB(ind,vtype,resBoard,val,lb,obj,'I');
        GRBfreemodel(model);
        GRBfreeenv(env);
        return 0;
    }

    freeMemoryGRB(ind,vtype,resBoard,val,lb,obj,'I');
    GRBfreemodel(model);
    GRBfreeenv(env);
    return 1; /*Success*/
}
/**  addConstraints:
* @brief Adds all LP constraints for the sudoku board to the Gurobi model
* @param GRBmodel *model - a pointer to the Gurobi model used to solve the board
* @param int *ind - array to define which variables will participate in a constraint
* @param double *val - array to define the coefficients for each variable participating in a constraint
* @param int DIM/SUBDIMROW/SUBDIMCOL - indicates the size of the board being solved
* @param char type - differs between adding constraints of an ILP solver or a LP solver
* @return int - returns 0 on success or 1 o/w
*/
int addConstraints(GRBmodel *model,int *ind,double *val,int DIM,int SUBDIMROW,int SUBDIMCOL,char type){
    int i,j,v,ig,jg,count,error=0;
    /* Each cell gets a value */
    for (i = 0; i < DIM; i++) {
        for (j = 0; j < DIM; j++) {
            for (v = 0; v < DIM; v++) {
                ind[v] = i*DIM*DIM + j*DIM + v;
                val[v] = 1.0;
            }
            error = GRBaddconstr(model, DIM, ind, val, GRB_EQUAL, 1.0, NULL);
            if (error) return 1;
        }
    }
    if(type=='L'){
        /*Non-negativity for continuous variables for LP version*/
        for (i = 0; i < DIM; i++) {
            for (j = 0; j < DIM; j++) {
                for (v = 0; v < DIM; v++) {
                    ind[0] = i*DIM*DIM + j*DIM + v;
                    val[0] = 1.0;
                    error = GRBaddconstr(model, 1, ind, val, GRB_GREATER_EQUAL, 0.0, NULL);
                    if (error) return 1;
                }
            }
        }
        /*Upper-bounding for continous variables for LP version*/
        for (i = 0; i < DIM; i++) {
            for (j = 0; j < DIM; j++) {
                for (v = 0; v < DIM; v++) {
                    ind[0] = i*DIM*DIM + j*DIM + v;
                    val[0] = 1.0;
                    error = GRBaddconstr(model, 1, ind, val, GRB_LESS_EQUAL, 1.0, NULL);
                    if (error) return 1;
                }
            }
        }
    }
    /* Each value must appear once in each row */
    for (v = 0; v < DIM; v++) {
        for (j = 0; j < DIM; j++) {
            for (i = 0; i < DIM; i++) {
                ind[i] = i*DIM*DIM + j*DIM + v;
                val[i] = 1.0;
            }
            error = GRBaddconstr(model, DIM, ind, val, GRB_EQUAL, 1.0, NULL);
            if (error) return 1;
        }
    }

    /* Each value must appear once in each column */
    for (v = 0; v < DIM; v++) {
        for (i = 0; i < DIM; i++) {
            for (j = 0; j < DIM; j++) {
                ind[j] = i*DIM*DIM + j*DIM + v;
                val[j] = 1.0;
            }
            error = GRBaddconstr(model, DIM, ind, val, GRB_EQUAL, 1.0, NULL);
            if (error) return 1;
        }
    }
    /* Each value must appear once in each subgrid */
    for (v = 0; v < DIM; v++) {
        for (ig = 0; ig < SUBDIMCOL; ig++) { /*Check if order of SUBDIMCOL and SUBDIMROW is OK*/
            for (jg = 0; jg < SUBDIMROW; jg++) {
            count = 0;
            for (i = ig*SUBDIMROW; i < (ig+1)*SUBDIMROW; i++) {
                for (j = jg*SUBDIMCOL; j < (jg+1)*SUBDIMCOL; j++) {
                    ind[count] = i*DIM*DIM + j*DIM + v;
                    val[count] = 1.0;
                    count++;
                }
            }
            error = GRBaddconstr(model, DIM, ind, val, GRB_EQUAL, 1.0, NULL);
            if (error) return 1;
            }
        }
    }
    return 0;/*Success*/
}
/**  allocateMemoryGRB:
* @brief allocates memory to all relevant arrays for the ILP/LP solvers
* @param int DIM - indicates the size of the board being solved
* @param double **lb- a pointer to an array that represents variables lower-bounds
* @param double **val - a pointer to an array that defines the coefficients for each variable participating in a constraint
* @param double **resBoard - a pointer to an array that will get the result of the Gurobi model
* @param char **vtype - a pointer to an array that defines the variable types (e.g. BINARY,CONTINOUOS)
* @param int **ind - a pointer to an array that defines which variables will participate in a constraint
* @param double **obj - a pointer to an array that represents the objective function for LP solver
* @param char type - differs between memory allocation of an ILP solver or a LP solver
* @return void
*/
void allocateMemoryGRB(int DIM,double **lb,double **val, double **resBoard,char **vtype,int **ind,double **obj,char type){
    *lb = (double *) malloc(DIM*DIM*DIM*sizeof(double));
    if(*lb==NULL){
        printf("Error: memory allocation failed\n");
        exit(0);
    }
    *val = (double *) malloc(DIM*sizeof(double));
    if(*val==NULL){
        printf("Error: memory allocation failed\n");
        exit(0);
    }
    *resBoard = (double *) malloc(DIM*DIM*DIM*sizeof(double));
    if(*resBoard==NULL){
        printf("Error: memory allocation failed\n");
        exit(0);
    }
    *vtype = (char *) malloc(DIM*DIM*DIM*sizeof(char));
    if(*vtype==NULL){
        printf("Error: memory allocation failed\n");
        exit(0);
    }
    *ind = (int *) malloc(DIM*sizeof(int));
    if(*ind==NULL){
        printf("Error: memory allocation failed\n");
        exit(0);
    }
    if (type=='L'){
        *obj = (double *) malloc(DIM*DIM*DIM*sizeof(double));
        if(*obj==NULL){
            printf("Error: memory allocation failed\n");
            exit(0);
        }
    }
    return;
}

/**  freeMemoryGRB:
* @brief frees memory to all relevant arrays for the ILP/LP solvers
* @param double *lb- array that represents variables lower-bounds
* @param double *val - array that defines the coefficients for each variable participating in a constraint
* @param double *resBoard - array that will get the result of the Gurobi model
* @param char *vtype - array that defines the variable types (e.g. BINARY,CONTINOUOS)
* @param int *ind - array that defines which variables will participate in a constraint
* @param double *obj - array that represents the objective function for LP solver
* @param char type - differs between memory allocation of an ILP solver or a LP solver
* @return void
*/
void freeMemoryGRB(int *ind,char *vtype,double *resBoard,double *val,double *lb,double *obj,char type){
    free(ind);
    free(vtype);
    free(resBoard);
    free(val);
    free(lb);
    if(type=='L'){
        free(obj);
    }
    return;
}

/**  getILPSolution:
* @brief Generates from the Gurobi model solution an update to the game's currSol board
* @param GRBenv *env - a pointer to the Gurobi environment used to solve the board
* @param GRBmodel *resBoard - array that will get the result of the Gurobi model
* @param sudokuGame *sGame - a pointer to the sudoku game being played
* @param int DIM - indicates the size of the board being solved
* @return int - returns 0 on success or 1 o/w
*/
int getILPSolution(GRBenv *env, GRBmodel *model, double* resBoard, sudokuGame* sGame, int DIM) {
	int i, j, v, error;

	error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, DIM*DIM*DIM,	resBoard);
	if (error) {
		printf("ERROR %d GRBgetdblattrarray(): %s\n", error,GRBgeterrormsg(env));
		return 1;
	}
	for (i = 0; i < DIM; i++) {
		for (j = 0; j < DIM; j++) {
			for (v = 0; v < DIM; v++) {
				if (resBoard[i * DIM * DIM + j * DIM + v] == 1)
					sGame->currSol[i][j] = v + 1;
			}
		}
	}
	return 0;
}

/**  LPSolverGuess:
* @brief Generates a guessed values for all empty cells of the board and fills all guesses with LP-value greater than X
* @param sudokuGame *sGame - a pointer to the sudoku game being played
* @param double x - a threshold number for setting guesses to the board
* @return int - 1 if LP was successful , 0 o\w
*/
int LPSolverGuess(sudokuGame *sGame,double x){
    int SUBDIMROW,SUBDIMCOL,DIM;
    GRBenv *env = NULL;
    GRBmodel *model = NULL;
    double *lb=NULL,*val=NULL,*resBoard=NULL,*obj=NULL,objval;
    char *vtype=NULL;
    int i,j,v,error=0,optimstatus, *ind = NULL;
    SUBDIMROW = sGame->currBoard->rowsInBlock , SUBDIMCOL = sGame->currBoard->colsInBlock;
    DIM = sGame->currBoard->length;

    /*allocate memory for all auxiliary arrays*/
    allocateMemoryGRB(DIM,&lb,&val,&resBoard,&vtype,&ind,&obj,'L');

    /* Create an empty model */
    for (i = 0; i < DIM; i++) {
        for (j = 0; j < DIM; j++) {
            for (v = 0; v < DIM; v++) {
                if (sGame->currBoard->board[i][j].value == v+1)
                    lb[i*DIM*DIM+j*DIM+v] = 1;
                else
                    lb[i*DIM*DIM+j*DIM+v] = 0;
                vtype[i*DIM*DIM+j*DIM+v] = GRB_CONTINUOUS;
            }
        }
    }

    /*Randomize coefficients for the objective value)*/
    randomizeCoefs(obj,DIM);

    /* Create environment */
    error = GRBloadenv(&env, "sudokuLP.log");
    if (error) goto QUIT;
    error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
    if (error) goto QUIT;


    /* Create new model */
    error = GRBnewmodel(env, &model, "sudoku", DIM*DIM*DIM, obj, lb, NULL,vtype, NULL);
    if (error) goto QUIT;

    /* Change objective sense to maximization */
    error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
    if (error) goto QUIT;

    /* update the model - to integrate new variables */
    error = GRBupdatemodel(model);
    if (error) goto QUIT;

    /*Add sudokuBoard constraints*/
    error = addConstraints(model,ind,val,DIM,SUBDIMROW,SUBDIMCOL,'L');
    if (error) goto QUIT;

    /* Optimize model */
    error = GRBoptimize(model);
    if (error) goto QUIT;

    /* Write model to 'sudoku.lp' */
    error = GRBwrite(model, "sudokuLP.lp");
    if (error) goto QUIT;

    /* Capture solution information */
    error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
    if (error) goto QUIT;
    error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
    if (error) goto QUIT;

    /*Getting solution*/
    error = getGuess(env,model,resBoard,sGame,DIM,x);
    if (error) goto QUIT;


    QUIT: /* Error reporting */
    if (error){
        freeMemoryGRB(ind,vtype,resBoard,val,lb,obj,'I');
        GRBfreemodel(model);
        GRBfreeenv(env);
        return 0;
    }
    freeMemoryGRB(ind,vtype,resBoard,val,lb,obj,'L');
    GRBfreemodel(model);
    GRBfreeenv(env);
    return 1;

}

/**  randomizeCoefs:
* @brief Randomizes and sets an objective function for the LP program
* @param double *obj - array that represents the objective function for LP solver
* @param int DIM - indicates the size of the board being solved
* @return void
*/
void randomizeCoefs(double *obj,int DIM){
    int i,j,v;
    for (i = 0; i < DIM; i++) {
            for (j = 0; j < DIM; j++) {
                for (v = 0; v < DIM; v++) {
                    obj[i*DIM*DIM + j*DIM + v]=rand()%(2*DIM);
                }
            }
        }
}

/**  getGuess:
* @brief Generates from the Gurobi model guesses for each cell and sets relevant cells in the game board
* @param GRBenv *env - a pointer to the Gurobi environment used to solve the board
* @param GRBmodel *resBoard - array that will get the result of the Gurobi model
* @param sudokuGame *sGame - a pointer to the sudoku game being played
* @param int DIM - indicates the size of the board being solved
* @param double x - a threshold number for setting guesses to the board
* @return int - returns 0 on success or 1 o/w
*/
int getGuess(GRBenv *env,GRBmodel *model,double *resBoard, sudokuGame *sGame,int DIM,double x){
	int i, j, v, count,countValues,k, error;
	int *values;

	error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0,DIM*DIM*DIM,resBoard);
	if (error) {
		printf("ERROR %d GRBgetdblattrarray(): %s\n", error,GRBgeterrormsg(env));
		return 1;
	}
	/*PUSH MOVE FOR MULTI-CELL MOVE*/
    pushGameMove(sGame);
	for (i = 0; i < DIM; i++) {
		for (j = 0; j < DIM; j++) {
			count=0;
			countValues=0;
			for(v=0;v<DIM;v++){
                if((resBoard[i * DIM * DIM + j * DIM + v]>x) && isLegalValue(sGame->currBoard,i,j,v)){
                    count++;
                    countValues += (int)(resBoard[i * DIM * DIM + j * DIM + v]*10);
                }
			}
			values = (int *)malloc(countValues*sizeof(int));
			if(values==NULL){
                printf("Error: memory allocation has failed\n");
                exit(0);
			}
			for(v=0;v<countValues;v++){
                values[v]=0;
			}
			for(v=0;v<DIM;v++){
                if(resBoard[i * DIM * DIM + j * DIM + v]>x && isLegalValue(sGame->currBoard,i,j,v))
                    for(k=0;k<(int)(resBoard[i * DIM * DIM + j * DIM + v]*10);k++)
                        values[k]=v;
			}
            if(count>0){
                int index;
                index = rand()%countValues;
                gameSetCell(sGame,i,j,values[index]+1,1);
            }
			free(values);
		}
	}
    return 0;
}

/**  LPSolverGuessHint:
* @brief Generates all guessed values for a cell in the board and prints it to the user
* @param sudokuGame *sGame - a pointer to the sudoku game being played
* @param int row/col - indicates for which cell a guess hint should be made (0-based)
* @return int - 1 if LP was successful , 0 o\w
*/
int LPSolverGuessHint(sudokuGame *sGame,int row,int col){
    int SUBDIMROW,SUBDIMCOL,DIM;
    GRBenv *env = NULL;
    GRBmodel *model = NULL;
    double *lb=NULL,*val=NULL,*resBoard=NULL,*obj=NULL,objval;
    char *vtype=NULL;
    int i,j,v,error=0,optimstatus, *ind = NULL;
    SUBDIMROW = sGame->currBoard->rowsInBlock , SUBDIMCOL = sGame->currBoard->colsInBlock;
    DIM = sGame->currBoard->length;

    /*allocate memory for all auxiliary arrays*/
    allocateMemoryGRB(DIM,&lb,&val,&resBoard,&vtype,&ind,&obj,'L');

    /* Create an empty model */
    for (i = 0; i < DIM; i++) {
        for (j = 0; j < DIM; j++) {
            for (v = 0; v < DIM; v++) {
                if (sGame->currBoard->board[i][j].value == v+1)
                    lb[i*DIM*DIM+j*DIM+v] = 1;
                else
                    lb[i*DIM*DIM+j*DIM+v] = 0;
                vtype[i*DIM*DIM+j*DIM+v] = GRB_CONTINUOUS;
            }
        }
    }

    /*Randomize coefficients for the objective value)*/
    randomizeCoefs(obj,DIM);

    /* Create environment */
    error = GRBloadenv(&env, "sudokuLP.log");
    if (error) goto QUIT;
    error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
    if (error) goto QUIT;


    /* Create new model */
    error = GRBnewmodel(env, &model, "sudoku", DIM*DIM*DIM, obj, lb, NULL,vtype, NULL);
    if (error) goto QUIT;

    /* Change objective sense to maximization */
    error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
    if (error) goto QUIT;

    /* update the model - to integrate new variables */
    error = GRBupdatemodel(model);
    if (error) goto QUIT;

    /*Add sudokuBoard constraints*/
    error = addConstraints(model,ind,val,DIM,SUBDIMROW,SUBDIMCOL,'L');
    if (error) goto QUIT;

    /* Optimize model */
    error = GRBoptimize(model);
    if (error) goto QUIT;

    /* Write model to 'sudoku.lp' */
    error = GRBwrite(model, "sudokuLP.lp");
    if (error) goto QUIT;

    /* Capture solution information */
    error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
    if (error) goto QUIT;
    error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
    if (error) goto QUIT;

    /*Getting solution*/
	error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0,DIM*DIM*DIM,resBoard);
	if (error) {
		printf("ERROR %d GRBgetdblattrarray(): %s\n", error,GRBgeterrormsg(env));
		return 1;
	}
	printf("Valid values options for cell <%d,%d> are\n",col+1,row+1);
	for(v=0;v<DIM;v++){
        if(resBoard[row*DIM*DIM + col*DIM + v]>0 && isLegalValue(sGame->currBoard,row,col,v)){
            printf("Chances for value %d is %.2f\n",v+1,resBoard[row*DIM*DIM + col*DIM + v]);
        }
	}

    QUIT: /* Error reporting */
    if (error){
        freeMemoryGRB(ind,vtype,resBoard,val,lb,obj,'L');
        GRBfreemodel(model);
        GRBfreeenv(env);
        return 0;
    }
    freeMemoryGRB(ind,vtype,resBoard,val,lb,obj,'L');
    GRBfreemodel(model);
    GRBfreeenv(env);
    return 1;

}



