// ChatterTabCtrl.h : Declaration of the CChatterTabCtrl
#pragma once
#include "resource.h"       // main symbols
#include <vector>
#include <atlctl.h>
#include "AXChatter.h"
#include "_IChatterTabCtrlEvents_CP.h"
#include <commctrl.h>

#include "atlcontrols.h"
using namespace ATLControls;

#define WM_CLOSEUSERDLG	(WM_USER+1)
#define WM_SENDMSG		(WM_USER+2)

//#define COLOR_DCHATTER RGB(40, 215, 175)

class CUserDlg;

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif


// CChatterTabCtrl
class ATL_NO_VTABLE CChatterTabCtrl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IChatterTabCtrl, &IID_IChatterTabCtrl, &LIBID_AXChatterLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IPersistStreamInitImpl<CChatterTabCtrl>,
	public IOleControlImpl<CChatterTabCtrl>,
	public IOleObjectImpl<CChatterTabCtrl>,
	public IOleInPlaceActiveObjectImpl<CChatterTabCtrl>,
	public IViewObjectExImpl<CChatterTabCtrl>,
	public IOleInPlaceObjectWindowlessImpl<CChatterTabCtrl>,
	public IConnectionPointContainerImpl<CChatterTabCtrl>,
	public CProxy_IChatterTabCtrlEvents<CChatterTabCtrl>,
	public IQuickActivateImpl<CChatterTabCtrl>,
	public IProvideClassInfo2Impl<&CLSID_ChatterTabCtrl, &__uuidof(_IChatterTabCtrlEvents), &LIBID_AXChatterLib>,
#ifdef _WIN32_WCE // IObjectSafety is required on Windows CE for the control to be loaded correctly
	public IObjectSafetyImpl<CChatterTabCtrl, INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
#endif
	public CComCoClass<CChatterTabCtrl, &CLSID_ChatterTabCtrl>,
	public CComControl<CChatterTabCtrl>
{
public:
////////////////////////////////////////////////////////////////////
// typedefs
////////////////////////////////////////////////////////////////////
typedef std::vector<CUserDlg*>	tUserDlgVec;
typedef tUserDlgVec::iterator	tUserDlgVecIter;


#pragma warning(push)
#pragma warning(disable: 4355) // 'this' : used in base member initializer list


	CChatterTabCtrl();
	~CChatterTabCtrl();

#pragma warning(pop)

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_INSIDEOUT |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_CHATTERTABCTRL)


BEGIN_COM_MAP(CChatterTabCtrl)
	COM_INTERFACE_ENTRY(IChatterTabCtrl)
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

BEGIN_PROP_MAP(CChatterTabCtrl)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// Example entries
	// PROP_ENTRY("Property Description", dispid, clsid)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()

BEGIN_CONNECTION_POINT_MAP(CChatterTabCtrl)
	CONNECTION_POINT_ENTRY(__uuidof(_IChatterTabCtrlEvents))
END_CONNECTION_POINT_MAP()

BEGIN_MSG_MAP(CChatterTabCtrl)
	MESSAGE_HANDLER(WM_CREATE, OnCreate)
	MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
	MESSAGE_HANDLER(WM_CLOSEUSERDLG,OnCloseUserDlg)	//sent by userdlg
	MESSAGE_HANDLER(WM_SENDMSG,OnSendDlgInputMsg)	//sent by userdlg
	NOTIFY_HANDLER(101,TCN_SELCHANGE,OnTabSelChange)
	MESSAGE_HANDLER(WM_SIZE, OnSize)
	MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
	//MESSAGE_HANDLER(WM_CTLCOLOR_REFLECT, OnCtlColor)
	CHAIN_MSG_MAP(CComControl<CChatterTabCtrl>)
ALT_MSG_MAP(1)
	// Replace this with message map entries for superclassed SysTabControl32
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	BOOL PreTranslateAccelerator(LPMSG pMsg, HRESULT& hRet)
	{
		return HandEventToUserDlg(pMsg, hRet);
	}

	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lRes = CComControl<CChatterTabCtrl>::OnSetFocus(uMsg, wParam, lParam, bHandled);
		if (m_bInPlaceActive)
		{
			if(!IsChild(::GetFocus()))
				m_tab.SetFocus();
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
		//m_tab.Create(m_hWnd, rc);
		m_tab.Create(this, 1, m_hWnd, &rc, NULL, 0,0, 101);
		m_tab.SetExtendedStyle(0,WS_BORDER);			//make it of no border.

		return 0;
	}

	STDMETHOD(SetObjectRects)(LPCRECT prcPos,LPCRECT prcClip)
	{
		IOleInPlaceObjectWindowlessImpl<CChatterTabCtrl>::SetObjectRects(prcPos, prcClip);
		int cx, cy;
		cx = prcPos->right - prcPos->left;
		cy = prcPos->bottom - prcPos->top;
		::SetWindowPos(m_tab.m_hWnd, NULL, 0,
			0, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);
		return S_OK;
	}

	LRESULT OnCloseUserDlg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSendDlgInputMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTabSelChange(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);

// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

// IChatterTabCtrl

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

private:
/////////////////////////////////////////////////////////////////
// private methods
/////////////////////////////////////////////////////////////////
	CUserDlg* AddUserDlg(unsigned long userid,LPTSTR username,LPTSTR myname);
	CUserDlg* FindUserDlgByUserid(unsigned long userid);
	int FindIndexByUserid(unsigned long userid);
	BOOL HandEventToUserDlg(LPMSG pMsg, HRESULT& hRet);
	void ActivateTabDialogs(void);
	void SetItemDirty(int nItem);
	void SetItemClean(int nItem);
	bool RemoveItem(int nItem);
	CUserDlg* GetCurUserDlg(void);

	

/////////////////////////////////////////////////////////////////
// private members
/////////////////////////////////////////////////////////////////
	CContainedWindowT<CTabCtrl,CWinTraitsOR<WS_CHILD|WS_VISIBLE| TCS_FOCUSNEVER|TCS_OWNERDRAWFIXED>> m_tab;
	tUserDlgVec m_userdlgVec;
	HBRUSH m_hDialogBrush;

	STDMETHOD(AddItem)(ULONG userid, BSTR username, BSTR myname);
	STDMETHOD(DeleteItem)(ULONG userid);
	STDMETHOD(SetCurSel)(ULONG userid);
	STDMETHOD(GetCurSel)(ULONG* userid);
	STDMETHOD(GetCurSelName)(BSTR* username);
	STDMETHOD(GetItemCount)(LONG* lCount);
	STDMETHOD(SetImageList)(LONG pImageList);
	STDMETHOD(DisplayReceivedUserMsg)(ULONG userid, BSTR username, BSTR myname, BSTR instantmsg);
	STDMETHOD(DisplayNotification)(ULONG userid, BSTR username, BSTR myname, BSTR notification);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDrawItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCtlColor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

OBJECT_ENTRY_AUTO(__uuidof(ChatterTabCtrl), CChatterTabCtrl)
