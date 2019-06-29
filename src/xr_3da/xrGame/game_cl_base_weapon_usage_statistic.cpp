#include "stdafx.h"
#include "game_cl_base.h"
#include "Level.h"
#include "Weapon.h"
#include "alife_space.h"
#include "hit.h"
#include "Actor.h"
#include "xrServer.h"
#include "hudmanager.h"
#include "game_cl_base_weapon_usage_statistic.h"
//#include "GameFont.h"

BulletData::BulletData(shared_str FName, shared_str WName, SBullet* pBullet) 
{
	FirerName = FName;
	WeaponName = WName;
	Bullet = *pBullet;
	HitRefCount = 0;
	HitResponds = 0;
	Removed = false;
};

void			HitData::net_save			(NET_Packet* P)
{
	P->w_vec3(Pos0);
	P->w_vec3(Pos1);
	P->w_stringZ(TargetName);
	P->w_stringZ(BoneName);
	P->w_s16(BoneID);
	P->w_u8(Deadly ? 1 : 0);
};
void			HitData::net_load			(NET_Packet* P)
{
	P->r_vec3(Pos0);
	P->r_vec3(Pos1);
	P->r_stringZ(TargetName);
	P->r_stringZ(BoneName);
	P->r_s16(BoneID);
	Deadly = !!P->r_u8();
	Completed = true;
};

Weapon_Statistic::Weapon_Statistic(LPCSTR Name)
{
	WName = Name;
	InvName = NULL;
	NumBought = 0;

	m_dwRoundsFired = m_dwRoundsFired_d = 0;
	m_dwBulletsFired = m_dwBulletsFired_d = 0;
	m_dwHitsScored = m_dwHitsScored_d = 0;
	m_dwKillsScored = m_dwKillsScored_d = 0;

	m_Hits.clear();
	m_dwNumCompleted = 0;
	ZeroMemory(m_Basket, sizeof(m_Basket));
};
Weapon_Statistic::~Weapon_Statistic()
{
	m_Hits.clear_and_free();
	ZeroMemory(m_Basket, sizeof(m_Basket));
};

void			Weapon_Statistic::net_save			(NET_Packet* P)
{
	m_dwRoundsFired_d = m_dwRoundsFired - m_dwRoundsFired_d;
	P->w_u32(m_dwRoundsFired_d);	m_dwRoundsFired_d = m_dwRoundsFired;
	P->w_u32(m_dwBulletsFired_d);	m_dwBulletsFired_d = 0;
	P->w_u32(m_dwHitsScored_d);		m_dwHitsScored_d = 0;
	P->w_u32(m_dwKillsScored_d);	m_dwKillsScored_d = 0;

	P->w_u32(m_dwNumCompleted);
	u32 i = 0;
	while (i<m_Hits.size())	
	{
		HitData& Hit = m_Hits[i];
		if (Hit.Completed)
		{
			Hit.net_save(P);
			//-------------
			Hit = m_Hits.back();
			m_Hits.pop_back();
			//-------------
			m_dwNumCompleted--;
		}
		else i++;
	}
};
void			Weapon_Statistic::net_load			(NET_Packet* P)
{
	m_dwRoundsFired += P->r_u32();
	m_dwBulletsFired += P->r_u32();
	m_dwHitsScored += P->r_u32();
	m_dwKillsScored += P->r_u32();
	u32 HitsSize = P->r_u32();
	for (u32 i = 0; i<HitsSize; i++)
	{
		HitData NewHit;
		NewHit.net_load(P);
		m_Hits.push_back(NewHit);
//		m_dwNumCompleted++;
	}
};

Player_Statistic::Player_Statistic(LPCSTR Name)
{
	PName = Name;
	m_dwTotalShots = 0;
	m_dwTotalShots_d = 0;

	ZeroMemory(m_dwTotalAliveTime, sizeof(m_dwTotalAliveTime));
	ZeroMemory(m_dwTotalMoneyRound, sizeof(m_dwTotalMoneyRound));
	ZeroMemory(m_dwNumRespawned, sizeof(m_dwNumRespawned));
};

Player_Statistic::~Player_Statistic()
{
	aWeaponStats.clear_and_free();
};

