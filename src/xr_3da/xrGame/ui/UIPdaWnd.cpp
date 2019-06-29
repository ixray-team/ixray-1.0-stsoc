// UIPdaWnd.cpp:  диалог PDA
// 
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "UIPdaWnd.h"
#include "../Pda.h"

#include "xrUIXmlParser.h"
#include "UIXmlInit.h"
#include "UIInventoryUtilities.h"

#include "../HUDManager.h"
#include "../level.h"
#include "../game_cl_base.h"

#include "UIStatic.h"
#include "UIFrameWindow.h"
#include "UITabControl.h"
//#include "UIPdaCommunication.h"
#include "UIPdaContactsWnd.h"
#include "UIMapWnd.h"
#include "UIDiaryWnd.h"
#include "UIFrameLineWnd.h"
#include "UIEncyclopediaWnd.h"
#include "UIStalkersRankingWnd.h"
#include "UIActorInfo.h"
#include "UIEventsWnd.h"
#include "../object_broker.h"
#include "UIMessagesWindow.h"
#include "UIMainIngameWnd.h"
#include "UITabButton.h"

#define		PDA_XML					"pda.xml"
u32			g_pda_info_state		= 0;


CUIPdaWnd::CUIPdaWnd()
{
	UIMapWnd				= NULL;
	UIPdaContactsWnd		= NULL;
	UIEncyclopediaWnd		= NULL;
	UIDiaryWnd				= NULL;
	UIActorInfo				= NULL;
	UIStalkersRanking		= NULL;
	UIEventsWnd				= NULL;
	m_updatedSectionImage	= NULL;

	Init					();
	SetFont					(HUD().Font().pFontMedium);
}

CUIPdaWnd::~CUIPdaWnd()
{
	delete_data		(UIMapWnd);
	delete_data		(UIPdaContactsWnd);
	delete_data		(UIEncyclopediaWnd);
	delete_data		(UIDiaryWnd);
	delete_data		(UIActorInfo);
	delete_data		(UIStalkersRanking);
	delete_data		(UIEventsWnd);
	delete_data		(m_updatedSectionImage);
}

//////////////////////////////////////////////////////////////////////////

void CUIPdaWnd::Init()
{
	CUIXml uiXml;
	bool xml_result			= uiXml.Init(CONFIG_PATH, UI_PATH,PDA_XML);
	R_ASSERT3				(xml_result, "xml file not found", PDA_XML);

	CUIXmlInit xml_init;
	
	m_pActiveDialog			= NULL;


	xml_init.InitWindow		(uiXml, "main", 0, this);

	UIMainPdaFrame			= xr_new<CUIStatic>(); UIMainPdaFrame->SetAutoDelete(true);
	AttachChild				(UIMainPdaFrame);
	xml_init.InitStatic		(uiXml, "background_static", 0, UIMainPdaFrame);

	//Ёлементы автоматического добавлени€
	xml_init.InitAutoStatic	(uiXml, "auto_static", this);

	// Main buttons background
	UIMainButtonsBackground = xr_new<CUIFrameLineWnd>(); UIMainButtonsBackground->SetAutoDelete(true);
	UIMainPdaFrame->AttachChild(UIMainButtonsBackground);
	xml_init.InitFrameLine	(uiXml, "mbbackground_frame_line", 0, UIMainButtonsBackground);

	// Timer background
	UITimerBackground		= xr_new<CUIFrameLineWnd>(); UITimerBackground->SetAutoDelete(true);
	UIMainPdaFrame->AttachChild(UITimerBackground);
	xml_init.InitFrameLine	(uiXml, "timer_frame_line", 0, UITimerBackground);

	// Oкно карты
	UIMapWnd				= xr_new<CUIMapWnd>();
	UIMapWnd->Init			("pda_map.xml","map_wnd");

	if( IsGameTypeSingle() )
	{
		// Oкно коммуникaции
		UIPdaContactsWnd		= xr_new<CUIPdaContactsWnd>();
		UIPdaContactsWnd->Init	();


		// Oкно новостей
		UIDiaryWnd				= xr_new<CUIDiaryWnd>();
		UIDiaryWnd->Init		();

		// ќкно энциклопедии
		UIEncyclopediaWnd		= xr_new<CUIEncyclopediaWnd>();
		UIEncyclopediaWnd->Init	();

		// ќкно статистики о актере
		UIActorInfo				= xr_new<CUIActorInfoWnd>();
		UIActorInfo->Init		();

		// ќкно рейтинга сталкеров
		UIStalkersRanking		= xr_new<CUIStalkersRankingWnd>();
		UIStalkersRanking->Init	();

		UIEventsWnd				= xr_new<CUIEventsWnd>();
		UIEventsWnd->Init		();
	}
	// Tab control
	UITabControl			= xr_new<CUITabControl>(); UITabControl->SetAutoDelete(true);
	UIMainPdaFrame->AttachChild(UITabControl);
	xml_init.InitTabControl	(uiXml, "tab", 0, UITabControl);
	UITabControl->SetMessageTarget(this);


	if(GameID()!=GAME_SINGLE){
		UITabControl->GetButtonsVector()->at(0)->Enable(false);
		UITabControl->GetButtonsVector()->at(2)->Enable(false);
		UITabControl->GetButtonsVector()->at(3)->Enable(false);
		UITabControl->GetButtonsVector()->at(4)->Enable(false);
		UITabControl->GetButtonsVector()->at(5)->Enable(false);
		UITabControl->GetButtonsVector()->at(6)->Enable(false);
	}
	
	m_updatedSectionImage			= xr_new<CUIStatic>();
	xml_init.InitStatic				(uiXml, "updated_section_static", 0, m_updatedSectionImage);

	m_pActiveSection				= eptNoActiveTab;

}

void CUIPdaWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	if(pWnd == UITabControl){
		if (TAB_CHANGED == msg){
			SetActiveSubdialog	((EPdaTabs)UITabControl->GetActiveIndex());
		}
	}else 
	{
		R_ASSERT(m_pActiveDialog);
		m_pActiveDialog->SendMessage(pWnd, msg, pData);
	}
}

void CUIPdaWnd::Show()
{
	InventoryUtilities::SendInfoToActor("ui_pda");

	inherited::Show();
}

void CUIPdaWnd::Hide()
{
	inherited::Hide();

	InventoryUtilities::SendInfoToActor("ui_pda_hide");
	HUD().GetUI()->UIMainIngameWnd->SetFlashIconState_(CUIMainIngameWnd::efiPdaTask, false);

}

void CUIPdaWnd::UpdateDateTime()
{
	static shared_str prevStrTime = " ";
	xr_string strTime = *InventoryUtilities::GetGameTimeAsString(InventoryUtilities::etpTimeToMinutes);
				strTime += " ";
				strTime += *InventoryUtilities::GetGameDateAsString(InventoryUtilities::edpDateToDay);

	if (xr_strcmp(strTime.c_str(), prevStrTime))
	{
		UITimerBackground->UITitleText.SetText(strTime.c_str());
		prevStrTime = strTime.c_str();
	}
}

void CUIPdaWnd::Update()
{
	inherited::Update		();
	UpdateDateTime			();
}

void CUIPdaWnd::SetActiveSubdialog(EPdaTabs section)
{
	if(	m_pActiveSection == section) return;

	if (m_pActiveDialog){
		UIMainPdaFrame->DetachChild(m_pActiveDialog);
		m_pActiveDialog->Show(false);
	}

	switch (section) 
	{
	case eptDiary:
		m_pActiveDialog			= smart_cast<CUIWindow*>(UIDiaryWnd);
		InventoryUtilities::SendInfoToActor("ui_pda_events");
		g_pda_info_state		&= ~pda_section::diary;
		break;
	case eptContacts:
		m_pActiveDialog			= smart_cast<CUIWindow*>(UIPdaContactsWnd);
		InventoryUtilities::SendInfoToActor("ui_pda_contacts");
		g_pda_info_state		&= ~pda_section::contacts;
		break;
	case eptMap:
		m_pActiveDialog			= smart_cast<CUIWindow*>(UIMapWnd);
		g_pda_info_state		&= ~pda_section::map;
		break;
	case eptEncyclopedia:
		m_pActiveDialog			= smart_cast<CUIWindow*>(UIEncyclopediaWnd);
		InventoryUtilities::SendInfoToActor("ui_pda_encyclopedia");
		g_pda_info_state		&= ~pda_section::encyclopedia;
		break;
	case eptActorStatistic:
		m_pActiveDialog			= smart_cast<CUIWindow*>(UIActorInfo);
		InventoryUtilities::SendInfoToActor("ui_pda_actor_info");
		g_pda_info_state		&= ~pda_section::statistics;
		break;
	case eptRanking:
		m_pActiveDialog			= smart_cast<CUIWindow*>(UIStalkersRanking);
		g_pda_info_state		&= ~pda_section::ranking;
		InventoryUtilities::SendInfoToActor("ui_pda_ranking");
		break;
	case eptQuests:
		m_pActiveDialog			= smart_cast<CUIWindow*>(UIEventsWnd);
		g_pda_info_state		&= ~pda_section::quests;
		break;
	default:
		Msg("not registered button identifier [%d]",UITabControl->GetActiveIndex());
	}
	UIMainPdaFrame->AttachChild		(m_pActiveDialog);
	m_pActiveDialog->Show			(true);

	if(UITabControl->GetActiveIndex()!=section)
		UITabControl->SetNewActiveTab	(section);

	m_pActiveSection = section;
}


