#pragma once

#include "GameSpy_FuncDefs.h"

class CGameSpy_Available
{
private:
	HMODULE	hGameSpyDLL;

	void	LoadGameSpy();
public:
	CGameSpy_Available();
	~CGameSpy_Available();

	bool	CheckAvailableServices	();
private:
	//------------------------------- GameSpy_Available ---------------------------
	GAMESPY_FN_VAR_DECL(void,	GSIStartAvailableCheck, (const char * gamename));
	GAMESPY_FN_VAR_DECL(GSIACResult, GSIAvailableCheckThink, ());
	GAMESPY_FN_VAR_DECL(void, msleep, (unsigned long msec));
public:
	GAMESPY_FN_VAR_DECL(int, GetQueryVersion, ());
};
