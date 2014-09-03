// ***************************************************************
//  CUISubject   version:  1.0  date: 03/21/2007
//  -------------------------------------------------------------
//  Observer Design Pattern, this is a Subject object.
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
//  Author - Charry Wong
// ***************************************************************
#pragma once

#include "UIObserver.h"
#include <list>

namespace viewbar
{
	class CUISubject
	{

	private:
		CUISubject();
		virtual ~CUISubject();
		static CUISubject * m_pInstance;
		std::list<IUIObserver*> m_listAllUI;

	public:
		static CUISubject * GetInstance();
		static void FreeInstance();

	public:

		void Attatch(IUIObserver* obs);
		void Detatch(IUIObserver* obs);
		void Notify();
		void SetState();
		int GetState();

	};
};