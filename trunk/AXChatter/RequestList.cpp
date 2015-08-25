// RequestList.cpp : Implementation of CRequestList
#include "stdafx.h"
#include "AXUtinity.h"
#include "RequestList.h"

#ifndef _WIN32_WCE
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "chatter.lib")
#else
#pragma comment(lib, "commctrl.lib")
#endif

void CRequestList::AcceptRequest(ULONG tstatusid,TCHAR* additional)
{
	Fire_AcceptRequest(tstatusid,additional);
}
void CRequestList::SaveAsRequest(ULONG ulTStatusId)
{}
void CRequestList::RejectRequest(ULONG tstatusid)
{
	Fire_RejectRequest(tstatusid);
}
void CRequestList::CancelTask(ULONG tstatusid)
{
	Fire_CancelTask(tstatusid);
}
int CRequestList::FindTittle(ULONG ulTStatusId)
{
	int ret = -1;

	int n = m_listview.GetItemCount();

	RequestSubItem* data;
	for (int i=0;i<n;i++)
	{
		data = reinterpret_cast<RequestSubItem*>(m_listview.GetItemData(i));

		if (!data)
			continue;

		if (data->GetTStatusId()==ulTStatusId && dynamic_cast<RequestSubItemTittle*>(data))
		{
			ret = i;
			break;
		}
	}

	return ret;
}

int CRequestList::FindProgress(ULONG ulTStatusId)
{
	int ret = -1;

	int n = m_listview.GetItemCount();

	RequestSubItem* data;
	for (int i=0;i<n;i++)
	{
		data = reinterpret_cast<RequestSubItem*>(m_listview.GetItemData(i));

		if (!data)
			continue;

		if (data->GetTStatusId()==ulTStatusId && dynamic_cast<RequestSubItemProgress*>(data))
		{
			ret = i;
			break;
		}
	}

	return ret;
}
int CRequestList::FindButtons(ULONG ulTStatusId)
{
	int ret = -1;

	int n = m_listview.GetItemCount();

	RequestSubItem* data;
	for (int i=0;i<n;i++)
	{
		data = reinterpret_cast<RequestSubItem*>(m_listview.GetItemData(i));

		if (!data)
			continue;

		if (data->GetTStatusId()==ulTStatusId && dynamic_cast<RequestSubItemButtons*>(data))
		{
			ret = i;
			break;
		}
	}

	return ret;
}
LRESULT CRequestList::OnNotify(UINT nMessage, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	USES_CONVERSION;
	NMLISTVIEW* pnmlv = (NMLISTVIEW*) lParam;
	if (pnmlv->hdr.code == LVN_ITEMCHANGED)
	{
		//::MessageBox(NULL,_T("LVN_ITEMCHANGED"),_T("OnNotify"),MB_OK);

		//NMLISTVIEW* pnmlv = (NMLISTVIEW*) lParam;
		//if (pnmlv->uNewState & LVIS_FOCUSED)
		//{
		//	CProperty* pProp = (CProperty*)m_listview.GetItemData(pnmlv->iItem);
		//	LPCTSTR lpszDesc = (pProp == NULL) ? _T("") : OLE2T(pProp->m_bstrDesc);
		//	m_wndDesc.SetWindowText(lpszDesc);
		//}
	}

	//if (pnmlv->hdr.code == NM_RETURN)
	//{
	//	pnmlv->hdr.code = NM_DBLCLK;
	//	pnmlv->iItem = m_listview.GetSelectedIndex();
	//	pnmlv->iSubItem = 1;
	//}
	if ((pnmlv->hdr.code == NM_DBLCLK) || (pnmlv->hdr.code == NM_CLICK) )
	{
		int nItem = pnmlv->iItem;
		int nSubItem = pnmlv->iSubItem;
		m_listview.SetItemState(nItem,0,LVIS_SELECTED|LVIS_FOCUSED);

		RequestSubItem* data = reinterpret_cast<RequestSubItem*>(m_listview.GetItemData(nItem));
		RequestSubItemButtons* buttons = dynamic_cast<RequestSubItemButtons*>(data);
		if(data==NULL || buttons==NULL)
			return S_FALSE;

		DWORD pos = GetMessagePos();
		POINT pt = {GET_X_LPARAM(pos), GET_Y_LPARAM(pos)};
		this->ScreenToClient(&pt);

		switch(buttons->GetClickTriggeredCmdType(pt))
		{
		case CLICK_TRIGGERED_CMD_ACCEPT:
			AcceptRequest(buttons->GetTStatusId());
			break;
		case CLICK_TRIGGERED_CMD_SAVEAS:
			break;
		case CLICK_TRIGGERED_CMD_REJECT:
			RejectRequest(buttons->GetTStatusId());
			break;
		case CLICK_TRIGGERED_CMD_CANCEL:
			CancelTask(buttons->GetTStatusId());
			break;
		case CLICK_TRIGGERED_CMD_TRANSFERFILEOFFLINE:
			break;
		default:
			break;
		}
	}
	return S_OK;
}
LRESULT CRequestList::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	WORD cx = LOWORD(lParam);
	WORD cy = HIWORD(lParam);

	m_listview.MoveWindow(0, 0, cx, cy);
	m_listview.SetColumnWidth(0,cx);

	return 0;
}

