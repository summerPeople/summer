#include "memoryContext.h"

#include <malloc.h>
#include <assert.h>
#include <math.h>

MemoryContext* memoryContext;

/*
 * find request size in which freelist
 */
int findFreeListIndex(Size size){
	int16_t cutter[FREELISTS_NUM] = {
		8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192
	};
	for(int i = 0; i < FREELISTS_NUM; i++){
		if(size <= cutter[i]){
			return i;
		}
	}
	return -1;
}

/*
 * function to create the context,flags has not been used now
 */
void summerCreateMemoryContext(char* name, Size size, int8_t flags){
	MemoryContext* context = (MemoryContext*)malloc(sizeof(MemoryContext));
	assert(context != NULL);
	
	//init some variables
	context->initAlloca = size;
	context->name = name;
	context->flags = flags;

	//init blocks field
	Block* init_block = (Block*)malloc(sizeof(Block));
	assert(init_block != NULL);
	init_block->block_size = size;
	init_block->used = 0;
	init_block->next_block = NULL;
	void* space = malloc(size);
	assert(space != NULL);
	init_block->space_ptr = space;
	init_block->free_start = space;
	context->blocks = init_block;

	//init freelist header
	for(int i=0; i < FREELISTS_NUM; i++){
		(context->free_list)[i].chunk_size = 2 << (i+3);
		(context->free_list)[i].used_list = NULL;
		(context->free_list)[i].free_list = NULL;
	}

	memoryContext = context;	
}

/*
 * function to destroy the context
 */
int deleteMemoryContext(MemoryContext* context){
	for(int i = 0; i < FREELISTS_NUM; i++){
		ListHeader list_header = (context->free_list)[i];
		if(list_header.used_list != NULL){
			Chunk* tmp_chunk = list_header.used_list;
			while(tmp_chunk->next != NULL){
				tmp_chunk = tmp_chunk->next;
			}
			tmp_chunk->next = list_header.free_list;
		}
		else{
			list_header.used_list = list_header.free_list;
		}
		//release used list
		Chunk* chunk_free = list_header.used_list;
		while(list_header.used_list != NULL){
			chunk_free = list_header.used_list;
			list_header.used_list = (list_header.used_list)->next;
			free(chunk_free);
		}
	}
	Block* block = context->blocks;
	Block* block_free = NULL;
	while(block){
		block_free = block;
		block = block->next_block;
		free(block_free);
	}
	free(context);
}

/*
 * function to reset the context
 */
void resetMemoryContext(MemoryContext* context){
	//free chunk struct
	for(int i = 0; i < FREELISTS_NUM; i++){
		ListHeader list_header = (context->free_list)[i];
		if(list_header.used_list != NULL){
			Chunk* tmp_chunk = list_header.used_list;
			while(tmp_chunk->next != NULL){
				tmp_chunk = tmp_chunk->next;
			}
			tmp_chunk->next = list_header.free_list;
		}
		else{
			list_header.used_list = list_header.free_list;
		}
		//release used list
		Chunk* chunk_free = list_header.used_list;
		while(list_header.used_list != NULL){
			chunk_free = list_header.used_list;
			list_header.used_list = (list_header.used_list)->next;
			free(chunk_free);
		}
	}
	//remain only one block
	Block* blocks_free = context->blocks->next_block;
	Block* block_free = NULL;
	while(blocks_free){
		block_free = blocks_free;
		blocks_free = blocks_free->next_block;
		free(block_free);
	}

	//free big blocks
	BigBlock* big_blocks_free = context->big_blocks;
	BigBlock* big_block_free = NULL;
	while(big_blocks_free){
		big_block_free = big_blocks_free;
		big_blocks_free = big_blocks_free->next_block;
		free(big_block_free);
	}

	//reset context
	context->blocks->used = 0;
	context->blocks->next_block = NULL;
	context->blocks->free_start = context->blocks->space_ptr;
	//init freelist header
	for(int i=0; i < FREELISTS_NUM; i++){
		(context->free_list)[i].used_list = NULL;
		(context->free_list)[i].free_list = NULL;
	}
}

/*
 * alloc memory
 */
