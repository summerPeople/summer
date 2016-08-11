#ifndef PAGE_INT_H
#define PAGE_INT_H

#include "../pager/src/page.h"

void insertTuple(void* page_ptr, tuple* tuple_ptr);
void deleteTuple(void* page_ptr, int16_t offset);
void updateTuple();
void getTuple(void* page_ptr, int16_t offset, tuple* temp);

void initFileHead(void* page_ptr);
void getFileHead(void* page_ptr, File_head* ptr);

void initSpecTablePageHead(void* page_ptr);

void getDataPageHead(void* page_ptr, Data_page_head* ptr);

#endif
