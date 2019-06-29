#pragma once

enum	EKeyBinding
{
	kFWD			=	1,
	kBACK			,
	kL_STRAFE		,
	kR_STRAFE		,
	kLEFT			,
	kRIGHT			,
	kUP				,
	kDOWN			,
	kJUMP			,
	kCROUCH			,
	kSPRINT_TOGGLE	,
	kACCEL			,
	kL_LOOKOUT		,
	kR_LOOKOUT		,
	kENGINE			,
	kCAM_1			,
	kCAM_2			,
	kCAM_3			,
	kCAM_4			,
	kCAM_ZOOM_IN	,
	kCAM_ZOOM_OUT	,

	kTORCH			,
	kNIGHT_VISION	,

	kWPN_1			,
	kWPN_2			,
	kWPN_3			,
	kWPN_4			,
	kWPN_5			,
	kWPN_6			,
	kARTEFACT		,
	kWPN_NEXT		,
//.	kWPN_PREV		,
	kWPN_FIRE		,
	kWPN_RELOAD		,
	kWPN_ZOOM		,
	kWPN_ZOOM_INC	,
	kWPN_ZOOM_DEC	,
	kWPN_FUNC		,
	kWPN_FIREMODE_PREV,
	kWPN_FIREMODE_NEXT,

	kUSE			,
	kPAUSE			,
	kDROP			,
	kSCORES			,
	kCHAT			,
	kCHAT_TEAM		,
	kSCREENSHOT		,
	kQUIT			,
	kCONSOLE		,
	kINVENTORY		,
	kBUY			,
	kSKIN			,
	kTEAM			,
	kPDA			,
	kACTIVE_JOBS	,
	kMAP			,
	kCONTACTS		,
	kEXT_1			,
	kEXT_2			,
	kEXT_3			,
	kEXT_4			,
	kEXT_5			,
	kEXT_6			,
	kEXT_7			,
	kEXT_8			,
	kEXT_9			,
	kEXT_10			,
	kEXT_11			,
	kEXT_12			,
	kEXT_13			,
	kEXT_14			,
	kEXT_15			,

    kVOTE_BEGIN		,
	kVOTE			,
	kVOTEYES		,
	kVOTENO			,

	kNEXT_SLOT		,
	kPREV_SLOT		,
	
	kSPEECH_MENU_0	,
	kSPEECH_MENU_1	,
	kSPEECH_MENU_2	,
	kSPEECH_MENU_3	,
	kSPEECH_MENU_4	,
	kSPEECH_MENU_5	,
	kSPEECH_MENU_6	,
	kSPEECH_MENU_7	,
	kSPEECH_MENU_8	,
	kSPEECH_MENU_9	,

//.	kINSULT_MSG		,
//.	kINSULT_JESTURE	,

	kUSE_BANDAGE	,
	kUSE_MEDKIT		,

	kFORCEDWORD		= u32(-1)
};

struct _keybind		{
	char *	name;
	int		DIK;
};

extern _keybind	keybind			[];
extern _keybind	keynames		[];
extern int		key_binding		[];
extern void		CCC_RegisterInput();

struct _conCmd	{
	shared_str	cmd;
};

class ConsoleBindCmds{
public:
	xr_map<int,_conCmd>	m_bindConsoleCmds;
	void bind(int dik, LPCSTR N);
	void unbind(int dik);
	bool execute(int dik);
	void clear();
	void save(IWriter* F);
};

void GetActionBinding		(LPCSTR action, char* dst_buff);
void GetActionBindingEx		(LPCSTR action, char* dst_buff);
bool IsActionExist			(LPCSTR action);

extern ConsoleBindCmds	bindConsoleCmds;

#define MOUSE_1		0x100
#define MOUSE_2		0x200
#define MOUSE_3		0x400
