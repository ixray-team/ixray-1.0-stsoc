#include "stdafx.h"
#include "targetcscask.h"
#include "targetcs.h"
#include "xrmessages.h"
#include "level.h"
#include "../skeletonanimated.h"

CTargetCSCask::CTargetCSCask(void) {}

CTargetCSCask::~CTargetCSCask(void) {}

IC BOOL BE(BOOL A, BOOL B) 
{
	bool a = !!A;
	bool b = !!B;
	return a==b;
}

void CTargetCSCask::OnEvent(NET_Packet& P, u16 type) 
{
	inherited::OnEvent(P,type);
	u16 id;
	switch (type) {
		case GE_OWNERSHIP_TAKE : {
			P.r_u16(id);
			CObject *l_pObj = Level().Objects.net_Find(id);
			///Log			("----------- void CTargetCSCask::OnEvent --- TAKE",l_pObj->cName());

			// Test for CS Target
			CTargetCS* l_pBall = smart_cast<CTargetCS*>(l_pObj);
			if(l_pBall) {
				R_ASSERT		(BE(Local(),l_pBall->Local()));	// remote can't take local
				l_pBall->H_SetParent(this);
				CKinematics* l_V = smart_cast<CKinematics*>(Visual());
				m_targets.push_back(l_pBall);
				char l_num[2]	= { char(0x30+m_targets.size()), 0 };
				int l_boneID	= l_V->LL_BoneID(l_num);
				VERIFY			(l_V); l_V->CalculateBones();
				Fmatrix& l_pos	= l_V->LL_GetTransform(u16(l_boneID));
				l_pBall->SetPos(l_pos);
				return;
			}
		} break;
		case GE_OWNERSHIP_REJECT :{
			P.r_u16(id);
			CObject* l_pObj	= Level().Objects.net_Find(id);
			//Log			("----------- void CTargetCSCask::OnEvent --- REJECT",l_pObj->cName());

			// Test for CS Target
			CTargetCS* l_pBall	= smart_cast<CTargetCS*>(l_pObj);
			if(l_pBall) {
				// R_ASSERT(BE(Local(),l_pBall->Local()));	// remote can't eject local
				l_pBall->H_SetParent(0);
				m_targets.erase(std::find(m_targets.begin(), m_targets.end(), l_pBall));
				return;
			}
		} break;
	}
}

void CTargetCSCask::renderable_Render() {
	inherited::renderable_Render();
	xr_list<CTargetCS*>::iterator l_it = m_targets.begin();
	while(m_targets.end() != l_it) { (*l_it)->renderable_Render(); ++l_it; }
}

void CTargetCSCask::Load	(LPCSTR section)
{
	inherited::Load(section);

	CSkeletonAnimated* V	= smart_cast<CSkeletonAnimated*>(Visual());
	if(V)			V->PlayCycle	("open");
}
