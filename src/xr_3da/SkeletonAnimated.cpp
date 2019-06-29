//---------------------------------------------------------------------------
#include 	"stdafx.h"
#pragma hdrstop

#include 	"SkeletonAnimated.h"
#include	"SkeletonX.h"
#include	"fmesh.h"

extern int	psSkeletonUpdate;

//////////////////////////////////////////////////////////////////////////
// BoneInstance methods
void		CBlendInstance::construct()
{	
	ZeroMemory			(this,sizeof(*this));
}
void		CBlendInstance::blend_add	(CBlend* H)
{	
	if (Blend.size()==MAX_BLENDED)	{
		BlendSVecIt _d	= Blend.begin();
		for (BlendSVecIt it=Blend.begin()+1; it!=Blend.end(); it++)
			if ((*it)->blendAmount<(*_d)->blendAmount) _d=it;
		Blend.erase		(_d);
	}
	VERIFY (Blend.size()<MAX_BLENDED);
	Blend.push_back(H);	
}
void		CBlendInstance::blend_remove	(CBlend* H)
{
	CBlend** I = std::find(Blend.begin(),Blend.end(),H);
	if (I!=Blend.end())	Blend.erase(I);
}

// Motion control
void	CKinematicsAnimated::Bone_Motion_Start		(CBoneData* bd, CBlend* handle) 
{
	LL_GetBlendInstance	(bd->GetSelfID()).blend_add	(handle);
	for (vecBonesIt I=bd->children.begin(); I!=bd->children.end(); I++)                        
		Bone_Motion_Start	(*I,handle);
}
void	CKinematicsAnimated::Bone_Motion_Stop		(CBoneData* bd, CBlend* handle) 
{
	LL_GetBlendInstance	(bd->GetSelfID()).blend_remove	(handle);
	for (vecBonesIt I=bd->children.begin(); I!=bd->children.end(); I++)
		Bone_Motion_Stop	(*I,handle);
}
void	CKinematicsAnimated::Bone_Motion_Start_IM	(CBoneData* bd,  CBlend* handle) 
{
	LL_GetBlendInstance	(bd->GetSelfID()).blend_add		(handle);
}
void	CKinematicsAnimated::Bone_Motion_Stop_IM	(CBoneData* bd, CBlend* handle) 
{
	LL_GetBlendInstance	(bd->GetSelfID()).blend_remove	(handle);
}

#ifdef DEBUG
std::pair<LPCSTR,LPCSTR> CKinematicsAnimated::LL_MotionDefName_dbg	(MotionID ID)
{
	shared_motions& s_mots	= m_Motions[ID.slot].motions;
	accel_map::iterator _I, _E=s_mots.motion_map()->end();
	for (_I	= s_mots.motion_map()->begin(); _I!=_E; ++_I)	if (_I->second==ID.idx) return std::make_pair(*_I->first,*s_mots.id());
	return std::make_pair((LPCSTR)0,(LPCSTR)0);
}
/*
LPCSTR CKinematicsAnimated::LL_MotionDefName_dbg	(LPVOID ptr)
{
//.
	// cycles
	mdef::const_iterator I,E;
	I = motions.cycle()->begin(); 
	E = motions.cycle()->end(); 
	for ( ; I != E; ++I) if (&(*I).second == ptr) return *(*I).first;
	// fxs
	I = motions.fx()->begin(); 
	E = motions.fx()->end(); 
	for ( ; I != E; ++I) if (&(*I).second == ptr) return *(*I).first;
	return 0;
}
*/
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
MotionID CKinematicsAnimated::LL_MotionID	(LPCSTR B)
{
	MotionID motion_ID;
	for (int k=int(m_Motions.size())-1; k>=0; --k){
    	shared_motions* s_mots	= &m_Motions[k].motions;
		accel_map::iterator I 	= s_mots->motion_map()->find(LPSTR(B));
    	if (I!=s_mots->motion_map()->end())	{ motion_ID.set(u16(k),I->second); break; }
    }
    return motion_ID;
}
u16 CKinematicsAnimated::LL_PartID		(LPCSTR B)
{
	if (0==m_Partition)	return BI_NONE;
	for (u16 id=0; id<MAX_PARTS; id++) {
		CPartDef&	P = (*m_Partition)[id];
		if (0==P.Name)	continue;
		if (0==stricmp(B,*P.Name)) return id;
	}
	return BI_NONE;
}

