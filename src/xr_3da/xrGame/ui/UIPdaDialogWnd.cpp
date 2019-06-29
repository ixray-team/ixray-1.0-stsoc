///////////////////////////////////////////////////////////
/// Режим диалога в PDA
///////////////////////////////////////////////////////////
#include "stdafx.h"
#include "UIPdaDialogWnd.h"

#include "../Pda.h"
#include "../InventoryOwner.h"
#include "../HUDManager.h"
#include "../character_info.h"
#include "xrXMLParser.h"
#include "UIXmlInit.h"

//////////////////////////////////////////////////////////////////////////

#define CONTACT_LOST		"The contact has been lost."
#define WAIT_FOR_REPLY		"Waiting for reply."
#define TECH_INFO			-1
#define PDA_DIALOG_XML		"pda_dialog.xml"
#define PDA_DIALOG_CHAR_XML	"pda_dialog_character.xml"

//////////////////////////////////////////////////////////////////////////

CUIPdaDialogWnd::CUIPdaDialogWnd()
{
	m_iClickedQuestion = -1;
	m_bContactActive = false;
}

//////////////////////////////////////////////////////////////////////////

CUIPdaDialogWnd::~CUIPdaDialogWnd()
{
}

//////////////////////////////////////////////////////////////////////////

void CUIPdaDialogWnd::Init(float x, float y, float width, float height)
{
	CUIXml		uiXml;
	bool xml_result =uiXml.Init(CONFIG_PATH, UI_PATH,PDA_DIALOG_XML);
	R_ASSERT3(xml_result, "xml file not found", PDA_DIALOG_XML);
	CUIXmlInit	xml_init;

	// Декоративное оформление
	AttachChild(&UICharIconFrame);
	xml_init.InitFrameWindow(uiXml, "chicon_frame_window", 0, &UICharIconFrame);
	UICharIconFrame.AttachChild(&UICharIconHeader);
	xml_init.InitFrameLine(uiXml, "chicon_frame_line", 0, &UICharIconHeader);
	UICharIconHeader.AttachChild(&UIAnimatedIcon);
	xml_init.InitAnimatedStatic(uiXml, "a_static", 0, &UIAnimatedIcon);

	AttachChild(&UIPhrasesFrame);
	xml_init.InitFrameWindow(uiXml, "ph_frame_window", 0, &UIPhrasesFrame);
	UIPhrasesFrame.AttachChild(&UIPhrasesHeader);
	xml_init.InitFrameLine(uiXml, "ph_frame_line", 0, &UIPhrasesHeader);
	UIPhrasesHeader.UITitleText.SetText(ALL_PDA_HEADER_PREFIX);

	AttachChild(&UIMsglogFrame);
	xml_init.InitFrameWindow(uiXml, "msglog_frame_window", 0, &UIMsglogFrame);
	UIMsglogFrame.AttachChild(&UIMsglogHeader);
	xml_init.InitFrameLine(uiXml, "msglog_frame_line", 0, &UIMsglogHeader);
	UIMsglogHeader.UITitleText.SetText(UICharacterInfo.UIName.GetText());

	UIMsglogFrame.AttachChild(&UILogListWnd);
	UILogListWnd.SetMessageTarget(this);
	xml_init.InitListWnd(uiXml, "list", 0, &UILogListWnd);
	UILogListWnd.ActivateList(false);
	UILogListWnd.EnableScrollBar(true);

	UIPhrasesFrame.AttachChild(&UIPhrasesListWnd);
	UIPhrasesListWnd.SetMessageTarget(this);
	xml_init.InitListWnd(uiXml, "list", 1, &UIPhrasesListWnd);
	UIPhrasesListWnd.EnableScrollBar(true);

	UICharIconFrame.AttachChild(&UICharacterWindow);
	UICharacterWindow.SetMessageTarget(this);
	xml_init.InitWindow(uiXml, "character_info", 0, &UICharacterWindow);
	
	UICharacterWindow.AttachChild(&UICharacterInfo);
	UICharacterInfo.Init(0,0,UICharacterWindow.GetWidth(), 
							 UICharacterWindow.GetHeight(), 
							 PDA_DIALOG_CHAR_XML);
	UICharacterInfo.UIName.Show(false);
	UICharacterInfo.m_bInfoAutoAdjust = false;
	
	xml_init.InitFrameWindow(uiXml, "mask_frame_window", 0, &UIMask);
	UICharacterInfo.UIIcon.SetMask(&UIMask);

	//Элементы автоматического добавления
	xml_init.InitAutoStatic(uiXml, "right_auto_static", &UICharIconFrame);

	// шрифт для индикации имени персонажа в окне разговора
	xml_init.InitFont(uiXml, "font", 0, m_iNameTextColor, m_pNameTextFont);
	CGameFont * pFont = NULL;
	xml_init.InitFont(uiXml, "font", 1, m_uOurReplicsColor, pFont);

	inherited::Init(x, y, width, height);
}

//////////////////////////////////////////////////////////////////////////

void CUIPdaDialogWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(pWnd == &UIPhrasesListWnd)
	{
		if(msg == LIST_ITEM_CLICKED)
		{
			//m_iMsgNum = EPdaMsg(((CUIListItem*)pData)->GetValue());
			m_iClickedQuestion = ((CUIListItem*)pData)->GetValue();
			
			//if(m_iMsgNum == ePdaMsgMax)
			//	GetTop()->SendMessage(this, BACK_BUTTON_CLICKED);
			//else
			GetTop()->SendMessage(this, PDA_DIALOG_WND_MESSAGE_BUTTON_CLICKED);
		}
	}

	inherited::SendMessage(pWnd, msg, pData);
}

//////////////////////////////////////////////////////////////////////////

void CUIPdaDialogWnd::Show(bool status)
{
	inherited::Show(status);
	inherited::Enable(status);

	if (status)
	{
		UILogListWnd.ResetAll();
		ResetAll();
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIPdaDialogWnd::ContactLoss()
{
	if(!m_bContactActive) return;
	m_bContactActive = false;
	UILogListWnd.AddItem<CUIListItem>(CONTACT_LOST, 0, NULL, TECH_INFO);

	UIPhrasesListWnd.Show(false);
	UIPhrasesListWnd.Enable(false);
}

//////////////////////////////////////////////////////////////////////////

void CUIPdaDialogWnd::ContactWaitForReply()
{
	UILogListWnd.AddItem<CUIListItem>(WAIT_FOR_REPLY, 0, NULL, TECH_INFO);

	UIPhrasesListWnd.Show(false);
	UIPhrasesListWnd.Enable(false);
}

//////////////////////////////////////////////////////////////////////////

void CUIPdaDialogWnd::ContactRestore()
{
	if(!m_bContactActive)
	{
		if(UILogListWnd.GetSize()>0 && UILogListWnd.GetItem(UILogListWnd.GetSize()-1)->GetValue() == TECH_INFO)
			UILogListWnd.RemoveItem(UILogListWnd.GetSize()-1);
	}
	m_bContactActive = true;
	
	UIPhrasesListWnd.Show(true);
	UIPhrasesListWnd.Enable(true);
}

//////////////////////////////////////////////////////////////////////////

void CUIPdaDialogWnd::Update()
{
	inherited::Update();
}