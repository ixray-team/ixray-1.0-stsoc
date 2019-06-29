// Spawn_Event.cpp: implementation of the CSpawn_Event class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "Spawn_Event.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSpawn_Event::CSpawn_Event()
{
	description.CLS					= CLSID_SPAWN_EVENT;
	Commands.push_back				(Pair());
	Commands.back().Target.Count	= 2;
}

CSpawn_Event::~CSpawn_Event()
{
	
}

void CSpawn_Event::Save	(CFS_Base& FS)
{
	for (u32 cmd=0; cmd<Commands.size(); cmd++)
	{
		xrP_Template	tmpl;
		xrP_TOKEN::Item	item;
		CLASS_ID		cls;
		Pair& P			= Commands[cmd];
		tmpl.Type		= 0;
		tmpl.Limit		= 10;
		
		// 
		xrPWRITE_PROP	(FS,"Action",		xrPID_MARKER_TEMPLATE,	tmpl		);
		xrPWRITE_PROP	(FS,"Execute Once",	xrPID_BOOL,				P.bOnce		);
		xrPWRITE_PROP	(FS,"Target Class",	xrPID_CLSID,			P.Target	);
		cls = CLSID_OBJECT_ACTOR;	FS.write(&cls,sizeof(cls));
		cls = CLSID_AI_HEN;			FS.write(&cls,sizeof(cls));
		
		//
		xrPWRITE_PROP	(FS,"ENTER: type",	xrPID_TOKEN,			P.OnEnter.type);
		item.ID = typeNone;			strcpy(item.str,"none");		FS.write(&item,sizeof(item));
		item.ID = typeActivate;		strcpy(item.str,"activate");	FS.write(&item,sizeof(item));
		item.ID = typeDeactivate;	strcpy(item.str,"deactivate");	FS.write(&item,sizeof(item));
		item.ID = typeCustom;		strcpy(item.str,"custom");		FS.write(&item,sizeof(item));
		xrPWRITE_PROP	(FS,"ENTER: object",xrPID_OBJECT,			P.OnEnter.target);
		xrPWRITE_PROP	(FS,"ENTER: custom",xrPID_STRING,			P.OnEnter.custom);
		
		//
		xrPWRITE_PROP	(FS,"LEAVE: type",	xrPID_TOKEN,			P.OnLeave.type);
		item.ID = typeNone;			strcpy(item.str,"none");		FS.write(&item,sizeof(item));
		item.ID = typeActivate;		strcpy(item.str,"activate");	FS.write(&item,sizeof(item));
		item.ID = typeDeactivate;	strcpy(item.str,"deactivate");	FS.write(&item,sizeof(item));
		item.ID = typeCustom;		strcpy(item.str,"custom");		FS.write(&item,sizeof(item));
		xrPWRITE_PROP	(FS,"LEAVE: object",xrPID_OBJECT,			P.OnLeave.target);
		xrPWRITE_PROP	(FS,"LEAVE: custom",xrPID_STRING,			P.OnLeave.custom);
	}
}

void CSpawn_Event::Load	(CStream& FS)
{
	Commands.clear		();
	while (!FS.Eof())	{
		Commands.push_back	(Pair());
		Pair& P			= Commands.back();
		xrP_Template	tmpl;
		
		// 
		xrPREAD_PROP	(FS,xrPID_MARKER_TEMPLATE,	tmpl			);
		xrPREAD_PROP	(FS,xrPID_BOOL,				P.bOnce			);
		xrPREAD_PROP	(FS,xrPID_CLSID,			P.Target		);
		
		//
		xrPREAD_PROP	(FS,xrPID_TOKEN,			P.OnEnter.type	);
		xrPREAD_PROP	(FS,xrPID_OBJECT,			P.OnEnter.target);
		xrPREAD_PROP	(FS,xrPID_STRING,			P.OnEnter.custom);
		
		//
		xrPREAD_PROP	(FS,xrPID_TOKEN,			P.OnLeave.type	);
		xrPREAD_PROP	(FS,xrPID_OBJECT,			P.OnLeave.target);
		xrPREAD_PROP	(FS,xrPID_STRING,			P.OnLeave.custom);
	}
	R_ASSERT			(!Commands.empty());
}

#ifndef _EDITOR
void CSpawn_Event::ExportAction(NET_Packet& P, Action& A)
{
	char	buffer[256];
	switch (A.type.IDselected)
	{
	default:
	case typeNone:		P.w_string("");												break;
	case typeActivate:	P.w_string(strconcat(buffer,"level.activate,",A.target));	break;
	case typeDeactivate:P.w_string(strconcat(buffer,"level.deactivate,",A.target));	break;
	case typeCustom:	P.w_string(A.custom);										break;
	}
}

void CSpawn_Event::Execute(CStream& FS_CFORM)
{
	NET_Packet	P;
	
	//*** generic
	P.w_begin	(M_SPAWN);
	P.w_string	("m_event");
	P.w_u8		(0);
	P.w_u8		(0);
	P.w_u8		(0);
	P.w_u8		(0xFE);
	P.w_vec3	(description.o_Position);
	P.w_vec3	(description.o_Orientation);
	
	//*** addititional data
	u32 size_pos= P.w_tell();
	P.w_u16		(0);
	
	// CForm
	CStream*	cform	= FS_CFORM.OpenChunk	(description.IDcform);
	R_ASSERT	(cform);
	P.w			(cform->Pointer(),cform->Length());
	cform->Close();
	
	// Commands
	P.w_u8		(Commands.size());
	for (u32 cmd=0; cmd<Commands.size(); cmd++)
	{
		Pair&			A = Commands[cmd];
		P.w_u8			(A.bOnce.value);
		P.w_u64			(A.Target.Selected);
		ExportAction	(P,A.OnEnter);
		ExportAction	(P,A.OnLeave);
	}

	// Save size
	/*
	u32 end		= P.w_tell();
	P.w_seek	(size_pos);
	P.w_u16		(end-size_pos);
	P.w_seek	(end);
	*/
}

#else
void CSpawn_Event::Execute(CStream& FS)
{
}
#endif
