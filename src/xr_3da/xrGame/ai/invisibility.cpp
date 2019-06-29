#include "stdafx.h"
#include "invisibility.h"
#include "../level.h"


void CInvisibility::reinit()
{
	inherited::reinit();
	
	m_time_start_blink	= 0;
	m_cur_visibility	= true;
	m_blink				= false;
	m_time_last_blink	= 0;

	set_manual_switch	(false);
	
	CEnergyHolder::enable();
	CEnergyHolder::set_auto_activate();
	CEnergyHolder::set_auto_deactivate();
}

void CInvisibility::reload(LPCSTR section)
{
	inherited::reload(section, "Invisibility_");

	timeBlink			= pSettings->r_u32(section,"Invisibility_BlinkTime");
	timeBlinkInterval	= pSettings->r_u32(section,"Invisibility_BlinkMicroInterval");

}

void CInvisibility::on_activate() 
{
	start_blink();
}

void CInvisibility::on_deactivate() 
{
	start_blink();
}

void CInvisibility::start_blink()
{
	m_blink				= true;
	m_time_start_blink	= Level().timeServer();
	m_time_last_blink	= 0;
}

void CInvisibility::stop_blink()
{
	m_blink				= false;		
	m_cur_visibility	= !is_active();
	
	on_change_visibility(m_cur_visibility);
}

void CInvisibility::update_blink()
{
	if (!m_blink) return;
	
	u32 cur_time = Level().timeServer();

	// check for whole blink time
	if (m_time_start_blink + timeBlink < cur_time) {
		stop_blink();
		return;
	}	

	// check for current blink interval time
	if (m_time_last_blink + timeBlinkInterval < cur_time) {
		// blink
		m_time_last_blink = cur_time; 
		m_cur_visibility = !m_cur_visibility;

		on_change_visibility(m_cur_visibility);
	}
}

void CInvisibility::frame_update()
{
	update_blink();
}

void CInvisibility::set_manual_switch(bool b_man)
{
	b_man ?  CEnergyHolder::disable() : CEnergyHolder::enable();

	m_manual = b_man;
}

void CInvisibility::manual_activate()
{
	if (m_manual)
		activate		();
}

void CInvisibility::manual_deactivate()
{
	if (m_manual)
		deactivate	();
}
