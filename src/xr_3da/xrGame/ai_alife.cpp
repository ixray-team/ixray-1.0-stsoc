////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife.cpp
//	Created 	: 25.12.2002
//  Modified 	: 29.12.2002
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"
#include "ai_space.h"
#include "game_graph.h"
#include "ef_storage.h"
#include "xrserver_objects_alife_monsters.h"
#include "date_time.h"

using namespace ALife;

CSE_ALifeSimulator::CSE_ALifeSimulator(xrServer *tpServer)
{
	m_tpServer					= tpServer;
	m_bLoaded					= false;
	m_tpActor					= 0;
	m_caSaveName[0]				= 0;
	shedule_register			();
	for (int i=0; i<2; ++i) {
		m_tpaCombatGroups[i].clear();
		m_tpaCombatGroups[i].reserve(255);
	}
	VERIFY						(!ai().get_alife());
	ai().set_alife				(this);
	m_dwInventorySlotCount		= pSettings->r_u32("inventory","slots");
	m_tpWeaponVector.resize		(m_dwInventorySlotCount);
	m_baMarks.assign			(u16(-1),false);

	m_tpItems1.reserve			(MAX_STACK_DEPTH);
	m_tpItems2.reserve			(MAX_STACK_DEPTH);
	m_tpBlockedItems1.reserve	(MAX_STACK_DEPTH);
	m_tpBlockedItems2.reserve	(MAX_STACK_DEPTH);
	m_tpTrader1.reserve			(MAX_STACK_DEPTH);
	m_tpTrader1.reserve			(MAX_STACK_DEPTH);
	m_tpSums1.reserve			(MAX_STACK_DEPTH);
	m_tpSums2.reserve			(MAX_STACK_DEPTH);
	m_changing_level			= false;

	shedule.t_min				= pSettings->r_s32	("alife","schedule_min");
	shedule.t_max				= pSettings->r_s32	("alife","schedule_max");
	m_max_process_time			= pSettings->r_s32	("alife","process_time");
	m_fSwitchDistance			= pSettings->r_float("alife","switch_distance");
	m_fSwitchFactor				= pSettings->r_float("alife","switch_factor");
	m_fTimeFactor				= pSettings->r_float("alife","time_factor");
	m_fNormalTimeFactor			= pSettings->r_float("alife","normal_time_factor");
	m_dwMaxCombatIterationCount	= pSettings->r_u32	("alife","max_combat_iteration_count");
	m_update_monster_factor		= pSettings->r_float("alife","update_monster_factor");
	vfSetProcessTime			((int)m_max_process_time);
	
	u32							years,months,days,hours,minutes,seconds;
	sscanf						(pSettings->r_string("alife","start_time"),"%d:%d:%d",&hours,&minutes,&seconds);
	sscanf						(pSettings->r_string("alife","start_date"),"%d.%d.%d",&days,&months,&years);
	m_start_time				= generate_time(years,months,days,hours,minutes,seconds);
}

CSE_ALifeSimulator::~CSE_ALifeSimulator()
{
	shedule_unregister		();
	D_OBJECT_PAIR_IT		I = m_tObjectRegistry.begin();
	D_OBJECT_PAIR_IT		E = m_tObjectRegistry.end();
	for ( ; I != E; ++I) {
		CSE_Abstract					*l_tpAbstract = dynamic_cast<CSE_Abstract*>((*I).second);
		if ((*I).second->m_bOnline)
			m_tpServer->PerformIDgen	(l_tpAbstract->ID);		
		m_tpServer->entity_Destroy		(l_tpAbstract);
	}

	ai().set_alife			(0);
}

float CSE_ALifeSimulator::shedule_Scale()
{
	return					(.5f); // (schedule_min + schedule_max)*0.5f
}

BOOL CSE_ALifeSimulator::shedule_Ready()
{
	return					(TRUE);
}

ref_str CSE_ALifeSimulator::cName()
{
	return					(ref_str("Simulator"));
}; 

