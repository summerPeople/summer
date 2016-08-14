#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "compile.h"
#include "../../sysconf/SQLtype.h"

/*
	解析WHERE子句，  
 */
void * whereCompile(){
	void * wp;
    int16_t n=0; //
    int16_t tok1 = yylex();
    Where_type * p =(Where_type *)malloc(sizeof(Where_type));
	//识别左值
    while(tok1 == NAME){
        Wh_node * new_wh_node = (Wh_node *) malloc(sizeof(Wh_node));
        strcpy(new_wh_node->str , yyname);
        int16_t tok2 = yylex();
		// 识别符号
        if(EQ <= tok2 && ME>= tok2) {
            if(tok2 == EQ) new_wh_node->cond = EQT;
			else if(tok2 == LS) new_wh_node->cond = LST;
			else if(tok2 == LE) new_wh_node->cond = LET;
			else if(tok2 == MO) new_wh_node->cond = MOT;
			else if(tok2 == ME) new_wh_node->cond = MET;
		}
		//识别右值
        int16_t tok3 = yylex();
        if(tok3 == QM || tok3 ==NUMBER){
              if(tok3 ==NUMBER){
                new_wh_node->flag = ATTRINT;
                new_wh_node->vint=yynum;
                new_wh_node->vchar = NULL;
                new_wh_node->next = NULL;
            }
            else if(tok3 == QM && yylex() == NAME) {
                new_wh_node->flag = ATTRCHAR;
                new_wh_node->vint = 0;
				new_wh_node->vchar = (char *)malloc(sizeof(char)*(yylen +1));
                *(int32_t *)new_wh_node->vchar = yyname;
                new_wh_node->next = NULL;
                if(yylex() == QM) ;
            }   
            int16_t tok4 = yylex();
            if(tok4 == AND || tok4 == OR || tok4 == SEM){
                Wh_node * cur_node = p->or_arr[n];
                if(cur_node == NULL){
				 	p->or_arr[n] = new_wh_node;
                }
                else{
                    while(cur_node ->next !=NULL)
                        cur_node = cur_node->next;
                    cur_node->next = new_wh_node;
                }
				if(tok4 == OR) n++;
				if(tok4 == SEM) {
					//printf("退出where子句\n");
					return p;
					break;
				}
            } 
        }	
        tok1 = yylex();
    } 
}

/*
	解析create 语句
 */
