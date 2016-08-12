/*
 	the type of SQL command
 */

#ifndef SQLTYPE
#define _SQLTYPE_H

#define CDB 2
#define CTB 3
#define DDB 4
#define DTB 5
#define SEL 6
#define INS 7
#define DEL 8
#define UPD 9

#define ATTRINT
#define ATTRCHAR
#define ATTRVARCHAR

#define NULL_BIT 1
#define KEY_BIT 2

typedef struct {
	char str[64];
	char type;
	int16_t length;
	char attr;
}Attribute_type;

#endif
