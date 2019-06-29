////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_trader_abstract.cpp
//	Created 	: 27.10.2005
//  Modified 	: 27.10.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife trader abstract class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "alife_simulator.h"
#include "specific_character.h"
#include "ai_space.h"
#include "alife_object_registry.h"
#include "ai_debug.h"
#include "alife_graph_registry.h"
#include "xrServer.h"
#include "alife_schedule_registry.h"

#ifdef DEBUG
	extern Flags32 psAI_Flags;
#endif

void CSE_ALifeTraderAbstract::spawn_supplies	()
{
	CSE_ALifeDynamicObject		*dynamic_object = smart_cast<CSE_ALifeDynamicObject*>(this);
	VERIFY						(dynamic_object);
	CSE_Abstract				*abstract = dynamic_object->alife().spawn_item("device_pda",base()->o_Position,dynamic_object->m_tNodeID,dynamic_object->m_tGraphID,base()->ID);
	CSE_ALifeItemPDA			*pda = smart_cast<CSE_ALifeItemPDA*>(abstract);
	pda->m_original_owner		= base()->ID;

#ifdef XRGAME_EXPORTS
	character_profile			();
	m_SpecificCharacter			= shared_str();
	m_community_index			= NO_COMMUNITY_INDEX;
	pda->m_specific_character	= specific_character();
#endif

	if(m_SpecificCharacter.size())
	{
		//если в custom data объекта есть
		//секция [dont_spawn_character_supplies]
		//то не вызывать spawn из selected_char.SupplySpawn()
		bool specific_character_supply = true;	

		if (xr_strlen(dynamic_object->m_ini_string))
		{
#pragma warning(push)
#pragma warning(disable:4238)
			CInifile					ini(
				&IReader				(
					(void*)(*dynamic_object->m_ini_string),
					xr_strlen(dynamic_object->m_ini_string)
				),
				FS.get_path("$game_config$")->m_Path
			);
#pragma warning(pop)

			if (ini.section_exist("dont_spawn_character_supplies")) 
				specific_character_supply = false;
		}

		if(specific_character_supply)
		{
			CSpecificCharacter selected_char;
			selected_char.Load(m_SpecificCharacter);
			dynamic_object->spawn_supplies(selected_char.SupplySpawn());
		}
	}
}

void CSE_ALifeTraderAbstract::vfInitInventory()
{
	m_fCumulativeItemMass		= 0.f;
	m_iCumulativeItemVolume		= 0;
}

#ifdef DEBUG
bool CSE_ALifeTraderAbstract::check_inventory_consistency	()
{
	int							volume = 0;
	float						mass = 0.f;
	xr_vector<ALife::_OBJECT_ID>::const_iterator	I = base()->children.begin();
	xr_vector<ALife::_OBJECT_ID>::const_iterator	E = base()->children.end();
	for ( ; I != E; ++I) {
		CSE_ALifeDynamicObject	*object = ai().alife().objects().object(*I,true);
		if (!object)
			continue;

		CSE_ALifeInventoryItem	*item = smart_cast<CSE_ALifeInventoryItem*>(object);
		if (!item)
			continue;

		volume					+= item->m_iVolume;
		mass					+= item->m_fMass;
	}

	R_ASSERT2					(fis_zero(m_fCumulativeItemMass - mass,EPS_L),base()->name_replace());
	if (!fis_zero(m_fCumulativeItemMass - mass,EPS_L))
		return					(false);

	R_ASSERT2					(m_iCumulativeItemVolume == volume,base()->name_replace());
	if (m_iCumulativeItemVolume != volume)
		return					(false);

#ifdef DEBUG
//	if (psAI_Flags.test(aiALife)) {
//		Msg						("[LSS] [%s] inventory is consistent [%f][%d]",base()->name_replace(),mass,volume);
//	}
#endif

	return						(true);
}
#endif

void CSE_ALifeTraderAbstract::attach	(CSE_ALifeInventoryItem *tpALifeInventoryItem, bool bALifeRequest, bool bAddChildren)
{
	VERIFY						(!bAddChildren || check_inventory_consistency());

	if (bALifeRequest) {
		if (bAddChildren) {
#ifdef DEBUG
			if (psAI_Flags.test(aiALife)) {
				Msg						("[LSS] Adding item [%s][%d] to the [%s] children list",tpALifeInventoryItem->base()->name_replace(),tpALifeInventoryItem->base()->ID,base()->name_replace());
			}
#endif

			R_ASSERT2					(std::find(base()->children.begin(),base()->children.end(),tpALifeInventoryItem->base()->ID) == base()->children.end(),"Item is already inside the inventory");
			base()->children.push_back	(tpALifeInventoryItem->base()->ID);
		}
#ifdef DEBUG
		if (psAI_Flags.test(aiALife)) {
			Msg							("[LSS] Assigning parent [%s] to item [%s][%d]",base()->name_replace(),tpALifeInventoryItem->base()->name_replace(),tpALifeInventoryItem->base()->ID);
		}
#endif
		tpALifeInventoryItem->base()->ID_Parent	= base()->ID;
	}
#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg								("[LSS] Updating [%s] inventory with attached item [%s][%d]",base()->name_replace(),tpALifeInventoryItem->base()->name_replace(),tpALifeInventoryItem->base()->ID);
	}
