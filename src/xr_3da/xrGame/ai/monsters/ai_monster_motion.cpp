#include "stdafx.h"
#include "ai_monster_motion.h"
#include "ai_monster_jump.h"
#include "BaseMonster/base_monster.h"
#include "ai_monster_utils.h"
#include "../../PHMovementControl.h"
#include "anim_triple.h"
#include "../../../skeletonanimated.h"
#include "../../detail_path_manager.h"
#include "ai_monster_movement.h"
#include "ai_monster_movement_space.h"
#include "critical_action_info.h"

// DEBUG purpose only
char *dbg_action_name_table[] = {
		"ACT_STAND_IDLE",
		"ACT_SIT_IDLE",
		"ACT_LIE_IDLE",
		"ACT_WALK_FWD",
		"ACT_WALK_BKWD",
		"ACT_RUN",
		"ACT_EAT",
		"ACT_SLEEP",
		"ACT_REST",
		"ACT_DRAG",
		"ACT_ATTACK",
		"ACT_STEAL",
		"ACT_LOOK_AROUND",
		"ACT_JUMP"
};	


CMotionManager::CMotionManager() 
{
}

CMotionManager::~CMotionManager()
{
}


void CMotionManager::Init (CBaseMonster	*pM)
{
	pMonster				= pM;
}

void CMotionManager::reinit()
{
	prev_action				= m_tAction	= ACT_STAND_IDLE;
	m_tpCurAnim.invalidate	();
	spec_params				= 0;

	Seq_Init				();
	
	fx_time_last_play		= 0;
	
	accel_init				();
	
	aa_time_last_attack		= 0;

	pCurAnimTriple			= 0;

	// обновить количество анимаций
	UpdateAnimCount			();

	pJumping				= 0;

	// инициализация информации о текущей анимации
	m_cur_anim.motion			= eAnimStandIdle;
	m_cur_anim.index			= 0;
	m_cur_anim.time_started		= 0;
	m_cur_anim.speed.current	= -1.f;
	m_cur_anim.speed.target		= -1.f;
	m_cur_anim.blend			= 0;
	m_cur_anim.speed_change_vel	= 1.f;

	prev_motion					= cur_anim_info().motion; 

	transition_sequence_used	= false;
	target_transition_anim		= cur_anim_info().motion;

	m_prev_character_velocity	= 0.01f;

	m_anim_motion_map.clear		();
}


// Устанавливает текущую анимацию, которую необходимо проиграть.
// Возвращает false, если в смене анимации нет необходимости
bool CMotionManager::PrepareAnimation()
{
	if (pJumping && pJumping->IsActive())  {
		m_cur_anim.speed.current	= -1.f;
		m_cur_anim.speed.target		= -1.f;
		return pJumping->PrepareAnimation(m_tpCurAnim);
	}
	if (m_tpCurAnim) return false;
	if (TA_IsActive()) {
		bool ret_value				= pCurAnimTriple->prepare_animation(m_tpCurAnim);

		m_cur_anim.name				= GetAnimTranslation(m_tpCurAnim);
			
		m_cur_anim.speed.current	= -1.f;
		m_cur_anim.speed.target		= -1.f;
		
		m_cur_anim.time_started		= Device.dwTimeGlobal;

		return ret_value;
	}

	// перекрыть все определения и установть анимацию
	pMonster->ForceFinalAnimation();

	// получить элемент SAnimItem, соответствующий текущей анимации
	ANIM_ITEM_MAP_IT anim_it = get_sd()->m_tAnims.find(cur_anim_info().motion);
	VERIFY(get_sd()->m_tAnims.end() != anim_it);

	// определить необходимый индекс
	int index;
	if (-1 != anim_it->second.spec_id) index = anim_it->second.spec_id;
	else {
		VERIFY(anim_it->second.count != 0);
		index = ::Random.randI(anim_it->second.count);
	}

	
	// установить анимацию	
	{
		string128			s1,s2;
		m_tpCurAnim			= smart_cast<CSkeletonAnimated*>(pMonster->Visual())->ID_Cycle_Safe(strconcat(s2,*anim_it->second.target_name,itoa(index,s1,10)));
	}

	// Заполнить текущую анимацию
	string64	st,tmp;
	strconcat	(st,*anim_it->second.target_name,itoa(index,tmp,10));
//	sprintf		(st, "%s%d", *anim_it->second.target_name, index);
	m_cur_anim.name				= st; 
	m_cur_anim.index			= u8(index);
	m_cur_anim.time_started		= Device.dwTimeGlobal;
	m_cur_anim.speed.current	= -1.f;
	m_cur_anim.speed.target		= -1.f;

	return true;
}

