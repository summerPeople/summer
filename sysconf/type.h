#ifndef TYPE_H
#define TYPE_H

#include <stdint.h>
#include "SQLtype.h"
/*
 * here is some type we defined for simple and transplant
 */ 

typedef int32_t page_no;
typedef int32_t Size;

/*
	the structure of a tuple 
*/
typedef struct{
	void *ptr;				//the ptr pointed to data of the tuple
	int size;				//the size of the tuple
}tuple;

/*
	the schemeInformation of a table
*/
typedef struct{
	char *tname;						//table name
	int16_t attr_num;					//the num of attributes
	Attribute_type *attr_array;			//the attr arrays 
}Scheme_info;

#endif
