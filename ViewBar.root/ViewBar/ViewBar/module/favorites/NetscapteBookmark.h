#pragma once

#include <vector>
#include <string>


class CNetscapteBookmark
{
private:
	enum ITEM_TYPE{tagDL, tagDT, tagLD};

	class Node
	{
	public:
		ITEM_TYPE type;
		CString name;
		CString url;
		size_t index;

		Node()
		{
			type	= tagDL;
			name	= _T("");
			url		= _T("");
			index	= 0;
		}
	};
	
public:	
	CNetscapteBookmark(CTreeCtrl *pTree);
	CNetscapteBookmark();
	~CNetscapteBookmark(void);
	void AttachTree(CTreeCtrl *pTree);
	CString LookUp(size_t index);
	void Parse(void);
	void GetFavUrl(CStringArray &arrayUrl);

private:
	
	BOOL LoadBookmark(void);
	BOOL HasNextElement();
	ITEM_TYPE NextElement();
	void DumpToTree(CTreeCtrl* pTreeCtrl);

	//
	// Helper function for retrieve FireFox bookmark path
	// Just for test, unsafe, you'd better rewrite these two functions
	//
	BOOL GetFFSectionCount(TCHAR *pIniFile, TCHAR *pEndSection, int endLen, TCHAR *defaultSection, int defLen, int &sCount);
	CString GetBookmarkPath();


private:
	CString m_strHtml;		// Content of bookmarks.html
	int m_iCurrentPosition;
	ITEM_TYPE m_nextItemType;
	std::vector<Node> m_stack;
	CTreeCtrl *m_pTree;
};
