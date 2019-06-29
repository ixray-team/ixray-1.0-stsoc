////////////////////////////////////////////////////////////////////////////
//	Module 		: prefetch_manager.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Prefetch manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "prefetch_manager_inline.h"

class CPrefetchManager {
public:
					CPrefetchManager		();
	virtual			~CPrefetchManager		();
	virtual	void	Load					(LPCSTR section);
};