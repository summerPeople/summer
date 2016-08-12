/*
 * this file is a pool where we put global varibales
 */
#include<stdio.h>

/*
 * file pointer of the database file we open,
 * it will be assigned when open file
 */
FILE* fp = NULL;

/*
 * fp setter
 */
void summerSetFP(FILE* file_ptr){
	fp = file_ptr;
}

