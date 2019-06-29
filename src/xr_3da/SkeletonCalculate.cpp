#include "stdafx.h"
#pragma hdrstop

#include "BodyInstance.h"

extern int	psSkeletonUpdate;

IC void	KEY_Interp(CKey& D, CKey& K1, CKey& K2, float delta)
{
	D.Q.slerp	(K1.Q,K2.Q,delta);
	D.T.lerp	(K1.T,K2.T,delta);
}

struct ConsistantKey
{
	CKey*	K;
	float	w;

	IC void	set(CKey* _K, float _w)
	{	K = _K; w = _w; }
};
IC bool operator < (const ConsistantKey& A, const ConsistantKey& B)	// note: inverse operator
{	return A.w>B.w; }

IC void MakeKeysConsistant(ConsistantKey *keys, int count)
{
	// sort in decreasing order
	std::sort(keys,keys+count);

	// recalc
	for (int i=0; i<count-1; i++) {
		Fquaternion Q1,Q2;
		Q1.add(keys[i].K->Q,keys[i+1].K->Q);
		Q2.sub(keys[i].K->Q,keys[i+1].K->Q);
		if (Q1.magnitude()<Q2.magnitude())	keys[i+1].K->Q.inverse_with_w();
	}
}

IC void MakeKeysSelected(ConsistantKey *keys, int count)
{
	// sort in decreasing order
	std::sort(keys,keys+count);
}

// Calculation
void CBoneData::Calculate(CKinematics* K, Fmatrix *parent)
{
	CBoneInstance& INST			= K->LL_GetInstance(SelfID);

	if (INST.Callback_overwrite && INST.Callback)
	{
		INST.Callback		(&INST);
	} else {
		CKey				R[MAX_BLENDED];
		ConsistantKey		S[MAX_BLENDED];

		// Calculate all keys
		CKey*	D					= R;
		BlendListIt					BI;
		for (BI=INST.Blend.begin(); BI!=INST.Blend.end(); BI++,D++)
		{
			CBlend*			B		=	*BI;
			float			time	=	B->timeCurrent*float(SAMPLE_FPS);
			CMotion&		M		=	Motions[B->motionID];
			u32				frame	=	iFloor(time);
			float			delta	=	time-float(frame);
			u32				count	=	M._count;
			CKeyQ&			K1		=	M._keys[(frame+0)%count];
			CKeyQ&			K2		=	M._keys[(frame+1)%count];
			PSGP.blerp				(D,&K1,&K2,delta);
		}

		// Blend them together
		BI				= INST.Blend.begin();

		CKey			Result;

		switch (INST.Blend.size())
		{
		case 0:
			Result.Q.set	(0,0,0,0);
			Result.T.set	(0,0,0);
			break;
		case 1: 
			Result			= R[0];
			break;
		case 2:
			{
				float w0 = BI[0]->blendAmount;
				float w1 = BI[1]->blendAmount;
				float ws = w0+w1;
				float w;
				if (fis_zero(ws))	w = 0;
				else				w = w1/ws;
				KEY_Interp		(Result,R[0],R[1],w);
			}
			break;
		default:
			{
				int count = INST.Blend.size();
				for (int i=0; i<count; i++)		S[i].set(R+i,BI[i]->blendAmount);
				std::sort	(S,S+count);
				KEY_Interp	(Result,*S[0].K, *S[1].K, S[1].w/(S[0].w+S[1].w));
			}
			break;
		}

		// Build matrix
		Fmatrix					RES;
		RES.mk_xform			(Result.Q,Result.T);
		INST.mTransform.mul_43	(*parent,RES);
		if (INST.Callback)		INST.Callback(&INST);
	}

	// Calculate children
	for (xr_vector<CBoneData*>::iterator C=children.begin(); C!=children.end(); C++)
		(*C)->Calculate(K,&INST.mTransform);
}

void CKinematics::Calculate(BOOL bLight)
{
	Update();

	if (Device.dwFrame == Update_Frame) return;
	Update_Frame		= Device.dwFrame; 

	// Calculate bones
	Device.Statistic.Animation.Begin();
	(*bones)[iRoot]->Calculate		(this,&Fidentity);
	Device.Statistic.Animation.End	();

	// Calculate BOXes/Spheres if needed
	Update_ID++; 
	if (Update_ID>=psSkeletonUpdate) 
	{
		// mark
		Update_ID		= -(::Random.randI(16));

		// the update itself
		Fbox	Box; Box.invalidate();
		for (u32 b=0; b<bones->size(); b++)
		{
			Fobb&		obb		= (*bones)[b]->obb;
			Fmatrix&	Mbone	= bone_instances[b].mTransform;
			Fmatrix		Mbox;	obb.xform_get(Mbox);
			Fmatrix		X;		X.mul(Mbone,Mbox);
			Fvector&	S		= obb.m_halfsize;

			Fvector			P,A;
			A.set( -S.x,	-S.y,	-S.z ); X.transform_tiny(P,A); Box.modify(P);
			A.set( -S.x,	-S.y,	 S.z ); X.transform_tiny(P,A); Box.modify(P);
			A.set(  S.x,	-S.y,	 S.z ); X.transform_tiny(P,A); Box.modify(P);
			A.set(  S.x,	-S.y,	-S.z ); X.transform_tiny(P,A); Box.modify(P);
			A.set( -S.x,	 S.y,	-S.z ); X.transform_tiny(P,A); Box.modify(P);
			A.set( -S.x,	 S.y,	 S.z ); X.transform_tiny(P,A); Box.modify(P);
			A.set(  S.x, 	 S.y,	 S.z ); X.transform_tiny(P,A); Box.modify(P);
			A.set(  S.x, 	 S.y,	-S.z ); X.transform_tiny(P,A); Box.modify(P);
		}

		// previous frame we have updated box - update sphere
		vis.box.min.average	(Box.min);
		vis.box.max.average	(Box.max);
		vis.box.getsphere	(vis.sphere.P,vis.sphere.R);
	}

	//
	if (Update_Callback)	Update_Callback(this);
}