//STDMETHODIMP CRequestList::AddRequestVoiceChatOut(ULONG tstatusid, BSTR username)
//{
//	RECT rc;
//	GetClientRect(&rc);
//	ULONG ulListViewWidth = (rc.right-rc.left);
//	HFONT hFont = m_listview.GetFont();
//
//	RequestSubItemTittle* tittle = new RequestSubItemTittle(tstatusid,username,REQUEST_ITEM_TYPE_VOICECHAT_OUT);
//	RequestSubItemButtons* button = new RequestSubItemButtons(tstatusid,ulListViewWidth,hFont,REQUEST_ITEM_TYPE_VOICECHAT_OUT,REQUEST_ITEM_STATE_CREATED);
//
//	int n = m_listview.GetItemCount();
//
//	m_listview.AddItem(n,0,tittle->GetText());
//	m_listview.SetItemData(n,reinterpret_cast<DWORD>(tittle));
//
//	m_listview.AddItem(n+1,0,button->GetText());
//	m_listview.SetItemData(n+1,reinterpret_cast<DWORD>(button));
//
//	return S_OK;
//}

STDMETHODIMP CRequestList::AddItem(ULONG tstatusid, BSTR tittle)
{
	RequestSubItemTittle* itemtittle = new RequestSubItemTittle(tstatusid,tittle);

	int n = m_listview.GetItemCount();

	m_listview.AddItem(n,0,itemtittle->GetTittle());
	m_listview.SetItemData(n,reinterpret_cast<DWORD>(itemtittle));

	return S_OK;
}

STDMETHODIMP CRequestList::DeleteItem(ULONG tstatusid)
{
	HRESULT res = S_FALSE;
	RequestSubItem* item = NULL;

	int tittleindex = FindTittle(tstatusid);
	if (tittleindex>=0)
	{
		item = reinterpret_cast<RequestSubItem*>(m_listview.GetItemData(tittleindex));
		delete item;
		item = NULL;
		m_listview.DeleteItem(tittleindex);
		res = S_OK;
	}
	
	int progressindex = FindProgress(tstatusid);
	if (progressindex>=0)
	{
		item = reinterpret_cast<RequestSubItem*>(m_listview.GetItemData(progressindex));
		delete item;
		item = NULL;
		m_listview.DeleteItem(progressindex);
	}
	
	int buttonsindex = FindButtons(tstatusid);
	if (buttonsindex>=0)
	{
		item = reinterpret_cast<RequestSubItem*>(m_listview.GetItemData(buttonsindex));
		delete item;
		item = NULL;
		m_listview.DeleteItem(buttonsindex);
	}

	return S_OK;
}

STDMETHODIMP CRequestList::SetButtonYesNo(ULONG tstatusid)
{
	RequestSubItem* subitem = 0;
	RequestSubItemButtons* button = 0;

	int index = FindButtons(tstatusid);

	if (index<0)
	{
		int index2 = FindProgress(tstatusid);
		if (index2<0)
			index2 = FindTittle(tstatusid);

		_ASSERT(index2>=0);

		RECT rc;
		GetClientRect(&rc);
		ULONG ulListViewWidth = (rc.right-rc.left);
		HFONT hFont = m_listview.GetFont();
		RequestSubItemButtons* button = new RequestSubItemButtons(tstatusid,ulListViewWidth,hFont);
		button = new RequestSubItemButtons(tstatusid,ulListViewWidth,hFont);

		button->CreateButtonsYesNo();

		m_listview.AddItem(index2+1,0,button->GetButtonsText());
		m_listview.SetItemData(index2+1,reinterpret_cast<DWORD>(button));
	}
	else
	{
		subitem = reinterpret_cast<RequestSubItem*>(m_listview.GetItemData(index));
		button = dynamic_cast<RequestSubItemButtons*>(subitem);

		if(button==0)
			return S_FALSE;

		button->CreateButtonsYesNo();

		m_listview.SetItem(index,0,LVIF_TEXT,button->GetButtonsText(),0,0,0,0);
	}
		
	return S_OK;
}

