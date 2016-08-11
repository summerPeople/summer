#include "pager.h"
#include "pageInt.h"
#include "fileopInt.h"
#include "memoryPoolInt.h"

/*
 * pager object for others to use
 */
Pager pager;

/*
 * create a pager to use
 */
void summerPagerCreatePager(){
	pager.getPage = allocMemoryPoolPage;
	pager.writePage = summerPagerWrite;
	pager.openDbFile = summerPagerOpenDbFile;
	pager.createDbFile = summerPagerCreateDbFile;

	pager.getDbFileHead = getFileHead;

	pager.getNewPage0 = initFileHead;
	pager.getNewSpecPage = initSpecTablePageHead;
	pager.getDataPage = NULL;
}
