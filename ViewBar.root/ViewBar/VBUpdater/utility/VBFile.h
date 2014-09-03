// VBFile.h header file 

#pragma once
using namespace viewbar;

class CVBFile
{
public:
	CVBFile(void);
public:
	~CVBFile(void);
private:
	char* m_buffer;

public:
	BOOL GetFile(CString strPath, CString strFile, CString strHttp);
};