void CSE_ALifeSimulator::vfUpdateDynamicData(CSE_ALifeDynamicObject *tpALifeDynamicObject)
{
	CSE_ALifeGraphRegistry::Update		(tpALifeDynamicObject);
	CSE_ALifeTraderRegistry::Update		(tpALifeDynamicObject);
	CSE_ALifeScheduleRegistry::Update	(tpALifeDynamicObject);
	
	CSE_ALifeInventoryItem				*l_tpALifeInventoryItem = dynamic_cast<CSE_ALifeInventoryItem*>(tpALifeDynamicObject);
	if (l_tpALifeInventoryItem && l_tpALifeInventoryItem->bfAttached()) {
		CSE_ALifeDynamicObject			*II = object(l_tpALifeInventoryItem->ID_Parent);
		if (std::find(II->children.begin(),II->children.end(),l_tpALifeInventoryItem->ID) != II->children.end()) {
#ifdef DEBUG
			if (psAI_Flags.test(aiALife)) {
				Msg						("[LSS] Specified item [%s][%d] is already attached to the specified object [%s][%d]",l_tpALifeInventoryItem->s_name_replace,l_tpALifeInventoryItem->ID,II->s_name_replace,II->ID);
			}
#endif
			Debug.fatal					("[LSS] Cannot recover from the previous error!");
		}
		II->children.push_back			(l_tpALifeInventoryItem->ID);
	}
	
	CSE_ALifeSchedulable				*l_tpALifeSchedulable = dynamic_cast<CSE_ALifeSchedulable*>(tpALifeDynamicObject);
	if (l_tpALifeSchedulable)
		l_tpALifeSchedulable->m_tpALife = this;
}

void CSE_ALifeSimulator::vfUpdateDynamicData(bool bReserveID)
{
	// update objects
	if (bReserveID) {
		// initialize
		CSE_ALifeGraphRegistry::Init	();
		CSE_ALifeTraderRegistry::Init	();
		CSE_ALifeScheduleRegistry::Init	();

		D_OBJECT_PAIR_IT				I = m_tObjectRegistry.begin();
		D_OBJECT_PAIR_IT				E = m_tObjectRegistry.end();
		for ( ; I != E; ++I) {
			vfUpdateDynamicData			((*I).second);
			_OBJECT_ID					l_tObjectID = (*I).second->ID;
			(*I).second->ID				= m_tpServer->PerformIDgen(l_tObjectID);
			R_ASSERT2					(l_tObjectID == (*I).second->ID,"Can't reserve a particular object identifier");
		}
		// update events
		{
			EVENT_PAIR_IT				I = m_tEventRegistry.begin();
			EVENT_PAIR_IT				E = m_tEventRegistry.end();
			for ( ; I != E; ++I)
				vfAddEventToGraphPoint	((*I).second,(*I).second->m_tGraphID);
		}
	}
}

void CSE_ALifeSimulator::Save()
{
	if (!xr_strlen(m_caSaveName))
		Log						("There is no file name specified!");
	else
		Save(m_caSaveName);
}

void CSE_ALifeSimulator::Save(LPCSTR caSaveName)
{
	strconcat					(m_caSaveName,caSaveName,SAVE_EXTENSION);
	CMemoryWriter				tStream;
	CSE_ALifeHeader::Save		(tStream);
	CSE_ALifeGameTime::Save		(tStream);
	CSE_ALifeObjectRegistry::Save(tStream);
	CSE_ALifeEventRegistry::Save(tStream);
	CSE_ALifeTaskRegistry::Save	(tStream);
	CSE_ALifeAnomalyRegistry::Save(tStream);
	CSE_ALifeOrganizationRegistry::Save(tStream);
	CSE_ALifeNewsRegistry::Save	(tStream);
	string256					S;
	FS.update_path				(S,"$game_saves$",m_caSaveName);
	tStream.save_to				(S);
	Msg							("* Game is successfully saved to file '%s'",m_caSaveName);
}