// cycles
MotionID CKinematicsAnimated::ID_Cycle_Safe(LPCSTR  N)
{
	MotionID motion_ID;
	for (int k=int(m_Motions.size())-1; k>=0; --k){
    	shared_motions* s_mots	= &m_Motions[k].motions;
		accel_map::iterator I 	= s_mots->cycle()->find(LPSTR(N));
		if (I!=s_mots->cycle()->end())	{	motion_ID.set(u16(k),I->second); break;}
    }
    return motion_ID;
}
MotionID CKinematicsAnimated::ID_Cycle	(shared_str  N)
{
	MotionID motion_ID		= ID_Cycle_Safe	(N);	R_ASSERT3(motion_ID.valid(),"! MODEL: can't find cycle: ", N.c_str());
    return motion_ID;
}
MotionID CKinematicsAnimated::ID_Cycle_Safe(shared_str  N)
{
	MotionID motion_ID;
	for (int k=int(m_Motions.size())-1; k>=0; --k){
		shared_motions* s_mots	= &m_Motions[k].motions;
		accel_map::iterator I 	= s_mots->cycle()->find(N);
		if (I!=s_mots->cycle()->end())	{	motion_ID.set(u16(k),I->second); break;}
	}
	return motion_ID;
}
MotionID CKinematicsAnimated::ID_Cycle	(LPCSTR  N)
{
	MotionID motion_ID		= ID_Cycle_Safe	(N);	R_ASSERT3(motion_ID.valid(),"! MODEL: can't find cycle: ", N);
	return motion_ID;
}
void	CKinematicsAnimated::LL_FadeCycle(u16 part, float falloff)
{
	BlendSVec&	Blend		= blend_cycles[part];
	
	for (u32 I=0; I<Blend.size(); I++)
	{
		CBlend& B			= *Blend[I];
		B.blend				= CBlend::eFalloff;
		B.blendFalloff		= falloff;
		if (B.stop_at_end)  B.playing = FALSE;		// callback не должен приходить!
	}
}
void	CKinematicsAnimated::LL_CloseCycle(u16 part)
{
	if (BI_NONE==part)		return;
	if (part>=MAX_PARTS)	return;

	// destroy cycle(s)
	BlendSVecIt	I = blend_cycles[part].begin(), E = blend_cycles[part].end();
	for (; I!=E; I++){
		CBlend& B = *(*I);

		B.blend = CBlend::eFREE_SLOT;
		
		CPartDef& P	= (*m_Partition)[B.bone_or_part];
		for (u32 i=0; i<P.bones.size(); i++)
			Bone_Motion_Stop_IM	((*bones)[P.bones[i]],*I);
	}
	blend_cycles[part].clear	(); // ?
}

