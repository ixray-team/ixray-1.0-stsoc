#include "stdafx.h"
#include "WeaponThrust.h"
#include "WeaponHUD.h"
#include "PhysicsShell.h"
#include "effectorshot.h"
#include "actor.h"

CWeaponThrust::CWeaponThrust(void) {
	m_state = MS_HIDDEN;
	m_pHUD = xr_new<CWeaponHUD>();
	m_throw = false;
	m_force = 0;
	m_minForce = 20.f;
	m_maxForce = 200.f;
	m_forceGrowSpeed = 50.f;
	m_destroyTime = 0xffffffff;
	m_stateTime = 0;
}

CWeaponThrust::~CWeaponThrust(void) {
	xr_delete(m_pHUD);
}

void CWeaponThrust::Load(LPCSTR section) {
	inherited::Load(section);
	LPCSTR hud_sect = pSettings->r_string(section,"hud");
	m_pHUD->Load(hud_sect);
	m_minForce = pSettings->r_float(section,"force_min");
	m_maxForce = pSettings->r_float(section,"force_max");
	m_forceGrowSpeed = pSettings->r_float(section,"force_grow_speed");
}

#define CHOOSE_MAX(x,inst_x,y,inst_y,z,inst_z)\
	if(x>y)\
		if(x>z){inst_x;}\
		else{inst_z;}\
	else\
		if(y>z){inst_y;}\
		else{inst_z;}

BOOL CWeaponThrust::net_Spawn(LPVOID DC) {
	BOOL l_res = inherited::net_Spawn(DC);

	R_ASSERT(!m_pInventory);
	CKinematics* V = PKinematics(Visual());
	if(V) V->PlayCycle("idle");
	setVisible					(true);
	setEnabled					(true);

	if (0==m_pPhysicsShell)
	{
		// Physics (Box)
		Fobb								obb;
		Visual()->vis.box.get_CD			(obb.m_translate,obb.m_halfsize);
		obb.m_rotate.identity				();

		// Physics (Elements)
		CPhysicsElement* E					= P_create_Element	();
		R_ASSERT							(E);
		
		Fvector ax;
		float	radius;
		CHOOSE_MAX(	obb.m_halfsize.x,ax.set(obb.m_rotate.i) ; ax.mul(obb.m_halfsize.x); radius=_min(obb.m_halfsize.y,obb.m_halfsize.z) ;obb.m_halfsize.y/=2.f;obb.m_halfsize.z/=2.f,
					obb.m_halfsize.y,ax.set(obb.m_rotate.j) ; ax.mul(obb.m_halfsize.y); radius=_min(obb.m_halfsize.x,obb.m_halfsize.z) ;obb.m_halfsize.x/=2.f;obb.m_halfsize.z/=2.f,
					obb.m_halfsize.z,ax.set(obb.m_rotate.k) ; ax.mul(obb.m_halfsize.z); radius=_min(obb.m_halfsize.y,obb.m_halfsize.x) ;obb.m_halfsize.y/=2.f;obb.m_halfsize.x/=2.f
					)
		//radius*=1.4142f;
		Fsphere sphere1,sphere2;
		sphere1.P.add						(obb.m_translate,ax);
		sphere1.R							=radius*1.4142f;

		sphere2.P.sub						(obb.m_translate,ax);
		sphere2.R							=radius/2.f;

		E->add_Box							(obb);
		E->add_Sphere						(sphere1);
		E->add_Sphere						(sphere2);
	
		// Physics (Shell)
		m_pPhysicsShell						= P_create_Shell	();
		R_ASSERT							(m_pPhysicsShell);
		m_pPhysicsShell->add_Element		(E);
		m_pPhysicsShell->setDensity			(2000.f);
		m_pPhysicsShell->Activate			(svXFORM(),0,svXFORM(),true);
		m_pPhysicsShell->mDesired.identity	();
		m_pPhysicsShell->fDesiredStrength	= 0.f;
		m_pPhysicsShell->SetAirResistance();
	}
	return l_res;
}

void CWeaponThrust::net_Destroy() {
	//R_ASSERT(!m_pInventory);
	if(m_pPhysicsShell) m_pPhysicsShell->Deactivate();
	xr_delete(m_pPhysicsShell);
	inherited::net_Destroy();
}

