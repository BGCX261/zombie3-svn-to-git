#pragma once

/**
 * to exclude the winsock.h in windows.h file
 */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include "common.h"
//
//#ifndef LPWSTR
//#define LPWSTR	short
//#endif
//#ifndef LPSTR
//#define LPSTR	char
//#endif

class EXPORT_COMMON CommonUtility
{
public:
	//static bool String2TString(const char* str,LPTSTR buf,ULONG buflen);
	static bool String2WString(const char* str,LPWSTR buf,ULONG buflen);
	//static bool TString2String(const TCHAR* str,LPSTR buf,ULONG buflen);
	static bool WString2String(const wchar_t* str,LPSTR buf,ULONG buflen);

private:
	CommonUtility(void);
	CommonUtility(const CommonUtility& model);
	~CommonUtility(void);
};