void * createStm(){
	char * sql; 
	int16_t tok1 = yylex();
	//识别“table"
	if(tok1 == TABLE){
	 	if(yylex() == NAME){
	 		char * tbname = (char *)malloc(sizeof(char)*(yylen+1));
			strcpy(tbname,yyname);
			if(yylex() == LEFTBRAC){
			 	int16_t n = 0;
				Ct_head * ct_head = (Ct_head *)malloc(sizeof(Ct_head));
				ct_head->next = NULL; ct_head->end = NULL;
				int16_t tok2 = yylex();
				while(tok2 == NAME){
			 		n++;
					Attribute_type * new_node = (Attribute_type*)malloc(sizeof(Attribute_type));
					new_node->next = NULL;
					new_node->attr = 0;
					strcpy(new_node->str,yyname);
				//	printf("%s \n",new_node->str);
					switch (yylex()){
	 			 		int char_flag = 0;
                        case INT :{new_node->type = ATTRINT; new_node->length=4;printf("int\n");break;}
                        case CHAR : char_flag = 1;
                        case VARCHAR : { 
                             if(yylex() == LEFTBRAC && yylex() == NUMBER){
				 				if(char_flag == 1)	new_node->type = ATTRCHAR;
								else new_node->type = ATTRVARCHAR;
								new_node->length = yynum;
	 		 				//	printf("char %d\n",yynum);
                            }
                            if(yylex() == RIGHTBRAC)
                                break;
                              }
                         default : printf("[compiler],没有这种字段类型\n");
                    }
					int tok3 = yylex();
                    while(tok3 != COMMA && tok3 != RIGHTBRAC){ 
                        if(tok3 == NOTNULL){new_node->attr|=(1<<NULL_BIT);}
                        else if(tok3 == PRIMARYKEY){
                            new_node->attr|=(1<<KEY_BIT);
                        }
						tok3 = yylex();
                    }
					if(tok3 == COMMA || tok3 == RIGHTBRAC) {
						if(ct_head->next == NULL){
			 			 	ct_head->next = new_node;
							ct_head->end = new_node;
                        }
                        else {
                            ct_head->end->next = new_node;
                            ct_head->end = new_node;
                        }
                        if(tok3 == COMMA){
                            tok2 = yylex();
                            continue ;
                         } 
					}
					if(tok3 == RIGHTBRAC){
                        sql  = (char *)malloc(sizeof(char)*(STMTYPE+DTNAME+ATTRNUM+ATTRLEN*n));
                        *(sql) = CTB;
                        *(int32_t *)(sql+1) = tbname;
						//printf("构造sql:tbname = %s\n",(char *)(*(int32_t *)(sql+1)));
                        *(int16_t *)(sql+5) = n;
                        int16_t i = 0;
                        Attribute_type *cur_node = ct_head->next;
                        while(cur_node != NULL){
                             *(int32_t *) (sql+7+i*ATTRLEN) = cur_node;
						//	printf("第%d 个参数 %s 类型%d 长度%d 属性%d \n",i+1,cur_node->str,cur_node->type,cur_node->length,cur_node->attr);
                            cur_node = cur_node->next;
                            i++;
                        }
						break;
                      }
	 	 		} 
	 	 	}
	 	} 
	return sql;
	}
	if(tok1 == DATABASE){
        if(yylex()==NAME){ 
            sql = (void *)malloc(sizeof (char)*(STMTYPE+DTNAME));
            *(sql) = CDB;
            *(int32_t *)(sql+1) = yyname;
			//printf("创建的数据库为 %d  %s\n",*(sql),(char *)(*(int32_t *)(sql+1)));
		 	return sql;
        }  
	} 
}
/*
	drop 语句，识别drop table tbname
 */
void * dropStm(){
	char * sql;
	int tok1 = yylex();
    if(tok1 == TABLE){
        if (yylex()==NAME){
            sql = (char *)malloc(sizeof(STMTYPE+DTNAME));
            *(sql)=DTB;
            *(int32_t *)(sql+1) = (int32_t)yyname;
			//printf("删除表 %d %s\n",*(sql),(char *)(*(int32_t *)(sql + 1)));
	 		return sql;
	 	}
	}
    if(tok1 == DATABASE){
	 	if(yylex() == NAME){
            sql = (char *)malloc(sizeof(STMTYPE + DTNAME));
			*(sql)=DDB;
			*(int32_t *)(sql+1) = (int32_t)yyname;
			//printf("删除数据库%d %s\n",*(sql),(char *)(*(int32_t *)(sql + 1)));
             return sql;
        }
	} 
}

/*
	select 语句，识别select * 和select filed1,field 2 ...form 语句
 */

