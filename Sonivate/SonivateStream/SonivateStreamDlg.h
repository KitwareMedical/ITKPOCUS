
// SonivateStreamDlg.h : header file
//

#pragma once
#include "RFFrameDescr.h"
#include <string>
#include "igtlClientSocket.h"
#include "ExternalInterface.h"
#include <memory>

enum class LogLevel { error = 0, warning = 1, info = 2};

// CSonivateStreamDlg dialog
class CSonivateStreamDlg : public CDialogEx
{
// Construction
public:
	CSonivateStreamDlg(CWnd* pParent, std::string hostname, int port);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SONIVATESTREAM_DIALOG };
#endif

public:
	void log(LogLevel level, const std::string &msg);
	virtual void OnCancel() override;

	/// <summary>
	/// Sends IGTLink Message to server
	/// </summary>
	/// <param name="lParam">Event handle from ExternalInterface that refers to the image frame</param>
	void sendIGTLinkMessage(LPARAM lParam);

protected:
	LogLevel logSetting = LogLevel::info;
	std::string hostname;
	int port;
	igtl::ClientSocket::Pointer socket = nullptr;
	std::unique_ptr< ExternalInterface > probe = nullptr;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonLive();
	afx_msg void OnBnClickedButtonFreeze();
	afx_msg void OnBnClickedButtonCopyLog();
	afx_msg LRESULT OnImageEvent(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRFEvent(WPARAM wParam, LPARAM lParam);

	afx_msg void OnTimer(UINT_PTR nIDEvent); //periodicaly ticks the hardware.If missed
										 //for too long, stops generation of US
	afx_msg void OnSaveImage(); // saves the image bitmap. If the image is frozen additionally saves
								// the RF frame as a text file
	afx_msg void OnDestroy();
};