void CSE_ALifeSimulator::Load	(LPCSTR caSaveName)
{
	// initializing
	Memory.mem_compact			();
	u32							dwMemUsage = Memory.mem_usage();
	pApp->LoadTitle				("SERVER: Loading a-life simulator...");
	m_tALifeVersion				= ALIFE_VERSION;
	m_tpActor					= 0;
	m_tGameTime					= 0;
	m_bActorEnabled				= true;
	strconcat					(m_caSaveName,caSaveName,SAVE_EXTENSION);

	// loading default settings from 'system.ltx'
	Log							("* Loading parameters...");
	vfSetSwitchDistance			(m_fSwitchDistance);

	string256					caFileName;
	IReader						*tpStream;

	// loading spawn registry
	R_ASSERT3					(FS.exist(caFileName, "$game_spawn$", caSaveName,".spawn"),"Can't find file spawn file:",caSaveName);
	int							spawn_age = FS.get_file_age(caFileName);
	string256					file_name;
	FS.update_path				(file_name,"$game_data$",GRAPH_NAME);
	int							graph_age = FS.get_file_age(file_name);
	VERIFY3						(spawn_age >= graph_age,"Rebuild spawn file ",caFileName);

	tpStream					= FS.r_open(caFileName);
	Log							("* Loading spawn registry");
	CSE_ALifeSpawnRegistry::Load(*tpStream);
	FS.r_close					(tpStream);

	// loading game
	Log							("* Loading simulator...");
	if (!FS.exist(caFileName,"$game_saves$",m_caSaveName)) {
		// creating _new_ game
		Log						("* Creating New Game...");
		vfNewGame				(caSaveName);
		strcpy					(m_caSaveName,caSaveName);
	}
	else {
		strcpy					(m_caSaveName,caSaveName);
		int						saved_age = FS.get_file_age(caFileName);
		VERIFY3					(saved_age >= spawn_age,"Delete saved game and try again ",caSaveName);
		tpStream				= FS.r_open(caFileName);
		if (!tpStream) {
			string4096			S;
			sprintf				(S,"Can't open saved game file '%s'",m_caSaveName);
			R_ASSERT2			(tpStream,S);
		}
		CSE_ALifeHeader::Load	(*tpStream);
		CSE_ALifeGameTime::Load	(*tpStream);
		Log						("* Loading objects...");
		CSE_ALifeObjectRegistry::Load(*tpStream);
		Log						("* Loading events...");
		CSE_ALifeEventRegistry::Load(*tpStream);
		Log						("* Loading tasks...");
		CSE_ALifeTaskRegistry::Load	(*tpStream);
		Log						("* Loading anomaly map...");
		CSE_ALifeAnomalyRegistry::Load(*tpStream);
		Log						("* Loading organizations and discoveries...");
		CSE_ALifeOrganizationRegistry::Load(*tpStream);
		Log						("* Loading news...");
		CSE_ALifeNewsRegistry::Load	(*tpStream);
		Log						("* Building dynamic objects...");
		vfUpdateDynamicData		();
	}
	
	// updating server options information for loading correct level
	{
		R_ASSERT2				(m_tpActor,"There is no actor spawn point!");
		string64				S;
		strcpy					(S,*m_cppServerOptions);
		LPSTR					l_cpPointer = strchr(S,'/');
		R_ASSERT2				(l_cpPointer,"Invalid server options!");
		xr_map<_LEVEL_ID,CGameGraph::SLevel>::const_iterator I = ai().game_graph().header().levels().find(ai().game_graph().vertex(m_tpActor->m_tGraphID)->level_id());
		R_ASSERT2				(ai().game_graph().header().levels().end() != I,"Graph point level ID not found!");
		strconcat				(*m_cppServerOptions,(*I).second.name(),l_cpPointer);

		int						id = pApp->Level_ID((*I).second.name());
		VERIFY					(id >= 0);
		pApp->Level_Set			(id);
		ai().load				((*I).second.name());
	}

	// finalizing
	m_bLoaded					= true;
	Msg							("* Loading ALife Simulator is successfully completed (%7.3f Mb)",float(Memory.mem_usage() - dwMemUsage)/1048576.0);
	pApp->LoadTitle				("SERVER: Connecting...");
}

void CSE_ALifeSimulator::vfNewGame(LPCSTR caSaveName)
{
	CSE_ALifeGraphRegistry::Init	();
	CSE_ALifeTraderRegistry::Init	();
	CSE_ALifeScheduleRegistry::Init	();
	CSE_ALifeNewsRegistry::clear	();

	m_tpServer->PerformIDgen	(0x0000);
	D_OBJECT_P_IT				B = m_tpSpawnPoints.begin();
	D_OBJECT_P_IT				E = m_tpSpawnPoints.end();
	for (D_OBJECT_P_IT I = B ; I != E; ) {
		u32						l_dwGroupID	= (*I)->m_dwSpawnGroup;
		for (D_OBJECT_P_IT m = I + 1, j = I; (m != E) && ((*m)->m_dwSpawnGroup == l_dwGroupID); ++m) ;

		CSE_Abstract			*tpSE_Abstract = F_entity_Create((*I)->s_name);
		R_ASSERT2				(tpSE_Abstract,"Can't create entity.");

		CSE_ALifeDynamicObject	*i = dynamic_cast<CSE_ALifeDynamicObject*>(tpSE_Abstract);
		R_ASSERT2				(i,"Non-ALife object in the 'game.spawn'");

		CSE_ALifeAnomalousZone	*tpALifeAnomalousZone = dynamic_cast<CSE_ALifeAnomalousZone*>(i);
		if (tpALifeAnomalousZone) {
			for ( ; j != m; ++j) {
				CSE_ALifeAnomalousZone	*tpALifeAnomalousZone = dynamic_cast<CSE_ALifeAnomalousZone*>(*j);
				R_ASSERT2				(tpALifeAnomalousZone,"Anomalous zones are grouped with incompatible objects!");
				vfCreateObjectFromSpawnPoint(i,*j,_SPAWN_ID(j - B));
			}
		}
		xr_delete				(tpSE_Abstract);
		I = m;
	}

	m_tGameTime					= m_start_time;
	m_tZoneState				= eZoneStateSurge;
	_TIME_ID					l_tFinishTime = m_tGameTime + 0*120000;//3*7*24*3600*1000; // 3 weeks in milliseconds
	float						l_fTimeFactor = m_fTimeFactor;
	m_fTimeFactor				= 1;

	vfInitAI_ALifeMembers		();
	// simulating ALife during the given time period
	do {
		switch (m_tZoneState) {
			case eZoneStateSurge : {
				vfPerformSurge		();
				m_tLastSurgeTime	= tfGetGameTime();
				m_tNextSurgeTime	= m_tLastSurgeTime + 7*24*3600*1000; // a week in milliseconds
				m_tZoneState		= eZoneStateAfterSurge;
				break;
			}
			case eZoneStateAfterSurge : {
				if (tfGetGameTime() > m_tNextSurgeTime) {
					m_tZoneState	= eZoneStateSurge;
					break;
				}

				SCHEDULE_P_PAIR_IT	I = m_tpScheduledObjects.begin();
				SCHEDULE_P_PAIR_IT	E = m_tpScheduledObjects.end();
				for ( ; I != E; ++I)
					(*I).second->Update();

				break;
			}
			default : NODEFAULT;
		}
	}
	while (tfGetGameTime() < l_tFinishTime);

	m_fTimeFactor				= l_fTimeFactor;
	Save						(caSaveName);
}

