#include "btree.h"
#include "memoryPoolInt.h"

#include<stdlib.h>
#include<assert.h>
#include<string.h>
#include<stdio.h>

page_no summerBtreeWriteFreeSpecPage();


/*
 * it is a btree created when application is starting
 */
Btree* global_btree = NULL;


/*
 * free a MemPage
 */
static
void btreeFreeMemPage(MemPage* page){
	free(page->header);
	free(page->offsets);
	free(page);
}

/*
 * init BtCursor to spec page(only one)
 */
static 
void btreeInitCursorSpec(){
	BtCursor* cursor = global_btree->btCursor;
	int i = cursor->trace_index;
	while(i > 1){
		btreeFreeMemPage((cursor->trace)[i]);
	}
	cursor->trace_index = 1;
}

/*
 * init BtCursor to root page, memPages in trace must be free
 */
static 
void btreeInitCursorRoot(){
	BtCursor* cursor = global_btree->btCursor;
	int i = cursor->trace_index;
	while(i > 0){
		btreeFreeMemPage((cursor->trace)[i]);
	}
	cursor->trace_index = 0;
}

/*
 * find the table tuple in in_which_table, it is a spec table
 * when return 0, tuple accord has found, return -1, not found
 * location is stored in BtCursor
 */
static
int btreeFindSpecLocation(char* in_which_table, char* table_name){
	//init Cursor to rootpage
	BtCursor* cursor = global_btree->btCursor;	
	btreeInitCursorRoot();
	//find the second page
	MemPage* spec_page = NULL;
	if(strcmp("_master", in_which_table) == 0){
		spec_page = (MemPage*)pager.getMemPage(pager.getPage(1));
		spec_page->pageno = 1;
	}
	else if(strcmp("_scheme", in_which_table) == 0){
		spec_page = (MemPage*)pager.getMemPage(pager.getPage(2));
		spec_page->pageno = 2;
	}
	else if(strcmp("_lock", in_which_table) == 0){
		spec_page = (MemPage*)pager.getMemPage(pager.getPage(3));
		spec_page->pageno = 3;
	}
	else if(strcmp("_sequence", in_which_table) == 0){
		spec_page = (MemPage*)pager.getMemPage(pager.getPage(4));
		spec_page->pageno = 4;
	}
	page_no next_spec_pageno = spec_page->header->pageno;
	cursor->trace_index++;
	(cursor->trace)[cursor->trace_index] = spec_page;
	//in spec table we find the tuple whose table name is table_name
	tuple* tuple_out = (tuple*)malloc(sizeof(tuple));
	assert(tuple_out);
	while(spec_page != NULL){
		//we lookup each tuple if it is table table_name
		for(CURRENT_CELL_INDEX = 0; CURRENT_CELL_INDEX < spec_page->cell_num;
				CURRENT_CELL_INDEX++){
			int16_t offset = *(spec_page->offsets + CURRENT_CELL_INDEX);
			pager.getTuple(spec_page->page_space, offset, tuple_out);
			char* tuple_char = (char*)(tuple_out->ptr);
			int16_t* attr_num = (int16_t*)(tuple_char + 12);
			char* table_name_spec = tuple_char + 14 + 2*(*attr_num);
			if(strcmp(table_name, table_name_spec) == 0){      //we find the table
				free(tuple_out);
				return 0;	
			}
		}
		//we dont find the table in the master page
		if(next_spec_pageno == -1){
			spec_page = NULL;
		}else{
			spec_page = (MemPage*)pager.getMemPage(pager.getPage(next_spec_pageno));
			spec_page->pageno = next_spec_pageno;
			next_spec_pageno = spec_page->header->pageno;
			cursor->trace_index++;
			(cursor->trace)[cursor->trace_index] = spec_page;
		}
	}
	free(tuple_out);
	return -1;	
}

/*
 * find the location of the akey
 * before the function, you must call btreeFindDataTable()
 * if return -1, we find the location whose key less than akey a little
 * if return 0, we find the location whose key equals akey
 * if return 1, we find the location whose key larger than akey a little
 */
