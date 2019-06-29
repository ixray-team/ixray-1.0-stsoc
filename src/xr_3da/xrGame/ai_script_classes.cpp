////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_classes.cpp
//	Created 	: 25.09.2003
//  Modified 	: 25.09.2003
//	Author		: Dmitriy Iassenev
//	Description : XRay Script classes
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_script_classes.h"
#include "ai_script_actions.h"
#include "script_hit.h"
#include "inventory_item.h"
#include "weapon.h"
#include "ParticlesObject.h"
#include "PDA.h"
#include "inventory.h"
#include "ai/stalker/ai_stalker.h"
#include "ai/biting/ai_biting.h"
#include "cover_point.h"
#include "cover_evaluators.h"
#include "script_ini_file.h"
#include "xrmessages.h"

#include "AI_PhraseDialogManager.h"
#include "character_info.h"

#include "helicopter.h"

void CLuaGameObject::Hit(CScriptHit &tLuaHit)
{
	NET_Packet		P;
	m_tpGameObject->u_EventGen(P,GE_HIT,m_tpGameObject->ID());
	P.w_u16			(u16(tLuaHit.m_tpDraftsman->ID()));
	P.w_u16			(0);
	P.w_dir			(tLuaHit.m_tDirection);
	P.w_float		(tLuaHit.m_fPower);
	CKinematics		*V = PKinematics(m_tpGameObject->Visual());
	R_ASSERT		(V);
	if (xr_strlen	(tLuaHit.m_caBoneName))
		P.w_s16		(V->LL_BoneID(tLuaHit.m_caBoneName));
	else
		P.w_s16		(s16(0));
	P.w_vec3		(Fvector().set(0,0,0));
	P.w_float		(tLuaHit.m_fImpulse);
	P.w_u16			(u16(tLuaHit.m_tHitType));
	m_tpGameObject->u_EventSend(P);
}

bool CLuaGameObject::GiveInfoPortion(LPCSTR info_index)
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return false;
	if(!pInventoryOwner->GetPDA()) return false;
	pInventoryOwner->TransferInfo(CInfoPortion::StrToID(info_index), true);
	return			true;
}

bool CLuaGameObject::DisableInfoPortion(LPCSTR info_index)
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return false;
	if(!pInventoryOwner->GetPDA()) return false;
	pInventoryOwner->TransferInfo(CInfoPortion::StrToID(info_index), false);
	return true;
}

bool CLuaGameObject::GiveInfoPortionViaPda(LPCSTR info_index, CLuaGameObject* pFromWho)
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return false;
	if(!pInventoryOwner->GetPDA()) return false;

	CInventoryOwner* pFromWhoInvOwner = dynamic_cast<CInventoryOwner*>(pFromWho->m_tpGameObject);
	if(!pFromWhoInvOwner) return false;
	if(!pFromWhoInvOwner->GetPDA()) return false;

	//отправляем от нашему PDA пакет информации с номером
	NET_Packet		P;
	m_tpGameObject->u_EventGen(P,GE_PDA,pInventoryOwner->GetPDA()->ID());
	P.w_u16			(u16(pFromWhoInvOwner->GetPDA()->ID()));		//отправитель
	P.w_s16			(ePdaMsgInfo);									
	P.w_s32			(CInfoPortion::StrToID(info_index));									
	m_tpGameObject->u_EventSend(P);
	return			true;
}


bool  CLuaGameObject::HasInfo				(LPCSTR info_index)
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return false;
	if(!pInventoryOwner->GetPDA()) return false;
	return pInventoryOwner->HasInfo(CInfoPortion::StrToID(info_index));

}
bool  CLuaGameObject::DontHasInfo			(LPCSTR info_index)
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return true;
	if(!pInventoryOwner->GetPDA()) return true;
	return !pInventoryOwner->HasInfo(CInfoPortion::StrToID(info_index));
}


