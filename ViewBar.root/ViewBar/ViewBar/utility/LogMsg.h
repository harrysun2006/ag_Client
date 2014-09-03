// ***************************************************************
//  CLogMessage   version:  1.0  date: 03/21/2007
//  -------------------------------------------------------------
//  A log helper class, for debugging
//  -------------------------------------------------------------
//  Copyright (C) 2007 - All Rights Reserved
// ***************************************************************
//  Author - Charry Wong
// ***************************************************************

namespace viewbar
{

	#ifndef __LOGMSG_H__
	#define __LOGMSG_H__

	#if _MSC_VER >= 1000
	#pragma once
	#endif // _MSC_VER >= 1000

	class CLogMessage  
	{
	public:
		BOOL WriteLog(const CString& strMessage);
		BOOL WriteLog(int iMsg);
		void NewLine(void);

		void OutputDebug(CString msg);
		void OutputDebug(LPCTSTR msg);
		void OutputDebug(LPCTSTR msg,int param);

		CLogMessage();
		BOOL Open(const CString &file);
		virtual ~CLogMessage();

	private:
		CStdioFile m_LogFile;
		BOOL  m_bSuccess;
		static CString m_strPrefix;
	};

	#endif // !defined(AFX_LOGMSG_H__AB5CEE07_A955_11D1_B258_006097960BB7__INCLUDED_)
}