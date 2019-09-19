#include "stdafx.h"
#include "voice.h"
#include "voiceDlg.h"
#include "loadDlg.h"
#include "afxdialogex.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BEGIN_MESSAGE_MAP(CvoiceDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCANCEL, &CvoiceDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON4, &CvoiceDlg::OnBnClickedTest)
	ON_EN_CHANGE(IDC_EDIT1, &CvoiceDlg::OnEnChangeEdit1)
	ON_EN_CHANGE(ID_SELECTBROWSER, &CvoiceDlg::OnEnChangeSelectbrowser)
	ON_BN_CLICKED(IDC_BUTTON2, &CvoiceDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, &CvoiceDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON5, &CvoiceDlg::OnBnClickedButton5)
	ON_LBN_SELCHANGE(IDC_LIST2, &CvoiceDlg::OnLbnSelchangeList2)
	ON_LBN_SELCHANGE(IDC_LIST3, &CvoiceDlg::OnLbnSelchangeList3)
	ON_LBN_SELCHANGE(IDC_LIST1, &CvoiceDlg::OnLbnSelchangeList1)
	ON_BN_CLICKED(IDC_BUTTON3, &CvoiceDlg::OnBnClickedButton3)
END_MESSAGE_MAP()




// CvoiceDlg message handlers

BOOL CvoiceDlg::OnInitDialog()
{
	EditMic = (CEdit *)GetDlgItem(IDC_EDIT3);
	EditApp = (CEdit *)GetDlgItem(IDC_EDIT4);
	stt = new STT_NEOP();

	
	
	//SttThread *service;
	service = new SttThread();
	service->set_STT(stt);

	
	//--------------Loading Barr --------------------
	CLoadDlg *loadDlg = new CLoadDlg();
	loadDlg->set_STT(stt);

	std::thread t1(&CLoadDlg::DoModal,loadDlg);
	stt->loadModal();
	t1.join();
	if (!stt->isLoaded())
	{
		MessageBox(L"Error while Loading Modal");
		this->EndDialog(true);
		return FALSE;
	}

	//-------------- Init Voice Dlg --------------------
	WAV_Path = (CEdit *)GetDlgItem(IDC_EDIT2);
	testBtn = (CButton *)GetDlgItem(IDC_BUTTON4);
	playBtn = (CButton *)GetDlgItem(IDC_BUTTON1);
	StartRecordBtn = (CButton *)GetDlgItem(IDC_BUTTON2);
	StopRecordBtn = (CButton *)GetDlgItem(IDC_BUTTON3);
	StopRecordBtn->EnableWindow(FALSE);
	WAV_Path->SetWindowTextW(L"Select WAV File ...");
	OutListBox =(CListBox *)GetDlgItem(IDC_LIST1);
	OutListBox2 =(CListBox *)GetDlgItem(IDC_LIST4);
	MICListBox=(CListBox *)GetDlgItem(IDC_LIST2);
	APPListBox=(CListBox *)GetDlgItem(IDC_LIST3);
	//----------------Init Service --------------------
	service->run();
	//----------------Init Soundio---------------------
	
	
	soundio = soundio_create();
	if (!soundio) {
		MessageBox(L"out of memory");
		return 1;
	}
	int err;
	if ((err = soundio_connect(soundio))) {
		MessageBox(L"error connecting");
	}
	soundio_flush_events(soundio);
	stt->setSoundio(soundio);
	stt->setDevice("Speaker");



	//-------------------------------------------------------
	//stt->getLineNamesSoundIo(OutListBox);
	//stt->getLineNamesSoundIo(OutListBox2);
	//stt->getLineNamesSFML( MICListBox);
	//stt->getLineNamesSFML(APPListBox);
	stt->initVoiceOutTextBox(EditMic, EditApp);
	//OutListBox->SetCurSel(1);

	OutListBox->AddString(L"Microphone");
	OutListBox->AddString(L"Speaker");
	OutListBox->AddString(L"Virtual Sound Card 1");
	OutListBox->AddString(L"Virtual Sound Card 2");

	OutListBox2->AddString(L"Microphone");
	OutListBox2->AddString(L"Speaker");
	OutListBox2->AddString(L"Virtual Sound Card 1");
	OutListBox2->AddString(L"Virtual Sound Card 2");

	MICListBox->AddString(L"Microphone");
	MICListBox->AddString(L"Speaker");
	MICListBox->AddString(L"Virtual Sound Card 1");
	MICListBox->AddString(L"Virtual Sound Card 2");

	APPListBox->AddString(L"Microphone");
	APPListBox->AddString(L"Speaker");
	APPListBox->AddString(L"Virtual Sound Card 1");
	APPListBox->AddString(L"Virtual Sound Card 2");

	//-------------------------------------------------------

	CDialogEx::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	
	SetIcon(m_hIcon, FALSE);		// Set small icon
	SetIcon(m_hIcon, TRUE);			// Set big icon
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CvoiceDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CvoiceDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// CvoiceDlg dialog
void CvoiceDlg::Process_STT()
{
	//std::thread t1(&STT_NEOP::multiProcess,stt);

}
CvoiceDlg::CvoiceDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_VOICE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CvoiceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


void CvoiceDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}