bool CLuaGameObject::SendPdaMessage(EPdaMsg pda_msg, CLuaGameObject* pForWho)
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return false;
	if(!pInventoryOwner->GetPDA()) return false;

	CInventoryOwner* pForWhoInvOwner = dynamic_cast<CInventoryOwner*>(pForWho->m_tpGameObject);
	if(!pForWhoInvOwner) return false;
	if(!pForWhoInvOwner->GetPDA()) return false;

	//отправляем от нашему PDA пакет информации с номером
	NET_Packet		P;
	m_tpGameObject->u_EventGen(P,GE_PDA,pForWhoInvOwner->GetPDA()->ID());
	P.w_u16			(u16(pInventoryOwner->GetPDA()->ID()));		//отправитель
	P.w_s16			((u16)pda_msg);
	P.w_s32			(-1);
	m_tpGameObject->u_EventSend(P);
	return			true;
}


bool CLuaGameObject::IsTalking()
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return false;
	return			pInventoryOwner->IsTalking();
}

void CLuaGameObject::StopTalk()
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return;
	pInventoryOwner->StopTalk();
}
void CLuaGameObject::EnableTalk()
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return;
	pInventoryOwner->EnableTalk();
}
void CLuaGameObject::DisableTalk()
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return;
	pInventoryOwner->DisableTalk();
}

bool CLuaGameObject::IsTalkEnabled()
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if(!pInventoryOwner) return false;
	return pInventoryOwner->IsTalkEnabled();
}


//передаче вещи из своего инвентаря в инвентарь партнера
void CLuaGameObject::TransferItem(CLuaGameObject* pItem, CLuaGameObject* pForWho)
{
	R_ASSERT(pItem);
	R_ASSERT(pForWho);

	CInventoryItem* pIItem = dynamic_cast<CInventoryItem*>(pItem->m_tpGameObject);
	VERIFY(pIItem);

	// выбросить у себя 
	NET_Packet						P;
	CGameObject::u_EventGen			(P,GE_OWNERSHIP_REJECT, m_tpGameObject->ID());
	P.w_u16							(pIItem->ID());
	CGameObject::u_EventSend		(P);

	// отдать партнеру
	CGameObject::u_EventGen			(P,GE_OWNERSHIP_TAKE, pForWho->m_tpGameObject->ID());
	P.w_u16							(pIItem->ID());
	CGameObject::u_EventSend		(P);
}


void CLuaGameObject::TransferMoney(int money, CLuaGameObject* pForWho)
{
	R_ASSERT(pForWho);
	CInventoryOwner* pOurOwner		= dynamic_cast<CInventoryOwner*>(m_tpGameObject); VERIFY(pOurOwner);
	CInventoryOwner* pOtherOwner	= dynamic_cast<CInventoryOwner*>(pForWho->m_tpGameObject); VERIFY(pOtherOwner);

	R_ASSERT3(pOurOwner->m_dwMoney-money>=0, "Character does not have enought money", pOurOwner->CharacterInfo().Name());
	
	pOurOwner->m_dwMoney	-= money;
	pOtherOwner->m_dwMoney	+= money;
}


void CLuaGameObject::SetGoodwill(int goodwill, CLuaGameObject* pWhoToSet)
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	pInventoryOwner->CharacterInfo().SetGoodwill(pWhoToSet->m_tpGameObject->ID(), goodwill);
}

void CLuaGameObject::SetRelation(ALife::ERelationType relation, CLuaGameObject* pWhoToSet)
{
	CInventoryOwner* pInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	VERIFY(pInventoryOwner);
	pInventoryOwner->CharacterInfo().SetRelationType(pWhoToSet->m_tpGameObject->ID(), relation);
}
void CLuaGameObject::SetStartDialog(LPCSTR dialog_id)
{
	CAI_PhraseDialogManager* pDialogManager = dynamic_cast<CAI_PhraseDialogManager*>(m_tpGameObject);
	if(!pDialogManager) return;
	pDialogManager->SetStartDialog(dialog_id);
}




CLuaGameObject *CLuaGameObject::GetCurrentWeapon() const
{
	CAI_Stalker		*l_tpStalker = dynamic_cast<CAI_Stalker*>(m_tpGameObject);
	if (!l_tpStalker) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member GetCurrentWeapon!");
		return		(0);
	}
	CGameObject		*current_weapon = l_tpStalker->GetCurrentWeapon();
	return			(current_weapon ? current_weapon->lua_game_object() : 0);
}

