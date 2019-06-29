// Script.cpp: implementation of the CScript class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Script.h"

extern "C" {
extern void SAPI scInit_SeeR(void);
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScript::CScript(scScript script)
{
	instance=scCreate_Instance(script,"");
	if (0==instance) {
		Msg("    %s\n> %s\n",scErrorMsg,scErrorLine);
	}
	R_ASSERT(instance);

	cStartupName[0]=0;
	iStartupPoint = 0;
}

CScript::~CScript()
{
	//let's delete instance and script
	scFree_Instance	(instance);
}

int __cdecl CScript::Call(char *name, ...)
{
	if (strcmp(name,cStartupName)!=0) {
		strcpy(cStartupName,name);
		iStartupPoint = scGet_Symbol(instance,name);
	}

	R_ASSERT(iStartupPoint>=0);
	int*	p	= (int *)((char *)LPVOID(&name) + 4);
	int		res = scVCall_Instance(instance,iStartupPoint,-1,p);
	FPU::m24r();
	return res;
}

int __cdecl CScript::FastCall(int fake, ...)
{
	int*	p=(int *)((char *)LPVOID(&fake) + 4);
	int		res=scVCall_Instance(instance,iStartupPointFastCall,-1,p);
	FPU::m24r();
	return res;
}

void CScript::SetupFastCall(char *name) {
	iStartupPointFastCall = scGet_Symbol(instance,name);
}

void CScript::RegisterSymbol(char *name, void *p)
{
	scAdd_External_Symbol(name,p);
}


//if paramc<0 , paramc autodetect
int CScript::VCall(scInstance* inst,int address,int paramc,int *params)
{
	/*
	int i;

	if (!inst) rerr("Invalid instance!");
	if (Register(inst)[regCP]!=0) rerr( "Instance already running!");
	if (address==-1)
		rerr2( "Function not found in `%s'!",scGet_Instance_Title(inst));
	// if (address==-1) return -1;
	scErrorNo=0;
	deb1("Calling %d\n",address);
	//Prepare register and stack:
	Register(inst)[regSP]= ToCodeINT(inst->code)[12];//SP -offset of SS - if reaches 0 - stack overflow
	ReadINT(inst->code+address,Register(inst)[regIP]);//Read Address
	ReadINT(inst->code+address+4,i);//Size Of Params;
	if (i<0&&paramc<0) return -1;//variable!
	if (paramc<0) paramc=i;
	deb1("Size of params:%d\n",paramc);
	if (paramc)
	{//push all params form va to the stack;
		Register(inst)[regSP]-=paramc+4;
		memcpy((void*)(Register(inst)[regSS]+Register(inst)[regSP]+4),
			(params),paramc);
	}
	Register(inst)[regCP]=Register(inst)[regSP];
	Executor(inst,-1);
	// debprintf("Result:%d\n",Register(inst)[0]);
	Register(inst)[regCP]=0;//xr_free
	return Register(inst)[0];
scError_Exit:
	*/
	return 0;
}

