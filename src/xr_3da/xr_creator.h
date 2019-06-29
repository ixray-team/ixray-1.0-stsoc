#ifndef __XR_CREATOR_H__
#define __XR_CREATOR_H__

#include "fcontroller.h"
#include "xr_object_list.h"
#include "cameramanager.h"
#include "xr_area.h"

// refs
class ENGINE_API CCursor;
class ENGINE_API CCustomHUD;
class ENGINE_API CEnvironment;

//-----------------------------------------------------------------------------------------------------------
//World Container
//-----------------------------------------------------------------------------------------------------------
class	ENGINE_API				CCreator	:
	public DLL_Pure,
	public CController,
	public pureRender,
	public pureFrame,
	public CEventBase
{
public:
	// Loader
	IReader*					LL_Stream;
	xr_vector<char *>			LL_strings;
	LPCSTR						getString	(int id);
protected:
	// Network interface
	CObject*					pCurrentEntity;
	CObject*					pCurrentViewEntity;

	// Static sounds
	sound						Sounds_Ambience;
	xr_vector<sound>			Sounds;
	xr_vector<sound>			Sounds_Random;
	u32							Sounds_dwNextTime;
public:
	CObjectList					Objects;
	CEnvironment*				Environment;
	CCameraManager				Cameras;
	CObjectSpace				ObjectSpace;

	BOOL						bReady;

	CInifile*					pLevel;
	CCustomHUD*					pHUD;
public:
	// Main, global functions
								CCreator				( );
	virtual						~CCreator				( );

	virtual BOOL				net_Start				( LPCSTR op_server, LPCSTR op_client)	= 0;
	virtual void				net_Load				( LPCSTR name )							= 0;
	virtual void				net_Save				( LPCSTR name )							= 0;
	virtual void				net_Stop				( );

	virtual BOOL				Load					( u32 dwNum );
	virtual BOOL				Load_GameSpecific_Before( )										{ return TRUE; };		// before object loading
	virtual BOOL				Load_GameSpecific_After	( )										{ return TRUE; };		// after object loading
	virtual void				Load_GameSpecific_CFORM	( CDB::TRI* T, u32 count )				= 0;

	virtual void				OnFrame					( void );
	virtual void				OnRender				( void );

	// Main interface
	CObject*					CurrentEntity			( void ) const							{ return pCurrentEntity; }
	CObject*					CurrentViewEntity		( void ) const							{ return pCurrentViewEntity; }
	void						SetEntity				( CObject* O  )							{ pCurrentEntity=pCurrentViewEntity=O; }

	// Loader interface
	Shader*						LL_CreateShader			(int S, int T, int M, int C);
	void						LL_CheckTextures		();
};

extern ENGINE_API	CCreator*	pCreator;

#endif //__XR_CREATOR_H__