CLuaGameObject *CLuaGameObject::GetCurrentEquipment() const
{
	CAI_Stalker		*l_tpStalker = dynamic_cast<CAI_Stalker*>(m_tpGameObject);
	if (!l_tpStalker) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member GetCurrentWeapon!");
		return		(0);
	}
	CGameObject		*current_equipment = l_tpStalker->GetCurrentEquipment();
	return			(current_equipment ? current_equipment->lua_game_object() : 0);
}

CLuaGameObject *CLuaGameObject::GetFood() const
{
	CAI_Stalker		*l_tpStalker = dynamic_cast<CAI_Stalker*>(m_tpGameObject);
	if (!l_tpStalker) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member GetCurrentWeapon!");
		return		(0);
	}
	CGameObject		*food = l_tpStalker->GetFood();
	return			(food ? food->lua_game_object() : 0);
}

CLuaGameObject *CLuaGameObject::GetMedikit() const
{
	CAI_Stalker		*l_tpStalker = dynamic_cast<CAI_Stalker*>(m_tpGameObject);
	if (!l_tpStalker) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member GetCurrentWeapon!");
		return		(0);
	}
	CGameObject		*medikit = l_tpStalker->GetMedikit();
	return			(medikit ? medikit->lua_game_object() : 0);
}

void CScriptMovementAction::SetObjectToGo(CLuaGameObject *tpObjectToGo)
{
	if (tpObjectToGo)
		m_tpObjectToGo	= tpObjectToGo->operator CObject*();
	else
		m_tpObjectToGo	= 0;
	m_tGoalType			= eGoalTypeObject;
	m_bCompleted		= false;
}

void CScriptWatchAction::SetWatchObject(CLuaGameObject *tpObjectToWatch)
{
	m_tpObjectToWatch	= tpObjectToWatch->operator CObject*();
	m_tGoalType			= eGoalTypeObject;
	m_bCompleted		= false;
}

void CMonsterAction::SetObject(CLuaGameObject *tObj)
{
	m_tObject	= tObj->operator CObject*();
}

CScriptParticleAction::~CScriptParticleAction()
{
	//xr_delete			(m_tpParticleSystem);
}

void CScriptParticleAction::SetParticle(LPCSTR caParticleToRun, bool bAutoRemove)
{
	m_caParticleToRun	= caParticleToRun;
	m_tGoalType			= eGoalTypeParticleAttached;
	m_tpParticleSystem	= xr_new<CParticlesObject>(*m_caParticleToRun,m_bAutoRemove = bAutoRemove);
	m_bStartedToPlay	= false;
	m_bCompleted		= false;
}

void CScriptObjectAction::SetObject(CLuaGameObject *tpLuaGameObject)
{
	m_tpObject			= tpLuaGameObject->operator CObject*();
	m_bCompleted		= false;
}

#pragma todo("Dima to Dima : find out why user defined conversion operators work incorrect")

CLuaGameObject::operator CObject*()
{
	return			(m_tpGameObject);
}

u32	CLuaGameObject::Cost			() const
{
	CInventoryItem		*inventory_item = dynamic_cast<CInventoryItem*>(m_tpGameObject);
	if (!inventory_item) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CSciptMonster : cannot access class member Cost!");
		return			(false);
	}
	return				(inventory_item->Cost());
}

float CLuaGameObject::GetCondition	() const
{
	CInventoryItem		*inventory_item = dynamic_cast<CInventoryItem*>(m_tpGameObject);
	if (!inventory_item) {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CSciptMonster : cannot access class member Cost!");
		return			(false);
	}
	return				(inventory_item->GetCondition());
}

u32	CLuaGameObject::GetInventoryObjectCount() const
{
	CInventoryOwner		*l_tpInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if (l_tpInventoryOwner)
		return			(l_tpInventoryOwner->inventory().dwfGetObjectCount());
	else {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CLuaGameObject : cannot access class member obj_count!");
		return			(0);
	}
}

CLuaGameObject	*CLuaGameObject::GetActiveItem()
{
	CInventoryOwner		*l_tpInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if (l_tpInventoryOwner)
		if (l_tpInventoryOwner->inventory().ActiveItem())
			return		(l_tpInventoryOwner->inventory().ActiveItem()->lua_game_object());
		else
			return		(0);
	else {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CLuaGameObject : cannot access class member activge_item!");
		return			(0);
	}
}

