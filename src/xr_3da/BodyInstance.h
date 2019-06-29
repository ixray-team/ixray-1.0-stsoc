// BodyInstance.h: interface for the CBodyInstance class.
//
//////////////////////////////////////////////////////////////////////

#ifndef BodyInstanceH
#define BodyInstanceH
#pragma once

#include		"fhierrarhyvisual.h"
#include		"bone.h"

// consts
const	u32		MAX_BONE_PARAMS		=	4;
const	u32		MAX_BLENDED			=	16;
const	u32		MAX_PARTS			=	4;
const	u32		MAX_BLENDED_POOL	=	(MAX_BLENDED*MAX_PARTS);
const	f32		SAMPLE_FPS			=	30.f;
const	f32		SAMPLE_SPF			=	(1.f/SAMPLE_FPS);

const	u16		BI_NONE				=	u16(-1);

// refs
class	ENGINE_API CKinematics;
struct	ENGINE_API CKey;
struct	ENGINE_API CKeyQ;
class	ENGINE_API CBoneData;
class	ENGINE_API CInifile;
class   ENGINE_API CBoneInstance;
class   ENGINE_API CBlend;

// t-defs
typedef xr_vector<CBoneData*>	vecBones;
typedef vecBones::iterator		vecBonesIt;

typedef void (__stdcall * BoneCallback)		(CBoneInstance* P);
typedef void (__stdcall * PlayCallback)		(CBlend*		P);
typedef void (__stdcall * UpdateCallback)	(CKinematics*	P);

//*** Key frame definition ************************************************************************
#pragma pack(push,2)
const float KEY_Quant		= 32767.f;
const float KEY_QuantI		= 1.f/KEY_Quant;
struct ENGINE_API CKey
{
	Fquaternion	Q;			// rotation
	Fvector		T;			// translation
};
struct ENGINE_API CKeyQ
{
	s16			x,y,z,w;	// rotation
	Fvector		t;			// translation
};
#pragma pack(pop)

//*** Run-time Blend definition *******************************************************************
class ENGINE_API CBlend {
public:
	enum ECurvature
	{
		eFREE_SLOT=0,
		eFixed,
		eAccrue,
		eFalloff,
		eFORCEDWORD = u32(-1)
	};
public:
	float			blendAmount;
	float			timeCurrent;
	float			timeTotal;
	u16				motionID;
	u16				bone_or_part;	// startup parameters

	ECurvature		blend;
	float			blendAccrue;	// increasing
	float			blendFalloff;	// decreasing
	float			blendPower;			
	float			speed;

	BOOL			playing;
	BOOL			stop_at_end;

	PlayCallback	Callback;
	void*			CallbackParam;
	
	u32				dwFrame;
};
typedef svector<CBlend*,MAX_BLENDED>	BlendList;
typedef BlendList::iterator				BlendListIt;

//*** Motion Data *********************************************************************************
class ENGINE_API		CMotion
{
public:
	CKeyQ*				_keys;
	u32					_count;
	float				GetLength()				{ return float(_count)*SAMPLE_SPF; }
};

//*** Bone Instance *******************************************************************************
#pragma pack(push,8)
class ENGINE_API		CBoneInstance
{
public:
	// data
	Fmatrix				mTransform;							// final x-form matrix
	BlendList			Blend;
	BoneCallback		Callback;
	void*				Callback_Param;
	BOOL				Callback_overwrite;					// performance hint - don't calc anims
	float				param			[MAX_BONE_PARAMS];	// 

	// methods
	void				construct		();
	void				blend_add		(CBlend* H);
	void				blend_remove	(CBlend* H);
	void				set_callback	(BoneCallback C, void* Param);
	void				set_param		(u32 idx, float data);
	float				get_param		(u32 idx);
};
#pragma pack(pop)

