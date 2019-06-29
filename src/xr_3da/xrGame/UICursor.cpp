// exxZERO Time Stamp AddIn. Document modified at : Thursday, March 07, 2002 14:11:58 , by user : Oles , from computer : OLES
#include "stdafx.h"
#include "uicursor.h"

#include "../CustomHUD.h"
#include "HUDManager.h"
#include "UI.h"

#define C_DEFAULT	D3DCOLOR_XRGB(0xff,0xff,0xff)

#define SENSITIVITY_DEFAULT 1.5f

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CUICursor::CUICursor()
{    
	m_fSensitivity	= SENSITIVITY_DEFAULT;
	bHoldMode		= false;
	bVisible		= false;
	vPos.set		(0.f,0.f);
	vHoldPos.set	(0,0);

	hShader.create	("hud\\cursor","ui\\ui_ani_cursor");
	m_si.SetShader(hShader);
	Device.seqRender.Add	(this,2);
}
//--------------------------------------------------------------------
CUICursor::~CUICursor	()
{
	Device.seqRender.Remove	(this);
}
//--------------------------------------------------------------------
u32 last_render_frame = 0;
void CUICursor::OnRender	()
{
	if( !IsVisible() ) return;
#ifdef DEBUG
	VERIFY(last_render_frame != Device.dwFrame);
	last_render_frame = Device.dwFrame;

	if(bDebug){
	CGameFont* F		= UI()->Font()->pFontDI;
	F->SetAligment		(CGameFont::alCenter);
	F->SetSizeI			(0.02f);
	F->OutSetI			(0.f,-0.9f);
	F->SetColor			(0xffffffff);
	Fvector2			pt = GetPos();
	F->OutNext			("%f-%f",pt.x, pt.y);

	if(bHoldMode)
		F->OutNext		("Hold Mode");
	};
#endif
	if(bHoldMode) return;
	// Convert to screen coords
	float cx	= (vPos.x+1)/2.0f*UI_BASE_WIDTH;
	float cy	= (vPos.y+1)/2.0f*UI_BASE_HEIGHT;

	m_si.SetPos(cx, cy);
	m_si.Render();
}
//--------------------------------------------------------------------

//move cursor to screen coordinates
void CUICursor::SetPos(float x, float y)
{
	vPos.x =2.0f*x/UI_BASE_WIDTH - 1.0f;
	vPos.y =2.0f*y/UI_BASE_HEIGHT - 1.0f;

	if(vPos.x<-1) vPos.x=-1;
	if(vPos.x>1) vPos.x=1;
	if(vPos.y<-1) vPos.y=-1;
	if(vPos.y>1) vPos.y=1;
}

void CUICursor::GetPos(float& x, float& y)
{
	x = (vPos.x+1.0f)*UI_BASE_WIDTH/2.0f;
	y = (vPos.y+1.0f)*UI_BASE_HEIGHT/2.0f;

}

Fvector2 CUICursor::GetPos()
{
	Fvector2 pt;

	GetPos(pt.x, pt.y);

	return  pt;
}

float DI2DD(float val, float base)
{
	return (val+1.0f)*base/2.0f;
}

Fvector2 CUICursor::GetPosDelta()
{
	Fvector2 res_delta;
	res_delta.x = DI2DD(vPos.x,UI_BASE_WIDTH)-DI2DD(vPrevPos.x,UI_BASE_WIDTH);
	res_delta.y = DI2DD(vPos.y,UI_BASE_HEIGHT)-DI2DD(vPrevPos.y,UI_BASE_HEIGHT);

	return res_delta;
}

void CUICursor::HoldMode		(bool b)
{
	if(bHoldMode==b) return;
	if(b){
		vHoldPos = vPos;
	}else{
		vPos = vHoldPos;
	}
	bHoldMode = b;
}

void CUICursor::MoveBy(float dx, float dy)
{
	vPrevPos = vPos;
	Fvector2 vDelta;
	vDelta.x = m_fSensitivity * dx / Device.dwWidth;
	vDelta.y = m_fSensitivity * dy / Device.dwHeight;

	if(!bHoldMode){
		vPos.x += vDelta.x;
		vPos.y += vDelta.y;
		clamp(vPos.x, -1.f, 1.f);
		clamp(vPos.y, -1.f, 1.f);
	}else{
		vPrevPos.x -= vDelta.x;
		vPrevPos.y -= vDelta.y;
		clamp(vPrevPos.x, -1.f, 1.f);
		clamp(vPrevPos.y, -1.f, 1.f);
	}
	


	if(!m_cursor_move_event.empty())
		m_cursor_move_event(GetPosDelta());
}
