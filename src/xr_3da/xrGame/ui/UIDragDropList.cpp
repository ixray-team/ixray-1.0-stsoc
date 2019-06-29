// UIDragDropList.cpp: список элементов Drag&Drop
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "uidragdroplist.h"
#include "UIOutfitSlot.h"
#include "../object_broker.h"

#define SCROLLBAR_OFFSET_X 5
#define SCROLLBAR_OFFSET_Y 0

const char * const CELL_TEXTURE		= "ui\\ui_inv_lattice";

CUIDragDropList::CUIDragDropList()
{
	SetCellHeight				(50);
	SetCellWidth				(50);

	m_pCheckProc				= NULL;



	m_iViewRowsNum				= 0;
	m_iCurrentFirstRow			= 0;

	m_DragDropItemsList.clear	();

	m_fItemsScaleX				= 1.0f;
	m_fItemsScaleY				= 1.0f;

	m_iColsNum = m_iRowsNum		= 0;

	m_flags.zero				();
	m_flags.set					(flCustomPlacement,		FALSE);
	m_flags.set					(flBlockCustomPlacement,FALSE);
	m_flags.set					(flScrollBarEnabled,	TRUE);
	m_flags.set					(flGridVisible,			TRUE);
	m_flags.set					(flUnlimitedCapacity,	FALSE);
	m_flags.set					(flNeedScrollToTop,		FALSE);
	m_flags.set					(flNeedScrollRecalculate,FALSE);
}

CUIDragDropList::~CUIDragDropList()
{
	
	DetachAll					();

	m_vGridState.clear			();
	delete_data					(m_vpCellStatic);
	m_DragDropItemsList.clear	();
	

}

void CUIDragDropList::SortList(DD_Check_func f){	
	xr_list<CUIDragDropItem*>::iterator it;
	
	xr_list<CUIDragDropItem*> copy = m_DragDropItemsList;
	
	for (it = copy.begin(); it != copy.end(); it++)
		DetachChild(*it);

	copy.sort(f);

	for (it = copy.begin(); it != copy.end(); it++)
		AttachChild(*it);
}

void CUIDragDropList::AttachChild(CUIWindow* pChild)
{	
	CUIDragDropItem* pDragDropItem = smart_cast<CUIDragDropItem*>(pChild);
	if(pDragDropItem) 
	{
		PlaceItemInGrid(pDragDropItem);


		pDragDropItem->SetCellWidth		(GetCellWidth());
		pDragDropItem->SetCellHeight	(GetCellHeight());

		m_DragDropItemsList.push_back	(pDragDropItem);

		pDragDropItem->ClipperOn		();
		ScrollBarRecalculate			(true);
	}

	inherited::AttachChild(pChild);
}
void CUIDragDropList::DetachChild(CUIWindow* pChild)
{
	if( IsChild(pChild) )
	{

		CUIDragDropItem* pDragDropItem = smart_cast<CUIDragDropItem*>(pChild);
		if(pDragDropItem)
		{
			RemoveItemFromGrid(pDragDropItem);
			m_DragDropItemsList.remove(pDragDropItem);
			ScrollBarRecalculate(false);
			HighlightAllCells(false);
		}
	}

	inherited::DetachChild(pChild);
}

void CUIDragDropList::DropAll()
{
	CUIWindow::DetachAll();
	m_DragDropItemsList.clear();

	if (GetParent())
		GetParent()->SetCapture(this, false);

	m_pMouseCapturer = NULL;


	for(u32 i=0; i<m_vpCellStatic.size(); ++i)
	{
		AttachChild(m_vpCellStatic[i]);
	}
	AttachChild(&m_ScrollBar);


	int k,m;

	for(k=0; k<GetRows(); ++k)
	{
		for(m=0; m<GetCols(); ++m)
		{
			if(k>=m_iRowsNum || m>=m_iColsNum)
				Msg("CUIDragDropList::DropAll failed. row=[%d] col=[%d] row_num=[%d] col_num=[%d]",k,m,m_iRowsNum,m_iColsNum);
			GetCell(k, m) = CELL_EMPTY;
		}
	}

	ScrollBarRecalculate(true);
}

