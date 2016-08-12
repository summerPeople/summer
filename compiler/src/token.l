%option noyywrap
%{
	#include "compile.h"
	#include <stdlib.h>
	#include <string.h>
%}

%%
"create"|"CREATE"	 {return CREATE;}		
"table"|"TABLE"		 {return TABLE;}
"database"|"DATABASE"   {return DATABASE;}
"drop"|"DROP"		 {return DROP;}
"select"|"SELECT"	 {return SELECT;}
"from"|"FROM"		 {return FROM;}
"where"|"WHERE"		 {return WHERE;}
"insert"|"INSERT"	 {return INSERT;}
"into"|"INTO"		 {return INTO;}
[(]					 {return LEFTBRAC;}
[)]					 {return RIGHTBRAC;}
"values"|"VALUES"	 {return VALUES;}
"delete"|"DELETE"	 {return DELETE;}
"update"|"UPDATE"	 {return UPDATE;}
"set"|"SET"			 {return SET;}
"int"|"INT"			 {return INT;}
"char"|"CHAR"		 {return CHAR;}
"varchar"|"VARCHAR"	 {return VARCHAR;}
"not null"|"NOT NULL" {return NOTNULL;}
"primary key"|"PRIMARY KEY" {return PRIMARYKEY;}
[a-zA-Z]+	 {yyname = (char *)malloc(sizeof(char)*(yyleng+1)); strcpy(yyname,yytext); return NAME;}
[0-9]+		 {yynum=atoi(yytext); return NUMBER;}
[,]			 {return COMMA;}
[;]			 {return EOF;}
[ \t]		 {/*忽略空白字符*/}
[*]			 {return STAR;} 
.			 {printf("error\n");}

%%

