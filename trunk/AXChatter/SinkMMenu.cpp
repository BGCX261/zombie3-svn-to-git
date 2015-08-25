// SinkMMenu.cpp : Implementation of CSinkMMenu

#include "stdafx.h"
#include "xchatter.h"
#include "SinkMMenu.h"

extern Cxchatter* g_pTheChatter;

// CSinkMMenu
STDMETHODIMP CSinkMMenu::InviteCurVoiceChat(void)
{
	//::MessageBox(NULL,_T("InviteCurVoiceChat"),_T("CSinkMMenu"),MB_OK);

	g_pTheChatter->InviteCurVoiceChat();

	return S_OK;
}

STDMETHODIMP CSinkMMenu::InviteCurVideoChat(void)
{
	//::MessageBox(NULL,_T("InviteCurVideoChat"),_T("CSinkMMenu"),MB_OK);

	g_pTheChatter->InviteCurVideoChat();

	return S_OK;
}

STDMETHODIMP CSinkMMenu::TransferCurFile(void)
{
	//::MessageBox(NULL,_T("TransferCurFile"),_T("CSinkMMenu"),MB_OK);

	g_pTheChatter->TransferCurFile();

	return S_OK;
}

STDMETHODIMP CSinkMMenu::TransferCurFileOffline(void)
{
	::MessageBox(NULL,_T("TransferCurFileOffline"),_T("CSinkMMenu"),MB_OK);

	g_pTheChatter->TransferCurFileOffline();

	return S_OK;
}