static 
int btreeFindDataLocation(int64_t akey){
	BtCursor* cursor = global_btree->btCursor;	
	MemPage* compare_page = (cursor->trace)[cursor->trace_index];
	tuple* compare_tuple = (tuple*)malloc(sizeof(tuple));
	while(compare_page->header->type != 3){   //if it is not a leaf page, we find the leaf
		for(CURRENT_CELL_INDEX = 0; CURRENT_CELL_INDEX < compare_page->cell_num;
				CURRENT_CELL_INDEX++){
			int16_t offset = (compare_page->offsets)[CURRENT_CELL_INDEX];
			pager.getTuple(compare_page->page_space, offset, compare_tuple);
			int64_t tuple_key = pager.getTupleKey(compare_tuple);
			if(akey <= tuple_key){
				char* tuple_char = (char*)compare_tuple;
				page_no child_pageno = *((page_no*)(tuple_char + 26)); 
				compare_page = (MemPage*)pager.getMemPage(pager.getPage(child_pageno));
				compare_page->pageno = child_pageno;
				cursor->trace_index++;
				(cursor->trace)[cursor->trace_index] = compare_page;
			}
		}
	}
	//now we find the leaf page which besides the akey, akey can only be found in the page
	//or in head or tail of the page
	for(CURRENT_CELL_INDEX = 0; CURRENT_CELL_INDEX < compare_page->cell_num;
			CURRENT_CELL_INDEX++){
		int16_t offset = (compare_page->offsets)[CURRENT_CELL_INDEX];
		pager.getTuple(compare_page->page_space, offset, compare_tuple);
		int64_t tuple_key = pager.getTupleKey(compare_tuple);
		if(akey < tuple_key){
			if(CURRENT_CELL_INDEX == 0){
				return -1;
			}else{
				CURRENT_CELL_INDEX++;
				return 1;
			}
		}else if(akey == tuple_key){
			return 0;
		}
	}
	if(CURRENT_CELL_INDEX == compare_page->cell_num - 1){
		free(compare_tuple);
		return -1;
	}
	if(compare_page->cell_num == 0){
		free(compare_tuple);
		return 0;
	}
	
	free(compare_tuple);
}

/*
 * find table table,when it return, cursor is in the table's first cell 
 */
static 
int btreeFindDataTable(char* table_name){
	if(btreeFindSpecLocation("_master", table_name) == 0){
		BtCursor* cursor = global_btree->btCursor;
		MemPage* find_page = (cursor->trace)[CURRENT_CELL_INDEX];
		tuple* tuple_out = (tuple*)malloc(sizeof(tuple));
		assert(tuple_out);
		int16_t offset = (find_page->offsets)[CURRENT_CELL_INDEX];
		pager.getTuple(find_page->page_space, offset, tuple_out);
		page_no* pageno = (page_no*)((char*)(tuple_out->ptr) + 92);
		find_page = pager.getMemPage(pager.getPage(*pageno));
		btreeInitCursorSpec();
		cursor->trace_index++;
		CURRENT_MEMPAGE = find_page;
		CURRENT_CELL_INDEX = 0;
	}
	else{
		return -1;
	}
}

/*
 * find the sequence_id of table_name in sequence table
 * if update_flag == 1, update, if == 0, do not update
 * it will return the old sequence id in sequence table
 */
static
int64_t btreeFindOrUpdateSequenceId(char* table_name, int update_flag){
	if(btreeFindSpecLocation("_sequence", table_name) == 0){
		BtCursor* cursor = global_btree->btCursor;	
		MemPage* sequence_page = CURRENT_MEMPAGE;	
		tuple* tuple_out = (tuple*)malloc(sizeof(tuple));
		assert(tuple_out);
		int16_t offset = *(sequence_page->offsets + CURRENT_CELL_INDEX);
		pager.getTuple(sequence_page->page_space, offset, tuple_out);
		int64_t* table_sequence_id = (int64_t*)((char*)(tuple_out->ptr) + 82);
		int64_t sequence_id = *table_sequence_id;
		if(update_flag == 0){    //update the sequence id
			*table_sequence_id = *table_sequence_id + 1;
		}
		free(tuple_out);
		return sequence_id;
	}
	else{
		return -1;
	}
}

/*
 * find one by one to confirm if the tuple accord where clause
 * for innner page, get child whose location is in cursor
 * for leaf page, get next tuple 
 * return 0,find a tuple ,return -1, selection is over
 */
