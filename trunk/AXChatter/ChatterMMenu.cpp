// ChatterMMenu.cpp : Implementation of CChatterMMenu
#include "stdafx.h"
#include "MMChatDlg.h"
#include "ChatterMMenu.h"


#ifndef _WIN32_WCE
#pragma comment(lib, "comctl32.lib")
#else
#pragma comment(lib, "commctrl.lib")
#endif

extern COLORREF COLOR_DCHATTER;

CChatterMMenu* g_pChatterMMenu = NULL;

// CChatterMMenu
CChatterMMenu::CChatterMMenu()
		: m_tab(_T("SysTabControl32"), this, 1),
		m_Dialog0(NULL)
{
	m_hDialogBrush = CreateSolidBrush(COLOR_DCHATTER);

	m_bWindowOnly = TRUE;
}
CChatterMMenu::~CChatterMMenu()
{
	DeleteObject(m_hDialogBrush);
}
LRESULT CChatterMMenu::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	RECT rc;
	GetWindowRect(&rc);
	rc.right -= rc.left;
	rc.bottom -= rc.top;
	rc.top = rc.left = 0;
	InitCommonControls();
	m_tab.Create(this, 1, m_hWnd, &rc, NULL, 0,0, 101);

	RECT l_rectClient;
	//RECT l_rectWnd;

	m_tab.GetClientRect(&l_rectClient);
	m_tab.AdjustRect(FALSE,&l_rectClient);

	// insert the chat tab.
	m_Dialog0 = new CMMenuDlg();

	if (NULL == m_Dialog0->Create(m_tab.m_hWnd))
	{
		::MessageBox(NULL,_T("Create a CMMenuDlg failed"),_T("CChatterMMenu"),MB_OK);
		delete m_Dialog0;
		return S_FALSE;
	}

	/** add for test only.*/
	TCITEM* item1 = new TCITEM();
	item1->mask = TCIF_TEXT;
	item1->pszText=_T("Chat");

	m_tab.InsertItem(m_tab.GetItemCount(),item1);
	ActivateTabDialogs();

	g_pChatterMMenu = this;

	return 0;
}

void CChatterMMenu::ActivateTabDialogs(void)
{
	int nSel = m_tab.GetCurSel();

	RECT l_rectClient;
	//RECT l_rectWnd;

	m_tab.GetClientRect(&l_rectClient);
	m_tab.AdjustRect(FALSE,&l_rectClient);
	//m_tab.GetWindowRect(&l_rectWnd);
	//m_tab.GetParent().ScreenToClient(&l_rectWnd);

	/** hide all tab windows.*/
	m_Dialog0->ShowWindow(SW_HIDE);

	/** show current tab window if any.*/
	switch (nSel)
	{
	case 0:
		m_Dialog0->SetWindowPos(HWND_TOP,
									l_rectClient.left,l_rectClient.top,
									l_rectClient.right-l_rectClient.left,l_rectClient.bottom-l_rectClient.top,
									SWP_SHOWWINDOW);
	default:
		break;
	}
		
}
LRESULT CChatterMMenu::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//RECT rc;
	//GetWindowRect(&rc);

	//m_tab.MoveWindow(&rc);

	WORD cx = LOWORD(lParam);
	WORD cy = HIWORD(lParam);

	m_tab.MoveWindow(0, 0, cx, cy);

	ActivateTabDialogs();

	return 999;
}
LRESULT CChatterMMenu::OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) lParam; // item drawing information
	//hTabCtrl = GetDlgItem(hDlg, IDC_TAB1);

	if (m_tab.m_hWnd == lpdis->hwndItem)   // is this the tab control?
	{
		m_hDialogBrush;
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

		RECT tabRect;
		m_tab.GetWindowRect(&tabRect);
		FillRect(lpdis->hDC, &tabRect, m_hDialogBrush);
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
// This OnCtlColor handler will change the color of a static control
// with the ID of IDC_MYSTATIC. The code assumes that the CMyDialog
// class has an initialized and created CBrush member named m_brush.
// The control will be painted with red text and a background
// color of m_brush.

LRESULT CChatterMMenu::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//(HDC)wParam
	RECT rect;
	GetClientRect(&rect);
	::FillRect((HDC)wParam, &rect, static_cast<HBRUSH>(m_hDialogBrush));

	return 1; // don't do the default erase
}