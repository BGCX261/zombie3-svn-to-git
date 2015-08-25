// UserDlg.cpp : Implementation of CUserDlg

#include "stdafx.h"
#include <shellapi.h>	// DragQueryFile
//#include <atlwin.h>
//#include "atlcontrols.h" // DragAcceptFile
#include <config.hxx>
#include "UserDlg.h"

#pragma comment(lib,"shell32")

extern COLORREF COLOR_WCHATTER;

CUserDlg::CUserDlg(HWND hwnd,unsigned long userid,CString username,CString myname)
:m_wndParent(hwnd),
m_ulUserid(userid),
m_strUserName(username),
m_strMyName(myname)
{
	//AtlAxWinInit();

	m_hEditBrush = CreateSolidBrush(COLOR_WCHATTER);

	if (m_strMyName.IsEmpty())
		m_strMyName=_T("Me");
}

CUserDlg::~CUserDlg()
{
	//DeleteObject(m_hDialogBrush);
	DeleteObject(m_hEditBrush);
}

LRESULT CUserDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//DragAcceptFile(m_hWnd,TRUE);

	m_editHistory = GetDlgItem(IDC_EDITHISTORY);
	m_editHistory.SetReadOnly();

	m_editInput = GetDlgItem(IDC_EDITINPUT);

	AdjustChildPoses();

	//this->KillTimer(TIMER_ID);
	//this->SetTimer(TIMER_ID,VIDEO_INTERVAL,0);

	CAxDialogImpl<CUserDlg>::OnInitDialog(uMsg, wParam, lParam, bHandled);
	bHandled = TRUE;
	return 1;  // Let the system set the focus
}
void CUserDlg::AppendReceivedHistory(LPCTSTR msg)
{
	AppendHistoryMsg(m_strUserName,msg);
}
void CUserDlg::AppendNotification(LPCTSTR notification)
{
	AppendHistoryMsg(_T("¡¾Notification¡¿"),notification);
}
void CUserDlg::ProcessCurInput()
{
	AppendHistoryMsg(m_strMyName,GetInputMsg());

	ClearInputMsg();
}
unsigned long CUserDlg::GetUserId() const
{
	return m_ulUserid;
}
CString CUserDlg::GetUserName() const
{
	return m_strUserName;
}
CString CUserDlg::GetInputMsg() const
{
	int len;
	CString strText, strLine;

	int nLineCount = m_editInput.GetLineCount();

	if (nLineCount>0)
	{
		len = m_editInput.LineLength(m_editInput.LineIndex(0));
		m_editInput.GetLine(0, strLine.GetBuffer(len), len);
		strLine.ReleaseBuffer(len);
		strText = strLine;
	}

	// Dump every line of text of the edit control.
	for (int i=1;i < nLineCount;i++)
	{
		// length of line i:
		len = m_editInput.LineLength(m_editInput.LineIndex(i));
		m_editInput.GetLine(i, strLine.GetBuffer(len), len);
		strLine.ReleaseBuffer(len);
		strText += _T("\r\n") + strLine;
	}

	return strText;
}
BOOL CUserDlg::HandleKeydownEvent(LPMSG pMsg, HRESULT& hRet)
{
	BOOL bRet = FALSE;
	HWND hWnd;
	int nStartChar;
    int nEndChar;

	CString test;


	if(pMsg->message == WM_KEYDOWN)
	{
		//GetKeyState(VK_CONTROL) < 0 ? it's down : it's up
		//GetAsyncKeyStatus(VK_CONTROL);
		//if (GetAsyncKeyStatus(VK_CONTROL) && )
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
		case VK_CONTROL:
			hRet = S_FALSE;	// still need the event to be done.
			bRet = TRUE;
			break;
		case VK_RETURN:
			hWnd = GetFocus();
			if (hWnd==m_editInput.m_hWnd)
			{
				if (GetKeyState(VK_CONTROL)<0)
					::PostMessage(m_wndParent,WM_SENDMSG,m_ulUserid,0);
					//hRet = S_FALSE;
				else
				{
					m_editInput.Cut();
					m_editInput.GetSel(nStartChar,nEndChar);
					m_editInput.InsertText(nStartChar,_T("\r\n"));
					//m_editInput.SetSel(nStartChar+1,nStartChar+1);
				}
			}
			else
				m_editInput.SetFocus();

			test = GetInputMsg();
			bRet = TRUE;
			break;
		default:
			hWnd = GetFocus();
			if (hWnd != m_editInput.m_hWnd)
				m_editInput.SetFocus();	// when user trying to edit,make the input Edit focused.
			hRet = S_FALSE;		// still need the input to be done.
			//m_editInput.AppendText(pMsg->wParam);
			bRet = FALSE;
			break;
		}
	}

	return bRet;
}
void CUserDlg::AppendHistoryMsg(LPCTSTR username,LPCTSTR msg)
{
	char timestring[100];
	TCHAR ttimestring[100];
	struct tm mytm;
	time_t t;

	// Getting current time stamp.
	time(&t);

	#if defined(linux)
	localtime_r(&t,&mytm);
	#elif defined(WIN32)
	mytm=*(localtime(&t));
	#else
	#error "FileLog::Log: Implementation partly empty due to unsupported OS"
	#endif

	strftime(timestring,100,"%Y/%m/%d %H:%M:%S",&mytm);
	AXUtinity::String2TString(timestring,ttimestring,100);

	m_editHistory.AppendText(username);
	m_editHistory.AppendText(_T(" "));

	m_editHistory.AppendText(ttimestring);
	m_editHistory.AppendText(_T("\n"));

	m_editHistory.AppendText(msg);
	m_editHistory.AppendText(_T("\n"));
}