CLuaGameObject	*CLuaGameObject::GetObjectByName	(LPCSTR caObjectName) const
{
	CInventoryOwner		*l_tpInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if (l_tpInventoryOwner) {
		CInventoryItem	*l_tpInventoryItem = l_tpInventoryOwner->inventory().GetItemFromInventory(caObjectName);
		CGameObject		*l_tpGameObject = dynamic_cast<CGameObject*>(l_tpInventoryItem);
		if (!l_tpGameObject)
			return		(0);
		else
			return		(l_tpGameObject->lua_game_object());
	}
	else {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CLuaGameObject : cannot access class member object!");
		return			(0);
	}
}

CLuaGameObject	*CLuaGameObject::GetObjectByIndex	(int iIndex) const
{
	CInventoryOwner		*l_tpInventoryOwner = dynamic_cast<CInventoryOwner*>(m_tpGameObject);
	if (l_tpInventoryOwner) {
		CInventoryItem	*l_tpInventoryItem = l_tpInventoryOwner->inventory().tpfGetObjectByIndex(iIndex);
		CGameObject		*l_tpGameObject = dynamic_cast<CGameObject*>(l_tpInventoryItem);
		if (!l_tpGameObject)
			return		(0);
		else
			return		(l_tpGameObject->lua_game_object());
	}
	else {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CLuaGameObject : cannot access class member object!");
		return			(0);	
	}
}

CLuaGameObject *CLuaGameObject::GetEnemy() const
{
	CCustomMonster		*l_tpCustomMonster = dynamic_cast<CCustomMonster*>(m_tpGameObject);
	if (l_tpCustomMonster) {
		if (l_tpCustomMonster->GetCurrentEnemy()) return (l_tpCustomMonster->GetCurrentEnemy()->lua_game_object());
		else return (0);
	} else {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CLuaGameObject : cannot access class member GetEnemy!");
		return			(0);
	}
}

CLuaGameObject *CLuaGameObject::GetCorpse() const
{
	CCustomMonster		*l_tpCustomMonster = dynamic_cast<CCustomMonster*>(m_tpGameObject);
	if (l_tpCustomMonster)
		if (l_tpCustomMonster->GetCurrentCorpse()) return (l_tpCustomMonster->GetCurrentCorpse()->lua_game_object());
		else return (0);
	else {
		ai().script_engine().script_log			(ScriptStorage::eLuaMessageTypeError,"CLuaGameObject : cannot access class member GetCorpse!");
		return			(0);
	}
}

CScriptSoundInfo CLuaGameObject::GetSoundInfo()
{
	CScriptSoundInfo	ret_val;
	
	CAI_Biting *l_tpMonster = dynamic_cast<CAI_Biting *>(m_tpGameObject);
	if (l_tpMonster) {
		if (l_tpMonster->SoundMemory.IsRememberSound()) {
			SoundElem se; 
			bool bDangerous;
			l_tpMonster->SoundMemory.GetSound(se, bDangerous);
			
			const CGameObject *pO = dynamic_cast<const CGameObject *>(se.who);
			ret_val.set((pO) ?  pO->lua_game_object() : 0, bDangerous, se.position, se.power, int(se.time));
		}
	} else {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError,"CLuaGameObject : cannot access class member GetSoundInfo!");
	}
	return			(ret_val);
}

CScriptMonsterHitInfo CLuaGameObject::GetMonsterHitInfo()
{
	CScriptMonsterHitInfo	ret_val;

	CAI_Biting *l_tpMonster = dynamic_cast<CAI_Biting *>(m_tpGameObject);
	if (l_tpMonster) {
		if (l_tpMonster->HitMemory.is_hit()) {
			CGameObject *pO = dynamic_cast<CGameObject *>(l_tpMonster->HitMemory.get_last_hit_object());
			ret_val.set((pO) ?  pO->lua_game_object() : 0, l_tpMonster->HitMemory.get_last_hit_dir(), l_tpMonster->HitMemory.get_last_hit_time());
		}
	} else {
		ai().script_engine().script_log(ScriptStorage::eLuaMessageTypeError,"CLuaGameObject : cannot access class member GetMonsterHitInfo!");
	}
	return			(ret_val);
}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void  CLuaGameObject::set_body_state			(EBodyState body_state)
{
	CAI_Stalker					*stalker = dynamic_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member movement!");
	else
		stalker->set_body_state	(body_state);
}

