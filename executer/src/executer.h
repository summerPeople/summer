/*
	 执行器调用btree的相关方法 
*/
#ifndef EXECUTER_H
#define EXECUTER_H

#include"../../sysconf/type.h"

/*
   the needed execute info
*/
typedef struct{
	unsigned char type;			//the type of sql commmad
	int16_t size;		// if the sql command is related to insert, update
						// it is the size of the tuple
	char tname[64];		//the objected table name
}Execute_info;

typedef struct {
	page_no (*btreeGetTableRoot)(char type);
	int32_t (*btreeGetTableID)(void);
	void (*btreeCreateDbFile)(char *dbname);
	int (*btreeOpenDbFile)(char *);
	int (*btreeDeleteDbFile)(char *dbname);
	page_no (*btreeInsertTuple)(char *tname, tuple *new_tuple);
	void (*btreeDeleteTuple)(char *tname, void *ptr);
	Scheme_info *(*btreeGetSchemeInfo)(char *tname);
	tuple *(*btreeSelectFromSpecialTable)(char *, char *);
}BtreeMethods;


BtreeMethods bt;
int _is_init;

void openDatabase(char *dbname);
bool createDatabase(char *dbname);
void dropDatabase(char *dbname);
bool createTable(char *tname, tuple *new_tuple);
void dropTable(char *tname);
void executeSelect(char *tname, void *ptr);
void executeInsert(char *tname, tuple *new_tuple);
void executeDelete(char *tname, void *ptr);
void executeUpdate(char *ptr);
#endif
