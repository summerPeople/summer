/*
	data structures used in the arrangement of dbfile
*/
#include"../../sysconf/type.h"
#ifndef PAGE_H
#define PAGE_H
/*
 dbfile headinfo, exist only in the start page of the dbfile
 */
typedef struct{
	char version_info[16];	//the dbfile version infomation
	int16_t page_size;		//page size
	char unused1;			//reserved
	char unused2;			//reserved
	char reserved_space;	//the special reserved space in the end of the page,usually 0
	char unused3;			//reserved
	char unused4;			//reserved
	char unused5;			//reserved
	int32_t modiy_count;		//the times of modifing
	int32_t unused6;			//reserved
	void *phead;			//the headnode point32_ter of the free page
	int32_t free_page_sum;		//the count of the free page
	int32_t unused7[16];		//reserved
}File_head;

/*
 	page headinfo, exist in every page of the dbfile
 */
typedef struct{
	char type;				//type of the page:special table page, leaf datapage, inner datapage
	int16_t start;			//the start place of the free space
	int16_t tuple_count;		//the number of the tuples in thi page now
	int16_t reserved;			//reserved
	int16_t fragment;			//the size of the frament space in the page
	int16_t fs_size;			//the size of free space
	union{
		void *next;			//type = SPECIAL,it indicates the next page of the special table
		int32_t page_no;		//type != SECIAL, it indicates the rightmost son of the inner node 
	}u;
}Data_page_head;

typedef struct{
	page_no pageno;			//if the tuple has been deleted, it will be -1
				   			//if the tuple has been updated, it will indicate the new page_no
							//of the tuple
							//in other cases, it will be 0
	int64_t rowid;			//tuple's row id
	int16_t column_count;
	int *length;

}Tuple_head; 
#define SPECIAL 1
#define INNER 2
#define LEAF 3


#define DPHSIZE 15
#define FHSIZE 100

#endif
