#pragma once

#include "path_manager_params.h"
#include "level_graph.h"

#define CHECK_CONDITION(A)				((qwFlags & A) == A)

#define CALL_FUNCTION(A,B)				{\
	if (CHECK_CONDITION(A)) {\
		B();\
	}\
}

#define DEST_POSITIONS
#define FIRE_SAFETY_ANGLE				PI/18.f
#define aiSearchRange					(u64(1) <<  0)
#define aiEnemySurround					(u64(1) <<  1)
#define aiTotalCover					(u64(1) <<  2)
#define aiTravelWeight					(u64(1) <<  3)
#define aiLightWeight					(u64(1) <<  4)
#define aiLaziness						(u64(1) <<  5)
#define aiTotalViewVectorWeight			(u64(1) <<  6)
#define aiCoverFromMemberWeight			(u64(1) <<  7)
#define aiOptMemberDistance				(u64(1) <<  8)
#define aiOptMemberDistanceWeight		(u64(1) <<  9)
#define aiMinMemberDistance				(u64(1) << 10)
#define aiMinMemberDistanceWeight		(u64(1) << 11)
#define aiMaxMemberDistance				(u64(1) << 12)
#define aiMaxMemberDistanceWeight		(u64(1) << 13)
#define aiMemberViewDeviationWeight		(u64(1) << 14)
#define aiCoverFromEnemyWeight			(u64(1) << 15)
#define aiOptEnemyDistance				(u64(1) << 16)
#define aiOptEnemyDistanceWeight		(u64(1) << 17)
#define aiMinEnemyDistance				(u64(1) << 18)
#define aiMinEnemyDistanceWeight		(u64(1) << 19)
#define aiMaxEnemyDistance				(u64(1) << 20)
#define aiMaxEnemyDistanceWeight		(u64(1) << 21)
#define aiEnemyViewDeviationWeight		(u64(1) << 22)
#define aiInsideNode					(u64(1) << 23)

#define aiMemberDistance				(aiOptMemberDistance | aiOptMemberDistanceWeight | aiMinMemberDistance | aiMinMemberDistanceWeight | aiMaxMemberDistance | aiMaxMemberDistanceWeight)
#define aiEnemyDistance					(aiOptEnemyDistance | aiOptEnemyDistanceWeight | aiMinEnemyDistance | aiMinEnemyDistanceWeight | aiMaxEnemyDistance | aiMaxEnemyDistanceWeight)
#define aiMemberDanger					(u64(1) << 63)

namespace PathManagers {
	const Fvector	tLeft	= Fvector().set(-1,0,0);
	const Fvector	tRight	= Fvector().set(1,0,0);
	
	typedef SBaseParameters<float,u32,u32> CBaseParameters;

	class CAbstractNodeEvaluator : public CBaseParameters {
		static const u32		dwSelectorVarCount	= 23;
	public:
		float		    m_fBestCost,                    //  0
						m_fSearchRange,					//  1
						m_fEnemySurround,				//  2
						m_fTotalCover,					//  3
						m_fTravelWeight,				//  4
						m_fLightWeight,					//  5
						m_fLaziness,					//  6
						m_fTotalViewVectorWeight,		//  7
						m_fCoverFromMemberWeight,		//  8
						m_fOptMemberDistance,			//  9
						m_fOptMemberDistanceWeight,		// 10
						m_fMinMemberDistance,			// 11
						m_fMinMemberDistanceWeight,		// 12
						m_fMaxMemberDistance,			// 13
						m_fMaxMemberDistanceWeight,		// 14
						m_fMemberViewDeviationWeight,	// 15
						m_fCoverFromEnemyWeight,		// 16
						m_fOptEnemyDistance,			// 17
						m_fOptEnemyDistanceWeight,		// 18
						m_fMinEnemyDistance,			// 19
						m_fMinEnemyDistanceWeight,		// 20
						m_fMaxEnemyDistance,			// 21
						m_fMaxEnemyDistanceWeight,		// 22
						m_fEnemyViewDeviationWeight;	// 23

		u32				m_dwBestNode;
		u32				m_dwStartNode;
		Fvector			m_tStartPosition;

		// hit information
		Fvector			m_hit_direction;
		u32				m_hit_time;

		// current update time
		u32				m_dwCurTime;

