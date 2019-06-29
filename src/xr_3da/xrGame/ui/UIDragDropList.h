// UIDragDropList.h: список элементов Drag&Drop
//
//////////////////////////////////////////////////////////////////////

#ifndef _UI_DRAG_DROP_LIST_H_
#define _UI_DRAG_DROP_LIST_H_


#pragma once
#error "this file removed from project !!!"

#include "uiframewindow.h"
#include "uidragdropitem.h"
#include "uiscrollbar.h"

#include "uistatic.h"
class CUIDragDropList;

typedef bool (*CHECK_PROC)(CUIDragDropItem* pItem, CUIDragDropList* pList);
typedef bool(*DD_Check_func)(CUIDragDropItem* v1, CUIDragDropItem* v2);

typedef enum{CELL_EMPTY, CELL_FULL} E_CELL_STATE;


DEF_LIST (DRAG_DROP_LIST, CUIDragDropItem*);
#define DRAG_DROP_LIST_IT xr_list<CUIDragDropItem*>::iterator


class CUIDragDropList: public CUIWindow
{
private:
	typedef CUIWindow inherited;
	enum{
		flNeedScrollToTop		=	(1<<0),
		flNeedScrollRecalculate	=	(1<<1),
		flCustomPlacement		=	(1<<2),//автоматическое расстановка элементов
		flBlockCustomPlacement	=	(1<<3),//запрещение ручной расстановки
		flGridVisible			=	(1<<4),
		flUnlimitedCapacity		=	(1<<5),
		flScrollBarEnabled		=	(1<<6),
	};
	Flags8										m_flags;
	void ScrollBarRecalculateTotal				();
public:
				CUIDragDropList					();
	virtual		~CUIDragDropList				();

			void SortList(DD_Check_func f);

	virtual void AttachChild					(CUIWindow* pChild);
	virtual void DetachChild					(CUIWindow* pChild);

	virtual void DetachAll						();

	virtual void DropAll						();
		
	virtual void SendMessage					(CUIWindow *pWnd, s16 msg, void *pData);
	virtual bool OnMouse						(float x, float y, EUIMessages mouse_action);

	virtual void Init							(float x, float y, float width, float height);
	virtual void InitGrid						(int iRowsNum, int iColsNum, 
													bool bGridVisible = true, int iViewRowsNum = 0);
	int			GetCols							() {return m_iColsNum;}
	int			GetRows							() {return m_iRowsNum;}
	int			GetViewRows						() {return m_iViewRowsNum;}
	
	//размеры клеточки сетки
	int			GetCellWidth					() {return m_iCellWidth;}
	int			GetCellHeight					() {return m_iCellHeight;}
	void		SetCellWidth					(int iCellWidth) {m_iCellWidth = iCellWidth;}
	void		SetCellHeight					(int iCellHeight) {m_iCellHeight = iCellHeight;}

	void		OnCustomPlacement				() {m_flags.set(flCustomPlacement,TRUE);}
	void		OffCustomPlacement				() {m_flags.set(flCustomPlacement,FALSE);}
	bool		GetCustomPlacement				() {return !!m_flags.test(flCustomPlacement);}

	void		BlockCustomPlacement			() {m_flags.set(flBlockCustomPlacement,TRUE);}
	void		UnblockCustomPlacement			() {m_flags.set(flBlockCustomPlacement,FALSE);}
	bool		IsCustomPlacementBlocked		() {return !!m_flags.test(flBlockCustomPlacement);}


	//дополнительна€ проверка
	CHECK_PROC  GetCheckProc					() {return m_pCheckProc;}
	void		SetCheckProc					(CHECK_PROC proc) {m_pCheckProc = proc;}

	//установление позиции скролинга
	void		SetScrollPos					(int iScrollPos);
	int			GetScrollPos					();

	virtual void Draw							();
	virtual void Update							();

	DRAG_DROP_LIST& GetDragDropItemsList		() {return m_DragDropItemsList;}

