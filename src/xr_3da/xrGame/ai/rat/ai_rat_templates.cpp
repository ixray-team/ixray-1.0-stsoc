////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_rat_templates.cpp
//	Created 	: 23.07.2002
//  Modified 	: 07.11.2002
//	Author		: Dmitriy Iassenev
//	Description : Templates for monster "Rat"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_rat.h"
#include "../ai_monsters_misc.h"
#include "../../game_graph.h"
#include "../../magic_box3.h"
#include "../../clsid_game.h"
#include "../../../fbasicvisual.h"
#include "../../ai_object_location.h"
#include "../../level_graph.h"
#include "../../movement_manager.h"
#include "../../location_manager.h"
#include "../../level.h"
#include "../../random32.h"
#include "../../ai_space.h"

IC bool CAI_Rat::bfCheckIfOutsideAIMap(Fvector &tTemp1)
{
	u32 dwNewNode = ai_location().level_vertex_id();
	const CLevelGraph::CVertex *tpNewNode = ai_location().level_vertex();
	CLevelGraph::CPosition	QueryPos;
	if (!ai().level_graph().valid_vertex_position(tTemp1))
		return	(false);
	ai().level_graph().vertex_position(QueryPos,tTemp1);
	if (!ai().level_graph().valid_vertex_id(dwNewNode) || !ai().level_graph().inside(*ai_location().level_vertex(),QueryPos)) {
		dwNewNode = ai().level_graph().vertex(ai_location().level_vertex_id(),tTemp1);
		tpNewNode = ai().level_graph().vertex(dwNewNode);
	}
	return(!ai().level_graph().valid_vertex_id(dwNewNode) || !ai().level_graph().inside(*tpNewNode,QueryPos));
};

void CAI_Rat::vfSetFire(bool bFire)
{
	if (bFire) {
		m_bFiring = true;
		m_tAction = eRatActionAttackBegin;
	}
	else {
		m_bFiring = false;
		m_tAction = eRatActionAttackEnd;
	}
}

void CAI_Rat::vfSetMovementType(float fSpeed)
{
//	StandUp();
	m_bMoving = _abs(fSpeed) > EPS_L;
	m_fSpeed = m_fCurSpeed = fSpeed;
}

void CAI_Rat::vfAdjustSpeed()
{
	Fvector					tTemp1, tTemp2;
	tTemp1.sub				(m_tGoalDir,Position());
	tTemp1.normalize_safe	();
	float					y,p;
	tTemp1.getHP			(y,p);
	tTemp2					= XFORM().k;
	tTemp2.normalize_safe	();
	float					fAngle = tTemp1.dotproduct(tTemp2);
	clamp					(fAngle,-.99999f,.99999f);
	fAngle					= acosf(fAngle);
	
	if (_abs(m_fSpeed - m_fMinSpeed) <= EPS_L)	{
		if (fAngle >= 2*PI_DIV_3) {
			m_fSpeed = 0;
			m_fASpeed = m_fNullASpeed;
			movement().m_body.target.yaw = -y;
		}
		else 
		{
			m_fSpeed = m_fMinSpeed;
			m_fASpeed = m_fMinASpeed;
		}
	}
	else
		if (_abs(m_fSpeed - m_fMaxSpeed) <= EPS_L)	{
			if (fAngle >= 2*PI_DIV_3) {
				m_fSpeed = 0;
				m_fASpeed = m_fNullASpeed;
				movement().m_body.target.yaw = -y;
			}
			else
				if (fAngle >= PI_DIV_2) {
					m_fSpeed = m_fMinSpeed;
					m_fASpeed = m_fMinASpeed;
				}
				else {
					m_fSpeed = m_fMaxSpeed;
					m_fASpeed = m_fMaxASpeed;
				}
		}
		else
			if (_abs(m_fSpeed - m_fAttackSpeed) <= EPS_L)	{
//				if (fAngle >= 2*PI_DIV_3) {
//					m_fSpeed = 0;
//					m_fASpeed = m_fNullASpeed;
//					movement().m_body.target.yaw = -y;
//				}
//				else
					if (fAngle >= PI_DIV_2) {
						m_fSpeed = m_fMinSpeed;
						m_fASpeed = m_fMinASpeed;
					}
					else
						if (fAngle >= PI_DIV_4) {
							m_fSpeed = m_fMaxSpeed;
							m_fASpeed = m_fMaxASpeed;
						}
						else {
							m_fSpeed = m_fAttackSpeed;
							m_fASpeed = m_fAttackASpeed;
						}
			}
			else {
				movement().m_body.target.yaw = -y;
				m_fSpeed = 0;
				m_fASpeed = m_fNullASpeed;
			}
	
	tTemp2 = XFORM().k;
	tTemp2.normalize_safe();
	
	tTemp1 = Position();
	tTemp1.mad(tTemp2,1*m_fSpeed*m_fTimeUpdateDelta);
	if (bfCheckIfOutsideAIMap(tTemp1)) {
		tTemp1 = Position();
		if (_abs(m_fSpeed - m_fAttackSpeed) < EPS_L) {
			tTemp1.mad(tTemp2,1*m_fMaxSpeed*m_fTimeUpdateDelta);
			if (bfCheckIfOutsideAIMap(tTemp1)) {
				m_fSpeed = m_fMinSpeed;
				m_fASpeed = m_fMinASpeed;
			}
			else {
				m_fSpeed = m_fMaxSpeed;
				m_fASpeed = m_fMaxASpeed;
			}
		}
		else 
		{
			m_fSpeed = m_fMinSpeed;
			m_fASpeed = m_fMinASpeed;
		}
	}
}

