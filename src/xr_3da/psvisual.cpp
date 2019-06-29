//----------------------------------------------------
// file: PSVisual.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "PSVisual.h"
#include "PSLibrary.h"

#define MAX_PARTICLES	1024

CPSVisual::CPSVisual():IRender_Visual()
{
	m_Definition= 0;
	m_Emitter	= 0;
    m_Particles.clear();
}
//----------------------------------------------------
CPSVisual::~CPSVisual()
{
	m_Particles.clear();
}
//----------------------------------------------------
void CPSVisual::Copy(IRender_Visual* pFrom)
{
	Debug.fatal("Can't duplicate particle system - NOT IMPLEMENTED");
}
//----------------------------------------------------
//////////////////////////////////////////////////
// - доделать рождение/умирание без удаления и добавления в массив
// 
void CPSVisual::Update(u32 dt)
{
	float fTime		= Device.fTimeGlobal;
	float dT		= float(dt)/1000.f;
	
	// calculate number of particles to destroy
	int iCount_Destroy	= 0;
	for (int i=0; i<int(m_Particles.size()); i++)
		if (fTime>m_Particles[i].m_Time.end)	iCount_Destroy++;
		
	// calculate how many particles we should create from ParticlesPerSec and time elapsed
	int iCount_Create	= m_Emitter->CalculateBirth(m_Particles.size() - iCount_Destroy, fTime, dT);
		
	// create/destroy/simulate particles that we own
	float TM	 		= fTime-dT;
	float dT_delta 		= dT/(iCount_Create?iCount_Create:1);
	float p_size		= 0;
	Fvector Pos; float size;
	vis.box.invalidate	();
	for (i=0; i<int(m_Particles.size()); i++)
	{
		if (fTime>m_Particles[i].m_Time.end) {
			// Need to destroy particle
			if (iCount_Create)	{
				// Replace
				m_Emitter->GenerateParticle(m_Particles[i], m_Definition, TM);
				TM				+= dT_delta;
				iCount_Create	-= 1;
			} else {
				// Erase
				m_Particles.erase	(m_Particles.begin()+i);
				i--;
				continue;
			}
		}
		
		// Simulate this particle
		SParticle& P	= m_Particles[i];
		float T 		= fTime-P.m_Time.start;
		float k 		= T/(P.m_Time.end-P.m_Time.start);
		
		PS::SimulatePosition(Pos, &P,T,k);		vis.box.modify		(Pos);
		PS::SimulateSize	(size,&P,k,1-k);	if (size>p_size)	p_size = size;
	}
	
	// if we need to create somewhat more particles...
	while (iCount_Create) {
		// Create
		m_Particles.push_back		(SParticle());
		SParticle& P				=	m_Particles.back();
		m_Emitter->GenerateParticle	(P, m_Definition, TM);
		TM							+=	dT_delta;
		iCount_Create				-=	1;

		// Simulate
		float T 					=	fTime-P.m_Time.start;
		float k 					=	T/(P.m_Time.end-P.m_Time.start);
		PS::SimulatePosition		(Pos, &P,T,k);		vis.box.modify		(Pos);
		PS::SimulateSize			(size,&P,k,1-k);	if (size>p_size)	p_size = size;
	}
	
	if (m_Particles.empty())	{
		vis.box.set			(m_Emitter->m_Position, m_Emitter->m_Position);
		vis.box.grow		(0.1f);
		vis.box.getsphere	(vis.sphere.P,vis.sphere.R);
	} else {
		vis.box.grow		(p_size);
		vis.box.getsphere	(vis.sphere.P,vis.sphere.R);
	}
}

