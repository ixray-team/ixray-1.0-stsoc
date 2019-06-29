#include "stdafx.h"


#include "game_sv_cs.h"

#ifndef NO_CSTRIKE

#include "HUDManager.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "xrServer.h"
#include "level.h"

cs_money::cs_money() {
	string256 fn;
	if (FS.exist(fn,"$game_data$","game_cs.ltx")) 
	{
		CInifile* ini = CInifile::Create(fn);
		startup = ini->r_s32("cs_money","startup");
		win = ini->r_s32("cs_money","win");
		lose = ini->r_s32("cs_money","lose");
		draw = ini->r_s32("cs_money","draw");
		kill = ini->r_s32("cs_money","kill");
		mission = ini->r_s32("cs_money","mission");
		CInifile::Destroy	(ini);
	}
}

void	game_sv_CS::Create			(LPSTR &options)
{
	__super::Create					(options);

	R_ASSERT2						(rpoints[0].size(), "No respawn point for team 0");
	R_ASSERT2						(rpoints[1].size(), "No respawn point for team 1");
	R_ASSERT2						(rpoints[2].size(), "No respawn point for artifacts");

	teams.resize					(2); // @@@ WT
	timelimit	= get_option_i		(options,"timelimit",0)*60000;	// in (ms)
	switch_Phase					(GAME_PHASE_PENDING);
	m_delayedRoundEnd = false;
}

void game_sv_CS::SavePlayerWeapon(u32 it, CMemoryWriter &store) {
	xrServer *l_pServer = Level().Server;
	game_PlayerState *l_pPS = get_it(it);
	u32 l_chunk = 0;
	NET_Packet l_packet;
	CMemoryWriter &l_mem = store;
	CSE_ALifeCreatureActor *l_pActor = dynamic_cast<CSE_ALifeCreatureActor*>(l_pServer->ID_to_entity(get_id_2_eid(get_it_2_id(it))));
	if(!l_pActor) return;
	xr_vector<u16>* l_pCilds = get_children(get_it_2_id(it));
	for(u32 cit = 0; cit < l_pCilds->size(); ++cit) {
		CSE_ALifeItemWeapon *l_pWeapon = dynamic_cast<CSE_ALifeItemWeapon*>(l_pServer->ID_to_entity((*l_pCilds)[cit]));
		if(!l_pWeapon) continue;
		//l_pWeapon->flags
		u16 id_save = l_pWeapon->ID;			// save wpn entity ID 
		l_pWeapon->ID = 0xffff;					// set 0xffff to get _new gen ID
		l_pWeapon->Spawn_Write(l_packet, true);
		l_pWeapon->ID = id_save;				// restore wpn entity ID 
		l_pWeapon->state = 4;
		l_mem.open_chunk((l_pActor->weapon==l_pWeapon->get_slot())?((int)l_pCilds->size()-(l_pPS->flags&GAME_PLAYER_FLAG_CS_HAS_ARTEFACT?2:1)):l_chunk++); l_mem.w(l_packet.B.data, l_packet.B.count); l_mem.close_chunk();
	}
}

void game_sv_CS::SaveDefaultWeapon(CMemoryWriter &store) {		//@@@ WT: Это надо переделать, чтоб читать ltx только один раз.
	string256 fn;
	if (FS.exist(fn,"$game_data$","game_cs.ltx")) {
		CInifile* ini = CInifile::Create(fn);
		LPCSTR prim = ini->r_string("cs_start_Arms","primary");
		u32 prim_ammo = ini->r_s32("cs_start_Arms","primary_ammo");
		LPCSTR pistol = ini->r_string("cs_start_Arms","pistol");
		u32 pistol_ammo = ini->r_s32("cs_start_Arms","pistol_ammo");
		CSE_ALifeItemWeapon *W_prim = 0, *W_pistol = 0;
		if(prim) {
			W_prim = dynamic_cast<CSE_ALifeItemWeapon*>(spawn_begin(prim));
			if(W_prim) {
				strcpy(W_prim->s_name_replace,prim);
				W_prim->s_flags.assign(M_SPAWN_OBJECT_LOCAL);
				W_prim->ID_Parent = 0;
				W_prim->ID = 0xffff;
				W_prim->a_elapsed = W_prim->get_ammo_magsize();
				W_prim->a_current = u16(prim_ammo) * W_prim->a_elapsed;
				W_prim->state = 4;
			}
		}
		if(pistol) {
			W_pistol = dynamic_cast<CSE_ALifeItemWeapon*>(spawn_begin(pistol));
			if(W_pistol) {
				strcpy(W_pistol->s_name_replace,pistol);
				W_pistol->s_flags.assign(M_SPAWN_OBJECT_LOCAL);
				W_pistol->ID_Parent = 0;
				W_pistol->ID = 0xffff;
				W_pistol->a_elapsed = W_pistol->get_ammo_magsize();
				W_pistol->a_current = u16(pistol_ammo) * W_pistol->a_elapsed;
				W_pistol->state = 4;
			}
		}
		CInifile::Destroy	(ini);
		u32 l_chunk = 0;
		NET_Packet l_packet;
		CMemoryWriter &l_mem = store;
		if(W_prim) {
			W_prim->Spawn_Write(l_packet, true);
			l_mem.open_chunk(l_chunk++); l_mem.w(l_packet.B.data, l_packet.B.count); l_mem.close_chunk();
		}
		if(W_pistol) {
			W_pistol->Spawn_Write(l_packet, true);
			l_mem.open_chunk(l_chunk); l_mem.w(l_packet.B.data, l_packet.B.count); l_mem.close_chunk();
		}
		if(W_prim) F_entity_Destroy(W_prim);
		if(W_pistol) F_entity_Destroy(W_pistol);
	}
}

