#include "stdafx.h"
#include <dinput.h>
#include "../xr_ioconsole.h"
#include "../xr_input.h"
#include "../xr_ioc_cmd.h"
#include "xr_level_controller.h"
#include "string_table.h"
 
//************************************************ Input
int key_binding			[ 2048	];

_keybind  keybind[]		= {
	{ "left",			kLEFT			},	
	{ "right",			kRIGHT			},
	{ "up",				kUP				},	
	{ "down",			kDOWN			},
	{ "jump",			kJUMP			},	
	{ "crouch",			kCROUCH			},
	{ "accel",			kACCEL			},
	{ "sprint_toggle",  kSPRINT_TOGGLE  },

	{ "forward",		kFWD			},	
	{ "back",			kBACK			},
	{ "lstrafe",		kL_STRAFE		},	
	{ "rstrafe",		kR_STRAFE		},

	{ "llookout",		kL_LOOKOUT		},	
	{ "rlookout",		kR_LOOKOUT		},

	{ "turn_engine",	kENGINE			},

	{ "cam_1",			kCAM_1			},	
	{ "cam_2",			kCAM_2			},
	{ "cam_3",			kCAM_3			},	
	{ "cam_4",			kCAM_4			},
	{ "cam_zoom_in",	kCAM_ZOOM_IN	},	
	{ "cam_zoom_out",	kCAM_ZOOM_OUT	},

	{ "torch",			kTORCH			},
	{ "night_vision",	kNIGHT_VISION	},
	{ "wpn_1",			kWPN_1			},
	{ "wpn_2",			kWPN_2			},
	{ "wpn_3",			kWPN_3			},
	{ "wpn_4",			kWPN_4			},
	{ "wpn_5",			kWPN_5			},
	{ "wpn_6",			kWPN_6			},
	{ "artefact",		kARTEFACT		},
	{ "wpn_next",		kWPN_NEXT		},
//.	{ "wpn_prev",		kWPN_PREV		},
	{ "wpn_fire",		kWPN_FIRE		},
	{ "wpn_zoom",		kWPN_ZOOM		},
	{ "wpn_reload",		kWPN_RELOAD		},
	{ "wpn_func",		kWPN_FUNC		},
	{ "wpn_firemode_prev",	kWPN_FIREMODE_PREV },
	{ "wpn_firemode_next",	kWPN_FIREMODE_NEXT },
	
	{ "pause",			kPAUSE			},	
	{ "drop",			kDROP			},	
	{ "use",			kUSE			},	
	{ "scores",			kSCORES			},	
	{ "chat",			kCHAT			},	
	{ "chat_team",		kCHAT_TEAM		},	
	{ "screenshot",		kSCREENSHOT		},	
	{ "quit",			kQUIT			},
	{ "console",		kCONSOLE		},
	{ "inventory",		kINVENTORY		},
	{ "buy_menu",		kBUY			},
	{ "skin_menu",		kSKIN			},
	{ "team_menu",		kTEAM			},
	{ "pda",			kPDA			},
	{ "active_jobs",	kACTIVE_JOBS	},
	{ "map",			kMAP			},
	{ "contacts",		kCONTACTS		},

	{ "ext_1",			kEXT_1			},
	{ "ext_2",			kEXT_2			},
	{ "ext_3",			kEXT_3			},
	{ "ext_4",			kEXT_4			},
	{ "ext_5",			kEXT_5			},
	{ "ext_6",			kEXT_6			},
	{ "ext_7",			kEXT_7			},
	{ "ext_8",			kEXT_8			},
	{ "ext_9",			kEXT_9			},
	{ "ext_10",			kEXT_10			},
	{ "ext_11",			kEXT_11			},
	{ "ext_12",			kEXT_12			},
	{ "ext_13",			kEXT_13			},
	{ "ext_14",			kEXT_14			},
	{ "ext_15",			kEXT_15			},
	{ "vote_begin",		kVOTE_BEGIN		},
	{ "vote",			kVOTE			},
	{ "vote_yes",		kVOTEYES		},
	{ "vote_no",		kVOTENO			},

	{ "next_slot",		kNEXT_SLOT		},
	{ "prev_slot",		kPREV_SLOT		},

	{ "speech_menu_0",	kSPEECH_MENU_0	},
	{ "speech_menu_1",	kSPEECH_MENU_1	},
	{ "speech_menu_2",	kSPEECH_MENU_2	},
	{ "speech_menu_3",	kSPEECH_MENU_3	},
	{ "speech_menu_4",	kSPEECH_MENU_4	},
	{ "speech_menu_5",	kSPEECH_MENU_5	},
	{ "speech_menu_6",	kSPEECH_MENU_6	},
	{ "speech_menu_7",	kSPEECH_MENU_7	},
	{ "speech_menu_8",	kSPEECH_MENU_8	},
	{ "speech_menu_9",	kSPEECH_MENU_9	},

//.	{ "insult_msg",		kINSULT_MSG		},
//.	{ "insult_jesture",	kINSULT_JESTURE	},
	{ "use_bandage",	kUSE_BANDAGE	},
	{ "use_medkit",		kUSE_MEDKIT		},

	{ NULL, 			0				}
};

