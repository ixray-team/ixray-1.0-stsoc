////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_manager_enemy.cpp
//	Created 	: 24.05.2004
//  Modified 	: 04.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Agent manager enemy distributor
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "agent_manager.h"
#include "ai/stalker/ai_stalker.h"
#include "ef_storage.h"
#include "ef_pattern.h"
#include "cover_point.h"
#include "memory_manager.h"
#include "visual_memory_manager.h"
#include "sound_memory_manager.h"
#include "hit_memory_manager.h"
#include "enemy_manager.h"
#include "missile.h"
#include "explosive.h"

const float GRENADE_RADIUS	= 20.f;
const u32 AFTER_GRENADE_DESTROYED_INTERVAL = 20000;

struct CEnemyFiller {
	xr_vector<CAgentManager::CEnemy>	*m_enemies;
	MemorySpace::squad_mask_type		m_mask;
	
	IC			CEnemyFiller		(xr_vector<CAgentManager::CEnemy> *enemies, MemorySpace::squad_mask_type mask)
	{
		m_enemies					= enemies;
		m_mask						= mask;
	}

	template <typename T>
	IC	void	operator()			(const CEntityAlive *enemy, const T &object) const
	{
		xr_vector<CAgentManager::CEnemy>::iterator	I = std::find(m_enemies->begin(),m_enemies->end(),enemy);
		if (I == m_enemies->end()) {
			m_enemies->push_back	(CAgentManager::CEnemy(enemy,m_mask));
			m_enemies->back().m_enemy_position	= object.m_object_params.m_position;
			m_enemies->back().m_level_time			= object.m_last_level_time;
		}
		else {
			(*I).m_mask.set			(m_mask,TRUE);
			if (object.m_last_level_time > (*I).m_level_time) {
				(*I).m_level_time				= object.m_last_level_time;
				(*I).m_enemy_position	= object.m_object_params.m_position;
			}
		}
	}
};

void CAgentManager::register_grenade	(const CExplosive *grenade, const CGameObject *game_object)
{
	{
		xr_vector<CMemberGrenade>::iterator	I = std::find(m_grenades.begin(),m_grenades.end(),grenade);
		if (I != m_grenades.end())
			return;
	}
	{
		xr_vector<u16>::iterator	I = std::find(m_grenades_to_remove.begin(),m_grenades_to_remove.end(),game_object->ID());
		if (I != m_grenades_to_remove.end())
			return;
	}
	m_grenades_to_remove.push_back		(game_object->ID());
//	Msg									("%6d : New grenade registered %s",Device.dwTimeGlobal,*game_object->cName());
	m_grenades.push_back				(CMemberGrenade(grenade,game_object,0,Device.dwTimeGlobal));
	
	u32									interval = AFTER_GRENADE_DESTROYED_INTERVAL;
	const CMissile						*missile = smart_cast<const CMissile*>(grenade);
	if (missile && (missile->destroy_time() > Device.dwTimeGlobal))
		interval						= missile->destroy_time() - Device.dwTimeGlobal + AFTER_GRENADE_DESTROYED_INTERVAL;
	add_danger_location					(game_object->Position(),Device.dwTimeGlobal,interval,GRENADE_RADIUS);
}

IC	CAgentManager::CDangerLocation *CAgentManager::danger_location	(const Fvector &position)
{
	xr_vector<CDangerLocation>::iterator	I = std::find(m_danger_covers.begin(),m_danger_covers.end(),position);
	if (I != m_danger_covers.end())
		return							(&*I);
	return								(0);
}

template <typename T>
IC	void CAgentManager::setup_mask	(xr_vector<T> &objects, CEnemy &enemy)
{
	xr_vector<T>::iterator			I = std::find(objects.begin(),objects.end(),enemy.m_object->ID());
	if (I != objects.end())
		(*I).m_squad_mask.assign	(enemy.m_distribute_mask.get());
}