void game_sv_CS::SpawnArtifacts() {
	xr_vector<RPoint>&		rp	= rpoints[2];
	srand				( Device.TimerAsync() );
	std::random_shuffle	( rp.begin( ), rp.end( ) );
	for(s32 i = 0; i < 3; ++i) {
		CSE_Abstract*		E	=	spawn_begin	("m_target_cs");								// create SE
		CSE_Target_CS*	A		=	(CSE_Target_CS*) E;					
		A->s_flags.assign			(M_SPAWN_OBJECT_LOCAL);				// flags
		RPoint&				r	= rp[i];
		A->o_Position.set	(r.P);
		A->o_Angle.set		(r.A);
		spawn_end			(A,0);
	}
}

void game_sv_CS::SpawnPlayer(u32 it, CMemoryWriter &weapon) {
	game_PlayerState *l_pPS = get_it(it);

	LPCSTR	options = get_name_it(it);
	CSE_Abstract *E;
	if(l_pPS->flags&GAME_PLAYER_FLAG_CS_SPECTATOR) {
		l_pPS->flags |= GAME_PLAYER_FLAG_VERY_VERY_DEAD;
		E = spawn_begin("spectator");
	} else {
		E = spawn_begin("actor"/*l_pPS->team?"actor_cs_2":"actor_cs_1"*/);
		CSE_ALifeCreatureActor *A = dynamic_cast<CSE_ALifeCreatureActor*>(E);					
		A->s_team = u8(l_pPS->team);
	}
	strcpy(E->s_name_replace,get_option_s(options,"name","Player"));
	E->s_flags.assign(M_SPAWN_OBJECT_LOCAL|M_SPAWN_OBJECT_ASPLAYER);
	xr_vector<RPoint> &rp = rpoints[l_pPS->team];
	RPoint &r = rp[it%rp.size()];
	E->o_Position.set(r.P);
	E->o_Angle.set(r.A);
	spawn_end(E,get_it_2_id(it));

	if(!(l_pPS->flags&GAME_PLAYER_FLAG_CS_SPECTATOR)) {
		NET_Packet l_packet;
		CMemoryWriter &l_mem = weapon;
		u32 l_chunk = 0;
		u16 skip_header;
		IReader l_stream(l_mem.pointer(), l_mem.size()), *l_pS;
		while(NULL != (l_pS = l_stream.open_chunk(l_chunk++))) {
			l_packet.B.count = l_pS->length();
			l_pS->r(l_packet.B.data, l_packet.B.count);
			l_packet.r_begin(skip_header);
			Level().Server->Process_spawn(l_packet,get_it_2_id(it),true);
		}
	}
}

void game_sv_CS::OnRoundStart() {
	m_delayedRoundEnd = false;
	u32 l_cnt = get_count();

	// Сохраняем оружие для следующего раунда
	// Для живых - то что у них есть, для мертвых - дефолтное.
	xr_vector<CMemoryWriter> l_memAr; l_memAr.resize(l_cnt);
	for(u32 i = 0; i < l_cnt; ++i) {
		game_PlayerState *l_pPS = get_it(i);
		if(l_pPS->flags&GAME_PLAYER_FLAG_CS_SPECTATOR) continue;		// Наблюдателей это не касается
		if((round==-1)||(l_pPS->flags&GAME_PLAYER_FLAG_VERY_VERY_DEAD)) SaveDefaultWeapon(l_memAr[i]);
		else SavePlayerWeapon(i, l_memAr[i]);
	}

	__super::OnRoundStart();

	// Обновляем состаяние всех игроков и команд
	if(round == 0) {
		teams[0].score = teams[1].score = 0;
		for(u32 i = 0; i < l_cnt; ++i) {
			game_PlayerState *l_pPS = get_it(i);
			if(l_pPS->flags&GAME_PLAYER_FLAG_CS_SPECTATOR) continue;		// Наблюдателей это не касается
			l_pPS->money_total = money.startup;
			l_pPS->money_for_round = 0;
			l_pPS->flags = 0;
			l_pPS->deaths = 0;
			l_pPS->kills = 0;
		}
	} else {
		for(u32 i = 0; i < l_cnt; ++i) {
			game_PlayerState *l_pPS = get_it(i);
			if(l_pPS->flags&GAME_PLAYER_FLAG_CS_SPECTATOR) continue;		// Наблюдателей это не касается
			l_pPS->money_total = l_pPS->money_total + l_pPS->money_for_round;
			l_pPS->money_for_round = 0;
			l_pPS->flags = 0;
		}
	}
	teams[0].num_targets = teams[1].num_targets = 0;
	SpawnArtifacts	();
	srand			( Device.TimerAsync() );
	xr_vector<RPoint> &rp1 = rpoints[0];
	std::random_shuffle(rp1.begin(), rp1.end());
	xr_vector<RPoint> &rp2 = rpoints[0];
	std::random_shuffle(rp2.begin(), rp2.end());
	for(u32 i = 0; i < l_cnt; ++i) SpawnPlayer(i, l_memAr[i]);
}


