// xchatter.h : Declaration of the Cxchatter
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include "chatter.hxx"
#include "axchatter.h"
#include "atlcontrols.h"

using namespace ATLControls;

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// Cxchatter
class ATL_NO_VTABLE Cxchatter :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<Ixchatter, &IID_Ixchatter, &LIBID_axchatterLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IPersistStreamInitImpl<Cxchatter>,
	public IOleControlImpl<Cxchatter>,
	public IOleObjectImpl<Cxchatter>,
	public IOleInPlaceActiveObjectImpl<Cxchatter>,
	public IViewObjectExImpl<Cxchatter>,
	public IOleInPlaceObjectWindowlessImpl<Cxchatter>,
	public ISupportErrorInfo,
	public IPersistStorageImpl<Cxchatter>,
	public ISpecifyPropertyPagesImpl<Cxchatter>,
	public IQuickActivateImpl<Cxchatter>,
#ifndef _WIN32_WCE
	public IDataObjectImpl<Cxchatter>,
#endif
	public IProvideClassInfo2Impl<&CLSID_xchatter, NULL, &LIBID_axchatterLib>,
#ifdef _WIN32_WCE // IObjectSafety is required on Windows CE for the control to be loaded correctly
	public IObjectSafetyImpl<Cxchatter, INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
#endif
	public CComCoClass<Cxchatter, &CLSID_xchatter>,
	public CComCompositeControl<Cxchatter>
{
public:


	Cxchatter():m_lMyId(0),m_bstrMyName(),m_pobjChatter(NULL)
	{
		m_bWindowOnly = TRUE;
		CalcExtent(m_sizeExtent);
	}

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
	COM_INTERFACE_ENTRY(IPersistStorage)
#ifndef _WIN32_WCE
	COM_INTERFACE_ENTRY(IDataObject)
#endif
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
	}
public:
	STDMETHOD(get_MyId)(LONG* pVal);
	STDMETHOD(put_MyId)(LONG newVal);
	STDMETHOD(get_MyName)(BSTR* pVal);
	STDMETHOD(put_MyName)(BSTR newVal);
	STDMETHOD(Start)(void);
	STDMETHOD(InvitePeerVoiceChat)(long peerid);
private:
////////////////////////////////////////////////////
// properties variables
////////////////////////////////////////////////////
	long m_lMyId;
	CComBSTR m_bstrMyName;

////////////////////////////////////////////////////
// private variables
////////////////////////////////////////////////////
	Chatter* m_pobjChatter;

////////////////////////////////////////////////////
// GUI variables
////////////////////////////////////////////////////
	CComPtr<IDispatch> m_spDispatch;
	CContainedWindowT<CTabCtrl, CWinTraitsOR<LVS_REPORT | LVS_EX_GRIDLINES | LVS_NOSORTHEADER | LVS_SINGLESEL, 0/*WS_EX_CLIENTEDGE*/> > m_tab;
	//CContainedWindowT<CEdit, CWinTraitsOR<WS_BORDER | ES_AUTOHSCROLL> > m_edit;
	//CContainedWindowT<CComboBox, CWinTraitsOR<CBS_DROPDOWNLIST> > m_combobox;
	//CContainedWindowT<CStatic, CWinTraits<SS_LEFT, 0> > m_wndDesc;
	
};

OBJECT_ENTRY_AUTO(__uuidof(xchatter), Cxchatter)
