/************************************************************************
       -= SEER - C-Scripting engine v 0.91a =-
State      : under implementation
Author     : Przemyslaw Podsiadly
             email: ppodsiad@elka.pw.edu.pl
             WWW:   http://home.elka.pw.edu.pl/~ppodsiad
File:compiler.c
Desc:main compiling routines
99.07.05 Classes support started
99.02.21 Declare.c and expressi.c moved to separate files
99.02.15 Initialization fixing...
************************************************************************/

#define INTERNAL_FILE
#include <seer.h>
#include "internal.h"
#include "code.h"

Source_Text *ST;
scDictionary *SC_types,*SC_symbols,*SC_temporary=NULL;
static scDictionary *SC_exports;
ActData__ act;
static scSymbol *function=NULL;
static scSymbol* global_symbols=NULL;

scSymbol this_symbol={
        {0,0,{0}},//address
        "this",NULL,
        0,              //params
        NULL,   //value
        {typUSERDEFINED,"*",NULL/*struct ptr here*/},//type
        NULL,NULL,NULL    //if !NULL, then this is valid and we're doing
                        //in member function or in struct
};

//usage:registers should be cleared at beginning and end of function!!!

static void scsCheckFunctionDefinitions(char *s,scSymbol *p);
static void scsReadOneStatement(scSymbol* deli);

///////////////////////////////////////////////////////////////////////////
////scsSymbolInfo//////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static void scsSymbolInfo(scSymbol *p)
{
#ifdef DEBUG
 if (p->type.params)
 {deb2(" %8s : `%s' at ",p->name,p->type.params);}
 else
 {deb1(" %8s : at ",p->name);}
 if ((p->adr.flags&adrPOINTER)==adrCS) deb0("CS+");
 if ((p->adr.flags&adrPOINTER)==adrDS) deb0("DS+");
 if ((p->adr.flags&adrPOINTER)==adrBP) deb0("BP+");
 deb1("%d",p->adr.address);
 if (p->type.flags&typSTATIC) deb0(" (static) ");
 deb0("\n");
#endif
}

///////////////////////////////////////////////////////////////////////////
////scsReadExp/////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//deli show the last variable from the statement one level up
static void scsReadExp(_value* val,BOOL RequiredExp)
{
       Advance;
       memset(act.registers,0,sizeof(act.registers));
       Read_Expression(val,RequiredExp);//we don't care about the result!
}

///////////////////////////////////////////////////////////////////////////
////scsReadExp_PreserveRegs////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static void scsReadExp_PreserveRegs(_value* val,BOOL RequiredExp)
{
       RegsTable p;
       //scget_lexema(ST);//mus
       Advance;
       memcpy(p,act.registers,sizeof(p));
       Read_Expression(val,RequiredExp);//we don't care about the result!
       memcpy(act.registers,p,sizeof(p));
}

///////////////////////////////////////////////////////////////////////////
////scsRoundTo32bit////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static int scsRoundTo32bit(int a)
{
    if (a%4) a+=4-a%4;
    return a;
}

//****************************************************
static void scsReadStatement(scSymbol* deli);

///////////////////////////////////////////////////////////////////////////
////sciCountFunctionParams/////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void sciCountFunctionParams(scSymbol *sym,scSymbol *in_struct,int Add_Params_To_Dict)
{char *x=sym->type.params;
 int a=8,pn=0;
 scSymbol* dx;
 sym->params=0;//we're counting them here

//when we return structure, we have the pointer to it just before
//other parameters (and after 'this', if it exists).
 if (!sctypIsReference(&sym->type)&&sctypGetValType(&sym->type)==valSTRUCT)
 {
     if (!this_symbol.reserved)//a trick - we use this_symbol
     {                         //to store the position of return struct
        this_symbol.adr.address=a+4;//'this' is normally just after ret_struct
        this_symbol.adr.flags=adrBP+adrIMMIDIATE;
     }
     a+=4;
 }

 if (in_struct)//do something with 'this'
 {
        sym->params+=4;
        if (Add_Params_To_Dict)
        {
                this_symbol.adr.address=a;
                this_symbol.adr.flags=adrBP+adrIMMIDIATE;
                this_symbol.type.main=in_struct;
                this_symbol.reserved=in_struct;
                scdicAddSymbol_Ex(SC_symbols,&this_symbol);
        }
        a+=4;
 }

 while (*x==','||*x=='(')
 {
 pn++;
 x++;//omit ( or ,
// deb("`%c'",*x);
 if (*x==')') break;
 if (x[0]=='.'&&x[1]=='.'&&x[2]=='.')
 {//... encountered- do nothing
  x+=3;//omit `...'
 }
 else
 {_value *v;
  dx=(scSymbol *)strtoul(x, &x, 16);
  if (*x=='=') v=(_value *)strtoul(++x, &x, 16);
  sym->params+=sctypSizeOf(&dx->type);
  dx->adr.address=a;
  dx->adr.flags=adrBP+adrIMMIDIATE;
  a+=sctypSizeOf(&dx->type);
//  deb1("%s,",dx->name);
  if (Add_Params_To_Dict&&!dx->name){scget_back(ST);
   serr3(scErr_Operand,"Parameter %d to function '%s' is anonymous",pn,sym->name);}
  //check if such a name is  ( e.g foo(int a,int a);)
  if (Add_Params_To_Dict) scdicAddSymbol_Ex(SC_symbols,dx);
  }//!...
 }
 if (*x!=')') serr(scErr_Internal,"Internal compiler error(1)");

}

///////////////////////////////////////////////////////////////////////////
////scsymCountAndDestruct/////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static int _fdFS;
static void scsymCountAndDestruct(scSymbol *sym)
{_fdFS+=sctypSizeOf(&sym->type);
 scsSymbolInfo(sym);
 if (sctypGetValType(&sym->type)==valSTRUCT&&!sctypIsReference(&sym->type))
 {//first find destructor, if it exists
  char dname[1024];
  scSymbol *_this;
#if DEBUGLEVEL==0
  deb0("Call destructor:\n");
  deb1("Symbol is %s\n",sym->name);
#endif
  _this=sctypGetStruct(&sym->type);
  scAssert(scsymCountAndDestruct,_this);
  strcpy(dname+1,_this->name);
  dname[0]='~';
#if DEBUGLEVEL==0
  deb2("Class %s with %s\n",_this->name,dname);
#endif
  if (scdicFoundSymbol(_this->type.main,dname))
  {
//  int regs[240];
  _value thisval,val;
  _value *params=scAlloc(sizeof(_value));
  scSymbol *fsym;
  //final-no parameters
  params->adr.flags=adrIMPORTED&adrTYPEONLY;
  params->type.flags=typINVALID;
  deb1("Call destructor %s!\n",dname);
  scvalSetSymbol(&thisval,sym);
  val=thisval;
//  scvalPushUsedRegisters(regs);
//  params=scvalReadParameters();
  fsym=scsFind_Function(dname,_this->type.main,NULL,params,NULL);
  scAssert(scsymCountAndDestruct,fsym);
  scvalCallFunction(fsym,&val,sym,&thisval,NULL,params,false);
//  scvalPopUsedRegisters(regs);
 }
 }
// scdicFreeFunctionSymbol(sym);
}

