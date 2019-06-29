#ifndef ParticleSystemH
#define ParticleSystemH

#include "..\WaveForm.h"
#include "Particle.h"

#include <time.h>

namespace PS
{
	DEFINE_VECTOR	(SParticle,Particles,ParticleIt);

#define PS_COLLIDE	 				0x00000001
#define PS_MOTIONBLUR				0x00000002
#define PS_FRAME_ENABLED			0x00000004
#define PS_FRAME_ANIMATE			0x00000008
#define PS_FRAME_RND_INIT			0x00000010
#define PS_ALIGNTOPATH				0x00000020
#define PS_RND_INIT_ANGLE			0x00000040
#define PS_FRAME_RND_PLAYBACK_DIR	0x00000080
#define PS_EM_BIRTHFUNC				0x00000001
#define PS_EM_BURST					0x00000002
#define PS_EM_PLAY_ONCE				0x00000004

#define PARTICLESYSTEM_VERSION		0x0011

#ifdef _EDITOR
	class PropItemVec;
#endif

#pragma pack( push,1 )
	struct  SEmitterDef
	{
		enum EEmitterType{
			emPoint=0,
			emCone,
			emSphere,
			emBox,
			emUser,
			emForceDWORD = u32(-1)
		};
		EEmitterType	m_EmitterType;
		Fvector			m_Position;
		// cone params
		Fvector			m_ConeHPB;
		Fvector			m_ConeDirection;
		float			m_ConeAngle;
		// sphere params
		float			m_SphereRadius;
		// box
		Fvector			m_BoxSize;
		// birth params
		float		 	m_fBirthRate;
		WaveForm		m_BirthFunc;
		float			m_ParticleLimit;

		Flags32			m_Flags;
		void			Reset()			{ ZeroMemory(this, sizeof(SEmitterDef));}
		void			InitDefault()
		{
			Reset		();
			m_fBirthRate 	= 100;
			m_ParticleLimit = 1000;
			m_ConeHPB.set	(0.f,-PI_DIV_2,0.f);
			m_BoxSize.set	(1.f,1.f,1.f);
			m_SphereRadius 	= 1.f;
			UpdateConeOrientation();
		}
		void			UpdateConeOrientation()
		{
			m_ConeDirection.set(0.f,0.f,1.f);
			Fmatrix M;
			M.setHPB(m_ConeHPB.x,m_ConeHPB.y,m_ConeHPB.z);
			M.transform_tiny(m_ConeDirection);
		}
#ifdef _EDITOR
		void 			FillProp(LPCSTR pref, PropItemVec& items);
#endif
	};

	struct  SAnimation
	{
		Fvector2		m_TexSize;
		Fvector2		m_FrameSize;
		int     		m_iFrameDimX;
		int				m_iFrameCount;
		float			m_fSpeed;
		float			m_fSpeedVar;

		IC void Reset      ()	{ZeroMemory(this,sizeof(SAnimation));}
		IC void InitDefault()
		{
			m_FrameSize.set(32.f,64.f);
			m_TexSize.set(32.f/256.f,64.f/128.f);
			m_iFrameDimX = 8;
			m_iFrameCount= 16;
			m_fSpeed	 = 24;
			m_fSpeedVar	 = 0;
		}
		IC void			SetTextureSize(float w, float h, float fw, float fh)
		{
			m_FrameSize.set(fw,fh);
			m_TexSize.set(fw/w,fh/h);
			m_iFrameDimX = iFloor(1.f/m_TexSize.x);
		}
		IC void         CalculateTC(int frame, Fvector2& lt, Fvector2& rb)
		{
			lt.x        = (frame%m_iFrameDimX)*m_TexSize.x;
			lt.y        = (frame/m_iFrameDimX)*m_TexSize.y;
			rb.x        = lt.x+m_TexSize.x;
			rb.y        = lt.y+m_TexSize.y;
		}
#ifdef _EDITOR
		void 			FillProp(LPCSTR pref, PropItemVec& items);
#endif
	};

	struct  	SParams
	{
		string64			m_Name;
		// params
		float			m_fLife;
		Pair<float>		m_Size;
		Pair<float>		m_Speed;
		Pair<Fvector>	m_Gravity;
		Pair<Fcolor> 	m_Color;
		Pair<float> 	m_ASpeed;

		float			m_fLifeVar;
		float			m_fSizeVar;
		float			m_fSpeedVar;
		Fcolor 			m_ColorVar;
		float			m_fASpeedVar;

		Pair<float>	 	m_BlurTime;
		Pair<float>		m_BlurSubdiv;

		// animation
		SAnimation		m_Animation;

