#include "stdafx.h"
#pragma hdrstop

#include "GameFont.h"
#include "Render.h"
extern ENGINE_API BOOL g_bRendering; 
ENGINE_API Fvector2		g_current_font_scale={1.0f,1.0f};

#define MAX_CHARS	1024
CGameFont::CGameFont(LPCSTR section, u32 flags)
{
	fCurrentHeight				= 0.0f;
	uFlags						= flags;
	Initialize	(pSettings->r_string(section,"shader"),pSettings->r_string(section,"texture"));
	if (pSettings->line_exist(section,"size")){
		float sz = pSettings->r_float(section,"size");
		if (uFlags&fsDeviceIndependent)	SetHeightI(sz);
		else							SetHeight(sz);
	}
	if (pSettings->line_exist(section,"interval"))
		SetInterval(pSettings->r_fvector2(section,"interval"));
}

CGameFont::CGameFont(LPCSTR shader, LPCSTR texture, u32 flags)
{
	fCurrentHeight				= 0.0f;
	uFlags						= flags;
	Initialize					(shader,texture);
}

void CGameFont::Initialize		(LPCSTR cShader, LPCSTR cTextureName)
{
	string_path					cTexture;

	LPCSTR _lang				= pSettings->r_string("string_table", "font_prefix");
	bool is_di					= strstr(cTextureName, "ui_font_hud_01") || 
								  strstr(cTextureName, "ui_font_hud_02") ||
								  strstr(cTextureName, "ui_font_console_02");
	if(_lang && !is_di)
		strconcat				(cTexture, cTextureName, _lang);
	else
		strcpy					(cTexture, cTextureName);

	uFlags						&=~fsValid;
	vTS.set						(1.f,1.f); // обязательно !!!

	eCurrentAlignment			= alLeft;
	vInterval.set				(1.f,1.f);

	for (int i=0; i<256; i++)	CharMap[i] = i;
	strings.reserve				(128);

	// check ini exist
	string256 fn,buf;
	strcpy		(buf,cTexture); if (strext(buf)) *strext(buf)=0;
	R_ASSERT2	(FS.exist(fn,"$game_textures$",buf,".ini"),fn);
	CInifile* ini				= CInifile::Create(fn);
	if (ini->section_exist("symbol_coords")){
		for (int i=0; i<256; i++){
			sprintf					(buf,"%03d",i);
			Fvector v				= ini->r_fvector3("symbol_coords",buf);
			TCMap[i].set			(v.x,v.y,v[2]-v[0]);
		}
		fHeight						= ini->r_float("symbol_coords","height");
	}else{
		if (ini->section_exist("char widths")){
			fHeight					= ini->r_float("char widths","height");
			int cpl					= 16;
			for (int i=0; i<256; i++){
				sprintf				(buf,"%d",i);
				float w				= ini->r_float("char widths",buf);
				TCMap[i].set		((i%cpl)*fHeight,(i/cpl)*fHeight,w);
			}
		}else{
			R_ASSERT(ini->section_exist("font_size"));
			fHeight					= ini->r_float("font_size","height");
			float width				= ini->r_float("font_size","width");
			const int cpl			= ini->r_s32	("font_size","cpl");
			for (int i=0; i<256; i++)
				TCMap[i].set		((i%cpl)*width,(i/cpl)*fHeight,width);
		}
	}
	fCurrentHeight				= fHeight;

	CInifile::Destroy			(ini);

	// Shading
	pShader.create				(cShader,cTexture);
	pGeom.create				(FVF::F_TL, RCache.Vertex.Buffer(), RCache.QuadIB);
}

CGameFont::~CGameFont()
{
	// Shading
	pShader.destroy		();
	pGeom.destroy		();
}

#define DI2PX(x) float(iFloor((x+1)*float(::Render->getTarget()->get_width())*0.5f))
#define DI2PY(y) float(iFloor((y+1)*float(::Render->getTarget()->get_height())*0.5f))

