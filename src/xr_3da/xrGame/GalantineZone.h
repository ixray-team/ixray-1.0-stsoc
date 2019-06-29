/////////////////////////////////////////////////////
// GalantineZone.h
// Аномальная зона: "ведьмин студень"
// при попадании живого объекта в зону происходит 
//
/////////////////////////////////////////////////////

#pragma once
#include "customzone.h"

class CGalantineZone : public CCustomZone
{
private:
    typedef	CCustomZone	inherited;
public:
	CGalantineZone(void);
	virtual ~CGalantineZone(void);

	virtual void Load(LPCSTR section);

	virtual void UpdateCL();
	virtual void Affect(CObject* O);

	virtual void Postprocess(f32 val);

protected:
	ref_str m_pHitEffect;
};
