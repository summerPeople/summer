#include <stdio.h>
#include <stdlib.h>
#include "memeryPool.h"

//keep a link of usedMemPage,while using array to record the free MemPage in memerypool

/*
initial a new MemeryPool 
p means the point to the MemeryPool
useHead means the head of the used link
freeArray means the free memPage in MemeryPool
*/
void * allocMemeryPool(){
	p=(void *) malloc(sizeof(char)*PAGESIZE*POOLSIZE);
	useHead=(Head *)malloc (sizeof(Head));
	useHead->next = NULL;
	int i=0;
	freeArray[0]=POOLSIZE;
	for(i=1;i<=POOLSIZE;i++){
		freeArray[i]=i;
	}
	return p;	
}

/*
alloc a MemPage in the MemeryPool
n means the nth diskPage needed to be stored in MemeryPool
memp is a point to the context of the diskPage
*/
void allocPage(int n,void *memp){
	if(p==NULL) {
		allocMemeryPool();
	}
	useNode * curUse = useHead->next;
	//find whther the nth diakPage is already in the MemeryPool
	while(curUse !=NULL){
		if(curUse->diskPage == n){
			if(curUse->next !=NULL){
				if(curUse->last!=NULL){
					curUse->next->last = curUse->last;
					curUse->last->next = curUse->next;
					curUse->last = useHead->end;
					useHead->end->next = curUse;
					curUse->next =NULL;
					useHead->end = curUse;

				}
				else {
					useHead->next = curUse->next;
					useHead->end->next = curUse;
					curUse->next->last =NULL;
					curUse->last= useHead->end;
					curUse->next=NULL;
					useHead->end = curUse;
				}
				//record n in the MemeryPool
				*(int *)(p+PAGESIZE*curUse->memPage)=n;
			}			
			return;
		}
	 	else {
			curUse=curUse->next;
		}	
	}
	int i=0;
	//while the nth diskPage is not in memerypool and there exists one free page at least.
	if(freeArray[0]!=0){
		for (i=1;i<=POOLSIZE;i++){
			if(freeArray[i]!=-1){
				useNode * newUseNode = (useNode *)malloc(sizeof(useNode));
				newUseNode->memPage = freeArray [i];
				newUseNode->diskPage = n;
				newUseNode->next = NULL;
				if(useHead->end!=NULL)
				{	newUseNode->last = useHead->end;
					useHead->end->next = newUseNode;
				}
				else{
					newUseNode->last = NULL;
					useHead->next = newUseNode;
				}
   				useHead->end= newUseNode;
				*(int *)(p+PAGESIZE*freeArray[i])=n;
				freeArray[i]=-1;
				freeArray[0]--;
				break;
			}
		}
	}
	//no free page left ,use LRU to substitute	
	else {
		 useHead->next->diskPage = n;
		 *(int *)(p+PAGESIZE*useHead->next->memPage)=n;
	} 
	return;
}

/*
print the allocation of the MemeryPool
*/
void printMemeryAlloc(){
	if(p==NULL){
		printf("no MemeryPool !\n");
		return;
	}
	useNode * curUseNode = useHead->next;
	int i=0;
	while(curUseNode !=NULL){
		printf("%d  memeryaddress: %p ,memeryPageNum %d , memeryDiskNUm %d;\n",i++,p+PAGESIZE*curUseNode->memPage,curUseNode->memPage,curUseNode->diskPage);
		curUseNode = curUseNode->next;
	}
	return;
}

/*
free the memory of the MemeryPool and the linklist of the used memPage
*/
void freeMemeryPool(){
	if(p!=NULL)
		free(p);
	useNode * curNode = useHead->next;
	useNode * temp;
	while(curNode!=NULL)
		{
		  temp = curNode->next;
                  free(curNode);
		  curNode = temp;
		}
	if(useHead!=NULL)
		free(useHead);
}
