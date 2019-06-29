////////////////////////////////////////////////////////////////////////////
//	Module 		: object_handler_goap_inline.h
//	Created 	: 11.03.2004
//  Modified 	: 11.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object handler using GOAP (inline functions)
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	bool CObjectHandler::firing		() const
{
	return					(m_bFiring);
}

IC	CObjectHandler::_condition_type CObjectHandler::uid(const u32 id0, const u32 id1) const
{
	VERIFY				(!((id0 << 16) & id1));
	return				((id0 << 16) | id1);
}

IC	bool CObjectHandler::object_action	(u32 action_id, CObject *object)
{
	return				((action_id >> 16) == object->ID());
}

IC	u32	CObjectHandler::current_action_object_id	() const
{
	return				(action_object_id(current_action_id()));
}

IC	u32	CObjectHandler::current_action_state_id	() const
{
	return				(action_state_id(current_action_id()));
}

IC	u32	CObjectHandler::action_object_id		(_condition_type action_id) const
{
	return				(action_id >> 16);
}

IC	u32	CObjectHandler::action_state_id			(_condition_type action_id) const
{
	return				(action_id & 0xffff);
}

IC	bool CObjectHandler::goal_reached			() const
{
	return				(solution().size() < 2);
}

IC	void CObjectHandler::add_condition			(CActionBase<CAI_Stalker> *action, u16 id, ObjectHandlerSpace::EWorldProperties property, _value_type value)
{
	action->add_condition(CWorldProperty(uid(id,property),value));
}

IC	void CObjectHandler::add_effect				(CActionBase<CAI_Stalker> *action, u16 id, ObjectHandlerSpace::EWorldProperties property, _value_type value)
{
	action->add_effect	(CWorldProperty(uid(id,property),value));
}

IC	ObjectHandlerSpace::EWorldProperties CObjectHandler::object_property(MonsterSpace::EObjectAction object_action) const
{
	switch (object_action) {
		case MonsterSpace::eObjectActionSwitch1		: return(ObjectHandlerSpace::eWorldPropertySwitch1);
		case MonsterSpace::eObjectActionSwitch2		: return(ObjectHandlerSpace::eWorldPropertySwitch2);
		case MonsterSpace::eObjectActionAim1		: return(ObjectHandlerSpace::eWorldPropertyAiming1);
		case MonsterSpace::eObjectActionAim2		: return(ObjectHandlerSpace::eWorldPropertyAiming2);
		case MonsterSpace::eObjectActionFire1		: return(ObjectHandlerSpace::eWorldPropertyFiring1);
		case MonsterSpace::eObjectActionFire2		: return(ObjectHandlerSpace::eWorldPropertyFiring2);
		case MonsterSpace::eObjectActionIdle		: return(ObjectHandlerSpace::eWorldPropertyIdle);
		case MonsterSpace::eObjectActionStrapped	: return(ObjectHandlerSpace::eWorldPropertyIdleStrap);
		case MonsterSpace::eObjectActionDrop		: return(ObjectHandlerSpace::eWorldPropertyDropped);
		case MonsterSpace::eObjectActionActivate	: return(ObjectHandlerSpace::eWorldPropertyIdle);
		case MonsterSpace::eObjectActionDeactivate	: return(ObjectHandlerSpace::eWorldPropertyNoItemsIdle);
		case MonsterSpace::eObjectActionAimReady1	: return(ObjectHandlerSpace::eWorldPropertyAimingReady1);
		case MonsterSpace::eObjectActionAimReady2	: return(ObjectHandlerSpace::eWorldPropertyAimingReady2);
		default							: NODEFAULT;
	}
#ifdef DEBUG
	return	(ObjectHandlerSpace::eWorldPropertyDummy);
#endif
}