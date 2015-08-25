// GroupList.h : Declaration of the CGroupList
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include "AXChatter.h"
#include "_IGroupListEvents_CP.h"
#include <commctrl.h>
#include "ATLControls.h"

using namespace ATLControls;

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// CGroupList
class ATL_NO_VTABLE CGroupList :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IGroupList, &IID_IGroupList, &LIBID_AXChatterLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IPersistStreamInitImpl<CGroupList>,
	public IOleControlImpl<CGroupList>,
	public IOleObjectImpl<CGroupList>,
	public IOleInPlaceActiveObjectImpl<CGroupList>,
	public IViewObjectExImpl<CGroupList>,
	public IOleInPlaceObjectWindowlessImpl<CGroupList>,
	public IConnectionPointContainerImpl<CGroupList>,
	public CProxy_IGroupListEvents<CGroupList>,
#ifdef _WIN32_WCE // IObjectSafety is required on Windows CE for the control to be loaded correctly
	public IObjectSafetyImpl<CGroupList, INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
#endif
	public CComCoClass<CGroupList, &CLSID_GroupList>,
	public CComControl<CGroupList>
{
public:
////////////////////////////////////////////////////////
// typedefs
////////////////////////////////////////////////////////
	typedef enum
	{
		POPUP_MENU_CMD_ID_UNDEFINED=30000,
		POPUP_MENU_CMD_ID_DELETE=30001,
	}ePopupMenuCmdID;

#pragma warning(push)
#pragma warning(disable: 4355) // 'this' : used in base member initializer list


	CGroupList()
		: m_listview(_T("SysListView32"), this, 1),
		  m_bAmIAdmin(FALSE)
	{
		m_bWindowOnly = TRUE;
	}

#pragma warning(pop)

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_INSIDEOUT |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_GROUPLIST)


BEGIN_COM_MAP(CGroupList)
	COM_INTERFACE_ENTRY(IGroupList)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IViewObjectEx)
	COM_INTERFACE_ENTRY(IViewObject2)
	COM_INTERFACE_ENTRY(IViewObject)
	COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceObject)
	COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
	COM_INTERFACE_ENTRY(IOleControl)
	COM_INTERFACE_ENTRY(IOleObject)
	COM_INTERFACE_ENTRY(IPersistStreamInit)
	COM_INTERFACE_ENTRY2(IPersist, IPersistStreamInit)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
#ifdef _WIN32_WCE // IObjectSafety is required on Windows CE for the control to be loaded correctly
	COM_INTERFACE_ENTRY_IID(IID_IObjectSafety, IObjectSafety)
#endif
END_COM_MAP()

BEGIN_PROP_MAP(CGroupList)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// Example entries
	// PROP_ENTRY("Property Description", dispid, clsid)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()

BEGIN_CONNECTION_POINT_MAP(CGroupList)
	CONNECTION_POINT_ENTRY(__uuidof(_IGroupListEvents))
END_CONNECTION_POINT_MAP()

BEGIN_MSG_MAP(CGroupList)
	MESSAGE_HANDLER(WM_CREATE, OnCreate)
	MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
	MESSAGE_HANDLER(WM_SIZE, OnSize)
	MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
	CHAIN_MSG_MAP(CComControl<CGroupList>)
ALT_MSG_MAP(1)
	// Replace this with message map entries for superclassed SysListView32
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	BOOL PreTranslateAccelerator(LPMSG pMsg, HRESULT& hRet)
	{
		if(pMsg->message == WM_KEYDOWN)
		{
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
				hRet = S_FALSE;
				return TRUE;
			}
		}
		//TODO: Add your additional accelerator handling code here
		return FALSE;
	}

	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lRes = CComControl<CGroupList>::OnSetFocus(uMsg, wParam, lParam, bHandled);
		if (m_bInPlaceActive)
		{
			if(!IsChild(::GetFocus()))
				m_listview.SetFocus();
		}
		return lRes;
	}

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		RECT rc;
		GetWindowRect(&rc);
		rc.right -= rc.left;
		rc.bottom -= rc.top;
		rc.top = rc.left = 0;
		InitCommonControls();
		//m_listview.Create(m_hWnd, rc);

		m_listview.Create(this, 1, m_hWnd, &rc, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, 101);
		m_listview.SetExtendedListViewStyle(0 | LVS_EX_FULLROWSELECT,LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

		//m_listview.AddColumn( 0);
		m_listview.InsertColumn(0,_T("Member List"),LVCFMT_CENTER,rc.right,0);

		////////////////test
		return 0;
	}

	STDMETHOD(SetObjectRects)(LPCRECT prcPos,LPCRECT prcClip)
	{
		IOleInPlaceObjectWindowlessImpl<CGroupList>::SetObjectRects(prcPos, prcClip);
		int cx, cy;
		cx = prcPos->right - prcPos->left;
		cy = prcPos->bottom - prcPos->top;
		::SetWindowPos(m_listview.m_hWnd, NULL, 0,
			0, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);
		return S_OK;
	}

// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

// IGroupList

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
	void FinalRelease()
	{
	}
private:
////////////////////////////////////////////////////////////////
// private variables
////////////////////////////////////////////////////////////////
	BOOL m_bAmIAdmin;
	CContainedWindowT<CListViewCtrl, CWinTraitsOR<LVS_REPORT | LVS_NOSORTHEADER | LVS_SINGLESEL, WS_EX_CLIENTEDGE> > m_listview;

////////////////////////////////////////////////////////////////
// private methods
////////////////////////////////////////////////////////////////
	LONG FindIndexByUserid(ULONG ulUserid) const;
	HMENU CreateMyPopupMenu(BOOL bEnableAction);

////////////////////////////////////////////////////////////////
// Event Handlers
////////////////////////////////////////////////////////////////
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnNotify(UINT nMessage, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
////////////////////////////////////////////////////////////////
// IGroupList Interface
////////////////////////////////////////////////////////////////
	STDMETHOD(AddMember)(ULONG userid, BSTR username,BOOL isadmin);
	STDMETHOD(DeleteMember)(ULONG userid);
	STDMETHOD(SetMeAdmin)(BOOL bAmIAdmin);
public:
	STDMETHOD(Empty)(void);
public:
	STDMETHOD(Size)(LONG* size);
};

OBJECT_ENTRY_AUTO(__uuidof(GroupList), CGroupList)
