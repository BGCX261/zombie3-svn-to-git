// RequestList.h : Declaration of the CRequestList
#pragma once
#include "resource.h"       // main symbols
#include <vector>
#include <atlctl.h>
#include <atlstr.h>			// CString
#include "AXChatter.h"
#include "chatter.hxx"		//tTStatus
#include "_IRequestListEvents_CP.h"
#include <commctrl.h>
#include "RequestItem.h"

#include "atlcontrols.h"
using namespace ATLControls;

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// CRequestList
class ATL_NO_VTABLE CRequestList :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IRequestList, &IID_IRequestList, &LIBID_AXChatterLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IPersistStreamInitImpl<CRequestList>,
	public IOleControlImpl<CRequestList>,
	public IOleObjectImpl<CRequestList>,
	public IOleInPlaceActiveObjectImpl<CRequestList>,
	public IViewObjectExImpl<CRequestList>,
	public IOleInPlaceObjectWindowlessImpl<CRequestList>,
	public IConnectionPointContainerImpl<CRequestList>,
	public CProxy_IRequestListEvents<CRequestList>,
	public IQuickActivateImpl<CRequestList>,
	public IProvideClassInfo2Impl<&CLSID_RequestList, &__uuidof(_IRequestListEvents), &LIBID_AXChatterLib>,
#ifdef _WIN32_WCE // IObjectSafety is required on Windows CE for the control to be loaded correctly
	public IObjectSafetyImpl<CRequestList, INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
#endif
	public CComCoClass<CRequestList, &CLSID_RequestList>,
	public CComControl<CRequestList>
{
public:
	////////////////////////////////////////////////////////
	// typedefs
	////////////////////////////////////////////////////////
	//typedef 
	CContainedWindowT<CListViewCtrl, CWinTraitsOR<LVS_REPORT | LVS_NOSORTHEADER | LVS_SINGLESEL, WS_EX_CLIENTEDGE> > m_listview;

#pragma warning(push)
#pragma warning(disable: 4355) // 'this' : used in base member initializer list


	CRequestList()
		: m_listview(_T("SysListView32"), this, 1)
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

		DECLARE_REGISTRY_RESOURCEID(IDR_REQUESTLIST)


	BEGIN_COM_MAP(CRequestList)
		COM_INTERFACE_ENTRY(IRequestList)
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
		COM_INTERFACE_ENTRY(IQuickActivate)
		COM_INTERFACE_ENTRY(IProvideClassInfo)
		COM_INTERFACE_ENTRY(IProvideClassInfo2)
#ifdef _WIN32_WCE // IObjectSafety is required on Windows CE for the control to be loaded correctly
		COM_INTERFACE_ENTRY_IID(IID_IObjectSafety, IObjectSafety)
#endif
	END_COM_MAP()

	BEGIN_PROP_MAP(CRequestList)
		PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
		PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
		// Example entries
		// PROP_ENTRY("Property Description", dispid, clsid)
		// PROP_PAGE(CLSID_StockColorPage)
	END_PROP_MAP()

	BEGIN_CONNECTION_POINT_MAP(CRequestList)
		CONNECTION_POINT_ENTRY(__uuidof(_IRequestListEvents))
	END_CONNECTION_POINT_MAP()

	BEGIN_MSG_MAP(CRequestList)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		//MESSAGE_HANDLER(WM_VSCROLL, OnVScroll)
		//MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
		//MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		//MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
		//MESSAGE_HANDLER(NM_CLICK, OnNMClick)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		CHAIN_MSG_MAP(CComControl<CRequestList>)
		ALT_MSG_MAP(1)
		// Replace this with message map entries for superclassed SysListView32
		//MESSAGE_HANDLER(WM_VSCROLL, OnVScroll)
		//MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		//MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		//MESSAGE_HANDLER(NM_CLICK, OnNMClick)
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
		LRESULT lRes = CComControl<CRequestList>::OnSetFocus(uMsg, wParam, lParam, bHandled);
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
		m_listview.InsertColumn(0,_T("Request List"),LVCFMT_CENTER,rc.right,0);

		return 0;
	}

	STDMETHOD(SetObjectRects)(LPCRECT prcPos,LPCRECT prcClip)
	{
		IOleInPlaceObjectWindowlessImpl<CRequestList>::SetObjectRects(prcPos, prcClip);
		int cx, cy;
		cx = prcPos->right - prcPos->left;
		cy = prcPos->bottom - prcPos->top;
		::SetWindowPos(m_listview.m_hWnd, NULL, 0,
			0, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);
		return S_OK;
	}

	// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

	// IRequestList

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}
private:
	void AcceptRequest(ULONG ulTStatusId,TCHAR* additional=_T(""));
	void SaveAsRequest(ULONG ulTStatusId);
	void RejectRequest(ULONG ulTStatusId);
	void CancelTask(ULONG ulTStatusId);

	int FindTittle(ULONG ulTStatusId);
	int FindProgress(ULONG ulTStatusId);
	int FindButtons(ULONG ulTStatusId);

	//int FindRequestIndex(ULONG ulTStatusId);
	//void InsertRequest(ULONG ulUserid,tTStatus* ptstatus);	//actually it's append	
	//bool UpdateRequest(ULONG ulUserid,int index,tTStatus* ptstatus);

	////////////////////////////////////////////////////////////
	// EVENT handlers
	////////////////////////////////////////////////////////////
	LRESULT OnNotify(UINT nMessage, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//LRESULT OnNMClick(UINT nMessage, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

public:
////////////////////////////////////////////////////////////////
// IRequestList interface
////////////////////////////////////////////////////////////////
	STDMETHOD(AddItem)(ULONG tstatusid, BSTR tittle);
	STDMETHOD(DeleteItem)(ULONG tstatusid);
	STDMETHOD(SetButtonYesNo)(ULONG tstatusid);
	STDMETHOD(SetButtonYesAsNo)(ULONG tstatusid);
	STDMETHOD(SetButtonCancel)(ULONG tstatusid);
	STDMETHOD(SetProgress)(ULONG tstatusid, ULONG percentage);
};

OBJECT_ENTRY_AUTO(__uuidof(RequestList), CRequestList)
