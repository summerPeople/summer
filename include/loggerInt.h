#ifndef LOGGER_INT_H
#define LOGGER_INT_H

#include "../commodule/src/logger.h"

/*
 * functions to init logger
 */
void logInitMethods();

/*
 * the global logger
 */
extern Logger logger;

/*
 * switcher of debug mode
 */
#define CAN_DEBUG

/*
 * according to DEBUG to log or not
 */
#ifdef CAN_DEBUG
#define LOG(x, y) (logger.log(x, y))
#else
#define LOG(x, y) 
#endif

#endif
