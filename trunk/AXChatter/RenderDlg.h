// RenderDlg.h : Declaration of the CRenderDlg

#pragma once

#include "resource.h"       // main symbols

#include <atlhost.h>
#include <string>
#include <config.hxx>
#include <databuffer.hxx>

#pragma comment(lib,"common")

// CRenderDlg

class DataBuffer;

class CRenderDlg : 
	public CAxDialogImpl<CRenderDlg>
{
public:
	CRenderDlg(char* from):m_from(from)
	{
		m_iFrameLen = CAM_WIDTH*CAM_HEIGHT*CAM_IMAGEBITS;
		m_pImage = new BYTE[m_iFrameLen];
		m_pDataBuffer = new DataBuffer(m_iFrameLen,RENDER_BLOCK_NUM);
	}

	~CRenderDlg()
	{
		delete m_pDataBuffer;
		delete[] m_pImage;
	}

	enum { IDD = IDD_RENDERDLG };

BEGIN_MSG_MAP(CRenderDlg)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	MESSAGE_HANDLER(WM_TIMER, OnTimer)
	MESSAGE_HANDLER(WM_PAINT, OnPaint)
	CHAIN_MSG_MAP(CAxDialogImpl<CRenderDlg>)
END_MSG_MAP()

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
private:
	void MakeBMPHeader();

	BYTE *m_pImageBytes;

	BITMAPINFO* m_pBmp;
	DataBuffer* m_pDataBuffer;
	BYTE* m_pImage;
	DWORD m_iFrameLen;
	std::string m_from;
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};


