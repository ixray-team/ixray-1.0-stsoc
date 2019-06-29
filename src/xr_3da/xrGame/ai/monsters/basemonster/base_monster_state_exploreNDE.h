#pragma once 


class CBaseMonsterExploreNDE : public IState {
	typedef IState inherited;
	CBaseMonster		*pMonster;
	SoundElem		m_tSound;

	enum {
		ACTION_LOOK_DESTINATION,
		ACTION_GOTO_SOUND_SOURCE,
		ACTION_LOOK_AROUND
	} m_tAction;


	u32 flags;

	Fvector target_pos;

public:

	CBaseMonsterExploreNDE	(CBaseMonster *p);

private:
	virtual void	Init				();
	virtual void	Run					();
};
