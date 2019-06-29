//=============================================================================
//  Filename:   UIJobsWnd.cpp
//	---------------------------------------------------------------------------
//  Jobs dialog subwindow in Diary window
//=============================================================================

#include "StdAfx.h"
#include "UIJobsWnd.h"
#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include "UIPdaWnd.h"
#include "UIJobItem.h"
#include "UIStatsWnd.h"
#include "UIInventoryUtilities.h"

#include "../GameTask.h"
#include "../Level.h"
#include "../HUDManager.h"
#include "../string_table.h"
#include "../alife_registry_wrappers.h"
#include "../actor.h"
#include "../gameTaskManager.h"
//////////////////////////////////////////////////////////////////////////

const char * const	JOBS_XML						= "jobs.xml";
const u32			uTaskIconSize					= 10;
const u32			clTaskSubItemColor				= 0xffe1e1fa;
const u32			clTaskHeaderColor				= 0xffffffff;
const float			secondaryObjectiveScaleFactor	= 0.8f;
const int			subitemsOffset					= 30;

//////////////////////////////////////////////////////////////////////////

CUIJobsWnd::CUIJobsWnd()
	:	filter	(eTaskStateDummy)
{
	pHeaderFnt		= HUD().Font().pFontLetterica18Russian;
	pSubTasksFnt	= HUD().Font().pFontLetterica16Russian;
	pHeaderFnt->SetColor(clTaskHeaderColor);
}

//////////////////////////////////////////////////////////////////////////

CUIJobsWnd::~CUIJobsWnd()
{

}

//////////////////////////////////////////////////////////////////////////

void CUIJobsWnd::Init()
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, JOBS_XML);
	R_ASSERT3(xml_result, "xml file not found", JOBS_XML);

	inherited::Init(0,0, UI_BASE_WIDTH, UI_BASE_HEIGHT);

	CUIXmlInit xml_init;

	UIList = xr_new<CUIListWnd>(); UIList->SetAutoDelete(true);
	AttachChild(UIList);
	xml_init.InitListWnd(uiXml, "list", 0, UIList);
	UIList->ActivateList(true);
	UIList->EnableScrollBar(true);
	UIList->EnableActiveBackground(false);
}

//////////////////////////////////////////////////////////////////////////

void CUIJobsWnd::AddTask(CGameTask * task)
{
	// Code guards
	R_ASSERT(task);
	R_ASSERT(task->m_Objectives.size() > 0);

	// Проверим на фильтре
	if (filter != eTaskStateDummy && task->m_Objectives[0].task_state != filter) return;

	CStringTable		stbl;
	SGameTaskObjective	*obj = NULL;
//	Frect				r;
	CUIJobItem			*pJobItem = NULL;
	u32					color = 0xffffffff;


	// Пробегаемся по таскам и заносим их как задания
	float tmp = 0;

	CActor* pActor = smart_cast<CActor*>(Level().CurrentEntity());

	for (u32 i = 0; i < task->m_Objectives.size(); ++i)
	{
		if (i > 0) tmp = subitemsOffset;

		pJobItem = xr_new<CUIJobItem>	(tmp);
		pJobItem->SetGameTask			(task, i);
		obj = &task->m_Objectives[i];
		UIList->AddItem<CUIListItem>	(pJobItem);
//		r.set(obj->icon_rect.x1, obj->icon_rect.y1, obj->icon_rect.x2, obj->icon_rect.y2);


		// Устанавливаем цвет иконок
		switch (task->m_Objectives[i].task_state)
		{
		case eTaskStateCompleted:
			color = 0xff00ff00;
			break;
		case eTaskStateFail:
			color = 0xffff0000;
			break;
		case eTaskStateInProgress:
			color = 0xffffffff;
			break;
		default:
			NODEFAULT;
		}

		if (0 != obj->icon_texture_name.size())
			pJobItem->SetPicture			(*obj->icon_texture_name, task->m_Objectives[i].icon_rect, color);


		if(i == 0)
			pJobItem->SetCaption				(*stbl(task->m_Title));
		pJobItem->SetDescription				(*stbl(task->m_Objectives[i].description));
/*		if (task->m_Objectives[i].article_id.size() )
		{
			pJobItem->SetAdditionalMaterialID	(task->m_Objectives[i].article_id);
		}*/
		if (i > 0) pJobItem->ScalePictureXY	(secondaryObjectiveScaleFactor,secondaryObjectiveScaleFactor);

		if(pActor && pActor->encyclopedia_registry->registry().objects_ptr())
		{
			for(ARTICLE_VECTOR::iterator it = pActor->encyclopedia_registry->registry().objects().begin();
				it != pActor->encyclopedia_registry->registry().objects().end(); it++)
			{
				if (task->m_Objectives[i].article_id == it->article_id)
				{
					if (ARTICLE_DATA::eEncyclopediaArticle == it->article_type)
						pJobItem->SetCallbackMessage(PDA_OPEN_ENCYCLOPEDIA_ARTICLE);
					else if (ARTICLE_DATA::eDiaryArticle == it->article_type)
						pJobItem->SetCallbackMessage(PDA_OPEN_DIARY_ARTICLE);
					break;
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIJobsWnd::ReloadJobs()
{
	UIList->RemoveAll();

	CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());

	GameTasks& tasks = pActor->GameTaskManager().GameTasks();
	GameTasks::iterator it =  tasks.begin();
	for(;it!=tasks.end();++it){
		AddTask((*it).game_task);
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIJobsWnd::Show(bool status)
{
	if (status)
	{
		ReloadJobs();

		InventoryUtilities::SendInfoToActor("ui_pda_jobs");

		switch (filter)
		{
		case eTaskStateCompleted:
			InventoryUtilities::SendInfoToActor("ui_pda_jobs_accomplished");
			break;
		case eTaskStateInProgress:
			InventoryUtilities::SendInfoToActor("ui_pda_jobs_current");
			break;
		case eTaskStateFail:
			InventoryUtilities::SendInfoToActor("ui_pda_jobs_failed");
			break;
		}
	}
	else
		InventoryUtilities::SendInfoToActor("ui_pda_jobs_hide");

	inherited::Show(status);
}

//////////////////////////////////////////////////////////////////////////

void CUIJobsWnd::SetFilter(ETaskState newFilter)
{
	filter = newFilter;

	switch (filter)
	{
	case eTaskStateCompleted:
		SetWindowName("Completed Jobs");
		break;
	case eTaskStateInProgress:
		SetWindowName("Active Jobs");
		break;
	case eTaskStateFail:
		SetWindowName("Failed Jobs");
		break;
	default:
		R_ASSERT(!"Unknown type of task state");
	}
}