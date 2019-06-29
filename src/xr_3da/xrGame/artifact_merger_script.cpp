////////////////////////////////////////////////////////////////////////////
//	Module 		: artifact_merger_script.cpp
//	Created 	: 28.06.2004
//  Modified 	: 28.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Artefact merger script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "artifactmerger.h"
#include "script_space.h"

using namespace luabind;

#pragma optimize("s",on)
void CArtefactMerger::script_register(lua_State *L)
{
	module(L)
	[
		class_<CArtefactMerger>("artifact_merger")
			.def("get_mercury_ball_num",	&CArtefactMerger::GetMercuryBallNum)
			.def("get_gravi_num",			&CArtefactMerger::GetGraviArtefactNum)
			.def("get_black_drops_num",		&CArtefactMerger::GetBlackDropsNum)
			.def("get_needles_num",			&CArtefactMerger::GetNeedlesNum)

			.def("spawn_mercury_ball",		&CArtefactMerger::SpawnMercuryBall)
			.def("spawn_gravi",				&CArtefactMerger::SpawnGraviArtefact)
			.def("spawn_black_drops",		&CArtefactMerger::SpawnBlackDrops)
			.def("spawn_needles",			&CArtefactMerger::SpawnNeedles)
	];
}
