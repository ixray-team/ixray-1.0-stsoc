#include "stdafx.h"
#include "HelicopterMovementManager.h"
#include "Helicopter.h"
#include "level.h"

#ifdef MOV_MANAGER_OLD

bool time_lesser (const CHelicopterMovementManager::STravelPathPoint& el1, 
				  const CHelicopterMovementManager::STravelPathPoint& el2)
{
	return el1.time < el2.time;
}

CHelicopterMovementManager::CHelicopterMovementManager()
{
}

CHelicopterMovementManager::~CHelicopterMovementManager()
{
	deInit();
}

#ifdef DEBUG
void CHelicopterMovementManager::OnRender()
{
	/*
	float path_box_size = .105f;
	Fvector pos=helicopter()->m_left_rocket_bone_xform.c;
	RCache.dbg_DrawAABB  (pos,path_box_size,path_box_size,path_box_size,D3DCOLOR_XRGB(0,255,0));

	pos=helicopter()->m_right_rocket_bone_xform.c;
	RCache.dbg_DrawAABB  (pos,path_box_size,path_box_size,path_box_size,D3DCOLOR_XRGB(0,255,0));
	*/
	/*	return;
	Fvector pos, prev_pos;
	float path_box_size = .105f;

	RCache.OnFrameEnd	();


	for(u32 i=0;i<m_path.size();++i) {

	pos = m_path[i].position;
	(i!=0)?prev_pos = m_path[i-1].position:prev_pos=pos;


	RCache.dbg_DrawAABB  (pos,path_box_size,path_box_size,path_box_size,D3DCOLOR_XRGB(0,255,0));


	RCache.dbg_DrawLINE  (Fidentity,prev_pos,pos,D3DCOLOR_XRGB(0,255,0));

	}
	*/
	/*	for(u32 i=0;i<m_keyTrajectory.size();++i) {
	pos = m_keyTrajectory[i].position;
	RCache.dbg_DrawAABB  (pos,traj_box_size,traj_box_size,traj_box_size,D3DCOLOR_XRGB(255,255,0));
	}*/

}
#endif //DEBUG


void CHelicopterMovementManager::init(CHelicopter* heli)
{
	m_pHelicopter = heli;
#ifdef DEBUG
	Device.seqRender.Add(this,REG_PRIORITY_LOW-1);
#endif
}

void CHelicopterMovementManager::deInit()
{
#ifdef DEBUG
	Device.seqRender.Remove(this);
#endif
}

void CHelicopterMovementManager::onFrame(Fmatrix& xform, 
										 float fTimeDelta)
{

	switch ( helicopter()->state() ) {
	case CHelicopter::eIdleState: 
		{
			Fmatrix M, D, R;
			D.set( xform );
			M.rotateY(0.0005f);
			R.mul(D, M);
			xform.set( R );
			break;
		}

	case CHelicopter::eMovingByPatrolZonePath:
	case CHelicopter::eMovingByAttackTraj:
	case CHelicopter::eMovingToWaitPoint:
	case CHelicopter::eInitiateHunt:
	case CHelicopter::eInitiateHunt2:
		{
			Fvector pos,w;
			pos.set(0.0f,0.0f,0.0f);
			w.set(0.0f,0.0f,0.0f);
			if( getPathPosition(Level().timeServer(),fTimeDelta, xform.c, pos, w) )	{
				xform.setXYZ(w);
				xform.c = pos;
			}
			break;
		}
	}//switch

}


bool CHelicopterMovementManager::getPathPosition(	u32 timeCurr, 
												 float fTimeDelta, 
												 const Fvector& src, 
												 Fvector& pos, 
												 Fvector& xyz)
{
	if(!m_path.size()){
		pos = src;
		xyz = m_lastXYZ;
		return true;
	}

	pathIt b,e;

	STravelPathPoint _p;
	_p.time = timeCurr;

	b = std::lower_bound(m_path.begin(),m_path.end(),_p,time_lesser);
	e = std::upper_bound(m_path.begin(),m_path.end(),_p,time_lesser);

	if(b==e)
		--b;

	u32 bt = (*b).time;
	u32 et = (*e).time;

	Fvector& bp = (*b).position;
	Fvector& ep = (*e).position;

	Fvector& bxyz = (*b).xyz;
	Fvector& exyz = (*e).xyz;

	float t = (float)(timeCurr-bt)/(float)(et-bt);


	pos.lerp(bp,ep,t);

	xyz.x = angle_lerp(bxyz.x, exyz.x, t);
	xyz.y = angle_lerp(bxyz.y, exyz.y, t);
	xyz.z = angle_lerp(bxyz.z, exyz.z, t);

	xyz.x = angle_normalize_signed(xyz.x);//-PI..PI
	xyz.y = angle_normalize_signed(xyz.y);
	xyz.z = angle_normalize_signed(xyz.z);

	m_lastXYZ.x = angle_normalize_signed(m_lastXYZ.x);
	m_lastXYZ.y = angle_normalize_signed(m_lastXYZ.y);
	m_lastXYZ.z = angle_normalize_signed(m_lastXYZ.z);

	float z_ang_delta = PI_DIV_4*(fTimeDelta)*HELI_VELOCITY_ROLL_K*helicopter()->velocity();

	if( !fsimilar(m_lastXYZ.z, xyz.z) ) {
		if(m_lastXYZ.z < xyz.z) {
			m_lastXYZ.z += z_ang_delta;
		}else {
			m_lastXYZ.z -= z_ang_delta;
		}

		xyz.z = m_lastXYZ.z;
	}


	m_lastXYZ = xyz;
	return true;
}