		//member variables
		Flags32			m_Flags;
#ifdef _EDITOR
		void 			FillProp(LPCSTR pref, PropItemVec& items);
#endif
	};

	struct  SDef: public SParams
	{
		ref_shader			m_CachedShader;
		u8					m_Reserved	[60];
		string64			m_Computer;
		string64			m_ShaderName;
		string64			m_TextureName;
		u32  				m_dwCreationTime;
		SEmitterDef			m_DefaultEmitter;

		void 				Reset()
		{
			m_Reserved[0]	= 0;
			m_Computer[0]	= 0;
			m_ShaderName[0]	= 0;
			m_TextureName[0]= 0;
			m_dwCreationTime= 0;
			m_DefaultEmitter.Reset();
		}
		void				InitDefault()
		{
			Reset		();

			m_Animation.InitDefault();

			m_Flags.assign		(PS_RND_INIT_ANGLE);

			m_fLife		 		= 5.600000f;
			m_fLifeVar 			= 0.100000f;
			m_Size.start 		= 0.10000f;
			m_Size.end 			= 1.08000f;
			m_fSizeVar 			= 0.0400000f;
			m_Speed.start		= 3.00000f;
			m_Speed.end			= 3.00000f;
			m_fSpeedVar 		= 0.0000f;
			m_ASpeed.start		= 0;
			m_ASpeed.end		= 0;
			m_fASpeedVar   		= 0;


			m_Gravity.start.set	(0.f,20.8f,0.f);
			m_Gravity.end.set	(0.f,-40.8f,0.f);

			m_Color.start.set	(128,128,128,255);
			m_Color.end.set		(128,128,128,0);
			m_ColorVar.set		(0.150000f,0.150000f,0.150000f,0.140000f);

			m_BlurTime.start	= 0.01f;
			m_BlurTime.end		= 0.01f;
			m_BlurSubdiv.start	= 5;
			m_BlurSubdiv.end	= 5;

			m_DefaultEmitter.InitDefault();
		}
		BOOL				SetName(const char* N)
		{
			// Name
			VERIFY(xr_strlen(N)<64);

			string64 new_name;
			strcpy(new_name,N);
			strlwr(new_name);
			if (xr_strcmp(m_Name,new_name)!=0){
				strcpy(m_Name,new_name);

				// Computer
				strcpy(m_Computer,Core.CompName);
				strlwr(m_Computer);

				// Time
				_tzset(); time( (long*)&m_dwCreationTime );
				return true;
			}
			return false;
		};
		void				SetShader(const char* sh)
		{
			VERIFY(xr_strlen(sh)<64);
			strcpy(m_ShaderName,sh);
		}
		void				SetTexture(const char* tx)
		{
			VERIFY(xr_strlen(tx)<64);
			strcpy(m_TextureName,tx);
		}
#ifdef _EDITOR
		void 				FillProp(LPCSTR pref, PropItemVec& items);
#endif
	};
#pragma pack( pop )

