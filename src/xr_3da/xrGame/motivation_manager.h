////////////////////////////////////////////////////////////////////////////
//	Module 		: motivation_manager.h
//	Created 	: 22.03.2004
//  Modified 	: 22.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Motivation manager class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graph_abstract.h"
#include "motivation.h"
#include "motivation_action.h"
#include "script_export_space.h"
#include "associative_vector.h"

class CScriptGameObject;

template <
	typename _object_type,
	template <typename _object_type> class _motivation_type = CMotivation,
	template <typename _object_type> class _motivation_action_type = CMotivationAction
>
class CMotivationManager {
public:
	typedef _motivation_type<_object_type>				CSMotivation;
	typedef _motivation_action_type<_object_type>		CSMotivationAction;
	typedef associative_vector<u32,CSMotivationAction*>	ACTIONS;

public:
	typedef CGraphAbstract<
				CSMotivation*,
				float,
				u32
			>										CSGraphAbstract;

protected:
	struct CMotivationWeight {
		u32							m_motivation_id;
		float						m_motivation_weight;

		IC				CMotivationWeight	(u32 motivation_id, float motivation_weight) :
							m_motivation_id		(motivation_id),
							m_motivation_weight	(motivation_weight)
		{
		}

		IC		bool	operator==			(u32 motivation_id) const
		{
			return					(m_motivation_id == motivation_id);
		}
	};
protected:
	CSGraphAbstract					*m_graph;
	ACTIONS							m_motivation_actions;
	xr_vector<u32>					m_motivations;
	xr_vector<CMotivationWeight>	m_actions;
	u32								m_selected_id;
	bool							m_actuality;
	xr_set<u32>						m_temp;
	xr_vector<float>				m_edges;
	_object_type					*m_object;

protected:
	IC		CSGraphAbstract			&graph				();
	IC		void					update_motivations	();
	IC		void					select_action		();
	IC		void					propagate			(u32 motivation_id, float weight);
	IC		void					add_motivation_action(u32 motivation_id, CSMotivation *motivation);

public:
	IC								CMotivationManager	();
	virtual							~CMotivationManager	();
	IC		void					init				();
	virtual void					update				();
	virtual void					clear				();
	virtual void					setup				(_object_type *object);

public:
	IC		void					add_motivation		(u32 motivation_id, CSMotivation *motivation);
	IC		void					remove_motivation	(u32 motivation_id);
	IC		void					add_connection		(u32 motivation_id, u32 sub_motivation_id);
	IC		void					remove_connection	(u32 motivation_id, u32 sub_motivation_id);
	IC		CSMotivation			*motivation			(u32 motivation_id) const;
	IC		CSMotivationAction		*selected			() const;
	IC		u32						selected_id			() const;
	IC		const CSGraphAbstract	&graph				() const;
	IC		bool					actual				() const;
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
typedef CMotivationManager<CScriptGameObject> CScriptMotivationManager;
add_to_type_list(CScriptMotivationManager)
#undef script_type_list
#define script_type_list save_type_list(CScriptMotivationManager)

#include "motivation_manager_inline.h"
