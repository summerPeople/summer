//*********************************************************
/*
	statement:
 
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"../../sysconf/SQLtype.h"
#include"../../sysconf/type.h"
#include<stdint.h>
#include<stdbool.h>
#include"executerInt.h"
#include"executer.h"
#include"btreeInt.h"
#include"loggerInt.h"

#ifndef PTRSIZE 
#define PTRSIZE 8
#endif

/*
***analyse the sql structure from compiler 
*/
void summerSqlAnalyse(void *sql, void **_out_info, void **_out_parameter)
{  
//	printf("hello\n");
	LOG(INFO, "###################suumer Analyse begin.....##########################");
	 Execute_info *info = (Execute_info *)malloc(sizeof(Execute_info));
	 info->type = *(unsigned char *)sql;
	 void *parameter;
	 *_out_info = info;
   	  switch(info->type){  
		case OPEND:
			    //sql format
				//****type           char                        1bytes
				//****dbname         char *                      PTRSIZE bytes
	 			{
					//get the name
					char *dbname = (char *)*(intptr_t *)(sql + 1);
					//malloc a new place to store the dbname,for the sql structure
					//will be deleted after sql Analyse
					int32_t length =  strlen(dbname) + 2;
					char *new_dbname = (char *)malloc(length * sizeof(char));
					//copy the dbname
					strcpy(new_dbname, dbname);
					//bring the dbname out
					*_out_parameter = (void *)new_dbname;	
 				}
				break;
		case CDB:						//create database
				//sql format
				//****type           char                        1byte
				//****dbname         char *                      PTRSIZE bytes
 	  			{ 
					//get the name
					char *dbname = (char *)*(intptr_t *)(sql + 1);
					//malloc a new place to store the dbname,for the sql structure
					//will be deleted after sql Analyse
					int32_t length =  strlen(dbname) + 2;
					char *new_dbname = (char *)malloc(length * sizeof(char));
					//copy the dbname
					strcpy(new_dbname, dbname);
					//bring the dbname out
					*_out_parameter = (void *)new_dbname;
				}  
				break;
		case CTB:						//create table
				//sql format
				//****type           char                        1byte
				//****tname          char *                      PTRSIZE bytes
				//****attr_num       int16_t                     2bytes
				//****attr1          Attribute_type *            PTRSIZE bytes
				//****attr2          Attribute_type *            PTRSIZE bytes
				//....attr_num attrs
				{    
				
					int16_t attr_num = *(int16_t *)(sql + 1 + PTRSIZE);
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
					int16_t size = 4 + 8 + 2 + attr_num * 2 + 64 + 68 * attr_num;
					// joint a tuple
					parameter =(void *)malloc(size * sizeof(char));
					*_out_parameter = parameter;
					memset(parameter, 0, size * sizeof(char));
					//keep the tuple size
					info->size = size;
					//tuple headinfo
					//set page_no
					*(page_no *)parameter = 0;
					//for now we don't know the rowid, we will set it later befor we insert the 
					//tuple into _scheme table
					//set the attr_num
					*(int16_t *)(parameter + 12) = attr_num;
					//set the length of property
					int16_t *length = (int16_t *)(parameter + 14);
					int i = 0;
				 	for(i = 0; i < attr_num; i++){
						*(length + i) = 68;
					} 
					//schemeInfo data
					void *schemeInfo = parameter + 4 + 8 +2 +attr_num * 2;
					// table name
					strcpy(schemeInfo, (char *)*(intptr_t *)(sql + 1));
					strcpy(info->tname, (char *)*(intptr_t *)(sql + 1));
					schemeInfo += 64;
					//attribute
					for(i = 0; i < attr_num; i++){
						Attribute_type *attr = (Attribute_type *)(*(intptr_t *)(sql + 1 + PTRSIZE + 2 + PTRSIZE * i));
						//attribute name
						strcpy(schemeInfo, attr->str);
						LOG(INFO, (char *)parameter);
						//data type
						memcpy(schemeInfo + 64, &attr->type, 1);
						//attr length
						memcpy(schemeInfo + 65, &attr->length, 2);
						//attribute's attr
						memcpy(schemeInfo + 67, &attr->attr, 1);
						schemeInfo += 68;
					} 
 				 }  
				break;
		case DDB:						//	drop database
				//sql format
				//****type           char                        1byte
				//****dbname         char *                      PTRSIZE byites
				{ 
					//get the name
					char *dbname = (char *)*(intptr_t *)(sql + 1);
					//malloc a new place to store the dbname,for the sql structure
					//will be deleted after sql Analyse
					int32_t length =  strlen(dbname) + 2;
					char *new_dbname = (char *)malloc(length * sizeof(char));
					//copy the dbname
					strcpy(new_dbname, dbname);
					//bring the dbname out
					*_out_parameter = (void *)new_dbname;	
				}  
				break;
		case DTB:    					//	drop table
				//sql format
				//****type           char                        1byte
				//****dbname         char *                      PTRSIZE bytes
				{ 
					//get the name
					char *dbname = (char *)*(intptr_t *)(sql + 1);
					//malloc a new place to store the dbname,for the sql structure
					//will be deleted after sql Analyse
					int32_t length =  strlen(dbname) + 2;
					char *new_dbname = (char *)malloc(length * sizeof(char));
					//copy the dbname
					strcpy(new_dbname, dbname);
					//bring the dbname out
					*_out_parameter = (void *)new_dbname;	
					
				}
				break;
		case SEL:						//select 
				//sql format
				//****type			   char						 1byte
				//****tname			   char*					 PTRSIZE bytes
				//****attr_num		   int16_t					 2bytes
				//****attr1			   Att_node*				 PTRSIZE bytes
				//****attr2			   Att_node*                 PTRSIZE bytes
				//****attr_num attrs
				//****where_p          Where_type*               PTRSIZE bytes
 				{ 
					//get the table name
					printf("analyse select\n");
					char *tname = (char *)*(intptr_t *)(sql + 1);
					strcpy(info->tname, tname);
					printf("%s\n", tname);
				/*	int16_t attr_num = *(int16_t *)(sql + 1 + PTRSIZE);
					//get the scheme information of the table
					Scheme_info *scheme = bt.btreeGetSchemeInfo(tname);
					int16_t parameter_size = 0;
					int16_t *attrs = NULL;
					//get the selected attrs
 					if(attr_num == 0){					//select * from tbname
						attr_num = scheme->attr_num;
						//keep the selected attr's column no
						attrs = (int16_t *)malloc(sizeof(int16_t) * scheme->attr_num);
						int i = 0;
						for(i = 0; i < attr_num; i++)
							*(attrs + i) = i;
						parameter_size = 2 + scheme->attr_num * 2 + PTRSIZE;
					}
					else{								//select attr1,attr2 from tbname
						//keep the selected attr's column no
						attrs = (int16_t *)malloc(sizeof(int16_t) * attr_num);
						int i = 0, j = 0;
						for(i = 0; i < attr_num; i++ ){
							Att_node *temp = (Att_node *)(sql + 1 + PTRSIZE + 2 + PTRSIZE * i);
							for(j = 0; j < scheme->attr_num; j++){
								if(!strcmp(scheme->attr_array[i].str, temp->str))
									*(attrs + i) = i;
								break;
							
 							}
 						}
						parameter_size = 2 + attr_num * 2 + PTRSIZE;
 					}
					
					//deal with the where clause
					//bring the where expr out straightly
					Where_type *wh_op = (Where_type *)*(intptr_t *)(sql + 1 + PTRSIZE);
					Where_conList *wh_list = (Where_conList *)malloc(sizeof(Where_conList));
					wh_list->n = wh_op->n;
					//translate the Where_type into Where_conList;
					int i = 0;
					int j = 0;
					for(i = 0; i < wh_op->n; i++){
				 		Wh_node *temp = wh_op->or_arr[i];
				 		while(temp){
							for(j = 0; j < scheme->attr_num; j++){
				 				if(!strcmp(scheme->attr_array[i].str, temp->str)){
									Where_conNode *ptr = (Where_conNode *)malloc(sizeof(Where_conNode));
									ptr->index = j;
									ptr->flag = temp->flag;
									ptr->vint = temp->vint;
									if(temp->flag != ATTRINT)
										ptr->vchar = (char *)malloc(strlen(temp->vchar) + 2);
									else
										ptr->vchar = NULL;
									wh_list->or_array[i] = ptr;
				 				}
				  			}
							temp = temp->next;
				 		}
				  	}
					//form the parameters to be brought out
					parameter = (void *)malloc(parameter_size);
					*(int16_t *)parameter = attr_num;	
					//brings the selected attr's column no out
					memcpy(parameter + 2, attrs, attr_num * 2);
					//brings the wh_list outs;
					*(intptr_t *)(parameter + 2 + attr_num * 2) = (intptr_t)wh_list;
				*/	
					*_out_parameter = parameter;

					
				
				} 
				break;
		case INS:						//insert
				//sql format
				//****type             char                      1byte
				//****tname            char*                     PTRSIZE bytes
				//****attr_num         int16_t                   2bytes
				//****attr1            Att_node*                 PTRSIZE bytes
				//****attr2            Att_node*                 PTRSIZE bytes
				//.....attr_num attrs]
				//****value_num		   int16_t					 2bytes
				//****value1		   Va_node*					 PTRSIZE bytes
				//****value2		   Va_node*					 PTRSIZE bytes
				//.....value_num values
				//if(attr_num == 0) it indicates insert all attrs
				{  
					//get the table name
					char *tname = (char *)*(intptr_t *)(sql + 1);					
					strcpy(info->tname, tname);
					//get the scheme information of the table
					Scheme_info *scheme = bt.btreeGetSchemeInfo(tname);
					//
					int16_t head_size = 4 + 8 + 2 + 2 * scheme->attr_num;
					int16_t data_size = 0;
					void *head = (void *)malloc(head_size * sizeof(char));
					*(int16_t *)(head + 12) = scheme->attr_num;
					//pageno and rowid ....
					//
					int16_t *length = (int16_t *)(head + 14);

					//deal with tow type of insert action
					int16_t attr_num = *(int16_t *)(sql + 1 + PTRSIZE);
					//the insert is type of "insert into tname values(val1,val2,...)"
				 	if(attr_num == 0){  
						int i = 0;
						Va_node * temp = NULL;
						//calc the length of attr
				 		for(i = 0; i < scheme->attr_num; i++){ 
							temp =(Va_node *)*(intptr_t *)(sql + 1 + PTRSIZE + 2 + 2 + PTRSIZE * i);
							if(temp->flag == ATTRINT)
								*(length + i) = 4;
							else if(temp->flag == ATTRCHAR || temp->flag == ATTRVARCHAR)
								*(length +i) = strlen(temp->vchar) + 1;
							data_size += *(length + i);
				 		}
						//form the tuple
						int16_t tuple_size = head_size + data_size;
						info->size = tuple_size;
						parameter =(void *)malloc(sizeof(char) * tuple_size);
						//bring out the parameter
						*_out_parameter = parameter;
						//form the tuple head
						memcpy(parameter, head, head_size);
						free(head);
						//form the tuple data
						void *data = parameter + head_size;
						for(i = 0; i < scheme->attr_num; i++){ 
							temp = (Va_node *)*(intptr_t *)(sql + 1 + PTRSIZE + 2 + 2 + PTRSIZE * i);
							if(temp->flag == ATTRINT)
								*(int32_t *)data = temp->vint;
							else
								strcpy((char *)data, temp->vchar);
							data += *(length + i);
						}
				 	} 
					//the insert is type of "insert into tname (attr1, attr2) values(val1, val2)"
					else{ 
						int i = 0, j = 0;
						//calc the length of attrs
						for(i = 0; i < scheme->attr_num; i++){
							for(j = 0; j < attr_num; j++){
								Att_node *temp1 = (Att_node *)(sql + 1 + PTRSIZE + 2  + PTRSIZE * j);
								if(!strcmp(scheme->attr_array[i].str, temp1->str)){
									Va_node *temp2 = (Va_node *)(sql + 1 + PTRSIZE + 2 + PTRSIZE * attr_num + 2 + PTRSIZE * j);
									if(temp2->flag == ATTRINT)
										*(length + i) = 4;
									else
										*(length + i) = strlen(temp2->vchar) + 1;
									break;
					 	  		}
					  	  	}
							//attri is not in the insert sql, set the length of it 0
							if(j == attr_num)
								*(length + i) = 0;
							data_size += *(length + i);
					  	} 
						//form the tuple
						int16_t tuple_size = head_size + data_size;
						info->size = tuple_size;
						parameter = (void *)malloc(sizeof(char) * tuple_size);
						*_out_parameter = parameter;
						//form the tuple head
						memcpy(parameter, head, head_size);
						free(head);
						//form the tuple data
						void *data = parameter + head_size;
					 	for(i = 0; i < scheme->attr_num; i++){
							for(j = 0; j < attr_num; j++){
								Att_node *temp1 = (Att_node *)(sql + 1 + PTRSIZE + 2 + PTRSIZE * i);
						 		if(!strcmp(scheme->attr_array[i].str, temp1->str)){
									Va_node *temp2 = (Va_node *)(sql + 1 + PTRSIZE + 2 + PTRSIZE * attr_num + 2 + PTRSIZE * j);
									if(temp2->flag == ATTRINT)
										*(int32_t *)data = temp2->vint;
									else
										strcpy((char *)data, temp2->vchar);
									break;
				 		  	 	}
				 		   	 }
						 	data += *(length + i);
				 		}    
				 	 }    
				 }   
				break;
		case DEL:						//delete
				//sql format
				//****type              char                      1byte
				//****tname             char*                     PTRSIZE bytes
				//****where_p           Where_type*               PTRSIZE bytes
				{ 
					char *tname = (char *)*(intptr_t *)(sql + 1);
					strcpy(info->tname, tname);
					Scheme_info *scheme = bt.btreeGetSchemeInfo(tname);
					//bring the where expr out straightly
					Where_type *wh_op = (Where_type *)*(intptr_t *)(sql + 1 + PTRSIZE);
					Where_conList *wh_list = (Where_conList *)malloc(sizeof(Where_conList));
					wh_list->n = wh_op->n;
					//translate the Where_type into Where_conList;
					int i = 0;
					int j = 0;
					for(i = 0; i < wh_op->n; i++){
				 		Wh_node *temp = wh_op->or_arr[i];
				 		while(temp){
							for(j = 0; j < scheme->attr_num; j++){
				 				if(!strcmp(scheme->attr_array[i].str, temp->str)){
									Where_conNode *ptr = (Where_conNode *)malloc(sizeof(Where_conNode));
									ptr->index = j;
									ptr->flag = temp->flag;
									ptr->vint = temp->vint;
									if(temp->flag != ATTRINT)
										ptr->vchar = (char *)malloc(strlen(temp->vchar) + 2);
									else
										ptr->vchar = NULL;
									wh_list->or_array[i] = ptr;
				 				}
				  			}
							temp = temp->next;
				 		}
				  	}
					//brings the wh_list out
					*_out_parameter = wh_list;
				} 
				break;
		case UPD:						//update
				break;
		default:
				printf("sql analyse error!\n");
				break;		
 	}
	summerSqlFree(sql);
}   