void * selectStm(){
	char *sql;
	int tok1 = yylex();
    if(tok1 == STAR){
        if(yylex() == FROM) {
            if(yylex() == NAME){
                char * tbname = (char *)malloc(sizeof(char) * (yylen + 1));
                strcpy(tbname,yyname);
                sql = (char *)malloc(sizeof(char)*(STMTYPE+DTNAME+ATTRNUM+WHEREP));
                *(sql) = SEL;
                *(int32_t *)(sql + 1) = tbname;
                *(int16_t *)(sql + 5) = 0;   //属性个数为0
				//printf("选择语句 %d ,%s 属性个数%d ,\n"，*(sql),(char *)(*(int *)(sql +1)),*(sql + 5));
				//printf("select 语句 %d ,表名 %s , 属性个数 %d \n",*(sql),(char *)(*(int32_t *)(sql +1)),*(int16_t *)(sql+5));
                if(yylex() == WHERE){
                    *(int32_t *)(sql + 7) = whereCompile(); //以整数存指针，将来解析用指针解读这个32位整数
				//printf("where 子句执行完毕\n");
                    return sql;
			 	}
			} 
         } 
    }
	if(tok1 == NAME){
        int16_t tok2;
        Att_head * att_head = (Att_head *)malloc(sizeof(Att_head));
        att_head->next = NULL;
		att_head->end = NULL;
		int16_t n = 0;
		//printf("进入select \n");
		while(tok1 == NAME){
	 		n++;
			Att_node * att_node = (Att_node *)malloc(sizeof(Att_node));
			att_node->next = NULL;
			strcpy(att_node->str,yyname);
			//printf("识别了%s \n",yyname);
			if(att_head->next == NULL){
	 			att_head->next = att_node;
				att_head->end = att_node;
			}   
			else{
	 			att_head->end->next = att_node;
				att_head->end = att_node;
			}   
			tok2 = yylex();
			if(tok2 == COMMA) {
	 			tok1 = yylex();
				continue;
			}
			else break;
		}   
		sql = (char *)malloc(sizeof(char)*(STMTYPE+DTNAME+ATTRNUM+ATTRLEN*n+WHEREP));
		*(sql) = SEL;
		if(tok2 == FROM){
		 	if(yylex() == NAME){
		 		char * tbname = (char *)malloc(sizeof(char)*(yylen + 1));
				strcpy(tbname,yyname);
			//	printf("tbname = %s \n",tbname);
		 		*(int32_t *)(sql + 1) = tbname;
				*(int16_t *)(sql + 5) = n;
				Att_node * cur_node = att_head->next;
				int16_t i = 0;
				while(cur_node != NULL){
					*(sql + 7 + i*ATTRLEN)=cur_node;
			//		printf("属性 %s \n",cur_node->str);
		 			cur_node = cur_node->next;
				}
				*(char *)(sql +7 + n*ATTRLEN) = 0 ; 
				if(yylex() == WHERE){
			 		*(char *)(sql + 7 + n*ATTRLEN ) = whereCompile();
				}   
			}   
		} 
	}  
}

