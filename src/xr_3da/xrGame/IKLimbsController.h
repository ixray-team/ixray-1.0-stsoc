#pragma once
#include "IK/IKLimb.h"
class CKinematics	;
class CGameObject	;
class CIKLimbsController 
{
public:
					CIKLimbsController			()									;
			void	Create						(CKinematics* K,	CGameObject*O)	;
			void	Destroy						()									;
			void	Calculate					(CKinematics* K,const Fmatrix &obj)					;
private:

protected:
private:
			CIKLimb				m_left_leg											;
			CIKLimb				m_right_leg											;
};