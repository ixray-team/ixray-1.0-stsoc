////////////////////////////////////////////////////////////////////////////
//	Module 		: graph_manager_interface.h
//	Created 	: 14.01.2004
//  Modified 	: 14.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Graph manager interface
////////////////////////////////////////////////////////////////////////////

#pragma once

interface IGraphManager {
public:
	virtual	void		update					(u32 time_delta) = 0;
};