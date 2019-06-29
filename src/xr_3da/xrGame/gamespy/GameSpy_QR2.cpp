#include "StdAfx.h"
#include "GameSpy_QR2.h"
#include "GameSpy_Base_Defs.h"

#include "GameSpy_QR2_callbacks.h"

CGameSpy_QR2::CGameSpy_QR2()
{
	m_SecretKey[0] = 't';
	m_SecretKey[1] = '9';
	m_SecretKey[2] = 'F';
	m_SecretKey[3] = 'j';
	m_SecretKey[4] = '3';
	m_SecretKey[5] = 'M';
	m_SecretKey[6] = 'x';
	m_SecretKey[7] = '\0';
	//-------------------------------
	hGameSpyDLL = NULL;

	LoadGameSpy();
};

CGameSpy_QR2::~CGameSpy_QR2()
{
	if (hGameSpyDLL)
	{
		FreeLibrary(hGameSpyDLL);
		hGameSpyDLL = NULL;
	}
};

void	CGameSpy_QR2::LoadGameSpy()
{
	LPCSTR			g_name	= "xrGameSpy.dll";
	Log				("Loading DLL:",g_name);
	hGameSpyDLL			= LoadLibrary	(g_name);
	if (0==hGameSpyDLL)	R_CHK			(GetLastError());
	R_ASSERT2		(hGameSpyDLL,"GameSpy DLL raised exception during loading or there is no game DLL at all");

	GAMESPY_LOAD_FN(xrGS_RegisteredKey);
	GAMESPY_LOAD_FN(xrGS_qr2_register_key);
	GAMESPY_LOAD_FN(xrGS_qr2_think);
	GAMESPY_LOAD_FN(xrGS_qr2_shutdown);
	GAMESPY_LOAD_FN(xrGS_qr2_buffer_add);
	GAMESPY_LOAD_FN(xrGS_qr2_buffer_add_int);
	GAMESPY_LOAD_FN(xrGS_qr2_keybuffer_add);

	GAMESPY_LOAD_FN(xrGS_qr2_register_natneg_callback);
	GAMESPY_LOAD_FN(xrGS_qr2_register_clientmessage_callback);
	GAMESPY_LOAD_FN(xrGS_qr2_register_publicaddress_callback);

	GAMESPY_LOAD_FN(xrGS_qr2_init);
}

void	CGameSpy_QR2::Think	(void* qrec)
{
	xrGS_qr2_think(qrec);
}
void	CGameSpy_QR2::ShutDown	(void* qrec)
{
	xrGS_qr2_shutdown(qrec);	
}

