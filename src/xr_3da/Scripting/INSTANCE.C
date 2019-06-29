/************************************************************************
-= SEER - C-Scripting engine v 0.3a =-
State      : under implementation
Author     : Przemyslaw Podsiadly
email: ppodsiad@elka.pw.edu.pl
WWW:   http://home.elka.pw.edu.pl/~ppodsiad
File:instance.c
Desc:creates and operates on instance of a script
************************************************************************/
#define INTERNAL_FILE
#include <seer.h>
#include "internal.h"
#include "code.h"

#define ToINT(ch) ((int*)(ch))
#define ReadINT(ptr,x) memcpy(&(x),ptr,4)
#define WriteINT(ptr,x) memcpy(ptr,&(x),4)
#define ToCodeINT(script) ((int*)script)
#define Register(inst) ((int*)inst->data_stack)

//variables:
scDictionary*	scImportSymbols=NULL;
scDictionary*	scInternalHeaders=NULL;
scScheduler *	scheduler=NULL;

static int scsXCall_Instance(scInstance* inst,int ip,int paramc,int *params);

scScheduler*  SAPI scGet_Scheduler()
{
	return scheduler;
}

scScheduler*  SAPI scSet_Scheduler(scScheduler* s)//returns previous scheduler
{
	scScheduler *t=scheduler;
	if (!s) {s=New(scScheduler);s->first=NULL;}
	scheduler=s;
	return t;
}

scInstance*  SAPI scCreate_Instance(scScript scrpt,char* vars,...)
//e.g in=scCreate_Instance(scr_ai,"x y",&r.x,&r.y);//initialize imported vars
{
	scInstance* inst=NULL;
	scInit_SeeR();
	//<ERASED> scKernelOnly(NULL);
	
	if (!scrpt||ToCodeINT(scrpt)[0]!=CHARS2INT('S','e','e','R'))
		return NULL;//not a script
	if (ToCodeINT(scrpt)[1]!=CHARS2INT('V','C','P','U'))
		return NULL;//not a VCPU machine
	
	inst=New(scInstance);
	// debprintf("Making instance:\n");
	inst->code=scrpt;
	//set typical flags, you can change on demand
	inst->flags.priority=scrights_USER;
	inst->flags.forkno=0;
	inst->flags.paused=0;
	inst->flags.speed=-1;
	inst->forks=inst->forked=inst->next_fork=NULL;
	
	//below:registers+DATA+STACK----------------
	inst->data_stack=xr_malloc(256*4+ToCodeINT(scrpt)[hdrDataSize]+ToCodeINT(scrpt)[hdrStackSize]);
	//actualize REGISTERS:----------------------
	Register(inst)[regCS]=ToCodeINT(scrpt)[hdrCodeOffset];//CS -offset only
	Register(inst)[regDS]=ToCodeINT(scrpt)[hdrDataOffset]+(int)inst->data_stack+256*4;//DS -real pointer
	Register(inst)[regES]=ToCodeINT(scrpt)[hdrConstsOffset]+(int)scrpt;//ES -real
	Register(inst)[regSS]=Register(inst)[regDS]+ToCodeINT(scrpt)[hdrDataSize];//SS -real,probably+ToCodeINT(scrpt)[10]
	Register(inst)[regSP]=ToCodeINT(scrpt)[hdrStackSize];//SP -offset of SS - if reaches 0 - stack overflow
	Register(inst)[regCP]=0;
	Register(inst)[regIE]=0;
	Register(inst)[regOPTIONS]=0;
	
	//now actualize IMPORT segment:----------------
	// debprintf("\nImporting data ...\n");
	{
		char *x;
		scSymbol *sym;
		char namer[1000];
		int addr,cfg;
		x=(char *)scrpt+ToCodeINT(scrpt)[hdrImportedOffset];//import offs
		do{
			strcpy(namer,x);
			x+=strlen(namer)+1;
			ALIGN_INT(x);
			deb2("Importing %s at %d\n",namer,(int)x);
			if (namer[0]) {
				if (!(sym=scdicFoundSymbol(scImportSymbols->first,namer)))
					rerr2( "Unresolved reference to `%s'",namer);
				addr=sym->adr.address;
				WriteINT(x,addr);x+=4;
				ReadINT(x,cfg);x+=4;
				deb2("with addr:%d and params:%d\n",addr,cfg);
			}
		}while(namer[0]);
	}
	
	//now actualize DATA segment:----------------
	// debprintf("Initializing data segment...\n");
	deb0("\n");
	{
		char *temp;
		int address,size;
		temp=(char *)scrpt+ToCodeINT(scrpt)[hdrInitOffset];//init offs
		do
		{
			//address=*ToINT(temp);
			deb1("%d:",(int)temp-((int)scrpt+ToCodeINT(scrpt)[hdrInitOffset]));
			ReadINT(temp,address);
			temp+=4;
			ReadINT(temp,size);
			temp+=4;
			deb2("Address %d, size %d,",address,size);
			if (size)
			{
				char *to=(char *)(Register(inst)[regDS]+address);
				if (size<0)//special, e.g. pointer
				{int x;
				void *from=&x;//copy from x by default
				ReadINT(temp,x);
				temp+=4;
				switch(size)
				{
				case -1://add DS
					x+=Register(inst)[regDS];
					size=4;
					break;
				case -2://add ES
					x+=Register(inst)[regES];
					deb1("S=%s;\n",(char*)x);
					size=4;
					break;
				case -3://copy [DS+from]
					from=(void*)(x+Register(inst)[regDS]);
					ReadINT(temp,size);
					deb2("size %d from [DS+%d],",size,x);
					temp+=4;
					break;
				case -4://copy [from]
					//            from=(void*)(x+Register(inst)[regDS]);
					ReadINT(scrpt+ToCodeINT(scrpt)[hdrImportedOffset]+(int)from,from);
					ReadINT(temp,size);
					deb2("size %d from [%d],",size,x);
					temp+=4;
					break;
					
				default:scError(scCreate_Instance);
				}
				memcpy(to,from,size);
				}
				else
				{
					memcpy(to,temp,size);
					temp+=size;
				}
				
				if (size!=4)
				{deb1(" value \"%s\"\n",temp);
				}
				else
				{int x;
				ReadINT(to,x);
				deb1(" value %d\n",x);
				}
			}
		} while (size);//till both zeros
	}
	
	//now call Script Constructor...
	scsXCall_Instance(inst,ToCodeINT(scrpt)[hdrConstructOffset],0,NULL);
	
	return inst;
scError_Exit:
	return NULL;
}

