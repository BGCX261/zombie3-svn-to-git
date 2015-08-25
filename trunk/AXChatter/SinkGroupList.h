// SinkGroupList.h : Declaration of the CSinkGroupList

#pragma once
#include "resource.h"       // main symbols

#include "AXChatter.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CSinkGroupList

class ATL_NO_VTABLE CSinkGroupList :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSinkGroupList, &CLSID_SinkGroupList>,
	public IDispatchImpl<ISinkGroupList, &IID_ISinkGroupList, &LIBID_AXChatterLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CSinkGroupList()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_SINKGROUPLIST)


BEGIN_COM_MAP(CSinkGroupList)
	COM_INTERFACE_ENTRY(ISinkGroupList)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY_IID(DIID__IGroupListEvents,IDispatch)
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
	STDMETHOD(DeleteMember)(ULONG userid);
};

OBJECT_ENTRY_AUTO(__uuidof(SinkGroupList), CSinkGroupList)
