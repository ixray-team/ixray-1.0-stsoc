// Spawn.h: interface for the CSpawn class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPAWN_H__88E2B303_2B31_4EA8_912D_5E9A2F9EBF10__INCLUDED_)
#define AFX_SPAWN_H__88E2B303_2B31_4EA8_912D_5E9A2F9EBF10__INCLUDED_
#pragma once

// CLSID
#define		CLSID_SPAWN_EVENT		MK_CLSID('S','_','E','V','E','N','T',' ')
#define		CLSID_SPAWN_ZONE		MK_CLSID('S','_','Z','O','N','E',' ',' ')
#define		CLSID_SPAWN_ENTITY		MK_CLSID('S','_','E','N','T','I','T','Y')

// CFORM usage

// Description
#pragma pack(push,4)
class CSpawn_DESC
{
public:
	CLASS_ID	CLS;
	char		cName		[128];
	WORD		version;
	DWORD		IDcform;
	Fvector		o_Position;			// position: x,y,z
	Fvector		o_Orientation;		// rotation: angle_x, angle_y, angle_z

	CSpawn_DESC	()	
	{
		CLS			= CLASS_ID(0);
		cName[0]	= 0;
		version		= 0;
	}
};
#pragma pack(push,4)

// Base class
class CSpawn : public CPropertyBase
{
protected:	
	CSpawn_DESC				description;
public:
	static		CSpawn*		Create			(CLASS_ID cls);
	static		void		CreatePalette	(vector<CSpawn*> & palette);

	CSpawn_DESC&			getDescription	()	{return description;}
	virtual 	LPCSTR		getName			()	{return description.cName;}
	virtual		LPCSTR		getComment		()	= 0;
	
	virtual		void		Save			(CFS_Base&  FS);
	virtual		void		Load			(CStream&	FS);
	
	virtual		void		Execute			(CStream&	FS_CFORM)		= 0;

	CSpawn();
	virtual ~CSpawn();
};

#endif // !defined(AFX_SPAWN_H__88E2B303_2B31_4EA8_912D_5E9A2F9EBF10__INCLUDED_)
