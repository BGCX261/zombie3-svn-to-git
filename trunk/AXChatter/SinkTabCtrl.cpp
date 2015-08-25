// SinkTabCtrl.cpp : Implementation of CSinkTabCtrl

#include "stdafx.h"
#include "xchatter.h"
#include "SinkTabCtrl.h"

extern Cxchatter* g_pTheChatter;

// CSinkTabCtrl
STDMETHODIMP CSinkTabCtrl::SendInstantMsg(ULONG userid, BSTR username, BSTR instantmsg)
{
	if (!g_pTheChatter)
		return S_FALSE;

	g_pTheChatter->SendInstantMsg(userid,username,instantmsg);

	return S_OK;
}
