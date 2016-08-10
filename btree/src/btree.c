#include "btree.h"

#include<stdlib.h>
#include<assert.h>
#include<string.h>

/*
 * it is a btree created when application is starting
 */
Btree* btree = NULL;

/*
 * create a btree, but only one page, it is page0
 */
Btree* summerBtreeCreateBtree(){
	Btree* btree = (Btree*)malloc(sizeof(Btree));
	assert(btree != NULL);

	//init memory pool

	//create BtShared struct
	BtShared* btShared = (BtShared*)malloc(sizeof(BtShared));
	assert(btShared);
	btShared->pager = &pager;
	void* root_page = pager.getPage(0);            //get root page of the db file
	btShared->file_header = pager.getDbFileHead(root_page);

	//create root MemPage
	MemPage* page0 = pager.getMemPage(root_page);

	//create btree cursor
	BtCursor* cursor = (BtCursor*)malloc(sizeof(BtCursor));
	assert(cursor);
	cursor->current_page = page0;
	cursor->cell_index = 0;
	(cursor->trace)[0] = 0;
	for(int i = 1; i < BTREE_MAX_DEEP; i++){
		(cursor->trace)[i] = -1;
	}

	btree->btShared = btShared;
	btree->rootPage = page0;
	btree->btCursor = cursor;

	return btree;
}

/*
 * delete btree
 */
void summerBtreeDelete(Btree* btree){
	free(btree->btShared->file_header);
	free(btree->btCursor);
	//free mem pool
}


/*
 * create db file
 * insert page0 into file, and insert 4 special pages into file, and the 4 is the same
 */
void summerBtreeCreateDbFile(char* file_name){
	//get a file name with postfix
	char* db_file_name = (char*)calloc(5*sizeof(char) + strlen(file_name));
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
	void* page = summerAlloc(memoryContext, page_siz);
	pager.getDataPage(page);
	page_no pageno = pager.writePage(-1, page);
	summerFree(memoryContext, page);
	return pageno;
}

/*
 *
 */
page_no summerBtreeInsert(char* table_name, ){
	MemPage* first_page = NULL;
	if(strcmp("_master", table_name) == 0){
		first_page = pager.getMemPage(pager.getPage(1));
	}
	else if(strcmp("_scheme", table_name) == 0){
		first_page = pager.getMemPage(pager.getPage(2));
	}
	else if(strcmp("_lock", table_name) == 0){
		first_page = pager.getMemPage(pager.getPage(3));
	}
	else if(strcmp("_sequence", table_name) == 0){
		first_page = pager.getMemPage(pager.getPage(4));
	}
	else{
		MemPage* master_page = pager.getMemPage(pager.getPage(1));
	}
}

