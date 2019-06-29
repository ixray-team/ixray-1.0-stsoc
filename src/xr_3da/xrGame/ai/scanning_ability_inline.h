#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CScanningAbilityAbstract CScanningAbility<_Object>

TEMPLATE_SPECIALIZATION
void CScanningAbilityAbstract::load(LPCSTR section)
{
	::Sound->create(sound_scan,	TRUE, pSettings->r_string(section,"scan_sound"), SOUND_TYPE_WORLD);

	critical_value		= pSettings->r_float(section,"scan_critical_value");
	scan_radius			= pSettings->r_float(section,"scan_radius");
	velocity_threshold	= pSettings->r_float(section,"scan_velocity_threshold");
	decrease_value		= pSettings->r_float(section,"scan_decrease_value");
}

TEMPLATE_SPECIALIZATION
void CScanningAbilityAbstract::reinit()
{
	state		= eStateDisabled;
	scan_value	= 0.f;
}

TEMPLATE_SPECIALIZATION
void CScanningAbilityAbstract::schedule_update()
{
	if (state == eStateDisabled) return;

	CObject *scan_obj	= Level().CurrentEntity();

	// проверка на активность
	if (state == eStateNotActive) {
		if (scan_obj->Position().distance_to(object->Position()) < scan_radius) state = eStateScanning;
	}

	if (state == eStateNotActive) return;

	if (state == eStateScanning) {
		// обновить scan_value
		float vel = get_velocity(scan_obj);
		if ( vel > velocity_threshold) {
			scan_value += vel;
			
			if (sound_scan.feedback) sound_scan.set_position(scan_obj->Position());
			else ::Sound->play_at_pos(sound_scan, 0, scan_obj->Position());
			
			on_scanning();
		}
	}

	if (scan_value > critical_value) {
		on_scan_success();
		state = eStateDisabled;
	}

}

TEMPLATE_SPECIALIZATION
void CScanningAbilityAbstract::frame_update(u32 dt)
{
	if (state != eStateScanning) return;

	if (scan_value < 0) scan_value = 0.f;
	else if (scan_value > 0) {
		scan_value -= decrease_value * float(dt) / 1000;
	}
}


TEMPLATE_SPECIALIZATION
float CScanningAbilityAbstract::get_velocity(CObject *obj)
{
	if (obj->ps_Size() < 2) return (0.f);

	CObject::SavedPosition	pos0 = obj->ps_Element(obj->ps_Size() - 2);
	CObject::SavedPosition	pos1 = obj->ps_Element(obj->ps_Size() - 1);

	return	(pos1.vPosition.distance_to(pos0.vPosition) /
             (float(pos1.dwTime)/1000.f - float(pos0.dwTime)/1000.f));
}

TEMPLATE_SPECIALIZATION
void CScanningAbilityAbstract::enable() 
{
	if (state != eStateDisabled) return;

	state		= eStateNotActive;
	scan_value	= 0.f; 
}

TEMPLATE_SPECIALIZATION
void CScanningAbilityAbstract::disable()
{
	state		= eStateDisabled;
	scan_value	= 0.f; 
}

#undef TEMPLATE_SPECIALIZATION