void SAPI scFree_Instance(scInstance *sci)
{ 
	scInit_SeeR();
	//<ERASED>  scKernelOnly(;);
	if (!sci) return;
	if (sci->forks)
	{//TODO:make one of the childrens the parent.
		//we'll remove i, but copy all necessery regs to sci.
		scInstance* i;
		deb0("Reorganizing forked instances\n");
		i=sci->forks;
		//copy all registers:
		memcpy(sci->data_stack,i->data_stack,240*4);//all not system regs.
		Register(sci)[regSP]=Register(i)[regSP];//stack
		xr_free(i->data_stack);
		xr_free(i);
	}
	else
		if (sci->forked)
		{//TODO:remove from the fork queue of parent
			scInstance *i=sci->forked->forks;
			deb0("Removing forked instance\n");
			if (i==sci)//it's first one
			{
				sci->forked->forks=i->next_fork;
			}
			else
			{
				while (i->next_fork!=sci) i=i->next_fork;
				i->next_fork=sci->next_fork;
			}
			xr_free(sci->data_stack);
			xr_free(sci);
		}
		else
		{//neither forked nor has forked, so delete completely
			//now call Script destructor...
			sci->flags.paused=0;
			sci->flags.speed=-1;
			scsXCall_Instance(sci,ToCodeINT(sci->code)[hdrDestructOffset],0,NULL);
			xr_free(sci->data_stack);
			xr_free(sci);
		}
}

int  SAPI scStart_Instance(scInstance *inst,...)
{
	va_list va;
	va_start(va,inst);
	return scVCall_Instance(inst,scGet_Symbol(inst,"main"),-1,(int *)va);
}

static int scsXCall_Instance(scInstance* inst,int ip,int paramc,int *params)
{
	Register(inst)[regSP]= ToCodeINT(inst->code)[hdrStackSize];//SP -offset of SS - if reaches 0 - stack overflow
	Register(inst)[regIP]= ip;
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
}

//if paramc<0 , paramc autodetect
int  SAPI scVCall_Instance(scInstance* inst,int address,int paramc,int *params)
{
	int i,ip;
	scInit_SeeR();
	//<ERASED> scKernelOnly(-1);
	if (!inst) rerr("Invalid instance!");
	if (Register(inst)[regCP]!=0) rerr( "Instance already running!");
	if (address==-1)
		rerr2( "Function not found in `%s'!",scGet_Instance_Title(inst));
	// if (address==-1) return -1;
	scErrorNo=0;
	deb1("Calling %d\n",address);
	//Prepare register and stack:
	ReadINT(inst->code+address,ip);//Read Address
	ReadINT(inst->code+address+4,i);//Size Of Params;
	if (i<0&&paramc<0) return -1;//variable!
	if (paramc<0) paramc=i;
	
	inst->flags.paused=0;
	inst->flags.speed=-1;
	
	return scsXCall_Instance(inst,ip,paramc,params);
scError_Exit:
	return 0;
}

