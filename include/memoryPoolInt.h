/*
the header file of the MemeryPool for interface
*/
#ifndef MEMERYPOOLINT_H
#define MEMERYPOOLINT_H

void *allocMemoryPool();

void allocPage(int n,void * memp);

void printMemoryAlloc();

void freeMemoryPool();

#endif
