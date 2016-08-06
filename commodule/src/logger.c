#include "logger.h"

#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>


/*
 * logger for others to use
 */
Logger logger;


/*
 * init logger
 */
int logInit(char* name, char* file_path){
	logger.name = name;
	logger.log_file_path = file_path;
	if((logger.fp = fopen(file_path, "a+")) != NULL){
		return 0;
	}
	return -1;
}

/*
 * set file path 
 */
void logSetFile(char* file_path){
	logger.log_file_path = file_path;
}

/*
 * log method ,write something to log file
 */
void logLog(LogLevel level, char* log_info){
	char* level_str;
	char* time_str = (char*)calloc(1, 24*sizeof(char));
	char* write_str = (char*)calloc(1,(strlen(log_info) + 34 + 1)*sizeof(char));

	//get time str
	time_t timep;
	struct tm* p;
	time(&timep);
	//p = gmtime(&timep);
	p = localtime(&timep);
	sprintf(time_str, "%d-%d-%d %d:%d:%d", (1900+p->tm_year), (1+p->tm_mon), p->tm_mday,
			p->tm_hour, p->tm_min, p->tm_sec);

	switch(level){
		case DEBUG:
			level_str = "[DEBUG]";
			break;
		case INFO:
			level_str = "[INFO]";
			break;
		case WARN:
			level_str = "[WARN]";
			break;
		case ERROR:
			level_str = "[ERROR]";
			break;
		case FATAL:
			level_str = "[FATAL]";
			break;
		default:
			break;
	}
	
	sprintf(write_str, "%s %s %s\n", time_str, level_str, log_info);
	fwrite(write_str, strlen(write_str), 1, logger.fp);

	free(time_str);
	free(write_str);
}

/*
 * close the file pointer
 */
void logClose(){
	if(logger.fp != NULL){
		fclose(logger.fp);
	}
}

/*
 * init methods field of logger
 */
void logInitMethods(){
	logger.setFile = logSetFile;
	logger.log = logLog;
	logger.init = logInit;
	logger.close = logClose;
}


