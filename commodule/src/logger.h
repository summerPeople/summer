#ifndef LOGGER_H
#define LOGGER_H

#include<stdio.h>

/*
 * log level
 */
typedef enum LogLevel{
	DEBUG,                             //only for debugging
	INFO,                              //there is something to say out
	WARN,                              //may be it is a error
	ERROR,                             //it is a error, but application work hardly
	FATAL                              //application exit
}LogLevel;

/*
 * Logger class
 */
typedef struct Logger{
	char* name;                       //the name of logger
	char* log_file_path;              //the log file path
	FILE* fp;                         //file pointer of the log file

	void (*setFile)(char* file_path);
	void (*log)(LogLevel level, char* log_info);
	int (*init)(char* name, char* file_path);
	void (*close)();
}Logger;

#endif
