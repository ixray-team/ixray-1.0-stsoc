#pragma once

#include "ai_monsters_misc.h"
#include "ai_monster_defs.h"

enum ESoundType {								// sound types
	SND_TYPE_IDLE,
	SND_TYPE_EAT,
	SND_TYPE_ATTACK,
	SND_TYPE_ATTACK_HIT,
	SND_TYPE_TAKE_DAMAGE,
	SND_TYPE_DIE,
	SND_TYPE_THREATEN,
};	

enum ESoundPriority {
	SND_PRIORITY_HIGH = 0,
	SND_PRIORITY_NORMAL,
	SND_PRIORITY_LOW
};

class CCustomMonster;

class CMonsterSound  {
	
	CCustomMonster *pMonster;						// pointer to data class

	ref_sound		*current;						// pointer to current sound
	ref_sound		*bkgnd;

	TTime			delay;							// random delay after which next sound of current type can be played
	TTime			time_next_play;					// calculated time when next sound of current time can be played
	bool			play_bkgnd;						// play bkground sound during current sched update

	ESoundType		cur_type, prev_type;			// current and previous sound types
	ESoundPriority	cur_priority, new_priority;
	
public:	
	// init class members
			void	Init			(CCustomMonster *pM);

	// load sounds from ltx
			void	LoadSounds		(LPCTSTR section);
			
	// set sound type to play and random delay after this sound
	IC		void	SetSound		(ESoundType sound_type, TTime d, ESoundPriority sp = SND_PRIORITY_LOW) { cur_type = sound_type; delay = d; new_priority = sp;}
			
	// control current state, call this function every schedule update
			void	ControlSound	(TTime time);

	// turn on/off bkground sound
	IC		void	SetBkgndSound	(bool bOn = true) { play_bkgnd = bOn;}

	// utils
	IC		void	SetSoundOnce	(ESoundType sound_type, TTime time) { SetSound(sound_type,0, SND_PRIORITY_NORMAL); ControlSound(time);}

	// implement this function to process event when sound starts playing
	virtual	void	OnSoundPlay		() {};

private:
			void	PlaySound		(ESoundType sound_type);
			void	PlayBkgndSound	();	
};

