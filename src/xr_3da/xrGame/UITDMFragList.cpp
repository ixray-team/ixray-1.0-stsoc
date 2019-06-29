#include "stdafx.h"
#include "UITDMFragList.h"
#include "hudmanager.h"
#include "game_cl_base.h"
#include "level.h"
#include "game_cl_teamdeathmatch.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------

void CUITDMFragList::Init(u8 Team)
{
	m_CurTeam = Team;	
}
//--------------------------------------------------------------------
/*
IC bool	pred_player		(LPVOID v1, LPVOID v2)
{
	return ((game_PlayerState*)v1)->kills>((game_PlayerState*)v2)->kills;
};
*/

void	CUITDMFragList::UpdateItemsList ()
{
	game_cl_GameState::PLAYERS_MAP_IT I=Game().players.begin();
	game_cl_GameState::PLAYERS_MAP_IT E=Game().players.end();

	// create temporary map (sort by kills)
	items.clear			();
	for (;I!=E;++I)		
	{
		game_PlayerState* P = I->second;
		if (!P || P->team != m_CurTeam) continue;
		if (Game().Phase() == GAME_PHASE_INPROGRESS)
		{
//			if (P->testFlag(GAME_PLAYER_FLAG_SPECTATOR)) continue;
		}		

		items.push_back(I->second);
	};
	std::sort			(items.begin(),items.end(),TDM_Compare_Players);
};
/*
void	CUITDMFragList::Update()
{
	inherited::Update();

	CUIStatsListItem *pItem = GetItem(GetItemCount()-1);
	if (!pItem) return;

	pItem->FieldsVector[0]->SetText("TOTAL");

	char Text[1024];
	sprintf(Text, "%d", Game().teams[m_CurTeam-1].score); pItem->FieldsVector[1]->SetText(Text);
};
*/

//--------------------------------------------------------------------