void CGameFont::OutSet			(float x, float y)
{
	fCurrentX=x; 
	fCurrentY=y;
}

void CGameFont::OutSetI			(float x, float y)	
{
	OutSet(DI2PX(x),DI2PY(y));
}

void CGameFont::OnRender()
{
	VERIFY				(g_bRendering);
	if (pShader)		RCache.set_Shader	(pShader);

	if (!(uFlags&fsValid)){
		CTexture* T		= RCache.get_ActiveTexture(0);
		vTS.set			((int)T->get_Width(),(int)T->get_Height());
/*
		vHalfPixel.set	(0.5f/float(vTS.x),0.5f/float(vTS.y));
		for (int i=0; i<256; i++){
			Fvector& tc	= TCMap[i];
			tc.x		/= float(vTS.x);
			tc.y		/= float(vTS.y);
			tc.z		/= float(vTS.x);
		}
		fTCHeight		= fHeight/float(vTS.y);
		uFlags			|= fsValid;
*/
		fTCHeight		= fHeight/float(vTS.y);
		uFlags			|= fsValid;
	}

	for (u32 i=0; i<strings.size(); ){
		// calculate first-fit
		int		count	=	1;
		int		length	=	xr_strlen(strings[i].string);
		while	((i+count)<strings.size()) {
			int	L	=	xr_strlen(strings[i+count].string);
			if ((L+length)<MAX_CHARS){
				count	++;
				length	+=	L;
			}
			else		break;
		}

		// lock AGP memory
		u32	vOffset;
		FVF::TL* v		= (FVF::TL*)RCache.Vertex.Lock	(length*4,pGeom.stride(),vOffset);
		FVF::TL* start	= v;

		// fill vertices
		u32 last		= i+count;
		for (; i<last; i++) {
			String		&PS	= strings[i];
			int			len	= xr_strlen(PS.string);
			if (len) {
				float	X	= float(iFloor(PS.x));
				float	Y	= float(iFloor(PS.y));
				float	S	= PS.height*g_current_font_scale.y;
				float	Y2	= Y+S;

				switch(PS.align)
				{
				case alCenter:	
						X	-= ( iFloor(SizeOf_(PS.string,PS.height)*.5f) ) * g_current_font_scale.x;	
						break;
				case alRight:	
						X	-=	iFloor(SizeOf_(PS.string,PS.height));		
						break;
				}

				u32	clr,clr2;
				clr2 = clr	= PS.c;
				if (uFlags&fsGradient){
					u32	_R	= color_get_R	(clr)/2;
					u32	_G	= color_get_G	(clr)/2;
					u32	_B	= color_get_B	(clr)/2;
					u32	_A	= color_get_A	(clr);
					clr2	= color_rgba	(_R,_G,_B,_A);
				}

				float	tu,tv;
				for (int j=0; j<len; j++)
				{
					int c			= GetCharRM	(PS.string[j]);
					
					Fvector l		= GetCharTC	(PS.string[j]);
					float scw		= l.z * g_current_font_scale.x;
					
//.					float scw		= vTS.x * l.z * g_current_font_scale.x;
					
					float fTCWidth	= l.z/vTS.x;

					if ((c>=0)&&!fis_zero(l.z))
					{
						tu			= l.x/vTS.x;//+vHalfPixel.x;
						tv			= l.y/vTS.y;//+vHalfPixel.y;
						v->set		(X-0.5f,		Y2-0.5f,	clr2,tu,			tv+fTCHeight);	v++;
						v->set		(X-0.5f,		Y-0.5f,		clr, tu,			tv);			v++;
						v->set		(X+scw-0.5f,	Y2-0.5f,	clr2,tu+fTCWidth,	tv+fTCHeight);	v++;
						v->set		(X+scw-0.5f,	Y-0.5f,		clr, tu+fTCWidth,	tv);			v++;
					}
					X+=scw*vInterval.x;
				}
			}
		}

		// Unlock and draw
		u32 vCount = (u32)(v-start);
		RCache.Vertex.Unlock		(vCount,pGeom.stride());
		if (vCount){
			RCache.set_Geometry		(pGeom);
			RCache.Render			(D3DPT_TRIANGLELIST,vOffset,0,vCount,0,vCount/2);
		}
	}
	strings.clear_not_free			();
}