/*
	execute the diffrent sql with the diffrent parameters  	
*/
void summerSqlExecute(Execute_info *_in_info, void *_in_parameters)
 {
	LOG(INFO, "###################summer execute begin.....##########################");
	//init the BtreeMethods sdtructure
	if(_is_init != 1){
		//init the bt
		bt.btreeCreateDbFile = summerBtreeCreateDbFile;
		bt.btreeOpenDbFile = summerBtreeOpenDbFile;
		bt.btreeInsertTuple = summerBtreeInsertTuple;
		bt.btreeSelectFromSpecialTable = summerBtreeSelectSpecTuple;
		bt.btreeDeleteDbFile = summerBtreeDeleteDbFile;
		bt.btreeGetTableRoot = summerBtreeWriteFreeDataPage;
	}
	switch(_in_info->type){
		case OPEND:				//open database
				{
					//_in_parameters format
					//****dbname             char *                PTRSIZE
					char *dbname = (char *)_in_parameters;
					openDatabase(dbname);
				}
				break;
		case CDB:			//create databae
				{ 
					//_in_paramters format
					//****dbname              char*                 PTRSIZE bytes
					char *dbname = (char *)_in_parameters;
					createDatabase(dbname);
				} 
				break;
		case CTB:			//create table
				{
					//_in_parameters format
					//a tuple in _scheme
					//to make a tuple
					tuple *new_tuple = (tuple *)malloc(sizeof(tuple));
					new_tuple->ptr = _in_parameters;
					new_tuple->size = _in_info->size;
					createTable(_in_info->tname, new_tuple);
				 }  
				break;
		case DDB:           //drop database
				{
					//_in_paramters format
					//****dbname              char*                 PTRSIZE bytes
					char *dbname = (char *)_in_parameters;
					dropDatabase(dbname);
				
				}
				break;
		case DTB:				//drop table
					//_in_parameters format
					//****tname            char *					PTRSIZE bytes
				{
					char *tname = (char *)_in_parameters;
					dropTable(tname);
				}
				break;
		case SEL:
				{
					executeSelect(_in_info->tname,NULL );
				
				
				}
				break;
		case INS:			//insert sql
				{  
					//_in_parameters format
					//a tuple in selected table;
					tuple *new_tuple = (tuple *)malloc(sizeof(tuple));
					new_tuple->ptr = _in_parameters;
					new_tuple->size = _in_info->size;
					executeInsert(_in_info->tname, new_tuple);
				}
				break;
		case DEL:			//delete a tuple
				{
					executeDelete(_in_info->tname, _in_parameters);		
				
				}
		default:
				break;
	
	}

}

