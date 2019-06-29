/************************************************************************
       -= SEER - C-Scripting engine v 0.3a =-
State      : under implementation
Author     : Przemyslaw Podsiadly
             email: ppodsiad@elka.pw.edu.pl
             WWW:   http://home.elka.pw.edu.pl/~ppodsiad
File:internal.h
Desc:internal header file
************************************************************************/
#ifndef __SCRIPT_INTERNAL_H__

#define STR_SeeR_Version "94"

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <setjmp.h>

#ifdef _MSC_VER
#define SC_MSVC
#define SC_INTEL
//**************************************************************************
//MS VISUAL C++

#include <string.h>
#define MAXINT 0x7fffffff

typedef __int64 int64;
#else
//**************************************************************************
//GCC
//__DJGPP__ || __LINUX__
#define SC_GCC
#define SC_INTEL
#include <strings.h>
#include <values.h>
typedef long long int64;
#endif

//**************************************************************************
#define _LITTLE_ENDIAN

//#define _SHOW_MACHINE_CODE

//#define  EXACT_DEBUG
//fflush every debX... - slows down everything as hell

//#define DEBUG

//When DEBUG is on, the DEBUGLEVEL says:
//0 - log all
//1 - some are skipped - declare.c
//2 ? more skipped
//3 ? "clean" - assembler and some comments
//4 - "clean" - only assembler + lines
//5 - "clean" - only assembler
#define DEBUGLEVEL 1

#ifdef DEBUG
#define debflush()  fflush(debfile)
extern int debug_flag;
extern FILE *debfile;
extern void __cdecl dbg_out(char *fmt,...);
#ifdef EXACT_DEBUG
#define adeb0(x)		dbg_out(x)
#define adeb1(x,y)		dbg_out(x,y)
#define adeb2(x,y,z)	dbg_out(x,y,z)
#define adeb3(x,y,z,q)	dbg_out(x,y,z,q)
#else //!EXACT_DEBUG
#define adeb0(x)		dbg_out(x)
#define adeb1(x,y)		dbg_out(x,y)
#define adeb2(x,y,z)	dbg_out(x,y,z)
#define adeb3(x,y,z,q)	dbg_out(x,y,z,q)
#endif
#if DEBUGLEVEL>=3
//clean
#define deb0(x)
#define deb1(x,y)
#define deb2(x,y,z)
#define deb3(x,y,z,q)
#else
//normal
#define deb0 adeb0
#define deb1 adeb1
#define deb2 adeb2
#define deb3 adeb3
#endif
//with gcc this one would work:
//#define deb(x...) {if (debug_flag)fprintf(debfile,x);}

#else//!DEBUG:

#define debflush()
#define adeb0(x)
#define adeb1(x,y)
#define adeb2(x,y,z)
#define adeb3(x,y,z,q)

#define deb0 adeb0
#define deb1 adeb1
#define deb2 adeb2
#define deb3 adeb3
//#define deb(x...)
//#define debprintf(x...)
#define debif(x)
#endif

//#endif


//#ifdef __GNUC__
#ifdef _LITTLE_ENDIAN
#define CHARS2INT(a,b,c,d) ((d<<24)+(c<<16)+(b<<8)+(a))
#else
#define CHARS2INT(a,b,c,d) ((a<<24)+(b<<16)+(c<<8)+(d))
#endif

#ifndef DIMENSION
#define DIMENSION(x)    ((int)sizeof(x)/sizeof(x[0]))
#endif
#define TRUE 1
#define FALSE 0

#define scAssert(func,v) if(!(v)) serr3(scErr_Internal, internal_compiler_error," <"#func":"#v"> "__FILE__,__LINE__);
#define scError(func) serr3(scErr_Internal, internal_compiler_error," <"#func"> "__FILE__,__LINE__);

#define IsSymbol(c) (((c)>' '&&(c)<'0')||((c)>'9'&&(c)<'A')||\
                    ((c)>'Z'&&(c)<'_')||((c)>'z'&&(c)<127)||c=='`')