static
int btreeSequencialSelect(char* table_name, void* where_clause){
	BtCursor* cursor = global_btree->btCursor;
	if(cursor->trace_index == 0){
		btreeFindDataTable(table_name);
	}
	MemPage* current_page = CURRENT_MEMPAGE; 
		
	tuple* tuple_out = (tuple*)malloc(sizeof(tuple));
	assert(tuple_out);

	while(1){			//if has not found a tuple and does not reach over
		pager.updateMemPage(current_page);      //update the mem page
		//if we are in master page, then selection is over
		if(current_page->pageno == 1){
			free(tuple_out);
			cursor->trace_index--;
			btreeFreeMemPage(current_page);
			return -1;
		}

		if(current_page->header->type == 2){    //now we are in inner node
			if(CURRENT_CELL_INDEX = current_page->cell_num + 1){  //we return to right most child
				cursor->trace_index--;
				btreeFreeMemPage(current_page);
				current_page = CURRENT_MEMPAGE;
				CURRENT_CELL_INDEX++;
			}
			else if(CURRENT_CELL_INDEX == current_page->cell_num){
				//get the right most child
				page_no most_child_pageno = current_page->header->pageno;
				current_page = (MemPage*)pager.getMemPage(pager.getPage(most_child_pageno));
				cursor->trace_index++;
				CURRENT_MEMPAGE = current_page;
				CURRENT_CELL_INDEX = 0;
			}else{
				//get the child page
				int16_t offset = (current_page->offsets)[CURRENT_CELL_INDEX];
				pager.getTuple(current_page->page_space, offset, tuple_out);
				char* tuple_char = (char*)(tuple_out->ptr);
				page_no child_pageno = *((page_no*)(tuple_char + 26)); 
				current_page = (MemPage*)pager.getMemPage(pager.getPage(child_pageno));
				cursor->trace_index++;
				CURRENT_MEMPAGE = current_page;
				CURRENT_CELL_INDEX = 0;
			}
		}
		else if(current_page->header->type == 3){    //now we are in leaf node
			for(CURRENT_CELL_INDEX = CURRENT_CELL_INDEX + 1;
					CURRENT_CELL_INDEX < current_page->cell_num;
					CURRENT_CELL_INDEX++){
				int16_t offset = (current_page->offsets)[CURRENT_CELL_INDEX];
				pager.getTuple(current_page->page_space, offset, tuple_out);
				//if we find a tuple accord the where clause
				if(pager.isTupleAccordWhere(tuple_out, where_clause) == 0){
					free(tuple_out);
					return 0;
				}
			}
			//if this page has been selected over, we go to it's parent
			if(CURRENT_CELL_INDEX == current_page->cell_num){
				cursor->trace_index--;
				btreeFreeMemPage(current_page);	
				current_page = CURRENT_MEMPAGE;
				CURRENT_CELL_INDEX++;
			}
		}
	}
}

/*
 * insert tuple into spec table
 */
 static 
page_no btreeInsertSpecTuple(char* table_name, tuple* tuple_ptr){
	//init Cursor to rootpage
	BtCursor* cursor = global_btree->btCursor;	
	btreeInitCursorRoot();
	//find the second page
	MemPage* special_page = NULL;
	if(strcmp("_master", table_name) == 0){
		special_page = (MemPage*)pager.getMemPage(pager.getPage(1));
		special_page->pageno = 1;
	}
	else if(strcmp("_scheme", table_name) == 0){
		special_page = (MemPage*)pager.getMemPage(pager.getPage(2));
		special_page->pageno = 2;
	}
	else if(strcmp("_lock", table_name) == 0){
		special_page = (MemPage*)pager.getMemPage(pager.getPage(3));
		special_page->pageno = 3;
	}
	else if(strcmp("_sequence", table_name) == 0){
		special_page = (MemPage*)pager.getMemPage(pager.getPage(4));
		special_page->pageno = 4;
	}
	if(special_page != NULL){
		MemPage* insert_mem_page = special_page;
		page_no next_page = insert_mem_page->header->pageno;
		//find the page we can insert
		while(insert_mem_page->header->fs_size < 2 + tuple_ptr->size){
			if(next_page == -1){
				break;
			}else{
				insert_mem_page = (MemPage*)pager.getMemPage(pager.getPage(next_page));
				insert_mem_page->pageno = next_page;
				next_page = insert_mem_page->header->pageno;
			}
		}

		if(insert_mem_page->header->fs_size >= 2 + tuple_ptr->size){       //we find the insert page
			pager.insertTuple(insert_mem_page->page_space, tuple_ptr);
			printf("insert page ptr: %p\n", insert_mem_page->page_space);
			return insert_mem_page->pageno;
		}
		else{
			//it is not over
			page_no new_pageno =  summerBtreeWriteFreeSpecPage();
			return -1;
		}
	}
	else{
		return -1;
	}
 }

