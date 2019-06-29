#pragma once

#include "ai_space.h"

class CCustomMonster;

namespace AI
{
	class CPathNodes : public NodePath
	{
	public:
		BOOL				bNeedRebuild;
		u32					DestNode;

		u32					TravelStart;
		xr_vector<CTravelNode>	TravelPath;
		xr_vector<PSegment>	Segments;

		float				fSpeed;
		float				m_fAccumulatedDT;
		BOOL				m_bCollision;

		CPathNodes()	{
			bNeedRebuild	= FALSE;
			DestNode		= 0;
			m_bCollision	= true;
			m_fAccumulatedDT = 0.f;
		}

		void				BuildTravelLine	(const Fvector& current_pos);

		void				Calculate		(CCustomMonster* Me, Fvector& p_dest, Fvector& p_src, float speed, float dt);
		void				Direction		(Fvector& dest);
	};

};
