#include "stdafx.h"
#include "targetcs.h"
#include "targetcscask.h"
#include "game_cl_base.h"
#include "level.h"
#include "../skeletonanimated.h"

CTargetCS::CTargetCS()
{
}

CTargetCS::~CTargetCS()
{
}

BOOL CTargetCS::net_Spawn(LPVOID DC) {
	if (!inherited::net_Spawn(DC))
		return			(FALSE);

	CSkeletonAnimated	*V	= smart_cast<CSkeletonAnimated*>(CTargetCS::Visual());
	if (V)
		V->PlayCycle	("idle");

	return				(TRUE);
}

void CTargetCS::net_Destroy			()
{
	inherited::net_Destroy();
	Game().targets.erase(std::find(Game().targets.begin(), Game().targets.end(), this));
}

void CTargetCS::OnH_A_Chield()
{
	inherited::OnH_A_Chield		();
}

void CTargetCS::OnH_B_Independent() {
	inherited::OnH_B_Independent();
}

void CTargetCS::UpdateCL		()
{
	inherited::UpdateCL();
	if(smart_cast<CTargetCSCask*>(H_Parent())) {
		//XFORM().set			();
		XFORM().mul				(H_Parent()->XFORM(), m_pos);
		Position().set			(XFORM().c);
		setVisible				(true);
		setEnabled				(true);
	}
}

void CTargetCS::	Hit					(float P, Fvector &dir,	CObject* who, 
											 s16 element,Fvector p_in_object_space, 
											 float impulse, ALife::EHitType hit_type /*ALife::eHitTypeWound*/)
{
	inherited::Hit(P,dir,who,element,p_in_object_space,impulse,hit_type);
}
void CTargetCS::SetPos(const Fmatrix& pos) {
	m_pos.set					(pos);
	XFORM().set					(m_pos);
	Position().set				(XFORM().c);
}

void CTargetCS::Load				(LPCSTR section)
{
	inherited::Load				(section);
}

void CTargetCS::net_Export			(NET_Packet& P)
{
	inherited::net_Export		(P);
}

void CTargetCS::net_Import			(NET_Packet& P)
{
	inherited::net_Import		(P);
}

void CTargetCS::shedule_Update		(u32 dt)
{
	inherited::shedule_Update	(dt);
}

void CTargetCS::reload				(LPCSTR section)
{
	inherited::reload			(section);
}

void CTargetCS::reinit				()
{
	inherited::reinit			();
}

void CTargetCS::OnH_B_Chield		()
{
	inherited::OnH_B_Chield		();
}


void CTargetCS::create_physic_shell	()
{
	CPhysicItem::create_physic_shell();
}
 
void CTargetCS::activate_physic_shell	()
{
	inherited::activate_physic_shell();
}

void CTargetCS::setup_physic_shell		()
{
	inherited::setup_physic_shell();
}