void  CLuaGameObject::set_movement_type		(EMovementType movement_type)
{
	CAI_Stalker					*stalker = dynamic_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member movement!");
	else
		stalker->set_movement_type	(movement_type);
}

void  CLuaGameObject::set_mental_state		(EMentalState mental_state)
{
	CAI_Stalker					*stalker = dynamic_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member movement!");
	else
		stalker->set_mental_state	(mental_state);
}

void  CLuaGameObject::set_path_type			(CMovementManager::EPathType path_type)
{
	CAI_Stalker					*stalker = dynamic_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member movement!");
	else
		stalker->set_path_type	(path_type);
}

void  CLuaGameObject::set_detail_path_type	(CMovementManager::EDetailPathType detail_path_type)
{
	CAI_Stalker					*stalker = dynamic_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member movement!");
	else
		stalker->set_detail_path_type	(detail_path_type);
}

void  CLuaGameObject::add_sound		(LPCSTR prefix, u32 max_count, ESoundTypes type, u32 priority, u32 mask, u32 internal_type, LPCSTR bone_name)
{
	CSoundPlayer				*sound_player = dynamic_cast<CSoundPlayer*>(m_tpGameObject);
	if (!sound_player)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member add!");
	else
		sound_player->add		(prefix,max_count,type,priority,mask,internal_type,bone_name);
}

void  CLuaGameObject::add_sound		(LPCSTR prefix, u32 max_count, ESoundTypes type, u32 priority, u32 mask, u32 internal_type, LPCSTR bone_name, LPCSTR head_anim)
{
	CSoundPlayer				*sound_player = dynamic_cast<CSoundPlayer*>(m_tpGameObject);
	if (!sound_player)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member add!");
	else
		sound_player->add		(prefix,max_count,type,priority,mask,internal_type, bone_name, head_anim);
}

void  CLuaGameObject::add_sound		(LPCSTR prefix, u32 max_count, ESoundTypes type, u32 priority, u32 mask, u32 internal_type)
{
	add_sound					(prefix,max_count,type,priority,mask,internal_type,"bip01_head");
}

void CLuaGameObject::remove_sound	(u32 internal_type)
{
	CSoundPlayer				*sound_player = dynamic_cast<CSoundPlayer*>(m_tpGameObject);
	if (!sound_player)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member add!");
	else
		sound_player->remove	(internal_type);
}

void CLuaGameObject::set_sound_mask	(u32 sound_mask)
{
	CSoundPlayer				*sound_player = dynamic_cast<CSoundPlayer*>(m_tpGameObject);
	if (!sound_player)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member set_sound_mask!");
	else
		sound_player->set_sound_mask(sound_mask);
}

void CLuaGameObject::set_sight		(SightManager::ESightType tLookType, const Fvector	*tPointToLook, u32 dwLookOverDelay)
{
	CSightManager				*sight_manager = dynamic_cast<CSightManager*>(m_tpGameObject);
	if (!sight_manager)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSightManager : cannot access class member set_sight!");
	else
		sight_manager->setup	(tLookType,tPointToLook,dwLookOverDelay);
}

u32 CLuaGameObject::GetRank		()
{
	CAI_Stalker					*stalker = dynamic_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker) {
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member GetRank!");
		return					(ALife::eStalkerRankNone);
	}
	else
		return					(stalker->GetRank());
}

void CLuaGameObject::play_sound		(u32 internal_type)
{
	CSoundPlayer				*sound_player = dynamic_cast<CSoundPlayer*>(m_tpGameObject);
	if (!sound_player)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member play!");
	else
		sound_player->play		(internal_type);
}

void CLuaGameObject::play_sound		(u32 internal_type, u32 max_start_time)
{
	CSoundPlayer				*sound_player = dynamic_cast<CSoundPlayer*>(m_tpGameObject);
	if (!sound_player)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member play!");
	else
		sound_player->play		(internal_type,max_start_time);
}

