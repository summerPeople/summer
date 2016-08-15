//*********************************************************
/*
statement: 
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"page.h"
#include"../../sysconf/type.h"
#include"configLoaderInt.h"
#include"../../btree/src/btree.h"
#include"../../sysconf/SQLtype.h"

/*  
   insert a tuple into a page in memmory
	the first parameter is the address of the page
	the second parameter is the address of a tuple
*/
void insertTuple(void *page_ptr, tuple *tuple_ptr)
{
	int16_t *pn = (int16_t *)(page_ptr + 1);
	//the offset of insert_tuple
	*pn = *(int *)(page_ptr + 1) + *(int16_t *)(page_ptr + 8) - tuple_ptr->size;
	//insert the tuple
	memcpy(page_ptr + *pn, tuple_ptr->ptr, tuple_ptr->size);
	//update the tuple_count
	*((int16_t *)(page_ptr + 3)) += 1;
	//update fs_size
	*((int16_t *)(page_ptr + 9)) = *((int16_t *)(page_ptr + 8)) - tuple_ptr->size;
	//update the start
    *((int16_t *)(page_ptr + 1)) = *((int16_t *)(page_ptr + 1)) + 2;
	
/*	head_info *head = (head_info *)page_ptr;
	void *pnew = page_ptr + head->start + head->fs_size - tuple->size;
	//insert the tuple
	memcpy(pnew, tuple->ptr, tuple->size);
	//update the tuple_count
	head->tuple_count++;
	//update the start;
	head->start = head->start + 2;
	//update the free space size
	head->fs_size = head->fs_size - tuple->size;
	*/
} 

/*
	delete a tuple from a page in memory
	the first parameter is the address of the page
	the second parameter is the offset of the tuple to be deleted
 */
void deleteTuple(void *page_ptr, int16_t offset)
{	 
	void *tuple_ptr = page_ptr + offset;
	//set the delete flag;
	*(int32_t *)tuple_ptr = -1;
	//update the the tuple_count
	*((int16_t *)(page_ptr + 3)) -= 1;
	//update the fragement size;
	int16_t n = *(int16_t *)(tuple_ptr + 4);
	int *p = (int *)(tuple_ptr + 6);
	int length = 4 + 2 + 4 * n;//the length of tuple head
	int i = 0;
	for(i = 0; i < length; i++){
		length += *(p + i);
	}
	*((int16_t *)(page_ptr + 7)) += length;
} 

/*
 	update the	tuple
 */ 
void updateTuple()
{

}

/*
 	get a tuple
	the first parameter is the address of the page
	the secong parameter is the offset of a tuple
	the third parameter brings the tuple out
 */
void getTuple(void *page_ptr, int16_t offset, tuple *temp)
{ 
	void *tuple_ptr = page_ptr + offset;
	int16_t n = *(int16_t *)(tuple_ptr + 4);
	//calc the size of the tuple
	int *p = (int *)(tuple_ptr + 6);
	int size = 4 + 2 + 4 * n;
	int i = 0;
	for(i = 0; i < n; i++){
		size += *(p + i);
 	}
	temp->ptr = tuple_ptr;
	temp->size = size;	
} 

/*
*** when create a database, this routin will init the file head Info
*/
void initFileHead(void *page_ptr)
{
	memset(page_ptr, 0, 100);
	strcpy((char *)page_ptr, "summer v1.0");
	*(int *)(page_ptr + 16) = config_info.page_size;
} 

/*
***Get the dbfile head info 
*/
void getFileHead(void *page_ptr, File_head *ptr)
{ 
	strcpy(ptr->version_info, (char *)page_ptr);
	ptr->free_page_sum = *(int32_t *)(page_ptr + 36);
}

/*
***Init the head of data page 
 */
void initDataPageHead(void *page_ptr, char type)
{ 
	*(unsigned char *)page_ptr = type;
	*(int16_t *)(page_ptr + 1) = 15;
	*(int16_t *)(page_ptr + 3) = 0;
	*(int16_t *)(page_ptr + 7) = 0;
	*(int16_t *)(page_ptr + 9) = config_info.page_size - DPHSIZE;
	*(int32_t *)(page_ptr + 11) = 0;
} 

/* 
***init special table head 
*/
void initSpecTablePageHead(void *page_ptr)
{
	initDataPageHead(page_ptr, SPECIAL);

}

/*
***Get the data page head info 
*/
void getDataPageHead(void *page_ptr, Data_page_head *head)
{
	printf("get page head\n");
	printf("%p\n", page_ptr);
	head->type = *(char *)page_ptr;
	head->start = *(int16_t *)(page_ptr + 1);
	printf("start : %d\n", head->start);
	head->tuple_count = *(int16_t *)(page_ptr + 3);
	head->fragment = *(int16_t *)(page_ptr + 7);
	head->fs_size = *(int16_t *)(page_ptr + 9);
	head->pageno = *(int *)(page_ptr + 11);
} 

