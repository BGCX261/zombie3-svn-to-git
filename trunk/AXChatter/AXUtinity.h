#pragma once

class AXUtinity
{
public:
	static bool String2TString(const char* str,LPTSTR buf,ULONG buflen);
	static bool String2WString(const char* str,LPWSTR buf,ULONG buflen);
	static bool TString2String(const TCHAR* str,LPSTR buf,ULONG buflen);
	static bool WString2String(const wchar_t* str,LPSTR buf,ULONG buflen);

private:
	AXUtinity(void);
	~AXUtinity(void);
};
