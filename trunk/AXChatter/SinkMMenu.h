// SinkMMenu.h : Declaration of the CSinkMMenu

#pragma once
#include "resource.h"       // main symbols

#include "AXChatter.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// CSinkMMenu

class ATL_NO_VTABLE CSinkMMenu :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSinkMMenu, &CLSID_SinkMMenu>,
	public IDispatchImpl<ISinkMMenu, &IID_ISinkMMenu, &LIBID_AXChatterLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CSinkMMenu()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SINKMMENU)


BEGIN_COM_MAP(CSinkMMenu)
	COM_INTERFACE_ENTRY(ISinkMMenu)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY_IID(DIID__IChatterMMenuEvents,IDispatch)
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
// _IChatterMMenuEvents interface
/////////////////////////////////////////////////////////////
	STDMETHOD(InviteCurVoiceChat)(void);
	STDMETHOD(InviteCurVideoChat)(void);
	STDMETHOD(TransferCurFile)(void);
	STDMETHOD(TransferCurFileOffline)(void);
};

OBJECT_ENTRY_AUTO(__uuidof(SinkMMenu), CSinkMMenu)
