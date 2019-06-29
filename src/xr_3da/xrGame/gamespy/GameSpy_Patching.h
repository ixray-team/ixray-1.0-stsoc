#pragma once

#include "GameSpy_FuncDefs.h"

class CGameSpy_Patching
{
private:
	HMODULE	hGameSpyDLL;

	void	LoadGameSpy();
public:
	CGameSpy_Patching();
	~CGameSpy_Patching();

	void CheckForPatch	();
private:
	//--------------------- GCD_Client -------------------------------------------	
	GAMESPY_FN_VAR_DECL(bool, ptCheckForPatch, (int productID, 
		const char * versionUniqueID, 
		int distributionID, 
		ptPatchCallback callback, 
		PTBool blocking, 
		void * instance ));
};