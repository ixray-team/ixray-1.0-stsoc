// CustomItem.h: interface for the CCustomItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CUSTOMITEM_H__50C86D07_B38B_4155_B724_7904136B0166__INCLUDED_)
#define AFX_CUSTOMITEM_H__50C86D07_B38B_4155_B724_7904136B0166__INCLUDED_
#pragma once

enum EItemState {
	esShowing,
	esHidding,
	esShow,
	esHide,
	es_forcedword = u32(-1)
};

class CCustomItem : public CObject  
{
private:
	float				fTime;
	float				fStateTime;

	Fvector				start_pos;
	EItemState			state;

	ref_sound				sndTake;
public:
	int					iValue;
	CLASS_ID			clsid_target;
public:
						CCustomItem		();
	virtual				~CCustomItem	();

	virtual void		SetState		(EItemState e);
	virtual void		OnMove			();
	virtual void		Load			(LPCSTR section);
};

#endif // !defined(AFX_CUSTOMITEM_H__50C86D07_B38B_4155_B724_7904136B0166__INCLUDED_)