void			Player_Statistic::net_save			(NET_Packet* P)
{
	P->w_u32(m_dwTotalShots_d); m_dwTotalShots_d = 0;
	P->w_u32(aWeaponStats.size());
	for (u32 i=0; i<aWeaponStats.size(); i++)
	{
		Weapon_Statistic& WS = aWeaponStats[i];
		P->w_stringZ(WS.WName);
		WS.net_save(P);
	}
};

void			Player_Statistic::net_load			(NET_Packet* P)
{
	m_dwTotalShots += P->r_u32();
	u32 NumWeapons = P->r_u32();
	for (u32 i=0; i<NumWeapons; i++)
	{
		shared_str WName;
		P->r_stringZ(WName);
		Weapon_Statistic& WS = *(FindPlayersWeapon(*WName));
		WS.net_load(P);
	}
};

WeaponUsageStatistic::WeaponUsageStatistic()
{
	Clear();
	m_dwUpdateTimeDelta = 60000;
	m_dwSaveTimeDelta = 300000;
	m_bCollectStatistic = false;
};

void	WeaponUsageStatistic::Clear			()
{
	ActiveBullets.clear();	
	aPlayersStatistic.clear();
	m_Requests.clear();
	m_dwLastRequestSenderID = 0;
	
	ZeroMemory(m_dwTotalPlayersAliveTime, sizeof(m_dwTotalPlayersAliveTime));
	ZeroMemory(m_dwTotalPlayersMoneyRound, sizeof(m_dwTotalPlayersMoneyRound));
	ZeroMemory(m_dwTotalNumRespawns, sizeof(m_dwTotalNumRespawns));

	m_dwLastUpdateTime = Level().timeServer();
	m_dwLastSaveTime = Level().timeServer();
	mFileName[0] = 0;
	m_bFirstSave = true;
};

WeaponUsageStatistic::~WeaponUsageStatistic()
{
	ActiveBullets.clear_and_free();
	aPlayersStatistic.clear_and_free();
	m_Requests.clear_and_free();
	m_dwLastRequestSenderID = 0;
};

bool					WeaponUsageStatistic::GetPlayer			(LPCSTR PlayerName, PLAYERS_STATS_it& pPlayerI)
{
	pPlayerI	= std::find(aPlayersStatistic.begin(), aPlayersStatistic.end(), PlayerName);
	if (pPlayerI == aPlayersStatistic.end() || !((*pPlayerI) == PlayerName))
		return false;
	return true;
}
PLAYERS_STATS_it					WeaponUsageStatistic::FindPlayer			(LPCSTR PlayerName)
{
	/*
	PLAYERS_STATS_it pPlayerI	= std::find(aPlayersStatistic.begin(), aPlayersStatistic.end(), PlayerName);
	if (pPlayerI == aPlayersStatistic.end() || !((*pPlayerI) == PlayerName))
	{
		aPlayersStatistic.push_back(Player_Statistic(PlayerName));
		pPlayerI = aPlayersStatistic.end()-1;
	};
	return pPlayerI;
	*/
	PLAYERS_STATS_it pPlayerI;
	if (!GetPlayer(PlayerName, pPlayerI))
	{
		aPlayersStatistic.push_back(Player_Statistic(PlayerName));
		pPlayerI = aPlayersStatistic.end()-1;
	};
	return pPlayerI;
};

WEAPON_STATS_it	Player_Statistic::FindPlayersWeapon	(LPCSTR WeaponName)
{
	WEAPON_STATS_it pWeaponI = std::find(aWeaponStats.begin(), aWeaponStats.end(), WeaponName);
	if (pWeaponI == aWeaponStats.end() || !((*pWeaponI) == WeaponName))
	{
		aWeaponStats.push_back(Weapon_Statistic(WeaponName));		
		pWeaponI = aWeaponStats.end()-1;
		pWeaponI->InvName = pSettings->r_string_wb(WeaponName, "inv_name");
	}
	return pWeaponI;
};

bool		WeaponUsageStatistic::FindBullet			(u32 BulletID, ABULLETS_it& Bullet_It)
{
	Bullet_It = std::find(ActiveBullets.begin(), ActiveBullets.end(),BulletID);
	if (Bullet_It == ActiveBullets.end() || (*Bullet_It) != BulletID) return false;
	return true;
}

