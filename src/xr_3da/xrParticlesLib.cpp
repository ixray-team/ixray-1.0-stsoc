#include "stdafx.h"
#pragma hdrstop

#include "xrParticlesLib.h"
#include "FS.h"

#define PS_LIB_SIGN "PS_LIB"

IC bool sort_pred(const PS::SDef& A, const PS::SDef& B)
{	return strcmp(A.m_Name,B.m_Name)<0; }

void psLibrary_Sort(PS::PSVec &LIB)
{
	std::sort(LIB.begin(),LIB.end(),sort_pred);
}

BOOL psLibrary_Load(const char *Name, PS::PSVec &LIB)
{
	// Check if file is compressed already
	string32	ID			= PS_LIB_SIGN;
	string32	id;
	IReader*	F			= FS.r_open(Name);
	F->r		(&id,8);
	if (0==strncmp(id,ID,8))	
	{
		Debug.fatal			("Unsupported PS library. Compressed?");
	}else{
    	F->seek	(0);
    }
	IReader&				fs	= *F;
    	
	// Load
	LIB.clear			();
    WORD version		= fs.r_u16();
    if (version!=PARTICLESYSTEM_VERSION) return false;
    u32 count = fs.r_u32();
    if (count){
        LIB.resize		(count);
        fs.r			(&*LIB.begin(), count*sizeof(PS::SDef));
    }
    psLibrary_Sort		(LIB);

	FS.r_close		(F);
    return true;
}

void psLibrary_Save(const char *Name, PS::PSVec &LIB)
{
	CMemoryWriter F;
    F.w_u16		(PARTICLESYSTEM_VERSION);
    F.w_u32		(LIB.size());
	F.w			(&*LIB.begin(), LIB.size()*sizeof(PS::SDef));
    F.save_to	(Name);
}

PS::SDef* psLibrary_FindSorted(const char* Name, PS::PSVec &LIB)
{
	PS::SDef	D; 
	strcpy		(D.m_Name,Name);
	xr_vector<PS::SDef>::iterator	P = std::lower_bound	(LIB.begin(),LIB.end(),D,sort_pred);
	if ((P!=LIB.end()) && (0==strcmp(P->m_Name,Name)) ) return &*P;
	return NULL;
}

PS::SDef* psLibrary_FindUnsorted(const char* Name, PS::PSVec &LIB)
{
	for (PS::PSIt it=LIB.begin(); it!=LIB.end(); it++)
    	if (0==strcmp(it->m_Name,Name)) return &*it;
	return NULL;
}