void CAI_Rat::make_turn()
{
	m_fSpeed			= m_fCurSpeed = 0.f;
	if (m_bFiring && (angle_difference(movement().m_body.target.yaw,movement().m_body.current.yaw) < PI_DIV_6)) {
//		movement().m_body.speed	= 0.f;
		return;
	}

//	Msg					("%6d : Rat %s, %f -> %f [%f]",Device.dwTimeGlobal,*cName(),movement().m_body.current.pitch,movement().m_body.target.pitch,get_custom_pitch_speed(0.f));

	m_turning			= true;
	movement().m_body.speed		= PI_MUL_2;

	Fvector				tSavedPosition = Position();
	m_tHPB.x			= -movement().m_body.current.yaw;
	m_tHPB.y			= -movement().m_body.current.pitch;

	XFORM().setHPB		(m_tHPB.x,m_tHPB.y,0.f);//m_tHPB.z);
	Position()			= tSavedPosition;
}

void CAI_Rat::vfComputeNewPosition(bool bCanAdjustSpeed, bool bStraightForward)
{
	// saving current parameters
	m_bCanAdjustSpeed	= bCanAdjustSpeed;
	m_bStraightForward	= bStraightForward;
	if (m_thinking)
		return;

//	Msg					("RAT : %6d : %f -> %f [%f][%f][%f]",Device.dwTimeGlobal,movement().m_body.current.yaw,movement().m_body.target.yaw,VPUSH(m_tGoalDir));
	Fvector tSafeHPB = m_tHPB;
	Fvector tSavedPosition = Position();
	SRotation tSavedTorsoTarget = movement().m_body.target;
	float fSavedDHeading = m_fDHeading;

	if (bCanAdjustSpeed)
		vfAdjustSpeed();

	if ((angle_difference(movement().m_body.target.yaw, movement().m_body.current.yaw) > PI_DIV_6)){
		make_turn	();
		return;
	}

//	movement().m_body.target.yaw	= movement().m_body.current.yaw;

	if (fis_zero(m_fSpeed))
		return;
	
	m_fCurSpeed		= m_fSpeed;

	// Update position and orientation of the planes
	float fAT = m_fASpeed * m_fTimeUpdateDelta;

	Fvector& tDirection = XFORM().k;

	// Tweak orientation based on last position and goal
	Fvector tOffset;
	tOffset.sub(m_tGoalDir,Position());

	if (!bStraightForward) {
		if (tOffset.y > 1.0) {			// We're too low
			m_tHPB.y += fAT;
			if (m_tHPB.y > 0.8f)	
				m_tHPB.y = 0.8f;
		}
		else 
			if (tOffset.y < -1.0) {	// We're too high
				m_tHPB.y -= fAT;
				if (m_tHPB.y < -0.8f)
					m_tHPB.y = -0.8f;
			}
			else							// Add damping
				m_tHPB.y *= 0.95f;
	}
	
	tDirection.normalize_safe	();
	tOffset.normalize_safe		();

	float fDot = tDirection.dotproduct(tOffset);
	float fSafeDot = fDot;

	fDot = (1.0f-fDot)/2.0f * fAT * 10.0f;

	tOffset.crossproduct(tOffset,tDirection);

	if (bStraightForward) {
		if (tOffset.y > 0.01f) {
			if (fSafeDot > .95f)
				m_fDHeading = 0.f;
			else
				if (fSafeDot > 0.75f)
					m_fDHeading = .10f;
			m_fDHeading = ( m_fDHeading * 9.0f + fDot )*0.1f;
		}
		else 
			if (tOffset.y < 0.01f) {
				if (fSafeDot > .95f)
					m_fDHeading = 0.f;
				else
					if (fSafeDot > 0.75f)
						m_fDHeading = -.10f;
				m_fDHeading = (m_fDHeading*9.0f - fDot)*0.1f;
			}
	}
	else {
		if (tOffset.y > 0.01f)		
			m_fDHeading = ( m_fDHeading * 9.0f + fDot )*0.1f;
		else 
			if (tOffset.y < 0.01f)
				m_fDHeading = (m_fDHeading*9.0f - fDot)*0.1f;
	}


	m_tHPB.x  +=  m_fDHeading;

	PitchCorrection		();
	m_tHPB.x			= angle_normalize_signed(m_tHPB.x);
//	m_tHPB.y			= angle_normalize_signed(m_tHPB.y);
	m_tHPB.y			= -movement().m_body.current.pitch;

	// Build the local matrix for the plane
	XFORM().setHPB		(m_tHPB.x,m_tHPB.y,0.f);//m_tHPB.z);
	Position()			= tSavedPosition;
	Position().mad		(tDirection,m_fSpeed*m_fTimeUpdateDelta);
//	Msg					("[%f][%f][%f]",VPUSH(Position()));
	movement().m_body.target.yaw	= -m_tHPB.x;
//	movement().m_body.target.pitch	= -m_tHPB.y;

//	Fvector tAcceleration;
//	tAcceleration.setHP(-movement().m_body.current.yaw,-movement().m_body.current.pitch);
//	tAcceleration.normalize_safe();
//	tAcceleration.mul(m_fSpeed*12.f);
//	m_PhysicMovementControl->SetPosition(Position());
//	m_PhysicMovementControl->Calculate	(tAcceleration,0,0,m_fTimeUpdateDelta,false);
//	m_PhysicMovementControl->GetPosition(Position());

	u32 dwNewNode = ai_location().level_vertex_id();
	const CLevelGraph::CVertex *tpNewNode = ai_location().level_vertex();
	CLevelGraph::CPosition	QueryPos;
	bool					a = !ai().level_graph().valid_vertex_id(dwNewNode) || !ai().level_graph().valid_vertex_position(Position());
	if (!a) {
		ai().level_graph().vertex_position	(QueryPos,Position());
		a					= !ai().level_graph().inside(*ai_location().level_vertex(),QueryPos);
	}
	if (a) {
		dwNewNode = ai().level_graph().vertex(ai_location().level_vertex_id(),Position());
		tpNewNode = ai().level_graph().vertex(dwNewNode);
	}
	if (ai().level_graph().valid_vertex_id(dwNewNode) && ai().level_graph().inside(*tpNewNode,QueryPos)) {
		Position().y = ai().level_graph().vertex_plane_y(*tpNewNode,Position().x,Position().z);
		m_tOldPosition.set(tSavedPosition);
		m_bNoWay		= false;
	}
	else {
		m_fSafeSpeed	= m_fSpeed = EPS_S;
		m_bNoWay		= true;
		m_tHPB			= tSafeHPB;
		XFORM().setHPB	(m_tHPB.x,m_tHPB.y,0.f);//m_tHPB.y,m_tHPB.z);
		Position()		= tSavedPosition;
		movement().m_body.target	= tSavedTorsoTarget;
		m_fDHeading		= fSavedDHeading;
	}

	if (m_bNoWay && (!m_turning || (angle_difference(movement().m_body.target.yaw, movement().m_body.current.yaw) < EPS_L))) {
		if ((Device.dwTimeGlobal - m_previous_query_time > TIME_TO_RETURN) || (!m_previous_query_time)) {
			float fAngle = ::Random.randF(m_fWallMinTurnValue,m_fWallMaxTurnValue);
			movement().m_body.target.yaw = movement().m_body.current.yaw + fAngle;
			movement().m_body.target.yaw = angle_normalize(movement().m_body.target.yaw);
			Fvector tTemp;
			tTemp.setHP(-movement().m_body.target.yaw,-movement().m_body.target.pitch);
			tTemp.mul(100.f);
			m_tGoalDir.add(Position(),tTemp);
			m_previous_query_time = Device.dwTimeGlobal;
		}
		make_turn		();
	}
	m_turning			= false;
}

