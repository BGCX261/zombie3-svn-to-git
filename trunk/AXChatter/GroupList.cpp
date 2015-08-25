// GroupList.cpp : Implementation of CGroupList
#include "stdafx.h"
#include <atlstr.h>	// CString	
#include "GroupList.h"

#ifndef _WIN32_WCE
#pragma comment(lib, "comctl32.lib")
#else
#pragma comment(lib, "commctrl.lib")
#endif

//extern CComModule _Module;

HMODULE GetMyModuleHandle()
{
    static int s_somevar = 0;
    MEMORY_BASIC_INFORMATION mbi;
    if(!::VirtualQuery(&s_somevar, &mbi, sizeof(mbi)))
    {
        return NULL;
    }
    return static_cast<HMODULE>(mbi.AllocationBase);
}

// CGroupList
LONG CGroupList::FindIndexByUserid(ULONG ulUserid) const
{
	int nCount = m_listview.GetItemCount();

	int i;
	for (i=0;i<nCount;i++)
	{
		if (ulUserid == static_cast<ULONG>(m_listview.GetItemData(i)))
			break;
	}

	return (i==nCount ? -1 : i);
}
HMENU CGroupList::CreateMyPopupMenu(BOOL bEnableAction)
{
	HMENU hMenu = CreatePopupMenu();
	if (hMenu)
	{
		UINT uFlags=0;
		if (!bEnableAction)
			uFlags |= MF_GRAYED;

			AppendMenu(hMenu,uFlags,POPUP_MENU_CMD_ID_DELETE,_T("delete"));
	}
	return hMenu;
}
LRESULT CGroupList::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	WORD cx = LOWORD(lParam);
	WORD cy = HIWORD(lParam);

	m_listview.MoveWindow(0, 0, cx, cy);
	m_listview.SetColumnWidth(0,cx);

	return 0;
}
LRESULT CGroupList::OnNotify(UINT nMessage, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	USES_CONVERSION;
	NMLISTVIEW* pnmlv = (NMLISTVIEW*) lParam;
	if (pnmlv->hdr.code == LVN_ITEMCHANGED)
	{
	}

	if ((pnmlv->hdr.code == NM_RCLICK) || (pnmlv->hdr.code == NM_RDBLCLK))
	{
		//int nItem = pnmlv->iItem;
		//int nSubItem = pnmlv->iSubItem;

		HMODULE h = GetMyModuleHandle();

		//_AtlBaseModule::GetResourceInstance();
		//HMENU hTop = LoadMenu(GetMyModuleHandle(), MAKEINTRESOURCE(IDR_MENU1));

		int  nItem = -1;
		UINT uSelectedCount = m_listview.GetSelectedCount();

		BOOL bEnableAction = (m_bAmIAdmin&uSelectedCount);
		HMENU hTop = CreateMyPopupMenu(bEnableAction);

		DWORD pos = GetMessagePos();
		POINT pt = {GET_X_LPARAM(pos), GET_Y_LPARAM(pos)};

		UINT uCmd = TrackPopupMenu(hTop,TPM_LEFTALIGN | TPM_NONOTIFY | TPM_RIGHTBUTTON | TPM_RETURNCMD,pt.x,pt.y,0,m_hWnd,NULL);

		switch (uCmd)
		{
		case POPUP_MENU_CMD_ID_DELETE:
			// Update all of the selected items.
			if (uSelectedCount > 0)
			{
			   for (UINT i=0;i < uSelectedCount;i++)
			   {
				  nItem = m_listview.GetNextItem(nItem, LVNI_SELECTED);
				  ATLASSERT(nItem != -1);
				  //m_listview->Update(nItem); 
				  ULONG userid = static_cast<ULONG>(m_listview.GetItemData(nItem));
				  DeleteMember(userid);
				  Fire_DeleteMember(userid);
			   }
			}
			break;
		default:
			break;
		}

		DestroyMenu(hTop);
	}
	return S_OK;
}
STDMETHODIMP CGroupList::AddMember(ULONG userid, BSTR username,BOOL isadmin)
{
	CString strNumber;
	BSTR bstrName(username);

	if (isadmin)
		wsprintf(strNumber.GetBuffer(32),_T("бя %d "), userid);
	else
		wsprintf(strNumber.GetBuffer(32),_T("   %d "), userid);

	strNumber.ReleaseBuffer(); // Must do this!


	LONG index = FindIndexByUserid(userid);
	if (index<0)
	{
		m_listview.AddItem(m_listview.GetItemCount(),0,strNumber+bstrName);
		m_listview.SetItemData(m_listview.GetItemCount()-1,static_cast<DWORD>(userid));
	}
	else
	{
		m_listview.SetItem(index,0,LVIF_TEXT,strNumber+bstrName,0,0,0,0);
	}

	return S_OK;
}

STDMETHODIMP CGroupList::DeleteMember(ULONG userid)
{
	HRESULT ret;

	int nIndex = FindIndexByUserid(userid);

	if (!m_listview.DeleteItem(nIndex))
		ret = S_FALSE;
	else 
		ret = S_OK;

	if (m_listview.GetItemCount()<=1)
		m_listview.DeleteAllItems();

	return ret;
}

STDMETHODIMP CGroupList::SetMeAdmin(BOOL bAmIAdmin)
{
	m_bAmIAdmin = bAmIAdmin;

	return S_OK;
}

STDMETHODIMP CGroupList::Empty(void)
{
	while (m_listview.GetItemCount()>0)
		m_listview.DeleteItem(0);

	return S_OK;
}

STDMETHODIMP CGroupList::Size(LONG* size)
{
	*size = m_listview.GetItemCount();

	return S_OK;
}
