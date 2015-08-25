#include "stdafx.h"
#include "AXUtinity.h"

AXUtinity::AXUtinity(void)
{
}

AXUtinity::~AXUtinity(void)
{
}

bool AXUtinity::String2TString(const char* str,LPTSTR buf,ULONG buflen)
{
#if defined(_UNICODE)
	return String2WString(str,buf,buflen);
#else
	memcpy(...);
	return TRUE;
#endif
}
bool AXUtinity::String2WString(const char* str,LPWSTR buf,ULONG buflen)
{
	// Get the required size of the buffer that receives the Unicode
    // string.
    DWORD dwMinSize;
    dwMinSize = MultiByteToWideChar (CP_ACP, 0, str, -1, NULL, 0);
 
    if(buflen < dwMinSize)
    {
     return FALSE;
    }
 
    
    // Convert headers from ASCII to Unicode.
    MultiByteToWideChar (CP_ACP, 0, str, -1, buf, dwMinSize);  
    return TRUE;
}
bool AXUtinity::TString2String(const TCHAR* str,LPSTR buf,ULONG buflen)
{
#if defined(_UNICODE)
	return WString2String(str,buf,buflen);
#else
	memcpy(...);
	return TRUE;
#endif
}
bool AXUtinity::WString2String(const wchar_t* str,LPSTR buf,ULONG buflen)
{
	DWORD dwMinSize;
	dwMinSize = WideCharToMultiByte(CP_OEMCP,NULL,str,-1,NULL,0,NULL,FALSE);
	
	if(buflen < dwMinSize)
	{
		return FALSE;
	}

	WideCharToMultiByte(CP_OEMCP,NULL,str,-1,buf,buflen,NULL,FALSE);

	return TRUE;
}