#pragma once

#define NO_CSTRIKE


#ifndef NO_CSTRIKE

#include "game_sv_base.h"

class cs_money {
public:
	cs_money		();
	s32 startup, win, lose, draw, kill, mission;
};

class	game_sv_CS					: public game_sv_GameState
{
public:
									game_sv_CS				(){type = GAME_CS;}
	virtual		void				Create					(LPSTR &options);
	virtual		LPCSTR				type_name			() const { return "cs";};

	// Events
	virtual		void				OnRoundStart			();										// старт раунда
	virtual		void				OnRoundEnd				(LPCSTR reason);
	virtual		void				OnDelayedRoundEnd		(LPCSTR reason);

	virtual		void				OnTeamScore				(u32 team);								// команда выиграла
	virtual		void				OnTeamsInDraw			();										// ничья

	virtual		BOOL				OnTouch					(u16 eid_who, u16 eid_what);
	virtual		BOOL				OnDetach				(u16 eid_who, u16 eid_what);

	virtual		void				OnPlayerConnect			(u32 id_who);
	virtual		void				OnPlayerDisconnect		(u32 id_who);
	virtual		void				OnPlayerBuy				(u32 id_who, u16 eid_who, LPCSTR what);
	virtual		void				OnPlayerReady			(u32 id_who);
	virtual		void				OnPlayerChangeTeam		(u32 id_who, s16 team);
	virtual		void				OnPlayerKillPlayer		(u32 id_killer, u32 id_killed);

	virtual		void				OnFraglimitExceed		()								{};
	virtual		void				OnTimelimitExceed		();
	virtual		void				OnPlayerHitPlayer		(u16 id_hitter, u16 id_hitted, NET_Packet& P) {}; //игрок получил Hit

	// Mains
	virtual		void				Update					();

	u8								AutoTeam				();
	void							SavePlayerWeapon		(u32 it, CMemoryWriter &store);
	void							SaveDefaultWeapon		(CMemoryWriter &store);
	void							SpawnArtifacts			();
	void							SpawnPlayer				(u32 it, CMemoryWriter &weapon);

	BOOL							m_delayedRoundEnd;
	u32								m_roundEndDelay;
	cs_money						money;
};

#endif