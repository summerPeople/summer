#include"../pager/fileop.h"
#include"schemeInfo.h"
#include"jiexi.h"
#include<stdlib.h>

FILE *fp;
int main()
{
	void *p = page();
	analyse(p);
	fp = fopen("a.hot", "wb");
	summerPagerAddNewPage(p);
	fclose(fp);
	fopen("a.hot", "rb");
	void *p_new = pageMalloc();
    int res = summerPagerRead(0, p_new);
	fclose(fp);
	printf("%d\n", res);
	analyse(p_new);
	return 0;
}