void __cdecl CGameFont::OutI(float _x, float _y, LPCSTR fmt,...)
{
//	if (!Device.bActive)	return;
	String		rs;
	rs.x		=DI2PX(_x);
	rs.y		=DI2PY(_y);
	rs.c		=dwCurrentColor;
	rs.height		=fCurrentHeight;
	rs.align	=eCurrentAlignment;

	va_list		 p;
	va_start	(p,fmt);
	int vs_sz	= _vsnprintf(rs.string,sizeof(rs.string)-1,fmt,p); rs.string[sizeof(rs.string)-1]=0;
	va_end		(p);
	if (vs_sz)	strings.push_back(rs);

}
void __cdecl CGameFont::Out(float _x, float _y, LPCSTR fmt,...)
{
	if (!Device.b_is_Active)	return;
	String		rs;
	rs.x		=_x;
	rs.y		=_y;
	rs.c		=dwCurrentColor;
	rs.height	=fCurrentHeight;
	rs.align	=eCurrentAlignment;

	va_list		 p;
	va_start	(p,fmt);
	int vs_sz	= _vsnprintf(rs.string,sizeof(rs.string)-1,fmt,p); rs.string[sizeof(rs.string)-1]=0;
	va_end		(p);
	if (vs_sz)	strings.push_back(rs);
}

void __cdecl CGameFont::OutNext(LPCSTR fmt,...)
{
	if (!Device.b_is_Active)	return;
	String rs;
	rs.x			=fCurrentX;
	rs.y			=fCurrentY;
	rs.c			=dwCurrentColor;
	rs.height		=fCurrentHeight;
	rs.align		=eCurrentAlignment;

	va_list			p;
	va_start		(p,fmt);
	int vs_sz		= _vsnprintf(rs.string,sizeof(rs.string)-1,fmt,p); rs.string[sizeof(rs.string)-1]=0;
	va_end			(p);

	if (vs_sz)	strings.push_back(rs);
	OutSkip(1);
}

void __cdecl CGameFont::OutPrev(LPCSTR fmt,...)
{
	if (!Device.b_is_Active)	return;
	String rs;
	rs.x			=fCurrentX;
	rs.y			=fCurrentY;
	rs.c			=dwCurrentColor;
	rs.height		=fCurrentHeight;
	rs.align		=eCurrentAlignment;

	va_list		p;
	va_start	(p,fmt);
	int vs_sz	= _vsnprintf(rs.string,sizeof(rs.string)-1,fmt,p); rs.string[sizeof(rs.string)-1]=0;
	va_end		(p);

	if (vs_sz)	strings.push_back(rs);
	OutSkip(-1);
}

void CGameFont::OutSkip(float val)
{
	fCurrentY += val*CurrentHeight_();
}

float CGameFont::SizeOf_(char s,float size)
{
	return		( GetCharTC(s).z*vInterval.x/* *vTS.x*/ );
}

float CGameFont::SizeOf_(LPCSTR s,float size)
{
	if (s&&s[0]){
		int		len			= xr_strlen(s);
		float	X			= 0;
		if (len) 
			for (int j=0; j<len; j++) 
				X			+= GetCharTC(s[j]).z;
		return				(X*vInterval.x/**vTS.x*/);
	}
	return 0;
}

float CGameFont::CurrentHeight_	()
{
	return fCurrentHeight * vInterval.y;
}

void CGameFont::SetHeightI(float S)	
{
	VERIFY			( uFlags&fsDeviceIndependent );
	fCurrentHeight	= S*Device.dwHeight;
};

void CGameFont::SetHeight(float S)	
{
	VERIFY			( uFlags&fsDeviceIndependent );
	fCurrentHeight	= S;
};
