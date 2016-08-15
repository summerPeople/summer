/*
 	the type of SQL command
 */

#ifndef _SQLTYPE_H
#define _SQLTYPE_H

#include <stdint.h>

#define CDB 2 //create database dbname (filed)
#define CTB 3 //create table tbname
#define DDB 4 //drop database dbname
#define DTB 5 //drop table tbname
#define SEL 6 //select [*| filed1,filed2,filed3] form tbname where field1 = 'v1' [and | or] filed2 = v2; 
#define INS 7 //insert into tbname [  |(filed1,field2,field3)] values (v1,v2,v3);
#define DEL 8 //delete from tbname where field1 = 'v1' [and | or] filed2 = v2; 
#define UPD 9 //update tbname set filed1 = v1 , filed2 = v2 where filed1 = v1 [and | or] filed2 = v2;
#define OPEND 10 

#define EQT 26  // =
#define LST 27	// <
#define LET 28	// <=
#define MOT 29	// >
#define MET 30	// >=

#define ATTRINT 88   //属性为int
#define ATTRCHAR 89		//属性为char
#define ATTRVARCHAR 90	//属性为varchar

#define NULL_BIT 1		//用一个字符表示字段的属性，第1位（不是第0位）表示 not null属性
#define KEY_BIT 2		//第2位表示primary key 属性

/*
   create table 
   表字段结构体
*/
typedef struct att_node{
	char str[64];	//字段名称
	char type;	//字段类型 ATTRINT / ATTRCHAR / ATTRVARCHAR
	int16_t length;	 //若为ATTRINT，则length = 4 ,否则为自定义长度
	char attr;	//属性值，表示not null 和 primary key。 2为not null , 4为primary key ,6为not null && primary key
	struct att_node * next; //执行器不需要，编译器使用
}Attribute_type;

/*
	select/delete/update 
	where 子句的每个where条件的结构体
*/
typedef struct W_node{
	char str[64];   //WHERE子句中条件的左值，为字段名称
	char cond; //条件< >  <= >=等	值为EOT，LST，LET，MOT，MET
	char flag;	//WHERE子句中条件的右值，为ATTRINT / ATTRCHAR / ATTRVARCHAR
	int32_t vint;	//当flag = ATTRINT时有用，保存右值，否则无用
	char * vchar;	//当flag = ATTRCHAR / ATTRVARCHAR 时有用，保存右值，否则无用
	struct W_node * next; 
}Wh_node;

/*
	select/delete/update
	where 子句第一个的结构体，每一个数组元素指向一个条件
*/
typedef struct { 
	int16_t n;			//where子句的结构体，n表示接下来有n个or连接的where条件
	Wh_node * or_arr[10];  //有n个数组元素使用，每一个指向一个由and连接的where条件的链表
}Where_type;

/*
	insert/update
	保存insert中values后面的值，保存update中set子句赋值语句的右值	
 */
typedef struct V_node{
	char flag;	//value值的标志，可以为ATTRINT / ATTRCHAR / ATTRVARCHAR
	int32_t vint;	//当为ATTRINT ,则此位记录这个属性的值
	char * vchar;	//当为ATTRCHAR / ATTRVARCHAR ，则此位记录这个属性的值
	struct V_node * next;  //执行器不用，编译时使用
}Va_node;

/*
	select/insert/update
	保存字段名，保存update语句中set子句里面赋值语句的左值（也是字段名）
*/
typedef struct At_node{
	char str[64];	//字段名
	struct At_node * next;  //执行器不需要，编译器使用
}Att_node;

/*
	used for executer 
*/
typedef struct _node{
	int16_t index;
	char cond;
	char flag;
	int32_t vint;
	char *vchar;
	struct _node *next;
}Where_conNode;

typedef struct{
	int16_t n;
	Where_conNode *or_array[10];
}Where_conList;
#endif
