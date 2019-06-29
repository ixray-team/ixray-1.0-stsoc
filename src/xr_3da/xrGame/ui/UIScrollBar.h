//////////////////////////////////////////////////////////////////////
// UIScrollBar.h: полоса прокрутки для окон
//////////////////////////////////////////////////////////////////////

#ifndef _UI_SCROLLBAR_H_
#define _UI_SCROLLBAR_H_

#pragma once
#include "uiwindow.h"

//#include "UIButton.h"
//#include "UIScrollBox.h"

//#define SCROLLBAR_WIDTH  16.0f
//#define SCROLLBAR_HEIGHT 16.0f
class CUI3tButton;
class CUIScrollBox;
class CUIStaticItem;

class CUIScrollBar :public CUIWindow
{
private:
	typedef CUIWindow inherited;
protected:
	//передвинуть каретку, если возможно
	bool			ScrollInc();
	bool			ScrollDec();

	//обновить полосу
	void			UpdateScrollBar();


	//горизонтальный или вертикальный 
	bool			m_bIsHorizontal;

	//кнопки скролинга
	CUI3tButton*		m_DecButton;
	CUI3tButton*		m_IncButton;

	//каретка скролинга
	CUIScrollBox*	m_ScrollBox;

	//подложка для скролинга
	CUIStaticItem*	m_StaticBackground;

	//текущая позиция
	int				m_iScrollPos;

	// step size
	int				m_iStepSize;

	//границы отображения
	int				m_iMinPos;
	int				m_iMaxPos;

	//размер отображаемой страницы
	int				m_iPageSize;

	// internal use
	int				m_ScrollWorkArea;
	bool			m_b_enabled;
protected:
	u32				ScrollSize			(){return _max(1,m_iMaxPos-m_iMinPos-m_iPageSize+1);}
	void			ClampByViewRect		();
	void			SetPosScrollFromView(float view_pos, float view_width, float view_offs);
	int				PosViewFromScroll	(int view_size, int view_offs);
	void			SetScrollPosClamped	(int iPos) { 
														m_iScrollPos = iPos; 
														clamp(m_iScrollPos,m_iMinPos,m_iMaxPos-m_iPageSize+1); }
public:
					CUIScrollBar	(void);
	virtual			~CUIScrollBar	(void);

			void	SetEnabled		(bool b)			{m_b_enabled = b;if(!m_b_enabled)Show(m_b_enabled);}
			bool	GetEnabled		()					{return m_b_enabled;}
	virtual void	Show			(bool b);
	virtual void	Enable			(bool b);
	virtual void	Init			(float x, float y, float length, bool bIsHorizontal, LPCSTR profile = "default");

	//сообщения, отправляемые родительскому окну
//	typedef enum{VSCROLL, HSCROLL} E_MESSAGE;

	virtual void	SendMessage(CUIWindow *pWnd, s16 msg, void *pData);
	virtual bool	OnMouse(float x, float y, EUIMessages mouse_action);

	virtual void	Draw			();

	virtual void	SetWidth		(float width);
	virtual void	SetHeight		(float height);

	virtual void	Reset			();
	// В листбоксе скроллбар глючит если мы удаляем элементы снизу листа, а скроллбар
	// в это время был в позиции не 0. Эта функция - фикс
	void			Refresh			();
	// скролинг
	// величина шага при нажатии кнопок или колеса мыши
	void			SetStepSize		(int step);
	// диапазон значений 
	void 			SetRange		(int iMin, int iMax);
	void 			GetRange		(int& iMin, int& iMax) {iMin = m_iMinPos;  iMax = m_iMaxPos;}
	int 			GetMaxRange		() {return m_iMaxPos;}
	int 			GetMinRange		() {return m_iMinPos;}
	// размер страницы (влияет на диапазон каретки от (m_iMinPos .. m_iMaxPos-m_iPageSize) )
	void			SetPageSize		(int iPage) { m_iPageSize = _max(0,iPage); UpdateScrollBar();}
	int				GetPageSize		() {return m_iPageSize;}
	// положение каретки
	void			SetScrollPos	(int iPos) { SetScrollPosClamped(iPos); UpdateScrollBar();}
	int				GetScrollPos	() {return _max(m_iMinPos,m_iScrollPos);}
	
	void			TryScrollInc	();
	void			TryScrollDec	();
};


#endif