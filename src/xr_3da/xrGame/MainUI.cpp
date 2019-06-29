#include "stdafx.h"
#include "MainUI.h"
#include "UI/UIDialogWnd.h"
#include "ui/UIMessageBoxEx.h"
#include "UICursor.h"
#include "HUDManager.h"
#include "../xr_IOConsole.h"
#include "../IGame_Level.h"
#include "../CameraManager.h"
#include "xr_Level_controller.h"
#include "ui\UITextureMaster.h"
#include "ui\UIXmlInit.h"
#include <dinput.h>
#include "ui\UIBtnHint.h"

extern CUICursor*	GetUICursor(){return UI()->GetUICursor();};
extern CMainUI*		UI(){return (CMainUI*)(g_pGamePersistent->m_pMainUI);};

//----------------------------------------------------------------------------------
void S2DVert::rotate_pt(const Fvector2& pivot, float cosA, float sinA)
{
	Fvector2 t		= pt;
	t.sub			(pivot);
	pt.x			= t.x*cosA+t.y*sinA;
	pt.y			= t.y*cosA-t.x*sinA;
	pt.add			(pivot);
}
void C2DFrustum::CreateFromRect	(const Frect& rect)
{
	m_rect.set(float(rect.x1), float(rect.y1), float(rect.x2), float(rect.y2) );
	planes.resize	(4);
	planes[0].build	(rect.lt, Fvector2().set(-1, 0));
	planes[1].build	(rect.lt, Fvector2().set( 0,-1));
	planes[2].build	(rect.rb, Fvector2().set(+1, 0));
	planes[3].build	(rect.rb, Fvector2().set( 0,+1));
}

sPoly2D* C2DFrustum::ClipPoly	(sPoly2D& S, sPoly2D& D) const
{
	bool bFullTest		= false;
	for (u32 j=0; j<S.size(); j++)
	{
		if( !m_rect.in(S[j].pt) ) {
			bFullTest	= true;
			break		;
		}
	}

	sPoly2D*	src		= &D;
	sPoly2D*	dest	= &S;
	if(!bFullTest)		return dest;

	for (u32 i=0; i<planes.size(); i++)
	{
		// cache plane and swap lists
		const Fplane2 &P	= planes[i]	;
		std::swap			(src,dest)	;
		dest->clear			()			;

		// classify all points relative to plane #i
		float cls[UI_FRUSTUM_SAFE]	;
		for (u32 j=0; j<src->size(); j++) cls[j]=P.classify((*src)[j].pt);

		// clip everything to this plane
		cls[src->size()] = cls[0]	;
		src->push_back((*src)[0])	;
		Fvector2 dir_pt,dir_uv;		float denum,t;
		for (j=0; j<src->size()-1; j++)	{
			if ((*src)[j].pt.similar((*src)[j+1].pt,EPS_S)) continue;
			if (negative(cls[j]))	{
				dest->push_back((*src)[j])	;
				if (positive(cls[j+1]))	{
					// segment intersects plane
					dir_pt.sub((*src)[j+1].pt,(*src)[j].pt);
					dir_uv.sub((*src)[j+1].uv,(*src)[j].uv);
					denum = P.n.dotproduct(dir_pt);
					if (denum!=0) {
						t = -cls[j]/denum	; //VERIFY(t<=1.f && t>=0);
						dest->last().pt.mad	((*src)[j].pt,dir_pt,t);
						dest->last().uv.mad	((*src)[j].uv,dir_uv,t);
						dest->inc();
					}
				}
			} else {
				// J - outside
				if (negative(cls[j+1]))	{
					// J+1  - inside
					// segment intersects plane
					dir_pt.sub((*src)[j+1].pt,(*src)[j].pt);
					dir_uv.sub((*src)[j+1].uv,(*src)[j].uv);
					denum = P.n.dotproduct(dir_pt);
					if (denum!=0)	{
						t = -cls[j]/denum	; //VERIFY(t<=1.f && t>=0);
						dest->last().pt.mad	((*src)[j].pt,dir_pt,t);
						dest->last().uv.mad	((*src)[j].uv,dir_uv,t);
						dest->inc();
					}
				}
			}
		}

		// here we end up with complete polygon in 'dest' which is inside plane #i
		if (dest->size()<3) return 0;
	}
	return dest;
}

