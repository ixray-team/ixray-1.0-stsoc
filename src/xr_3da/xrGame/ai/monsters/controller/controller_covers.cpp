#include "stdafx.h"
#include "controller_covers.h"
#include "../../../cover_point.h"
#include "../../../ai_space.h"
#include "../../../level_navigation_graph.h"
#include "../../../game_graph.h"
#include "../../../game_level_cross_table.h"
#include "../../../level.h"
#include "../../../level_debug.h"

//////////////////////////////////////////////////////////////////////////
// CControllerCoverEvaluator
//////////////////////////////////////////////////////////////////////////

CControllerCoverEvaluator::CControllerCoverEvaluator(CRestrictedObject *object) : inherited(object)
{
	m_enemy_position.set	(flt_max,flt_max,flt_max);
	m_deviation				= flt_max;
	m_min_distance			= flt_max;
	m_max_distance			= flt_max;
	m_best_distance			= flt_max;
	m_current_distance		= flt_max;
}

void CControllerCoverEvaluator::setup(const Fvector &enemy_position, float min_enemy_distance, float max_enemy_distance, float deviation)
{
	inherited::setup();

	m_enemy_position		= enemy_position;

	m_actuality				= m_actuality && fsimilar(m_deviation,deviation);
	m_deviation				= deviation;

	m_actuality				= m_actuality && fsimilar(m_min_distance,min_enemy_distance);
	m_min_distance			= min_enemy_distance;

	m_actuality				= m_actuality && fsimilar(m_max_distance,max_enemy_distance);
	m_max_distance			= max_enemy_distance;

}

void CControllerCoverEvaluator::initialize(const Fvector &start_position)
{
	inherited::initialize	(start_position);
	m_current_distance		= m_start_position.distance_to(m_enemy_position);
#ifdef DEBUG
	DBG().level_info(this).clear();
#endif
}


void CControllerCoverEvaluator::evaluate(CCoverPoint *cover_point, float weight)
{
#ifdef DEBUG
	//DBG().level_info(this).add_item(cover_point->position(), D3DCOLOR_XRGB(0,255,0));
#endif

	if (fis_zero(weight))
		return;

	float					enemy_distance	= m_enemy_position.distance_to(cover_point->position());

	if ((enemy_distance <= m_min_distance) && (m_current_distance > enemy_distance))
		return;

	if((enemy_distance >= m_max_distance) && (m_current_distance < enemy_distance))
		return;

	Fvector					direction;
	float					y,p;
	direction.sub			(m_enemy_position,cover_point->position());
	direction.getHP			(y,p);

	float					cover_value = ai().level_graph().cover_in_direction(y,cover_point->level_vertex_id());
	float					value = cover_value;
	if (ai().level_graph().neighbour_in_direction(direction,cover_point->level_vertex_id()))
		value				+= 10.f;

	value					/= weight;

	if (value >= m_best_value)
		return;

	m_selected				= cover_point;
	m_best_value			= value;
}

//////////////////////////////////////////////////////////////////////////
// CControllerCoverPredicate
//////////////////////////////////////////////////////////////////////////


CControllerCoverPredicate::CControllerCoverPredicate	()
{
}

bool CControllerCoverPredicate::operator()				(CCoverPoint *cover) const
{
	return				(true);
}

float CControllerCoverPredicate::weight					(CCoverPoint *cover) const
{
	return				(1.f);
}

void CControllerCoverPredicate::finalize				(CCoverPoint *cover) const
{
}


