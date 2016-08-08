/*
the header file of memoryPool.c
*/
#ifndef MEMERY_POOL_H
#define MEMERY_POOL_H
//#include "config.h"

#define PAGESIZE 4096
#define POOLSIZE 20

typedef struct useLink{
	int memPage;
	int diskPage;
	struct useLink * last;
	struct useLink * next;
	struct useLink * lchild;
	struct useLink * rchild;
}useNode;

typedef struct {
	useNode * next;
	useNode * end;
}uHead;

//use freeArray to keep the link of freeMemPage,this struct hasn't     been used
 typedef struct freeLink{
	int freeMemPage;
	struct freeLink *next;

}freeNode;

typedef struct {
	freeNode * next;
	freeNode * end;	
}fHead;

uHead *useHead;
useNode * T; //BST root
fHead * freeHead;
void *p ;
//int freeArray[POOLSIZE+2];
int flag;
#endif