//void	game_sv_CS_OnRoundStart	()
//{
//	m_delayedRoundEnd = false;
//	NET_Packet l_packet;
//	xr_vector<CMemoryWriter> l_memAr;
//	if(-1!=round) {							// Если раунд не первый сохраняем игроков и их оружие
//		xrServer *l_pServer = Level().Server;
//		u32 cnt = get_count();
//		l_memAr.resize(cnt);
//		for(u32 it = 0; it < cnt; ++it) {
//			if(get_it(it)->flags&GAME_PLAYER_FLAG_VERY_VERY_DEAD) continue;
//			u32 l_chunk = 0;
//			CMemoryWriter &l_mem = l_memAr[it];
//			xr_vector<u16>* l_pCilds = get_children(get_it_2_id(it));
//			for(u32 cit = 0; cit < l_pCilds->size(); ++cit) {
//				CSE_ALifeItemWeapon *l_pWeapon = dynamic_cast<CSE_ALifeItemWeapon*>(l_pServer->ID_to_entity((*l_pCilds)[cit]));
//				if(!l_pWeapon) continue;
//				u16 id_save = l_pWeapon->ID;			// save wpn entity ID 
//				l_pWeapon->ID = 0xffff;					// set 0xffff to get _new gen ID
//				l_pWeapon->Spawn_Write(l_packet, true);
//				l_pWeapon->ID = id_save;				// restore wpn entity ID 
//				l_mem.open_chunk(l_chunk++); l_mem.write(l_packet.B.data, l_packet.B.count); l_mem.close_chunk();
//			}
//		}
//	}
//	{
//		string256 fn;
//		if (FS.exist(fn,Path.GameData,"game_cs.ltx")) {
//			CInifile* ini = CInifile::Create(fn);
//			LPCSTR prim = ini->r_string("cs_start_Arms","primary");
//			u32 prim_ammo = ini->r_s32("cs_start_Arms","primary_ammo");
//			LPCSTR pistol = ini->r_string("cs_start_Arms","pistol");
//			u32 pistol_ammo = ini->r_s32("cs_start_Arms","pistol_ammo");
//			CSE_ALifeItemWeapon *W_prim = 0, *W_pistol = 0;
//			if(prim) {
//				W_prim = dynamic_cast<CSE_ALifeItemWeapon*>(spawn_begin(prim));
//				if(W_prim) {
//					strcpy(W_prim->s_name_replace,prim);
//					W_prim->s_flags = M_SPAWN_OBJECT_LOCAL;
//					W_prim->ID_Parent = 0;
//					W_prim->ID = 0xffff;
//					W_prim->a_elapsed = W_prim->get_ammo_magsize();
//					W_prim->a_current = u16(prim_ammo) * W_prim->a_elapsed;
//				}
//			}
//			if(pistol) {
//				W_pistol = dynamic_cast<CSE_ALifeItemWeapon*>(spawn_begin(pistol));
//				if(W_pistol) {
//					strcpy(W_pistol->s_name_replace,pistol);
//					W_pistol->s_flags = M_SPAWN_OBJECT_LOCAL;
//					W_pistol->ID_Parent = 0;
//					W_pistol->ID = 0xffff;
//					W_pistol->a_elapsed = W_pistol->get_ammo_magsize();
//					W_pistol->a_current = u16(pistol_ammo) * W_pistol->a_elapsed;
//				}
//			}
//			CInifile::Destroy	(ini);
//			u32 cnt = get_count();
//			l_memAr.resize(cnt);
//			for(u32 it = 0; it < cnt; ++it) {
//				if(!(get_it(it)->flags&GAME_PLAYER_FLAG_VERY_VERY_DEAD) && (-1!=round)) continue;
//				u32 l_chunk = 0;
//				CMemoryWriter &l_mem = l_memAr[it];
//				if(W_prim) {
//					W_prim->Spawn_Write(l_packet, true);
//					l_mem.open_chunk(l_chunk++); l_mem.write(l_packet.B.data, l_packet.B.count); l_mem.close_chunk();
//				}
//				if(W_pistol) {
//					W_pistol->Spawn_Write(l_packet, true);
//					l_mem.open_chunk(l_chunk); l_mem.write(l_packet.B.data, l_packet.B.count); l_mem.close_chunk();
//				}
//			}
//			if(W_prim) F_entity_Destroy(W_prim);
//			if(W_pistol) F_entity_Destroy(W_pistol);
//		}
//	}
//
//	__super::OnRoundStart	();
//
//	if (0==round)	
//	{
//		// give $1000 to everybody
//		u32		cnt = get_count();
//		for		(u32 it=0; it<cnt; ++it)	
//		{
//			game_PlayerState*	ps	=	get_it	(it);
//			ps->money_total			=	1000;
//			ps->money_for_round		=	0;
//			ps->flags = 0;
//			ps->deaths = 0;
//			ps->kills = 0;
//		}
//	} else {
//		// sum-up money for round with total
//		u32		cnt = get_count();
//		for		(u32 it=0; it<cnt; ++it)	
//		{
//			game_PlayerState*	ps	=	get_it	(it);
//			ps->money_total			=	ps->money_total + ps->money_for_round;
//			ps->money_for_round		=	0;
//			ps->flags = 0;
//		}
//	}
//	teams[0].num_targets = teams[1].num_targets = 0;
//
//	// Spawn "artifacts"
//	xr_vector<RPoint>&		rp	= rpoints[2];
//	srand				( (unsigned)time( NULL ) );
//	random_shuffle		( rp.begin( ), rp.end( ) );
//	for(s32 i = 0; i < 3; ++i) {
//		CSE_Abstract*		E	=	spawn_begin	("m_target_cs");									// create SE
//		CSE_Target_CS*	A		=	(CSE_Target_CS*) E;					
//		A->s_flags				=	M_SPAWN_OBJECT_LOCAL;				// flags
//		RPoint&				r	= rp[i];
//		A->o_Position.set	(r.P);
//		A->o_Angle.set		(r.A);
//		spawn_end			(A,0);
//	}
//
//	// Respawn all players and some info
//	u32		cnt = get_count();
//	for		(u32 it=0; it<cnt; ++it)	
//	{
//		// init
//		game_PlayerState*	ps	=	get_it	(it);
//		//ps->kills				=	0;
//		//ps->deaths				=	0;
//
//		// spawn
//		LPCSTR	options			=	get_name_it	(it);
//		CSE_Abstract*		E	=	spawn_begin	(ps->team==2?"spectator":(ps->team?"actor_cs_2":"actor_cs_1"));						// create SE
//		CSE_ALifeCreatureActor*	A			=	(CSE_ALifeCreatureActor*) E;					
//		strcpy					(A->s_name_replace,get_option_s(options,"name","Player"));					// name
//		A->s_team				=	u8(ps->team);															// team
//		A->s_flags				=	M_SPAWN_OBJECT_LOCAL | M_SPAWN_OBJECT_ASPLAYER;// flags
//		assign_RP				(A);
//		spawn_end				(A,get_it_2_id(it));
//
//		if(l_memAr.size() > it) {
//			CMemoryWriter &l_mem = l_memAr[it];
//			u32 l_chunk = 0;
//			u16 skip_header;
//			IReader l_stream(l_mem.pointer(), l_mem.size()), *l_pS;
//			while(NULL != (l_pS = l_stream.open_chunk(l_chunk++))) {
//				l_packet.B.count = l_pS->Length();
//				l_pS->Read(l_packet.B.data, l_packet.B.count);
//				l_packet.r_begin(skip_header);
//				Level().Server->Process_spawn(l_packet,get_it_2_id(it),true);
//			}
//		}
//	}
//}

