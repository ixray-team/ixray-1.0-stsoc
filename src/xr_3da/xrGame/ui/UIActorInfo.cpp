#include "StdAfx.h"
#include "UIActorInfo.h"
#include "UIXmlInit.h"
#include "UIPdaAux.h"
#include "../Level.h"
#include "../actor.h"

#include "UIInventoryUtilities.h"
#include "UIFrameWindow.h"
#include "UIFrameLineWnd.h"
#include "UIAnimatedStatic.h"
#include "UIScrollView.h"
#include "UICharacterInfo.h"
#include "UI3tButton.h"
#include "UIInventoryUtilities.h"
#include "../actor_statistic_mgr.h"
#include "../character_community.h"
#include "../character_reputation.h"
#include "../relation_registry.h"
#include "../string_table.h"

#define				ACTOR_STATISTIC_XML		"actor_statistic.xml"
#define				ACTOR_CHARACTER_XML		"pda_dialog_character.xml"


CUIActorInfoWnd::CUIActorInfoWnd()
{}


void CUIActorInfoWnd::Init()
{
	CUIXml									uiXml;
	CUIXmlInit								xml_init;
	uiXml.Init								(CONFIG_PATH, UI_PATH,ACTOR_STATISTIC_XML);

	xml_init.InitWindow						(uiXml, "main_wnd", 0, this);

	// Декоративное оформление
	UICharIconFrame							= xr_new<CUIFrameWindow>();	UICharIconFrame->SetAutoDelete	(true);
	xml_init.InitFrameWindow				(uiXml, "chicon_frame_window", 0, UICharIconFrame);
	AttachChild								(UICharIconFrame);

	UICharIconHeader						= xr_new<CUIFrameLineWnd>();	UICharIconHeader->SetAutoDelete	(true);
	xml_init.InitFrameLine					(uiXml, "chicon_frame_line", 0, UICharIconHeader);
	UICharIconFrame->AttachChild			(UICharIconHeader);

	UIAnimatedIcon							= xr_new<CUIAnimatedStatic>();	UIAnimatedIcon->SetAutoDelete	(true);
	xml_init.InitAnimatedStatic				(uiXml, "a_static", 0, UIAnimatedIcon);
	UICharIconHeader->AttachChild			(UIAnimatedIcon);

	UIInfoFrame								= xr_new<CUIFrameWindow>(); UIInfoFrame->SetAutoDelete	(true);
	xml_init.InitFrameWindow				(uiXml, "info_frame_window", 0, UIInfoFrame);
	AttachChild								(UIInfoFrame);
	
	UIInfoHeader							= xr_new<CUIFrameLineWnd>();UIInfoHeader ->SetAutoDelete(true);
	xml_init.InitFrameLine					(uiXml, "info_frame_line", 0, UIInfoHeader);
	UIInfoFrame->AttachChild				(UIInfoHeader);

	UIDetailList							= xr_new<CUIScrollView>();UIDetailList->SetAutoDelete(true);
	UIInfoFrame->AttachChild				(UIDetailList);
	xml_init.InitScrollView					(uiXml, "detail_list", 0, UIDetailList);

	UIMasterList							= xr_new<CUIScrollView>();UIMasterList->SetAutoDelete(true);
	UICharIconFrame->AttachChild			(UIMasterList);
	xml_init.InitScrollView					(uiXml, "master_list", 0, UIMasterList);

	UICharacterWindow						= xr_new<CUIWindow>();UICharacterWindow->SetAutoDelete(true);
	UICharIconFrame->AttachChild			(UICharacterWindow);
	xml_init.InitWindow						(uiXml, "character_info", 0, UICharacterWindow);

	UICharacterInfo							= xr_new<CUICharacterInfo>(); UICharacterInfo->SetAutoDelete(true);
	UICharacterWindow->AttachChild			(UICharacterInfo);
	UICharacterInfo->Init					(0,0,UICharacterWindow->GetWidth(), UICharacterWindow->GetHeight(), ACTOR_CHARACTER_XML);

	//Элементы автоматического добавления
	xml_init.InitAutoStatic					(uiXml, "right_auto_static", UICharIconFrame);
	xml_init.InitAutoStatic					(uiXml, "left_auto_static",  UIInfoFrame);

}