bool			Weapon_Statistic::FindHit				(u32 BulletID, HITS_VEC_it& Hit_it)
{
	Hit_it = std::find(m_Hits.begin(), m_Hits.end(),BulletID);
	if (Hit_it == m_Hits.end() || (*Hit_it) != BulletID) return false;
	return true;
};

void		WeaponUsageStatistic::RemoveBullet		(ABULLETS_it& Bullet_it)
{
	if (!Bullet_it->Removed || Bullet_it->HitRefCount!=Bullet_it->HitResponds) return;
	//-------------------------------------------------------------
	PLAYERS_STATS_it PlayerIt = FindPlayer(*(Bullet_it->FirerName));
	WEAPON_STATS_it WeaponIt = PlayerIt->FindPlayersWeapon(*(Bullet_it->WeaponName));
	HITS_VEC_it HitIt;
	if (WeaponIt->FindHit(Bullet_it->Bullet.m_dwID, HitIt))
	{
		HitIt->Completed = true;
		WeaponIt->m_dwNumCompleted++;
	};	
	//-------------------------------------------------------------
	*Bullet_it = ActiveBullets.back();
	ActiveBullets.pop_back();
}

void				WeaponUsageStatistic::OnWeaponBought		(game_PlayerState* ps, LPCSTR WeaponName)
{
	if (!CollectData()) return;
	if (!ps) return;
	PLAYERS_STATS_it PlayerIt = FindPlayer(ps->getName());
	WEAPON_STATS_it WeaponIt = PlayerIt->FindPlayersWeapon(WeaponName);
	WeaponIt->NumBought++;
	//-----------------------------------------------
	int BasketPos = 0;
	if (ps->money_for_round > 500)
	{
		BasketPos = (ps->money_for_round-1)/1000 + 1;
	};
	WeaponIt->m_Basket[ps->team][BasketPos]++;
};

void				WeaponUsageStatistic::OnBullet_Fire			(SBullet* pBullet, const CCartridge& cartridge)
{
	if (!CollectData()) return;

	if (!pBullet || !pBullet->flags.allow_sendhit) return;
	CObject					*object_weapon = Level().Objects.net_Find(pBullet->weapon_id);
	if (!object_weapon) return;
	CObject					*object_parent = Level().Objects.net_Find(pBullet->parent_id);
	if (!object_parent) return;
	CActor* pActor = smart_cast<CActor*>(object_parent);
	if (!pActor) return;
	//-----------------------------------------------------------------------------------
	PLAYERS_STATS_it PlayerIt = FindPlayer(*object_parent->cName());
	pBullet->m_dwID = PlayerIt->m_dwTotalShots++;
	PlayerIt->m_dwTotalShots_d++;
	WEAPON_STATS_it WeaponIt = PlayerIt->FindPlayersWeapon(*object_weapon->cNameSect());
	WeaponIt->m_dwRoundsFired = (++WeaponIt->m_dwBulletsFired)/cartridge.m_buckShot;
	WeaponIt->m_dwBulletsFired_d++;
	//-----------------------------------------------------------------------------------
	ActiveBullets.push_back(BulletData(object_parent->cName(), object_weapon->cNameSect(), pBullet));
	
//	Msg("! OnBullet Fire ID[%d]", pBullet->m_dwID);
}

