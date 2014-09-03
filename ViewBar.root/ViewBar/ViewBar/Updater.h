#pragma once

class CUpdater
{
public:
	CUpdater(void);
public:
	~CUpdater(void);

	BOOL AutoUpdate();
	
	BOOL CheckStatus(CString &strLatestVer);

	void CallUpdater(BOOL bAutoDown, BOOL bAutoInstall);


private:

	// check whether current version expired or not and get latest version.
	

	// get the parameter of server performance and check the status.
	BOOL CheckPerformance();

	BOOL GetVersion(CString &out);
	
	BOOL AopB(CString A, CString op, CString B);



};
