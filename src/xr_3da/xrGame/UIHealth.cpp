#include "stdafx.h"
#include "uihealth.h"
#include "hudmanager.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define S_COLOR 0xffEF0023
#define M_COLOR 0xffFFF200
#define E_COLOR 0xff00AEEF

CUIHealth::CUIHealth()
{
	s_color.set		(S_COLOR);
	m_color.set		(M_COLOR);
	e_color.set		(E_COLOR);
}
//--------------------------------------------------------------------

void CUIHealth::Init(){
	back.Init		("ui\\hud_health_back",			"hud\\default",15,525,alLeft|alBottom);
	health_bar.Init	("ui\\hud_health_bar_health",	"hud\\default",18,539,alLeft|alBottom);
	armor_bar.Init	("ui\\hud_health_bar_armor",	"hud\\default",18,556,alLeft|alBottom);
	back.SetRect	(0,0,90,61);
	health_rect.set	(0,0,84,16);
	armor_rect.set	(0,0,84,16);
}
//--------------------------------------------------------------------

void CUIHealth::Out	(float _health, float _armor)
{
	health = _health;
	armor = _armor;

	float val;
	Fcolor C;
	// health bar
	val = health/100.f; clamp(val,0.f,1.f);
	C.lerp(s_color,m_color,e_color,val);
	health_bar.SetRect	(health_rect.x1,health_rect.y1,health_rect.x2*val,health_rect.y2);
	health_bar.SetColor	(C);
	// armor bar
	val = armor/100.f;	clamp(val,0.f,1.f);
	C.lerp(s_color,m_color,e_color,val);
	armor_bar.SetRect	(armor_rect.x1,armor_rect.y1,armor_rect.x2*val,armor_rect.y2);
	armor_bar.SetColor	(C);
}
//--------------------------------------------------------------------
void CUIHealth::Render	()
{
	back.Render			();
	health_bar.Render	();
	armor_bar.Render	();
}
//--------------------------------------------------------------------
