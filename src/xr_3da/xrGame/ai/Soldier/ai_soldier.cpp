////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_soldier.cpp
//	Created 	: 25.04.2002
//  Modified 	: 25.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Soldier"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_soldier.h"
#include "..\\ai_monsters_misc.h"
#include "..\\..\\xr_weapon_list.h"
#include "..\\..\\hudmanager.h"

CAI_Soldier::CAI_Soldier()
{
	m_dwHitTime = u32(-1);
	m_tHitDir.set(0,0,1);
	tSavedEnemy = 0;
	tSavedEnemyPosition.set(0,0,0);
	tpSavedEnemyNode = 0;
	dwSavedEnemyNodeID = u32(-1);
	dwLostEnemyTime = 0;
	bBuildPathToLostEnemy = false;
	m_dwLastRangeSearch = 0;
	m_dwLastSuccessfullSearch = 0;
	m_fAggressiveness = ::Random.randF(0,1);
	m_fTimorousness = ::Random.randF(0,1);
	m_bFiring = false;
	m_bLessCoverLook = false;
	q_look.o_look_speed = r_spine_speed = r_torso_speed = PI_DIV_2;
	m_tpCurrentGlobalAnimation = 
	m_tpCurrentTorsoAnimation = 
	m_tpCurrentLegsAnimation = 0;
	m_tpCurrentGlobalBlend = 
	m_tpCurrentTorsoBlend = 
	m_tpCurrentLegsBlend = 0;
	m_bActionStarted = false;
	m_bJumping = false;
	tpaDynamicObjects.clear();
	tpaDynamicSounds.clear();
	m_tpEventSay = Engine.Event.Handler_Attach("level.entity.say",this);
	m_tpEventAssignPath = Engine.Event.Handler_Attach("level.entity.path.assign",this);
	m_dwLastUpdate = 0;
	m_fSensetivity = .0f;
	m_iSoundIndex = -1;
	m_cGestureState = GESTURE_STATE_NONE;
	m_dwLastRadioTalk = 0;
	m_tpSoundBeingPlayed = 0;
	m_dwLastSoundRefresh = 0;
	m_fDistanceWent = 0.f;
	m_cStep = 0;
	AI_Path.fSpeed = 0;
	r_torso_speed = q_look.o_look_speed = PI_DIV_2;
	m_tActionType = ACTION_TYPE_NONE;
	m_tFightType = FIGHT_TYPE_NONE;
	m_dwLastUpdate = 0;
	m_dwCurrentUpdate = Level().timeServer();
	m_dwUpdateCount = 0;
	m_iCurrentSuspiciousNodeIndex = -1;
	m_tpaNodeStack.clear();
}

CAI_Soldier::~CAI_Soldier()
{
	DELETE_SOUNDS(SND_HIT_COUNT,	sndHit);
	DELETE_SOUNDS(SND_DIE_COUNT,	sndDie);
	DELETE_SOUNDS(SND_RADIO_COUNT,	sndRadio);

	Engine.Event.Handler_Detach (m_tpEventSay,this);
	Engine.Event.Handler_Detach (m_tpEventAssignPath,this);
	Msg("FSM report for %s :",cName());
	for (int i=0; i<(int)m_tStateList.size(); i++)
		Msg("%3d %6d",m_tStateList[i].eState,m_tStateList[i].dwTime);
	Msg("Total updates : %d",m_dwUpdateCount);
}

// when soldier is dead
void CAI_Soldier::Die()
{
	// perform death operations
	inherited::Die();
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	vfSetFire(false,Group);
	m_eCurrentState = aiSoldierDie;
	
	// removing from group
	//Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()].Member_Remove(this);
	
	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(XFORM().k,dir,AI_Path.fSpeed);
	
	// Play ref_sound
	::Sound->play_at_pos(sndDie[Random.randI(SND_DIE_COUNT)],this,Position());
}

void CAI_Soldier::vfLoadSelectors(LPCSTR section)
{
	SelectorAttack.Load				(section,"selector_attack");
	SelectorDefend.Load				(section,"selector_defend");
	SelectorFindEnemy.Load			(section,"selector_find_enemy");
	SelectorFollowLeader.Load		(section,"selector_follow_leader");
	SelectorFreeHunting.Load		(section,"selector_free_hutning");
	SelectorMoreDeadThanAlive.Load	(section,"selector_mtda");
	SelectorNoWeapon.Load			(section,"selector_no_weapon");
	SelectorPatrol.Load				(section,"selector_patrol");
	SelectorPursuit.Load			(section,"selector_pursuit");
	SelectorReload.Load				(section,"selector_reload");
	SelectorRetreat.Load			(section,"selector_retreat");
	SelectorSenseSomething.Load		(section,"selector_sense_something");
	SelectorUnderFireCover.Load		(section,"selector_under_fire_cover");
	SelectorUnderFireLine.Load		(section,"selector_under_fire_line");
}