/*
	open database 
*/
void openDatabase(char *dbname)
{ 
	LOG(INFO, "open database begin!");
	LOG(INFO, dbname);
	bt.btreeOpenDbFile(dbname);
	summerBtreeCreateBtree();
}

/*
 	create database
 */
bool createDatabase(char *dbname)
{     
	//create database
	LOG(INFO, "create database begin");
	LOG(INFO, dbname);
	bt.btreeCreateDbFile(dbname);
} 

/*
   drop database
*/
void dropDatabase(char *dbname)
{
	LOG(INFO, "drop database begin");
	LOG(INFO, dbname);
	bt.btreeDeleteDbFile(dbname);
}   

/*
 	create table
 */
bool createTable(char *tname, tuple *new_tuple)
{
	LOG(INFO, "create table begin");
	LOG(INFO, tname);
	//create a table needs four steps
	//firstly, insert a record into _master table
	//secondly, insert a record into _scheme table
	//thirdly, insert a record into _sequence table
	//at last, insert a record into _lock table
	
	//insert a new tuple into master table
	int32_t root = bt.btreeGetTableRoot(3);
//    int32_t tID = bt.btreeGetTableID();
	int32_t tID = 1;
	int16_t size;
	size = 4 + 8 + 2 + 2 * 3 + 64 + 4 + 4;
	void *str;
	str = (void *) malloc(size * sizeof(char));
	memset(str, 0, size);
	//inti the tuple head
	//pageno
	*(int32_t *)(str) = 0;
	//row id
	*(int16_t *)(str + 4) = tID;
	//attr_num
	*(int16_t *)(str + 12) = 3;
	//attr length
	*(int16_t *)(str + 14) = 64;
	*(int16_t *)(str + 16) = 4;
	*(int16_t *)(str + 18) = 4;
	//tname
	strcpy((char *)(str + 20), tname);
	//tID
	*(int32_t *)(str + 84) = tID;
	//root page
	*(int32_t *)(str + 88) = root;
	//form the tuple
	tuple *temp =(tuple *)malloc(sizeof(tuple));
	temp->ptr = str;
	temp->size = size;
	//insert a new tuple into _master table
	bt.btreeInsertTuple("_master", temp);
	free(str);

	//insert a new tuple into scheme table
	bt.btreeInsertTuple("_scheme", new_tuple);

	//insert into a new tuple into sequence table
	size = 4 + 8 + 2 + 2 * 2 + 64 +8;
	str =(void *)malloc(size * sizeof(char));
	*(int32_t *)str = 0;
	*(int16_t *)(str + 12) = 2;
	*(int16_t *)(str + 14) = 64;
	*(int16_t *)(str + 16) = 8;
	strcpy((char *)(str + 18), tname);
	*(int64_t *)(str + 82) = 0;
	temp->size = size;
	temp->ptr = str;
	bt.btreeInsertTuple("_sequence", temp);

	//insert into a new tuple into lock table
	//summerBtreeInsertTuple("_lock", );
}      

