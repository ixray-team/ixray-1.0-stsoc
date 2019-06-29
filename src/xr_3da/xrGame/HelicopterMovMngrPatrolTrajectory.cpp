#include "stdafx.h"
#include "HelicopterMovementManager.h"
#include "Helicopter.h"
#include "level.h"
#include "hudmanager.h"



void CHelicopterMovManager::createLevelPatrolTrajectory(u32 keyCount, 
														const Fvector& fromPos, 
														xr_vector<Fvector>& keys )
{
	Fvector					keyPoint;	//working
	Fvector					prevPoint;	//pre-last point
	Fvector					lastPoint;
	Fvector					dir;

	keyPoint = fromPos;
	VERIFY( m_boundingAssert.contains(keyPoint) );

	keys.push_back(keyPoint);
	lastPoint = keyPoint;
	
	if(CHelicopterMotion::KeyCount()>1) {
		CHelicopterMotion::GetKeyT(CHelicopterMotion::KeyCount()-2, prevPoint);
		makeNewPoint(prevPoint, lastPoint, m_boundingVolume, keyPoint);
	}else {
		selectSafeDir(lastPoint, m_boundingVolume, dir);
		makeNewPoint(m_boundingVolume, lastPoint, dir, keyPoint);
	};

	VERIFY( m_boundingAssert.contains(keyPoint) );

	keys.push_back(keyPoint);
	prevPoint = lastPoint;

	for(u32 i = 0; i<keyCount; ++i)	{
		lastPoint = keyPoint;

		makeNewPoint(prevPoint, lastPoint, m_boundingVolume, keyPoint);
		VERIFY( m_boundingAssert.contains(keyPoint) );
		
		
		prevPoint = lastPoint;
		keys.push_back(keyPoint);
	};

}
void CHelicopterMovManager::makeNewPoint (const Fbox& fbox, 
										  const Fvector& point, 
										  const Fvector& direction, 
										  Fvector& newPoint)
{
	Fvector dir = direction;
	//find new point
	float kp1,kp2,kp3,kp4;
	//plane1
	kp1 = (fbox.max.x-point.x)/dir.x;
	//plane2
	kp2 = (fbox.max.z-point.z)/dir.z;
	//plane3
	kp3 = (fbox.min.x-point.x)/dir.x;
	//plane4
	kp4 = (fbox.min.z-point.z)/dir.z;

	float resK = flt_max;

	if((kp1>EPS_L)&&(kp1<resK))
		resK = kp1;

	if((kp2>EPS_L)&&(kp2<resK))
		resK = kp2;

	if((kp3>EPS_L)&&(kp3<resK))
		resK = kp3;

	if((kp4>EPS_L)&&(kp4<resK))
		resK = kp4;

	dir.y = 0.0f;
	newPoint.add(point, dir.mul(resK) );

	Fbox b = fbox;
	b.scale(0.05f);
	if(!b.contains(point) )
		newPoint.set((fbox.max.x-fbox.min.x)/2.0f, 0.0f, (fbox.max.z-fbox.min.z)/2.0f);
	
	newPoint.y = (fbox.max.y-fbox.min.y)/2.0f;
	VERIFY( m_boundingAssert.contains(newPoint) );

	getPathAltitude(newPoint,m_heli->m_data.m_baseAltitude);
	VERIFY( m_boundingAssert.contains(newPoint) );


}

void CHelicopterMovManager::makeNewPoint(	const Fvector& prevPoint, 
											const Fvector& point,
											const Fbox& fbox,
											Fvector& newPoint)
{
	VERIFY( m_boundingAssert.contains(prevPoint) );
	VERIFY( m_boundingAssert.contains(point) );
	u16 planeID		= getPlaneID(fbox, point); 

	Fvector prevDir;
	prevDir.sub(point, prevPoint);
	prevDir.y = 0.0f;
	prevDir.normalize_safe();

	VERIFY(fsimilar(prevDir.y, 0.0f));

	Fvector newDir;

	getReflectDir(prevDir, planeID, newDir);
	newDir.normalize_safe();

	VERIFY(fsimilar(newDir.y, 0.0f));
	makeNewPoint(fbox, point, newDir, newPoint);
	VERIFY( m_boundingAssert.contains(newPoint) );

}

u16 CHelicopterMovManager::getPlaneID(const Fbox& box, const Fvector& point)
{
//					(2)
//		----------------------------maxZ
//		|						|
//	 (3)|		level Box		|(1)
//		|						|
//		----------------------------minZ
//		|minX		(4)			|maxX
	VERIFY( m_boundingAssert.contains(point) );

	if( fsimilar(box.min.x, point.x, EPS_L) )
		return 3;

	if( fsimilar(box.max.x, point.x, EPS_L) )
		return 1;

	if( fsimilar(box.min.z, point.z, EPS_L) )
		return 4;

	if( fsimilar(box.max.z, point.z, EPS_L) )
		return 2;

	return 5;
}


void CHelicopterMovManager::getReflectDir(const Fvector& dir,
										  const u16 planeID,
										  Fvector& newDir)
{
	switch (planeID) {
		case 1:
		case 3:
			{
				newDir.set(-dir.x, dir.y, dir.z);
				if(_abs(newDir.z)<0.3f) {
					float sign;
					(newDir.z>0.0f)?sign=1.0f:sign=-1.0f;
					newDir.z += sign*0.3f;
				}
				break;
			}
		case 2:
		case 4:
			{
				newDir.set(dir.x, dir.y, -dir.z);
				if(_abs(newDir.x)<0.3f) {
					float sign;
					(newDir.x>0.0f)?sign=1.0f:sign=-1.0f;
					newDir.x += sign*0.3f;
				}
				break;
			}
		case 5:
			{
				newDir.set(dir.x, 0.0f, dir.z);
			}
	};

}