void * insertStm(){
	char  * sql;
	if(yylex() == INTO){
		if(yylex() == NAME){
		  	char * tbname = (char *)malloc(sizeof(char)*(yylen+1));
			strcpy(tbname,yyname);
		//	printf("tbname = %s \n",tbname);
		 	int16_t tok2 = yylex();
			int16_t tok5;
			int16_t n = 0; //insert中属性的个数
			Att_head * att_head = (Att_head *)malloc(sizeof(Att_head));
			att_head->next = NULL;
            att_head->end = NULL; 
			if(tok2 == LEFTBRAC){
				int16_t tok3 = yylex();
				while(tok3 == NAME){
					n++;
					Att_node * att_node = (Att_node *)malloc(sizeof(Att_node));
					strcpy(att_node->str,yyname);
					att_node->next = NULL;
					if(att_head->next == NULL){
		 				att_head->next = att_node;
						att_head->end = att_node;
					}
					else{
		 				att_head->end->next = att_node;
						att_head->end = att_node;
					}
					int16_t tok4 = yylex();
					if(tok4 == COMMA){
						tok3 = yylex();
		 				continue;
					}
					else if(tok4 == RIGHTBRAC) {
		 				tok5 = yylex();
						break;
		  		 	}
		  		} 
			}
			int16_t m = 0;
			if(tok2 == VALUES || tok5 == VALUES){
		 		if(yylex() == LEFTBRAC){
		 			int tok6 = yylex();
					Va_head * va_head = (Va_head *)malloc(sizeof(Va_head));
					va_head->next = NULL;
					va_head->end = NULL;
					if(tok6 == QM) tok6 = yylex();
					//printf("tok6 = %d\n",tok6);
					while(tok6 == NAME || tok6 == NUMBER){
						m++;
						Va_node * va_node = (Va_node *)malloc(sizeof(Va_node));
						va_node->next = NULL;
						if(tok6 == NUMBER){
		 					va_node->flag = ATTRINT;
							va_node->vint = yynum;
							//printf("yynum = %d \n",yynum);
						} 
						if(tok6 == NAME){
		 					va_node->flag = ATTRCHAR;
							va_node->vchar = yyname;
							//printf("yyvcahr = %s\n",va_node->vchar);
							if(yylex() == QM); 		
						}
						if(va_head->next == NULL){
							va_head->next = va_node;
							va_head->end = va_node;
						}
						else{ 
							va_head->end->next =  va_node;
							va_head->end = va_node;
						} 

						int16_t tok7 = yylex();
						//printf("tok7 = %d \n",tok7);
						if(tok7 == COMMA) {
							tok6 = yylex();
							if(tok6 == QM) tok6 = yylex();
							//printf("tok6 = %d \n",tok6);
							continue; 
						}
						else if(tok7 == RIGHTBRAC){
							//printf("识别之后构建sql");
							sql = (char *)malloc(sizeof(char)*(STMTYPE+DTNAME+ATTRNUM+ATTRLEN*n+VALUENUM+VALUEP*m));          
							*(sql) = INS;
							*(int32_t *)(sql+1) = tbname;
							//printf("sql 语句 %d tbname %s \n",*(sql),(char *)(*(int *)(sql+1)));
							*(int16_t *)(sql+5) = n;
							//printf("属性个数 %d\n",*(int16_t *)(sql+5));
							int16_t i=0;
							
							Att_node * cur_att_node = att_head->next;
							while(cur_att_node != NULL){
								*(int32_t *)(sql+7+i*ATTRLEN) = (int)cur_att_node;
								cur_att_node = cur_att_node->next;
								//p rintf("属性值 %s \n",(char *)(*( int *)(sql + 7+i*ATTRLEN)));
							}
							//保存value 的值
							*(int16_t *)(sql + 7 + n*ATTRLEN) = m;
							//printf("values 个数 %d \n",*(int16_t *)(sql + 7 +n*ATTRLEN));
							Va_node * cur_va_node = va_head->next;
							int16_t j = 0;      
							while(cur_va_node != NULL){     
								*(int32_t  *)(sql+7+n*ATTRLEN + 2 + j*VALUEP) = cur_va_node;
								cur_va_node = cur_va_node->next;
							}
							return sql;
							break; 
		 				}
					}
				}
			}
		} 
	}
}
/*
	识别简单的delete语句
 */
void * deleteStm(){
	char * sql;
	if(yylex() == FROM){
	 	if(yylex() == NAME) {
	 		char * tbname = (char *)malloc(sizeof(char)*(yylen+1));
			strcpy(tbname,yyname);
			sql = (char*)malloc(sizeof(char)*(STMTYPE+DTNAME+WHEREP));  
			*(sql) = DEL;
			*(int32_t *)(sql + 1) = tbname;
			//printf("删除语句 %d %s \n",*(sql),(char *)(*(int *)(sql+1)));
			if(yylex() == WHERE){
				*(int32_t *)(sql + 5) = whereCompile();
	 	 		return sql;
		 	}
		 }
	  } 
}
   
/*
	 识别简单update语句
 */