// проверить существует ли переход из анимации from в to
void CMotionManager::CheckTransition(EMotionAnim from, EMotionAnim to)
{
	
	// поиск соответствующего перехода
	bool		b_activated	= false;
	EMotionAnim cur_from = from; 
	EPState		state_from	= GetState(cur_from);
	EPState		state_to	= GetState(to);

	TRANSITION_ANIM_VECTOR_IT I = get_sd()->m_tTransitions.begin();
	bool bVectEmpty = get_sd()->m_tTransitions.empty();

	while (!bVectEmpty) {		// вход в цикл, если вектор переходов не пустой

		bool from_is_good	= ((I->from.state_used) ? (I->from.state == state_from) : (I->from.anim == cur_from));
		bool target_is_good = ((I->target.state_used) ? (I->target.state == state_to) : (I->target.anim == to));

		if (from_is_good && target_is_good) {

			if (I->skip_if_aggressive && pMonster->m_bAggressive) return;

			// переход годится
			Seq_Add(I->anim_transition);
			b_activated	= true;	

			if (I->chain) {
				cur_from	= I->anim_transition;
				state_from	= GetState(cur_from);
				I = get_sd()->m_tTransitions.begin();			// начать сначала
				continue;
			} else break;
		}
		if (get_sd()->m_tTransitions.end() == ++I) break;
	}

	if (b_activated) {
		transition_sequence_used	= true;
		target_transition_anim		= to;
		Seq_Switch				();
	}
}

// Callback на завершение анимации
void CMotionManager::OnAnimationEnd() 
{ 
	m_tpCurAnim.invalidate();
	if (seq_playing) Seq_Switch();

	if (pJumping && pJumping->IsActive()) pJumping->OnAnimationEnd();
}