bool CUIDragDropList::OnMouse(float x, float y, EUIMessages mouse_action)
{
	switch(mouse_action){
	case WINDOW_MOUSE_WHEEL_DOWN:
		if( m_flags.test(flScrollBarEnabled) )
			m_ScrollBar.TryScrollInc();
			return true;
			break;
	case WINDOW_MOUSE_WHEEL_UP:
		if( m_flags.test(flScrollBarEnabled) )
			m_ScrollBar.TryScrollDec();
			return true;
			break;
	}

	return inherited::OnMouse(x, y, mouse_action);
}

//обработка сообщений дл€ DragDrop
void CUIDragDropList::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(pWnd == &m_ScrollBar)
	{
		if(msg == SCROLLBAR_VSCROLL)
		{
			m_iCurrentFirstRow = m_ScrollBar.GetScrollPos();
			UpdateList();
			// “еперь надо переинициализировать подсветку активного элемента
			HighlightAllCells(false);
			GetMessageTarget()->SendMessage(this, DRAG_DROP_REFRESH_ACTIVE_ITEM , NULL);
		}
	}
	else if(smart_cast<CUIDragDropItem*>(pWnd) && msg == DRAG_DROP_ITEM_DRAG)
	{
		if( IsChild(pWnd) ) 
		{
			//подн€ть окно вместе с родител€ми на вершину
			pWnd->BringAllToTop(); 

			pWnd->Show(true);

			//выбросить элемент из сетки
			RemoveItemFromGrid((CUIDragDropItem*)pWnd);
			smart_cast<CUIDragDropItem*>(pWnd)->Highlight(false);
			
		}
	}
	else if(smart_cast<CUIDragDropItem*>(pWnd) && msg == DRAG_DROP_ITEM_DROP)
	{	
		CUIDragDropItem* pItem = (CUIDragDropItem*)pWnd;

		if( !IsChild(pWnd) ) 
		{
			Frect rect = GetAbsoluteRect();
			Fvector2 pt_lt, pt_lb, pt_rt, pt_rb;
			Frect wnd_rect = pWnd->GetAbsoluteRect();
			
			pt_lt.x = wnd_rect.left;
			pt_lt.y = wnd_rect.top;

			pt_lb.x = wnd_rect.left;
			pt_lb.y = wnd_rect.bottom;

			pt_rt.x = wnd_rect.right;
			pt_rt.y = wnd_rect.top;

			pt_rb.x = wnd_rect.right;
			pt_rb.y = wnd_rect.bottom;		

			Fvector2 pt_center;
			pt_center.x = (wnd_rect.right+
						   wnd_rect.left)/2;
			pt_center.y = (wnd_rect.top+
						   wnd_rect.bottom)/2;;


			if( rect.in(pt_center) || pItem->NeedMoveWithoutRectCheck())
			{
				////если есть куда брать и работает доп. проверка
				OnCustomPlacement();
				int place_row, place_col;


				//доп. проверка при помощи присоединенной функции
				bool additional_check;

				if(GetCheckProc())
					additional_check = (*(GetCheckProc()))(pItem, this);
				else
					additional_check = true;

				pItem->GetParent()->SetCapture(pItem, false);

				if(additional_check)
				{

					bool placeExists = false;
					if ( m_flags.test(flUnlimitedCapacity) )
					{
						while (!CanPlaceItemInGrid(pItem, place_row, place_col))
						{
							InitGrid(GetRows() + 1, GetCols(), !!m_flags.test(flGridVisible), m_iViewRowsNum);
						}
						placeExists = true;
					}
					else
					{
						placeExists = CanPlaceItemInGrid(pItem, place_row, place_col);
					}

					if (placeExists)
					{	
						//отсоединить у прошлого родител€
						pItem->GetParent()->SetCapture(pItem, false);
						pItem->GetParent()->DetachChild(pItem);
						AttachChild(pItem);
						pItem->Rescale(m_fItemsScaleX,m_fItemsScaleY);
						pItem->Highlight(true);

						pItem->BringAllToTop(); 


					}
					else
					{
						RemoveItemFromGrid(pItem);
					}
				}
				OffCustomPlacement();
			}
		}
		//элемент наш, вернуть его на место где был раньше
		else
		{
			OnCustomPlacement();
			PlaceItemInGrid(pItem);
			OffCustomPlacement();

			ScrollBarRecalculate(false);

			pItem->GetParent()->SetCapture(pItem, false);
			pItem->Highlight(true);
			// ѕросигнализировать о том, что если это был костюм, то надо его оп€ть спр€тать
			pItem->GetMessageTarget()->SendMessage(pItem , OUTFIT_RETURNED_BACK, NULL);
			UpdateList();
		}
	}

