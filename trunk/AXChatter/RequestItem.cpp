#include "StdAfx.h"
#include "RequestItem.h"

RequestSubItem::RequestSubItem(ULONG ulTStatusId)
:	m_ttstatusid(ulTStatusId)		 
{}
RequestSubItem::~RequestSubItem()
{}
ULONG RequestSubItem::GetTStatusId(void) const
{
	return m_ttstatusid;
}
RequestSubItemTittle::RequestSubItemTittle(ULONG ulTStatusId,CString csTittle)
:	RequestSubItem(ulTStatusId),
	m_csTittle(csTittle)
{}
CString RequestSubItemTittle::GetTittle(void) const
{
	return m_csTittle;
}
//CString RequestSubItemTittle::GetText(void)
//{
//	CString description=m_csUserName;
//
//	switch (m_eRIType)
//	{
//	case REQUEST_ITEM_TYPE_VOICECHAT:
//		description = _T("语音请求来自 ");
//		break;
//	case REQUEST_ITEM_TYPE_VIDEOCHAT:
//		description = _T("视频请求来自 ");
//		break;
//	case REQUEST_ITEM_TYPE_TRANSFERFILE:
//		description = _T("接收文件来自 ");
//		break;
//	case REQUEST_ITEM_TYPE_VOICECHAT_OUT:
//		description = _T("语音请求到 ");
//		break;
//	case REQUEST_ITEM_TYPE_VIDEOCHAT_OUT:
//		description = _T("视频请求至 ");
//		break;
//	case REQUEST_ITEM_TYPE_TRANSFERFILE_OUT:
//		description = _T("传送文件至 ");
//		break;
//	default:
//		description += _T(" UNKNOWN task to/from ");
//		break;
//	}
//
//	description += m_csUserName;
//	description += _T(".");
//
//	return description;
//}

RequestSubItemProgress::RequestSubItemProgress(ULONG ulTStatusId,HWND hWnd)
:	RequestSubItem(ulTStatusId),
	m_wndParent(hWnd)
{
	m_progress.Create(m_wndParent);
	m_progress.SetRange(0,100);
}
RequestSubItemProgress::~RequestSubItemProgress()
{
	m_progress.DestroyWindow();
}
void RequestSubItemProgress::MoveCtrlWindow(RECT rc)
{
	m_progress.MoveWindow(&rc);
}
void RequestSubItemProgress::SetProgress(int nNOfHundred)
{
	m_progress.SetPos(nNOfHundred);
}

RequestSubItemButtons::RequestSubItemButtons(ULONG ulTStatusId,ULONG ulListViewWidth,HFONT hFont)
:	RequestSubItem(ulTStatusId),
	m_ulListViewWidth(ulListViewWidth),
	m_hFont(hFont),
	m_ButtonTextVec()
{
	//CreateButtons(ulListViewWidth,hFont);
}
RequestSubItemButtons::~RequestSubItemButtons()
{
	DestroyButtons();
}
tClickTriggeredCmdType RequestSubItemButtons::GetClickTriggeredCmdType(POINT pt)
{
	tClickTriggeredCmdType thetype = CLICK_TRIGGERED_CMD_UNKNOWN;

	tTSTRVecIter iter=m_ButtonTextVec.begin();
	while (iter!=m_ButtonTextVec.end())
	{
		if ((*iter)->XStart<pt.x && (*iter)->XEnd>pt.x)
		{
			thetype = (*iter)->eClickTriggeredCmdType;
			break;
		}
		++iter;
	}

	return thetype;
}
CString RequestSubItemButtons::GetButtonsText()
{
	CString cstrButtonText=_T("");

	tTSTRVecIter iter = m_ButtonTextVec.begin();

	while (iter!=m_ButtonTextVec.end())
	{
		cstrButtonText += (*iter)->strCmdText;
		cstrButtonText += _T("  ");	//inter words
		++iter;
	}

	return cstrButtonText;
}
//void RequestSubItemButtons::CreateButtons(ULONG ulListViewWidth,HFONT hFont)
//{
//	DestroyButtons();
//
//	switch(m_eRIType)
//	{
//	case REQUEST_ITEM_TYPE_VOICECHAT:
//	case REQUEST_ITEM_TYPE_VIDEOCHAT:
//		if (m_eRIState==REQUEST_ITEM_STATE_REQUESTED)
//			CreateButtonsYesNo();
//		break;
//	case REQUEST_ITEM_TYPE_TRANSFERFILE:
//		if (m_eRIState==REQUEST_ITEM_STATE_REQUESTED)
//			CreateButtonsYesAsNo();
//		else if (m_eRIState==REQUEST_ITEM_STATE_ONGOING)
//			CreateButtonsCancel();
//		break;
//	case REQUEST_ITEM_TYPE_VOICECHAT_OUT:
//	case REQUEST_ITEM_TYPE_VIDEOCHAT_OUT:
//		if (m_eRIState==REQUEST_ITEM_STATE_CREATED)
//			CreateButtonsCancel();
//		break;
//	case REQUEST_ITEM_TYPE_TRANSFERFILE_OUT:
//		if (m_eRIState==REQUEST_ITEM_STATE_CREATED || (m_eRIState==REQUEST_ITEM_STATE_ONGOING))
//			CreateButtonsCancel();
//		break;
//	}
//
//	CalcButtonAreas(ulListViewWidth,hFont);
//}

