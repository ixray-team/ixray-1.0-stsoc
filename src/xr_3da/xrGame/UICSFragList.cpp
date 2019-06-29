#include "stdafx.h"
#include "UICSFragList.h"
#include "hudmanager.h"
#include "game_cl_base.h"
#include "level.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUICSFragList::CUICSFragList()
{
}
//--------------------------------------------------------------------

void CUICSFragList::Init()
{
	inherited::Init		("ui\\ui_hud_frame",0,0,500,400,alCenter);
}
//--------------------------------------------------------------------
IC bool	pred_player		(LPVOID v1, LPVOID v2)
{
	return ((game_cl_GameState::Player*)v1)->kills>((game_cl_GameState::Player*)v2)->kills;
}
void CUICSFragList::OnFrame()
{
	inherited::OnFrame();
	xr_map<u32,game_cl_GameState::Player>::iterator I=Game().players.begin();
	xr_map<u32,game_cl_GameState::Player>::iterator E=Game().players.end();

	// create temporary (sort by kills)
	items.clear			();
	for (;I!=E;++I)		items.push_back(&I->second);
	std::sort			(items.begin(),items.end(),pred_player);

	// out info
	int k=1;
	CGameFont* H		= HUD().pFontMedium;
	float h 			= H->CurrentHeight();
	float y				= float(list_rect.lt.y);
	float x0			= float(list_rect.lt.x);
	float x1			= x0+20;
    float x2			= x1+200;
	float x3			= x2+50;
	float x4			= x3+50;
	// global info
	H->SetColor			(0xf0ffa0a0); 
	H->Out				(x0,y,"Team 1: %d %d", Game().teams[0].num_targets, Game().teams[0].score);
	H->Out				(x0,y,"__________________________________________________________");
	y+=1*h;
	for (ItemIt mI=items.begin(); items.end() != mI; ++mI){
		game_cl_GameState::Player* P = (game_cl_GameState::Player*)*mI;
		if(P->flags&GAME_PLAYER_FLAG_CS_SPECTATOR) continue;
		if(0 != P->team) continue;
		if (P->flags&GAME_PLAYER_FLAG_LOCAL)	H->SetColor(0xf0a0ffa0);
		else									H->SetColor(0xb0a0a0a0);
		H->Out			(x0,y,"%3d.",		k++);
		H->Out			(x1,y,"%-20s",		P->name);
		H->Out			(x2,y,"%-8s",		P->flags&GAME_PLAYER_FLAG_VERY_VERY_DEAD?"dead":"");
		H->Out			(x3,y,"%-5d",		P->kills);
		H->Out			(x4,y,"ping:%-4d",	P->ping);
		y				+= h;
	}
	y+=2*h;
	H->SetColor			(0xf0a0a0ff); 
	H->Out				(x0,y,"Team 2: %d %d", Game().teams[1].num_targets, Game().teams[1].score);
	H->Out				(x0,y,"__________________________________________________________");
	y+=1*h;
	k=1;
	for (ItemIt mI=items.begin(); items.end() != mI; ++mI){
		game_cl_GameState::Player* P = (game_cl_GameState::Player*)*mI;
		if(P->flags&GAME_PLAYER_FLAG_CS_SPECTATOR) continue;
		if(1 != P->team) continue;
		if (P->flags&GAME_PLAYER_FLAG_LOCAL)	H->SetColor(0xf0a0ffa0);
		else									H->SetColor(0xb0a0a0a0);
		H->Out			(x0,y,"%3d.",		k++);
		H->Out			(x1,y,"%-20s",		P->name);
		H->Out			(x2,y,"%-8s",		P->flags&GAME_PLAYER_FLAG_VERY_VERY_DEAD?"dead":"");
		H->Out			(x3,y,"%-5d",		P->kills);
		H->Out			(x4,y,"ping:%-4d",	P->ping);
		y				+= h;
	}
	y+=2*h;
	H->SetColor			(0xf0afafaf); 
	H->Out				(x0,y,"Spectators", Game().teams[1].num_targets);
	H->Out				(x0,y,"__________________________________________________________");
	y+=1*h;
	k=1;
	for (ItemIt mI=items.begin(); items.end() != mI; ++mI){
		game_cl_GameState::Player* P = (game_cl_GameState::Player*)*mI;
		if(!(P->flags&GAME_PLAYER_FLAG_CS_SPECTATOR)) continue;
		if (P->flags&GAME_PLAYER_FLAG_LOCAL)	H->SetColor(0xf0a0ffa0);
		else									H->SetColor(0xb0a0a0a0);
		H->Out			(x0,y,"%3d.",		k++);
		H->Out			(x1,y,"%-20s",		P->name);
		H->Out			(x2,y,"%-8s",		P->flags&GAME_PLAYER_FLAG_VERY_VERY_DEAD?"    ":"");
		H->Out			(x3,y,"%-5d",		P->kills);
		H->Out			(x4,y,"ping:%-4d",	P->ping);
		y				+= h;
	}
}
//--------------------------------------------------------------------
