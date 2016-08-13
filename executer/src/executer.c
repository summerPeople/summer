//*********************************************************
/*
	statement:
 
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"../../sysconf/SQLtype.h"
#include<stdint.h>
#include<stdbool.h>
#include<executerInt.h>
#include"exevuter.h"
#include"btreeInt.h"
#include"loggerInt.h"

//extern void summerBtreeCreateDbFile(char* file_name);

/*
***analyse the sql structure from compiler 
*/
void summerSqlAnalyse(void *sql, unsigned char **_out_type, void **_out_parameter)
{ 
	 unsigned char *type = (unsigned char *)malloc(sizeof(char));
	 void *parameter;
	*type = *(unsigned char*)sql;
	*_out_type = type;
//	summerSqlExecute(type, sql + 1);
 	switch(*type){ 
		case CDB:						//create database
				//sql format
				//****type           char                        1byte
				//****dbname         char *                      4bytes
				{
					//para
					parameter = malloc(sizeof(char *));
					//get the name
					char *dbname = (char *)*(int *)(sql + 1);
					//malloc a new place to store the dbname,for the sql structure
					//will be deleted after sql Analyse
					int32_t length =  strlen(dbname) + 2;
					char *new_dbname = (char *)malloc(length * sizeof(char));
					//copy the dbname
					strcpy(new_dbname, dbname);
					//bring the dbname out
					*(int *)parameter = (int)new_dbname;
					*_out_parameter = parameter;
				}
				break;
		case CTB:						//create table
				//sql format
				//****type           char                        1byte
				//****tname          char *                      4bytes
				//****attr_num       int16_t                     2bytes
				//****attr1          Attribute_type *            4bytes
				//****attr2          Attribute_type *            4bytes
				//....attr_num attrs
				{ 
				
					int16_t attr_num = *(int16_t *)(sql + 5);
					//the format of a tuple  in _scheme table
					//head info:
					//****pageno		page_no					 4bytes
					//****rowid         int64_t                  8bytes
					//****attr_num      int16_t					 2bytes
					//****length[attr_num]    int16_t			 2*attr_num bytes
					//data:
					//****tname			char[64]                 64bytes
					//attr1:
					//****attr1_name    char[64]                 64bytes
					//****type          char                     1bytes
					//****attr_length   int16_t                  2bytes
					//****attr_attr     char                     1bytes
					int16_t size = 2 + 4 + 8 + 2 + attr_num * 2 + 64 + 68 * attr_num;
					// joint a tuple
					parameter =(void *)malloc(size * sizeof(char));
					*_out_parameter = parameter;
					memset(parameter, 0, size * sizeof(char));
					//keep the tuple size
					*(int16_t *)(parameter) = size -2;
					//tuple headinfo
					//set page_no
					*(page_no *)parameter = 0;
					//for now we don't know the rowid, we will set it later befor we insert the 
					//tuple into _scheme table
					//set the attr_num
					*(int16_t *)(parameter + 14) = attr_num;
					//set the length of property
					int16_t *length = (int16_t *)(parameter + 16);
					int i = 0;
					for(i = 0; i < attr_num; i++){
						*(length + i) = 68;
					}
					//schemeInfo data
					void *schemeInfo = parameter + 2 + 4 + 8 +2 +attr_num * 2;
					// table name
					strcpy(schemeInfo, (char *)(*(int *)(sql + 1)));
					schemeInfo += 64;
					//attribute
					for(i = 0; i < attr_num; i++){
						Attribute_type *attr = (Attribute_type *)(*(int *)(sql + 7 + 4 * i));
						//attribute name
						strcpy(schemeInfo, attr->str);
						//data type
						memcpy(schemeInfo + 64, &attr->type, 1);
						//attr length
						memcpy(schemeInfo + 65, &attr->length, 2);
						//attribute's attr
						memcpy(schemeInfo + 67, &attr->attr, 1);
						schemeInfo += 68;
					}
				} 
				
		case DDB:						//	drop database
				break;
		case DTB:    					//	drop table
				break;
		case SEL:						//select 
				break;
		case INS:						//insert
				break;
		case DEL:						//delete
				break;
		case UPD:						//update
				break;
		default: break;		
	}

	summerSqlFree(sql);
}

/*
	execute the diffrent sql with the diffrent parameters  	
*/
void summerSqlExecute(unsigned char type, void *_in_parameters)
{
	switch(type){
		case CDB:			//create databae
				{
					//_in_paramters format
					//****dbname              char*                 4bytes
					char *dbname = (char *)(*(int *)_in_parameters);
					createDatabase(dbname);
				}
				break;
		case CTB:			//create table
				{
				/*
					tuple *new_tuple = (tuple *)malloc(sizeof(tuple));
					new_tuple->ptr = parameters + 2;
					new_tuple->size = *(int16_t *)parameters;
					int16_t attr_num = *(int16_t *)(parameters + 14);
					char *tname = (char *)(parameters + 16 + attr_num * 2);
					createTable(tname, new_tuple);
				*/
				 }
				break;
		default:
				printf("sql analyse error!\n");
				break;
	
	}

}

/*
 	create database
 */
bool createDatabase(char *dbname)
{  
	//create database
	LOG(INFO, "begin excute");
	summerBtreeCreateDbFile(dbname);
}

/*
 	create table
 */
bool createTable(char *tname, tuple *new_tuple)
{
	//create a table needs four steps
	//firstly, insert a record into _master table
	//secondly, insert a record into _scheme table
	//thirdly, insert a record into _sequence table
	//at last, insert a record into _lock table
	
	//insert a new tuple into master table
	int32_t tID = summerBtreeGetTableID();
    int32_t root = summerBtreeGetTableRoot();
	int16_t size = 4 + 8 + 2 + 2 * 3 + 64 + 4 + 4;
	void *str =(void *) malloc(size * sizeof(char));
	memset(str, 0, size);
	//
	*(int32_t *)(str) = 0;
	//row id
	//
	*(int16_t *)(str + 12) = 3;
	*(int16_t *)(str + 14) = 64;
	*(int16_t *)(str + 16) = 4;
	*(int16_t *)(str + 18) = 4;
	strcpy((char *)(str + 20), tname);
	*(int32_t *)(str + 84) = tID;
	*(int32_t *)(str + 88) = root;
	tuple *temp =(tuple *)malloc(sizeof(tuple));
	temp->str = str;
	temp->size = size;
	summerBtreeInsertTuple("_master", temp);

	//insert a new tuple into scheme table
	summerBtreeInsertTuple("_scheme", new_tuple);

	//insert into a new tuple into sequence table
	summerBtreeInsertTuple("_sequence", );

	//insert into a new tuple into lock table
	summerBtreeInsertTuple("_lock", );

}  

/*
	free the sql parameter space 
 */
void summerSqlFree(void *sql)
{
	unsigned char type = *(unsigned char*)sql;
	switch(type){
		case CDB:
				{
					char *dbname =(char *)(* (int *)(sql + 1));
					free(dbname);
					free(sql);
				}
				break;
		case CTB:
				{
					char *tname =(char *)(* (int *)(sql + 1));
					free(tname);
					int16_t attr_num = *(int16_t *)(sql + 5);
					int i = 0;
					for(i = 0; i < attr_num; i++){
						free((Attribute_type *)(*(int *)(sql + 7)));
					}
					free(sql);
				
				}
				break;
		default:
				break;
	}
}
