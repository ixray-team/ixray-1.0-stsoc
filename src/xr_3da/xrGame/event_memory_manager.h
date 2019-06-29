////////////////////////////////////////////////////////////////////////////
//	Module 		: event_memory_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Event memory manager
////////////////////////////////////////////////////////////////////////////

#pragma once

class CEventMemoryManager {
public:
					CEventMemoryManager			();
	virtual			~CEventMemoryManager		();
			void	init						();
	virtual void	Load						(LPCSTR section);
	virtual void	reinit						();
	virtual void	reload						(LPCSTR section);
	virtual void	update						();
};