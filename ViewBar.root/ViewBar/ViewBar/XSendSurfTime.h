#pragma once

#include "Biz.h"

namespace http
{

class CXSendSurfTime : public CBiz
{
	CXSendSurfTime(void);
public:	
	CXSendSurfTime(CString &xml);
	virtual ~CXSendSurfTime(void);
	void FillRequest();
//	void FillXMLFormat(CString &xml);
};
}