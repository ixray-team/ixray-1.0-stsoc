#include "stdafx.h"
#include "biting_state_eat_drag.h"
#include "ai_biting.h"

////////////////////////////////////////////////////////////////////////////////////

CStateBitingEatPrepareDrag::CStateBitingEatPrepareDrag(LPCSTR state_name) : inherited(state_name)
{
}

CStateBitingEatPrepareDrag::~CStateBitingEatPrepareDrag()
{
}


void CStateBitingEatPrepareDrag::initialize()
{
	inherited::initialize();
}

void CStateBitingEatPrepareDrag::execute()
{
	m_object->MotionMan.m_tAction = ACT_SLEEP;

	// если монстр подбежал к трупу, необходимо отыграть проверку трупа
	m_object->MotionMan.SetSpecParams(ASP_CHECK_CORPSE);
}

void CStateBitingEatPrepareDrag::finalize()
{
	inherited::finalize();
}

bool CStateBitingEatPrepareDrag::completed() const
{

	return false;
}


////////////////////////////////////////////////////////////////////////////////////

CStateBitingEatDrag::CStateBitingEatDrag(LPCSTR state_name) : inherited(state_name)
{
}

CStateBitingEatDrag::~CStateBitingEatDrag()
{
}


void CStateBitingEatDrag::initialize()
{
	inherited::initialize();
}

void CStateBitingEatDrag::execute()
{
	m_object->MotionMan.m_tAction = ACT_SLEEP;
}

void CStateBitingEatDrag::finalize()
{
	inherited::finalize();
}

bool CStateBitingEatDrag::completed() const
{
	return false;
}
