#pragma once
#include "customtarget.h"

class CTargetCS;
class CTargetCSCask : public CCustomTarget {
	typedef				CCustomTarget inherited;
public:
	CTargetCSCask(void);
	virtual ~CTargetCSCask(void);
	virtual void OnEvent		(NET_Packet& P, u16 type);
	virtual void Load			(LPCSTR section);
	virtual void renderable_Render		();

	xr_list<CTargetCS*>			m_targets;
};
