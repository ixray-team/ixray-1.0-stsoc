#pragma once

enum EStateAnimTriple {
	eStatePrepare,
	eStateExecute,
	eStateFinalize,
	eStateNone
};

class CAnimTriple {
	EStateAnimTriple	cur_state;

	typedef CMotionDef* MOTION_PTR;
	
	MOTION_PTR			pool[3];

	bool				m_active;

public:
						CAnimTriple			();
	virtual 			~CAnimTriple		();

	virtual void		init_external		(CMotionDef *m_def1, CMotionDef *m_def2, CMotionDef *m_def3);
	
	virtual void		activate			();
	virtual void		deactivate			();

	virtual bool		is_active			() {return m_active;}

	// завершилась анимация, необходимо выбрать новую
	virtual bool		prepare_animation	(CMotionDef **m);

	// завершить фиксированную анимацию (вторую) и перейти к финальной
	virtual void		pointbreak			();

};