//	CUIWindow::SendMessage(pWnd, msg, pData);
}



void CUIDragDropList::Init(float x, float y, float width, float height)
{
	inherited::Init(x, y, width, height);
	ScrollBarRecalculate(true);
	m_ScrollBar.Show(false);
}

//инициализаци€ сетки Drag&Drop
void CUIDragDropList::InitGrid(int iRowsNum, int iColsNum, 
							   bool bGridVisible, int iViewRowsNum)
{
	OffCustomPlacement();

	m_iColsNum=iColsNum;
	m_iRowsNum=iRowsNum;

	if(iViewRowsNum>=m_iRowsNum || iViewRowsNum == 0)
	{
        m_iViewRowsNum = m_iRowsNum;
		m_ScrollBar.Show(false);
		m_ScrollBar.Enable(false);
	}
	else
	{
		m_iViewRowsNum = iViewRowsNum;

		//инициализировать скроллинг
		m_ScrollBar.SetRange(0, GetRows()-1);
		m_ScrollBar.SetPageSize(m_iViewRowsNum);
		m_ScrollBar.SetScrollPos(0);
	}



	SetWidth(float(m_iColsNum*GetCellWidth()) + m_ScrollBar.GetWidth() + SCROLLBAR_OFFSET_X);
	SetHeight(float(GetViewRows()*GetCellHeight()));

	if (!IsChild(&m_ScrollBar))
		AttachChild(&m_ScrollBar);
	m_ScrollBar.Init(GetWidth() - m_ScrollBar.GetWidth(), SCROLLBAR_OFFSET_Y, 
					GetHeight(), false);

	m_vGridState.resize(m_iRowsNum*m_iColsNum, CELL_EMPTY);

	m_flags.set(flGridVisible,bGridVisible);
	if(bGridVisible)
	{
		ReallocateCells(GetViewRows()*m_iColsNum);

		int i,j;

		CELL_STATIC_IT it=m_vpCellStatic.begin();

		for(i=0; i<GetViewRows(); ++i){
			for(j=0; j<GetCols(); ++j){

				(*it)->Init(CELL_TEXTURE,
								float(j*(GetCellWidth())),
								float(i*(GetCellHeight())),
								float(GetCellWidth()),
								float(GetCellHeight()));
				(*it)->GetUIStaticItem().SetOriginalRect(0.0f, 0.0f, 52.0f, 52.0f);
				(*it)->ClipperOn();
				(*it)->SetStretchTexture(true);

				if (!IsChild( *it ) )
					AttachChild( *it );
				
				++it;
			}
		}

	}
}

//размещение элемента на свободном месте в сетке
bool CUIDragDropList::PlaceItemInGrid(CUIDragDropItem* pItem)
{
	int place_row = -1,  place_col = -1;
	bool found_place = false;

	//разместить элемент на найденном месте если лист с бесконечной вместимостью
	if ( m_flags.test(flUnlimitedCapacity) )
	{
		while (!CanPlaceItemInGrid(pItem, place_row, place_col))
		{
			InitGrid(GetRows() + 7, GetCols(), !!m_flags.test(flGridVisible), m_iViewRowsNum);
		}
		found_place = true;
	}
	else
	{
		found_place = CanPlaceItemInGrid(pItem, place_row, place_col);
	}
	if (found_place)
	{
		PlaceItemAtPos(place_row, place_col, pItem);
		GetMessageTarget()->SendMessage(this, DRAG_DROP_ITEM_PLACED, NULL);
	}
	return found_place;
}

