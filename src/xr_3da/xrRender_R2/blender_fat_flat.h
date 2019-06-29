#pragma once

class CBlender_fat_flat : public CBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "LEVEL: base-flat";	}
	virtual		BOOL		canBeDetailed()	{ return FALSE;					}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;					}

	virtual		void		Save			(IWriter&	FS);
	virtual		void		Load			(IReader&	FS, WORD version);
	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_fat_flat();
	virtual ~CBlender_fat_flat();
};