void	game_sv_CS::OnRoundEnd		(LPCSTR reason)
{
	__super::OnRoundEnd(reason);
//	u32 cnt = get_count(); for(u32 it=0; it<cnt; ++it) get_it(it)->flags &= (GAME_PLAYER_FLAG_CS_SPECTATOR|GAME_PLAYER_FLAG_VERY_VERY_DEAD);
}

void	game_sv_CS::OnDelayedRoundEnd		(LPCSTR /**reason/**/)
{
	m_delayedRoundEnd = true;
	m_roundEndDelay = Device.TimerAsync() + 10000;
}

void	game_sv_CS::OnTeamScore		(u32 team)
{
	if(GAME_PHASE_INPROGRESS != phase) return;

	++(teams[team].score);
	// Increment/decrement money
	u32		cnt = get_count();
	for		(u32 it=0; it<cnt; ++it)	
	{
		game_PlayerState*	ps	=	get_it	(it);
		ps->money_for_round = ps->money_for_round + (s32(team)==ps->team)?(s16)money.win:(s16)money.lose;
	}
	phase = u16(team?GAME_PHASE_TEAM2_SCORES:GAME_PHASE_TEAM1_SCORES);
}

void	game_sv_CS::OnTeamsInDraw	()
{
	if(GAME_PHASE_INPROGRESS != phase) return;

	// Give $1000 to everybody
	u32		cnt = get_count();
	for		(u32 it=0; it<cnt; ++it)	
	{
		game_PlayerState*	ps	=	get_it	(it);
		ps->money_for_round = ps->money_for_round + (s16)money.draw;
	}
	phase = GAME_PHASE_TEAMS_IN_A_DRAW;
}

