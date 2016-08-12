#include "../mempool/src/memoryContext.h"

extern MemoryContext* memoryContext;

/*
 * create memoryContext
 */
void summerCreateMemoryContext(char* name, Size size, int8_t flags);

/*
 * alloc memory
 */
void* summerAlloc(MemoryContext* context, Size size);

/*
 * free memory
 */
void summerFree(MemoryContext* context, void* pointer);