#define STALine ST->lines[ST->act_line]

#define New(T) scAlloc(sizeof(T))

#define lexeq(a) (strcmp(ST->lexema,a)==0)

#define Advance if (!scget_lexema(ST)) {serr(scErr_Parse,"Parse error");}

#define ALIGN_INT(a) if ((unsigned)(a)%4) (unsigned)(a)+=4-(unsigned)(a)%4


/* scScript - seer script header (as (int []))*/
#define hdrAllSize              3
#define hdrHeaderSize           4
#define hdrSymbolOffset         5
#define hdrCodeOffset           6
#define hdrDataOffset           7
#define hdrConstsOffset         8
#define hdrInitOffset           9
#define hdrCodeSize             10
#define hdrDataSize             11
#define hdrConstsSize           12
#define hdrStackSize            13
#define hdrImportedOffset       14
#define hdrConstructOffset      15
#define hdrDestructOffset       16
//EOH! is next                  17
#define hdrHeaderSizeOf         18*4
//**************************************************************************

struct scSymbol;
struct _value;
//PREDEFINED TYPES,returned by scvalGetValType
typedef enum valTYPE{
     valINT,
     valLONG,
     valCHAR,
     valFIXED,
     valFLOAT,
     valDOUBLE,
     valSHORT,
     valREGISTER,
     valVOID,
     //not predefined types:
     valPOINTER,
     valSTRUCT,
     valSCRIPT,
     } valTYPE;
//TYPE descriptor
enum {
      typTYPE=1+2+4+8,//switch(flags&typTYPE) {case typPREDEFINED:}
      typPREDEFINED=0,//in main :index of basic types
      typUSERDEFINED=1,//in main:pointer to symbol with type
      typSTRUCT=2,typUNION=3,typENUM=4,//in main there is a list
      typSCRIPT=8,     //alike struct (in main)
      typINVALID=15,

      typOPTION=16+32+64+128,//option standing before
      typCONST=16,
      typUNSIGNED=32,
      typSTATIC=64,
      typREGISTER=128,
      typCONSTRUCTOR=256,
      typDESTRUCTOR=512,
/*We don't need these, do we?
      typSIZE=512+1024+2048,
      typDEFAULT=0,
      typSHORT=512,typLONG=1024,
*/
      };

typedef struct scType {
        int flags;//typPREDEFINED...
        char *params;//it is e.g.  (1aff32a) or **[12]* (p->p->table[12]of p->T
        //what about function
        struct scSymbol *main;//Main type or Table with scSymbol, last has flags==-1(structs)
} scType;

typedef enum scAddrFlag{
        adrPOINTER=1+2+4,
        adrSTOREPOINTER=1+2,
        adrES=1,
        adrDS=2,
        adrBP=3,
        adrCPU=4,
        adrSTORECPU=adrCPU&adrSTOREPOINTER,
        adrCS=5,
        adrIMMIDIATE=8,
        adrREGISTER=16,
        adrIMPORTED=32,
        adrTYPEONLY=64,
        adrSCRIPT=128,
        adrADDRESS=256,//we have eg [ES+12], but we should have address of it
                       //,like a0=ES+12; with this we use PUSHADR etc.
                       //with * or [] we need only to delete this flag.
                       //for address we have to scvalPutToRegister.
} scAddrFlag;
/*
 a0    adrREGISTER
 12    adrIMMIDIATE
 [CS+a0]adrREGISTER|adrCS
*/

typedef struct scAddress {
        int address;//here we store offset, register, but not value!
        scAddrFlag flags;
        union
        {int ival;
         float fval;
         double dval;
        } val;//typically value,
              //but when imported, ival stores dispatcher number
} scAddress;

