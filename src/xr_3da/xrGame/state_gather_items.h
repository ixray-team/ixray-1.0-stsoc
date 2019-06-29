////////////////////////////////////////////////////////////////////////////
//	Module 		: state_gather_items.h
//	Created 	: 28.01.2004
//  Modified 	: 28.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Gathering items state
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "state_base.h"

class CAI_Stalker;

class CStateGatherItems : public CStateBase<CAI_Stalker> {
protected:
	typedef CStateBase<CAI_Stalker> inherited;

private:
	u32					m_priority;

public:
						CStateGatherItems	(LPCSTR state_name);
	virtual				~CStateGatherItems	();
			void		Init				();
	virtual	void		Load				(LPCSTR section);
	virtual	void		reinit				(CAI_Stalker *object);
	virtual	void		reload				(LPCSTR section);
	virtual	void		initialize			();
	virtual	void		execute				();
	virtual	void		finalize			();
};

#include "state_free_no_alife_inline.h"