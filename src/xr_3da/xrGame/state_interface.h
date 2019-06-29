////////////////////////////////////////////////////////////////////////////
//	Module 		: state_interface.h
//	Created 	: 12.01.2004
//  Modified 	: 12.01.2004
//	Author		: Dmitriy Iassenev
//	Description : State interface
////////////////////////////////////////////////////////////////////////////

#pragma once

interface IStateAbstract {
public:
	virtual	void		initialize()	= 0;
	virtual	void		execute()		= 0;
	virtual	void		finalize()		= 0;
};