//----------------------------------------------------------------------------------
CMainUI::CMainUI	()
{
	m_Flags.zero				();
	m_startDialog				= NULL;
	m_pUICursor					= xr_new<CUICursor>();
	m_pFontManager				= xr_new<CFontManager>();
	g_pGamePersistent->m_pMainUI= this;
	if (Device.bReady)			OnDeviceCreate();  	
	ReadTextureInfo				();
	CUIXmlInit::InitColorDefs	();
	g_btnHint					= xr_new<CUIButtonHint>();
	m_bPostprocess				= false;

	m_pMessageBox = NULL;
	//m_pMessageInvalidPass = NULL;
	//m_pMessageSessionFull = NULL;
}

CMainUI::~CMainUI	()
{
	xr_delete						(g_btnHint);
	xr_delete						(m_startDialog);
	xr_delete						(m_pFontManager);
	xr_delete						(m_pUICursor);
	xr_delete						(m_pMessageBox);
	//xr_delete						(m_pMessageInvalidPass);
	//xr_delete						(m_pMessageSessionFull);
	g_pGamePersistent->m_pMainUI	= NULL;
	CUITextureMaster::WriteLog		();
}

void CMainUI::ReadTextureInfo(){
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

void CMainUI::Activate	(bool bActivate)
{
	if(!!m_Flags.is(flActive) == bActivate)	return;
		
	if(bActivate){
		m_Flags.set					(flActive|flNeedChangeCapture,TRUE);
		DLL_Pure* dlg = NEW_INSTANCE (TEXT2CLSID("MAIN_MNU"));
		if(!dlg) {
			m_Flags.set				(flActive|flNeedChangeCapture,FALSE);
			return;
		}
		xr_delete(m_startDialog);
		m_startDialog = smart_cast<CUIDialogWnd*>(dlg);
		VERIFY(m_startDialog);

		m_Flags.set					(flRestoreConsole,Console->bVisible);
		m_Flags.set					(flRestorePause,Device.Pause());
		Console->Hide				();

		m_Flags.set					(flRestoreCursor,GetUICursor()->IsVisible());

		if(!m_Flags.is(flRestorePause))
			Device.Pause			(TRUE);
		::Sound->set_volume			(1.0f);// pause set to 0

		StartStopMenu				(m_startDialog,true);
		if(g_pGameLevel){
			Device.seqFrame.Remove	(g_pGameLevel);
			Device.seqRender.Remove	(g_pGameLevel);
			CCameraManager::ResetPP	();
		};
		Device.seqRender.Add		(this, 3); // 1-console 2-cursor
	}else{
		m_Flags.set					(flActive,				FALSE);
		m_Flags.set					(flNeedChangeCapture,	TRUE);

		Device.seqRender.Remove		(this);
		
		bool b = !!Console->bVisible;
		if(b){
			Console->Hide			();
		}

		IR_Release					();
		if(b){
			Console->Show			();
		}

		CleanInternals				();
		if(g_pGameLevel){
			Device.seqFrame.Add		(g_pGameLevel);
			Device.seqRender.Add	(g_pGameLevel);
		};
		if(m_Flags.is(flRestoreConsole))
			Console->Show			();

		if(!m_Flags.is(flRestorePause))
			Device.Pause(FALSE);
	
		if(m_Flags.is(flRestoreCursor))
			GetUICursor()->Show();
	}
}
bool CMainUI::IsActive	()
{
	return !!m_Flags.is(flActive);
}


//IInputReceiver
static int mouse_button_2_key []	=	{MOUSE_1,MOUSE_2,MOUSE_3};
void	CMainUI::IR_OnMousePress				(int btn)	
{	
	if(!IsActive()) return;

	IR_OnKeyboardPress(mouse_button_2_key[btn]);
};

void	CMainUI::IR_OnMouseRelease				(int btn)	
{
	if(!IsActive()) return;
	IR_OnKeyboardRelease(mouse_button_2_key[btn]);
};
void	CMainUI::IR_OnMouseHold					(int btn)	
{
	if(!IsActive()) return;
	IR_OnKeyboardHold(mouse_button_2_key[btn]);
};

void	CMainUI::IR_OnMouseMove					(int x, int y)
{
	if(!IsActive()) return;
		MainInputReceiver()->IR_OnMouseMove(x, y);
};

void	CMainUI::IR_OnMouseStop					(int x, int y)
{
	if(!IsActive()) return;
};

void	CMainUI::IR_OnKeyboardPress				(int dik)
{
	if(!IsActive()) return;

	if(key_binding[dik]== kCONSOLE){
		Console->Show();
		return;
	}
	if (DIK_F12 == dik){
		Render->Screenshot();
		return;
	}


	MainInputReceiver()->IR_OnKeyboardPress( dik);
};

void	CMainUI::IR_OnKeyboardRelease			(int dik)
{
	if(!IsActive()) return;
	MainInputReceiver()->IR_OnKeyboardRelease(dik);
};

void	CMainUI::IR_OnKeyboardHold				(int dik)	
{
	if(!IsActive()) return;
};

void CMainUI::IR_OnMouseWheel(int direction)
{
	if(!IsActive()) return;
	MainInputReceiver()->IR_OnMouseWheel(direction);
}


bool CMainUI::OnRenderPPUI_query()
{
	if (!m_Flags.is(flWpnScopeDraw))
		return true;
	return IsActive() && !m_Flags.is(flGameSaveScreenshot);
}


void CMainUI::OnRender	()
{
	if(m_Flags.is(flGameSaveScreenshot))
		return;

	Render->Render				();
}

void CMainUI::SetWnpScopeDraw(bool draw){
	m_Flags.set(flWpnScopeDraw, draw);
}

void CMainUI::OnRenderPPUI_main	()
{
	if (!m_Flags.is(flWpnScopeDraw)){
		if(!IsActive()) return;

		if(m_Flags.is(flGameSaveScreenshot)){
			return;
		};
	}

	m_bPostprocess = true;

	m_2DFrustum2.CreateFromRect	(Frect().set(	0.0f,
												0.0f,
												ClientToScreenScaledX(UI_BASE_WIDTH),
												ClientToScreenScaledY(UI_BASE_HEIGHT)
												));

	DoRenderDialogs();

	m_pFontManager->Render();
	m_bPostprocess = false;
}

void CMainUI::OnRenderPPUI_PP	()
{
	if (!m_Flags.is(flWpnScopeDraw)){
		if(!IsActive()) 
			return;
	}
	m_bPostprocess = true;
	
	xr_vector<CUIWindow*>::iterator it = m_pp_draw_wnds.begin();
	for(; it!=m_pp_draw_wnds.end();++it){
			(*it)->Draw();
	}
	m_bPostprocess = false;
}

//pureFrame
void	CMainUI::OnFrame		(void)
{
	m_2DFrustum.CreateFromRect	(Frect().set(	0.0f,
												0.0f,
												ClientToScreenScaledX(UI_BASE_WIDTH),
												ClientToScreenScaledY(UI_BASE_HEIGHT)
												));

	if(!IsActive() && m_startDialog){
		xr_delete					(m_startDialog);
	}
	if (m_Flags.is(flNeedChangeCapture)){
		m_Flags.set					(flNeedChangeCapture,FALSE);
		if (m_Flags.is(flActive))	IR_Capture();
		else						IR_Release();
	}
	CDialogHolder::OnFrame		();


	//screenshot stuff
	if(m_Flags.is(flGameSaveScreenshot) && Device.dwFrame > m_screenshotFrame  ){
		m_Flags.set					(flGameSaveScreenshot,FALSE);
		::Render->Screenshot		(IRender_interface::SM_FOR_GAMESAVE, m_screenshot_name);
		
		if(g_pGameLevel && m_Flags.is(flActive)){
			Device.seqFrame.Remove	(g_pGameLevel);
			Device.seqRender.Remove	(g_pGameLevel);
		};

		if(m_Flags.is(flRestoreConsole))
			Console->Show			();
	}

}

void CMainUI::OnDeviceCreate()
{
}

void CMainUI::ClientToScreenScaled(Fvector2& dest, float left, float top)
{
	dest.set(ClientToScreenScaledX(left),	ClientToScreenScaledY(top));
}

float CMainUI::ClientToScreenScaledX(float left)
{
	return left * GetScaleX();
}

float CMainUI::ClientToScreenScaledY(float top)
{
	return top * GetScaleY();
}

void CMainUI::OutText(CGameFont *pFont, Frect r, float x, float y, LPCSTR fmt, ...)
{
	if (r.in(x, y))
	{
		R_ASSERT(pFont);
		va_list	lst;
		static string512 buf;
		::ZeroMemory(buf, 512);
		xr_string str;

		va_start(lst, fmt);
		vsprintf(buf, fmt, lst);
		str += buf;
		va_end(lst);

		// Rescale position in lower resolution
		if (x >= 1.0f && y >= 1.0f)
		{
			x *= GetScaleX();
			y *= GetScaleY();
		}

		pFont->Out(x, y, "%s", str.c_str());
	}
}

Frect CMainUI::ScreenRect()
{
	static Frect R={0.0f, 0.0f, UI_BASE_WIDTH, UI_BASE_HEIGHT};
	return R;
}

void CMainUI::PushScissor(const Frect& r_tgt, bool overlapped)
{
//	return;
	Frect r_top			= ScreenRect();
	Frect result		= r_tgt;
	if (!m_Scissors.empty()&&!overlapped){
		r_top			= m_Scissors.top();
	}
	if (!result.intersection(r_top,r_tgt))
			result.set	(0.0f,0.0f,0.0f,0.0f);

	VERIFY(result.x1>=0&&result.y1>=0&&result.x2<=UI_BASE_WIDTH&&result.y2<=UI_BASE_HEIGHT);
	m_Scissors.push		(result);

	result.lt.x 		= ClientToScreenScaledX(result.lt.x);
	result.lt.y 		= ClientToScreenScaledY(result.lt.y);
	result.rb.x 		= ClientToScreenScaledX(result.rb.x);
	result.rb.y 		= ClientToScreenScaledY(result.rb.y);

	Irect r;
	r.x1 = iFloor(result.x1);
	r.x2 = iFloor(result.x2+0.5f);
	r.y1 = iFloor(result.y1);
	r.y2 = iFloor(result.y2+0.5f);
	VERIFY(r.x1>=0&&r.y1>=0&&(r.x2<=UI_BASE_WIDTH*GetScaleX())&&(r.y2<=UI_BASE_HEIGHT*GetScaleY()));
	RCache.set_Scissor	(&r);
}

void CMainUI::PopScissor()
{
//	return;
	VERIFY(!m_Scissors.empty());
	m_Scissors.pop		();
	
	if(m_Scissors.empty())
		RCache.set_Scissor(NULL);
	else{
		const Frect& top= m_Scissors.top();
		Irect tgt;
		tgt.lt.x 		= iFloor(ClientToScreenScaledX(top.lt.x));
		tgt.lt.y 		= iFloor(ClientToScreenScaledY(top.lt.y));
		tgt.rb.x 		= iFloor(ClientToScreenScaledX(top.rb.x));
		tgt.rb.y 		= iFloor(ClientToScreenScaledY(top.rb.y));

		RCache.set_Scissor(&tgt);
	}
}

void CMainUI::Screenshot						(IRender_interface::ScreenshotMode mode, LPCSTR name)
{
	if(mode != IRender_interface::SM_FOR_GAMESAVE){
		::Render->Screenshot		(mode,name);
	}else{
		m_Flags.set					(flGameSaveScreenshot, TRUE);
		strcpy(m_screenshot_name,name);
		if(g_pGameLevel && m_Flags.is(flActive)){
			Device.seqFrame.Add		(g_pGameLevel);
			Device.seqRender.Add	(g_pGameLevel);
		};
		m_screenshotFrame			= Device.dwFrame+1;
		m_Flags.set					(flRestoreConsole,		Console->bVisible);
		Console->Hide				();
	}
}

void CMainUI::RegisterPPDraw					(CUIWindow* w)
{
	UnregisterPPDraw				(w);
	m_pp_draw_wnds.push_back		(w);
}

void CMainUI::UnregisterPPDraw				(CUIWindow* w)
{
	xr_vector<CUIWindow*>::iterator it = remove( m_pp_draw_wnds.begin(), m_pp_draw_wnds.end(), w);
	m_pp_draw_wnds.erase(it, m_pp_draw_wnds.end());
}

void CMainUI::OnInvalidHost(){
	if (!m_pMessageBox)
	{
        m_pMessageBox = xr_new<CUIMessageBoxEx>();		
	}

	m_pMessageBox->Init("message_box_invalid_host");
	StartStopMenu(m_pMessageBox, false);
}

void CMainUI::OnInvalidPass(){
	if (!m_pMessageBox)
	{
        m_pMessageBox = xr_new<CUIMessageBoxEx>();
	}

	m_pMessageBox->Init("message_box_invalid_pass");
	StartStopMenu(m_pMessageBox, false);
}

void CMainUI::OnSessionFull(){
	if (!m_pMessageBox)
	{
        m_pMessageBox = xr_new<CUIMessageBoxEx>();
	}

	m_pMessageBox->Init("message_box_session_full");
	StartStopMenu(m_pMessageBox, false);
}

void CMainUI::OnServerReject(){
	if (!m_pMessageBox)
	{
        m_pMessageBox = xr_new<CUIMessageBoxEx>();
	}

	m_pMessageBox->Init("message_box_server_reject");
	StartStopMenu(m_pMessageBox, false);	
}