CBlend*	CKinematicsAnimated::LL_PlayCycle(u16 part, MotionID motion_ID, BOOL  bMixing, float blendAccrue, float blendFalloff, float Speed, BOOL noloop, PlayCallback Callback, LPVOID CallbackParam)
{
	// validate and unroll
	if (!motion_ID.valid())	return 0;
	if (BI_NONE==part)		{
		for (u16 i=0; i<MAX_PARTS; i++)
			LL_PlayCycle(i,motion_ID,bMixing,blendAccrue,blendFalloff,Speed,noloop,Callback,CallbackParam);
		return 0;
	}
	if (part>=MAX_PARTS)	return 0;
	if (0==m_Partition->part(part).Name)	return 0;

//	shared_motions* s_mots	= &m_Motions[motion.slot];
//	CMotionDef* m_def		= s_mots->motion_def(motion.idx);
    
	// Process old cycles and create _new_
	{
		_DBG_SINGLE_USE_MARKER;
		if (bMixing)	LL_FadeCycle	(part,blendFalloff);
		else			LL_CloseCycle	(part);
	}
	CPartDef& P	=	(*m_Partition)[part];
	CBlend*	B	=	IBlend_Create	();

	_DBG_SINGLE_USE_MARKER;
	for (u32 i=0; i<P.bones.size(); i++)
		Bone_Motion_Start_IM	((*bones)[P.bones[i]],B);
	blend_cycles[part].push_back(B);

	// Setup blend params
	if (bMixing)	{
		B->blend		= CBlend::eAccrue;
		B->blendAmount	= EPS_S;
	} else {
		B->blend		= CBlend::eFixed;
		B->blendAmount	= 1;
	}
	B->blendAccrue	= blendAccrue;
	B->blendFalloff	= 0; // blendFalloff used for previous cycles
	B->blendPower	= 1;
	B->speed		= Speed;
	B->motionID		= motion_ID;
	B->timeCurrent	= 0;
	B->timeTotal	= m_Motions[B->motionID.slot].bone_motions[LL_GetBoneRoot()]->at(motion_ID.idx).GetLength();
	B->bone_or_part	= part;
	B->playing		= TRUE;
	B->stop_at_end	= noloop;
	B->Callback		= Callback;
	B->CallbackParam= CallbackParam;
	return			B;
}
CBlend*	CKinematicsAnimated::LL_PlayCycle		(u16 part, MotionID motion_ID, BOOL bMixIn, PlayCallback Callback, LPVOID CallbackParam)
{
	VERIFY					(motion_ID.valid()); 
    CMotionDef* m_def		= m_Motions[motion_ID.slot].motions.motion_def(motion_ID.idx);
    VERIFY					(m_def);
	return LL_PlayCycle		(part,motion_ID,bMixIn, 
    						 m_def->Accrue(),m_def->Falloff(),m_def->Speed(),m_def->StopAtEnd(), 
                             Callback,CallbackParam);
}
CBlend*	CKinematicsAnimated::PlayCycle		(LPCSTR  N, BOOL bMixIn, PlayCallback Callback, LPVOID CallbackParam)
{
	MotionID motion_ID		= ID_Cycle(N);
	if (motion_ID.valid())	return PlayCycle(motion_ID,bMixIn,Callback,CallbackParam);
	else					{ Debug.fatal(DEBUG_INFO,"! MODEL: can't find cycle: %s", N); return 0; }
}
CBlend*	CKinematicsAnimated::PlayCycle		(MotionID motion_ID,  BOOL bMixIn, PlayCallback Callback, LPVOID CallbackParam)
{	
	VERIFY					(motion_ID.valid()); 
    CMotionDef* m_def		= m_Motions[motion_ID.slot].motions.motion_def(motion_ID.idx);
    VERIFY					(m_def);
	return LL_PlayCycle		(m_def->bone_or_part,motion_ID,bMixIn, 
    						 m_def->Accrue(),m_def->Falloff(),m_def->Speed(),m_def->StopAtEnd(), 
                             Callback,CallbackParam);
}

// fx'es
MotionID CKinematicsAnimated::ID_FX_Safe		(LPCSTR  N)
{
	MotionID motion_ID;
	for (int k=int(m_Motions.size())-1; k>=0; --k){
    	shared_motions* s_mots	= &m_Motions[k].motions;
		accel_map::iterator I 	= s_mots->fx()->find(LPSTR(N));
		if (I!=s_mots->fx()->end())	{	motion_ID.set(u16(k),I->second); break;}
    }
    return motion_ID;
}
MotionID CKinematicsAnimated::ID_FX			(LPCSTR  N)
{
	MotionID motion_ID		= ID_FX_Safe(N);R_ASSERT3(motion_ID.valid(),"! MODEL: can't find FX: ", N);
    return motion_ID;
}
CBlend*	CKinematicsAnimated::PlayFX			(MotionID motion_ID, float power_scale)
{
	VERIFY					(motion_ID.valid()); 
    CMotionDef* m_def		= m_Motions[motion_ID.slot].motions.motion_def(motion_ID.idx);
    VERIFY					(m_def);
	return LL_PlayFX		(m_def->bone_or_part,motion_ID,
    						 m_def->Accrue(),m_def->Falloff(),
                             m_def->Speed(),m_def->Power()*power_scale);
}
CBlend*	CKinematicsAnimated::PlayFX			(LPCSTR  N, float power_scale)
{
	MotionID motion_ID		= ID_FX(N);
    return PlayFX 			(motion_ID,power_scale);
}
CBlend*	CKinematicsAnimated::LL_PlayFX		(u16 bone, MotionID motion_ID, float blendAccrue, float blendFalloff, float Speed, float Power)
{
	if (!motion_ID.valid())	return 0;
	if (blend_fx.size()>=MAX_BLENDED) return 0;
	if (BI_NONE==bone)		bone = iRoot;
	
	CBlend*	B		= IBlend_Create();
	_DBG_SINGLE_USE_MARKER;
	Bone_Motion_Start	((*bones)[bone],B);
	
	B->blend		= CBlend::eAccrue;
	B->blendAmount	= EPS_S;
	B->blendAccrue	= blendAccrue;
	B->blendFalloff	= blendFalloff;
	B->blendPower	= Power;
	B->speed		= Speed;
	B->motionID		= motion_ID;
	B->timeCurrent	= 0;
	B->timeTotal	= m_Motions[B->motionID.slot].bone_motions[bone]->at(motion_ID.idx).GetLength();;
	B->bone_or_part	= bone;
	
	B->playing		= TRUE;
	B->stop_at_end	= FALSE;
	
	blend_fx.push_back(B);
	return			B;
}

