
#pragma once

#include "../motion.h"

class CEnvelope;


class CHelicopterMotion
{
	CEnvelope*		envs			[ctMaxChannel];
	float			fFPS;

protected:
	float			m_startTime;
	float			m_endTime;

public:
	CHelicopterMotion						();
	virtual			~CHelicopterMotion		();

	void			Clear					();

	void			_Evaluate		(float t, Fvector& T, Fvector& R);

	void 			FindNearestKey	(float t, float& min_t, float& max_t, u32 &min_idx, u32 &max_idx, float eps=EPS_L);
	void			CreateKey		(float t, const Fvector& P, const Fvector& R);
	void			DeleteKey		(float t);
	void			DropHeadKeys	(u32 cnt);
	void			DropTailKeys	(u32 cnt);
	void			NormalizeKeys	();
	int				KeyCount		();
	CEnvelope*		Envelope		(EChannelType et=ctPositionX){return envs[et];}
	BOOL			ScaleKeys		(float from_time, float to_time, float scale_factor);
	BOOL			NormalizeKeys	(float from_time, float to_time, float speed);
	float			GetLength		(float* mn=0, float* mx=0);
	void			GetKeyT			(u32 idx, Fvector& T);
	void			GetKeyR			(u32 idx, Fvector& R);
	void			GetKey			(u32 idx, Fvector& T, Fvector& R);
	void			GetKeyTime		(u32 idx, float& time);
	float			GetDistanceInWay(float from_tm, float to_tm);
};