STDMETHODIMP CRequestList::SetButtonYesAsNo(ULONG tstatusid)
{
	RequestSubItem* subitem = 0;
	RequestSubItemButtons* button = 0;

	int index = FindButtons(tstatusid);

	if (index<0)
	{
		int index2 = FindProgress(tstatusid);
		if (index2<0)
			index2 = FindTittle(tstatusid);

		_ASSERT(index2>=0);

		RECT rc;
		GetClientRect(&rc);
		ULONG ulListViewWidth = (rc.right-rc.left);
		HFONT hFont = m_listview.GetFont();
		RequestSubItemButtons* button = new RequestSubItemButtons(tstatusid,ulListViewWidth,hFont);
		button = new RequestSubItemButtons(tstatusid,ulListViewWidth,hFont);

		button->CreateButtonsYesAsNo();

		m_listview.AddItem(index2+1,0,button->GetButtonsText());
		m_listview.SetItemData(m_listview.GetItemCount()-1,reinterpret_cast<DWORD>(button));
	}
	else
	{
		subitem = reinterpret_cast<RequestSubItem*>(m_listview.GetItemData(index));
		button = dynamic_cast<RequestSubItemButtons*>(subitem);

		if(button==0)
			return S_FALSE;

		button->CreateButtonsYesAsNo();

		m_listview.SetItem(index,0,LVIF_TEXT,button->GetButtonsText(),0,0,0,0);
	}
		
	return S_OK;
}

STDMETHODIMP CRequestList::SetButtonCancel(ULONG tstatusid)
{
	RequestSubItem* subitem = 0;
	RequestSubItemButtons* button = 0;

	int index = FindButtons(tstatusid);

	if (index<0)
	{
		int index2 = FindProgress(tstatusid);
		if (index2<0)
			index2 = FindTittle(tstatusid);

		_ASSERT(index2>=0);

		RECT rc;
		GetClientRect(&rc);
		ULONG ulListViewWidth = (rc.right-rc.left);
		HFONT hFont = m_listview.GetFont();
		RequestSubItemButtons* button = new RequestSubItemButtons(tstatusid,ulListViewWidth,hFont);
		button = new RequestSubItemButtons(tstatusid,ulListViewWidth,hFont);

		button->CreateButtonsCancel();

		m_listview.AddItem(index2+1,0,button->GetButtonsText());
		m_listview.SetItemData(index2+1,reinterpret_cast<DWORD>(button));
	}
	else
	{
		subitem = reinterpret_cast<RequestSubItem*>(m_listview.GetItemData(index));
		button = dynamic_cast<RequestSubItemButtons*>(subitem);

		if(button==0)
			return S_FALSE;

		button->CreateButtonsCancel();

		m_listview.SetItem(index,0,LVIF_TEXT,button->GetButtonsText(),0,0,0,0);
	}
		
	return S_OK;
}

STDMETHODIMP CRequestList::SetProgress(ULONG tstatusid, ULONG percentage)
{
	RequestSubItemProgress* progress;

	int index = FindProgress(tstatusid);

	if (index<0)
	{
		index = FindTittle(tstatusid);

		progress = new RequestSubItemProgress(tstatusid,m_listview.m_hWnd);
		progress->SetProgress(percentage);

		m_listview.AddItem(index+1,0,_T(""));
		m_listview.SetItemData(index+1,reinterpret_cast<DWORD>(progress));

		RECT rc;
		m_listview.GetItemRect(index+1,&rc,LVIR_LABEL);
		progress->MoveCtrlWindow(rc);
	}
	else
	{
		progress = dynamic_cast<RequestSubItemProgress*>(reinterpret_cast<RequestSubItem*>(m_listview.GetItemData(index)));
		if (progress==0)
			return S_FALSE;

		progress->SetProgress(percentage);
	}

	return S_OK;
}
