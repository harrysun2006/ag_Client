// FileVersion.h: header for the CFileVersion class.


#ifndef __FILEVERSION_H_
#define __FILEVERSION_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "..\\VBObject.h"

class CFileVersion:public CVBObject
{ 
// Construction
public: 
    CFileVersion();

// Operations	
public: 
    BOOL    Open(TCHAR *lpszModuleName);
    void    Close();

    CString QueryValue(TCHAR *lpszValueName, DWORD dwLangCharset = 0);
    CString GetFileDescription()  {return QueryValue(_T("FileDescription")); };
    CString GetFileVersion()      {return QueryValue(_T("FileVersion"));     };
    CString GetInternalName()     {return QueryValue(_T("InternalName"));    };
    CString GetCompanyName()      {return QueryValue(_T("CompanyName"));     }; 
    CString GetLegalCopyright()   {return QueryValue(_T("LegalCopyright"));  };
    CString GetOriginalFilename() {return QueryValue(_T("OriginalFilename"));};
    CString GetProductName()      {return QueryValue(_T("ProductName"));     };
    CString GetProductVersion()   {return QueryValue(_T("ProductVersion"));  };

    BOOL    GetFixedInfo(VS_FIXEDFILEINFO& vsffi);
    CString GetFixedFileVersion();
    CString GetFixedProductVersion();

// Attributes
protected:
    LPBYTE  m_lpVersionData;
    DWORD   m_dwLangCharset;

// Implementation
public:
    ~CFileVersion(); 
}; 

#endif  // __FILEVERSION_H_