void sciAdjustScriptConstructor(int level)
{
        Gen_Align();
        switch(level)
        {
        case 0://the end of initialization part
          if (act.ip!=act.lastFuncIp)//there was some code, so we jump out
                {_value vt1;
                 act.lastInitIp=act.ip;
#if DEBUGLEVEL<5
                 adeb0("Script constructor adjust\n");
#endif
                 Gen_Opcode(opvcpuJMP,scvalSetINT(&vt1,0),NULL);
                }
          break;
        case 1://the beginning of initialization part
          if (act.lastInitIp)//we don't need to check, but...
                {_value vt1;
                 act.lastFuncIp=act.ip;
                 act.ip=act.lastInitIp;
#if DEBUGLEVEL<5
                 deb0("Script constructor adjust\n");
#endif
                 Gen_Opcode(opvcpuJMP,scvalSetINT(&vt1,act.lastFuncIp),NULL);
                 act.code.pos=act.ip=act.lastFuncIp;
                }
          break;
        }
        Gen_Align();
}
///////////////////////////////////////////////////////////////////////////
////scsReadFunctionBody////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static void scsReadFunctionBody(scSymbol* deli,scSymbol *sym,int previous_address,scSymbol *in_struct)
{
  scSymbol* par_symbols;
  if (!sctypIsFunction(&sym->type))
     serr2(scErr_Declaration, "Invalid variable `%s' initialization!",sym->name);
  if (deli)
     serr2(scErr_Declaration, "Function '%s'in function definition",sym->name);
  //here jump
  sciAdjustScriptConstructor(0);

  Gen_Align();
  if (previous_address<-1)
   {_value vt1;
    int i,o=act.ip;
    scvalSetINT(&vt1,o);
#if DEBUGLEVEL<5
    adeb0("Declaring previously used function\n");
#endif
    i=previous_address;
    while (i!=-1)
    {
     act.ip=-i;
//     deb1("call at %d\n",act.ip);
     i=((int*)(act.code.mem))[act.ip/4+1];
     Gen_Opcode(opvcpuCALL,&vt1,NULL);
    }
    act.ip=o;
   }
  sym->adr.address=act.ip;//none
  sym->params=0;
  deb1("Address:%d\n",sym->adr.address);
     //    debprintf("ENTERING function definition!!!\n");
  deb1("=======================>>ENTERING function `%s'!\n",sym->name);
  global_symbols=scdicStoreDictionary(SC_symbols);
  act.afterbp=0;//from the bp!- 4 bytes for BP on stack

  //SET THE FUNCTION ADRESS

  Gen_Opcode(opvcpuENTER,NULL,NULL);

  //Add parameters
  sciCountFunctionParams(sym,in_struct,1);//add params to dict

  par_symbols=scdicStoreDictionary(SC_symbols);
  function=sym;
  scsReadStatement(sym);//global_symbols);

  this_symbol.reserved=NULL;//not in member!
  this_symbol.type.main=NULL;

  deb0("+++\nLocal symbols:\n");
  {scSymbol* p=SC_symbols->first;
   while (p!=global_symbols)
   {if (!(p->type.flags&typSTATIC))
        {
        scsymCountAndDestruct(p);
        p=p->next;
        }
        else
        {//FIXME:make STATIC be the end, if !global_symbols
        if (!global_symbols)
        {global_symbols=p;
         deb0("WARNING:no global symbols before first static. UNTESTED!\n");
        }
        else
        p=p->next;
        }
   }
  //change all STATICs to special name, so you can reuse the originalname
   while(p)
   {if (p->name[0]!='%'&&(p->type.flags&typSTATIC))
        {char *x=scAlloc(strlen(p->name)+strlen(function->name)+6+3+1);
           x[0]='%';
           strcpy(x+1,"STATIC");
           strcpy(x+strlen(x),":");
           strcpy(x+strlen(x),function->name);
           strcpy(x+strlen(x),"-");
           strcpy(x+strlen(x),p->name);
           scFree(p->name);
           p->name=x;
        }
    p=p->next;
   }
  }//TODO:make STATIC add to all symbols
  function=NULL;

  Gen_Opcode(opvcpuLEAVE,NULL,NULL);
  Gen_Opcode(opvcpuRET,NULL,NULL);
  //there was a jump somewhere of course
  sciAdjustScriptConstructor(1);
//  Gen_Align();

  deb1("=======================>>LEAVING  function `%s'!\n",sym->name);
  scdicRestoreDictionary(SC_symbols,par_symbols,scdicFreeFunctionSymbol);
  SC_symbols->first=global_symbols;
}