void CWeaponThrust::OnH_B_Chield() {
	inherited::OnH_B_Chield		();
	setVisible					(false);
	setEnabled					(false);
	if(m_pPhysicsShell) m_pPhysicsShell->Deactivate();
}

void CWeaponThrust::OnH_B_Independent() {
	inherited::OnH_B_Independent();
	if(!m_destroyTime) {
		NET_Packet			P;
		u_EventGen			(P,GE_DESTROY,ID());
		u_EventSend			(P);
		return;
	}
	//xr_delete(m_pHUD);
	setVisible					(true);
	setEnabled					(true);
	CObject*	E		= dynamic_cast<CObject*>(H_Parent());
	R_ASSERT		(E);
	svTransform.set(E->clXFORM());
	vPosition.set(svTransform.c);
	if(m_pPhysicsShell) {
		Fmatrix trans;
		Level().Cameras.unaffected_Matrix(trans);
		Fvector l_fw; l_fw.set(trans.k);// l_fw.mul(2.f);
		Fvector l_up; l_up.set(svTransform.j); l_up.mul(2.f);
		Fmatrix l_p1, l_p2;
		l_p1.set(svTransform); l_p1.c.add(l_up); l_up.mul(1.2f); //l_p1.c.add(l_fw);
		l_p2.set(svTransform); l_p2.c.add(l_up); l_fw.mul(1.f+m_force); l_p2.c.add(l_fw);
		//Log("aaa",l_p1.c);
		//Log("bbb",l_p2.c);
		//m_pPhysicsShell->Activate(l_p1, 0, l_p2);
		Fvector l_vel,a_vel;
		float fi,teta,r;
		l_vel.add(l_up,l_fw);
		fi=  ::Random.randF(0.f,2.f*M_PI);
		teta=::Random.randF(0.f,M_PI);
		r=	 ::Random.randF(2.f*M_PI,3.f*M_PI);
		float rxy=r*_sin(teta);
		a_vel.set(rxy*_cos(fi),rxy*_sin(fi),r*_cos(teta));
		//a_vel.set(::Random.randF(ri*2.f*M_PI,ri*3.f*M_PI),::Random.randF(ri*2.f*M_PI,ri*3.f*M_PI),::Random.randF(ri*2.f*M_PI,ri*3.f*M_PI));

		m_pPhysicsShell->Activate(l_p1, l_vel, a_vel);
		svTransform.set(l_p1);
		vPosition.set(svTransform.c);
	}
}

void CWeaponThrust::UpdateCL() {
	inherited::UpdateCL();
	if(m_pHUD && m_showHUD) {}
	m_stateTime += Device.dwTimeDelta;
	if(State() == MS_IDLE && m_stateTime > 10000) State(MS_PLAYING);
	if(State() == MS_READY) {
		if(m_throw) State(MS_THROW);
		else {
			m_force += (m_forceGrowSpeed * Device.dwTimeDelta) * .001f;
			if(m_force > m_maxForce) m_force = m_maxForce;
		}
	}
	if(getVisible() && m_pPhysicsShell) {
		if(m_destroyTime <= Device.dwTimeDelta) {
			m_destroyTime = 0xffffffff;
			R_ASSERT(!m_pInventory);
			Destroy();
			return;
		}
		if(m_destroyTime < 0xffffffff) m_destroyTime -= Device.dwTimeDelta;
		m_pPhysicsShell->Update	();
		svTransform.set			(m_pPhysicsShell->mXFORM);
		vPosition.set(m_pPhysicsShell->mXFORM.c);
	} else if(H_Parent()) svTransform.set(H_Parent()->clXFORM());
}

u32 CWeaponThrust::State() {
	return m_state;
}