	void		EnableScrollBar					(bool enable);
	bool		IsScrollBarEnabled				() {return !!m_flags.test(flScrollBarEnabled);}

	bool		CanPlaceItem					(CUIDragDropItem *pDDItem);

	void		SetItemsScaleXY					(float fItemsScale_x, float fItemsScale_y);
	float		GetItemsScaleX					() const { return m_fItemsScaleX; }
	float		GetItemsScaleY					() const { return m_fItemsScaleY; }

	// ѕодсветить указанную клеточку в листе
	void		HighlightCell					(int row, int col, bool on);
	void		HighlightAllCells				(bool on);

	const int	GetCurrentFirstRow				() const { return m_iCurrentFirstRow; }

	// ќтсортировать элементы в листе, чтобы они компактно разместились в клиентской области
	// листа
	void		RearrangeItems					();

	// ”становить режим работы листа: бесконечна€/конечна€ вместимость
	void		SetUnlimitedCapacity			(bool value) {m_flags.set(flUnlimitedCapacity,value);}

protected:
	//полоса прокрутки
	CUIScrollBar								m_ScrollBar;
//	bool										m_bScrollBarEnabled;

	bool		PlaceItemInGrid					(CUIDragDropItem* pItem);
	void		RemoveItemFromGrid				(CUIDragDropItem* pItem);

	bool		CanPlaceItemInGrid				(CUIDragDropItem* pItem, int& place_row, int& place_col);
	void		PlaceItemAtPos					(int place_row, int place_col, CUIDragDropItem* pItem);
	bool		CanPlace						(int row, int col, CUIDragDropItem* pItem);

	// ƒл€ прив€зки скроллбара к контенту листа необходимо определить самую
	// нижнюю (правую не надо пока, так как в DDList'е HSCROLL не используетс€) зан€тую клеточку
	int			GetLastBottomFullCell			();

	// ѕересчитать состо€ние скроллбара
	// Params:	needScrollToTop	- true если после пересчета состо€ние необходимо прокрутить скроллбар
	//							на начало	
	void		ScrollBarRecalculate			(bool needScrollToTop);

	//состо€ние клеточки в сетке
	E_CELL_STATE& 	GetCell						(int row, int col){VERIFY(row<m_iRowsNum && col<m_iColsNum);return m_vGridState[row*GetCols() + col];}
	E_CELL_STATE    GetCellState				(int row, int col);

	DEFINE_VECTOR	(E_CELL_STATE, GRID_STATE_VECTOR, GRID_STATE_IT);
    
	//состо€ние клеточек сетки
	GRID_STATE_VECTOR							m_vGridState;

	//дл€ изображени€ клеточек сетки
	DEFINE_VECTOR	(CUIStatic*, CELL_STATIC_VECTOR, CELL_STATIC_IT);
	CELL_STATIC_VECTOR							m_vpCellStatic;
	int				ReallocateCells				(u32 new_size);

	//размеры сетки дл€ элементов 
	int											m_iColsNum;
	int											m_iRowsNum;
	//видима€ часть р€дков в сетке
	int											m_iViewRowsNum;

	//дл€ прокрутки листинга
	int											m_iCurrentFirstRow;
	void			UpdateList					();

	//размеры клеточки сетки в пиксел€х
	int											m_iCellWidth;
	int											m_iCellHeight;

	//указатель на произвольную функцию
	//необходимую дл€ дополнительной проверки
	//на прием элемента DragDrop себе
	CHECK_PROC m_pCheckProc;

	//список эл-тов DrapDrop присоединенных в данный момент к списку
	xr_list<CUIDragDropItem*>					m_DragDropItemsList;

	// ƒл€ возможности изменени€ размера лежащих на листе элементов, запомним скейл
	float										m_fItemsScaleX;
	float										m_fItemsScaleY;
};

#endif //_UI_DRAG_DROP_LIST_H_