#include "configLoader.h"

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

/*
 * global config info here
 * it can be used in other c file by using extern
 */

Config_Info config_info;


/*
 * trim a specific string
 * warning: it can alter the string 
 */
char *
delSpace(char* in){
	char* p = in;
	char* out = NULL;

	while(*p == ' ' || *p == '\t')
		p++;

	out = p;

	while(1){
		if(*p == ' ' || *p == '\n' || *p == '\0' || *p == '\t'){
			break;
		}
		p++;
	}
	*p = '\0';

	return out;
}

/*
 * function to read config info from file
 */
int
summerComModuleLoadConfig(){
	FILE* config_fp = fopen("../sysconf/config/config.ini", "r");
	if(config_fp == NULL){
		fclose(config_fp);
		return -1;
	}
	else{
		//read a line and put it here
		char line_str[LINE_STR_LEN] = {0};
		//read a line
		while(fgets(line_str, LINE_STR_LEN, config_fp)){
			if(line_str[0] == '#' || line_str[0] == '\n'){
				continue;
			}
			
	        char key[KEY_STR_LEN] = {0};
			char val[VAL_STR_LEN] = {0};
			if(strstr(line_str, "=")){
				sscanf(line_str, "%[0-9a-zA-Z\t ]=%s", key, val);	
				char* key_str = NULL;
				char* val_str = NULL;
				if((key_str = delSpace(key)) == NULL){
					continue;
				}
				if((val_str = delSpace(val)) == NULL){
					continue;
				}
				//now, we parse the key_value
				if(strcmp(key_str, "pageSize") == 0){
					config_info.page_size = atoi(val_str);
				}
				if(strcmp(key_str, "memPageNum") == 0){
					config_info.mem_page_num = atoi(val_str);
				}
			}
		}
		fclose(config_fp);
		return 0;
	}
}
