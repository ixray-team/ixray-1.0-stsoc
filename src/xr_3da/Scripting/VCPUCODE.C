/************************************************************************
       -= SEER - C-Scripting engine v 0.3a =-
State      : under implementation
Author     : Przemyslaw Podsiadly
             email: ppodsiad@elka.pw.edu.pl
             WWW:   http://home.elka.pw.edu.pl/~ppodsiad
File:vcpucode.c
Desc:code generation for VCPU
************************************************************************/
#define INTERNAL_FILE
#include <stdio.h>
#include <seer.h>
#include "internal.h"
#include "code.h"
#define PUSHAlways_INT 1
#define PUSHAnything 0



NameNPar opcodes_vcpu[]={
{"NAO",0},//Not arguments opcode
{"PUSH",1},
{"PUSHADR",1},
{"DPUSH",1},
{"POP",1},
{"NEG",1},//  ???:-1-ARG+1
{"DNEG",1},
{"NOT",1},//log.
{"OPTIONS",1},
{"JMP",1},
{"CALL",1},
{"MOV",2},
{"XCHG",2},
{"CALLEX",2},
{"COPY",2},//see code.h
{"ADD",2},
{"SUB",2},
{"MUL",2},
{"DIV",2},
{"CMPE",2},
{"CMPG",2},
{"CMPL",2},
{"CMPNG",2},
{"CMPNL",2},
{"MOD",2},
{"AND",2},//bit
{"OR",2},//bit
{"XOR",2},//bit
{"ANDL",2},//log.
{"ORL",2},//logic
{"SHL",2},
{"SHR",2},
{"JTRUE",2},//JTRUE a0,209 // if (a0) 209
{"JFALSE",2},//31
{"DADD",2},
{"DSUB",2},
{"DMUL",2},
{"DDIV",2},
{"DCMPE",2},
{"DCMPG",2},
{"DCMPL",2},
{"DCMPNG",2},
{"DCMPNL",2},
{"FIXMUL",2},
{"FIXDIV",2},
{"CMOV",2},//copies 1 byte
{"WMOV",2},//copies 2 bytes
{"DMOV",2},//copies 8 bytes
{"IDBL",2},
{"DINT",2},
{"FDBL",2},
{"DFLT",2},
{"DFIX",2},
{"FIXDBL",2},
{"FORK",2},
{"RET",0},//must be the first of all nonargument
{"WAIT",0},// -
{"CLI",0},// \Turn OFF\ Multitasking
{"STI",0},// /     ON /
{"ENTER",0},//push bp;mov bp,sp
{"LEAVE",0},//mov sp,bp; pop bp
{"NOP",0}
};
int ConvOpcodeTo_Double(int opcode)
{
     switch (opcode)
     {
      case opvcpuMOV:
       opcode=opvcpuDMOV;
       break;
      case opvcpuADD:
       opcode=opvcpuDADD;
       break;
      case opvcpuSUB:
       opcode=opvcpuDSUB;
       break;
      case opvcpuMUL:
       opcode=opvcpuDMUL;
       break;
      case opvcpuDIV:
       opcode=opvcpuDDIV;
       break;
      case opvcpuCMPE:
       opcode=opvcpuDCMPE;
       break;
      case opvcpuCMPG:
       opcode=opvcpuDCMPG;
       break;
      case opvcpuCMPL:
       opcode=opvcpuDCMPL;
       break;
      case opvcpuCMPNG:
       opcode=opvcpuDCMPNG;
       break;
      case opvcpuCMPNL:
       opcode=opvcpuDCMPNL;
       break;
      default:
       serr2(scErr_CodeGen,"Invalid `%s' operand for floating",opcodes_vcpu[opcode].name);
     }
     return opcode;

     return 0;
}

int ConvOpcodeTo_Fixed(int opcode)
{
     switch (opcode)
     {
      case opvcpuMOV:
      case opvcpuADD:
      case opvcpuSUB:
      case opvcpuCMPE:
      case opvcpuCMPG:
      case opvcpuCMPL:
      case opvcpuCMPNG:
      case opvcpuCMPNL:
       //Don't change
       break;
      case opvcpuMUL:
       opcode=opvcpuFIXMUL;
       break;
      case opvcpuDIV:
       opcode=opvcpuFIXDIV;
       break;
      default:;
//       serr(scErr_CodeGen,"Invalid operand for fixed");
     }
     return opcode;
}

