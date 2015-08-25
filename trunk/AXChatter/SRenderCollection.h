// SRenderCollection.h : Declaration of the CSRenderCollection
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include <map>
#include <string>
#include <config.hxx>
#include "AXChatter.h"
#include "_ISRenderCollectionEvents_CP.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

class CRenderDlg;

// CSRenderCollection
class ATL_NO_VTABLE CSRenderCollection :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<ISRenderCollection, &IID_ISRenderCollection, &LIBID_AXChatterLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IPersistStreamInitImpl<CSRenderCollection>,
	public IOleControlImpl<CSRenderCollection>,
	public IOleObjectImpl<CSRenderCollection>,
	public IOleInPlaceActiveObjectImpl<CSRenderCollection>,
	public IViewObjectExImpl<CSRenderCollection>,
	public IOleInPlaceObjectWindowlessImpl<CSRenderCollection>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CSRenderCollection>,
	public CProxy_ISRenderCollectionEvents<CSRenderCollection>,
	public IPersistStorageImpl<CSRenderCollection>,
	public ISpecifyPropertyPagesImpl<CSRenderCollection>,
	public IQuickActivateImpl<CSRenderCollection>,
#ifndef _WIN32_WCE
	public IDataObjectImpl<CSRenderCollection>,
#endif
	public IProvideClassInfo2Impl<&CLSID_SRenderCollection, &__uuidof(_ISRenderCollectionEvents), &LIBID_AXChatterLib>,
#ifdef _WIN32_WCE // IObjectSafety is required on Windows CE for the control to be loaded correctly
	public IObjectSafetyImpl<CSRenderCollection, INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
#endif
	public CComCoClass<CSRenderCollection, &CLSID_SRenderCollection>,
	public CComControl<CSRenderCollection>
{
public:
////////////////////////////////////////////////////////////////////////
// typedef
////////////////////////////////////////////////////////////////////////
	typedef std::map<std::string,CRenderDlg*> tFromRenderMap;
	typedef tFromRenderMap::iterator tFromRenderMapIter;
	typedef tFromRenderMap::const_iterator tFromRenderMapConstIter;

	CSRenderCollection()
	{
		m_pImageBuf = new BYTE[IMAGE_LEN];
	}

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_INSIDEOUT |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_SRENDERCOLLECTION)


BEGIN_COM_MAP(CSRenderCollection)
	COM_INTERFACE_ENTRY(ISRenderCollection)
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
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
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

BEGIN_PROP_MAP(CSRenderCollection)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// Example entries
	// PROP_ENTRY("Property Description", dispid, clsid)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()

BEGIN_CONNECTION_POINT_MAP(CSRenderCollection)
	CONNECTION_POINT_ENTRY(__uuidof(_ISRenderCollectionEvents))
END_CONNECTION_POINT_MAP()

BEGIN_MSG_MAP(CSRenderCollection)
	MESSAGE_HANDLER(WM_TIMER, OnTimer)
	CHAIN_MSG_MAP(CComControl<CSRenderCollection>)
	DEFAULT_REFLECTION_HANDLER()
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid)
	{
		static const IID* arr[] =
		{
			&IID_ISRenderCollection,
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

// ISRenderCollection
public:
		HRESULT OnDraw(ATL_DRAWINFO& di)
		{
		RECT& rc = *(RECT*)di.prcBounds;
		// Set Clip region to the rectangle specified by di.prcBounds
		HRGN hRgnOld = NULL;
		if (GetClipRgn(di.hdcDraw, hRgnOld) != 1)
			hRgnOld = NULL;
		bool bSelectOldRgn = false;

		HRGN hRgnNew = CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);

		if (hRgnNew != NULL)
		{
			bSelectOldRgn = (SelectClipRgn(di.hdcDraw, hRgnNew) != ERROR);
		}

		Rectangle(di.hdcDraw, rc.left, rc.top, rc.right, rc.bottom);
		SetTextAlign(di.hdcDraw, TA_CENTER|TA_BASELINE);
		LPCTSTR pszText = _T("ATL 8.0 : SRenderCollection");
#ifndef _WIN32_WCE
		TextOut(di.hdcDraw,
			(rc.left + rc.right) / 2,
			(rc.top + rc.bottom) / 2,
			pszText,
			lstrlen(pszText));
#else
		ExtTextOut(di.hdcDraw,
			(rc.left + rc.right) / 2,
			(rc.top + rc.bottom) / 2,
			ETO_OPAQUE,
			NULL,
			pszText,
			ATL::lstrlen(pszText),
			NULL);
#endif

		if (bSelectOldRgn)
			SelectClipRgn(di.hdcDraw, hRgnOld);

		return S_OK;
	}


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
		delete[] m_pImageBuf;
	}
	
	void GetRectForNewRender(RECT& rc);

	STDMETHOD(CreateRender)(BYTE* host, ULONG port, BYTE* caption);
	STDMETHOD(DestroyRender)(BYTE* host, ULONG port);
	STDMETHOD(Play)(ULONG interval);
	STDMETHOD(Stop)(void);

private:
	tFromRenderMap m_mapFromRender;
	BYTE* m_pImageBuf;

	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

OBJECT_ENTRY_AUTO(__uuidof(SRenderCollection), CSRenderCollection)