void CUIActorInfoWnd::Show(bool status)
{
	inherited::Show(status);
	if (!status) return;
	
	UICharacterInfo->InitCharacter			(Actor()->ID());
	UICharIconHeader->UITitleText.SetText	(Actor()->Name());
	FillPointsInfo							();
}


void CUIActorInfoWnd::FillPointsInfo			()
{
	CUIXml									uiXml;
	uiXml.Init								(CONFIG_PATH, UI_PATH,ACTOR_STATISTIC_XML);

	UIMasterList->Clear						();

	int items_num = uiXml.GetNodesNum		("actor_stats_wnd", 0, "master_part");
	uiXml.SetLocalRoot						(uiXml.NavigateToNode("actor_stats_wnd",0));
	string64 buff;
	
	for(int i=0; i<items_num; ++i){
		CUIActorStaticticHeader* itm		= xr_new<CUIActorStaticticHeader>(this);
		itm->Init							(&uiXml, "master_part", i);
		
		if(_ParseItem("foo",actor_stats_token)!=(u32)itm->m_index){

			if(_ParseItem("reputation",actor_stats_token)==(u32)itm->m_index){
				itm->m_text2->SetTextST				(InventoryUtilities::GetReputationAsText(Actor()->Reputation()));
				itm->m_text2->SetTextColor			(InventoryUtilities::GetReputationColor(Actor()->Reputation()));
			}else{
				sprintf								(buff,"%d", Actor()->StatisticMgr().GetSectionPoints(itm->m_index));
				itm->m_text2->SetTextST				(buff);
			}
		}
		UIMasterList->AddWindow				(itm, true);
	}

	UIMasterList->SetSelected(UIMasterList->GetItem(1) );

}

void CUIActorInfoWnd::FillPointsDetail	(int idx)
{

	UIDetailList->Clear						();
	CUIXml									uiXml;
	uiXml.Init								(CONFIG_PATH, UI_PATH,ACTOR_STATISTIC_XML);
	uiXml.SetLocalRoot						(uiXml.NavigateToNode("actor_stats_wnd",0));
	
	string512 path;
	sprintf									(path,"detail_part_%d",idx);
	
	XML_NODE* n								= uiXml.NavigateToNode(path,0);
	if(!n)
		sprintf								(path,"detail_part_def");
	if(idx==5)//reputation
	{
		UIInfoHeader->GetTitleStatic()->SetTextST	("st_detail_list_for_community_relations");
		FillReputationDetails						(&uiXml, path);
		return;
	}
	string256	str;
	sprintf		(str,"st_detail_list_for_%s", get_token_name(actor_stats_token,idx));
	UIInfoHeader->GetTitleStatic()->SetTextST	(str);

	SStatSectionData&	section				= Actor()->StatisticMgr().GetSection(idx);
	vStatDetailData::const_iterator it		= section.data.begin();
	vStatDetailData::const_iterator it_e	= section.data.end();

	int _cntr = 0;
	string64 buff;
	for(;it!=it_e;++it,++_cntr){
		CUIActorStaticticDetail* itm		= xr_new<CUIActorStaticticDetail>();
		itm->Init							(&uiXml, path, 0);

		sprintf								(buff,"%d. %s",_cntr, *CStringTable().translate((*it).key));
		itm->m_text1->SetTextST				(buff);

		sprintf								(buff,"x%d", (*it).count);
		itm->m_text2->SetTextST				(buff);

		sprintf								(buff,"%d", (*it).points);
		itm->m_text3->SetTextST				(buff);

		UIDetailList->AddWindow				(itm, true);
	}
}
void	CUIActorInfoWnd::Reset()
{
	inherited::Reset();
}

