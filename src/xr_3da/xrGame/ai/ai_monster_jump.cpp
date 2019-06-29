#include "stdafx.h"
#include "ai_monster_jump.h"
#include "../custommonster.h"
#include "../phmovementcontrol.h"

CJumping::CJumping()
{
	active	= false;
}

void CJumping::Init(CCustomMonster *pM)
{
	pMonster	= pM;
	Reset		();
}

void CJumping::Reset()
{
	active				= false;
	time_next_allowed	= 0;
}

void CJumping::Load(LPCSTR section)
{
	m_fJumpFactor					= pSettings->r_float(section,"jump_factor");
	m_fJumpMinDist					= pSettings->r_float(section,"jump_min_dist");		
	m_fJumpMaxDist					= pSettings->r_float(section,"jump_max_dist");		
	m_fJumpMaxAngle					= pSettings->r_float(section,"jump_max_angle");
	m_dwDelayAfterJump				= pSettings->r_u32	(section,"jump_delay");
	m_fTraceDist					= pSettings->r_float(section,"jump_trace_dist");
}

void CJumping::AddState(CMotionDef *motion, EJumpStateType type, bool change, float linear, float angular)
{
	SJumpState jmp;

	jmp.motion			= motion;
	jmp.type			= type;
	jmp.change			= change;
	jmp.speed.linear	= linear;
	jmp.speed.angular	= angular;

	bank.push_back(jmp);
}

void CJumping::Start()
{
	ptr_cur		= bank.begin();
	cur_motion	= 0;
	active		= true;

	ApplyParams();
	if (JT_GLIDE == ptr_cur->type) Execute();

}

void CJumping::Stop()
{
	active = false;
		
	OnJumpStop();
}
// вызывается на каждом SelectAnimation
bool CJumping::PrepareAnimation(CMotionDef **m)
{
	if (0 != cur_motion) return false;

	*m = cur_motion = ptr_cur->motion;
	return true;
}
// вызывается по окончанию анимации
void CJumping::OnAnimationEnd()
{
	if (ptr_cur->change) NextState();
}

void CJumping::ApplyParams()
{
	pMonster->m_fCurSpeed		= ptr_cur->speed.linear;
	pMonster->CMovementManager::m_body.speed		= ptr_cur->speed.angular;
}

void CJumping::NextState()
{
	++ptr_cur;
	if (bank.end() == ptr_cur) {
		Stop();
		return;
	}

	cur_motion = 0;
	ApplyParams();

	if (JT_GLIDE == ptr_cur->type) Execute();
}

void CJumping::Execute()
{
	if (entity) {
		// установить целевую точку 
		CObject		*_entity = const_cast<CObject*>(entity);
		VERIFY		(_entity);
		u16 bone_id = smart_cast<CKinematics*>(_entity->Visual())->LL_BoneID("bip01_head");
		CBoneInstance &bone = smart_cast<CKinematics*>(_entity->Visual())->LL_GetBoneInstance(bone_id);

		Fmatrix global_transform;
		global_transform.set(entity->XFORM());
		global_transform.mulB(bone.mTransform);
		target_pos = global_transform.c;
		
		target_pos.y -= 0.5f;
	}

	// получить время физ.прыжка
	ph_time = pMonster->m_PhysicMovementControl->JumpMinVelTime(target_pos);
	// выполнить прыжок в соответствии с делителем времени
	pMonster->m_PhysicMovementControl->Jump(target_pos,ph_time/m_fJumpFactor);

	time_started		= pMonster->m_dwCurrentTime;
	time_next_allowed	= time_started + m_dwDelayAfterJump;

}

void CJumping::Update()
{
	if (!active) return;

	TTime itime = TTime(ph_time * 1000);

	// проверить на завершение прыжка
	if ((time_started + itime < pMonster->m_dwCurrentTime + TTime(itime/5))) {
		NextState();
	}

}

// Проверка на возможность прыжка
bool CJumping::Check(Fvector from_pos, Fvector to_pos, const CObject *pO)
{
	if (!CanJump() || active || (time_next_allowed > pMonster->m_dwCurrentTime)) return false;

	// растояние до цели	
	float dist = from_pos.distance_to(to_pos);	

	// получить вектор направления и его мир угол
	Fvector dir;
	float  dest_yaw, pitch;
	dir.sub(to_pos, from_pos);
	dir.getHP(dest_yaw, pitch);
	dest_yaw *= -1;
	dest_yaw = angle_normalize(dest_yaw);

	// проверка на max_angle и на dist
	if ((angle_difference(pMonster->CMovementManager::m_body.current.yaw, dest_yaw) > m_fJumpMaxAngle)|| !(m_fJumpMinDist <=dist && dist <= m_fJumpMaxDist)) return false;

	// можно прыгать; инициализировать параметры прыжка
	active			= true;

	target_yaw		= dest_yaw;
	this->from_pos	= from_pos;
	target_pos		= to_pos;
	entity			= pO;
	ph_time			= 0.f;

	pMonster->CMovementManager::m_body.target.yaw = target_yaw;
	pMonster->enable_movement(false);

	Start();
	return true;
}