//*** Shared Bone Data ****************************************************************************
class ENGINE_API		CBoneData
{
public:
	typedef svector<int,128>	BoneDebug;
public:
	u16					SelfID;
	vecBones			children;		// bones which are slaves to this
	xr_vector<CMotion>	Motions;		// all known motions
	Fobb				obb;			

	Fvector				bind_xyz;
	Fvector				bind_translate;
    SBoneShape			shape;
    string64			game_mtl;
    SJointIKData		IK_data;
    float				mass;
    Fvector				center_of_mass;
    
	// Motion control
	void				Motion_Start	(CKinematics* K, CBlend* handle);	// with recursion
	void				Motion_Start_IM	(CKinematics* K, CBlend* handle);
	void				Motion_Stop		(CKinematics* K, CBlend* handle);	// with recursion
	void				Motion_Stop_IM	(CKinematics* K, CBlend* handle);

	// Calculation
	void				Calculate		(CKinematics* K, Fmatrix *Parent);
	void				DebugQuery		(BoneDebug& L);

	CBoneData(u16 ID) : SelfID	(ID)	{}
};

//*** Shared motion Data **************************************************************************
class ENGINE_API CMotionDef
{
public:
	u16			bone_or_part;
	u16			motion;
	u16			speed;		// quantized: 0..10
	u16			power;		// quantized: 0..10
	u16			accrue;		// quantized: 0..10
	u16			falloff;	// quantized: 0..10
    u32			flags;

	IC float	Dequantize	(u16 V)		{	return  float(V)/655.35f; }
	IC u16		Quantize	(float V)	{	s32		t = iFloor(V*655.35f); clamp(t,0,65535); return u16(t); }

	void		Load		(CKinematics* P, CInifile* INI, LPCSTR section, BOOL bCycle);
	void		Load		(CKinematics* P, IReader* MP, u32 fl);
	CBlend*		PlayCycle	(CKinematics* P, BOOL	bMixIn, PlayCallback Callback, LPVOID Callback_Param);
	CBlend*		PlayCycle	(CKinematics* P, u16	part, BOOL bMixIn, PlayCallback Callback, LPVOID Callback_Param);
	CBlend*		PlayFX		(CKinematics* P, float	power_scale);
};

//*** Shared partition Data ***********************************************************************
class ENGINE_API	CPartDef
{
public:
	ref_str			Name;
	xr_vector<u32>	bones;
	CPartDef()		: Name(0) {};
};
class ENGINE_API	CPartition
{
	CPartDef		P[MAX_PARTS];
public:
	IC CPartDef&	operator[] (u16 id)	{ return P[id]; }
};

//*** The visual itself ***************************************************************************
class ENGINE_API	CKinematics	: public FHierrarhyVisual
{
	typedef FHierrarhyVisual					inherited;
	friend class								CBoneData;
	friend class								CMotionDef;
	friend class								CSkeletonX;
private:
	struct str_pred : public std::binary_function<ref_str, ref_str, bool>	{	
		IC bool operator()(const ref_str& x, const ref_str& y) const	{	return strcmp(*x,*y)<0;	}
	};
public:
	typedef xr_map<ref_str,u16,str_pred>		accel;
	typedef xr_map<ref_str,CMotionDef,str_pred>	mdef;
private:
	// Globals
    CInifile*									pUserData;
	vecBones*									bones;			// all bones+motions	(shared)
	CBoneInstance*								bone_instances;	// bone instances
	u16											iRoot;			// Root bone index

	// Fast search
	accel*										motion_map;		// motion assotiations	(shared)
	accel*										bone_map;		// bones  assotiations	(shared)
	mdef*										m_cycle;		// motion data itself	(shared)
	mdef*										m_fx;			// motion data itself	(shared)

	// Partition
	CPartition*									partition;

	// Blending
	svector<CBlend, MAX_BLENDED_POOL>			blend_pool;
	BlendList									blend_cycles	[MAX_PARTS];
	BlendList									blend_fx;

