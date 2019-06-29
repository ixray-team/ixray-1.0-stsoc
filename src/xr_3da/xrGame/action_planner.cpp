////////////////////////////////////////////////////////////////////////////
//	Module 		: action_planner.cpp
//	Created 	: 28.01.2004
//  Modified 	: 10.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Action planner
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "action_planner.h"

CActionPlanner::CActionPlanner			()
{
	init					();
}

CActionPlanner::~CActionPlanner			()
{
}

void CActionPlanner::init				()
{
}

void CActionPlanner::Load				(LPCSTR section)
{
}

void CActionPlanner::reinit				(bool clear_all)
{
	inherited::reinit		(clear_all);
	m_actuality				= true;
}

void CActionPlanner::reload				(LPCSTR section)
{
}

void CActionPlanner::update				(u32 time_delta)
{

}