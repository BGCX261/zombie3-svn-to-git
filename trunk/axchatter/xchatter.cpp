// xchatter.cpp : Implementation of Cxchatter
#include "stdafx.h"
#include "xchatter.h"


// Cxchatter

STDMETHODIMP Cxchatter::get_MyId(LONG* pVal)
{
	*pVal = m_lMyId;

	return S_OK;
}

STDMETHODIMP Cxchatter::put_MyId(LONG newVal)
{
	m_lMyId = newVal;

	return S_OK;
}

STDMETHODIMP Cxchatter::get_MyName(BSTR* pVal)
{
	*pVal = m_bstrMyName;

	return S_OK;
}

STDMETHODIMP Cxchatter::put_MyName(BSTR newVal)
{
	m_bstrMyName = newVal;

	return S_OK;
}

STDMETHODIMP Cxchatter::Start(void)
{
	//::MessageBox(NULL,L"Starting chatter...",L"Starting",MB_OK);

	if (m_bstrMyName.Length()==0 || m_lMyId == 0)
		return S_FALSE;

	/**check it it's already running.*/
	if (m_pobjChatter && 
		m_pobjChatter->ChatterState()!=Chatter::CHATTER_STATE_UNKNOWN &&
		m_pobjChatter->ChatterState()!=Chatter::CHATTER_STATE_FINISHED)
		return S_FALSE;

	/**clean up the stale chatter instance.*/
	if(m_pobjChatter)
	{
		m_pobjChatter->Shutdown();
		int counter=0;
		while (counter<10)
		{
			if (m_pobjChatter->State()==Thread::FINISHED_STATE||m_pobjChatter->State()==Thread::UNDEFINED_STATE)
			{
				m_pobjChatter->Join();
				delete m_pobjChatter;
				m_pobjChatter=NULL;
				break;
			}

			Sleep(100);
			
			counter++;
		}

		if (counter>=10)
			::MessageBox(NULL,L"Chatter won't end,sorry but we'll not join any longer, force shutting it down...",L"Chatter won't end",MB_OK);
	}

	////CW2A aMyName(m_bstrMyName);
	////string strMyName(aMyName); //= _bstr_t(m_bstrMyName);
	////string strLogFilePathAndName = CHATTER_LOGFILE;
	m_pobjChatter = new Chatter(NULL,m_lMyId,string(CW2A(m_bstrMyName))/*,string(CW2A(m_bstrCentralSHost)),m_lCentralSPort,CHATTER_LOGFILE*/);
	m_pobjChatter->Start();

	//::MessageBox(NULL,L"Starting chatted",L"Started",MB_OK);

	return S_OK;
}

STDMETHODIMP Cxchatter::InvitePeerVoiceChat(long peerid)
{
	if(!m_pobjChatter || m_pobjChatter->ChatterState()!=Chatter::CHATTER_STATE_RUNNING)
		return S_FALSE;

	if (m_pobjChatter->InvitePeerVoiceChat(peerid)==0)
		return S_FALSE;

	return S_OK;
}
