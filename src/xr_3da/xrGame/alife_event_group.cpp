////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_event_group.cpp
//	Created 	: 02.06.2004
//  Modified 	: 02.06.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife event group class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_event_group.h"

CALifeEventGroup::~CALifeEventGroup	()
{
}

void CALifeEventGroup::load			(IReader &stream)
{
	stream.r				(&m_wCountAfter,sizeof(m_wCountAfter));
}

void CALifeEventGroup::save			(IWriter &stream)
{
	stream.w				(&m_wCountAfter,sizeof(m_wCountAfter));
}