_keybind keynames[] = {
	{ "kESCAPE",	 	DIK_ESCAPE		},	{ "k1",				DIK_1			},
	{ "k2",				DIK_2			},	{ "k3",				DIK_3			},
	{ "k4",				DIK_4			},	{ "k5",				DIK_5			},
	{ "k6",				DIK_6			},	{ "k7",				DIK_7			},
	{ "k8",				DIK_8			},	{ "k9",				DIK_9			},
	{ "k0",				DIK_0			},	{ "kMINUS",			DIK_MINUS		},
	{ "kEQUALS",		DIK_EQUALS		},	{ "kBACK",			DIK_BACK		},
	{ "kTAB",			DIK_TAB 		},	{ "kQ",				DIK_Q			},
	{ "kW",				DIK_W			},	{ "kE",				DIK_E			},
	{ "kR",				DIK_R			},	{ "kT",				DIK_T			},
	{ "kY",				DIK_Y			},	{ "kU",				DIK_U			},
	{ "kI",				DIK_I			},	{ "kO",				DIK_O			},
	{ "kP",				DIK_P			},	{ "kLBRACKET",		DIK_LBRACKET	},
	{ "kRBRACKET",		DIK_RBRACKET	},	{ "kRETURN",		DIK_RETURN		},
	{ "kLCONTROL",		DIK_LCONTROL	},	{ "kA",				DIK_A			},
	{ "kS",				DIK_S			},	{ "kD",				DIK_D			},
	{ "kF",				DIK_F			},	{ "kG",				DIK_G			},
	{ "kH",				DIK_H			},	{ "kJ",				DIK_J			},
	{ "kK",				DIK_K			},	{ "kL",				DIK_L			},
	{ "kSEMICOLON",		DIK_SEMICOLON	},	{ "kAPOSTROPHE",	DIK_APOSTROPHE	},
	{ "kGRAVE",			DIK_GRAVE		},	{ "kLSHIFT",	 	DIK_LSHIFT		},
	{ "kBACKSLASH",		DIK_BACKSLASH	},	{ "kZ",				DIK_Z			},
	{ "kX",				DIK_X			},	{ "kC",				DIK_C			},
	{ "kV",				DIK_V			},	{ "kB",				DIK_B			},
	{ "kN",				DIK_N			},	{ "kM",				DIK_M			},
	{ "kCOMMA",			DIK_COMMA		},	{ "kPERIOD",		DIK_PERIOD		},
	{ "kSLASH",			DIK_SLASH		},	{ "kRSHIFT",		DIK_RSHIFT		},
	{ "kMULTIPLY",		DIK_MULTIPLY	},	{ "kLMENU",			DIK_LMENU		},
	{ "kSPACE",			DIK_SPACE		},	{ "kCAPITAL",		DIK_CAPITAL		},
	{ "kF1",			DIK_F1			},	{ "kF2",			DIK_F2			},
	{ "kF3",			DIK_F3			},	{ "kF4",			DIK_F4			},
	{ "kF5",			DIK_F5			},	{ "kF6",			DIK_F6			},
	{ "kF7",			DIK_F7			},	{ "kF8",			DIK_F8			},
	{ "kF9",			DIK_F9			},	{ "kF10",			DIK_F10			},
	{ "kNUMLOCK",		DIK_NUMLOCK		},	{ "kSCROLL",		DIK_SCROLL		},
	{ "kNUMPAD7",		DIK_NUMPAD7		},	{ "kNUMPAD8",		DIK_NUMPAD8		},
	{ "kNUMPAD9",		DIK_NUMPAD9		},	{ "kSUBTRACT",		DIK_SUBTRACT	},
	{ "kNUMPAD4",		DIK_NUMPAD4		},	{ "kNUMPAD5",		DIK_NUMPAD5		},
	{ "kNUMPAD6",		DIK_NUMPAD6		},	{ "kADD",			DIK_ADD			},
	{ "kNUMPAD1",		DIK_NUMPAD1		},	{ "kNUMPAD2",		DIK_NUMPAD2		},
	{ "kNUMPAD3",		DIK_NUMPAD3		},	{ "kNUMPAD0",		DIK_NUMPAD0		},
	{ "kDECIMAL",		DIK_DECIMAL		},	{ "kF11",			DIK_F11			},
	{ "kF12",			DIK_F12			},	{ "kF13",			DIK_F13			},
	{ "kF14",			DIK_F14			},	{ "kF15",			DIK_F15			},
	{ "kKANA",			DIK_KANA		},	{ "kCONVERT",		DIK_CONVERT		},
	{ "kNOCONVERT",		DIK_NOCONVERT	},	{ "kYEN",			DIK_YEN			},
	{ "kNUMPADEQUALS",	DIK_NUMPADEQUALS},	{ "kCIRCUMFLEX",	DIK_CIRCUMFLEX	},
	{ "kAT",			DIK_AT			},	{ "kCOLON",			DIK_COLON		},
	{ "kUNDERLINE",		DIK_UNDERLINE	},	{ "kKANJI",			DIK_KANJI		},
	{ "kSTOP",			DIK_STOP		},	{ "kAX",			DIK_AX			},
	{ "kUNLABELED",		DIK_UNLABELED	},	{ "kNUMPADENTER",	DIK_NUMPADENTER	},
	{ "kRCONTROL",		DIK_RCONTROL	},	{ "kNUMPADCOMMA",	DIK_NUMPADCOMMA	},
	{ "kDIVIDE",		DIK_DIVIDE		},	{ "kSYSRQ",			DIK_SYSRQ		},
	{ "kRMENU",			DIK_RMENU		},	{ "kHOME",			DIK_HOME		},
	{ "kUP",			DIK_UP			},	{ "kPRIOR",			DIK_PRIOR		},
	{ "kLEFT",			DIK_LEFT		},	{ "kRIGHT",			DIK_RIGHT		},
	{ "kEND",			DIK_END			},	{ "kDOWN",			DIK_DOWN		},
	{ "kNEXT",			DIK_NEXT		},	{ "kINSERT",		DIK_INSERT		},
	{ "kDELETE",		DIK_DELETE		},	{ "kLWIN",			DIK_LWIN		},
	{ "kRWIN",			DIK_RWIN		},	{ "kAPPS",			DIK_APPS		},
	{ "kPAUSE",			DIK_PAUSE		},	{ "mouse1",			MOUSE_1			},
	{ "mouse2",			MOUSE_2			},	{ "mouse3",			MOUSE_3			},
	{ NULL, 			0				}
};
ConsoleBindCmds	bindConsoleCmds;