int  SAPI scCall_Instance(scInstance* inst,int address,...)
{
	va_list va;
	va_start(va,address);
	return scVCall_Instance(inst,address,-1,&(va_arg(va,int)));
}


int SAPI scGet_Symbol(scInstance* inst,char *name)
// e.g. x=scCall_Instance(inst,scGet_Symbol(inst,"move"),10);//10 is a parameter
{
	int pocz,addr,cfg;
	char namer[1000];
	char* x;
	if (!inst) return -1;
	x=(char *)inst->code+ToCodeINT(inst->code)[hdrSymbolOffset];
	scInit_SeeR();
	if (!inst||(!inst->code)||(ToCodeINT(inst->code)[0]!=CHARS2INT('S','e','e','R'))
		||(ToCodeINT(inst->code)[1]!=CHARS2INT('V','C','P','U')))
		return -1;
	//<ERASED> scKernelOnly(-1);
	do{
		strcpy(namer,x);
		if (!namer[0]) return -1;//not found
		x+=strlen(namer)+1;
		ALIGN_INT(x);
		pocz=x-(char *)inst->code;
		ReadINT(x,addr);x+=4;
		ReadINT(x,cfg);x+=4;
		if (strcmp(namer,name)==0)
		{
			// deb3("Get_Symbol=%s, cfg=%d,addr=%d",name,cfg,addr);
			// deb1(", pocz=%d\n",pocz);
			if (cfg==-1) return addr;
			return pocz;
		}
	}while(namer[0]);
	return -1;//not found
}

//#define scRead_Var(inst,adr,typ) ((typ)(*(inst->data_stack+(adr))))
// e.g x=scRead_Var(inst,scGet_Symbol(inst,"x"),int);

void scAdd_Member_Symbol(char *name,...)
{
	void *i;
	va_list va;
	va_start(va,name);
	i=va_arg(va,void *);
	va_end(va);
	scAdd_External_Symbol(name,i);
}

void __cdecl scAdd_External_Symbol(char *name,void *addr)
{scSymbol *sym;
scInit_SeeR();
//<ERASED> scKernelOnly(;);
if (!scImportSymbols)//initialize dict
scImportSymbols=scdicNewDictionary();
if ((sym=scdicFoundSymbol(scImportSymbols->first,name)))
{scdicRemoveSymbol(scImportSymbols,name);
xr_free(sym->name);
xr_free(sym);
}
scdicAddSymbol(scImportSymbols,name,(int)addr,0,NULL);
}
// like Add_External_Symbol("printf",&printf); or
// ("gamelogic",&gamelogic); where gamelogic is a variable in the script
// and  the main program (the same for all instances & scripts)

void __cdecl scAdd_Internal_Header(char *name,char *addr)
{scSymbol *sym;
scInit_SeeR();
if (!scInternalHeaders)//initialize dict
scInternalHeaders=scdicNewDictionary();
if ((sym=scdicFoundSymbol(scInternalHeaders->first,name)))
{scdicRemoveSymbol(scInternalHeaders,name);
xr_free(sym->name);
xr_free(sym);
}
scdicAddSymbol(scInternalHeaders,name,(int)addr,0,NULL);
}

void  SAPI scPause_Instance(scInstance* inst,int p)
{
	scInstance *pr=scheduler->first;
	while (pr)
	{if (pr==inst)
	{pr->flags.paused=p;return;}
	pr=pr->next;
	}
}

void  SAPI scKill_Instance(scInstance* inst)
{
	scInstance *pr=scheduler->first;
	if (!pr) return;
	if (pr==inst) {
		scheduler->first=pr->next;
		Register(pr)[regCP]=0;
		return;
	}
	while (pr->next)
	{
		if (pr->next==inst)
		{scInstance *qr=pr->next;
		pr->next=pr->next->next;
		Register(qr)[regCP]=0;
		if (!pr->next) return;
		}
		pr=pr->next;
	}
}

