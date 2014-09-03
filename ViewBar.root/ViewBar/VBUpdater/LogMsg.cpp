// LogMsg.cpp: implementation of the CLogMessage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LogMsg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace viewbar
{

	CLogMessage::CLogMessage(): m_bSuccess(FALSE)
	{

	}

	BOOL CLogMessage::Open(const CString &file)
	{
		if (m_LogFile.Open (file, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
		{   
			m_bSuccess = TRUE;
		}
		else
		{
			m_bSuccess = FALSE;
		}
		return m_bSuccess;
	}

	CLogMessage::~CLogMessage()
	{
		if (m_bSuccess)
			m_LogFile.Close ();
	}

	BOOL CLogMessage::WriteLog(const CString & strMessage)
	{
		TRY
		{
			if (m_bSuccess)
			{
				m_LogFile.WriteString (strMessage);
			}
			else
			{
				throw;
			}
		}
		CATCH (CFileException, e)
		{
			return FALSE;
		}
		END_CATCH

		return TRUE;
	}

	BOOL CLogMessage::WriteLog(int iMsg)
	{
		TRY
		{
			if (m_bSuccess)
			{
				CString msg;
				msg.Format(_T("%d"), iMsg);
				m_LogFile.WriteString (msg);
			}
			else
			{
				throw;
			}
		}
		CATCH (CFileException, e)
		{
			return FALSE;
		}
		END_CATCH

		return TRUE;
	}

	void CLogMessage::NewLine()
	{
		TRY
		{
			if (m_bSuccess)
			{
				m_LogFile.WriteString (_T("\r\n"));
			}
			else
			{
				throw;
			}
		}
		CATCH (CFileException, e)
		{
			return;
		}
		END_CATCH
	}
}