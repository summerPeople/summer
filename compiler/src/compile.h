#define STMTYPE 1 //y
#define DTNAME  4 //数据库或者表的名字的指针

enum token_type{
	EOL = 0,
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
	ERROR = 99
};

int yynum;
int yylen;
char * yyname;
