// ***************************************************************
//  CUtility   version:  1.0  date: 03/19/2007
//  -------------------------------------------------------------
//  
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
//  Author - Charry Wong
// ***************************************************************

#ifndef __UTILITY_H__
#define __UTILITY_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CUtility  
{
public:
	void SlideWindow (HWND hWnd, LPRECT prc);
	CUtility();
	virtual ~CUtility();

private:
	int m_iSlideHide;
	int m_iSlideShow;
};

#endif // !defined(AFX_UTILITY_H__AB5CEE03_A955_11D1_B258_006097960BB7__INCLUDED_)
