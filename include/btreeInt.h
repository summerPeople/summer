#ifndef BTREE_INT_H
#define BTREE_INT_H

#include "../btree/src/btree.h"

void summerBtreeCreateBtree();                  //create btree
void summerBtreeDelete();                       //delete btree

void summerBtreeCreateDbFile(char* file_name);  //create db file

page_no summerBtreeWriteFreeDataPage();         //write a free data page into file, get pageno

page_no summerBtreeInsert(char* table_name, tuple* tuple_ptr); //insert tuple into table

void test(char* str);

#endif

