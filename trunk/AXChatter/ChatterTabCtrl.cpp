// ChatterTabCtrl.cpp : Implementation of CChatterTabCtrl
#include "stdafx.h"
#include "userdlg.h"
#include "ChatterTabCtrl.h"

#ifndef _WIN32_WCE
#pragma comment(lib, "comctl32.lib")
#else
#pragma comment(lib, "commctrl.lib")
#endif

extern COLORREF COLOR_DCHATTER;

// CChatterTabCtrl
CChatterTabCtrl::CChatterTabCtrl()
		: m_tab(_T("SysTabControl32"), this, 1)
{
	m_hDialogBrush = CreateSolidBrush(COLOR_DCHATTER);
	m_bWindowOnly = TRUE;
}

CChatterTabCtrl::~CChatterTabCtrl()
{
	DeleteObject(m_hDialogBrush);
}

LRESULT CChatterTabCtrl::OnCloseUserDlg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ULONG userid = static_cast<ULONG>(wParam);

	if (!RemoveItem(FindIndexByUserid(userid)))
		return S_FALSE;

	return S_OK;
}
LRESULT CChatterTabCtrl::OnSendDlgInputMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ULONG userid = static_cast<ULONG>(wParam);

	CUserDlg* userdlg = FindUserDlgByUserid(userid);
	if (!userdlg)
		return S_FALSE;

	if (userdlg->GetInputMsg().IsEmpty())
		return S_FALSE;

	BSTR username = userdlg->GetUserName().AllocSysString();
	BSTR textmsg = userdlg->GetInputMsg().AllocSysString();
	
	Fire_SendInstantMsg(userid,username,textmsg);

	::SysFreeString(username);
	::SysFreeString(textmsg);

	userdlg->ProcessCurInput();

	return S_OK;
}

