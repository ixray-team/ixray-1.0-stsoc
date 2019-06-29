////////////////////////////////////////////////////////////////////////////
//	Module 		: goap_test.cpp
//	Created 	: 24.02.2004
//  Modified 	: 24.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Goal Oriented Action Planning tests
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "problem_solver.h"
#include "condition_evaluator.h"
#include "random32.h"
#include "graph_engine.h"

//#define SEED_TEST
//#define COMPLEXITY_TEST
#define DUMP_TEST

u32 world_state_dimension = 8;
const u32 min_operator_count	= 4;
const u32 max_operator_count	= 8;

typedef u32																				_condition_type;
typedef bool																			_value_type;
typedef u16																				_edge_value_type;
typedef u32																				_operator_id_type;
typedef COperatorConditionAbstract<_condition_type,_value_type>							CCondition;
typedef CConditionState<CCondition>														CState;
typedef COperatorAbstract<CCondition,_edge_value_type>									COperator;
typedef CProblemSolver<CCondition,CState,COperator,IConditionEvaluator<bool>,_operator_id_type>CSProblemSolver;
typedef u8																				_dist_type;
typedef u32																				_iteration_type;
typedef CSProblemSolver::_edge_type														_edge_type;
typedef CSProblemSolver::CState															_index_type;
typedef SBaseParameters<::_dist_type,::_index_type,::_iteration_type>							CBaseParameters;
CRandom32																				random;

const int	seeds[] = {
	#include "seed.dat"
};

CState random_condition(u32 _max = 100, u32 _min = 20, u32 __max = 2, u32 __min = 1)
{
	CState		result;
	for (u32 i=0; i<world_state_dimension; ++i)
		if (random.random(_max) < _min)
			result.add_condition(CCondition(i,random.random(__max) < __min ? 1 : 0));
	return		(result);
}

COperator *random_operator()
{
	return		(xr_new<COperator>(random_condition(),random_condition()));
}

LPSTR show_condition(const xr_vector<CCondition> &condition, CSProblemSolver &problem_solver, LPSTR s)
{
	char					*s1 = s;
	xr_vector<CCondition>::const_iterator	i = condition.begin();
	xr_vector<CCondition>::const_iterator	e = condition.end();
	CSProblemSolver::EVALUATORS::const_iterator	I = problem_solver.evaluators().begin();
	CSProblemSolver::EVALUATORS::const_iterator	E = problem_solver.evaluators().end();
	for ( ; I != E; )
		if ((i == e) || ((*I).first < (*i).condition())) {
			s1				+= sprintf(s1,". ");
			++I;
		}
		else {
			VERIFY	((*I).first == (*i).condition());
			s1				+= sprintf(s1,"%d ",(*i).value());
			++I;
			++i;
		}
	return					(s);
}

void show_condition(const CState &condition, CSProblemSolver &problem_solver, u32 i = 0)
{
	string4096				s;
	Msg						("condition %5d : %s",i,show_condition(condition.conditions(),problem_solver,s));
}

void show_operator(const COperator &_operator, CSProblemSolver &problem_solver, u32 i)
{
	string4096				s;
	char					*s1;
	show_condition			(_operator.conditions().conditions(),problem_solver,s);
	strcat					(s," -> ");
	s1						= s + xr_strlen(s);
	show_condition			(_operator.effects().conditions(),problem_solver,s1);
	Msg						("operator  %5d : %s",i,s);
}

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