void	game_sv_CS::OnPlayerKillPlayer	(u32 id_killer, u32 id_killed)
{
	game_PlayerState*	ps_killer	=	get_id	(id_killer);
	game_PlayerState*	ps_killed	=	get_id	(id_killed);
	ps_killed->flags				|=	GAME_PLAYER_FLAG_VERY_VERY_DEAD;
	ps_killed->deaths				+=	1;
	if (ps_killer->team == ps_killed->team)	
	{
		// Teammate killed - no frag, chop money
//		ps_killer->money_for_round	-=	500;
		ps_killer->money_total	-=	money.kill; if(ps_killer->money_total < 0) ps_killer->money_total = 0;
	} else {
		// Opponent killed - frag + money
//		ps_killer->money_for_round	+=	500;
		ps_killer->money_total	+=	money.kill;
		ps_killer->kills			+=	1;

	}
	// Check if there is no opponents left
	u32 alive					=	get_alive_count	(ps_killed->team);
	if (0==alive) {
		OnTeamScore(ps_killer->team);
		if(get_alive_count(ps_killer->team)) OnDelayedRoundEnd("ENEMY_quelled");
		else OnRoundEnd("ENEMY_quelled");/**/
		signal_Syncronize();
	}

	xrServer*	S					=	Level().Server;

	// Drop everything
	xr_vector<u16>*	C				=	get_children(id_killed);
	if (0==C)						return;
	while(C->size())
	{
		u16		eid						= (*C)[0];

		CSE_Abstract*		from		= S->ID_to_entity(get_id_2_eid(id_killed));
		CSE_Abstract*		what		= S->ID_to_entity(eid);
		S->Perform_reject				(what,from);
	}

	// spectator
	CSE_Spectator*		A			=	(CSE_Spectator*)spawn_begin	("spectator");															// create SE
	strcpy							(A->s_name_replace,get_option_s(get_name_id(id_killed),"name","Player"));					// name
	A->s_flags.assign				(M_SPAWN_OBJECT_LOCAL | M_SPAWN_OBJECT_ASPLAYER);					// flags
	A->o_Position					=	S->ID_to_entity(get_id_2_eid(id_killed))->o_Position;
	spawn_end						(A,id_killed);
}

void	game_sv_CS::OnPlayerDisconnect	(u32 id_who)
{
	__super::OnPlayerDisconnect			(id_who);

	xrServer*	S					=	Level().Server;

	// Drop everything
	xr_vector<u16>*	C				=	get_children(id_who);
	if (0==C)						return;
	while(C->size())
	{
		u16		eid						= (*C)[0];

		CSE_Abstract*		from		= S->ID_to_entity(get_id_2_eid(id_who));
		CSE_Abstract*		what		= S->ID_to_entity(eid);
		S->Perform_reject				(what,from);
	}
}

void	game_sv_CS::OnTimelimitExceed	()
{
	// Если у команд поровну артефактов, то ничья.
	R_ASSERT(teams.size() == 2);
	if(teams[0].num_targets == teams[1].num_targets) OnTeamsInDraw();
	else OnTeamScore((teams[0].num_targets > teams[1].num_targets) ? 0 : 1);
	OnRoundEnd		("TIME_limit");
}