typedef struct scSymbol {
        scAddress adr;//copy it to val
        char *name;
        char *import_name;//usually NULL
//        int used;
        int params;//size of params
        struct _value* val;
        scType type;
        struct scSymbol* next;//TODO:here should be a place for parent
        struct scSymbol* parent;
        void *reserved;
} scSymbol;

typedef struct{
       scAddress adr;//IMPORTED flag must be correct
       scType type;
       scSymbol* sym;
} _value;
/*  Examples of description
                Adres:Adres,flags,val        Type:flags,params,main
int x;  _value={{12,adrDS|adrIMMIDIATE,0}, {typPREDEFINED,NULL,valINT},x}
a5      _value={{5,adrREGISTER,0}          {typPREDEFINED,NULL,valINT},x}
[DS+a4] _value={{4,adrDS|adrREGISTER,0},   {typSTRUCT,NULL,xxxx},xxx}
[12]    _value={{12,adrIMPORT,0},          {typPREDEFINED,NULL,valINT},x}
int *x; _value={{12,adrDS|adrIMMIDIATE,0}, {typPREDEFINED,"*",valINT},x}
*/
//**************************************************************************
//vars.c:
BOOL sciValidIdentifier(char * name);
BOOL sctypIsReference(scType *type);
int sctypSizeOf(scType *type);
int scvalSizeOf(_value *val);
void sctypTypeOfStr(scType* type,char *name,char *buf);
int sctypInsistsAlignment(scType *type);
void scvalFreeValue(_value *v);
int scvalFindFreeRegister(_value *val);
int scvalFindFreeDoubleRegister(_value *val);
valTYPE scvalGetValType(_value* v);
valTYPE sctypGetValType(scType *type);
void scvalPutToRegister(_value *val,int freeit);
void scvalArrayToPointer(_value *v);
char *sciValTypeStr(valTYPE v);
BOOL sciValTypeFloating(valTYPE fl);
BOOL scvalIsOfType(_value *v,valTYPE vt);
BOOL sctypIsFunction(scType *type);
BOOL scvalIsImmidiate(_value *v);
BOOL scvalIsImmidiateOfType(_value *v,valTYPE vt);
double scvalImmidiateValue(_value *v);
_value* scvalSetImmidiate(_value *v,valTYPE vt,int ival,double dval);
_value* scvalSetINT(_value *v,int ival);
_value* scvalSetRegister(_value *v,valTYPE vt,int reg);
_value* scvalSetSymbol(_value *v,scSymbol *s);
void scvalGetAddress(_value *val,int situ);
void scvalAccessReference(_value *val);
void sctypGoDeeper(scType* typ);
BOOL scvalSameType(_value *val1,_value *val2);
void scvalConvToDouble(_value *val);
void scvalConvToFloat(_value *val);
void scvalConvToInt(_value *val);
void scvalConvToFixed(_value *val);
void scvalCast(_value *val,scSymbol *sc);
void scvalDisplayValue(char *x,_value *v);
void sctypSimplifyType(scType *type);
void sciAddImportedFunction(scSymbol* sym,scSymbol* in_struct);
int sciReadIntegerInBrackets(int def);
void sciCountFunctionParams(scSymbol *sym,scSymbol *in_struct,int Add_Params_To_Dict);
void scvalBothTypes(_value* v1,_value* v2);
scSymbol *sctypGetStruct(scType *typ);
void scvalConstantString(_value *val,char *s);
//expr.c:
void scvalCallFunction(scSymbol *sym,_value *val,scSymbol* in_struct,
                       _value *thisval,_value *scriptval,_value *params,BOOL CallerIgnore);
_value* scvalReadParameters();
scSymbol *scsFind_Function(char *name,scSymbol *find_among,scSymbol *also_find,_value *params,char *desc);
scSymbol *sctypFindExactFunction(char *name,scSymbol *find_among,scSymbol *also_find,scType *params);
BOOL sctypExactFunction(scType *t1,scType *t2);
BOOL scsTypesExact(scType* t1,scType* t2);
int sciReadImport(char **import_name);
int *scvalPushUsedRegisters();
//void scvalPushUsedRegisters(int *regs);
void scvalPopUsedRegisters(int *regs);
void sciAdjustScriptConstructor(int level);



