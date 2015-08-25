// xchatter.cpp : Implementation of Cxchatter
#include "stdafx.h"
#include "chatter.hxx"
#include "AXUtinity.h"
#include "SinkTabCtrl.h"
#include "SinkMMenu.h"
#include "SinkRequestList.h"
#include "SinkGroupList.h"
//#include "therender_i.c"
#include "xchatter.h"

COLORREF COLOR_DCHATTER = RGB(224,224,224);
COLORREF COLOR_WCHATTER = RGB(255,255,255);

#ifdef WIN32

//#pragma comment(lib,"mythread")
//#pragma comment(lib,"myfile")
//#pragma comment(lib,"message")
//#pragma comment(lib,"mynet")
//#pragma comment(lib,"myudpnet")
//#pragma comment(lib,"bridgenet")
//#pragma comment(lib,"axaudio")
//#pragma comment(lib,"libspeex")
//#pragma comment(lib,"libspeexdsp")
#pragma comment(lib,"chatter")
//#pragma comment(lib,"therender")

#endif


Cxchatter* g_pTheChatter=NULL;
//static void UpdateGUI(unsigned long tstatusid)
//{
//	if (!g_pTheChatter)
//		return;
//
//	//tUpdateFuncArg* ufarg = static_cast<tUpdateFuncArg*>(arg);
//
//	/**just send a notification to the main dialog.*/
//	SendMessage(g_pTheChatter->m_hWnd,WM_UPDATEGUI,0,tstatusid);
//}
//static void AddRemoveRender(unsigned long userid)
//{
//	if (!g_pTheChatter)
//		return;
//
//	//tUpdateFuncArg* ufarg = static_cast<tUpdateFuncArg*>(arg);
//
//	/**just send a notification to the main dialog.*/
//	SendMessage(g_pTheChatter->m_hWnd,WM_ADDREMOVERENDER,0,userid);
//}
// Cxchatter
Cxchatter::Cxchatter():
	  m_ulMyId(0),
	  m_bstrMyName(),
	  m_pChatter(NULL),
	  m_pUnkCP(NULL),
	  m_pUnkTabCtrlSink(NULL),
	  m_dwTabCtrl(0),
	  m_spTabCtrl(NULL),
	  m_wndTabCtrl(0),
	  m_pUnkMMenuSink(NULL),
	  m_dwMMenu(0),
	  m_spMMenu(NULL),
	  m_wndMMenu(0),
	  m_pUnkRequestListSink(NULL),
	  m_dwRequestList(0),
	  m_spRequestList(NULL),
	  m_wndRequestList(0),
	  m_pUnkGroupListSink(NULL),
	  m_dwGroupList(0),
	  m_spGroupList(NULL),
	  m_wndGroupList(0)
	{
		m_hDialogBrush = CreateSolidBrush(COLOR_DCHATTER);

		m_bWindowOnly = TRUE;
		CalcExtent(m_sizeExtent);
	}