void	CUIActorInfoWnd::FillReputationDetails		(CUIXml* xml, LPCSTR path)
{
	XML_NODE* _list_node					= xml->NavigateToNode	("relation_communities_list",0);
	int cnt = xml->GetNodesNum				("relation_communities_list",0,"r");

	CHARACTER_COMMUNITY						comm;


	CHARACTER_REPUTATION					rep_actor, rep_neutral;
	rep_actor.set							(Actor()->Reputation());
	rep_neutral.set							(NEUTAL_REPUTATION);

	CHARACTER_GOODWILL d_neutral			= CHARACTER_REPUTATION::relation(rep_actor.index(), rep_neutral.index());


	string64 buff;
	for(int i=0;i<cnt;++i){
		CUIActorStaticticDetail* itm		= xr_new<CUIActorStaticticDetail>();
		itm->Init							(xml, path, 0);
		comm.set							(xml->Read(_list_node,"r",i,"unknown_community"));
		itm->m_text1->SetTextST				(*(comm.id()));
		
		CHARACTER_GOODWILL	gw				= RELATION_REGISTRY().GetCommunityGoodwill(comm.index(), Actor()->ID());
		gw									+= CHARACTER_COMMUNITY::relation(Actor()->Community(),comm.index());
		gw									+= d_neutral;

		itm->m_text2->SetTextST				(InventoryUtilities::GetGoodwillAsText(gw));
		itm->m_text2->SetTextColor			(InventoryUtilities::GetGoodwillColor(gw));

		sprintf								(buff,"%d", gw);
		itm->m_text3->SetTextST				(buff);

		UIDetailList->AddWindow				(itm, true);
	}
}


CUIActorStaticticHeader::CUIActorStaticticHeader	(CUIActorInfoWnd* w)
:m_actorInfoWnd(w)
{}


void CUIActorStaticticHeader::Init	(CUIXml* xml, LPCSTR path, int idx)
{
	XML_NODE* _stored_root				= xml->GetLocalRoot();

	CUIXmlInit							xml_init;
	xml_init.InitWindow					(*xml, path, idx, this);

	xml->SetLocalRoot					(xml->NavigateToNode(path,idx));

	m_text1								= xr_new<CUIStatic>(); m_text1->SetAutoDelete(true);
	AttachChild							(m_text1);
	xml_init.InitStatic					(*xml, "text_1", 0, m_text1);

	m_text2								= xr_new<CUIStatic>(); m_text2->SetAutoDelete(true);
	AttachChild							(m_text2);
	xml_init.InitStatic					(*xml, "text_2", 0, m_text2);

	xml_init.InitAutoStaticGroup		(*xml, "auto", 0, this);

	m_index								= _ParseItem(xml->ReadAttrib(xml->GetLocalRoot(),"id",0), actor_stats_token);

	m_stored_alpha						= color_get_A(m_text1->GetTextColor());
	xml->SetLocalRoot					(_stored_root);

}

bool CUIActorStaticticHeader::OnMouseDown	(bool left_button)
{
	if(left_button && m_index!=100 && m_index>0){
		m_actorInfoWnd->MasterList().SetSelected	(this);
		return true;
	}else
		return true;
}

void CUIActorStaticticHeader::SetSelected(bool b)
{
	CUISelectable::SetSelected(b);
	m_text1->SetTextColor( subst_alpha(m_text1->GetTextColor(), b?255:m_stored_alpha ));
	if(b){ 
		m_actorInfoWnd->FillPointsDetail			(m_index);
	}
}


void CUIActorStaticticDetail::Init		(CUIXml* xml, LPCSTR path, int idx)
{
	XML_NODE* _stored_root				= xml->GetLocalRoot();

	CUIXmlInit							xml_init;
	xml_init.InitWindow					(*xml, path, idx, this);

	xml->SetLocalRoot					(xml->NavigateToNode(path,idx));

	m_text1								= xr_new<CUIStatic>(); m_text1->SetAutoDelete(true);
	AttachChild							(m_text1);
	xml_init.InitStatic					(*xml, "text_1", 0, m_text1);

	m_text2								= xr_new<CUIStatic>(); m_text2->SetAutoDelete(true);
	AttachChild							(m_text2);
	xml_init.InitStatic					(*xml, "text_2", 0, m_text2);
	
	m_text3								= xr_new<CUIStatic>(); m_text3->SetAutoDelete(true);
	AttachChild							(m_text3);
	xml_init.InitStatic					(*xml, "text_3", 0, m_text3);

	xml_init.InitAutoStaticGroup		(*xml, "auto", 0, this);

	xml->SetLocalRoot					(_stored_root);
}
