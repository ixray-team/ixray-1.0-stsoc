#pragma once
#define GAMESPY_GAMENAME			"stalkersc"
#define GAMESPY_PRODUCTID			0

#define	GAMESPY_BASEPORT			5447
#define START_PORT					5445
#define END_PORT					65535//(START_PORT + 200) //GameSpy only process 500 ports

#define	GAMESPY_MAXCHALLANGESIZE	32

#define	DEDICATED_KEY			100
#define	FFIRE_KEY				101

#define MAX_UPDATES		10

#include "GameSpy/QR2/qr2regkeys.h"
static unsigned char Fields_Of_Interest[] = { HOSTNAME_KEY, NUMPLAYERS_KEY, MAXPLAYERS_KEY, MAPNAME_KEY, GAMETYPE_KEY, PASSWORD_KEY };