//размещение элемента на свободном месте в сетке
bool CUIDragDropList::CanPlaceItemInGrid(CUIDragDropItem* pItem, int& place_row, int& place_col)
{
	R_ASSERT(pItem->GetGridHeight()>0);
	R_ASSERT(pItem->GetGridWidth()>0);

	R_ASSERT(GetRows()>0);
	R_ASSERT(GetCols()>0);

	int i,j;

	place_row = -1;
	place_col = -1;

	bool found_place;
	bool can_place;

	found_place = false;

	//провер€ем, можно ли разместить элемент на том месте на 
	//котором он сейчас
	Frect item_rect		= pItem->GetAbsoluteRect();
	Frect rect			= GetAbsoluteRect();

	if(item_rect.left - rect.left<0)
		place_col = 0;
	else if(item_rect.right - rect.right>=0)
		place_col = GetCols()-pItem->GetGridWidth();

	if(item_rect.top - rect.top<0)
		place_row = m_iCurrentFirstRow;
	else if(item_rect.bottom - rect.bottom>=0)
		place_row = m_iCurrentFirstRow + GetViewRows()- pItem->GetGridHeight();


	int item_center_x = iFloor(pItem->GetWidth()/2 + item_rect.left - rect.left);
	int item_center_y = iFloor(pItem->GetHeight()/2 + item_rect.top - rect.top);


	if(place_col == -1)
	{
		place_col = iFloor((float)item_center_x/GetCellWidth() - (float)pItem->GetGridWidth()/2  + .1f);
	}
	if(place_row == -1)
	{
		place_row = m_iCurrentFirstRow + 
			iFloor((float)item_center_y/GetCellHeight()
			- (float)pItem->GetGridHeight()/2  + .5f);
	}


	if(place_col<0)
		place_col = 0;
	if(place_col>=GetCols())
		place_col = GetCols()- 1 - pItem->GetGridWidth()/2;
	if(place_row<0)
		place_row = 0;
	if(place_row>=GetRows())
		place_row = GetRows()- 1 - pItem->GetGridHeight()/2;


	if(GetCustomPlacement() && !IsCustomPlacementBlocked())
	{
		if(CanPlace(place_row, place_col, pItem))
		{
			found_place = true;
		}
		else if(CanPlace(place_row, place_col+1, pItem))
		{
			place_row;
			++place_col;
			found_place = true;
		}
		else if(CanPlace(place_row+1, place_col, pItem))
		{
			++place_row;
			place_col;
			found_place = true;
		}
		else if(CanPlace(place_row-1, place_col, pItem))
		{
			--place_row;
			place_col;
			found_place = true;
		}
		else if(CanPlace(place_row, place_col-1, pItem))
		{
			place_row;
			--place_col;
			found_place = true;
		}
	}


	//проверить можно ли разместить элемент,
	//провер€ем последовательно каждую клеточку

	for(i=0; (i<GetRows()-pItem->GetGridHeight()+1) && !found_place; ++i)
	{
		for(j=0; (j<GetCols()-pItem->GetGridWidth()+1) && !found_place; ++j)
		{
			can_place = CanPlace(i,j, pItem);

			if(can_place)
			{
				found_place=true;	
				place_row = i;
				place_col = j;
			}

		}
	}

	return found_place;
}