void print_value(_value *v)
{
 if (v->adr.flags&adrPOINTER)
 {
  adeb0("[");
  if ((v->adr.flags&adrPOINTER)==adrCS) adeb0("CS+");
  if ((v->adr.flags&adrPOINTER)==adrDS) adeb0("DS+");
  if ((v->adr.flags&adrPOINTER)==adrBP) adeb0("BP+");
  if ((v->adr.flags&adrPOINTER)==adrES) adeb0("ES+");
 }
 if (v->adr.flags&adrREGISTER)
  {
    if (v->adr.address==regCS) {adeb0("CS");} else
    if (v->adr.address==regDS) {adeb0("DS");} else
    if (v->adr.address==regES) {adeb0("ES");} else
    if (v->adr.address==regSS) {adeb0("SS");} else
    if (v->adr.address==regIP) {adeb0("IP");} else
    if (v->adr.address==regSP) {adeb0("SP");} else
    if (v->adr.address==regBP) {adeb0("BP");} else
    if (v->adr.address==regCX) {adeb0("CX");} else
//    if (v->adr.address==regTHIS) {deb0("THIS");} else
       adeb1("a%d",v->adr.address);
  }
 else {
  if (v->adr.flags&adrPOINTER)//in address
   {adeb1("%d",v->adr.address);}
   else
  if (scvalGetValType(v)==valDOUBLE)
   {
   adeb1("%f",v->adr.val.dval);
   }
    else
   {
   adeb1("%d",v->adr.val.ival);
   }
 }
 if (v->adr.flags&adrPOINTER) adeb0("]");
}

void push_value(_value *v,int it)
{
 if ((it==PUSHAnything)&&(scvalGetValType(v)==valDOUBLE)&&(scvalIsImmidiate(v)))
 {push_pmem(&act.code,8,&v->adr.val.dval);/*deb0(":DBL:");*/}
 else
 { if (!scvalIsImmidiate(v))
   pushint_pmem(&act.code,v->adr.address);
   else
   pushint_pmem(&act.code,v->adr.val.ival);
 }
}

void Gen_Align()
{ //align to 4-byte word
 act.code.pos=act.ip;
 if (act.ip%4)//add nops
  {int j;
   unsigned char cod=opvcpuNOP;
   for (j=act.ip%4;j<4;j++)
    push_pmem(&act.code,1,&cod);
  }
 act.ip=act.code.pos;
}
/*
Code generation :
-no param opcodes:
 00000cod
-1  param opcodes:
 codec000 PRtyp000 --par1-- --add---
          01234567 - bit positions
-2  param opcodes:
 codec-PP RtypRtyp --par1-- --par2--

type:
if (!P) decode_type_as typ for _value.flags://numerical value
  &(16+32+64) - 000int 001double 010float 011fixed 100char 101short 110longlong 111(ptr-reserved)
  else//pointer 210-bits    ^^^^^^                                       ^^^^^^^^-2 registers
     valCPU=0,  000
     valDS=4,   001
     valCS=8,   010
     valBP=4+8, 011
       100 ES+       \
       101 SP+
       110 a239+      not used by compiler
       111 a240      /
WARNING: all operations with params have to be 32bit-Word Aligned
*/


