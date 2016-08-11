#include "btree.h"
#include "memoryPoolInt.h"

#include<stdlib.h>
#include<assert.h>
#include<string.h>

/*
 * it is a btree created when application is starting
 */
Btree* global_btree = NULL;

/*
 * create a btree, but only one page, it is page0
 */
void summerBtreeCreateBtree(){
	Btree* btree = (Btree*)malloc(sizeof(Btree));
	assert(btree != NULL);

	//init memory pool
	allocMemPool();

	//create BtShared struct
	BtShared* btShared = (BtShared*)malloc(sizeof(BtShared));
	assert(btShared);
	btShared->pager = &pager;
	void* root_page = pager.getPage(0);            //get root page of the db file
	File_head* file_header = (File_head*)malloc(sizeof(File_head));	
	pager.getDbFileHead(root_page, file_header);
	btShared->file_header = file_header;

	//create root MemPage
	MemPage* page0 = pager.getMemPage(root_page);

	//create btree cursor
	BtCursor* cursor = (BtCursor*)malloc(sizeof(BtCursor));
	assert(cursor);
	cursor->cell_index = 0;
	(cursor->trace)[0] = page0;
	for(int i = 1; i < BTREE_MAX_DEEP; i++){
		(cursor->trace)[i] = NULL;
	}

	btree->btShared = btShared;
	btree->rootPage = page0;
	btree->btCursor = cursor;
	
	global_btree = btree;
}

/*
 * delete btree
 */
void summerBtreeDelete(){
	free(global_btree->btShared->file_header);
	for(int i = 0; i <= global_btree->btCursor->cell_index; i++){
		free((global_btree->btCursor->trace)[i]);
	}
	free(global_btree->btCursor);
	//free mem pool
	freeMemPool();
}


/*
 * create db file
 * insert page0 into file, and insert 4 special pages into file, and the 4 is the same
 */
void summerBtreeCreateDbFile(char* file_name){
	//get a file name with postfix
	char* db_file_name = (char*)calloc(1, 5*sizeof(char) + strlen(file_name));
	char* postfix_str = ".hot";
	strcpy(db_file_name, file_name);
	strcat(db_file_name, postfix_str);

	int page_size = config_info.page_size;
	//create db file
	pager.createDbFile(db_file_name);
	//write page0
	void* page = summerAlloc(memoryContext, page_size);
	pager.getNewPage0(page);
	pager.writePage(-1, page);
	//write special pages
	pager.getNewSpecPage(page);
	for(int i = 0; i < 4; i++){
		pager.writePage(-1, page);	
	}
	//free memory
	summerFree(memoryContext, page);
	free(db_file_name);
}

/*
 * write a new free data page into db file
 */
page_no summerBtreeWriteFreeDataPage(){
	int page_size = config_info.page_size;
	void* page = summerAlloc(memoryContext, page_size);
	pager.getDataPage(page);
	page_no pageno = pager.writePage(-1, page);
	summerFree(memoryContext, page);
	return pageno;
}

/*
 * write a new special page into db file
 */
page_no summerBtreeWriteFreeSpecPage(){
	int page_size = config_info.page_size;
	void* page = summerAlloc(memoryContext, page_size);
	pager.getNewSpecPage(page);
	page_no pageno = pager.writePage(-1, page);
	summerFree(memoryContext, page);
	return pageno;
}

/*
 * insert tuple into data table
 */
static
page_no btreeInsertDataTuple(char* table_name, tuple* tuple_ptr){
	//init Cursor to rootpage
	BtCursor* cursor = global_btree->btCursor;	
	cursor->cell_index = 0;
	MemPage* master_page = (MemPage*)pager.getMemPage(pager.getPage(1));
	cursor->cell_index++;
	(cursor->trace)[cursor->cell_index] = master_page;
	//in master table we find the first page of data table

}

/*
 * insert tuple into special table
 */
page_no summerBtreeInsert(char* table_name, tuple* tuple_ptr){
	//find the second page
	MemPage* special_page = NULL;
	if(strcmp("_master", table_name) == 0){
		special_page = (MemPage*)pager.getMemPage(pager.getPage(1));
	}
	else if(strcmp("_scheme", table_name) == 0){
		special_page = (MemPage*)pager.getMemPage(pager.getPage(2));
	}
	else if(strcmp("_lock", table_name) == 0){
		special_page = (MemPage*)pager.getMemPage(pager.getPage(3));
	}
	else if(strcmp("_sequence", table_name) == 0){
		special_page = (MemPage*)pager.getMemPage(pager.getPage(4));
	}
	else{
		btreeInsertDataTuple(table_name, tuple_ptr);
	}
	//we insert into special table
	if(special_page != NULL){
		MemPage* insert_mem_page = special_page;
		page_no next_page = insert_mem_page->header->pageno;
		//find the page we can insert
		while(insert_mem_page->header->fs_size < 2 + tuple_ptr->size){
			if(next_page == -1){
				break;
			}else{
				insert_mem_page = pager.getMemPage(pager.getPage(next_page));
				next_page = insert_mem_page->header->pageno;
			}
		}

		if(insert_mem_page->header->fs_size >= 2 + tuple_ptr->size){       //we find the insert page
			pager.insertTuple(insert_mem_page->page_space, tuple_ptr);
			return insert_mem_page->pageno;
		}
		else{
			page_no new_pageno =  summerBtreeWriteFreeSpecPage();
			return -1;
		}
	}
	else{
		return -1;
	}
}




