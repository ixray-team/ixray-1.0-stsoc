#pragma once

#include "GameSpy_FuncDefs.h"

class CGameSpy_GCD_Client
{
	private:
		HMODULE	hGameSpyDLL;

		void	LoadGameSpy();
	public:
		CGameSpy_GCD_Client();
		~CGameSpy_GCD_Client();

		void CreateRespond	(char* RespondStr, char* ChallengeStr);
private:
	//--------------------- GCD_Client -------------------------------------------	
	GAMESPY_FN_VAR_DECL(void, gcd_compute_response_newauth, (char *cdkey, char *challenge,char* response));
	GAMESPY_FN_VAR_DECL(void, gcd_compute_response_reauth, (char *cdkey, char *challenge,char* response));
};
