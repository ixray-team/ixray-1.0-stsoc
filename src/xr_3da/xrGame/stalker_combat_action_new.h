////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_combat_actions_new.h
//	Created 	: 25.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker combat action classes new (!?)
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stalker_combat_actions.h"

//////////////////////////////////////////////////////////////////////////
// CStalkerActionRetreatFromEnemyNew
//////////////////////////////////////////////////////////////////////////

class CStalkerActionRetreatFromEnemyNew : public CStalkerActionCombatBase {
protected:
	typedef CStalkerActionCombatBase inherited;

public:
	CStalkerActionRetreatFromEnemyNew		(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void				initialize	();
	virtual void				execute		();
	virtual void				finalize	();
	virtual _edge_value_type	weight		(const CSConditionState &condition0, const CSConditionState &condition1) const;
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionTakeCoverNew
//////////////////////////////////////////////////////////////////////////

class CStalkerActionTakeCoverNew : public CStalkerActionCombatBase {
protected:
	typedef CStalkerActionCombatBase inherited;

private:
	MonsterSpace::EBodyState		m_body_state;
	MonsterSpace::EMovementType		m_movement_type;

public:
		CStalkerActionTakeCoverNew	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize	();
	virtual void		execute		();
	virtual void		finalize	();
};

//////////////////////////////////////////////////////////////////////////
// CStalkerActionGetReadyToKillNew
//////////////////////////////////////////////////////////////////////////

class CStalkerActionGetReadyToKillNew : public CStalkerActionCombatBase {
protected:
	typedef CStalkerActionCombatBase inherited;

private:
	MonsterSpace::EBodyState		m_body_state;
	MonsterSpace::EMovementType		m_movement_type;

public:
						CStalkerActionGetReadyToKillNew	(CAI_Stalker *object, LPCSTR action_name = "");
	virtual void		initialize						();
	virtual void		execute							();
	virtual void		finalize						();
};
