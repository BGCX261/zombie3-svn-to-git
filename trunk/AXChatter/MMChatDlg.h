// MMenuDlg.h : Declaration of the CMMenuDlg

#pragma once

#include "resource.h"       // main symbols
#include <atlhost.h>
#include "ATLControls.h"

using namespace ATLControls;

// CMMenuDlg
class CMMenuDlg : 
	public CAxDialogImpl<CMMenuDlg>
{
public:
	CMMenuDlg();
	~CMMenuDlg();

	enum { IDD = IDD_MMCHATDLG };

BEGIN_MSG_MAP(CMMenuDlg)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	MESSAGE_HANDLER(WM_SIZE, OnSize)
	MESSAGE_HANDLER(WM_CTLCOLORDLG, OnCtlColorDlg)
	MESSAGE_HANDLER(WM_CTLCOLORLISTBOX, OnCtlColorListBox)
	MESSAGE_HANDLER(WM_CTLCOLOREDIT, OnCtlColorListBox)
	MESSAGE_HANDLER(WM_CTLCOLORBTN, OnCtlColorListBox)
	MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColorListBox)
	//MESSAGE_HANDLER(WM_CTLCOLOR, OnCtlColorListBox)
	CHAIN_MSG_MAP(CAxDialogImpl<CMMenuDlg>)
	COMMAND_HANDLER(101, CBN_SELCHANGE, OnCombVideoSelChange)
	COMMAND_HANDLER(102, CBN_SELCHANGE, OnCombVoiceSelChange)
	ALT_MSG_MAP(1)	//Combobox
		//COMMAND_HANDLER(101, CBN_SELCHANGE, OnCombVideoSelChange)
		//COMMAND_HANDLER(102, CBN_SELCHANGE, OnCombVoiceSelChange)
	ALT_MSG_MAP(2)
END_MSG_MAP()

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
////////////////////////////////////////////////////////
// control variables
////////////////////////////////////////////////////////
	CContainedWindowT<CComboBox,CWinTraitsOR<WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST>> m_combVideo;
	CContainedWindowT<CComboBox,CWinTraitsOR<WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST>> m_combVoice;
	HBRUSH m_hDialogBrush;
	HBRUSH m_hListBoxBrush;

	void SetDropLengths();

	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCtlColorDlg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCtlColorListBox(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCombVideoSelChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCombVoiceSelChange(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};