Cxchatter::~Cxchatter()
{
	DeleteObject(m_hDialogBrush);
}
BOOL Cxchatter::ConnectMMenu()
{
	HRESULT hr = m_spMMenu->QueryInterface(IID_IUnknown,(void**)&m_pUnkCP);

	if(FAILED(hr) || !m_pUnkCP)
		return FALSE;

	CComObject<CSinkMMenu>*	pObj = NULL;
	CComObject<CSinkMMenu>::CreateInstance(&pObj);

	if	(!pObj)
		return FALSE;

	pObj->QueryInterface(IID_IUnknown, (void**)&m_pUnkMMenuSink);

	if	(!m_pUnkMMenuSink)
		return FALSE;

		hr = AtlAdvise(m_pUnkCP, m_pUnkMMenuSink, DIID__IChatterMMenuEvents, &m_dwMMenu);

	if	(FAILED(hr))
		return FALSE;


	return TRUE;
}
BOOL Cxchatter::ConnectTabCtrl()
{
	HRESULT hr = m_spTabCtrl->QueryInterface(IID_IUnknown,(void**)&m_pUnkCP);

	if(FAILED(hr) || !m_pUnkCP)
		return FALSE;

	CComObject<CSinkTabCtrl>*	pObj = NULL;
	CComObject<CSinkTabCtrl>::CreateInstance(&pObj);

	if	(!pObj)
		return FALSE;

	pObj->QueryInterface(IID_IUnknown, (void**)&m_pUnkTabCtrlSink);

	if	(!m_pUnkTabCtrlSink)
		return FALSE;

		hr = AtlAdvise(m_pUnkCP, m_pUnkTabCtrlSink, DIID__IChatterTabCtrlEvents, &m_dwTabCtrl);

	if	(FAILED(hr))
		return FALSE;

	return TRUE;
}
BOOL Cxchatter::ConnectRequestList()
{
	HRESULT hr = m_spRequestList->QueryInterface(IID_IUnknown,(void**)&m_pUnkCP);

	if(FAILED(hr) || !m_pUnkCP)
		return FALSE;

	CComObject<CSinkRequestList>*	pObj = NULL;
	CComObject<CSinkRequestList>::CreateInstance(&pObj);

	if	(!pObj)
		return FALSE;

	pObj->QueryInterface(IID_IUnknown, (void**)&m_pUnkRequestListSink);

	if	(!m_pUnkRequestListSink)
		return FALSE;

		hr = AtlAdvise(m_pUnkCP, m_pUnkRequestListSink, DIID__IRequestListEvents, &m_dwRequestList);

	if	(FAILED(hr))
		return FALSE;


	return TRUE;
}
BOOL Cxchatter::ConnectGroupList()
{
	HRESULT hr = m_spGroupList->QueryInterface(IID_IUnknown,(void**)&m_pUnkCP);

	if(FAILED(hr) || !m_pUnkCP)
		return FALSE;

	CComObject<CSinkGroupList>*	pObj = NULL;
	CComObject<CSinkGroupList>::CreateInstance(&pObj);

	if	(!pObj)
		return FALSE;

	pObj->QueryInterface(IID_IUnknown, (void**)&m_pUnkGroupListSink);

	if	(!m_pUnkGroupListSink)
		return FALSE;

		hr = AtlAdvise(m_pUnkCP, m_pUnkGroupListSink, DIID__IGroupListEvents, &m_dwGroupList);

	if	(FAILED(hr))
		return FALSE;


	return TRUE;
}
BOOL Cxchatter::AmIRunning() const
{
	return (m_pChatter && m_pChatter->ChatterState()==Chatter::CHATTER_STATE_RUNNING);
}
LRESULT Cxchatter::InvitePeerVoiceChat(ULONG userid,char* username)
{
	if (!AmIRunning())
		return S_FALSE;

	if (0==m_pChatter->InvitePeerVoiceChat(userid,username))
		return S_FALSE;

	return S_OK;
}
LRESULT Cxchatter::InvitePeerVideoChat(ULONG userid,char* username)
{
	if (!AmIRunning())
		return S_FALSE;

	if (0==m_pChatter->InvitePeerVideoChat(userid,username))
		return S_FALSE;

	return S_OK;
}
LRESULT Cxchatter::SendInstantMsg(ULONG userid,BSTR username,BSTR instantmsg)
{
	if (!m_pChatter || !AmIRunning())
		return 0UL;

	return m_pChatter->SendInstantMsg(userid,CW2A(username),CW2A(instantmsg));
}
LRESULT Cxchatter::InviteCurVoiceChat(void)
{
	ULONG userid;
	BSTR username;

	if (S_OK!=m_spTabCtrl->GetCurSel(&userid) || S_OK!=m_spTabCtrl->GetCurSelName(&username))
		return 0UL;

	LRESULT ret = InvitePeerVoiceChat(userid,CW2A(username));

	::SysFreeString(username);

	return ret;
}
LRESULT Cxchatter::InviteCurVideoChat(void)
{
	if (!m_pChatter || !AmIRunning())
		return 0UL;

	ULONG userid;
	BSTR username;

	m_spTabCtrl->GetCurSel(&userid);
	m_spTabCtrl->GetCurSelName(&username);

	return m_pChatter->InvitePeerVideoChat(userid,CW2A(username));
}
LRESULT Cxchatter::TransferCurFile(void)
{
	if (!m_pChatter || !AmIRunning())
		return 0UL;

	return 0;
}
LRESULT Cxchatter::TransferCurFileOffline(void)
{
	if (!m_pChatter || !AmIRunning())
		return 0UL;

	return 0;
}
LRESULT Cxchatter::AcceptRequest(ULONG tstatusid, BSTR additional)
{
	if (!m_pChatter || !AmIRunning())
		return S_FALSE;

	m_pChatter->AcceptRequest(tstatusid,CW2A(additional));

	return S_OK;
}
LRESULT Cxchatter::RejectRequest(tUInt32 tstatusid)
{
	if (!m_pChatter || !AmIRunning())
		return S_FALSE;

	m_pChatter->RejectRequest(tstatusid);

	return S_OK;
}
LRESULT Cxchatter::CancelTask(tUInt32 tstatusid)
{
	if (!m_pChatter || !AmIRunning())
		return S_FALSE;

	m_pChatter->CancelTask(tstatusid);

	return S_OK;
}
LRESULT Cxchatter::DeleteMember(tUInt32 userid)
{
	if (!m_pChatter || !AmIRunning())
		return S_FALSE;

	m_pChatter->DeleteMember(userid);

	return S_OK;
}
void Cxchatter::UpdateChatterState()
{
}
//void Cxchatter::UpdateMenu(const tTStatus* tstatus)
//{
//}
void Cxchatter::UpdateTab(const tTStatus* tstatus)
{
	TCHAR username[50]={0};
	TCHAR info[200]={0};
	
	AXUtinity::String2TString(tstatus->username.c_str(),username,50);

	switch(tstatus->ttype)
	{
	case TASK_TYPE_INSTANT_MSG:
		AXUtinity::String2TString(tstatus->msgtext().c_str(),info,200);
		m_spTabCtrl->DisplayReceivedUserMsg(tstatus->userid,username,m_bstrMyName,info);
		break;
	case TASK_TYPE_VOICE_CHAT:
	case TASK_TYPE_VOICE_GROUP_CHAT:
	case TASK_TYPE_VIDEO_CHAT:
	case TASK_TYPE_TRANSFER:
	case TASK_TYPE_VOICE_CHAT_OUT:
	case TASK_TYPE_VOICE_GROUP_CHAT_OUT:
	case TASK_TYPE_VIDEO_CHAT_OUT:
	case TASK_TYPE_TRANSFER_OUT:
		if (tstatus->tstate==TASK_STATE_REQUEST_IGNORED_FOR_BUSY && tstatus->arg1)
		{
			AXUtinity::String2TString((char*)tstatus->arg1,info,200);
			m_spTabCtrl->DisplayReceivedUserMsg(tstatus->userid,username,m_bstrMyName,info);
		}
		else if (!tstatus->notification().empty())
		{
			AXUtinity::String2TString(tstatus->notification().c_str(),info,200);
			m_spTabCtrl->DisplayNotification(tstatus->userid,username,m_bstrMyName,info);
		}
	default:
		break;
	}
}
void Cxchatter::UpdateReqestList(const tTStatus* tstatus)
{
	TCHAR info[200]={0};

	switch(tstatus->ttype)
	{
	case TASK_TYPE_VOICE_CHAT:
	case TASK_TYPE_VOICE_GROUP_CHAT:
	case TASK_TYPE_VIDEO_CHAT:
	case TASK_TYPE_VOICE_CHAT_OUT:
	case TASK_TYPE_VOICE_GROUP_CHAT_OUT:
	case TASK_TYPE_VIDEO_CHAT_OUT:
		if (tstatus->tstate==TASK_STATE_REQUEST_REQUESTED || 
			tstatus->tstate==TASK_STATE_REQUEST_CREATED)
		{
			AXUtinity::String2TString(tstatus->tittle().c_str(),info,200);
			m_spRequestList->AddItem(tstatus->tstatusid,info);
			if (tstatus->isin()) m_spRequestList->SetButtonYesNo(tstatus->tstatusid);
			else m_spRequestList->SetButtonCancel(tstatus->tstatusid);
		}
		else if (tstatus->tstate==TASK_STATE_REQUEST_REJECTED	||
				 tstatus->tstate==TASK_STATE_REQUEST_CONFIRMED	||
				 tstatus->tstate==TASK_STATE_CANCELLED			|| 
				 tstatus->tstate==TASK_STATE_TERMINATED			||
				 tstatus->tstate==TASK_STATE_FINISHED)
		{
			m_spRequestList->DeleteItem(tstatus->tstatusid);
		}
		break;
	case TASK_TYPE_TRANSFER:
	case TASK_TYPE_TRANSFER_OUT:
		if (tstatus->tstate==TASK_STATE_REQUEST_REQUESTED || 
			tstatus->tstate==TASK_STATE_REQUEST_CREATED)
		{
			AXUtinity::String2TString(tstatus->tittle().c_str(),info,200);
			m_spRequestList->AddItem(tstatus->tstatusid,info);
			if (tstatus->isin()) m_spRequestList->SetButtonYesAsNo(tstatus->tstatusid);
			else m_spRequestList->SetButtonCancel(tstatus->tstatusid);
		}
		else if (tstatus->tstate==TASK_STATE_ON_GOING)
			m_spRequestList->SetProgress(tstatus->tstatusid,tstatus->progress());
		else if (tstatus->tstate==TASK_STATE_REQUEST_REJECTED	||
				 tstatus->tstate==TASK_STATE_REQUEST_CONFIRMED	||
				 tstatus->tstate==TASK_STATE_CANCELLED			|| 
				 tstatus->tstate==TASK_STATE_TERMINATED			||
				 tstatus->tstate==TASK_STATE_FINISHED)
			m_spRequestList->DeleteItem(tstatus->tstatusid);
		break;
	default:
		break;
	}
}
void Cxchatter::UpdateGroup(const tTStatus* tstatus)
{
	ULONG uThirdId;
	LONG adminid;
	TCHAR info[200]={0};
	LONG uGroupSize=0;

	switch(tstatus->ttype)
	{
	case TASK_TYPE_VOICE_CHAT:
	case TASK_TYPE_VIDEO_CHAT_OUT:
	case TASK_TYPE_VOICE_GROUP_CHAT:
	case TASK_TYPE_VOICE_GROUP_CHAT_OUT:
	case TASK_TYPE_VIDEO_CHAT:
	case TASK_TYPE_VOICE_CHAT_OUT:
	case TASK_TYPE_VIDEO_GROUP_CHAT:
	case TASK_TYPE_VIDEO_GROUP_CHAT_OUT:
		if (tstatus->tstate==TASK_STATE_REQUEST_CONFIRMED)
		{
			adminid = m_pChatter->GetAdmin((char*)info,200);

			// add myself at first
			if(m_pChatter->GroupSize()==0)
			{
				m_spGroupList->AddMember(m_ulMyId,m_bstrMyName,adminid==m_ulMyId);
				m_spGroupList->SetMeAdmin(adminid==m_ulMyId);	//make group know whether I have right to deletion
			}

			AXUtinity::String2TString(tstatus->username.c_str(),info,200);
			m_spGroupList->AddMember(tstatus->userid,info,adminid==tstatus->userid);
		}
		else if (tstatus->tstate==TASK_STATE_CANCELLED	|| 
				 tstatus->tstate==TASK_STATE_TERMINATED	||
				 tstatus->tstate==TASK_STATE_FINISHED)
		{
			m_spGroupList->DeleteMember(m_ulMyId);

			 //if(m_pChatter->GroupSize()==0)
				// m_spGroupList->Empty();
		}
		break;
	case TASK_TYPE_NOTIFY_ADD:
		AXUtinity::String2TString(tstatus->username.c_str(),info,200);
		uThirdId = reinterpret_cast<ULONG>(tstatus->arg1);
		m_spGroupList->AddMember(uThirdId,info,uThirdId==adminid);
		break;
	case TASK_TYPE_NOTIFY_DEL:
		uThirdId = reinterpret_cast<ULONG>(tstatus->arg1);
		m_spGroupList->DeleteMember(uThirdId);
		m_spGroupList->Size(&uGroupSize);
		if (uGroupSize==1)	//only me left,so empty it.
			m_spGroupList->Empty();
		break;
	case TASK_TYPE_NOTIFY_QUIT:
		m_spGroupList->Empty();
		break;
	case TASK_TYPE_NOTIFY_LEAVE:
		m_spGroupList->DeleteMember(tstatus->userid);
		break;
	default:
		break;
	}
}
void Cxchatter::AdjustChildrenPoses(void)
{
	RECT rcClient;
	GetClientRect(&rcClient);
	RECT menuRect(rcClient);
	RECT tabRect(rcClient);
	RECT reqeustlistRect(rcClient);
	RECT grouplistRect(rcClient);

	const long MenuWidth = 56;
	const long ChatterDlgRightEdge = 2;
	const float UserDlgWidthPercent = 0.6;
	const float RequestListHeightPercent = 0.5;

	menuRect.bottom = MenuWidth;

	tabRect.top += MenuWidth;
	tabRect.right = static_cast<long>(rcClient.right*(UserDlgWidthPercent));

	reqeustlistRect.top += MenuWidth;
	reqeustlistRect.left = static_cast<long>(rcClient.right*(UserDlgWidthPercent));
	reqeustlistRect.right = rcClient.right - ChatterDlgRightEdge;
	reqeustlistRect.bottom = static_cast<long>(rcClient.bottom*(RequestListHeightPercent));

	grouplistRect.top = reqeustlistRect.bottom;
	grouplistRect.left = reqeustlistRect.left;
	grouplistRect.right = rcClient.right - ChatterDlgRightEdge;

	m_wndMMenu.MoveWindow(&menuRect);
	m_wndTabCtrl.MoveWindow(&tabRect);
	m_wndRequestList.MoveWindow(&reqeustlistRect);
	m_wndGroupList.MoveWindow(&grouplistRect);
}
BOOL Cxchatter::GetImage(const BYTE* from,BYTE* buf,DWORD bufsize)
{
	return m_pChatter->GetImage((const char*)from,buf,bufsize);
}
LRESULT Cxchatter::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	AtlAxWinInit();

	USES_CONVERSION;

	RECT rc = {0,0,0,0};
	
	////////////////////////////////////////////////////////////////////
	// create the menu control
	////////////////////////////////////////////////////////////////////
	m_wndMMenu.Create(m_hWnd, rc, _T(""), WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	CChatterMMenu::CreateInstance(&m_spMMenu);
	{
		// Obtain a pointer to the container object
		CComPtr<IAxWinHostWindow> spContainer;
		m_wndMMenu.QueryHost(&spContainer);
		// Attach the browser control to the container
		spContainer->AttachControl(m_spMMenu, m_wndMMenu);
	}

	////////////////////////////////////////////////////////////////////
	// create the tab control
	////////////////////////////////////////////////////////////////////
	m_wndTabCtrl.Create(m_hWnd, rc, _T(""), WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	CChatterTabCtrl::CreateInstance(&m_spTabCtrl);
	{
		// Obtain a pointer to the container object
		CComPtr<IAxWinHostWindow> spContainer;
		m_wndTabCtrl.QueryHost(&spContainer);
		// Attach the browser control to the container
		spContainer->AttachControl(m_spTabCtrl, m_wndTabCtrl);
	}

	////////////////////////////////////////////////////////////////////
	// create the requestlist control
	////////////////////////////////////////////////////////////////////
	m_wndRequestList.Create(m_hWnd, rc, _T(""), WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	CRequestList::CreateInstance(&m_spRequestList);
	{
		// Obtain a pointer to the container object
		CComPtr<IAxWinHostWindow> spContainer;
		m_wndRequestList.QueryHost(&spContainer);
		// Attach the browser control to the container
		spContainer->AttachControl(m_spRequestList, m_wndRequestList);
	}

	////////////////////////////////////////////////////////////////////
	// create the grouplist control
	////////////////////////////////////////////////////////////////////
	m_wndGroupList.Create(m_hWnd, rc, _T(""), WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	CGroupList::CreateInstance(&m_spGroupList);
	{
		// Obtain a pointer to the container object
		CComPtr<IAxWinHostWindow> spContainer;
		m_wndGroupList.QueryHost(&spContainer);
		// Attach the browser control to the container
		spContainer->AttachControl(m_spGroupList, m_wndGroupList);
	}

	////////////////////////////////////////////////////////////////////
	// create the render collection
	////////////////////////////////////////////////////////////////////
	m_wndRenderCollection.Create(m_hWnd, rc, _T(""), WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	CSRenderCollection::CreateInstance(&m_spRenderCollection);
	{
		// Obtain a pointer to the container object
		CComPtr<IAxWinHostWindow> spContainer;
		m_wndRenderCollection.QueryHost(&spContainer);
		// Attach the browser control to the container
		spContainer->AttachControl(m_spRenderCollection, m_wndRenderCollection);
	}
	m_spRenderCollection->Play(VIDEO_INTERVAL);

	if (!ConnectMMenu())
		return FALSE;

	if (!ConnectTabCtrl())
		return FALSE;

	if (!ConnectRequestList())
		return FALSE;

	if (!ConnectGroupList())
		return FALSE;

	g_pTheChatter = this;

	AdjustChildrenPoses();

	return TRUE;
}

LRESULT Cxchatter::OnUpdateTheGUI(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	tUInt32 tstatusid = static_cast<tUInt32>(lParam);

	if (tstatusid==0)
	{
		UpdateChatterState();
		return S_OK;
	}

	const tTStatus* tstatus;

	if (!(tstatus=m_pChatter->QueryTStatus(tstatusid)))
		return S_FALSE;

	UpdateTab(tstatus);
	UpdateReqestList(tstatus);
	UpdateGroup(tstatus);

	return 0;
}
LRESULT Cxchatter::OnCtlColorDlg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    return (long) m_hDialogBrush;
}
LRESULT Cxchatter::OnChange2Render(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    tRenderInfo* info = reinterpret_cast<tRenderInfo*>(lParam);

	if(info==NULL)
		return S_FALSE;

	if (info->is_add)
	{
		m_spRenderCollection->CreateRender((BYTE*)info->host,info->port,(BYTE*)info->username);
	}
	else
	{
		m_spRenderCollection->DestroyRender((BYTE*)info->host,info->port);
	}

	return S_OK;
}

STDMETHODIMP Cxchatter::InstantMsgChat(ULONG userid, BSTR username)
{
	if (!AmIRunning())
		return S_FALSE;

	m_spTabCtrl->AddItem(userid,username,m_bstrMyName);

	m_spTabCtrl->SetCurSel(userid);

	//m_spRenderCollection->CreateRender((BYTE*)"goo",132,(BYTE*)"caption1");

	return S_OK;
}

STDMETHODIMP Cxchatter::get_MyId(ULONG* pVal)
{
	*pVal = m_ulMyId;

	return S_OK;
}

STDMETHODIMP Cxchatter::put_MyId(ULONG newVal)
{
	m_ulMyId = newVal;

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
	if (m_bstrMyName.Length()==0 || m_ulMyId == 0)
		return S_FALSE;

	if (AmIRunning())
		return S_FALSE;

	/**clean up the stale chatter instance.*/
	if(m_pChatter)
	{
		m_pChatter->Shutdown();
		int counter=0;
		while (counter<10)
		{
			if (m_pChatter->State()==Thread::FINISHED_STATE||m_pChatter->State()==Thread::UNDEFINED_STATE)
			{
				m_pChatter->Join();
				delete m_pChatter;
				m_pChatter=NULL;
				break;
			}

			Sleep(100);
			
			counter++;
		}

		if (counter>=10)
			::MessageBox(NULL,L"Chatter won't end,sorry but we'll not join any longer, force shutting it down...",L"Chatter won't end",MB_OK);
	}

	m_pChatter = new Chatter(/*UpdateGUI,m_spRenderCollection,*/this->m_hWnd,m_ulMyId,CW2A(m_bstrMyName));
	m_pChatter->Start();

	return S_OK;
}



STDMETHODIMP Cxchatter::InvitePeerVoiceChat(ULONG userid, BSTR username)
{
	return InvitePeerVoiceChat(userid,CW2A(username));
}

STDMETHODIMP Cxchatter::InvitePeerVideoChat(ULONG userid, BSTR username)
{
	return InvitePeerVideoChat(userid,CW2A(username));
}