BOOL	game_sv_CS::OnTouch			(u16 eid_who, u16 eid_what)
{
	xrServer*			S		= Level().Server;
	CSE_Abstract*		e_who	= S->ID_to_entity(eid_who);		VERIFY(e_who	);
	CSE_Abstract*		e_what	= S->ID_to_entity(eid_what);	VERIFY(e_what	);

	CSE_ALifeCreatureActor*			A		= dynamic_cast<CSE_ALifeCreatureActor*> (e_who);
	if (A) 	{
		// player_id = entity->owner->ID			-- 
		// entity->owner->owner == entity			-- is this entity player

		game_PlayerState*	ps_who	=	get_id			(e_who->owner->ID);

		if(ps_who->flags&GAME_PLAYER_FLAG_VERY_VERY_DEAD) return false;

		// Actor touches something
		CSE_ALifeItemWeapon*	W			=	dynamic_cast<CSE_ALifeItemWeapon*> (e_what);
		if (W) 
		{
			// Weapon
			xr_vector<u16>&	C			=	A->children;
			//u8 slot						=	W->get_slot	();
			for (u32 it=0; it<C.size(); ++it)
			{
				CSE_Abstract*		Et	= S->ID_to_entity				(C[it]);
				if (0==Et)				continue;
				CSE_ALifeItemWeapon*		T	= dynamic_cast<CSE_ALifeItemWeapon*>	(Et);
				if (0==T)				continue;
				//if (slot == T->get_slot())	
				//{
				//	// We've found same slot occupied - disallow ownership
				//	return FALSE;
				//}
			}

			// Weapon slot empty - ownership OK
			return TRUE;
		}

		CSE_Target_CS_Base *l_pCSBase	=	dynamic_cast<CSE_Target_CS_Base*>(e_what);
		if(l_pCSBase) {
			// База
			if(ps_who->team == -1) ps_who->team = l_pCSBase->g_team(); // @@@ WT : Пока не сделан респавн
			if(l_pCSBase->s_team == ps_who->team) {				// Если игрок пришел на свою базу
				ps_who->flags |= GAME_PLAYER_FLAG_CS_ON_BASE;
			} else ps_who->flags |= GAME_PLAYER_FLAG_CS_ON_ENEMY_BASE;
			return false;
		}

		CSE_Target_CS *l_pMBall =  dynamic_cast<CSE_Target_CS*>(e_what);
		if(l_pMBall) {
			// Мяч
			if(ps_who->flags&GAME_PLAYER_FLAG_CS_HAS_ARTEFACT)		return false;
			ps_who->flags |= GAME_PLAYER_FLAG_CS_HAS_ARTEFACT;
			return true;
		}

		CSE_Target_CS_Cask *l_pCSCask =  dynamic_cast<CSE_Target_CS_Cask*>(e_what);
		if(l_pCSCask) {
			// Бочка
			// Если игрок на своей базе и у него есть мяч
			if((ps_who->flags&GAME_PLAYER_FLAG_CS_HAS_ARTEFACT) && (ps_who->flags&GAME_PLAYER_FLAG_CS_ON_BASE))		{
				l_pMBall = NULL;									// Отбираем у игрока мяч
				xr_vector<u16>&	C			=	A->children;
				for (u32 it=0; it<C.size(); ++it) {
					l_pMBall = dynamic_cast<CSE_Target_CS*>(S->ID_to_entity(C[it]));
					if (l_pMBall) break;
				}
				R_ASSERT(l_pMBall);
				S->Perform_transfer(l_pMBall, A, l_pCSCask);		// Кладем мяч в бочку
				ps_who->flags &= ~GAME_PLAYER_FLAG_CS_HAS_ARTEFACT;
				++(teams[ps_who->team].num_targets);
				if(teams[ps_who->team].num_targets == 3) {
					OnTeamScore(ps_who->team);
					u32	cnt = get_count();						// Доп. бонус за выполнение задания
					for(u32 it=0; it<cnt; ++it)	{
						game_PlayerState* ps = get_it(it);
						if(ps->team == ps_who->team) ps->money_for_round = ps->money_for_round + (s16)money.mission;
					}											//
					OnDelayedRoundEnd("MISSION_complete");
				}
				signal_Syncronize();
				return false;
			}
			// Если игрок на чужой базе и у него нет мяча
			if(!(ps_who->flags&GAME_PLAYER_FLAG_CS_HAS_ARTEFACT) && (ps_who->flags&GAME_PLAYER_FLAG_CS_ON_ENEMY_BASE))		{
				l_pMBall = NULL;									// Достаем мяч из бочки
				xr_vector<u16>&	C			=	l_pCSCask->children;
				for (u32 it=0; it<C.size(); ++it) {
					l_pMBall = dynamic_cast<CSE_Target_CS*>(S->ID_to_entity(C[it]));
					if(l_pMBall) break;
				}
				if (l_pMBall){
					S->Perform_transfer(l_pMBall, l_pCSCask, A);		// Отдаем игроку
					ps_who->flags |= GAME_PLAYER_FLAG_CS_HAS_ARTEFACT;
					--(teams[(ps_who->team+1)%2].num_targets);
					signal_Syncronize();
				}
				return false;
			}
		}
	}

	// We don't know what the hell is it, so disallow ownership just for safety 
	return FALSE;
}

BOOL	game_sv_CS::OnDetach		(u16 eid_who, u16 eid_what)
{
	xrServer*			S		= Level().Server;
	CSE_Abstract*		e_who	= S->ID_to_entity(eid_who);		VERIFY(e_who	);
	CSE_Abstract*		e_what	= S->ID_to_entity(eid_what);	VERIFY(e_what	);

	CSE_ALifeCreatureActor*			A		= dynamic_cast<CSE_ALifeCreatureActor*> (e_who);
	if (A) 	{
		game_PlayerState*	ps_who	=	get_id			(e_who->owner->ID);


		CSE_Target_CS *l_pMBall =  dynamic_cast<CSE_Target_CS*>(e_what);
		if(l_pMBall) {
			// Мяч
			if(ps_who->flags&GAME_PLAYER_FLAG_CS_HAS_ARTEFACT)	{
				ps_who->flags &= ~GAME_PLAYER_FLAG_CS_HAS_ARTEFACT;
				return true;
			}
			return false;
		}

		CSE_Target_CS_Base *l_pCSBase	=	dynamic_cast<CSE_Target_CS_Base*>(e_what);
		if(l_pCSBase) {
			if(l_pCSBase->s_team == ps_who->team) {				// Если игрок пришел на свою базу
				ps_who->flags &= ~GAME_PLAYER_FLAG_CS_ON_BASE;
			} else ps_who->flags &= ~GAME_PLAYER_FLAG_CS_ON_ENEMY_BASE;
			return false;
		}

		CSE_Target_CS_Cask *l_pCSCask =  dynamic_cast<CSE_Target_CS_Cask*>(e_what);
		if(l_pCSCask) return false;
	}
	return TRUE;
}

