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
	while(1){
		sql = (void *)summerCompile();
		assert(sql);
		LOG(INFO, "compile complete!");
		//void *parameter;
		void *parameter;
		void *execute_info;
		summerSqlAnalyse(sql, &execute_info, &parameter);
		LOG(INFO, "SQL Analyses complete!");
		summerSqlExecute(execute_info, parameter);
		printf("over!\n");
	}
	appExit();
	return 0;
}