void CAI_Rat::vfChooseNextGraphPoint()
{
	GameGraph::_GRAPH_ID	tGraphID		= m_tNextGP;
	CGameGraph::const_iterator	i,e;
	ai().game_graph().begin		(tGraphID,i,e);
	int					iPointCount		= (int)movement().locations().vertex_types().size();
	int					iBranches		= 0;
	for ( ; i != e; ++i)
		for (int j=0; j<iPointCount; ++j)
			if (ai().game_graph().mask(movement().locations().vertex_types()[j].tMask,ai().game_graph().vertex((*i).vertex_id())->vertex_type()) && ((*i).vertex_id() != m_tCurGP))
				++iBranches;
	ai().game_graph().begin		(tGraphID,i,e);
	if (!iBranches) {
		for ( ; i != e; ++i) {
			for (int j=0; j<iPointCount; ++j)
				if (ai().game_graph().mask(movement().locations().vertex_types()[j].tMask,ai().game_graph().vertex((*i).vertex_id())->vertex_type())) {
					m_tCurGP	= m_tNextGP;
					m_tNextGP	= (*i).vertex_id();
					m_dwTimeToChange	= Device.dwTimeGlobal + ::Random32.random(60000) + 60000;
					return;
				}
		}
	}
	else {
		int iChosenBranch = ::Random.randI(0,iBranches);
		iBranches = 0;
		for ( ; i != e; ++i) {
			for (int j=0; j<iPointCount; ++j)
				if (ai().game_graph().mask(movement().locations().vertex_types()[j].tMask,ai().game_graph().vertex((*i).vertex_id())->vertex_type()) && ((*i).vertex_id() != m_tCurGP)) {
					if (iBranches == iChosenBranch) {
						m_tCurGP			= m_tNextGP;
						m_tNextGP			= (*i).vertex_id();
						m_dwTimeToChange	= Device.dwTimeGlobal + ::Random32.random(60000) + 60000;
						return;
					}
					++iBranches;
				}
		}
	}
}

bool CAI_Rat::can_stand_here	()
{
	xr_vector<CObject*>					tpNearestList								; 
	Level().ObjectSpace.GetNearest		(tpNearestList, Position(),Radius(),this)	; 
	//xr_vector<CObject*>				&tpNearestList = Level().ObjectSpace.q_nearest; 
	if (tpNearestList.empty())
		return							(true);

	Fvector								c, d, C2;
	Visual()->vis.box.get_CD			(c,d);
	Fmatrix								M = XFORM();
	M.transform_tiny					(C2,c);
	M.c									= C2;
	MagicBox3							box(M,d);

	xr_vector<CObject*>::iterator		I = tpNearestList.begin();
	xr_vector<CObject*>::iterator		E = tpNearestList.end();
	for ( ; I != E; ++I) {
		if ((*I)->CLS_ID != CLSID_AI_RAT)
			continue;
		
		(*I)->Visual()->vis.box.get_CD	(c,d);
		M								= (*I)->XFORM();
		M.transform_tiny				(C2,c);
		M.c								= C2;
		
		if (box.intersects(MagicBox3(M,d)))
			return						(false);
	}
	return								(true);
}
