// UIMapSpot.cpp:  пятно на карте, 
// при наводке мыше посылает сообщение родителю
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
/*
#include "UIMapSpot.h"
#include "UIMapBackground.h"

#include "../actor.h"
#include "../level.h"
#include "../xrserver.h"
#include "../xrServer_Objects_ALife.h"
#include "../ai_object_location.h"
#include "../ai_space.h"
#include "../game_graph.h"

//////////////////////////////////////////////////////////////////////////

const char * const	ARROW_TEX			= "ui\\ui_map_arrow_04";
const char * const	MAP_AREA_ANIMATION	= "ui_map_area_anim";
const int			ARROW_DIMENTIONS	= 32;

//////////////////////////////////////////////////////////////////////////

CUIMapSpot::CUIMapSpot()
	:	m_bArrowEnabled		(false),
		m_bArrowVisible		(false)
{
	m_our_level_id			= 0xffff;
	m_object_id				= 0xffff;
	m_eAlign				= eNone;
	m_fHeading				= 0.f;
	m_bHeading				= false;
	arrow_color				= 0xffffffff;
	m_LevelName				= NULL;
	m_vWorldPos.set			(0,0,0);
	m_sDescText.SetText		("");
	m_sNameText.SetText		("");
	ClipperOn				();
	m_Arrow.CreateShader	(ARROW_TEX, "hud\\default");
	m_MapSpotAnimation.SetColorToModify(&GetColorRef());
	m_MapSpotAnimation.SetDone	(true);
	m_MapSpotAnimation.Cyclic	(false);
}

//////////////////////////////////////////////////////////////////////////

CUIMapSpot::~CUIMapSpot()
{
}

//////////////////////////////////////////////////////////////////////////

void CUIMapSpot::Draw()
{
	if(m_bHeading)
	{
		Irect rect = GetAbsoluteRect();
		if(m_bClipper) TextureClipper();
		m_UIStaticItem.SetPos(rect.left, rect.top);
		m_UIStaticItem.Render(m_fHeading);
 	}
	else
		CUIStatic::Draw();

	// Если нужно рисовать стрелочку-указатель
	if (m_bArrowEnabled)
	{
		// Определяем, видимы ли на парентовом окне, и если нет, то рисуем стрелочку-указатель
		CUIWindow *p = GetParent();

		R_ASSERT(p);
		if (!p) return;

		m_bArrowVisible = false;

		Irect parentRect = p->GetAbsoluteRect();
		Irect ourRect	= GetAbsoluteRect(), ourRect2 = GetAbsoluteRect();

		// Clamp x coordinate
		clamp(ourRect.left, parentRect.left - ourRect.right + ourRect.left, parentRect.right);
		// Clamp y coordinate
		clamp(ourRect.top, parentRect.top - ourRect.bottom + ourRect.top, parentRect.bottom);

		// Рисуем стрелку, только если иконка не видна
		if (ourRect.left != ourRect2.left || ourRect2.top != ourRect.top)
		{

			m_bArrowVisible = true;
			float arrowHeading = 0;
			if (ourRect2.top == ourRect.top)
			{
				if (ourRect.left > ourRect2.left)
					arrowHeading = PI + PI_DIV_2;
				else
					arrowHeading = PI_DIV_2;
			}
			else if (ourRect2.left == ourRect.left && ourRect.top > ourRect2.top)
			{
				arrowHeading = PI;
			}
			else
			{
				arrowHeading = std::atan(static_cast<float>(ourRect2.left - ourRect.left) / (ourRect2.top - ourRect.top));
				if (ourRect2.top < ourRect.top) arrowHeading += PI;
			}

			// Some small coordinate fixes
			clamp(ourRect.left, parentRect.left, parentRect.right - ARROW_DIMENTIONS);
			// Clamp y coordinate
			clamp(ourRect.top, parentRect.top, parentRect.bottom - ARROW_DIMENTIONS);

			m_Arrow.SetPos(ourRect.left, ourRect.top);
			m_Arrow.SetColor(arrow_color);
			m_Arrow.Render(arrowHeading);
		}
	}
}

void CUIMapSpot::Update()
{
	CUIStatic::Update();
	m_MapSpotAnimation.Update();
}

Fvector CUIMapSpot::MapPos()
{
	if(0xffff == m_object_id)
		return m_vWorldPos;

	Fvector src = {0,0,0};

	CSE_Abstract* E = Level().Server->game->get_entity_from_eid(m_object_id);
	CSE_ALifeObject* O = NULL;
	if(E) O = smart_cast<CSE_ALifeObject*>(E);
	if(O)
	{
		//объект в оффлайне на нашем уровне
		if(m_our_level_id ==
			ai().game_graph().vertex(O->m_tGraphID)->level_id())
		{
			src.x = O->position().x;
			src.y = 0;
			src.z = O->position().z;
		}
	}


	CObject* pObject =  Level().Objects.net_Find(m_object_id);
	//объект в онлайне и в инвентаре у актера, то ничего не показывать
	if(pObject && pObject->H_Parent() == Level().CurrentEntity())
		Show(false);
	else
		Show(true);
	

		

	return src;
}	

void CUIMapSpot::SetObjectID(u16 id)
{
	m_object_id = id;
	m_our_level_id = ai().game_graph().vertex(smart_cast<CGameObject*>(Level().CurrentEntity())->ai_location().game_vertex_id())->level_id();
}

//////////////////////////////////////////////////////////////////////////

void CUIMapSpot::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	if (!m_MapSpotAnimation.GetAnimation()) return;

	CUIMapBackground *b = smart_cast<CUIMapBackground*>(GetParent());

	if (b && b == pWnd && b->m_pActiveMapSpot == this)
	{
		switch (msg)
		{
		case MAPSPOT_FOCUS_RECEIVED:
			m_MapSpotAnimation.Reverese(false);            
			break;
		case MAPSPOT_FOCUS_LOST:
			m_MapSpotAnimation.Reverese(true);
			break;
		default:
			return;
		}
		if (m_MapSpotAnimation.Done())
                 m_MapSpotAnimation.Reset();
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIMapSpot::DynamicManifestation(bool value)
{
	if (value)
	{
		m_MapSpotAnimation.SetColorAnimation(MAP_AREA_ANIMATION);
		m_MapSpotAnimation.Reverese(true);
		m_MapSpotAnimation.GoToEnd();
		m_MapSpotAnimation.Update();
		//SetColor(m_MapSpotAnimation.GetColor());
	}
	else
	{
		m_MapSpotAnimation.SetColorAnimation(NULL);
	}
}
*/