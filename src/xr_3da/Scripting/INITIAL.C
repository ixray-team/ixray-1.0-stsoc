/************************************************************************
       -= SEER - C-Scripting engine v 0.91a =-
State      : under implementation
Author     : Przemyslaw Podsiadly
             email: ppodsiad@elka.pw.edu.pl
             WWW:   http://home.elka.pw.edu.pl/~ppodsiad
File:compiler.c
Desc:initialization routines
************************************************************************/
#define INTERNAL_FILE
#include <seer.h>
#include "internal.h"
#include "code.h"

/***********************INITIALIZATION**************************************/
static scSymbol CharAsterisk={
{0,0,{0}},NULL,NULL,0,NULL,
{typPREDEFINED,"*",(scSymbol *)valCHAR},
NULL,NULL
};

///////////////////////////////////////////////////////////////////////////
////RoundTo32bit///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static int RoundTo32bit(int a)
{
    if (a%4) a+=4-a%4;
    return a;
}

/*
-What
   Pointer
   Table
   Struct
   Variable
-How
   Variable (if !packed)
   Struct {}
   String ""
   Value number|''
1.If (How.Variable&&packed) error;
2.If (How.Variable) push(Variable.value);
3.If (What.Struct) for(each) initialize(each,packed);
4.If (if (!packed) push address;
What.Pointer:
              How.Variable+ (!packed)
              How.String  +
              How.Struct  -
              How.Value   I
What.Table:
              How.Variable- (!packed)
              How.Stuct   -
              How.String  -
              How.Value   I
*/


/*packed means also, that we're not allowed to read expressions with vars,
  only numbers*/
static pmem init_end;
static int init_start,init_pos,init_begin;