void RequestSubItemButtons::CreateButtonsYesNo(void)
{
	DestroyButtons();
	m_ButtonTextVec.push_back(new tagCmdInfo(ACCEPT_TEXT,CLICK_TRIGGERED_CMD_ACCEPT));
	m_ButtonTextVec.push_back(new tagCmdInfo(REJECT_TEXT,CLICK_TRIGGERED_CMD_REJECT));
	CalcButtonAreas();;
}
void RequestSubItemButtons::CreateButtonsYesAsNo(void)
{
	DestroyButtons();
	m_ButtonTextVec.push_back(new tagCmdInfo(ACCEPT_TEXT,CLICK_TRIGGERED_CMD_ACCEPT));
	m_ButtonTextVec.push_back(new tagCmdInfo(SAVEAS_TEXT,CLICK_TRIGGERED_CMD_SAVEAS));
	m_ButtonTextVec.push_back(new tagCmdInfo(REJECT_TEXT,CLICK_TRIGGERED_CMD_REJECT));
	CalcButtonAreas();;
}
void RequestSubItemButtons::CreateButtonsCancel(void)
{
	DestroyButtons();
	m_ButtonTextVec.push_back(new tagCmdInfo(CANCEL_TEXT,CLICK_TRIGGERED_CMD_CANCEL));
	CalcButtonAreas();;
}
void RequestSubItemButtons::CreateButtonsUnknown(void)
{
	DestroyButtons();
	m_ButtonTextVec.push_back(new tagCmdInfo(UNKNOWN_TEXT,CLICK_TRIGGERED_CMD_UNKNOWN));
	CalcButtonAreas();;
}
void RequestSubItemButtons::CreateButtonsTransferFile(void)
{
	DestroyButtons();
	m_ButtonTextVec.push_back(new tagCmdInfo(TFOFFLINE_TEXT,CLICK_TRIGGERED_CMD_TRANSFERFILEOFFLINE));
	m_ButtonTextVec.push_back(new tagCmdInfo(REJECT_TEXT,CLICK_TRIGGERED_CMD_REJECT));
	CalcButtonAreas();;
}
void RequestSubItemButtons::CalcButtonAreas()
{
	CString csButtonText = GetButtonsText();
	ULONG ulLineLen = GetTextWidth(csButtonText,m_hFont);

	ULONG ulLenWithoutSpace = 0;
	tTSTRVecIter iter=m_ButtonTextVec.begin();
	while (iter!=m_ButtonTextVec.end())
		ulLenWithoutSpace += GetTextWidth((*iter++)->strCmdText,m_hFont);

	ULONG ulInterButton = 0;
	if (m_ButtonTextVec.size()>0)
		ulInterButton = (ulLineLen-ulLenWithoutSpace)/m_ButtonTextVec.size();

	ULONG ulXStart = 0;
	ULONG ulXEnd=ulInterButton;
	iter=m_ButtonTextVec.begin();
	while (iter!=m_ButtonTextVec.end())
	{
		(*iter)->XStart = ulXEnd;
		(*iter)->XEnd = ulXEnd + GetTextWidth((*iter)->strCmdText,m_hFont);
		ulXEnd = (*iter)->XEnd+ulInterButton;
		++iter;
	}
}

void RequestSubItemButtons::DestroyButtons(void)
{
	tTSTRVecIter iter = m_ButtonTextVec.begin();

	while (iter!=m_ButtonTextVec.end())
	{
		delete (*iter);
		++iter;
	}

	m_ButtonTextVec.clear();
}