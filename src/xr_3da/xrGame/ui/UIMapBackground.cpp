// CUIMapBackground.cpp:перемещаемая картинка (сам ландшафт)
//						интреактивной карты
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
/*
#include "UIMapBackground.h"
#include "UIGlobalMapLocation.h"

#include "../actor.h"
#include "../levelFogOfWar.h"
#include "../level.h"

//////////////////////////////////////////////////////////////////////////

//	Cколько пиксилей соответствует одному метру
#define FOG_OF_WAR_TEXTURE			"ui\\ui_fog_of_war"
#define FOG_TEX_WIDTH				64
#define FOG_TEX_HEIGHT				64
#define FOG_TEX_SIZE				FOG_TEX_WIDTH


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIMapBackground::CUIMapBackground()
{
	Show(false);
	Enable(false);
	m_bNoActorFocus = false;
}

//////////////////////////////////////////////////////////////////////////

CUIMapBackground::~CUIMapBackground()
{
}

//////////////////////////////////////////////////////////////////////////

void CUIMapBackground::Init(int x, int y, int width, int height)
{
	CUIWindow::Init(x, y, width, height);

	m_iMapViewWidthPixels	= width;
	m_iMapViewHeightPixels	= height;

	m_fMapViewWidthMeters = pSettings->r_float("game_map","local_map_width_meters");
	//m_fMapViewWidthMeters =  width*MAP_PIXEL_TO_METERS;
	//m_fMapViewHeightMeters = height*MAP_PIXEL_TO_METERS;
	m_fMapViewHeightMeters = height*(m_fMapViewWidthMeters/width);
	
	//вычислить ширину одной клеточки тумана в пикселях
	Fvector cell_world_pos;
	Ivector2 screen_pos, screen_pos1;
	Level().FogOfWar().GetFogCellWorldPos(0,0,cell_world_pos);
	ConvertToLocal(cell_world_pos, screen_pos);
	Level().FogOfWar().GetFogCellWorldPos(1,1,cell_world_pos);
	ConvertToLocal(cell_world_pos, screen_pos1);
	
	int fog_cell_size = _max(_abs(screen_pos1.x - screen_pos.x),
							 _abs(screen_pos1.y - screen_pos.y));

	AttachChild(&m_fogOfWarCell);
	m_fogOfWarCell.Init(FOG_OF_WAR_TEXTURE, 0, 0, fog_cell_size, fog_cell_size);
	m_fogOfWarCell.Show(false);
	m_fogOfWarCell.Enable(false);
	
	m_fogOfWarCell.SetTextureScaleXY((float)fog_cell_size/(FOG_TEX_SIZE-2),(float)fog_cell_size/(FOG_TEX_SIZE-2));
}

//-----------------------------------------------------------------------------/
//	Вызывается каждый раз перед вызовом карты
//-----------------------------------------------------------------------------/

void CUIMapBackground::InitMapBackground(const ref_shader &sh)
{
	Irect rect = GetAbsoluteRect();
	landscape.SetPos(rect.left, rect.top);
	landscape.SetRect(0, 0, GetWidth(), GetHeight());
	landscape.SetShader(sh);

	if (!m_bNoActorFocus)
	{
		UpdateActorPos();
	}
	else
	{
		UpdateActivePos();
		m_bNoActorFocus	= false;
	}
	
	UpdateMapSpots();
	m_pActiveMapSpot	= NULL;
}

//-----------------------------------------------------------------------------/
//	Global predicate for pointer with boost::shared_ptr comparing
//-----------------------------------------------------------------------------/

struct Cmp: public std::unary_function<CUIMapBackground::MapSpotPtr, bool>
{
	CUIWindow * wnd_;

	Cmp(CUIWindow *wnd):wnd_(wnd)
	{}

	result_type operator ()(const argument_type &lhs)
	{
		return lhs.get() == wnd_;
	}
};

//////////////////////////////////////////////////////////////////////////

void CUIMapBackground::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	//проверить сообщение от иконок на карте

	if(m_vMapSpots.end() != std::find_if(m_vMapSpots.begin(), m_vMapSpots.end(), Cmp(pWnd)))
	{
		if (STATIC_FOCUS_RECEIVED == msg)
		{
			m_pActiveMapSpot = smart_cast<CUIMapSpot*>(pWnd);
			GetTop()->SendMessage(this, MAPSPOT_FOCUS_RECEIVED);
		}
		else if (STATIC_FOCUS_LOST == msg)
		{
			GetTop()->SendMessage(this, MAPSPOT_FOCUS_LOST);
			m_pActiveMapSpot = NULL;
		}
		else if (BUTTON_CLICKED == msg)
		{
			GetTop()->SendMessage(this, MAPSPOT_CLICKED);
			m_pActiveMapSpot = NULL;
		}
	}
	inherited::SendMessage(pWnd, msg, pData);
}

//-----------------------------------------------------------------------------/
//	Перевод из мировой системы координат в координаты карты на экране
//	с началом координат в левом верхнем углу
//-----------------------------------------------------------------------------/

void CUIMapBackground::ConvertToLocal(const Fvector& src, Ivector2& dest)
{
	dest.x = iFloor(0.5f + ((src.x - m_fMapLeftMeters - m_fMapX)/m_fMapViewWidthMeters)*(float)m_iMapViewWidthPixels);

	dest.y = iFloor(0.5f + ((m_fMapHeightMeters - (src.z - m_fMapBottomMeters)
			     - m_fMapY)/m_fMapViewHeightMeters)*(float)m_iMapViewHeightPixels);
}

//-----------------------------------------------------------------------------/
//	Cопоставление экранным координатам мировых
//-----------------------------------------------------------------------------/

void CUIMapBackground::ConvertFromLocalToMap(int x, int y, Fvector2& dest)
{
	dest.x = m_fMapViewWidthMeters*float(x)/float(m_iMapViewWidthPixels);
	dest.y = m_fMapViewHeightMeters*float(y)/float(m_iMapViewHeightPixels);

}

//-----------------------------------------------------------------------------/
//	Перевод из мировой системы координат в 
//	текстурные координаты карты
//-----------------------------------------------------------------------------/

void CUIMapBackground::ConvertToTexture(const Fvector& src, Fvector2& dest)
{
	ConvertToTexture(src.x, src.z, dest);
}

//////////////////////////////////////////////////////////////////////////

void CUIMapBackground::ConvertToTexture(float x, float y, Fvector2& dest)
{
	dest.x = x/m_fMapWidthMeters;
	dest.y = y/m_fMapHeightMeters;
}

//////////////////////////////////////////////////////////////////////////

void CUIMapBackground::Update()
{
	const Fbox& level_box	= Level().ObjectSpace.GetBoundingVolume();
	m_fMapWidthMeters		= level_box.x2 - level_box.x1;
	m_fMapHeightMeters		= level_box.z2 - level_box.z1;
	m_fMapLeftMeters		= level_box.x1;
	m_fMapTopMeters			= level_box.z2;
	m_fMapBottomMeters		= level_box.z1;
 
	m_fMapViewWidthMeters	= m_fMapWidthMeters;
	m_fMapViewHeightMeters	= GetHeight()*(m_fMapViewWidthMeters/GetWidth());

	UpdateMapSpots();

	//ignore CUIButton update, call CUIWindow not inherited::Update()
	CUIWindow::Update();
}

//////////////////////////////////////////////////////////////////////////

void CUIMapBackground::Draw()
{
	Fvector2 top_left;
	Fvector2 right_bottom;
	
	ConvertToTexture(m_fMapX, m_fMapY, top_left);
	ConvertToTexture(m_fMapX + m_fMapViewWidthMeters,
					 m_fMapY + m_fMapViewHeightMeters, 
					 right_bottom);
	
	landscape.Render( top_left.x,	top_left.y, 
				      top_left.x,	right_bottom.y,
				  right_bottom.x,	right_bottom.y,
				  right_bottom.x,	top_left.y);

	//ignore CUIButton draw
	CUIWindow::Draw();

	DrawFogOfWar();
}

//////////////////////////////////////////////////////////////////////////

void CUIMapBackground::DrawFogOfWar()
{
	Ivector2 cell_left_top_pos;
	Ivector2 cell_right_bottom_pos;
	
	Fvector2 left_top_pos;
	Fvector2 right_bottom_pos;
	
	left_top_pos.x = m_fMapX + m_fMapLeftMeters;
	left_top_pos.y = m_fMapHeightMeters - m_fMapY + m_fMapBottomMeters - m_fMapViewHeightMeters;

	right_bottom_pos.x = left_top_pos.x + m_fMapViewWidthMeters;
	right_bottom_pos.y = left_top_pos.y + m_fMapViewHeightMeters;


	CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if(pActor) Level().FogOfWar().GetFogCell(pActor->Position(), cell_left_top_pos);

	//получить индексы квадрата тумана
	//в левой верхней видимой части карты
	Level().FogOfWar().GetFogCell(left_top_pos, cell_left_top_pos);
	//в правой нижней видимой части карты
	Level().FogOfWar().GetFogCell(right_bottom_pos, cell_right_bottom_pos);

	int i = 0, j = 0;

	for(i=cell_left_top_pos.y; i<=cell_right_bottom_pos.y; ++i)
	{
		for(j=cell_left_top_pos.x; j<=cell_right_bottom_pos.x; ++j)
		{
			DrawFogOfWarCell(j, i);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIMapBackground::DrawFogOfWarCell(int x, int y)
{
	if(Level().FogOfWar().GetFogCell(x,y).IsOpened()) return;

	unsigned char fog_shape = Level().FogOfWar().GetFogCell(x,y).shape;


	if(fog_shape == 0)
	{
		m_fogOfWarCell.GetUIStaticItem().SetOriginalRect(
										1*FOG_TEX_WIDTH + 1, 0*FOG_TEX_HEIGHT + 1,
										2*FOG_TEX_WIDTH - 1, 1*FOG_TEX_HEIGHT - 1);
	}
	else if((fog_shape & FOG_OPEN_LEFT) && (fog_shape & FOG_OPEN_UP))
	{
		m_fogOfWarCell.GetUIStaticItem().SetOriginalRect(
										3*FOG_TEX_WIDTH + 1, 2*FOG_TEX_HEIGHT + 1,
										4*FOG_TEX_WIDTH - 1, 3*FOG_TEX_HEIGHT - 1);
	}
	else if((fog_shape & FOG_OPEN_RIGHT) && (fog_shape & FOG_OPEN_UP))
	{
		m_fogOfWarCell.GetUIStaticItem().SetOriginalRect(
										2*FOG_TEX_WIDTH + 1, 2*FOG_TEX_HEIGHT + 1,
										3*FOG_TEX_WIDTH - 1, 3*FOG_TEX_HEIGHT - 1);
	}
	else if((fog_shape & FOG_OPEN_LEFT) && (fog_shape & FOG_OPEN_DOWN))
	{
		m_fogOfWarCell.GetUIStaticItem().SetOriginalRect(
										3*FOG_TEX_WIDTH + 1, 1*FOG_TEX_HEIGHT + 1,
										4*FOG_TEX_WIDTH - 1, 2*FOG_TEX_HEIGHT - 1);

	}
	else if((fog_shape & FOG_OPEN_RIGHT) && (fog_shape & FOG_OPEN_DOWN))
	{
		m_fogOfWarCell.GetUIStaticItem().SetOriginalRect(
										2*FOG_TEX_WIDTH + 1, 1*FOG_TEX_HEIGHT + 1,
										3*FOG_TEX_WIDTH - 1, 2*FOG_TEX_HEIGHT - 1);
	}
	else if(fog_shape & FOG_OPEN_LEFT)
	{
		m_fogOfWarCell.GetUIStaticItem().SetOriginalRect(
										0*FOG_TEX_WIDTH + 1, 1*FOG_TEX_HEIGHT + 1,
										1*FOG_TEX_WIDTH - 1, 2*FOG_TEX_HEIGHT - 1);
	}	
	else if(fog_shape & FOG_OPEN_RIGHT)
	{
		m_fogOfWarCell.GetUIStaticItem().SetOriginalRect(
										1*FOG_TEX_WIDTH + 1, 1*FOG_TEX_HEIGHT + 1,
										2*FOG_TEX_WIDTH - 1, 2*FOG_TEX_HEIGHT - 1);
	}
	else if(fog_shape & FOG_OPEN_UP)
	{
		m_fogOfWarCell.GetUIStaticItem().SetOriginalRect(
										0*FOG_TEX_WIDTH + 1, 2*FOG_TEX_HEIGHT + 1,
										1*FOG_TEX_WIDTH - 1, 3*FOG_TEX_HEIGHT - 1);
	}
	else if(fog_shape & FOG_OPEN_DOWN)
	{
		m_fogOfWarCell.GetUIStaticItem().SetOriginalRect(
										1*FOG_TEX_WIDTH + 0, 2*FOG_TEX_HEIGHT + 0,
										2*FOG_TEX_WIDTH - 1, 3*FOG_TEX_HEIGHT - 1);
	}
	else if(fog_shape & FOG_OPEN_UP_LEFT)
	{
		m_fogOfWarCell.GetUIStaticItem().SetOriginalRect(
										2*FOG_TEX_WIDTH + 1, 3*FOG_TEX_HEIGHT + 1,
										3*FOG_TEX_WIDTH - 1, 4*FOG_TEX_HEIGHT - 1);
	}
	else if(fog_shape & FOG_OPEN_UP_RIGHT)
	{
		m_fogOfWarCell.GetUIStaticItem().SetOriginalRect(
										3*FOG_TEX_WIDTH + 1, 3*FOG_TEX_HEIGHT + 1,
										4*FOG_TEX_WIDTH - 1, 4*FOG_TEX_HEIGHT - 1);
	}
	else if(fog_shape & FOG_OPEN_DOWN_LEFT)
	{
		m_fogOfWarCell.GetUIStaticItem().SetOriginalRect(
										0*FOG_TEX_WIDTH + 1, 3*FOG_TEX_HEIGHT + 1,
										1*FOG_TEX_WIDTH - 1, 4*FOG_TEX_HEIGHT - 1);
	}
	else if(fog_shape & FOG_OPEN_DOWN_RIGHT)
	{
		m_fogOfWarCell.GetUIStaticItem().SetOriginalRect(
										1*FOG_TEX_WIDTH + 1, 3*FOG_TEX_HEIGHT + 1,
										2*FOG_TEX_WIDTH - 1, 4*FOG_TEX_HEIGHT - 1);
	}
	else
	{
		m_fogOfWarCell.SetText("?");
	}

	Fvector cell_world_pos;
	Ivector2 screen_pos;

	
	Level().FogOfWar().GetFogCellWorldPos(x,y,cell_world_pos);
	ConvertToLocal(cell_world_pos, screen_pos);
	m_fogOfWarCell.SetWndPos(screen_pos.x, screen_pos.y);
	m_fogOfWarCell.Update();
	m_fogOfWarCell.Draw();
}

//////////////////////////////////////////////////////////////////////////

void CUIMapBackground::OnMouse(int x, int y, EUIMessages mouse_action)
{
	CUIWindow::OnMouse( x, y, mouse_action);
	
	int deltaX = 0;
	int deltaY = 0;


	//проверить попадает ли курсор на кнопку
	//координаты заданы относительно самой кнопки
	bool cursor_on_button;

	if(x>=0 && x<GetWidth() && y>=0 && y<GetHeight())
	{
		cursor_on_button = true;
	}
	else
	{
		cursor_on_button = false;
	}

	
	m_bCursorOverWindow = cursor_on_button;


	m_bButtonClicked = false;


	if(mouse_action == WINDOW_MOUSE_MOVE && m_eButtonState == BUTTON_NORMAL)
	{
		GetParent()->SetCapture(this, cursor_on_button);
		// Check for spot arrow hit
		Irect r, absR = GetAbsoluteRect();
		Ivector2 p;
		p.x = x;
		p.y = y;

		for (WINDOW_LIST_it it = GetChildWndList().begin(); it != GetChildWndList().end(); ++it)
		{
			CUIMapSpot				*pSpot		= smart_cast<CUIMapSpot*>(*it);
			if (!pSpot) continue;

			if (pSpot->m_bArrowVisible && pSpot->IsShown())
			{
				r.left		= pSpot->m_Arrow.GetPosX() - absR.left;
				r.top		= pSpot->m_Arrow.GetPosY() - absR.top;
				r.right		= ARROW_DIMENTIONS + r.left;
				r.bottom	= ARROW_DIMENTIONS + r.top;

				static CUIMapSpot *focusHolder = NULL;

				if (NULL == focusHolder || pSpot == focusHolder)
				{
					if ( r.in(p))
					{
						if (NULL == focusHolder)
						{
							m_pActiveMapSpot = pSpot;
							GetMessageTarget()->SendMessage(this, MAPSPOT_ARROW_FOCUS_RECEIVED, NULL);
							focusHolder = pSpot;
						}
					}
					else
					{
						if (pSpot == focusHolder)
						{
							GetMessageTarget()->SendMessage(this, MAPSPOT_ARROW_FOCUS_LOST, NULL);
							focusHolder = NULL;
						}
					}
				}
			}
		}
	}

	if(mouse_action == WINDOW_LBUTTON_DB_CLICK && m_bCursorOverWindow)
	{
		GetParent()->SetCapture(this, false);
	}
	else if(mouse_action == WINDOW_RBUTTON_DOWN && m_bCursorOverWindow)
	{
//		GetTop()->SendMessage(this, ITEM_RBUTTON_CLICK);
	}


	if(m_eButtonState == BUTTON_NORMAL)
	{
		if(mouse_action == WINDOW_LBUTTON_DOWN)
		{
			m_eButtonState = BUTTON_PUSHED;
			
			GetParent()->SetCapture(this, true);
		}
	}
	else if(m_eButtonState == BUTTON_PUSHED)
	{
		if(mouse_action == WINDOW_LBUTTON_UP)
		{
			if(cursor_on_button)
			{
				m_bButtonClicked = true;
			}
		
			m_eButtonState = BUTTON_NORMAL;
			
			//освободить мышь
			GetParent()->SetCapture(this, false);
		}
		else if(mouse_action == WINDOW_MOUSE_MOVE)
		{
			deltaX = x - m_iOldMouseX;
			deltaY = y - m_iOldMouseY;
			MoveMap(deltaX, deltaY);
			if (m_pActiveMapSpot && smart_cast<CUIGlobalMapLocation*>(m_pActiveMapSpot))
				SendMessage(m_pActiveMapSpot, STATIC_FOCUS_RECEIVED, NULL);
			GetMessageTarget()->SendMessage(this, MAP_MOVED, NULL);
		}
	}
	else if(m_eButtonState == BUTTON_UP)
	{
		if(mouse_action == WINDOW_MOUSE_MOVE)
		{
			if(cursor_on_button)
				m_eButtonState = BUTTON_PUSHED;
		}
		else if(mouse_action == WINDOW_LBUTTON_UP)
		{
			m_eButtonState = BUTTON_NORMAL;
			GetParent()->SetCapture(this, false);
		}
	}

	//запомнить положение мыши
	m_iOldMouseX = x;
	m_iOldMouseY = y;
}

//-----------------------------------------------------------------------------/
//	Oбновления иконок на карте
//-----------------------------------------------------------------------------/

void CUIMapBackground::UpdateMapSpots()
{
	//разместить на карте объекты
	Ivector2 pos;
	
	for(u32 i = 0; i<m_vMapSpots.size(); ++i)
	{
		ConvertToLocal(m_vMapSpots[i]->MapPos(), pos);
		
		switch(m_vMapSpots[i]->m_eAlign) {
		case CUIMapSpot::eBottom:
            m_vMapSpots[i]->SetWndPos(pos.x - MAP_ICON_WIDTH/2, pos.y - MAP_ICON_HEIGHT);
			break;
		case CUIMapSpot::eCenter:
			m_vMapSpots[i]->SetWndPos(pos.x - MAP_ICON_WIDTH/2, pos.y - MAP_ICON_HEIGHT/2);
			break;
		default:
			m_vMapSpots[i]->SetWndPos(pos.x, pos.y);
		}
	}
}

//-----------------------------------------------------------------------------/
//	Установить положение карты, так чтобы актер 
//	был по центру
//-----------------------------------------------------------------------------/

void CUIMapBackground::UpdateActorPos()
{
	//установить положение карты, так чтобы актер 
	//был по центру
	CEntityAlive *pActor = smart_cast<CEntityAlive*>(Level().CurrentEntity());
	if(!pActor) return;
	m_vActivePos = pActor->Position();
	UpdateActivePos();
}

//-----------------------------------------------------------------------------/
//	Центрируем карту по заданной активной точке
//-----------------------------------------------------------------------------/

void CUIMapBackground::UpdateActivePos()
{
	//установить положение карты, так чтобы заданая точка 
	//была по центру
	m_fMapX = m_vActivePos.x - m_fMapLeftMeters 
		- m_fMapViewWidthMeters/2.f;
	m_fMapY = m_fMapHeightMeters - (m_vActivePos.z - m_fMapBottomMeters)
		- m_fMapViewHeightMeters/2.f;

	if(m_fMapX<0) m_fMapX = 0;
	if(m_fMapX>m_fMapWidthMeters - m_fMapViewWidthMeters) 
		m_fMapX = m_fMapWidthMeters - m_fMapViewWidthMeters;

	if(m_fMapY<0) m_fMapY = 0;
	if(m_fMapY>m_fMapHeightMeters - m_fMapViewHeightMeters) 
		m_fMapY = m_fMapHeightMeters - m_fMapViewHeightMeters;
}

//////////////////////////////////////////////////////////////////////////

void CUIMapBackground::MoveMap(const int deltaX, const int deltaY)
{
	Fvector2 dest;
	ConvertFromLocalToMap(deltaX, deltaY, dest);

	m_fMapX -= dest.x;
	m_fMapY -= dest.y;

	if(m_fMapX<0) m_fMapX = 0;
	if(m_fMapX>m_fMapWidthMeters - m_fMapViewWidthMeters) 
		m_fMapX = m_fMapWidthMeters - m_fMapViewWidthMeters;

	if(m_fMapY<0) m_fMapY = 0;
	if(m_fMapY>m_fMapHeightMeters - m_fMapViewHeightMeters) 
		m_fMapY = m_fMapHeightMeters - m_fMapViewHeightMeters;
}

//////////////////////////////////////////////////////////////////////////

void CUIMapBackground::RemoveAllSpots()
{
	for(u32 i=0; i<m_vMapSpots.size(); ++i)
	{	
		if (m_vMapSpots[i]->GetParent() && m_vMapSpots[i]->GetParent()->IsChild(m_vMapSpots[i].get()))
			m_vMapSpots[i]->GetParent()->DetachChild(m_vMapSpots[i].get());
	}

	m_vMapSpots.clear();
}
*/