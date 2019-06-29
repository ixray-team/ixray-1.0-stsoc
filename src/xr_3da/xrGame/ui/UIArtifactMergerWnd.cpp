// UIArtefactMergerWnd.cpp:  подменю для работы с аппаратом 
// производства новых артефактов
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
/*
#include "UIArtifactMergerWnd.h"

#include "../Artifact.h"

CUIArtefactMerger::CUIArtefactMerger()
{
}
CUIArtefactMerger::~CUIArtefactMerger()
{
}
void CUIArtefactMerger::Init(int x, int y, int width, int height)
{

	inherited::Init(x, y, width, height);
	inherited::InitTexture("ui_frame");

	AttachChild(&UIArtefactList);
	UIArtefactList.Init(10,10,100,100);
	UIArtefactList.SetCellHeight(50);
	UIArtefactList.SetCellWidth(50);
	UIArtefactList.InitGrid(4, 4, true, 4);
	
	UIArtefactList.SetCheckProc(ArtefactProc);


	AttachChild(&UIPerformButton);
	UIPerformButton.Init("ui\\ui_button_01", 10, 250,150,40);
	UIPerformButton.SetText("perform");

	AttachChild(&UICloseButton);
	UICloseButton.Init("ui\\ui_button_01", 10, 300,150,40);
	UICloseButton.SetText("close");
}
void CUIArtefactMerger::InitArtefactMerger(CArtefactMerger* pArtefactMerger)
{
	m_pArtefactMerger = pArtefactMerger;
	R_ASSERT(m_pArtefactMerger);

	m_pArtefactMerger->RemoveAllArtefacts();

	UIArtefactList.DropAll();
	ResetAll();
}

void CUIArtefactMerger::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(pWnd == &UIPerformButton && msg == BUTTON_CLICKED)
	{
		PerformArtefactMerger();
		GetMessageTarget()->SendMessage(this, ARTEFACT_MERGER_PERFORM_BUTTON_CLICKED);
	}
	else if(pWnd == &UICloseButton && msg == BUTTON_CLICKED)
	{
		//выкинуть все артефакты
		m_pArtefactMerger->RemoveAllArtefacts();

		GetMessageTarget()->SendMessage(this, ARTEFACT_MERGER_CLOSE_BUTTON_CLICKED);
	}

	inherited::SendMessage(pWnd, msg, pData);
}



void CUIArtefactMerger::Show()
{
	inherited::Show(true);
	inherited::Enable(true);
}
void CUIArtefactMerger::Hide()
{
	inherited::Show(false);
	inherited::Enable(false);
}

void CUIArtefactMerger::Update()
{
	inherited::Update();
}

bool CUIArtefactMerger::ArtefactProc(CUIDragDropItem* pItem, CUIDragDropList*)
{
	PIItem pInvItem = (PIItem)pItem->GetData();

	if(smart_cast<CArtefact*>(pInvItem))
		return true;
	else
		return false;
}

void CUIArtefactMerger::PerformArtefactMerger()
{
	for(DRAG_DROP_LIST_it it = UIArtefactList.GetDragDropItemsList().begin(); 
 						  UIArtefactList.GetDragDropItemsList().end() != it;
						  ++it)
	{
		CUIDragDropItem* pDragDropItem = *it;
		PIItem pItem = (PIItem)pDragDropItem->GetData();

		m_pArtefactMerger->AddArtefact(smart_cast<CArtefact*>(pItem));
	}

	m_pArtefactMerger->PerformMerge();
	m_pArtefactMerger->RemoveAllArtefacts();



	//удалить из списка иконки тех артефактов, которые
	//изчезли при сочетании
	if(!m_pArtefactMerger->m_ArtefactDeletedList.empty())
	{
		for(ARTIFACT_LIST_it it = m_pArtefactMerger->m_ArtefactDeletedList.begin();
							 m_pArtefactMerger->m_ArtefactDeletedList.end() != it; 
							 ++it)
		{
		
			for(DRAG_DROP_LIST_it it1 = UIArtefactList.GetDragDropItemsList().begin(); 
 								  UIArtefactList.GetDragDropItemsList().end() != it1;
								  ++it1)
			{
				if((*it1)->GetData() == *it)
				{
					((CUIDragDropList*)(*it1)->GetParent())->DetachChild(*it1);
					it1 = UIArtefactList.GetDragDropItemsList().begin();	
				}
			}
		}

		m_pArtefactMerger->m_ArtefactDeletedList.clear();
	}
}

*/