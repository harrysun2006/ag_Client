#include "StdAfx.h"
#include "XSendURLTrack.h"

namespace http
{

CXSendURLTrack::CXSendURLTrack(void)
{
}

CXSendURLTrack::~CXSendURLTrack(void)
{
}


CXSendURLTrack::CXSendURLTrack(CString &xml)
{
	CBiz::FillRequest(xml);
}

CXSendURLTrack::CXSendURLTrack(std::string strxml)
{
	CBiz::FillRequest(strxml);
}

}