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
***Get the data page head info 
*/
void getDataPageHead(void *page_ptr, Data_page_head *head)
{
	head->type = *(char *)page_ptr;
	head->start = *(int16_t *)(page_ptr + 1);
	head->tuple_count = *(int16_t *)(page_ptr + 3);
	head->fragment = *(int16_t *)(page_ptr + 7);
	head->fs_size = *(int16_t *)(page_ptr + 9);
	head->pageno = *(int *)(page_ptr + 11);
}

/*
	 
*/
