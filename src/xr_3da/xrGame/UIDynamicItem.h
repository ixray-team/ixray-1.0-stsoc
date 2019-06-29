#ifndef __XR_UIDYNAMICITEM_H__
#define __XR_UIDYNAMICITEM_H__
#pragma once
/*
#include "uicustomitem.h"

struct SDynamicItemData{
	Ivector2		pos;
	u32			color;
	float		angle;
};

DEFINE_VECTOR(SDynamicItemData,DIDVec,DIDIt);

class CUIDynamicItem: public CUICustomItem
{
	ref_shader		hShader;
	ref_geom		hGeom;	
		
	DIDVec			data;
	u32				item_cnt;

	int				offset_x, offset_y;
protected:
	typedef CUICustomItem inherited;
public:
					CUIDynamicItem	();
	virtual			~CUIDynamicItem	();
	void			Init			(LPCSTR tex, LPCSTR sh);
	void			SetOffset		(int x, int y) {offset_x = x; offset_y = y;}
	void			Out				(int left, int top, u32 color, float angle = 0.f);
	void			Clear			(){item_cnt=0;}
	void			Render			();
	void			Render			(float angle);
	u32				size			(){return item_cnt;}
};
*/
#endif //__XR_UIDYNAMICITEM_H__