void	game_sv_CS::Update			()
{
	__super::Update	();
	switch(phase) 	{
		case GAME_PHASE_TEAM1_SCORES :
		case GAME_PHASE_TEAM2_SCORES :
		case GAME_PHASE_TEAMS_IN_A_DRAW :
		case GAME_PHASE_INPROGRESS : {
			if (timelimit) if (s32(Device.TimerAsync()-u32(start_time))>timelimit) OnTimelimitExceed();
			if(m_delayedRoundEnd && m_roundEndDelay < Device.TimerAsync()) OnRoundEnd("Finish");
		} break;
		case GAME_PHASE_PENDING : {
			if ((Device.TimerAsync()-start_time)>u32(30*1000)) OnRoundStart();
		} break;
	}
}

void	game_sv_CS::OnPlayerReady	(u32 id)
{
	if	(GAME_PHASE_PENDING != phase) return;

	game_PlayerState*	ps	=	get_id	(id);
	if (ps)
	{
		if(ps->flags&GAME_PLAYER_FLAG_CS_SPECTATOR) return;
		if (ps->flags & GAME_PLAYER_FLAG_READY)	
		{
			ps->flags &= ~GAME_PLAYER_FLAG_READY;
		} else {
			ps->flags |= GAME_PLAYER_FLAG_READY;

			// Check if all players ready
			u32		cnt		= get_count	();
			u32		ready	= 0;
			for		(u32 it=0; it<cnt; ++it)	
			{
				ps		=	get_it	(it);
				if((ps->flags&GAME_PLAYER_FLAG_READY) || (ps->flags&GAME_PLAYER_FLAG_CS_SPECTATOR))	++ready;
			}

			if (ready == cnt)
			{
				OnRoundStart	();
			}
		}
	}
}
void game_sv_CS::OnPlayerChangeTeam(u32 id_who, s16 team) {
	if(team == 0 || team == 1) {
		s16 l_old_team = get_id(id_who)->team;
		get_id(id_who)->team = team;
		get_id(id_who)->flags &= ~GAME_PLAYER_FLAG_CS_SPECTATOR;
		if(get_alive_count(l_old_team) == 0) {
			OnTeamScore((l_old_team+1)%2);
			OnRoundEnd("????");
		}
	} else {
		get_id(id_who)->flags |= GAME_PLAYER_FLAG_CS_SPECTATOR|GAME_PLAYER_FLAG_VERY_VERY_DEAD;
		if(get_alive_count(0)+get_alive_count(1) == 0) OnRoundEnd("????");
	}
}

void game_sv_CS::OnPlayerConnect	(u32 id_who)
{
	__super::OnPlayerConnect	(id_who);

	LPCSTR	options				=	get_name_id	(id_who);
	game_PlayerState*	ps_who	=	get_id	(id_who);
	ps_who->money_total			=	money.startup;
	ps_who->flags				|=	GAME_PLAYER_FLAG_VERY_VERY_DEAD;
	ps_who->team				=	u8(get_option_i(options,"team",AutoTeam()));
	ps_who->kills				=	0;
	ps_who->deaths				=	0;

	// Spawn "actor"
	CSE_Spectator*		A	=	(CSE_Spectator*)spawn_begin	("spectator");															// create SE
	strcpy					(A->s_name_replace,get_option_s(options,"name","Player"));					// name
	A->s_flags.assign		(M_SPAWN_OBJECT_LOCAL | M_SPAWN_OBJECT_ASPLAYER);	// flags
	assign_RP				(A);
	spawn_end				(A,id_who);

	// Даем игроку децл оружия для начала. Если игрок изначально будет коннектится как наблюдатель то все это не нужно.
/*	string256 fn;
	if (FS.exist(fn,Path.GameData,"game_cs.ltx")) {
		CInifile* ini = CInifile::Create(fn);
		LPCSTR prim = ini->r_string("cs_start_Arms","primary");
		u32 prim_ammo = ini->r_s32("cs_start_Arms","primary_ammo");
		LPCSTR pistol = ini->r_string("cs_start_Arms","pistol");
		u32 pistol_ammo = ini->r_s32("cs_start_Arms","pistol_ammo");
		CSE_ALifeItemWeapon *W_prim = 0, *W_pistol = 0;
		if(prim) {
			W_prim = dynamic_cast<CSE_ALifeItemWeapon*>(spawn_begin(prim));
			if(W_prim) {
				strcpy(W_prim->s_name_replace,prim);
				W_prim->s_flags = M_SPAWN_OBJECT_LOCAL;
				//W_prim->ID_Parent = A->owner->ID;
				W_prim->a_elapsed = W_prim->get_ammo_magsize();
				W_prim->a_current = u16(prim_ammo) * W_prim->a_elapsed;
				NET_Packet						P;
				u16								skip_header;
				W_prim->Spawn_Write					(P,TRUE);
				P.r_begin						(skip_header);
				Level().Server->Process_spawn	(P,id_who,true);
				F_entity_Destroy				(W_prim);
			}
		}
		if(pistol) {
			W_pistol = dynamic_cast<CSE_ALifeItemWeapon*>(spawn_begin(pistol));
			if(W_pistol) {
				strcpy(W_pistol->s_name_replace,pistol);
				W_pistol->s_flags = M_SPAWN_OBJECT_LOCAL;
				W_pistol->a_elapsed = W_pistol->get_ammo_magsize();
				W_pistol->a_current = u16(pistol_ammo) * W_pistol->a_elapsed;
				NET_Packet						P;
				u16								skip_header;
				W_pistol->Spawn_Write					(P,TRUE);
				P.r_begin						(skip_header);
				Level().Server->Process_spawn	(P,id_who,true);
				F_entity_Destroy				(W_pistol);
			}
		}
	}
	*/
}

