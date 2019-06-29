#include "stdafx.h"
#include "HelicopterMotion.h"
#include "../envelope.h"      


CHelicopterMotion::CHelicopterMotion()
{
//	mtype			=mtObject;
	for (int ch=0; ch<ctMaxChannel; ch++)
		envs[ch]	= xr_new<CEnvelope> ();
	
	fFPS			= 30.0f;
	
	m_startTime		= 0.0f;
	m_endTime		= 0.0f;

}


CHelicopterMotion::~CHelicopterMotion()
{
	Clear			();

}

void CHelicopterMotion::Clear()
{
	for (int ch=0; ch<ctMaxChannel; ch++) xr_delete(envs[ch]);
}

void CHelicopterMotion::_Evaluate(float t, Fvector& T, Fvector& R)
{
	T.x = envs[ctPositionX]->Evaluate(t);
	T.y = envs[ctPositionY]->Evaluate(t);
	T.z = envs[ctPositionZ]->Evaluate(t);

	R.y = envs[ctRotationH]->Evaluate(t);
	R.x = envs[ctRotationP]->Evaluate(t);
	R.z = envs[ctRotationB]->Evaluate(t);
}

void CHelicopterMotion::CreateKey(float t, const Fvector& P, const Fvector& R)
{
	envs[ctPositionX]->InsertKey(t,P.x);
	envs[ctPositionY]->InsertKey(t,P.y);
	envs[ctPositionZ]->InsertKey(t,P.z);
	envs[ctRotationH]->InsertKey(t,R.y);
	envs[ctRotationP]->InsertKey(t,R.x);
	envs[ctRotationB]->InsertKey(t,R.z);
}

void CHelicopterMotion::DeleteKey(float t)
{
	envs[ctPositionX]->DeleteKey(t);
	envs[ctPositionY]->DeleteKey(t);
	envs[ctPositionZ]->DeleteKey(t);
	envs[ctRotationH]->DeleteKey(t);
	envs[ctRotationP]->DeleteKey(t);
	envs[ctRotationB]->DeleteKey(t);
}

int CHelicopterMotion::KeyCount()
{
	return envs[ctPositionX]->keys.size();
}

void CHelicopterMotion::FindNearestKey(float	t, 
									   float&	min_t, 
									   float&	max_t, 
									   u32&		min_idx, 
									   u32&		max_idx, 
									   float	eps)
{
	KeyIt min_k;
	KeyIt max_k;

	envs[ctPositionX]->FindNearestKey(t, min_k, max_k, eps);

	min_t =		(min_k!=envs[ctPositionX]->keys.end())?(*min_k)->time:t; 
	max_t =		(max_k!=envs[ctPositionX]->keys.end())?(*max_k)->time:t; 

	max_idx =	(u32)std::distance(envs[ctPositionX]->keys.begin(), max_k );
	min_idx =	(u32)std::distance(envs[ctPositionX]->keys.begin(), min_k );
}

float CHelicopterMotion::GetDistanceInWay(float from_tm, float to_tm)
{
	float tm_delta = 0.5f;
	float dist = 0.0f;
	Fvector p_prevT,T,R;
	_Evaluate	(from_tm, p_prevT, R);
	
	float dt = from_tm+tm_delta;

	for(; dt < to_tm; dt+=tm_delta){
		_Evaluate	(dt, T, R);
		dist		+= p_prevT.distance_to(T);	
		p_prevT = T;
	};

	return dist;
}

float CHelicopterMotion::GetLength(float* mn, float* mx)
{
	float ln,len=0.f; 
	for (int ch=0; ch<ctMaxChannel; ch++)
		if ((ln=envs[ch]->GetLength(mn,mx))>len) len=ln;
	
	return len;
}

BOOL CHelicopterMotion::ScaleKeys(float from_time, float to_time, float scale_factor)
{
	BOOL bRes=TRUE;
	for (int ch=0; ch<ctMaxChannel; ch++)
		if (FALSE==(bRes=envs[ch]->ScaleKeys(from_time, to_time, scale_factor, 1.f/fFPS))) break;
	
	return bRes;
}

BOOL CHelicopterMotion::NormalizeKeys(float from_time, float to_time, float speed)
{
	CEnvelope* E = Envelope(ctPositionX);
	float new_tm	= 0;
	float t0		= E->keys.front()->time;
	FloatVec tms;
	tms.push_back	(t0);
	for (KeyIt it=E->keys.begin()+1; it!=E->keys.end(); it++){
		if (((*it)->time>from_time)&&((*it)->time<to_time)){
			float dist	= 0;
			Fvector PT,T,R;
			_Evaluate	(t0, PT, R);
			for (float tm=t0+1.f/fFPS; tm<=(*it)->time; tm+=EPS_L){
				_Evaluate	(tm, T, R);
				dist		+= PT.distance_to(T);
				PT.set		(T);
			}
			new_tm			+= dist / speed;
			t0				= (*it)->time;
			tms.push_back	(new_tm);
		}
	}
	for (int ch=0; ch<ctMaxChannel; ch++){
		E				= Envelope(EChannelType(ch));
		FloatIt	f_it	= tms.begin();
		for (KeyIt k_it=E->keys.begin(); k_it!=E->keys.end(); k_it++,f_it++)
			if (((*k_it)->time>from_time)&&((*k_it)->time<to_time))
				(*k_it)->time = *f_it;
	}
	return FALSE;
}


void CHelicopterMotion::DropTailKeys(u32 cnt)
{
	VERIFY(KeyCount()>(int)cnt+2);

	CEnvelope*	env = Envelope();
	for(u32 i=0; i<cnt; ++i) {
		DeleteKey(env->keys.front()->time);
	};
	GetKeyTime(0, m_startTime);
}

void CHelicopterMotion::DropHeadKeys(u32 cnt)
{
//	VERIFY(KeyCount()>(int)cnt+2);

	CEnvelope*	env = Envelope();
	for(u32 i=0; i<cnt; ++i) {
		DeleteKey(env->keys.back()->time);
	};

//	VERIFY(KeyCount()>4);
	if(KeyCount())
		GetKeyTime(KeyCount()-1, m_endTime);
	else
		m_endTime = 0.0f;
}

void CHelicopterMotion::GetKey(u32 idx, Fvector& T, Fvector& R)
{
	GetKeyT (idx, T);
	GetKeyR (idx, R);
}

void CHelicopterMotion::GetKeyT(u32 idx, Fvector& T)
{
	T.x		= envs[ctPositionX]->keys[idx]->value;
	T.y		= envs[ctPositionY]->keys[idx]->value;
	T.z		= envs[ctPositionZ]->keys[idx]->value;
}

void CHelicopterMotion::GetKeyR(u32 idx, Fvector& R)
{
	R.y		= envs[ctRotationH]->keys[idx]->value;
	R.x		= envs[ctRotationP]->keys[idx]->value;
	R.z		= envs[ctRotationB]->keys[idx]->value;
}

void CHelicopterMotion::GetKeyTime		(u32 idx, float& time)
{
	time	= envs[ctPositionX]->keys[idx]->time; 
}
