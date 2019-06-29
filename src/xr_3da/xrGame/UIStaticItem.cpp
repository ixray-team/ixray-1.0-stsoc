// exxZERO Time Stamp AddIn. Document modified at : Thursday, March 07, 2002 14:12:36 , by user : Oles , from computer : OLES
#include "stdafx.h"
#include "uistaticitem.h"
#include "hudmanager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CUIStaticItem::CUIStaticItem()
{    
	dwColor			= 0xffffffff;
	iTileX			= 1;
	iTileY			= 1;
	iRemX			= 0.0f;
	iRemY			= 0.0f;
	alpha_ref		= -1;
	hShader			= NULL;
#ifdef DEBUG
	dbg_tex_name = NULL;
#endif
//	hGeom_list.create	(FVF::F_TL, RCache.Vertex.Buffer(), RCache.QuadIB);
	hGeom_fan.create	(FVF::F_TL, RCache.Vertex.Buffer(), 0);
}
//--------------------------------------------------------------------
CUIStaticItem::~CUIStaticItem()
{
}
//--------------------------------------------------------------------

void CUIStaticItem::CreateShader(LPCSTR tex, LPCSTR sh)
{
//	CTimer T;
//	T.Start();
//	Msg("----hShader.create-begin");
	hShader.create(sh,tex);
//	Msg("----hShader.create[%d]",T.GetElapsed_ms());

#ifdef DEBUG
	dbg_tex_name = tex;
#endif
	uFlags &= !flValidRect;
}
//--------------------------------------------------------------------

void CUIStaticItem::SetShader(const ref_shader& sh)
{
//	hShader.destroy();
	hShader = sh;
}
//--------------------------------------------------------------------

void CUIStaticItem::Init(LPCSTR tex, LPCSTR sh, float left, float top, u32 align)
{
	//by Dandy
	//used for reinitializing StaticItems during the game
//	hShader.destroy();

	uFlags &= !flValidRect;

	CreateShader	(tex,sh);
	SetPos			(left,top);
	SetAlign		(align);


}
//--------------------------------------------------------------------

void CUIStaticItem::Render		(const ref_shader& sh)
{
	VERIFY(g_bRendering);
	// установить обязательно перед вызовом CustomItem::Render() !!!
	VERIFY(sh?sh:hShader);
	RCache.set_Shader			(sh?sh:hShader);
	if(alpha_ref!=-1)
		CHK_DX(HW.pDevice->SetRenderState(D3DRS_ALPHAREF,alpha_ref));
	// convert&set pos
	Fvector2		bp;
	UI()->ClientToScreenScaled	(bp,float(iPos.x),float(iPos.y));

	// actual rendering
	u32							vOffset;
	Fvector2					pos;
	Fvector2					f_len;
	UI()->ClientToScreenScaled	(f_len, iVisRect.x2, iVisRect.y2 );

	int tile_x					= fis_zero(iRemX)?iTileX:iTileX+1;
	int tile_y					= fis_zero(iRemY)?iTileY:iTileY+1;
	int							x,y;
	if (!(tile_x&&tile_y))		return;
	// render
	FVF::TL* start_pv			= (FVF::TL*)RCache.Vertex.Lock	(8*tile_x*tile_y,hGeom_fan.stride(),vOffset);
	FVF::TL* pv					= start_pv;
	for (x=0; x<tile_x; ++x){
		for (y=0; y<tile_y; ++y){
			pos.set				(bp.x+f_len.x*x,bp.y+f_len.y*y);
			inherited::Render	(pv,pos,dwColor);
		}
	}
	std::ptrdiff_t p_cnt		= (pv-start_pv)/3;						VERIFY((pv-start_pv)<=8*tile_x*tile_y);
	RCache.Vertex.Unlock		(u32(pv-start_pv),hGeom_fan.stride());
	// set scissor
	Frect clip_rect				= {iPos.x,iPos.y,iPos.x+iVisRect.x2*iTileX+iRemX,iPos.y+iVisRect.y2*iTileY+iRemY};
	UI()->PushScissor			(clip_rect);
	// set geom
	RCache.set_Geometry			(hGeom_fan);
	if (p_cnt!=0)RCache.Render	(D3DPT_TRIANGLELIST,vOffset,u32(p_cnt));
	if(alpha_ref!=-1)
		CHK_DX(HW.pDevice->SetRenderState(D3DRS_ALPHAREF,0));
	UI()->PopScissor			();
}
//--------------------------------------------------------------------

void CUIStaticItem::Render(float angle, const ref_shader& sh)
{
	VERIFY(g_bRendering);
	// установить обязательно перед вызовом CustomItem::Render() !!!
	VERIFY(sh?sh:hShader);
	RCache.set_Shader			(sh?sh:hShader);
	if(alpha_ref!=-1)
		CHK_DX(HW.pDevice->SetRenderState(D3DRS_ALPHAREF,alpha_ref));
	// convert&set pos
	Fvector2 bp;
	UI()->ClientToScreenScaled	(bp, iPos.x, iPos.y);
	// actual rendering
	u32		vOffset;
	FVF::TL* start_pv			= (FVF::TL*)RCache.Vertex.Lock	(32,hGeom_fan.stride(),vOffset);
	FVF::TL* pv					= start_pv;
	inherited::Render			(pv,bp,dwColor,angle);
	// unlock VB and Render it as triangle LIST
	std::ptrdiff_t p_cnt		= pv-start_pv;
	RCache.Vertex.Unlock		(u32(p_cnt),hGeom_fan.stride());
	RCache.set_Geometry	 		(hGeom_fan);
	if (p_cnt>2) RCache.Render	(D3DPT_TRIANGLEFAN,vOffset,u32(p_cnt-2));
	if(alpha_ref!=-1)
		CHK_DX(HW.pDevice->SetRenderState(D3DRS_ALPHAREF,0));
}
//--------------------------------------------------------------------
