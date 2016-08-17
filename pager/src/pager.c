#include "pager.h"
#include "pageInt.h"
#include "fileopInt.h"
#include "memoryPoolInt.h"
#include "btreeInt.h"

/*
 * pager object for others to use
 */
Pager pager;

/*
 * to update the mem page, because its page_space may be invalid
 */
void pagerUpdateMemPage(void* memPage_ptr){
	MemPage* page = (MemPage*)memPage_ptr;	
	void* space = pager.getPage(page->pageno);
	if(page->page_space == space){
		return;
	}
	else{
		page->page_space = space;
	}
}

/*
 * create a pager to use
 */
void summerPagerCreatePager(){
	pager.getPage = allocMemPoolPage;
	pager.getMemPage = getMemPage;
	pager.getRootMemPage = getRootPage;
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
	pager.updateMemPage = pagerUpdateMemPage;
}