/*
static u32 HitChecksSended = 0;
static u32 HitChecksReceived = 0;
static u32 HitChecksResponded = 0;
static u32 HitChecksRespondedFalse = 0;
static u32 HitChecksRespondedTrue = 0;
static u32 HitChecksRespondReceived = 0;
static u32 HitChecksRespondReceivedFalse = 0;
static u32 HitChecksRespondReceivedTrue = 0;
*/
void				WeaponUsageStatistic::OnBullet_Hit			(SBullet* pBullet, u16 TargetID, s16 element, Fvector HitLocation)
{
	if (!pBullet || !pBullet->flags.allow_sendhit) return;
//	Msg("! OnBullet Hit ID[%d]", pBullet->m_dwID);
	ABULLETS_it BulletIt;
	if (!FindBullet(pBullet->m_dwID, BulletIt)) return;
	//-----------------------------------------------------
	PLAYERS_STATS_it PlayerIt = FindPlayer(*(BulletIt->FirerName));
	WEAPON_STATS_it WeaponIt = PlayerIt->FindPlayersWeapon(*(BulletIt->WeaponName));	
	if (!BulletIt->HitRefCount++) 
	{
		WeaponIt->m_dwHitsScored++;
		WeaponIt->m_dwHitsScored_d++;
		//---------------------------
		CObject					*pTarget = Level().Objects.net_Find(TargetID);
		if (!pTarget) return;
		CActor* pActor = smart_cast<CActor*>(pTarget);
		if (!pActor) return;
		//---------------------------
		BulletData& BD = *BulletIt;
		HitData NewHit;
		//---------------------------		
		NewHit.Completed = false;
		NewHit.Deadly = false;
		NewHit.BoneID = element;
		NewHit.TargetID = TargetID;
		NewHit.BulletID = BD.Bullet.m_dwID;
		NewHit.Pos0 = BD.Bullet.pos;
		NewHit.Pos1 = HitLocation;
		NewHit.TargetName = pTarget->cName();
		NewHit.BoneName = smart_cast<CKinematics*>(pTarget->Visual())->LL_BoneName_dbg(element);
		//---------------------------
		WeaponIt->m_Hits.push_back(NewHit);
	};
	
//	HitChecksSended++;
}
void				WeaponUsageStatistic::OnBullet_Remove		(SBullet* pBullet)
{
	if (!pBullet || !pBullet->flags.allow_sendhit)	return;
	ABULLETS_it		BulletIt;
	if (!FindBullet(pBullet->m_dwID, BulletIt))		return;
//	Msg("! Bullet Removed ID[%d]", BulletIt->Bullet.m_dwID);
	BulletIt->Removed = true;
	RemoveBullet	(BulletIt);
}

void				WeaponUsageStatistic::OnBullet_Check_Request	(SHit* pHDS)
{
	if (!pHDS || OnClient()) return;
	s16 BoneID = pHDS->bone();
	u32 BulletID = pHDS->BulletID;
	u32 SenderID = pHDS->SenderID;

	BChA_it pSenderI	= std::find(m_Requests.begin(), m_Requests.end(), SenderID);
	if (pSenderI == m_Requests.end() || (*pSenderI) != SenderID)
	{
		m_Requests.push_back(Bullet_Check_Array(SenderID));
		pSenderI = m_Requests.end()-1;
	};
	
	(*pSenderI).Requests.push_back(Bullet_Check_Request(BulletID, BoneID));
	m_dwLastRequestSenderID = SenderID;

//	HitChecksReceived++;
};

void				WeaponUsageStatistic::OnBullet_Check_Result		(bool Result)
{
	if (OnClient()) return;
	if (m_dwLastRequestSenderID)
	{
		BChA_it pSenderI	= std::find(m_Requests.begin(), m_Requests.end(), m_dwLastRequestSenderID);
		if (pSenderI != m_Requests.end() && (*pSenderI) == m_dwLastRequestSenderID)
		{
			(*pSenderI).Requests.back().Result = Result;
			(*pSenderI).Requests.back().Processed = true;
			if (Result) (*pSenderI).NumTrue++;
			else (*pSenderI).NumFalse++;			
		}
		else
		{
			Msg ("! Warning can't Find Check!");
			R_ASSERT(0);
		}
		m_dwLastRequestSenderID = 0;
	}
};

