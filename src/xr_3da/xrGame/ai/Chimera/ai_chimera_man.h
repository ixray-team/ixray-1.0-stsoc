#pragma once

#include "../ai_monster_share.h"



struct _chimera_base_shared : public CSharedResource {

};


class CChimeraBaseManager : public CSharedClass<_chimera_base_shared>{
	typedef CSharedClass<_chimera_base_shared> inherited_shared;

public:
					CChimeraBaseManager		();
	virtual			~CChimeraBaseManager	();

			void	Init					();
	virtual	void	Load					(LPCSTR section);
	virtual	void	reinit					();
	virtual	void	reload					(LPCSTR section);

	virtual void	load_shared				(LPCSTR section);

			void	update					();
};

