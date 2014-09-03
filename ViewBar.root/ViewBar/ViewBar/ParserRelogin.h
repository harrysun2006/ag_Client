#pragma once

namespace http
{

	class CParserRelogin
	{
	public:
		CParserRelogin(void);
	public:
		~CParserRelogin(void);
		CParserRelogin(const CString&xml);
		BOOL IsLogin(CString &reason);

	private:
		void Parse();

		CString m_xml;
		BOOL m_bLoginSuccess;
	};
}
