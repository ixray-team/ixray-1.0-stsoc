//////////////////////////////////////////////////////////////////////
// UIListWnd.h: окно со списком
//////////////////////////////////////////////////////////////////////

#ifndef _UI_LIST_WND_H_
#define _UI_LIST_WND_H_


#pragma once
#include "uiwindow.h"
#include "uilistitem.h"
#include "uiscrollbar.h"
//#include "UIXmlInit.h"

#include "../script_export_space.h"

#define DEFAULT_ITEM_HEIGHT 30


DEF_LIST (LIST_ITEM_LIST, CUIListItem*);
class CUIScrollBar;

class CUIListWnd :public CUIWindow
{
private:
	typedef CUIWindow inherited;
	friend class CUIGameLog;
	shared_str		m_scrollbar_profile;
public:
	CUIListWnd();
	virtual ~CUIListWnd();

	virtual void Init(float x, float y, float width, float height);
	virtual void Init(float x, float y, float width, float height, float item_height);

	virtual bool OnMouse		(float x, float y, EUIMessages mouse_action);
	virtual bool OnKeyboard		(int dik, EUIMessages keyboard_action);

	//сообщения, отправляемые родительскому окну
//	typedef enum{LIST_ITEM_CLICKED} E_MESSAGE;
			 
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void* pData);
	virtual void Draw();
	virtual void Update();
	virtual void DetachChild			(CUIWindow* pChild);
			void	SetScrollBarProfile	(LPCSTR profile)		{m_scrollbar_profile=profile;};

	// Добавление элементов в листбокс
	template <class Element>
	bool AddItem			(const char*  str, const float shift = 0.0f, void* pData = NULL,
							 int value = 0, int insertBeforeIdx = -1);

//	virtual bool AddText_script (LPCSTR str, int shift, u32 color, CGameFont* pFont, bool doPreProcess);
	virtual bool AddItem_script(CUIListItem* item);

	//template <class Element>
	//bool AddParsedItem		(const CUIString &str, const float shift,
	//						 const u32 &MsgColor, CGameFont* pFont = NULL,
	//						 void* pData = NULL, int value = 0, int insertBeforeIdx = -1);

	//////////////////////////////////////////////////////////////////////////
	// Добавить элемент в лист
	// Params:
	// 1.	pItem - указатель на элемент добавления
	// 2.	insertBeforeIdx - индекс элемента перед которым вставляем.
	//		-1 означает добавить в конец
	//////////////////////////////////////////////////////////////////////////
	
	template <class Element>
	bool AddItem			(Element* pItem, int insertBeforeIdx = -1);
	
	void RemoveItem(int index);
	void RemoveAll();
	//находит первый элемент с заданной pData, иначе -1
	int FindItem(void* pData);
	int FindItemWithValue(int iValue);
	CUIListItem* GetItem(int index);
	// Получить индекс элемента по адресу. Либо -1 если нет такого
	int GetItemPos(CUIListItem *pItem);

	int GetSize();

	void SetItemWidth(float iItemWidth);
	float GetItemWidth() {return m_iItemWidth;}

	void SetItemHeight(float iItemHeight); 
	float GetItemHeight() {return m_iItemHeight;}
    virtual void SetHeight(float height);

	void SetAlwaysShowScroll(bool flag = true)	{m_bAlwaysShowScroll = flag;}
	void EnableAlwaysShowScroll(bool flag)		{m_bAlwaysShowScroll_enable = flag;}


	int	GetItemsCount()	{return m_ItemList.size();}

	//подготовить все элементы заново
	void Reset();

	void EnableScrollBar(bool enable);
	bool IsScrollBarEnabled();
	void UpdateScrollBar();

	void ScrollToBegin();
	void ScrollToEnd();
	void ScrollToPos(int position);
	
	bool IsActiveBackgroundEnabled() {return m_bActiveBackgroundEnable;}
	void EnableActiveBackground(bool enable) {m_bActiveBackgroundEnable = enable;}

	//возвращает ширину в пикселях самой длиной надписи на элементах
	int GetLongestSignWidth();

	virtual void SetWidth(float width);

	void SetTextColor(u32 color) {m_dwFontColor = color;} 
	u32 GetTextColor() {return m_dwFontColor;}

	//делает активными (как кнопки) элементы списка
	void ActivateList(bool activity);
	bool IsListActive() {return m_bListActivity;}

	void SetVertFlip(bool vert_flip) {m_bVertFlip = vert_flip;}
	bool GetVertFlip() {return m_bVertFlip;}

	// Принудительная установка фокуса
	void SetFocusedItem(int iNewFocusedItem);
	int GetFocusedItem() { return m_iFocusedItem; }
	int GetSelectedItem() { return m_iSelectedItem; }
	void SetSelectedItem(int sel) {m_iSelectedItem = sel;}
	void ShowSelectedItem(bool show = true)	{ m_bShowSelectedItem = show;}
	
	void ResetFocusCapture() { m_bForceFocusedItem = false; }

	// Интерактивная запись представляет собой 
	// запись с полями, которые можно нажать. Для создания интерактивной записи входная строка 
	// должна иметь вид: "инфа %интерактивный элемент% инфа2 %интерактивный элемент№2%...",
	// где '%' - сSeparatorChar (разделитель).
	// Поля заключенные в разделители будут интерактивными элементками, на которые можно нажать.
	// Создавать мультистрочные интерактивные поля (на манер AddParsedItem) нельзя.

	// Для создания обычного поля, строка записи должна быть без четного количества знаков "%"
	// NOTE: До инициализации рендеринг контекста, строки не добавляють! Причина: невозможно получить 
	// ширину надписи в пискелях
	// Возвращаем вектор уникальных идентификаторов для интерактивных элементов в строке, которые нам 
	// будут посылаться в поле pData при нажатии на интерактивный элемент
	//xr_vector<int> AddInteractiveItem(const char *str2, const float shift = 0.0f,
	//	const u32 &MsgColor = 0xffffffff, CGameFont* pFont = 0, int pushAfter = -1);

	void	SetNewRenderMethod	(bool value)	{ m_bNewRenderMethod = value; }
	int		GetListPosition		() const		{ return m_iFirstShownIndex; }


