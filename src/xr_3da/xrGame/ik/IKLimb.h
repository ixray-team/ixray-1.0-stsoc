#pragma once


#include	"limb.h"

class	CKinematics	;
class	CDB::TRI	;


struct SCalculateData;

class CIKLimb
{
public:
		enum	Etype{
			tpRight,
			tpLeft
		};
					CIKLimb				()															;
		void		Create				(CKinematics* K,u16 bones[3],const Fvector& toe_pos,Etype tp);	
		void		Destroy				()															;
		void		Calculate			(CKinematics* K,const Fmatrix &obj)							;

private:

		Limb		m_limb																		;
		Fvector		m_toe_position																;
		u16			m_bones				[4]														;	
		u32			m_prev_frame																;
		bool		m_prev_state_anim															;
		void		Invalidate			()														;
		void		GoalMatrix			(Matrix	&M,SCalculateData* cd)							;
		void		Collide				(SCalculateData* cd)									;
		void		CalculateBones		(SCalculateData* cd)									;

static	void 		BonesCallback0		(CBoneInstance* B)										;
static	void 		BonesCallback1		(CBoneInstance* B)										;
static	void 		BonesCallback2		(CBoneInstance* B)										;
protected:
private:
};