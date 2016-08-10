#include "../mempool/src/memoryContext.h"

extern MemoryContext* memoryContext;

/*
 * alloc memory
 */
void* summerAlloc(MemoryContext* context, Size size);

/*
 * free memory
 */
void summerFree(MemoryContext* context, void* pointer);
