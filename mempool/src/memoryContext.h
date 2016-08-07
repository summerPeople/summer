#ifndef MEMORY_CONTEXT_H
#define MEMORY_CONTEXT_H


/*
 * algorithm
 * threr are FREELISTS_NUM types chunk in a block or MEMORYCONTEXT
 * each has a ListHeader, each header manager free and used list of
 * special size. At first, each list has no elements, when a request
 * arrives, we alloc space for it and append it to a used list.when 
 * free it, we append it to corresponding free list. so number of free
 * list and used list for a special size will not decrease.but each type 
 * list may not has the same length, it depends on request. when a 
 * block is limit, we alloc another block
 */

#define FREELISTS_NUM  11                      //number of freelist

/*
 * a node in list, stand for a chunk
 */
typedef struct MemoryNode{
	void* space;                              //a chunk for using
	MemoryNode* next;                         //a pointer pointing to next node
}MemoryNode;

/*
 * struct of listheader, it also manager memory used
 */
typedef struct ListHeader{
	int32_t chunk_size;                        //chunk size on list
	MemoryNode* used_list;                     //chunk list for used
	MemoryNode* free_list;                     //chunk list for free
}ListHeader;

/*
 * info of memoryContext
 */
typedef struct MemoryContext{
	FreeList free_list[FREELISTS_NUM];
	struct ListHead list;                      //list of all known pools
	int32_t used;                              //how many chunks are curently in use
	int32_t allocated;                         //how many chunks have been allocated
	int32_t limit;                             //hard limit on the number of chunks
	int32_t minavail;                          //how many chunks are expected to be used
	int32_t size;                              //chunk size
	int32_t flags;                             //MEM_F_
	int32_t users;                             //number of pools sharing this zone
	int8_t name[12];                           //name of the pool
}MemoryContext

/*
 * create MemoryContext 

#endif

