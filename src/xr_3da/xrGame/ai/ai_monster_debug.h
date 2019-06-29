	#pragma once

class CEntityAlive;

class CMonsterDebug {
	CEntityAlive *pMonster;

	struct _elem {
		string128	text;
		u32			col;
	};
	
	struct _elem_point {
		Fvector pos;
		u32		col;
		float	box_size;
	};

	struct _elem_line {
		Fvector p1;
		Fvector p2;
		u32		col;
	};


	struct _elem_const {
		string128	text;
		float		x;
		float		y;
	};

	xr_vector<_elem>		_data;
	xr_vector<_elem_point>	_points;
	xr_vector<_elem_const>	_text;
	xr_vector<_elem_line>	_lines;

	float	floor_height;
	Fvector shift;

	bool	active;

public:
			CMonsterDebug		(CEntityAlive *pM, const Fvector &shift_vec, float height);
			~CMonsterDebug();

	void	SetActive			(bool bActivity = true) {active = bActivity;}	
	
	// -------------------------------------------------------------------
	// Monster caption
	void	M_Add				(u32 index, LPCSTR str, u32 col);
	void	M_Update			();
	void	M_Clear				();	
	
	// -------------------------------------------------------------------
	// Level lines
	void	L_AddPoint			(const Fvector &pos, float box_size, u32 col);
	void	L_AddLine			(const Fvector &pos, const Fvector &pos2, u32 col);	
	void	L_Update			();
	void	L_Clear				();

	// -------------------------------------------------------------------
	// Hud text
	void	HT_Add				(float x, float y, LPCSTR str);
	void	HT_Update			();
	void	HT_Clear			();

};


