#ifndef BTREE_H
#define BTREE_H
/*
 * structs used by btree
 */
#include "../../sysconf/type.h"
#include "pagerInt.h"
#include "pageInt.h"
#include "memoryContextInt.h"
#include "configLoaderInt.h"

#define BTREE_MAX_DEEP 10
#define BTREE_MAX_CHILDS 134                         //how many childs inner nodes have

#define CURRENT_CELL_INDEX  ((cursor->cell_index)[cursor->trace_index])   //current cell index
#define CURRENT_MEMPAGE     ((cursor->trace)[cursor->trace_index])		  //current memPage

/*
 * a cell stands for 
 */

/*
 * BtShared struct, it is for the database file
 * it contains status info about the file
 */
typedef struct BtShared{
	File_head* file_header;                  //root page's page header info
	Pager* pager;                            //it ops the page
}BtShared;

/*
 * a page struct in memory
 */
typedef struct MemPage{
	page_no pageno;                          //page number in db file
	void* page_space;                        //where is the page in memory
	int16_t cell_num;                        //how many cell in the page
	int16_t* offsets;                        //an array stores offsets of tuples 
	uint8_t is_modify;                       //is the page modified
	Data_page_head* header;                  //page header struct
}MemPage;

/*
 * BtCursor struct,it points to where we are looking up
 */
typedef struct BtCursor{
	int16_t cell_index[BTREE_MAX_DEEP];      //index in MemPage.offsets
	int16_t trace_index;                     //index in trace
	MemPage* trace[BTREE_MAX_DEEP];          //those pages the cursor has moved
}BtCursor;

/*
 * a database file will map into a btree
 */
typedef struct Btree{
	BtShared* btShared;                      //use this to find pager
	MemPage* rootPage;                       //the root page of the db file
	BtCursor* btCursor;                      //use this find cursor
}Btree;

#endif
