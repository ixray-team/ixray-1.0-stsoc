	////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_goap_inline.h
//	Created 	: 22.02.2004
//  Modified 	: 22.02.2004
//	Author		: Dmitriy Iassenev
//	Description : State manager GOAP inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename T1,\
	typename T2,\
	typename T3\
>

#define CGOAPStateManager CStateManagerGOAP<T1,T2,T3>

TEMPLATE_SPECIALIZATION
CGOAPStateManager::CStateManagerGOAP			()
{
	Init					();
}

TEMPLATE_SPECIALIZATION
CGOAPStateManager::~CStateManagerGOAP			()
{
}

TEMPLATE_SPECIALIZATION
void CGOAPStateManager::Init			()
{
	m_conditions.reserve	(64);
}

TEMPLATE_SPECIALIZATION
void CGOAPStateManager::Load			(LPCSTR section)
{
}

TEMPLATE_SPECIALIZATION
void CGOAPStateManager::reinit			(bool clear_all)
{
	inherited::reinit		(clear_all);
}

TEMPLATE_SPECIALIZATION
void CGOAPStateManager::reload			(LPCSTR section)
{
}

TEMPLATE_SPECIALIZATION
bool CGOAPStateManager::condition_contradiction(const CState &state0, const CState &state1)
{
	const xr_vector<CStateCondition>::const_iterator I = state0.pre_conditions().begin();
	const xr_vector<CStateCondition>::const_iterator E = state0.pre_conditions().end();
	const xr_vector<CStateCondition>::const_iterator i = state0.post_conditions().begin();
	const xr_vector<CStateCondition>::const_iterator e = state0.post_conditions().end();
	const xr_vector<CStateCondition>::const_iterator II = state1.pre_conditions().begin();
	const xr_vector<CStateCondition>::const_iterator EE = state1.pre_conditions().end();

	for ( ; (I != E) && (i != e) && (II != EE); ) {
		if ((*i).m_parameter > (*II).m_parameter) {
			if ((*I).m_parameter > (*II).m_parameter) {
				++II;
				continue;
			}
			else {
				if ((*I).m_parameter < (*II).m_parameter) {
					++I;
					continue;
				}
				else {
					if ((*I).m_value != (*II).m_value)
						return			(true);
					++I;
					++II;
				}
			}
		}
		else
			if ((*i).m_parameter < (*II).m_parameter) {
				++i;
				if ((*I).m_parameter < (*II).m_parameter) {
					++I;
					continue;
				}
			}
			else {
				if ((*i).m_value != (*II).m_value)
					return				(true);
				++i;
				++II;
				if ((*I).m_parameter < (*II).m_parameter)
					++I;
			}
	}

	if (II == EE)
		return							(false);

	if (I == E) {
		I = i;
		E = e;
	}

	for ( ; (I != E) && (II != EE); )
		if ((*I).m_parameter < (*II).m_parameter)
			++I;
		else
			if ((*I).m_parameter > (*II).m_parameter)
				++II;
			else {
				if ((*I).m_value != (*II).m_value)
					return				(true);
				++I;
				++II;
			}
	return								(false);
}

TEMPLATE_SPECIALIZATION
void CGOAPStateManager::update_transitions(CState &state)
{
	xr_vector<CVertex>::const_iterator	I = graph().vertices().begin();
	xr_vector<CVertex>::const_iterator	E = graph().vertices().end();
	for ( ; I != E; ++I) {
		// we do not want to have vertex loops
		if (&(*I).data().m_state == &state)
			continue;
		if (!condition_contradiction((*I).data().m_state,state))
			add_edge		((*I).data().m_state,state,1);
		if (!condition_contradiction(state,(*I).data().m_state))
			add_edge		(state,(*I).data().m_state,1);
	}
}

TEMPLATE_SPECIALIZATION
void CGOAPStateManager::add_state		(T1 *state, u32 state_id, u32 priority, const xr_vector<CStateCondition> &pre_conditions, const xr_vector<CStateCondition> &post_conditions)
{
	VERIFY					(!graph().vertex(state_id));
	CState					state = CState(state,priority,pre_conditions,post_conditions);
	graph().add_vertex		(state,state_id);
	VERIFY					(graph().vertex(state_id));
	update_transitions		(state);
}

TEMPLATE_SPECIALIZATION
void CGOAPStateManager::remove			(u32 state_id)
{
	VERIFY					(graph().vertex(state_id));
	graph().vertex(state_id)->data().destroy();
	graph().vertex(state_id)->destroy();
	graph().remove_vertex	(state_id);
	VERIFY					(!graph().vertex(state_id));
}

TEMPLATE_SPECIALIZATION
void CGOAPStateManager::update					(u32 time_delta)
{
	inherited::update		(time_delta);
}

TEMPLATE_SPECIALIZATION
IC	T1	&CGOAPStateManager::state		(const u32 state_id)
{
	VERIFY					(graph().vertex(state_id));
	VERIFY					(graph().vertex(state_id)->data().m_state);
	return					(*graph().vertex(state_id)->data().m_state);
}

TEMPLATE_SPECIALIZATION
IC	T1	&CGOAPStateManager::current_state	()
{
	VERIFY					(graph().vertex(current_vertex_id()));
	return					(*graph().vertex(current_vertex_id())->data().m_state);
}

TEMPLATE_SPECIALIZATION
IC	const T1	&CGOAPStateManager::current_state	() const
{
	VERIFY					(graph().vertex(current_vertex_id()));
	return					(*graph().vertex(current_vertex_id())->data().m_state);
}

TEMPLATE_SPECIALIZATION
IC	T1	&CGOAPStateManager::dest_state	()
{
	VERIFY					(graph().vertex(dest_vertex_id()));
	return					(*graph().vertex(dest_vertex_id())->data().m_state);
}

TEMPLATE_SPECIALIZATION
IC	const T1	&CGOAPStateManager::dest_state	() const
{
	VERIFY					(graph().vertex(dest_vertex_id()));
	return					(*graph().vertex(dest_vertex_id())->data().m_state);
}

TEMPLATE_SPECIALIZATION
IC	void CGOAPStateManager::set_dest_state			(const u32 dest_state_id)
{
	set_dest_vertex_id		(dest_state_id);
}

TEMPLATE_SPECIALIZATION
IC	void CGOAPStateManager::set_current_state		(const u32 current_state_id)
{
	if (current_state_id != current_vertex_id()) {
		set_current_vertex_id	(current_state_id);
		current_state().initialize();
	}
}

TEMPLATE_SPECIALIZATION
void CGOAPStateManager::remove_state	(u32 state_id)
{
	graph().remove_vertex	(state_id);
}

TEMPLATE_SPECIALIZATION
IC	const u32 &CGOAPStateManager::current_state_id	() const
{
	return					(current_vertex_id());
}

TEMPLATE_SPECIALIZATION
IC	const u32 &CGOAPStateManager::dest_state_id		() const
{
	return					(dest_vertex_id());
}

#undef TEMPLATE_SPECIALIZATION
#undef CAbstractObjectManager