void CvoiceDlg::OnEnChangeEdit1() 
{

}

void CvoiceDlg::OnBnClickedTest()
{
	EditMic->SetWindowTextW(L" ");
	EditApp->SetWindowTextW(L" ");
}
std::string CvoiceDlg::charToString(const char * ch) {
	std::string str(ch);
	return str;
}
LPWSTR CvoiceDlg::getSelection(CListBox * listbox) {
	LPWSTR szTemp = L" ";
	CString str, str2;
	int selectedItemID = listbox->GetCurSel();
	if (selectedItemID > -1) {
		int n = listbox->GetTextLen(selectedItemID);
		listbox->GetText(selectedItemID, str.GetBuffer(n));
		str.ReleaseBuffer();
		str2.Format(_T("%s"), str.GetBuffer(0));
		szTemp = (LPWSTR)str2.GetBuffer();
		return szTemp;
	}
	
	return szTemp;
}

void CvoiceDlg::OnEnChangeSelectbrowser()
{
	
}





void CvoiceDlg::OnBnClickedButton1()
{
	//MessageBox(L"Hi! It's not yet configured ;)");
	
	std::string name("sample.wav");
	const char * n_ame = name.c_str();
	SNDFILE *infile = NULL;
	SF_INFO sfinfo;
	if ((infile = sf_open(n_ame, SFM_READ, &sfinfo)) == NULL) MessageBox(L"Error");
	PassThrough *pass = new PassThrough;
	pass->infile = infile;
	pass->sfinfo = sfinfo;
	stt->outstream->userdata = static_cast<void *>(pass);
	
}


void CvoiceDlg::OnBnClickedButton5()
{
	CFileDialog fileDlg(TRUE, L"WAV", L".wav", OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, L"WAV File (*.WAV)|*.WAV||", this);
	if (fileDlg.DoModal() == IDOK)
	{
		m_strPathname = fileDlg.GetPathName();
		WAV_Path->SetWindowTextW(m_strPathname);
		playBtn->SetState(TRUE);
		playBtn->EnableWindow(TRUE);
	}
}


void CvoiceDlg::OnLbnSelchangeList2()
{
	// TODO: Add your control notification handler code here
	if (service->isRunning())
		MessageBox(L"Stop STT To change Input device");
	else {
		service->setDevice(stt->LPWSTRToString(getSelection(MICListBox)));
		MessageBox(stt->stringToLPCTSTR(std::string("Input Device changed to: ") + service->getDevice()));

	}
}


void CvoiceDlg::OnLbnSelchangeList3()
{
	// TODO: Add your control notification handler code here
	//MessageBox(getSelection(APPListBox));
	MessageBox(L"Hi! It's not yet configured ;)");
}

void CvoiceDlg::OnLbnSelchangeList1()
{
	// TODO: Add your control notification handler code here
	//MessageBox(getSelection(OutListBox));
	MessageBox(L"Hi! It's not yet configured ;) sss");
}
void CvoiceDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	if (!service->isRunning()) {
		service->Start();
		StopRecordBtn->SetState(TRUE);
		StopRecordBtn->EnableWindow(TRUE);
		StartRecordBtn->SetState(FALSE);
		StartRecordBtn->EnableWindow(FALSE);
	}
}

void CvoiceDlg::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here
	
	service->Stop();
	StopRecordBtn->EnableWindow(FALSE);
	StopRecordBtn->SetState(FALSE);
	StartRecordBtn->EnableWindow(TRUE);
	StartRecordBtn->SetState(TRUE);
	
}
