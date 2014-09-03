/*
* CFavorite head file.
* 
* Author: Bruce Shao, bruce_shao@amaxgs.com.
*
* Copyright: AMAX GLOBAL SERVICE.
*/
#pragma once
#include <afxcmn.h>

#include "..\\VBObject.h"
using namespace viewbar;

#define FT_NONE   0X0000
#define FT_FOLDER 0x0001
#define FT_FILE   0x0002

#define MAXNODE   1000

enum BROWSERTYPE {NONE=0, IEXPLORE, FIREFOX, NETSCAPE};
enum NODETYPE {NONT=0,DL,DT,DD,LD};
typedef struct tagFavoriteTreeList
{
	int		m_dwSize;
	DWORD	m_FAVOType;
	TCHAR	m_strFavoTitle[256];
	TCHAR m_strFavoURL[1024];
	struct tagFavoriteTreeList *m_pParent;
	struct tagFavoriteTreeList *m_pChild;
	struct tagFavoriteTreeList *m_pNext;
	struct tagFavoriteTreeList *m_pPrev;
}FAVORITETREELIST, *PFAVORITETREELIST;

typedef struct _node
{
	NODETYPE nt;
	CString url;
	CString title;
	struct _node *childNode;
	struct _node *nextNode;
}NODE, *PNODE;

typedef struct _stack
{
	int count; // index-one.
	int pos;   // index-zero.
	int bot;
	NODE arrNode[MAXNODE];
} STACK, PSTACK;

class CFavorite:public CVBObject
{
private:
	STACK _bmStack ; // this stack is just for firefox.
public:
	CFavorite(void);
	CFavorite(CTreeCtrl* iTreeCtrl);

	virtual bool communicate();

public:
	~CFavorite(void);
	
	//stack operation.
public:
	bool reversePop(NODE &node);
	bool push(NODE &node);

public:
	bool reset();
	bool chawDTDL(TCHAR *cBuf);
	bool ejectToTree(CTreeCtrl* iTreeCtrl);

	// get the default bowser type, currently, it support IE, FireFox, Netscape
	bool getBrowserType(BROWSERTYPE &bType);
	bool getFFSectionCount(TCHAR *iniFile, TCHAR *endSection, int endLen, TCHAR *defaultSection, int defLen, int &sCount);

	//get ie favorite path or firefox bookmark path.
	bool getFavoritePath(TCHAR ofPath[1024]);

	bool getFavoriteFromDefaultBrowser(CTreeCtrl* iTreeCtrl);
	void getFavoriteFromIE(CTreeCtrl* iTreeCtrl);
	BOOL getFavoriteFromFireFox(CTreeCtrl* iTreeCtrl);
	
	void getFavUrl(CStringArray &arrayUrl);

	void GetFavoriterFromIEOrder(CTreeCtrl* iTreeCtrl);

private:
	// create ie favorite tree.
	bool createFavoriteTree(HTREEITEM hParent, TCHAR parPath[1024],CTreeCtrl* iTreeCtrl);

	// create firefox bookmark tree.
	bool createFireFoxTree(HTREEITEM hParent, TCHAR *filename,CTreeCtrl* iTreeCtrl);

	bool createFavoriteMenu(PFAVORITETREELIST pCurTreeList, 
							PFAVORITETREELIST pParentTreeList,
							CString parPath);
	

private:

	bool replaceString(TCHAR *source, TCHAR *oldStr, TCHAR *newStr);
	bool deleteAtrribute(TCHAR *srcdes, TCHAR *attribute);
	bool deleteTag(TCHAR *srcdes, TCHAR * tag);
	bool deleteDDContent(TCHAR *srcdes);

public:
	CStringArray		 m_favString;		// store url data.
private:
	BOOL CreateFavoriteTreeOrder(HTREEITEM hParent, TCHAR *regKey, CString &strFavFilePath,CTreeCtrl* iTreeCtrl);
	CString GetFavoriteName(int codepage, LPBYTE lpData);
	CString GetFavoriteFilePath();
};
