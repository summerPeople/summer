#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

/*
 * some nums for read config info
 */

#define LINE_STR_LEN 50
#define KEY_STR_LEN 20
#define VAL_STR_LEN 20


/*
 * all of config info here
 */
typedef struct Config_Info{
	int page_size;
	int mem_page_num;
}Config_Info;


#endif