void game_sv_CS::OnPlayerBuy		(u32 id_who, u16 eid_who, LPCSTR what)
{
	__super::OnPlayerBuy	(id_who,eid_who,what);

	// cost
	int cost				=	get_option_i	(what,"cost",0);
	if (0==cost)			return;
	int iAmmoMagCount		= 	get_option_i	(what,"ammo",0);
	if (iAmmoMagCount)	
	{ 
		// Buy ammo
		//int		slot			=	get_option_i(what,"primary") ? 1:2;

		// Search weapon slot
		xrServer*		S		=	Level().Server;
		xr_vector<u16>*	C		=	get_children(id_who);
		if (0==C)				return;
		for (u32 it=0; it<C->size(); ++it)
		{
			CSE_Abstract*		Et	= S->ID_to_entity				((*C)[it]);
			if (0==Et)				continue;
			CSE_ALifeItemWeapon*		T	= dynamic_cast<CSE_ALifeItemWeapon*>	(Et);
			if (0==T)				continue;
//			if (true || slot == T->get_slot())	
			{
				// We've found this slot - buy something :) |  491 05 36 - Andy

				// TO: VITYA: I didn't bother to check all conditions, money, etc. Sorry :)))
				CSE_ALifeItemWeapon*		W	= T;
				u16		a_limit			= W->get_ammo_limit		();
				u16		a_total			= W->get_ammo_total		();
				u16		a_magsize		= W->get_ammo_magsize	();
				while (iAmmoMagCount && (iAmmoMagCount*a_magsize+a_total)>(a_limit+a_magsize-1))	--iAmmoMagCount;
				if		(0==iAmmoMagCount)	return;
				int		a_cost			= iAmmoMagCount*cost;

				// check if has money to pay
				game_PlayerState*	ps_who	=	get_id	(id_who);
				if(ps_who->money_total < cost)	return;
				ps_who->money_total		= ps_who->money_total - s16(a_cost);

				// Event
				NET_Packet			P;
				u_EventGen			(P, GE_WPN_AMMO_ADD, W->ID);	   
				P.w_u16				(u16(iAmmoMagCount*a_magsize+a_total)>u16(a_limit)?u16(a_limit-a_total):u16(iAmmoMagCount*a_magsize));	// Amount of ammo to add
				u_EventSend			(P);
				return;
			}
		}
	} 
	else 
	{
		// Buy weapon
		// entity-name
		string64				name;
		strcpy					(name,what);
		if ( strchr(name,'/') )	*strchr(name,'/') = 0;

		// initialize spawn
		CSE_Abstract*		E	=	spawn_begin	(name);														// create SE
		strcpy					(E->s_name_replace,name);													// name
		E->s_flags.assign		(M_SPAWN_OBJECT_LOCAL);							// flags
		E->ID_Parent			=	u16(eid_who);

		// check if has same-slot-weapon(s)
		CSE_ALifeItemWeapon*		W	=	dynamic_cast<CSE_ALifeItemWeapon*>(E);
		W->state = 4;
		if (W)
		{
			xrServer*		S		=	Level().Server;
			xr_vector<u16>*	C		=	get_children(id_who);
			if (0==C)				
			{
				F_entity_Destroy	(E);
				return;
			}
			//u8 slot					=	W->get_slot	();
			for (u32 it=0; it<C->size(); ++it)
			{
				CSE_Abstract*		Et	= S->ID_to_entity				((*C)[it]);
				if (0==Et)				continue;
				CSE_ALifeItemWeapon*		T	= dynamic_cast<CSE_ALifeItemWeapon*>	(Et);
				if (0==T)				continue;
				//if (slot == T->get_slot())	
				//{
				//	// We've found same slot occupied - don't buy anything
				//	F_entity_Destroy	(E);
				//	return;

				//	// Выбрасываем старое
				//	//S->Perform_reject				(T,S->ID_to_entity(eid_who));
				//}
			}
			W->a_current = 0;
			W->a_elapsed = W->get_ammo_magsize();
		}

		// check if has money to pay
		game_PlayerState*	ps_who	=	get_id	(id_who);
		if(ps_who->money_total < cost)	{ F_entity_Destroy(E); return; }
		ps_who->money_total			= ps_who->money_total - s16(cost);

		// Spawn item
		spawn_end				(E,	id_who);
	}
}

u8 game_sv_CS::AutoTeam() 
{
	u32	cnt = get_count(), l_teams[2] = {0,0};
	for(u32 it=0; it<cnt; ++it)	{
		game_PlayerState* ps = get_it(it);
		if(ps->team>=0) ++(l_teams[ps->team]);
	}
	return (l_teams[0]>l_teams[1])?1:0;
}

#endif