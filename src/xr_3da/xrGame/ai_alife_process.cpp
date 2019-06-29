////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_process.cpp
//	Created 	: 14.01.2003
//  Modified 	: 14.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation processing
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"

using namespace ALife;

void CSE_ALifeSimulator::shedule_Update			(u32 dt)
{
	ISheduled::shedule_Update			(dt);

	Device.Statistic.TEST3.Begin		();
	if (!m_bLoaded) {
		Device.Statistic.TEST3.End		();
		return;
	}
	
	vfInitAI_ALifeMembers				();

	switch (m_tZoneState) {
		case eZoneStateSurge : {
			vfPerformSurge				();
			Save						(m_caSaveName);
			m_tLastSurgeTime			= tfGetGameTime();
			m_tNextSurgeTime			= m_tLastSurgeTime + 7*24*3600*1000; // a week in milliseconds
			m_tZoneState				= eZoneStateAfterSurge;
			break;
		}
		case eZoneStateAfterSurge : {
			if (tfGetGameTime() > m_tNextSurgeTime) {
				m_tZoneState			= eZoneStateSurge;
				D_OBJECT_P_PAIR_IT		B = m_tpCurrentLevel->begin();
				D_OBJECT_P_PAIR_IT		E = m_tpCurrentLevel->end();
				D_OBJECT_P_PAIR_IT		I;
				for (I = B ; I != E; ++I)
					vfFurlObjectOffline((*I).second);
				break;
			}
			
			CSE_ALifeGraphRegistry::update		();
			CSE_ALifeScheduleRegistry::update	();
			
			break;
		}
		default : NODEFAULT;
	}
	Device.Statistic.TEST3.End();
}