/*
***get a MemPage structure
*/
void *getMemPage(void *page_ptr)
{
	MemPage *ptr = (MemPage *)malloc(sizeof(MemPage));
	Data_page_head* head = (Data_page_head *)malloc(sizeof(Data_page_head));
	ptr->page_space = page_ptr;
	ptr->is_modify = 0;
	ptr->header = head;
	getDataPageHead(page_ptr, head);
	printf("get head finished!\n");
	ptr->cell_num = head->tuple_count;
	ptr->offsets = (int16_t *)malloc(ptr->cell_num * sizeof(int16_t));
	int32_t i = 0, k = 0;
	int32_t offset;
	//get the offset of undeleted tuples;
	for(i = 0; i < head->start; i = i + 2){
		offset = *(int16_t *)(page_ptr + DPHSIZE + i);
		if(*(page_no *)(page_ptr + offset) != -1)
			ptr->offsets[k] = offset;
	 }
	return ptr;
}

/*
	this routin judge if the tuple accorded with where clause
	if accoeded, return 0, else return -1
	the first parameter is a tuple ptr
	the second parameter is the where_clause
*/
int isTupleAccordWhere(tuple *tuple_ptr, void *wh_ptr)
{
	Where_conList *wh_list = (Where_conList *)wh_ptr;
	//the flag indicates whether the tuple is accorded with where clause
	int16_t flag = 0;

	int16_t n = wh_list->n;
	int i = 0;
	int j = 0;
	int16_t *length_array = (int16_t *)(tuple_ptr->ptr + 14);
	int16_t attr_num = *(int16_t *)(tuple_ptr->ptr + 12);
	int16_t head_size = 4 + 8 + 2 + attr_num * 2;
	int16_t start;
	int16_t attr_length;
	Where_conNode *temp = NULL;
	//check the condition given in where clause in order
	//block of code below isn't complite, lack of thoughts about special cases
	//such as when a sring attr in  the tuple is null, while the attr is given in where clause
	//error will occur
	for(i = 0; i < n; i++){
		temp = wh_list->or_array[i];
		while(temp){
			start = 0;
			for(j = 0; j < temp->index; j++){
				start += *(length_array + j);
			}
			attr_length = *(length_array + j);
			start += head_size;
			if(temp->flag == ATTRINT){
				if(*(int32_t *)(tuple_ptr->ptr + start) == temp->vint)
					flag = 1;
				else
					flag = 0;
			}
			else{
				char *attr = (char *)(tuple_ptr->ptr + start);
				if(!strcmp(attr, temp->vchar))
					flag = 1;
				else
					flag = 0;
			
			}
		}
		//shor circuit principle
		if(flag)
			return flag;
	}
	return flag;
}

/*
	assemble the scheme Info structure according to a record of _scheme table
	the parameter is a tuple of _scheme table
	return a Scheme_info pointer
*/
Scheme_info *assembleSchemeInfo(tuple *tuple_ptr)
{
	Scheme_info *scheme = (Scheme_info *)malloc(sizeof(Scheme_info));
	scheme->attr_num = *(int16_t *)(tuple_ptr->ptr + 12);
	scheme->tname = (char *)malloc(64);
	void *start = tuple_ptr->ptr + 4 + 8 + 2 + 2 * scheme->attr_num;
	//table name
	strcpy(scheme->tname, (char *)start);
	scheme->attr_array = (Attribute_type *)malloc(sizeof(Attribute_type) * scheme->attr_num);
	//move the ptr to the start of attr
	start += 64;
	int i = 0;
	while(i < scheme->attr_num){
		Attribute_type *temp = &scheme->attr_array[i];
		strcpy(temp->str, (char *)start);
		temp->type = *(char *)(start + 64);
		temp->length = *(int16_t *)(start + 65);
		temp->attr = *(char *)(start + 67);
		i++;
		//move the ptr to the next attr
		start += 68;
	}
	return scheme;
}

void parseTuple(tuple *tuple_ptr)
{
	int16_t attr_num = *(int16_t *)(tuple_ptr->ptr + 12);
	void *start = tuple_ptr->ptr + 14 + attr_num * 2;
	printf("table name:\t%s\n", (char *)start);
	start += 64;
	int i = 0;
	for(i = 0; i < attr_num; i++){
		printf("attr%d:\t%s\n", i, (char *)start);
		start += 68;
	}
}