void CLuaGameObject::play_sound		(u32 internal_type, u32 max_start_time, u32 min_start_time)
{
	CSoundPlayer				*sound_player = dynamic_cast<CSoundPlayer*>(m_tpGameObject);
	if (!sound_player)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member play!");
	else
		sound_player->play		(internal_type,max_start_time,min_start_time);
}

void CLuaGameObject::play_sound		(u32 internal_type, u32 max_start_time, u32 min_start_time, u32 max_stop_time)
{
	CSoundPlayer				*sound_player = dynamic_cast<CSoundPlayer*>(m_tpGameObject);
	if (!sound_player)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member play!");
	else
		sound_player->play		(internal_type,max_start_time,min_start_time,max_stop_time);
}

void CLuaGameObject::play_sound		(u32 internal_type, u32 max_start_time, u32 min_start_time, u32 max_stop_time, u32 min_stop_time)
{
	CSoundPlayer				*sound_player = dynamic_cast<CSoundPlayer*>(m_tpGameObject);
	if (!sound_player)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member play!");
	else
		sound_player->play		(internal_type,max_start_time,min_start_time,max_stop_time,min_stop_time);
}

void CLuaGameObject::play_sound		(u32 internal_type, u32 max_start_time, u32 min_start_time, u32 max_stop_time, u32 min_stop_time, u32 id)
{
	CSoundPlayer				*sound_player = dynamic_cast<CSoundPlayer*>(m_tpGameObject);
	if (!sound_player)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CSoundPlayer : cannot access class member play!");
	else
		sound_player->play		(internal_type,max_start_time,min_start_time,max_stop_time,min_stop_time,id);
}

void CLuaGameObject::set_item		(MonsterSpace::EObjectAction object_action)
{
	CObjectHandler			*object_handler = dynamic_cast<CObjectHandler*>(m_tpGameObject);
	if (!object_handler)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CObjectHandler : cannot access class member set_item!");
	else
		object_handler->set_goal(object_action);
}

void CLuaGameObject::set_item		(MonsterSpace::EObjectAction object_action, CLuaGameObject *lua_game_object)
{
	CObjectHandler			*object_handler = dynamic_cast<CObjectHandler*>(m_tpGameObject);
	if (!object_handler)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CObjectHandler : cannot access class member set_item!");
	else
		object_handler->set_goal(object_action,lua_game_object ? lua_game_object->object() : 0);
}

void CLuaGameObject::set_desired_position	()
{
	CAI_Stalker							*stalker = dynamic_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker)
		ai().script_engine().script_log							(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member movement!");
	else
		stalker->set_desired_position	(0);
}

void CLuaGameObject::set_desired_position	(const Fvector *desired_position)
{
	CAI_Stalker							*stalker = dynamic_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker)
		ai().script_engine().script_log							(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member movement!");
	else
		stalker->set_desired_position	(desired_position);
}

void  CLuaGameObject::set_desired_direction	()
{
	CAI_Stalker					*stalker = dynamic_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member movement!");
	else
		stalker->set_desired_direction	(0);
}

void  CLuaGameObject::set_desired_direction	(const Fvector *desired_direction)
{
	CAI_Stalker					*stalker = dynamic_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member movement!");
	else
		stalker->set_desired_direction	(desired_direction);
}

void  CLuaGameObject::set_node_evaluator		(CAbstractVertexEvaluator *node_evaluator)
{
	CAI_Stalker					*stalker = dynamic_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member movement!");
	else
		stalker->set_node_evaluator	(node_evaluator);
}

void  CLuaGameObject::set_node_evaluator		()
{
	CAI_Stalker					*stalker = dynamic_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member movement!");
	else
		stalker->set_node_evaluator	(0);
}

void  CLuaGameObject::set_path_evaluator		()
{
	CAI_Stalker					*stalker = dynamic_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member movement!");
	else
		stalker->set_path_evaluator	(0);
}

void CLuaGameObject::set_path_evaluator		(CAbstractVertexEvaluator *path_evaluator)
{
	CAI_Stalker					*stalker = dynamic_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member movement!");
	else
		stalker->set_path_evaluator	(path_evaluator);
}

void CLuaGameObject::set_patrol_path		(LPCSTR path_name, const CMovementManager::EPatrolStartType patrol_start_type, const CMovementManager::EPatrolRouteType patrol_route_type, bool random)
{
	CAI_Stalker					*stalker = dynamic_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member movement!");
	else
		stalker->set_path		(path_name,patrol_start_type,patrol_route_type,random);
}

