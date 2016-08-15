/*
	operations related to a page in memory 
*/
#ifndef PAGE_INT_H
#define PAGE_INT_H

#include "../pager/src/page.h"
#include "../sysconf/type.h"
#include <stdint.h>

/*
	insert a tuple into a page in memory
	the first parameter is the base addr of the page
	the second parameter is a pointer pointed to a tuple
*/
void insertTuple(void *page_ptr, tuple *tuple_ptr);

/*
   delete a tuple from a page in memory
   the first parameter is the base addr of the page
   the second parameter is the offset of the tuple to be deleted
*/
void deleteTuple(void *page_ptr, int16_t offset);

/*
	update a tuple in a page in memory
*/
void updateTuple();

/*
	get a tuple from a page in memory
	the first parameter is the base addr of the page
	the second parameter is the offset of a tuple
	the third parameter brings the tuple out
*/
void getTuple(void *page_ptr, int16_t offset, tuple *_out_tuple);

/*
   when create a database, this routin will init the file head Info
*/
void initFileHead(void *page_ptr);

/*
	get the dbfile head info 
	the first parameter is the base addr of the page with dbfile head info
	the second parameter brings the head info out
*/
void getFileHead(void *page_ptr, File_head *_out_head);

/*
	init the head of data page
	the first parameter is the base addr of the datapage
	the second parameter is the type of the pae
	type:SPECIAL|INNER|LEAF
*/
void initDataPageHead(void *page_ptr, char type);

/*
	init special table page head
	the parameter is the base addr of the page
*/
void initSpecTablePageHead(void *page_ptr);

/*
   Get the data page head info
   the first parameter is the base addr of the page
   the second parameter brings the head info out
*/
void getDataPageHead(void *page_ptr, Data_page_head *head);

/*
	get a MemPage structure
	the parameter is the base addr of the page to be transformed into MemPage
*/
void *getMemPage(void *page_ptr);

/*
	this routin judge if the tuple accorded with where clause
	if accoeded, return 0, else return -1
	the first parameter is a tuple ptr
	the second parameter is the where_clause
*/
int isTupleAccordWhere(tuple *tuple_ptr, void *wh_ptr);

/*
    assemble the scheme Info structure according to a record of _scheme table
	the parameter is a tuple of _scheme table
	return a Scheme_info pointer
*/
Scheme_info *assembleSchemeInfo(tuple *tuple_ptr);

void parseTuple(tuple *tuple_ptr);
#endif