void CAI_Soldier::Load				(LPCSTR section)
{ 
	// load parameters from ".ini" file
	inherited::Load					(section);
	
	// initialize start position
	Fvector							P = Position();
	P.x								+= ::Random.randF();
	P.z								+= ::Random.randF();
	
	vfLoadSounds();
	vfLoadSelectors(section);
	
	// visibility
	m_dwMovementIdleTime			= pSettings->r_s32(section,"MovementIdleTime");
	m_fMaxInvisibleSpeed			= pSettings->r_float(section,"MaxInvisibleSpeed");
	m_fMaxViewableSpeed				= pSettings->r_float(section,"MaxViewableSpeed");
	m_fMovementSpeedWeight			= pSettings->r_float(section,"MovementSpeedWeight");
	m_fDistanceWeight				= pSettings->r_float(section,"DistanceWeight");
	m_fSpeedWeight					= pSettings->r_float(section,"SpeedWeight");
	m_fCrouchVisibilityMultiplier	= pSettings->r_float(section,"CrouchVisibilityMultiplier");
	m_fLieVisibilityMultiplier		= pSettings->r_float(section,"LieVisibilityMultiplier");
	m_fVisibilityThreshold			= pSettings->r_float(section,"VisibilityThreshold");
	m_fLateralMultiplier			= pSettings->r_float(section,"LateralMultiplier");
	m_fShadowWeight					= pSettings->r_float(section,"ShadowWeight");
	
	//fire
	m_dwFireRandomMin				= pSettings->r_s32(section,"FireRandomMin");
	m_dwFireRandomMax				= pSettings->r_s32(section,"FireRandomMax");
	m_dwNoFireTimeMin				= pSettings->r_s32(section,"NoFireTimeMin");
	m_dwNoFireTimeMax				= pSettings->r_s32(section,"NoFireTimeMax");
	m_fMinMissDistance				= pSettings->r_float(section,"MinMissDistance");
	m_fMinMissFactor				= pSettings->r_float(section,"MinMissFactor");
	m_fMaxMissDistance				= pSettings->r_float(section,"MaxMissDistance");
	m_fMaxMissFactor				= pSettings->r_float(section,"MaxMissFactor");

	m_fMinRadioIinterval			= pSettings->r_float(section,"MinRadioIinterval");
	m_fMaxRadioIinterval			= pSettings->r_float(section,"MaxRadioIinterval");
	m_fRadioRefreshRate				= pSettings->r_float(section,"RadioRefreshRate");
	
	m_dwMaxDynamicObjectsCount		= _min(pSettings->r_s32(section,"DynamicObjectsCount"),MAX_DYNAMIC_OBJECTS);
	m_dwMaxDynamicSoundsCount		= _min(pSettings->r_s32(section,"DynamicSoundsCount"),MAX_DYNAMIC_SOUNDS);
}

BOOL CAI_Soldier::net_Spawn	(LPVOID DC)
{
	if (!inherited::net_Spawn(DC))	return FALSE;

	//tSavedEnemyPosition = Position();
	CSE_Abstract			*e	= (CSE_Abstract*)(DC);
	CSE_ALifeHumanAbstract						*O = dynamic_cast<CSE_ALifeHumanAbstract*>(e);
	r_torso_current.yaw				= r_torso_target.yaw	= -O->o_Angle.y;
	r_torso_current.pitch			= r_torso_target.pitch	= 0;

	m_tStateStack.push(m_eCurrentState = aiSoldierLookingOver);
	vfAddStateToList(m_eCurrentState);
	return TRUE;
}

// soldier update
void CAI_Soldier::shedule_Update(u32 DT)
{
	inherited::shedule_Update(DT);
	vfUpdateSounds(DT);
}

