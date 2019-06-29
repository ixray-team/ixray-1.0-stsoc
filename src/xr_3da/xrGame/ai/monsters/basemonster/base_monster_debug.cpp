#include "stdafx.h"
#include "base_monster.h"
#include "../../../level.h"
#include "../../../level_debug.h"
#include "../../../entitycondition.h"
#include "../../../ai_debug.h"
#include "../state_defs.h"
#include "../state_manager.h"
#include "../../../phmovementcontrol.h"
#include "../../../characterphysicssupport.h"
#include "../../../actor.h"

#ifdef DEBUG
CBaseMonster::SDebugInfo CBaseMonster::show_debug_info()
{
	if (!g_Alive()) return SDebugInfo();

	if (m_show_debug_info == 0) {
		DBG().text(this).clear();
		return SDebugInfo();
	}

	float y				= 200;
	float x				= (m_show_debug_info == 1) ? 40.f : float(::Render->getTarget()->get_width() / 2) + 40.f;
	const float delta_y	= 12;

	string256	text;

	u32			color			= D3DCOLOR_XRGB(0,255,0);
	u32			delimiter_color	= D3DCOLOR_XRGB(0,0,255);

	DBG().text(this).clear	 ();
	DBG().text(this).add_item("---------------------------------------", x, y+=delta_y, delimiter_color);

	sprintf(text, "-- Monster : [%s]  Current Time = [%u]", *cName(), Device.dwTimeGlobal);
	DBG().text(this).add_item(text, x, y+=delta_y, color);
	DBG().text(this).add_item("-----------   PROPERTIES   ------------", x, y+=delta_y, delimiter_color);

	sprintf(text, "Health = [%f]", conditions().GetHealth());
	DBG().text(this).add_item(text,										 x, y+=delta_y, color);

	sprintf(text, "Morale = [%f]", Morale.get_morale());
	DBG().text(this).add_item(text,										 x, y+=delta_y, color);


	DBG().text(this).add_item("-----------   MEMORY   ----------------", x, y+=delta_y, delimiter_color);

	if (EnemyMan.get_enemy()) {
		sprintf(text, "Current Enemy = [%s]", *EnemyMan.get_enemy()->cName());
	} else 
		sprintf(text, "Current Enemy = [NONE]");
	DBG().text(this).add_item(text,										 x, y+=delta_y, color);
	
	if (EnemyMan.get_enemy()) {
		sprintf(text, "SeeEnemy[%u] EnemySeeMe[%u] TimeLastSeen[%u]", EnemyMan.see_enemy_now(),EnemyMan.enemy_see_me_now(),EnemyMan.get_enemy_time_last_seen());
		DBG().text(this).add_item(text,									x, y+=delta_y, color);
	}

	if (CorpseMan.get_corpse()) {
		sprintf(text, "Current Corpse = [%s] Satiety = [%.2f]", *CorpseMan.get_corpse()->cName(), GetSatiety());
	} else 
		sprintf(text, "Current Corpse = [NONE] Satiety = [%.2f]", GetSatiety());

	DBG().text(this).add_item(text,										 x, y+=delta_y, color);

	// Sound
	if (SoundMemory.IsRememberSound()) {
		SoundElem	sound_elem;
		bool		dangerous_sound;
		SoundMemory.GetSound(sound_elem, dangerous_sound);

		string128	s_type;

		switch(sound_elem.type){
			case WEAPON_SHOOTING:			strcpy(s_type,"WEAPON_SHOOTING"); break;
			case MONSTER_ATTACKING:			strcpy(s_type,"MONSTER_ATTACKING"); break;
			case WEAPON_BULLET_RICOCHET:	strcpy(s_type,"WEAPON_BULLET_RICOCHET"); break;
			case WEAPON_RECHARGING:			strcpy(s_type,"WEAPON_RECHARGING"); break;

			case WEAPON_TAKING:				strcpy(s_type,"WEAPON_TAKING"); break;
			case WEAPON_HIDING:				strcpy(s_type,"WEAPON_HIDING"); break;
			case WEAPON_CHANGING:			strcpy(s_type,"WEAPON_CHANGING"); break;
			case WEAPON_EMPTY_CLICKING:		strcpy(s_type,"WEAPON_EMPTY_CLICKING"); break;

			case MONSTER_DYING:				strcpy(s_type,"MONSTER_DYING"); break;
			case MONSTER_INJURING:			strcpy(s_type,"MONSTER_INJURING"); break;
			case MONSTER_WALKING:			strcpy(s_type,"MONSTER_WALKING"); break;
			case MONSTER_JUMPING:			strcpy(s_type,"MONSTER_JUMPING"); break;
			case MONSTER_FALLING:			strcpy(s_type,"MONSTER_FALLING"); break;
			case MONSTER_TALKING:			strcpy(s_type,"MONSTER_TALKING"); break;

			case DOOR_OPENING:				strcpy(s_type,"DOOR_OPENING"); break;
			case DOOR_CLOSING:				strcpy(s_type,"DOOR_CLOSING"); break;
			case OBJECT_BREAKING:			strcpy(s_type,"OBJECT_BREAKING"); break;
			case OBJECT_FALLING:			strcpy(s_type,"OBJECT_FALLING"); break;
			case NONE_DANGEROUS_SOUND:		strcpy(s_type,"NONE_DANGEROUS_SOUND"); break;
		}

		if (sound_elem.who)
			sprintf(text,"Sound: type[%s] time[%u] power[%.3f] val[%i] src[+]", s_type, sound_elem.time, sound_elem.power, sound_elem.value);
		else 
			sprintf(text,"Sound: type[%s] time[%u] power[%.3f] val[%i] src[?]", s_type, sound_elem.time, sound_elem.power, sound_elem.value);


	} else 
		sprintf(text, "Sound: NONE");

	DBG().text(this).add_item(text,										 x, y+=delta_y, color);

	// Hit
	if (HitMemory.is_hit()) {
		if (HitMemory.get_last_hit_object()) {
			sprintf(text,"Hit Info: object=[%s] time=[%u]", *(HitMemory.get_last_hit_object()->cName()), HitMemory.get_last_hit_time());
		} else {
			sprintf(text,"Hit Info: object=[NONE] time=[%u]", HitMemory.get_last_hit_time());
		}
	} else 
		sprintf(text, "Hit Info: NONE");

	DBG().text(this).add_item(text,										 x, y+=delta_y, color);

	DBG().text(this).add_item("-----------   MOVEMENT   ------------", x, y+=delta_y, delimiter_color);

	sprintf(text, "Actual = [%u] Enabled = [%u]",			 control().path_builder().actual(), control().path_builder().enabled());
	DBG().text(this).add_item(text,										x, y+=delta_y, color);
	
	sprintf(text, "Speed: Linear = [%.3f] Angular = [%.3f]", control().movement().velocity_current(), 0.f);
	DBG().text(this).add_item(text,										x, y+=delta_y, color);
	
	DBG().text(this).add_item("------- Attack Distances -------------", x, y+=delta_y, delimiter_color);
	sprintf(text, "MinDist[%.3f] MaxDist[%.3f] As_Step[%.3f] As_MinDist[%.3f]", 
		MeleeChecker.get_min_distance(),
		MeleeChecker.get_max_distance(),
		MeleeChecker.dbg_as_step(),
		MeleeChecker.dbg_as_min_dist()
	);
	DBG().text(this).add_item(text,										x, y+=delta_y, color);


	if (EnemyMan.get_enemy()) {
		sprintf(text, "Current Enemy = [%s]", *EnemyMan.get_enemy()->cName());
	} else 
		sprintf(text, "Current Enemy = [NONE]");
	DBG().text(this).add_item(text,										 x, y+=delta_y, color);



	return SDebugInfo(x, y, delta_y, color, delimiter_color);
}

