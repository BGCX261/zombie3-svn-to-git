// SinkGroupList.cpp : Implementation of CSinkGroupList

#include "stdafx.h"
#include "xchatter.h"
#include "SinkGroupList.h"

extern Cxchatter* g_pTheChatter;

STDMETHODIMP CSinkGroupList::DeleteMember(ULONG userid)
{
	if (!g_pTheChatter)
		return S_FALSE;

	g_pTheChatter->DeleteMember(userid);

	return S_OK;
}