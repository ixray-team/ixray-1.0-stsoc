// UIArtefactMergerWnd.h:  подменю для работы с аппаратом 
// производства новых артефактов
//////////////////////////////////////////////////////////////////////
/*
#pragma once

#include "uiframewindow.h"
#include "uidragdroplist.h"
#include "uibutton.h"
#include "../ArtifactMerger.h"

class CUIArtefactMerger: public CUIFrameWindow
{
private:
	typedef CUIFrameWindow inherited;
public:
	CUIArtefactMerger();
	virtual ~CUIArtefactMerger();
	
	
	virtual void Init(int x, int y, int width, int height);

	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);

	void InitArtefactMerger(CArtefactMerger* pArtefactMerger);
	void PerformArtefactMerger();

	void Show();
	void Hide();

	virtual void Update();


	//места для артефактов в приборе
	CUIDragDropList		UIArtefactList;
protected:
	CUIButton UIPerformButton;
	CUIButton UICloseButton;

	static bool ArtefactProc(CUIDragDropItem* pItem, CUIDragDropList* pList);

	//указатель на устройство сочетания артефактов
	CArtefactMerger* m_pArtefactMerger;
};
*/