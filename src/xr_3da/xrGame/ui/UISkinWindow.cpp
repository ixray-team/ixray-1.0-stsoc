// file:		UISkinWindow
// description:	SkinWindow for SkinSelector
// created:		03.02.2005
// author:		Serge Vynnychenko

// copyright 2005 GSC Game World

#include "StdAfx.h"
#include "UISkinWindow.h"
#include "../UI.h"
#include "UISkinSelector.h"

const int BORDER = 30;


CUISkinWindow::CUISkinWindow(){

}

CUISkinWindow::~CUISkinWindow(){

}

void CUISkinWindow::Init(CUIXml& xmlDoc, int index, shared_str section){
	CUIXmlInit xml_init;
	shared_str texture;
	string256 buf;
	float width	 = UI_BASE_WIDTH / SKINS_COUNT;
	float height = xmlDoc.ReadAttribFlt(PATH_XML, 0, "height");
	float x		 = index*width;
	float y		 = UI_BASE_HEIGHT/2.0f - height/2.0f;

	CUIWindow::Init(x, y, width, height);

/////// UIBackground
	AttachChild(&UIBackground);

	texture = xmlDoc.Read(strconcat(buf,PATH_XML,":texture"), 0, NULL);
	UIBackground.Init(*texture, 0, 0, width, height);

	CUIFrameWindow* pWnd = &UIBackground;
// init left top
	strconcat(buf, PATH_XML, ":left_top_texture");
	texture = xmlDoc.Read(buf, index, NULL);
	x = float(xmlDoc.ReadAttribInt(buf, index, "x"));
	y = float(xmlDoc.ReadAttribInt(buf, index, "y"));
	if(*texture)
		pWnd->InitLeftTop(*texture, x, y);

// init left bottom	
	strconcat(buf,PATH_XML,":left_bottom_texture");
	texture = xmlDoc.Read(buf, index, NULL);
	x = float(xmlDoc.ReadAttribInt(buf, index, "x"));
	y = float(xmlDoc.ReadAttribInt(buf, index, "y"));
	if(*texture) 
		pWnd->InitLeftBottom(*texture, x, y);

// init title
	strconcat(buf,PATH_XML,":title");
	if(xmlDoc.NavigateToNode(buf,index)) 
		xml_init.InitStatic(xmlDoc, buf, index, &pWnd->UITitleText);

/////// UIHighlighted
	AttachChild(&UIHighlight);
	UIHighlight.Init(0, 0, width, height);

	Frect r	= UIBackground.GetAbsoluteRect();

	// Координаты бекграунда
	r.bottom	-= float(BORDER);
	r.left		+= float(BORDER);
	r.top		+= float(BORDER);
	r.right		-= float(BORDER);

	float	bkWidth				= r.width(), 
			bkHeight			= r.height(),
		// Координаты текстуры
		texWidth			= float(SKIN_TEX_WIDTH),
		texHeight			= float(SKIN_TEX_HEIGHT);

	// Коеффициент сжатия
	float stretchKoeff = 1.0f;

	// Проверяем какое из отношений меньше
	float dW = bkWidth	/ texWidth;
	float dH = bkHeight	/ texHeight;

	(dW < dH) ? stretchKoeff = dW : stretchKoeff = dH;

	texWidth	= texWidth		* stretchKoeff;
	texHeight	= texHeight	* stretchKoeff;

	if (dW < dH)
	{
		float f		 = (bkHeight - texHeight) / 2.0f;
		r.top		+= f;
		r.bottom	-= f;
	}
	else
	{
		float f		 = (bkWidth - texWidth) / 2.0f;
		r.left		+= f;
		r.right		-= f;
	}

	UIHighlight.Init(float(BORDER), float(BORDER), r.right - r.left, r.bottom - r.top);

	UIHighlight.SetShader(GetMPCharIconsShader());
	UIHighlight.GetUIStaticItem().SetColor(clInactive);

	int m_iSkinX = 0, m_iSkinY = 0;
	sscanf(pSettings->r_string("multiplayer_skins", *GetSkinName(section, index)), "%i,%i", &m_iSkinX, &m_iSkinY);

	// Размеры иконки персонажа : 128х341
	UIHighlight.GetUIStaticItem().SetOriginalRect(
		float(m_iSkinX),
		float(m_iSkinY),
		float(CHAR_ICON_FULL_WIDTH*ICON_GRID_HEIGHT),
		(5.07f*ICON_GRID_WIDTH));

	UIHighlight.ClipperOn();
//	UIHighlight.SetTextureScaleXY(stretchKoeff,stretchKoeff);
	UIHighlight.SetStretchTexture(true);

	if (0 == index)
        UIHighlight.GetUIStaticItem().SetColor(clActive);
}

bool CUISkinWindow::OnDbClick(){
	GetMessageTarget()->SendMessage(this, WINDOW_LBUTTON_DB_CLICK, NULL);
	return true;
}

shared_str CUISkinWindow::GetSkinName(shared_str section, int index){
	string256 skinName;
	shared_str	cfgRecord;
	shared_str ret;
	

	cfgRecord = pSettings->r_string(section, "skins");
	_GetItem(*cfgRecord, index, skinName);

	return ret = skinName;
}

