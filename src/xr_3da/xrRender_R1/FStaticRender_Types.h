#pragma once

struct _FpsController {
	// General params
	float	fMinFPS;			//	Minimal possible FPS'es
	float	fMaxFPS;			//	Maximal possible FPS'es
	float	fGeometryLOD;		//	User prefferred LOD
};
extern	_FpsController		QualityControl;