void CUIDragDropList::PlaceItemAtPos(int place_row, int place_col, CUIDragDropItem* pItem)
{
	int k,m;

	for(k=0; k<pItem->GetGridHeight(); ++k)
	{
		for(m=0; m<pItem->GetGridWidth(); ++m)
		{
			if(place_row+k>=m_iRowsNum || place_col+m>=m_iColsNum)
				Msg("CUIDragDropList::PlaceItemAtPos failed. row=[%d] col=[%d] row_num=[%d] col_num=[%d]",place_row+k,place_col+m,m_iRowsNum,m_iColsNum);
			GetCell(place_row+k, place_col+m) = CELL_FULL;
		}
	}

	pItem->SetGridRow(place_row);
	pItem->SetGridCol(place_col);


	//разместить само окно элемента
	pItem->SetWndPos(	float(place_col*GetCellWidth()),
						float((place_row-m_iCurrentFirstRow)*GetCellHeight()));

	pItem->Rescale(m_fItemsScaleX,m_fItemsScaleY);

	pItem->m_bInFloat = false;
}


bool CUIDragDropList::CanPlace(int row, int col, CUIDragDropItem* pItem)
{
	int k,m;

	
	if(row<0 || col<0)
		return false;
	if(row+pItem->GetGridHeight()-1 >= GetRows())
		return false;
	if(col+pItem->GetGridWidth()-1  >= GetCols())
		return false;



	for(k=0; k<pItem->GetGridHeight(); ++k)
	{
		for(m=0; m<pItem->GetGridWidth(); ++m)
		{
			if(GetCellState(row+k, col+m) == CELL_FULL)
				return false;
		}
	}
	
	return true;
}

//удаление элемента из сетки
void CUIDragDropList::RemoveItemFromGrid(CUIDragDropItem* pItem)
{
	if (pItem->m_bInFloat) return;
	pItem->m_bInFloat = true;

	int k, m;

	int place_row = pItem->GetGridRow();
	int place_col = pItem->GetGridCol();

    for(k=0; k<pItem->GetGridHeight(); ++k)
	{
		for(m=0; m<pItem->GetGridWidth(); ++m)
		{
			if(place_row+k>=m_iRowsNum || place_col+m>=m_iColsNum)
				Msg("CUIDragDropList::RemoveItemFromGrid failed. row=[%d] col=[%d] row_num=[%d] col_num=[%d]",place_row+k,place_col+m,m_iRowsNum,m_iColsNum);
			GetCell(place_row+k, place_col+m) = CELL_EMPTY;
		}
	}
}

E_CELL_STATE CUIDragDropList::GetCellState(int row, int col) 
{
	if(row<GetRows() && col<GetCols())
		return m_vGridState[row*GetCols() + col];
	else
		return CELL_FULL;
}


void CUIDragDropList::Draw()
{
	UI()->PushScissor	(GetAbsoluteRect());
	inherited::Draw();
	UI()->PopScissor	();
}
void CUIDragDropList::Update()
{
	if( m_flags.test(flNeedScrollRecalculate) )
		ScrollBarRecalculateTotal();

	inherited::Update();
}

void CUIDragDropList::UpdateList()
{
	for(WINDOW_LIST_it it = m_ChildWndList.begin(); m_ChildWndList.end() != it; ++it)
	{
		CUIDragDropItem* pDragDropItem = smart_cast<CUIDragDropItem*>(*it);
		if(pDragDropItem)
		{
			float y = float((pDragDropItem->GetGridRow()-m_iCurrentFirstRow)*GetCellHeight());
			float x = float((pDragDropItem->GetGridCol())*GetCellWidth());
			pDragDropItem->SetWndPos(x, y);
		}
	}
}

//установление позиции скролинга
void CUIDragDropList::SetScrollPos(int iScrollPos)
{
	m_ScrollBar.SetScrollPos(iScrollPos);
	m_iCurrentFirstRow = m_ScrollBar.GetScrollPos();
	UpdateList();
}
int CUIDragDropList::GetScrollPos()
{
	return m_iCurrentFirstRow;
}
void CUIDragDropList::DetachAll()
{
	inherited::DetachAll();
}

void CUIDragDropList::EnableScrollBar(bool enable)
{
	m_flags.set(flScrollBarEnabled,enable);

	if(enable)
	{
		m_ScrollBar.Enable(true);
		m_ScrollBar.Show(true);
	}
	else
	{
		m_ScrollBar.Enable(false);
		m_ScrollBar.Show(false);
	}

}

