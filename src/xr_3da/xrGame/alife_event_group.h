////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_event_group.h
//	Created 	: 02.06.2004
//  Modified 	: 02.06.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife event group class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_interfaces.h"

class CALifeEventGroup : public IPureSerializeObject<IReader,IWriter> {
public:
	u16				m_wCountBefore;
	u16				m_wCountAfter;

public:
	virtual			~CALifeEventGroup	();
	virtual void	load				(IReader &stream);
	virtual void	save				(IWriter &stream);
};

#include "alife_event_group_inline.h"