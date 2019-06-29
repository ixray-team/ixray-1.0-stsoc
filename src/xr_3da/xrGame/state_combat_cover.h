////////////////////////////////////////////////////////////////////////////
//	Module 		: state_combat_cover.h
//	Created 	: 24.01.2004
//  Modified 	: 24.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Combat state cover from enemy
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "state_base.h"

class CAI_Stalker;
class CCoverPoint;

class CStateCover : public CStateBase<CAI_Stalker> {
protected:
	typedef CStateBase<CAI_Stalker> inherited;
	xr_vector<CCoverPoint*>	m_nearest;

public:
							CStateCover			(LPCSTR state_name);
	virtual					~CStateCover		();
			void			Init				();
	virtual	void			Load				(LPCSTR section);
	virtual	void			reinit				(CAI_Stalker *object);
	virtual	void			reload				(LPCSTR section);
	virtual	void			initialize			();
	virtual	void			execute				();
	virtual	void			finalize			();
};

#include "state_combat_cover_inline.h"