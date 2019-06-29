#pragma once

#include "../ai_monster_share.h"


struct _chimera_watch_shared : public CSharedResource {


};

enum EWatchEventType {
	WE_BoneTurnStart,
	WE_BoneTurnFinished,
	WE_BodyTurnStart,
	WE_BodyTurnFinished
};


class CChimeraWatchManager : public CSharedClass<_chimera_watch_shared> {
	typedef CSharedClass<_chimera_watch_shared> inherited_shared;



public:
					CChimeraWatchManager		();
	virtual			~CChimeraWatchManager		();

			void	Init						();
	virtual	void	Load						(LPCSTR section);
	virtual	void	reinit						();
	virtual	void	reload						(LPCSTR section);

	virtual void	load_shared					(LPCSTR section);

			void	update						();

	//////////////////////////////////////////////////////////////////////////
	// Event stuff			


};