void* summerAlloc(MemoryContext* context, Size size){
	int index = findFreeListIndex(size);
	if(index != -1){        //request space doesn't larger than 8K 
		//first, find free space in free list
		ListHeader listheader = (context->free_list)[index];
		if(listheader.free_list != NULL){   //there is free space in the list
			Chunk* free_chunk = listheader.free_list;
			listheader.free_list = free_chunk->next;
			free_chunk->next = listheader.used_list;
			listheader.used_list = free_chunk;

			//return the free chunk
			free_chunk->used = size;
			return free_chunk->space;
		}
		else{                              //there is no free space in the list
			Size malloc_size = 2 << (index + 3);
			Block* free_block = context->blocks;
			while(free_block != NULL){
				Size free = free_block->block_size - free_block->used;
				if(free >= malloc_size){
					break;
				}else{
					free_block = free_block->next_block;
				}
			}
			//there is no block has malloc_size free space
			//so we alloc a block
			if(free_block == NULL){
				Block* new_block = (Block*)malloc(sizeof(Block));
				assert(new_block != NULL);
				new_block->block_size = context->initAlloca;
				new_block->used = 0;
				new_block->next_block = NULL;
				Block* tail = context->blocks;
				while(tail->next_block){
					tail = tail->next_block;
				}
				tail->next_block = new_block;
				void* space = malloc(new_block->block_size);
				assert(space != NULL);
				new_block->free_start = space;
				new_block->space_ptr = space;
				free_block = new_block;
			}
			//we alloc chunk on the block
			Chunk* new_chunk = (Chunk*)malloc(sizeof(Chunk));
			new_chunk->size = malloc_size;
			new_chunk->used = size;
			new_chunk->block = free_block;
			new_chunk->space = free_block->free_start;
			new_chunk->next = listheader.used_list;
			listheader.used_list = new_chunk;
			//change the block
			free_block->free_start = free_block->free_start + malloc_size;
			return new_chunk->space;
		}
	}
	else{          //we should alloc a big block for it
		BigBlock* big_block = (BigBlock*)malloc(sizeof(Block));
		big_block->block_size = size;
		big_block->next_block = context->big_blocks;
		context->big_blocks = big_block;
		void* big_space = malloc(size);
		big_block->space_ptr = big_space;
		return big_space;
	}
}

/*
 * free memory
 */
void summerFree(MemoryContext*context, void* pointer){
	//find it in used_list or big block list
	for(int i = 0; i < FREELISTS_NUM; i++){
		ListHeader list_header = (context->free_list)[i];
		Chunk* pre = NULL;                      //pre node of used node
		Chunk* used = list_header.used_list;
		while(used != NULL){
			if(used->space == pointer){
				break;
			}else{
				pre = used;
				used = used->next;
			}
		}
		//find the chunk
		if(used != NULL){
			//put the chunk into free_list;
			if(pre == NULL){            //used is the first node of the list
				list_header.used_list = used->next;
			}
			else{
				pre->next = used->next;
			}
			//free the block
			Block* block = used->block;
			if(block->used == used->size){    //it is the last used chunk of the block
				if(block == context->blocks){ //can not free the block,it is the first block
					used->used = 0;
					used->next = list_header.free_list;
					list_header.free_list = used;
				}
				else{                         //free the block and chunk on it
					//free chunks on the block
					free(used);
					//free chunks in free lists and on the block
					for(int j = 0; j < FREELISTS_NUM; j++){
						ListHeader header = (context->free_list)[j];
						Chunk* free_list_chunks = header.free_list;
						Chunk* should_free = NULL;
						while(free_list_chunks != NULL){
							if(free_list_chunks->block == block){
								should_free = free_list_chunks;
								free_list_chunks = free_list_chunks->next;
								free(should_free);
							}
							else{
								free_list_chunks = free_list_chunks->next;
							}
						}
					}
					//free the block
					Block* pre_block = context->blocks;
					Block* tmp_block = context->blocks->next_block;
					while(tmp_block != NULL){
						if(tmp_block == block){
							pre_block->next_block = tmp_block->next_block;
						}
						else{
							pre_block = tmp_block;
							tmp_block = tmp_block->next_block;
						}
					}
					free(block->space_ptr);
					free(block);
				}
			}
			else{                             //only put it into free list
				used->used = 0;
				used->next = list_header.free_list;
				list_header.free_list = used;
			}
			return;
		}
	}
	//find it in the big blocks
	BigBlock* pre_big_block = NULL;
	BigBlock* free_big_block = context->big_blocks;
	while(free_big_block != NULL){
		if(free_big_block->space_ptr == pointer){
			if(pre_big_block == NULL){
				context->big_blocks = free_big_block->next_block;
			}
			else{
				pre_big_block->next_block = free_big_block->next_block;
			}
			free(free_big_block->space_ptr);
			return;
		}
		else{
			pre_big_block = free_big_block;
			free_big_block = free_big_block->next_block;
		}
	}
}

/*
 * realloc memory
 * pointer stands for momory to be realloc
 */
void* summerRealloc(void* pointer, Size size){

}

