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
	pager.getPage = allocMemPoolPage;
	pager.getMemPage = getMemPage;
	pager.writePage = summerPagerWrite;

	pager.openDbFile = summerPagerOpenDbFile;
	pager.createDbFile = summerPagerCreateDbFile;
	pager.deleteDbFile = summerPagerDeleteDbFile;

	pager.getDbFileHead = getFileHead;
	pager.getPageHead = getDataPageHead;

	pager.getNewPage0 = initFileHead;
	pager.getNewSpecPage = initSpecTablePageHead;
	pager.getDataPage = initDataPageHead;

	pager.insertTuple = insertTuple;
	pager.deleteTuple = deleteTuple;
	pager.getTuple = getTuple;
	//pager.getTupleKey = ;
	pager.isTupleAccordWhere = isTupleAccordWhere;

	pager.setPageModified = setMemPoolFlag;
	//pager.updateMemPage = ;
}
