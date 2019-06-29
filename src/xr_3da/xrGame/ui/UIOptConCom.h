#pragma once

class CUIOptConCom {
public:
	void Init();
	CUIOptConCom();
protected:
	enum{
		flNetSrvDedicated				= 1,
		flNetConPublicServer			= 2,
		flNetConSpectatorOn				= 8,
	};
    int			m_iMaxPlayers;
	Flags32		m_uNetSrvParams;
	u32			m_curGameMode;
	string64	m_playerName;
	string64	m_serverName;
	int			m_iNetConSpectator;
	int			m_iReinforcementType;
	float		m_fNetWeatherRate;
};