#ifndef PAGER_H
#define PAGER_H
/*
 * pager class can handle all ops of page
 * btree use it to do anything about page
 */
#include "../../sysconf/type.h"
#include "pageInt.h"

#include <stdio.h>

typedef struct Pager{
	void* (*getPage)(page_no pageno);                       //get a page and return it , not memPage
	void* (*getMemPage)(void* page);                        //get a mempage and return it , you should cast it to a mem page 
	page_no (*writePage)(page_no, void* page);              //write page into db file, if page is new, page_no == -1

	void (*openDbFile)(char* file_name);                   //open db file
	void (*createDbFile)(char* file_name);                 //create a new db file

	void (*getDbFileHead)(void* root_page, File_head* ptr); //get file header of the db file
	void (*getPageHead)(void* page_ptr, Data_page_head* ptr);//get page header of db file page

	void (*getNewPage0)(void* space_ptr);                   //init space_ptr into a new page0
	void (*getNewSpecPage)(void* space_ptr);                //init space_ptr into a new special page
	void (*getDataPage)(void* space_ptr);                   //init space_ptr into a new data page

	void (*insertTuple)(void* page_ptr, tuple* tuple_ptr);  //insert tuple into page
	void (*deleteTuple)(void* page_ptr, int16_t offset);    //delete tuple in the page_ptr
	void (*getTuple)(void* page_ptr, int16_t offset, tuple* temp);  //get a tuple in the page and store it in temp
}Pager;

#endif
