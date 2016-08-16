#ifndef BTREE_INT_H
#define BTREE_INT_H

#include "../btree/src/btree.h"

void summerBtreeCreateBtree();                  //create btree
void summerBtreeDeleteBtree();					//delete btree

void summerBtreeCreateDbFile(char* file_name);  //create db file
int summerBtreeDeleteDbFile(char* file_name);   //delete a db file
int summerBtreeOpenDbFile(char* file_name);     //open a db file

page_no summerBtreeWriteFreeDataPage();         //write a free data page into file, get pageno
page_no summerBtreeWriteFreeSpecPage();         //write a free spec page into file, get pageno

int64_t summerBtreeGetSequenceId(char* table_name); //get the sequence id of a table, the newer id must plus 1

page_no summerBtreeInsertTuple(char* table_name, tuple* tuple_ptr);		//insert tuple into table
int summerBtreeDeleteTuple(char* table_name, void* where_clause);       //delete a tuple which accord where_clause
tuple* summerBtreeSelectTuple(char* table_name, void* where_clause);    //select a tuple which accord where_clause
int summerBtreeUpdateTuple(char*table_name);                            //update a tuple

#endif

