#pragma once
#include "inventory.h"

class CWeaponHUD;

#define MS_HIDDEN	0
#define MS_SHOWING	1
#define MS_IDLE		2
#define MS_THREATEN	3
#define MS_READY	4
#define MS_THROW	5
#define MS_END		6
#define MS_EMPTY	7
#define MS_HIDING	8
#define MS_PLAYING	9

class CWeaponThrust :
	public CInventoryItem
{
	typedef CInventoryItem inherited;
public:
	CWeaponThrust(void);
	virtual ~CWeaponThrust(void);

	void SoundCreate(sound& dest, LPCSTR name, int iType=0, BOOL bCtrlFreq=FALSE);
	void SoundDestroy(sound& dest);

	virtual void Load(LPCSTR section);
	virtual BOOL net_Spawn(LPVOID DC);
	virtual void net_Destroy();
	//virtual void net_Export(NET_Packet& P);
	//virtual void net_Import(NET_Packet& P);
	//virtual void Update(u32 dt);
	virtual void UpdateCL();
	virtual void OnVisible();
	virtual void OnH_B_Chield();
	virtual void OnH_B_Independent();
	virtual void OnAnimationEnd();
	//virtual void OnDeviceCreate();
	//virtual void OnDeviceDestroy();

	virtual void Show();
	virtual void Hide();
	virtual void Throw();
	virtual void Destroy();

	virtual bool Action(s32 cmd, u32 flags);

	u32 State();
	virtual u32 State(u32 state);

	CWeaponHUD* m_pHUD;
	u32 m_state;
	bool m_throw;
	f32 m_force, m_minForce, m_maxForce, m_forceGrowSpeed;
	u32 m_destroyTime, m_stateTime;
};
