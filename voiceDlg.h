
// voiceDlg.h : header file
//

#pragma once

#include "ServiceStt.h"

// CvoiceDlg dialog
class CvoiceDlg : public CDialogEx
{
public:
	
// Construction
public:
	void Process_STT();
	CvoiceDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VOICE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedTest();
	afx_msg void OnBnClickedCancel();
	

private:
	SttThread *service;
	STT_NEOP * stt;
	CEdit * EditMic;
	CEdit * EditApp;
	CEdit * WAV_Path;
	CFileDialog *dlgFile;
	CButton *testBtn;
	CButton * playBtn;
	CButton *StartRecordBtn;
	CButton *StopRecordBtn;
	CListBox *MICListBox;
	CListBox *APPListBox;
	CListBox *OutListBox;
	CListBox *OutListBox2;
	CString m_strPathname;
	struct SoundIo *soundio;
	std::string charToString(const char * str);
	LPWSTR getSelection(CListBox * listbox);
	
public:
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnEnChangeSelectbrowser();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnLbnSelchangeList2();
	afx_msg void OnLbnSelchangeList3();
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnBnClickedButton3();
};
