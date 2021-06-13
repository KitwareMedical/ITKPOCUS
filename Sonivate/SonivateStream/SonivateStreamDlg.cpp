
// SonivateStreamDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "SonivateStream.h"
#include "SonivateStreamDlg.h"
#include "afxdialogex.h"
#include <tchar.h>

// Sonivate includes
#include "ExternalInterface.h"
#include "RFFrameDescr.h"

// OpenIGTLink includes
#include "igtlOSUtil.h"
#include "igtlImageMessage.h"
#include "igtlClientSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSonivateStreamDlg dialog



CSonivateStreamDlg::CSonivateStreamDlg(CWnd* pParent /*=nullptr*/, std::string hostname, int port)
	: CDialogEx(IDD_SONIVATESTREAM_DIALOG, pParent), hostname { hostname }, port { port }
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSonivateStreamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSonivateStreamDlg, CDialogEx)
	ON_MESSAGE(WM_USER_IMAGE_EVENT, OnImageEvent)
	ON_MESSAGE(WM_USER_RF_EVENT, OnRFEvent)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_LIVE, &CSonivateStreamDlg::OnBnClickedButtonLive)
	ON_BN_CLICKED(IDC_BUTTON_FREEZE, &CSonivateStreamDlg::OnBnClickedButtonFreeze)
	ON_BN_CLICKED(IDC_BUTTON_COPYLOG, &CSonivateStreamDlg::OnBnClickedButtonCopyLog)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void CSonivateStreamDlg::log(LogLevel level, const std::string& msg) {
	// get edit control from dialog
	if (level > logSetting) {
		return;
	}
	std::string prefix;
	switch (level) {
	case LogLevel::error:
		prefix = "LogLevel::error";
		break;
	case LogLevel::warning:
		prefix = "LogLevel::warning";
		break;
	case LogLevel::info:
		prefix = "LogLevel::info";
		break;
	}

	prefix += "\t" + msg + "\r\n";
	const TCHAR* buffer = prefix.c_str(); // this won't work if TCHAR is defined as being unicode-16


	//sizeof(TCHAR) == 1 ? prefix.c_str() : std::wstring(prefix.begin(), prefix.end()).c_str();
	/*if (sizeof(TCHAR) == 1) {
		buffer = prefix.c_str();
	}
	else {
		buffer = std::wstring()
	}*/

	CWnd* editCwnd = GetDlgItem(IDC_EDIT_LOG);

	//// get the current selection
	DWORD startPos, endPos;
	editCwnd->SendMessage(EM_GETSEL, reinterpret_cast<WPARAM>(&startPos), reinterpret_cast<WPARAM>(&endPos));

	//// move the caret to the end of the text
	int outLength = editCwnd->GetWindowTextLength();
	editCwnd->SendMessage(EM_SETSEL, outLength, outLength);

	//// insert the text at the new caret position
	editCwnd->SendMessage( EM_REPLACESEL, TRUE, reinterpret_cast<LPARAM>(buffer));

	//// restore the previous selection
	editCwnd->SendMessage(EM_SETSEL, startPos, endPos);
}



// CSonivateStreamDlg message handlers

