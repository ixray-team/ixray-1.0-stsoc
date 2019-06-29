////////////////////////////////////////////////////////////////////////////
//	Module 		: detailed_path_manager_dodge.h
//	Created 	: 04.12.2003
//  Modified 	: 04.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Detail path manager dodge path builder
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "detail_path_manager.h"
#include "ai_space.h"

void CDetailPathManager::build_dodge_path		(const xr_vector<u32> &level_path, u32 intermediate_index)
{
	build_straight_path		(level_path,intermediate_index);
}