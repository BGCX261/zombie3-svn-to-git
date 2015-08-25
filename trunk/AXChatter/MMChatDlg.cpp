// MMenuDlg.cpp : Implementation of CMMenuDlg

#include "stdafx.h"
#include "ChatterMMenu.h"
#include "MMChatDlg.h"

extern CChatterMMenu* g_pChatterMMenu;

extern COLORREF COLOR_DCHATTER;
extern COLORREF COLOR_DCHATTER;

// CMMenuDlg
CMMenuDlg::CMMenuDlg()
{
	m_hDialogBrush = CreateSolidBrush(COLOR_DCHATTER);
	m_hListBoxBrush = CreateSolidBrush(COLOR_DCHATTER);
}

CMMenuDlg::~CMMenuDlg()
{
	DeleteObject(m_hDialogBrush);
	DeleteObject(m_hListBoxBrush);
}
LRESULT CMMenuDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DefWindowProc();

	RECT rc = {0,0,0,0};
	InitCommonControls();

	m_combVideo.Create(this, 1, m_hWnd, &rc, NULL, 0,0, 101);
	m_combVoice.Create(this, 2, m_hWnd, &rc, NULL, 0,0, 102);

	m_combVideo.AddString(_T(" ” ∆µ"));
	m_combVideo.AddString(_T("≥¨º∂ ”∆µ"));
	m_combVideo.SetCurSel(0);
	//m_combVideo(_T("name"));

	m_combVoice.AddString(_T("”Ô “Ù"));
	m_combVoice.AddString(_T("≥¨º∂”Ô“Ù"));
	m_combVoice.SetCurSel(0);

	CAxDialogImpl<CMMenuDlg>::OnInitDialog(uMsg, wParam, lParam, bHandled);
	bHandled = TRUE;
	return 1;  // Let the system set the focus
}
void CMMenuDlg::SetDropLengths()
{
    RECT rcComboVideo;
	RECT rcComboVoice;
    RECT rcDroppedVideo;
	RECT rcDroppedVoice;

	int nVedioItemHeight = m_combVideo.GetItemHeight(-1);
	int nVoiceItemHeight = m_combVoice.GetItemHeight(-1);
      
    m_combVideo.GetClientRect(&rcComboVideo);
    m_combVideo.GetDroppedControlRect(&rcDroppedVideo);
    m_combVideo.GetParent().ScreenToClient(&rcDroppedVideo);
	rcDroppedVideo.bottom = rcDroppedVideo.top + (rcComboVideo.bottom-rcComboVideo.top) + nVedioItemHeight*m_combVideo.GetCount();
    m_combVideo.MoveWindow(&rcDroppedVideo);

	m_combVoice.GetClientRect(&rcComboVoice);
	m_combVoice.GetDroppedControlRect(&rcDroppedVoice);
	m_combVoice.GetParent().ScreenToClient(&rcDroppedVoice);
	rcDroppedVoice.bottom = rcDroppedVoice.top + (rcComboVoice.bottom-rcComboVoice.top) + nVoiceItemHeight*m_combVoice.GetCount();
    m_combVoice.MoveWindow(&rcDroppedVoice);
}
LRESULT CMMenuDlg::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	RECT rc;
	GetClientRect(&rc);

	RECT rcVideo(rc);
	rcVideo.right = 70;

	RECT rcVoice(rc);
	rcVoice.left=rcVideo.right+2;
	rcVoice.right=rcVoice.left+70;

	m_combVideo.MoveWindow(&rcVideo);
	m_combVoice.MoveWindow(&rcVoice);

	SetDropLengths();

	return 0;
}
LRESULT CMMenuDlg::OnCtlColorDlg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    return (long) m_hDialogBrush;
}
LRESULT CMMenuDlg::OnCtlColorListBox(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HDC hdc = (HDC)wParam;
	HWND hwnd = (HWND)lParam;

	// handle history edit only
	//if (hwnd != m_editHistory.m_hWnd)
	//	bHandled=FALSE;

	SetBkMode(hdc, TRANSPARENT );
	SetBkColor(hdc,COLOR_DCHATTER);

    return (long) m_hListBoxBrush;
}
LRESULT CMMenuDlg::OnCombVideoSelChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (!g_pChatterMMenu)
		return S_FALSE;

	switch (m_combVideo.GetCurSel())
	{
	case 0:
		//nothing to do.
		break;
	default:
		g_pChatterMMenu->Fire_InviteCurVideoChat();
		break;
	}

	m_combVideo.SetCurSel(0);

	return 0;
}
LRESULT CMMenuDlg::OnCombVoiceSelChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	switch (m_combVoice.GetCurSel())
	{
	case 0:
		//nothing to do
		break;
	default:
		g_pChatterMMenu->Fire_InviteCurVoiceChat();
		break;
	}

	m_combVoice.SetCurSel(0);

	return 0;
}
