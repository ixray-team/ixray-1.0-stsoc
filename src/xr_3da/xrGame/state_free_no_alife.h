////////////////////////////////////////////////////////////////////////////
//	Module 		: state_free_no_alife.h
//	Created 	: 13.01.2004
//  Modified 	: 13.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Free state when no simulation started (for test purposes only!)
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "state_base.h"

class CAI_Stalker;

class CStateFreeNoAlife : public CStateBase<CAI_Stalker> {
protected:
	typedef CStateBase<CAI_Stalker> inherited;

public:
						CStateFreeNoAlife	(LPCSTR state_name);
	virtual				~CStateFreeNoAlife	();
			void		Init				();
	virtual	void		Load				(LPCSTR section);
	virtual	void		reinit				(CAI_Stalker *object);
	virtual	void		reload				(LPCSTR section);
	virtual	void		initialize			();
	virtual	void		execute				();
	virtual	void		finalize			();
};

#include "state_free_no_alife_inline.h"