u32 CWeaponThrust::State(u32 state) {
	m_state = state;
	m_stateTime = 0;

	u32 iRand = ::Random.randI(100);
	switch(State()) {
		case MS_SHOWING : {
			m_pHUD->animPlay(m_pHUD->animGet("draw_0"), true, this);
		} break;
		case MS_IDLE : {
			//setVisible(true);
			//setEnabled(true);
			m_pHUD->animPlay(m_pHUD->animGet("idle_0"));
		} break;
		case MS_HIDING : {
			m_pHUD->animPlay(m_pHUD->animGet("holster_0"), true, this);
		} break;
		case MS_HIDDEN : {
			setVisible(false);
			setEnabled(false);
		} break;
		case MS_THREATEN : {
			m_force = m_minForce;
			
			m_pHUD->animPlay(m_pHUD->animGet("attack_0"), true, this);
		} break;
		case MS_READY : {
			m_pHUD->animPlay(m_pHUD->animGet("idle_0"), true, this);
		} break;
		case MS_THROW : {
			m_throw = false;
			
			(iRand < 50) ?	m_pHUD->animPlay(m_pHUD->animGet("attack_0"), true, this):
							m_pHUD->animPlay(m_pHUD->animGet("attack_1"), true, this);
		} break;
		case MS_END : {
			m_pHUD->animPlay(m_pHUD->animGet("attack_1"), true, this);
		} break;
		case MS_PLAYING : {
			m_pHUD->animPlay(m_pHUD->animGet("idle_0"), true, this);
		} break;
	}
	return State();
}

void CWeaponThrust::OnVisible() {
	if(m_pHUD && H_Parent() && dynamic_cast<CActor*>(H_Parent())) {
		Fmatrix trans;
		Level().Cameras.affected_Matrix(trans);
		m_pHUD->UpdatePosition(trans);

		PKinematics(m_pHUD->Visual())->Update();
		if(m_showHUD) {
			::Render->set_Transform		(&m_pHUD->Transform());
			::Render->add_Visual		(m_pHUD->Visual());
		} else {
		}
	}
	if(getVisible() && !H_Parent()) {
		::Render->set_Transform		(&clTransform);
		::Render->add_Visual		(Visual());
	}
}

void CWeaponThrust::Show() {
	State(MS_SHOWING);
}

void CWeaponThrust::Hide() {
	State(MS_HIDING);
}

void CWeaponThrust::Throw() {
	//// Camera
	//if (m_showHUD) {
	//	CEffectorShot* S		= dynamic_cast<CEffectorShot*>	(Level().Cameras.GetEffector(cefShot)); 
	//	if (!S)	S				= (CEffectorShot*)Level().Cameras.AddEffector(xr_new<CEffectorShot>(1.f,5.5f));
	//	R_ASSERT				(S);
	//	S->Shot					(.1f);
	//}
}

void CWeaponThrust::Destroy() {
	NET_Packet			P;
	u_EventGen			(P,GE_DESTROY,ID());
	u_EventSend			(P);
}

void CWeaponThrust::OnAnimationEnd() {
	switch(State()) {
		case MS_HIDING : {
			State(MS_HIDDEN);
		} break;
		case MS_SHOWING : {
			State(MS_IDLE);
		} break;
		case MS_THREATEN : {
			if(m_throw) State(MS_THROW); else State(MS_READY);
		} break;
		case MS_THROW : {
			Throw();
			State(MS_END);
		} break;
		case MS_END : {
			State(MS_SHOWING);
		} break;
		case MS_PLAYING : {
			State(MS_IDLE);
		} break;
	}
}

bool CWeaponThrust::Action(s32 cmd, u32 flags) {
	if(inherited::Action(cmd, flags)) return true;
	switch(cmd) {
		case kWPN_FIRE : {
			if(flags&CMD_START) {
				 m_throw = false;
				if(State() == MS_IDLE) State(MS_THREATEN);
			} else if(State() == MS_READY || State() == MS_THREATEN) {
				m_throw = true; if(State() == MS_READY) State(MS_THROW);
			}
		} return true;
	}
	return false;
}

void CWeaponThrust::SoundCreate(sound& dest, LPCSTR s_name, int iType, BOOL bCtrlFreq) {
	string256	name,temp;
	strconcat	(name,"weapons\\",Name(),"_",s_name,".wav");
	if (FS.exist(temp,"$game_sounds$",name)) 
	{
		dest.create		(TRUE,name,iType);
		return;
	}
	strconcat	(name,"weapons\\","generic_",s_name,".wav");
	if (FS.exist(temp,"$game_sounds$",name))	
	{
		dest.create		(TRUE,name,iType);
		return;
	}
	Debug.fatal	("Can't find sound '%s' for weapon '%s'", name, Name());
}

void CWeaponThrust::SoundDestroy(sound& dest) {
	dest.destroy();
}
