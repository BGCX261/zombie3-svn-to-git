// RenderDlg.cpp : Implementation of CRenderDlg

#include "stdafx.h"
#include "xchatter.h"
#include "RenderDlg.h"


// CRenderDlg

extern Cxchatter* g_pTheChatter;

LRESULT CRenderDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MakeBMPHeader();

	this->KillTimer(TIMER_ID);
	this->SetTimer(TIMER_ID,VIDEO_INTERVAL,0);

	CAxDialogImpl<CRenderDlg>::OnInitDialog(uMsg, wParam, lParam, bHandled);
	bHandled = TRUE;
	return 1;  // Let the system set the focus
}

void CRenderDlg::MakeBMPHeader()
{
	DWORD  dwBitmapInfoSize;
	dwBitmapInfoSize = sizeof(BITMAPINFO);
	m_pBmp    = (BITMAPINFO *)new BYTE [dwBitmapInfoSize];

	m_pBmp->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    m_pBmp->bmiHeader.biWidth = CAM_WIDTH;
    m_pBmp->bmiHeader.biHeight = CAM_HEIGHT*-1;	
    m_pBmp->bmiHeader.biPlanes = 1;
    m_pBmp->bmiHeader.biBitCount = CAM_IMAGEBITS;
    m_pBmp->bmiHeader.biCompression = BI_RGB;
    m_pBmp->bmiHeader.biSizeImage = 0;
    m_pBmp->bmiHeader.biXPelsPerMeter = 0;
    m_pBmp->bmiHeader.biYPelsPerMeter = 0;
    m_pBmp->bmiHeader.biClrUsed = 0;
    m_pBmp->bmiHeader.biClrImportant = 0;
}
LRESULT CRenderDlg::OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (g_pTheChatter->GetImage((const BYTE*)m_from.c_str(),m_pImage,IMAGE_LEN))
		this->Invalidate();

	//::MessageBox(NULL,_T("OnTimer"),_T("CRenderDlg"),MB_OK);

	return 0;
}

LRESULT CRenderDlg::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	RECT rc;
	HDC hdc = this->GetDC();
	//int readsize=0;

	//readsize = m_pDataBuffer->ReadBlock(m_pImageBytes,m_iFrameLen);

	//if (readsize<m_iFrameLen)
	//	return 0;

	// TODO: Add your message handler code here
	//if(m_pImageBytes!=NULL)
	//{
		GetClientRect(&rc);
		RealizePalette(hdc);
		SetStretchBltMode(hdc, COLORONCOLOR);
		
		//int m_un32ImageHeight;
		int nResult = ::StretchDIBits(
									hdc,
									rc.right-rc.left,
									rc.top,
									-rc.right-rc.left,
									rc.bottom-rc.top,
									0,
									0,
									CAM_WIDTH, 
									CAM_HEIGHT,
									m_pImage, 
									m_pBmp,
									DIB_RGB_COLORS,
									SRCCOPY);
	//}

	return 0;
}
