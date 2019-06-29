#pragma once

#include "../../../cover_evaluators.h"

class CCoverPoint;

//////////////////////////////////////////////////////////////////////////
// CControllerCoverEvaluator
class CControllerCoverEvaluator : public CCoverEvaluatorBase {
	typedef CCoverEvaluatorBase inherited;

	Fvector				m_enemy_position;
	float				m_min_distance;
	float				m_max_distance;
	float				m_current_distance;
	float				m_deviation;
	float				m_best_distance;

public:
				CControllerCoverEvaluator	(CRestrictedObject *object);
	
	// setup by cover_manager
	void		initialize					(const Fvector &start_position);

	// manual setup
	void		setup						(const Fvector &enemy_position, float min_enemy_distance, float	max_enemy_distance, float deviation = 0.f);

	void		evaluate					(CCoverPoint *cover_point, float weight);
};

//////////////////////////////////////////////////////////////////////////
// CControllerCoverPredicate
class CControllerCoverPredicate {
public:
	// setup internals here
				CControllerCoverPredicate	();
	// called from cover_manager for every cover (for suitable cover)
	bool		operator()					(CCoverPoint *cover) const;
	// must return a value that is transfered to cover evaluator
	float		weight						(CCoverPoint *cover) const;
	
	void		finalize					(CCoverPoint *cover) const;
};