IC	void CAgentManager::setup_mask	(CEnemy &enemy)
{
	setup_mask						(*m_visible_objects,enemy);
	setup_mask						(*m_sound_objects,enemy);
	setup_mask						(*m_hit_objects,enemy);
}

float CAgentManager::evaluate			(const CEntityAlive *object0, const CEntityAlive *object1) const
{
	ai().ef_storage().non_alife().member() = object0;
	ai().ef_storage().non_alife().enemy()  = object1;
	return								(ai().ef_storage().m_pfVictoryProbability->ffGetValue()/100.f);
}

void CAgentManager::exchange_enemies	(CMemberOrder &member0, CMemberOrder &member1)
{
	u32								enemy0 = member0.selected_enemy();
	u32								enemy1 = member1.selected_enemy();
	MemorySpace::squad_mask_type	mask0 = mask(&member0.object());
	MemorySpace::squad_mask_type	mask1 = mask(&member1.object());
	m_enemies[enemy0].m_distribute_mask.set(mask0,FALSE);
	m_enemies[enemy1].m_distribute_mask.set(mask1,FALSE);
	m_enemies[enemy0].m_distribute_mask.set(mask1,TRUE);
	m_enemies[enemy1].m_distribute_mask.set(mask0,TRUE);
	member0.selected_enemy			(enemy1);
	member1.selected_enemy			(enemy0);
//	Msg								("Member %s changed enemy from %s to %s",*member0.object().cName(),*m_enemies[enemy0].m_object->cName(),*m_enemies[enemy1].m_object->cName());
//	Msg								("Member %s changed enemy from %s to %s",*member1.object().cName(),*m_enemies[enemy1].m_object->cName(),*m_enemies[enemy0].m_object->cName());
}

void CAgentManager::fill_enemies		()
{
	m_enemies.clear					();
	iterator						I = members().begin();
	iterator						E = members().end();
	for ( ; I != E; ++I) {
		(*I).probability			(1.f);
		(*I).object().memory().fill_enemies	(CEnemyFiller(&m_enemies,mask(&(*I).object())));
	}
}

void CAgentManager::compute_enemy_danger()
{
	xr_vector<CEnemy>::iterator		I = m_enemies.begin();
	xr_vector<CEnemy>::iterator		E = m_enemies.end();
	for ( ; I != E; ++I) {
		float						best = -1.f;
		xr_vector<CMemberOrder>::const_iterator	i = members().begin();
		xr_vector<CMemberOrder>::const_iterator	e = members().end();
		for ( ; i != e; ++i) {
			float					value = evaluate((*I).m_object,&(*i).object());
			if (value > best)
				best				= value;
		}
		(*I).m_probability			= best;
	}
	std::sort						(m_enemies.begin(),m_enemies.end());
}

void CAgentManager::assign_enemies		()
{
	for (;;) {
		MemorySpace::squad_mask_type	J, K, N = 0;
		float							best = flt_max;
		
		xr_vector<CEnemy>::iterator		I = m_enemies.begin();
		xr_vector<CEnemy>::iterator		E = m_enemies.end();
		for ( ; I != E; ++I) {
			J							= (*I).m_mask.get();
			N							= 0;
			best						= -1.f;
			for ( ; J; J &= J - 1) {
				K						= (J & (J - 1)) ^ J;
				iterator				i = member(K);
				if (!fsimilar((*i).probability(),1.f))
					continue;
				float					value = evaluate(&(*i).object(),(*I).m_object);
				if (value > best) {
					best				= value;
					N					= K;
				}
			}
			if (N)
				break;
		}
		if (!N)
			break;
		
		(*I).m_distribute_mask.set	(N,TRUE);
		iterator					i = member(N);
		(*i).probability			(best);
		(*I).m_probability			*= 1.f - best; 

		// recovering sort order
		for (u32 i=0, n = m_enemies.size() - 1; i<n; ++i)
			if (m_enemies[i + 1] < m_enemies[i]) {
				CEnemy				temp = m_enemies[i];
				m_enemies[i]		= m_enemies[i + 1];
				m_enemies[i + 1]	= temp;
			}
			else
				break;
	}
}