int keyname_to_dik(LPCSTR keyname)
{
	int idx				= 0;
	while( keynames[idx].name )
	{
		if(!stricmp(keyname, keynames[idx].name) )
			return keynames[idx].DIK;
		++idx;
	}
	R_ASSERT2("cant find corresponding DIK for keyname", keyname);
	return 0;
}
//-----------------------------------------------------------------------
// Bind/Unbind/List
//-----------------------------------------------------------------------
class CCC_Bind : public IConsole_Command
{
public:
	CCC_Bind(LPCSTR N) : IConsole_Command(N) {};
	virtual void Execute(LPCSTR args) {
		BOOL	binded=false;
		char	action[256],key[256];

		sscanf(args,"%s %s",action,key);
		for (int i=0; keybind[i].name; ++i) {
			if (_stricmp(keybind[i].name,action)==0) {
				// i = action num
				for (int j=0; keynames[j].name; ++j) {
					if (_stricmp(keynames[j].name,key)==0) {
						// j = key
						binded=true;
						key_binding[keynames[j].DIK]=keybind[i].DIK;
					}
				}
			}
		}
		if (!binded) {
			Log("! Can't bind action: ", action);
			Log("! Syntax error. See 'list_actions'.");
		}
		
		CStringTable::ReparseKeyBindings();
	}
	virtual void Save(IWriter* F) 
	{
		F->w_printf("unbindall\r\n");
		for (int i=0; i<2048; ++i) {
			if (key_binding[i]) {
				for (int j=0; keybind[j].name; ++j) {
					if (keybind[j].DIK==key_binding[i]) {
						for (int k=0; keynames[k].name; ++k) {
							if (keynames[k].DIK==i) {
								F->w_printf("bind %s %s\r\n", keybind[j].name, keynames[k].name);
							}
						}
					}
				}
			}
		}
	}
};

