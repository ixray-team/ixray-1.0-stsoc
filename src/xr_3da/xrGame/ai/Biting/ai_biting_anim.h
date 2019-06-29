////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_anim.h
//	Created 	: 22.05.2003
//  Modified 	: 22.05.2003
//	Author		: Serge Zhem
//	Description : Animations, Bone transformations and Sounds for monsters of biting class 
////////////////////////////////////////////////////////////////////////////
#pragma once

class		 CAI_Biting;

typedef		ref_str			anim_string;

#define		DEFAULT_ANIM	eAnimStandIdle

enum EMotionAnim {
	eAnimStandIdle = 0,
	eAnimStandTurnLeft,
	eAnimStandTurnRight,
	eAnimStandDamaged,

	eAnimSitIdle,
	eAnimLieIdle,

	eAnimSitToSleep,
	eAnimLieToSleep,
	eAnimStandSitDown,
	eAnimStandLieDown,
	eAnimLieStandUp,
	eAnimSitStandUp,
	eAnimStandLieDownEat,
	eAnimSitLieDown,
	eAnimLieSitUp,

	eAnimWalkFwd,
	eAnimWalkBkwd,
	eAnimWalkTurnLeft,
	eAnimWalkTurnRight,

	eAnimRun,
	eAnimRunTurnLeft,
	eAnimRunTurnRight,
	eAnimFastTurn,					

	eAnimAttack,
	eAnimAttackRat,

	eAnimEat,
	eAnimSleep,
	eAnimDie,

	eAnimDragCorpse,
	eAnimCheckCorpse,
	eAnimScared,
	eAnimAttackJump,

	eAnimLookAround,

	eAnimJump,
	eAnimSteal,

	eAnimJumpStart,
	eAnimJumpFly,		
	eAnimJumpFinish,

	eAnimJumpLeft,
	eAnimJumpRight,
};


enum EAction {
	ACT_STAND_IDLE = 0,
	ACT_SIT_IDLE,
	ACT_LIE_IDLE,
	ACT_WALK_FWD,
	ACT_WALK_BKWD,
	ACT_RUN,
	ACT_EAT,
	ACT_SLEEP,
	ACT_REST,
	ACT_DRAG,
	ACT_ATTACK,
	ACT_STEAL,
	ACT_LOOK_AROUND,
	ACT_JUMP
};

enum EPState {
	PS_STAND,
	PS_SIT, 
	PS_LIE,
};

enum ECriticalState {
	CS_NORMAL,
	CS_JUMP
};


// специальные параметры анимаций (animation spec params)
#define ASP_MOVE_BKWD			(1 << 0) 
#define ASP_DRAG_CORPSE			(1 << 1) 
#define ASP_CHECK_CORPSE		(1 << 2)
#define ASP_ATTACK_RAT			(1 << 3)
#define ASP_ATTACK_RAT_JUMP		(1 << 4)
#define	ASP_STAND_SCARED		(1 << 5)

DEFINE_VECTOR	(CMotionDef*, ANIM_VECTOR, ANIM_IT);

// элемент анимации
struct SAnimItem {

	anim_string	target_name;	// "stand_idle_"
	int			spec_id;		// (-1) - any,  (0 - ...) - идентификатор 3
				
	ANIM_VECTOR	pMotionVect;	// вектор указателей на анимации модели

	struct{
		float	linear;
		float	angular;
	} speed;

	EPState		pos_state;
};

// описание перехода
struct STransition {
	
	struct {
		bool		state_used;
		EMotionAnim anim;
		EPState		state;
	} from, target;

	EMotionAnim		anim_transition;
	bool			chain;
};

// элемент движения
struct SMotionItem {
	EMotionAnim		anim;
	bool			is_turn_params;

	struct{
		EMotionAnim	anim_left;			// speed, r_speed got from turn_left member
		EMotionAnim	anim_right;
		float		min_angle;
	} turn;
};

#define AA_FLAG_ATTACK_RAT		(1 << 0)			// аттака крыс?
#define AA_FLAG_FIRE_ANYWAY		(1 << 1)			// трассировка не нужна

#define CRITICAL_STAND_TIME 1400
#define TIME_STAND_RECHECK  2000

// Определение времени аттаки по анимации
typedef struct {
	EMotionAnim	anim;				// параметры конкретной анимации 
	u32			anim_i3; 			

	TTime		time_from;			// диапазон времени когда можно наносить hit (от)
	TTime		time_to;		    // диапазон времени когда можно наносить hit (до)

	Fvector		trace_offset;		// направление трассировки
	float		dist;				// дистанция трассировки

	u32			flags;				// специальные флаги

	float		damage;				// урон при данной атаке
	float		dir_yaw;			// угол направления приложения силы к объекту 
	float		dir_pitch;			//  - || -
} SAttackAnimation;


// Motion and animation management
class CMotionManager {