void CAgentManager::permutate_enemies	()
{
	// filling member enemies
	iterator					I = m_members.begin();
	iterator					E = m_members.end();
	for ( ; I != E; ++I) {
		// clear enemies
		(*I).enemies().clear	();
		// setup procesed flag
		(*I).processed			(false);
		// get member squad mask
		MemorySpace::squad_mask_type		member_mask = mask(&(*I).object());
		// setup if player has enemy
		bool								enemy_selected = false;
		// iterate on enemies
		xr_vector<CEnemy>::const_iterator	i = m_enemies.begin(), b = i;
		xr_vector<CEnemy>::const_iterator	e = m_enemies.end();
		for ( ; i != e; ++i) {
			if ((*i).m_mask.is(member_mask))
				(*I).enemies().push_back	(u32(i - b));
			if ((*i).m_distribute_mask.is(member_mask)) {
				(*I).selected_enemy			(u32(i - b));
				enemy_selected				= true;
			}
		}
		// if there is enemy - all is ok
		if (enemy_selected)
			continue;

		// otherwise temporary make the member processed
		(*I).processed			(true);
	}
	
	// perform permutations
	bool						changed;
	do {
		changed					= false;
		iterator				I = m_members.begin();
		iterator				E = m_members.end();
		for ( ; I != E; ++I) {
			// if member is processed the continue;
			if ((*I).processed())
				continue;

			float				best = (*I).object().Position().distance_to(m_enemies[(*I).selected_enemy()].m_object->Position());
			bool				found = false;
			xr_vector<u32>::const_iterator	i = (*I).enemies().begin();
			xr_vector<u32>::const_iterator	e = (*I).enemies().end();
			for ( ; i != e; ++i) {
				if ((*I).selected_enemy() == *i)
					continue;
				float			my_distance = (*I).object().Position().distance_to(m_enemies[*i].m_object->Position());
				if (my_distance < best) {
					// check if we can exchange enemies
					MemorySpace::squad_mask_type	J = m_enemies[*i].m_distribute_mask.get(), K;
					// iterating on members, whose current enemy is the new one
					for ( ; J; J &= J - 1) {
						K			= (J & (J - 1)) ^ J;
						iterator	j = member(K);
						xr_vector<u32>::iterator	ii = std::find((*j).enemies().begin(),(*j).enemies().end(),(*I).selected_enemy());
						// check if member can my current enemy
						if (ii == (*j).enemies().end())
							continue;

						// check if I'm closer to the enemy
						float		member_distance = (*j).object().Position().distance_to(m_enemies[*i].m_object->Position());
						if (member_distance <= my_distance)
							continue;

						// check if our effectiveness is near the same
						float		my_to_his = evaluate(&(*I).object(),m_enemies[(*j).selected_enemy()].m_object);
						float		his_to_my = evaluate(&(*j).object(),m_enemies[(*I).selected_enemy()].m_object);
						if (!fsimilar(my_to_his,(*j).probability()) || !fsimilar(his_to_my,(*I).probability()))
							continue;

						exchange_enemies	(*I,*j);

						found		= true;
						best		= my_distance;
						break;
					}
				}

				if (found)
					break;
			}

			if (!found) {
				(*I).processed	(true);
				continue;
			}

			changed				= true;
		}
	}
	while						(changed);

	{
		iterator					I = m_members.begin();
		iterator					E = m_members.end();
		for ( ; I != E; ++I) {
			xr_vector<CEnemy>::iterator	i = m_enemies.begin();
			xr_vector<CEnemy>::iterator	e = m_enemies.end();
			for ( ; i != e; ++i)
				if ((*I).object().memory().visual().visible_now((*i).m_object))
					(*i).m_distribute_mask.assign((*i).m_distribute_mask.get() | mask(&(*I).object()));
		}
	}
}