class CCC_UnBind : public IConsole_Command
{
public:
	CCC_UnBind(LPCSTR N) : IConsole_Command(N) 
	{ bEmptyArgsHandled=TRUE; };
	virtual void Execute(LPCSTR args) {
		for (int i=0; keybind[i].name; ++i) {
			if (_stricmp(keybind[i].name,args)==0) {
				// i = action num
				for (int j=0; j<2048; ++j) {
					if (key_binding[j]==keybind[i].DIK) key_binding[j]=0;
				}
			}
		}
		CStringTable::ReparseKeyBindings();
	}
};

class CCC_ListActions : public IConsole_Command
{
public:
	CCC_ListActions(LPCSTR N) : IConsole_Command(N)
	{ bEmptyArgsHandled=TRUE; };

	virtual void Execute(LPCSTR args) {
		Log("- --- Action list start ---");
		for (int i=0; keybind[i].name; ++i) {
			Log("-",keybind[i].name);
		}
		Log("- --- Action list end   ---");
	}
};

class CCC_UnBindAll : public IConsole_Command
{
public:
	CCC_UnBindAll(LPCSTR N) : IConsole_Command(N)
	{ bEmptyArgsHandled=TRUE; };

	virtual void Execute(LPCSTR args) {
		ZeroMemory(key_binding,sizeof(key_binding));
		bindConsoleCmds.clear();

		Console->Execute("cfg_load default_controls.ltx");
	}
};

int NameIdx(int dik, bool b=true)
{
	int res = -1;
	int i=0;
	if(b){
		for(i=0;i<2048; ++i){
			if(	key_binding[i]==dik){
				res = i;
				break;
			}
		}
	}else{
		i=dik;
		res = 0;
	}

	if(res!=-1){//found
		res = -1;
		for(int j=0;keynames[j].name;++j)
			if(keynames[j].DIK == i){
				res = j;
				break;
			}
	}
	return res;

}
class CCC_BindList : public IConsole_Command
{
public:
	CCC_BindList(LPCSTR N) : IConsole_Command(N)
	{ bEmptyArgsHandled=TRUE; };

	virtual void Execute(LPCSTR args) {
		Log("- --- Bind list start ---");
		string512	buff;			
		string64	key_name;		
		
		for (int i=0; keybind[i].name; ++i) {
			ZeroMemory(buff,sizeof(buff));
			ZeroMemory(key_name,sizeof(key_name));
			strcat(buff,keybind[i].name);
			int idx = NameIdx(keybind[i].DIK);
			if(idx!=-1){
				strcat(key_name,keynames[idx].name);
				strconcat(buff,buff," binded to ",key_name);
			}else
				strconcat(buff,buff," binded to nil");

			Log(buff);
		}
		Log("- --- Bind list end   ---");
	}
};

void GetActionBinding(LPCSTR action, char* dst_buff)
{
	for (int i=0; i<2048; ++i) {
		if (key_binding[i]) {
			for (int j=0; keybind[j].name ; ++j) {
				if ( (keybind[j].DIK==key_binding[i])&&(_stricmp(keybind[j].name,action)==0) ) {
					for (int k=0; keynames[k].name; ++k) {
						if (keynames[k].DIK==i) {
							if( xr_strlen(dst_buff) )
							strcat(dst_buff, " or ");

							strcat(dst_buff, (keynames[k].name[0]=='k')? keynames[k].name+1 : keynames[k].name);
							//F->w_printf("bind %s %s\r\n", keybind[j].name, keynames[k].name);
						}
					}
				}
			}
		}
	}

}

