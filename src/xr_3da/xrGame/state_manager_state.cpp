////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_state.cpp
//	Created 	: 14.01.2004
//  Modified 	: 14.01.2004
//	Author		: Dmitriy Iassenev
//	Description : State manager state
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "state_manager_state.h"

CStateManagerState::CStateManagerState	()
{
	Init					();
}

CStateManagerState::~CStateManagerState	()
{
}

void CStateManagerState::Init			()
{
}

void CStateManagerState::Load			(LPCSTR section)
{
	CStateBase::Load				(section);
	CSStateManagerAbstract::Load	(section);
	for (u32 i=0, n=graph().vertices().size(); i<n; ++i)
		graph().vertices()[i].data().m_state->Load(section);
}

void CStateManagerState::reinit			(CAI_Stalker *object, u32 state_id)
{
	CStateBase::reinit				(object);
	CSStateManagerAbstract::reinit	(state_id);
	for (u32 i=0, n=graph().vertices().size(); i<n; ++i)
		graph().vertices()[i].data().m_state->reinit(object);
}

void CStateManagerState::reload			(LPCSTR section)
{
	CStateBase::reload				(section);
	CSStateManagerAbstract::reload	(section);
	for (u32 i=0, n=graph().vertices().size(); i<n; ++i)
		graph().vertices()[i].data().m_state->reload(section);
}

void CStateManagerState::update			(u32 time_delta)
{
	execute							();
	CSStateManagerAbstract::update	(time_delta);
	CSStateManagerAbstract			*state_manager_abstract = dynamic_cast<CSStateManagerAbstract*>(&current_state());
	if (state_manager_abstract)
		state_manager_abstract->update(time_delta);
}

void CStateManagerState::initialize		()
{
	CStateBase::initialize			();
	current_state().initialize		();
}

void CStateManagerState::finalize		()
{
	CStateBase::finalize			();
	current_state().finalize		();
}

bool CStateManagerState::completed		()
{
	return							(current_state().completed());
}

void CStateManagerState::execute		()
{
	if (current_vertex_id() == dest_vertex_id()) {
		current_state().execute		();
		return;
	}

	if (path().size() < 2)
		return;

	for (;;) {
		if (!state(path().front()).completed()) {
			state(path().front()).finalize();
			return;
		}
		go_path			();
		state(path().front()).initialize();
		if (path().size() < 2) {
			go_path			();
			break;
		}
	}
	current_state().execute();
}

