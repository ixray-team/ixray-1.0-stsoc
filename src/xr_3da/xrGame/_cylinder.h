#ifndef Pcylinder_H
#define Pcylinder_H

struct	Pcylinder
{
	Fmatrix33	m_rotate;
	Fvector		m_translate;
	float		m_halflength;
	float		m_radius;
	IC void		invalidate() {
		m_rotate.identity	()		;
		m_translate.set		(0,0,0)	;
		m_halflength		=0.f	;
		m_radius			=0.f	;
	}
	IC void		xform_get(Fmatrix& D)	
	{
		D.i.set(m_rotate.i); D._14_ = 0;
		D.j.set(m_rotate.j); D._24_ = 0;
		D.k.set(m_rotate.k); D._34_ = 0;
		D.c.set(m_translate);D._44_ = 1;
	}
	IC void		xform_set(const Fmatrix& S)
	{
		m_rotate.i.set	(S.i);
		m_rotate.j.set	(S.j);
		m_rotate.k.set	(S.k);
		m_translate.set	(S.c);
	}
};

#endif