void CKinematicsAnimated::UpdateTracks	()
{
	_DBG_SINGLE_USE_MARKER;
	if (Update_LastTime==Device.dwTimeGlobal) return;
	u32 DT	= Device.dwTimeGlobal-Update_LastTime;
	if (DT>66) DT=66;
	float dt = float(DT)/1000.f;
	Update_LastTime 	= Device.dwTimeGlobal;

	BlendSVecIt I,E;

	// Cycles
	for (u16 part=0; part<MAX_PARTS; part++){
		if (0==m_Partition->part(part).Name)	continue;

		I = blend_cycles[part].begin(); E = blend_cycles[part].end();
		for (; I!=E; I++)
		{
			CBlend& B = *(*I);
			if (B.dwFrame==Device.dwFrame)	continue;
			B.dwFrame		=	Device.dwFrame;

			if (B.playing) {
				if(B.timeCurrent > 10000 || B.speed > 10000)
					Msg("--Atc %f %f",B.timeCurrent,B.speed);
				B.timeCurrent += dt*B.speed; // stop@end - time is not going 
				if(B.timeCurrent > 10000)
					Msg("--Btc %f %f",B.timeCurrent,B.speed);

			}
			switch (B.blend) 
			{
			case CBlend::eFREE_SLOT: 
				NODEFAULT;
			case CBlend::eFixed:	
				B.blendAmount 		+= dt*B.blendAccrue*B.blendPower;
				if (B.blendAmount>B.blendPower)	B.blendAmount = B.blendPower; 
				if (B.stop_at_end && (B.timeCurrent > (B.timeTotal-SAMPLE_SPF) )) {
					B.timeCurrent	= B.timeTotal-SAMPLE_SPF;		// stop@end - time frozen at the end
					if (B.playing&&B.Callback)	B.Callback(&B);		// callback only once
					B.playing		= FALSE;
				}
				break;
			case CBlend::eAccrue:	
				B.blendAmount 		+= dt*B.blendAccrue*B.blendPower;
				if (B.stop_at_end && (B.timeCurrent > (B.timeTotal-SAMPLE_SPF) )) {
					B.timeCurrent	= B.timeTotal-SAMPLE_SPF;		// stop@end - time frozen at the end
					if (B.playing&&B.Callback)	B.Callback(&B);		// callback only once
					B.playing		= FALSE;
					B.blend			= CBlend::eFixed;
				} 
				if (B.blendAmount>=B.blendPower) {
					// switch to fixed
					B.blendAmount	= B.blendPower;
					B.blend			= CBlend::eFixed;
				}
				break;
			case CBlend::eFalloff:
				B.blendAmount 		-= dt*B.blendFalloff*B.blendPower;
				if (B.blendAmount<=0) {
					// destroy cycle
					B.blend 		= CBlend::eFREE_SLOT;

					CPartDef& P		= m_Partition->part(B.bone_or_part);
					for (u32 i=0; i<P.bones.size(); i++)
						Bone_Motion_Stop_IM((*bones)[P.bones[i]],*I);

					blend_cycles[part].erase(I);
					E				= blend_cycles[part].end(); I--; 
				}
				break;
			default: 
				NODEFAULT;
			}
		}
	}
	
	// FX
	I = blend_fx.begin(); E = blend_fx.end();
	for (; I!=E; I++)
	{
		CBlend& B = *(*I);
		if (!B.playing)	continue;
		B.timeCurrent += dt*B.speed;
		switch (B.blend) 
		{
		case CBlend::eFREE_SLOT: 
			NODEFAULT;
		case CBlend::eFixed:
			{
/*				B.blendAmount = B.blendPower; 
				// calc time to falloff
				float time2falloff = B.timeTotal - 1/(B.blendFalloff*B.speed);
				if (B.timeCurrent >= time2falloff) {
					// switch to falloff
					B.blend		= CBlend::eFalloff;
				}
*/
				B.blend		= CBlend::eFalloff;
			}
			break;
		case CBlend::eAccrue:
            B.blendAmount 	+= dt*B.blendAccrue*B.blendPower*B.speed;
			if (B.blendAmount>=B.blendPower) {
				// switch to fixed
				B.blendAmount	= B.blendPower;
				B.blend			= CBlend::eFixed;
			}
			break;
		case CBlend::eFalloff:
			B.blendAmount 	-= dt*B.blendFalloff*B.blendPower*B.speed;
			if (B.blendAmount<=0) {
				// destroy fx
				B.blend = CBlend::eFREE_SLOT;
				Bone_Motion_Stop((*bones)[B.bone_or_part],*I);
				blend_fx.erase(I); 
				E=blend_fx.end(); I--; 
			}
			break;
		default: NODEFAULT;
		}
	}
}