LRESULT CChatterTabCtrl::OnTabSelChange(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
{
	ActivateTabDialogs();

	return 0;
}

CUserDlg* CChatterTabCtrl::AddUserDlg(unsigned long userid,LPTSTR username,LPTSTR myname)
{
	CUserDlg* userdlg=NULL;

	if (userdlg=FindUserDlgByUserid(userid))
		return userdlg;

	userdlg = new CUserDlg(m_hWnd,userid,username,myname);

	if (NULL == userdlg->Create(m_tab.m_hWnd))
	{
		::MessageBox(NULL,_T("Create a CUserDlg failed"),_T("AddUserDlg"),MB_OK);
		delete userdlg;
		return NULL;
	}

	m_userdlgVec.push_back(userdlg);

	/** add for test only.*/
	TCITEM* item1 = new TCITEM();
	item1->mask = TCIF_TEXT;
	item1->pszText=username;

	//int insertpos = (pos<0) ? m_tab.GetItemCount() : pos;

	m_tab.InsertItem(m_tab.GetItemCount(),item1);
	
	return userdlg;
}
CUserDlg* CChatterTabCtrl::FindUserDlgByUserid(unsigned long userid)
{
	CUserDlg* userdlg = NULL;

	tUserDlgVecIter it = m_userdlgVec.begin();
	while (it != m_userdlgVec.end())
	{
		if ((*it)->GetUserId()==userid)
		{
			userdlg = (*it);
			break;
		}
		else
			++it;
	}

	return userdlg;
}
int CChatterTabCtrl::FindIndexByUserid(unsigned long userid)
{
	tUserDlgVecIter it = m_userdlgVec.begin();
	while (it != m_userdlgVec.end())
	{
		if ((*it)->GetUserId()==userid)
		{
			break;
		}
		else
			++it;
	}

	if (it==m_userdlgVec.end())
		return -1;
	else
		return static_cast<int>(it-m_userdlgVec.begin());
}
BOOL CChatterTabCtrl::HandEventToUserDlg(LPMSG pMsg, HRESULT& hRet)
{
	CUserDlg* userdlg = GetCurUserDlg();

	if (userdlg!=NULL)
		return userdlg->HandleKeydownEvent(pMsg, hRet);

	return FALSE;
}
void CChatterTabCtrl::ActivateTabDialogs(void)
{
	int nSel = m_tab.GetCurSel();

	RECT l_rectClient;
	//RECT l_rectWnd;

	m_tab.GetClientRect(&l_rectClient);
	m_tab.AdjustRect(FALSE,&l_rectClient);
	//m_tab.GetWindowRect(&l_rectWnd);
	//m_tab.GetParent().ScreenToClient(&l_rectWnd);

	/** hide all tab windows.*/
	tUserDlgVecIter it = m_userdlgVec.begin();
	while (it != m_userdlgVec.end())
	{
		(*it++)->ShowWindow(SW_HIDE);
	}

	/** show current tab window if any.*/
	if(nSel>=0)
		m_userdlgVec[nSel]->SetWindowPos(HWND_TOP,
									l_rectClient.left,l_rectClient.top,
									l_rectClient.right-l_rectClient.left,l_rectClient.bottom-l_rectClient.top,
									SWP_SHOWWINDOW);
}

void CChatterTabCtrl::SetItemDirty(int nItem)
{
	if (nItem<0||nItem>=m_tab.GetItemCount())
		return;

	TCITEM item;
	item.mask=TCIF_STATE;

	m_tab.GetItem(nItem,&item);
	if (item.dwState==TCIS_HIGHLIGHTED)
		return;

	item.dwState = TCIS_HIGHLIGHTED;
	m_tab.SetItem(nItem,&item);
}
void CChatterTabCtrl::SetItemClean(int nItem)
{
	if (nItem<0||nItem>=m_tab.GetItemCount())
		return;

	TCITEM item;
	item.mask=TCIF_STATE;

	m_tab.GetItem(nItem,&item);
	if (item.dwState==0)
		return;

	item.dwState = 0;
	m_tab.SetItem(nItem,&item);
}
bool CChatterTabCtrl::RemoveItem(int nItem)
{
	if (nItem<0||nItem>=m_tab.GetItemCount())
		return false;

	CUserDlg* userdlg = m_userdlgVec[nItem];
	userdlg->DestroyWindow();
	delete userdlg;

	bool isCursel = (nItem==m_tab.GetCurSel());

	m_userdlgVec.erase(m_userdlgVec.begin()+nItem);

	int cursel = m_tab.GetCurSel();

	m_tab.DeleteItem(nItem);

	if (m_tab.GetItemCount()>0 && nItem==cursel)
		m_tab.SetCurSel(cursel==0 ? 0 : cursel-1);

	ActivateTabDialogs();

	return true;
}
CUserDlg* CChatterTabCtrl::GetCurUserDlg(void)
{
	int cursel = m_tab.GetCurSel();

	if (cursel<0)
		return NULL;

	return m_userdlgVec[cursel];
}
STDMETHODIMP CChatterTabCtrl::AddItem(ULONG userid, BSTR username, BSTR myname)
{
	if (!AddUserDlg(userid,username,myname))
		return S_FALSE;

	//m_tab.SetCurSel(m_tab.GetItemCount()-1);

	ActivateTabDialogs();

	return S_OK;
}

STDMETHODIMP CChatterTabCtrl::DeleteItem(ULONG userid)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CChatterTabCtrl::SetCurSel(ULONG userid)
{
	int cursel = FindIndexByUserid(userid);

	if (cursel<0||cursel>=m_tab.GetItemCount())
		return S_FALSE;

	m_tab.SetCurSel(cursel);

	ActivateTabDialogs();

	return S_OK;
}

STDMETHODIMP CChatterTabCtrl::GetCurSel(ULONG* userid)
{
	int cursel = m_tab.GetCurSel();

	if (cursel<0)
	{
		*userid=0;
		return S_FALSE;
	}

	*userid = m_userdlgVec[cursel]->GetUserId();

	return S_OK;
}
STDMETHODIMP CChatterTabCtrl::GetCurSelName(BSTR* username)
{
	int cursel = m_tab.GetCurSel();

	if (cursel<0)
	{
		*username=NULL;
		return S_FALSE;
	}

	*username = m_userdlgVec[cursel]->GetUserName().AllocSysString();
	return S_OK;
}
STDMETHODIMP CChatterTabCtrl::GetItemCount(LONG* lCount)
{
	*lCount = m_tab.GetItemCount();

	return S_OK;
}

STDMETHODIMP CChatterTabCtrl::SetImageList(LONG pImageList)
{
	// TODO: Add your implementation code here

	return S_OK;
}

STDMETHODIMP CChatterTabCtrl::DisplayReceivedUserMsg(ULONG userid, BSTR username, BSTR myname, BSTR instantmsg)
{
	int index = -1;
	CUserDlg* userdlg = FindUserDlgByUserid(userid);

	if (!userdlg)
	{
		userdlg = AddUserDlg(userid,username,myname);
		if (!userdlg)
			return S_FALSE;
		index = m_tab.GetItemCount()-1;
	}
	else
		index = FindIndexByUserid(userid);

	userdlg->AppendReceivedHistory(instantmsg);

	if (index!=m_tab.GetCurSel())
		SetItemDirty(index);

	ActivateTabDialogs();

	return S_OK;
}

STDMETHODIMP CChatterTabCtrl::DisplayNotification(ULONG userid, BSTR username, BSTR myname, BSTR notification)
{
	int index = -1;
	CUserDlg* userdlg = FindUserDlgByUserid(userid);

	if (!userdlg)
	{
		userdlg = AddUserDlg(userid,username,myname);
		if (!userdlg)
			return S_FALSE;
		index = m_tab.GetItemCount()-1;
	}
	else
		index = FindIndexByUserid(userid);

	userdlg->AppendNotification(notification);

	if (index!=m_tab.GetCurSel())
		SetItemDirty(index);

	ActivateTabDialogs();

	return S_OK;
}

LRESULT CChatterTabCtrl::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	WORD cx = LOWORD(lParam);
	WORD cy = HIWORD(lParam);

	m_tab.MoveWindow(0, 0, cx, cy);

	ActivateTabDialogs();

	return 0;
}
LRESULT CChatterTabCtrl::OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) lParam; // item drawing information
	//hTabCtrl = GetDlgItem(hDlg, IDC_TAB1);

	if (m_tab.m_hWnd == lpdis->hwndItem)   // is this the tab control?
	{
		COLORREF bkColor = COLOR_DCHATTER;

	 //// which tab? first, second...fifth
	 //switch (lpdis->itemID)
	 //{
	 //case 0:
		//hbr = hbrRed;
		//bkColor = RED;
		//break;
	 //case 1:
		//hbr = hbrYellow;
		//bkColor = YELLOW;
		//break;
	 //case 2:
		//hbr = hbrMagenta;
		//bkColor = MAGENTA;
		//break;
	 //case 3:
		//hbr = hbrWhite;
		//bkColor = WHITE;
		//break;
	 //case 4:
		//hbr = hbrBlue;
		//bkColor = BLUE;
		//break;
	 //}
		TCHAR szTabText[MAX_PATH]={0};
		//memset(szTabText, '\0', sizeof(szTabText));

		TCITEM tci;
		tci.mask = TCIF_TEXT;
		tci.pszText = szTabText;
		tci.cchTextMax = sizeof(szTabText)-1;

		TabCtrl_GetItem(m_tab, lpdis->itemID, &tci);

		FillRect(lpdis->hDC, &lpdis->rcItem, m_hDialogBrush);
		SetBkColor(lpdis->hDC, bkColor);

		TextOut(lpdis->hDC,
		   lpdis->rcItem.left,
		   lpdis->rcItem.top,
		   tci.pszText,
		   lstrlen(tci.pszText));
	}

	return S_OK;
}
LRESULT CChatterTabCtrl::OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HDC hdc = (HDC)wParam;
	HWND hwnd = (HWND)lParam;

	// handle history edit only
	if (hwnd != m_tab.m_hWnd)
		bHandled=FALSE;

    return (long)m_hDialogBrush;
}