#endif
	m_fCumulativeItemMass				+= tpALifeInventoryItem->m_fMass;
	m_iCumulativeItemVolume				+= tpALifeInventoryItem->m_iVolume;

	VERIFY								(fis_zero(m_fCumulativeItemMass) || (m_fCumulativeItemMass >= 0.f));
	VERIFY								(!bALifeRequest || !bAddChildren || check_inventory_consistency());
}

void CSE_ALifeTraderAbstract::detach(CSE_ALifeInventoryItem *tpALifeInventoryItem, ALife::OBJECT_IT *I, bool bALifeRequest, bool bRemoveChildren)
{
	VERIFY								(!bRemoveChildren || check_inventory_consistency());
	if (bALifeRequest) {
		if (!I) {
			if (bRemoveChildren) {
				ALife::OBJECT_IT				I = std::find	(base()->children.begin(),base()->children.end(),tpALifeInventoryItem->base()->ID);
				R_ASSERT2						(base()->children.end() != I,"Can't detach an item which is not on my own");
#ifdef DEBUG
				if (psAI_Flags.test(aiALife)) {
					Msg							("[LSS] Removinng item [%s][%d] from [%s] children list",tpALifeInventoryItem->base()->name_replace(),tpALifeInventoryItem->base()->ID,base()->name_replace());
				}
#endif
				base()->children.erase			(I);
			}
		}
		else {
#ifdef DEBUG
			if (psAI_Flags.test(aiALife)) {
				Msg								("[LSS] Removinng item [%s][%d] from [%s] children list",tpALifeInventoryItem->base()->name_replace(),tpALifeInventoryItem->base()->ID,base()->name_replace());
			}
#endif
			base()->children.erase				(*I);
		}
		tpALifeInventoryItem->base()->ID_Parent	= 0xffff;
	}
	
	CSE_ALifeDynamicObject					*l_tpALifeDynamicObject1 = smart_cast<CSE_ALifeDynamicObject*>(tpALifeInventoryItem);
	CSE_ALifeDynamicObject					*l_tpALifeDynamicObject2 = smart_cast<CSE_ALifeDynamicObject*>(this);
	R_ASSERT2								(l_tpALifeDynamicObject1 && l_tpALifeDynamicObject2,"Invalid parent or children objects");
#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg									("[LSS] Removing parent [%s] from the item [%s][%d] and updating its position and graph point [%f][%f][%f] : [%d]",base()->name_replace(),tpALifeInventoryItem->base()->name_replace(),tpALifeInventoryItem->base()->ID,VPUSH(l_tpALifeDynamicObject2->o_Position),l_tpALifeDynamicObject2->m_tGraphID);
	}
#endif
	l_tpALifeDynamicObject1->o_Position		= l_tpALifeDynamicObject2->o_Position;
	l_tpALifeDynamicObject1->m_tNodeID		= l_tpALifeDynamicObject2->m_tNodeID;
	l_tpALifeDynamicObject1->m_tGraphID		= l_tpALifeDynamicObject2->m_tGraphID;
	l_tpALifeDynamicObject1->m_fDistance	= l_tpALifeDynamicObject2->m_fDistance;
	tpALifeInventoryItem->m_tPreviousParentID = l_tpALifeDynamicObject2->ID;

#ifdef DEBUG
	if (psAI_Flags.test(aiALife)) {
		Msg						("[LSS] Updating [%s] inventory with detached item [%s][%d]",base()->name_replace(),tpALifeInventoryItem->base()->name_replace(),tpALifeInventoryItem->base()->ID);
	}
#endif
	m_fCumulativeItemMass		-= tpALifeInventoryItem->m_fMass;
	m_iCumulativeItemVolume		-= tpALifeInventoryItem->m_iVolume;

//	if(!(fis_zero(m_fCumulativeItemMass) || m_fCumulativeItemMass > 0.f)){
//		Msg("ERROR !!! m_fCumulativeItemMass = %f",m_fCumulativeItemMass);
//	}
//	VERIFY									(m_fCumulativeItemMass >= 0.f);
	VERIFY									(!bALifeRequest || check_inventory_consistency());
}

