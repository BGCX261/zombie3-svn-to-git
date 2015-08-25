#pragma once

#include <atlstr.h>			// CString
#include <vector>
#include <atlwin.h>
#include "ATLControls.h"	// CProgressBarCtrl

using namespace ATLControls;

////////////////////////////////////////////////////////////
// DEFINITIONS
////////////////////////////////////////////////////////////
#define UNKNOWN_TEXT	TEXT("UNKNOWN")
#define ACCEPT_TEXT		TEXT("接受")
#define SAVEAS_TEXT		TEXT("另存为")
#define REJECT_TEXT		TEXT("拒绝")
#define CANCEL_TEXT		TEXT("取消")
#define TFOFFLINE_TEXT	TEXT("发送离线文件")

////////////////////////////////////////////////////////////
// TYPEDEFS
////////////////////////////////////////////////////////////
//typedef enum
//{
//	REQUEST_ITEM_TYPE_UNKNOWN,
//	REQUEST_ITEM_TYPE_VOICECHAT,
//	REQUEST_ITEM_TYPE_VIDEOCHAT,
//	REQUEST_ITEM_TYPE_TRANSFERFILE,
//	REQUEST_ITEM_TYPE_VOICECHAT_OUT,
//	REQUEST_ITEM_TYPE_VIDEOCHAT_OUT,
//	REQUEST_ITEM_TYPE_TRANSFERFILE_OUT,
//	REQUEST_ITEM_TYPE_NUM
//}tRequestItemType;
//typedef enum
//{
//	REQUEST_ITEM_STATE_UNKNOWN,
//	REQUEST_ITEM_STATE_CREATED,
//	REQUEST_ITEM_STATE_REQUESTED,
//	REQUEST_ITEM_STATE_ONGOING,
//	REQUEST_ITEM_STATE_NUM
//}tRequestItemState;
typedef enum
{
	REQUEST_SUBITEM_TYPE_UNKNOWN,
	REQUEST_SUBITEM_TYPE_TITTLE,
	REQUEST_SUBITEM_TYPE_PROGRESS,
	REQUEST_SUBITEM_TYPE_BUTTONS,
	REQUEST_SUBITEM_TYPE_NUM
}tRequestSubItemType;
typedef enum
{
	CLICK_TRIGGERED_CMD_UNKNOWN,
	CLICK_TRIGGERED_CMD_ACCEPT,
	CLICK_TRIGGERED_CMD_SAVEAS,
	CLICK_TRIGGERED_CMD_REJECT,
	CLICK_TRIGGERED_CMD_CANCEL,
	CLICK_TRIGGERED_CMD_TRANSFERFILEOFFLINE,
	CLICK_TRIGGERED_CMD_NUM
}tClickTriggeredCmdType;
typedef struct tagCmdInfo
{
	tagCmdInfo(LPTSTR CmdText,tClickTriggeredCmdType ClickTriggeredCmdType)
		: strCmdText(CmdText),
		  eClickTriggeredCmdType(ClickTriggeredCmdType),
		  XStart(0),
		  XEnd(0)
	{}

	LPTSTR strCmdText;
	tClickTriggeredCmdType eClickTriggeredCmdType;
	LONG XStart;
	LONG XEnd;
}tCmdInfo;
static ULONG GetTextWidth(LPCTSTR strText,HFONT hFont)
{
	HDC hdc = ::GetDC(NULL);
	HFONT hOldFont = (HFONT)::SelectObject(hdc, hFont);
	RECT rc={0,0,0,0};
	DrawText(hdc,strText,static_cast<int>(wcslen(strText)),&rc,DT_CALCRECT);
	//::GetTextMetrics(hdc, &tm);
	::SelectObject(hdc, hOldFont);
	::ReleaseDC(NULL, hdc);

	return static_cast<ULONG>(rc.right-rc.left);
}
//
//class RequestSubItem;
//
//class RequestItem
//{
//public:
//	typedef std::vector<RequestSubItem*> tRequestSubItemVec;
//	typedef tRequestSubItemVec::iterator tRequestSubItemVecIter;
//	typedef tRequestSubItemVec::const_iterator tRequestSubItemVecConstIter;
//
//	RequestItem(ULONG ulUserid,CString csUserName,ULONG ulTid,tRequestItemType riType,tRequestItemState eRIState,CString csFileName=_T(""));
//	virtual ~RequestItem(void);
//	tRequestItemType GetType(void) const;
//private:
//	void CreateSubItems(void);
//	int m_nItemCount;
//	ULONG m_ulUserid;
//	CString m_csUserName;
//	CString m_csFileName;
//	ULONG m_ulTid;
//	tRequestItemType m_eRIType;
//	tRequestItemState m_eRIState;
//	tRequestSubItemVec m_SubItemVec;
//};
class RequestSubItem
{
public:
	RequestSubItem(ULONG ulTStatusId);
	virtual ~RequestSubItem();
	ULONG GetTStatusId(void) const;
protected:
	ULONG m_ttstatusid;
};
class RequestSubItemTittle:public RequestSubItem
{
public:
	RequestSubItemTittle(ULONG ulTStatusId,CString csTittle);
	CString GetTittle(void) const;
private:
	CString m_csTittle;
};
class RequestSubItemProgress:public RequestSubItem
{
public:
	RequestSubItemProgress(ULONG ulTStatusId,HWND hWnd);
	~RequestSubItemProgress();
	void MoveCtrlWindow(RECT rc);
	void SetProgress(int nNOfHundred);
private:
	CProgressBarCtrl m_progress;
	HWND m_wndParent;
};
class RequestSubItemButtons:public RequestSubItem
{
public:
////////////////////////////////////////////////////////////
// typedefs
////////////////////////////////////////////////////////////
	typedef std::vector<tCmdInfo*> tTSTRVec;
	typedef tTSTRVec::iterator tTSTRVecIter;
	typedef tTSTRVec::const_iterator tTSTRVecConstIter;
	RequestSubItemButtons(ULONG ulTStatusId,ULONG ulListViewWidth,HFONT hFont);
	virtual ~RequestSubItemButtons();
	tClickTriggeredCmdType GetClickTriggeredCmdType(POINT pt);

