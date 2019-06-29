////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_idol.h
//	Created 	: 16.04.2003
//  Modified 	: 16.04.2003
//	Author		: Dmitriy Iassenev
//	Description : No AI, just an idol
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../../CustomMonster.h"
#include "../../inventoryowner.h"

class CMotionDef;
class CBlend;

class CAI_Idol : public CCustomMonster, public CInventoryOwner 
{
public:
	typedef CCustomMonster inherited;
	xr_vector<CMotionDef*>	m_tpaAnims;
	CBlend				*m_tpCurrentBlend;
	bool				m_bPlaying;
	u32					m_dwAnyPlayType;
	u32					m_dwCurrentAnimationIndex;
	
						CAI_Idol		();
	virtual				~CAI_Idol		();
public:
	virtual CInventoryOwner*			cast_inventory_owner		()						{return this;}
	virtual CEntityAlive*				cast_entity_alive			()						{return this;}
	virtual CEntity*					cast_entity					()						{return this;}
	virtual CGameObject*				cast_game_object			()						{return this;}

public:

	virtual void		Load			( LPCSTR section );
	virtual BOOL		net_Spawn		( LPVOID DC );
	virtual void		net_Export		(NET_Packet& /**P/**/){};
	virtual void		net_Import		(NET_Packet& /**P/**/){};
	virtual void		Think			(){};
	virtual void		HitSignal		(float /**P/**/, Fvector &/**local_dir/**/,	CObject* /**who/**/, s16 /**element/**/){};
	virtual void		HitImpulse		(float /**P/**/, Fvector &/**vWorldDir/**/, 	Fvector& /**vLocalDir/**/){};
	virtual void		SelectAnimation	(const Fvector& _view, const Fvector& _move, float speed);
	
	static void			AnimCallback	(CBlend* B);
	virtual void		g_fireParams	(const CHudItem* pHudItem, Fvector& P, Fvector& D);
	virtual void		g_WeaponBones	(int &L, int &R1, int &R2);
	virtual	float		ffGetFov		()const{return 150.f;}
	virtual	float		ffGetRange		()const{return 30.f;}
	virtual	void		OnEvent			(NET_Packet& P, u16 type);
	virtual	void		DropItemSendMessage(CObject *O);

	virtual BOOL		feel_vision_isRelevant	(CObject* /**O/**/)	{ return FALSE;	}
	virtual	void		feel_touch_new			(CObject* O);
	virtual void		shedule_Update			(u32 dt);
	virtual void		renderable_Render		();
};
