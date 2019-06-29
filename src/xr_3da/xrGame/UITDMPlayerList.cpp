#include "stdafx.h"
#include "UITDMPlayerList.h"
#include "hudmanager.h"
#include "game_cl_base.h"
#include "level.h"
#include "game_cl_teamdeathmatch.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------

void CUITDMPlayerList::Init(u8 Team)
{
	m_CurTeam = Team;
}
//--------------------------------------------------------------------
/*
IC bool	pred_player		(LPVOID v1, LPVOID v2)
{
	return ((game_PlayerState*)v1)->kills>((game_PlayerState*)v2)->kills;
}
*/

void	CUITDMPlayerList::UpdateItemsList ()
{
	game_cl_GameState::PLAYERS_MAP_IT I=Game().players.begin();
	game_cl_GameState::PLAYERS_MAP_IT E=Game().players.end();

	// create temporary map (sort by kills)
	items.clear			();
	for (;I!=E;++I)		
	{
		game_PlayerState* P = I->second;
		if (!P || P->team != m_CurTeam) continue;

		items.push_back(I->second);
	};
	std::sort			(items.begin(),items.end(),TDM_Compare_Players);
};
//--------------------------------------------------------------------
