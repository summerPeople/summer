/*
the header file of the MemeryPool for interface
*/
#ifndef MEMERYPOOLINT_H
#define MEMERYPOOLINT_H

void *allocMemeryPool();

void allocPage(int n,void * memp);

void printMemeryAlloc();

void freeMemeryPool();

#endif
