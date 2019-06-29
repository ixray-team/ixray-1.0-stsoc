#pragma once

#include "phcharacter.h"

class CPHWheeledCharacter : public CPHCharacter {

	dBodyID m_wheel_body;


	////////////////////////////
	dJointID m_wheel_joint;
	/////////////////////////////

	CPHWheeledCharacter();

	/////////////////CPHObject//////////////////////////////////////////////
	virtual		void		PhDataUpdate						(dReal /**step/**/)	{}	;
	virtual		void		PhTune								(dReal /**step/**/)	{}	;
	virtual		void		InitContact							(dContact* /**c/**/,bool	&do_collide)	{}	;
	/////////////////CPHCharacter////////////////////////////////////////////
public:
	virtual		EEnvironment CheckInvironment					()				{ return peOnGround; }	;
	virtual		bool		ContactWas							(){return false;//b_meet;
	}	;
	virtual		void		Create							  (dVector3 sizes);
	virtual		void		Destroy								(void)				;
	virtual		void		SetAcceleration						(Fvector /**accel/**/)	{}	;
	virtual		void		SetMaterial							(u16	/**material/**/){}	;
	virtual		void		SetPosition							(Fvector /**pos/**/)	{}	;
	virtual		void		SetAirControlFactor					(float /**factor/**/)	{}	;
	virtual		void		GetVelocity							(Fvector& /**vel/**/)	{}	;
	virtual		void		SetVelocity							(Fvector /**vel/**/)	{}	;
	virtual		void		GetPosition							(Fvector& /**pos/**/)	{}	;
	virtual		float		FootRadius							()			 {return 0.f;}	;
	virtual		void		SetMas								(dReal mass)		;
	virtual		bool		TryPosition							(Fvector /**pos/**/)	{return false;};
	virtual		void		SetPhysicsRefObject					(CGameObject* /**ref_object/**/){};
	virtual		void		AddControlVel						(const Fvector& vel){};
};