void CSE_ALifeTraderAbstract::add_online	(const bool &update_registries)
{
	CSE_ALifeDynamicObject		*object = smart_cast<CSE_ALifeDynamicObject*>(this);
	VERIFY						(object);

	NET_Packet					tNetPacket;
	ClientID					clientID;
	clientID.set				(object->alife().server().GetServerClient() ? object->alife().server().GetServerClient()->ID.value() : 0);

	ALife::OBJECT_IT			I = object->children.begin();
	ALife::OBJECT_IT			E = object->children.end();
	for ( ; I != E; ++I) {
//	this was for the car only
//		if (*I == ai().alife().graph().actor()->ID)
//			continue;
//
		CSE_ALifeDynamicObject	*l_tpALifeDynamicObject = ai().alife().objects().object(*I);
		CSE_ALifeInventoryItem	*l_tpALifeInventoryItem = smart_cast<CSE_ALifeInventoryItem*>(l_tpALifeDynamicObject);
		R_ASSERT2				(l_tpALifeInventoryItem,"Non inventory item object has parent?!");
		l_tpALifeInventoryItem->base()->s_flags.or(M_SPAWN_UPDATE);
		CSE_Abstract			*l_tpAbstract = smart_cast<CSE_Abstract*>(l_tpALifeInventoryItem);
		object->alife().server().entity_Destroy(l_tpAbstract);

#ifdef DEBUG
		if (psAI_Flags.test(aiALife))
			Msg					("[LSS] Spawning item [%s][%s][%d]",l_tpALifeInventoryItem->base()->name_replace(),*l_tpALifeInventoryItem->base()->s_name,l_tpALifeDynamicObject->ID);
#endif

//		R_ASSERT3								(ai().level_graph().valid_vertex_id(l_tpALifeDynamicObject->m_tNodeID),"Invalid vertex for object ",l_tpALifeInventoryItem->name_replace());
		l_tpALifeDynamicObject->o_Position		= object->o_Position;
		l_tpALifeDynamicObject->m_tNodeID		= object->m_tNodeID;
		object->alife().server().Process_spawn	(tNetPacket,clientID,FALSE,l_tpALifeInventoryItem->base());
		l_tpALifeDynamicObject->s_flags.and		(u16(-1) ^ M_SPAWN_UPDATE);
		l_tpALifeDynamicObject->m_bOnline		= true;
	}

	if (!update_registries)
		return;

	object->alife().scheduled().remove	(object);
	object->alife().graph().remove		(object,object->m_tGraphID,false);
}

void CSE_ALifeTraderAbstract::add_offline	(const xr_vector<ALife::_OBJECT_ID> &saved_children, const bool &update_registries)
{
	CSE_ALifeDynamicObject		*object = smart_cast<CSE_ALifeDynamicObject*>(this);
	VERIFY						(object);

	for (u32 i=0, n=saved_children.size(); i<n; ++i) {
		CSE_ALifeDynamicObject	*child = smart_cast<CSE_ALifeDynamicObject*>(ai().alife().objects().object(saved_children[i],true));
		R_ASSERT				(child);
		child->m_bOnline		= false;

		CSE_ALifeInventoryItem	*inventory_item = smart_cast<CSE_ALifeInventoryItem*>(child);
		VERIFY2					(inventory_item,"Non inventory item object has parent?!");
#ifdef DEBUG
		if (psAI_Flags.test(aiALife))
			Msg					("[LSS] Destroying item [%s][%s][%d]",inventory_item->base()->name_replace(),*inventory_item->base()->s_name,inventory_item->base()->ID);
#endif
		
		ALife::_OBJECT_ID				item_id = inventory_item->base()->ID;
		inventory_item->base()->ID		= object->alife().server().PerformIDgen(item_id);

		if (!child->can_save()) {
			object->alife().release		(child);
			--i;
			--n;
			continue;
		}

#ifdef DEBUG
		if (!child->client_data.empty())
			Msg							("CSE_ALifeTraderAbstract::add_offline: client_data is cleared for [%d][%s]",child->ID,child->name_replace());
#endif // DEBUG
		child->client_data.clear		();
		object->alife().graph().add		(child,child->m_tGraphID,false);
		object->alife().graph().attach	(*object,inventory_item,child->m_tGraphID,true);
	}

	if (!update_registries)
		return;

	object->alife().scheduled().add		(object);
	object->alife().graph().add			(object,object->m_tGraphID,false);
}
