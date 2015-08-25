// SinkRequestList.cpp : Implementation of CSinkRequestList

#include "stdafx.h"
#include "xchatter.h"
#include "SinkRequestList.h"

extern Cxchatter* g_pTheChatter;

 //CSinkRequestList
STDMETHODIMP CSinkRequestList::AcceptRequest(ULONG tstatusid, BSTR additional)
{
	//::MessageBox(NULL,_T("AcceptRequest"),_T("CSinkRequestList"),MB_OK);

	if (!g_pTheChatter)
		return S_FALSE;

	return g_pTheChatter->AcceptRequest(tstatusid,additional);
}

STDMETHODIMP CSinkRequestList::RejectRequest(ULONG tstatusid)
{
	//::MessageBox(NULL,_T("RejectRequest"),_T("CSinkRequestList"),MB_OK);

	if (!g_pTheChatter)
		return S_FALSE;

	return g_pTheChatter->RejectRequest(tstatusid);
}

STDMETHODIMP CSinkRequestList::CancelTask(ULONG tstatusid)
{
	//::MessageBox(NULL,_T("CancelTask"),_T("CSinkRequestList"),MB_OK);

	if (!g_pTheChatter)
		return S_FALSE;

	return g_pTheChatter->CancelTask(tstatusid);
}
