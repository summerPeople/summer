/*
the header file of the MemeryPool for interface
*/
#ifndef MEMERYPOOLINT_H
#define MEMERYPOOLINT_H
#include "../sysconf/type.h"

void *allocMemPool();  //allocate a memory pool

void *allocMemPoolPage(page_no n);  //allocate memory page for n(n is a disk page number)

void printMemPoolAlloc(); //print memorypool allocation

void freeMemPool(); //free all space

void setMemPoolFlag(void *mpp); //set a flag indicating whether it has been changed for every mem page in mem pool

#endif
