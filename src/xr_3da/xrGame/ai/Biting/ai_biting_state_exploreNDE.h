#pragma once 


class CBitingExploreNDE : public IState {
	typedef IState inherited;
	CAI_Biting		*pMonster;
	SoundElem		m_tSound;

	enum {
		ACTION_LOOK_DESTINATION,
		ACTION_GOTO_SOUND_SOURCE,
		ACTION_LOOK_AROUND
	} m_tAction;


	u32 flags;

	Fvector target_pos;

public:

	CBitingExploreNDE	(CAI_Biting *p);

private:
	virtual void	Init				();
	virtual void	Run					();
};
