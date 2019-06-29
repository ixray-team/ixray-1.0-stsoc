// TextureManager.h: interface for the CTextureManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTUREMANAGER_H__0E25CF4B_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_TEXTUREMANAGER_H__0E25CF4B_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_
#pragma once

#include "shader.h"
#include "tss_def.h"

class ENGINE_API CShaderManager
{
private:
	struct str_pred : public std::binary_function<char*, char*, bool>	{
		IC bool operator()(LPCSTR x, LPCSTR y) const
		{	return strcmp(x,y)<0;	}
	};
	struct texture_detail
	{
		LPCSTR T;
		LPCSTR M;
	};
public:
	DEFINE_MAP_PRED(LPSTR,IBlender*,	map_Blender,	map_BlenderIt,		str_pred);
	DEFINE_MAP_PRED(LPSTR,CTexture*,	map_Texture,	map_TextureIt,		str_pred);
	DEFINE_MAP_PRED(LPSTR,CMatrix*,		map_Matrix,		map_MatrixIt,		str_pred);
	DEFINE_MAP_PRED(LPSTR,CConstant*,	map_Constant,	map_ConstantIt,		str_pred);
	DEFINE_MAP_PRED(LPSTR,CRT*,			map_RT,			map_RTIt,			str_pred);
	DEFINE_MAP_PRED(LPSTR,CRTC*,		map_RTC,		map_RTCIt,			str_pred);
	DEFINE_MAP_PRED(LPSTR,SVS*,			map_VS,			map_VSIt,			str_pred);
	DEFINE_MAP_PRED(LPSTR,SPS*,			map_PS,			map_PSIt,			str_pred);
	DEFINE_MAP_PRED(LPSTR,texture_detail,map_TD,		map_TDIt,			str_pred);
private:
	// data
	map_Blender						m_blenders;
	map_Texture						m_textures;
	map_Matrix						m_matrices;
	map_Constant					m_constants;
	map_RT							m_rtargets;
	map_RTC							m_rtargets_c;
	map_VS							m_vs;
	map_PS							m_ps;
	map_TD							m_td;

	xr_vector<SState*>				v_states;
	xr_vector<SDeclaration*>		v_declarations;
	xr_vector<SGeometry*>			v_geoms;
	xr_vector<R_constant_table*>	v_constant_tables;

	// lists
	xr_vector<STextureList*>		lst_textures;
	xr_vector<SMatrixList*>			lst_matrices;
	xr_vector<SConstantList*>		lst_constants;

	// main shader-array
	xr_vector<SPass*>				v_passes;
	xr_vector<ShaderElement*>		v_elements;
	xr_vector<Shader*>				v_shaders;

	// misc
	BOOL							bDeferredLoad;
public:
	// Miscelaneous
	void							_ParseList			(sh_list& dest, LPCSTR names);
	IBlender*						_GetBlender			(LPCSTR Name);
	IBlender* 						_FindBlender		(LPCSTR Name);
	void							_GetMemoryUsage		(u32& m_base, u32& c_base, u32& m_lmaps, u32& c_lmaps);
	BOOL							_GetDetailTexture	(LPCSTR Name, LPCSTR& T, LPCSTR& M);

    map_Blender&					_GetBlenders		()		{	return m_blenders;	}

	// Debug
	void							DBG_VerifyGeoms		();
	void							DBG_VerifyTextures	();

	// Editor cooperation
	void							ED_UpdateBlender	(LPCSTR Name, IBlender*		data);
	void							ED_UpdateMatrix		(LPCSTR Name, CMatrix*		data);
	void							ED_UpdateConstant	(LPCSTR Name, CConstant*	data);
#ifdef _EDITOR
	void							ED_UpdateTextures	(AStringVec* names);
#endif

	// Low level resource creation
	CTexture*						_CreateTexture		(LPCSTR Name);
	void							_DeleteTexture		(const CTexture* T);

	CMatrix*						_CreateMatrix		(LPCSTR Name);
	void							_DeleteMatrix		(const CMatrix*  M);

	CConstant*						_CreateConstant		(LPCSTR Name);
	void							_DeleteConstant		(const CConstant* C);

	R_constant_table*				_CreateConstantTable(R_constant_table& C);
	void							_DeleteConstantTable(const R_constant_table* C);

	CRT*							_CreateRT			(LPCSTR Name, u32 w, u32 h,	D3DFORMAT f);
	void							_DeleteRT			(const CRT*	RT	);

	CRTC*							_CreateRTC			(LPCSTR Name, u32 size,	D3DFORMAT f);
	void							_DeleteRTC			(const CRTC*	RT	);

	SPS*							_CreatePS			(LPCSTR Name);
	void							_DeletePS			(const SPS*	PS	);

	SVS*							_CreateVS			(LPCSTR Name);
	void							_DeleteVS			(const SVS*	VS	);

	SPass*							_CreatePass			(ref_state& _state, ref_ps& _ps, ref_vs& _vs, ref_ctable& _ctable, ref_texture_list& _T, ref_matrix_list& _M, ref_constant_list& _C);
	void							_DeletePass			(const SPass* P	);

	// Shader compiling / optimizing
	SState*							_CreateState		(SimulatorStates& Code);
	void							_DeleteState		(const SState* SB);

	SDeclaration*					_CreateDecl			(D3DVERTEXELEMENT9* dcl);
	void							_DeleteDecl			(const SDeclaration* dcl);

	STextureList*					_CreateTextureList	(STextureList& L);
	void							_DeleteTextureList	(const STextureList* L);

	SMatrixList*					_CreateMatrixList	(SMatrixList& L);
	void							_DeleteMatrixList	(const SMatrixList* L);

	SConstantList*					_CreateConstantList	(SConstantList& L);
	void							_DeleteConstantList	(const SConstantList* L);

	ShaderElement*					_CreateElement		(CBlender_Compile& C);
	void							_DeleteElement		(const ShaderElement* L);

	CShaderManager			()
	{
		bDeferredLoad		= FALSE;
	}

	void			xrStartUp			();
	void			xrShutDown			();

	void			OnDeviceCreate		(IReader* F);
	void			OnDeviceCreate		(LPCSTR name);
	void			OnDeviceDestroy		(BOOL   bKeepTextures);

	// Creation/Destroying
	Shader*			Create				(LPCSTR s_shader=0, LPCSTR s_textures=0, LPCSTR s_constants=0, LPCSTR s_matrices=0);
	Shader*			Create_B			(IBlender*	B,	LPCSTR s_shader=0, LPCSTR s_textures=0, LPCSTR s_constants=0, LPCSTR s_matrices=0);
	void			Delete				(const Shader*	S			);

	SGeometry*		CreateGeom			(D3DVERTEXELEMENT9* decl, IDirect3DVertexBuffer9* vb, IDirect3DIndexBuffer9* ib);
	SGeometry*		CreateGeom			(u32 FVF				, IDirect3DVertexBuffer9* vb, IDirect3DIndexBuffer9* ib);
	void			DeleteGeom			(const SGeometry* VS		);
	void			DeferredLoad		(BOOL E)					{ bDeferredLoad=E;	}
	void			DeferredUpload		();
	void			DeferredUnload		();
	void			Evict				();
};

#endif // !defined(AFX_TEXTUREMANAGER_H__0E25CF4B_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_)