void test_goap	()
{
	CGraphEngine						*graph_engine	= xr_new<CGraphEngine>(0);
	xr_vector<_edge_type>				path;
	u32 vertex_count					= 0;
#ifdef SEED_TEST
	u32	max_length						= 0;
	u32 best_test						= 0;
	u32 jj								= 0;
	u32 total_solved					= 0;
	random.seed							(0);
	u64 total							= 0;
	for (u64 ii=0; ii<sizeof(seeds)/sizeof(seeds[0]); ++ii) {
		random.seed						(seeds[ii]);
		CSProblemSolver					problem_solver;
		
		CSProblemSolver::CState			condition = random_condition(100,100,4,1);
		{
			xr_vector<CSProblemSolver::COperatorCondition>::const_iterator	I = condition.conditions().begin();
			xr_vector<CSProblemSolver::COperatorCondition>::const_iterator	E = condition.conditions().end();
			for ( ; I != E; ++I)
				problem_solver.add_evaluator((*I).condition(),xr_new<CConstConditionEvaluator>((*I).value()));
		}

		problem_solver.set_target_state	(random_condition(100,100,4,3));
		u32						operator_count = random.random(max_operator_count - min_operator_count + 1) + min_operator_count;
		for (u32 i=0; i<operator_count; ++i)
			problem_solver.add_operator	(i,random_operator());

		path.clear						();

		u64 start						= CPU::GetCycleCount();
#ifndef STRAIGHT_SEARCH
		graph_engine->search			(problem_solver,problem_solver.target_state(),CState(),&path,CBaseParameters());
#else
		graph_engine->search			(problem_solver,CState(),problem_solver.target_state(),&path,CBaseParameters());
#endif
		u64 finish						= CPU::GetCycleCount();
		total							+= finish - start;
		vertex_count					+= graph_engine->solver_algorithm().data_storage().get_visited_node_count();

		xr_vector<_edge_type>::iterator	I = path.begin(), B = I;
		xr_vector<_edge_type>::iterator	E = path.end();
		
		if (B == E)
			continue;
	
		++total_solved;

		if (path.size() <= max_length)
			continue;

		max_length						= path.size();
		best_test						= jj;
		
		Msg								("Problem %5d (try %I64d, %f sec)",jj,ii,CPU::cycles2seconds*total);
		++jj;
		show_condition					(problem_solver.current_state(),problem_solver);
		show_condition					(problem_solver.target_state(),problem_solver,1);

		{
			CSProblemSolver::const_iterator	I = problem_solver.operators().begin();
			CSProblemSolver::const_iterator	E = problem_solver.operators().end();
			for ( ; I != E; ++I)
				show_operator			(*(*I).m_operator,problem_solver,(*I).m_operator_id);
		}

		Msg								("Solution : ");

		CState							world_state = problem_solver.current_state(), temp;
		show_condition					(world_state,problem_solver);
		for ( ; I != E; ++I) {
			CSProblemSolver::const_iterator	i = std::lower_bound(problem_solver.operators().begin(),problem_solver.operators().end(),*I);
			VERIFY						(i != problem_solver.operators().end());
			VERIFY						((*i).m_operator_id == *I);
			show_operator				(*(*i).m_operator,problem_solver,(*i).m_operator_id);
			VERIFY						((*i).m_operator->applicable_reverse(world_state,problem_solver.current_state(),(*i).m_operator->conditions()));
			world_state					= (*i).m_operator->apply(world_state,(*i).m_operator->effects(),temp);
			show_condition				(world_state,problem_solver);
		}

		VERIFY							(world_state.includes(problem_solver.target_state()));
		Msg								("Max solution length : %d (test %d, vertices %d, solved : %d)",max_length,best_test,vertex_count,total_solved);
		Msg								("");
		FlushLog						();
	}
#endif
#ifdef COMPLEXITY_TEST
	{
		const u32							world_complexity = 128;

		CSProblemSolver						problem_solver;
		CSProblemSolver::CState				condition;
		for (u32 i=0; i<world_complexity; ++i) {
			condition.add_condition			(CCondition(i,true));
			problem_solver.add_evaluator	(i,xr_new<CConstConditionEvaluator>(false));
			COperator						*op = xr_new<COperator>();
			op->add_effect					(CCondition(i,true));
			problem_solver.add_operator		(i,op);
		}

		problem_solver.set_target_state		(condition);

		path.clear							();

#ifndef STRAIGHT_SEARCH
		graph_engine->search				(problem_solver,problem_solver.target_state(),CState(),&path,CBaseParameters());
#else
		graph_engine->search				(problem_solver,CState(),problem_solver.target_state(),&path,CBaseParameters());
#endif
		vertex_count						+= graph_engine->solver_algorithm().data_storage().get_visited_node_count();

		{
			CSProblemSolver::const_iterator	I = problem_solver.operators().begin();
			CSProblemSolver::const_iterator	E = problem_solver.operators().end();
			for ( ; I != E; ++I)
				show_operator				(*(*I).m_operator,problem_solver,(*I).m_operator_id);
		}
		Msg									(" ");
		show_condition						(problem_solver.current_state(),problem_solver);
		show_condition						(problem_solver.target_state(),problem_solver,1);
		Msg									("\nSolution (%d vertices visited) : ",vertex_count);
		CState								world_state = problem_solver.current_state(), temp;
		show_condition						(world_state,problem_solver);
		xr_vector<_edge_type>::iterator		I = path.begin();
		xr_vector<_edge_type>::iterator		E = path.end();
		for ( ; I != E; ++I) {
			CSProblemSolver::const_iterator	i = std::lower_bound(problem_solver.operators().begin(),problem_solver.operators().end(),*I);
			VERIFY							(i != problem_solver.operators().end());
			VERIFY							((*i).m_operator_id == *I);
			show_operator					(*(*i).m_operator,problem_solver,(*i).m_operator_id);
			VERIFY							((*i).m_operator->applicable_reverse(world_state,problem_solver.current_state(),(*i).m_operator->conditions()));
			world_state						= (*i).m_operator->apply(world_state,(*i).m_operator->effects(),temp);
			show_condition					(world_state,problem_solver);
		}

		VERIFY								(world_state.includes(problem_solver.target_state()));
		FlushLog							();
	}
#endif
#ifdef DUMP_TEST
	{
		CSProblemSolver						problem_solver;
		CSProblemSolver::CState				start_condition;
		FILE								*f = fopen("x:\\goap.dat","rt");
		{
			u32								count;
			fscanf							(f,"%d",&count);
			world_state_dimension			= count;
			for (u32 i=0; i<count; ++i) {
				u32							id, value;
				fscanf						(f,"%d,%d",&id,&value);
				start_condition.add_condition(CCondition(id,!!value));
				problem_solver.add_evaluator(id,xr_new<CConstConditionEvaluator>(!!value));
			}
		}
		{
			CSProblemSolver::CState			condition;
			u32								count;
			fscanf							(f,"%d",&count);
			for (u32 i=0; i<count; ++i) {
				u32							id, value;
				fscanf						(f,"%d,%d",&id,&value);
				condition.add_condition		(CCondition(id,!!value));
			}
			problem_solver.set_target_state	(condition);
		}
		{
			u32								count;
			fscanf							(f,"%d",&count);
			for (u32 i=0; i<count; ++i) {
				COperator					*op = xr_new<COperator>();
				u32							count1,weight,id;
				fscanf						(f,"%d,%d",&id,&weight);
				VERIFY						(weight == 1);
	//			op->set_weight				(weight);
				{
					fscanf						(f,"%d",&count1);
					for (u32 j=0; j<count1; ++j) {
						u32						id, value;
						fscanf					(f,"%d,%d",&id,&value);
						op->add_condition		(CCondition(id,!!value));
					}
				}
				{
					fscanf						(f,"%d",&count1);
					for (u32 j=0; j<count1; ++j) {
						u32						id, value;
						fscanf					(f,"%d,%d",&id,&value);
						op->add_effect			(CCondition(id,!!value));
					}
				}
				problem_solver.add_operator	(id,op);
			}
		}
		fclose								(f);

		path.clear							();

	#ifndef STRAIGHT_SEARCH
		graph_engine->search				(problem_solver,problem_solver.target_state(),CState(),&path,::CBaseParameters());
	#else
		graph_engine->search				(problem_solver,CState(),problem_solver.target_state(),&path,::CBaseParameters());
	#endif
		vertex_count						+= graph_engine->solver_algorithm().data_storage().get_visited_node_count();
		
		{
			CSProblemSolver::const_iterator	I = problem_solver.operators().begin();
			CSProblemSolver::const_iterator	E = problem_solver.operators().end();
			for ( ; I != E; ++I)
				show_operator				(*(*I).m_operator,problem_solver,(*I).m_operator_id);
		}
		Msg									(" ");
		show_condition						(problem_solver.current_state(),problem_solver);
		show_condition						(problem_solver.target_state(),problem_solver,1);
		Msg									("\nSolution (%d vertices visited) : ",vertex_count);
		CState								world_state = problem_solver.current_state(), temp;
		show_condition						(world_state,problem_solver);
		xr_vector<_edge_type>::iterator		I = path.begin();
		xr_vector<_edge_type>::iterator		E = path.end();
		for ( ; I != E; ++I) {
			CSProblemSolver::const_iterator	i = std::lower_bound(problem_solver.operators().begin(),problem_solver.operators().end(),*I);
			VERIFY							(i != problem_solver.operators().end());
			VERIFY							((*i).m_operator_id == *I);
			show_operator					(*(*i).m_operator,problem_solver,(*i).m_operator_id);
			VERIFY							((*i).m_operator->applicable_reverse(world_state,problem_solver.current_state(),(*i).m_operator->conditions()));
			world_state						= (*i).m_operator->apply(world_state,(*i).m_operator->effects(),temp);
			show_condition					(world_state,problem_solver);
		}

		VERIFY								(world_state.includes(problem_solver.target_state()));
		FlushLog							();
	}
#endif
	xr_delete							(graph_engine);
}