// CustomItem.cpp: implementation of the CCustomItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "actor.h"
#include "CustomItem.h"

#define TIME_TO_SHOW	10.f
#define SHOWING_TIME	.5f
#define HIDDING_TIME	.07f

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCustomItem::CCustomItem() : CObject()
{
	fTime			= Random.randF(PI_MUL_2);
	state			= es_forcedword;
	SetState		(esShowing);
}

CCustomItem::~CCustomItem()
{
	::Sound->destroy	(sndTake);
}

void	CCustomItem::SetState(EItemState e)
{
	const float SC=.0001f;
	const float SM=1.f;
	Fvector		vScale;
	if (e==state) return;
	state = e;
	switch(state){
	case esShowing:
		vScale.set			(SC,SC,SC);
		setVisible			(true);
		fStateTime			= SHOWING_TIME; 
		break;
	case esShow:	
		setVisible			(true);
		break;
	case esHidding:	
		::Sound->play_at_pos	(sndTake,this,Position());
		vScale.set			(SM,SM,SM);
		setVisible			(true);
		fStateTime			= HIDDING_TIME; 
		break;
	case esHide:	
		setVisible			(false);
		fStateTime			= TIME_TO_SHOW;
		break;
	}
}

void	CCustomItem::Load	(LPCSTR section)
{
	CObject::Load	(section);
	
	clsid_target	= TEXT2CLSID(pSettings->r_string(section, "target"));
	iValue			= pSettings->r_s32(section, "value");
	start_pos.set	(Position());
	::Sound->create	(sndTake, TRUE, pSettings->r_string(section, "snd_take"));
}

void	CCustomItem::OnMove()
{
	Fvector	vScale;

	fTime			+=	Device.fTimeDelta*2.5f; //*.1f;
	Position().y	=	start_pos.y +	_sin(fTime)*.1f;
	Fvector	D,N;
	Fmatrix	mXFORM;
	D.set			(-_sin(fTime),0.f,-_cos(fTime));
	N.set			(0,1,0);
	mXFORM.rotation	(D,N);
	mXFORM.c.set	(Position());
	XFORM().set		(mXFORM);

	switch(state){
	case esShowing:
		{ 
			fStateTime -= Device.fTimeDelta;
			float   fS = 1.f-(fStateTime/SHOWING_TIME);
			vScale.set(fS,fS,fS);
			if (fStateTime<=0.f) SetState(esShow);
		}
		break;
	case esShow:	
		break;
	case esHidding:	
		{
			fStateTime -= Device.fTimeDelta;
			float   fS = fStateTime/HIDDING_TIME;
			vScale.set(fS,fS,fS);
			if (fStateTime<=0.f) SetState(esHide);
		}
		break;
	case esHide:
		fStateTime-=Device.fTimeDelta;
		if (fStateTime<0) SetState(esShowing);
		break;
	}
}