///////////////////////////////////////////////////////////////////////////
////scsReadDirectInitialization///////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//returns 0, if all was read and nothing (but push init_end) remains to be
//done, else returns number of bytes to be copied
static int scsReadDirectInitialization(scSymbol* deli,scType* type,BOOL packed,pmem* dest,int type_size,scSymbol *sym)
{
 _value val;
/* write to inits or consts*/
#if DEBUGLEVEL==0
 deb2(": %s%s",sciValTypeStr(sctypGetValType(type)),packed?" as packed":"");
 deb1(" (%db)",type_size);
 if(deli)//inside a function - initialize on the stack
  {deb0(" on the stack\n");
  }else
  {deb0(" on the heap\n");
  }
#endif
 if (type->params&&type->params[0])
 {

/*******************************************Pointer - OK*/

   if (type->params[0]=='*'/*||(type->params[0]=='['&&type->params[1]==']')*/)
   {//x* t;
    scType inner_type=*type;
    inner_type.params++;
    sctypSimplifyType(&inner_type);
#if DEBUGLEVEL==0
    deb0("* Pointer");
    //Advance;
    deb1(" %s ",ST->lexema);
#endif
    scAssert(scsReadDirectInitialization,type_size==4);
    scget_back(ST);
/*as STRING*/
    if (ST->lexema[0]=='"')
     {
      if (sctypGetValType(&inner_type)!=valCHAR)
       serr(scErr_Declaration,"Illegal array initialization with string");
      if(deli)
       {
        _value val1,val2;
        char *s=ST->lexema+1;
        s[strlen(s)-1]=0;
//        j=4;
#if DEBUGLEVEL==0
        deb1("Storing %s\n",s);
#endif
        if (sym)
        {
           init_start=act.consts.pos;//inits will be earlier by 4 bytes.
	   s=scStrdup(s);
           scvalConstantString(&val2,s);
//           pushstr_pmem(&act.consts,s);
           Gen_Opcode(opvcpuPUSH,&val2,NULL);
        }
        else
        {
           pushstr_pmem(&init_end,s);
           push_pmem(&act.consts,4,s);//copy to consts anything as pointer
           Gen_Opcode(opvcpuMOV,scvalSetRegister(&val1,valINT,0),scvalSetRegister(&val2,valINT,regES));
           Gen_Opcode(opvcpuADD,scvalSetRegister(&val1,valINT,0),scvalSetINT(&val2,init_start));
           scvalSetINT(&val1,init_pos);
           val1.adr.flags=adrBP;
           val1.adr.address=init_pos;
           Gen_Opcode(opvcpuMOV,&val1,scvalSetRegister(&val2,valINT,0));
        }

        init_start+=strlen(s)+1;
        if (sym) {Advance;Advance;return 0;}//all done
       }
       else
       {
        int x=-2,y=init_start+act.inits.pos;
        char *s=ST->lexema+1;
//        j=4;
        s[strlen(s)-1]=0;
#if DEBUGLEVEL==0
        deb1("Storing %s\n",s);
        deb3("(%d,%d,%d)",y,x,act.consts.pos);
#endif
        push_pmem(&act.inits,4,&y);//can be anything
        push_pmem(&init_end,4,&y);//destination position
        push_pmem(&init_end,4,&x);//size=-2:ES
        //copy to consts segment
        push_pmem(&init_end,4,&act.consts.pos);
#if DEBUGLEVEL==0
        deb1("after %d,",init_end.pos);
#endif
        pushstr_pmem(&act.consts,s);
       }
      Advance;
      Advance;
      return 4;
     }
     else
     if (lexeq("{"))
     {
     serr(scErr_Declaration,"Don't use {} in initialization of pointer.");
     }
     else
/*as VARIABLE*/
     {//exit to primitive read
     }
   }//Pointer*
   else

/*******************************************Table*/

   if (type->params[0]=='[')
   {//x t[xx];
    scType tp=*type;
    int tabsize,initsize=0,inner_type_size;
#if DEBUGLEVEL==0
    deb0("Table");
#endif
    tp.params++;
    tabsize=strtoul(tp.params,&tp.params,10);
    scAssert(Read_Init_Type[],*tp.params==']');
    tp.params++;
    sctypSimplifyType(&tp);
    inner_type_size=sctypSizeOf(&tp);
#if DEBUGLEVEL==0
    deb2("[%d] of %d bytes;\n",tabsize,inner_type_size);
#endif
    //Advance;
    if (ST->lexema[0]=='"')
    {char *data;
/*as STRING*/
      if (sctypGetValType(&tp)!=valCHAR)
       serr(scErr_Declaration,"Illegal array initialization with string");
#if DEBUGLEVEL==0
      deb1("char[%d] init\n",tabsize);
#endif
      data=scStrdup(ST->lexema+1);
      data[strlen(data)-1]=0;//erase "
      pushstr_pmem(dest,data);
/*      {char temp[100];
       int i=tabsize-strlen(data)-1;
       while(i)
        if (i>100)
         {push_pmem(dest,100,temp);i-=100;}
         else
         {push_pmem(dest,i,temp);i=0;}
      }*/
      initsize=strlen(data)+1;
      /*initialize all the data, if packed */
      if (packed&&initsize<type_size)
      {char c=0;
       int i;
       for(i=initsize;i<type_size;i++)
          push_pmem(dest,1,&c);
       initsize=type_size;
      }
      scFree(data);
      Advance;
      return initsize;
    }
    else
    if (lexeq("{"))
    {int i=0,j;
     for(;i<tabsize;i++)
     {Advance;
      j=scsReadDirectInitialization(deli,&tp,true,dest,inner_type_size,NULL);
      init_pos+=inner_type_size;

      scAssert(scsReadDirectInitialization[],j);//same here
      initsize+=j;
      if (i<tabsize-1)
      {
      if (!lexeq(","))
         serr2(scErr_Parse, parse_error,ST->lexema);
      }
     }
     if (lexeq(",")) {Advance;}
     if (!lexeq("}"))
         serr2(scErr_Parse, parse_error,ST->lexema);
     Advance;
     return initsize;
    }
    else//VARIABLE
    {
      serr2(scErr_Parse, parse_error,ST->lexema);
    }
   }//Table[]
 }


/*******************************************Other*/




#if DEBUGLEVEL==0
 deb0(" Primitive or struct type\n");
#endif
 switch(sctypGetValType(type))
 {
/*******************************************Struct*/
 case valSTRUCT:
      if (lexeq("{"))
      {int j,inner_size;
       scSymbol *p=type->main;

       while (p)
       {
        Advance;
        inner_size=sctypSizeOf(&p->type);
        //unpack shorts and bytes
        //FIXME: char[10] or short[10] should insist alignment
        j=init_pos-init_begin;
        {
        int t=sctypInsistsAlignment(&p->type);
        if (t&&j%t)
           {
#if DEBUGLEVEL==0
             deb1("{aligning from %d}",j);
#endif
             push_pmem(dest,t-(j%t),&j);
             init_pos+=t-(j%t);
           }
        }
        j=scsReadDirectInitialization(deli,&p->type,true,dest,inner_size,NULL);

        p=p->next;
#if DEBUGLEVEL==0
        deb2("init_pos:%d+=%d\n",init_pos,inner_size);
#endif
        //if (j)
        //scAssert(scsReadDirectInitialization,j==inner_size);

        init_pos+=inner_size;
        if (p&&!lexeq(","))
           serr2(scErr_Declaration,"`,' expected instead of `%s'",ST->lexema);
       }
       if (lexeq(",")) {Advance;}
       if (!lexeq("}"))
           serr2(scErr_Declaration,"`}' expected instead of `%s'",ST->lexema);
       //fill the rest, if needed
       if (init_pos-init_begin<type_size)
       {
        j=init_pos-init_begin;
        push_pmem(dest,type_size-j,&j);
        init_pos+=type_size-j;
       }
      Advance;
      return type_size;
      }
      else
      {//leave for default
      }
/*******************************************Primitive*/
 default:
 {_value val1,val2;
  int ip=act.ip;

  if (deli&&sym)
  {BOOL DoPush=true;

#if DEBUGLEVEL==0
   deb0("Indirect\n");
#endif
   memset(act.registers,0,sizeof(act.registers));//TODO:really here?
   sciReadSubExp(1,&val1,true);
   val2.sym=sym;
   val2.type=*type;
   val2.adr=sym->adr;
   switch ((int)scvalGetValType(&val2))
   {
    case valDOUBLE:scvalConvToDouble(&val1);break;
    case valFIXED:scvalConvToFixed(&val1);break;
    case valFLOAT:scvalConvToFloat(&val1);break;
    case valCHAR:
    case valSHORT:
    case valINT:scvalConvToInt(&val1);break;
    case valSTRUCT:
         {_value v1,v2;
          Gen_Opcode(opvcpuMOV,scvalSetRegister(&v1,valINT,regCX),scvalSetINT(&v2,type_size));
          Gen_Opcode(opvcpuSUB,scvalSetRegister(&v1,valINT,regSP),scvalSetRegister(&v2,valINT,regCX));
          Gen_Opcode(opvcpuCOPY,&val2,&val1);
          DoPush=false;
         }
         break;
    case valPOINTER:
         {
          scvalArrayToPointer(&val1);//maybe needed
          if (scvalGetValType(&val1)!=valPOINTER)
             serr(scErr_Declaration,"Illegal initializer");
         }
         break;
   }
   if (DoPush)Gen_Opcode(opvcpuPUSH,&val1,NULL);
   return 0;
  }
  sciReadSubExp(1,&val,true);
  if (ip!=act.ip&&!deli)
  {
     serr(scErr_Declaration,"Illegal initializer");
  }
  switch(sctypGetValType(type))
  {
  case valDOUBLE:scvalConvToDouble(&val);break;
  case valFIXED:scvalConvToFixed(&val);break;
  case valFLOAT:scvalConvToFloat(&val);break;
  case valCHAR:
  case valSHORT:
  case valINT:scvalConvToInt(&val);break;
  case valSTRUCT:break;//check type compliance
  case valPOINTER:
         {
          if (scvalGetValType(&val)!=valPOINTER)
             serr(scErr_Declaration,"Illegal initializer");
          break;
         }
  default:
           scError(scsReadDirectInitialization);
  }
  if (ip!=act.ip&&!deli)
     serr(scErr_Declaration,"Illegal initializer");
  if (scvalIsImmidiate(&val))
  {
     push_pmem(dest,type_size,&val.adr.val);
     return type_size;
  }
  //check if both are of the same type
  if (deli)  //STACK
  {char s[1024];
   _value val1,val2;
   valTYPE v=scvalGetValType(&val);
   if (type_size==4&&(v==valINT||v==valPOINTER||v==valFIXED||v==valFLOAT))
   {
   scvalArrayToPointer(&val);//maybe needed
   scvalSetINT(&val1,init_pos);
   val1.adr.flags=adrBP;
   val1.adr.address=init_pos;
   Gen_Opcode(opvcpuMOV,&val1,&val);
   }
   else
   {
   //mov cx,sizeof;copy [bp+pos],val;
   Gen_Opcode(opvcpuMOV,scvalSetRegister(&val1,valINT,regCX),scvalSetINT(&val2,type_size));
   scvalArrayToPointer(&val);//maybe needed
   scvalSetINT(&val1,init_pos);
   val1.adr.flags=adrBP;
   val1.adr.address=init_pos;
   Gen_Opcode(opvcpuCOPY,&val1,&val);
   }
   init_start+=type_size;
   if (sym) return 0;//all done
   push_pmem(&act.consts,type_size,s);//copy to consts anything...
   return type_size;
  }
  else
  {
   int x=-3,y=init_start+act.inits.pos;
   if (ip!=act.ip)
      serr(scErr_Declaration,"Illegal initializer");
   //initializer on DS, copy from val to [ds+init_pos]
#if DEBUGLEVEL==0
   deb1("Initialize as copy of %d bytes.\n",type_size);
#endif
   push_pmem(&act.inits,type_size,&y);//can be anything
   if ((val.adr.flags&adrPOINTER)==adrDS)
   {
#if DEBUGLEVEL==0
   deb1("       from DS+%d\n",val.adr.address);
#endif
   push_pmem(&init_end,4,&y);//destination position
   push_pmem(&init_end,4,&x);//size=-3:[DS+from]
//   scAssert(scsReadDirectInitialization,(val.adr.flags&adrPOINTER));
   push_pmem(&init_end,4,&val.adr.address);//position of val
   push_pmem(&init_end,4,&type_size);//size
   }
   else
   if (val.adr.flags&adrIMPORTED)
   {x=-4;//WARNING:untested
#if DEBUGLEVEL==0
   deb1("       from imported %d\n",val.adr.address);
#endif
   push_pmem(&init_end,4,&y);//destination position
   push_pmem(&init_end,4,&x);//size=-3:[DS+from]
   push_pmem(&init_end,4,&val.adr.address);//position of val
   push_pmem(&init_end,4,&type_size);//size
   }
   else
   {
   scError(scsReadDirectInitialization:imported);
   }
#if DEBUGLEVEL==0
   deb1("after %d,",init_end.pos);
#endif
  }
  return type_size;
 }

 }//switch

 return 0;
}





