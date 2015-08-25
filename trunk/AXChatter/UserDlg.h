// UserDlg.h : Declaration of the CUserDlg

#pragma once

#include "resource.h"       // main symbols
#include <time.h>			// time_t,localtime,...
#include <atlhost.h>
//#include <sstream>		//ostringstream
#include <atlstr.h>			// CString
#include "AXUtinity.h"		// String2TString
#include "chattertabctrl.h"
#include "atlcontrols.h"

using namespace ATLControls;


// CUserDlg

class CUserDlg : 
	public CAxDialogImpl<CUserDlg>
{
public:
	CUserDlg(HWND hwnd,unsigned long userid,CString username,CString myname=_T(""));
	~CUserDlg();

	enum { IDD = IDD_USERDLG };

BEGIN_MSG_MAP(CUserDlg)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_HANDLER(IDC_BUTTONCLOSE, BN_CLICKED, OnBnClickedButtonclose)
	COMMAND_HANDLER(IDC_BUTTONSEND, BN_CLICKED, OnBnClickedButtonsend)
	MESSAGE_HANDLER(WM_SIZE, OnSize)
	MESSAGE_HANDLER(WM_CTLCOLORDLG, OnCtlColorDlg)
	MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColorEdit)		//disabled edit doesn't send WM_CTLCOLOREDIT other than WM_CTLCOLORSTATIC according to msdn.
	MESSAGE_HANDLER(WM_DROPFILES,OnDropFiles)
	MESSAGE_HANDLER(WM_TIMER, OnTimer)
	CHAIN_MSG_MAP(CAxDialogImpl<CUserDlg>)
	//ALT_MSG_MAP(2)      //Edit control
END_MSG_MAP()

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void AppendReceivedHistory(LPCTSTR msg);
	void AppendNotification(LPCTSTR notification);
	void ProcessCurInput();

	unsigned long GetUserId() const;
	CString GetUserName() const;
	CString GetInputMsg() const;
	BOOL HandleKeydownEvent(LPMSG pMsg, HRESULT& hRet);
private:
//////////////////////////////////////////////////////////////////////////
// private methods
//////////////////////////////////////////////////////////////////////////
	void AppendHistoryMsg(LPCTSTR username,LPCTSTR msg);

	void SetInputFocus(void);
	void ClearInputMsg();
	void AdjustChildPoses(void);

	BOOL PreTranslateAccelerator(LPMSG pMsg, HRESULT& hRet)
	{
		if(pMsg->message == WM_KEYDOWN)
		{
			switch(pMsg->wParam)
			{
			case VK_LEFT:
			case VK_RIGHT:
			case VK_UP:
			case VK_DOWN:
			case VK_HOME:
			case VK_END:
			case VK_NEXT:
			case VK_PRIOR:
				hRet = S_FALSE;
				break;
			default:
				// when user trying to edit,make the input Edit focused.
				hRet = S_FALSE;
				m_editInput.SetFocus();
				return TRUE;
			}
		}
		//TODO: Add your additional accelerator handling code here
		return FALSE;
	}

	
//////////////////////////////////////////////////////////////////////////
// dialog control variables
//////////////////////////////////////////////////////////////////////////
	HWND m_wndParent;
	CEdit m_editHistory;
	CEdit m_editInput;

	HBRUSH m_hDialogBrush;
	HBRUSH m_hEditBrush;

	//for test only
	CContainedWindowT<CEdit, CWinTraitsOR<WS_BORDER | ES_AUTOHSCROLL> > m_edit;

	unsigned long m_ulUserid;
	CString m_strUserName;
	CString m_strMyName;

////////////////////////////////////////////////////////////
// Event Handlers
////////////////////////////////////////////////////////////
	LRESULT OnBnClickedButtonclose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButtonsend(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCtlColorDlg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCtlColorEdit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDropFiles(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};