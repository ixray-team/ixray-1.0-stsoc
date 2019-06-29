//=============================================================================
//  Filename:   UIJobsWnd.h
//	---------------------------------------------------------------------------
//  Jobs dialog subwindow in Diary window
//=============================================================================
#pragma once

#include "UIWindow.h"
#include "../GameTask.h"

class CUIListWnd;

class CUIJobsWnd: public CUIWindow
{
	typedef CUIWindow inherited;
public:

	CUIJobsWnd();
	virtual ~CUIJobsWnd();
	virtual void Init();
	virtual void Show(bool status);
	// Добавить 1 таск
	void AddTask(CGameTask * task);
	// Нет смысла скрывать листбокс, так как все равно пока это практически все, что есть, и
	// придется писать обертки над всеми управляющими функциями листбокса
	CUIListWnd*		UIList;
	// Загрузить jobs
	void ReloadJobs();

	// Установить фильтр
	void SetFilter(ETaskState newFilter);
protected:
	// Добавляем заголовок крупным шрифтом
	void AddHeader(shared_str strHeader);

	CGameFont			*pHeaderFnt;
	CGameFont			*pSubTasksFnt;

	// CurrentFilter
	ETaskState			filter;
};
