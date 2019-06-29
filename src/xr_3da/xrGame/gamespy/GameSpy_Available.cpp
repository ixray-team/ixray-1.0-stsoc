#include "StdAfx.h"
#include "GameSpy_Available.h"
#include "GameSpy_Base_Defs.h"

CGameSpy_Available::CGameSpy_Available()
{
	hGameSpyDLL = NULL;

	LoadGameSpy();
};

CGameSpy_Available::~CGameSpy_Available()
{
	if (hGameSpyDLL)
	{
		FreeLibrary(hGameSpyDLL);
		hGameSpyDLL = NULL;
	}
};

void	CGameSpy_Available::LoadGameSpy()
{
	LPCSTR			g_name	= "xrGameSpy.dll";
	Log				("Loading DLL:",g_name);
	hGameSpyDLL			= LoadLibrary	(g_name);
	if (0==hGameSpyDLL)	R_CHK			(GetLastError());
	R_ASSERT2		(hGameSpyDLL,"GameSpy DLL raised exception during loading or there is no game DLL at all");

	GAMESPY_LOAD_FN(xrGS_GSIStartAvailableCheck);
	GAMESPY_LOAD_FN(xrGS_GSIAvailableCheckThink);
	GAMESPY_LOAD_FN(xrGS_msleep);
	GAMESPY_LOAD_FN(xrGS_GetQueryVersion);
}

bool	CGameSpy_Available::CheckAvailableServices		()
{
	GSIACResult result;
	xrGS_GSIStartAvailableCheck(GAMESPY_GAMENAME);

	while((result = xrGS_GSIAvailableCheckThink()) == GSIACWaiting)
		xrGS_msleep(5);

	if(result != GSIACAvailable)
	{
		Msg("! The GameSpy backend is not available");
		return false;
	}
	else
	{
		Msg("- The GameSpy backend is available");
	};
	return true;
};