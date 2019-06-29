#include "StdAfx.h"
#include "../../xr_ioconsole.h"
#include "../../xr_ioc_cmd.h"
#include "UIOptConCom.h"
#include "../../xrcore/xrCore.h"
#include "../game_base_space.h"
#include "../RegistryFuncs.h"

xr_token g_GameModes	[] = {
	{ "st_deathmatch",			GAME_DEATHMATCH	},
	{ "st_team_deathmatch",		GAME_TEAMDEATHMATCH	},
	{ "st_artefacthunt",		GAME_ARTEFACTHUNT	},
	{ 0,						0}
};

CUIOptConCom::CUIOptConCom()
{
	strcpy(m_playerName, "");
}

class CCC_UserName: public CCC_String{
public:
	CCC_UserName(LPCSTR N, LPSTR V, int _size) : CCC_String(N, V, _size)  { bEmptyArgsHandled = false; };	
	virtual void Execute(LPCSTR arguments)
	{
		CCC_String::Execute(arguments);	

		WriteRegistry_StrValue(REGISTRY_VALUE_USERNAME, value);

//		if (g_pGamePersistent && MainMenu()) MainMenu()->ValidateCDKey();
	}
	virtual void	Save	(IWriter *F)	{};
};

void CUIOptConCom::Init(){
	ReadPlayerNameFromRegistry();
	// common
//	if (xr_strlen(Core.UserName))
//		strcpy				(m_playerName, Core.UserName);
	CMD3				(CCC_UserName,	"mm_net_player_name", m_playerName,	64);

	m_iMaxPlayers		= 32;
	m_curGameMode		= GAME_DEATHMATCH;
	CMD4(CCC_Integer,	"mm_net_srv_maxplayers",			&m_iMaxPlayers,	2, 32);
	CMD3(CCC_Token,		"mm_net_srv_gamemode",				&m_curGameMode,	g_GameModes);
	m_uNetSrvParams.assign(0x0);
	CMD3(CCC_Mask,		"mm_mm_net_srv_dedicated",			&m_uNetSrvParams,	flNetSrvDedicated);
	CMD3(CCC_Mask,		"mm_net_con_publicserver",			&m_uNetSrvParams,	flNetConPublicServer);
	CMD3(CCC_Mask,		"mm_net_con_spectator_on",			&m_uNetSrvParams,	flNetConSpectatorOn);

	m_iNetConSpectator	= 20;
	CMD4(CCC_Integer,	"mm_net_con_spectator",				&m_iNetConSpectator, 1, 32);

	m_iReinforcementType = 1;
	CMD4(CCC_Integer,	"mm_net_srv_reinforcement_type",	&m_iReinforcementType, 0, 2);
	
	m_fNetWeatherRate = 1.0f;
	CMD4(CCC_Float,		"mm_net_weather_rateofchange",		&m_fNetWeatherRate,	0.0, 100.0f);

	strcpy(m_serverName, Core.CompName);
	CMD3(CCC_String,	"mm_net_srv_name",					m_serverName,	64);
};

void		CUIOptConCom::ReadPlayerNameFromRegistry	()
{
	ReadRegistry_StrValue(REGISTRY_VALUE_USERNAME, m_playerName);
	
};

void		CUIOptConCom::WritePlayerNameToRegistry		()
{
	WriteRegistry_StrValue(REGISTRY_VALUE_USERNAME, m_playerName);
};