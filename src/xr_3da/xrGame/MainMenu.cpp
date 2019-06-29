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


extern CMainMenu*	MainMenu()	{return (CMainMenu*)g_pGamePersistent->m_pMainMenu; };
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
CMainMenu::CMainMenu	()
{
	m_Flags.zero				();
	m_startDialog				= NULL;
	g_pGamePersistent->m_pMainMenu= this;
	if (Device.bReady)			OnDeviceCreate();  	
	ReadTextureInfo				();
	CUIXmlInit::InitColorDefs	();
	g_btnHint					= xr_new<CUIButtonHint>();

	m_pMessageBox				= NULL;
	m_deactivated_frame			= 0;
}

CMainMenu::~CMainMenu	()
{
	xr_delete						(g_btnHint);
	xr_delete						(m_startDialog);
	xr_delete						(m_pMessageBox);
	g_pGamePersistent->m_pMainMenu	= NULL;

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

extern ENGINE_API BOOL bShowPauseString;
extern bool	IsGameTypeSingle();

void CMainMenu::Activate	(bool bActivate)
{
	if(!!m_Flags.test(flActive) == bActivate)	return;
	
	bool b_is_single		= IsGameTypeSingle();

	if(bActivate)
	{
		Device.PauseSound			(TRUE);
		m_Flags.set					(flActive|flNeedChangeCapture,TRUE);

//.		if(!m_startDialog)
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
		
		if(b_is_single)	m_Flags.set	(flRestorePause,Device.Pause());
		
		Console->Hide				();

		m_Flags.set					(flRestoreCursor,GetUICursor()->IsVisible());

		if(b_is_single)
		{
			m_Flags.set					(flRestorePauseStr, bShowPauseString);
			bShowPauseString			= FALSE;
			if(!m_Flags.test(flRestorePause))
				Device.Pause			(TRUE);
		}

		m_startDialog->m_bWorkInPause		= true;
		StartStopMenu						(m_startDialog,true);
		
		if(g_pGameLevel)
		{
			if(b_is_single)
				Device.seqFrame.Remove		(g_pGameLevel);
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
//.		xr_delete							(m_startDialog);
		if(g_pGameLevel)
		{
			if(b_is_single)
				Device.seqFrame.Add			(g_pGameLevel);
			Device.seqRender.Add			(g_pGameLevel);
		};
		if(m_Flags.test(flRestoreConsole))
			Console->Show			();

		if(b_is_single)
		{
			if(!m_Flags.test(flRestorePause))
				Device.Pause			(FALSE);

			bShowPauseString			= m_Flags.test(flRestorePauseStr);
		}	

	
		if(m_Flags.test(flRestoreCursor))
			GetUICursor()->Show			();

		Device.PauseSound				(FALSE);
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

	if(key_binding[dik]== kCONSOLE)
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

void	CMainMenu::IR_OnKeyboardHold				(int dik)	
{
};

void CMainMenu::IR_OnMouseWheel(int direction)
{
	if(!IsActive()) return;
	
	if(MainInputReceiver())
		MainInputReceiver()->IR_OnMouseWheel(direction);
}


bool CMainMenu::OnRenderPPUI_query()
{
	return IsActive() && !m_Flags.test(flGameSaveScreenshot);
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
	if(!IsActive() && m_startDialog)
	{
//.		xr_delete					(m_startDialog);
	}
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

void CMainMenu::OnInvalidHost()
{
	if (!m_pMessageBox)
	{
        m_pMessageBox = xr_new<CUIMessageBoxEx>();		
	}

	m_pMessageBox->Init("message_box_invalid_host");
	StartStopMenu(m_pMessageBox, false);
}

void CMainMenu::OnInvalidPass()
{
	if (!m_pMessageBox)
	{
        m_pMessageBox = xr_new<CUIMessageBoxEx>();
	}

	m_pMessageBox->Init("message_box_invalid_pass");
	StartStopMenu(m_pMessageBox, false);
}

void CMainMenu::OnSessionFull()
{
	if (!m_pMessageBox)
	{
        m_pMessageBox = xr_new<CUIMessageBoxEx>();
	}

	m_pMessageBox->Init("message_box_session_full");
	StartStopMenu(m_pMessageBox, false);
}

void CMainMenu::OnServerReject()
{
	if (!m_pMessageBox)
	{
        m_pMessageBox = xr_new<CUIMessageBoxEx>();
	}

	m_pMessageBox->Init("message_box_server_reject");
	StartStopMenu(m_pMessageBox, false);	
}

void CMainMenu::DestroyInternal()
{
	if(m_startDialog && m_deactivated_frame < Device.dwFrame+4)
		xr_delete		(m_startDialog);
}