void CLuaGameObject::set_dest_level_vertex_id(u32 level_vertex_id)
{
	CAI_Stalker					*stalker = dynamic_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker)
		ai().script_engine().script_log					(ScriptStorage::eLuaMessageTypeError,"CAI_Stalker : cannot access class member set_dest_level_vertex_id!");
	else
		stalker->set_level_dest_vertex	(level_vertex_id);
}

u32	CLuaGameObject::level_vertex_id		() const
{
	return						(m_tpGameObject->level_vertex_id());
}

LPCSTR CLuaGameObject::GetPatrolPathName()
{
	CAI_Stalker			*stalker = dynamic_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker) {
		CScriptMonster	*script_monster = dynamic_cast<CScriptMonster*>(m_tpGameObject);
		if (!script_monster) {
			ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member GetPatrolPathName!");
			return		("");
		}
		else
			return		(script_monster->GetPatrolPathName());
	}
	else
		return			(*stalker->path_name());
}

void CLuaGameObject::add_animation			(LPCSTR animation, bool hand_usage)
{
	CAI_Stalker			*stalker = dynamic_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member add_animation!");
		return;
	}
	stalker->add_animation(animation,hand_usage);
}

void CLuaGameObject::clear_animations		()
{
	CAI_Stalker			*stalker = dynamic_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member clear_animations!");
		return;
	}
	stalker->clear_animations();
}

int	CLuaGameObject::animation_count		() const
{
	CAI_Stalker			*stalker = dynamic_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member clear_animations!");
		return			(-1);
	}
	return				((int)stalker->m_script_animations.size());
}

CScriptBinderObject	*CLuaGameObject::binded_object	()
{
	CScriptBinder	*binder = dynamic_cast<CScriptBinder*>(m_tpGameObject);
	if (!binder) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member binded_object!");
		return		(0);
	}
	return			(binder->object());
}

void CLuaGameObject::set_previous_point	(int point_index)
{
	CMovementManager	*movement_manager = dynamic_cast<CMovementManager*>(m_tpGameObject);
	if (!movement_manager)
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member set_previous_point!");
	else
		movement_manager->set_previous_point(point_index);
}

void CLuaGameObject::set_start_point	(int point_index)
{
	CMovementManager	*movement_manager = dynamic_cast<CMovementManager*>(m_tpGameObject);
	if (!movement_manager)
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member set_start_point!");
	else
		movement_manager->set_start_point(point_index);
}

void CLuaGameObject::enable_memory_object	(CLuaGameObject *object, bool enable)
{
	CMemoryManager	*memory_manager = dynamic_cast<CMemoryManager*>(m_tpGameObject);
	if (!memory_manager)
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member enable_memory_object!");
	else
		memory_manager->enable				(object->m_tpGameObject,enable);
}

int  CLuaGameObject::active_sound_count		()
{
	CSoundPlayer	*sound_player = dynamic_cast<CSoundPlayer*>(m_tpGameObject);
	if (!sound_player) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member active_sound_count!");
		return								(-1);
	}
	else
		return								(sound_player->active_sound_count());
}

const CCoverPoint *CLuaGameObject::best_cover	(const Fvector &position, const Fvector &enemy_position, float radius, float min_enemy_distance, float max_enemy_distance)
{
	CAI_Stalker		*stalker = dynamic_cast<CAI_Stalker*>(m_tpGameObject);
	if (!stalker) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member best_cover!");
		return		(0);
	}
	stalker->m_ce_best->setup(enemy_position,min_enemy_distance,max_enemy_distance,0.f);
	CCoverPoint		*point = ai().cover_manager().best_cover(position,radius,*stalker->m_ce_best);
	return			(point);
}

CScriptIniFile *CLuaGameObject::spawn_ini			() const
{
	return			((CScriptIniFile*)m_tpGameObject->spawn_ini());
}

const xr_vector<CVisibleObject>	&CLuaGameObject::memory_visible_objects	() const
{
	CCustomMonster	*monster = dynamic_cast<CCustomMonster*>(m_tpGameObject);
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member memory_visible_objects!");
		NODEFAULT;
	}
	return			(monster->memory_visible_objects());
}

