//----------------------------------------------------
// file: TempObject.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "TempObject.h"
#include "render.h"

CTempObject::CTempObject()
{
	m_pVisual		= 0;
	m_iLifeTime		= int_max;
	m_bAutoRemove	= TRUE;
}
//----------------------------------------------------

CTempObject::~CTempObject()
{
	Render->model_Delete(m_pVisual);
}
//----------------------------------------------------
void CTempObject::Update(u32 dt)
{
	m_iLifeTime		-= dt;
}
