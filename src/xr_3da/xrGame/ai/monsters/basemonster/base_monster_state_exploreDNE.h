#pragma once 

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBaseMonsterExploreDNE class	// Explore danger-non-expedient enemy
///////////////////////////////////////////////////////////////////////////////////////////////////////////
class CBaseMonsterExploreDNE : public IState {
	typedef	IState inherited;
	CBaseMonster	*pMonster;

	enum {
		ACTION_RUN_AWAY,
		ACTION_LOOK_BACK_POSITION,
		ACTION_LOOK_AROUND
	} m_tAction;

	SoundElem		m_tSound;
	
	bool			flag_once_1;

	Fvector 		SavedPosition;
	Fvector 		StartPosition;
	Fvector 		LastPosition;

	float			m_fRunAwayDist;

	TTime			m_dwLastPosSavedTime;
	TTime			m_dwStayLastTimeCheck;

public:
					CBaseMonsterExploreDNE	(CBaseMonster *p);

	virtual bool	GetStateAggressiveness	(){return true;}
private:
	virtual	void	Init				();
	virtual	void	Run					();
};


