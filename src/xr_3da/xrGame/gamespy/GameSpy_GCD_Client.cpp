#include "StdAfx.h"
#include "GameSpy_GCD_Client.h"
#include "GameSpy_Base_Defs.h"

#define	REGISTRY_CDKEY_STR	"Software\\S.T.A.L.K.E.R\\CDKey"

CGameSpy_GCD_Client::CGameSpy_GCD_Client()
{
	hGameSpyDLL = NULL;

	LoadGameSpy();
};

CGameSpy_GCD_Client::~CGameSpy_GCD_Client()
{
	if (hGameSpyDLL)
	{
		FreeLibrary(hGameSpyDLL);
		hGameSpyDLL = NULL;
	}
};
void	CGameSpy_GCD_Client::LoadGameSpy()
{
	LPCSTR			g_name	= "xrGameSpy.dll";
	Log				("Loading DLL:",g_name);
	hGameSpyDLL			= LoadLibrary	(g_name);
	if (0==hGameSpyDLL)	R_CHK			(GetLastError());
	R_ASSERT2		(hGameSpyDLL,"GameSpy DLL raised exception during loading or there is no game DLL at all");

	GAMESPY_LOAD_FN(xrGS_gcd_compute_response_newauth);
	GAMESPY_LOAD_FN(xrGS_gcd_compute_response_reauth);
}

string64	gsCDKey = "xxxx-xxxx-xxxx-xxxx";
void CGameSpy_GCD_Client::CreateRespond	(char* RespondStr, char* ChallengeStr)
{
/*
	//--------------- Get CDKey ------------------------------------------
	string128 CDKeyStr = "aaaa-ssss-dddd-ffff";

	HKEY KeyCDKey = 0;
	long res = RegOpenKeyEx(HKEY_CURRENT_USER, 
		REGISTRY_CDKEY_STR, 0, KEY_READ, &KeyCDKey);
	if (res == ERROR_SUCCESS && KeyCDKey != 0)
	{
		DWORD KeyValueSize = 128;
		DWORD KeyValueType = REG_SZ;
		res = RegQueryValueEx(KeyCDKey, "value", NULL, &KeyValueType, (LPBYTE)CDKeyStr, &KeyValueSize);
	};
	switch (res)
	{
	case ERROR_PATH_NOT_FOUND:
	case ERROR_FILE_NOT_FOUND:
		{
			Msg("No CD-key found");
			//			return;
		}break;
	};
	//--------------- Create Respond -------------------------------------
*/
//	string128 ResponseStr;	
	xrGS_gcd_compute_response_newauth(gsCDKey, ChallengeStr, RespondStr);
}