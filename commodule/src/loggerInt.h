#ifndef LOGGER_INT_H
#define LOGGER_INT_H

/*
 * the global logger
 */
extern Logger logger;

/*
 * switcher of debug mode
 */
#define DEBUG

/*
 * according to DEBUG to log or not
 */
#ifdef DEBUG
#define LOG(x, y) (logger.log((x), (y)))
#else
#define LOG(x, y) 
#endif

#endif
