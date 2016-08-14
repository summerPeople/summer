#ifndef _COMPILE_H
#define _COMPILE_H

#include "../../sysconf/SQLtype.h"

#define STMTYPE 1 //语句类型，对应SQLtype.h中的宏定义语句类型
#define DTNAME  4 //数据库或者表的名字的指针
#define ATTRNUM 2	//属性的个数
#define ATTRLEN 4 //指向每一个属性的指针
#define WHEREP 4 //指向where子句的指针
#define VALUENUM 2
#define VALUEP 4  //指向value子句的指针

enum token_type{
 	SEM = 1,	//;
	CREATE = 2,
	TABLE = 3,
	DATABASE =4,
	DROP = 5,
    SELECT = 6,
    FROM = 7,
    WHERE = 8,
    INSERT = 9,
    INTO = 10, 
    LEFTBRAC = 11, 
    RIGHTBRAC = 12, 
    VALUES =13,
    DELETE = 14, 
    UPDATE = 15, 
    SET = 16, 
    NAME =17,
    NUMBER =18,
	INT = 19,
	CHAR = 20,
	VARCHAR = 21,
	STAR = 22,
	COMMA = 23,
	NOTNULL = 24,
	PRIMARYKEY = 25,
	EQ = 26,	//=
	LS = 27,	//<
	LE = 28,	//<=
	MO = 29,	// >
	ME = 30,	// >=
	QM = 31,	//quotation marks 引号 '
	OR = 32,
	AND = 33,
	OPEN = 34,
	ERROR = 99
};

int yynum;
int yylen;
char * yyname;

typedef struct{
	Attribute_type *next;
 	Attribute_type *end;	
}Ct_head;   //创建表的时候的属性的头节点

typedef struct {
 	Va_node *next;
	Va_node *end;
}Va_head;

typedef struct {
	Att_node *next;
 	Att_node *end;
}Att_head;

#endif
