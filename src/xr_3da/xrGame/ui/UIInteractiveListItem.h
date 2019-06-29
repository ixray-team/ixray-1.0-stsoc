//=============================================================================
//  Filename:   UIInteractiveListItem.h
//	---------------------------------------------------------------------------
//  Representation of list item with many interactive fields
//=============================================================================

#ifndef UI_INTERACTIVE_LIST_ITEM_H_
#define UI_INTERACTIVE_LIST_ITEM_H_

#include "UIListItem.h"

// #pragma once

//=============================================================================
//  Интерактивный ListItem
//=============================================================================

class CUIInteractiveListItem: public CUIListItem
{
	// Служебная структура для представления интерактивного элемента
	typedef struct tagInteractiveItemData
	{
		tagInteractiveItemData(): ID(-1) {}
		// оконные координаты 
		std::pair<int, int>	pairScreenCrd;
		// текст интерактивного элемента
		xr_string			subStr;
		// идентификатор интерактивного подэлемента
		int					ID;
	} InteractiveItemData;
	// Inherited
	typedef CUIListItem inherited;
public:
	typedef xr_vector<InteractiveItemData> FIELDS_COORDS_VECTOR;
	typedef FIELDS_COORDS_VECTOR::iterator FIELDS_COORDS_VECTOR_it;
	typedef FIELDS_COORDS_VECTOR::const_iterator FIELDS_COORDS_VECTOR_c_it;
	// Сообщение
//	typedef enum { INTERACTIVE_ITEM_CLICK = 7011 } E_MESSAGE;

	CUIInteractiveListItem();
	virtual ~CUIInteractiveListItem() {}
	virtual bool OnMouse(float x, float y, EUIMessages mouse_action);
	virtual void Update();
	virtual void Draw();

	// Инициализируем поля из входного вектора данных. Каждая подстрока в векторе
	// заключенная в знаки-разделители является интерактивной. При нажатии на
	// кнопку соответствующая константа из массива pIDArr передается родитeльскому
	// окну
	virtual void Init(const char *str, const xr_vector<char *> &Data, xr_vector<int> &IDs, float height);
	//	// Задаем цвет текста неинтерактивной части поля
	//	virtual void SetStaticTextColor(u32 uColor) {}
	//	// Задаем цвет теста интерактивной части тесктового поля
	//	virtual void SetInteractiveTextColor(u32 uColor) {}
	// Переопределяем функцию получения абсолютных координат.
	// Это нужно для выделения интерактивных элементов
	virtual Frect GetAbsoluteSubRect();
	// Вкл/выкл интреактивное поведение
	bool	m_bInteractiveBahaviour;
	// Назначаем ID нужному элементу
	void SetIItemID(const u32 uIndex, const int ID);
	// получаем количество интерактивных элементов
	u32 GetIFieldsCount();
protected:
	// Координаты интерактивных полей в записи
	FIELDS_COORDS_VECTOR		vPositions;
	// Итератор на координаты текущего активного интерактивного элемента
	FIELDS_COORDS_VECTOR_c_it	itCurrIItem;
	// Текущая подстрока
	xr_string	sub_str;
private:
	// Бинарная функция для проверки попадания курсора мыши в диапазон
	struct mouse_hit: 
		public std::binary_function<FIELDS_COORDS_VECTOR::value_type, 
		std::pair<float, float>, 
		bool>
	{
		bool operator() (const FIELDS_COORDS_VECTOR::value_type &interactiveCrd,
			const std::pair<float, float> &mouseCrd) const
		{
			return interactiveCrd.pairScreenCrd.first <= mouseCrd.first && interactiveCrd.pairScreenCrd.second >= mouseCrd.first;
		}
	};
};

#endif