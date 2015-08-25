// AXChatter.cpp : Implementation of DLL Exports.


#include "stdafx.h"
#include "resource.h"
#include "AXChatter.h"


class CAXChatterModule : public CAtlDllModuleT< CAXChatterModule >
{
public :
	DECLARE_LIBID(LIBID_AXChatterLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_AXCHATTER, "{706E31E3-D9EA-452A-BC26-F8B4B6DA9DCB}")
};

CAXChatterModule _AtlModule;


#ifdef _MANAGED
#pragma managed(push, off)
#endif

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	hInstance;
	//HICON hicon = LoadIcon(hInstance,MAKEINTRESOURCE(IDI_ICON1));//for test
	
    return _AtlModule.DllMain(dwReason, lpReserved); 
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
    return _AtlModule.DllCanUnloadNow();
}


// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}


// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    HRESULT hr = _AtlModule.DllRegisterServer();
	return hr;
}


// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer();
	return hr;
}

