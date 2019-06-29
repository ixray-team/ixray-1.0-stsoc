#include "stdafx.h"
#pragma hdrstop

#pragma warning(disable:4995)
#include <d3dx9.h>
#pragma warning(default:4995)

#include "r_shader.h"

BOOL	R_shader::compile		(LPDIRECT3DDEVICE9 pDevice, LPCSTR name)
{
	LPD3DXBUFFER				pShaderBuf	= NULL;
	LPD3DXBUFFER				pErrorBuf	= NULL;
	LPD3DXSHADER_CONSTANTTABLE	pConstants	= NULL;
	HRESULT						hr;

	// pixel
	hr = D3DXCompileShaderFromFile	(name, NULL, NULL, "p_main", "ps_2_0", D3DXSHADER_DEBUG | D3DXSHADER_PACKMATRIX_ROWMAJOR, &pShaderBuf, &pErrorBuf, NULL);
	if (SUCCEEDED(hr))
	{
		if (pShaderBuf)
		{
			hr = pDevice->CreatePixelShader	((DWORD*)pShaderBuf->GetBufferPointer(), &ps);
			if (SUCCEEDED(hr))	{
				LPCVOID			data		= NULL;
				hr	= D3DXFindShaderComment	((DWORD*)pShaderBuf->GetBufferPointer(),MAKEFOURCC('C','T','A','B'),&data,NULL);
				if (SUCCEEDED(hr) && data)
				{
					pConstants		= LPD3DXSHADER_CONSTANTTABLE(data);
					constants.parse	(pConstants,0x1);
				} else	hr = E_FAIL;
			}
		}
		else	hr = E_FAIL;
	}
	_RELEASE		(pShaderBuf);
	_RELEASE		(pErrorBuf);
	pConstants		= NULL;
	if (FAILED(hr))	return FALSE;

	// vertex
	hr = D3DXCompileShaderFromFile	(name, NULL, NULL, "v_main", "vs_2_0", D3DXSHADER_DEBUG | D3DXSHADER_PACKMATRIX_ROWMAJOR, &pShaderBuf, &pErrorBuf, NULL);
	if (SUCCEEDED(hr))
	{
		if (pShaderBuf)
		{
			hr = pDevice->CreateVertexShader	((DWORD*)pShaderBuf->GetBufferPointer(), &vs);
			if (SUCCEEDED(hr))	{
				LPCVOID			data		= NULL;
				hr	= D3DXFindShaderComment	((DWORD*)pShaderBuf->GetBufferPointer(),MAKEFOURCC('C','T','A','B'),&data,NULL);
				if (SUCCEEDED(hr) && data)
				{
					pConstants		= LPD3DXSHADER_CONSTANTTABLE(data);
					constants.parse	(pConstants,0x2);
				} else	hr = E_FAIL;
			}
		}
		else	hr = E_FAIL;
	}
	_RELEASE	(pShaderBuf);
	_RELEASE	(pErrorBuf);
	pConstants		= NULL;
	if (FAILED(hr))	return FALSE;

	return			TRUE;
}
