#include "stdafx.h"
#include "UICustomMenu.h"
#include "HUDManager.h"
#include "../xr_ioconsole.h"

void CUICustomMenuItem::Execute()			
{
	if (value0&&(value0[0]=='$'))		Console->Execute(value0+1);
	else if (value1&&(value1[0]=='$'))	Console->Execute(value1+1);
	else if (OnExecute)					OnExecute(this);
}
//--------------------------------------------------------------------
CUICustomMenuItem* UIParseMenu	(CUICustomMenu* owner, CInifile* ini, CUICustomMenuItem* root, LPCSTR sect, OnExecuteEvent exec, OnItemDrawEvent draw)
{
	CUICustomMenuItem* I=0;
	string256	buf,buf1;
	string64	buf2;
	int ln_cnt = ini->line_count(sect);
	if (ln_cnt){
		if (!root)	root		= xr_new<CUICustomMenuItem> (owner,(CUICustomMenuItem *)NULL,"root",(LPCSTR)NULL,(LPCSTR)NULL,(OnExecuteEvent)NULL,(OnItemDrawEvent)NULL);
		if (ini->line_exist(sect,"title"))	root->SetTitle(ini->r_string(sect,"title"));
		for (int i=1; i<=ln_cnt; ++i){
			// append if exist menu item
			sprintf(buf,"menu_%d",i);
			if (ini->line_exist(sect,buf)){
				LPCSTR line		= ini->r_string(sect,buf);	R_ASSERT(_GetItemCount(line)==2);
				LPCSTR	name	= _GetItem(line,0,buf);
				LPCSTR	new_sect= strlwr(_GetItem(line,1,buf1));
				I				= xr_new<CUICustomMenuItem> (owner,root,name,(LPCSTR)NULL,(LPCSTR)NULL,(OnExecuteEvent)NULL,draw);
				UIParseMenu(owner,ini,I,new_sect,exec,draw);
				root->AppendItem(I);
			}
			// append if exist value item
			sprintf(buf,"item_%d",i);
			if (ini->line_exist(sect,buf)){
				LPCSTR line		= ini->r_string(sect,buf);	R_ASSERT(_GetItemCount(line)>=2);
				LPCSTR	name	= _GetItem(line,0,buf);
				LPCSTR	value0	= _GetItem(line,1,buf1);
				LPCSTR	value1	= (_GetItemCount(line)>2)?_GetItem(line,2,buf2):0;
				root->AppendItem(xr_new<CUICustomMenuItem> (owner,root,name,value0,value1,exec,draw));
			}
		}
	}
	return root;
}
//--------------------------------------------------------------------

CUICustomMenuItem* UILoadMenu		(CUICustomMenu* owner, LPCSTR ini_name, LPCSTR sect, OnExecuteEvent exec, OnItemDrawEvent draw)
{
	// check ini exist
	CUICustomMenuItem*	I=0;
	string256			fn;
	if (FS.exist(fn,"$game_data$",ini_name))
	{
		CInifile* ini		= CInifile::Create(fn);
		I					= UIParseMenu(owner,ini,0,sect,exec,draw);
		CInifile::Destroy	(ini);
	}
	return I;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
// Class CUICustomMenu
//--------------------------------------------------------------------
CUICustomMenu::CUICustomMenu()
{
	bVisible		= FALSE;
	menu_root		= 0;	
	menu_active		= 0;
	m_Owner			= 0;
}
CUICustomMenu::~CUICustomMenu()
{
	xr_delete(menu_root);
}

void CUICustomMenu::Load(LPCSTR ini, LPCSTR start_sect, LPVOID owner, OnExecuteEvent exec)
{
	m_Owner				= owner;
	menu_root			= UILoadMenu(this,ini,start_sect,exec);
	menu_active			= menu_root;
}

void CUICustomMenuItem::DrawItem	(CGameFont* F, int num, int col)
{
	if (OnItemDraw)	{
		OnItemDraw	(F,num,col);
	}else{
		switch(col){
		case 0:						F->OutNext	("%d. %s",num,caption);	break;
		case 1: if (!HasChildren())	F->OutNext	("%s",value0);			break;
		default:	Debug.fatal		("CUICustomMenuItem::DrawItem - invalid usage");
		}
	}
}