void CHelicopterMovementManager::addCurrentPosToTrajectory(u32 time)
{
	if(time==0)	{
		float h,p,b;
		helicopter()->XFORM().getHPB(h,p,b);
		m_lastXYZ.setHP(h,p);
		m_keyTrajectory.push_back( SWayPoint(helicopter()->XFORM().c, m_lastXYZ) );
	}else {
		Fvector xyz;
		Fvector pos;
		Fvector dir;
		getPathPosition(time,0.0f,helicopter()->XFORM().c,pos,xyz);
		dir.setHP(xyz.y, xyz.z);
		m_keyTrajectory.push_back( SWayPoint(pos, dir) );
	}
}


void CHelicopterMovementManager::shedule_Update(u32 time_delta)
{
	if ( helicopter()->state()==CHelicopter::eWaitForStart ) {
		if(Device.dwTimeGlobal >= helicopter()->m_time_delay_before_start)
			helicopter()->setState(CHelicopter::eInitiatePatrolZone);
	};

	if ( helicopter()->state()==CHelicopter::eInitiateHunt ) {
		buildHuntPath( helicopter()->lastEnemyPos() );
		if( !failed() ) {
			helicopter()->setState(CHelicopter::eMovingByAttackTraj);
		};
	};

	if( (helicopter()->state()==CHelicopter::eMovingByPatrolZonePath)&&(m_path.size()) ) {
		if(Device.dwTimeGlobal > helicopter()->m_time_last_patrol_start+helicopter()->m_time_patrol_period) {
			helicopter()->setState(CHelicopter::eInitiateWaitBetweenPatrol);
			return;
		};

		u32 z = m_keyTrajectory.size();
		if( m_currKeyIdx == (int)z-1 ) {
			helicopter()->setState(CHelicopter::eInitiatePatrolZone);
			return;
		}


		u32 tt = m_path.back().time;
		u32 lt = Level().timeServer();
		if( (int)(tt - lt) < 1000 ) {
			build_smooth_path(++m_currKeyIdx, false, false);
			pathIt b;
			STravelPathPoint _p;
			_p.time = lt-2000;
			b = std::lower_bound(m_path.begin(),m_path.end(),_p,time_lesser);

			if( (b!=m_path.end()) && (std::distance(b,m_path.end()) > 5)    )
				m_path.erase( m_path.begin(), b);
		};

		if((u32)m_currKeyIdx >= m_keyTrajectory.size()-1 )
			helicopter()->setState(CHelicopter::eInitiatePatrolZone);
	};

	if( (helicopter()->state()==CHelicopter::eMovingByAttackTraj)&&(m_path.size()) ) {
		u32 tt = m_path.back().time;
		u32 lt = Level().timeServer();
		if( (int)(tt - lt) < 500 ) {
			helicopter()->setState(CHelicopter::eInitiatePatrolZone);
		};
	};


	if( (helicopter()->state()==CHelicopter::eInitiateWaitBetweenPatrol) ) {
		m_keyTrajectory.clear();

		if( m_path.size() )
			addCurrentPosToTrajectory( m_path.back().time-1 );
		else
			addCurrentPosToTrajectory();

		m_keyTrajectory.push_back( helicopter()->m_stayPos );

		{
			m_currKeyIdx = 0;
			build_smooth_path(0, false, false);
			if(!m_failed)
				helicopter()->setState(CHelicopter::eMovingToWaitPoint);
		};	

	};
	if( helicopter()->state()==CHelicopter::eMovingToWaitPoint) {
		u32 tt = m_path.back().time;
		u32 lt = Level().timeServer();
		if( (int)(tt - lt) < 1000 ) {
			helicopter()->setState(CHelicopter::eWaitBetweenPatrol);
			m_path.clear();
			helicopter()->m_time_last_patrol_end		= Device.dwTimeGlobal;
			helicopter()->m_time_last_patrol_start		= 0;
		};
	};

	if( helicopter()->state()==CHelicopter::eWaitBetweenPatrol) {
		if( Device.dwTimeGlobal >= helicopter()->m_time_last_patrol_end+helicopter()->m_time_delay_between_patrol)
			helicopter()->setState(CHelicopter::eInitiatePatrolZone);
	};

	if( (helicopter()->state()==CHelicopter::eInitiatePatrolZone) ) {
		xr_vector<Fvector> t;
		createLevelPatrolTrajectory(200, t);
		m_keyTrajectory.clear();

		if( m_path.size() )
			addCurrentPosToTrajectory( m_path.back().time-1 );
		else
			addCurrentPosToTrajectory();

		m_keyTrajectory.insert(m_keyTrajectory.end(), t.begin(), t.end());

		{
			m_currKeyIdx = 0;
			build_smooth_path(0, false, false);
			if(!m_failed)
				helicopter()->setState(CHelicopter::eMovingByPatrolZonePath);
		};	
		helicopter()->m_time_last_patrol_start = Device.dwTimeGlobal;
		helicopter()->m_time_last_patrol_end = 0;

	};

}

float CHelicopterMovementManager::computeB(float angVel)
{
	//	float dv = 1-(angVel-MIN_ANGULAR_VELOCITY);
	//	float res = MIN_ANGULAR_VELOCITY*dv + MAX_ANGULAR_VELOCITY*angVel;
	R_ASSERT(angVel>=0.0f);
	return angVel/3.0f;
}

float CHelicopterMovementManager::_lerp(float src, 
										   float dst, 
										   float t)
{
	return src*(1.f-t) + dst*t;
}

#endif
