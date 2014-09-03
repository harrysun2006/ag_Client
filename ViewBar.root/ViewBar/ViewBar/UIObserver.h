// ***************************************************************
//  IUIObserver   version:  1.0  date: 03/21/2007
//  -------------------------------------------------------------
//  Observer Design Pattern, this is a Observer object.
//  All UI dialog should derive from this interface and
//  implement its own UpdateUI() method
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
//  Author - Charry Wong
// ***************************************************************
#pragma once

namespace viewbar
{
	class IUIObserver
	{
	public:
		IUIObserver(void);

		virtual void UpdateUI() = 0;

	public:
		virtual ~IUIObserver(void);
	};
};
