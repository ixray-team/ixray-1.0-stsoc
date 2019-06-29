#include "stdafx.h"
#include "targetcsbase.h"
#include "targetcs.h"
#include "xrserver_objects.h"
#include "game_cl_base.h"
#include "level.h"

CTargetCSBase::CTargetCSBase(void)
{
}

CTargetCSBase::~CTargetCSBase(void)
{
}
BOOL CTargetCSBase::net_Spawn		(LPVOID DC)
{
	BOOL res	= inherited::net_Spawn(DC);

	if (res){
		CSE_Abstract				*e	= (CSE_Abstract*)(DC);
		CSE_Target_CS_Base			*T	= smart_cast<CSE_Target_CS_Base*>(e);
		radius	= T->radius;
		team	= T->s_team;
		setVisible					(true);
		setEnabled					(true);
		Game().targets.push_back	(this);

		CCF_Shape*	shape			= xr_new<CCF_Shape>	(this);
		collidable.model			= shape;
		Fsphere S;	S.P.set			(0,0,0); S.R = radius;
		shape->add_sphere			(S);
		shape->ComputeBounds		();
	}

	return res;
}

void CTargetCSBase::net_Destroy	()
{
	inherited::net_Destroy		();
	Game().targets.erase(std::find(Game().targets.begin(), Game().targets.end(), this));
}

void CTargetCSBase::feel_touch_new			(CObject* O) {
	CTargetCS *l_pTarg = smart_cast<CTargetCS*>(O);
	if(l_pTarg) {
		l_pTarg=l_pTarg;
	}
}

void CTargetCSBase::feel_touch_delete		(CObject* O) {
}

void CTargetCSBase::shedule_Update			(u32 dt)
{
	inherited::shedule_Update	(dt);

	feel_touch_update			(Position(),radius);
}