void CUIPdaWnd::Draw()
{
	inherited::Draw									();
//	HUD().GetUI()->m_pMessagesWnd->DrawPdaMessages	();
	DrawUpdatedSections								();
}

void CUIPdaWnd::PdaContentsChanged	(pda_section::part type)
{
	bool b = true;

	if(type==pda_section::encyclopedia){
		UIEncyclopediaWnd->ReloadArticles	();
	}else
	if(type==pda_section::news){
		UIDiaryWnd->AddNews					();
		UIDiaryWnd->MarkNewsAsRead			(UIDiaryWnd->IsShown());
	}else
	if(type==pda_section::quests){
		UIEventsWnd->Reload					();
	}else
	if(type==pda_section::contacts){
		UIPdaContactsWnd->Reload		();
		b = false;
	}

	if(b){
		g_pda_info_state |= type;
		HUD().GetUI()->UIMainIngameWnd->SetFlashIconState_(CUIMainIngameWnd::efiPdaTask, true);
	}

}
void draw_sign		(CUIStatic* s, Frect& r)
{
	Fvector2 pos;
	pos					= r.lt;
	pos.y				+= r.height()/2.0f;
	s->SetWndPos		(pos);
	s->Draw				();
}

void CUIPdaWnd::DrawUpdatedSections				()
{
	Frect r;
	m_updatedSectionImage->Update				();
	if(g_pda_info_state&pda_section::quests){

		UITabControl->GetButtonByIndex		(eptQuests)->GetAbsoluteRect(r);
		draw_sign								(m_updatedSectionImage, r);
	}
	if(g_pda_info_state&pda_section::map){
		UITabControl->GetButtonByIndex		(eptMap)->GetAbsoluteRect(r);
		draw_sign								(m_updatedSectionImage, r);
	}
	if(g_pda_info_state&pda_section::diary){
		UITabControl->GetButtonByIndex		(eptDiary)->GetAbsoluteRect(r);
		draw_sign								(m_updatedSectionImage, r);
	}
	if(g_pda_info_state&pda_section::contacts){
		UITabControl->GetButtonByIndex		(eptContacts)->GetAbsoluteRect(r);
		draw_sign								(m_updatedSectionImage, r);
	}
	if(g_pda_info_state&pda_section::ranking){
		UITabControl->GetButtonByIndex		(eptRanking)->GetAbsoluteRect(r);
		draw_sign								(m_updatedSectionImage, r);
	}
	if(g_pda_info_state&pda_section::statistics){
		UITabControl->GetButtonByIndex		(eptActorStatistic)->GetAbsoluteRect(r);
		draw_sign								(m_updatedSectionImage, r);
	}
	if(g_pda_info_state&pda_section::encyclopedia){
		UITabControl->GetButtonByIndex		(eptEncyclopedia)->GetAbsoluteRect(r);
		draw_sign								(m_updatedSectionImage, r);
	}
	
}

void CUIPdaWnd::Reset()
{
	inherited::Reset		();
	UIMapWnd->Reset			();
	UIPdaContactsWnd->Reset	();
	UIEncyclopediaWnd->Reset();
	UIDiaryWnd->Reset		();
	UIActorInfo->Reset		();
	UIStalkersRanking->Reset();
	UIEventsWnd->Reset		();
}
