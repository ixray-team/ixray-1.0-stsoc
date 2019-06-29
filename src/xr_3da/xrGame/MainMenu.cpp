#include "stdafx.h"
#include "MainMenu.h"
#include "UI/UIDialogWnd.h"
#include "ui/UIMessageBoxEx.h"
#include "../xr_IOConsole.h"
#include "../IGame_Level.h"
#include "../CameraManager.h"
#include "xr_Level_controller.h"
#include "ui\UITextureMaster.h"
#include "ui\UIXmlInit.h"
#include <dinput.h>
#include "ui\UIBtnHint.h"
#include "UICursor.h"
#include "gamespy/GameSpy_Full.h"
#include "gamespy/GameSpy_HTTP.h"
#include "gamespy/GameSpy_Available.h"
#include "gamespy/CdkeyDecode/cdkeydecode.h"

#include "object_broker.h"



string128	ErrMsgBoxTemplate	[]	= {
	"ErrNoError",
		"message_box_invalid_pass",
		"message_box_invalid_host",
		"message_box_session_full",
		"message_box_server_reject",
		"message_box_cdkey_in_use",
		"message_box_cdkey_disabled",
		"message_box_cdkey_invalid",
		"message_box_different_version",
		"message_box_gs_service_not_available",
		"message_box_sb_master_server_connect_failed"
};

CMainMenu*	MainMenu()	{return (CMainMenu*)g_pGamePersistent->m_pMainMenu; };
//----------------------------------------------------------------------------------
#define INIT_MSGBOX(_box, _template)	{ _box = xr_new<CUIMessageBoxEx>(); _box->Init(_template);}
//----------------------------------------------------------------------------------
CMainMenu::CMainMenu	()
{
	m_Flags.zero				();
	m_startDialog				= NULL;
	g_pGamePersistent->m_pMainMenu= this;
	if (Device.b_is_Ready)		OnDeviceCreate();  	
	ReadTextureInfo				();
	CUIXmlInit::InitColorDefs	();
	g_btnHint					= xr_new<CUIButtonHint>();

	m_pMessageBox				= NULL;
	m_deactivated_frame			= 0;	
	
	m_sPatchURL					= "";

//	m_pGameSpyHTTP				= xr_new<CGameSpy_HTTP>();
	m_pGameSpyFull				= xr_new<CGameSpy_Full>();

	m_sPDProgress.IsInProgress	= false;
	//---------------------------------------------------------------
	m_NeedErrDialog				= ErrNoError;

	for (u32 i=u32(ErrInvalidPassword); i<u32(ErrMax); i++)
	{
		CUIMessageBoxEx* pNewErrDlg;
		INIT_MSGBOX(pNewErrDlg, ErrMsgBoxTemplate[i]);
		m_pMB_ErrDlgs.push_back(pNewErrDlg);
	}
	//---------------------------------------------------------------
	INIT_MSGBOX(m_pMSB_NoNewPatch, "msg_box_no_new_patch");
	m_pMSB_NewPatch = NULL;
	INIT_MSGBOX(m_pMSB_PatchDownloadError, "msg_box_patch_download_error");

	INIT_MSGBOX(m_pMSB_PatchDownloadSuccess, "msg_box_patch_download_success");
	Register(m_pMSB_PatchDownloadSuccess);
	m_pMSB_PatchDownloadSuccess->SetWindowName("msg_box");
	m_pMSB_PatchDownloadSuccess->AddCallback	("msg_box", MESSAGE_BOX_YES_CLICKED, CUIWndCallback::void_function(this, &CMainMenu::OnRunDownloadedPatch));

	INIT_MSGBOX(m_pMSB_ConnectToMasterServer, "msg_box_connect_to_master_server");
	m_screenshotFrame = u32(-1);
}

CMainMenu::~CMainMenu	()
{
	xr_delete						(g_btnHint);
	xr_delete						(m_startDialog);
	xr_delete						(m_pMessageBox);
	g_pGamePersistent->m_pMainMenu	= NULL;
//	xr_delete						(m_pGameSpyHTTP);
	xr_delete						(m_pGameSpyFull);
	//---------------------------------------------------
	delete_data						(m_pMB_ErrDlgs);	
	//---------------------------------------------------
	delete_data						(m_pMSB_NoNewPatch);
	delete_data						(m_pMSB_NewPatch);
	delete_data						(m_pMSB_PatchDownloadError);
	delete_data						(m_pMSB_PatchDownloadSuccess);
	delete_data						(m_pMSB_ConnectToMasterServer);

	CUITextureMaster::WriteLog		();
}

