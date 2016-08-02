#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define PAGESIZE 4096
#define COLUMNITEM 87
#define TABLEITEM 68
#define HEAD 32

void  analyse(void *p){
	int tbOffset = *((int *)p + 1);
	int i=0;
	char *s=(char *)malloc(64*sizeof(char));
	char *s4=(char *)malloc(4*sizeof(char));
	char *s1=(char *)malloc(sizeof(char));
	int startp,endp;
	char *tbName;
	if(tbOffset > 0){
		int n=(tbOffset - HEAD )/TABLEITEM;
		while(n>=1){	
			tbName = (char *)malloc(64 * sizeof(char));
			strcpy(tbName, p + HEAD+TABLEITEM*i);
			printf("%s\n",tbName);
			startp = *(int *)(p+HEAD+64+TABLEITEM*i); 
			if(n>1)   endp = *(int *)(p+HEAD+64+TABLEITEM*(i+1));
			else endp = *(int *)(p+8);
			while (endp< startp){
				strcpy(s,p+startp);
				printf("%s\t",s);
				strcpy(s4,p+startp+64);
				printf("%s\t",s4);
				short int len = *(short int *)(p+startp+68);
				printf("%d\t",len);
				strcpy(s4,p+startp+70);
				printf("%s\t",s4);
				strcpy(s1,p+startp+74);
				printf("%s\t",s1);
				startp-=COLUMNITEM;
				printf("\n*********************\n");
 		  		}
			printf("\n\n#######################\n\n");
		        n--;
			i++;
			}	
		}	
}
<<<<<<< HEAD
=======

int main(){
	void *mp;
	analyse(mp);
}
>>>>>>> 8352ded2d8015cc3143a22ed2980288d33d28ff0
