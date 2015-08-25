// xchatter.h : Declaration of the Cxchatter
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include <chatter.hxx>
#include "AXChatter.h"
#include "ChatterTabCtrl.h"
#include "ChatterMMenu.h"
#include "RequestList.h"
#include "GroupList.h"
#include "SRenderCollection.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// Cxchatter
class ATL_NO_VTABLE Cxchatter :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<Ixchatter, &IID_Ixchatter, &LIBID_AXChatterLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IPersistStreamInitImpl<Cxchatter>,
	public IOleControlImpl<Cxchatter>,
	public IOleObjectImpl<Cxchatter>,
	public IOleInPlaceActiveObjectImpl<Cxchatter>,
	public IViewObjectExImpl<Cxchatter>,
	public IOleInPlaceObjectWindowlessImpl<Cxchatter>,
	public ISupportErrorInfo,
	public ISpecifyPropertyPagesImpl<Cxchatter>,
	public IQuickActivateImpl<Cxchatter>,
	public IProvideClassInfo2Impl<&CLSID_xchatter, NULL, &LIBID_AXChatterLib>,
#ifdef _WIN32_WCE // IObjectSafety is required on Windows CE for the control to be loaded correctly
	public IObjectSafetyImpl<Cxchatter, INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
#endif
	public CComCoClass<Cxchatter, &CLSID_xchatter>,
	public CComCompositeControl<Cxchatter>
{
public:

	Cxchatter();
	~Cxchatter();

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_INSIDEOUT |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_XCHATTER)


BEGIN_COM_MAP(Cxchatter)
	COM_INTERFACE_ENTRY(Ixchatter)
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
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(ISpecifyPropertyPages)
	COM_INTERFACE_ENTRY(IQuickActivate)
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
#ifdef _WIN32_WCE // IObjectSafety is required on Windows CE for the control to be loaded correctly
	COM_INTERFACE_ENTRY_IID(IID_IObjectSafety, IObjectSafety)
#endif
END_COM_MAP()

BEGIN_PROP_MAP(Cxchatter)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// Example entries
	// PROP_ENTRY("Property Description", dispid, clsid)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()


BEGIN_MSG_MAP(Cxchatter)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	MESSAGE_HANDLER(WM_UPDATEGUI, OnUpdateTheGUI)
	MESSAGE_HANDLER(WM_CTLCOLORDLG, OnCtlColorDlg)
	MESSAGE_HANDLER(WM_CHANGE2RENDER, OnChange2Render)
	CHAIN_MSG_MAP(CComCompositeControl<Cxchatter>)
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

BEGIN_SINK_MAP(Cxchatter)
	//Make sure the Event Handlers have __stdcall calling convention
END_SINK_MAP()

	STDMETHOD(OnAmbientPropertyChange)(DISPID dispid)
	{
		if (dispid == DISPID_AMBIENT_BACKCOLOR)
		{
			SetBackgroundColorFromAmbient();
			FireViewChange();
		}
		return IOleControlImpl<Cxchatter>::OnAmbientPropertyChange(dispid);
	}
// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid)
	{
		static const IID* arr[] =
		{
			&IID_Ixchatter,
		};

		for (int i=0; i<sizeof(arr)/sizeof(arr[0]); i++)
		{
			if (InlineIsEqualGUID(*arr[i], riid))
				return S_OK;
		}
		return S_FALSE;
	}

// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

// Ixchatter

	enum { IDD = IDD_XCHATTER };

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
		if (m_pChatter) m_pChatter->Shutdown();

		ULONG counter = 1000;
		while (m_pChatter && --counter>0)
		{
			if (m_pChatter->State()!=Thread::FINISHED_STATE)
				Thread::Yield();
			else
			{
				m_pChatter->Join();
				delete m_pChatter;
				m_pChatter = NULL;
			}
		}

		if (m_pChatter) 
		{
			delete m_pChatter;
			m_pChatter = NULL;
		}
	}
