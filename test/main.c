//test
#include"applicationInt.h"
#include"configLoaderInt.h"
#include"loggerInt.h"
#include"memoryPoolInt.h"
#include"pagerInt.h"
#include"btreeInt.h"
#include"executerInt.h"
#include"memoryContextInt.h"
#include"pageInt.h"
#include<assert.h>
#include<stdio.h>

int main()
{

	appInit();
	LOG(INFO, "summer test start!");
	void *sql = NULL;
	sql = summerCompile();
	assert(sql);
	LOG(INFO, "compile complete!");
	//void *parameter;
	void *parameter;
	unsigned char *type;
	summerSqlAnalyse(sql, &type, &parameter);
	LOG(INFO, "SQL Analyses complete!");
	summerSqlExecute(*type, parameter);
	appExit();
	printf("over!\n");

	return 0;
}