void *  updateStm(){
	char * sql;
	if( yylex()  ==  NAME){
		char * tbname = malloc(sizeof(char) *( yylen + 1)) ;
		strcpy(tbname, yyname);
		if(yylex() == SET){
	 		Att_head * att_head = (Att_head *)malloc(sizeof(Att_head));
 	 		att_head->next = NULL;
			att_head->end = NULL;
			Va_head * va_head = (Va_head *)malloc(sizeof(Va_head));
			va_head->next = NULL;
			va_head->end = NULL; 
			int32_t tok1 = yylex();
			int16_t n = 0;
			//printf("进入update : \n");
			while(tok1 == NAME){
 				n++;
				Att_node * att_node = (Att_node *)malloc(sizeof(Att_node));
				strcpy(att_node->str,yyname);
				att_node->next = NULL;
				if(att_head->next == NULL){
		 			att_head->next = att_node;
	 				att_head->end = att_node;
				}
				else {
	 	 			att_head->end->next = att_node;
					att_head->end = att_node;
				}
				//printf("识别等号、\n");
				if(yylex() == EQ){
	 	 			Va_node * va_node = (Va_node *)malloc(sizeof(Va_node));
					va_node->next = NULL;
					int32_t tok2 = yylex();
					//printf("tok2 = %d \n",tok2);
					if(tok2 == QM) tok2 = yylex();
					//printf("tok2 new = %d\n",tok2);
					if(tok2 == NUMBER){
	 	 				va_node->flag = ATTRINT;					
						va_node->vint = yynum;
				//		printf("等号右边数字 %d \n"，yynum);
	 					//printf(" = youbian = %d \n",yynum);
					}
					if(tok2 == NAME){
	 	 			 	va_node->flag = ATTRCHAR;
						va_node->vchar = yyname;
						//printf("等号右边字符 %s \n",yyname);
					}
					if(va_head->next == NULL){
	 	 			 	va_head->next = va_node;
						va_head->end = va_node;
						//printf("va_head->next->vint = %d \n",va_node->vint);

					}
					else{
						va_head->end->next = va_node;
	 					va_head->end = va_node;
		 			 }
				}
				tok1 = yylex();
				while(tok1 == QM || tok1 == COMMA) tok1 = yylex();
				if(tok1 == NAME ) continue ;
				if(tok1 == WHERE){
					sql = (char *)malloc(sizeof(char)*(STMTYPE + DTNAME +ATTRNUM +2*n*ATTRLEN + WHEREP));
					*(sql) = UPD;
					*(int32_t *)(sql + 1) = tbname;
					*(int16_t *)(sql + 5) = n;
					Att_node * cur_att_node = att_head->next;
					int16_t i = 0;
	 				while(cur_att_node != NULL){
						*(int32_t  *)(sql + 7 + i*ATTRLEN) = cur_att_node;
						cur_att_node = cur_att_node->next;
		 				i++;
					}
					Va_node * cur_va_node = va_head->next;
					i = 0;
					while(cur_va_node != NULL){
						*(int32_t *)(sql +7 +n*ATTRLEN + i*ATTRLEN) = cur_va_node;
						cur_va_node = cur_va_node->next;
		 		 		i++; 
					}
		  			*(int32_t *)(sql + STMTYPE + DTNAME + ATTRNUM + 2*n*ATTRLEN) = whereCompile();
					return sql;
	 				break;
				} 
	  	 		break;
	 		} 
 	 	}
	} 
}

/*
	识别简单open 数据库语句
 */
void * openStm(){
	char * sql;
	if(yylex() == NAME){
		sql = (char *)malloc(sizeof(char)*(STMTYPE + DTNAME));
		*(sql) = OPEND;
		*(int32_t *)(sql + 1) = yyname;
	 	return sql;
	} 
}

/*
	返回一个void *sql指针，指向识别之后的内存片
 */
void * summerCompile(){
 	void * sql;
	switch(yylex()){
 		case CREATE:{
	 		sql = createStm();
			 break;
		}
		case DROP :{
			sql = dropStm();	
		 	break;
		}
		case SELECT :{
 			sql = selectStm();	
			break;
		}
		case INSERT :{
 			sql = insertStm();	
			break;
		}
		case DELETE : {
 			sql = deleteStm();
			break; 
		}
		case UPDATE :{
			sql = updateStm();
	 		break;
		}
		case OPEN :{
			sql = openStm();
			break;
		}
		default : {
		 	printf("不支持这种语句;\n");	
			break;
	  	} 
	 } 
	return sql;  
}


