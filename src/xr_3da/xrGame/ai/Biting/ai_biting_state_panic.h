#pragma once 

class CBitingPanic : public IState {
	CAI_Biting		*pMonster;

	typedef IState inherited;

	enum {
		ACTION_RUN, 
		ACTION_FACE_BACK_SCARED,
		ACTION_ATTACK_MELEE
	} m_tAction, m_tPrevAction;

	Fvector			position;
	
	Fvector			target_pos;
	u32				target_vertex_id;

	TTime			last_time_cover_selected;
	TTime			m_dwFaceEnemyLastTime;

public:
					CBitingPanic			(CAI_Biting *p);
	
	virtual bool	CheckCompletion			();
	virtual bool	GetStateAggressiveness	(){return true;}

private:

	virtual void	Init					();
	virtual void	Run						();
};