void CAI_Soldier::Exec_Movement	( float dt )
{
	if (_abs(Position().x) > 10000.f) {
		if (ps_Size() > 1)
			Position() = ps_Element(ps_Size() - 2).vPosition;
		Msg("%s",cName());
	}
	AI_Path.Calculate(this,Position(),Position(),m_fCurSpeed,dt);
	/**
	if (m_eCurrentState != aiSoldierJumping)
		AI_Path.Calculate(this,Position(),Position(),m_fCurSpeed,dt);
	else {
		UpdateTransform();
		if (m_bActionStarted) {
			m_bActionStarted = false;
			Fvector tAcceleration, tVelocity;
			tVelocity.set(0,1,0);
			Movement.SetPosition(Position());
			Movement.SetVelocity(tVelocity);
			tAcceleration.set(0,0,0);
			Movement.SetPosition(Position());
			Movement.Calculate	(tAcceleration,0,m_cBodyState == BODY_STATE_STAND ? m_fJumpSpeed : m_fJumpSpeed*.8f,dt > .1f ? .1f : dt,false);
			Movement.GetPosition(Position());
		}
		else {
			Fvector tAcceleration;
			tAcceleration.set(0,m_cBodyState == BODY_STATE_STAND ? m_fJumpSpeed : m_fJumpSpeed*.8f,0);
			Movement.SetPosition(Position());
			Movement.Calculate	(tAcceleration,0,0,dt > .1f ? .1f : dt,false);
			Movement.GetPosition(Position());
		}
		UpdateTransform	();
	}
	/**/
}

void CAI_Soldier::OnEvent(EVENT E, u64 P1, u64 P2)
{

	if (E == m_tpEventSay) {
		if (0==P2 || (u64)(this)==P2) {
			char* caTextToShow;
			caTextToShow = (char *)(P1);
			HUD().outMessage(0xffffffff,cName(),"%s",caTextToShow);
		}
	}
	else
		if (E == m_tpEventAssignPath) {
			if (0==P2 || u64(this)==P2) {
				char *buf2, *buf, monster_name[100];
				buf2 = buf = (char *)P1;
				int iArgCount = _GetItemCount(buf2);
				if (iArgCount >= 4) {
					u32 team,squad,group;
					for ( ; ; buf2++)
						if (*buf2 == ',') {
							Memory.mem_copy(monster_name,buf,((u32)(buf2 - buf))*sizeof(char));
							monster_name[buf2++ - buf] = 0;
							break;
						}
						
					if ((xr_strlen(monster_name)) && (strcmp(monster_name,cName())))
						return;
					
					if (!(xr_strlen(monster_name))) {
						sscanf(buf2,"%d,%d,%d",&team,&squad,&group);
						
						if (((int)team != g_Team()) || ((int)squad != g_Squad()) || ((int)group != g_Group()))
							return;
						
						for (int komas=0; komas<3; buf2++)
							if (*buf2 == ',')
								komas++;
					}
				}
				
				INIT_SQUAD_AND_LEADER;
				CGroup &Group = Squad.Groups[g_Group()];
				
				m_tpPath = 0;
				CCustomMonster *tpCustomMonster = dynamic_cast<CCustomMonster *>(Leader);
				m_dwLoopCount = 0;
				if (tpCustomMonster && (tpCustomMonster->m_tpPath))
					m_tpPath = tpCustomMonster->m_tpPath;
				else {
					for (int i=0; i<(int)Group.Members.size(); i++) {
						CCustomMonster *tpCustomMonster = dynamic_cast<CCustomMonster *>(Group.Members[i]);
						if (tpCustomMonster && (tpCustomMonster->m_tpPath)) {
							m_tpPath = tpCustomMonster->m_tpPath;
							break;
						}
					}
					if (!m_tpPath) {
						for (int i=0, iCount = 1; buf2[i]; i++)
							if (buf2[i] == ',')
								iCount++;
						if (iCount == 1) {
							m_tpPath = &(Level().m_PatrolPaths[buf2]);
							if (!m_tpPath) {
								Debug.fatal("Cannot find specified path (%s)",buf2);
							}
						}
						else {
							iCount = ::Random.randI(0,iCount);
							for (int i=0, iCountX = 0; buf2[i]; i++) {
								if (iCountX == iCount) {
									int j=i;
									for (; buf2[i]; i++)
										if (buf2[i] == ',') {
											buf2[i] = 0;
											break;
										}
									buf2 += j;
									m_tpPath = &(Level().m_PatrolPaths[buf2]);
									if (!m_tpPath) {
										Debug.fatal("Cannot find specified path (%s)",buf2);
									}
									break;
								}
								if (buf2[i] == ',')
									iCountX++;
							}
						}
					}
				}
				if (!m_tpPath) {
					Debug.fatal("Cannot find specified path");
				}
			}
		}
}