bool CUIDragDropList::CanPlaceItem(CUIDragDropItem *pDDItem)
{
	int a, b;
	return CanPlaceItemInGrid(pDDItem, a, b);
}

//////////////////////////////////////////////////////////////////////////

void CUIDragDropList::SetItemsScaleXY(float fItemsScaleX, float fItemsScaleY)
{
	R_ASSERT(fItemsScaleX > 0 && fItemsScaleY > 0);

	m_fItemsScaleX = fItemsScaleX;
	m_fItemsScaleY = fItemsScaleY;
}

//////////////////////////////////////////////////////////////////////////

int CUIDragDropList::GetLastBottomFullCell()
{
	for (int i = GetRows() - 1; i >= 0; --i)
	{
		for (int j = 0; j < GetCols(); ++j)
		{
			if (CELL_FULL == GetCellState(i, j))
				return i;
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
void CUIDragDropList::ScrollBarRecalculateTotal()
{
	const int bottom = GetLastBottomFullCell() ;

	// ≈сли все элементы помещаютс€ в видимую область, то скроллбар не требуетс€
	if (bottom < GetViewRows())
	{
		if (m_iCurrentFirstRow != 0)
			SetScrollPos(0);
		EnableScrollBar(false);
	}
	else
	{
		// »наче требуетс€ и необходим пересчет
		EnableScrollBar(true);

		int min, max;
		m_ScrollBar.GetRange(min, max);
		m_ScrollBar.SetRange(0, bottom);
		m_ScrollBar.SetPageSize(m_iViewRowsNum);
//.		
		if (max > bottom) 								SetScrollPos(m_ScrollBar.GetScrollPos() - max + bottom);

		if ( m_flags.test(flNeedScrollToTop) )			SetScrollPos(0);
	}
	UpdateList();
	m_flags.set(flNeedScrollRecalculate,FALSE);
}

void CUIDragDropList::ScrollBarRecalculate(bool needScrollToTop)
{
	m_flags.set				(flNeedScrollRecalculate,TRUE);
	m_flags.set				(flNeedScrollToTop,needScrollToTop);
}

//////////////////////////////////////////////////////////////////////////

void CUIDragDropList::HighlightCell(int row, int col, bool on)
{
	R_ASSERT(row < m_iRowsNum && col < m_iColsNum);

	if (static_cast<u32>(row*GetCols() + col) >= m_vpCellStatic.size()) return;
	CUIStatic * cell = m_vpCellStatic[row*GetCols() + col];
	cell->GetUIStaticItem().SetOriginalRect(static_cast<int>(on) * 64.0f, 0.0f, 52.0f, 52.0f);
}

//////////////////////////////////////////////////////////////////////////

void CUIDragDropList::HighlightAllCells(bool on)
{
	for (int i = 0; i < m_iRowsNum; ++i)
	{
		for (int j = 0; j < m_iColsNum; ++j)
		{
			HighlightCell(i, j, on);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIDragDropList::RearrangeItems()
{
	for (u32 i = 0; i < m_vGridState.size(); ++i)
		m_vGridState[i] = CELL_EMPTY;

	for (DRAG_DROP_LIST_it it = m_DragDropItemsList.begin(); it != m_DragDropItemsList.end(); ++it)
	{
		PlaceItemInGrid(*it);
	}
	ScrollBarRecalculate(true);
}

int	CUIDragDropList::ReallocateCells(u32 new_size)
{
	int res = new_size - m_vpCellStatic.size() ;
	if( new_size>m_vpCellStatic.size() )
		while(m_vpCellStatic.size() != new_size ){
			m_vpCellStatic.push_back( xr_new<CUIStatic>() );
			++res;
			}
	else
		while(m_vpCellStatic.size() != new_size ){
			xr_delete(m_vpCellStatic.back());
			m_vpCellStatic.pop_back();
			}
	
	return res;
}
