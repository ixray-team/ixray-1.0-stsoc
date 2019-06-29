#include "stdafx.h"
#include "UIDMFragList.h"
#include "hudmanager.h"
#include "game_cl_base.h"
#include "level.h"
#include "game_cl_deathmatch.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIDMFragList::CUIDMFragList()
{
	SetHeaderColumnText(0, "Name");
	SetHeaderColumnText(1, "");
	SetHeaderColumnText(2, "Frags");
	SetHeaderColumnText(3, "Deaths");
	SetHeaderColumnText(4, "Ping");

	Show();
}

CUIDMFragList::~CUIDMFragList()
{
}

//--------------------------------------------------------------------
CUIStatsListItem*		CUIDMFragList::GetItem			(int index)
{
	return (smart_cast<CUIStatsListItem*> (UIStatsList.GetItem(index)));
};
/*
IC bool	pred_player		(LPVOID v1, LPVOID v2)
{
	return ((game_PlayerState*)v1)->kills>((game_PlayerState*)v2)->kills;
};
*/

void	CUIDMFragList::UpdateItemsList ()
{
	game_cl_GameState::PLAYERS_MAP_IT I=Game().players.begin();
	game_cl_GameState::PLAYERS_MAP_IT E=Game().players.end();

	// create temporary map (sort by kills)
	items.clear			();
	for (;I!=E;++I)		
	{
//		game_PlayerState* PS = (game_PlayerState*) I->second;
		if (Game().Phase() == GAME_PHASE_INPROGRESS)
		{
///			if (PS->testFlag(GAME_PLAYER_FLAG_SPECTATOR)) continue;
		}		
		items.push_back(I->second);
	};
	std::sort			(items.begin(),items.end(),DM_Compare_Players);
}

bool	CUIDMFragList::SetItemData		(u32 ItemID, CUIStatsListItem *pItem)
{
	if (ItemID>= items.size()) return false;

	game_PlayerState* PS = (game_PlayerState*)(items[ItemID]);//(game_cl_GameState::Player*)*mI;

	if (/*PS->testFlag(GAME_PLAYER_FLAG_LOCAL) || */(Level().CurrentViewEntity() && Level().CurrentViewEntity()->ID() == PS->GameID))
	{
		SelectItem(ItemID);
	};

	char Text[1024];
	pItem->FieldsVector[0]->SetText(PS->name);
	pItem->FieldsVector[1]->SetText("");
	if (PS->testFlag(GAME_PLAYER_FLAG_SPECTATOR))
	{
		pItem->FieldsVector[1]->SetText("SPECTR");
		pItem->FieldsVector[2]->SetText("");
		pItem->FieldsVector[3]->SetText("");
		pItem->FieldsVector[3]->SetText("");
	}
	else
	{
		if (PS->testFlag(GAME_PLAYER_FLAG_VERY_VERY_DEAD))
			pItem->FieldsVector[1]->SetText("DEAD");

		sprintf(Text, "%d", PS->kills); pItem->FieldsVector[2]->SetText(Text);
		sprintf(Text, "%d", PS->deaths); pItem->FieldsVector[3]->SetText(Text);
		if (PS == Game().local_player)
			sprintf(Text, "%d/%d", PS->ping, Level().GetRealPing()); 
		else
			sprintf(Text, "%d/%d", PS->ping, PS->Rping); 
		pItem->FieldsVector[4]->SetText(Text);
	}
	
	return true;
};

void	CUIDMFragList::Update()
{
	inherited::Update();

	UpdateItemsList();

	HighlightItem(0xffffffff);
	SelectItem(0xffffffff);
	//---------------------------------------
	while (items.size() < GetItemCount())
	{
		RemoveItem(0);
	};
	//---------------------------------------
	while (items.size() > GetItemCount())
	{
		AddItem();
	};
	//---------------------------------------

	for (u32 i=0; i<GetItemCount(); i++)
	{
		CUIStatsListItem *pItem = GetItem(i);
		if (!pItem) continue;
		if (SetItemData(i, pItem)) continue;

		pItem->FieldsVector[0]->SetText(NULL);
		pItem->FieldsVector[1]->SetText(NULL);
		pItem->FieldsVector[2]->SetText(NULL);
		pItem->FieldsVector[3]->SetText(NULL);
		pItem->FieldsVector[4]->SetText(NULL);
	};
};

//--------------------------------------------------------------------