BOOL CSonivateStreamDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	ShowWindow(1);
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	log(LogLevel::info, "Begin Initializing");
	log(LogLevel::info, "Attempting Connection: " + hostname + ":" + std::to_string(port));
	socket = igtl::ClientSocket::New();
	int r = socket->ConnectToServer(hostname.c_str(), port);
	if (r != 0) {
		log(LogLevel::error, "Could not connect to server, return code: " + std::to_string(r));
		socket = nullptr;
	}
	else {
		log(LogLevel::info, "Connection successful");
	}
	
	log(LogLevel::info, "Connecting to Sonivate Probe");
	
	probe = std::unique_ptr< ExternalInterface >(new ExternalInterface());
	probe->SetCallbackWindow((int)this->m_hWnd);
	
	if (probe->InitHardware()) {
		log(LogLevel::info, "Connection successful");
		probe->LoadPreset(2);
		//m_pMCE->LoadPreset(2);
		//m_pImgDisp = new ImgDisplay;
		//m_pImgDisp->Setup(m_pMCE->ImgWidth(), m_pMCE->ImgHeight(), m_eView.GetDC());

	}
	else {
		log(LogLevel::error, "Connection failed (make sure the probe is plugged in and powered on, and that the Cyprex driver has been installed)");
		probe = nullptr;
	}
	
	SetTimer(0, 1000, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSonivateStreamDlg::OnCancel() {
	if (socket != nullptr) {
		log(LogLevel::info, "Closing socket");
		socket->CloseSocket();
	}

	if (probe != nullptr) {
		log(LogLevel::info, "Shutting Down Sonivate Device");
		if (probe->IsRunning()) {
			probe->Stop();
		}
		probe = nullptr;
	}

	log(LogLevel::info, "Terminated");
	CDialogEx::OnCancel();
}

afx_msg LRESULT CSonivateStreamDlg::OnImageEvent(WPARAM wParam, LPARAM lParam) {
	log(LogLevel::info, "Image Event");
	if (socket != nullptr) {
		log(LogLevel::info, "Sending OpenIGTLink Message");
		sendIGTLinkMessage(lParam);
	}

	return TRUE;
}

void CSonivateStreamDlg::sendIGTLinkMessage(LPARAM lParam) {
	// sends IGTLinkMessage
	int dims[3] = { probe->ImgWidth(lParam), probe->ImgHeight(lParam), 1 };
	int offset[3] = { 0, 0, 0 };
	float spacing[3] = { probe->ImgPixWidth(lParam), probe->ImgPixHeiht(lParam), 1 };
	int numComponents = 1;
	int scalarType = igtl::ImageMessage::TYPE_UINT8;
	const char* deviceName = "Sonivate";

	// TODO: make sure to confirm this orientation
	float n1[3] = { -1.0f, 0, 0 };
	float n2[3] = { 0, -1.0f, 0 };
	float n3[3] = { 0, 0, 1.0f };
	
	auto msg = igtl::ImageMessage::New();
	msg->SetDimensions(dims);
	msg->SetSpacing(spacing);
	msg->SetNumComponents(numComponents);
	msg->SetScalarType(scalarType);
	msg->SetDeviceName(deviceName);
	
	msg->SetSubVolume(dims, offset);
	msg->SetNormals(n1, n2, n3);
	msg->AllocateScalars();

	// TODO: confirm with Sonivate the byte interpretation (i.e. literal interpration) used here for scalar values (e.g. what about gain?)
	auto buffer = (unsigned char*)msg->GetScalarPointer();
	std::memcpy(buffer, probe->ImgData(lParam), dims[0] * dims[1]);
	msg->Pack();
	socket->Send(msg->GetPackPointer(), msg->GetPackSize());
}

afx_msg LRESULT CSonivateStreamDlg::OnRFEvent(WPARAM wParam, LPARAM lParam) {
	log(LogLevel::info, "RF Event");
	return TRUE;
}

void CSonivateStreamDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSonivateStreamDlg::OnPaint()
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
HCURSOR CSonivateStreamDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSonivateStreamDlg::OnBnClickedButtonLive()
{
	// TODO: Handle this with inactivating the button instead
	if (probe != nullptr) {
		probe->Start();
	}
}

void CSonivateStreamDlg::OnBnClickedButtonFreeze()
{
	// TODO: Handle this with inactivating the button instead
	// TODO: Consider IsRunning() API checks
	if (probe != nullptr) {
		probe->Stop();
	}
}

void CSonivateStreamDlg::OnBnClickedButtonCopyLog()
{
	// TODO: Add your control notification handler code here
	log(LogLevel::info, "Copy Pressed");
	
	CEdit* editCwnd = (CEdit*)GetDlgItem(IDC_EDIT_LOG);
	int startPos, endPos;
	editCwnd->GetSel(startPos, endPos); // get original selection

	editCwnd->SetSel(0, editCwnd->GetWindowTextLength() - 1);
	editCwnd->Copy();

	editCwnd->SetSel(startPos, endPos); // reset selection
}

afx_msg void CSonivateStreamDlg::OnTimer(UINT_PTR nIDEvent) {
	log(LogLevel::info, "OnTimer");
	if (nIDEvent == 0)
	{
		log(LogLevel::info, "tick");
		probe->TickWatchDog();

	}
	CDialog::OnTimer(nIDEvent);
}

afx_msg void CSonivateStreamDlg::OnSaveImage() {

}

afx_msg void CSonivateStreamDlg::OnDestroy() {

}