/*
	drop table 
*/
void dropTable(char *tname)
{
	LOG(INFO, "drop table begin");
	LOG(INFO, tname);
	//bt.btreeCreateTable
}

/*
	select sql 
*/
void executeSelect(char *tname, void *ptr)
{
	LOG(INFO, "select begin");
	LOG(INFO, tname);
//	bt.btreeSelectFromSpecialTable(tname, "hello");

}

/*
   insert a tuple into table
*/ 
void executeInsert(char *tname, tuple *new_tuple)
{
	LOG(INFO, "begin Insert");
	LOG(INFO, tname);
//	bt.btreeInsertTuple(tname, new_tuple);
	//free the new_tuple
	free(new_tuple->ptr);
	free(new_tuple);
}

/*
	delete a tuple from table 
*/
void executeDelete(char *tname, void *ptr)
{
	LOG(INFO, "begin delete");
	LOG(INFO, tname);
	Where_conList *wh_list = ptr;
	//bt.btreeDeleteTuple(tname, wh_list);
	//free the wh_list
	int i = 0;
	 for(i = 0; i < wh_list->n; i++){
		free(wh_list->or_array[i]);
	}
	free(wh_list);
}

void executeUpdate(char *tname)
{
	LOG(INFO, "update begin");
	LOG(INFO, tname);
}
/*
	free the sql parameter space 
 */