void GetActionBindingEx(LPCSTR action, char* dst_buff)
{
	for (int i=0; i<2048; ++i) {
		if (key_binding[i]) {
			for (int j=0; keybind[j].name ; ++j) {
				if ( (keybind[j].DIK==key_binding[i])&&(_stricmp(keybind[j].name,action)==0) ) {
					for (int k=0; keynames[k].name; ++k) {
						if (keynames[k].DIK==i) {
							if( xr_strlen(dst_buff) )
								strcat(dst_buff, " or ");

							strcat(dst_buff, keynames[k].name);
							return;							
						}
					}
				}
			}
		}
	}
}

bool IsActionExist(LPCSTR action){
    for (int i=0; true; i++)
	{
        if (keybind[i].name)
		{
			if (0 == xr_strcmp(action,keybind[i].name))
				return true;
		}
		else
			return false;
	}
}

class CCC_BindConsoleCmd : public IConsole_Command
{
public:
	CCC_BindConsoleCmd(LPCSTR N) : IConsole_Command(N) {};
	virtual void Execute(LPCSTR args) {
		string512 console_command;
		string256 key;
		int cnt = _GetItemCount(args,' ');
		_GetItems(args,0,cnt-1,console_command,' ');
		_GetItem(args,cnt-1,key,' ');
		for (int j=0; keynames[j].name; ++j) {
			if (_stricmp(keynames[j].name,key)==0) {
				// j = key
				bindConsoleCmds.bind(keynames[j].DIK, console_command);
				return;
			}
		}
	}

	virtual void Save(IWriter* F) 
	{
		bindConsoleCmds.save(F);
	}

};


class CCC_UnBindConsoleCmd : public IConsole_Command
{
public:
	CCC_UnBindConsoleCmd(LPCSTR N) : IConsole_Command(N)
	{ bEmptyArgsHandled=FALSE; };

	virtual void Execute(LPCSTR args) {
	for (int i=0; keybind[i].name; ++i) {
		if (_stricmp(keybind[i].name,args)==0) {
				bindConsoleCmds.unbind(keybind[i].DIK);
			}
		}
	}
};

void ConsoleBindCmds::bind(int dik, LPCSTR N)
{
	_conCmd& c	= m_bindConsoleCmds[dik];
	c.cmd		= N;
}
void ConsoleBindCmds::unbind(int dik)
{
	xr_map<int,_conCmd>::iterator it = m_bindConsoleCmds.find(dik);
	if(it==m_bindConsoleCmds.end())
		return;

	m_bindConsoleCmds.erase(it);
}

void ConsoleBindCmds::clear()
{
	m_bindConsoleCmds.clear();
}

bool ConsoleBindCmds::execute(int dik)
{
	xr_map<int,_conCmd>::iterator it = m_bindConsoleCmds.find(dik);
	if(it==m_bindConsoleCmds.end())
		return false;

	Console->Execute(it->second.cmd.c_str());
	return true;
}

void ConsoleBindCmds::save(IWriter* F)
{
	xr_map<int,_conCmd>::iterator it = m_bindConsoleCmds.begin();
	for(;it!=m_bindConsoleCmds.end();++it){
		string64 keyname;
		int idx = NameIdx(it->first,false);
		if(idx!=-1){
			ZeroMemory(keyname,sizeof(keyname));
			strcat(keyname,keynames[idx].name);
			F->w_printf("bind_console %s %s\n", *it->second.cmd, keyname);
		}
	}
}


void CCC_RegisterInput()
{
	CMD1(CCC_Bind,		"bind"					);
	CMD1(CCC_UnBind,	"unbind"				);
	CMD1(CCC_UnBindAll,	"unbindall"				);
	CMD1(CCC_ListActions,"list_actions"			);

	CMD1(CCC_BindList,	"bind_list"				);
	CMD1(CCC_BindConsoleCmd, "bind_console"			);
	CMD1(CCC_UnBindConsoleCmd, "unbind_console"		);
};
