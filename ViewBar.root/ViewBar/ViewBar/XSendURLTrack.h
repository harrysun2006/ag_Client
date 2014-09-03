#pragma once
#include "Biz.h"

namespace http
{
class CXSendURLTrack : public CBiz
{
	CXSendURLTrack(void);
public:
	CXSendURLTrack(CString &xml);
	CXSendURLTrack(std::string strxml);
	virtual ~CXSendURLTrack(void);
	void FillRequest(){};
};
}