void CBaseMonster::debug_fsm()
{
	if (!g_Alive()) return;

	if (!psAI_Flags.test(aiMonsterDebug)) {
		DBG().object_info(this,this).clear ();
		return;
	}
	
	EMonsterState state = StateMan->get_state_type();
	
	string128 st;

	switch (state) {
		case eStateRest_WalkGraphPoint:					sprintf(st,"Rest :: Walk Graph");			break;
		case eStateRest_Idle:							sprintf(st,"Rest :: Idle");					break;
		case eStateRest_Fun:							sprintf(st,"Rest :: Fun");					break;
		case eStateRest_Sleep:							sprintf(st,"Rest :: Sleep");				break;
		case eStateRest_MoveToHomePoint:				sprintf(st,"Rest :: MoveToHomePoint");		break;
		case eStateRest_WalkToCover:					sprintf(st,"Rest :: WalkToCover");			break;
		case eStateRest_LookOpenPlace:					sprintf(st,"Rest :: LookOpenPlace");		break;

		case eStateEat_CorpseApproachRun:				sprintf(st,"Eat :: Corpse Approach Run");	break;
		case eStateEat_CorpseApproachWalk:				sprintf(st,"Eat :: Corpse Approach Walk");	break;
		case eStateEat_CheckCorpse:						sprintf(st,"Eat :: Check Corpse");			break;
		case eStateEat_Eat:								sprintf(st,"Eat :: Eating");				break;
		case eStateEat_WalkAway:						sprintf(st,"Eat :: Walk Away");				break;
		case eStateEat_Rest:							sprintf(st,"Eat :: Rest After Meal");		break;
		case eStateEat_Drag:							sprintf(st,"Eat :: Drag");					break;
		
		case eStateAttack_Run:							sprintf(st,"Attack :: Run");				break;
		case eStateAttack_Melee:						sprintf(st,"Attack :: Melee");				break;
		case eStateAttack_RunAttack:					sprintf(st,"Attack :: Run Attack");			break;
		case eStateAttack_RunAway:						sprintf(st,"Attack :: Run Away");			break;
		case eStateAttack_FindEnemy:					sprintf(st,"Attack :: Find Enemy");			break;
		case eStateAttack_Steal:						sprintf(st,"Attack :: Steal");				break;
		case eStateAttack_AttackHidden:					sprintf(st,"Attack :: Attack Hidden");		break;
		
		case eStateAttackCamp_Hide:						sprintf(st,"Attack Camp:: Hide");			break;
		case eStateAttackCamp_Camp:						sprintf(st,"Attack Camp:: Camp");			break;
		case eStateAttackCamp_StealOut:					sprintf(st,"Attack Camp:: Steal Out");		break;

		case eStateAttack_HideInCover:					sprintf(st,"Attack :: Hide In Cover");		break;
		case eStateAttack_MoveOut:						sprintf(st,"Attack :: Move Out From Cover");break;
		case eStateAttack_CampInCover:					sprintf(st,"Attack :: Camp In Cover");		break;

		case eStateAttack_Psy:							sprintf(st,"Attack :: Psy");				break;
		case eStateAttack_MoveToHomePoint:				sprintf(st,"Attack :: Move To Home Point");	break;
		case eStateAttack_HomePoint_Hide:				sprintf(st,"Attack :: Home Point :: Hide");	break;
		case eStateAttack_HomePoint_Camp:				sprintf(st,"Attack :: Home Point :: Camp");	break;
		case eStateAttack_HomePoint_LookOpenPlace:		sprintf(st,"Attack :: Home Point :: Look Open Place");	break;
		
		case eStatePanic_Run:							sprintf(st,"Panic :: Run Away");				break;
		case eStatePanic_FaceUnprotectedArea:			sprintf(st,"Panic :: Face Unprotected Area");	break;
		case eStatePanic_HomePoint_Hide:				sprintf(st,"Panic :: Home Point :: Hide");		break;
		case eStatePanic_HomePoint_LookOpenPlace:		sprintf(st,"Panic :: Home Point :: Look Open Place");	break;
		case eStatePanic_HomePoint_Camp:				sprintf(st,"Panic :: Home Point :: Camp");		break;

		case eStateHitted_Hide:							sprintf(st,"Hitted :: Hide");					break;
		case eStateHitted_MoveOut:						sprintf(st,"Hitted :: MoveOut");				break;
		case eStateHitted_Home:							sprintf(st,"Hitted :: Home");				break;

		case eStateHearDangerousSound_Hide:				sprintf(st,"Dangerous Snd :: Hide");			break;
		case eStateHearDangerousSound_FaceOpenPlace:	sprintf(st,"Dangerous Snd :: FaceOpenPlace");	break;
		case eStateHearDangerousSound_StandScared:		sprintf(st,"Dangerous Snd :: StandScared");		break;
		case eStateHearDangerousSound_Home:				sprintf(st,"Dangerous Snd :: Home");			break;

		case eStateHearInterestingSound_MoveToDest:		sprintf(st,"Interesting Snd :: MoveToDest");	break;
		case eStateHearInterestingSound_LookAround:		sprintf(st,"Interesting Snd :: LookAround");	break;
		
		case eStateHearHelpSound:						sprintf(st,"Hear Help Sound");	break;
		case eStateHearHelpSound_MoveToDest:			sprintf(st,"Hear Help Sound :: MoveToDest");	break;
		case eStateHearHelpSound_LookAround:			sprintf(st,"Hear Help Sound :: LookAround");	break;

		case eStateControlled_Follow_Wait:				sprintf(st,"Controlled :: Follow : Wait");			break;
		case eStateControlled_Follow_WalkToObject:		sprintf(st,"Controlled :: Follow : WalkToObject");	break;
		case eStateControlled_Attack:					sprintf(st,"Controlled :: Attack");					break;
		case eStateThreaten:							sprintf(st,"Threaten :: ");							break;
		case eStateFindEnemy_Run:						sprintf(st,"Find Enemy :: Run");							break;
		case eStateFindEnemy_LookAround_MoveToPoint:	sprintf(st,"Find Enemy :: Look Around : Move To Point");	break;
		case eStateFindEnemy_LookAround_LookAround:		sprintf(st,"Find Enemy :: Look Around : Look Around");		break;
		case eStateFindEnemy_LookAround_TurnToPoint:	sprintf(st,"Find Enemy :: Look Around : Turn To Point");	break;
		case eStateFindEnemy_Angry:						sprintf(st,"Find Enemy :: Angry");							break;
		case eStateFindEnemy_WalkAround:				sprintf(st,"Find Enemy :: Walk Around");					break;
		case eStateSquad_Rest_Idle:						sprintf(st,"Squad :: Rest : Idle");					break;
		case eStateSquad_Rest_WalkAroundLeader:			sprintf(st,"Squad :: Rest : WalkAroundLeader");		break;
		case eStateSquad_RestFollow_Idle:				sprintf(st,"Squad :: Follow Leader : Idle");		break;
		case eStateSquad_RestFollow_WalkToPoint:		sprintf(st,"Squad :: Follow Leader : WalkToPoint");	break;
		case eStateCustom_Vampire:						sprintf(st,"Attack :: Vampire");					break;
		case eStateVampire_ApproachEnemy:				sprintf(st,"Vampire :: Approach to enemy");			break;
		case eStateVampire_Execute:						sprintf(st,"Vampire :: Hit");						break;
		case eStateVampire_RunAway:						sprintf(st,"Vampire :: Run Away");					break;
		case eStateVampire_Hide:						sprintf(st,"Vampire :: Hide");						break;
		case eStatePredator:							sprintf(st,"Predator");								break;
		case eStatePredator_MoveToCover:				sprintf(st,"Predator :: MoveToCover");				break;
		case eStatePredator_LookOpenPlace:				sprintf(st,"Predator :: Look Open Place");			break;
		case eStatePredator_Camp:						sprintf(st,"Predator :: Camp");						break;
		case eStateBurerAttack_Tele:					sprintf(st,"Attack :: Telekinesis");			break;
		case eStateBurerAttack_Gravi:					sprintf(st,"Attack :: Gravi Wave");				break;
		case eStateBurerAttack_RunAround:				sprintf(st,"Attack :: Run Around");			break;
		case eStateBurerAttack_FaceEnemy:				sprintf(st,"Attack :: Face Enemy");			break;
		case eStateBurerAttack_Melee:					sprintf(st,"Attack :: Melee");				break;
		case eStateBurerScanning:						sprintf(st,"Attack :: Scanning");			break;
		case eStateCustomMoveToRestrictor:				sprintf(st,"Moving To Restrictor :: Position not accessible");	break;
		case eStateSmartTerrainTask:					sprintf(st,"ALIFE");	break;
		case eStateSmartTerrainTaskGamePathWalk:		sprintf(st,"ALIFE :: Game Path Walk");	break;
		case eStateSmartTerrainTaskLevelPathWalk:		sprintf(st,"ALIFE :: Level Path Walk");	break;
		case eStateSmartTerrainTaskWaitCapture:			sprintf(st,"ALIFE :: Wait till smart terrain will capture me");	break;
		case eStateUnknown:								sprintf(st,"Unknown State :: ");			break;
		default:										sprintf(st,"Undefined State ::");			break;
	}
	
	DBG().object_info(this,this).remove_item (u32(0));
	DBG().object_info(this,this).remove_item (u32(1));
	DBG().object_info(this,this).remove_item (u32(2));

	DBG().object_info(this,this).add_item	 (*cName(), D3DCOLOR_XRGB(255,0,0), 0);
	DBG().object_info(this,this).add_item	 (st, D3DCOLOR_XRGB(255,0,0), 1);
	
	sprintf(st, "Team[%u]Squad[%u]Group[%u]", g_Team(), g_Squad(), g_Group());
	DBG().object_info(this,this).add_item	 (st, D3DCOLOR_XRGB(255,0,0), 2);

	CEntityAlive *entity = smart_cast<CEntityAlive *>(Level().CurrentEntity());
	if (entity && entity->character_physics_support()->movement()) {
		sprintf(st,"VELOCITY [%f,%f,%f] Value[%f]",VPUSH(entity->character_physics_support()->movement()->GetVelocity()),entity->character_physics_support()->movement()->GetVelocityActual());
		DBG().text(this).clear();
		DBG().text(this).add_item(st,200,100,COLOR_GREEN,100);
	}
}


#endif