	s32											Update_ID;
	u32											Update_LastTime;
	u32											Update_Frame;

public:
	UpdateCallback								Update_Callback;
	void*										Update_Callback_Param;

protected:
	// internal functions
	void						IBoneInstances_Create	();
	void						IBoneInstances_Destroy	();

	void						IBlend_Startup			();
	CBlend*						IBlend_Create			();

public:
	// Low level interface
	u16							LL_BoneID		(LPCSTR B);
	u16							LL_MotionID		(LPCSTR B);
	u16							LL_PartID		(LPCSTR B);

    CInifile*					LL_UserData		(){return pUserData;}
	accel*						LL_Bones		(){return bone_map;}
	accel*						LL_Motions		(){return motion_map;}
	CBoneInstance&				LL_GetInstance	(u16 bone_id)		{	VERIFY(bone_id<LL_BoneCount()); return bone_instances[bone_id];	}
	CBoneData&					LL_GetData		(u16 bone_id)		{	VERIFY(bone_id<LL_BoneCount()); return *((*bones)[bone_id]);	}
	u16							LL_BoneRoot		()					{	return iRoot;													}
	u16							LL_BoneCount	()					{	return u16(bones->size());										}
	Fmatrix&					LL_GetTransform	(u16 bone_id)		{	return LL_GetInstance(bone_id).mTransform;						}
	Fobb&						LL_GetBox		(u16 bone_id)		{	VERIFY(bone_id<LL_BoneCount());	return (*bones)[bone_id]->obb;	}

	CBlend*						LL_PlayFX		(u16 bone,		u16		motion, float blendAccrue,	float blendFalloff, float Speed, float Power);
	CBlend*						LL_PlayCycle	(u16 partition, u16		motion, BOOL  bMixing,		float blendAccrue,	float blendFalloff, float Speed, BOOL noloop, PlayCallback Callback, LPVOID Callback_Param);
	void						LL_FadeCycle	(u16 partition, float	falloff);
	void						LL_CloseCycle	(u16 partition);
	
	// Main functionality
	void						Update			();								// Update motions
	void						Calculate		(BOOL bLight=FALSE);			// Recalculate skeleton (Light mode can be used to avoid interpolation)
	void						Callback		(UpdateCallback C, void* Param)	{	Update_Callback	= C; Update_Callback_Param	= Param;	}

	// cycles
	CMotionDef*					ID_Cycle		(LPCSTR  N);
	CMotionDef*					ID_Cycle_Safe	(LPCSTR  N);
	CBlend*						PlayCycle		(LPCSTR  N,  BOOL bMixIn=TRUE, PlayCallback Callback=0, LPVOID CallbackParam=0);
	CBlend*						PlayCycle		(CMotionDef* M,  BOOL bMixIn=TRUE, PlayCallback Callback=0, LPVOID CallbackParam=0)
	{	VERIFY(M); return M->PlayCycle(this,bMixIn,Callback,CallbackParam); }
	void						Invalidate		()
	{	Update_Frame = 0xffffff; }

	// fx'es
	CMotionDef*					ID_FX			(LPCSTR  N);
	CMotionDef*					ID_FX_Safe		(LPCSTR  N);
	CBlend*						PlayFX			(LPCSTR  N, float power_scale);
	CBlend*						PlayFX			(CMotionDef* M, float power_scale)
	{	VERIFY(M); return M->PlayFX(this,power_scale);	}

	// debug
	void						DebugRender		(Fmatrix& XFORM);

	// General "Visual" stuff
	virtual void				Copy			(IRender_Visual *pFrom);
	virtual void				Load			(const char* N, IReader *data, u32 dwFlags);
	virtual void				Release			();
	virtual void				Spawn			();
	virtual						~CKinematics	();
};
IC CKinematics* PKinematics		(IRender_Visual* V) { return dynamic_cast<CKinematics*>(V); }

#endif // BodyInstanceH
