#include "configLoaderInt.h"
#include "loggerInt.h"
#include "memoryContextInt.h"
#include "pagerInt.h"

/*
 * function to init the application
 */
int appInit(){
	//read configure info
	if(summerComModuleLoadConfig() != 0){
		return -1;
	}

	//init logger
	logInitMethods();
	logger.init("global", "../log/log.log");

	//init MemoryContext
	summerCreateMemoryContext("main", 8*1024, 0);

	//init pager
	summerPagerCreatePager();

	return 0;
}

/*
 * function to exit the application
 * recover resource
 */
int appExit(){
	//shutdown the log file
	logger.close();
}
