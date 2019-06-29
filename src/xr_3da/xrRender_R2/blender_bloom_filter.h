#pragma once

class CBlender_bloom_filter : public CBlender  
{
public:
	virtual		LPCSTR		getComment()	{ return "INTERNAL: bloom filtering";	}
	virtual		BOOL		canBeDetailed()	{ return FALSE;	}
	virtual		BOOL		canBeLMAPped()	{ return FALSE;	}

	virtual		void		Compile			(CBlender_Compile& C);

	CBlender_bloom_filter();
	virtual ~CBlender_bloom_filter();
};
