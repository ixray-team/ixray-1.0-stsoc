
#include "stdafx.h"
#include "ai_biting.h"
#include "biting_state_eat.h"
#include "biting_state_eat_approach.h"
#include "biting_state_eat_eat.h"
#include "biting_state_eat_drag.h"

CStateBitingEat::CStateBitingEat(LPCSTR state_name) : inherited(state_name)
{
}

CStateBitingEat::~CStateBitingEat	()
{
}

void CStateBitingEat::Load(LPCSTR section)
{
	add_state				(xr_new<CStateBitingEatApproach>	("Approach"),		eES_ApproachCorpse,	1);
	add_state				(xr_new<CStateBitingEatEat>			("Eat"),			eES_Eat,			0);
	//add_state				(xr_new<CStateBitingEatPrepreDrag>	("Prepare Drag"),	eES_PrepareDrag,	1);
	add_state				(xr_new<CStateBitingEatDrag>		("Drag"),			eES_Drag,			1);

	add_transition			(eES_ApproachCorpse,eES_PrepareDrag,1);
	add_transition			(eES_PrepareDrag,	eES_Drag,		1);
	add_transition			(eES_Drag,			eES_Eat,		1);

	inherited::Load			(section);

}

void CStateBitingEat::reinit(CAI_Biting *object)
{
	inherited::reinit		(object);
	set_current_state		(eES_ApproachCorpse);
	set_dest_state			(eES_Eat);
}

void CStateBitingEat::initialize()
{
	inherited::finalize();
}

void CStateBitingEat::execute()
{
	inherited::execute();
}

void CStateBitingEat::finalize()
{
	inherited::finalize();
}

//////////////////////////////////////////////////////////////////////////

void CStateBitingEatCheckCorpse::initialize()
{
	// если монстр подбежал к трупу, необходимо отыграть проверку трупа
	m_object->MotionMan.SetSpecParams	(ASP_CHECK_CORPSE);
	

	inherited::initialize();
}

void CStateBitingEatCheckCorpse::execute()
{
}

void CStateBitingEatCheckCorpse::finalize()
{
	inherited::finalize();
}

bool CStateBitingEatCheckCorpse::completed() const
{
	return false;
}



