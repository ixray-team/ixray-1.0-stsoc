////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_state.h
//	Created 	: 14.01.2004
//  Modified 	: 14.01.2004
//	Author		: Dmitriy Iassenev
//	Description : State manager state
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "state_manager_abstract.h"
#include "state_base.h"

template <
	typename _Object
>
class CStateManagerState : 
	public CStateManagerAbstract<
		CStateBase<_Object>
	>,
	public CStateBase<_Object>
{
protected:
	typedef CStateBase<_Object> CSStateBase;
	typedef CStateManagerAbstract<CStateBase<_Object> > CSStateManagerAbstract;

protected:
	bool	m_initialized;

public:
						CStateManagerState		(LPCSTR state_name);
	virtual				~CStateManagerState		();
			void		init					();
	virtual	void		Load					(LPCSTR section);
	virtual	void		reinit					(_Object *object, bool clear_all = false);
	virtual	void		reload					(LPCSTR section);
	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();
	virtual	void		update					();
	virtual	bool		completed				() const;
	IC		void		add_state				(CSStateBase *state, u32 state_id, u32 priority);
};

#include "state_manager_state_inline.h"