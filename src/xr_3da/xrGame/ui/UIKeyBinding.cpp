#include "StdAfx.h"
#include "UIKeyBinding.h"
#include "UIXmlInit.h"
#include "xrUIXmlParser.h"
#include "UIListItemAdv.h"
#include "UIEditKeyBind.h"
#include "../xr_level_controller.h"
#include "../string_table.h"

//extern void GetActionBinding(LPCSTR action, char* dst_buff);
//extern _keybind  keybind[];

CUIKeyBinding::CUIKeyBinding(){
	for (int i=0; i<2; i++)
		AttachChild(&m_header[i]);
	AttachChild(&m_frame);
	AttachChild(&m_list);
}

void CUIKeyBinding::InitFromXml(CUIXml& xml_doc, LPCSTR path){
	CUIXmlInit::InitWindow		(xml_doc, path, 0, this);
	string256 buf;
	CUIXmlInit::InitListWnd		(xml_doc, strconcat(buf,path,":list"),		0, &m_list);
	CUIXmlInit::InitFrameWindow	(xml_doc, strconcat(buf,path,":frame"),		0, &m_frame);
	CUIXmlInit::InitLabel		(xml_doc, strconcat(buf,path,":header_1"),	0, &m_header[0]);
	CUIXmlInit::InitLabel		(xml_doc, strconcat(buf,path,":header_2"),	0, &m_header[1]);
//	CUIXmlInit::InitLabel		(xml_doc, strconcat(buf,path,":header_3"),	0, &m_header[2]);

	CGameFont* pFake;
	CUIXmlInit::InitFont		(xml_doc, strconcat(buf,path,":list:group_name"),0,m_dwGroupColor,pFake);
	CUIXmlInit::InitFont		(xml_doc, strconcat(buf,path,":list:item_text"),0,m_dwItemColor,pFake);

	FillUpList();
}

void CUIKeyBinding::FillUpList(){
	CUIXml xml_doc;
	CStringTable st;
	xml_doc.Init(CONFIG_PATH, UI_PATH, "ui_keybinding.xml");

	int groupsCount = xml_doc.GetNodesNum("",0,"group");

	for (int i = 0; i<groupsCount; i++){
		// add group
		shared_str grp_name = xml_doc.ReadAttrib("group",i,"name");
		R_ASSERT(xr_strlen(grp_name));

		CUIListItemAdv*	pItem = xr_new<CUIListItemAdv>();
		pItem->AddField(*st.translate(grp_name), m_header[0].GetWidth());
		pItem->SetTextColor(m_dwGroupColor);
		m_list.AddItem(pItem);

		// add group items
		int commandsCount = xml_doc.GetNodesNum("group",i,"command");
		XML_NODE* tab_node = xml_doc.NavigateToNode("group",i);
		xml_doc.SetLocalRoot(tab_node);

		for (int j = 0; j<commandsCount; j++){
			// first field of list item
			shared_str command_id = xml_doc.ReadAttrib("command",j,"id");
			pItem = xr_new<CUIListItemAdv>();
			m_list.AddItem(pItem);
			pItem->AddField(*st.translate(command_id),m_header[0].GetWidth());

			shared_str exe = xml_doc.ReadAttrib("command",j,"exe");

#ifdef DEBUG
			if (!::IsActionExist(*exe))
			{
				pItem->AddField("not exist. update data",m_header[1].GetWidth());
				continue;
			}
#endif
			
			CUIEditKeyBind* pEditKB = xr_new<CUIEditKeyBind>();
			pEditKB->Init(0,0,m_header[1].GetWidth()/2,m_list.GetItemHeight());
			pEditKB->Register(*exe,"key_binding");

			pItem->AddWindow(pEditKB);
			pItem->SetTextColor(m_dwItemColor);
		}
		xml_doc.SetLocalRoot(xml_doc.GetRoot());
	}
#ifdef DEBUG
    CheckStructure(xml_doc);
#endif
}

#ifdef DEBUG
void CUIKeyBinding::CheckStructure(CUIXml& xml_doc){
	bool first = true;
	CUIListItemAdv*	pItem = false;
	for (int i=0; true; i++)
	{
		if (keybind[i].name)
		{
			if (IsActionExist(keybind[i].name, xml_doc))
				continue;
			else
			{
				if (first)
				{
					pItem = xr_new<CUIListItemAdv>();
					pItem->AddField("NEXT ITEMS NOT DESCRIBED IN COMMAND DESC LIST", m_header[0].GetWidth());
					pItem->SetTextColor(m_dwGroupColor);
					m_list.AddItem(pItem);
					first = false;
				}

				pItem = xr_new<CUIListItemAdv>();
				m_list.AddItem(pItem);
				pItem->AddField(keybind[i].name,m_header[0].GetWidth());
			}
		}
		else
			break;				
	}
}

bool CUIKeyBinding::IsActionExist(LPCSTR action, CUIXml& xml_doc){
	bool ret = false;
	int groupsCount = xml_doc.GetNodesNum("",0,"group");
	if (0 == xr_strcmp(action,"brutal_porn"))
	{
		Msg("hello");
	}

	for (int i = 0; i<groupsCount; i++){
		// add group items
		int commandsCount = xml_doc.GetNodesNum("group",i,"command");
		XML_NODE* tab_node = xml_doc.NavigateToNode("group",i);
		xml_doc.SetLocalRoot(tab_node);

		for (int j = 0; j<commandsCount; j++){
			// first field of list item
			shared_str command_id = xml_doc.ReadAttrib("command",j,"exe");
			if (0 == xr_strcmp(action, *command_id))
			{
				ret = true;
				break;
			}
		}
		xml_doc.SetLocalRoot(xml_doc.GetRoot());
		if (ret)
			break;
	}
	return ret;
}
#endif
