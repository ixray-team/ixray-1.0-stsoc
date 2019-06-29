////////////////////////////////////////////////////////////////////////////
//	Module 		: game_location_selector.cpp
//	Created 	: 02.10.2001
//  Modified 	: 18.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Game location selector
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "game_location_selector.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _VertexEvaluator,\
	typename _vertex_id_type\
>

#define CGameLocationSelector CBaseLocationSelector<CGameGraph,_VertexEvaluator,_vertex_id_type>

#undef TEMPLATE_SPECIALIZATION
#undef CGameLocationSelector
