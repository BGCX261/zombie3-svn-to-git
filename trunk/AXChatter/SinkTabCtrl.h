// SinkTabCtrl.h : Declaration of the CSinkTabCtrl

#pragma once
#include "resource.h"       // main symbols

#include "AXChatter.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif


// CSinkTabCtrl
class ATL_NO_VTABLE CSinkTabCtrl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSinkTabCtrl, &CLSID_SinkTabCtrl>,
	public IDispatchImpl<ISinkTabCtrl, &IID_ISinkTabCtrl, &LIBID_AXChatterLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CSinkTabCtrl()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SINKTABCTRL)


BEGIN_COM_MAP(CSinkTabCtrl)
	COM_INTERFACE_ENTRY(ISinkTabCtrl)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY_IID(DIID__IChatterTabCtrlEvents,IDispatch)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
/////////////////////////////////////////////////////////////
// _IChatterTabCtrlEvents interface
/////////////////////////////////////////////////////////////
	STDMETHOD(SendInstantMsg)(ULONG userid, BSTR username, BSTR instantmsg);
};

OBJECT_ENTRY_AUTO(__uuidof(SinkTabCtrl), CSinkTabCtrl)