void				WeaponUsageStatistic::Send_Check_Respond			()
{
	if (!OnServer()) return;
	NET_Packet P;
	string1024 STrue, SFalse;
	for (u32 i=0; i<m_Requests.size(); i++)
	{
		Bullet_Check_Array& BChA_Request = m_Requests[i];
		if (BChA_Request.Requests.empty()) continue;
		Bullet_Check_Respond_True* pSTrue = (Bullet_Check_Respond_True*) STrue;
		u32* pSFalse = (u32*) SFalse;
		//-----------------------------------------------------
		u32 NumFalse = 0;
		u32 NumTrue = 0;
		u32 j=0;
		while (j<BChA_Request.Requests.size())
		{
			Bullet_Check_Request& curBChR = BChA_Request.Requests[j];
			if (!curBChR.Processed)
			{
				j++;
				continue;
			}
			else
			{
				if (curBChR.Result) 
				{
					pSTrue->BulletID = curBChR.BulletID;
					pSTrue->BoneID = curBChR.BoneID;
					pSTrue++;				
//					HitChecksRespondedTrue++;
					NumTrue++;
				}
				else 
				{
					*(pSFalse++) = curBChR.BulletID;
//					HitChecksRespondedFalse++;
					NumFalse++;
				};
//				HitChecksResponded++;
				//-----------------------------------------------------
				*(BChA_Request.Requests.begin()+j) = BChA_Request.Requests.back();
				BChA_Request.Requests.pop_back();
			}
		}
		//-----------------------------------------------------
		P.w_begin(M_BULLET_CHECK_RESPOND);		
//		Msg("%d-%d || %d-%d", NumFalse, BChA_Request.NumFalse, NumTrue, BChA_Request.NumTrue);
		P.w_u8(BChA_Request.NumFalse);		BChA_Request.NumFalse = 0;
		P.w_u8(BChA_Request.NumTrue);		BChA_Request.NumTrue = 0;

		if ((char*)pSFalse != (char*)SFalse) P.w(SFalse, u32((char*)pSFalse-(char*)SFalse));
		if ((char*)pSTrue != (char*)STrue) P.w(STrue, u32((char*)pSTrue-(char*)STrue));
		//-----------------------------------------------------
		ClientID ClID; ClID.set(BChA_Request.SenderID);
		if(Level().Server) Level().Server->SendTo(ClID, P);
	};
}

