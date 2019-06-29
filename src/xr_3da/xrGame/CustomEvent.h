// CustomEvent.h: interface for the CCustomEvent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CUSTOMEVENT_H__092059D7_D2E8_4BC3_95C5_F2B3D48AAE5C__INCLUDED_)
#define AFX_CUSTOMEVENT_H__092059D7_D2E8_4BC3_95C5_F2B3D48AAE5C__INCLUDED_
#pragma once

class CCustomEvent		: 
	public CObject
#ifdef DEBUG
	,public pureRender
#endif
{
private:
	typedef CObject			inherited;
protected:
	// t-defs
	struct DEF_EVENT	{
		u8			type;
		u16			count;
		CLASS_ID	CLS;
		EVENT		E;
		ref_str		P1;
	};
	typedef xr_vector<DEF_EVENT>	tActions;
	typedef tActions::iterator		tActions_it;

	tActions				Actions;
	xr_vector<CObject*>		Contacted;
protected:
	void					Parse				( DEF_EVENT& D, LPCSTR DEF);
public:
	CCustomEvent();
	virtual ~CCustomEvent();

	virtual void			Load				( LPCSTR section );
	virtual BOOL			net_Spawn			( BOOL bLocal, int server_id, Fvector& o_pos, Fvector& o_angle, NET_Packet& P, u16 flags );
	
	// Update
	virtual void			shedule_Update		( u32 dt );

	// Collision
	virtual void			OnNear				( CObject* near   );
	virtual void			OnRender			( );
};

#endif // !defined(AFX_CUSTOMEVENT_H__092059D7_D2E8_4BC3_95C5_F2B3D48AAE5C__INCLUDED_)
