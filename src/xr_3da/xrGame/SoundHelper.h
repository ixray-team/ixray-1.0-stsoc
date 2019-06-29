#pragma once

#include "..\3dsound.h"

const int SoundHelper_Limit=8;
class C3DSoundHelper
{
	svector<C3DSound*,SoundHelper_Limit>	sounds;
	int										handle;
	int										limit;
public:
	void	Load	(const char* fName, int max_simultaneous = SoundHelper_Limit)
	{
		handle	= pSounds->Create	(fName,TRUE);
		limit	= _min				(max_simultaneous,SoundHelper_Limit-1);
	}
	void	Unload	()
	{
		pSounds->Delete(handle);
	}
	void	Update(Fvector& pos)
	{
		for (int i=0; i<sounds.size(); i++)
		{
			C3DSound* S = sounds[i];
			if (S->isPlaying()) {
				S->SetPosition(pos);
			} else {
				sounds.erase(i);
				i--;
			}
		}
	}
	void	Play(Fvector& pos)
	{
		if (sounds.size()==limit) {
			// select sound to kill
			float	s_perc	= sounds[0]->GetPercentageRested();
			int		s_id	= 0;
			for (int i=1; i<sounds.size(); i++)
			{
				float p = sounds[i]->GetPercentageRested();
				if (p<s_perc) {
					s_perc	= p;
					s_id	= i;
				}
			}

			// stop old and play new
			sounds[s_id]->Rewind();
		} else {
			sounds.push_back(pSounds->PlayAtPos(handle,pos));
		}
	}
};
