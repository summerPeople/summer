#include <stdint.h>

/*
 * here is some type we defined for simple and transplant
 */

typedef int32_t page_no;
typedef int32_t Size;

typedef struct{
	void *ptr;
	int size;
}tuple;
