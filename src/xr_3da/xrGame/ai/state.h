#pragma once

#include "state_defs.h"


template<typename _Object>
class CState {
	typedef CState<_Object> CSState;
public:
						CState					(_Object *obj, EStateType s_type = ST_Unknown, void *data = 0);
	virtual 			~CState					(); 

	virtual void		initialize				();
	virtual void 		execute					();
	virtual void 		finalize				();
	virtual void 		critical_finalize		();
	
	virtual void 		reset					();

	virtual bool 		check_completion		() {return false;}
	virtual bool 		check_start_conditions	() {return true;}

	virtual void		reselect_state			() {}	
	virtual void		check_force_state		() {}

			CSState 	*get_state				(u32 state_id);
			CSState 	*get_state_current		();

			void		fill_data_with			(void *ptr_src, u32 size);

			EStateType	get_state_type			();
			void		set_state_type			(EStateType s_type) {state_type = s_type;}

protected:
			void 		select_state			(u32 new_state_id);	
			void		add_state				(u32 state_id, CSState *s);

	virtual void		setup_substates			(){}
			
	u32					current_substate;
	u32					prev_substate;

	u32					time_state_started;

	_Object				*object;
	
	void				*_data;

private:
			void		free_mem				();

	xr_map<u32, CSState*> substates;	
	typedef typename xr_map<u32, CSState*>::iterator STATE_MAP_IT;		

	EStateType			state_type;
			
};

#include "state_inline.h"