	CString GetButtonsText();
	void CreateButtonsYesNo(void);
	void CreateButtonsYesAsNo(void);
	void CreateButtonsCancel(void);
	void CreateButtonsUnknown(void);
	void CreateButtonsTransferFile(void);
	void CalcButtonAreas();
	void DestroyButtons(void);
private:
	
	ULONG m_ulListViewWidth;
	HFONT m_hFont;
	tTSTRVec m_ButtonTextVec;
};
//class CProperty
//{
//public:
//////////////////////////////////////////////////////////////
//// typedefs
//////////////////////////////////////////////////////////////
//	typedef std::vector<tCmdInfo*> tTSTRVec;
//	typedef tTSTRVec::iterator tTSTRVecIter;
//	typedef tTSTRVec::const_iterator tTSTRVecConstIter;
//
//	CProperty(tTStatus* ptstatus,ULONG userid,ULONG tid,CString username,CString filename=_T(""))
//		:	m_ulId(GetAUniquePropertyId()),
//			//m_ePropertyType(ptype),
//			//m_ePropertyState(PROPERTY_STATE_CREATED),
//			m_tstatus(ptstatus),
//			m_ulUserid(userid),
//			m_ulTid(tid),
//			m_bstrUserName(username),
//			m_bstrFileName(filename),
//			m_ButtonTextVec()
//	{
//		CreateButtons();
//	}
//	~CProperty()
//	{
//		//if (m_ulButtonFamily) delete m_ulButtonFamily;
//	}
//
//	CString GetDescription()
//	{
//		CString description=m_bstrUserName;
//
//		switch (m_tstatus->ttype)
//		{
//		case TASK_TYPE_VOICE_CHAT:
//		case TASK_TYPE_VOICE_GROUP_CHAT:
//			description += _T(" 请求语音聊天...");
//			break;
//		case TASK_TYPE_VIDEO_CHAT:
//			description += _T(" 请求视频聊天...");
//			break;
//		case TASK_TYPE_TRANSFER:
//			description = _T(" 接收文件来自 ");
//			description += m_bstrUserName+_T(" ");
//			description += m_bstrFileName;
//			break;
//		case TASK_TYPE_VOICE_CHAT_OUT:
//		case TASK_TYPE_VOICE_GROUP_CHAT_OUT:
//			description = _T("请求 ");
//			description+=m_bstrUserName;
//			description += _T(" 语音聊天...");
//			break;
//		case TASK_TYPE_VIDEO_CHAT_OUT:
//			description = _T("请求 ");
//			description+=m_bstrUserName;
//			description += _T(" 视频聊天...");
//			break;
//		case TASK_TYPE_TRANSFER_OUT:
//			description = _T("传送文件到 ");
//			description += m_bstrUserName;
//			description += _T(" ");
//			description += m_bstrFileName;
//			break;
//		default:
//			description += _T(" UNKNOWN!!!");
//			break;
//		}
//
//		*bstrDescription = description.AllocSysString();
//
//		return S_OK;
//	}
//
//	void GetButtonsText(BSTR* bstrButtonText)
//	{
//		CString cstrButtonText=_T("");
//
//		tTSTRVecIter iter = m_ButtonTextVec.begin();
//
//		while (iter!=m_ButtonTextVec.end())
//		{
//			cstrButtonText += (*iter)->strCmdText;
//			cstrButtonText += _T("  ");	//inter words
//			++iter;
//		}
//
//		*bstrButtonText = cstrButtonText.AllocSysString();
//	}
//	void GetTextProgress(BSTR* bstrProgress)
//	{}
//	HRESULT GetButtonTexts(BSTR* bstrButtonText)
//	{
//		CString cstrButtonText=_T("");
//
//		switch (m_tstatus->tstate)
//		{
//		case TASK_STATE_REQUEST_CREATED:
//		case TASK_STATE_REQUEST_REQUESTED:
//			GetButtonsText(bstrButtonText);
//			break;
//		case TASK_STATE_REQUEST_CONFIRMED:
//			GetButtonsText(bstrButtonText);
//		//	if (m_tstatus->ttype==TASK_TYPE_TRANSFER ||
//		//		m_tstatus->ttype==TASK_TYPE_TRANSFER_OUT)
//		//		GetButtonsText(bstrButtonText);
//		//	else
//		//	{
//		//		if(TASK_TYPE_VOICE_CHAT_OUT<=m_tstatus->ttype &&
//		//			TASK_TYPE_TRANSFER_OUT>=m_tstatus->ttype)
//		//			cstrButtonText = _T("对方同意了您的请求!");
//		//		else
//		//			cstrButtonText = _T("您同意了对方的请求!");
//		//	}
//		//	break;
//		//case TASK_STATE_REQUEST_REJECTED:
//		//	if(TASK_TYPE_VOICE_CHAT_OUT<=m_tstatus->ttype &&
//		//		TASK_TYPE_TRANSFER_OUT>=m_tstatus->ttype)
//		//		cstrButtonText = _T("对方同意了您的请求!");
//		//	else
//		//		cstrButtonText = _T("您同意了对方的请求!");
//		//	break;
//		default:
//			cstrButtonText = _T("");
//			break;
//		}
//		
//		//switch(m_tstatus->ttype)
//		//{
//		//case TASK_TYPE_VOICE_CHAT_OUT:
//		//case TASK_TYPE_VOICE_GROUP_CHAT_OUT:
//		//case TASK_TYPE_VIDEO_CHAT_OUT:
//		//	if (m_tstatus->tstate==TASK_STATE_REQUEST_CREATED)
//		//		GetButtonsText(bstrButtonText);
//		//	else if 
//		//	break;
//		//case TASK_TYPE_TRANSFER_OUT:
//		//	if (m_tstatus->tstate==TASK_STATE_REQUEST_CREATED)
//		//		CreateButtonsTransferFile();
//		//	else if (m_tstatus->tstate==TASK_STATE_REQUEST_CONFIRMED)
//		//		CreateButtonsCancel();
//		//	break;
//		//case TASK_TYPE_VOICE_CHAT:
//		//case TASK_TYPE_VOICE_GROUP_CHAT:
//		//case TASK_TYPE_VIDEO_CHAT:
//		//	if (m_tstatus->tstate==TASK_STATE_REQUEST_REQUESTED)
//		//		CreateButtonsYesNo();
//		//	break;
//		//case TASK_TYPE_TRANSFER:
//		//	if (m_tstatus->tstate==TASK_STATE_REQUEST_REQUESTED)
//		//		CreateButtonsYesAsNo();
//		//	else if (m_tstatus->tstate==TASK_STATE_REQUEST_CONFIRMED)
//		//		CreateButtonsCancel();
//		//	break;
//		//default:
//		//	CreateButtonsUnknown();
//		//	break;
//
//		//if (m_tstatus->ttype==PROPERTY_STATE_CREATED)
//		//{
//		//	CString cstrButtonText=_T("");
//
//		//	tTSTRVecIter iter = m_ButtonTextVec.begin();
//
//		//	while (iter!=m_ButtonTextVec.end())
//		//	{
//		//		cstrButtonText += (*iter)->strCmdText;
//		//		cstrButtonText += _T("  ");	//inter words
//		//		++iter;
//		//	}
//
//		//	*bstrButtonText = cstrButtonText.AllocSysString();
//		//}
//		//else if (m_ePropertyState==PROPERTY_STATE_CONFIRMED)
//		//{
//		//	if (PROPERTY_TYPE_TRANSFER_FILE_IN
//		//}
//		//else if (m_ePropertyState==PROPERTY_STATE_REJECTED)
//		//	*bstrButtonText = _T("您拒绝了对方的请求!");
//		//else if (m_ePropertyState==PROPERTY_STATE_CANCELED)
//		//	*bstrButtonText = _T("您取消了任务!");
//		//else
//		//	(0);
//
//		return 0;
//	}
//	ULONG GetPropertyId(void) const
//	{
//		return m_ulId;
//	}
//	ULONG GetUserid(void) const
//	{
//		return m_ulUserid;
//	}
//	ULONG GetTid(void) const
//	{
//		return m_ulTid;
//	}
//	tClickTriggeredCmdType GetClickTriggeredCmdType(POINT p)
//	{
//		tClickTriggeredCmdType thetype = CLICK_TRIGGERED_CMD_UNKNOWN;
//
//		tTSTRVecIter iter=m_ButtonTextVec.begin();
//		while (iter!=m_ButtonTextVec.end())
//		{
//			if ((*iter)->XStart<p.x && (*iter)->XEnd>p.x)
//			{
//				thetype = (*iter)->eClickTriggeredCmdType;
//				break;
//			}
//			++iter;
//		}
//
//		return thetype;
//	}
//	void SetTStatus(tTStatus* tstatus)
//	{
//		m_tstatus = tstatus;
//	}
//	void CreateButtons()
//	{
//		DestroyButtons();
//
//		switch(m_tstatus->ttype)
//		{
//		case TASK_TYPE_VOICE_CHAT_OUT:
//		case TASK_TYPE_VOICE_GROUP_CHAT_OUT:
//		case TASK_TYPE_VIDEO_CHAT_OUT:
//			if (m_tstatus->tstate==TASK_STATE_REQUEST_CREATED)
//				CreateButtonsCancel();
//			break;
//		case TASK_TYPE_TRANSFER_OUT:
//			if (m_tstatus->tstate==TASK_STATE_REQUEST_CREATED)
//				CreateButtonsTransferFile();
//			else if (m_tstatus->tstate==TASK_STATE_REQUEST_CONFIRMED)
//				CreateButtonsCancel();
//			break;
//		case TASK_TYPE_VOICE_CHAT:
//		case TASK_TYPE_VOICE_GROUP_CHAT:
//		case TASK_TYPE_VIDEO_CHAT:
//			if (m_tstatus->tstate==TASK_STATE_REQUEST_REQUESTED)
//				CreateButtonsYesNo();
//			break;
//		case TASK_TYPE_TRANSFER:
//			if (m_tstatus->tstate==TASK_STATE_REQUEST_REQUESTED)
//				CreateButtonsYesAsNo();
//			else if (m_tstatus->tstate==TASK_STATE_REQUEST_CONFIRMED)
//				CreateButtonsCancel();
//			break;
//		default:
//			CreateButtonsUnknown();
//			break;
//		}
//	}
//	void CreateButtonsYesNo(void)
//	{
//		m_ButtonTextVec.push_back(new tagCmdInfo(ACCEPT_TEXT,CLICK_TRIGGERED_CMD_ACCEPT));
//		m_ButtonTextVec.push_back(new tagCmdInfo(REJECT_TEXT,CLICK_TRIGGERED_CMD_REJECT));
//	}
//	void CreateButtonsYesAsNo(void)
//	{
//		m_ButtonTextVec.push_back(new tagCmdInfo(ACCEPT_TEXT,CLICK_TRIGGERED_CMD_ACCEPT));
//		m_ButtonTextVec.push_back(new tagCmdInfo(SAVEAS_TEXT,CLICK_TRIGGERED_CMD_SAVEAS));
//		m_ButtonTextVec.push_back(new tagCmdInfo(REJECT_TEXT,CLICK_TRIGGERED_CMD_REJECT));
//	}
//	void CreateButtonsCancel(void)
//	{
//		m_ButtonTextVec.push_back(new tagCmdInfo(CANCEL_TEXT,CLICK_TRIGGERED_CMD_CANCEL));
//	}
//	void CreateButtonsUnknown(void)
//	{
//		m_ButtonTextVec.push_back(new tagCmdInfo(UNKNOWN_TEXT,CLICK_TRIGGERED_CMD_UNKNOWN));
//	}
//	void CreateButtonsTransferFile(void)
//	{
//		m_ButtonTextVec.push_back(new tagCmdInfo(TFOFFLINE_TEXT,CLICK_TRIGGERED_CMD_TRANSFERFILEOFFLINE));
//		m_ButtonTextVec.push_back(new tagCmdInfo(REJECT_TEXT,CLICK_TRIGGERED_CMD_REJECT));
//	}
//	void DestroyButtons(void)
//	{
//		tTSTRVecIter iter = m_ButtonTextVec.begin();
//
//		while (iter!=m_ButtonTextVec.end())
//		{
//			delete (*iter);
//			m_ButtonTextVec.erase(iter++);
//		}
//	}
//	void CalcButtonAreas(ULONG ulListViewWidth,HFONT hFont)
//	{
//		BSTR bstrButtonText;
//		GetButtonTexts(&bstrButtonText);
//		ULONG ulLineLen = GetTextWidth(bstrButtonText,hFont);
//
//		ULONG ulLenWithoutSpace = 0;
//		tTSTRVecIter iter=m_ButtonTextVec.begin();
//		while (iter!=m_ButtonTextVec.end())
//			ulLenWithoutSpace += GetTextWidth((*iter++)->strCmdText,hFont);
//
//		ULONG ulInterButton = 0;
//		if (m_ButtonTextVec.size()>0)
//			ulInterButton = (ulLineLen-ulLenWithoutSpace)/m_ButtonTextVec.size();
//
//		ULONG ulXStart = 0;
//		ULONG ulXEnd=ulInterButton;
//		iter=m_ButtonTextVec.begin();
//		while (iter!=m_ButtonTextVec.end())
//		{
//			(*iter)->XStart = ulXEnd;
//			(*iter)->XEnd = ulXEnd + GetTextWidth((*iter)->strCmdText,hFont);
//			ulXEnd = (*iter)->XEnd+ulInterButton;
//			++iter;
//		}
//	}
//	static ULONG GetAUniquePropertyId(void)
//	{
//		static const ULONG ulIdTop=10000;
//		static const ULONG ulIdBase=1000;
//		static ULONG ulIdCounter=ulIdBase;
//
//		if (ulIdCounter==ulIdTop)
//			ulIdCounter=ulIdBase;
//
//		return ++ulIdCounter;
//	}
//	//static tPropertyType GetPropertyType(tTaskType ttype)
//	//{
//	//	tPropertyType ptype;
//
//	//	switch (ttype)
//	//	{
//	//	case TASK_TYPE_VOICE_CHAT:
//	//	case TASK_TYPE_VOICE_GROUP_CHAT:
//	//		ptype = PROPERTY_TYPE_VOICE_CHAT_IN;
//	//		break;
//	//	case TASK_TYPE_VIDEO_CHAT:
//	//		ptype = PROPERTY_TYPE_VIDEO_CHAT_IN;
//	//		break;
//	//	case TASK_TYPE_TRANSFER:
//	//		ptype = PROPERTY_TYPE_TRANSFER_FILE_IN;
//	//		break;
//	//	case TASK_TYPE_VOICE_CHAT_OUT:
//	//	case TASK_TYPE_VOICE_GROUP_CHAT_OUT:
//	//		ptype = PROPERTY_TYPE_VOICE_CHAT_OUT;
//	//		break;
//	//	case TASK_TYPE_VIDEO_CHAT_OUT:
//	//		ptype = PROPERTY_TYPE_VIDEO_CHAT_OUT;
//	//		break;
//	//	case TASK_TYPE_TRANSFER_OUT:
//	//		ptype = PROPERTY_TYPE_TRANSFERFILE_OUT;
//	//		break;
//	//	default:
//	//		ptype = PROPERTY_TYPE_UNKNOWN;
//	//		break;
//	//	}
//
//	//	return ptype;
//	//}
//protected:
//	ULONG m_ulId;
//	//tPropertyType m_ePropertyType;
//	//tPropertyState m_ePropertyState;
//	tTStatus* m_tstatus;
//	ULONG m_ulUserid;
//	ULONG m_ulTid;
//	BSTR m_bstrUserName;
//	BSTR m_bstrFileName;
//	tTSTRVec m_ButtonTextVec;
//};