///////////////////////////////////////////////////////////////////////////
////sciReadDefinition//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
/* after sctypGetDefinition was successful
   in_struct==NULL - not in struct
   in_struct!=NULL - inside structure definition
*/
void sciReadDefinition(scSymbol *deli,scSymbol *sym,scSymbol *in_struct)
{_value val;
 scSymbol *prev_sm=NULL;
 int previous_address=-1,first=1,newdecl=0;
 do{
 if (!first)
    {sym=sctypGetShortDefinition(&sym->type);}
 newdecl=0;
 if (first)
    {
    if (((sym->type.flags&typTYPE)==typUSERDEFINED) &&
        ((sym->type.main->type.flags&typTYPE)==typSTRUCT
        ||(sym->type.main->type.flags&typTYPE)==typSCRIPT))
      {//deb1("here we are %s\n",sym->type.main->name);
       if (!deli&&sym->type.main!=in_struct
            && !sym->type.main->parent)
       {scSymbol *psc;
       if (!(psc=scdicFoundSymbol(SC_types->first,sym->type.main->name)))
       {
        scdicAddSymbol_Ex(SC_types,sym->type.main);//add as a global type
        deb1("struct/script %s added.\n",sym->type.main->name);
        newdecl=1;
        //definition of new type struct!
       }
       else
       {
#if DEBUGLEVEL==0
        deb0("Already defined?\n");
#endif
        if (!sym->type.params||!sym->type.params[0])
           newdecl=1;//assumption,TODO
       }
       }
      }
    }
 if (!sym->name)
    {
     if (newdecl) continue;
     serr(scErr_Declaration, "Declaration of anonymous variable");
    }
 //here
 getStructMemberFromName(sym->name,&in_struct);
//TODO :do we need this ever?
// if (!sciValidIdentifier(sym->name))
//     serr2(scErr_Declaration, "Invalid identifier `%s' in declaration",sym->name);
   if (!in_struct)
    prev_sm=sctypFindExactFunction(sym->name,SC_symbols->first,NULL,&sym->type);
   else
   {
    prev_sm=sctypFindExactFunction(sym->name,in_struct->type.main,NULL,&sym->type);

    if (!prev_sm)
      serr3(scErr_Declaration, "Function %s is not a member of %s",sym->name,in_struct->name);
   }

  //Add function prototype
   if (sctypIsFunction(&sym->type)){
    if (!first)//function prototype can be the only one here
           serr(scErr_Declaration, "Forbidden function declaration");
    if (prev_sm)
     {//function had a prototype

      //TODO:compare declaration
      if (prev_sm->adr.address>=0)
       serr2(scErr_Declaration,"Symbol `%s' already defined",prev_sm->name);
      //erase sym:
      previous_address=prev_sm->adr.address;
          if (prev_sm!=sym)//let's delete new one
          {
                if (!in_struct)
                {
                scdicRemoveSymbol(SC_symbols,prev_sm->name);
                scdicFreeFunctionSymbol(prev_sm);prev_sm=NULL;
                }
                else
                {
                        scdicFreeFunctionSymbol(sym);
                        sym=prev_sm;
                }
          }

     }
   }
   if (!in_struct)
   {
     if (sym->type.flags&typSTATIC)
      scdicAddSymbolToEnd_Ex(SC_symbols,sym);
     else
      scdicAddSymbol_Ex(SC_symbols,sym);
   }

/* Count address of symbol */
 if(!sctypIsFunction(&sym->type))
 {
  if (deli)
   {//stack
    int i;
    i=scsRoundTo32bit(sctypSizeOf(&sym->type));
    if (prev_sm)
    {if ((prev_sm->adr.flags&adrPOINTER)==adrBP)
        {
          serr2(scErr_Declaration,"Symbol `%s' already defined",prev_sm->name);
        }
    }
    if (sym->type.flags&typSTATIC)
    {//static
     sym->adr.address=act.dataused;act.dataused+=sctypSizeOf(&sym->type);
     sym->adr.flags=adrDS+adrIMMIDIATE;
     scvalSetINT(&val,0);
    }
    else
    {//automatic
     act.afterbp-=i;
     sym->adr.address=act.afterbp;
     sym->adr.flags=adrBP+adrIMMIDIATE;
     deb1("At BP+%d\n",act.afterbp);
     scvalSetINT(&val,0);
    }
   } else
   {//Align_Data();
    if (sym->type.flags&typSTATIC)
        serr2(scErr_Declaration,"Global symbol `%s' declared as static",sym->name);
    act.dataused=scsRoundTo32bit(act.dataused);
    sym->adr.address=act.dataused;act.dataused+=sctypSizeOf(&sym->type);
    deb1("sctypSizeOf:%d\n",sctypSizeOf(&sym->type));
    sym->adr.flags=adrDS+adrIMMIDIATE;
    scvalSetINT(&val,0);
    if (prev_sm)
       serr2(scErr_Declaration,"Symbol `%s' already defined",prev_sm->name);
   }
 } else
 {//function
    sym->adr.address=-1;//none
    sym->adr.flags=adrCS|adrIMMIDIATE;
 }

 Advance;

 if (lexeq("="))
 {
   if (deli && sym->type.flags&typSTATIC)
        sciReadInitialization(NULL,sym);
   else
        sciReadInitialization(deli,sym);
 }
   else
 if (lexeq("{"))
 {
   scsReadFunctionBody(deli,sym,previous_address,in_struct);
 }
 else
 {
  if (sctypIsReference(&sym->type))
    serr(scErr_Declaration,"reference without initializer");
  if (deli&&!(sym->type.flags&typSTATIC))
   {//stack
    _value vt1,vt2;
    int i=sctypSizeOf(&sym->type);

    if (i*4>act.stacksize) act.stacksize=i*4;//adjust stack size
    if (i==4)
    Gen_Opcode(opvcpuPUSH,&val,NULL);
    else
    Gen_Opcode(opvcpuSUB,scvalSetRegister(&vt1,valINT,regSP),scvalSetINT(&vt2,i));
   }
  if (lexeq(";")||lexeq(","))
  {if (sctypGetValType(&sym->type)==valSTRUCT)
   {
   scSymbol *_this;
  _this=sctypGetStruct(&sym->type);
  //TODO:static!
   scAssert(sciReadDefinition,_this);
     {
   //  int regs[240];
     _value thisval,val;
     _value *params=scAlloc(sizeof(_value));
     scSymbol *fsym;
     //final-no parameters
     params->adr.flags=adrIMPORTED&adrTYPEONLY;
     params->type.flags=typINVALID;
     scvalSetSymbol(&thisval,sym);
     val=thisval;
   //  scvalPushUsedRegisters(regs);
   //  params=scvalReadParameters();
     fsym=scsFind_Function(_this->name,_this->type.main,NULL,params,NULL);
     if (fsym)
     {
          int Static_Start=0;
          if (sym->type.flags&typSTATIC)
          {
             _value vt1;
             Static_Start=act.ip;
             Gen_Opcode(opvcpuJMP,scvalSetINT(&vt1,0),NULL);//omit global initialization
             sciAdjustScriptConstructor(1);
          }
          deb0("Call constructor()\n");
          scvalCallFunction(fsym,&val,sym,&thisval,NULL,params,false);

          if (Static_Start)
          {
             _value vt1;
             int oip;
             sciAdjustScriptConstructor(0);
             oip=act.ip;
             act.ip=Static_Start;
             Gen_Opcode(opvcpuJMP,scvalSetINT(&vt1,oip),NULL);//complete omit
             act.ip=oip;
          }
     }
     else
          scFree(params);
   //  scvalPopUsedRegisters(regs);
    }
   }
  }
  else
  {if (lexeq("(")&&sctypGetValType(&sym->type)==valSTRUCT)
   {
   scSymbol *_this;
   deb0("Call constructor(...)\n");
  _this=sctypGetStruct(&sym->type);
   scAssert(sciReadDefinition,_this);
     {
//     int regs[240];
     _value thisval,val;
     _value *params=scAlloc(sizeof(_value));
     scSymbol *fsym;
     //final-no parameters
//     params->adr.flags=adrIMPORTED&adrTYPEONLY;
//     params->type.flags=typINVALID;
     deb0("Found constructor!\n");
     scvalSetSymbol(&thisval,sym);
     val=thisval;
//     scvalPushUsedRegisters(regs);
//     Advance;scget_back(ST);
     params=scvalReadParameters();
     Advance;
     fsym=scsFind_Function(_this->name,_this->type.main,NULL,params,"Constructor `%s' couldn't be found.%s");
     if (!fsym)
         serr(scErr_Operand,"Unable to find suitable constructor for initialization");
     scvalCallFunction(fsym,&val,sym,&thisval,NULL,params,false);
//     scvalPopUsedRegisters(regs);
    }
   }
   else
   serr2(scErr_Parse, parse_error,ST->lexema);
  }
 }


 first=0;
// deb1("Next=`%s'",ST->lexema);
 if (!lexeq(",") && !lexeq(";") && !lexeq("}"))
   serr2(scErr_Parse, parse_error,ST->lexema);
 }while(!(lexeq(";")||lexeq("}")));

 return;


}

#define ___do_switch_MAX_CASE 1000