		// myself
		CEntity*        m_tMe;
		CLevelGraph::CVertex *m_tpMyNode;
		Fvector			m_tMyPosition;
		Fvector			m_tDirection;

		//enemy
		CEntity*        m_tEnemy;
		CLevelGraph::CVertex *m_tpEnemyNode;
		u32				m_dwEnemyNode;
		Fvector			m_tEnemyPosition;
		Fvector			m_tEnemyDirection;
		Fvector			m_tEnemySurroundDirection;
		Fvector			m_tLastEnemyPosition;
		float			m_fRadius;

		// members
		MemberPlacement m_taMemberPositions;
		MemberNodes		m_taMemberNodes;
		MemberPlacement m_taDestMemberPositions;
		MemberNodes		m_taDestMemberNodes;
		EntityVec		*m_taMembers;

		// postion being tested
		float			m_fDistance;
		CLevelGraph::CVertex *m_tpCurrentNode;
		Fvector			m_tCurrentPosition;

		// members
		int				m_iAliveMemberCount;
		int				m_iCurrentMember;
		CLevelGraph::CVertex *m_tpCurrentMemberNode;
		Fvector			m_tCurrentMemberPosition;
		Fvector			m_tCurrentMemberDirection;

		// common
		float			m_fResult;
		float			m_fFireDispersionAngle;

		virtual			~CAbstractNodeEvaluator				() {}
		virtual	float	ffEvaluateNode						() = 0;
		virtual	void	vfShallowGraphSearch				(xr_vector<u32> &tpaStack, xr_vector<u8> &tpaMask) = 0;
		virtual	void	vfShallowGraphSearch				(xr_vector<u32> &tpaStack, xr_vector<bool> &tpaMask) = 0;
	};