	DEFINE_MAP		(EMotionAnim,		SAnimItem,				ANIM_ITEM_MAP,		ANIM_ITEM_MAP_IT);
	DEFINE_VECTOR	(STransition,		TRANSITION_ANIM_VECTOR, TRANSITION_ANIM_VECTOR_IT);
	DEFINE_MAP		(EAction,			SMotionItem,			MOTION_ITEM_MAP,	MOTION_ITEM_MAP_IT);
	DEFINE_VECTOR	(EMotionAnim,		SEQ_VECTOR,				SEQ_VECTOR_IT);
	DEFINE_VECTOR	(SAttackAnimation,	ATTACK_ANIM,			ATTACK_ANIM_IT);
//	DEFINE_VECTOR	(SJump,				JUMP_VECTOR,			JUMP_VECTOR_IT);

	ANIM_ITEM_MAP			m_tAnims;			// карта анимаций
	MOTION_ITEM_MAP			m_tMotions;			// карта соответсвий EAction к SMotionItem
	TRANSITION_ANIM_VECTOR	m_tTransitions;		// вектор переходов из одной анимации в другую


	CAI_Biting				*pMonster;
	CSkeletonAnimated		*tpKinematics;

	ANIM_VECTOR				m_tHitFXs;
	
	// работа с последовательностями
	SEQ_VECTOR				seq_states;
	SEQ_VECTOR_IT			seq_it;				// итератор (текущий элемент)
	bool					seq_playing;

	EMotionAnim				cur_anim; 
	EMotionAnim				prev_anim; 

	// исправления сосояния 'бега на месте'
	TTime					time_start_stand;
	EAction					prev_action;

	// работа с анимациями атаки
	TTime					aa_time_started;		// время начала анимации	
	TTime					aa_time_last_attack;	// время последнего нанесения хита
	ATTACK_ANIM				aa_all;					// список всех атак
	ATTACK_ANIM				aa_stack;				// список атак для текущей анимации

	u32						spec_params;			// дополнительные параметры

	// ----------------------------------------------

	bool					should_play_die_anim;	// должен отыгрывать анимацию смерти

public:

	EAction					m_tAction;
	CMotionDef				*m_tpCurAnim;

public:
			
				CMotionManager			();
	
	void		Init					(CAI_Biting	*pM, CSkeletonAnimated *tpKin);
	void		Destroy					();
	
	// создание карты анимаций, переходов 
	void		AddAnim					(EMotionAnim ma, LPCTSTR tn, int s_id, float speed, float r_speed, EPState p_s);
	
	
	// добавить анимацию перехода (A - Animation, S - Position)
	void		AddTransition_A2A		(EMotionAnim from,	EMotionAnim to, EMotionAnim trans, bool chain);
	void		AddTransition_A2S		(EMotionAnim from,	EPState to,		EMotionAnim trans, bool chain);
	void		AddTransition_S2A		(EPState from,		EMotionAnim to, EMotionAnim trans, bool chain);
	void		AddTransition_S2S		(EPState from,		EPState to,		EMotionAnim trans, bool chain);
	
	// -------------------------------------

	void		LinkAction				(EAction act, EMotionAnim pmt_motion, EMotionAnim pmt_left, EMotionAnim pmt_right, float pmt_angle);
	void		LinkAction				(EAction act, EMotionAnim pmt_motion);
	
	// -------------------------------------

	void		CheckTransition			(EMotionAnim from, EMotionAnim to);
	void		ApplyParams				();

	// выполнить текущий m_tAction
	void		ProcessAction			();
	// подготовить текущую анимацию на запрос из SelectAnimation
	bool		PrepareAnimation		();

	void		OnAnimationEnd			();
	void		ShowDeath				();

	void		SetSpecParams			(u32 param) {spec_params |= param;}
	void		SetCurAnim				(EMotionAnim a) {cur_anim = a;}
	EMotionAnim	GetCurAnim				() {return  cur_anim;} 

	// работа с последовательностями
	void		Seq_Add					(EMotionAnim a);
	void		Seq_Switch				();					// Перейти в следующее состояние, если такового не имеется - завершить

	// проиграть hit fx
	void		AddHitFX				(LPCTSTR name);
	void		PlayHitFX				(float amount);

private:	
	// загрузка анимаций
	void		Load					(LPCTSTR pmt_name, ANIM_VECTOR	*pMotionVect);

	// работа с последовательностями
	void		Seq_Init				();
	void		Seq_Finish				();

	// работа с анимациями атак
	void		AA_Clear				(); 
	void		AA_SwitchAnimation		(EMotionAnim a, u32 i3);

public:
	void		AA_PushAttackAnim		(SAttackAnimation AttackAnim);
	void		AA_PushAttackAnim		(EMotionAnim a, u32 i3, TTime from, TTime to, Fvector &ray, 
										 float dist, float damage, float yaw, float pitch, u32 flags = 0);
	bool		AA_CheckTime			(TTime cur_time, SAttackAnimation &anim); 
	void		AA_UpdateLastAttack		(TTime cur_time) {aa_time_last_attack = cur_time;}

private:

	// дополнительные функции
	EPState		GetState				(EMotionAnim a);

	void		FixBadState				();
	bool		IsMoving				();

	// sctipting
public:
	void		SCRIPT_SetAnim			(LPCSTR sa);

	// Вызов PrepareAnimation() и установка анимации
	void		ForceAnimSelect			();
};


