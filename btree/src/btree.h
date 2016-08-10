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

#define BTEE_MAX_DEEP 10

/*
 * a cell stands for 
 */

/*
 * BtShared struct, it is for the database file
 * it contains status info about the file
 */
typedef struct BtShared{
	file_head* file_header;                  //root page's page header info
	Pager* pager;                            //it ops the page
}BtShared;

/*
 * a page struct in memory
 */
typedef struct MemPage{
	page_no pageno;                          //page number in db file
	void* page_space;                        //where is the page in memory
	int16_t cell_num;                        //how many cell in the page
	int16_t offsets[];                       //an array stores offsets of tuples 
	datapage_head_info* header;              //page header struct
}MemPage;

/*
 * BtCursor struct,it points to where we are looking up
 */
typedef struct BtCursor{
	MemPage* current_page;                   //page that the corsur points to now
	int16_t cell_index;                      //index in MemPage.offsets
	page_no trace[BTREE_MAX_DEEP];           //those pages the cursor has moved
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