void	CGameSpy_QR2::RegisterAdditionalKeys	()
{
	//---- Global Keys ----
	xrGS_qr2_register_key(GAMETYPE_NAME_KEY,			("gametypename"));	
	xrGS_qr2_register_key(DEDICATED_KEY,				("dedicated"));	
	//---- game_sv_base ---
	xrGS_qr2_register_key(G_MAP_ROTATION_KEY,			("maprotation"));
	xrGS_qr2_register_key(G_VOTING_ENABLED_KEY,			("voting"));
	//---- game sv mp ----
	xrGS_qr2_register_key(G_SPECTATOR_MODES_KEY,		("spectatormodes"));
	//---- game_sv_deathmatch ----
	xrGS_qr2_register_key(G_FRAG_LIMIT_KEY,				("fraglimit"));	
	xrGS_qr2_register_key(G_TIME_LIMIT_KEY,				("timelimit"));	
	xrGS_qr2_register_key(G_DAMAGE_BLOCK_TIME_KEY,		("damageblocktime"));
	xrGS_qr2_register_key(G_DAMAGE_BLOCK_INDICATOR_KEY, ("damageblockindicator"));
	xrGS_qr2_register_key(G_ANOMALIES_ENABLED_KEY,		("anomalies"));
	xrGS_qr2_register_key(G_ANOMALIES_TIME_KEY,			("anomaliestime"));
	xrGS_qr2_register_key(G_WARM_UP_TIME_KEY,			("warmuptime"));
	xrGS_qr2_register_key(G_FORCE_RESPAWN_KEY,			("forcerespawn"));
	//---- game_sv_teamdeathmatch ----
	xrGS_qr2_register_key(G_AUTO_TEAM_BALANCE_KEY,		("autoteambalance"));
	xrGS_qr2_register_key(G_AUTO_TEAM_SWAP_KEY,			("autoteamswap"));
	xrGS_qr2_register_key(G_FRIENDLY_INDICATORS_KEY,	("friendlyindicators"));
	xrGS_qr2_register_key(G_FRIENDLY_NAMES_KEY,			("friendlynames"));
	xrGS_qr2_register_key(G_FRIENDLY_FIRE_KEY,			("friendlyfire"));
	//---- game_sv_artefacthunt ----	
	xrGS_qr2_register_key(G_ARTEFACTS_COUNT_KEY,		("artefactscount"));
	xrGS_qr2_register_key(G_ARTEFACT_STAY_TIME_KEY,		("artefactstaytime"));
	xrGS_qr2_register_key(G_ARTEFACT_RESPAWN_TIME_KEY,	("artefactrespawntime"));	
	xrGS_qr2_register_key(G_REINFORCEMENT_KEY,			("reinforcement"));
	xrGS_qr2_register_key(G_SHIELDED_BASES_KEY,			("shieldedbases"));
	xrGS_qr2_register_key(G_RETURN_PLAYERS_KEY,			("returnplayers"));
	xrGS_qr2_register_key(G_BEARER_CANT_SPRINT_KEY,		("bearercant_sprint"));

	//---- Player keys	
	xrGS_qr2_register_key(P_NAME__KEY,					("name_"));
	xrGS_qr2_register_key(P_FRAGS__KEY,					("frags_"));
	xrGS_qr2_register_key(P_DEATH__KEY,					("death_"));
	xrGS_qr2_register_key(P_RANK__KEY,					("rank_"));
	xrGS_qr2_register_key(P_TEAM__KEY,					("p_team_"));
	xrGS_qr2_register_key(P_SPECTATOR__KEY,				("spectator_"));
	xrGS_qr2_register_key(P_ARTEFACTS__KEY,				("artefacts_"));

	//---- Team keys
//	xrGS_qr2_register_key(T_NAME_KEY,					("t_name_key"));
	xrGS_qr2_register_key(T_SCORE_T_KEY,					("t_score_t"));
};

bool	CGameSpy_QR2::Init		(int Public, void* instance)
{	
	//--------- QR2 Init -------------------------/
	//call qr_init with the query port number and gamename, default IP address, and no user data
	if (xrGS_qr2_init(NULL,NULL,GAMESPY_BASEPORT, GAMESPY_GAMENAME, m_SecretKey, Public, 1,
		callback_serverkey, callback_playerkey, callback_teamkey,
		callback_keylist, callback_count, callback_adderror, instance) != e_qrnoerror)
	{
		//		_tprintf(_T("Error starting query sockets\n"));
		Msg("GameSpy::QR2 : Failes to Initialize!");
		return false;
	}
	RegisterAdditionalKeys();

	// Set a function to be called when we receive a game specific message
	xrGS_qr2_register_clientmessage_callback(NULL, callback_cm);

	// Set a function to be called when we receive a nat negotiation request
	xrGS_qr2_register_natneg_callback(NULL, callback_nn);

	Msg("GameSpy::QR2 : Initialized");
	return true;
};

void	CGameSpy_QR2::BufferAdd		(void* outbuf, const char* value)
{
	xrGS_qr2_buffer_add(outbuf, value);
};

void	CGameSpy_QR2::BufferAdd_Int	(void* outbuf, int value)
{
	xrGS_qr2_buffer_add_int(outbuf, value);
};

void	CGameSpy_QR2::KeyBufferAdd	(void* keybuffer, int keyid)
{
	xrGS_qr2_keybuffer_add(keybuffer, keyid);
}