void CMotionManager::CheckReplacedAnim()
{
	for (REPLACED_ANIM_IT it=m_tReplacedAnims.begin(); m_tReplacedAnims.end()!=it ;++it) 
		if ((cur_anim_info().motion == it->cur_anim) && (*(it->flag) == true)) { 
			cur_anim_info().motion = it->new_anim;
			return;
		}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Работа с последовательностями
void CMotionManager::Seq_Init()
{
	seq_states.clear	();
	seq_it				= seq_states.end();
	seq_playing			= false;	
}

void CMotionManager::Seq_Add(EMotionAnim a)
{
	if (Seq_Active()) Seq_Init();
	seq_states.push_back(a);
}

void CMotionManager::Seq_Switch()
{
	if (!seq_playing) {
		// активация последовательностей
		seq_it = seq_states.begin();
	} else {
		++seq_it; 
		if (seq_states.end() == seq_it) {
			Seq_Finish();
			return;
		}
	}

	seq_playing								= true;

	// установить параметры
	cur_anim_info().motion					= *seq_it;

	ForceAnimSelect							();

	// lock path
	pMonster->movement().stop_now			();
	pMonster->CriticalActionInfo->set		(CAF_LockPath | CAF_LockTurn | CAF_LockMotion);
}


void CMotionManager::Seq_Finish()
{
	Seq_Init(); 

	if (transition_sequence_used) { 
		prev_motion = cur_anim_info().motion	= target_transition_anim;
		transition_sequence_used = false;
	} else {
		prev_motion	= cur_anim_info().motion	= get_sd()->m_tMotions[m_tAction].anim;
	}

	pMonster->CriticalActionInfo->clear			(CAF_LockPath | CAF_LockTurn | CAF_LockMotion);
	pMonster->movement().initialize_movement	();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
// Attack Animation

#define TIME_OFFSET 10
#define TIME_DELTA	200

// Сравнивает тек. время с временами хитов в стеке
bool CMotionManager::AA_TimeTest(SAAParam &params)
{

	// Частота хитов не может быть больше 'TIME_DELTA'
	TTime cur_time	= Device.dwTimeGlobal;

	if (aa_time_last_attack + TIME_DELTA > cur_time) return false;

	// искать текущую анимацию в AA_MAP

	AA_MAP_IT it = get_sd()->aa_map.find(m_cur_anim.name);
	if (it == get_sd()->aa_map.end()) return false;
	
	// вычислить смещённое время хита в соответствии с параметрами анимации атаки
	TTime offset_time = m_cur_anim.time_started + u32(1000 * GetCurAnimTime() * it->second.time);

	if ((offset_time >= (cur_time - TIME_OFFSET)) && (offset_time <= (cur_time + TIME_OFFSET)) ){
		params = it->second;
		return true;
	}

	return false;
}

void CMotionManager::AA_GetParams(SAAParam &params, LPCSTR anim_name)
{
	shared_str st = anim_name;

	// искать текущую анимацию в AA_MAP
	AA_MAP_IT it = get_sd()->aa_map.find(st);
	if (it == get_sd()->aa_map.end()) return;
	
	params = it->second;
}
//////////////////////////////////////////////////////////////////////////


EPState	CMotionManager::GetState (EMotionAnim a)
{
	// найти анимацию 
	ANIM_ITEM_MAP_IT  item_it = get_sd()->m_tAnims.find(a);
	VERIFY(get_sd()->m_tAnims.end() != item_it);

	return item_it->second.pos_state;
}

void CMotionManager::ForceAnimSelect() 
{
	m_tpCurAnim.invalidate(); 
	pMonster->SelectAnimation(pMonster->Direction(),pMonster->Direction(),0);
}

#define FX_CAN_PLAY_MIN_INTERVAL	50

void CMotionManager::FX_Play(EHitSide side, float amount)
{
	if (fx_time_last_play + FX_CAN_PLAY_MIN_INTERVAL > pMonster->m_dwCurrentTime) return;

	ANIM_ITEM_MAP_IT anim_it = get_sd()->m_tAnims.find(cur_anim_info().motion);
	VERIFY(get_sd()->m_tAnims.end() != anim_it);
	
	clamp(amount,0.f,1.f);

	shared_str	*p_str = 0;
	switch (side) {
		case eSideFront:	p_str = &anim_it->second.fxs.front;	break;
		case eSideBack:		p_str = &anim_it->second.fxs.back;	break;
		case eSideLeft:		p_str = &anim_it->second.fxs.left;	break;
		case eSideRight:	p_str = &anim_it->second.fxs.right;	break;
	}
	
	if (p_str && p_str->size()) smart_cast<CSkeletonAnimated*>(pMonster->Visual())->PlayFX(*(*p_str), amount);

	fx_time_last_play = pMonster->m_dwCurrentTime;
}


float CMotionManager::GetCurAnimTime()
{
	VERIFY(m_cur_anim.blend);

	return (m_cur_anim.blend->timeTotal / m_cur_anim.blend->speed);
}

float CMotionManager::GetAnimSpeed(EMotionAnim anim)
{
	ANIM_ITEM_MAP_IT	anim_it = get_sd()->m_tAnims.find(anim);
	VERIFY				(get_sd()->m_tAnims.end() != anim_it);

	CMotionDef			*def = get_motion_def(anim_it, 0);

	return				(def->Dequantize(def->speed));
}


bool CMotionManager::IsStandCurAnim()
{
	ANIM_ITEM_MAP_IT	item_it = get_sd()->m_tAnims.find(cur_anim_info().motion);
	VERIFY(get_sd()->m_tAnims.end() != item_it);

	if (fis_zero(item_it->second.velocity->velocity.linear)) return true;
	return false;
}


EAction CMotionManager::VelocityIndex2Action(u32 velocity_index)
{
	switch (velocity_index) {
		case MonsterMovement::eVelocityParameterStand:			return ACT_STAND_IDLE;
		case MonsterMovement::eVelocityParameterWalkNormal:		return ACT_WALK_FWD;
		case MonsterMovement::eVelocityParameterRunNormal:		return ACT_RUN;
		case MonsterMovement::eVelocityParameterWalkDamaged:	return ACT_WALK_FWD;
		case MonsterMovement::eVelocityParameterRunDamaged:		return ACT_RUN;
		case MonsterMovement::eVelocityParameterSteal:			return ACT_STEAL;
		case MonsterMovement::eVelocityParameterDrag:			return ACT_DRAG;
		case MonsterMovement::eVelocityParameterInvisible:		return ACT_RUN;
	}

	return pMonster->CustomVelocityIndex2Action(velocity_index);
}

EAction CMotionManager::GetActionFromPath()
{
	EAction action;
	
	u32 cur_point_velocity_index = pMonster->movement().detail().path()[pMonster->movement().detail().curr_travel_point_index()].velocity;
	action = VelocityIndex2Action(cur_point_velocity_index);

	u32 next_point_velocity_index = u32(-1);
	if (pMonster->movement().detail().path().size() > pMonster->movement().detail().curr_travel_point_index() + 1) 
		next_point_velocity_index = pMonster->movement().detail().path()[pMonster->movement().detail().curr_travel_point_index() + 1].velocity;

	if ((cur_point_velocity_index == MonsterMovement::eVelocityParameterStand) && (next_point_velocity_index != u32(-1))) {
		if (!pMonster->DirMan.is_turning()) 
			action = VelocityIndex2Action(next_point_velocity_index);
	}

	return action;
}



//////////////////////////////////////////////////////////////////////////
// Debug

LPCSTR CMotionManager::GetAnimationName(EMotionAnim anim)
{
	ANIM_ITEM_MAP_IT	item_it = get_sd()->m_tAnims.find(anim);
	VERIFY(get_sd()->m_tAnims.end() != item_it);

	return *item_it->second.target_name;
}

LPCSTR CMotionManager::GetActionName(EAction action)
{
	return dbg_action_name_table[action];
}
//////////////////////////////////////////////////////////////////////////


void CMotionManager::TA_Activate(CAnimTriple *p_triple, bool skip_prepare)
{
	if (pCurAnimTriple && pCurAnimTriple->is_active()) pCurAnimTriple->deactivate();
	
	pCurAnimTriple				= p_triple;
	pCurAnimTriple->activate	(skip_prepare);

	ForceAnimSelect				();
}

void CMotionManager::TA_Deactivate()
{
	if (pCurAnimTriple && pCurAnimTriple->is_active()) pCurAnimTriple->deactivate();
	
	ForceAnimSelect				();
}

void CMotionManager::TA_PointBreak()
{
	if (pCurAnimTriple && pCurAnimTriple->is_active()) pCurAnimTriple->pointbreak();
	
	ForceAnimSelect();
}

bool CMotionManager::TA_IsActive()
{	
	if (pCurAnimTriple && pCurAnimTriple->is_active()) return true;

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////

void CMotionManager::ValidateAnimation()
{

	ANIM_ITEM_MAP_IT item_it = get_sd()->m_tAnims.find(cur_anim_info().motion);
	bool is_moving_anim		= !fis_zero(item_it->second.velocity->velocity.linear);
	bool is_moving_on_path	= pMonster->movement().IsMovingOnPath();

	if (is_moving_on_path && is_moving_anim) {
		pMonster->DirMan.use_path_direction(item_it->first == eAnimDragCorpse);
		return;
	}

	if (!is_moving_on_path && is_moving_anim) {
		m_tpCurAnim.invalidate				();
		cur_anim_info().motion				= eAnimStandIdle;
		pMonster->movement().stop_now();
		return;
	}

	if (is_moving_on_path && !is_moving_anim) {
		pMonster->movement().stop_now();
		return;
	}

	if (!pMonster->DirMan.is_turning() && ((item_it->first == eAnimStandTurnLeft) || (item_it->first == eAnimStandTurnRight))) {
		m_tpCurAnim.invalidate		();
		cur_anim_info().motion		= eAnimStandIdle;
		return;
	}
}

///////////////////////////////////////////////////////////////////////////////////////
void CMotionManager::UpdateAnimCount()
{
	for (ANIM_ITEM_MAP_IT it = get_sd()->m_tAnims.begin(); it != get_sd()->m_tAnims.end(); it++)	{
		
		// проверить, были ли уже загружены данные
		if (it->second.count != 0) return;

		string128	s, s_temp; 
		u8 count = 0;
		
		for (int i=0; ; ++i) {
			if (smart_cast<CSkeletonAnimated*>(pMonster->Visual())->ID_Cycle_Safe(strconcat(s_temp, *it->second.target_name,itoa(i,s,10))))  count++;
			else break;
		}

		if (count != 0) it->second.count = count;
		else {
			sprintf(s, "Error! No animation: %s for monster %s", *it->second.target_name, *pMonster->cName());
			R_ASSERT2(count != 0, s);
		} 
	}
}

CMotionDef *CMotionManager::get_motion_def(ANIM_ITEM_MAP_IT &it, u32 index)
{
	string128			s1,s2;
	CSkeletonAnimated	*skeleton_animated = smart_cast<CSkeletonAnimated*>(pMonster->Visual());
	const MotionID		&motion_id = skeleton_animated->ID_Cycle_Safe(strconcat(s2,*it->second.target_name,itoa(index,s1,10)));
	return				(skeleton_animated->LL_GetMotionDef(motion_id));
}


void CMotionManager::ActivateJump()
{
	pJumping = smart_cast<CJumping *>(pMonster);
}
void CMotionManager::DeactivateJump()
{
	pJumping = 0;
}

bool CMotionManager::JumpActive()
{
	if (pJumping && pJumping->IsActive()) return true;
	return false;

}

bool CMotionManager::IsCriticalAction()
{
	return (JumpActive() || Seq_Active() || TA_IsActive());
	//return (JumpActive() || Seq_Active());
}

void CMotionManager::AddAnimTranslation(const MotionID &motion, LPCSTR str)
{
	m_anim_motion_map.insert(mk_pair(motion, str));	
}
shared_str CMotionManager::GetAnimTranslation(const MotionID &motion)
{
	shared_str				ret_value;

	ANIM_TO_MOTION_MAP_IT	anim_it = m_anim_motion_map.find(motion);
	if (anim_it != m_anim_motion_map.end()) ret_value = anim_it->second;

	return ret_value;
}