void CMainMenu::ReadTextureInfo()
{
	if (pSettings->section_exist("texture_desc"))
	{
		xr_string itemsList; 
		string256 single_item;

		itemsList = pSettings->r_string("texture_desc", "files");
		int itemsCount	= _GetItemCount(itemsList.c_str());

		for (int i = 0; i < itemsCount; i++)
		{
			_GetItem(itemsList.c_str(), i, single_item);
			strcat(single_item,".xml");
			CUITextureMaster::ParseShTexInfo(single_item);
		}		
	}
}

extern ENGINE_API BOOL	bShowPauseString;
extern bool				IsGameTypeSingle();

void CMainMenu::Activate	(bool bActivate)
{
	if (	!!m_Flags.test(flActive) == bActivate)		return;
	if (	m_Flags.test(flGameSaveScreenshot)	)		return;
	if (	(m_screenshotFrame == Device.dwFrame)	||
			(m_screenshotFrame == Device.dwFrame-1) ||
			(m_screenshotFrame == Device.dwFrame+1))	return;

	bool b_is_single		= IsGameTypeSingle();

	if(bActivate)
	{
		Device.Pause				(TRUE, FALSE, TRUE, "mm_activate1");
			m_Flags.set					(flActive|flNeedChangeCapture,TRUE);

		{
			DLL_Pure* dlg = NEW_INSTANCE(TEXT2CLSID("MAIN_MNU"));
			if(!dlg) 
			{
				m_Flags.set				(flActive|flNeedChangeCapture,FALSE);
				return;
			}
			xr_delete					(m_startDialog);
			m_startDialog				= smart_cast<CUIDialogWnd*>(dlg);
			VERIFY						(m_startDialog);
		}

		m_Flags.set					(flRestoreConsole,Console->bVisible);
		
		if(b_is_single)	m_Flags.set	(flRestorePause,Device.Paused());
		
		Console->Hide				();

		m_Flags.set					(flRestoreCursor,GetUICursor()->IsVisible());

		if(b_is_single)
		{
			m_Flags.set					(flRestorePauseStr, bShowPauseString);
			bShowPauseString			= FALSE;
			if(!m_Flags.test(flRestorePause))
				Device.Pause			(TRUE, TRUE, FALSE, "mm_activate2");
		}

		m_startDialog->m_bWorkInPause		= true;
		StartStopMenu						(m_startDialog,true);
		
		if(g_pGameLevel)
		{
			if(b_is_single){
				Device.seqFrame.Remove		(g_pGameLevel);
			}
			Device.seqRender.Remove			(g_pGameLevel);
			CCameraManager::ResetPP			();
		};
		Device.seqRender.Add				(this, 4); // 1-console 2-cursor 3-tutorial

	}else{
		m_deactivated_frame					= Device.dwFrame;
		m_Flags.set							(flActive,				FALSE);
		m_Flags.set							(flNeedChangeCapture,	TRUE);

		Device.seqRender.Remove				(this);
		
		bool b = !!Console->bVisible;
		if(b){
			Console->Hide					();
		}

		IR_Release							();
		if(b){
			Console->Show					();
		}

		StartStopMenu						(m_startDialog,true);
		CleanInternals						();
		if(g_pGameLevel)
		{
			if(b_is_single){
				Device.seqFrame.Add			(g_pGameLevel);

			}
			Device.seqRender.Add			(g_pGameLevel);
		};
		if(m_Flags.test(flRestoreConsole))
			Console->Show			();

		if(b_is_single)
		{
			if(!m_Flags.test(flRestorePause))
				Device.Pause			(FALSE, TRUE, FALSE, "mm_deactivate1");

			bShowPauseString			= m_Flags.test(flRestorePauseStr);
		}	

	
		if(m_Flags.test(flRestoreCursor))
			GetUICursor()->Show			();

		Device.Pause					(FALSE, FALSE, TRUE, "mm_deactivate2");

		if(m_Flags.test(flNeedVidRestart))
		{
			m_Flags.set			(flNeedVidRestart, FALSE);
			Console->Execute	("vid_restart");
		}

	}
}
bool CMainMenu::IsActive()
{
	return !!m_Flags.test(flActive);
}


