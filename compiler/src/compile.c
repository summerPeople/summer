#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compile.h"

#include "../../sysconf/SQLtype.h"

void * summerCompile(){ //summerCompile
	int tok1=yylex();
	char *sql;
	switch (tok1) {
		case CREATE :{
				int tok2=yylex();
				/*if(tok2==TABLE){
						int tok3=yylex();
						if()
					}*/
				if(tok2==DATABASE){
						int tok3=yylex();
						if(tok3==NAME){
							sql =(char *)malloc(sizeof(char)*(STMTYPE+DTNAME+1));	
							sql[0] = CDB;
							printf("%s\n",yyname);
							//char *dbname = (char *)malloc(64 * sizeof(char));
							//*(char *)(sql+1) = yyname;
							//strcpy(dbname, yyname);
							*(int *)(sql + 1) = yyname;
						//	strcpy((char *)(sql+1),yyname);
							printf("%d,%s\n",sql[0],(char *)(*(int *)(sql + 1)));
							return sql;
							}
						//else {printf("表名错误\n");}
					}
				break ;
             }
		default : {printf("不支持这种语句");}
	}
}

