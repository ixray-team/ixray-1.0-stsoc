// Spawn_Event.h: interface for the CSpawn_Event class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPAWN_EVENT_H__7515F9EA_5014_4080_BB28_71497FCFCC6A__INCLUDED_)
#define AFX_SPAWN_EVENT_H__7515F9EA_5014_4080_BB28_71497FCFCC6A__INCLUDED_
#pragma once

#include "Spawn.h"

class CSpawn_Event : public CSpawn  
{
public:
	enum	ev_types
	{
		typeNone		= 0,
		typeActivate,
		typeDeactivate,
		typeCustom,

		typeFORCEDWORD	= DWORD(-1)
	};
	struct	Action 
	{
		xrP_TOKEN	type;
		string64	target;	// object name
		string128	custom;	// custom event
		Action() {
			type.IDselected	= typeNone;
			type.Count		= 4;
			strcpy			(target,"");
			strcpy			(custom,"");
		}
	};
	struct	Pair
	{
		xrP_BOOL	bOnce;
		xrP_CLSID	Target;
		Action		OnEnter;
		Action		OnLeave;
		Pair() {
			bOnce.value		= true;
			Target.Selected	= CLSID_OBJECT_ACTOR;
			Target.Count	= 0;
		}
	};
protected:
	vector<Pair>	Commands;

	void					ExportAction	(NET_Packet& P, Action& A);
public:
	virtual		LPCSTR		getComment		()				{ return "Event generator"; };
	
	virtual		void		Save			(CFS_Base&  FS);
	virtual		void		Load			(CStream&	FS);
	
	virtual		void		Execute			(CStream&	FS_CFORM);

	CSpawn_Event();
	virtual ~CSpawn_Event();
};

#endif // !defined(AFX_SPAWN_EVENT_H__7515F9EA_5014_4080_BB28_71497FCFCC6A__INCLUDED_)