//----------------------------------------------------
IC void FillSprite	(FVF::TL*& pv, const Fmatrix& M, const Fvector& pos, const Fvector2& lt, const Fvector2& rb, float radius, u32 clr, float angle, float scale, float w_2, float h_2)
{
	FVF::TL			PT;

	PT.transform	(pos, M);
	float sz		= scale * radius / PT.p.w;
	// 'Cause D3D clipping have to clip Four points
	// We can help him :)

	if (sz<1.5f)	return;
	if (PT.p.x< -1)	return;
	if (PT.p.x>  1)	return;
	if (PT.p.y< -1)	return;
	if (PT.p.y>  1)	return;
	if (PT.p.z<  0) return;
	
	// Convert to screen coords
	Fvector2	c;
	c.x				= (PT.p.x+1)*w_2;
	c.y				= (PT.p.y+1)*h_2;
	// Rotation
	float	_sin1,_cos1,_sin2,_cos2,da;
	da = angle;		 _sin1=_sin(da); _cos1=_cos(da);
	da += PI_DIV_2;  _sin2=_sin(da); _cos2=_cos(da);
	
	pv->set	(c.x+sz*_sin1,	c.y+sz*_cos1,	PT.p.z, PT.p.w, clr, lt.x,rb.y);	pv++;
	pv->set	(c.x-sz*_sin2,	c.y-sz*_cos2,	PT.p.z, PT.p.w, clr, lt.x,lt.y);	pv++;
	pv->set	(c.x+sz*_sin2,	c.y+sz*_cos2,	PT.p.z, PT.p.w, clr, rb.x,rb.y);	pv++;
	pv->set	(c.x-sz*_sin1,	c.y-sz*_cos1,	PT.p.z, PT.p.w, clr, rb.x,lt.y);	pv++;
}

IC void FillSprite	(FVF::TL*& pv, const Fmatrix& M, const Fvector& pos, const Fvector2& lt, const Fvector2& rb, float radius, u32 clr, const Fvector& D, float scale, float w_2, float h_2)
{
	Fvector			P1,P2;
    P1.mad			(pos,D,-radius);
    P2.mad			(pos,D,radius);

	FVF::TL			s1,s2;
	s1.transform	(P1,M);
	s2.transform	(P2,M);

    if ((s1.p.w<=0)||(s2.p.w<=0)) return;

	float l1		= scale*radius/s1.p.w*1.4142f;
    float l2		= scale*radius/s2.p.w*1.4142f;

	Fvector2		dir,R;
	dir.set			(s2.p.x-s1.p.x,s2.p.y-s1.p.y);
	dir.norm		();
	R.cross			(dir);

	pv->set			((s1.p.x+l1*R.x+1)*w_2,	(s1.p.y+l1*R.y+1)*h_2,	s2.p.z, s2.p.w, clr, lt.x,rb.y);	pv++;
	pv->set			((s2.p.x+l2*R.x+1)*w_2,	(s2.p.y+l2*R.y+1)*h_2,	s2.p.z, s2.p.w, clr, lt.x,lt.y);	pv++;
	pv->set			((s1.p.x-l1*R.x+1)*w_2,	(s1.p.y-l1*R.y+1)*h_2,	s2.p.z, s2.p.w, clr, rb.x,rb.y);	pv++;
	pv->set			((s2.p.x-l2*R.x+1)*w_2,	(s2.p.y-l2*R.y+1)*h_2,	s2.p.z, s2.p.w, clr, rb.x,lt.y);	pv++;
}

void CPSVisual::Render		(float LOD)
{
	u32			vOffset;
	FVF::TL*	pv			= (FVF::TL*)RCache.Vertex.Lock(m_Particles.size()*4,geom->vb_stride,vOffset);
	u32			dwCount		= RenderTO(pv);
	RCache.Vertex.Unlock(dwCount,geom->vb_stride);
	if (dwCount)    {
		RCache.set_Geometry		(geom);
		RCache.Render			(D3DPT_TRIANGLELIST,vOffset,0,dwCount,0,dwCount/2);
	}
}
 
