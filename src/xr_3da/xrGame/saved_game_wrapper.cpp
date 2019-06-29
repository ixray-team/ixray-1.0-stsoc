////////////////////////////////////////////////////////////////////////////
//	Module 		: saved_game_wrapper.cpp
//	Created 	: 21.02.2006
//  Modified 	: 21.02.2006
//	Author		: Dmitriy Iassenev
//	Description : saved game wrapper class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "saved_game_wrapper.h"
#include "alife_time_manager.h"
#include "alife_object_registry.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "ai_space.h"
#include "game_graph.h"

extern LPCSTR alife_section;

LPCSTR CSavedGameWrapper::saved_game_full_name	(LPCSTR saved_game_name, LPSTR result)
{
	string256					temp;
	strconcat					(temp,saved_game_name,SAVE_EXTENSION);
	FS.update_path				(result,"$game_saves$",temp);
	return						(result);
}

bool CSavedGameWrapper::saved_game_exist		(LPCSTR saved_game_name)
{
	string256					file_name;
	return						(!!FS.exist(saved_game_full_name(saved_game_name,file_name)));
}

CSavedGameWrapper::CSavedGameWrapper		(LPCSTR saved_game_name)
{
	string256					file_name;
	saved_game_full_name		(saved_game_name,file_name);
	R_ASSERT3					(FS.exist(file_name),"There is no saved game ",file_name);
	
	IReader						*stream = FS.r_open(file_name);
	u32							source_count = stream->r_u32();
	void						*source_data = xr_malloc(source_count);
	rtc_decompress				(source_data,source_count,stream->pointer(),stream->length() - sizeof(source_count));
	FS.r_close					(stream);

	IReader						reader(source_data,source_count);

	{
		CALifeTimeManager		time_manager(alife_section);
		time_manager.load		(reader);
		m_game_time				= time_manager.game_time();
	}

	{
		R_ASSERT2				(reader.find_chunk(OBJECT_CHUNK_DATA),"Can't find chunk OBJECT_CHUNK_DATA!");
		u32						count = reader.r_u32();
		VERIFY					(count > 0);
		CSE_ALifeDynamicObject	*object = CALifeObjectRegistry::get_object(reader);
		VERIFY					(object->ID == 0);
		CSE_ALifeCreatureActor	*actor = smart_cast<CSE_ALifeCreatureActor*>(object);
		VERIFY					(actor);

		m_actor_health			= actor->g_Health();
		m_level_id				= ai().game_graph().vertex(object->m_tGraphID)->level_id();

		F_entity_Destroy		(object);
	}

	xr_free						(source_data);
}
