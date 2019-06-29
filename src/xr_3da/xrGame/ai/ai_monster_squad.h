#pragma once 
class CEntity;

//////////////////////////////////////////////////////////////////////////
// Member local goal notification
//////////////////////////////////////////////////////////////////////////
enum EMemberGoalType {
	MG_AttackEnemy,				// entity
	MG_PanicFromEnemy,			// entity
	MG_InterestingSound,		// position
	MG_DangerousSound,			// position
	MG_WalkGraph,				// node
	MG_Rest,					// node, position
	MG_None,
};

struct SMemberGoal {
	EMemberGoalType		type;
	CEntity				*entity;
	Fvector				position;
	u32					node;
};

//////////////////////////////////////////////////////////////////////////
// Squad command 
//////////////////////////////////////////////////////////////////////////
enum ESquadCommandType { 
	SC_EXPLORE,
	SC_ATTACK,
	SC_THREATEN,
	SC_COVER,
	SC_FOLLOW,
	SC_FEEL_DANGER,
	SC_EXPLICIT_ACTION,
	SC_REST,
	SC_NONE,
};

struct SSquadCommand {
	ESquadCommandType	type;	// тип команды

	CEntity		*entity;
	Fvector		position;
	u32			node;
	Fvector		direction;

};


/////////////////////////////////////////////////////////////////////////////////////////
// MonsterSquad Class
class CMonsterSquad {
	CEntity				*leader;

	DEFINE_MAP		(CEntity*, SMemberGoal,		MEMBER_GOAL_MAP,	MEMBER_GOAL_MAP_IT);
	DEFINE_MAP		(CEntity*, SSquadCommand,	MEMBER_COMMAND_MAP, MEMBER_COMMAND_MAP_IT);

	// карта целей членов группы (обновляется со стороны объекта)
	MEMBER_GOAL_MAP		m_goals;

	// карта комманд членов группы (обновляется со стороны squad manager)
	MEMBER_COMMAND_MAP	m_commands;



public:
	
				CMonsterSquad		() : leader(0) {}
				~CMonsterSquad		() {}

	// -----------------------------------------------------------------

	void		RegisterMember		(CEntity *pE);
	void		RemoveMember		(CEntity *pE);

	bool		SquadActive			();

	// -----------------------------------------------------------------

	void			SetLeader			(CEntity *pE) {leader = pE;}
	CEntity			*GetLeader			() {return leader;}

	// -----------------------------------------------------------------
	
	void			UpdateGoal			(CEntity *pE, const SMemberGoal	&goal);
	void			UpdateCommand		(CEntity *pE, const SSquadCommand &com);

	
	void			GetGoal				(CEntity *pE, SMemberGoal &goal);
	void			GetCommand			(CEntity *pE, SSquadCommand &com);
	SMemberGoal		&GetGoal			(CEntity *pE);
	SSquadCommand	&GetCommand			(CEntity *pE);

	// -----------------------------------------------------------------
	
	
	void		UpdateSquadCommands	();	
	
	///////////////////////////////////////////////////////////////////////////////////////
	//  Общие данные
	//////////////////////////////////////////////////////////////////////////////////////
	
	DEFINE_VECTOR	(CEntity*, ENTITY_VEC,	ENTITY_VEC_IT);	
	ENTITY_VEC		m_temp_entities;
	
	///////////////////////////////////////////////////////////////////////////////////////
	//  Атака группой монстров
	//////////////////////////////////////////////////////////////////////////////////////
	
	DEFINE_MAP		(CEntity*, ENTITY_VEC,	ENEMY_MAP, ENEMY_MAP_IT);
	
	ENEMY_MAP		m_enemy_map;

	void			ProcessAttack					();
	
	
	// -- Temp -- 
	struct _elem {
		CEntity		*pE;
		Fvector		p_from;
		float		yaw;
	};
	xr_vector<_elem>	lines;
	// ------------

	void			Attack_AssignTargetDir			(ENTITY_VEC &members, CEntity *enemy);

	////////////////////////////////////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////////////////////////////////
	//  групповой idle
	//////////////////////////////////////////////////////////////////////////////////////
	ENTITY_VEC		front, back, left, right;
	
	void			ProcessIdle				();
	void			Idle_AssignAction		(ENTITY_VEC &members);

	////////////////////////////////////////////////////////////////////////////////////////

};
