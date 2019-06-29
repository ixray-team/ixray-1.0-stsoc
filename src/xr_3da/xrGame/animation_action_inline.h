////////////////////////////////////////////////////////////////////////////
//	Module 		: animation_action_inline.h
//	Created 	: 05.04.2004
//  Modified 	: 05.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Animation action inline functions
////////////////////////////////////////////////////////////////////////////

IC	CAnimationAction::CAnimationAction	() :
	m_mental_state	(MonsterSpace::eMentalStateDanger)
{
}

IC	CAnimationAction::CAnimationAction	(const MonsterSpace::EMentalState &mental_state) :
	m_mental_state	(mental_state)
{
}
