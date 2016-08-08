#ifndef MEMORY_CONTEXT_H
#define MEMORY_CONTEXT_H

#include "../../sysconf/type.h"

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
 * struct of block
 */
typedef struct Block{
	Size block_size;                          //size of the block
	Size used;                                //size of used space
	struct Block* next_block;                 //next block for the same memoryContext
	void* space_ptr;                          //where is block space
	void* free_start;                         //where is free space in the block
}Block;

/*
 * struct of bigblock, larger than 8K
 */
typedef struct BigBlock{
	Size block_size;                          //size 
	struct BigBlock* next_block;              //next bigblock
	void* space_ptr;                          //space for the bigblock
}BigBlock;

/*
 * a node in list, stand for a chunk
 */
typedef struct Chunk{
	void* space;                              //a chunk for using
	Size size;                                //size of the chunk
	Size used;                                //size of used space
	struct Chunk* next;                       //a pointer pointing to next Chunk
	Block* block;                             //block the chunk belongs to
}Chunk;

/*
 * struct of listheader, it also manager memory used
 */
typedef struct ListHeader{
	Size chunk_size;                           //chunk size on list
	Chunk* used_list;                          //chunk list for used
	Chunk* free_list;                          //chunk list for free
}ListHeader;

/*
 * info of memoryContext
 */
typedef struct MemoryContext{
	ListHeader free_list[FREELISTS_NUM];
	Size initAlloca;                           //how many have been allocated when create context
	int8_t flags;                              //some flags
	char* name;                                //name of the context
	Block* blocks;                             //blocks belongs to this context
	BigBlock* big_blocks;                      //big blocks belongs to this context
}MemoryContext;

#endif

