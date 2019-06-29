#ifndef r_shaderH
#define r_shaderH
#pragma once

#include "r_constants.h"

class	R_shader
{
public:
	IDirect3DPixelShader9*		ps;
	IDirect3DVertexShader9*		vs;
	R_constant_table			constants;
public:
	BOOL						compile		(LPDIRECT3DDEVICE9 D, LPCSTR name);
};
#endif
