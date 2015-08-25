// SRenderCollection.cpp : Implementation of CSRenderCollection
#include "stdafx.h"
#include <config.hxx>
#include "xchatter.h"
#include "RenderDlg.h"
#include "SRenderCollection.h"

extern Cxchatter* g_pTheChatter;

// CSRenderCollection
void CSRenderCollection::GetRectForNewRender(RECT& rc)
{
	static int SCREEN_WIDTH	= GetSystemMetrics(SM_CXFULLSCREEN);
	static int SCREEN_HEIGHT= GetSystemMetrics(SM_CYFULLSCREEN);
	static int NUM_IN_ROW	= SCREEN_WIDTH / CAM_WIDTH;

	int cnum = m_mapFromRender.size();

	rc.top = ((cnum+1)/NUM_IN_ROW) * CAM_HEIGHT;
	rc.left = (cnum % NUM_IN_ROW) * CAM_WIDTH;

	rc.bottom = rc.top + CAM_HEIGHT;
	rc.right = rc.left + CAM_WIDTH;
}
STDMETHODIMP CSRenderCollection::CreateRender(BYTE* host, ULONG port, BYTE* caption)
{
	char from[100]={0};
	sprintf_s(from,"%s::%d",host,port);
	tFromRenderMapIter iter = m_mapFromRender.find(from);

	// already exists
	if (iter!=m_mapFromRender.end())
		return S_OK;

	RECT rc;
	GetRectForNewRender(rc);

	CRenderDlg* dlg = new CRenderDlg((char*)from);

	HWND hwnd = dlg->Create(this->m_hWnd,rc);
	if (NULL == hwnd)
		return S_FALSE;

	//dlg->ModifyStyle(0,1,WS_POPUP);
	dlg->SetWindowText(CA2W((char*)caption));
	dlg->ShowWindow(SW_SHOW);

	m_mapFromRender.insert(tFromRenderMap::value_type(from,dlg));

	return S_OK;
}

STDMETHODIMP CSRenderCollection::DestroyRender(BYTE* host, ULONG port)
{
	// TODO: Add your implementation code here

	return S_OK;
}

LRESULT CSRenderCollection::OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	//// TODO: Add your message handler code here and/or call default
	////g_pTheChatter
	//tFromRenderMapIter iter = m_mapFromRender.begin();

	//while (iter!=m_mapFromRender.end())
	//{
	//	if (g_pTheChatter->GetImage((const BYTE*)iter->first.c_str(),m_pImageBuf,IMAGE_LEN))
	//		iter->second->DisplayImage(m_pImageBuf);

	//	++iter;		
	//}

	return 0;
}

STDMETHODIMP CSRenderCollection::Play(ULONG interval)
{
	//this->KillTimer(TIMER_ID);
	//this->SetTimer(TIMER_ID,interval,0);

	return S_OK;
}

STDMETHODIMP CSRenderCollection::Stop(void)
{
	//this->KillTimer(TIMER_ID);

	return S_OK;
}
