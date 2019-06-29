// Particle.h: interface for the CParticle class.
//---------------------------------------------------------------------------

#ifndef ParticleH
#define ParticleH

template <class T> struct Pair{
    T start;
    T end;
};
struct SParticle
{
public:
	Pair<float>		m_Time;
    Pair<float>		m_ASpeed;
    Pair<float>  	m_Size;

	Pair<Fcolor>	m_Color;
    Fvector			m_GravityStart;
    Fvector			m_GravityLambda;

	Pair<Fvector>	m_Velocity;
	Fvector 		m_vLocation;

	float	 		m_fAngle;

    float			m_fAnimSpeed;
    int				m_iAnimStartFrame;
};

#endif //ParticleH

