#include<stdbool.h>
#include"../executer/src/executer.h"
/*
	analyse the sql command
	the first parameter is the sql command
	the second parameter brings out the needed excute info
	the third parameter brings out the needed excute parameters
*/
void summerSqlAnalyse(void *sql, void **_out_type, void **_out_parameter);
/*
	execute the sql command 
	the first parameter is the execute info
	the second poarameter is the execute parameters
*/
void summerSqlExecute(Execute_info *einfo, void *parameters);

/*
	free the sql parameter space from compiler
*/
void summerSqlFree(void *sql);

