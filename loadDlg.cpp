#include "stdafx.h"
#include "voice.h"
#include "loadDlg.h"
#include "afxdialogex.h"


BEGIN_MESSAGE_MAP(CLoadDlg, CDialog)
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDOK, &CLoadDlg::OnBnClickedOk)
END_MESSAGE_MAP()
void CLoadDlg::set_STT(STT_NEOP *STT) {
	stt = STT;
}
CLoadDlg::CLoadDlg() : CDialog(IDD_ABOUTBOX)
{

}

void CLoadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, ID_SELECTBROWSER, m_EditBrowse);
}




void CLoadDlg::OnTimer(UINT_PTR nIDEvent)
{
	if ((i < 80) && !stt->isLoaded()) {
		i += 10;
		myCtrl->SetPos(i);
	}
	if (stt->isLoaded()) {
		myCtrl->SetPos(98);
		this->EndDialog(true);
	}
}


void CLoadDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	//BOOL Crt=myCtrl.Create(WS_CHILD | WS_VISIBLE | PBS_SMOOTH, CRect(50, 40, 200, 50),this, IDC_PROGRESS1);
	//if (Crt) MessageBox(L"Success CrtBarr");
	myCtrl = (CProgressCtrl *)GetDlgItem(IDC_PROGRESS1);
	SetTimer(1, 2000, NULL);
	myCtrl->SetRange(0, 100);
	myCtrl->SetPos(0);
}

void CLoadDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK();
}