#include "stdafx.h"
#include "ai_monster_sound.h"
#include "../custommonster.h"

void CMonsterSound::Init(CCustomMonster *pM)
{
	pMonster			= pM;
	
	current				= 0;
	bkgnd				= 0;
	cur_type			= prev_type = SND_TYPE_IDLE;
	delay				= 0;
	time_next_play		= 0;
	play_bkgnd			= false;
	cur_priority		= new_priority = SND_PRIORITY_NORMAL;
}

void CMonsterSound::LoadSounds(LPCTSTR section)
{
}

void CMonsterSound::ControlSound(TTime time)
{
	bool b_cur_playing		= current && current->feedback;
	bool b_bkgnd_playing	= bkgnd && bkgnd->feedback;
	
	// process bkground sound
	if (play_bkgnd && !b_bkgnd_playing) PlayBkgndSound();
	play_bkgnd = false;
	
	if ((prev_type != cur_type) && (cur_priority >= new_priority)) {
		// if sound is playing now - stop sound and start _new one
		if (b_cur_playing) current->stop();			
		PlaySound(cur_type);
		return;
	};

	// update sound position
	if (b_bkgnd_playing) bkgnd->feedback->set_position(pMonster->Position());
	if (b_cur_playing) { 
		current->feedback->set_position(pMonster->Position());
		return;
	}

	// there is no sound to play
	if (0 == delay) return;

	if (0 == time_next_play) time_next_play = time + ::Random.randI(delay);
	
	// check if it's time to play sound
	if (time_next_play < time) PlaySound(cur_type);
}

void CMonsterSound::PlaySound(ESoundType sound_type)
{
}

void CMonsterSound::PlayBkgndSound()
{
}

