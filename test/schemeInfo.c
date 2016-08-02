#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define PAGESIZE 4096
#define TABLEITEM 68
#define COLUMNITEM 87
#define HEAD 32
void *pageMalloc()
{
	void *p = malloc(PAGESIZE * sizeof(char));
	return p;
}
void *page()
{
	void *p = NULL;
	int *p1 = NULL;
	char *p2 = NULL;
	p = pageMalloc();
	//biao tou
	p1 = (int *)p;
	*p1 = 0;
	*(p1 + 1) = TABLEITEM * 2;
	*(p1 + 2) = 4095 - 5 * COLUMNITEM;//dai ji suan

	//gou zao person biao moshi xinxi
	p2 = (char *)p + HEAD;
	memcpy(p2, "person", 6);
	p1 = (int *)(p2 + 64);
	*p1 = 4095;

	p2 = p + 4095;
	//column1
	p2 = (char *)(p2 - COLUMNITEM);
	memcpy(p2, "no", 2);
	memcpy(p2 + 64, "char", 4);
	p1 = (int *)(p2 + 80);
	*p1 = 20;
	memcpy(p2 + 82, "##", 2);
	memcpy(p2 + 86, "Y", 1);
	//cloumn2
	p2 = (char *)(p2 - COLUMNITEM);
	memcpy(p2, "name", 4);
	memcpy(p2 + 64, "char", 4);
	p1 = (int *)(p2 + 80);
	*p1 = 20;
	memcpy(p2 + 82, "##", 2);
	memcpy(p2 + 86, "N", 1);
	//column3 
	 p2 = (char *)(p2 - COLUMNITEM);
	 memcpy(p2, "age", 3);
	 memcpy(p2 + 64, "int", 3);
	 p1 = (int *)(p2 + 80);
     *p1 = sizeof(int);
	 *((short *)(p1 + 1)) = 0;
	 memcpy(p2 + 86, "N", 1);

	 //gou zao animal biao
	 p2 = (char *)(p + HEAD + TABLEITEM);
	 memcpy(p2, "animal", 6);
	 p1 = (int *)(p2 + 64);
	 *p1 = 4095 - COLUMNITEM * 3;

	 p2 = p + *p1;
	 //column1
	 p2 = p2 - COLUMNITEM;
	 memcpy(p2, "ID", 2);
	 memcpy(p2 + 64, "char", 4);
	 p1 = (int *)(p2 + 80);
	 *p1 = 20;
	 memcpy(p2 + 82, "##", 2);
	 memcpy(p2 + 86, "Y", 1);
	 //column2
	 p2 = p2 - COLUMNITEM;
	 memcpy(p2, "species", 7);
	 memcpy(p2 + 64, "char", 4);
	 p1 = (int *)(p2 + 80);
	 *p1 = 60;
	 memcpy(p2 + 82, "##", 2);
	 memcpy(p2 + 86, "Y", 1);

	 return p;

}