//IInputReceiver
static int mouse_button_2_key []	=	{MOUSE_1,MOUSE_2,MOUSE_3};
void	CMainMenu::IR_OnMousePress				(int btn)	
{	
	if(!IsActive()) return;

	IR_OnKeyboardPress(mouse_button_2_key[btn]);
};

void	CMainMenu::IR_OnMouseRelease(int btn)	
{
	if(!IsActive()) return;

	IR_OnKeyboardRelease(mouse_button_2_key[btn]);
};

void	CMainMenu::IR_OnMouseHold(int btn)	
{
	if(!IsActive()) return;

	IR_OnKeyboardHold(mouse_button_2_key[btn]);

};

void	CMainMenu::IR_OnMouseMove(int x, int y)
{
	if(!IsActive()) return;

	if(MainInputReceiver())
		MainInputReceiver()->IR_OnMouseMove(x, y);

};

void	CMainMenu::IR_OnMouseStop(int x, int y)
{
};

void	CMainMenu::IR_OnKeyboardPress(int dik)
{
	if(!IsActive()) return;

	if( is_binded(kCONSOLE, dik) )
	{
		Console->Show();
		return;
	}
	if (DIK_F12 == dik){
		Render->Screenshot();
		return;
	}

	if(MainInputReceiver())
		MainInputReceiver()->IR_OnKeyboardPress( dik);

};

void	CMainMenu::IR_OnKeyboardRelease			(int dik)
{
	if(!IsActive()) return;
	
	if(MainInputReceiver())
		MainInputReceiver()->IR_OnKeyboardRelease(dik);

};

void	CMainMenu::IR_OnKeyboardHold(int dik)	
{
	if(!IsActive()) return;
	
	if(MainInputReceiver())
		MainInputReceiver()->IR_OnKeyboardHold(dik);
};

void CMainMenu::IR_OnMouseWheel(int direction)
{
	if(!IsActive()) return;
	
	if(MainInputReceiver())
		MainInputReceiver()->IR_OnMouseWheel(direction);
}

extern bool b_shniaganeed_pp;

bool CMainMenu::OnRenderPPUI_query()
{
	return IsActive() && !m_Flags.test(flGameSaveScreenshot) /*&& b_shniaganeed_pp*/;
}


void CMainMenu::OnRender	()
{
	if(m_Flags.test(flGameSaveScreenshot))
		return;

	Render->Render				();
}

void CMainMenu::OnRenderPPUI_main	()
{
	if(!IsActive()) return;

	if(m_Flags.test(flGameSaveScreenshot))
		return;

	UI()->pp_start();

	DoRenderDialogs();

	UI()->RenderFont();
	UI()->pp_stop();
}

void CMainMenu::OnRenderPPUI_PP	()
{
	if ( !IsActive() ) return;

	if(m_Flags.test(flGameSaveScreenshot))	return;

	UI()->pp_start();
	
	xr_vector<CUIWindow*>::iterator it = m_pp_draw_wnds.begin();
	for(; it!=m_pp_draw_wnds.end();++it)
	{
		(*it)->Draw();
	}
	UI()->pp_stop();
}

void CMainMenu::StartStopMenu(CUIDialogWnd* pDialog, bool bDoHideIndicators)
{
	pDialog->m_bWorkInPause = true;
	CDialogHolder::StartStopMenu(pDialog, bDoHideIndicators);
}

//pureFrame
void CMainMenu::OnFrame()
{
	if (m_Flags.test(flNeedChangeCapture))
	{
		m_Flags.set					(flNeedChangeCapture,FALSE);
		if (m_Flags.test(flActive))	IR_Capture();
		else						IR_Release();
	}
	CDialogHolder::OnFrame		();


	//screenshot stuff
	if(m_Flags.test(flGameSaveScreenshot) && Device.dwFrame > m_screenshotFrame  )
	{
		m_Flags.set					(flGameSaveScreenshot,FALSE);
		::Render->Screenshot		(IRender_interface::SM_FOR_GAMESAVE, m_screenshot_name);
		
		if(g_pGameLevel && m_Flags.test(flActive))
		{
			Device.seqFrame.Remove	(g_pGameLevel);
			Device.seqRender.Remove	(g_pGameLevel);
		};

		if(m_Flags.test(flRestoreConsole))
			Console->Show			();
	}

//	m_pGameSpyHTTP->Think();
	m_pGameSpyFull->Update();
	CheckForErrorDlg();
}