void				WeaponUsageStatistic::On_Check_Respond			(NET_Packet* P)
{
	if (!P) return;
	u8 NumFalse = P->r_u8();
	u8 NumTrue = P->r_u8();

//	HitChecksRespondReceived += (NumFalse+NumTrue);
//	HitChecksRespondReceivedFalse += NumFalse;
//	HitChecksRespondReceivedTrue += NumTrue;

	u8 i;
	ABULLETS_it BulletIt;	
	for (i=0; i<NumFalse; i++)
	{
//		HitChecksRespondReceived++;
		u32 BulletID = P->r_u32();
		if (!FindBullet(BulletID, BulletIt)) 
		{
			Msg("! Warning: No bullet found! ID[%d]", BulletID);
			continue;
		};
		BulletIt->HitResponds++;
		RemoveBullet(BulletIt);
	}

	for (i=0; i<NumTrue; i++)
	{
//		HitChecksRespondReceived++;
		u32 BulletID = P->r_u32();
		s16 BoneID = P->r_s16();
		if (!FindBullet(BulletID, BulletIt)) 
		{
			Msg("! Warning: No bullet found! ID[%d]", BulletID);
			continue;
		};
		BulletIt->HitResponds++;
		//---------------------------------------------------------------
		PLAYERS_STATS_it PlayerIt = FindPlayer(*(BulletIt->FirerName));
		WEAPON_STATS_it WeaponIt = PlayerIt->FindPlayersWeapon(*(BulletIt->WeaponName));
		(*WeaponIt).m_dwKillsScored++;
		(*WeaponIt).m_dwKillsScored_d++;
		HITS_VEC_it HitIt;
		if (WeaponIt->FindHit(BulletID, HitIt))
		{
			HitData& HData = *HitIt;
			HData.Deadly = true;
			HData.BoneID = BoneID;
			CObject* pObj = Level().Objects.net_Find(HData.TargetID);
			if (pObj)
				HData.BoneName = smart_cast<CKinematics*>(pObj->Visual())->LL_BoneName_dbg(BoneID);
		}
		//---------------------------------------------------------------
		RemoveBullet(BulletIt);
	}
};
//---------------------------------------------------------------
void				WeaponUsageStatistic::Draw						()
{
	return;
	/*
	CGameFont* pFont = HUD().Font().pFontMedium;
	pFont->SetColor(color_rgba(255, 255, 255, 255));
	float X = 0;
	float Y = 200;
	string16 SBought = "   Bought", SRounds = "   Rounds", SBullets = "   Bullets", SHits = "   Hits", SKills = "   Kills";
	string64 tmp;
	pFont->OutSet(X, Y);	
	PLAYERS_STATS_it it;
	//---------------------------------------------------------------------
	float MaxNameLen = 0;
	for ( it = aPlayersStatistic.begin(); it != aPlayersStatistic.end(); ++it)
	{
		Player_Statistic& PS = *it;		
		WEAPON_STATS_it w_it;
		for (w_it = PS.aWeaponStats.begin(); w_it!=PS.aWeaponStats.end(); ++w_it)
		{
			Weapon_Statistic& WS = *w_it;
			float NameLen = pFont->SizeOf(*(WS.InvName))+20;
			if (NameLen > MaxNameLen) MaxNameLen = NameLen;
		};
	};
	//---------------------------------------------------------------------
	for ( it = aPlayersStatistic.begin(); it != aPlayersStatistic.end(); ++it)
	{
		Player_Statistic& PS = *it;		
		X=0;
		pFont->Out(X, Y, "-----------------------------------"); Y += pFont->CurrentHeight();
		pFont->Out(X, Y, "PLAYER:\t%s",*(PS.PName));		Y += pFont->CurrentHeight();
		pFont->Out(X, Y, "AliveTime (s) -- %.2f",float(PS.m_dwTotalAliveTime)/1000/PS.m_dwNumRespawned);		Y += pFont->CurrentHeight();
		pFont->Out(X, Y, "MoneyForRound -- %.2f",float(PS.m_dwTotalMoneyRound)/PS.m_dwNumRespawned);		Y += pFont->CurrentHeight();
		pFont->Out(MaxNameLen, Y, "%s%s%s%s%s", SBought, SRounds, SBullets, SHits, SKills);	Y += pFont->CurrentHeight();
//		pFont->Out(X, Y, "%d/%d - %d/%d-%d - %d/%d-%d", 
//					HitChecksSended, HitChecksReceived, 
//					HitChecksResponded, HitChecksRespondedFalse, HitChecksRespondedTrue,
//					HitChecksRespondReceived, HitChecksRespondReceivedFalse, HitChecksRespondReceivedTrue); Y += pFont->CurrentHeight();		
		WEAPON_STATS_it w_it;
		for (w_it = PS.aWeaponStats.begin(); w_it!=PS.aWeaponStats.end(); ++w_it)
		{
			X=0;
			Weapon_Statistic& WS = *w_it;
			pFont->Out(X, Y, " %s", *WS.InvName);			
			X=MaxNameLen;

			sprintf(tmp, "%d", WS.NumBought);
			X += pFont->SizeOf(SBought);
			pFont->Out(X - pFont->SizeOf(tmp), Y, tmp);

			sprintf(tmp, "%d", WS.RoundsFired);
			X += pFont->SizeOf(SRounds);
			pFont->Out(X - pFont->SizeOf(tmp), Y, tmp);

			sprintf(tmp, "%d", WS.BulletsFired);
			X += pFont->SizeOf(SBullets);
			pFont->Out(X - pFont->SizeOf(tmp), Y, tmp);

			sprintf(tmp, "%d", WS.HitsScored);
			X += pFont->SizeOf(SHits);
			pFont->Out(X - pFont->SizeOf(tmp), Y, tmp);

			X += pFont->SizeOf(SKills);
			sprintf(tmp, "%d", WS.KillsScored);
			pFont->Out(X - pFont->SizeOf(tmp), Y, tmp);

			sprintf(tmp, " ---- NumHits/Completed/Bullets = %d/%d/%d", WS.m_Hits.size(), WS.m_dwNumCompleted, ActiveBullets.size());
			pFont->Out(X, Y, tmp);

			Y += pFont->CurrentHeight();
		}
	};
	pFont->Out(0, Y, "-----------------------------------");
	*/
}

