// WeaponLightGrenade.cpp:	объект для эмулирования подствольной гранаты
//							которая свется (типа ракетницы) 	
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WeaponLightGrenade.h"


CWeaponLightGrenade::CWeaponLightGrenade()
{
	light_render				= ::Render->light_create();
	light_render->set_type		(IRender_Light::SPOT);
	light_render->set_shadow	(true);
	glow_render					= ::Render->glow_create();
}

CWeaponLightGrenade::~CWeaponLightGrenade()
{
	::Render->light_destroy	(light_render);
	::Render->glow_destroy	(glow_render);
}

void CWeaponLightGrenade::Load(LPCSTR section)
{
	inherited::Load(section);
}

BOOL CWeaponLightGrenade::net_Spawn(LPVOID DC) 
{
	BOOL result = inherited::net_Spawn(DC);

	light_render->set_active(true);
	glow_render->set_active	(true);

	return result;
}


void CWeaponLightGrenade::net_Destroy() 
{
	light_render->set_active(false);
	glow_render->set_active	(false);

	inherited::net_Destroy();
}

/*
void CWeaponLightGrenade::UpdateCL()
{
	inherited::UpdateCL();

	//родителя у нас быть не может 
	VERIFY(0 == H_Parent());

	CBoneInstance& BI = PKinematics(Visual())->LL_GetBoneInstance(guid_bone);
	Fmatrix M;

	
		PKinematics(H_Parent()->Visual())->Calculate();
		M.mul						(XFORM(),BI.mTransform);
		light_render->set_rotation	(M.k,M.i);
		light_render->set_position	(M.c);
		glow_render->set_position	(M.c);
		glow_render->set_direction	(M.k);
	}
	else
		if(getVisible() && m_pPhysicsShell) {
			M.mul					(XFORM(),BI.mTransform);

			if (light_render->get_active()){
				light_render->set_rotation	(M.k,M.i);
				light_render->set_position	(M.c);
				glow_render->set_position	(M.c);
				glow_render->set_direction	(M.k);
				
				time2hide			-= Device.fTimeDelta;
				if (time2hide<0){
					light_render->set_active(false);
					glow_render->set_active(false);
				}
			}
		} 
	
	// update light source
	if (light_render->get_active()){
		// calc color animator
		if (lanim){
			int frame;
			// возвращает в формате BGR
			u32 clr			= lanim->Calculate(Device.fTimeGlobal,frame); 
		
			Fcolor			fclr;
			fclr.set		((float)color_get_B(clr),(float)color_get_G(clr),(float)color_get_R(clr),1.f);
			fclr.mul_rgb	(fBrightness/255.f);
			light_render->set_color(fclr);
			glow_render->set_color(fclr);
		}
	}
}

*/