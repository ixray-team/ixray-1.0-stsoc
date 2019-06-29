#include "stdafx.h"
#include "UIDMPlayerList.h"
#include "hudmanager.h"
#include "game_cl_base.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CUIDMPlayerList::CUIDMPlayerList()
{
	SetHeaderColumnText(0, "Name");
	SetHeaderColumnText(1, "");
	SetHeaderColumnText(2, "Frags");
	SetHeaderColumnText(3, "Status");
	SetHeaderColumnText(4, "");
}

CUIDMPlayerList::~CUIDMPlayerList()
{
}

bool	CUIDMPlayerList::SetItemData		(u32 ItemID, CUIStatsListItem *pItem)
{
	if (ItemID>= items.size()) return false;

	game_PlayerState* P = (game_PlayerState*)(items[ItemID]);//(game_cl_GameState::Player*)*mI;

	if (P->testFlag(GAME_PLAYER_FLAG_LOCAL)) SelectItem(ItemID);

	char Text[1024];
	pItem->FieldsVector[0]->SetText(P->name);
	pItem->FieldsVector[1]->SetText("");
	sprintf(Text, "%d", P->kills); pItem->FieldsVector[2]->SetText(Text);
	pItem->FieldsVector[3]->SetText("");
	if (P->testFlag(GAME_PLAYER_FLAG_READY) )
		pItem->FieldsVector[3]->SetText("READY");
	else
		pItem->FieldsVector[3]->SetText("");
	pItem->FieldsVector[4]->SetText("");
	return true;
};
