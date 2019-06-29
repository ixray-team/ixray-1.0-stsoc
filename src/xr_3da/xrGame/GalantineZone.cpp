/////////////////////////////////////////////////////
// GalantineZone.cpp
// Аномальная зона: "ведьмин студень"
// при попадании живого объекта в зону происходит 
//
/////////////////////////////////////////////////////

#include "stdafx.h"
#include "galantinezone.h"
#include "hudmanager.h"


CGalantineZone::CGalantineZone(void) 
{
}

CGalantineZone::~CGalantineZone(void) 
{
}

void CGalantineZone::Load(LPCSTR section) 
{
	inherited::Load(section);
}

void CGalantineZone::Affect(CObject* O) 
{
}

void CGalantineZone::UpdateCL() 
{
	inherited::UpdateCL();
	
}

void CGalantineZone::Postprocess(f32 val) 
{
}
