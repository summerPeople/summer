#ifndef FILE_OP_INT_H
#define FILE_OP_INE_H

/*
 *functions provide to other layer
 */

void summerPagerCreateDbFile(char* file_name);
int summerPagerOpenDbFile(char* file_name);
int summerPagerDeleteDbFile(char* file_name);

int summerPagerRead(page_no pageno, void* page);
page_no summerPagerWrite(page_no pageno, void* page);

#endif
