#include<stdbool.h>
void summerSqlAnalyse(void *sql, unsigned char **_out_type, void **_out_parameter);
void summerSqlExecute(unsigned char type, void *parameters);
bool createDatabase(char *dbname);
bool createTable(char *tname, tuple *new_tuple);
void summerSqlFree(void *sql);