///////////////////////////////////////////////////////////////////////////
////do_switch//////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void do_switch(scSymbol* deli)
   {
    _value val,vt1,vt2;
    int ip_on_start;
    int ip_table[___do_switch_MAX_CASE];
    _value ip_value[___do_switch_MAX_CASE];
    int count=0;
    int defaulted=0;
    RegsTable rtab;
    scPosition pos;

    Advance;
    if (!deli) serr2(scErr_Parse, parse_error,ST->lexema);
      Advance;
      if (!lexeq("(")) serr2(scErr_Parse,  "`%s' instead of `(' required after switch",ST->lexema);
      scsReadExp(&val,true);
      scvalPutToRegister(&val,FALSE);
      if (!lexeq(")")) serr(scErr_Parse,  "`)' missing");
      Advance;
      memcpy(rtab,act.registers,sizeof(rtab));
      if (!lexeq("{")) serr(scErr_Parse,  "`{' missing");
      deb0("\n<SWITCH:start>\n");
      scGetPosition(ST,&pos);
      ip_on_start=act.ip;act.unreal++;
      while (!lexeq("}"))
      {
       Advance;
       deb1("\n>%s   //SWITCH:1st pass\n",STALine);
       if (lexeq("case"))
        { int loop=1;
          act.ip=ip_on_start;
          //scvalPutToRegister(&val,FALSE);//restore used register
          memcpy(act.registers,rtab,sizeof(rtab));
          scsReadExp_PreserveRegs(&vt1,true);
//          if (!valIsANumber(&val)) serr( "non-constant in case");
          scvalPutToRegister(&vt1,TRUE);
          Gen_Opcode(opvcpuCMPE,&vt1,&val);
          ip_table[count]=act.ip;
          scvalConvToInt(&vt1);//HERE

          ip_value[count]=vt1;
          Gen_Opcode(opvcpuJTRUE,&vt1,scvalSetINT(&vt2,0));
          count++;
          ip_on_start=act.ip;
          if (count>=___do_switch_MAX_CASE)
             serr2( scErr_Internal, "Too many cases in switch (max:%d)",___do_switch_MAX_CASE);
          if (!lexeq(":")) serr(scErr_Parse,  "`:' missing");
          do{
          Advance;scget_back(ST);
          if (lexeq("}"))
            {loop=0;Advance;
            }
              else
          if (lexeq("break"))
            {Advance;Advance;
             if (!lexeq(";")) serr(scErr_Parse,  "`;' missing");
             loop=0;
            }
              else
            if (lexeq("case")||lexeq("default"))
            {loop=0;
            }
              else
            {scsReadOneStatement(deli);}
          } while (loop);
        }
       else
       if (lexeq("default"))
        {int loop=1;
         defaulted=1;
         Advance;
         if (!lexeq(":")) serr(scErr_Parse,  "`:' missing");
          do{
          Advance;scget_back(ST);
          if (lexeq("}"))
            {loop=0;Advance;
            }
              else
          if (lexeq("break"))
            {Advance;Advance;
             if (!lexeq(";")) serr(scErr_Parse,  "`;' missing");
             loop=0;
            }
              else
            if (lexeq("case")||lexeq("default"))
            {loop=0;
             serr(scErr_Parse,  "default expected to be last in switch");
            }
              else
            {scsReadOneStatement(deli);}
          } while (loop);
        }
       else
       if (!lexeq("}")) serr2( scErr_Parse,parse_error,ST->lexema);
      }
//////////////////////////////////////////////////////////////////////
      act.ip=ip_on_start;act.unreal--;
      ip_table[count]=ip_on_start;
      Gen_Opcode(opvcpuJMP,scvalSetINT(&vt2,0),NULL);//end

      scSetPosition(pos);
      Advance;scget_back(ST);
      count=0;
       deb0("\n<SWITCH:2nd pass>\n");
//////////////////////////////////////////////////////////////////////
      while (!lexeq("}"))
      {
       Advance;
       if (lexeq("case"))
        { int loop=1;
          int k;
          Gen_Align();
          k=act.ip;
          scsReadExp(&vt1,true);
          act.ip=k;
//          if (!valIsANumber(&val)) serr( "non-constant in case");
          act.ip=ip_table[count];ip_table[count]=-1;
          Gen_Opcode(opvcpuJTRUE,&ip_value[count],scvalSetINT(&vt2,k));
          act.ip=k;
          count++;
          do{
          Advance;scget_back(ST);
          if (lexeq("}"))
            {loop=0;Advance;
            }
              else
          if (lexeq("break"))
            {Advance;Advance;
             if (!lexeq(";")) serr(scErr_Parse,  "`;' missing");
             loop=0;
             ip_table[count-1]=act.ip;//mark ending jump!
             Gen_Opcode(opvcpuJMP,scvalSetINT(&vt2,0),NULL);//end
            }
              else
            if (lexeq("case")||lexeq("default"))
            {loop=0;
            }
              else
            {scsReadOneStatement(deli);}
          } while (loop);
        }
       else
       if (lexeq("default"))
        {int loop=1,k;
         Advance;
         if (!lexeq(":")) serr(scErr_Parse,  "`:' missing");
          Gen_Align();
          k=act.ip;
          act.ip=ip_table[count];ip_table[count]=-1;
          Gen_Opcode(opvcpuJMP,scvalSetINT(&vt2,k),NULL);
          act.ip=k;
          do{
          Advance;scget_back(ST);
          if (lexeq("}"))
            {loop=0;Advance;
            }
              else
          if (lexeq("break"))
            {Advance;Advance;
             if (!lexeq(";")) serr(scErr_Parse,  "`;' missing");
             loop=0;
             ip_table[count-1]=act.ip;//mark ending jump!
             Gen_Opcode(opvcpuJMP,scvalSetINT(&vt2,0),NULL);//end
            }
              else
            if (lexeq("case")||lexeq("default"))
            {serr(scErr_Internal, "Internal compiler error (switch/default)");
            }
              else
            {scsReadOneStatement(deli);}
          } while (loop);
        }
       else
       if (!lexeq("}")) serr2(scErr_Parse,  parse_error,ST->lexema);
      }
//////////////////////////////////////////////////////////////////////
      Gen_Align();
      ip_on_start=act.ip;
      deb0("\n<SWITCH:jump list>\n");
      for(;count>=0;count--)
       if (ip_table[count]>-1)
       {act.ip=ip_table[count];
        Gen_Opcode(opvcpuJMP,scvalSetINT(&vt2,ip_on_start),NULL);//end
       }
      act.ip=ip_on_start;
      Advance;
      deb0("\n<SWITCH:End>\n");
      scget_back(ST);

      }