protected:

	//полоса прокрутки
	CUIScrollBar*	m_ScrollBar;

	//обновления елементов списка, вызвается
	//если произошли изменения
	void	UpdateList();

	//список элементов листа
	LIST_ITEM_LIST	m_ItemList; 

	//размеры элемента списка
	float		m_iItemHeight;
	float		m_iItemWidth;

	//количество рядов для элементов
	int		m_iRowNum;
	
	//индекс первого показанного элемента
	int		m_iFirstShownIndex;

	//элемент над которым курсор в данный момент или -1, если такого нет
	int		m_iFocusedItem;
	int		m_iFocusedItemGroupID;
	int     m_iSelectedItem;
	int     m_iSelectedItemGroupID;

	bool	m_bShowSelectedItem;
	bool	m_bAlwaysShowScroll_enable;
	bool	m_bAlwaysShowScroll;

	// Если хотим принудительно выставлять фокус, то поднять этот флаг
	bool	m_bForceFocusedItem;

	//подсветка активного элемента
	CUIStaticItem	m_StaticActiveBackground;
	bool	m_bActiveBackgroundEnable;

	//текущий цвет текста
	u32		m_dwFontColor;
	bool	m_bListActivity;

	//переворот списка по вертикали
	bool	m_bVertFlip;
	
	// Признак того, что мышь подвинули
	bool	m_bUpdateMouseMove;

	// Текущий уникальный идентификатор
	int		m_iLastUniqueID;
	bool	m_bNewRenderMethod;

	DECLARE_SCRIPT_REGISTER_FUNCTION

	// Вспомогательная функци нахождения начала следующего слова начиная с текущей позиции
	LPSTR	FindNextWord(LPSTR currPos) const;
	// Вспомогательная функци определения длинны строки до конца слова начиная с текущей позиции
	float		WordTailSize(LPCSTR currPos, CGameFont *font, int &charsCount) const;
	bool	IsEmptyDelimiter(const char c) const;
};

#include "UIListWnd_inline.h"

add_to_type_list(CUIListWnd)
#undef script_type_list
#define script_type_list save_type_list(CUIListWnd)

#endif //_UI_LIST_WND_H_