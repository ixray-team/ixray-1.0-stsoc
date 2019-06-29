////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_space_obstacles.cpp
//	Created 	: 02.11.2003
//  Modified 	: 02.11.2003
//	Author		: Dmitriy Iassenev
//	Description : AI Space obstacles solver
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_space.h"

#ifdef DEBUG

#include "_fbox.h"

void CAI_Space::dbg_DrawOBB(Fmatrix &T, Fvector &half_dim, u32 C, u32 C1)
{
	RCache.dbg_DrawOBB				(T,half_dim,C1);

	Fmatrix mL2W_Transform,mScaleTransform;

	mScaleTransform.scale(half_dim);
	mL2W_Transform.mul_43(T,mScaleTransform);

	Fvector aabb[8];
	aabb[0].set( -1, -1, -1); // 0
	aabb[1].set( -1, +1, -1); // 1
	aabb[2].set( +1, +1, -1); // 2
	aabb[3].set( +1, -1, -1); // 3
	aabb[4].set( -1, -1, +1); // 4
	aabb[5].set( -1, +1, +1); // 5
	aabb[6].set( +1, +1, +1); // 6
	aabb[7].set( +1, -1, +1); // 7

	//	u16             aabb_id[12*2] = {
	//		0,1,  1,2,  2,3,  3,0,  4,5,  5,6,  6,7,  7,4,  1,5,  2,6,  3,7,  0,4
	//	};
	for (int i=0; i<8; i++) {
		Fmatrix						V = Fidentity;
		mL2W_Transform.transform_tiny(V.c,aabb[i]);
		V.i.x						= .05f;
		V.j.y						= .05f;
		V.k.z						= .05f;
		RCache.dbg_DrawEllipse		(V,C);
	}
}

void CAI_Space::DrawTravelLine()
{
	return;
	DrawDynamicObstacles				();
	if (!m_tpTravelLine.empty()) {
		Fvector							P = m_tpTravelLine[0];
		P.y								+= .1f;
		RCache.dbg_DrawAABB				(P,.1f,.1f,.1f,D3DCOLOR_XRGB(0,0,255));
	}
	for (u32 I=1; I<m_tpTravelLine.size(); I++) {
		Fvector	P1, P2; 
		P1.set							(m_tpTravelLine[I - 1]);
		P2.set							(m_tpTravelLine[I]);

		P1.y							+= 0.1f;
		P2.y							+= 0.1f;

		RCache.dbg_DrawLINE				(Fidentity,P1,P2,D3DCOLOR_XRGB(0,255,0));

		if (I == (m_tpTravelLine.size() - 1 ))
			RCache.dbg_DrawAABB			(P1,.1f,.1f,.1f,D3DCOLOR_XRGB(255,0,0));
		else 
			RCache.dbg_DrawAABB			(P1,.1f,.1f,.1f,D3DCOLOR_XRGB(0,0,255));

		Fvector         T;
		Fvector4        S;

		T.set							(m_tpTravelLine[I]);
		T.y								+= (1.5f);

		Device.mFullTransform.transform (S,T);
	}
}

