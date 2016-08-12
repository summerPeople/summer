#include "../btree/src/btree.h"

void summerBtreeCreateBtree();                  //create btree
void summerBtreeDelete();                       //delete btree

void summerBtreeCreateDbFile();                 //create db file

page_no summerBtreeWriteFreeDataPage();         //write a free data page into file, get pageno

page_no summerBtreeInsert(char* table_name, tuple* tuple_ptr); //insert tuple into table