/*
 * insert tuple into data table
 * before this we find the data table
 */
static
page_no btreeInsertDataTuple(char* table_name, tuple* tuple_ptr){

}

/*
 * balance the BTree, if tuple_ptr != NULL, it is insertion balance
 * if tuple_ptr == NULL, it is delete or update(update = insert and update)
 * before this, you should find the location
 */
static 
void btreeBalance(tuple* tuple_ptr){
	BtCursor* cursor = global_btree->btCursor;
	//get location page op occurs
	MemPage* op_page = (cursor->trace)[cursor->trace_index];
	if(tuple_ptr != NULL){

	}
}

/************************************************************************************************************/
/*********************************************** interface for others ***************************************/
/************************************************************************************************************/

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
	page0->pageno = 0;

	//create btree cursor
	BtCursor* cursor = (BtCursor*)malloc(sizeof(BtCursor));
	assert(cursor);
	CURRENT_CELL_INDEX = 0;
	cursor->trace_index = 0;
	CURRENT_MEMPAGE = page0;

	btree->btShared = btShared;
	btree->rootPage = page0;
	btree->btCursor = cursor;
	
	global_btree = btree;
}

/*
 * delete btree
 */
void summerBtreeDeleteBtree(){
	free(global_btree->btShared->file_header);
	free(global_btree->btShared->pager);
	free(global_btree->btShared);

	for(int i = 0; i <= global_btree->btCursor->trace_index; i++){
		btreeFreeMemPage((global_btree->btCursor->trace)[i]);
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
 * delete db file
 */
int summerBtreeDeleteDbFile(char* file_name){
	//get a file name with postfix
	char* db_file_name = (char*)calloc(1, 5*sizeof(char) + strlen(file_name));
	char* postfix_str = ".hot";
	strcpy(db_file_name, file_name);
	strcat(db_file_name, postfix_str);
	page_no pageno = pager.deleteDbFile(db_file_name);
	free(db_file_name);
	return pageno;
}

int summerBtreeOpenDbFile(char* file_name){
	//get a file name with postfix
	char* db_file_name = (char*)calloc(1, 5*sizeof(char) + strlen(file_name));
	char* postfix_str = ".hot";
	strcpy(db_file_name, file_name);
	strcat(db_file_name, postfix_str);
	page_no pageno = pager.openDbFile(db_file_name);
	free(db_file_name);
	return pageno;
}

/*
 * write a new free data page into db file
 */
page_no summerBtreeWriteFreeDataPage(char type){
	int page_size = config_info.page_size;
	void* page = summerAlloc(memoryContext, page_size);
	pager.getDataPage(page, type);
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
 * get sequence id and update it to sequenceId + 1
 */
int64_t summerBtreeGetSequenceId(char* table_name){
	return btreeFindOrUpdateSequenceId(table_name, 0);
}

/*
 * insert tuple into special table, but it is also the entrance of 
 * insertion into data table, only special table name starts with '_'
 */
page_no summerBtreeInsertTuple(char* table_name, tuple* tuple_ptr){
	//find if table_name is spec table or not
	if(*table_name == '_'){
		return btreeInsertSpecTuple(table_name, tuple_ptr);
	}
	else{
		return btreeInsertDataTuple(table_name, tuple_ptr);
	}
}

/*
 * delete tuple
 */

/* 
 * select from special table
 */
tuple* summerBtreeSelectSpecTuple(char* in_which_table, char* table_name){
	printf("get tuple begin, in %s, find %s\n", in_which_table, table_name);
	if(btreeFindSpecLocation(in_which_table, table_name) == 0){
		BtCursor* cursor = global_btree->btCursor;
		MemPage* find_page = (cursor->trace)[cursor->trace_index];
		tuple* tuple_out = (tuple*)malloc(sizeof(tuple));
		assert(tuple_out);
		int16_t offset = (find_page->offsets)[CURRENT_CELL_INDEX];
		pager.getTuple(find_page->page_space, offset, tuple_out);
		parseTuple(tuple_out);
		return tuple_out;
	}
	else{
		printf("get tuple failed! \n");
		return NULL;
	}

}