///////////////////////////////////////////////////////////////////////////
////scsReadOneStatement////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static void scsReadOneStatement(scSymbol* deli)
{
  static int lpro=-1;
  scSymbol* sym;
  int previousIp=act.ip;
#ifdef DEBUG
  static char *last_line=NULL;
#endif
  if (scProgressCallback)
    {int l=100*ST->act_line/ST->num_lines;
     if (l!=lpro)
        scProgressCallback(lpro=l);
    }

  Advance;
  {//debprintf("%s_",ST->lexema);
#ifdef DEBUG
#if DEBUGLEVEL<5
   if (STALine!=last_line) adeb2("\n>%4d:%s\n",ST->act_line,STALine);
//   debif (STALine!=last_line) debprintf("\n>%s\n",STALine);
   last_line=STALine;
#endif
#endif
   scget_back(ST);
 while (lexeq("#")) {
                  Advance;Advance;
                  if (!lexeq("!")) serr(scErr_Internal, "Internal compiler error");
                  Advance;
                  if (ST->whitespaces&&(ST->lexema[1]||!(IsSymbol(ST->lexema[0]))))
                   {//deb0("# line numbering directive\n");
                    ST->linedif=atoi(ST->lexema)-ST->act_line-1;
                    Advance;
                    if (lexeq("\"!!_internal_Script\""))
                    strcpy(ST->name,scScript_Title); else
                    strcpy(ST->name,ST->lexema);
                    Advance;
                    scget_back(ST);
#if DEBUGLEVEL<5
                    adeb2("\n>%4d:%s\n",ST->act_line,STALine);
#endif
                   }
                  else
                   {char *x;
                    deb2("skipping #%s.... at line %d\n",ST->lexema,ST->act_line);
                    if (ST->act_line)
                       serr2(scErr_Parse, parse_error,ST->lexema);
                    //skip rest of the line - for #!/bin/sh
                    x=scget_line(ST,0);
                    scFree(x);Advance;scget_back(ST);
                   }
                  }
 if (lexeq(";")) {Advance; return;}
 if (lexeq("}"))
   {
//     debprintf("%d,EXITING!\n",ST->lexstart);
//     deb("Leaving substatement {}\n");
     if (!deli)
     serr(scErr_Parse, "parse:`}' without `{'");
//     scget_lexema(ST);
     Advance;
     return;
   }else

 if (lexeq("{"))
   {
    scSymbol* prev_symbols;
    int old_bp=act.afterbp;
    if (!deli) {Advance;serr2(scErr_Parse, parse_error,ST->lexema);}
//    debprintf("ENTERING!!!\n");
    Advance;
    prev_symbols=scdicStoreDictionary(SC_symbols);
    scsReadStatement(prev_symbols);
    _fdFS=0;
    //here will have to deallocate variables we want to xr_free
    scdicRestoreDictionary(SC_symbols,prev_symbols,scsymCountAndDestruct);
    if (_fdFS)
     {_value v1,v2;
      Gen_Opcode(opvcpuADD,scvalSetRegister(&v1,valINT,regSP),scvalSetINT(&v2,_fdFS));
     }
    if (old_bp!=act.afterbp+_fdFS)
    {//deb3("\n!!!WARNING:after {} block afterbp=%d,before=%d,SP change=%d\n\n",act.afterbp,old_bp,_fdFS);
    }
//    act.afterbp=old_bp;
    act.afterbp+=_fdFS;
   }
   else

//**************************************************keywords
 if (lexeq("__vcpu"))
   {Advance;
    Advance;
    if (!lexeq("(")) serr2( scErr_Parse,parse_error,ST->lexema);
    Advance;scget_back(ST);
    act.code.pos=act.ip;
    while(!lexeq(")"))
    {_value val;
    scsReadExp(&val,true);
    if (!scvalIsImmidiateOfType(&val,valINT))
      serr( scErr_Parse,"__vcpu requires constants as parameters");
//    Gen_Opcode(val.val,NULL,NULL);
    push_pmem(&act.code,1,&val.adr.val.ival);
    act.ip=act.code.pos;
    if (lexeq(",")) {Advance;}
    }
    Advance;
    if (!lexeq(";")) serr2(scErr_Parse, parse_error,ST->lexema);
    Gen_Align();
   }
   else
 if (lexeq("secured"))
   {
    Gen_Opcode(opvcpuCLI,NULL,NULL);
    Advance;
    Advance;
    scget_back(ST);
    scsReadOneStatement(deli);
    Advance;
    scget_back(ST);
    Gen_Opcode(opvcpuSTI,NULL,NULL);
   }
   else
 if (lexeq("fork"))
   {int oip,tip;
    _value val,vt1,vt2;
    Advance;
    Advance;
    scvalSetINT(&vt1,0);
    if (lexeq("("))
     {
      scsReadExp(&val,true);
      if (!(val.adr.flags&adrPOINTER))
         serr(scErr_Operand, "Invalid lvalue in assignment");
      if (scvalGetValType(&val)!=valINT)
         serr(scErr_Operand, "int variable required in fork");
      if (!lexeq(")"))
         serr2(scErr_Parse, parse_error,ST->lexema);
      Advance;
      oip=act.ip;
      Gen_Opcode(opvcpuFORK,&val,&vt1);
     }
     else//don't output the number of forked
     {
      oip=act.ip;
      scvalSetINT(&val,0);
      val.adr.address=0;
      val.adr.flags=adrREGISTER;
      Gen_Opcode(opvcpuFORK,&val,&vt1);
     }
    scget_back(ST);
    scsReadOneStatement(deli);
    Advance;
    scget_back(ST);
    scvalSetINT(&vt1,-1);
    scvalSetINT(&vt2,-1);
    Gen_Opcode(opvcpuFORK,&vt2,&vt1);
    tip=act.ip;act.ip=oip;scvalSetINT(&vt1,tip);
    Gen_Opcode(opvcpuFORK,&val,&vt1);
    act.ip=tip;
   }
   else
 if (lexeq("import"))
   { //scPosition pos;
    int imp=0;
    char *iname=NULL;
    if (deli)
       serr(scErr_Declaration, "Illegal import declaration inside a function");
    imp=sciReadImport(&iname);

    if (!(sym=sctypGetDefinition()))
      {
       serr2(scErr_Declaration, "Invalid import declaration at `%s'",ST->lexema);
      }

    if (iname) sym->import_name=iname;

    Advance;
    if (!lexeq(";"))
       {scget_back(ST);serr(scErr_Declaration, "`;' expected after import declaration");}

    sym->adr.flags=adrIMPORTED+adrCPU;
    sym->adr.val.ival=imp;
    sym->params=0;
//    sym->flags^=sym->flags&valBP;//valCPU
    if (!sym->name)
     serr( scErr_Declaration,"Declaration of anonymous variable");
    if (!sciValidIdentifier(sym->name))
     serr2( scErr_Declaration,"Invalid identifier `%s' in declaration",sym->name);

    if (sctypIsFunction(&sym->type)){
        scSymbol *sm2=NULL;
//        deb0("Counting size of parameters:");//in_struct=NULL,sure?
        sciCountFunctionParams(sym,NULL,0);
//        deb0("end.\n");
        if ((sm2=scdicFoundSymbol(SC_symbols->first,sym->name)))
         {//function had a prototype

          if (sm2->adr.flags&adrIMPORTED)
           serr2(scErr_Parse,"Function `%s' previously declared as imported",sm2->name);

          if (sm2->adr.address!=-1)
           serr2(scErr_Parse,"Symbol `%s' already defined",sm2->name);
          //erase sym:
          scdicRemoveSymbol(SC_symbols,sm2->name);
          scdicFreeFunctionSymbol(sm2);
         }
        }
    sym->adr.address=-1;
//    sciAddImportedFunction(sym,NULL);//it is done while first call to function
    scdicAddSymbol_Ex(SC_symbols,sym);
   }
   else
 if (lexeq("export"))
   { //scPosition pos;
     //pos=ST->lastpos;
     char name[1024];
     Advance;
     Advance;
     if (!sciValidIdentifier(ST->lexema))
       serr2(scErr_Declaration, "Invalid identifier `%s' in declaration",ST->lexema);
     strcpy(name,ST->lexema);
     Advance;
     while (lexeq(".")||lexeq("::"))
      {
       strcpy(name+strlen(name),".");
       Advance;
       if (!sciValidIdentifier(ST->lexema))
         serr2(scErr_Declaration, "Invalid identifier `%s' in declaration",ST->lexema);
       strcpy(name+strlen(name),ST->lexema);
       Advance;
      }
     if (!scdicFoundSymbol(SC_exports->first,name))
     {deb1("Exporting symbol %s\n",name);
      scdicAddSymbol(SC_exports,name,0,0,NULL);
     }
     if (!lexeq(";")) serr2( scErr_Parse,parse_error,ST->lexema);
   }
   else
 if (lexeq("typedef"))
   {//structure def : typedef int fixed
    Advance;
    Advance;
    scget_back(ST);
    if (!(sym=sctypGetDefinition()))
      {

       serr(scErr_Declaration, "not a type in type definition");
      }
    if ((sym->type.flags&typTYPE)==typSTRUCT)
    {scFree(sym);deb0("Defining struct.\n");}
    else
    {
     if (!sym->name)
         serr(scErr_Declaration, "type name not specified in type definition");
     if (!sciValidIdentifier(sym->name))
         serr2(scErr_Declaration, "invalid identifier `%s' in declaration",sym->name);
     if (scdicFoundSymbol(SC_types->first,sym->name))
      serr2( scErr_Declaration,"duplicate definition `%s'",sym->name);
     scdicAddSymbol_Ex(SC_types,sym);
     Advance;
     if (!lexeq(";")) serr2( scErr_Parse,parse_error,ST->lexema);
     if (sym->type.params)
     {deb2("Defining type '%s' as %s\n",sym->name,sym->type.params);}
     else
     {deb1("Defining type '%s'\n",sym->name);}
    }
   }else


 if (lexeq("return"))
   {_value val,vt1;
    int ret_struct=0;
    if (!deli) serr(scErr_Parse,  "return in not a function");
    Advance;

    if (sctypGetValType(&function->type)!=valVOID)
    {
    scsReadExp(&val,true);
    //TODO:convert to required return type
    if (sctypIsReference(&function->type))
    {scvalGetAddress(&val,2);}
    else
    {
    if (sctypGetValType(&function->type)==valSTRUCT)
    {
     ret_struct=sctypSizeOf(&function->type.main->type);
    }
//    scAssert(scsReadOneStatement,deli!=function);
    scvalCast(&val,function);
    }
    if (ret_struct)
    {_value vt2;
    deb0("Returning struct\n");
    Gen_Opcode(opvcpuMOV,scvalSetRegister(&vt1,valINT,regCX),scvalSetINT(&vt2,ret_struct));
    vt2.adr=this_symbol.adr;//the ret_struct is [THIS-4]
    vt2.adr.address-=4;//see sciCountFunctionParams
    {_value vt1;
     vt1=vt2;
     vt1.adr.address=scvalFindFreeRegister(&vt1);
     vt1.adr.flags=adrREGISTER;
     Gen_Opcode(opvcpuMOV,&vt1,&vt2);
     scvalFreeValue(&vt2);
     vt2=vt1;
     vt2.adr.flags|=adrCPU;
    }
    Gen_Opcode(opvcpuCOPY,&vt2,&val);
    }
    else
    if (!(val.adr.flags&adrREGISTER&&val.adr.address==0&&!(val.adr.flags&adrPOINTER)))//Output via a0
    {
    scvalSetRegister(&vt1,valINT,0);
    vt1.sym=function;//TODO:deli;
    Gen_Opcode(opvcpuMOV,&vt1,&val);
    }
    } else
    {Advance;
     if (!lexeq(";")) serr2(scErr_Parse, parse_error,ST->lexema);
    }
    deb0("+++\nLocal symbols:\n");
    {scSymbol* p=SC_symbols->first;
     while (p!=global_symbols)
     {
      scsymCountAndDestruct(p);
      p=p->next;
     }
    }
    Gen_Opcode(opvcpuLEAVE,NULL,NULL);
    Gen_Opcode(opvcpuRET,NULL,NULL);
   }else

//**************************************************
 if (lexeq("if"))
   {
    _value val,vt1;
    int oip,tip;
    if (!deli) {Advance;serr2( scErr_Parse,parse_error,ST->lexema);}
      Advance;
      Advance;
      if (!lexeq("(")) serr2( scErr_Parse,"`%s' instead of `(' required after if",ST->lexema);
      scsReadExp(&val,true);
      if (!lexeq(")")) serr( scErr_Parse,"`)' missing");
      scvalSetINT(&vt1,0);
      scvalConvToInt(&val);//HERE
      scvalPutToRegister(&val,TRUE);
      oip=act.ip;
      Gen_Opcode(opvcpuJFALSE,&val,&vt1);
      Advance;
      scget_back(ST);
      scsReadOneStatement(deli);
      Advance;
      scget_back(ST);
      if (lexeq("else"))
        {int _oip;
         _oip=act.ip;

         Gen_Opcode(opvcpuJMP,&vt1,NULL);
         tip=act.ip;act.ip=oip;scvalSetINT(&vt1,tip);
         Gen_Opcode(opvcpuJFALSE,&val,&vt1);
         act.ip=tip;
         Advance;
         Advance;
         scget_back(ST);
         scsReadOneStatement(deli);
         Gen_Align();
         tip=act.ip;act.ip=_oip;scvalSetINT(&vt1,tip);
         Gen_Opcode(opvcpuJMP,&vt1,NULL);
         act.ip=tip;
       }//else
       else
       {Gen_Align();
        tip=act.ip;act.ip=oip;
        scvalSetINT(&vt1,tip);
        Gen_Opcode(opvcpuJFALSE,&val,&vt1);
        act.ip=tip;
       }
      }else
//**************************************************
 if (lexeq("switch")) do_switch(deli);
 else

//**************************************************
 if (lexeq("for"))
   {
    _value val2,val3,vt1;
    scPosition pos,pos2;
    int oip,tip,sip;
    if (!deli) {Advance;serr2( scErr_Parse,parse_error,ST->lexema);}
      Advance;
      Advance;
      if (!lexeq("(")) serr2(scErr_Parse,  "`%s' instead of `(' required after for",ST->lexema);
      scsReadExp(&vt1,false);
      if (!lexeq(";")) serr2(scErr_Parse,  parse_error,ST->lexema);
      Gen_Align();
      oip=act.ip;
      val2.type.flags=typINVALID;
      scsReadExp(&val2,false);
      if (val2.type.flags==typINVALID) scvalSetINT(&val2,1);//always for(..;;..)
      scvalConvToInt(&val2);//HERE

      if (!lexeq(";")) serr2(scErr_Parse,  parse_error,ST->lexema);
      scGetPosition(ST,&pos);
      tip=act.ip;act.unreal++;
      scsReadExp(&val3,false);
      act.unreal--;
      act.ip=tip;
      if (!lexeq(")")) serr(scErr_Parse,  "`)' missing");
      scvalSetINT(&vt1,0);
      sip=act.ip;
      Gen_Opcode(opvcpuJFALSE,&val2,&vt1);
      Advance;
      scget_back(ST);
      scsReadOneStatement(deli);
      scGetPosition(ST,&pos2);
      scSetPosition(pos);
      scsReadExp(&val3,false);
      scvalSetINT(&vt1,oip);
      Gen_Opcode(opvcpuJMP,&vt1,NULL);
      scvalSetINT(&vt1,act.ip);
      tip=act.ip;act.ip=sip;
      Gen_Opcode(opvcpuJFALSE,&val2,&vt1);
      act.ip=tip;
      scSetPosition(pos2);
      Advance;
      scget_back(ST);
   }else

//**************************************************
 if (lexeq("while"))
   {
    _value val,vt1;
    int oip,tip,sip;
    if (!deli) {Advance;serr2(scErr_Parse,  parse_error,ST->lexema);}
      Advance;
      Advance;
      if (!lexeq("(")) serr2(scErr_Parse,  "`%s' instead of `(' required after while",ST->lexema);
      Gen_Align();
      sip=act.ip;
      scsReadExp(&val,true);
      if (!lexeq(")")) serr(scErr_Parse,  "`)' missing");
      scvalSetINT(&vt1,0);
      scvalConvToInt(&val);//HERE
      scvalPutToRegister(&val,TRUE);
      oip=act.ip;
      Gen_Align();
      Gen_Opcode(opvcpuJFALSE,&val,&vt1);
      Advance;
      scget_back(ST);
      scsReadOneStatement(deli);
      scvalSetINT(&vt1,sip);
      Gen_Opcode(opvcpuJMP,&vt1,NULL);
      Advance;
      scget_back(ST);
      tip=act.ip;act.ip=oip;
      scvalSetINT(&vt1,tip);
      Gen_Opcode(opvcpuJFALSE,&val,&vt1);
      act.ip=tip;
   }else

//**************************************************
 if (lexeq("do"))
   {
    _value val,vt1;
    int sip;
    if (!deli) {Advance;serr2(scErr_Parse,  parse_error,ST->lexema);}
      Advance;
      Gen_Align();
      sip=act.ip;
      scsReadOneStatement(deli);
      Advance;
      if (!lexeq("while")) serr2(scErr_Parse,  "while expected instead of '%s'",ST->lexema);
      Advance;
      if (!lexeq("(")) serr2(scErr_Parse,  "`%s' instead of `(' required after while",ST->lexema);
      scsReadExp(&val,true);
      scvalPutToRegister(&val,TRUE);
      if (!lexeq(")")) serr(scErr_Parse,  "`)' missing");
      Advance;
      if (!lexeq(";")) serr2(scErr_Parse,  parse_error,ST->lexema);
      scvalSetINT(&vt1,sip);
      scvalConvToInt(&vt1);//HERE

      Gen_Opcode(opvcpuJTRUE,&val,&vt1);
   }else

//*****************symbols means an expression - compiler speed optimization:
 if (IsSymbol(ST->lexema[0]))
   {
    _value val;
    if (!deli) {Advance;serr2(scErr_Parse,  parse_error,ST->lexema);}
    scsReadExp(&val,false);//NEW:from true ?TODO
   }else


//********************declaration of a variable or function

 if ((sym=sctypGetDefinition()))
   {
//    if (deli) deb1("deli=%d\n",sctypGetValType(&deli->type));
    sciReadDefinition(deli,sym,NULL);
//    if (deli) deb1("deli=%d\n",sctypGetValType(&deli->type));
   }
   else
//**************************************************
   {
    _value val;

   if (!deli) {Advance;serr2(scErr_Parse,  parse_error,ST->lexema);}
//   debprintf("Reading expression!\n");
   scsReadExp(&val,true);//?TODO
   }
 }//while
 if (previousIp!=act.ip)
  {
   if (act.options&optEXPR)
   Gen_Opcode(opvcpuEXPR,NULL,NULL);//separator
   if (SC_temporary->first)
   {scSymbol* p=SC_temporary->first;
    deb0("Here I delete all temporary symbols:\n");
    while (p)
    {scSymbol *q=p->next;
//     scsSymbolInfo(p);
     scsymCountAndDestruct(p);
 //    scdicFreeFunctionSymbol(p);
     scFree(p);
     p=q;
    }
    SC_temporary->first=NULL;
   }
  }
}

