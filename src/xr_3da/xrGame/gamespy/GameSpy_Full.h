#pragma once

class CGameSpy_Available;
class CGameSpy_Patching;
class CGameSpy_HTTP;
class CGameSpy_Browser;

class CGameSpy_Full
{
	HMODULE	m_hGameSpyDLL;

	void	LoadGameSpy();

	bool	m_bServicesAlreadyChecked;
public:
	CGameSpy_Full	();
	~CGameSpy_Full	();

	CGameSpy_Available*	m_pGSA;
	CGameSpy_Patching*	m_pGS_Patching;
	CGameSpy_HTTP*		m_pGS_HTTP;
	CGameSpy_Browser*	m_pGS_SB;

	void		Update	();
};
