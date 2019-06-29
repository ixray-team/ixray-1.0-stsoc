////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_crow.cpp
//	Created 	: 13.05.2002
//  Modified 	: 13.05.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Crow"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_idol.h"
#include "../../inventory.h"
#include "../../phmovementcontrol.h"
#include "../../xrserver_objects_alife_monsters.h"
#include "../../../skeletonanimated.h"

CAI_Idol::CAI_Idol					()
{
	
	m_tpCurrentBlend				= 0;
	m_bPlaying						= false;
	m_dwCurrentAnimationIndex		= 0;
	m_PhysicMovementControl->AllocateCharacterObject(CPHMovementControl::CharacterType::ai);
}

CAI_Idol::~CAI_Idol					()
{
}

void CAI_Idol::Load					(LPCSTR section)
{
	setEnabled						(false);
	inherited::Load					(section);
}

BOOL CAI_Idol::net_Spawn			(LPVOID DC)
{
	CSE_Abstract					*e	= (CSE_Abstract*)(DC);
	CSE_ALifeObjectIdol				*tpIdol	= smart_cast<CSE_ALifeObjectIdol*>(e);
	
	R_ASSERT						(tpIdol);
	
	if (!inherited::net_Spawn(DC))	return FALSE;
	
	m_dwAnyPlayType					= tpIdol->m_dwAniPlayType;
	m_tpaAnims.clear				();
	
	m_body.current.yaw				= m_body.target.yaw	= -tpIdol->o_Angle.y;
	m_body.current.pitch			= m_body.target.pitch	= 0;
	
	u32								N = _GetItemCount(*tpIdol->m_caAnimations);
	string32						I;
	for (u32 i=0; i<N; ++i)
		m_tpaAnims.push_back		(smart_cast<CSkeletonAnimated*>(Visual())->ID_Cycle(_GetItem(*tpIdol->m_caAnimations,i,I)));

	return							TRUE;
}

void CAI_Idol::SelectAnimation		(const Fvector& /**_view/**/, const Fvector& /**_move/**/, float /**speed/**/)
{
	//R_ASSERT						(!m_tpaAnims.empty());
	if (m_tpaAnims.empty())
		return;
	if (g_Alive()) {
		switch (m_dwAnyPlayType) {
			case 0 : {
				if (!m_bPlaying) {
					m_tpCurrentBlend		= smart_cast<CSkeletonAnimated*>(Visual())->PlayCycle	(m_tpaAnims[::Random.randI((int)m_tpaAnims.size())],TRUE,AnimCallback,this);
					m_bPlaying				= true;
				}
				break;
			}
			case 1 : {
				if (!m_bPlaying) {
					m_tpCurrentBlend		= smart_cast<CSkeletonAnimated*>(Visual())->PlayCycle	(m_tpaAnims[m_dwCurrentAnimationIndex],TRUE,AnimCallback,this);
					m_bPlaying				= true;
					m_dwCurrentAnimationIndex = (m_dwCurrentAnimationIndex + 1) % m_tpaAnims.size();
				}
				break;
			}
			case 2 : {
				if (!m_bPlaying) {
					m_tpCurrentBlend		= smart_cast<CSkeletonAnimated*>(Visual())->PlayCycle	(m_tpaAnims[m_dwCurrentAnimationIndex],TRUE,AnimCallback,this);
					m_bPlaying				= true;
					if (m_dwCurrentAnimationIndex < m_tpaAnims.size() - 1)
						++m_dwCurrentAnimationIndex;
				}
				break;
			}
			default : NODEFAULT;
		}
	}
}

IC BOOL BE	(BOOL A, BOOL B)
{
	bool a = !!A;
	bool b = !!B;
	return a==b;
}

void CAI_Idol::OnEvent		(NET_Packet& P, u16 type)
{
	inherited::OnEvent		(P,type);

	u16 id;
	switch (type)
	{
	case GE_OWNERSHIP_TAKE:
		{
			P.r_u16		(id);
			CObject* O	= Level().Objects.net_Find	(id);

			if (bDebug)	Log("Trying to take - ", *O->cName());
			if(g_Alive() && inventory().Take(smart_cast<CGameObject*>(O), false, false)) {
				O->H_SetParent(this);
				if (bDebug)	Log("TAKE - ", *O->cName());
			}
		}
		break;
	case GE_OWNERSHIP_REJECT:
		{
			P.r_u16		(id);
			CObject* O	= Level().Objects.net_Find	(id);
			
			if(inventory().Drop(smart_cast<CGameObject*>(O))) {
				O->H_SetParent(0);
				feel_touch_deny(O,2000);
			}

		}
		break;
	case GE_TRANSFER_AMMO:
		{
		}
		break;
	}
}

void CAI_Idol::feel_touch_new				(CObject* O)
{
	if (!g_Alive())		return;
	if (Remote())		return;

	// Now, test for game specific logical objects to minimize traffic
	CInventoryItem		*I	= smart_cast<CInventoryItem*>	(O);

	if (I && I->useful_for_NPC()) {
		Msg("Taking item %s!",*I->cName());
		NET_Packet		P;
		u_EventGen		(P,GE_OWNERSHIP_TAKE,ID());
		P.w_u16			(u16(I->ID()));
		u_EventSend		(P);
	}
}

void CAI_Idol::DropItemSendMessage(CObject *O)
{
	if (!O || !O->H_Parent() || (this != O->H_Parent()))
		return;

	Msg("Dropping item!");
	// We doesn't have similar weapon - pick up it
	NET_Packet				P;
	u_EventGen				(P,GE_OWNERSHIP_REJECT,ID());
	P.w_u16					(u16(O->ID()));
	u_EventSend				(P);
}

void CAI_Idol::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
}

void CAI_Idol::g_WeaponBones	(int &L, int &R1, int &R2)
{
	if (g_Alive() && inventory().ActiveItem()) {
		CKinematics *V	= smart_cast<CKinematics*>(Visual());
		R1				= V->LL_BoneID("bip01_r_hand");
		R2				= V->LL_BoneID("bip01_r_finger2");
		L				= V->LL_BoneID("bip01_l_finger1");
	}
}

void CAI_Idol::renderable_Render	()
{
	inherited::renderable_Render	();
	if(inventory().ActiveItem())
		inventory().ActiveItem()->renderable_Render();
}

void CAI_Idol::g_fireParams(const CHudItem* pHudItem, Fvector& P, Fvector& D)
{
	if (g_Alive() && inventory().ActiveItem()) {
		Center(P);
		D.setHP(0,0);
		D.normalize_safe();
	}
}

void CAI_Idol::AnimCallback	(CBlend* B)
{
	CAI_Idol		*tpIdol = (CAI_Idol*)B->CallbackParam;
	tpIdol->m_bPlaying = false;
}

