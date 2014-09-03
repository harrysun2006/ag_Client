// ***************************************************************
//  LimitSingleInstance   version:  1.0  date: 03/21/2007
//  -------------------------------------------------------------
//  For single instance
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
//  Author - Charry Wong
// ***************************************************************

#ifndef LimitSingleInstance_H
#define LimitSingleInstance_H
#include <windows.h> 
namespace viewbar
{

	class CLimitSingleInstance
	{
	protected:
	  DWORD  m_dwLastError;
	  HANDLE m_hMutex;

	public:
	  CLimitSingleInstance(TCHAR *strMutexName)
	  {
		m_hMutex = CreateMutex(NULL, FALSE, strMutexName); //do early
		m_dwLastError = GetLastError(); //save for use later...
	  }
	   
	  ~CLimitSingleInstance() 
	  {
		if (m_hMutex)
		{
		   CloseHandle(m_hMutex);
		   m_hMutex = NULL;
		}
	  }

	  BOOL IsAnotherInstanceRunning() 
	  {
		return (ERROR_ALREADY_EXISTS == m_dwLastError);
	  }
	};
};
#endif