///////////////////////////////////////////////////////////////////////////
////scsReadStatement///////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static void scsReadStatement(scSymbol* deli)
{
  while(scget_lexema(ST))
  {

  scget_back(ST);

  if (lexeq("}"))
  {
   if (deli) {Advance;return;}
      else
     serr(scErr_Parse, "`}' without `{'");
  }
  scsReadOneStatement(deli);

  };
  if(deli) serr2(scErr_Parse,parse_error," end of file");

}

///////////////////////////////////////////////////////////////////////////
////scCompile//////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
scScript scCompile(Source_Text *ST_)
{//int i;
 int old_stack_size=act.stacksize;
 scScript code;
 if (!scScript_SeeR) serr(scErr_Internal, "internal compiler error");

 act.lines_compiled=0;
 ST=ST_;
 {int i;
/* for(i=0;i<=ST->num_lines;i++)
 {printf(">%s\n",ST->lines[i]);
 }*/
 act.during=duringPREPROCESSING;
 ST=scPreprocessor(ST);
 act.during=duringCOMPILING;
 act.lines_compiled=0;
 act.lines_compiled=ST->num_lines;


// getchar();
// debprintf("Preprocessed:\n");
 for(i=0;i<=ST->num_lines;i++)
 {deb2("%2d>%s\n",i+1,ST->lines[i]);
 }
// getchar();
 }
// debprintf("Compiling:\n");

//***********The Compilation begins*****************************
 {

  SC_types=scdicNewDictionary();
  SC_symbols=scdicNewDictionary();
  SC_exports=scdicNewDictionary();
  SC_temporary=scdicNewDictionary();


  act.dataused=4;//first 4 bytes for this!
  act.ip=0;//processor pos, after symbols data - here it should be known
  act.lastInitIp=act.lastFuncIp=0;
  act.unreal=0;
  //act.stacksize=4000;
  new_pmem(&act.code,4090);
  new_pmem(&act.inits,1023);
  new_pmem(&act.consts,1023);
  new_pmem(&act.imports,4090);
  ST->act_char=0;ST->act_line=0;
  ST->newline=-1;
  Gen_Opcode(opvcpuENTER,NULL,NULL);
  Gen_Align();
//COMPILATION
  scsReadStatement(NULL);

//The script constructor
  if (!act.lastInitIp)
  {//there was no init
   int oip=act.ip;
   act.ip=0;
#if DEBUGLEVEL<5
   adeb0("No script constructor needed.\n");
#endif
   Gen_Opcode(opvcpuRET,NULL,NULL);//change enter to ret
   Gen_Align();
   act.code.pos=act.ip=oip;
  }
  else
  {
   int oip=act.ip;
   act.ip=act.lastInitIp;
#if DEBUGLEVEL<5
   adeb0("Adjusting script constructor.\n");
#endif
   Gen_Opcode(opvcpuLEAVE,NULL,NULL);//change jmp 0 to leave,ret
   Gen_Opcode(opvcpuRET,NULL,NULL);
   Gen_Align();
   act.code.pos=act.ip=oip;
  }

//END
//#ifdef DEBUG

  {scSymbol* p=SC_types->first,*q;
   deb0("*************************\nGlobal SC_types:\n");
   while (p)
   {if (p->type.params)
    {deb2(" %8s : `%s'\n",p->name,p->type.params);}
     else
    {deb1(" %8s :\n",p->name);}
    if (sctypGetValType(&p->type)==valSTRUCT)
       {
        scsCheckFunctionDefinitions(p->name,p->type.main);
       }

    q=p;
    p=p->next;
//    scdicFreeFunctionSymbol(q);
   }
  }

//MAKING FINALs************************************************
  new_pmem(&act.final,4090);//HERE
//HEADer
  {int i;

   //header
   i=CHARS2INT('S','e','e','R');push_pmem(&act.final,4,&i);//0
   i=CHARS2INT('V','C','P','U');push_pmem(&act.final,4,&i);//1 int
   i=((SeeR_VERSION/100)<<16)+SeeR_VERSION%100;
       push_pmem(&act.final,4,&i);   //version,2
   i=0;push_pmem(&act.final,4,&i);   //hdrAllSize,3
   i=hdrHeaderSizeOf;push_pmem(&act.final,4,&i);   //hdrHeaderSize,4
   i=0;push_pmem(&act.final,4,&i);   //hdrSymbolOffset - in code,5
   i=0;push_pmem(&act.final,4,&i);   //hdrCodeOffset - in code,6
   i=0;push_pmem(&act.final,4,&i);   //hdrDataOffset - in data,7, NOT USED!
   i=0;push_pmem(&act.final,4,&i);   //hdrConstsOffset,8
   i=0;push_pmem(&act.final,4,&i);   //hdrInitOffset,9
   i=0;push_pmem(&act.final,4,&i);   //hdrCodeSize,10
   i=0;push_pmem(&act.final,4,&i);   //hdrDataSize,11
   i=0;push_pmem(&act.final,4,&i);   //hdrConstsSize,12
   i=act.stacksize;push_pmem(&act.final,4,&i);   //hdrStackSize,13
   act.stacksize=old_stack_size;
   i=0;push_pmem(&act.final,4,&i);   //hdrImportedOffset,14
   i=0;push_pmem(&act.final,4,&i);   //hdrConstructOffset - in code,15
   i=act.code.pos;push_pmem(&act.final,4,&i);   //hdrDestructOffset - in code,16
   i=CHARS2INT('E','O','H','!');push_pmem(&act.final,4,&i);//EOH!,17

   //custom features:
   align_pmem(&act.final);
   i=CHARS2INT('T','I','T','L');push_pmem(&act.final,4,&i);
   i=strlen(scScript_Title)+1;ALIGN_INT(i);push_pmem(&act.final,4,&i);
   pushstr_pmem(&act.final,scScript_Title);//title
   align_pmem(&act.final);
   i=CHARS2INT('A','U','T','H');push_pmem(&act.final,4,&i);
   i=strlen(scScript_Author)+1;ALIGN_INT(i);push_pmem(&act.final,4,&i);
   pushstr_pmem(&act.final,scScript_Author);//author
   align_pmem(&act.final);
   i=CHARS2INT('E','N','D','!');push_pmem(&act.final,4,&i);
 }
  //Checking symbols
   deb0("\nChecking Symbols...\n");
   scsCheckFunctionDefinitions(NULL,SC_symbols->first);
//   pushstr_pmem(&act.final,"");//the end of exports

  //Global symbols-count data size, call destructors.
  {scSymbol* p=SC_symbols->first;
   deb0("\nGlobal Symbols (calling destructors to some):\n");
   //start destructor function
   Gen_Opcode(opvcpuENTER,NULL,NULL);

   while (p)
   {scSymbol *q=p->next;
//    scsSymbolInfo(p);
   //TODO:call destructor
   //calculate size of DS
   if (((p->adr.flags&adrPOINTER)==adrDS)//global variable in DS
       && ((int*)act.final.mem)[hdrDataSize]<p->adr.address+sctypSizeOf(&p->type))
      ((int*)act.final.mem)[hdrDataSize]=p->adr.address+sctypSizeOf(&p->type);
//    scdicFreeFunctionSymbol(p);
    scsymCountAndDestruct(p);//call destructor
    p=q;
   }
   //end destructor function
   Gen_Opcode(opvcpuLEAVE,NULL,NULL);
   Gen_Opcode(opvcpuRET,NULL,NULL);
   Gen_Align();
  }

  //Exports
  align_pmem(&act.final);
  {scSymbol* p=SC_exports->first,*w;
   deb0("\nExported Symbols:\n");
   ((int*)act.final.mem)[hdrSymbolOffset]=act.final.pos;
   while(p)
    {scSymbol *q;
     w=p->next;
     if (!(q=scdicFoundSymbol(SC_symbols->first,p->name)))
      {//possibly a member
       scSymbol *d=SC_types->first;
       char *nam=p->name;
       do{
       char *dot=strstr(nam,".");
       if (!dot) break;
       *dot=0;
       if (!(d=scdicFoundSymbol(d,nam)))
        serr2(scErr_Declaration,"Structure %s does not exist!",p->name);
       *dot='.';
       if (d->type.flags!=typSTRUCT)
        serr2(scErr_Declaration,"%s is not a structure!",p->name);
       nam=dot+1;
       } while (1);
       if (!d||!(d=scdicFoundSymbol(d->type.main,nam)))
        serr2(scErr_Declaration,"Unable to export %s!",p->name);
       q=d;
      }
      //do the export
      {int flags=0;
       deb2("\n-->%s at %d\n",p->name,q->adr.address);
       pushstr_pmem(&act.final,p->name);
       align_pmem(&act.final);
       push_pmem(&act.final,4,&q->adr.address);
       if (sctypIsFunction(&q->type))
            flags=q->params;//variable! or function w/o params
       else
            flags=-1;
       push_pmem(&act.final,4,&flags);
      }
     p=w;
    }
   pushstr_pmem(&act.final,"");//the end of exports
  }

  //Copy imports to final buffer:
  pushstr_pmem(&act.imports,"");//the end of imports
  align_pmem(&act.final);
  ((int*)act.final.mem)[hdrImportedOffset]=act.final.pos;
  push_pmem(&act.final,act.imports.pos,act.imports.mem);

  //scdicDeleteDictionary(exports);
  //scdicDeleteDictionary(SC_types);
  //scdicDeleteDictionary(symbols);

//#ifdef DEBUG
  deb0("End.\n");
  deb2("Code size:%d\nInits size:%d\n",act.code.pos,act.inits.pos);
  deb1("Consts size:%d\n",act.consts.pos);
  deb1("Data size:%d\n",((int*)act.final.mem)[hdrDataSize]);
  deb1("Stack size:%d\n",((int*)act.final.mem)[hdrStackSize]);
  //header and symbols to final first ^^^

  //now code
   ((int*)act.final.mem)[hdrCodeOffset]=act.final.pos;
   ((int*)act.final.mem)[hdrCodeSize]=act.code.pos;
  push_pmem(&act.final,act.code.pos,act.code.mem);

  //now data inits
   ((int*)act.final.mem)[hdrInitOffset]=act.final.pos;
  push_pmem(&act.final,act.inits.pos,act.inits.mem);
  //end it with two 00:
  {int x=0;
  push_pmem(&act.final,4,&x);
  push_pmem(&act.final,4,&x);
  }

  //now consts!
   ((int*)act.final.mem)[hdrConstsOffset]=act.final.pos;
   ((int*)act.final.mem)[hdrConstsSize]=act.consts.pos;
  push_pmem(&act.final,act.consts.pos,act.consts.mem);
  //size
   ((int *)act.final.mem)[hdrAllSize]=act.final.pos;

  //To the script:
  code=xr_malloc(act.final.pos);
  memcpy(code,act.final.mem,act.final.pos);

  free_pmem(&act.final);
  free_pmem(&act.code);
  free_pmem(&act.inits);
  free_pmem(&act.consts);
  free_pmem(&act.imports);

 }
#ifdef DEBUG
//  fclose(debfile);
#endif
 scCleanUp();
 act.during=duringIDLE;
 return code;

 scCleanUp();
 act.during=duringIDLE;
 return NULL;
}

///////////////////////////////////////////////////////////////////////////
////scsCheckFunctionDefinitions////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static void scsCheckFunctionDefinitions(char *s,scSymbol *p)
{
   while(p)
    {
     if (sctypIsFunction(&p->type)&&p->adr.address<0&&(!(p->adr.flags&adrIMPORTED)))
     {
      if (s)
      {serr3(scErr_Declaration,"Function `%s.%s' declared and not defined!",s,p->name);}
      else
      {serr2(scErr_Declaration,"Function `%s' declared and not defined!",p->name);}
     }
     p=p->next;
    }

}
