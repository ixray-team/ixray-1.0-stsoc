#include "StdAfx.h"
#include "GameSpy_GCD_Server.h"
#include "GameSpy_FuncDefs.h"
#include "GameSpy_Base_Defs.h"
#include "../xrGameSpyServer.h"

CGameSpy_GCD_Server::CGameSpy_GCD_Server()
{
	hGameSpyDLL = NULL;

	LoadGameSpy();
};

CGameSpy_GCD_Server::~CGameSpy_GCD_Server()
{
	if (hGameSpyDLL)
	{
		FreeLibrary(hGameSpyDLL);
		hGameSpyDLL = NULL;
	}
};

void	CGameSpy_GCD_Server::LoadGameSpy()
{
	LPCSTR			g_name	= "xrGameSpy.dll";
	Log				("Loading DLL:",g_name);
	hGameSpyDLL			= LoadLibrary	(g_name);
	if (0==hGameSpyDLL)	R_CHK			(GetLastError());
	R_ASSERT2		(hGameSpyDLL,"GameSpy DLL raised exception during loading or there is no game DLL at all");

	GAMESPY_LOAD_FN(xrGS_gcd_init_qr2);
	GAMESPY_LOAD_FN(xrGS_gcd_shutdown);
	GAMESPY_LOAD_FN(xrGS_gcd_authenticate_user);
	GAMESPY_LOAD_FN(xrGS_gcd_disconnect_user);
	GAMESPY_LOAD_FN(xrGS_gcd_think);
}

bool	CGameSpy_GCD_Server::Init()
{
	int res = xrGS_gcd_init_qr2(NULL);
	if (res == -1)
	{
		Msg("! GameSpy::CDKey : Failes to Initialize!");
		return false;
	};
	Msg("- GameSpy::CDKey : Initialized");
	return true;
};

void	CGameSpy_GCD_Server::ShutDown()
{
	xrGS_gcd_shutdown();
}

void	CGameSpy_GCD_Server::CreateRandomChallenge(char* challenge, int nchars)
{
	if (nchars > GAMESPY_MAXCHALLANGESIZE) nchars = GAMESPY_MAXCHALLANGESIZE;
	challenge[nchars] = 0;
	while (nchars--)
	{
		challenge[nchars] = char('a' + ::Random.randI(26));
	};
}

//--------------------------- CD Key callbacks -----------------------------------
void __cdecl ClientAuthorizeCallback(int productid, int localid, int authenticated, char *errmsg, void *instance)
{
	xrGameSpyServer* pServer = (xrGameSpyServer*) (instance);
	if (pServer) pServer->OnCDKey_Validation(localid, authenticated, errmsg);
};

void __cdecl ClientReAuthorizeCallback(int gameid, int localid, int hint, char *challenge, void *instance)
{
//	xrGameSpyServer* pServer = (xrGameSpyServer*) (instance);
//	if (pServer) pServer->OnCDKey_Validation(localid, authenticated, errmsg);
//	int x=0;
//	x=x;
};

void	CGameSpy_GCD_Server::AuthUser(int localid, unsigned int userip, char *challenge, char *response, 
									  void *instance)
{
	xrGS_gcd_authenticate_user(localid, userip, challenge, response, ClientAuthorizeCallback, ClientReAuthorizeCallback, instance);
};

void	CGameSpy_GCD_Server::DisconnectUser(int localid)
{
	xrGS_gcd_disconnect_user(localid);
};

void	CGameSpy_GCD_Server::Think()
{
	xrGS_gcd_think();
};