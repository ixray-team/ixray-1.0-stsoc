////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_manager.h
//	Created 	: 24.05.2004
//  Modified 	: 24.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Agent manager
////////////////////////////////////////////////////////////////////////////

#pragma once

class CAgentCorpseManager;
class CAgentEnemyManager;
class CAgentExplosiveManager;
class CAgentLocationManager;
class CAgentMemberManager;
class CAgentMemoryManager;
class CAgentManagerPlanner;

class CAgentManager : public ISheduled {
private:
	CAgentCorpseManager		*m_corpse;
	CAgentEnemyManager		*m_enemy;
	CAgentExplosiveManager	*m_explosive;
	CAgentLocationManager	*m_location;
	CAgentMemberManager		*m_member;
	CAgentMemoryManager		*m_memory;
	CAgentManagerPlanner	*m_brain;

protected:
			void					init_scheduler		();
			void					init_components		();
			void					remove_scheduler	();
			void					remove_components	();

public:
									CAgentManager		();
	virtual							~CAgentManager		();
	virtual bool					shedule_Needed		()					{return true;};
	virtual float					shedule_Scale		();
	virtual void					shedule_Update		(u32 time_delta);	
	virtual	shared_str				shedule_Name		() const			{return shared_str("agent_manager"); };
	virtual	void					remove_links		(CObject *object);
	virtual	shared_str				cName				() const;

public:
	IC		CAgentCorpseManager		&corpse				() const;
	IC		CAgentEnemyManager		&enemy				() const;
	IC		CAgentExplosiveManager	&explosive			() const;
	IC		CAgentLocationManager	&location			() const;
	IC		CAgentMemberManager		&member				() const;
	IC		CAgentMemoryManager		&memory				() const;
	IC		CAgentManagerPlanner	&brain				() const;
};

#include "agent_manager_inline.h"