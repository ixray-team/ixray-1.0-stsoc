// UIWpnDragDropItem.h: специальный класс перетаскиваемой картинки
//						переделанный для оружия с апгрейдами
//////////////////////////////////////////////////////////////////////

#ifndef _UI_WPN_DRAG_DROP_ITEM_H_
#define _UI_WPN_DRAG_DROP_ITEM_H_

#pragma once
#error "this file removed from project !!!"

#include "../weapon.h"

#include "UIDragDropItem.h"

class CUIWpnDragDropItem : public CUIDragDropItem
{
private:
	typedef CUIDragDropItem inherited;
public:
	CUIWpnDragDropItem();
	virtual ~CUIWpnDragDropItem();

	virtual void ClipperOn();
	virtual void ClipperOff();

	virtual void SetData(void* pData);
	virtual void Draw();
	virtual void Update();
	virtual void Rescale(float, float);
///.	virtual void SetTextureScaleXY(float x, float y);


protected:
			void UpdateGridWidth					(bool b);

	int iSilencerGridWidth,iSilencerGridHeight;
	int iScopeGridWidth,iScopeGridHeight;
	int iLauncherGridWidth,iLauncherGridHeight;

	CUIStaticItem m_UIStaticGrenadeLauncher;
	CUIStaticItem m_UIStaticScope;
	CUIStaticItem m_UIStaticSilencer;

	CWeapon* m_pWeapon;
};

#endif // _UI_WPN_DRAG_DROP_ITEM_H_