const xr_vector<CSoundObject>	&CLuaGameObject::memory_sound_objects	() const
{
	CCustomMonster	*monster = dynamic_cast<CCustomMonster*>(m_tpGameObject);
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member memory_sound_objects!");
		NODEFAULT;
	}
	return			(monster->sound_objects());
}

const xr_vector<CHitObject>		&CLuaGameObject::memory_hit_objects		() const
{
	CCustomMonster	*monster = dynamic_cast<CCustomMonster*>(m_tpGameObject);
	if (!monster) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member memory_hit_objects!");
		NODEFAULT;
	}
	return			(monster->hit_objects());
}

const xr_vector<CNotYetVisibleObject> &CLuaGameObject::not_yet_visible_objects() const
{
	CMemoryManager	*manager = dynamic_cast<CMemoryManager*>(m_tpGameObject);
	if (!manager) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member not_yet_visible_objects!");
		NODEFAULT;
	}
	return					(manager->not_yet_visible_objects());
}

float CLuaGameObject::visibility_threshold	() const
{
	CMemoryManager	*manager = dynamic_cast<CMemoryManager*>(m_tpGameObject);
	if (!manager) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member visibility_threshold!");
		NODEFAULT;
	}
	return					(manager->visibility_threshold());
}

void CLuaGameObject::air_attack (CLuaGameObject * object)
{
	CHelicopter		*helicopter = dynamic_cast<CHelicopter*>(m_tpGameObject);
	if (!helicopter) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member do_hunt!");
		NODEFAULT;
	}
	helicopter->doHunt(object->m_tpGameObject);
}

void CLuaGameObject::air_attack_wait (CLuaGameObject* object, float dist, float t)
{
	CHelicopter		*helicopter = dynamic_cast<CHelicopter*>(m_tpGameObject);
	if (!helicopter) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member air_attack_wait!");
		NODEFAULT;
	}
	helicopter->doHunt2(object->m_tpGameObject,dist, t);
}

bool CLuaGameObject::air_attack_active ()
{
	CHelicopter		*helicopter = dynamic_cast<CHelicopter*>(m_tpGameObject);
	if (!helicopter) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member air_attack_active!");
		NODEFAULT;
		return true;
	}
	return !!helicopter->isOnAttack();
}

void CLuaGameObject::heli_goto_stay_point (float time)
{
	CHelicopter		*helicopter = dynamic_cast<CHelicopter*>(m_tpGameObject);
	if (!helicopter) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member heli_goto_stay_point!");
		NODEFAULT;
	}
	helicopter->gotoStayPoint(time);
}

void CLuaGameObject::heli_goto_stay_point (Fvector& pos, float time)
{
	CHelicopter		*helicopter = dynamic_cast<CHelicopter*>(m_tpGameObject);
	if (!helicopter) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member heli_goto_stay_point!");
		NODEFAULT;
	}
	helicopter->gotoStayPoint(time, &pos);
}

void CLuaGameObject::heli_go_patrol(float time)
{
	CHelicopter		*helicopter = dynamic_cast<CHelicopter*>(m_tpGameObject);
	if (!helicopter) {
		ai().script_engine().script_log		(ScriptStorage::eLuaMessageTypeError,"CGameObject : cannot access class member heli_go_patrol!");
		NODEFAULT;
	}
	helicopter->goPatrol(time);
}
/*
void				heli_use_rocket			(bool b);
bool				heli_is_use_rocket		()const;
void				heli_use_mgun			(bool b);
bool				heli_use_mgun			()const;*/


Fvector	CLuaGameObject::bone_position	(LPCSTR bone_name) const
{
	u16					bone_id;
	if (xr_strlen(bone_name))
		bone_id			= PKinematics(m_tpGameObject->Visual())->LL_BoneID(bone_name);
	else
		bone_id			= PKinematics(m_tpGameObject->Visual())->LL_GetBoneRoot();

	Fmatrix				matrix;
	matrix.mul_43		(m_tpGameObject->XFORM(),PKinematics(m_tpGameObject->Visual())->LL_GetBoneInstance(bone_id).mTransform);
	return				(matrix.c);
}
