/*
	 执行器调用btree的相关方法 
*/
#ifndef EXECUTER_H
#define EXECUTER_H

#include"../../sysconf/type.h"

typedef struct {
	void* (*btreeInsertTuple)(char *, tuple *new_tuple);
	int32_t (*btreeGetTableRoot)(void);
	int32_t (*btreeGetTableID)();
}BtreeMethods;

#endif
