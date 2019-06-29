///////////////////////////////////////////////////////////////
// Bread.h
// Bread - хлеб, еда персонажей
///////////////////////////////////////////////////////////////


#pragma once

#include "eatable_item.h"

class CBread: public CEatableItem
{
private:
    typedef	CEatableItem inherited;
public:
	CBread(void);
	virtual ~CBread(void);

	virtual BOOL net_Spawn			(LPVOID DC);
	virtual void Load				(LPCSTR section);
	virtual void net_Destroy		();
	virtual void shedule_Update		(u32 dt);
	virtual void UpdateCL			();
	virtual void renderable_Render	();

	virtual void OnH_A_Chield		();
	virtual void OnH_B_Independent	();

};
