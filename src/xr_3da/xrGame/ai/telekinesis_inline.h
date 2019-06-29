#pragma once

template <typename CMonster>
CTelekinesis<CMonster>::CTelekinesis()
{
	active = false;
}

template <typename CMonster>
CTelekinesis<CMonster>::~CTelekinesis()
{

}

template <typename CMonster>
void CTelekinesis<CMonster>::InitExtern(CMonster *pM, float s, float h, u32 keep_time)
{
	monster			= pM;
	strength		= s;
	height			= h;
	max_time_keep	= keep_time;
}

template <typename CMonster>
void CTelekinesis<CMonster>::Activate()
{
	if (active)		return;
	VERIFY(objects.empty());

	active = true;

	// set object list
	Level().ObjectSpace.GetNearest		(monster->Position(),10.f); 
	xr_vector<CObject*> &tpNearest		= Level().ObjectSpace.q_nearest; 

//	float ar_linear		= 0.f;
//	float ar_angular	= 0.f;

	for (u32 i = 0; i < tpNearest.size(); i++) {
		CGameObject *obj = dynamic_cast<CGameObject *>(tpNearest[i]);
		if (!obj || !obj->m_pPhysicsShell) continue;
		objects.push_back(CTelekineticObject().set(obj,TS_Raise));
		obj->m_pPhysicsShell->set_ApplyByGravity(FALSE);

		//if (fis_zero(ar_linear)) obj->m_pPhysicsShell->GetAirResistance(ar_linear, ar_angular);
		//obj->m_pPhysicsShell->SetAirResistance(ar_linear, ar_angular);
		//obj->m_pPhysicsShell->SetAirResistance(EPS_L, EPS_L);
	}

	if (!objects.empty()) monster->CPHUpdateObject::Activate();
}

template <typename CMonster>
void CTelekinesis<CMonster>::Deactivate()
{
	active			= false;

	for (u32 i = 0; i < objects.size(); i++) {
		Release(objects[i]);
		//objects[i].obj->m_pPhysicsShell->SetAirResistance();
	}

	objects.clear	();

	monster->CPHUpdateObject::Deactivate();
}

template <typename CMonster>
void CTelekinesis<CMonster>::Raise(CTelekineticObject &obj, float power) 
{
	Fvector dir;
	dir.set(0.f,1.0f,0.f);
	dir.mul(0.5f/fixed_step);
	obj.obj->m_pPhysicsShell->applyGravityAccel(dir);	
	//obj.obj->m_pPhysicsShell->applyImpulse(dir, 2.0f * obj.obj->m_pPhysicsShell->getMass());

	//VelocityCorrection(obj.obj->m_pPhysicsShell, 1.5f);
}

#define KEEP_IMPULSE_UPDATE 200

template <typename CMonster>
void CTelekinesis<CMonster>::Keep(CTelekineticObject &obj)
{
	// check last update
	if (obj.last_time_updated + KEEP_IMPULSE_UPDATE > Level().timeServer()) return;

	// check height
	float cur_y		= obj.obj->Position().y;
	float delta_h	= _abs(obj.start_y - cur_y);

	// set dir according current height
	Fvector dir;
	if (delta_h > height+ 0.1f)			dir.set(0.f,-1.0f,0.f);
	else if (delta_h < height+ 0.1f)	dir.set(0.f,1.0f,0.f);
	else {
		dir.set(Random.randF(-1.0f,1.0f), Random.randF(-1.0f,1.0f), Random.randF(-1.0f,1.0f));
		dir.normalize_safe();
	}
	
	dir.mul(0.5f/fixed_step);
	obj.obj->m_pPhysicsShell->applyGravityAccel(dir);	
	VelocityCorrection						(obj.obj->m_pPhysicsShell, 0.0005f);

	obj.last_time_updated = Level().timeServer();
}

template <typename CMonster>
void CTelekinesis<CMonster>::Release(CTelekineticObject &obj)
{
	Fvector dir_inv;
	dir_inv.set(0.f,-1.0f,0.f);

	obj.obj->m_pPhysicsShell->set_ApplyByGravity(TRUE);
	obj.obj->m_pPhysicsShell->applyImpulse(dir_inv, 0.5f * obj.obj->m_pPhysicsShell->getMass());
}

template <typename CMonster>
void CTelekinesis<CMonster>::Throw(const Fvector &target)
{
	if (!active) return;

	for (u32 i = 0; i < objects.size(); i++) {
		Throw(objects[i], target);
	}

	Deactivate();
}


template <typename CMonster>
void CTelekinesis<CMonster>::Throw(CTelekineticObject &obj, const Fvector &target)
{
	// calc dir
	Fvector dir;
	dir.sub(target,obj.obj->Position());
	dir.normalize();

	// just throw here
	obj.obj->m_pPhysicsShell->set_ApplyByGravity(TRUE);
	obj.obj->m_pPhysicsShell->applyImpulse(dir, 10.0f * obj.obj->m_pPhysicsShell->getMass());
}


template <typename CMonster>
void CTelekinesis<CMonster>::UpdateCL(float dt)
{
	if (!active) return;

	for (u32 i = 0; i < objects.size(); i++) {
		switch (objects[i].state) {
		case TS_Raise:	Raise(objects[i], strength * dt); break;
		case TS_Keep:	Keep(objects[i]); break;
		case TS_None:	break;
		}
	}
}

template <typename CMonster>
void CTelekinesis<CMonster>::UpdateSched()
{
	if (!active) return;
	
	// обновить состояние объектов
	for (u32 i = 0; i < objects.size(); i++) {
		CTelekineticObject *cur_obj = &objects[i]; 
		switch (cur_obj->state) {
		case TS_Raise: 
			{			
				float cur_y		= cur_obj->obj->Position().y;
				float delta_h	= _abs(cur_obj->start_y - cur_y);
				
				if (delta_h > height) { // начать удержание предмета
					cur_obj->state				= TS_Keep;
					cur_obj->time_state_started = Level().timeServer();
					cur_obj->last_time_updated	= 0;
				}
			}		 
			break;
		case TS_Keep:
			if (cur_obj->time_state_started + max_time_keep < Level().timeServer()) {

				Release(*cur_obj);

				// удалить объект из массива
				if (objects.size() > 1) {
					if (i != (objects.size()-1)) objects[i] = objects.back();
					objects.pop_back();
				} else {
					objects.clear();
					active = false;
				}
				
			}
			break;
		case TS_None: continue; 
		}
	}
}

template <typename CMonster>
void CTelekinesis<CMonster>::VelocityCorrection(CPhysicsShell *pShell, float max_val)
{
	Fvector vel;
	pShell->get_LinearVel(vel); 
	
	//Msg("Cur Vel = [%f,%f,%f] mag = [%f]", VPUSH(vel), vel.magnitude());

	if (vel.magnitude() > max_val) {
		vel.normalize();
		vel.mul(max_val);
		pShell->set_LinearVel(vel);
		//Msg("New Vel Set = [%f,%f,%f] mag = [%f]", VPUSH(vel), vel.magnitude());
	}
}

template <typename CMonster>
void  CTelekinesis<CMonster>::PhTune(dReal step)
{
	for (u32 i = 0; i < objects.size(); i++) {
		switch (objects[i].state) {
		case TS_Raise:	
		case TS_Keep:	objects[i].obj->m_pPhysicsShell->Enable();
				
		case TS_None:	break;
		}
	}
}