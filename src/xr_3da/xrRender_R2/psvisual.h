#ifndef _INCDEF_PSVisual_H_
#define _INCDEF_PSVisual_H_

#include "..\FBasicVisual.h"
#include "ParticleSystem.h"

class CPSVisual			: public IRender_Visual
{
protected:
	PS::Particles	  	m_Particles;
	PS::SParams*		m_Definition;
public:
    PS::SEmitter*		m_Emitter;
public:
						CPSVisual		();
	virtual				~CPSVisual		();

	// Visual
	u32					RenderTO		(FVF::TL* V);
	virtual void		Render			(float LOD);
	virtual void		Copy			(IRender_Visual* pFrom);

	// Functionality
	void				Update			(u32 dt);
	void				Compile			(PS::SDef* source, PS::SEmitter* E);
	IC u32				ParticleCount	()	{ return (u32)m_Particles.size();}
	IC void				Stop			()	{ m_Particles.clear();      }
};
#endif /*_INCDEF_PSVisual_H_*/