private:
////////////////////////////////////////////////////////////
// private methods
////////////////////////////////////////////////////////////
	BOOL ConnectMMenu();
	BOOL ConnectTabCtrl();
	BOOL ConnectRequestList();
	BOOL ConnectGroupList();
	BOOL AmIRunning() const;

	LRESULT InvitePeerVoiceChat(ULONG userid,char* username);
	LRESULT InvitePeerVideoChat(ULONG userid,char* username);
	
////////////////////////////////////////////////////////////
// private methods
////////////////////////////////////////////////////////////
	ULONG		m_ulMyId;
	CComBSTR	m_bstrMyName;
	Chatter*	m_pChatter;

	HBRUSH		m_hDialogBrush;

	IUnknown*	m_pUnkCP;

	// the tab control
	IUnknown*	m_pUnkTabCtrlSink;
	DWORD		m_dwTabCtrl;
	CComPtr<IChatterTabCtrl> m_spTabCtrl;
	CAxWindow m_wndTabCtrl;

	// the menu control
	IUnknown*	m_pUnkMMenuSink;
	DWORD		m_dwMMenu;
	CComPtr<IChatterMMenu> m_spMMenu;
	CAxWindow m_wndMMenu;

	// the request list
	IUnknown*	m_pUnkRequestListSink;
	DWORD		m_dwRequestList;
	CComPtr<IRequestList> m_spRequestList;
	CAxWindow m_wndRequestList;

	// the group list
	IUnknown*	m_pUnkGroupListSink;
	DWORD		m_dwGroupList;
	CComPtr<IGroupList> m_spGroupList;
	CAxWindow m_wndGroupList;

	// the render collection
	//IUnknown*	m_pUnkRenderCollectionSink;
	DWORD		m_dwRenderCollection;
	CComPtr<ISRenderCollection> m_spRenderCollection;
	CAxWindow m_wndRenderCollection;

public:
////////////////////////////////////////////////////////////
// public methods
////////////////////////////////////////////////////////////
	LRESULT SendInstantMsg(ULONG userid,BSTR username,BSTR instantmsg);
	LRESULT InviteCurVoiceChat(void);
	LRESULT InviteCurVideoChat(void);
	LRESULT TransferCurFile(void);
	LRESULT TransferCurFileOffline(void);
	LRESULT AcceptRequest(ULONG tstatusid, BSTR additional);
	LRESULT RejectRequest(tUInt32 tstatusid);
	LRESULT CancelTask(tUInt32 tstatusid);
	LRESULT DeleteMember(tUInt32 userid);

	void UpdateChatterState();
	//void UpdateMenu(tUInt32 userid,BSTR username,const tTStatus* tstatus);
	void UpdateTab(const tTStatus* tstatus);
	void UpdateReqestList(const tTStatus* tstatus);
	void UpdateGroup(const tTStatus* tstatus);
	void AdjustChildrenPoses(void);

	BOOL GetImage(const BYTE* from,BYTE* buf,DWORD bufsize);

	//void OnUpdateTheGUI(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//void OnAddRemoveRender(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

////////////////////////////////////////////////////////////
// Event Handlers
////////////////////////////////////////////////////////////
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnUpdateTheGUI(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCtlColorDlg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnChange2Render(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

////////////////////////////////////////////////////////////
// Ixchatter interface
////////////////////////////////////////////////////////////
	STDMETHOD(InstantMsgChat)(ULONG userid, BSTR username);	// Start a tab session
	STDMETHOD(get_MyId)(ULONG* pVal);
	STDMETHOD(put_MyId)(ULONG newVal);
	STDMETHOD(get_MyName)(BSTR* pVal);
	STDMETHOD(put_MyName)(BSTR newVal);
	STDMETHOD(Start)(void);
	STDMETHOD(InvitePeerVoiceChat)(ULONG userid, BSTR username);
public:
	STDMETHOD(InvitePeerVideoChat)(ULONG userid, BSTR username);
};

OBJECT_ENTRY_AUTO(__uuidof(xchatter), Cxchatter)