void				WeaponUsageStatistic::OnPlayerSpawned				(game_PlayerState* ps)
{
	if (!CollectData()) return;
	if (!ps) return;
	Player_Statistic& PlayerStat = *(FindPlayer(ps->getName()));
	PlayerStat.m_dwNumRespawned[ps->team]++;
	PlayerStat.m_dwCurMoneyRoundDelta = 0;
	m_dwTotalNumRespawns[ps->team]++;	
}

void				WeaponUsageStatistic::OnPlayerAddMoney			(game_PlayerState* ps, s32 MoneyAmount)
{
	if (!CollectData()) return;
	if (!ps || MoneyAmount<=0) return;
	Player_Statistic& PlayerStat = *(FindPlayer(ps->getName()));
	PlayerStat.m_dwCurMoneyRoundDelta += MoneyAmount;
};

void				WeaponUsageStatistic::OnPlayerKilled				(game_PlayerState* ps)
{
	if (!CollectData()) return;
	if (!ps) return;
	u32 dwAliveTime = ps->DeathTime - ps->RespawnTime;
	
	m_dwTotalPlayersAliveTime[ps->team] += dwAliveTime;

	Player_Statistic& PlayerStat = *(FindPlayer(ps->getName()));
	PlayerStat.m_dwTotalAliveTime[ps->team] += dwAliveTime;
	
	PlayerStat.m_dwTotalMoneyRound[ps->team] += PlayerStat.m_dwCurMoneyRoundDelta;
	m_dwTotalPlayersMoneyRound[ps->team] += PlayerStat.m_dwCurMoneyRoundDelta;
};

BOOL	g_bStatisticSaveAuto = FALSE;

void				WeaponUsageStatistic::Update()
{
	if (!CollectData()) return;
	//bool NeedSave = false;
	//bool NeedUpdate = false;
	if (Level().timeServer() < (m_dwLastUpdateTime + m_dwUpdateTimeDelta))
	{
		//---------------------------------------------
		m_dwLastUpdateTime = Level().timeServer();
		//---------------------------------------------
		NET_Packet P;
		P.w_begin	(M_STATISTIC_UPDATE);
		P.w_u32		(Level().timeServer());
		Level().Send(P);
	}
	if (Level().timeServer() < (m_dwLastSaveTime + m_dwSaveTimeDelta))
	{
		if (m_bFirstSave)
		{
			m_bFirstSave = false;
			//-----------------------------------------------
			string1024 GameType;
			SYSTEMTIME Time;	
			switch (GameID())
			{
			case GAME_DEATHMATCH: sprintf(GameType, "dm"); break;
			case GAME_TEAMDEATHMATCH: sprintf(GameType, "tdm"); break;
			case GAME_ARTEFACTHUNT: sprintf(GameType, "ah"); break;
			default:
				return;
				break;
			};
			GetLocalTime(&Time);	
			sprintf(mFileName, "logs\\(%s)_(%s)_%02d.%02d.%02d_%02d.%02d.%02d.wus", *(Level().name()), GameType, Time.wMonth, Time.wDay, Time.wYear, Time.wHour, Time.wMinute, Time.wSecond);
		}
		//---------------------------------------------
		m_dwLastSaveTime = Level().timeServer();
		//---------------------------------------------
		if (g_bStatisticSaveAuto) SaveData();
	}
};

void				WeaponUsageStatistic::OnUpdateRequest				(NET_Packet* /* P*/)
{
	if (aPlayersStatistic.empty()) return;
	Player_Statistic& PS = aPlayersStatistic.front();
	//-------------------------------------------------
	NET_Packet P;
	P.w_begin(M_STATISTIC_UPDATE_RESPOND);
	//-------------------------------------------------
	P.w_stringZ(PS.PName);
	PS.net_save(&P);
	//-------------------------------------------------
	Level().Send(P);
};

void				WeaponUsageStatistic::OnUpdateRespond				(NET_Packet* P)
{
	if (!P) return;
	shared_str PName;
	P->r_stringZ(PName);
	Player_Statistic& PS = *(FindPlayer(*PName));
	PS.net_load(P);
};

void			WeaponUsageStatistic::SetCollectData			(bool Collect)
{
	if (Collect && !m_bCollectStatistic)
		Clear();
	m_bCollectStatistic = Collect;
}