//**************************************************************************
//int sciReadSubExp(int i,_value *val,BOOL ExpRequired);


//**************************************************************************


typedef struct {
        scSymbol *first;
} scDictionary;

void scdicAddSymbol(scDictionary *dict,char *nam,int addr,int typeflags,void *reserved);
void scdicAddSymbol_Ex(scDictionary *dict,scSymbol* s);
void scdicAddSymbolToEnd_Ex(scDictionary *dict,scSymbol* s);
void scdicRemoveSymbol(scDictionary *dict,char *nam);
//before store and restore you can only add symbols to the dict ,else
//horrible crash possible!
scSymbol* scdicStoreDictionary(scDictionary *dict);
void scdicRestoreDictionary(scDictionary *dict,scSymbol *frst,void Remove(scSymbol *));
void scdicFreeFunctionSymbol(scSymbol*);

scSymbol* scdicFoundSymbol(scSymbol *first,char *nam);
scDictionary *scdicNewDictionary();
scDictionary* scdicDeleteDictionary(scDictionary *l);

//**************************************************************************

//source
struct Source_Text;
typedef struct {struct Source_Text *ST;int lin,chr;} scPosition;

typedef struct Source_Text{
       char **lines;
       int act_line,act_char;
       int num_lines,alloced_lines;
       int erase;              //permission for deletion after use
       char name[256];
       int linedif;
       char lexema[128];
       int lextype;//==0 - unknown
       int lexstart;
       int whitespaces;//before readed lexema.
       int newline;//if newline apeared
       scPosition lastpos;
} Source_Text;


void scSetPosition(scPosition n);
void scGetPosition(Source_Text *ST,scPosition *n);

Source_Text* new_Source_Text();
void delete_Source_Text(Source_Text*);
Source_Text* scPreprocessor(Source_Text*);

BOOL scomit_whitespaces(Source_Text* ST);
BOOL scget_lexema (Source_Text*);
char* scget_line(Source_Text *ST,int ignore_whitespaces);
void scSwap(char *temp,char *source,char *toins,int beg,int end);
void scget_back (Source_Text* ST);

int read_integer(Source_Text *ST,char *s,int *integ);
int getStructMemberFromName(char *name,scSymbol **in_struct);

//**************************************************************************
char *scStrdup(char *x);
void* scAlloc(int size);
void *scRealloc(void *mem,int size);
void scFree(void *x);
void scInitAlloc();
void scCleanUp();
void scmemTestUp();

//**************************************************************************
scScript scCompile(Source_Text *ST);

void scScript_Error(Source_Text* ST,char* filename,int linenum,int err_num,char *er, ...);
/*************************ERROR HANDLING********************************
MACROS:
check_error
***********************************************************************/
/*
#define serr(num,x) {scScript_Error(ST,__FILE__,__LINE__,num,x);goto scError_Exit;}
#define serr2(num,x,y) {scScript_Error(ST,__FILE__,__LINE__,num,x,y);goto scError_Exit;}
#define serr3(num,x,y,z) {scScript_Error(ST,__FILE__,__LINE__,num,x,y,z);goto scError_Exit;}
#define serr4(num,x,y,z,q) {scScript_Error(ST,__FILE__,__LINE__,num,x,y,z,q);goto scError_Exit;}
#define cerr {if (scErrorNo) goto scError_Exit;}
#define rerr(x) {scRuntime_Error(x);goto scError_Exit;}
#define rerr2(x,y) {scRuntime_Error(x,y);goto scError_Exit;}
*/
#define serr(num,x) {scScript_Error(ST,__FILE__,__LINE__,num,x);longjmp(act.jump,1);}
#define serr2(num,x,y) {scScript_Error(ST,__FILE__,__LINE__,num,x,y);longjmp(act.jump,1);}
#define serr3(num,x,y,z) {scScript_Error(ST,__FILE__,__LINE__,num,x,y,z);longjmp(act.jump,1);}
#define serr4(num,x,y,z,q) {scScript_Error(ST,__FILE__,__LINE__,num,x,y,z,q);longjmp(act.jump,1);}
//#define cerr {}
#define rerr(x) {scRuntime_Error(x);goto scError_Exit;}
#define rerr2(x,y) {scRuntime_Error(x,y);goto scError_Exit;}
#define rerr3(x,y,z) {scRuntime_Error(x,y,z);goto scError_Exit;}
//Language definition,FIRST THE LONGEST!:

