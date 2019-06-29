#include "stdafx.h"
#include "playerlist.h"
excluded from build 
game_GameState::game_GameState()
{
	round	= -1;
}

void	game_GameState::OnStartRound	()
{
	++round;
	if (0==round)	
	{
		// give $1000 to everybody
		Lock	();
		u32		cnt = get_count();
		for		(u32 it=0; it<cnt; ++it)	
		{
			game_PlayerState*	ps	=	get_it	(ps);
			ps->money_total			=	1000;
			ps->money_for_round		=	0;
		}
		Unlock	();
	} else {
		// sum-up money for round with total
		Lock	();
		u32		cnt = get_count();
		for		(u32 it=0; it<cnt; ++it)	
		{
			game_PlayerState*	ps	=	get_it	(ps);
			ps->money_total			+=	ps->money_for_round;
			ps->money_for_round		=	0;
		}
		Unlock	();
	}
}

void	game_GameState::OnTeamScore		(u32 team)
{
	// Increment/decrement money
	Lock	();
	u32		cnt = get_count();
	for		(u32 it=0; it<cnt; ++it)	
	{
		game_PlayerState*	ps	=	get_it	(ps);
		ps->money_for_round		+=	(team==ps->team)?+1000:+500;
	}
	Unlock	();
}

void	game_GameState::OnTeamsInDraw	()
{
	// Give $1000 to everybody
	Lock	();
	u32		cnt = get_count();
	for		(u32 it=0; it<cnt; ++it)	
	{
		game_PlayerState*	ps	=	get_it	(ps);
		ps->money_for_round		+=	+1000;
	}
	Unlock	();
}

void	game_GameState::OnPlayerKillPlayer	(u32 id_killer, u32 id_killed)
{
	Lock	();
	game_PlayerState*	ps_killer	=	get_id	(id_killer);
	game_PlayerState*	ps_killed	=	get_id	(id_killed);
	ps_killed->deaths				+=	1;
	if (ps_killer->team == ps_killed->team)	
	{
		// Teammate killed - no frag, chop money
		ps_killer->money_for_round	-=	500;
	} else {
		// Opponent killed - frag + money
		ps_killer->money_for_round	+=	500;
		ps_killer->kills			+=	1;

		// Check if there is no opponents left
		u32 alive					=	get_alive_count	(ps_killed->team);
		if (0==alive)				OnTeamScore(ps_killer->team);
	}
	Unlock	();
}

void	game_GameState::OnTimeElapsed	()
{
	// Artefacts count
	if ()
}

void	game_GameState::OnTargetTouched	()
{
}


/*
CPlayers::CPlayers(void)
{
}

CPlayers::~CPlayers(void)
{
}

void CPlayers::add(NET_Packet& P)
{
	u32				ID;
	P.r_u32			(ID);

	Item			I;
	P.r_stringZ		(I.name);
	P.r_s16			(I.score);
	items.insert	(mk_pair(ID,I));
}

void CPlayers::remove(NET_Packet& P)
{
	u32				ID;
	P.r_u32			(ID);

	items.erase		(ID);
}

CPlayers::Item* CPlayers::access(u32 ID)
{
	xr_map<u32,Item>::iterator	it = items.find	(ID);
	if (items.end() != it)		return &it->second;
	else						return 0;
}
*/
