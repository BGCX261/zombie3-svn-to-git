// ChatterMMenu.h : Declaration of the CChatterMMenu
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include "AXChatter.h"
#include "_IChatterMMenuEvents_CP.h"
#include <commctrl.h>
#include "ATLControls.h"

using namespace ATLControls;

//#define COLOR_DCHATTER RGB(40, 215, 175)

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

class CMMenuDlg;

// CChatterMMenu
class ATL_NO_VTABLE CChatterMMenu :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IChatterMMenu, &IID_IChatterMMenu, &LIBID_AXChatterLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IPersistStreamInitImpl<CChatterMMenu>,
	public IOleControlImpl<CChatterMMenu>,
	public IOleObjectImpl<CChatterMMenu>,
	public IOleInPlaceActiveObjectImpl<CChatterMMenu>,
	public IViewObjectExImpl<CChatterMMenu>,
	public IOleInPlaceObjectWindowlessImpl<CChatterMMenu>,
	public IConnectionPointContainerImpl<CChatterMMenu>,
	public CProxy_IChatterMMenuEvents<CChatterMMenu>,
#ifdef _WIN32_WCE // IObjectSafety is required on Windows CE for the control to be loaded correctly
	public IObjectSafetyImpl<CChatterMMenu, INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
#endif
	public CComCoClass<CChatterMMenu, &CLSID_ChatterMMenu>,
	public CComControl<CChatterMMenu>
{
public:
	//CContainedWindow m_tab;
	CContainedWindowT<CTabCtrl,CWinTraitsOR<WS_CHILD|WS_VISIBLE| TCS_FOCUSNEVER|TCS_OWNERDRAWFIXED>> m_tab;

#pragma warning(push)
#pragma warning(disable: 4355) // 'this' : used in base member initializer list


	CChatterMMenu();
	~CChatterMMenu();
#pragma warning(pop)

	DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_CANTLINKINSIDE |
		OLEMISC_INSIDEOUT |
		OLEMISC_ACTIVATEWHENVISIBLE |
		OLEMISC_SETCLIENTSITEFIRST
		)

		DECLARE_REGISTRY_RESOURCEID(IDR_CHATTERMMENU)


	BEGIN_COM_MAP(CChatterMMenu)
		COM_INTERFACE_ENTRY(IChatterMMenu)
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

	BEGIN_PROP_MAP(CChatterMMenu)
		PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
		PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
		// Example entries
		// PROP_ENTRY("Property Description", dispid, clsid)
		// PROP_PAGE(CLSID_StockColorPage)
	END_PROP_MAP()

	BEGIN_CONNECTION_POINT_MAP(CChatterMMenu)
		CONNECTION_POINT_ENTRY(__uuidof(_IChatterMMenuEvents))
	END_CONNECTION_POINT_MAP()

	BEGIN_MSG_MAP(CChatterMMenu)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		CHAIN_MSG_MAP(CComControl<CChatterMMenu>)
		ALT_MSG_MAP(1)
		// Replace this with message map entries for superclassed SysTabControl32
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
		LRESULT lRes = CComControl<CChatterMMenu>::OnSetFocus(uMsg, wParam, lParam, bHandled);
		if (m_bInPlaceActive)
		{
			if(!IsChild(::GetFocus()))
				m_tab.SetFocus();
		}
		return lRes;
	}

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	STDMETHOD(SetObjectRects)(LPCRECT prcPos,LPCRECT prcClip)
	{
		IOleInPlaceObjectWindowlessImpl<CChatterMMenu>::SetObjectRects(prcPos, prcClip);
		int cx, cy;
		cx = prcPos->right - prcPos->left;
		cy = prcPos->bottom - prcPos->top;
		::SetWindowPos(m_tab.m_hWnd, NULL, 0,
			0, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);
		return S_OK;
	}

	// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

	// IChatterMMenu

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:

private:
/////////////////////////////////////////////////////////////////
// private methods
/////////////////////////////////////////////////////////////////
	void ActivateTabDialogs(void);

/////////////////////////////////////////////////////////////////
// private members
/////////////////////////////////////////////////////////////////
	CMMenuDlg* m_Dialog0;
	HBRUSH m_hDialogBrush;
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDrawItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};

OBJECT_ENTRY_AUTO(__uuidof(ChatterMMenu), CChatterMMenu)