void CAgentManager::assign_enemy_masks	()
{
	xr_vector<CEnemy>::iterator		I = m_enemies.begin();
	xr_vector<CEnemy>::iterator		E = m_enemies.end();
	for ( ; I != E; ++I)
		setup_mask					(*I);
}

void CAgentManager::distribute_enemies	()
{
	fill_enemies					();
	compute_enemy_danger			();
	assign_enemies					();
	permutate_enemies				();
	assign_enemy_masks				();
}

bool CAgentManager::suitable_location	(CAI_Stalker *object, CCoverPoint *location, bool use_enemy_info) const
{
	const_iterator					I = members().begin();
	const_iterator					E = members().end();
	for ( ; I != E; ++I) {
		if ((*I).object().ID() == object->ID())
			continue;

		if (!(*I).cover())
			continue;

		if ((*I).cover()->m_position.distance_to(location->position()) <= 5.f)
			if ((*I).object().Position().distance_to(location->position()) <= object->Position().distance_to(location->position()))
				return				(false);
	}

	if (use_enemy_info) {
		xr_vector<CEnemy>::const_iterator	I = m_enemies.begin();
		xr_vector<CEnemy>::const_iterator	E = m_enemies.end();
		for ( ; I != E; ++I)
			if ((*I).m_enemy_position.distance_to_sqr(location->position()) < 100.f)
				return				(false);
	}
	return							(true);
}

void CAgentManager::distribute_locations()
{
}

void CAgentManager::setup_actions		()
{
}

void CAgentManager::add_danger_location	(const Fvector &position, u32 time, u32 interval, float radius)
{
	CDangerLocation					*danger = danger_location(position);
	if (!danger) {
		CDangerLocation				danger;
		danger.m_position			= position;
		danger.m_level_time			= time;
		danger.m_interval			= interval;
		danger.m_radius				= radius;
		m_danger_covers.push_back	(danger);
		return;
	}
	danger->m_level_time			= time;
	if (danger->m_interval < interval)
		danger->m_interval			= interval;
	if (danger->m_radius < radius)
		danger->m_radius			= radius;
}

void CAgentManager::remove_old_danger_covers	()
{
	xr_vector<CDangerLocation>::iterator	I = remove_if(m_danger_covers.begin(),m_danger_covers.end(),CRemoveOldDangerCover());
	m_danger_covers.erase				(I,m_danger_covers.end());
}

float CAgentManager::cover_danger		(CCoverPoint *cover) const
{
	float			result = 1;
	xr_vector<CDangerLocation>::const_iterator	I = m_danger_covers.begin();
	xr_vector<CDangerLocation>::const_iterator	E = m_danger_covers.end();
	for ( ; I != E; ++I) {
		if (Device.dwTimeGlobal > (*I).m_level_time + (*I).m_interval)
			continue;

		float		distance = 1.f + (*I).m_position.distance_to(cover->position());
		if (distance > (*I).m_radius)
			continue;

		result		*= distance/(*I).m_radius*float(Device.dwTimeGlobal - (*I).m_level_time)/float((*I).m_interval);
	}

	return			(result);
}

bool CAgentManager::process_corpse			(CMemberOrder &member)
{
	float			min_dist_sqr = flt_max;
	CMemberCorpse	*best_corpse = 0;
	xr_vector<CMemberCorpse>::iterator	I = m_corpses.begin();
	xr_vector<CMemberCorpse>::iterator	E = m_corpses.end();
	for ( ; I != E; ++I) {
		if (!member.object().memory().visual().visible_now((*I).m_member))
			continue;

		float		dist_sqr = (*I).m_member->Position().distance_to_sqr(member.object().Position());
		if (dist_sqr < min_dist_sqr) {
			if	(
					(*I).m_reactor && 
					((*I).m_reactor->Position().distance_to_sqr((*I).m_member->Position()) <= min_dist_sqr)
				)
				continue;
			min_dist_sqr	= dist_sqr;
			best_corpse		= &*I;
		}
	}
	
	if (!best_corpse)
		return				(false);

	best_corpse->m_reactor	= &member.object();
	return					(true);
}