	struct  SEmitter : public SEmitterDef
	{
		float			m_fEmissionResidue;
		BOOL            m_bPlaying;
		int 			m_iPlayResidue;
	public:
		SEmitter(){
			m_fEmissionResidue 	= 0;
			m_bPlaying          = false;
			m_iPlayResidue		= 0;
		}
		IC BOOL IsPlaying(){
			if (m_Flags.is(PS_EM_PLAY_ONCE))	return (m_iPlayResidue!=0);
			else								return m_bPlaying;
		}
		IC void Play(){
			m_bPlaying 		= true;
			if (m_Flags.is(PS_EM_PLAY_ONCE))
				m_iPlayResidue = iFloor(m_ParticleLimit);
		}
		IC void Stop(){
			m_bPlaying 			= false;
			m_fEmissionResidue 	= 0;
		}
		IC void Compile(SEmitterDef* E){
			*((SEmitterDef*)this) = *E;
		}
		IC void GenerateParticle(SParticle& P, SParams* m_Definition, float fTime){
			P.m_Time.start 		= fTime;
			P.m_Time.end		= fTime+m_Definition->m_fLife+Random.randF(-0.5f,0.5f)*m_Definition->m_fLifeVar*m_Definition->m_fLife;

			// our start color is going to be the System's StartColor + the System's color variation
			P.m_Color.start.r 	= m_Definition->m_Color.start.r*(1.f-m_Definition->m_ColorVar.r) + m_Definition->m_ColorVar.r * Random.randF(255);
			P.m_Color.start.g 	= m_Definition->m_Color.start.g*(1.f-m_Definition->m_ColorVar.g) + m_Definition->m_ColorVar.g * Random.randF(255);
			P.m_Color.start.b 	= m_Definition->m_Color.start.b*(1.f-m_Definition->m_ColorVar.b) + m_Definition->m_ColorVar.b * Random.randF(255);
			P.m_Color.start.a 	= m_Definition->m_Color.start.a*(1.f-m_Definition->m_ColorVar.a) + m_Definition->m_ColorVar.a * Random.randF(255);
			P.m_Color.end.r 	= m_Definition->m_Color.end.r*(1.f-m_Definition->m_ColorVar.r) + m_Definition->m_ColorVar.r * Random.randF(255);
			P.m_Color.end.g 	= m_Definition->m_Color.end.g*(1.f-m_Definition->m_ColorVar.g) + m_Definition->m_ColorVar.g * Random.randF(255);
			P.m_Color.end.b 	= m_Definition->m_Color.end.b*(1.f-m_Definition->m_ColorVar.b) + m_Definition->m_ColorVar.b * Random.randF(255);
			P.m_Color.end.a 	= m_Definition->m_Color.end.a;
			// clamp any overflow
			clamp(P.m_Color.start.a, 	0.f,255.f);
			clamp(P.m_Color.start.r, 	0.f,255.f);
			clamp(P.m_Color.start.g, 	0.f,255.f);
			clamp(P.m_Color.start.b, 	0.f,255.f);
			clamp(P.m_Color.end.a,	 	0.f,255.f);
			clamp(P.m_Color.end.r,		0.f,255.f);
			clamp(P.m_Color.end.g,   	0.f,255.f);
			clamp(P.m_Color.end.b,   	0.f,255.f);

			//Start Size and End Size then delta and clamping
			P.m_Size.start	= m_Definition->m_Size.start + Random.randF(-0.5f,0.5f)*m_Definition->m_fSizeVar*m_Definition->m_Size.start;
			P.m_Size.end 	= m_Definition->m_Size.end   + Random.randF(-0.5f,0.5f)*m_Definition->m_fSizeVar*m_Definition->m_Size.end;

			//GRAVITY
			// It's a percentage of normal gravity.
			P.m_GravityStart.set(m_Definition->m_Gravity.start);
			P.m_GravityLambda.sub(m_Definition->m_Gravity.end,P.m_GravityStart);
			P.m_GravityLambda.div(m_Definition->m_fLife);

			//calculate startup position & velocity
			GeneratePosAndDir(P.m_vLocation,P.m_Velocity.start);

			// Multiply Velocity by speed
			float rel = m_Definition->m_Speed.end/m_Definition->m_Speed.start;
			float start_velocity = m_Definition->m_Speed.start + Random.randF(-0.5f,0.5f)*m_Definition->m_fSpeedVar*m_Definition->m_Speed.start;
			P.m_Velocity.start.mul(start_velocity);
			P.m_Velocity.end.mul(P.m_Velocity.start,rel);

			int sign[2]		= {-1,1};
			// Angular
			if (m_Definition->m_Flags.is(PS_ALIGNTOPATH)){
				P.m_fAngle 		= 0;
				P.m_ASpeed.start= 0;
				P.m_ASpeed.end	= 0;
			}else{
				P.m_fAngle = (m_Definition->m_Flags.is(PS_RND_INIT_ANGLE))?Random.randF(-PI,PI):-PI_DIV_4;
				int iSide 		= sign[Random.randI(2)];
				P.m_ASpeed.start= iSide*m_Definition->m_ASpeed.start * (1+Random.randF(-0.5f,0.5f)*m_Definition->m_fASpeedVar);
				P.m_ASpeed.end	= iSide*m_Definition->m_ASpeed.end * (1+Random.randF(-0.5f,0.5f)*m_Definition->m_fASpeedVar);
			}

			// Animation
			if (m_Definition->m_Flags.is(PS_FRAME_ENABLED)){
				if (m_Definition->m_Flags.is(PS_FRAME_RND_INIT))
					P.m_iAnimStartFrame	= Random.randI(m_Definition->m_Animation.m_iFrameCount);
				if (m_Definition->m_Flags.is(PS_FRAME_ANIMATE)){
					int iSide 		= (m_Definition->m_Flags.is(PS_FRAME_RND_PLAYBACK_DIR))?sign[Random.randI(2)]:1;
					P.m_fAnimSpeed	= iSide*m_Definition->m_Animation.m_fSpeed * (1+Random.randF(-0.5f,0.5f)*m_Definition->m_Animation.m_fSpeedVar);
				}
			}
		}

