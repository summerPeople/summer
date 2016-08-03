#include "fileop.h"


/*
 * functions those can be used only in the layer
 */

int pagerWrite(page_no pageno, void* page){
	int page_size = config_info.page_size; 
	if(pageno == -1){
		fseek(fp, 0L, SEEK_END);
	}
	else{
		long page_seek_location = pageno * page_size;
		fseek(fp, page_seek_location, SEEK_SET);
	}
	size_t write_num = fwrite(page, page_size, 1, fp);
	if(write_num == 1){
		return 0;
	}
	return -1;
}

/*

 * open a specifical database file
 * return NULL when open a file failed
 */
FILE* summerPagerOpendbFie(char* file_name){ 
	return fopen(file_name, "rb+");	
}

/*
 * create database file named file_name
 */
FILE* summerPagerCreateDbFile(char* file_name){
	return fopen(file_name, "wb");	
}

/*
 * read a page from database file which page number is pageno
 * pageno from 0 to ......
 */
int summerPagerRead(page_no pageno, void* page){
	int page_size = config_info.page_size; 
	long page_seek_location = pageno * page_size;
	fseek(fp, page_seek_location, SEEK_SET);
	size_t read_num = fread(page, page_size, 1, fp);
	if(read_num == 1){
		return 0;
	}
	return -1;
}

int summerPagerWrite(page_no pageno, void* page){
	return pagerWrite(pageno, page);
}

int summerPagerAddNewPage(void* page){
	return pagerWrite(-1, page);
}
