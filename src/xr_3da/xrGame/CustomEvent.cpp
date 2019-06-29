// CustomEvent.cpp: implementation of the CCustomEvent class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CustomEvent.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCustomEvent::CCustomEvent()
{
}

CCustomEvent::~CCustomEvent		()
{
	for (tActions_it it=Actions.begin(); Actions.end() != it; ++it) 
	{
		Engine.Event.Destroy(it->E);
		xr_free(it->P1);
	}
	Actions.clear();
}

void CCustomEvent::Load			(LPCSTR section)
{
	// Name
	R_ASSERT					(section);
	cName_set					(section);
	cNameSect_set				(section);
	
	// Geometry and transform
	/*
	Fvector dir,norm;
	Position()					= ini->r_fvector3(section,"position");
	vScale						= ini->r_fvector3(section,"scale");
	dir							= ini->r_fvector3(section,"direction");
	norm						= ini->r_fvector3(section,"normal");
	mRotate.rotation			(dir,norm);
	UpdateTransform				();
	*/
	
	// General stuff
	pVisualName					= NULL;
	setVisible					(FALSE);
	
	// Sheduler
	shedule.t_min				= 300;
	shedule.t_max				= 1000;
}

ENGINE_API void _parse(LPSTR dest, LPCSTR src);

void CCustomEvent::Parse		(DEF_EVENT& D, LPCSTR DEF_Base)
{
	string512	DEF;
	_parse		(DEF,DEF_Base);
		
	D.E			= 0;
	D.P1		= 0;
	string512	Event,Param;
	Event[0]=0; Param[0]=0;
	sscanf	(DEF,"%[^,],%s",Event,Param);
	if (Event[0]) {
		// Parse param's macroses
		char	Parsed	[1280]="", sBegin[1280]="", sName[1280]="", sEnd[1280]="", sBuf[128]="";
		sscanf	(Param,"%[^$]$rp$%[^$]$%s",sBegin,sName,sEnd);
		if (sName[0])	{
			int id		= Level().get_RPID(sName);
			R_ASSERT	(id>=0);
			strconcat	(Parsed,sBegin,itoa(id,sBuf,10),sEnd);
		} else {
			strcpy		(Parsed,Param);
		}
		
		// Create
		D.E  = Engine.Event.Create(Event); 
		D.P1 = xr_strdup	(Parsed); 
	}
}

BOOL CCustomEvent::net_Spawn	( LPVOID DC )
{
	inherited::net_Spawn		( DC );

	// Read CFORM
	{
		CCF_Shape*	shape			= xr_new<CCF_Shape>	(this);
		CFORM()						= shape;
		u8 count;	P.r_u8			(count);
		while (count)	{
			u8 type; P.r_u8	(type);
			switch (type)	{
			case 0:
				{
					Fsphere data;
					P.r					(&data,sizeof(data));
					shape->add_sphere	(data);
				}
				break;
			case 1:
				{
					Fmatrix data;
					P.r_matrix			(data);
					shape->add_box		(data);
				}
				break;
			}
			--count;
		}
		shape->ComputeBounds		();
		g_pGameLevel->ObjectSpace.Object_Register		(this);
		CFORM()->OnMove				();
	}
	
	// Read actions
	{
		u8 count;	P.r_u8			(count);
		while (count)	{
			DEF_EVENT			E;
			string256			str;
			P.r_u8				(E.type	);
			P.r_u16				(E.count);
			P.r_u64				(E.CLS	);
			P.r_string			(str	);
			Parse				(E,str	);
			Actions.push_back	(E);
			--count;
		}
	}
	
	return TRUE;
}

void CCustomEvent::shedule_Update	(u32 dt)
{
	inherited::shedule_Update		(dt);
	if (Remote())	return;
	
	if (!Contacted.empty()) {
		CCF_Shape* M = dynamic_cast<CCF_Shape*> (CFORM()); R_ASSERT	(M);
		for (u32 i=0; i<Contacted.size(); ++i) 
		{
			// Check if it is still contact us
			CObject* O = Contacted[i];
			if (!M->Contact(O)) 
			{
				// Search if we have some action for this type of object
				CLASS_ID cls = O->SUB_CLS_ID;
				for (tActions_it it=Actions.begin(); Actions.end() != it; ++it) {
					if ((it->type==1) && (it->CLS == cls) && (it->count))	{
						if (0xffff != it->count)	it->count -= 1;
						Engine.Event.Signal(it->E,u32(it->P1),u32(O));
					}
				}

				// Erase it from list
				Contacted.erase(Contacted.begin()+i);
				--i;
			}
		}
	}
}

void CCustomEvent::OnNear( CObject* O )
{
	if (Remote())	return;

	// check if not contacted before
	if (find(Contacted.begin(),Contacted.end(),O)!=Contacted.end()) return;
	
	// check if it is actually contacted
	CCF_Shape* M = dynamic_cast<CCF_Shape*>(CFORM()); R_ASSERT(M);
	if (M->Contact(O)) {
		Contacted.push_back	(O);

		// search if we have some action for this type of object
		CLASS_ID cls = O->SUB_CLS_ID;
		for (tActions_it it=Actions.begin(); Actions.end() != it; ++it) {
			if ((it->type==0) && (it->CLS == cls) && (it->count))	{
				if (0xffff != it->count)	it->count -= 1;
				Engine.Event.Signal(it->E,u32(it->P1),u32(O));
			}
		}
	}
}

#ifdef DEBUG
void CCustomEvent::OnRender()
{
	if (!bDebug)	return;

	Fvector H1; H1.set(0.5f,0.5f,0.5f);
	RCache.dbg_DrawOBB(XFORM(),H1,D3DCOLOR_XRGB(255,255,255));
}
#endif