void summerSqlFree(void *sql)
{
	unsigned char type = *(unsigned char*)sql;
	switch(type){
		case OPEND:
			    {
					char *dbname = (char *)*(intptr_t *)(sql + 1);
					free(dbname);
					free(sql);
				}
				break;
		case CDB:
				{
					char *dbname = (char *)*(intptr_t *)(sql + 1);
					free(dbname);
					free(sql);
				}
				break;
		case CTB:
				{
					char *tname =(char *)*(intptr_t *)(sql + 1);
					free(tname);
					int16_t attr_num = *(int16_t *)(sql + 1 + PTRSIZE);
					int i = 0;
					for(i = 0; i < attr_num; i++){ 
						free((Attribute_type *)(*(intptr_t *)(sql + 1 + PTRSIZE + 2 + PTRSIZE * i)));
					}
					free(sql);
				
				}  
				break;
		case DDB: 
				{ 
					char *dbname =(char *)(* (intptr_t *)(sql + 1));
					free(dbname);
					free(sql);	
				} 
				break;
		case DTB:
				{
					char *tname =(char *)(* (intptr_t *)(sql + 1));
					free(tname);
					free(sql);	
				}  
				break;
		case INS:
				{
					char *tname = (char *)(*(intptr_t *)(sql + 1));
					free(tname);
					int16_t attr_num = *(int16_t *)(sql + 1 + PTRSIZE);
					if(attr_num){
						int i = 0;
						for(i = 0; i < attr_num; i++){
							free((Att_node *)(sql + 1 + PTRSIZE + PTRSIZE * i));
					 	}
						for(i = 0; i < attr_num; i++){ 
							free((Att_node *)(sql + 1 + PTRSIZE + 2 + PTRSIZE * attr_num + 2 + PTRSIZE * i));
						}
					} 
					else{
						int i = 0; 
						int16_t attr_num = *(int16_t *)(sql + 1 + PTRSIZE + 2);
						for(i = 0; i < attr_num; i++){
							free((Att_node *)(sql + 1 + PTRSIZE + 2 + 2  + PTRSIZE * i));
					 	}
					} 
					free(sql);
				}  
				break;
		case DEL:
				{
					char *tname = (char *)*(intptr_t *)(sql + 1);
					free(tname);
					Where_type *ptr = (Where_type *)*(intptr_t *)(sql + 1 + PTRSIZE);
					int i = 0;
					for(i = 0; i < ptr->n; i++){
						free(ptr->or_arr[i]);
					}
					free(sql);
				} 
				break;
		default:
				break;
	}
}
