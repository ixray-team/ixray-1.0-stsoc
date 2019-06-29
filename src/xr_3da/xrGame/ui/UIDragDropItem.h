// UIDragDropItem.h: класс перетаскиваемой картинки
//
//////////////////////////////////////////////////////////////////////

#ifndef _UI_DRAG_DROP_ITEM_H_
#define _UI_DRAG_DROP_ITEM_H_

#pragma once

#error "this file removed from project !!!"

#include "uibutton.h"
#include "UIInventoryUtilities.h"

class CUIDragDropItem;

typedef void (*CUSTOM_UPDATE_PROC)(CUIDragDropItem* pItem);
typedef void (*CUSTOM_DRAW_PROC)(CUIDragDropItem* pItem);


class CUIDragDropItem : public CUIButton
{
	typedef CUIButton inherited;
public:
					CUIDragDropItem				();
	virtual			~CUIDragDropItem			();


	virtual void	Init						(LPCSTR tex_name, float x, float y, float width, float height);
	virtual CUIDragDropItem* cast_drag_drop_item()	{return this;}

	virtual bool	OnMouse						(float x, float y, EUIMessages mouse_action);
	virtual void	Draw						();
	virtual void	Update						();

	void			EnableDragDrop				(bool bEnable = true)	{m_bDDEnabled = bEnable;}
	bool			IsDragDropEnabled			() const		{ return m_bDDEnabled; }

	Fvector2		GetPreviousPos				() {return m_previousPos;}


	//размеры в сетке Drag&Drop
	void			SetGridWidth				(int iGridWidth) {m_iGridWidth = iGridWidth;}
	void			SetGridHeight				(int iGridHeight) {m_iGridHeight = iGridHeight;}
	int				GetGridWidth				() {return m_iGridWidth;}
	int				GetGridHeight				() {return m_iGridHeight;}

	//положение в сетке
	int				GetGridRow					() {return m_iGridRow;}
	int				GetGridCol					() {return m_iGridCol;}
	void			SetGridRow					(int iGridRow) {m_iGridRow = iGridRow;}
	void			SetGridCol					(int iGridCol) {m_iGridCol = iGridCol;}

	virtual void*	GetData						() {return m_pData;}
	virtual void	SetData						(void* pData) {m_pData = pData;}

	void			SetCustomDraw				(CUSTOM_DRAW_PROC pCustomDrawProc){m_pCustomDrawProc = pCustomDrawProc;}

	//размеры клеточки сетки
	int				GetCellWidth				() {return m_iCellWidth;}
	int				GetCellHeight				() {return m_iCellHeight;}
	void			SetCellWidth				(int iCellWidth) {m_iCellWidth = iCellWidth;}
	void			SetCellHeight				(int iCellHeight) {m_iCellHeight = iCellHeight;}

	bool m_bInFloat;

	// —пециальные функции дл€ перемещени€ вещи посредством посылки сообщени€
	// ITEM_DROP даже если она не находитс€ над контролом в который бросают.
	// »спользуетс€ дл€ принудительного вытаскивани€ вещи из слота.
	void			MoveOnNextDrop				() { m_bMoveOnNextDrop = true; }
	// ѕроверка необходимости перемещени€ вещи без проверки находжени€ вещи над контролом
	bool			NeedMoveWithoutRectCheck	() { return m_bMoveOnNextDrop ? m_bMoveOnNextDrop = false, true : false; }

	// ѕропорционально скейлим себ€ по типу зума
	virtual void	Rescale						(float scale_x, float scale_y);

	// ѕодсвечиваем себ€ в инвентаре
	void			Highlight					(bool on);

protected:
	
	float cur_scale_x, cur_scale_y;
	float m_iOldMouseX;
	float m_iOldMouseY;

	//включен ли drag drop
	bool m_bDDEnabled;

	Fvector2 m_previousPos;

	//размер и положение в сетке
	//элементов Drag&Drop 
	int m_iGridWidth;
	int m_iGridHeight;

	int m_iGridCol;
	int m_iGridRow;

	//размеры клеточки сетки в пиксел€х
	int m_iCellWidth;
	int m_iCellHeight;


	//указатель на произвольные данные которые могут быть
	//присоеденены к элементу
	void* m_pData; 

	CUSTOM_DRAW_PROC m_pCustomDrawProc;

	// ѕризнак перемещени€ вещи в другой слот, при следующей посылке ITEM_DROP сообщени€.
	// см. функцию MoveOnNextDrop()
	bool m_bMoveOnNextDrop;

	// Ќеобходимость пересчитать предыдущее положение мышки, дл€ корректного перемещени€ иконки
	bool m_bNeedOldMousePosRecalc;
};

#endif // _UI_DRAG_DROP_ITEM_H_
