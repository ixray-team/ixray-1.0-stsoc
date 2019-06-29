#include "stdafx.h"
#include "game_base.h"
#include "ai_space.h"
#include "script_engine.h"
#include "script_space.h"
#include "level.h"
#include "xrMessages.h"

u64		g_qwStartGameTime		= 12*60*60*1000;
float	g_fTimeFactor			= pSettings->r_float("alife","time_factor");
u64		g_qwEStartGameTime		= 12*60*60*1000;

xr_token game_types[];

game_PlayerState::game_PlayerState()
{
	name[0]				=	0;
	team				=	-1;
	kills				=	0;
	m_iKillsInRow		=	0;
	deaths				=	0;
	money_total			=	0;
	money_for_round		=	0;	
	//  [7/27/2005]
	experience_D		=	0;
	experience_Real		=	0;
	rank				=	0;
	af_count			=	0;
	experience_New		=	0;
	//  [7/27/2005]
	flags				=	0;

	skin				=	0;

	Skip = false;
	//---------------------------------
	pItemList.clear();

	LastBuyAcount = 0;
	m_bClearRun = false;
	//---------------------------------
	m_bCurrentVoteAgreed = 2;
	m_s16LastSRoint = -1;
	DeathTime = 0;
	RespawnTime = 0;
	//--------------------------------
	Rping = 0;
	mOldIDs.clear();
	//--------------------------------
	money_added = 0;
	m_aBonusMoney.clear();
	//--------------------------------
	m_bPayForSpawn = false;
}

void game_PlayerState::clear()
{
	name[0]				= 0;
	kills				= 0;
	m_iKillsInRow		= 0;
	deaths				= 0;
	lasthitter		= 0;
	lasthitweapon		= 0;	

	//  [7/27/2005]
	experience_D		=	0;
	experience_Real		=	0;
	rank				=	0;
	af_count			=	0;
	experience_New		=	0;
	//  [7/27/2005]

	pItemList.clear();
	pSpawnPointsList.clear();
	m_s16LastSRoint = -1;
	LastBuyAcount = 0;
	m_bClearRun = false;

	DeathTime = 0;
	mOldIDs.clear();
	//---------------------
	money_added = 0;
	m_aBonusMoney.clear();
}

game_PlayerState::~game_PlayerState()
{
	pItemList.clear();
	pSpawnPointsList.clear();
	m_s16LastSRoint = -1;
	LastBuyAcount = 0;
};

bool game_PlayerState::testFlag	(u16 f)
{
	return !!(flags & f);
}

void game_PlayerState::setFlag	(u16 f)
{
	flags |= f;
}

void game_PlayerState::resetFlag	(u16 f)
{
	flags &= ~(f);
}

void	game_PlayerState::net_Export		(NET_Packet& P)
{
	P.w_stringZ		(	name	);

	P.w_s16			(	team	);
	P.w_s16			(	kills	);
	P.w_s16			(	deaths	);
	P.w_s32			(	money_total	);
	P.w_s32			(	money_for_round	);
	//  [7/27/2005]
	P.w_float_q8	(	experience_D, -1.0f, 2.0f);
	P.w_u8			(	rank		);
	P.w_u8			(	af_count	);
	//  [7/27/2005]
	P.w_u16			(	flags	);
	P.w_u16			(	ping	);
	P.w_u16			(	Rping	);

	P.w_u16			(	GameID	);
	P.w_s8			(	skin	);
	P.w_u8			(	m_bCurrentVoteAgreed	);

	P.w_u32			(Device.dwTimeGlobal - DeathTime);
};

void	game_PlayerState::net_Import		(NET_Packet& P)
{
	P.r_stringZ		(	name	);

	P.r_s16			(	team	);
	P.r_s16			(	kills	);
	P.r_s16			(	deaths	);
	P.r_s32			(	money_total	);
	P.r_s32			(	money_for_round	);
	//  [7/27/2005]
	P.r_float_q8	(	experience_D, -1.0f, 2.0f);
	P.r_u8			(	rank		);
	P.r_u8			(	af_count	);
	//  [7/27/2005]
	P.r_u16			(	flags	);
	P.r_u16			(	ping	);
	P.r_u16			(	Rping	);

	P.r_u16			(	GameID	);
	P.r_s8			(	skin	);
	P.r_u8			(	m_bCurrentVoteAgreed	);

	DeathTime = P.r_u32();
//	DeathTime = Level().timeServer() - xdt;
};

