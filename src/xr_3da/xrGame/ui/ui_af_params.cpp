#include "stdafx.h"
#include "ui_af_params.h"
#include "UIStatic.h"
#include "../object_broker.h"
#include "UIXmlInit.h"

CUIArtefactParams::CUIArtefactParams()
{
	Memory.mem_fill			(m_info_items, 0, sizeof(m_info_items));
}

CUIArtefactParams::~CUIArtefactParams()
{
	for(u32 i=_item_start; i<_max_item_index; ++i)
	{
		CUIStatic* _s			= m_info_items[i];
		xr_delete				(_s);
	}
}

LPCSTR af_item_sect_names[] = {
	"health_restore_speed",
	"radiation_restore_speed",
	"satiety_restore_speed",
	"power_restore_speed",
	"bleeding_restore_speed",
	
	"burn_immunity",
	"strike_immunity",
	"shock_immunity",
	"wound_immunity",		
	"radiation_immunity",
	"telepatic_immunity",
	"chemical_burn_immunity",
	"explosion_immunity",
	"fire_wound_immunity",
};

LPCSTR af_item_param_names[] = {
	"(health)",
	"(radiation)",
	"(satiety)",
	"(power)",
	"(bleeding)",

	"(burn_imm)",
	"(strike_imm)",
	"(shock_imm)",
	"(wound_imm)",
	"(radiation_imm)",
	"(telepatic_imm)",
	"(chemical_burn_imm)",
	"(explosion_imm)",
	"(fire_wound_imm)",
};


void CUIArtefactParams::InitFromXml(CUIXml& xml_doc)
{
	LPCSTR _base				= "af_params";
	if (!xml_doc.NavigateToNode(_base, 0))	return;

	string256					_buff;
	CUIXmlInit::InitWindow		(xml_doc, _base, 0, this);

	for(u32 i=_item_start; i<_max_item_index; ++i)
	{
		m_info_items[i]			= xr_new<CUIStatic>();
		CUIStatic* _s			= m_info_items[i];
		_s->SetAutoDelete		(false);
		strconcat				(_buff, _base, ":static_", af_item_sect_names[i]);
		CUIXmlInit::InitStatic	(xml_doc, _buff,	0, _s);
	}
}

bool CUIArtefactParams::Check(const shared_str& af_section)
{
	return !!pSettings->line_exist(af_section, "af_actor_properties");
}

void CUIArtefactParams::SetInfo(const shared_str& af_section)
{

	string128					_buff;
	float						_h = 0.0f;
	DetachAll					();
	for(u32 i=_item_start; i<_max_item_index; ++i)
	{
		CUIStatic* _s			= m_info_items[i];

		float					_val;
		if(i<_max_item_index1){
			_val				= pSettings->r_float(af_section, af_item_sect_names[i]);
			if						(fis_zero(_val))				continue;
			_val					*= 10000.0f;
		}else
		{
			shared_str _sect	= pSettings->r_string(af_section, "hit_absorbation_sect");
			_val				= pSettings->r_float(_sect, af_item_sect_names[i]);
			if					(fsimilar(_val, 1.0f))				continue;
			_val				= (1.0f - _val);
			_val				*= 100.0f;

		}

		sprintf					(_buff, "%s %s %+.0f %%", af_item_param_names[i], (_val>0)?"%c[green]":"%c[red]", _val);
		_s->SetText				(_buff);
		_s->SetWndPos			(_s->GetWndPos().x, _h);
		_h						+= _s->GetWndSize().y;
		AttachChild				(_s);
	}
	SetHeight					(_h);
}
