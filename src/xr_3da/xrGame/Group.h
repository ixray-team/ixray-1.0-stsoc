////////////////////////////////////////////////////////////////////////////
//	Module 		: group.h
//	Created 	: 25.05.2004
//  Modified 	: 25.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Group class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "fixedset.h"

#define MAX_SUSPICIOUS_NODE_COUNT 32
#define MAX_GROUP_SIZE 256

struct SSearchPlace {
	u32		dwNodeID:24;
	u32		dwSearched:2;
	u32		dwGroup:6;
	float	m_cost;
};

typedef xr_vector<Fvector>		MemberPlacement;
typedef xr_vector<u32>			MemberNodes;
typedef xr_vector<SSearchPlace>	SuspiciousNodes;
typedef xr_vector<bool>			SuspiciousGroups;

class CEntity;
class CAgentManager;

DEFINE_VECTOR(CEntity*,EntityVec,EntityIt);

class CGroup {
private:
	CAgentManager			*m_agent_manager;

public:
	u32						Flags;
	Fvector					vTargetDirection;
	Fvector					vTargetPosition;
	EntityVec				Members;
	Fvector					vCentroid;
	bool					m_bLeaderViewsEnemy;
	bool					m_bEnemyNoticed;
	u32						m_dwLeaderChangeCount;
	Fvector					m_tLastHitDirection;
	Fvector					m_tHitPosition;
	u32						m_dwLastHitTime;
	u32						m_dwFiring;
	bool					m_bLessCoverLook;
	u32						m_dwLastViewChange;
	SuspiciousNodes			m_tpaSuspiciousNodes;
	SuspiciousGroups		m_tpaSuspiciousGroups;
	xr_vector<u32>			m_tpaPatrolPath;
	u32						m_dwLastActionTime;
	u32						m_dwLastAction;
	u32						m_dwActiveCount;
	u32						m_dwAliveCount;
	u32						m_dwStandingCount;

public:
							CGroup						();
							CGroup						(const CGroup &group);
							~CGroup						();
			void			Member_Add					(CEntity* E);
			void			Member_Remove				(CEntity* E);
			bool			Empty						() {return Members.empty();}
			int				Size						() {return (int)Members.size();}
			void			GetAliveMemberPlacement		(MemberPlacement& P, CEntity* Me);
			void			GetAliveMemberDedication	(MemberPlacement& P, CEntity* Me);
			void			GetAliveMemberPlacementN	(MemberNodes& P, CEntity* Me);
			void			GetAliveMemberDedicationN	(MemberNodes& P, CEntity* Me);
			void			GetAliveMemberInfo			(MemberPlacement& P0, MemberNodes& P1, MemberPlacement& P2, MemberNodes& P3, CEntity* Me);
			const Fvector	&GetCentroid				();
	IC		CAgentManager	&agent_manager				() const;
	IC		CAgentManager	*get_agent_manager			() const;
};

#include "group_inline.h"