////////////////////////////////////////////////////////////////////////////
//	Module 		: goap_test2.cpp
//	Created 	: 17.02.2006
//  Modified 	: 17.02.2006
//	Author		: Dmitriy Iassenev
//	Description : Goal Oriented Action Planning tests second part
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "problem_solver.h"
#include "condition_evaluator.h"
#include "graph_engine.h"

extern u32 world_state_dimension;
extern const u32 min_operator_count;
extern const u32 max_operator_count;

typedef u32																						_condition_type;
typedef bool																					_value_type;
typedef u16																						_edge_value_type;
typedef u32																						_operator_id_type;
typedef COperatorConditionAbstract<_condition_type,_value_type>									CCondition;
typedef CConditionState<CCondition>																CState;
typedef COperatorAbstract<CCondition,_edge_value_type>											COperator;
typedef CProblemSolver<CCondition,CState,COperator,IConditionEvaluator<bool>,_operator_id_type>	CSProblemSolver;
typedef u8																						_dist_type;
typedef u32																						_iteration_type;
typedef CSProblemSolver::_edge_type																_edge_type;
typedef CSProblemSolver::CState																	_index_type;
//typedef SBaseParameters<::_dist_type,::_index_type,::_iteration_type>							CBaseParameters;

struct CConstConditionEvaluator : public IConditionEvaluator<bool> {
protected:
	bool					m_value;

public:
							CConstConditionEvaluator(bool value) :
								m_value(value)
	{
	}

	virtual bool			evaluate				() const {
		return				(m_value);
	}

	virtual void			Load					(LPCSTR section)
	{
	}

	virtual void			reload					(LPCSTR section)
	{
	}

	virtual void			reinit					()
	{
	}
};

void add_evaluators	(CSProblemSolver &solver)
{
}

void add_operators	(CSProblemSolver &solver)
{
//	COperator					*action = 0;
//	action						= xr_new<COperator>();
//	action->add_condition		(CCondition());
}

void test_goap2		()
{
	CGraphEngine						*graph_engine = xr_new<CGraphEngine>(0);
	xr_vector<_edge_type>				path;

	CSProblemSolver						problem_solver;

	add_evaluators						(problem_solver);
	add_operators						(problem_solver);

	path.clear							();

#ifndef STRAIGHT_SEARCH
	graph_engine->search				(problem_solver,problem_solver.target_state(),CState(),&path,CBaseParameters());
#else
	graph_engine->search				(problem_solver,CState(),problem_solver.target_state(),&path,CBaseParameters());
#endif

	xr_delete							(graph_engine);
}
