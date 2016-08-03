#ifndef FILE_OP_INT_H
#define FILE_OP_INE_H

/*
 *functions provide to other layer
 */

FILE* summerPagerCreateDbFile(char* file_name);
int summerPagerDeleteDbFile(char* file_name);

FILE* summerPagerOpenDbFile(char* file_name);

int summerPagerRead(page_no pageno, void* page);
int summerPagerWrite(page_no pageno, void* page);
int summerPagerAddNewPage(void *page);

#endif
