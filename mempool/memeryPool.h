/*
the header file of memeryPool.c
*/
#ifndef MEMERY_POOL_H
#define MEMERY_POOL_H

#define PAGESIZE 4096
#define POOLSIZE 20

typedef struct useLink{
	int memPage;
	int diskPage;
	struct	useLink * last;
	struct	useLink * next;
}useNode;

typedef struct useH{
	useNode * next;
	useNode * end;
}Head;

Head *useHead;
void *p ;
int freeArray[POOLSIZE+2];

#endif
