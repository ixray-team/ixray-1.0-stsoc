#include "stdafx.h"
#include "uigroup.h"
#include "hudmanager.h"
#include "entity.h"
#include "level.h"
#include "seniority_hierarchy_holder.h"
#include "team_hierarchy_holder.h"
#include "squad_hierarchy_holder.h"
#include "group_hierarchy_holder.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define GROUP_OFFSET		55
#define GROUP_FIRST_ITEM	59
#define GROUP_ITEM_SIZE		10
#define ITEM_COLOR_GOOD		0xFF00A651
#define ITEM_COLOR_MED		0xFFFFF200
#define ITEM_COLOR_BAD		0xFFEF0023

CUIGroup::CUIGroup()
{
	back.Init		("ui\\hud_group_back",		"hud\\default",				0,		8,	alRight|alTop);
	back_sel.Init	("ui\\hud_group_back_sel",	"hud\\default",				0,		8,	alRight|alTop);
	list_top.Init	("ui\\hud_group_list_top",	"hud\\default",				0+3,	42,	alRight|alTop);
	list_item.Init	("ui\\hud_group_list_item",	"hud\\default",				0+3,	0,	alRight|alTop);
	list_bottom.Init("ui\\hud_group_list_bottom","hud\\default",			0+3,	0,	alRight|alTop);
	list_item_health.Init	("ui\\hud_group_list_item_health","hud\\default",0+3,	0,	alRight|alTop);
	list_item_ammo.Init		("ui\\hud_group_list_item_health","hud\\default",0+7,	0,	alRight|alTop);
	back.SetRect	(0,0,50,43);
}
//--------------------------------------------------------------------

const u32 DET_COLOR[3]={ITEM_COLOR_BAD,ITEM_COLOR_MED,ITEM_COLOR_GOOD};

void CUIGroup::Render(CGroupHierarchyHolder& G, int idx, int grp_index, bool bSelected)
{
	// convert
	int offset			= HUD().ClientToScreenX(UI_BASE_WIDTH-(idx+1)*GROUP_OFFSET, alRight|alTop);

	// out labels
	float sc			= HUD().GetScale();
	int sc_offset		= iFloor(offset*sc);

	if (bSelected){
		back_sel.SetPosX(sc_offset);
		back_sel.Render	();
	}else{
		back.SetPosX	(sc_offset);
		back.Render		();
	}
	list_top.SetPosX	(iFloor(sc_offset+3*sc));
	list_top.Render		();
	int item_cnt=0;
	float Y = GROUP_FIRST_ITEM;
	for (u32 i=0; i<G.members().size(); ++i,Y+=GROUP_ITEM_SIZE){
		CEntity* E = G.members()[i];
		if (E->IsVisibleForHUD())
		{
			// update item back
			list_item.SetPos			(iFloor(sc_offset+3*sc),iFloor(Y*sc));
			// update item health
			int val = iFloor			(3*float(E->g_Health())/100.f); clamp(val,0,2);
			list_item_health.SetColor	(DET_COLOR[val]);	
			list_item_health.SetPos		(iFloor(sc_offset+3*sc),iFloor(Y*sc));	
			// update item ammo 
			list_item_ammo.SetColor		(DET_COLOR[2]);	
			list_item_ammo.SetPos		(iFloor(sc_offset+7*sc),iFloor(Y*sc));	
			// render all
			list_item.Render			();
			list_item_health.Render		();
			list_item_ammo.Render		();
			// out name
			HUD().pFontSmall->Out		(float(offset+11),Y,"%6.6s",*E->cName());
			++item_cnt;
		}
	}
	list_bottom.SetPos(iFloor(sc_offset+3*sc),iFloor(Y*sc));
	list_bottom.Render();
	// counters
	HUD().pFontSmall->SetColor	(0xffffffff);
	HUD().pFontSmall->Out		(float(offset+5),	10,	"#%02d",grp_index+1);
	HUD().pFontSmall->Out		(float(offset+30),	40,	"%02d",item_cnt);
}
//--------------------------------------------------------------------

CUISquad::CUISquad(){
}

void CUISquad::Init(){
}

CUISquad::~CUISquad(){
}

void CUISquad::Render(CSquadHierarchyHolder& S, bool* bSel, bool /**bActive/**/){
	int idx=0;
	for (u32 i=0; i<S.groups().size(); ++i,idx){
		CGroupHierarchyHolder& G		= S.group(i);
		if (!G.members().empty())	group.Render(G,idx++,i,bSel[i]);
/*		if (!G.Empty()){
			if (bSelGroups[i]) m_Parent->pSmallFont->Color(0xffffff00);
			m_Parent->pSmallFont->OutSet(x,y);
			m_Parent->pSmallFont->OutNext("%d:G#%d",i+1,i+1);
			m_Parent->pSmallFont->Color(0xffffffff);
			CGroup& G					= S.Groups[i];
			m_Parent->pSmallFont->OutNext("+-----");
			m_Parent->pSmallFont->OutNext("|sz:%d",G.Size());
			m_Parent->pSmallFont->OutNext("+-----");
			m_Parent->pSmallFont->OutNext("|CM:%d",G.State);
			m_Parent->pSmallFont->OutNext("+-----");
			m_Parent->pSmallFont->OutNext("|AG:%s",(G.Flags&gtAgressive)?"+":"-");
			m_Parent->pSmallFont->OutNext("|QT:%s",(G.Flags&gtQuiet)?"+":"-");
			m_Parent->pSmallFont->OutNext("+-----");
		}
*/	}
}