int FixUp_Opcode(int opcode,_value* v1,_value* v2)
{if (opcode==-1) return -1;


 if (opcodes_vcpu[opcode].pars==0)/////////////////////////////////////
    return opcode;
 else

 if (opcodes_vcpu[opcode].pars==1)/////////////////////////////////////
 {
//DELS pv1=*v1;
 if (v1->adr.flags&adrADDRESS)
 {switch(opcode){
   case opvcpuPUSH:return opvcpuPUSHADR;
   default:
    serr2(scErr_CodeGen,"Code generation error for %s with ADDRESS value",opcodes_vcpu[opcode].name);
  }
 }
 if (scvalGetValType(v1)==valSHORT&&(v1->adr.flags&adrPOINTER))
  scvalPutToRegister(v1,TRUE);
 if (scvalGetValType(v1)==valCHAR&&(v1->adr.flags&adrPOINTER))
  scvalPutToRegister(v1,TRUE);

 if (scvalSizeOf(v1)==8)
  { switch (opcode)
    {
     case opvcpuPUSH:opcode=opvcpuDPUSH;
                break;
     case opvcpuNEG:opcode=opvcpuDNEG;
                break;
     default:serr2(scErr_CodeGen,"Code generation error for %s with double",opcodes_vcpu[opcode].name);
    }
  }
 }

  else

//===============================2 params============
 if(opcodes_vcpu[opcode].pars==2)/////////////////////////////////////
 {
 if (v1->adr.flags&adrADDRESS||v2->adr.flags&adrADDRESS)
 {switch(opcode){
//   case opvcpuPUSH:return opvcpuPUSHADR;
   default:
    serr2(scErr_CodeGen,"Code generation error for %s with ADDRESS value",opcodes_vcpu[opcode].name);
  }
 }
 if ((v1->type.flags&typUNSIGNED)||(v2->type.flags&typUNSIGNED))
  {_value v;
   switch(opcode)
   {
    case opvcpuCMPG:case opvcpuCMPNG:case opvcpuCMPL:case opvcpuCMPNL:
    case opvcpuDIV:case opvcpuMOD:case opvcpuSHR:
         Gen_Opcode(opvcpuOPTIONS,scvalSetINT(&v,optionsvcpuUNSIGNED),NULL);
    default:;//only those opcodes that are sign sensitive
   }
  }

//DELS pv1=*v1;pv2=*v2;
 if (opcode!=opvcpuDFLT&&
     opcode!=opvcpuFDBL&&
     opcode!=opvcpuIDBL&&
     opcode!=opvcpuFIXDBL&&
     opcode!=opvcpuDFIX&&
     opcode!=opvcpuDINT&&
     opcode!=opvcpuCMOV&&
     opcode!=opvcpuWMOV
     )
  {//Conversion allowed
   if (v1->adr.flags&adrPOINTER)//----------------POINTER-------------
   {_value tval;
    switch(scvalGetValType(v1))
    {
     case valCHAR:
         if (opcode!=opvcpuCMOV)
         {
          tval=*v1;
          if(opcode==opvcpuMOV)
          {
          scvalConvToInt(v2);
          return opvcpuCMOV;
          }
          else
          {
          scvalPutToRegister(v1,TRUE);
  //        valConvTo_Char(v2);
          Gen_Opcode(opcode,v1,v2);
//          Gen_Opcode(opvcpuCMOV,&tval,v1);//write-back to mem-will be done
          *v2=*v1;*v1=tval;
          return opvcpuCMOV;
          }
//DELS         *v1=pv1;*v2=pv2;
         }
       break;
     case valSHORT:
         if (opcode!=opvcpuWMOV)
         {
          tval=*v1;
          if(opcode==opvcpuMOV)
          {
          scvalConvToInt(v2);
          return opvcpuWMOV;
          }
          else
          {
          scvalPutToRegister(v1,TRUE);
          Gen_Opcode(opcode,v1,v2);
          *v2=*v1;*v1=tval;
          return opvcpuWMOV;//write-back to mem
          }
//DELS         *v1=pv1;*v2=pv2;
         }
       break;
     case valFIXED:
       scvalConvToFixed(v2);
       return ConvOpcodeTo_Fixed(opcode);

       break;
     case valFLOAT:
         if (opcode!=opvcpuMOV)
         {int reg=-1;
          tval=*v1;//also remember where was an original
          if (tval.adr.flags&adrREGISTER)
            reg=tval.adr.address;
          scvalPutToRegister(v1,FALSE);
//          if (reg!=-1&&!act.registers[reg].used)
//             act.registers[reg].used=&tval;//make sure we lock the reg.
          Gen_Opcode(opcode,v1,v2);
          *v2=*v1;*v1=tval;
          if (scvalGetValType(v2)==valDOUBLE)
          {return opvcpuDFLT;//back to float quicker
          }
          scvalConvToFloat(v2);//back to float
          return opvcpuMOV;//write-back to mem
         }
         else //scvalConvToFloat(v2);
         {if (scvalIsImmidiate(v2)) scvalConvToFloat(v2);
          else
          switch(scvalGetValType(v2))
          {
           case valDOUBLE:
                return opvcpuDFLT;
           case valFIXED:
           case valINT:
           case valSHORT:
           case valCHAR:
                scvalConvToDouble(v2);
                return opvcpuDFLT;
           default:;
          }
         }
       break;
     case valDOUBLE:
       scvalConvToDouble(v2);
       return ConvOpcodeTo_Double(opcode);
     default:;
    }
   }//pointer - cannot convert variable to different type
   else//------------------------------------------------not a pointer
   {int t1,t2=0;
   t1=scvalGetValType(v1);
   t2=scvalGetValType(v2);
   if ((t2==valSHORT)&&(v2->adr.flags&adrPOINTER))//has to be short
    { if(opcode==opvcpuMOV) opcode=opvcpuWMOV;
      else
      { scvalPutToRegister(v2,TRUE);
      }
    }
   if ((t2==valCHAR)&&(v2->adr.flags&adrPOINTER))//has to be char
    { if(opcode==opvcpuMOV) opcode=opvcpuCMOV;
      else
      { scvalPutToRegister(v2,TRUE);
      }
    }
   if (t1==valFIXED)
    {  scvalConvToFixed(v2);t2=valFIXED;
       opcode=ConvOpcodeTo_Fixed(opcode);

    } else
   if (t1==valINT&&t2==valFIXED)
    {  scvalConvToFixed(v1);t1=valFIXED;
       opcode=ConvOpcodeTo_Fixed(opcode);

    }

   if ((t2==valFLOAT)&&(v2->adr.flags&adrPOINTER))//has to be float
    { scvalPutToRegister(v2,TRUE);
    }

   if (sciValTypeFloating(t2)||sciValTypeFloating(t1))
    {
/*      deb0("floating\n");
      deb1("v1=%s,",ValTypeStr(t1));
      deb1("v2=%s,",ValTypeStr(t2));
      deb1(" {%s",opcodes_vcpu[opcode].name);
   print_value(v1);
   deb0(" , ");
   print_value(v2);
   deb0("}\n ");*/
      scvalConvToDouble(v1);
      scvalConvToDouble(v2);
      opcode=ConvOpcodeTo_Double(opcode);


      switch(opcode)
      {
       case opvcpuDCMPE:
       case opvcpuDCMPL:
       case opvcpuDCMPG:
       case opvcpuDCMPNL:
       case opvcpuDCMPNG:
         v1->adr.flags=adrREGISTER;
         v1->type.flags=typPREDEFINED;
         (valTYPE)v1->type.main=valINT;
//DELS         pv1.type=v1->type;

//         v1->sym=NULL;
//         deb1("Now it is of type:%s\n",ValTypeStr(t1));
         break;
       default:;
      }
    }//floating
   }//not a pointer -can modify both
 }//can modify

 if (opcode==opvcpuJFALSE||opcode==opvcpuJTRUE)
  {//optimize JFALSE0,x->JMP x, JTRUE 0,x ->nothing
   if (scvalIsImmidiate(v1))
    {if (v1->adr.val.ival==0)
     {switch(opcode)
      {
       case opvcpuJFALSE:opcode=opvcpuJMP;*v1=*v2;v2=NULL;return opcode;
       case opvcpuJTRUE:return -1;//nothing to do
      }
     }else
     {
      switch(opcode)
      {
       case opvcpuJTRUE:opcode=opvcpuJMP;*v1=*v2;v2=NULL;break;
       case opvcpuJFALSE:return -1;//nothing to do
      }
     }
    }
  }//JTRUE/FALSE optimization
 }//2 params
 return opcode;
}
//INSTRUCTION:<0-63>+(64/128)
//DELS
void Gen_Opcode(int opcode,_value* v1,_value* v2)
{
 char cod;
 unsigned char instr[4]={0,0,0,0};
 act.code.pos=act.ip;

 if ((opcode&0xff)==0)//extended opcode
 {
  Gen_Align();
  adeb1(" %3d:",act.ip);
  switch(opcode)
  {case opvcpuEXPR:adeb0("EXPR\n");break;
   default:adeb1("UNKNOWN(%x)\n",opcode);
  }
  push_pmem(&act.code,4,&opcode);
  act.ip=act.code.pos;
  return;
 }

 opcode=FixUp_Opcode(opcode,v1,v2);

 if (opcode==-1) return;
 if(opcodes_vcpu[opcode].pars==0)
 {
 adeb1(" %3d:",act.ip);
 adeb1(" %s ",opcodes_vcpu[opcode].name);
 cod=opcode;
#ifdef _SHOW_MACHINE_CODE
 adeb0("\t| ");
#endif
 push_pmem(&act.code,1,&cod);
 act.ip=act.code.pos;
 adeb0("\n");
 }
 else

 if(opcodes_vcpu[opcode].pars==1)
 {
 Gen_Align();
 adeb1(" %3d:",act.ip);
 adeb1(" %s ",opcodes_vcpu[opcode].name);
 switch(opcode)
 {case opvcpuJMP:
     if (scvalIsImmidiate(v1))
     {adeb1("(%d)",v1->adr.val.ival);
      v1->adr.val.ival-=act.ip+8;
     }
     break;
 }
 print_value(v1);
#ifdef _SHOW_MACHINE_CODE
 adeb0("\t| ");
#endif
 cod=opcode;
 instr[0]=cod;
 if (v1->adr.flags&adrPOINTER)//not 000
  {
    instr[1]|=1;//pointer
    if (v1->adr.flags&adrREGISTER)
     {
      instr[1]|=2;//register
      instr[2]=v1->adr.address;//register numer
     }
    instr[1]|=(v1->adr.flags&adrSTOREPOINTER)<<2;//base register
  } else
  {
    if (v1->adr.flags&adrREGISTER)
      {
       instr[1]|=2;//register
       instr[2]=v1->adr.address;//register numer
      }
  }

 push_pmem(&act.code,4,instr);
 if (!(instr[1]&2)) // not register - push value
    push_value(v1,PUSHAnything);

 act.ip=act.code.pos;
 adeb0("\n");
 }

  else

//===============================2 params============
 if(opcodes_vcpu[opcode].pars==2)
 {
  Gen_Align();
  adeb1(" %3d:",act.ip);
  adeb1(" %s ",opcodes_vcpu[opcode].name);
  print_value(v1);
  adeb0(" , ");
  print_value(v2);
#ifdef _SHOW_MACHINE_CODE
  adeb0("\t| ");
#endif
  cod=opcode;
  instr[0]=cod;
//1-st opcode
 if (v1->adr.flags&adrPOINTER)//not 000
  {
    instr[0]|=64;//pointer
    if (v1->adr.flags&adrREGISTER)
     {
      instr[1]|=1;//register
      instr[2]=v1->adr.address;//register numer
     }
    instr[1]|=((v1->adr.flags&adrSTOREPOINTER))<<1;//base register
  } else
  {
    if (v1->adr.flags&adrREGISTER)
      {
       instr[1]|=1;//register
       instr[2]=v1->adr.address;//register numer
      }
  }
//2-st opcode
 if (v2->adr.flags&adrPOINTER)//not 000
  {
    instr[0]|=128;//pointer
    if (v2->adr.flags&adrREGISTER)
      {
      instr[1]|=(1<<4);//register
      instr[3]=v2->adr.address;//register numer
      }
    instr[1]|=((v2->adr.flags&adrSTOREPOINTER))<<5;//>>2)<<5;//base register
  } else
  {
    if (v2->adr.flags&adrREGISTER)
      {
       instr[1]|=1<<4;//register
       instr[3]=v2->adr.address;//register numer
      }
  }

 push_pmem(&act.code,4,instr);
 if (!(instr[1]&1)) // not register - push value
    push_value(v1,PUSHAnything);
  //fix jumps
  switch(opcode)
  {
   case opvcpuJFALSE:
   case opvcpuJTRUE:
      if (scvalIsImmidiate(v2))
      {
       v2->adr.val.ival-=act.code.pos+4;
       adeb1("(%d)",v2->adr.val.ival);
      }
   break;
  }
 if (!(instr[1]&(1<<4))) // not register - push value
    push_value(v2,PUSHAnything);
 act.ip=act.code.pos;
 adeb0("\n");
 }
  else serr2(scErr_CodeGen, "Code generation error %s","for more than 2 opcodes");
 return;
}

