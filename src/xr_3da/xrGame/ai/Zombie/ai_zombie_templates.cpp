////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_zombie_templates.cpp
//	Created 	: 23.07.2002
//  Modified 	: 07.11.2002
//	Author		: Dmitriy Iassenev
//	Description : Templates for monster "Zombie"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_zombie.h"
#include "../ai_monsters_misc.h"

void CAI_Zombie::vfSetFire(bool bFire, CGroup &/**Group/**/)
{
	if (bFire) {
		m_bFiring = true;
		m_tAction = eZombieActionAttackBegin;
	}
	else {
		m_bFiring = false;
		m_tAction = eZombieActionAttackEnd;
	}
}

void CAI_Zombie::vfSetMovementType(float fSpeed)
{
//	StandUp();
	m_fSpeed = m_fCurSpeed = fSpeed;
}

void CAI_Zombie::vfAdjustSpeed()
{
	Fvector tTemp1, tTemp2;
	tTemp1.sub(m_tGoalDir,Position());
	tTemp1.normalize_safe();
	tTemp2 = XFORM().k;
	tTemp2.normalize_safe();
	float fAngle = tTemp1.dotproduct(tTemp2);
	clamp(fAngle,0.f,.99999f);
	fAngle = acosf(fAngle);
	
	float fMinASpeed = PI_MUL_2, fNullASpeed = PI_MUL_2, fMaxASpeed = .2f, fAttackASpeed = .15f;
	
	if (_abs(m_fSpeed - m_fMinSpeed) <= EPS_L)	{
		if (fAngle >= 3*PI_DIV_2) {
			m_fSpeed = 0;
			m_fASpeed = fNullASpeed;
			m_body.target.yaw = fAngle;
		}
		else 
		{
			m_fSpeed = m_fMinSpeed;
			m_fASpeed = fMinASpeed;
		}
	}
	else
		if (_abs(m_fSpeed - m_fMaxSpeed) <= EPS_L)	{
			if (fAngle >= 3*PI_DIV_2) {
				m_fSpeed = 0;
				m_fASpeed = fNullASpeed;
				m_body.target.yaw = fAngle;
			}
			else
				if (fAngle >= PI_DIV_2) {
					m_fSpeed = m_fMinSpeed;
					m_fASpeed = fMinASpeed;
				}
				else {
					m_fSpeed = m_fMaxSpeed;
					m_fASpeed = fMaxASpeed;
				}
		}
		else
			if (_abs(m_fSpeed - m_fAttackSpeed) <= EPS_L)	{
				if (fAngle >= 3*PI_DIV_2) {
					m_fSpeed = 0;
					m_fASpeed = fNullASpeed;
					m_body.target.yaw = fAngle;
				}
				else
					if (fAngle >= PI_DIV_2) {
						m_fSpeed = m_fMinSpeed;
						m_fASpeed = fMinASpeed;
					}
					else
						if (fAngle >= PI_DIV_4) {
							m_fSpeed = m_fMaxSpeed;
							m_fASpeed = fMaxASpeed;
						}
						else {
							m_fSpeed = m_fAttackSpeed;
							m_fASpeed = fAttackASpeed;
						}
			}
			else {
				m_body.target.yaw = fAngle;
				m_fSpeed = 0;
				m_fASpeed = fNullASpeed;
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
				m_fASpeed = fMinASpeed;
			}
			else {
				m_fSpeed = m_fMaxSpeed;
				m_fASpeed = fMaxASpeed;
			}
		}
		else 
		{
			m_fSpeed = m_fMinSpeed;
			m_fASpeed = fMinASpeed;
		}
	}
}