struct CRemoveMemberCorpsesPredicate {
	IC	bool operator()		(CAgentManager::CMemberCorpse &corpse) const
	{
		return				(!!corpse.m_reactor);
	}
};

void CAgentManager::react_on_member_death	()
{
	for (;;) {
		bool						changed = false;
		MEMBER_STORAGE::iterator	I = members().begin();
		MEMBER_STORAGE::iterator	E = members().end();
		for ( ; I != E; ++I)
			if (!(*I).member_death_reaction().m_processing)
				changed				= process_corpse(*I);

		if (!changed)
			break;
	}

	{
		xr_vector<CMemberCorpse>::iterator	I = m_corpses.begin();
		xr_vector<CMemberCorpse>::iterator	E = m_corpses.end();
		for ( ; I != E; ++I) {
			if (!(*I).m_reactor)
				continue;

			CMemberOrder::CMemberDeathReaction	&reaction = member((*I).m_reactor).member_death_reaction();
			reaction.m_member		= (*I).m_member;
			reaction.m_time			= (*I).m_time;
			reaction.m_processing	= true;
		}

		I				= remove_if(m_corpses.begin(),m_corpses.end(),CRemoveMemberCorpsesPredicate());
		m_corpses.erase	(I,m_corpses.end());
	}
}

bool CAgentManager::process_grenade			(CMemberOrder &member)
{
	float			min_dist_sqr = flt_max;
	CMemberGrenade	*best_grenade = 0;
	xr_vector<CMemberGrenade>::iterator	I = m_grenades.begin();
	xr_vector<CMemberGrenade>::iterator	E = m_grenades.end();
	for ( ; I != E; ++I) {
		if (!member.object().memory().visual().visible_now((*I).m_game_object))
			continue;

		float		dist_sqr = (*I).m_game_object->Position().distance_to_sqr(member.object().Position());
		if (dist_sqr < min_dist_sqr) {
			if	(
				(*I).m_reactor && 
				((*I).m_reactor->Position().distance_to_sqr((*I).m_game_object->Position()) <= min_dist_sqr)
				)
				continue;
			min_dist_sqr	= dist_sqr;
			best_grenade	= &*I;
		}
	}

	if (!best_grenade)
		return				(false);

	best_grenade->m_reactor	= &member.object();
	return					(true);
}

struct CRemoveGrenadesPredicate {
	IC	bool operator()		(CAgentManager::CMemberGrenade &grenade) const
	{
		return				(!!grenade.m_reactor);
	}
};

void CAgentManager::react_on_grenades	()
{
	for (;;) {
		bool						changed = false;
		MEMBER_STORAGE::iterator	I = members().begin();
		MEMBER_STORAGE::iterator	E = members().end();
		for ( ; I != E; ++I)
			if (!(*I).grenade_reaction().m_processing)
				changed				= process_grenade(*I);

		if (!changed)
			break;
	}

	{
		xr_vector<CMemberGrenade>::iterator	I = m_grenades.begin();
		xr_vector<CMemberGrenade>::iterator	E = m_grenades.end();
		for ( ; I != E; ++I) {
			if (!(*I).m_reactor)
				continue;

			CMemberOrder::CGrenadeReaction	&reaction = member((*I).m_reactor).grenade_reaction();
			reaction.m_grenade		= (*I).m_grenade;
			reaction.m_game_object	= (*I).m_game_object;
			reaction.m_time			= (*I).m_time;
			reaction.m_processing	= true;
		}

		I					= remove_if(m_grenades.begin(),m_grenades.end(),CRemoveGrenadesPredicate());
		m_grenades.erase	(I,m_grenades.end());
	}
}
