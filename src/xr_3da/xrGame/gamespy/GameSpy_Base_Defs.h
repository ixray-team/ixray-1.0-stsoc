#pragma once

#include "GameSpy_Keys.h"

#define GAMESPY_GAMENAME			"stalkersc"
#define GAMESPY_MAX_UPDATES			10

#define GAMESPY_PATCHING_PRODUCT_ID			0
#define GAMESPY_PATCHING_VERSIONUNIQUE_ID	"0.1"
#define GAMESPY_PATCHING_DISTRIBUTION_ID	0

#define	GAMESPY_BASEPORT			5447
#define START_PORT					5445
#define END_PORT					65535//(START_PORT + 200) //GameSpy only process 500 ports

static unsigned char Fields_Of_Interest[] = 
{ 
	HOSTNAME_KEY, 
		NUMPLAYERS_KEY, 
		MAXPLAYERS_KEY, 
		MAPNAME_KEY, 
		GAMETYPE_KEY, 
		PASSWORD_KEY
};