void CUserDlg::ClearInputMsg()
{
	SetDlgItemText(IDC_EDITINPUT,_T(""));
}

void CUserDlg::AdjustChildPoses(void)
{
	RECT rc;
	GetClientRect(&rc);

	RECT historyRect(rc);	//38 pixels for 'send,close,...' buttons.
	RECT inputRect(rc);
	RECT closeRect(rc);
	RECT sendRect(rc);

	const float HistoryHeightPercent = 0.6;
	const long InterHistoryAndInputHeight = 10;
	const long InterButtonsSpace = 10;
	const long ButtonAreaHeight = 38;
	const long ButtonWidth = 60;
	const long ButtonsLeftMargin = 4;
	const long ButtonsBottomMargin = 4;

	historyRect.bottom = static_cast<long>(rc.bottom*(HistoryHeightPercent));
	
	inputRect.top = historyRect.bottom+InterHistoryAndInputHeight;
	inputRect.bottom = rc.bottom-ButtonAreaHeight;

	closeRect.left = rc.right-2*ButtonWidth-InterButtonsSpace-ButtonsLeftMargin;
	closeRect.right = rc.right-ButtonWidth-ButtonsLeftMargin;
	closeRect.top = inputRect.bottom+ButtonsBottomMargin;
	closeRect.bottom = rc.bottom-ButtonsLeftMargin;

	sendRect.left = rc.right-ButtonWidth-ButtonsLeftMargin;
	sendRect.right = rc.right-ButtonsLeftMargin;
	sendRect.top = closeRect.top;
	sendRect.bottom = closeRect.bottom;

	m_editHistory.MoveWindow(&historyRect);
	m_editInput.MoveWindow(&inputRect);

	CWindow wndClose = GetDlgItem(IDC_BUTTONCLOSE);
	wndClose.MoveWindow(&closeRect);
	CWindow wndSend = GetDlgItem(IDC_BUTTONSEND);
	wndSend.MoveWindow(&sendRect);
}
void CUserDlg::SetInputFocus(void)
{
	m_editInput.SetFocus();
}

// CUserDlg

LRESULT CUserDlg::OnBnClickedButtonclose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	::PostMessage(m_wndParent,WM_CLOSEUSERDLG,m_ulUserid,0);

	return 0;
}

LRESULT CUserDlg::OnBnClickedButtonsend(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	::PostMessage(m_wndParent,WM_SENDMSG,m_ulUserid,0);

	return 0;
}
LRESULT CUserDlg::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	WORD cx = LOWORD(lParam);
	WORD cy = HIWORD(lParam);

	AdjustChildPoses();

	return 0;
}
LRESULT CUserDlg::OnCtlColorDlg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    return (long) m_hDialogBrush;
}
LRESULT CUserDlg::OnCtlColorEdit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//SetTextColor( this->GetDC(), m_hEditBrush);
	HDC hdc = (HDC)wParam;
	HWND hwnd = (HWND)lParam;

	// handle history edit only
	if (hwnd != m_editHistory.m_hWnd)
		bHandled=FALSE;

    return (long) m_hEditBrush;
	//// Call the base class implementation first! Otherwise, it may
 //  // undo what we're trying to accomplish here.
 //  //HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

 //  // Are we painting the IDC_MYSTATIC control? We can use
 //  // CWnd::GetDlgCtrlID() to perform the most efficient test.
 //  if (pWnd->GetDlgCtrlID() == IDC_MYSTATIC)
 //  {
 //     // Set the text color to red
 //     pDC->SetTextColor(RGB(255, 0, 0));

 //     // Set the background mode for text to transparent 
 //     // so background will show thru.
 //     pDC->SetBkMode(TRANSPARENT);

 //     // Return handle to our CBrush object
 //     hbr = m_brush;
 //  }

 //  return hbr;
}
LRESULT CUserDlg::OnDropFiles(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//HDROP hDrop = reinterpret_cast<HDROP>(wParam);

	//TCHAR szDroppedFile[MAX_PATH];

	//DragQueryFile(hDrop,    // Struture Identifier
 //       -1,        // -1 to Drop more than one file or ( integer 0 to max )
 //                  // to drop selected No of files
 //       szDroppedFile,// Droped File Name
 //       MAX_PATH);   // Max char 


    return 0;
}
LRESULT CUserDlg::OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	::MessageBox(NULL,_T("OnTimer"),_T("CUserDlg"),MB_OK);

	return 0;
}
