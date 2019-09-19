
#pragma once

class CLoadDlg : public CDialog
{
public:
	CLoadDlg();
	void set_STT(STT_NEOP *stt);
private:
	STT_NEOP *stt;
	int i = 0;
	CProgressCtrl *myCtrl;

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnShowWindow(
		BOOL bShow,
		UINT nStatus
	);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support


// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};