void CAI_Space::ComputeTravelLine(AI::NodePath &AI_Path, u32 dwStartNodeID, u32 dwFinishNodeID)
{
	Fvector						*tpDestinationPosition = &m_tFinishPoint;

	if (tpDestinationPosition && bfInsideNode(Node(dwFinishNodeID),*tpDestinationPosition))
		tpDestinationPosition->y = ffGetY(*Node(dwFinishNodeID),tpDestinationPosition->x,tpDestinationPosition->z);

	m_tpaPoints.clear			();
	m_tpaDeviations.clear		();
	m_tpaTravelPath.clear		();
	m_tpaPointNodes.clear		();

	u32							N = (int)AI_Path.Nodes.size();
	if (!N) {
		Msg						("! Node list is empty!");
		AI_Path.Nodes.clear		();
		m_tpTravelLine.clear	();
		return;
	}

	Fvector						tStartPosition = m_tStartPoint;
	u32							dwCurNode = dwStartNodeID;
	m_tpaPoints.push_back		(tStartPosition);
	m_tpaPointNodes.push_back	(dwCurNode);

	for (u32 i=1; i<=N; i++)
		if (i<N) {
			if (!bfCheckNodeInDirection(dwCurNode,tStartPosition,AI_Path.Nodes[i])) {
				if (dwCurNode != AI_Path.Nodes[i - 1])
					m_tpaPoints.push_back(tStartPosition = tfGetNodeCenter(dwCurNode = AI_Path.Nodes[--i]));
				else
					m_tpaPoints.push_back(tStartPosition = tfGetNodeCenter(dwCurNode = AI_Path.Nodes[i]));
				m_tpaPointNodes.push_back(dwCurNode);
			}
		}
		else
			if (tpDestinationPosition)
				if (dwfCheckPositionInDirection(dwCurNode,tStartPosition,*tpDestinationPosition) == u32(-1)) {
					if (dwCurNode != dwFinishNodeID)
						m_tpaPointNodes.push_back(dwFinishNodeID);
					m_tpaPoints.push_back(*tpDestinationPosition);
				}
				else {
					dwCurNode = dwFinishNodeID;
					if (bfInsideNode(Node(dwCurNode),*tpDestinationPosition)) {
						tpDestinationPosition->y = ffGetY(*Node(dwFinishNodeID),tpDestinationPosition->x,tpDestinationPosition->z);
						m_tpaPointNodes.push_back(dwFinishNodeID);
						m_tpaPoints.push_back(*tpDestinationPosition);
					}
				}

				m_tpaDeviations.resize	(N = (int)m_tpaPoints.size());

				m_tpTravelLine.clear();

				AI_Path.Nodes.clear		();

				Fvector			T;
				T.set			(0,0,0);
				for (i=1; i<N; i++) {
					m_tpaLine.clear();
					m_tpaLine.push_back(m_tpaPoints[i-1]);
					m_tpaLine.push_back(m_tpaPoints[i]);
					bfCreateStraightPTN_Path(m_tpaPointNodes[i-1],m_tpaPoints[i-1],m_tpaPoints[i],m_tpaTravelPath,m_tpaNodes, i == 1);
					u32 n = (u32)m_tpaTravelPath.size();
					for (u32 j= 0; j<n; j++) {
						T = m_tpaTravelPath[j];
						m_tpTravelLine.push_back(T);
						AI_Path.Nodes.push_back(m_tpaNodes[j]);
					}
				}
				if (N > 1) {
					AI_Path.Nodes[AI_Path.Nodes.size() - 1] = dwFinishNodeID;
					if (tpDestinationPosition && m_tpTravelLine.size() && m_tpTravelLine[m_tpTravelLine.size() - 1].distance_to(*tpDestinationPosition) > EPS_L) {
						if (bfInsideNode(Node(dwFinishNodeID),*tpDestinationPosition) && dwfCheckPositionInDirection(dwFinishNodeID,T,*tpDestinationPosition) != -1) {
							T = *tpDestinationPosition;
							m_tpTravelLine.push_back(T);
						}
					}
				}
				else
					if (tpDestinationPosition && m_tpTravelLine.size() && m_tpTravelLine[m_tpTravelLine.size() - 1].distance_to(*tpDestinationPosition) > EPS_L)
						if (bfInsideNode(Node(dwFinishNodeID),*tpDestinationPosition) && dwfCheckPositionInDirection(dwFinishNodeID,T,*tpDestinationPosition) != -1)
							m_tpTravelLine.push_back(*tpDestinationPosition);
}

#include "a_star.h"
void CAI_Space::ComputePath()
{
	return;
	u32						l_dwStartNodeID		= q_LoadSearch(m_tStartPoint);
	VERIFY					(bfInsideNode(Node(l_dwStartNodeID),m_tStartPoint));
	u32						l_dwFinishNodeID	= q_LoadSearch(m_tFinishPoint);
	VERIFY					(bfInsideNode(Node(l_dwFinishNodeID),m_tFinishPoint));
	AI::NodePath			l_tpNodePath;
	m_tpAStar->ffFindMinimalPath(l_dwStartNodeID,l_dwFinishNodeID,l_tpNodePath,false);
	ComputeTravelLine		(l_tpNodePath,l_dwStartNodeID,l_dwFinishNodeID);
}

void CAI_Space::DrawDynamicObstacles()
{
	Level().CurrentEntity()->setEnabled	(false);
	Level().ObjectSpace.GetNearest		(m_tStartPoint,100.f); 
	xr_vector<CObject*>					&tpNearestList = Level().ObjectSpace.q_nearest; 
	Level().CurrentEntity()->setEnabled	(true);

	{
		xr_vector<CObject*>::iterator		I = tpNearestList.begin();
		xr_vector<CObject*>::iterator		E = tpNearestList.end();
		for ( ; I != E; ++I) {
			Fvector							c, d, C2;
			(*I)->Visual()->vis.box.get_CD	(c,d);
			Fmatrix							M = (*I)->XFORM();
			M.transform_tiny				(C2,c);
			M.c								= C2;
			dbg_DrawOBB						(M,d,color_rgba(0,0,255,255),color_rgba(0,255,0,255));
		}
	}
}
#endif