void CKinematicsAnimated::Release()
{
	// xr_free bones
//.	for (u32 i=0; i<bones->size(); i++)
//.	{
//.		CBoneDataAnimated* B	= (CBoneDataAnimated*)(*bones)[i];
//.		B->Motions.clear		();
//.	}

	// destroy shared data
//.	xr_delete(partition);
//.	xr_delete(motion_map);
//.	xr_delete(m_cycle);
//.	xr_delete(m_fx);

    inherited::Release	();
}

CKinematicsAnimated::~CKinematicsAnimated	()
{
	IBoneInstances_Destroy	();
}

void	CKinematicsAnimated::IBoneInstances_Create()
{
    inherited::IBoneInstances_Create();
	u32				size	= bones->size();
	blend_instances			= xr_alloc<CBlendInstance>(size);
	for (u32 i=0; i<size; i++)
		blend_instances[i].construct();
}

void	CKinematicsAnimated::IBoneInstances_Destroy()
{
    inherited::IBoneInstances_Destroy();
	if (blend_instances) {
		xr_free(blend_instances);
		blend_instances = NULL;
	}
}

#define PCOPY(a)	a = pFrom->a
void CKinematicsAnimated::Copy(IRender_Visual *P) 
{
	inherited::Copy	(P);

	CKinematicsAnimated* pFrom = (CKinematicsAnimated*)P;
	PCOPY			(m_Motions);
    PCOPY			(m_Partition);

	IBlend_Startup			();
}

void CKinematicsAnimated::Spawn	()
{
	inherited::Spawn		();

	IBlend_Startup			();

	for (u32 i=0; i<bones->size(); i++)
		blend_instances[i].construct();
}

void CKinematicsAnimated::IBlend_Startup	()
{
	_DBG_SINGLE_USE_MARKER;
	CBlend B; ZeroMemory(&B,sizeof(B));
	B.blend				= CBlend::eFREE_SLOT;
	blend_pool.clear	();
	for (u32 i=0; i<MAX_BLENDED_POOL; i++)
		blend_pool.push_back(B);

	// cycles+fx clear
	for (i=0; i<MAX_PARTS; i++)
		blend_cycles[i].clear();
	blend_fx.clear		();
}

CBlend*	CKinematicsAnimated::IBlend_Create	()
{
	UpdateTracks	();
	_DBG_SINGLE_USE_MARKER;
	CBlend *I=blend_pool.begin(), *E=blend_pool.end();
	for (; I!=E; I++)
		if (I->blend == CBlend::eFREE_SLOT) return I;
	FATAL("Too many blended motions requisted");
	return 0;
}
void CKinematicsAnimated::Load(const char* N, IReader *data, u32 dwFlags)
{
	inherited::Load	(N, data, dwFlags);

	// Globals
	blend_instances		= NULL;
    m_Partition			= NULL;
	Update_LastTime 	= 0;

	// Load animation
    if (data->find_chunk(OGF_S_MOTION_REFS)){
    	string_path		items_nm;
        data->r_stringZ	(items_nm,sizeof(items_nm));
        u32 set_cnt		= _GetItemCount(items_nm);
        R_ASSERT		(set_cnt<MAX_ANIM_SLOT);
		m_Motions.reserve(set_cnt);
    	string_path		nm;
        for (u32 k=0; k<set_cnt; k++){
        	_GetItem	(items_nm,k,nm);
            strcat		(nm,".omf");
            string_path	fn;
            if (!FS.exist(fn, "$level$", nm)){
                if (!FS.exist(fn, "$game_meshes$", nm)){
#ifdef _EDITOR
                    Msg			("!Can't find motion file '%s'.",nm);
                    return;
#else            
                    Debug.fatal	(DEBUG_INFO,"Can't find motion file '%s'.",nm);
#endif
                }
            }
            IReader* MS		= FS.r_open(fn);
            // Check compatibility
            m_Motions.push_back(SMotionsSlot());
            m_Motions.back().motions.create(nm,MS,bones);
            MS->close		();
    	}
    }else{
		string_path	nm;
		strconcat			(nm,N,".ogf");
		m_Motions.push_back(SMotionsSlot());
		m_Motions.back().motions.create(nm,data,bones);
    }

    R_ASSERT				(m_Motions.size());

    m_Partition				= m_Motions[0].motions.partition();
    
	// initialize motions
	for (MotionsSlotVecIt m_it=m_Motions.begin(); m_it!=m_Motions.end(); m_it++){
		SMotionsSlot& MS	= *m_it;
		MS.bone_motions.resize(bones->size());
		for (u32 i=0; i<bones->size(); i++){
			CBoneData* BD		= (*bones)[i];
			MS.bone_motions[i]	= MS.motions.bone_motions(BD->name);
		}
	}

	// Init blend pool
	IBlend_Startup	();

//.	if (motions.cycle()->size()<2)			
//.		Msg("* WARNING: model '%s' has only one motion. Candidate for SkeletonRigid???",N);
}