	template <const u64 qwFlags> class CNodeEvaluator : public CAbstractNodeEvaluator {
	private:
		IC void vfNormalizeSafe(Fvector& Vector)
		{
			float fMagnitude = Vector.magnitude(); 
			if (fMagnitude > EPS_L) {
				Vector.x /= fMagnitude;
				Vector.y /= fMagnitude;
				Vector.z /= fMagnitude;
			}
			else {
				Vector.x = 1.f;
				Vector.y = 0.f;
				Vector.z = 0.f;
			}
		}

	public:

						CNodeEvaluator			(LPCSTR section, LPCSTR Name) : CBaseParameters()
		{
			svector<float *,dwSelectorVarCount>	m_tpVarValues;

			m_tpVarValues.resize(dwSelectorVarCount);
			m_tpVarValues[ 0] = &m_fSearchRange;
			m_tpVarValues[ 1] = &m_fEnemySurround;
			m_tpVarValues[ 2] = &m_fTotalCover;
			m_tpVarValues[ 3] = &m_fTravelWeight;
			m_tpVarValues[ 4] = &m_fLightWeight;
			m_tpVarValues[ 5] = &m_fLaziness;
			m_tpVarValues[ 6] = &m_fTotalViewVectorWeight;
			m_tpVarValues[ 7] = &m_fCoverFromMemberWeight;
			m_tpVarValues[ 8] = &m_fOptMemberDistance;
			m_tpVarValues[ 9] = &m_fOptMemberDistanceWeight;
			m_tpVarValues[10] = &m_fMinMemberDistance;
			m_tpVarValues[11] = &m_fMinMemberDistanceWeight;
			m_tpVarValues[12] = &m_fMaxMemberDistance;
			m_tpVarValues[13] = &m_fMaxMemberDistanceWeight;
			m_tpVarValues[14] = &m_fMemberViewDeviationWeight;
			m_tpVarValues[15] = &m_fCoverFromEnemyWeight;
			m_tpVarValues[16] = &m_fOptEnemyDistance;
			m_tpVarValues[17] = &m_fOptEnemyDistanceWeight;
			m_tpVarValues[18] = &m_fMinEnemyDistance;
			m_tpVarValues[19] = &m_fMinEnemyDistanceWeight;
			m_tpVarValues[20] = &m_fMaxEnemyDistance;
			m_tpVarValues[21] = &m_fMaxEnemyDistanceWeight;
			m_tpVarValues[22] = &m_fEnemyViewDeviationWeight;

			LPCSTR		S = pSettings->r_string(section,Name);
			string16	I;
			for (u32 i=0, j=0; i<dwSelectorVarCount; i++)
				if ((qwFlags & (u64(1) << i)) == (u64(1) << i))
					*(m_tpVarValues[i]) = float(atof(_GetItem(S,j++,I)));
		}

		IC		float	Evaluate							()
		{
			if (!CHECK_CONDITION(aiInsideNode))
				vfInit();
			CALL_FUNCTION(aiInsideNode,vfAddIfInsideNode);
			CALL_FUNCTION(aiTravelWeight,vfAddTravelCost);
			CALL_FUNCTION(aiLightWeight,vfAddLightCost);
			CALL_FUNCTION(aiTotalCover,vfAddTotalCoverCost);
			CALL_FUNCTION(aiEnemyDistance,vfAddDistanceToEnemyCost);
			CALL_FUNCTION(aiCoverFromEnemyWeight,vfAddCoverFromEnemyCost);
			CALL_FUNCTION(aiEnemyViewDeviationWeight,vfAddEnemyLookCost);
			CALL_FUNCTION(aiMemberDanger,vfAddMemberDanger);
			if (CHECK_CONDITION(aiMemberDistance) || CHECK_CONDITION(aiCoverFromMemberWeight) || CHECK_CONDITION(aiMemberViewDeviationWeight) || CHECK_CONDITION(aiEnemySurround)) {
				if (m_taMemberPositions.size()) {
					if (m_iAliveMemberCount) {
						for ( m_iCurrentMember=0 ; m_iCurrentMember<(int)m_taMemberPositions.size(); m_iCurrentMember++) {
							vfAssignMemberPositionAndNode();
							vfComputeMemberDirection();
							CALL_FUNCTION(aiMemberDistance,vfAddDistanceToMemberCost);
							vfComputeSurroundEnemy();
							CALL_FUNCTION(aiCoverFromMemberWeight,vfAddCoverFromMemberCost);
							CALL_FUNCTION(aiMemberViewDeviationWeight,vfAddDeviationFromMemberViewCost);
						}
						CALL_FUNCTION(aiEnemySurround,vfAddSurroundEnemyCost);
					}
				}
			}
			//vfCheckForEpsilon();
			return(m_fResult);
		}

		virtual	float	ffEvaluateNode						()
		{
			Evaluate();
			return(m_fResult);
		}

		IC		void vfAddIfInsideNode						()
		{
			if (!ai().level_graph().inside(m_tpCurrentNode,m_tStartPosition) || (_abs(m_tStartPosition.y - ai().level_graph().vertex_plane_y(m_tpCurrentNode,m_tStartPosition.x,m_tStartPosition.z) >= 1.f)))
				m_fResult = m_fDistance;
			else {
				m_fResult = -1.f;
				m_bStopSearch = true;
			}
		}

		IC		void vfAddTravelCost						()
		{
			m_fResult += m_fDistance*m_fTravelWeight;
		}

		IC		void vfAddLightCost							()
		{
			m_fResult += ((float)(m_tpCurrentNode->light())/255.f)*m_fLightWeight;
		}

		IC		void vfInit									()
		{
			m_fResult = 0.f;
			m_tEnemySurroundDirection.set(0,0,0);
			m_tCurrentPosition = ai().level_graph().vertex_position(m_tpCurrentNode);
			m_iAliveMemberCount = (int)m_taMemberPositions.size();
			m_fFireDispersionAngle = PI/10;
		}

		IC		void vfAddDistanceToEnemyCost				()
		{
			float fDistanceToEnemy = m_tCurrentPosition.distance_to(m_tEnemyPosition);
			if (fDistanceToEnemy < m_fMinEnemyDistance)
				m_fResult += m_fMinEnemyDistanceWeight*(m_fMinEnemyDistance + 0.1f)/(fDistanceToEnemy + 0.1f);
			else
				if (fDistanceToEnemy > m_fMaxEnemyDistance)
					m_fResult += m_fMaxEnemyDistanceWeight*(fDistanceToEnemy + 0.1f)/(m_fMaxEnemyDistance + 0.1f);
				else
					m_fResult += m_fOptEnemyDistanceWeight*_abs(fDistanceToEnemy - m_fOptEnemyDistance + 1.f)/(m_fOptEnemyDistance + 1.f);
		}

		IC		void vfAddDistanceToMemberCost				()
		{
	#ifdef DEST_POSITIONS
			float fDistanceToMember = m_tCurrentPosition.distance_to(m_taDestMemberPositions[m_iCurrentMember]);
	#else
			float fDistanceToMember = m_tCurrentPosition.distance_to(m_taMemberPositions[m_iCurrentMember]);
	#endif
			if (fDistanceToMember < m_fMinMemberDistance)
				m_fResult += m_fMinMemberDistanceWeight*(m_fMinMemberDistance + 0.1f)/(fDistanceToMember + 0.1f)/m_iAliveMemberCount;
			else
				if (fDistanceToMember > m_fMaxMemberDistance)
					m_fResult += m_fMaxMemberDistanceWeight*(fDistanceToMember + 0.1f)/(m_fMaxMemberDistance + 0.1f)/m_iAliveMemberCount;
				else
					m_fResult += m_fOptMemberDistanceWeight*_abs(fDistanceToMember - m_fOptMemberDistance + 1.f)/(m_fOptMemberDistance + 1.f)/m_iAliveMemberCount;
		}

		IC		void vfAddCoverFromEnemyCost				()
		{
			if (m_tpEnemyNode) {
				u8	*my_cover	= ai().level_graph().cover(m_tpCurrentNode);
				u8	*enemy_cover = ai().level_graph().cover(m_tpEnemyNode);

	#ifdef OLD_COVER_COST
				m_tEnemyDirection.x = m_tEnemyPosition.x - m_tCurrentPosition.x;
				m_tEnemyDirection.y = m_tEnemyPosition.y - m_tCurrentPosition.y;
				m_tEnemyDirection.z = m_tEnemyPosition.z - m_tCurrentPosition.z;
				if (m_tEnemyDirection.x < 0.f) {
					m_fResult += m_fCoverFromEnemyWeight*(1.f - float(my_cover[0])/255.f);
					m_fResult += m_fCoverFromEnemyWeight*(1.f - float(enemy_cover[2])/255.f);
				}
				else {
					m_fResult += m_fCoverFromEnemyWeight*(1.f - float(my_cover[2])/255.f);
					m_fResult += m_fCoverFromEnemyWeight*(1.f - float(enemy_cover[0])/255.f);
				}
				if (m_tEnemyDirection.z < 0.f) {
					m_fResult += m_fCoverFromEnemyWeight*(1.f - float(my_cover[3])/255.f);
					m_fResult += m_fCoverFromEnemyWeight*(1.f - float(enemy_cover[1])/255.f);
				}
				else {
					m_fResult += m_fCoverFromEnemyWeight*(1.f - float(my_cover[1])/255.f);
					m_fResult += m_fCoverFromEnemyWeight*(1.f - float(enemy_cover[3])/255.f);
				}
	#else
				m_tEnemyDirection.x = m_tEnemyPosition.x - m_tCurrentPosition.x;
				m_tEnemyDirection.y = m_tEnemyPosition.y - m_tCurrentPosition.y;
				m_tEnemyDirection.z = m_tEnemyPosition.z - m_tCurrentPosition.z;
				vfNormalizeSafe(m_tEnemyDirection);
				float fAlpha;
				if (m_tEnemyDirection.x < 0.f)
					if (m_tEnemyDirection.z >= 0.f) {
						fAlpha = acosf(m_tEnemyDirection.dotproduct(tLeft));
						m_fResult += m_fCoverFromEnemyWeight*(fAlpha/PI_DIV_2*(1.f - float(my_cover[0])/255.f) + (PI/2 - fAlpha)/PI_DIV_2*(1.f - float(my_cover[1])/255.f));
						fAlpha = PI/2 - fAlpha;
						m_fResult += m_fCoverFromEnemyWeight*(fAlpha/PI_DIV_2*(0.f + float(enemy_cover[2])/255.f) + (PI/2 - fAlpha)/PI_DIV_2*(0.f + float(enemy_cover[3])/255.f));
					}
					else {
						fAlpha = acosf(m_tEnemyDirection.dotproduct(tLeft));
						m_fResult += m_fCoverFromEnemyWeight*(fAlpha/PI_DIV_2*(1.f - float(my_cover[0])/255.f) + (PI/2 - fAlpha)/PI_DIV_2*(1.f - float(my_cover[3])/255.f));
						fAlpha = PI/2 - fAlpha;
						m_fResult += m_fCoverFromEnemyWeight*(fAlpha/PI_DIV_2*(0.f + float(enemy_cover[2])/255.f) + (PI/2 - fAlpha)/PI_DIV_2*(0.f + float(enemy_cover[1])/255.f));
					}
				else
					if (m_tEnemyDirection.z >= 0.f) {
						fAlpha = acosf(m_tEnemyDirection.dotproduct(tRight));
						m_fResult += m_fCoverFromEnemyWeight*(fAlpha/PI_DIV_2*(1.f - float(my_cover[2])/255.f) + (PI/2 - fAlpha)/PI_DIV_2*(1.f - float(my_cover[1])/255.f));
						fAlpha = PI/2 - fAlpha;
						m_fResult += m_fCoverFromEnemyWeight*(fAlpha/PI_DIV_2*(0.f + float(enemy_cover[0])/255.f) + (PI/2 - fAlpha)/PI_DIV_2*(0.f + float(enemy_cover[3])/255.f));
					}
					else {
						fAlpha = acosf(m_tEnemyDirection.dotproduct(tRight));
						m_fResult += m_fCoverFromEnemyWeight*(fAlpha/PI_DIV_2*(1.f - float(my_cover[2])/255.f) + (PI/2 - fAlpha)/PI_DIV_2*(1.f - float(my_cover[3])/255.f));
						fAlpha = PI/2 - fAlpha;
						m_fResult += m_fCoverFromEnemyWeight*(fAlpha/PI_DIV_2*(0.f + float(enemy_cover[0])/255.f) + (PI/2 - fAlpha)/PI_DIV_2*(0.f + float(enemy_cover[1])/255.f));
					}
	#endif
			}
		}

		IC		void vfAddCoverFromMemberCost				()
		{
			u8	*my_cover	= ai().level_graph().cover(m_tpCurrentNode);
			u8	*member_cover = ai().level_graph().cover(m_tpCurrentMemberNode);
	#ifdef OLD_COVER_COST
			m_tCurrentMemberDirection.x = m_tCurrentMemberPosition.x - m_tCurrentPosition.x;
			m_tCurrentMemberDirection.y = m_tCurrentMemberPosition.y - m_tCurrentPosition.y;
			m_tCurrentMemberDirection.z = m_tCurrentMemberPosition.z - m_tCurrentPosition.z;
			if (m_tCurrentMemberDirection.x < 0.f) {
				m_fResult += fCoverFromLeaderWeight*(1.f - float(my_cover[0])/255.f);
				m_fResult += fCoverFromLeaderWeight*(1.f - float(member_cover[2])/255.f);
			}
			else {
				m_fResult += fCoverFromLeaderWeight*(1.f - float(my_cover[2])/255.f);
				m_fResult += fCoverFromLeaderWeight*(1.f - float(member_cover[0])/255.f);
			}
			if (m_tCurrentMemberDirection.z < 0.f) {
				m_fResult += fCoverFromLeaderWeight*(1.f - float(my_cover[3])/255.f);
				m_fResult += fCoverFromLeaderWeight*(1.f - float(member_cover[1])/255.f);
			}
			else {
				m_fResult += fCoverFromLeaderWeight*(1.f - float(my_cover[1])/255.f);
				m_fResult += fCoverFromLeaderWeight*(1.f - float(member_cover[3])/255.f);
			}
	#else
			m_tCurrentMemberDirection.x = m_tCurrentMemberPosition.x - m_tCurrentPosition.x;
			m_tCurrentMemberDirection.y = m_tCurrentMemberPosition.y - m_tCurrentPosition.y;
			m_tCurrentMemberDirection.z = m_tCurrentMemberPosition.z - m_tCurrentPosition.z;
			vfNormalizeSafe(m_tCurrentMemberDirection);
			float fAlpha;
			if (m_tCurrentMemberDirection.x < 0.f)
				if (m_tCurrentMemberDirection.z >= 0.f) {
					fAlpha = acosf(m_tCurrentMemberDirection.dotproduct(tLeft));
					m_fResult += m_fCoverFromMemberWeight*(fAlpha/PI_DIV_2*(1.f - float(my_cover[0])/255.f) + (PI/2 - fAlpha)/PI_DIV_2*(1.f - float(my_cover[1])/255.f));
					fAlpha = PI/2 - fAlpha;
					m_fResult += m_fCoverFromMemberWeight*(fAlpha/PI_DIV_2*(1.f - float(member_cover[2])/255.f) + (PI/2 - fAlpha)/PI_DIV_2*(1.f - float(member_cover[3])/255.f));
				}
				else {
					fAlpha = acosf(m_tCurrentMemberDirection.dotproduct(tLeft));
					m_fResult += m_fCoverFromMemberWeight*(fAlpha/PI_DIV_2*(1.f - float(my_cover[0])/255.f) + (PI/2 - fAlpha)/PI_DIV_2*(1.f - float(my_cover[3])/255.f));
					fAlpha = PI/2 - fAlpha;
					m_fResult += m_fCoverFromMemberWeight*(fAlpha/PI_DIV_2*(1.f - float(member_cover[2])/255.f) + (PI/2 - fAlpha)/PI_DIV_2*(1.f - float(member_cover[1])/255.f));
				}
			else
				if (m_tCurrentMemberDirection.z >= 0.f) {
					fAlpha = acosf(m_tCurrentMemberDirection.dotproduct(tRight));
					m_fResult += m_fCoverFromMemberWeight*(fAlpha/PI_DIV_2*(1.f - float(my_cover[2])/255.f) + (PI/2 - fAlpha)/PI_DIV_2*(1.f - float(my_cover[1])/255.f));
					fAlpha = PI/2 - fAlpha;
					m_fResult += m_fCoverFromMemberWeight*(fAlpha/PI_DIV_2*(1.f - float(member_cover[0])/255.f) + (PI/2 - fAlpha)/PI_DIV_2*(1.f - float(member_cover[3])/255.f));
				}
				else {
					fAlpha = acosf(m_tCurrentMemberDirection.dotproduct(tRight));
					m_fResult += m_fCoverFromMemberWeight*(fAlpha/PI_DIV_2*(1.f - float(my_cover[2])/255.f) + (PI/2 - fAlpha)/PI_DIV_2*(1.f - float(my_cover[3])/255.f));
					fAlpha = PI/2 - fAlpha;
					m_fResult += m_fCoverFromMemberWeight*(fAlpha/PI_DIV_2*(1.f - float(member_cover[0])/255.f) + (PI/2 - fAlpha)/PI_DIV_2*(1.f - float(member_cover[1])/255.f));
				}
	#endif
		}

		IC		void vfAddTotalCoverCost					()
		{
			u8	*my_cover	= ai().level_graph().cover(m_tpCurrentNode);
			m_fResult += m_fTotalCover*(float(my_cover[0])/255.f + float(my_cover[1])/255.f + float(my_cover[2])/255.f + float(my_cover[3])/255.f);
		}

		IC		void vfAddEnemyLookCost						()
		{
			if (_abs(s32(m_dwCurTime - m_hit_time)) < ATTACK_HIT_REACTION_TIME) {
				Fvector tTempDirection0, tTempDirection1 = m_hit_direction;
				tTempDirection0.sub(m_tEnemyPosition,m_tCurrentPosition);
				vfNormalizeSafe(tTempDirection0);
				vfNormalizeSafe(tTempDirection1);
				float fAlpha = acosf(tTempDirection0.dotproduct(tTempDirection1));
				m_fResult += m_fEnemyViewDeviationWeight*(PI - fAlpha);
			}
		}

		IC		void vfAddEnemyLookCost						(float fAngle)
		{
			if (_abs(m_dwCurTime - m_hit_time) < ATTACK_HIT_REACTION_TIME) {
				Fvector tTempDirection0, tTempDirection1 = m_hit_direction;
				tTempDirection0.sub(m_tEnemyPosition,m_tCurrentPosition);
				vfNormalizeSafe(tTempDirection0);
				vfNormalizeSafe(tTempDirection1);
				float fAlpha = acosf(tTempDirection0.dotproduct(tTempDirection1));
				m_fResult += fEnemyViewDeviationWeight*_abs(fAngle - fAlpha);
			}
		}

		IC		void vfAssignMemberPositionAndNode			()
		{
	#ifdef DEST_POSITIONS
			m_tCurrentMemberPosition = m_taDestMemberPositions[m_iCurrentMember];
			m_tpCurrentMemberNode = ai().level_graph().vertex(m_taDestMemberNodes[m_iCurrentMember]);
	#else
			m_tCurrentMemberPosition = m_taMemberPositions[m_iCurrentMember];
			m_tpCurrentMemberNode = ai().level_graph().vertex(m_taMemberNodes[m_iCurrentMember]);
	#endif
		}

		IC		void vfComputeMemberDirection				()
		{
	#ifdef DEST_POSITIONS
			m_tCurrentMemberDirection.sub(m_taDestMemberPositions[m_iCurrentMember],m_tCurrentPosition);
	#else
			m_tCurrentMemberDirection.sub(m_taMemberPositions[m_iCurrentMember],m_tCurrentPosition);
	#endif
		}

		IC		void vfComputeSurroundEnemy					()
		{
	#ifdef DEST_POSITIONS
			m_tEnemySurroundDirection.x += m_taDestMemberPositions[m_iCurrentMember].x - m_tCurrentPosition.x;
			m_tEnemySurroundDirection.y += m_taDestMemberPositions[m_iCurrentMember].y - m_tCurrentPosition.y;
			m_tEnemySurroundDirection.z += m_taDestMemberPositions[m_iCurrentMember].z - m_tCurrentPosition.z;
	#else
			m_tEnemySurroundDirection.x += m_taMemberPositions[m_iCurrentMember].x - m_tCurrentPosition.x;
			m_tEnemySurroundDirection.y += m_taMemberPositions[m_iCurrentMember].y - m_tCurrentPosition.y;
			m_tEnemySurroundDirection.z += m_taMemberPositions[m_iCurrentMember].z - m_tCurrentPosition.z;
	#endif
		}

		IC		void vfAddSurroundEnemyCost					()
		{
			vfNormalizeSafe(m_tEnemySurroundDirection);
			m_fResult += m_fEnemySurround*m_tEnemySurroundDirection.magnitude();
		}

		IC		void vfCheckForEpsilon()
		{
			if (m_fResult < EPS)
				m_bStopSearch = TRUE;
		}

		IC		void vfAddDeviationFromMemberViewCost		()
		{
			Fvector tTempDirection0, tTempDirection1;
			tTempDirection0.sub(m_tEnemyPosition,m_tCurrentMemberPosition);
			tTempDirection1.sub(m_tCurrentPosition,m_tCurrentMemberPosition);
			vfNormalizeSafe(tTempDirection0);
			vfNormalizeSafe(tTempDirection1);
			float fAlpha = tTempDirection0.dotproduct(tTempDirection1);
			clamp(fAlpha,-0.999999f,0.999999f);
			fAlpha = acosf(fAlpha);
			if (fAlpha < m_fFireDispersionAngle)
				m_fResult += m_fMemberViewDeviationWeight;
		}

		IC		void vfAddDeviationFromPreviousDirectionCost()
		{
			Fvector tTempDirection;
			tTempDirection.sub(m_tMyPosition,m_tCurrentPosition);
			vfNormalizeSafe(tTempDirection);
			float fAlpha = tTempDirection.dotproduct(m_tDirection);
			clamp(fAlpha,-0.99999f,0.99999f);
			fAlpha = acosf(fAlpha);
			m_fResult += fAlpha * m_fMemberViewDeviationWeight;
		}

		IC		void vfAddDistanceToLastPositionCost		()
		{
			Fvector tTemp;
			tTemp.sub(m_tCurrentPosition,m_tLastEnemyPosition);
			m_fResult += 20.f*tTemp.magnitude();
		}

		IC		void vfAddMemberDanger						()
		{
			for (int i=0; i<(int)m_taMemberPositions.size(); i++) {
				float fAlpha = m_tEnemyDirection.dotproduct(m_taMemberPositions[i]);
				clamp(fAlpha,-.99999f,+.99999f);
				fAlpha = acosf(fAlpha);
				if (fAlpha < FIRE_SAFETY_ANGLE) {
					m_fResult += 1000.f;
					break;
				}
			}
		}
	};
};