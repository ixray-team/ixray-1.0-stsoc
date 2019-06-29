#pragma once


#include "HelicopterMotion.h"
#include "patrol_path.h"
#include "intrusive_ptr.h"

class CHelicopter;

class CHelicopterMovManager :public CHelicopterMotion
#ifdef DEBUG
	,public pureRender
#endif

{
public:
	const CPatrolPath*				m_currPatrolPath;
	const CPatrolPath::CVertex*		m_currPatrolVertex;

private:
	CHelicopter*					m_heli;
	bool							m_need_to_del_path;
//	Fmatrix							m_XFORM;
	
	
	float	_flerp					(float src, float dst, float t)		{return src*(1.f-t) + dst*t;};
	bool	dice					()		{return (::Random.randF(-1.0f, 1.0f) > 0.0f); };

	void	insertRounding			(const Fvector& fromPos, const Fvector& destPos, float radius, xr_vector<Fvector>& vKeys);
	void	makeSmoothKeyPath		(float from_time, const xr_vector<Fvector>& vAddedKeys, xr_vector<Fvector>& vSmoothKeys);
	void	createLevelPatrolTrajectory(u32 keyCount, const Fvector& fromPos, xr_vector<Fvector>& keys );
	void	createHuntPathTrajectory(float from_time, const Fvector& fromPos, const Fvector& enemyPos, xr_vector<Fvector>& keys );
	void	createStayPathTrajectory(const Fvector& fromPos, xr_vector<Fvector>& keys );
	void	createRocking			(const Fvector& fromPos, const Fvector& dir, xr_vector<Fvector>& keys, float time_ );

	void	fixateKeyPath			(float from_time);
	void	buildHuntPath			(const Fvector& enemyPos);
	void	onTime					(float t, Fvector& P, Fvector& R);
	void	insertKeyPoints			(float from_time, xr_vector<Fvector>& keys, float velocity, bool updateHPB, bool normalizeTime);
	void	updatePathHPB			(float from_time);
	void	buildHPB				(const Fvector& p_prev, const Fvector& p_prev_phb, const Fvector& p0, const Fvector& p_next, Fvector& p0_phb_res, float time);
	void	addPartolPath			(float from_time);
	void	updatePatrolPath		(float from_time);
	void	addHuntPath				(float from_time, const Fvector& enemyPos);
	void	addHuntPath2			(float from_time, const Fvector& enemyPos);
	void	addPathToStayPoint		(float from_time);
	void	addGoToPointPath		(float from_time);

	void	getPathAltitude			(Fvector& point, float base_altitude);
	void	truncatePathSafe		(float from_time, float& safe_time, Fvector& lastPoint);
	void	addGoBySpecifiedPatrolPath	(float time_from);


	void	GoBySpecifiedPatrolPath		();
	void	UpdatePatrolPath			();
	void	GoByRoundPatrolPath			();
	//patrol path
	void	makeNewPoint			(const Fvector& prevPoint, const Fvector& point, const Fbox& box, Fvector& newPoint);
	void	makeNewPoint			(const Fbox& fbox, const Fvector& point, const Fvector& direction, Fvector& newPoint);
	u16		getPlaneID				(const Fbox& box, const Fvector& point);
	void	getReflectDir			(const Fvector& dir, const u16 planeID, Fvector& newDir);
	void	selectSafeDir			(const Fvector& prevPoint,const Fbox& fbox, Fvector& newDir);

public:
	CHelicopterMovManager			();
	virtual ~CHelicopterMovManager	();

	void	init					(CHelicopter* h);
	void	load					(LPCSTR		section);
	void	shedule_Update			(u32 timeDelta);
	void	getPathPosition			(float time, Fvector& P, Fvector& R);

	float	EndTime					();

	Fbox							m_boundingVolume;
	Fbox							m_boundingAssert;

#ifdef DEBUG
public:
	virtual void	OnRender		();
	void			DrawPath		(bool bDrawInterpolated, bool bDrawKeys, float dTime=0.5f);
#endif

};