void CSE_ALifeSimulator::vfInitAI_ALifeMembers()
{
	ai().ef_storage().m_tpCurrentMember	= 0;
	ai().ef_storage().m_tpCurrentEnemy	= 0;
	ai().ef_storage().m_tpGameObject	= 0;
	ai().ef_storage().m_tpALife			= this;
}

bool CSE_ALifeSimulator::change_level	(NET_Packet &net_packet)
{
	if (m_changing_level)
		return					(false);

	m_changing_level			= true;
	for (u32 i=0, n = m_tpActor->children.size(); i<n; ++i)
		if (object(m_tpActor->children[i],true))
			Msg					("%2d[%5d] : Item %s",i,m_tpActor->children[i],object(m_tpActor->children[i])->s_name_replace);

	ALife::_GRAPH_ID			safe_graph_vertex_id	= m_tpActor->m_tGraphID;
	u32							safe_level_vertex_id	= m_tpActor->m_tNodeID;
	Fvector						safe_position			= m_tpActor->o_Position;
	Fvector						safe_angles				= m_tpActor->o_Angle;
	
	net_packet.r				(&m_tpActor->m_tGraphID,sizeof(m_tpActor->m_tGraphID));
	net_packet.r				(&m_tpActor->m_tNodeID,sizeof(m_tpActor->m_tNodeID));
	net_packet.r_vec3			(m_tpActor->o_Position);
	net_packet.r_vec3			(m_tpActor->o_Angle);
	
	Save						();

	m_tpActor->m_tGraphID		= safe_graph_vertex_id;
	m_tpActor->m_tNodeID		= safe_level_vertex_id;
	m_tpActor->o_Position		= safe_position;
	m_tpActor->o_Angle			= safe_angles;

	return						(true);
}

void CSE_ALifeSimulator::vfCreateItem	(CSE_ALifeObject *object)
{
	CSE_ALifeDynamicObject		*dynamic_object = dynamic_cast<CSE_ALifeDynamicObject*>(object);
	if (!dynamic_object)
		return;
	
	if (0xffff != dynamic_object->ID_Parent) {
		u16							id = dynamic_object->ID_Parent;
		CSE_ALifeDynamicObject		*parent = this->object(id);
		VERIFY						(parent);
		dynamic_object->m_tGraphID	= parent->m_tGraphID;
		dynamic_object->o_Position	= parent->o_Position;
		dynamic_object->m_tNodeID	= parent->m_tNodeID;
		dynamic_object->ID_Parent	= 0xffff;
		CSE_ALifeObjectRegistry::Add(dynamic_object);
		vfUpdateDynamicData			(dynamic_object);
		vfRemoveObjectFromGraphPoint(dynamic_object,parent->m_tGraphID,false);
		dynamic_object->ID_Parent	= id;
	}
	else {
		CSE_ALifeObjectRegistry::Add(dynamic_object);
		vfUpdateDynamicData			(dynamic_object);
	}
	
	dynamic_object->m_bOnline		= true;
}

void CSE_ALifeSimulator::vfSetProcessTime			(int	iMicroSeconds)
{
	m_max_process_time	= iMicroSeconds;
	CSE_ALifeGraphRegistry::set_process_time	(u64(float(m_max_process_time) - float(m_max_process_time)*m_update_monster_factor)*CPU::cycles_per_microsec);
	CSE_ALifeScheduleRegistry::set_process_time	(u64(float(m_max_process_time)*m_update_monster_factor)*CPU::cycles_per_microsec);
	m_max_process_time	*= CPU::cycles_per_microsec;
}