void CMainMenu::OnDeviceCreate()
{
}


void CMainMenu::Screenshot(IRender_interface::ScreenshotMode mode, LPCSTR name)
{
	if(mode != IRender_interface::SM_FOR_GAMESAVE)
	{
		::Render->Screenshot		(mode,name);
	}else{
		m_Flags.set					(flGameSaveScreenshot, TRUE);
		strcpy(m_screenshot_name,name);
		if(g_pGameLevel && m_Flags.test(flActive)){
			Device.seqFrame.Add		(g_pGameLevel);
			Device.seqRender.Add	(g_pGameLevel);
		};
		m_screenshotFrame			= Device.dwFrame+1;
		m_Flags.set					(flRestoreConsole,		Console->bVisible);
		Console->Hide				();
	}
}

void CMainMenu::RegisterPPDraw(CUIWindow* w)
{
	UnregisterPPDraw				(w);
	m_pp_draw_wnds.push_back		(w);
}

void CMainMenu::UnregisterPPDraw				(CUIWindow* w)
{
	xr_vector<CUIWindow*>::iterator it = remove( m_pp_draw_wnds.begin(), m_pp_draw_wnds.end(), w);
	m_pp_draw_wnds.erase(it, m_pp_draw_wnds.end());
}

void CMainMenu::SetErrorDialog					(EErrorDlg ErrDlg)	
{ 
	m_NeedErrDialog = ErrDlg;
};

void CMainMenu::CheckForErrorDlg()
{
	if (m_NeedErrDialog == ErrNoError) return;
	StartStopMenu(m_pMB_ErrDlgs[m_NeedErrDialog-ErrInvalidPassword], false);
	m_NeedErrDialog = ErrNoError;
};

void CMainMenu::SwitchToMultiplayerMenu			()
{
	m_startDialog->Dispatch				(2,1);
};

void CMainMenu::DestroyInternal(bool bForce)
{
	if(m_startDialog && ((m_deactivated_frame < Device.dwFrame+4)||bForce) )
		xr_delete		(m_startDialog);
}

void CMainMenu::OnNewPatchFound					(LPCSTR VersionName, LPCSTR URL)
{
	if (m_sPDProgress.IsInProgress) return;
	if (m_pMSB_NewPatch)	
	{
		delete_data(m_pMSB_NewPatch);
		m_pMSB_NewPatch = NULL;
	}
	if (!m_pMSB_NewPatch)
	{
		INIT_MSGBOX(m_pMSB_NewPatch, "msg_box_new_patch");

		shared_str tmpText;
		tmpText.sprintf(m_pMSB_NewPatch->GetText(), VersionName, URL);
		m_pMSB_NewPatch->SetText(*tmpText);		
	}
	m_sPatchURL = URL;
	
	Register						(m_pMSB_NewPatch);
	m_pMSB_NewPatch->SetWindowName	("msg_box");
	m_pMSB_NewPatch->AddCallback	("msg_box", MESSAGE_BOX_YES_CLICKED, CUIWndCallback::void_function(this, &CMainMenu::OnDownloadPatch));
	StartStopMenu(m_pMSB_NewPatch, false);
};

void			CMainMenu::OnNoNewPatchFound				()
{
	StartStopMenu(m_pMSB_NoNewPatch, false);
}

