// VBFile.h header file 

#pragma once
#include "..\\vbobject.h"
using namespace viewbar;

class CVBFile :
	public CVBObject
{
public:
	CVBFile(void);
public:
	~CVBFile(void);

public:
	bool getFile(TCHAR *url, TCHAR *location);

};
