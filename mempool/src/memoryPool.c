#include <stdio.h>
#include <stdlib.h>
#include "memoryPool.h"
#include "pagerInt.h"
#include "fileopInt.h"
#include "../../sysconf/type.h"
//#include "loggerInt.h"

//keep a link of usedMemPage,while using array to record the free MemPage in memorypool

/*
create BST tree if needed
*/
void insertBST(useNode* *p,useNode * newNode){
        if((*p)== NULL){ 
 		*p = newNode;
                (*p)->lchild = NULL;
                (*p)->rchild = NULL;
                 return ;
        }
        int diskNum = newNode->diskPage;
        if(diskNum<(*p)->diskPage)
                return insertBST(&((*p)->lchild),newNode);
        if(diskNum>(*p)->diskPage)
                return insertBST(&((*p)->rchild),newNode);
}

/*
find the diskPage node in O(log(n));
*/
useNode * preOrderTraverse(useNode * T,int n){ 
	useNode * tmp = NULL;
        if(T&&!flag){
		//printf("进入先序遍历查找BST,T = %d \n",T->diskPage);
		//LOG(DEBUG,“[内存池]|BST树查找磁盘页号，BST树先序遍历");
                if(T->diskPage == n){
					flag =  1; //find 
		//	printf(" T=%p\n",p+PAGESIZE*T->memPage);
		//	printf("  T->diskPage= %d\n",T->diskPage);
		//	printf("  T->memPage = %d \n",T->memPage);
					return  T;
			}
		else {
              		 tmp = preOrderTraverse(T->lchild,n);
		if(!flag)
              		 tmp = preOrderTraverse(T->rchild,n);
		}
		return tmp;
        }
}

/*
initial a new MemoryPool 
p means the point to the MemoryPool
useHead means the head of the used link
freeArray means the free memPage in MemoryPool
*/
void * allocMemPool(){
	//LOG(DEBUG,  "[内存池]，调用分配内存池函数");
	p=(void *) malloc(sizeof(char)*PAGESIZE*POOLSIZE);
	useHead=(uHead *)malloc (sizeof(uHead));
	useHead->next = NULL;
	T=NULL ;
	freeHead = (fHead *)malloc(sizeof(fHead));
	freeHead->next = NULL;
	freeHead->end = NULL;
	int i=0;
	for(i=0;i<POOLSIZE;i++){
		change_flag[i]=0;
		} 
	for(i=0;i<POOLSIZE;i++){
		freeNode * newFreeNode = (freeNode *)malloc (sizeof(freeNode));
		newFreeNode->freeMemPage = i;
		newFreeNode->next = NULL;
		if(freeHead->next==NULL){
	 		freeHead->next = newFreeNode;
			freeHead->end = newFreeNode;
		 	}
		else {
			freeHead->end->next = newFreeNode;
	 	 	freeHead->end = newFreeNode;
	 	}
	}
	return p ;	 
}

/*
alloc a MemPage in the MemoryPool
n means the nth diskPage needed to be stored in MemoryPool,页号
memp is a point to the context of the diskPage
*/
void * allocMemPoolPage(page_no n){
 	//LOG(DEBUG,[内存池],"进入allocPage");
	flag=0;
	if(p==NULL) {
		allocMemPool();
	}
	//useNode * curUse = useHead->next;
	//find whther the nth diakPage is already in the MemoryPool
	useNode *curUse = preOrderTraverse(T,n);
	//printf("curUse= %p\n",curUse);
	if(curUse!=NULL ){
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
			//record n in the MemoryPool
			//*(int *)(p+PAGESIZE*curUse->memPage)=n;
			}
		//LOG(DEBUG,"[内存池]|分配函数，内存中已存在此页“);
		return p+PAGESIZE*curUse->memPage;
		
	}		
	int i=0;
	//while the nth diskPage is not in memorypool and there exists one free page at least.
	freeNode * curFreeNode = freeHead->next;
	if(curFreeNode!=NULL){
		 	useNode * newUseNode = (useNode *)malloc(sizeof(useNode));
			newUseNode->memPage = curFreeNode->freeMemPage;
			freeHead->next = curFreeNode->next;
			newUseNode->diskPage = n;
			newUseNode->next = NULL;
			if(useHead->end!=NULL)
			{	
		 		newUseNode->last = useHead->end;
				useHead->end->next = newUseNode;
				//when the used linklist isn't NULL,build a BST tree
				insertBST(&T,newUseNode);
			}
			else{
				newUseNode->last = NULL;
			 	useHead->next = newUseNode;
				useHead->end = newUseNode;
	 	 		insertBST(&T,newUseNode);
			}
   			useHead->end = newUseNode;
				
			//*(int *)(p+PAGESIZE*curFreeNode->freeMemPage)=n;
			free(curFreeNode);
			//LOG(DEBUG,"[内存池]|分配函数，新分配一个内存池中的页”);
			summerPagerRead(n,p+PAGESIZE*curFreeNode->freeMemPage);
			return p+PAGESIZE*curFreeNode->freeMemPage;
	 	 	}
	//no free page left ,use LRU to substitute	
	else {
		u seHead->next->diskPage = n;
		int mem_num=useHead->next->memPage;

		if(change_flag[mem_num]!=0){
			pager.writePage(useHead->next->diskPage,p+PAGESIZE*mem_num);
			change_flag[mem_num]=1;
			}
		// *(int *)(p+PAGESIZE*useHead->next->memPage)=n;
		//LOG(DEBUG,"[内存池]|分配函数，替换内存池最久未用的页");
		summerPagerRead(n,p+PAGESIZE*useHead->next->memPage);
	 	return p+PAGESIZE*useHead->next->memPage;
	}   
//	return; 
}

/*
print the allocation of the MemeryPool
*/
void printMemPoolAlloc(){
	if(p == NULL){
		// printf("no MemoryPool !\n");
		//LOG(DEBUG,"[内存池]，打印内存分配");
 		return;
	}
	//LOG(DEBUG,"[内存池]，打印内存分配");
	useNode * curUseNode = useHead->next;
	int i=0;
	while(curUseNode !=NULL){
		printf("%d  memoryAddress: %p ,memoryPageNum %d , memoryDiskNUm %d;\n",i++,p+PAGESIZE*curUseNode->memPage,curUseNode->memPage,curUseNode->diskPage);
		curUseNode = curUseNode->next; 
	}
 	return;
}

/*
free the memory of the MemoryPool and the linklist of the used memPage
*/
void freeMemPool(){
	//LOG（DEBUG，”[内存池]|释放函数，释放整个内存池空间以及已用链表
	useNode * curNode = useHead->next;
	useNode * temp;
	while(curNode!=NULL)
		{ 
		if(change_flag[curNode->memPage]!=0){
			pager.writePage(curNode->diskPage,p+PAGESIZE*curNode->memPage);
			change_flag[curNode->memPage]=0;
		}	
		temp = curNode->next;
                free(curNode);
		curNode = temp;
	 	} 
	if(useHead!=NULL)
	 	free (useHead); 
	if(p!=NULL)
		free(p);
}

void setMemPoolFlag(void *mpp){
	int n = (mpp-p)/PAGESIZE;
	change_flag[n]=1;
}