void	game_PlayerState::SetGameID				(u16 NewID)
{
	if (mOldIDs.size()>=10)
	{
		mOldIDs.pop_front();
	};
	mOldIDs.push_back(GameID);
	GameID = NewID;
}
bool	game_PlayerState::HasOldID				(u16 ID)
{
	OLD_GAME_ID_it ID_i = std::find(mOldIDs.begin(), mOldIDs.end(), ID);
	if (ID_i != mOldIDs.end() && *(ID_i)== ID)
		return true;
	return false;
}

game_TeamState::game_TeamState()
{
	score				=	0;
	num_targets			=	0;
}

game_GameState::game_GameState()
{
	type				=	-1;
	phase				=	GAME_PHASE_NONE;
	round				=	-1;

	m_qwStartProcessorTime		= Level().timeServer_Async();
	m_qwStartGameTime			= g_qwStartGameTime;
	m_fTimeFactor				= g_fTimeFactor;
	//-------------------------------------------------------
	m_qwEStartProcessorTime		= m_qwStartProcessorTime;	
	m_qwEStartGameTime			= g_qwEStartGameTime	;
	m_fETimeFactor				= m_fTimeFactor			;
	//-------------------------------------------------------
}

CLASS_ID game_GameState::getCLASS_ID(LPCSTR game_type_name, bool isServer)
{
	string256		S;
	FS.update_path	(S,"$game_config$","script.ltx");
	CInifile		*l_tpIniFile = xr_new<CInifile>(S);
	R_ASSERT		(l_tpIniFile);

	string256				I;
	strcpy(I,l_tpIniFile->r_string("common","game_type_clsid_factory"));

	luabind::functor<LPCSTR>	result;
	R_ASSERT					(ai().script_engine().functor(I,result));
	shared_str clsid = result		(game_type_name, isServer);

	xr_delete			(l_tpIniFile);
	if(clsid.size()==0){
		Debug.fatal(DEBUG_INFO,"Unknown game type: %s",game_type_name);
	}

	return TEXT2CLSID(*clsid);
}

void game_GameState::switch_Phase		(u32 new_phase)
{
		OnSwitchPhase(phase, new_phase);

		phase				= u16(new_phase);
		start_time			= Level().timeServer();//Device.TimerAsync();		
}



ALife::_TIME_ID game_GameState::GetGameTime()
{
	return			(m_qwStartGameTime + iFloor(m_fTimeFactor*float(Level().timeServer_Async() - m_qwStartProcessorTime)));
}

float game_GameState::GetGameTimeFactor()
{
	return			(m_fTimeFactor);
}

void game_GameState::SetGameTimeFactor (const float fTimeFactor)
{
	m_qwStartGameTime			= GetGameTime();
	m_qwStartProcessorTime		= Level().timeServer_Async();
	m_fTimeFactor				= fTimeFactor;
}

void game_GameState::SetGameTimeFactor	(ALife::_TIME_ID GameTime, const float fTimeFactor)
{
	m_qwStartGameTime			= GameTime;
	m_qwStartProcessorTime		= Level().timeServer_Async();
	m_fTimeFactor				= fTimeFactor;
}

ALife::_TIME_ID game_GameState::GetEnvironmentGameTime()
{
	return			(m_qwEStartGameTime + iFloor(m_fETimeFactor*float(Level().timeServer_Async() - m_qwEStartProcessorTime)));
}

float game_GameState::GetEnvironmentGameTimeFactor()
{
	return			(m_fETimeFactor);
}

void game_GameState::SetEnvironmentGameTimeFactor (const float fTimeFactor)
{
	m_qwEStartGameTime			= GetEnvironmentGameTime();
	m_qwEStartProcessorTime		= Level().timeServer_Async();
	m_fETimeFactor				= fTimeFactor;
}

void game_GameState::SetEnvironmentGameTimeFactor	(ALife::_TIME_ID GameTime, const float fTimeFactor)
{
	m_qwEStartGameTime			= GameTime;
	m_qwEStartProcessorTime		= Level().timeServer_Async();
	m_fETimeFactor				= fTimeFactor;
}
