#pragma once

#include "AutoUpdate.h"
#include "afxcmn.h"
#include "afxwin.h"

// CVBUpdaterDlg dialog

class CVBUpdaterDlg : public CDialog
{
	DECLARE_DYNAMIC(CVBUpdaterDlg)

public:
	CVBUpdaterDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CVBUpdaterDlg();

// Dialog Data
	enum { IDD = IDD_AU };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	void reset();
	void ParseCmd();		// by Charry
	BOOL UpdateSilent();	// by Charry

	void StartDownload();


	static UINT m_ProcessMsg;
public:
	afx_msg void OnBnClickedExit();

	afx_msg LRESULT OnDownloadFinished(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnInstallFinished(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCheckPerformance(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnBeginDownload(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDownloadOne(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnBegininstall(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnInstallOne(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCheckStatus(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnProcessMsg(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedStart();
	afx_msg LRESULT OnAutoDownload(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnAutoInstall(WPARAM wParam, LPARAM lParam);

public:
	CWinThread  *m_auT;
	int m_cfc;
	CAutoUpdate m_AutoUpdate;
public:
	CProgressCtrl m_prog;
	CEdit m_edtCon;
	CButton m_btnStart;
	//CButton m_btnCancel;
	CButton m_btnExit;
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();

	
};

extern CVBUpdaterDlg *g_pMainDlg;