void CMainMenu::OnDownloadPatch(CUIWindow*, void*)
{
	CGameSpy_Available GSA;
	shared_str result_string;
	if (!GSA.CheckAvailableServices(result_string))
	{
		Msg(*result_string);
		return;
	};
	
	LPCSTR fileName = *m_sPatchURL;
	if (!fileName) return;

	string4096 FilePath = "";
	char* FileName = NULL;
	GetFullPathName(fileName, 4096, FilePath, &FileName);
	/*
	if (strrchr(fileName, '/')) fileName = strrchr(fileName, '/')+1;
	else
		if (strrchr(fileName, '\\')) fileName = strrchr(fileName, '\\')+1;
	if (!fileName) return;
	*/

	string_path		fname;
	if (FS.path_exist("$downloads$"))
	{
		FS.update_path(fname, "$downloads$", FileName);
		m_sPatchFileName = fname;
	}
	else
		m_sPatchFileName.sprintf	("downloads\\%s", FileName);	
	
	m_sPDProgress.IsInProgress	= true;
	m_sPDProgress.Progress		= 0;
	m_sPDProgress.FileName		= m_sPatchFileName;
	m_sPDProgress.Status		= "";

	m_pGameSpyFull->m_pGS_HTTP->DownloadFile(*m_sPatchURL, *m_sPatchFileName);
}

void	CMainMenu::OnDownloadPatchError()
{
	m_sPDProgress.IsInProgress	= false;
	StartStopMenu(m_pMSB_PatchDownloadError, false);
};

void	CMainMenu::OnDownloadPatchSuccess			()
{
	m_sPDProgress.IsInProgress	= false;
	
/*
	if (!m_pMessageBox)
	{
		m_pMessageBox = xr_new<CUIMessageBoxEx>();		
	}
	m_pMessageBox->Init("msg_box_patch_download_success");	
	Register						(m_pMessageBox);
	m_pMessageBox->SetWindowName("msg_box_DPS");
	m_pMessageBox->AddCallback	("msg_box_DPS", MESSAGE_BOX_YES_CLICKED, CUIWndCallback::void_function(this, &CMainMenu::OnRunDownloadedPatch));
*/
	StartStopMenu(m_pMSB_PatchDownloadSuccess, false);
}

void	CMainMenu::OnDownloadPatchProgress			(u64 bytesReceived, u64 totalSize)
{
	m_sPDProgress.Progress = (float(bytesReceived)/float(totalSize))*100.0f;
};

extern ENGINE_API string512  g_sLaunchOnExit_app;
extern ENGINE_API string512  g_sLaunchOnExit_params;
void	CMainMenu::OnRunDownloadedPatch			(CUIWindow*, void*)
{
	strcpy					(g_sLaunchOnExit_app,*m_sPatchFileName);
	strcpy					(g_sLaunchOnExit_params,"");
	Console->Execute		("quit");
}

void CMainMenu::CancelDownload()
{
	m_pGameSpyFull->m_pGS_HTTP->StopDownload();
	m_sPDProgress.IsInProgress	= false;
}

void CMainMenu::SetNeedVidRestart()
{
	m_Flags.set(flNeedVidRestart,TRUE);
}

void CMainMenu::OnDeviceReset()
{
	if(IsActive() && g_pGameLevel)
		SetNeedVidRestart();
}

extern	void	GetCDKey(char* CDKeyStr);
//extern	int VerifyClientCheck(const char *key, unsigned short cskey);

bool CMainMenu::IsCDKeyIsValid()
{
	if (!m_pGameSpyFull->m_pGS_HTTP) return false;
	string64 CDKey = "";
	GetCDKey(CDKey);
	int GameID = 0;
	for (int i=0; i<4; i++)
	{
		m_pGameSpyFull->m_pGS_HTTP->xrGS_GetGameID(&GameID, i);
		if (VerifyClientCheck(CDKey, unsigned short (GameID)) == 1)
			return true;
	};	
	return false;
}

bool		CMainMenu::ValidateCDKey					()
{
	if (IsCDKeyIsValid()) return true;
	SetErrorDialog(CMainMenu::ErrCDKeyInvalid);
//.	CheckForErrorDlg();
	return false;
}

void		CMainMenu::Show_CTMS_Dialog				()
{
	if (!m_pMSB_ConnectToMasterServer) return;
	if (m_pMSB_ConnectToMasterServer->IsShown()) return;
	StartStopMenu(m_pMSB_ConnectToMasterServer, false);
}
void		CMainMenu::Hide_CTMS_Dialog				()
{
	if (!m_pMSB_ConnectToMasterServer) return;
	if (!m_pMSB_ConnectToMasterServer->IsShown()) return;
	StartStopMenu(m_pMSB_ConnectToMasterServer, false);
}