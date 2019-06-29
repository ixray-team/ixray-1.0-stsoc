#include "stdafx.h"
#include "ai_monster_debug.h"
#include "../../entity_alive.h"
#include "../../hudmanager.h"
#include "../../level.h"

CMonsterDebug::CMonsterDebug(CEntityAlive *pM, const Fvector &shift_vec, float height)
{
	pMonster		= pM;
	floor_height	= height;
	shift			= shift_vec;
	active			= false;
}
CMonsterDebug::~CMonsterDebug()
{
	
}
void CMonsterDebug::M_Clear()
{
	_data.clear();
}
void CMonsterDebug::M_Add(u32 index, LPCSTR str, u32 col)
{
	if (_data.size() < index) index = _data.size();

	_elem new_elem;
	strcpy(new_elem.text, str);
	new_elem.col = col;

	if (index < _data.size()) _data[index] = new_elem;
	else _data.push_back(new_elem);
}

void CMonsterDebug::M_Update()
{ 
#ifdef DEBUG
	if (!active) return;

	Fmatrix res;
	res.mul(Device.mFullTransform,pMonster->XFORM());

	Fvector4 v_res;

	res.transform(v_res,shift);

	if (v_res.z < 0 || v_res.w < 0)	return;
	if (v_res.x < -1.f || v_res.x > 1.f || v_res.y<-1.f || v_res.y>1.f) return;

	float x = (1.f + v_res.x)/2.f * (Device.dwWidth);
	float y = (1.f - v_res.y)/2.f * (Device.dwHeight);

	for (u32 i=0; i<_data.size(); i++) {
		HUD().Font().pFontMedium->SetColor(_data[i].col);
		HUD().Font().pFontMedium->OutSet(x,y-=floor_height);
		HUD().Font().pFontMedium->OutNext(_data[i].text);
	}
#endif
}

//-------------------------------------------------------------------------------------

void CMonsterDebug::L_AddPoint(const Fvector &pos, float box_size, u32 col)
{
	for	(u32 i=0; i<_points.size(); i++) {
		if (_points[i].pos.similar(pos)) return;
	}
	
	_elem_point new_point;

	new_point.pos		= pos;
	new_point.col		= col;
	new_point.box_size	= box_size;

	_points.push_back(new_point);
}

void CMonsterDebug::L_AddLine(const Fvector &pos, const Fvector &pos2, u32 col)
{
	for	(u32 i=0; i<_lines.size(); i++) {
		if (_lines[i].p1.similar(pos) && _lines[i].p2.similar(pos2)) return;
	}

	_elem_line new_line;

	new_line.p1		= pos;
	new_line.p2		= pos2;
	new_line.col	= col;

	_lines.push_back(new_line);
}

void CMonsterDebug::L_Clear()
{
	_points.clear();
	_lines.clear();
}

void CMonsterDebug::L_Update()
{
#ifdef DEBUG
	if (!active) return;

	for (u32 i=0; i<_points.size(); i++) {
		RCache.dbg_DrawAABB(_points[i].pos,_points[i].box_size,_points[i].box_size,_points[i].box_size,_points[i].col);

		Fvector upV;
		upV = _points[i].pos;
		upV.y += 5.0f;

		RCache.dbg_DrawLINE(Fidentity,_points[i].pos,upV,_points[i].col);
	}
	

	for (i=0; i<_lines.size(); i++) {
		RCache.dbg_DrawLINE(Fidentity,_lines[i].p1,_lines[i].p2,_lines[i].col);		
	}
#endif
}

//-------------------------------------------------------------------------------------

void CMonsterDebug::HT_Add(float x, float y, LPCSTR str)
{
	for	(u32 i=0; i<_text.size(); i++) {
		if (xr_strcmp(_text[i].text, str) == 0) return;
	}

	_elem_const new_text;
		
	strcpy(new_text.text, str);
	
	new_text.x	=	x;
	new_text.y	=	y;
	
	_text.push_back(new_text);
}

void CMonsterDebug::HT_Clear()
{
	_text.clear();
}

void CMonsterDebug::HT_Update()
{
#ifdef DEBUG
	if (!active) return;

	for (u32 i=0; i<_text.size(); i++) {
		HUD().Font().pFontMedium->OutSet(_text[i].x,_text[i].y);
		HUD().Font().pFontMedium->OutNext(_text[i].text);
	}
#endif
}