		IC void	GeneratePosAndDir(Fvector& pos, Fvector& dir)
		{
			switch (m_EmitterType){
	case emPoint:
		pos.set			(m_Position);
		dir.random_dir	();
		break;
	case emCone:
		pos.set			(m_Position);
		dir.random_dir	(m_ConeDirection,m_ConeAngle);
		break;
	case emSphere:
		pos.random_point(m_SphereRadius);
		pos.add			(m_Position);
		dir.random_dir	();
		break;
	case emBox:
		pos.random_point(m_BoxSize);
		pos.add			(m_Position);
		dir.random_dir	();
		break;
			}
		}

		IC int		CalculateBirth(int p_present, float fTime, float dT)
		{
			if (!m_bPlaying) return 0;
			// calculate how many particles we should create from ParticlesPerSec and time elapsed taking the
			// previous frame's EmissionResidue into account.
			float fBK = (m_Flags.is(PS_EM_BIRTHFUNC))?m_BirthFunc.Calculate(fTime):1;
			float fParticlesNeeded = m_fBirthRate * fBK * dT + m_fEmissionResidue;

			// cast the float fparticlesNeeded to a INT to see how many particles we really need to create.
			int iParticlesCreated = iFloor(fParticlesNeeded);

			if (p_present+iParticlesCreated>=m_ParticleLimit)
				iParticlesCreated = iFloor(m_ParticleLimit)-p_present;
			if (iParticlesCreated<0) iParticlesCreated = 0;

			// burst effect
			if (m_Flags.is(PS_EM_BURST)){
				if ((p_present==0)&&(iParticlesCreated>=m_ParticleLimit))	m_fEmissionResidue 	= 0;
				else{
					m_fEmissionResidue = fParticlesNeeded;
					iParticlesCreated = 0;
				}
			}else
				m_fEmissionResidue = fParticlesNeeded - iParticlesCreated;

			// play once
			if (m_Flags.is(PS_EM_PLAY_ONCE)){
				m_iPlayResidue -= iParticlesCreated;
				if (m_iPlayResidue<=0){
					iParticlesCreated+=m_iPlayResidue;
					if (iParticlesCreated<0) iParticlesCreated = 0;
					Stop();
				}
			}

			return iParticlesCreated;
		}
	};

	IC void SimulatePosition(Fvector& pos, const SParticle* P, float T, float k)
	{
		// this moves the particle using the last known velocity and the time that has passed
		Fvector 	vel;
		vel.lerp	(P->m_Velocity.start,P->m_Velocity.end,k);

		float tt_6	= T*T/6;
		pos.x 		= P->m_vLocation.x+vel.x*T+(3*P->m_GravityStart.x+P->m_GravityLambda.x*T)*tt_6;
		pos.y 		= P->m_vLocation.y+vel.y*T+(3*P->m_GravityStart.y+P->m_GravityLambda.y*T)*tt_6;
		pos.z 		= P->m_vLocation.z+vel.z*T+(3*P->m_GravityStart.z+P->m_GravityLambda.z*T)*tt_6;
	}
	IC void SimulateColor(u32& C, const SParticle* P, float k, float k_inv, float alpha_factor)
	{
		// adjust current Color from calculated Deltas and time elapsed.
		int r = iFloor(P->m_Color.start.r*k_inv+P->m_Color.end.r*k);				clamp(r,0,255);
		int g = iFloor(P->m_Color.start.g*k_inv+P->m_Color.end.g*k);				clamp(g,0,255);
		int b = iFloor(P->m_Color.start.b*k_inv+P->m_Color.end.b*k);				clamp(b,0,255);
		int a = iFloor((P->m_Color.start.a*k_inv+P->m_Color.end.a*k)*alpha_factor);	clamp(a,0,255);
		C = color_rgba(r,g,b,a);
	}
	IC void SimulateSize(float& sz, const SParticle* P, float k, float k_inv)
	{
		// adjust current Size & Angle
		sz 			= P->m_Size.start*k_inv+P->m_Size.end*k;
	}
	IC void SimulateAngle(float& angle, const SParticle* P, float T, float k, float k_inv)
	{
		float ang_vel = P->m_ASpeed.start*k_inv+P->m_ASpeed.end*k;
		angle 		= P->m_fAngle+ang_vel*T;
	}
	IC void SimulateAnimation(int& frame, SParams* def, const SParticle* P, float T)
	{
		frame = iFloor(P->m_fAnimSpeed*T)+P->m_iAnimStartFrame;
		frame %=def->m_Animation.m_iFrameCount;
	}
	DEFINE_VECTOR		(PS::SDef, PSVec, PSIt);
}
#endif //ParticleSystemH