//------------------------------------------------------------------------------
// calculate
//------------------------------------------------------------------------------
ICF void	KEY_Interp	(CKey& D, CKey& K1, CKey& K2, float delta)
{
	VERIFY			(_valid(delta));
	VERIFY			(delta>=0.f && delta<=1.f);
	D.Q.slerp		(K1.Q,K2.Q,delta);
	D.T.lerp		(K1.T,K2.T,delta);
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

/*
ICF float smooth(float x)
{
    float x0	= x*2.f-1.f;
    float s 	= (x0<0.f)?-1.f:1.f;

    return ((s*pow(_abs(x0),1.f/1.5f))+1.f)/2.f;
}
*/

void CKinematicsAnimated::Bone_Calculate(CBoneData* bd, Fmatrix *parent)
{
	u16 SelfID					= bd->GetSelfID();
    if (LL_GetBoneVisible(SelfID)){
        CBoneInstance& BONE_INST	= LL_GetBoneInstance(SelfID);
        CBlendInstance& BLEND_INST	= LL_GetBlendInstance(SelfID);

        if (BONE_INST.Callback_overwrite){
            if (BONE_INST.Callback)	BONE_INST.Callback(&BONE_INST);
        } else {
            CKey				R[MAX_BLENDED];
            ConsistantKey		S[MAX_BLENDED];

            // Calculate all keys
            CKey*	D					= R;
            BlendSVecIt					BI;
            for (BI=BLEND_INST.Blend.begin(); BI!=BLEND_INST.Blend.end(); BI++,D++)
            {
                CBlend*			B		=	*BI;
                float			time	=	B->timeCurrent*float(SAMPLE_FPS);
                CMotion&		M		=	*LL_GetMotion(B->motionID,SelfID);
                u32				frame	=	iFloor(time);
                float			delta	=	time-float(frame);
                u32				count	=	M.get_count();
                // rotation
                if (M.test_flag(flRKeyAbsent)){
					CKeyQR*		K		=	&M._keysR[0];
					D->Q.x		= float(K->x)*KEY_QuantI;
					D->Q.y		= float(K->y)*KEY_QuantI;
					D->Q.z		= float(K->z)*KEY_QuantI;
					D->Q.w		= float(K->w)*KEY_QuantI;
                }else{
					CKeyQR*		K1r		=	&M._keysR[(frame+0)%count];
					CKeyQR*		K2r		=	&M._keysR[(frame+1)%count];
					Fquaternion	Q1,Q2;
                    Q1.x		= float(K1r->x)*KEY_QuantI;
                    Q1.y		= float(K1r->y)*KEY_QuantI;
                    Q1.z		= float(K1r->z)*KEY_QuantI;
                    Q1.w		= float(K1r->w)*KEY_QuantI;

                    Q2.x		= float(K2r->x)*KEY_QuantI;
                    Q2.y		= float(K2r->y)*KEY_QuantI;
                    Q2.z		= float(K2r->z)*KEY_QuantI;
                    Q2.w		= float(K2r->w)*KEY_QuantI;

                    D->Q.slerp	(Q1,Q2,clampr(delta,0.f,1.f));
                }

                // translate
                if (M.test_flag(flTKeyPresent))
				{
	                CKeyQT*	K1t	= &M._keysT[(frame+0)%count];
    	            CKeyQT*	K2t	= &M._keysT[(frame+1)%count];

                    Fvector T1,T2;
                    T1.x		= float(K1t->x)*M._sizeT.x+M._initT.x;
                    T1.y		= float(K1t->y)*M._sizeT.y+M._initT.y;
                    T1.z		= float(K1t->z)*M._sizeT.z+M._initT.z;
                    T2.x		= float(K2t->x)*M._sizeT.x+M._initT.x;
                    T2.y		= float(K2t->y)*M._sizeT.y+M._initT.y;
                    T2.z		= float(K2t->z)*M._sizeT.z+M._initT.z;
                    
	                D->T.lerp	(T1,T2,delta);
/*					
					if ((_abs(D->T.y)>10000) || (_abs(D->T.x)>10000) || (_abs(D->T.z)>10000))
					{
						Log("xxx");
						Log("Blend--------");
						Log("blendAmount", B->blendAmount);
						Log("timeCurrent", B->timeCurrent);
						Log("timeTotal", B->timeTotal);
						Log("bone_or_part", B->bone_or_part);

						Log("blendAccrue", B->blendAccrue);
						Log("blendFalloff", B->blendFalloff);
						Log("blendPower", B->blendPower);
						Log("speed", B->speed);
						Log("playing", B->playing);
						Log("stop_at_end", B->stop_at_end);
						Log("motionID", (u32)B->motionID.idx);
						Log("blend", B->blend);

						Log("dwFrame", B->dwFrame);
						Log("Device.dwFrame", Device.dwFrame);
						Log("Blend-------end");

						Log("Bone",LL_BoneName_dbg(SelfID));
						Log("parent",*parent);
						Msg("K1t %d,%d,%d",K1t->x,K1t->y,K1t->z);
						Msg("K2t %d,%d,%d",K2t->x,K2t->y,K2t->z);

						Log("count",count);
						Log("time",time);
						Log("frame",frame);
						Log("T1",T1);
						Log("T2",T2);
						Log("delta",delta);
						Log("Dt",D->T);
						VERIFY(0);

					}
*/
				} //if (M.test_flag(flTKeyPresent))
				else
				{
	                D->T.set	(M._initT);
                }

//                PSGP.blerp				(D,&K1,&K2,delta);
            }

            // Blend them together
            BI				= BLEND_INST.Blend.begin();

            CKey			Result;

            switch (BLEND_INST.Blend.size())
            {
            case 0:
                Result.Q.set	(0,0,0,0);
                Result.T.set	(0,0,0);
                break;
            case 1: 
                Result			= R[0];
/*
				if(Result.T.y>10000){
					Log("1");
					Log("BLEND_INST",BLEND_INST.Blend.size());
					Log("Bone",LL_BoneName_dbg(SelfID));
					Msg("Result.Q %f,%f,%f,%f",Result.Q.x,Result.Q.y,Result.Q.z,Result.Q.w);
					Log("Result.T",Result.T);
					VERIFY(0);
				}
*/
                break;
            case 2:
                {
                    float w0 = BI[0]->blendAmount;
                    float w1 = BI[1]->blendAmount;
                    float ws = w0+w1;
                    float w;
                    if (fis_zero(ws))	w = 0;
                    else				w = w1/ws;
#ifdef DEBUG
//.					if (fis_zero(w0+w1) || (!_valid(w))){
//.						Debug.fatal		(DEBUG_INFO,"TO ALEXMX VERY IMPORTANT: (TOTAL: %f) w: %f, w0: %f, w1: %f, ws:%f, BIS: %d",w0+w1,w,w0,w1,ws,BLEND_INST.Blend.size());
//.					}
#endif
                    KEY_Interp	(Result,R[0],R[1], clampr(w,0.f,1.f));
/*
					if(Result.T.y>10000){
						Log("2");
						Log("BLEND_INST",BLEND_INST.Blend.size());
						Log("Bone",LL_BoneName_dbg(SelfID));
						Msg("Result.Q %f,%f,%f,%f",Result.Q.x,Result.Q.y,Result.Q.z,Result.Q.w);
						Log("Result.T",Result.T);
						Log("parent",*parent);
						VERIFY(0);
					}
*/
                }
                break;
            default:
                {
                    int 	count 	= BLEND_INST.Blend.size();
                    float   total 	= 0;
					for (int i=0; i<count; i++)
						S[i].set	(R+i,BI[i]->blendAmount);

					std::sort	(S,S+count);
                    CKey		tmp;
                    total		= S[0].w;
                    tmp			= *S[0].K;
                    for 		(int cnt=1; cnt<count; cnt++){
                    	total	+= S[cnt].w;
						float	d;
						if (fis_zero(total))	d = 0.0f;
						else d	= S[cnt].w/total;

						clampr(d,0.f,1.f);

#ifdef DEBUG
//.						if ((total==0) || (!_valid(S[cnt].w/total))){
//.							Debug.fatal		(DEBUG_INFO,"TO ALEXMX VERY IMPORTANT: (TOTAL: %f) w: %f, total: %f, count: %d, real count: %d",total,S[cnt].w,total,count,BLEND_INST.Blend.size());
//.						}
#endif

                    	KEY_Interp	(Result,tmp, *S[cnt].K, d );
                        tmp 		= Result;
                    }
                }
                break;
            }

            // Build matrix
            Fmatrix					RES;
            RES.mk_xform			(Result.Q,Result.T);
            BONE_INST.mTransform.mul_43(*parent,RES);
/*
			if(BONE_INST.mTransform.c.y>10000)
			{
				Log("BLEND_INST",BLEND_INST.Blend.size());
				Log("Bone",LL_BoneName_dbg(SelfID));
				Msg("Result.Q %f,%f,%f,%f",Result.Q.x,Result.Q.y,Result.Q.z,Result.Q.w);
				Log("Result.T",Result.T);
				Log("lp parent",(u32)parent);
				Log("parent",*parent);
				Log("RES",RES);
				Log("mT",BONE_INST.mTransform);

                CBlend*			B		=	*BI;
                CMotion&		M		=	*LL_GetMotion(B->motionID,SelfID);
                float			time	=	B->timeCurrent*float(SAMPLE_FPS);
                u32				frame	=	iFloor(time);
                u32				count	=	M.get_count();
                float			delta	=	time-float(frame);

				Log("flTKeyPresent",M.test_flag(flTKeyPresent));
				Log("M._initT",M._initT);
				Log("M._sizeT",M._sizeT);
				
                // translate
                if (M.test_flag(flTKeyPresent))
				{
	                CKeyQT*	K1t	= &M._keysT[(frame+0)%count];
    	            CKeyQT*	K2t	= &M._keysT[(frame+1)%count];

                    Fvector T1,T2,Dt;
                    T1.x		= float(K1t->x)*M._sizeT.x+M._initT.x;
                    T1.y		= float(K1t->y)*M._sizeT.y+M._initT.y;
                    T1.z		= float(K1t->z)*M._sizeT.z+M._initT.z;
                    T2.x		= float(K2t->x)*M._sizeT.x+M._initT.x;
                    T2.y		= float(K2t->y)*M._sizeT.y+M._initT.y;
                    T2.z		= float(K2t->z)*M._sizeT.z+M._initT.z;
                    
	                Dt.lerp	(T1,T2,delta);

					Msg("K1t %d,%d,%d",K1t->x,K1t->y,K1t->z);
					Msg("K2t %d,%d,%d",K2t->x,K2t->y,K2t->z);

					Log("count",count);
					Log("frame",frame);
					Log("T1",T1);
					Log("T2",T2);
					Log("delta",delta);
					Log("Dt",Dt);

				}else
				{
					D->T.set	(M._initT);
                }
				VERIFY(0);
			}
*/
            if (BONE_INST.Callback)		BONE_INST.Callback(&BONE_INST);
        }
        BONE_INST.mRenderTransform.mul_43(BONE_INST.mTransform,bd->m2b_transform);

        // Calculate children
        for (xr_vector<CBoneData*>::iterator C=bd->children.begin(); C!=bd->children.end(); C++)
            Bone_Calculate(*C,&BONE_INST.mTransform);
    }
}

void CKinematicsAnimated::OnCalculateBones		()
{
	UpdateTracks	()	;
}

#ifdef _EDITOR
MotionID CKinematicsAnimated::ID_Motion(LPCSTR  N, u16 slot)
{
	MotionID 				motion_ID;
    if (slot<MAX_ANIM_SLOT){
        shared_motions* s_mots	= &m_Motions[slot].motions;
        // find in cycles
        accel_map::iterator I 	= s_mots->cycle()->find(LPSTR(N));
        if (I!=s_mots->cycle()->end())	motion_ID.set(slot,I->second);
        if (!motion_ID.valid()){
            // find in fx's
            accel_map::iterator I 	= s_mots->fx()->find(LPSTR(N));
            if (I!=s_mots->fx()->end())	motion_ID.set(slot,I->second);
        }
    }
    return motion_ID;
}
#endif
