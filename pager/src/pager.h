#ifndef PAGER_H
#define PAGER_H
/*
 * pager class can handle all ops of page
 * btree use it to do anything about page
 */

typedef struct Pager{
	void* (*getPage)(page_no pageno);                       //get a page and return it , not memPage
	MemPage* (*getMemPage)(void* page);                     //get a mempage and return it by using a page in mem 
	page_no (*writePage)(page_no, void* page);              //write page into db file, if page is new, page_no == -1

	FILE* (*openDbFile)(char* file_name);                   //open db file
	FILE* (*createDbFile)(char* file_name);                 //create a new db file

	file_head* (*getDbFileHead)(void* root_page);           //get file header of the db file

	void (*getNewPage0)(void* space_ptr);                   //init space_ptr into a new page0
	void (*getNewSpecPage)(void* space_ptr);                //init space_ptr into a new special page
	void (*getDataPage)(void* space_ptr);                   //init space_ptr into a new data page
}Pager;

Pager* summerCreatePager();
#endif
