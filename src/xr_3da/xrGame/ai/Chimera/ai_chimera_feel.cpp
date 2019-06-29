////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_chimera_feel.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : Visibility and look for all the chimera monsters
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_chimera.h"

// Развернуть объект в направление движения
void CAI_Chimera::SetReversedDirectionLook()
{
	int i = ps_Size();		// position stack size
	if (i > 1) {
		CObject::SavedPosition tPreviousPosition = ps_Element(i - 2), tCurrentPosition = ps_Element(i - 1);
		tWatchDirection.sub(tPreviousPosition.vPosition,tCurrentPosition.vPosition);
		if (tWatchDirection.magnitude() > EPS_L) {
			tWatchDirection.normalize();
			r_torso_target.yaw += PI;
			mk_rotation(tWatchDirection,r_torso_target);
		}
	}
	else
		r_torso_target.pitch = 0;
	r_target = r_torso_target;
}

void CAI_Chimera::feel_sound_new(CObject* who, int eType, const Fvector &Position, float power)
{
	if (!g_Alive())
		return;

#ifdef IGNORE_ACTOR
	if ((who == Level().CurrentEntity()) || (who && (who->H_Parent() == Level().CurrentEntity())))
		return;
#endif

	if ((eType & SOUND_TYPE_WEAPON_SHOOTING) == SOUND_TYPE_WEAPON_SHOOTING)
		power = 1.f;//expf(.1f*log(power));

	if (power >= m_fSoundThreshold) {
		if (this != who) {
			HearSound(who,eType,Position,power,m_dwCurrentUpdate);
		}
 	}

}

void CAI_Chimera::feel_touch_new	(CObject* O)
{
	if (!g_Alive()) return;

}

void CAI_Chimera::DoDamage(CEntity *pEntity)
{
	if (!g_Alive()) return;
	if (!pEntity) return;

	VisionElem ve;
	if (!GetEnemy(ve)) return;

	if ((ve.obj->CLS_ID == CLSID_ENTITY) && (ve.obj == pEntity)) {
		Fvector tDirection;
		Fvector position_in_bone_space;
		position_in_bone_space.set(0.f,0.f,0.f);
		tDirection.sub(ve.obj->Position(),this->Position());
		tDirection.normalize();

		pEntity->Hit(m_fHitPower,tDirection,this,0,position_in_bone_space,0);
	}
}


BOOL  CAI_Chimera::feel_vision_isRelevant(CObject* O)
{
	if (O->CLS_ID!=CLSID_ENTITY)	
		return FALSE;
	else  {
		CEntityAlive* E = dynamic_cast<CEntityAlive*> (O);
		if (!E) return FALSE;
		if (E->g_Team() == g_Team() && E->g_Alive()) return FALSE;
		return TRUE;
	}
}

