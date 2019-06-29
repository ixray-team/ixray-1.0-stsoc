// CUIMapBackground.h:  перемещаемая картинка (сам ландшафт)
//						интерактивной карты
//////////////////////////////////////////////////////////////////////
/*
#pragma once

#include "UIStatic.h"
#include "UIMapSpot.h"

//////////////////////////////////////////////////////////////////////////

class CUIMapBackground: public CUIButton
{
private:
	typedef CUIButton inherited;
protected:
	//подложка для карты
	CUIStaticItem	landscape;
	CUIMapSpot		m_fogOfWarCell;

	int				m_iOldMouseX;
	int				m_iOldMouseY;

	// Точка на карте по которой идет центрирование
	Fvector			m_vActivePos;
public:
	//ширина и высота отображаемого участка карты в метрах
	float			m_fMapViewWidthMeters;
	float			m_fMapViewHeightMeters;

	//положение и размеры всей карты
	float			m_fMapWidthMeters;
	float			m_fMapHeightMeters;
	float			m_fMapLeftMeters;
	float			m_fMapTopMeters;
	float			m_fMapBottomMeters;

	//текущее положение верхнего левого угла карты
	//в метрах от начала координат 
	//(0,0)- left top
	float			m_fMapX;
	float			m_fMapY;

	// флажек на то, что не надо при следующем показе карты центрировать ее на игроке, так как 
	// поступило требование показать определенную точку на карте
	bool			m_bNoActorFocus;

	//вектор обозначений на карте
	typedef boost::shared_ptr<CUIMapSpot> MapSpotPtr;
	DEFINE_VECTOR	(MapSpotPtr, MapSpotPtrVec, MapSpotPtrVecIt);
	MapSpotPtrVec	m_vMapSpots;
	CUIMapSpot*		m_pActiveMapSpot;
protected:
	void			DrawFogOfWar			();
	void			DrawFogOfWarCell		(int x, int y);

	//обновления иконок на карте
	void			UpdateMapSpots			();
	void			UpdateActorPos			();
public:
	// Центровать карту по координатам
	void			UpdateActivePos			();

	// Устанавливаем/читаем активную позицию на карте
	Fvector			GetActivePos			() const					{ return m_vActivePos; }
	void			SetActivePos			(const Fvector &vNewPos)	{ m_vActivePos = vNewPos; }

	// Двигаем карту
	void			MoveMap					(const int deltaX, const int deltaY);
	void			RemoveAllSpots			();
public:
					CUIMapBackground		();
	virtual			~CUIMapBackground		();

	virtual void	Init					(int x, int y, int width, int height);
	virtual void	InitMapBackground		(const ref_shader &sh);
	virtual void	Draw					();
	virtual void	Update					();
	virtual void	SendMessage				(CUIWindow* pWnd, s16 msg, void* pData = NULL);
	//для перетаскивания карты при помощи мыши
	virtual void	OnMouse					(int x, int y, EUIMessages mouse_action);

	void			ConvertToLocal			(const Fvector& src, Ivector2& dest);
	void			ConvertFromLocalToMap	(int x, int y, Fvector2& dest);
	void			ConvertToTexture		(const Fvector& src, Fvector2& dest);
	void			ConvertToTexture		(float x, float y, Fvector2& dest);
};
*/