//if paramc<0 , paramc autodetect
int  SAPI scVLaunch_Instance(scInstance* inst,int spd,int address,int paramc,int *params)
{
	int i;
	scInit_SeeR();
	if (Register(inst)[regCP]!=0) rerr( "Instance already running!");
	if (address==-1)
		rerr2( "Function not found in `%s'!",scGet_Instance_Title(inst));
	// if (address==-1) return -1;
	/***********Add to the list***************/
	inst->next=scheduler->first;
	inst->flags.speed=spd;
	inst->flags.paused=0;
	scheduler->first=inst;
	
	scErrorNo=0;
	deb1("Calling %d\n",address);
	//Prepare register and stack:
	Register(inst)[regSP]= ToCodeINT(inst->code)[hdrStackSize];//SP -offset of SS - if reaches 0 - stack overflow
	ReadINT(inst->code+address,Register(inst)[regIP]);//Read Address
	ReadINT(inst->code+address+4,i);//Size Of Params;
	if (i<0&&paramc<0)
		rerr2("Unknown number of paramters to function in `%s'",scGet_Instance_Title(inst));
	if (paramc<0) paramc=i;
	deb1("Size of params:%d\n",paramc);
	if (paramc)
	{//push all params form va to the stack;
		Register(inst)[regSP]-=paramc+4;
		memcpy((void*)(Register(inst)[regSS]+Register(inst)[regSP]+4),
			(params),paramc);
	}
	Register(inst)[regCP]=Register(inst)[regSP];
	return 1;
scError_Exit:
	return 0;
}

//if paramc<0 , paramc autodetect
int  SAPI scLaunch_Instance_GOTO(scInstance* inst,int spd,int address)
{
	scInit_SeeR();
	//<ERASED> scKernelOnly(-1);
	if (Register(inst)[regCP]!=0) rerr( "Instance already running!");
	if (address==-1)
		rerr2( "Function not found in `%s'!",scGet_Instance_Title(inst));
	// if (address==-1) return -1;
	/***********Add to the list***************/
	inst->next=scheduler->first;
	inst->flags.speed=spd;
	inst->flags.paused=0;
	scheduler->first=inst;
	
	scErrorNo=0;
	deb1("GOTO %d\n",address);
	//Prepare register and stack:
	Register(inst)[regCP]= Register(inst)[regSP]= ToCodeINT(inst->code)[hdrStackSize];//SP -offset of SS - if reaches 0 - stack overflow
	Register(inst)[regIP]=address;
	return 1;
scError_Exit:
	return 0;
}

int __cdecl scLaunch_Instance(scInstance* inst,int spd,int address,...)
{
	va_list va;
	va_start(va,address);
	return scVLaunch_Instance(inst,spd,address,-1,&(va_arg(va,int)));
}

//the scheduler
int  SAPI scContinue_Instances(void)//returns number of instances still running
{
	int i=0;
	scInstance *act,*prev=NULL;
	deb0("scContinue_Instances()\n");
	if (!scheduler->first) return 0;
	act=scheduler->first;
	scErrorNo=0;
	while (act)
	{deb1("Inst(%d)\n",i);
	i++;
	if ((!(act->flags.paused))&&(Executor(act,act->flags.speed)))//finished-erase from the list
	{scInstance* ol=act;
    if (prev) prev->next=act->next;
    else scheduler->first=act->next;
    act=act->next;
    Register(ol)[regCP]=0;
    deb0("Killed process\n");
	}
	else
	{
		prev=act;
		act=prev->next;
	}
	if (scErrorNo) return -1;
	}
	return i;
}

scInstance*  SAPI scGet_Forked(scInstance* I,int n)
{
	//<ERASED> scKernelOnly(NULL);
	I=(I->forked)?I->forked:I;//forked instance hasn't forked instances
	{
		scInstance *i=I->forks;
		while(i)
		{
			if (i->flags.forkno==n) return i;
			i=i->next_fork;
		}
	}
	return NULL;
}

scInstance*  SAPI scGet_Actual_Forked(int n)
{
	return scGet_Forked(scActual_Instance,n);
}