bool CAI_Zombie::bfComputeNewPosition(bool bCanAdjustSpeed, bool bStraightForward)
{
	// saving current parameters
	Fvector tSafeHPB = m_tHPB;
	Fvector tSavedPosition = Position();
	SRotation tSavedTorsoTarget = m_body.target;
	float fSavedDHeading = m_fDHeading;

	if (bCanAdjustSpeed)
		vfAdjustSpeed();

	if (m_fSpeed < EPS_L)
		return(true);
	
	m_fCurSpeed = m_fSpeed;
	
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
	
	tDirection.normalize();
	tOffset.normalize	();

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

	m_tHPB.x = angle_normalize_signed(m_tHPB.x);
	m_tHPB.y = angle_normalize_signed(m_tHPB.y);

	// Build the local matrix for the pplane
	Fmatrix			mXFORM;
	mXFORM.setHPB	(m_tHPB.x,m_tHPB.y,m_tHPB.z);
	mXFORM.c.set	(Position());
	XFORM().set		(mXFORM);
	m_body.target.yaw = -m_tHPB.x;

	// Update position
//	Level().ObjectSpace.GetNearest(Position(),1.f);
//	if (Level().ObjectSpace.q_nearest.size()) {
//		Fvector tAcceleration;
//		tAcceleration.setHP(-m_body.current.yaw,-m_body.current.pitch);
//		tAcceleration.normalize_safe();
//		tAcceleration.mul(m_fSpeed*12.f);
//		m_PhysicMovementControl->SetPosition(Position());
//		m_PhysicMovementControl->Calculate	(tAcceleration,0,0,m_fTimeUpdateDelta,false);
//		m_PhysicMovementControl->GetPosition(Position());
//	}
//	else 
	{
//		if (feel_touch.size() || true) {
//			Fvector tTemp1;
//			tTemp1.set(Position());
//			tTemp1.mad(tDirection,m_fSpeed*m_fTimeUpdateDelta);
//			Position().set(tfGetNextCollisionPosition(this,tTemp1));
//		}
//		else
			Position().mad(tDirection,m_fSpeed*m_fTimeUpdateDelta);
	}


	u32 dwNewNode = level_vertex_id();
	const CLevelGraph::CVertex *tpNewNode = level_vertex();
	CLevelGraph::CPosition	QueryPos;
	ai().level_graph().vertex_position	(QueryPos,Position());
	if (!ai().level_graph().valid_vertex_id(dwNewNode) || !ai().level_graph().inside(*level_vertex(),QueryPos)) {
		dwNewNode = ai().level_graph().vertex(level_vertex_id(),Position());
		tpNewNode = ai().level_graph().vertex(dwNewNode);
	}
	if (ai().level_graph().valid_vertex_id(dwNewNode) && ai().level_graph().inside(*tpNewNode,QueryPos)) {
		Position().y = ai().level_graph().vertex_plane_y(*tpNewNode,Position().x,Position().z);
		m_tOldPosition.set(tSavedPosition);
		m_bNoWay = false;
	}
	else {
		m_fSafeSpeed = m_fSpeed = EPS_S;
		m_bNoWay = true;
		Position() = tSavedPosition;
		m_tHPB = tSafeHPB;
		m_body.target = tSavedTorsoTarget;
		m_fDHeading = fSavedDHeading;
	}

	bool m_bResult = false;
	if ((angle_difference(m_body.target.yaw, m_body.current.yaw) > PI_DIV_8) || m_bNoWay) {
		m_fSpeed = .1f;
		if (m_bNoWay)
			if ((Level().timeServer() - m_previous_query_time > TIME_TO_RETURN) || (!m_previous_query_time)) {
				float fAngle = ::Random.randF(m_fWallMinTurnValue,m_fWallMaxTurnValue);
				m_body.target.yaw = m_body.current.yaw + fAngle;
				m_body.target.yaw = angle_normalize(m_body.target.yaw);
				Fvector tTemp;
				tTemp.setHP(-m_body.target.yaw,-m_body.target.pitch);
				tTemp.mul(100.f);
				m_tGoalDir.add(Position(),tTemp);
				m_previous_query_time = Level().timeServer();
			}
		m_bResult = true;
	}

	return(m_bResult);
}