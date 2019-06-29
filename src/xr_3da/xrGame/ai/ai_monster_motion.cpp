#include "stdafx.h"
#include "ai_monster_motion.h"
#include "ai_monster_jump.h"
#include "biting/ai_biting.h"
#include "ai_monster_utils.h"
#include "../PHMovementControl.h"
#include "anim_triple.h"
#include "ai_monster_debug.h"

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
		"ACT_JUMP",
		"ACT_TURN",
};	


CMotionManager::CMotionManager() 
{
}

CMotionManager::~CMotionManager()
{
}


void CMotionManager::Init (CAI_Biting	*pM)
{
	pMonster				= pM;
}

void CMotionManager::reinit()
{
	prev_action				= m_tAction	= ACT_STAND_IDLE;
	m_tpCurAnim				= 0;
	spec_params				= 0;

	should_play_die_anim	= true;			//этот флаг на NetSpawn должен устанавливаться в true
	
	Seq_Init				();
	
	fx_time_last_play		= 0;
	b_forced_velocity		= false;
	
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
}


// Устанавливает текущую анимацию, которую необходимо проиграть.
// Возвращает false, если в смене анимации нет необходимости
bool CMotionManager::PrepareAnimation()
{
	// проверка на отыгрывание анимации смерти
	if (!pMonster->g_Alive()) 
		if (should_play_die_anim) {
			should_play_die_anim = false;  // отыграть анимацию смерти только раз
			if (get_sd()->m_tAnims.find(eAnimDie) != get_sd()->m_tAnims.end()) cur_anim_info().motion = eAnimDie;
			else return false;
		} else return false;

	if (pJumping && pJumping->IsActive())  return pJumping->PrepareAnimation(&m_tpCurAnim);
	
	if (0 != m_tpCurAnim) return false;
	
	if (TA_IsActive() && pCurAnimTriple->prepare_animation(&m_tpCurAnim)) return true;

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
	m_tpCurAnim = get_motion_def(anim_it,index);

	// Заполнить текущую анимацию
	string64 st;
	sprintf(st, "%s%d", *anim_it->second.target_name, index);
	m_cur_anim.name				= st; 
	m_cur_anim.index			= u8(index);
	m_cur_anim.time_started		= Level().timeServer();
	m_cur_anim.speed.current	= -1.f;
	m_cur_anim.speed.target		= -1.f;

	// инициализировать информацию о текущей анимации шагания
	//STEPS_Initialize();
	pMonster->on_animation_start(m_cur_anim.name);

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


// Установка линейной и угловой скоростей для текущей анимации
void CMotionManager::ApplyParams()
{
	ANIM_ITEM_MAP_IT	item_it = get_sd()->m_tAnims.find(cur_anim_info().motion);
	VERIFY(get_sd()->m_tAnims.end() != item_it);

	pMonster->m_velocity_linear.target	= item_it->second.velocity->velocity.linear;
	if (!b_forced_velocity) pMonster->m_velocity_angular = item_it->second.velocity->velocity.angular_real;
}

// Callback на завершение анимации
void CMotionManager::OnAnimationEnd() 
{ 
	m_tpCurAnim = 0;
	if (seq_playing) Seq_Switch();

	if (pJumping && pJumping->IsActive()) pJumping->OnAnimationEnd();
}

// если монстр стоит на месте и играет анимацию движения - force stand idle
void CMotionManager::FixBadState()
{	
//	if (!pMonster->MotionStats->is_good_motion(3) || (IsMoving() && !pMonster->IsMovingOnPath())) {
//		cur_anim = eAnimStandIdle;
//	}
}

void CMotionManager::CheckReplacedAnim()
{
	for (REPLACED_ANIM_IT it=m_tReplacedAnims.begin(); m_tReplacedAnims.end()!=it ;++it) 
		if ((cur_anim_info().motion == it->cur_anim) && (*(it->flag) == true)) { 
			cur_anim_info().motion = it->new_anim;
			return;
		}
}

bool CMotionManager::IsMoving()
{
	return ((ACT_WALK_FWD == m_tAction) || (ACT_WALK_BKWD == m_tAction) || (ACT_RUN == m_tAction) || 
		(ACT_DRAG == m_tAction) || (ACT_STEAL == m_tAction));
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

	seq_playing = true;

	// установить параметры
	cur_anim_info().motion	= *seq_it;
	ApplyParams ();

	ForceAnimSelect();
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
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
// Attack Animation

#define TIME_OFFSET 10
#define TIME_DELTA	1000

// Сравнивает тек. время с временами хитов в стеке
bool CMotionManager::AA_TimeTest(SAAParam &params)
{
	// Частота хитов не может быть больше 'TIME_DELTA'
	TTime cur_time	= Level().timeServer();

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
	m_tpCurAnim = 0; 
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

	return m_cur_anim.blend->timeTotal;
}

float CMotionManager::GetAnimSpeed(EMotionAnim anim)
{
	ANIM_ITEM_MAP_IT anim_it = get_sd()->m_tAnims.find(anim);
	VERIFY(get_sd()->m_tAnims.end() != anim_it);

	CMotionDef *def = get_motion_def(anim_it, 0);

	return def->Dequantize(def->speed);
}


void CMotionManager::ForceAngularSpeed(float vel)
{
	pMonster->m_velocity_angular = vel;
	b_forced_velocity = true;
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
		case pMonster->eVelocityParameterStand:			return ACT_STAND_IDLE;
		case pMonster->eVelocityParameterWalkNormal:	return ACT_WALK_FWD;
		case pMonster->eVelocityParameterRunNormal:		return ACT_RUN;
		case pMonster->eVelocityParameterWalkDamaged:	return ACT_WALK_FWD;
		case pMonster->eVelocityParameterRunDamaged:	return ACT_RUN;
		case pMonster->eVelocityParameterSteal:			return ACT_STEAL;
		case pMonster->eVelocityParameterDrag:			return ACT_DRAG;
		case pMonster->eVelocityParameterInvisible:		return ACT_RUN;
		default:										NODEFAULT;
	}

	return ACT_STAND_IDLE;
}

EAction CMotionManager::GetActionFromPath()
{
	EAction action;
	
	u32 cur_point_velocity_index = pMonster->CDetailPathManager::path()[pMonster->curr_travel_point_index()].velocity;
	action = VelocityIndex2Action(cur_point_velocity_index);

	u32 next_point_velocity_index = u32(-1);
	if (pMonster->CDetailPathManager::path().size() > pMonster->curr_travel_point_index() + 1) 
		next_point_velocity_index = pMonster->CDetailPathManager::path()[pMonster->curr_travel_point_index() + 1].velocity;

	if ((cur_point_velocity_index == pMonster->eVelocityParameterStand) && (next_point_velocity_index != u32(-1))) {
		if (angle_difference(pMonster->m_body.current.yaw, pMonster->m_body.target.yaw) < deg(1)) 
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

// End Debug
//////////////////////////////////////////////////////////////////////////

//void CMotionManager::STEPS_Update(u8 legs_num)
//{
//	if (step_info.disable) return;
//	
//	SGameMtlPair* mtl_pair		= pMonster->CMaterialManager::get_current_pair();
//	if (!mtl_pair) return;
//
//	// получить параметры шага
//	SStepParam &step		= step_info.params;
//	TTime		cur_time	= Level().timeServer();
//
//	// время одного цикла анимации
//	float cycle_anim_time	= GetCurAnimTime() / step.cycles;
//
//	for (u32 i=0; i<legs_num; i++) {
//		
//		// если событие уже обработано для этой ноги, то skip
//		if (step_info.activity[i].handled && (step_info.activity[i].cycle == step_info.cur_cycle)) continue;
//		
//		// вычислить смещённое время шага в соответствии с параметрами анимации ходьбы
//		TTime offset_time = m_cur_anim.time_started + u32(1000 * (cycle_anim_time * (step_info.cur_cycle-1) + cycle_anim_time * step.step[i].time));
//
//		if ((offset_time >= (cur_time - TIME_OFFSET)) && (offset_time <= (cur_time + TIME_OFFSET)) ){
//			
//			// Играть звук
//			if (!mtl_pair->StepSounds.empty()) {
//				
//				Fvector sound_pos = pMonster->Position();
//				sound_pos.y += 0.5;
//
//				SELECT_RANDOM(step_info.activity[i].sound, mtl_pair, StepSounds);
//				step_info.activity[i].sound.play_at_pos	(pMonster,sound_pos);
//
//			}
//			
//			// Играть партиклы
//			if (!mtl_pair->CollideParticles.empty()) {
//				LPCSTR ps_name = *mtl_pair->CollideParticles[::Random.randI(0,mtl_pair->CollideParticles.size())];
//				
//				//отыграть партиклы столкновения материалов
//				CParticlesObject* ps = xr_new<CParticlesObject>(ps_name);
//
//				// вычислить позицию и направленность партикла
//				Fmatrix pos; 
//
//				// установить направление
//				pos.k.set(Fvector().set(0.0f,1.0f,0.0f));
//				Fvector::generate_orthonormal_basis(pos.k, pos.j, pos.i);
//				
//				// установить позицию
//				pos.c.set(pMonster->get_foot_position(ELegType(i)));
//				
//				ps->UpdateParent(pos,Fvector().set(0.f,0.f,0.f));
//				Level().ps_needtoplay.push_back(ps);
//			}
//
//			// Play Camera FXs
//			pMonster->event_on_step();
//
//			// обновить поле handle
//			step_info.activity[i].handled	= true;
//			step_info.activity[i].cycle		= step_info.cur_cycle;
//		}
//	}
//
//	// определить текущий цикл
//	if (step_info.cur_cycle < step.cycles) step_info.cur_cycle = 1 + u8(float(cur_time - m_cur_anim.time_started) / (1000.f*cycle_anim_time));
//
//	// позиционировать играемые звуки
//	for (i=0; i<legs_num; i++) {
//		if (step_info.activity[i].handled && step_info.activity[i].sound.feedback) {
//			Fvector sound_pos = pMonster->Position();
//			sound_pos.y += 0.5;
//			step_info.activity[i].sound.set_position	(sound_pos);
//			//step_info.activity[i].sound.set_volume	(step_info.params.step[i].power);
//		}
//	}
//}
//
//void CMotionManager::STEPS_Initialize() 
//{
//	// искать текущую анимацию в STEPS_MAP
//	STEPS_MAP_IT it = get_sd()->steps_map.find(m_cur_anim.name);
//	if (it == get_sd()->steps_map.end()) {
//		step_info.disable = true;
//		return;
//	}
//
//	step_info.disable	= false;
//	step_info.params	= it->second;
//	step_info.cur_cycle = 1;					// all cycles are 1-based
//
//	for (u32 i=0; i<4; i++) {
//		step_info.activity[i].handled	= false;
//		step_info.activity[i].cycle		= step_info.cur_cycle;	
//	}
//
//}

/////////////////////////////////////////////////////////////////////////////////////////

void CMotionManager::TA_Activate(CAnimTriple *p_triple)
{
	if (pCurAnimTriple && pCurAnimTriple->is_active()) pCurAnimTriple->deactivate();
	
	pCurAnimTriple				= p_triple;
	pCurAnimTriple->activate	();

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

#ifdef _DEBUG	
	Fvector P1,P2;
	P1 = pMonster->Position(); P1.y+=2.0f;
 	P2.mad(P1, pMonster->Direction(), 2.f);
	pMonster->HDebug->L_Clear	();
	pMonster->HDebug->L_AddLine	(P1, P2, D3DCOLOR_XRGB(255,255,0));
#endif 
	
	ANIM_ITEM_MAP_IT item_it = get_sd()->m_tAnims.find(cur_anim_info().motion);
	bool is_moving_anim		= !fis_zero(item_it->second.velocity->velocity.linear);
	bool is_moving_on_path	= pMonster->IsMovingOnPath();

	if (is_moving_on_path && is_moving_anim) {
		pMonster->SetDirectionLook(item_it->first == eAnimDragCorpse);
		return;
	}

	if (!is_moving_on_path && is_moving_anim) {
		m_tpCurAnim							= 0;
		cur_anim_info().motion				= eAnimStandIdle;
		pMonster->stop_now();
		return;
	}

	if (is_moving_on_path && !is_moving_anim) {
		pMonster->stop_now();
		return;
	}

	float angle_diff = angle_difference(pMonster->m_body.target.yaw, pMonster->m_body.current.yaw);
	if (angle_diff < deg(1) && ((item_it->first == eAnimStandTurnLeft) || (item_it->first == eAnimStandTurnRight))) {
		m_tpCurAnim					= 0;
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
			if (0 != smart_cast<CSkeletonAnimated*>(pMonster->Visual())->ID_Cycle_Safe(strconcat(s_temp, *it->second.target_name,itoa(i,s,10))))  count++;
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
	string128 s1,s2;
	return smart_cast<CSkeletonAnimated*>(pMonster->Visual())->ID_Cycle_Safe(strconcat(s2,*it->second.target_name,itoa(index,s1,10)));
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
	//return (JumpActive() || Seq_Active() || TA_IsActive());
	return (JumpActive() || Seq_Active());
}