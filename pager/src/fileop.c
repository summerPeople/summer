#include "fileop.h"
#include <stdio.h>
#include <stdlib.h>

extern FILE* fp;

extern void summerSetFP(FILE* file_ptr);

/*
 * open a specifical database file
 */
int summerPagerOpenDbFile(char* file_name){ 
	FILE* file_ptr = fopen(file_name, "rb+");	
	if(file_ptr != NULL){
		summerSetFP(file_ptr);
		return 0;
	}
	return -1;
}

/*
 * create database file named file_name
 */
void summerPagerCreateDbFile(char* file_name){
	FILE* file_ptr = fopen(file_name, "wb");	
	summerSetFP(file_ptr);
}

/*
 * delete db file
 */
int summerPagerDeleteDbFile(char* file_name){
	if(remove(file_name) == 0){
		return 0;
	}else{
		return -1;
	}
}

/*
 * read a page from database file which page number is pageno
 * pageno from 0 to ......
 */
int summerPagerRead(page_no pageno, void* page){
	rewind(fp);
	int page_size = config_info.page_size; 
	long page_seek_location = pageno * page_size;
	fseek(fp, page_seek_location, SEEK_SET);
	size_t read_num = fread(page, page_size, 1, fp);
	if(read_num == 1){
		return 0;
	}
	return -1;
}

/*
 * functions those can be used only in the layer
 */

page_no pagerWrite(page_no pageno, void* page){
	rewind(fp);
	int page_size = config_info.page_size; 
	page_no write_page = -1;
	if(pageno == -1){
		fseek(fp, 0L, SEEK_END);
		long file_size = ftell(fp);
		write_page = file_size/page_size;
	}
	else{
		long page_seek_location = pageno * page_size;
		fseek(fp, page_seek_location, SEEK_SET);
		write_page = pageno;
	}
	size_t write_num = fwrite(page, page_size, 1, fp);
	if(write_num == 1){
		return write_page;	
	}
	return -1;
}

int summerPagerWrite(page_no pageno, void* page){
	return pagerWrite(pageno, page);
}

int summerPagerAddNewPage(void* page){
	return pagerWrite(-1, page);
}
