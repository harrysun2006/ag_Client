#pragma once

namespace http
{
	class CParserLogin
	{
	public:
		CParserLogin(const CString&xml);
	public:
		~CParserLogin(void);

		BOOL IsLogin(CString &reason);
		BOOL GetSearchEngineList(const CString &engineList);

	private:
		void Parse();

		CString m_xml;
		BOOL m_bLoginSuccess;
	};
}