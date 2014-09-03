#include "StdAfx.h"
#include "Updater.h"
#include "utility/XmlNodeWrapper.h"
#include "XAutoUPdate.h"
#include "MyUtility.h"
#include "ViewBarDlg.h"

using namespace utility;
using namespace viewbar;
using namespace http;


CUpdater::CUpdater(void)
{
}

CUpdater::~CUpdater(void)
{

}


BOOL CUpdater::AutoUpdate()
{
	CString latestVer;

	if (CheckStatus(latestVer))	// start to download.
	{
		if (latestVer == thePrefs.GetVersion())
		{
			return FALSE;
		}

		if (!CheckPerformance())
		{
			return FALSE;
		}	

		if (thePrefs.GetUpdateFlag() == CPreferences::UPDATE_AUTO)
		{
			CallUpdater(TRUE, TRUE);
		}
		else
		if (thePrefs.GetUpdateFlag() == CPreferences::UPDATE_SEMIAUTO)
		{
			CallUpdater(TRUE, FALSE);
		}
		else
		if (thePrefs.GetUpdateFlag() == CPreferences::UPDATE_MANNUALLY)
		{
			// not necessarily to call updater
			// CallUpdater(FALSE, FALSE);
		}
		

		return TRUE;

	}

	return FALSE;
}


BOOL CUpdater::CheckStatus(CString &strLatestVer)
{
	CXAutoUPdate au;
	CString	xmlin, xmlout;
	GetVersion(xmlin);
	xmlin = _T("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>   <viewbarReq viewbarId=\"") + xmlin + _T("\" />");
	au.fetchRequest(_T("viewbarCheckService"), xmlin, xmlout); //CHECKSERVICE,

	/* 
	*  <?xml version="1.0" encoding="UTF-8" ?> 
	*  <latestViewbar result="7836485" latestViewbarId="w98.1.1.0.1" forceUpdate="FALSE" /> 
	*/

	CXmlDocumentWrapper doc;
	doc.LoadXML(xmlout);
	CXmlNodeWrapper node;
	node = doc.AsNode();

	CString result;
	result = node.GetValue(_T("result"));
	strLatestVer = node.GetValue(_T("latestViewbarId"));
	CString forceUpdate;
	forceUpdate = node.GetValue(_T("forceUpdate"));

	if (forceUpdate.CompareNoCase(_T("TRUE")) == 0)
	{
		thePrefs.SetForceUpdate(true);
	}

	if (result != _VB_SUCCESS)
		return FALSE;

	// force user to quit viewbar
	// no new version available in server
	if ( forceUpdate.CompareNoCase(_T("TRUE")) == 0 && thePrefs.GetVersion() == strLatestVer)
	{
		// post message
		::PostMessage(g_MainDlg->GetSafeHwnd(), WM_VB_FORCE_QUIT, 0, 0);

		return FALSE;
	}

	// forceUpdate == true, call updater.exe and quit
	if (forceUpdate.CompareNoCase(_T("TRUE")) == 0)
	{
		CallUpdater(TRUE, TRUE);
		::PostMessage(g_MainDlg->GetSafeHwnd(), WM_VB_FORCE_QUIT, 0, 0);

		return FALSE;
	}

	if ( (forceUpdate.CompareNoCase(_T("FALSE")) == 0 ) && xmlin == strLatestVer)
		return FALSE;

	return TRUE;
}

BOOL CUpdater::GetVersion(CString &out)
{
	out = thePrefs.GetVersion();

	return TRUE;
}

/*
*<array-list>
*	<VBPerformanceRes name="cpu" value="wwww" currentValue="aa"
*		operator="sss" />
*	<VBPerformanceRes name="mem" value="wwww" currentValue="aa"
*		operator="sss" />
*	<VBPerformanceRes name="EE" value="wwww" currentValue="aa"
*		operator="sss" />
*</array-list>

*/

BOOL CUpdater::CheckPerformance()
{
	CString xmlin, xmlout;
	xmlin = _T("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>");
	CXAutoUPdate au;
	au.fetchRequest(_T("performanceService"), xmlin, xmlout);

	/*
	<?xml version="1.0" encoding="UTF-8"?>
	<vbPerformanceListRes result="4444" nextTime="38838383">
	<performance name="EE" value="wwww" currentValue="aa" operator="sss" />
	</vbPerformanceListRes>
	*/

	CXmlDocumentWrapper doc;
	doc.LoadXML(xmlout);

	CXmlNodeWrapper root;
	root = doc.AsNode();
	CString result;
	result = root.GetValue(_T("result"));  //success: 100000.

	if ( result.CompareNoCase(_VB_SUCCESS) != 0 )
	{
		return FALSE;
	}

	CString nexttime;
	nexttime = root.GetValue(_T("nextTime"));

	CXmlNodeListWrapper nodes(root.FindNodes(_T("performance")));

	for (int i=0; i<nodes.Count(); ++i)
	{
		CXmlNodeWrapper node = nodes.Node(i);

		CString aval, cval, oper;
		aval = node.GetValue(_T("value"));
		cval = node.GetValue(_T("currentValue"));
		oper = node.GetValue(_T("operator"));

		if(!AopB(cval, oper, aval))
		{
			return FALSE;
		}
	}

	return TRUE;
}

/*
*01: more than
*02: less than
*03: not more than
*04: not less than
*05: equal
*/
BOOL CUpdater::AopB(CString A, CString op, CString B)
{
	BOOL bRet = TRUE;
	long a, b, iop;

	a  = _ttol(A.GetBuffer(0));
	b  = _ttol(B.GetBuffer(0));
	iop = _ttol(op.GetBuffer(0));

	switch(iop)
	{
	case 1:
		if (a<=b)
			bRet = FALSE;break;
	case 2:
		if (a>=b)
			bRet = FALSE;break;
	case 3:
		if (a>b)
			bRet = FALSE;break;
	case 4:
		if (a<b)
			bRet = FALSE;break;
	case 5:
		if (a!=b)
			bRet = FALSE;break;
	}

	return bRet;
}


void CUpdater::CallUpdater(BOOL bAutoDown, BOOL bAutoInstall)
{
	CString strTool;
	strTool = CMyUtility::GetCurDir() + _T("\\Updater.exe");
	CString param;

	CXmlDocumentWrapper xmlDoc;

	try
	{
		xmlDoc.LoadXML(_T("<foo />"));
		CXmlNodeWrapper node(xmlDoc.AsNode());
		node.SetValue(_T("version"), thePrefs.GetVersion());
		node.SetValue(_T("proxyIp"), thePrefs.GetProxyIp());
		node.SetValue(_T("proxyPort"), thePrefs.GetProxyPort());
		node.SetValue(_T("WS"), thePrefs.GetWebServiceURL());
		node.SetValue(_T("WSPath"), thePrefs.GetWebServicePath());

		param.Format(_T("%d.%d"), bAutoDown, bAutoInstall);

		node.SetValue(_T("update"), param);

		CString out = node.GetXML();
		out.Replace('"', '\'');

		CString quoteOut;
		quoteOut.Format(_T("\"%s\""), out);

		g_log.NewLine();
		g_log.WriteLog(out);

		int iShow = SW_HIDE;
		if (!bAutoDown && !bAutoInstall)
		{
			iShow = SW_SHOW;
		}

		ShellExecute(NULL, _T("open"), strTool, quoteOut, NULL, iShow);
	}
	catch (_com_error e)
	{
		// Failed to call updater.exe
	}
}