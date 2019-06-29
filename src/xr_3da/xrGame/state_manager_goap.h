////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_goap.h
//	Created 	: 22.02.2004
//  Modified 	: 22.02.2004
//	Author		: Dmitriy Iassenev
//	Description : State manager GOAP (Goal Oriented Action Planning)
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graph_manager.h"
#include "state_management_config.h"

namespace StateManagerGOAP {
	template <
		typename T2,
		typename T3
	>
	struct CStateConditionAbstract {
		typedef CStateConditionAbstract<T2,T3> CStateCondition;
		T2				m_parameter;
		T3				m_value;

		IC				CStateConditionAbstract(const T2 &parameter, const T3 &value) :
							m_parameter(parameter),
							m_value(value)
		{
		}

		IC		bool	operator<(const CStateCondition &condition) const
		{
			return		(m_parameter < condition.m_parameter);
		}
	};

	template <
		typename T1,
		typename T2,
		typename T3
	>
	struct CStateAbstract {
		typedef CStateConditionAbstract<T2,T3> CStateCondition;
		T1							*m_state;
		u32							m_priority;
		xr_vector<CStateCondition>	m_pre_conditions;
		xr_vector<CStateCondition>	m_post_conditions;

				CStateAbstract(T1 *state, u32 priority, const xr_vector<CStateCondition> &pre_conditions, const xr_vector<CStateCondition> &post_conditions) : 
					m_state(state), 
					m_priority(priority),
					m_pre_conditions(pre_conditions),
					m_post_conditions(post_conditions)
		{
			std::sort				(m_pre_conditions.begin(),m_pre_conditions.end());
			std::sort				(m_post_conditions.begin(),m_post_conditions.end());
		}

		IC		void	destroy		()
		{
			xr_delete	(m_state);
		}

		IC		u32		priority	() const
		{
			return		(m_priority);
		}

		IC	const xr_vector<CStateCondition> &pre_conditions() const
		{
			return		(m_pre_conditions);
		}

		IC	const xr_vector<CStateCondition> &post_conditions() const
		{
			return		(m_post_conditions);
		}
	};
};

template <
	typename T1,
	typename T2,
	typename T3
>
class CStateManagerGOAP : 
	public CGraphManagerAbstract<
		StateManagerGOAP::CStateAbstract<T1,T2,T3>,
		float,
		u32,
		u32
	>
{
public:
	typedef StateManagerGOAP::CStateAbstract<T1,T2,T3>						CState;
	typedef StateManagerGOAP::CStateConditionAbstract<T2,T3>				CStateCondition;
	typedef CGraphManagerAbstract<CState,float,u32,u32>						inherited;
	typedef typename CGraphAbstract<CState,float,u32,u32>::vertex_iterator	state_iterator;
private:
			bool		condition_contradiction	(const CState &state0, const CState &state1);
			void		update_transitions		(CState &state);

protected:
	virtual	void		add_state				(T1			*state,				u32 state_id, u32 priority, const xr_vector<CStateCondition> &pre_conditions, const xr_vector<CStateCondition> &post_conditions);
	virtual	void		remove_state			(u32		state_id);
	virtual	void		remove					(u32		state_id);
	IC		T1			&current_state			();
	IC		T1			&dest_state				();
	IC		void		set_dest_state			(const u32	dest_state_id);
	IC		void		set_current_state		(const u32	current_state_id);
	IC		const CState &internal_state		(u32		state_id) const
	{
		return			(graph().vertex(state_id)->data());
	}

public:
						CStateManagerGOAP		();
	virtual				~CStateManagerGOAP		();
			void		Init					();
	virtual	void		Load					(LPCSTR		section);
	virtual	void		reinit					(bool		clear_all = false);
	virtual	void		reload					(LPCSTR		section);
	virtual	void		update					(u32		time_delta);
	IC		T1			&state					(const u32	state_id);
	IC		const u32	&current_state_id		() const;
	IC		const u32	&dest_state_id			() const;
	IC		const T1	&current_state			() const;
	IC		const T1	&dest_state				() const;
};

#include "state_manager_goap_inline.h"