u32 CPSVisual::RenderTO	(FVF::TL* dest)
{
	float fTime					= Device.fTimeGlobal;
	
	// build transform matrix
	Fmatrix mSpriteTransform	= Device.mFullTransform;

	float	w_2					= float(::Render->getTarget()->get_width()) / 2;
	float	h_2					= float(::Render->getTarget()->get_height()) / 2;
	float	fov_scale			= float(::Render->getTarget()->get_width()) / (Device.fFOV/90.f);
	
    int 	mb_samples 			= 1;
    float 	mb_step 			= 0;
	
	Fvector2	lt,rb;
    lt.set		(0.f,0.f);
	rb.set		(1.f,1.f);
	
	// actual rendering
	vis.box.invalidate	();
	float			p_size	= 0;
	FVF::TL*		pv_start= dest;
	FVF::TL*		pv		= pv_start;
	for (PS::ParticleIt P=m_Particles.begin(); P!=m_Particles.end(); P++)
	{
		u32 	C;
		float 	sz;
		float 	angle;
        if (m_Definition->m_Flags.is(PS_MOTIONBLUR))
		{
            float T 	=	fTime-P->m_Time.start;
            float k 	=	T/(P->m_Time.end-P->m_Time.start);
            float k_inv =	1-k;
            mb_samples 	=	iFloor(m_Definition->m_BlurSubdiv.start*k_inv+m_Definition->m_BlurSubdiv.end*k+0.5f);
            mb_step 	=	m_Definition->m_BlurTime.start*k_inv+m_Definition->m_BlurTime.end*k;
            mb_step		/=	mb_samples;
        }
		
        // update
		for (int sample=mb_samples-1; sample>=0; sample--)
		{
            float T 	=	fTime-P->m_Time.start-(sample*mb_step);
            if (T<0)		continue;
            float mb_v	=	1-float(sample)/float(mb_samples);
            float k 	=	T/(P->m_Time.end-P->m_Time.start);
			if ((m_Emitter->m_Flags.is(PS_EM_PLAY_ONCE)) && (k>1)) continue;
            float k_inv =	1-k;
			
            Fvector Pos;
            
            PS::SimulatePosition(Pos,&*P,T,k);			// this moves the particle using the last known velocity and the time that has passed
			vis.box.modify		(Pos);
            PS::SimulateColor	(C,&*P,k,k_inv,mb_v);	// adjust current Color from calculated Deltas and time elapsed.
            PS::SimulateSize	(sz,&*P,k,k_inv);		// adjust current Size & Angle
			if (sz>p_size)		p_size = sz;
			
            Fvector D;
			if (m_Definition->m_Flags.is(PS_ALIGNTOPATH)) {
				Fvector p;
                float PT = T-0.1f;
				float kk = PT/(P->m_Time.end-P->m_Time.start);
                PS::SimulatePosition(p,&*P,PT,kk);
				D.sub				(Pos,p);
                D.normalize_safe	();
				
				if (m_Definition->m_Flags.is(PS_FRAME_ENABLED)){
					int frame;
					if (m_Definition->m_Flags.is(PS_FRAME_ANIMATE))PS::SimulateAnimation(frame,m_Definition,&*P,T);
					else										frame = P->m_iAnimStartFrame;
					m_Definition->m_Animation.CalculateTC	(frame,lt,rb);
				}
				FillSprite	(pv,mSpriteTransform,Pos,lt,rb,sz*.5f,C,D,fov_scale,w_2,h_2);
			}else{
				PS::SimulateAngle	(angle,&*P,T,k,k_inv);
				
				if (m_Definition->m_Flags.is(PS_FRAME_ENABLED)){
					int frame;
					if (m_Definition->m_Flags.is(PS_FRAME_ANIMATE)) PS::SimulateAnimation(frame,m_Definition,&*P,T);
					else										frame = P->m_iAnimStartFrame;
					m_Definition->m_Animation.CalculateTC(frame,lt,rb);
				}
				FillSprite(pv,mSpriteTransform,Pos,lt,rb,sz*.5f,C,angle,fov_scale,w_2,h_2);
            }
        }
    }
	if (m_Particles.empty())	{
		vis.box.set			(m_Emitter->m_Position,m_Emitter->m_Position);
		vis.box.grow		(0.1f);
		vis.box.getsphere	(vis.sphere.P,vis.sphere.R);
	} else {
		vis.box.grow		(p_size);
		vis.box.getsphere	(vis.sphere.P,vis.sphere.R);
	}
	
	return u32(pv-pv_start);
}

//----------------------------------------------------
void CPSVisual::Compile(PS::SDef* source, PS::SEmitter* E)
{
	VERIFY				(source);
    hShader				= source->m_CachedShader;
	m_Definition		= source;
	geom.create		(FVF::F_TL, RCache.Vertex.Buffer(), RCache.QuadIB);

	// set default emitter data
	VERIFY				(E);
    m_Emitter			= E;
	m_Emitter->Compile	(&source->m_DefaultEmitter);
}
//----------------------------------------------------
