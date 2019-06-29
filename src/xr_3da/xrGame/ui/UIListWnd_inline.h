//=============================================================================
//  Filename:   UIListWnd_inline.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Реализация темплейтовых функций листбокса
//=============================================================================

template <class Element>
bool CUIListWnd::AddItem(const char*  str, const float shift, void* pData,
						 int value, int insertBeforeIdx)
{
	//создать новый элемент и добавить его в список
	Element* pItem = NULL;
	pItem = xr_new<Element>();

	VERIFY(pItem);

	pItem->Init(str, shift, m_bVertFlip?GetHeight()-GetSize()* m_iItemHeight-m_iItemHeight:GetSize()* m_iItemHeight, 
		m_iItemWidth, m_iItemHeight);

	pItem->SetData(pData);
	pItem->SetValue(value);
	pItem->SetTextColor(m_dwFontColor);

	return AddItem<Element>(pItem, insertBeforeIdx);
}

//////////////////////////////////////////////////////////////////////////

//template <class Element>
//bool CUIListWnd::AddParsedItem(const CUIString &str, const float shift,
//							   const u32 &MsgColor, CGameFont* pFont,
//							   void* pData, int value, int insertBeforeIdx)
//{
//	bool	ReturnStatus	= true;
//	// I know, i know. :) But for optimization purposes I really need this.
//	// Actually I didn't change string contents. I only temorary replace some symbols
//	STRING&	text			= const_cast<STRING&>(str.m_str);
//
//	CGameFont *elementFont = (NULL == pFont) ? this->GetFont() : pFont;
//	R_ASSERT(elementFont);
//
//	int			GroupID				= GetSize();
//	float		lineTotalWidth		= this->GetItemWidth() - shift - this->GetRightIndention();
//	LPSTR		nextWord			= &text.front();
//	LPSTR		memorizedWord		= &text.front();
//	LPSTR		prevWord			= nextWord;
//	bool		wrapSentence		= false;
//	char		tmpChar				= 0;
//	float			lineLength			= 0;
//	float		wordLength			= 0;
//	float			totalWordLength		= 0;
//	int			wordLengthInChars	= 0;
//	int			symToSkip			= 0;
//	size_t		stringLength		= text.size();
//	int			spaceSize			= static_cast<int>(elementFont->SizeOf(' '));
//
//	while (prevWord < str + xr_strlen(str))
//	{
//		prevWord	= nextWord;
//		nextWord	= FindNextWord(nextWord);
//
//		if (!prevWord)
//		{
//			prevWord		= &text.front() + stringLength - 1;
//			wrapSentence	= true;
//		}
//
//		if (prevWord + 2 <= &text.front() + stringLength && (*prevWord == '\\' || *prevWord == '%'))
//		{
//			if (*(prevWord + 1) == 'n')
//			{
//				symToSkip		= 2;
//				wrapSentence	= true;
//				lineLength		= 0;
//			}
//
//		}
//
//		wordLength		= WordTailSize(prevWord + symToSkip, elementFont, wordLengthInChars);
//
//		if (0 != symToSkip && 0 != lineLength)
//		{
//			int dummy = 0;
//			totalWordLength = WordTailSize(prevWord, elementFont, dummy);
//		}
//		else
//		{
//			totalWordLength = wordLength;
//		}
//
//		if ( float(lineLength + wordLength) > lineTotalWidth)
//		{
//			wrapSentence	= true;
//			lineLength		= 0;
//		}
//
//		wordLength		=  (wordLength + (nextWord - prevWord - wordLengthInChars) * spaceSize + wordLength - totalWordLength);
//		lineLength		+= wordLength;
//
//		if (wrapSentence)
//		{
//			tmpChar					= *prevWord;
//			*prevWord				= 0;
//			ReturnStatus			&= AddItem<Element>(memorizedWord, shift, pData, value, insertBeforeIdx);
//			Element *pLocalItem		= smart_cast<Element*>(GetItem(GetSize() - 1));
//			pLocalItem->SetGroupID	(GroupID);
//			pLocalItem->SetTextColor(MsgColor);
//			pLocalItem->SetFont		(elementFont);
//			*prevWord				= tmpChar;
//			memorizedWord			= prevWord + symToSkip;
//			wrapSentence			= false;
//		}
//		symToSkip = 0;
//	}
//
//	return ReturnStatus;
//}

//////////////////////////////////////////////////////////////////////////

template <class Element>
bool CUIListWnd::AddItem(Element* pItem, int insertBeforeIdx)
{	
	AttachChild(pItem);

	pItem->Init(pItem->GetWndRect().left, m_bVertFlip?GetHeight()-GetSize()* m_iItemHeight-m_iItemHeight:GetSize()* m_iItemHeight, 
		m_iItemWidth, m_iItemHeight);


	//добавление в конец или начало списка
	if(-1 == insertBeforeIdx)
	{
		m_ItemList.push_back(pItem);
		pItem->SetIndex(m_ItemList.size()-1);
	}
	else
	{
		//изменить значения индексов уже добавленых элементов
		if (!m_ItemList.empty())
			R_ASSERT(static_cast<u32>(insertBeforeIdx) <= m_ItemList.size());

		LIST_ITEM_LIST_it it2 = m_ItemList.begin();
		std::advance(it2, insertBeforeIdx);
		for(LIST_ITEM_LIST_it it = it2; m_ItemList.end() != it; ++it)
		{
			(*it)->SetIndex((*it)->GetIndex()+1);
		}
		m_ItemList.insert(it2, pItem);
		pItem->SetIndex(insertBeforeIdx);
	}

	UpdateList();

	//обновить полосу прокрутки
	m_ScrollBar->SetRange(0,s16(m_ItemList.size()-1));
	m_ScrollBar->SetPageSize(s16(
		(u32)m_iRowNum<m_ItemList.size()?m_iRowNum:m_ItemList.size()));
	m_ScrollBar->SetScrollPos(s16(m_iFirstShownIndex));
//	m_ScrollBar.Refresh();

	UpdateScrollBar();

	return true;
}
