#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "memeryPoolInt.h"

int main(){
	srand((int) time(0));
	int k;
        allocMemeryPool();
	void *memp;
	int i=0;
	for(i=0;i<30;i++){
		k = rand()%10;
		printf("THE %dth time allloc   k=%d \n",i,k);
		allocPage(k,memp); //init uHead & freeArray
		printMemeryAlloc();
		printf("\n$$$$$$$$$$$$$$$$$$$\n");
	}
	freeMemeryPool();
}
	
