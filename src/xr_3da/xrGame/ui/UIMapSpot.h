// UIMapSpot.h:  пятно на карте, 
// при наводке мыше посылает сообщение родителю
//////////////////////////////////////////////////////////////////////

#pragma once
/*
#include "UIStatic.h"
#include "UIButton.h"
#include "UIColorAnimatorWrapper.h"
#include "../InfoPortion.h"
#include "../game_graph.h"

//////////////////////////////////////////////////////////////////////////

#define					MAP_ICON_WIDTH			32
#define					MAP_ICON_HEIGHT			32

#define					MAP_ICON_GRID_WIDTH		32
#define					MAP_ICON_GRID_HEIGHT	32

extern const int		ARROW_DIMENTIONS;

//////////////////////////////////////////////////////////////////////////

class CUIMapSpot: public CUIButton
{
private:
	typedef CUIButton inherited;
public:
							CUIMapSpot	();
	virtual					~CUIMapSpot	();

	virtual void			Draw		();
	virtual void			Update		();
	virtual void			SendMessage	(CUIWindow* pWnd, s16 msg, void* pData );

	//возвращает позицию отметки на карте в мировых координатах
	virtual Fvector			MapPos		();

	virtual void			SetObjectID	(u16 id);
	// Установка динамического проявления и исчезания иконки
	void					DynamicManifestation(bool value);

private:
	//id текущего уровня
	GameGraph::_LEVEL_ID	m_our_level_id;
	//id объекта, который показывается
	//на карте, если такого нет то 0xffff
	u16						m_object_id;

public:
	//позиция точки в мировой системе координат,
	//нужно если m_pObject NULL
	Fvector					m_vWorldPos;
	//имя локации
	CUIString				m_sNameText;
	//если есть текст с описанием 
	CUIString				m_sDescText;
	// Вкл/выкл стрелку-указатель
	bool					m_bArrowEnabled;
	// Признак видимости стрелочки
	bool					m_bArrowVisible;
	//цвет стерлочки указателя для объекта
	u32						arrow_color;
	
	//центрирование иконки 
	enum EMapSpotAlign
	{
		eBottom, eCenter, eNone
	};
	EMapSpotAlign			m_eAlign;
	
	//направление иконки
	bool					m_bHeading;
	float					m_fHeading;
	// Cтрелочка - указатель
	CUIStaticItem			m_Arrow;
	// Имя уровня которому пренадлежит спот
	shared_str				m_LevelName;

protected:
	// Анимация для мапспота при его фокусировании/дефокусировании
	CUIColorAnimatorWrapper	m_MapSpotAnimation;

public:
	// Установть цветовую анимацию для мапспота
	void					SetColorAnimation	(const shared_str &animationName) { m_MapSpotAnimation.SetColorAnimation(animationName); }
};
*/