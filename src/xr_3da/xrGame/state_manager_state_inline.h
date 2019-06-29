////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_state_inline.h
//	Created 	: 14.01.2004
//  Modified 	: 14.01.2004
//	Author		: Dmitriy Iassenev
//	Description : State manager state inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateManagerAbstract CStateManagerState<_Object>

TEMPLATE_SPECIALIZATION
CStateManagerAbstract::CStateManagerState	(LPCSTR state_name) : CSStateBase(state_name)
{
	init					();
}

TEMPLATE_SPECIALIZATION
CStateManagerAbstract::~CStateManagerState	()
{
}

TEMPLATE_SPECIALIZATION
void CStateManagerAbstract::init			()
{
}

TEMPLATE_SPECIALIZATION
void CStateManagerAbstract::Load			(LPCSTR section)
{
	CSStateBase::Load				(section);
	CSStateManagerAbstract::Load	(section);
	for (u32 i=0, n=graph().vertices().size(); i<n; ++i)
		graph().vertices()[i].data().m_state->Load(section);
}

TEMPLATE_SPECIALIZATION
void CStateManagerAbstract::reinit			(_Object *object, bool clear_all)
{
	CSStateBase::reinit				(object);
	CSStateManagerAbstract::reinit	(clear_all);
	for (u32 i=0, n=graph().vertices().size(); i<n; ++i)
		graph().vertices()[i].data().m_state->reinit(object);
	m_initialized					= false;
}

TEMPLATE_SPECIALIZATION
void CStateManagerAbstract::reload			(LPCSTR section)
{
	CSStateBase::reload				(section);
	CSStateManagerAbstract::reload	(section);
	for (u32 i=0, n=graph().vertices().size(); i<n; ++i)
		graph().vertices()[i].data().m_state->reload(section);
}

TEMPLATE_SPECIALIZATION
void CStateManagerAbstract::update			()
{
	if (!m_initialized) {
		initialize					();
		m_initialized				= true;
	}

	execute							();
	CSStateManagerAbstract::update	();
	IGraphManager					*state_manager_interface = smart_cast<IGraphManager*>(&current_state());
	if (state_manager_interface)
		state_manager_interface->update();
}

TEMPLATE_SPECIALIZATION
void CStateManagerAbstract::initialize		()
{
	CSStateBase::initialize			();
	current_state().initialize		();
}

TEMPLATE_SPECIALIZATION
void CStateManagerAbstract::finalize		()
{
	CSStateBase::finalize			();
	current_state().finalize		();
}

TEMPLATE_SPECIALIZATION
bool CStateManagerAbstract::completed		() const
{
	return							(current_state().completed());
}

TEMPLATE_SPECIALIZATION
void CStateManagerAbstract::execute	()
{
	CSStateBase::execute			();
}

TEMPLATE_SPECIALIZATION
IC	void CStateManagerAbstract::add_state	(CSStateBase *state, u32 state_id, u32 priority)
{
	CSStateManagerAbstract::add_state(state,state_id,priority);
	if (m_object)
		state->reinit(m_object);
}

#undef TEMPLATE_SPECIALIZATION
#undef CStateManagerAbstract