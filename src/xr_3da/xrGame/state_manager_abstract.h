////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_abstract.h
//	Created 	: 12.01.2004
//  Modified 	: 12.01.2004
//	Author		: Dmitriy Iassenev
//	Description : State manager abstract
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graph_manager.h"
#include "state_management_config.h"

namespace StateManagerAbstract {
	template <typename T>
	struct CStateAbstract {
		T				*m_state;
		u32				m_priority;
				CStateAbstract(T *state, u32 priority) : 
					m_state(state), 
					m_priority(priority)
		{
		}

		IC	void	destroy()
		{
			xr_delete	(m_state);
		}

		IC	u32		priority() const
		{
			return		(m_priority);
		}
	};
};

template <typename T>
class CStateManagerAbstract : 
	public CGraphManagerAbstract<
		StateManagerAbstract::CStateAbstract<T>,
		float,
		u32,
		u32
	>
{
public:
	typedef StateManagerAbstract::CStateAbstract<T>							CState;
	typedef CGraphManagerAbstract<CState,float,u32,u32>						inherited;
	typedef typename CGraphAbstract<CState,float,u32,u32>::vertex_iterator	state_iterator;
private:

protected:
	virtual	void		add_state				(T *state, u32 state_id, u32 priority);
	virtual	void		remove_state			(u32 state_id);
	IC		void		add_transition			(u32 state_id0, u32 state_id1, float weight);
	IC		void		add_transition			(u32 state_id0, u32 state_id1, float weight0, float weight1);
	virtual	void		remove					(u32 state_id);
	IC		T			&current_state			();
	IC		T			&dest_state				();
	IC		void		set_dest_state			(const u32 dest_state_id);
	IC		void		set_current_state		(const u32 current_state_id);
	IC		const CState &internal_state		(u32 state_id) const;

public:
						CStateManagerAbstract	();
	virtual				~CStateManagerAbstract	();
			void		init					();
	virtual	void		Load					(LPCSTR section);
	virtual	void		reinit					(bool clear_all = false);
	virtual	void		reload					(LPCSTR section);
	virtual	void		update					();
	IC		T			&state					(const u32 state_id);
	IC		const u32	&current_state_id		() const;
	IC		const u32	&dest_state_id			() const;
	IC		const T		&current_state			() const;
	IC		const T		&dest_state				() const;
};

#include "state_manager_abstract_inline.h"