void CHelicopterMovManager::selectSafeDir(const Fvector& prevPoint,
										  const Fbox& fbox, 
										  Fvector& newDir)
{
	Fvector pt = prevPoint;
	newDir = m_heli->m_data.m_startDir;
	newDir.normalize_safe();
	VERIFY(fsimilar(newDir.y, 0.0f));

	VERIFY( m_boundingAssert.contains(prevPoint) );

	while( !fbox.contains(pt.add(newDir)) ) {
		pt = prevPoint;
		
		float h ;
		float p ;
		newDir.getHP(h,p);
		h += PI_DIV_2;
		newDir.setHP(h,p);
	}

	newDir.y = 0.0f;
}

void CHelicopterMovManager::getPathAltitude (Fvector& point, float base_altitude)
{
	collide::rq_result		cR;
	Fvector down_dir;
	down_dir.set(0.0f, -1.0f, 0.0f);

	point.y = m_boundingVolume.max.y+EPS_L;
	//VERIFY( m_boundingAssert.contains(point) );
	VERIFY( _valid(point) );

	Level().ObjectSpace.RayPick(point, down_dir, m_boundingVolume.max.y-m_boundingVolume.min.y+1.0f, collide::rqtStatic, cR);
	
	point.y = point.y-cR.range;

	if( point.y+m_heli->m_data.m_baseAltitude < m_boundingVolume.max.y )
		point.y += base_altitude;
	else
		point.y = m_boundingVolume.max.y-EPS_L;

//	VERIFY( m_boundingAssert.contains(point) );
	VERIFY( _valid(point) );

	float minY = m_boundingVolume.min.y+(m_boundingVolume.max.y-m_boundingVolume.min.y)*m_heli->m_data.m_alt_korridor;
	float maxY = m_boundingVolume.max.y+base_altitude;
//	minY = maxY-EPS_L;
	clamp (point.y,minY,maxY);
//	VERIFY( m_boundingAssert.contains(point) );
	VERIFY( _valid(point) );

}

#ifdef DEBUG
void CHelicopterMovManager::DrawPath(bool bDrawInterpolated, bool bDrawKeys, float dTime)
{



	RCache.dbg_DrawLINE(Fidentity,m_heli->m_right_rocket_bone_xform.c, m_heli->m_data.m_destEnemyPos,D3DCOLOR_XRGB(0,255,0));

	RCache.dbg_DrawLINE(Fidentity,m_heli->XFORM().c,m_heli->m_data.m_destEnemyPos,D3DCOLOR_XRGB(255,0,0));
	return;

//	float t = Level().timeServer()/1000.0f;
//	Fvector P,R;
//	_Evaluate(t,P,R);
//	RCache.dbg_DrawAABB  (P,0.1f,0.1f,0.1f,D3DCOLOR_XRGB(255,0,0));
///*	Fvector P_,R_;
//	_Evaluate(t+1.0f,P_,R_);
//	float s = P_.distance_to(P);
//
//	HUD().pFontSmall->SetColor(color_rgba(0xff,0xff,0xff,0xff));
//	HUD().pFontSmall->OutSet	(320,630);
//	HUD().pFontSmall->OutNext("Motion Speed:			[%3.2f]",s);
//*/
//	if(bDrawInterpolated){
//	
//	FvectorVec path_points;
//	FvectorVec path_rot;
//
//	float min_t				= m_startTime;
//	float max_t				= m_endTime;
//
//	Fvector 				T,r;
//	path_points.clear		();
//	for (float t=min_t; (t<max_t)||fsimilar(t,max_t,EPS_L); t+=dTime){
//		_Evaluate(t,T,r);
//		path_points.push_back(T);
//		path_rot.push_back(r);
//	}
//
//	float path_box_size = .05f;
//	for(u32 i = 0; i<path_points.size (); ++i) {
//		RCache.dbg_DrawAABB  (path_points[i],path_box_size,path_box_size,path_box_size,D3DCOLOR_XRGB(0,255,0));
//	}	
///*		r.setHP(path_rot[i].y, path_rot[i].x );
//		r.mul (3.0f);
//		T.add (path_points[i],r);
//		RCache.dbg_DrawLINE (Fidentity, path_points[i], T, D3DCOLOR_XRGB(255,0,0));
//*/
//	}
//
//	if(bDrawKeys){
//		float key_box_size = .25f;
//		u32 cnt = KeyCount();
//		for(u32 ii=0;ii<cnt;++ii) {
//			Fvector _t;
//			Fvector _r;
//			GetKey (ii,_t,_r);
//			RCache.dbg_DrawAABB  (_t,key_box_size,key_box_size,key_box_size,D3DCOLOR_XRGB(0,255,255));
//
////			_r.setHP(_r.y, _r.x );
////			_r.mul (6.0f);
////			TT.add (_t,_r);
////			RCache.dbg_DrawLINE (Fidentity, _t, TT, D3DCOLOR_XRGB(255,0,0));
//		}
//	}
//
}
void CHelicopterMovManager::OnRender()
{
//	DrawPath (false, true);
}
#endif