typedef struct{
 char *name;
 int size;
} NameNInt;

typedef struct{
 char* name;
 int pars;
} NameNPar;

enum {regCS=240,regDS,regES,regSS,regIP,regSP,regBP,
   /*counter=1*//*FORKED N.*/
      regCX,regCP,regIE,regFN,regOPTIONS};
            /*Call Stack Ptr*/

void Gen_Opcode(int opcode,_value* v1,_value* v2);
void Gen_Align();

//*************************************************************************

typedef struct {
        char *mem;
        int size;
        int pos;//used
} pmem;

int new_pmem(pmem* mem,int init_size);
void free_pmem(pmem*);
void pop_pmem(pmem* mem,int count);
void push_pmem(pmem* mem,int size,void *data);
void pushint_pmem(pmem* mem,int x);
void pushstr_pmem(pmem* mem,char *s);
void align_pmem(pmem *mem);

//**************************************************************************
typedef struct {
       _value* used;//if used=NULL, it can be temporary!
       int locked;
       } RegsTable[256];

#define duringIDLE                      0
#define duringPREPROCESSING             1
#define duringCOMPILING                 2

#define optEXPR                         1

typedef struct {
       int during;
       RegsTable registers;
       int afterbp,dataused;//first xr_free byte in data segment (used by global vars)
       int stacksize;
       int ip;//exact copy of code.pos
       int unreal;//the code generated may not apear on output!
       int lines_compiled;
       jmp_buf jump;
       int options;
       int lastInitIp,lastFuncIp;
       pmem code,inits,consts,final,imports;
} ActData__;

//**************************************************************************
//vcpudeco:
extern scFunctionDispatcher scUserFunctionDispatcher[SC_MAX_FUNCTION_DISPATCHERS];

int  SAPI	Executor(scInstance *Ins,int speed);

//other:
void SAPI	scInit_SeeR			();
int	 SAPI	scLaunch_Instance_GOTO(scInstance* inst,int spd,int address);

//**************************************************************************
//variables:

//extern scScript *code;

extern pmem scExternals;//name\0addr...til name[0]==0;
extern scDictionary* scInternalHeaders;

extern char *Special_Symbols[];
extern NameNInt Standard_Types[];
extern NameNPar opcodes_vcpu[];
extern char *Keywords[];

extern char *parse_error,*invalid_operand_to,
            *illegal_cast,*internal_compiler_error,
            *illegal_type_conversion;
//compiler
extern Source_Text *ST;
extern ActData__ act;
extern scDictionary *SC_types,*SC_symbols,*SC_temporary;
//static still scDictionary *SC_exports;

//declare
scSymbol *sctypGetDefinition();
scSymbol *sctypGetShortDefinition(scType* typ);
scSymbol *scsGetTypeDeli();
//expr
int Read_Expression(_value *val,BOOL RequiredExp);
int sciReadSubExp(int i,_value *val,BOOL ExpRequired);

//initial
void sciReadInitialization(scSymbol* deli,scSymbol* sym);

extern scSymbol this_symbol;
extern char *hdrInstances;
extern char *hdrKernel;

extern BOOL SeeR_Inited;
//**************************************************************************

#ifdef __cplusplus
}
#endif

#define __SCRIPT_INTERNAL_H__
#endif
