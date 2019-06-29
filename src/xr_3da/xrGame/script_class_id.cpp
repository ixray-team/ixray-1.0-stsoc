////////////////////////////////////////////////////////////////////////////
//	Module 		: script_class_id.cpp
//	Created 	: 18.04.2004
//  Modified 	: 18.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Script class identifiers
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_class_id.h"

CLSID_Holder::~CLSID_Holder	()
{
}

void CLSID_Holder::init		()
{
	VERIFY				(m_classes.empty());

	add					("actor",				CLSID_OBJECT_ACTOR			);
	add					("hanging_lamp",		CLSID_OBJECT_HLAMP			);
	add					("entity",				CLSID_ENTITY				);
	add					("spectator",			CLSID_SPECTATOR				);
	add					("projector",			CLSID_OBJECT_PROJECTOR		);
	add					("level_point",			CLSID_LEVEL_POINT			);
	add					("script_object",		CLSID_SCRIPT_OBJECT			);
	add					("spawn_group",			CLSID_AI_SPAWN_GROUP		);
	add					("graph_point",			CLSID_AI_GRAPH				);
	add					("crow",				CLSID_AI_CROW				);
	add					("rat",					CLSID_AI_RAT				);
	add					("rat_group",			CLSID_AI_RAT_GROUP			);
	add					("rat_wolf",			CLSID_AI_RAT_WOLF			);
	add					("zombie",				CLSID_AI_ZOMBIE				);
	add					("zombie_human",		CLSID_AI_ZOMBIE_HUMAN		);
	add					("poltergeist",			CLSID_AI_POLTERGEIST		);
	add					("flesh",				CLSID_AI_FLESH				);
	add					("flesh_group",			CLSID_AI_FLESH_GROUP		);
	add					("dwarf",				CLSID_AI_DWARF				);
	add					("scientist",			CLSID_AI_SCIENTIST			);
	add					("phantom",				CLSID_AI_PHANTOM			);
	add					("sponger",				CLSID_AI_SPONGER			);
	add					("controller",			CLSID_AI_CONTROLLER			);
	add					("bloodsucker",			CLSID_AI_BLOODSUCKER		);
	add					("soldier",				CLSID_AI_SOLDIER			);
	add					("stalker_dark",		CLSID_AI_STALKER_DARK		);
	add					("stalker_military",	CLSID_AI_STALKER_MILITARY	);
	add					("stalker",				CLSID_AI_STALKER			);
	add					("burer",				CLSID_AI_BURER				);
	add					("giant",				CLSID_AI_GIANT				);
	add					("chimera",				CLSID_AI_CHIMERA			);
	add					("fracture",			CLSID_AI_FRACTURE			);
	add					("dog_black",			CLSID_AI_DOG_BLACK			);
	add					("dog_red",				CLSID_AI_DOG_RED			);
	add					("trader",				CLSID_AI_TRADER				);
	add					("boar",				CLSID_AI_BOAR				);
	add					("idol",				CLSID_AI_IDOL				);
	add					("car_niva",			CLSID_CAR					);
	add					("helicopter",			CLSID_VEHICLE_HELICOPTER	);
	add					("event",				CLSID_EVENT					);
	add					("flyer",				CLSID_OBJECT_FLYER			);
	add					("door",				CLSID_OBJECT_DOOR			);
	add					("lift",				CLSID_OBJECT_LIFT			);
	add					("art_mercury_ball",	CLSID_AF_MERCURY_BALL		);
	add					("art_gravi",			CLSID_AF_GRAVI				);
	add					("art_black_drops",		CLSID_AF_BLACKDROPS			);
	add					("art_needles",			CLSID_AF_NEEDLES			);
	add					("art_bast",			CLSID_AF_BAST				);
	add					("art_gravi_black",		CLSID_AF_BLACK_GRAVI		);
	add					("art_dummy",			CLSID_AF_DUMMY				);
	add					("art_zuda",			CLSID_AF_ZUDA				);
	add					("art_thorn",			CLSID_AF_THORN				);
	add					("art_faded_ball",		CLSID_AF_FADED_BALL			);
	add					("art_electricball",	CLSID_AF_ELECTRIC_BALL		);
	add					("art_rusty_hair",		CLSID_AF_RUSTY_HAIR			);
	add					("art_galantine",		CLSID_AF_GALANTINE			);
	add					("artefact",			CLSID_ARTEFACT				);
	add					("wpn_m134",			CLSID_OBJECT_W_M134			);
	add					("wpn_fn2000",			CLSID_OBJECT_W_FN2000		);
	add					("wpn_ak74",			CLSID_OBJECT_W_AK74			);
	add					("wpn_lr300",			CLSID_OBJECT_W_LR300		);
	add					("wpn_hpsa",			CLSID_OBJECT_W_HPSA			);
	add					("wpn_pm",				CLSID_OBJECT_W_PM			);
	add					("wpn_fort",			CLSID_OBJECT_W_FORT			);
	add					("wpn_binocular",		CLSID_OBJECT_W_BINOCULAR	);
	add					("wpn_shotgun",			CLSID_OBJECT_W_SHOTGUN		);
	add					("wpn_svd",				CLSID_OBJECT_W_SVD			);
	add					("wpn_svu",				CLSID_OBJECT_W_SVU			);
	add					("wpn_rpg7",			CLSID_OBJECT_W_RPG7			);
	add					("wpn_val",				CLSID_OBJECT_W_VAL			);
	add					("wpn_vintorez",		CLSID_OBJECT_W_VINTOREZ		);
	add					("wpn_walter",			CLSID_OBJECT_W_WALTHER		);
	add					("wpn_usp45",			CLSID_OBJECT_W_USP45		);
	add					("wpn_groza",			CLSID_OBJECT_W_GROZA		);
	add					("wpn_knife",			CLSID_OBJECT_W_KNIFE		);
	add					("wpn_mounted",			CLSID_OBJECT_W_MOUNTED		);
	add					("ammo_m134",			CLSID_OBJECT_A_M134			);
	add					("ammo_fn2000",			CLSID_OBJECT_A_FN2000		);
	add					("ammo_ak74",			CLSID_OBJECT_A_AK74			);
	add					("ammo_lr300",			CLSID_OBJECT_A_LR300		);
	add					("ammo_hpsa",			CLSID_OBJECT_A_HPSA			);
	add					("ammo_pm",				CLSID_OBJECT_A_PM			);
	add					("ammo_fort",			CLSID_OBJECT_A_FORT			);
	add					("wpn_scope",			CLSID_OBJECT_W_SCOPE		);
	add					("wpn_silencer",		CLSID_OBJECT_W_SILENCER		);
	add					("wpn_grenade_launcher",CLSID_OBJECT_W_GLAUNCHER	);
	add					("quad_damage",			CLSID_OBJECT_M_QDAMAGE		);
	add					("immortal",			CLSID_OBJECT_M_IMMORTAL		);
	add					("invisibility",		CLSID_OBJECT_M_INVIS		);
	add					("obj_health",			CLSID_OBJECT_HEALTH			);
	add					("obj_armor",			CLSID_OBJECT_ARMOR			);
	add					("net_target",			CLSID_TARGET				);
	add					("net_target_assault",	CLSID_TARGET_ASSAULT		);
	add					("net_target_cs_base",	CLSID_TARGET_CS_BASE		);
	add					("net_target_cs",		CLSID_TARGET_CS				);
	add					("net_target_cs_cask",	CLSID_TARGET_CS_CASK		);
	add					("obj_item_std",		CLSID_OBJECT_ITEM_STD		);
	add					("obj_physic",			CLSID_OBJECT_PHYSIC			);
	add					("obj_breakable",		CLSID_OBJECT_BREAKABLE		);
	add					("zone",				CLSID_ZONE					);
	add					("zone_mosquito_bald",	CLSID_Z_MBALD				);
	add					("zone_mincer",			CLSID_Z_MINCER				);
	add					("zone_acid_fog",		CLSID_Z_ACIDF 				);
	add					("zone_galantine",		CLSID_Z_GALANT				);
	add					("zone_radioactive",	CLSID_Z_RADIO				);
	add					("zone_bfuzz",			CLSID_Z_BFUZZ				);
	add					("zone_rusty_hair",		CLSID_Z_RUSTYH				);
	add					("zone_fry_up",			CLSID_Z_FRYUP  				);
	add					("zone_dead",			CLSID_Z_DEAD  				);
	add					("level_changer",		CLSID_LEVEL_CHANGER			);
	add					("script_zone",			CLSID_SCRIPT_ZONE			);
	add					("team_base_zone",		CLSID_Z_TEAM_BASE  			);
	add					("detector_simple",		CLSID_DETECTOR_SIMPLE		);
	add					("detector_visual",		CLSID_DETECTOR_VISUAL		);
	add					("device_pda",			CLSID_DEVICE_PDA			);
	add					("device_torch",		CLSID_DEVICE_TORCH			);
	add					("device_art_merger",	CLSID_DEVICE_AF_MERGER		);
	add					("obj_bolt",			CLSID_IITEM_BOLT			);
	add					("obj_medikit",			CLSID_IITEM_MEDKIT			);
	add					("obj_food",			CLSID_IITEM_FOOD			);
	add					("obj_bottle",			CLSID_IITEM_BOTTLE			);
	add					("obj_antirad",			CLSID_IITEM_ANTIRAD			);
	add					("obj_explosive",		CLSID_IITEM_EXPLOSIVE		);
	add					("obj_document",		CLSID_IITEM_DOCUMENT		);
	add					("obj_attachable",		CLSID_IITEM_ATTACH			);
	add					("wpn_grenade_f1",		CLSID_GRENADE_F1			);
	add					("wpn_grenade_rpg7",	CLSID_OBJECT_G_RPG7			);
	add					("wpn_grenade_rgd5",	CLSID_GRENADE_RGD5			);
	add					("wpn_grenade_fake",	CLSID_OBJECT_G_FAKE			);
	add					("equ_simple",			CLSID_EQUIPMENT_SIMPLE		);
	add					("equ_scientific",		CLSID_EQUIPMENT_SCIENTIFIC	);
	add					("equ_stalker",			CLSID_EQUIPMENT_STALKER		);
	add					("equ_military",		CLSID_EQUIPMENT_MILITARY	);
	add					("equ_exo",				CLSID_EQUIPMENT_EXO			);

	std::sort			(m_classes.begin(),m_classes.end());
}

