#pragma once
#include "state.h"

template<typename _Object>
class CStateMovement : public CState <_Object> {
	typedef CStateMovement<_Object> inherited;

	enum EPathState {
		ePathStateUndefined,	// состояние не определено
		ePathStateOutdated,		// в данном состоянии путь не был построен
		ePathStateNotReady,		// в данном состоянии путь уже был задан но не был построен
		ePathStateBuilt,		// в данном состоянии путь построен
	} m_path_state;
	
	bool		m_path_targeted;	// построенный путь ведёт к m_target 

	struct {
		Fvector	position;
		u32		node;
	} m_target;

public:
						CStateMovement			(_Object *obj, EStateType s_type = ST_Unknown, void *data = 0);
	virtual 			~CStateMovement			(); 

	virtual void		initialize				();
	virtual void		path_update				();
	
	// путь не может быть построен
	virtual	void		path_failed				() {}
	// достигли конца пути
	virtual void		path_end				() {}
	// выбрать цель
	virtual	void		select_target_point		() {}
			
protected:
			EPathState	path_state				() {return m_path_state;}
private:

			void		update_path_info		();

};
#include "state_movement_inline.h"


