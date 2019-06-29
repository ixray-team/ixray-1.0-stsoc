#include "stdafx.h"
#include "ai_monster_motion.h"
#include "BaseMonster/base_monster.h"

// Shared
// Загрузка параметров анимации. Вызывать необходимо на Monster::Load
void CMotionManager::AddAnim(EMotionAnim ma, LPCSTR tn, int s_id, SVelocityParam *vel, EPState p_s, LPCSTR fx_front, LPCSTR fx_back, LPCSTR fx_left, LPCSTR fx_right)
{
	SAnimItem new_item;

	new_item.target_name	= tn;
	new_item.spec_id		= s_id;
	new_item.velocity		= vel;
	new_item.pos_state		= p_s;

	new_item.fxs.front		= fx_front;
	new_item.fxs.back		= fx_back;
	new_item.fxs.left		= fx_left;
	new_item.fxs.right		= fx_right;

	new_item.count			= 0;

	get_sd()->m_tAnims.insert			(mk_pair(ma, new_item));
}

// Shared
// Загрузка параметров анимации. Вызывать необходимо на Monster::Load
void CMotionManager::AddAnim(EMotionAnim ma, LPCSTR tn, int s_id, SVelocityParam *vel, EPState p_s)
{
	SAnimItem new_item;

	new_item.target_name	= tn;
	new_item.spec_id		= s_id;
	new_item.velocity		= vel;
	new_item.pos_state		= p_s;

	new_item.count			= 0;

	get_sd()->m_tAnims.insert			(mk_pair(ma, new_item));
}

// Shared
void CMotionManager::AddTransition(EMotionAnim from, EMotionAnim to, EMotionAnim trans, bool chain, bool skip_aggressive)
{
	STransition new_item;

	new_item.from.state_used	= false;
	new_item.from.anim			= from;

	new_item.target.state_used	= false;
	new_item.target.anim		= to;

	new_item.anim_transition	= trans;
	new_item.chain				= chain;

	new_item.skip_if_aggressive	= skip_aggressive;

	get_sd()->m_tTransitions.push_back(new_item);
}

// Shared
void CMotionManager::AddTransition(EMotionAnim from, EPState to, EMotionAnim trans, bool chain, bool skip_aggressive)
{
	STransition new_item;

	new_item.from.state_used	= false;
	new_item.from.anim			= from;

	new_item.target.state_used	= true;
	new_item.target.state		= to;

	new_item.anim_transition	= trans;
	new_item.chain				= chain;
	new_item.skip_if_aggressive	= skip_aggressive;

	get_sd()->m_tTransitions.push_back(new_item);
}

// Shared
void CMotionManager::AddTransition(EPState from, EMotionAnim to, EMotionAnim trans, bool chain, bool skip_aggressive)
{
	STransition new_item;

	new_item.from.state_used	= true;
	new_item.from.state			= from;

	new_item.target.state_used	= false;
	new_item.target.anim		= to;


	new_item.anim_transition	= trans;
	new_item.chain				= chain;
	new_item.skip_if_aggressive	= skip_aggressive;

	get_sd()->m_tTransitions.push_back(new_item);
}

// Shared
void CMotionManager::AddTransition(EPState from, EPState to, EMotionAnim trans, bool chain, bool skip_aggressive)
{
	STransition new_item;

	new_item.from.state_used	= true;
	new_item.from.state			= from;

	new_item.target.state_used	= true;
	new_item.target.state		= to;

	new_item.anim_transition	= trans;
	new_item.chain				= chain;
	new_item.skip_if_aggressive	= skip_aggressive;

	get_sd()->m_tTransitions.push_back(new_item);
}

// Shared
void CMotionManager::LinkAction(EAction act, EMotionAnim pmt_motion, EMotionAnim pmt_left, EMotionAnim pmt_right, float pmt_angle)
{

	SMotionItem new_item;

	new_item.anim				= pmt_motion;
	new_item.is_turn_params		= true;
	new_item.turn.anim_left		= pmt_left;
	new_item.turn.anim_right	= pmt_right;
	new_item.turn.min_angle		= pmt_angle;

	get_sd()->m_tMotions.insert	(mk_pair(act, new_item));
}

// Shared
void CMotionManager::LinkAction(EAction act, EMotionAnim pmt_motion)
{
	SMotionItem new_item;

	new_item.anim				= pmt_motion;
	new_item.is_turn_params		= false;

	get_sd()->m_tMotions.insert	(mk_pair(act, new_item));
}

// не может быть shared!!! поле на которое указывает b_flag, у каждого экземпляра класса biting - содержит свои данные
void CMotionManager::AddReplacedAnim(bool *b_flag, EMotionAnim pmt_cur_anim, EMotionAnim pmt_new_anim)
{
	SReplacedAnim ra;

	ra.flag		= b_flag;
	ra.cur_anim = pmt_cur_anim;
	ra.new_anim = pmt_new_anim;

	m_tReplacedAnims.push_back(ra);
}

// Shared
void CMotionManager::AA_Load(LPCSTR section)
{
	if (!pSettings->section_exist(section)) return;

	SAAParam	anim;
	LPCSTR		anim_name,val;
	string16	cur_elem;

	for (u32 i=0; pSettings->r_line(section,i,&anim_name,&val); ++i) {
		_GetItem	(val,0,cur_elem);		anim.time			= float(atof(cur_elem));
		_GetItem	(val,1,cur_elem);		anim.hit_power		= float(atof(cur_elem));
		_GetItem	(val,2,cur_elem);		anim.impulse		= float(atof(cur_elem));
		_GetItem	(val,3,cur_elem);		anim.impulse_dir.x	= float(atof(cur_elem));
		_GetItem	(val,4,cur_elem);		anim.impulse_dir.y	= float(atof(cur_elem));
		_GetItem	(val,5,cur_elem);		anim.impulse_dir.z	= float(atof(cur_elem));
		_GetItem	(val,6,cur_elem);		anim.foh.from_yaw	= float(atof(cur_elem));
		_GetItem	(val,7,cur_elem);		anim.foh.to_yaw		= float(atof(cur_elem));
		_GetItem	(val,8,cur_elem);		anim.foh.from_pitch	= float(atof(cur_elem));
		_GetItem	(val,9,cur_elem);		anim.foh.to_pitch	= float(atof(cur_elem));
		_GetItem	(val,10,cur_elem);		anim.dist			= float(atof(cur_elem));

		anim.impulse_dir.normalize();
		
		float clamp_val = PI_DIV_2 - EPS_L;
		clamp(anim.foh.from_yaw,	-clamp_val, clamp_val);
		clamp(anim.foh.to_yaw,		-clamp_val, clamp_val);
		clamp(anim.foh.from_pitch,	-clamp_val, clamp_val);
		clamp(anim.foh.to_pitch,	-clamp_val, clamp_val);

		get_sd()->aa_map.insert(mk_pair(anim_name, anim));
	}
}
