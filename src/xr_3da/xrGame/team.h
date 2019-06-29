////////////////////////////////////////////////////////////////////////////
//	Module 		: team.h
//	Created 	: 24.05.2004
//  Modified 	: 24.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Team class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "squad.h"

const int maxSquads				= 32;

class CTeam {
public:
	svector<CSquad,maxSquads>	Squads;
};