///////////////////////////////////////////////////////////////////////////
////sciReadInitialization//////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void sciReadInitialization(scSymbol* deli,scSymbol* sym)
{//_value val;
 _value val1,val2;
 int size,type_size;
 scType type=sym->type;
 sctypSimplifyType(&type);
 deb1("Initialization of %s",sym->name);
 if (sctypIsFunction(&type)) serr2(scErr_Declaration,"Declaring function `%s' with `='",sym->name);
 Advance;
 if (sctypIsReference(&sym->type))//reference
 {int ip=act.ip;
//  _value v;
#if DEBUGLEVEL==0
  deb0(" as a reference\n");
#endif
  sciReadSubExp(1,&val1,true);
  //access reference, so we can check type
//  v.type=sym->type;sctypGoDeeper(&v.type);
  //check type
  if (scvalGetValType(&val1)!=sctypGetValType(&sym->type))
       serr2(scErr_Declaration,"types do not match for initialization of reference `%s'",sym->name);
  scvalGetAddress(&val1,2);
  if (deli)//stack
  {
    Gen_Opcode(opvcpuPUSH,&val1,NULL);
  }
  else
  {//heap
   if (act.ip!=ip) //code generated!
   {
     serr2(scErr_Declaration,"invalid initialization of reference `%s'",sym->name);
   }
   //TODO:initialization code for heap version
   serr2(scErr_Declaration,"invalid initialization of reference `%s'",sym->name);
  }
  return;
 }
 deb0("\n");
 type_size=sctypSizeOf(&type);
 if (sctypGetValType(&sym->type)==valSTRUCT&&!lexeq("{"))
 {
     _value thisval,val;
     scSymbol *fsym;
     scSymbol *_this;
     _value *params=scAlloc(2*sizeof(_value));
     int Static_Start=0;
     if (sym->type.flags&typSTATIC)
     {
	     _value vt1;
	     Static_Start=act.ip;
	     Gen_Opcode(opvcpuJMP,scvalSetINT(&vt1,0),NULL);//omit global initialization
	     sciAdjustScriptConstructor(1);
//1	     serr2(scErr_Declaration,"invalid initializer for static `%s'",sym->name);
     }
     _this=sctypGetStruct(&sym->type);
     scAssert(sciReadInitialization,_this);
     sciReadSubExp(1,&params[0],true);
     params[1].adr.flags=adrIMPORTED&adrTYPEONLY;
     params[1].type.flags=typINVALID;
     scvalSetSymbol(&thisval,sym);
     val=thisval;
   //  scvalPushUsedRegisters(regs);
   //  params=scvalReadParameters();
     fsym=scsFind_Function(_this->name,_this->type.main,NULL,params,NULL);
     if (deli)
     {//local
      if (fsym)
      {
           Gen_Opcode(opvcpuSUB,scvalSetRegister(&val1,valINT,regSP),scvalSetINT(&val2,type_size));
           deb0("Call copy constructor()\n");
           scvalCallFunction(fsym,&val,sym,&thisval,NULL,params,false);
      }
      else
      {
           if (!scsTypesExact(&sym->type,&params[0].type))
            serr2(scErr_Declaration,"invalid initializer for `%s'",sym->name);
           if (type_size==4)
             Gen_Opcode(opvcpuPUSH,&params[0],NULL);
           else
           {
             Gen_Opcode(opvcpuMOV,scvalSetRegister(&val1,valINT,regCX),scvalSetINT(&val2,type_size));
             Gen_Opcode(opvcpuSUB,scvalSetRegister(&val1,valINT,regSP),scvalSetRegister(&val2,valINT,regCX));
             Gen_Opcode(opvcpuCOPY,&thisval,&params[0]);
           }
           scFree(params);
      }
     }
     else
     {//global
      if (fsym)
      {
           deb0("Call copy constructor()\n");
           scvalCallFunction(fsym,&val,sym,&thisval,NULL,params,false);
      }
      else
      {
           if (!scsTypesExact(&sym->type,&params[0].type))
            serr2(scErr_Declaration,"invalid initializer for `%s'",sym->name);
           if (type_size==4)
             Gen_Opcode(opvcpuMOV,&thisval,&params[0]);
           else
           {
             Gen_Opcode(opvcpuMOV,scvalSetRegister(&val1,valINT,regCX),scvalSetINT(&val2,type_size));
             Gen_Opcode(opvcpuCOPY,&thisval,&params[0]);
           }
           scFree(params);
      }
     }
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
     return;
   //  scvalPopUsedRegisters(regs);
 }
/****************STACK**********************/
 if (deli)
 {int ip=act.ip,zip=0;//save, where we were in code
 new_pmem(&init_end,64);
#if DEBUGLEVEL==0
 deb2("Consts:%d,other:%d\n",act.consts.pos,type_size);
#endif
 init_start=act.consts.pos+type_size;//position of all additional data in
                                  //consts(temporarily stored in init_end
 init_pos=sym->adr.address;//position actually read by initializer
 init_begin=sym->adr.address;//always is the beginning
#if DEBUGLEVEL==0
 deb1("start at BP+%d\n",init_pos);
#endif
 if ((size=scsReadDirectInitialization(deli,&type,false,&act.consts,type_size,sym)))
  {
   char *code=NULL;
   _value val3;

#if DEBUGLEVEL==0
   deb2("next:%d,size:%d\n",act.consts.pos,size);
#endif
   if (act.ip>ip) //store what was generated in Gen_Opcode(assume no jumps)
    {
     deb2("Code (%d to %d) was generated in initialization.It will be moved after the copy.\n",ip,act.ip-1);
     ip=act.ip-ip;
     act.ip-=ip;
     code=scAlloc(ip);
     memcpy(code,act.code.mem+act.ip,ip);
     zip=act.ip;
    }
   scAssert(sciReadInitialization,size<=type_size);//char[] sometimes are shorter
   /*SUB SP,CX;COPY*/
   Gen_Opcode(opvcpuMOV,scvalSetRegister(&val1,valINT,regCX),scvalSetINT(&val2,size));
   type_size=RoundTo32bit(type_size);
   if (type_size==size)
   Gen_Opcode(opvcpuSUB,scvalSetRegister(&val1,valINT,regSP),scvalSetRegister(&val2,valINT,regCX));
    else
   Gen_Opcode(opvcpuSUB,scvalSetRegister(&val1,valINT,regSP),scvalSetINT(&val2,type_size));
   val1.sym=sym;val1.type=type;val1.adr=sym->adr;
   scvalSetINT(&val2,act.consts.pos-size);
   scvalPutToRegister(&val2,TRUE);
   val2.sym=NULL;val2.type=CharAsterisk.type;
   scvalSetRegister(&val3,valINT,regES);
   Gen_Opcode(opvcpuADD,&val2,&val3);
   val2.adr.flags|=adrCPU;
   Gen_Opcode(opvcpuCOPY,&val1,&val2);

   if (code) //restore what was generated in Gen_Opcode
    {
     deb2("Code from %d to %d copied from generation above.\n",zip,act.ip);
     push_pmem(&act.code,ip,code);
     act.ip+=ip;
     //fixJumps(zip,act.ip-ip);//TODO:fix the jumps (if they are in region, add act.ip-zip
     scFree(code);
    }

   if (init_end.pos)
    {
#if DEBUGLEVEL==0
     deb2("Copying %d to consts at %d.\n",init_end.pos,act.consts.pos);
#endif
     push_pmem(&act.consts,init_end.pos,init_end.mem);//copy additional data
    }

  }
  else
  {//all was done
  }
  free_pmem(&init_end);
 }//stack
 else
 {
 /*heap*/
 new_pmem(&init_end,64);
 init_start=-act.inits.pos+sym->adr.address-8;//init_start+act.inits.pos elsewhere is the DS address
 init_pos=init_begin=init_start;
#if DEBUGLEVEL==0
 deb2("Inits at %d size:%d\n",act.inits.pos,type_size);
#endif
 push_pmem(&act.inits,4,&sym->adr.address);//address
 push_pmem(&act.inits,4,&type_size);//size
 if ((size=scsReadDirectInitialization(deli,&type,true,&act.inits,type_size,sym)))
  {

#if DEBUGLEVEL==0
   deb0("Direct\n");
#endif
   scAssert(sciReadInitialization,size==type_size);
  }
  else
  {//size=0 - nothing to copy!
  }
#if DEBUGLEVEL==0
 deb2("Storing %d at inits:%d\n",init_end.pos,act.inits.pos);
#endif
 if (init_end.pos)
 push_pmem(&act.inits,init_end.pos,init_end.mem);//copy additional data
 free_pmem(&init_end);
 }//heap
 deb0("End of initialization\n");
#if DEBUGLEVEL==0
 deb1("Next=%s\n",ST->lexema);
#endif
}