scInstance*  SAPI scFork_Instance(scInstance* I)
{
	scInstance *nI=NULL,*pI=I;
	//<ERASED> scKernelOnly(NULL);
	if (!I) return NULL;
	nI=New(scInstance);
	//if I is forked instance, let I be the parent
	if (I->forked)
	{deb0("Taking parent for creating forked");
    pI=I->forked;
	}
	//copy the flags, required changes will follow later
	nI->flags=I->flags;
	//set next number of fork
	nI->flags.forkno=-(--pI->flags.forkno);
	nI->forks=NULL;//has no children
	nI->forked=pI;// and has parent
	
	//add to the list of forks
	nI->next_fork=pI->forks;
	pI->forks=nI;
	
	//copy the address to the code segment (script)
	nI->code=I->code;
	
	//below:registers+STACK,DATA is in parent-----------------------------
	nI->data_stack=xr_malloc(256*4+ToCodeINT(I->code)[hdrStackSize]);
	memcpy(nI->data_stack,I->data_stack,256*4);//copy regs
	memcpy(nI->data_stack+256*4,I->data_stack+256*4+ToCodeINT(I->code)[hdrDataSize],ToCodeINT(I->code)[hdrStackSize]);//copy stack
	
	//actualize REGISTERS:----------------------
	// Register(nI)[regCS]=Register(I)[regCS];//CS -offset only
	// Register(nI)[regDS]=Register(I)[regDS];//DS -real pointer, common data seg
	// Register(nI)[regES]=ToCodeINT(I->code)[7]+(int)I->code;//ES -real,common
	Register(nI)[regSS]=(int)nI->data_stack+256*4;//SS -real,uncommon stack
	//SP is the same
	// Register(nI)[regSP]=ToCodeINT(I->code)[hdrStackSize];//SP -offset of SS - if reaches 0 - stack overflow
	Register(nI)[regBP]=(int)((int64)Register(nI)[regBP]+Register(nI)[regSS]-Register(I)[regSS]);//-ToCodeINT(I->code)[10];
	// Register(nI)[regSP]+=-ToCodeINT(I->code)[10];
	Register(nI)[regCP]=0;
	
	// Register(nI)[regIE]=0;
	
	return nI;
}

void  SAPI scKill_Forked_Instances(scInstance* I)
{
	//<ERASED> scKernelOnly(;);
	if (!I) return;
	if (I->forks)
	{scInstance *i=I->forks,*n;
	while(i)
	{
		scKill_Instance(i);
		n=i->next_fork;
		scFree_Instance(i);
		i=n;
	}
	I->forks=NULL;
	I->flags.forkno=0;
	}
}

void  SAPI scKill_My_Forked_Instances()
{
	int pr;
	pr=scGet_Priority(scActual_Instance);
	scSet_Priority(scActual_Instance,scrights_TOP);
	scKill_Forked_Instances(scActual_Instance);
	scSet_Priority(scActual_Instance,pr);
}

void  SAPI scKill_Forked(int i)
{
	int pr;
	pr=scGet_Priority(scActual_Instance);
	scSet_Priority(scActual_Instance,scrights_TOP);
	scKill_Instance(scGet_Actual_Forked(i));
	scFree_Instance(scGet_Actual_Forked(i));
	scSet_Priority(scActual_Instance,pr);
}

int  SAPI scGet_Forked_Status(int i)
{
	int pr;
	pr=scGet_Priority(scActual_Instance);
	scSet_Priority(scActual_Instance,scrights_TOP);
	i=scGet_Instance_Status(scGet_Actual_Forked(i));
	scSet_Priority(scActual_Instance,pr);
	return i;
}

int  SAPI scGet_Instance_Status(scInstance *I)
{
	scInstance *pr=scheduler->first;
	//<ERASED> scKernelOnly(scstatus_FREE);
	if (!I) return scstatus_FREE;
	while (pr)
	{if (pr==I)
    {
		if (pr->flags.paused) return scstatus_PAUSED;
		else return scstatus_RUNNING;
    }
	pr=pr->next;
	}
	return scstatus_FREE;
}

BOOL  SAPI scAssignInstance(scScriptTypeHeader *script,scInstance *ins,char *typeinfo)
{
	int i=3;
	char *t;
	char *ti;
	if (!script) return false;
	script->ins=NULL;
	if (!typeinfo||!ins) return false;
	deb1("\nAssigning instance:%s\n",typeinfo);
	ti=xr_strdup(typeinfo);
	t=strtok(ti,"{;}");
	if (strcmp(t,"script"))
    {xr_free(ti);return false;}//not a script
	for(t=strtok(NULL,"{;}");t;t=strtok(NULL,"{;}"))
	{
		((int*)script)[i]=scGet_Symbol(ins,t);
		deb2(" member '%s' at %d\n",t,((int*)script)[i]);
		if (!((int*)script)[i])
		{xr_free(ti);return false;}
		i++;
	}
	script->regDS=Register(ins)[regDS];
	script->ins=ins;
	deb1("At exit typeinfo=%s\n",typeinfo);
	xr_free(ti);
	return true;
}
