#pragma once

#include "../ai_monster_share.h"


struct _chimera_animation_shared : public CSharedResource {


};

enum EAnimationEventType {
	AE_AnimationStart,
	AE_AnimationFinished,
	AE_AnimationStackEmpty,
	AE_CriticalAnimStart,
	AE_CriticalAnimFinished,
};


class CChimeraAnimationManager : public CSharedClass<_chimera_animation_shared> {
	
	typedef CSharedClass<_chimera_animation_shared> inherited_shared;

public:
					CChimeraAnimationManager	();
	virtual			~CChimeraAnimationManager	();

			void	Init						();
	virtual	void	Load						(LPCSTR section);
	virtual	void	reinit						();
	virtual	void	reload						(LPCSTR section);

	virtual void	load_shared					(LPCSTR section);

			void	update						();

	//////////////////////////////////////////////////////////////////////////
	// Event stuff			


};