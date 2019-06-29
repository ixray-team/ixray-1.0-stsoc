#include "stdafx.h"
#include "jump_ability.h"
#include "anim_triple.h"

CJumpingAbility::CJumpingAbility()
{
	m_animation_holder = xr_new<CAnimTriple>();
}

CJumpingAbility::~CJumpingAbility()
{
	xr_delete(m_animation_holder);
}

void CJumpingAbility::init_external(CMotionDef *m_def1, CMotionDef *m_def2, CMotionDef *m_def3)
{
	m_animation_holder->init_external(m_def1, m_def2, m_def3);
}

void CJumpingAbility::jump()
{
}

void CJumpingAbility::update_frame()
{

}

void CJumpingAbility::update_scheduled()
{

}
