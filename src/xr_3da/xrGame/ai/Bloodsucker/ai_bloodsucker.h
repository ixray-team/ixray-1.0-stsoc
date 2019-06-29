#pragma once

#include "../biting/ai_biting.h"
#include "../ai_monster_bones.h"
#include "../invisibility.h"

class CAI_Bloodsucker : public CAI_Biting, 
						public CInvisibility {

	typedef		CAI_Biting	inherited;
	
	
	static	void __stdcall	BoneCallback			(CBoneInstance *B);
			void			vfAssignBones			();
			void			LookDirection			(Fvector to_dir, float bone_turn_speed);


	bonesManipulation		Bones;

	CBoneInstance			*bone_spine;
	CBoneInstance			*bone_head;

	SMotionVel				invisible_vel;
	LPCSTR					invisible_particle_name;
	
public:
							CAI_Bloodsucker	();
	virtual					~CAI_Bloodsucker();	

	virtual void			reinit					();
	virtual	void			reload					(LPCSTR section);

	virtual void			UpdateCL				();
	virtual void			shedule_Update			(u32 dt);

	virtual void			Die						();

	virtual void			StateSelector			();
	virtual BOOL			net_Spawn				(LPVOID DC);
	virtual void			net_Destroy				();

	virtual	void			Load					(LPCSTR section);
	virtual	void			LookPosition			(Fvector to_point, float angular_speed = PI_DIV_3);
	virtual	void			PitchCorrection			() {}

	virtual	void			CheckSpecParams			(u32 spec_params);
	virtual bool			ability_invisibility	() {return true;}

			
	// PP Effector		
	
			void			LoadEffector			(LPCSTR section);	
			void			ActivateEffector		(float life_time);
			
			SPPInfo			pp_effector;


	virtual	void			on_activate				();
	virtual	void			on_deactivate			();
	virtual	void			on_change_visibility	(bool b_visibility);

};