/************************************************************************
       -= SEER - C-Scripting engine v 0.9a =-
State      : under implementation
Author     : Przemyslaw Podsiadly
             email: ppodsiad@elka.pw.edu.pl
             WWW:   http://home.elka.pw.edu.pl/~ppodsiad
File:expressi.c
Desc:calculating expressions
************************************************************************/
#define INTERNAL_FILE
#include <seer.h>
#include "internal.h"
#include "code.h"

//******All Read_* functions ends with separator in ending ST->lexema!!*******
/*
expr := exp1,expr | exp1
exp1 := lvalue [=] expr | exp2      [=] =: =,+=,-=,*=,/=,%=,&=,^=,|=,<<=,>>=
exp2 := exp3? exp3 : exp3 | exp3
exp3 := exp4 [||] exp3 | exp4
exp4 := exp5 [&&] exp4 | exp5
exp5 := exp6 [|] exp5 | exp6
exp6 := exp7 [^] exp6 | exp7
exp7 := exp8 [&] exp7 | exp8
exp8 := exp9 [==] exp8 | exp9      [==] =: ==,!=
exp9 := exp10 [<] exp9 | exp10      [<] =: <,<=,>,>=
exp10:= exp11 [<<] exp10 | exp11
exp11:= exp12 [+] exp11 | exp12     [+] =: +,-
exp12:= exp13 [*] exp12 | exp13     [*] =: *,/,%
exp13:= ~special~ : ~exp14 | !exp14 | -exp14 | ++exp14 | exp14++ |
        --exp14 | exp14-- | +exp14 | -exp14 | *exp14 | &exp14
        (type)exp14 | sizeof(type) | exp14

exp14:= (expr) | exp14[expr] | exp14 -> exp14 | exp14 . symbol | terminal | error

exp14:= (expr) | exp15 -> exp14 | exp15 . exp14 | exp15[expr]... | exp15

exp14:= (expr) | exp15 | exp14[expr] | exp14 -> exp14 | exp14 . exp14

exp14:= (expr) | exp15[expr] | exp15 -> exp15 | exp15 . symbol | exp15

exp15:= terminal | error
lvalue:= exp13
lvalue:= (type)exp14 | *exp14 | exp14

terminal := symbol (paramlist) | symbol | number | string
paramlist := exp1 , paramlist | exp1 | ""

*/

#define Dis(x)
//deb1("*%d,",x);scmemTestUp();
//{debprintf("%d;",x);}
extern scSymbol this_symbol;

char *scReadOperatorName();
void valNotConst(_value *val);


static int scsGenStruct(scType *stype,_value *val,scType *temptype,BOOL toStack);
static void scvalPushParameters(_value* fun,_value *params,int *size,char **param_end);
static BOOL scsCallOperator(char *operatorX,_value *val,_value *v1,_value *v2,_value *v3,BOOL CallerIgnore);

///////////////////////////////////////////////////////////////////////////
////MACROS/////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

//for right ++ and -- , eg. a++;a--;
//for ++a use TryOperator1Arg.
#define TryOperator1ArgRight(sign)                                              \
            if (scvalGetValType(val)==valSTRUCT)                                \
              {_value vt1;                                                      \
               scvalSetINT(&vt1,1);                                             \
               if (!scsCallOperator("operator"sign,val,&vt1,NULL,NULL,CallerIgnore))\
                 serr3(scErr_Operand, invalid_operand_to,"",sign);              \
               return 1;                                                        \
              }

#define TryOperator1Arg(sign)                                                   \
            if (scvalGetValType(val)==valSTRUCT)                                \
              {                                                                 \
               if (!scsCallOperator("operator"sign,val,NULL,NULL,NULL,CallerIgnore))\
                 serr3(scErr_Operand, invalid_operand_to,"",sign);              \
               return 1;                                                        \
              }

#define IfNotOperator2Arg(sign,ThisSubExpr)                                    \
            if (scvalGetValType(val)==valSTRUCT)                                \
              {Advance;                                                         \
               Read_MusExp(ThisSubExpr,&vt1);                                   \
               if (!scsCallOperator("operator"sign,val,&vt1,NULL,NULL,CallerIgnore))\
                 serr3(scErr_Operand, invalid_operand_to,"s",sign);             \
              } else

//przemienne-

#define TwoParamInstr(isneutral,operand,opvcpuCODE,neutral,ThisSubExpr,przemienne,divs)\
            Advance;                                                     \
            if (!scvalIsImmidiate(val)) scvalPutToRegister(val,TRUE);        \
            Read_MusExp(ThisSubExpr,&vt1);                          \
            if (scvalGetValType(&vt1)==valSTRUCT)                   \
               serr(scErr_Operand,"Invalid argument to operator "#operand);\
            if (scvalIsImmidiate(val)&&scvalIsImmidiate(&vt1))               \
            {                                                            \
             scvalBothTypes(val,&vt1);                                \
             if (scvalGetValType(val)==valINT)                                \
             {                                                           \
             if (divs&&vt1.adr.val.ival==0)                              \
                serr(scErr_Operand,"Division by zero");                  \
             if (((val->type.flags&typUNSIGNED)                          \
                 ||(vt1.type.flags&typUNSIGNED)))                        \
              val->adr.val.ival=(unsigned)val->adr.val.ival operand      \
                                (unsigned)vt1.adr.val.ival;              \
             else                                                        \
              val->adr.val.ival=val->adr.val.ival                        \
                                operand vt1.adr.val.ival;                \
             }                                                           \
             else                                                        \
             {                                                           \
             if (divs&&vt1.adr.val.dval==0)                              \
                serr(scErr_Operand,"Division by zero");                  \
             val->adr.val.dval=val->adr.val.dval operand vt1.adr.val.dval;\
             }                                                           \
             scvalFreeValue(&vt1);                                            \
             return 1;                                                   \
            }                                                            \
            /*Optimize*/                                                 \
            /*if ((!(vt1.adr.flags&adrPointer) &&vt1.adr.flags&adrRegister)\
               ||scvalIsImmidiate(val))                                    \
              if (przemienne) XChgVal(val,&vt1);*/                       \
            if (isneutral&&scvalIsImmidiate(&vt1)&&                        \
               ((scvalImmidiateValue(&vt1)==neutral)))                     \
               return 1;                                                 \
            scvalPutToRegister(val,TRUE);                             \
            Gen_Opcode(opvcpuCODE,val,&vt1);                        \
            scvalFreeValue(&vt1);

#define TwoParamInstrWithSkip(isneutral,operand,opvcpuCODE,neutral,skipper,ThisSubExpr)  \
           {                                                             \
            _value vt2;/*Check if skip*/                                 \
            int oip,tip;                                                 \
            if (scvalGetValType(&vt1)==valSTRUCT)                   \
               serr(scErr_Operand,"Invalid argument to operator "#operand);\
            if (!scvalIsImmidiate(val)) scvalPutToRegister(val,TRUE);        \
            oip=act.ip;                                                  \
            scvalSetINT(&vt2,0);                                           \
            if (skipper)                                                 \
            Gen_Opcode(opvcpuJTRUE,val,&vt2);/*order to skip*/           \
            else                                                         \
            Gen_Opcode(opvcpuJFALSE,val,&vt2);/*order to skip*/          \
            Advance;                                                \
            Read_MusExp(ThisSubExpr,&vt1);                          \
            if (scvalGetValType(&vt1)==valSTRUCT)                   \
               serr(scErr_Operand,"Invalid argument to operator "#operand);\
            scvalConvToInt(&vt1);                                    \
            if (scvalIsImmidiate(val)&&scvalIsImmidiate(&vt1))               \
            {scvalBothTypes(val,&vt1);                                     \
             if (scvalGetValType(val)==valINT)                                \
             val->adr.val.ival=val->adr.val.ival operand vt1.adr.val.ival;           \
             else                                                        \
             val->adr.val.dval=val->adr.val.dval operand vt1.adr.val.dval;           \
             scvalFreeValue(&vt1);                                            \
             return 1;                                                   \
            }                                                            \
                                                                         \
            if (!(isneutral&&scvalIsImmidiate(&vt1)&&(scvalImmidiateValue(&vt1)==neutral)))    \
            {                                                            \
            Gen_Opcode(opvcpuCODE,val,&vt1);                        \
            }                                                            \
            tip=act.ip;act.ip=oip;                                       \
            scvalSetINT(&vt2,tip);                                         \
            if (skipper)                                                 \
            Gen_Opcode(opvcpuJTRUE,val,&vt2);/*order to skip*/           \
            else                                                         \
            Gen_Opcode(opvcpuJFALSE,val,&vt2);/*order to skip*/          \
            act.ip=tip;                                             \
            scvalFreeValue(&vt1);                                             \
           }


#define TwoParamInstrNoDouble(isneutral,operand,operstr,opvcpuCODE,neutral,ThisSubExpr,przemienne) \
            Advance;                                                     \
            if (!scvalIsImmidiate(val)) scvalPutToRegister(val,TRUE);        \
            Read_MusExp(ThisSubExpr,&vt1);                          \
            if (scvalGetValType(&vt1)==valSTRUCT)                   \
               serr(scErr_Operand,"Invalid argument to operator "#operand);\
            if (sciValTypeFloating(scvalGetValType(val))||sciValTypeFloating(scvalGetValType(&vt1)))   \
                serr3(scErr_Operand,invalid_operand_to,"s","binary "operstr);\
            if (scvalIsImmidiate(val)&&scvalIsImmidiate(&vt1))               \
            {scvalBothTypes(val,&vt1);                                     \
             if (scvalGetValType(val)==valDOUBLE)                             \
                serr3(scErr_Operand,invalid_operand_to,"s","binary "operstr);\
             if (((val->type.flags&typUNSIGNED)                          \
                 ||(vt1.type.flags&typUNSIGNED)))                        \
              (unsigned)val->adr.val.ival=(unsigned)val->adr.val.ival    \
                               operand  (unsigned)vt1.adr.val.ival;      \
             else                                                        \
             val->adr.val.ival##operand##=vt1.adr.val.ival;                      \
             scvalFreeValue(&vt1);                                            \
             return 1;                                                   \
            } /*Optimize*/                                               \
            /*if ((!(vt1.flags&valPointer)&&vt1.flags&valRegister)       \
               ||valIsANumber(val))                                      \
              if (przemienne) XChgVal(val,&vt1);*/                       \
            if (isneutral&&scvalIsImmidiate(&vt1)&&(vt1.adr.val.ival==neutral))\
               return 1;                                                 \
            scvalPutToRegister(val,TRUE);                             \
            Gen_Opcode(opvcpuCODE,val,&vt1);                             \
            scvalFreeValue(&vt1);

///////////////////////////////////////////////////////////////////////////
//int Read_Expression(_value *val,BOOL RequiredExp);
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
////get_definition_on_cast/////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static scSymbol* get_definition_on_cast()
{scSymbol *t=NULL;
 scPosition p;
 scget_back(ST);
 scGetPosition(ST,&p);
 Advance;
// deb1("Type %s?\n",ST->lexema);
/* if (lexeq("(")) {
  scSetPosition(p);
  Advance;
  return NULL;
 }*/
 t=sctypGetDefinition();

 if (!t) {
  scSetPosition(p);
  Advance;
 }
 return t;
}

///////////////////////////////////////////////////////////////////////////
////get_definition_no_construct////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static scSymbol* get_definition_on_construct()
{scSymbol *t=NULL;
 scPosition p;
 scget_back(ST);
 scGetPosition(ST,&p);
// Advance;
 t=scsGetTypeDeli();
 if (!t) {
  scSetPosition(p);
  Advance;
 }
 return t;
}

///////////////////////////////////////////////////////////////////////////
////get_definition_on_typeof///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static scSymbol* get_definition_on_typeof()
{scSymbol *t=NULL;
 scPosition p;
 scget_back(ST);
 scGetPosition(ST,&p);
// Advance;
// deb1("Type %s?\n",ST->lexema);
// deb0("KUKA");
 t=sctypGetDefinition();
// t=scsGetTypeDeli();
// deb1("=>%s\n",ST->lexema);
 if (!t||!lexeq(")")) {
  scSetPosition(p);
  Advance;
  if (t) {scFree(t);t=NULL;}
 }
 return t;
}

///////////////////////////////////////////////////////////////////////////
///////////////sciReadSubExp/////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
#define Read_BegExp(a,b) {if (!sciReadSubExp(a,b,ExpRequired)) return 0;}
#define Read_MusExp(a,b)   {sciReadSubExp(a,b,true);}
#define CallerIgnore (!ExpRequired)
//if !val, then the caller ignores the value,
//maybe ExpRequired is enough for Ignore. When !ExpRequired, you can ignore
//caller don't really need us.
int sciReadSubExp(int i,_value *val,BOOL ExpRequired)
{_value vt1={{/*ADDRESS*/0,adrREGISTER,{0}},{/*type*/typPREDEFINED,NULL,(scSymbol*)valINT},NULL};
// scPosition befpos;
 scget_back(ST);
// scGetPosition(ST,&befpos);
 Advance;

// scSetPosition(befpos);
 switch(i)
 {



#define AfterLValueRead if (!(val->adr.flags&adrPOINTER)) serr(scErr_Operand, "Invalid lvalue in assignment");\
                        Advance;Read_MusExp(1,&vt1);
///////////////////////////////////////////////////////////////////////////
/////////////////////////////////1////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
  case 1://****** exp1
//exp1 := lvalue [=] expr | exp2      [=] =: =,+=,-=,*=,/=,%=,&=,^=,|=,<<=,>>=
       {Dis(1);
        Read_BegExp(2,val);//lvalue
        if (lexeq("=")){
           AfterLValueRead
           valNotConst(val);
           scvalArrayToPointer(&vt1);
           if (scvalGetValType(val)==valSTRUCT)
           {
            if (scsCallOperator("operator=",val,&vt1,NULL,NULL,CallerIgnore))
             return 1;
            if (scvalGetValType(&vt1)==valSTRUCT)
             {
             if (scvalSizeOf(&vt1)!=scvalSizeOf(val))
              {serr(scErr_Operand, "Incompatible struct in copy.");}
              else
              {int size;
               if ((size=scvalSizeOf(val))==4)
                  Gen_Opcode(opvcpuMOV,val,&vt1);
               else
                  {_value v1,v2;
                  Gen_Opcode(opvcpuMOV,scvalSetRegister(&v1,valINT,regCX),scvalSetINT(&v2,size));
                  Gen_Opcode(opvcpuCOPY,val,&vt1);
                  }
               return 1;
              }//copy
             }//both are struct-copy it normally
            serr3(scErr_Operand, invalid_operand_to,"s","=");
           }//val==struct
           Gen_Opcode(opvcpuMOV,val,&vt1);
           scvalFreeValue(&vt1);
           }else
        if (lexeq("+=")){
           AfterLValueRead
           valNotConst(val);
//           if (scvalGetValType(&vt1)==valPointer)//werr-warning
//              werr(scErr_Operand, "Invalid pointer operation");
           if (scvalGetValType(val)==valSTRUCT)
           {if (!scsCallOperator("operator+=",val,&vt1,NULL,NULL,CallerIgnore))
              serr3(scErr_Operand, invalid_operand_to,"s","+=");
           }
           else
           {
            if (scvalGetValType(&vt1)==valSTRUCT)
                serr3(scErr_Operand, invalid_operand_to,"s","+=");
            if (scvalIsImmidiate(&vt1)&&(scvalImmidiateValue(&vt1)==0)) return 1;
            Gen_Opcode(opvcpuADD,val,&vt1);
           }
           scvalFreeValue(&vt1);
           }else
        if (lexeq("-=")){
           AfterLValueRead
           valNotConst(val);
           if (scvalGetValType(val)==valSTRUCT)
           {if (!scsCallOperator("operator-=",val,&vt1,NULL,NULL,CallerIgnore))
              serr3(scErr_Operand, invalid_operand_to,"s","-=");
           }
           else
           {
            if (scvalGetValType(&vt1)==valSTRUCT)
                serr3(scErr_Operand, invalid_operand_to,"s","-=");
            if (scvalIsImmidiate(&vt1)&&(scvalImmidiateValue(&vt1)==0)) return 1;
            Gen_Opcode(opvcpuSUB,val,&vt1);
           }
           scvalFreeValue(&vt1);
           }else
        if (lexeq("*=")){
           AfterLValueRead
           valNotConst(val);
           if (scvalGetValType(val)==valSTRUCT)
           {if (!scsCallOperator("operator*=",val,&vt1,NULL,NULL,CallerIgnore))
              serr3(scErr_Operand, invalid_operand_to,"s","*=");
           }
           else
           {
            if (scvalGetValType(&vt1)==valSTRUCT)
                serr3(scErr_Operand, invalid_operand_to,"s","*=");
            if (scvalIsImmidiate(&vt1)&&(scvalImmidiateValue(&vt1)==1)) return 1;
            Gen_Opcode(opvcpuMUL,val,&vt1);
           }
           scvalFreeValue(&vt1);
           }else
        if (lexeq("/=")){
           AfterLValueRead
           valNotConst(val);
           if (scvalGetValType(val)==valSTRUCT)
           {if (!scsCallOperator("operator/=",val,&vt1,NULL,NULL,CallerIgnore))
              serr3(scErr_Operand, invalid_operand_to,"s","/=");
           }
           else
           {
            if (scvalGetValType(&vt1)==valSTRUCT)
                serr3(scErr_Operand, invalid_operand_to,"s","/=");
            if (scvalIsImmidiate(&vt1)&&(scvalImmidiateValue(&vt1)==1)) return 1;
            Gen_Opcode(opvcpuDIV,val,&vt1);
           }
           scvalFreeValue(&vt1);
           }else
        if (lexeq("%=")){
           AfterLValueRead
           valNotConst(val);
           if (scvalGetValType(val)==valSTRUCT)
           {if (!scsCallOperator("operator%=",val,&vt1,NULL,NULL,CallerIgnore))
              serr3(scErr_Operand, invalid_operand_to,"s","%=");
           }
           else
           {
            if (scvalGetValType(&vt1)==valSTRUCT)
                serr3(scErr_Operand, invalid_operand_to,"s","*=");
            Gen_Opcode(opvcpuMOD,val,&vt1);
           }
           scvalFreeValue(&vt1);
           }else
        if (lexeq("&=")){
           AfterLValueRead
           valNotConst(val);
           if (scvalGetValType(val)==valSTRUCT)
           {if (!scsCallOperator("operator&=",val,&vt1,NULL,NULL,CallerIgnore))
              serr3(scErr_Operand, invalid_operand_to,"s","&=");
           }
           else
           {
            if (scvalGetValType(&vt1)==valSTRUCT)
                serr3(scErr_Operand, invalid_operand_to,"s","&=");
            Gen_Opcode(opvcpuAND,val,&vt1);
           }
           scvalFreeValue(&vt1);
           }else
        if (lexeq("|=")){
           AfterLValueRead
           valNotConst(val);
           if (scvalGetValType(val)==valSTRUCT)
           {if (!scsCallOperator("operator|=",val,&vt1,NULL,NULL,CallerIgnore))
              serr3(scErr_Operand, invalid_operand_to,"s","|=");
           }
           else
           {
            if (scvalGetValType(&vt1)==valSTRUCT)
                serr3(scErr_Operand, invalid_operand_to,"s","|=");
            if (scvalIsImmidiate(&vt1)&&(scvalImmidiateValue(&vt1)==0)) return 1;
            Gen_Opcode(opvcpuOR,val,&vt1);
           }
           scvalFreeValue(&vt1);
           }else
        if (lexeq("^=")){
           AfterLValueRead
           valNotConst(val);
           if (scvalGetValType(val)==valSTRUCT)
           {if (!scsCallOperator("operator^=",val,&vt1,NULL,NULL,CallerIgnore))
              serr3(scErr_Operand, invalid_operand_to,"s","^=");
           }
           else
           {
            if (scvalGetValType(&vt1)==valSTRUCT)
                serr3(scErr_Operand, invalid_operand_to,"s","^=");
            if (scvalIsImmidiateOfType(&vt1,valINT)&&(scvalImmidiateValue(&vt1)==0)) return 1;
            Gen_Opcode(opvcpuXOR,val,&vt1);
           }
           scvalFreeValue(&vt1);
           }else
        if (lexeq("<<=")){
           AfterLValueRead
           valNotConst(val);
           if (scvalGetValType(val)==valSTRUCT)
           {if (!scsCallOperator("operator<<=",val,&vt1,NULL,NULL,CallerIgnore))
              serr3(scErr_Operand, invalid_operand_to,"s","<<=");
           }
           else
           {
            if (scvalGetValType(&vt1)==valSTRUCT)
                serr3(scErr_Operand, invalid_operand_to,"s","<<=");
            if (scvalIsImmidiateOfType(&vt1,valINT)&&(scvalImmidiateValue(&vt1)==1)) return 1;
            Gen_Opcode(opvcpuSHL,val,&vt1);
           }
           scvalFreeValue(&vt1);
           }else
        if (lexeq(">>=")){
           AfterLValueRead
           valNotConst(val);
           if (scvalGetValType(val)==valSTRUCT)
           {if (!scsCallOperator("operator>>=",val,&vt1,NULL,NULL,CallerIgnore))
              serr3(scErr_Operand, invalid_operand_to,"s",">>=");
           }
           else
           {
            if (scvalGetValType(&vt1)==valSTRUCT)
                serr3(scErr_Operand, invalid_operand_to,"s",">>=");
            if (scvalIsImmidiateOfType(&vt1,valINT)&&(scvalImmidiateValue(&vt1)==1)) return 1;
            Gen_Opcode(opvcpuSHR,val,&vt1);
           }
           scvalFreeValue(&vt1);
           }
        break;
       }




///////////////////////////////////////////////////////////////////////////
/////////////////////////////////2////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
  case 2://****** exp2
//exp2 := exp3? exp3 : exp3 | exp3
       {Dis(2);
        Read_BegExp(3,val);
        if (lexeq("?"))
           {_value vt2;
            int tip,oip1,oip2;
              scvalSetINT(&vt2,0);
              Gen_Align();
              scvalConvToInt(val);//HERE
              scvalPutToRegister(val,TRUE);
              oip1=act.ip;
              Gen_Opcode(opvcpuJFALSE,val,&vt2);
              scvalFreeValue(val);
            Advance;
            vt1=*val;
            Read_MusExp(3,val);
            scvalPutToRegister(val,TRUE);
            scvalFreeValue(val);
            if (!lexeq(":")) serr(scErr_Parse, "`:' expected");
              oip2=act.ip;
              Gen_Opcode(opvcpuJMP,&vt2,NULL);
              Gen_Align();
              tip=act.ip;act.ip=oip1;
              scvalSetINT(&vt2,tip);
              Gen_Opcode(opvcpuJFALSE,&vt1,&vt2);
              act.ip=tip;
            Advance;
            Read_MusExp(3,&vt2);
            if (vt2.adr.flags&adrADDRESS)
            {
               scvalPutToRegister(&vt2,TRUE);

            }

            if (vt2.adr.flags&adrREGISTER&&vt2.adr.address==val->adr.address)//Same reg
            {}
            else
            {
             Gen_Opcode(opvcpuMOV,val,&vt2);
            }
            scvalFreeValue(&vt2);
              Gen_Align();
              tip=act.ip;act.ip=oip2;
              scvalSetINT(&vt2,tip);
              Gen_Opcode(opvcpuJMP,&vt2,NULL);
              act.ip=tip;
              act.registers[val->adr.address].used=val;
              //*val=vt1;
           }
        break;
       }



///////////////////////////////////////////////////////////////////////////
/////////////////////////////////3////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
  case 3://****** exp3
//exp3 := exp4 [||] exp3 | exp4
       {BOOL quit=false;
        Dis(3);
        Read_BegExp(4,val);
        while(!quit)
        {quit=true;

        if (lexeq("||"))
           {quit=false;
            scvalConvToInt(val);
            TwoParamInstrWithSkip(1,||,opvcpuORL,0,1,3);
           }
        }
        break;
       }


///////////////////////////////////////////////////////////////////////////
/////////////////////////////////4////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
  case 4://****** exp4 (if first 0, second is not processed!,val =0)
//exp4 := exp5 [&&] exp4 | exp5
       {BOOL quit=false;
        Dis(4);
        Read_BegExp(5,val);
        while(!quit)
        {quit=true;

        if (lexeq("&&"))
           {quit=false;
            scvalConvToInt(val);
            TwoParamInstrWithSkip(1,&&,opvcpuANDL,1,0,4);
           }
        }
        break;
       }



///////////////////////////////////////////////////////////////////////////
/////////////////////////////////5////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
  case 5://****** exp5
//exp5 := exp6 [|] exp5 | exp6
       {BOOL quit=false;
        Dis(5);
        Read_BegExp(6,val);
        while(!quit)
        {quit=true;

        if (lexeq("|"))
           {quit=false;
            IfNotOperator2Arg("|",5)
            {
            TwoParamInstrNoDouble(1,|,"|",opvcpuOR,0,5,true);
            }
           }
        }
        break;
       }

///////////////////////////////////////////////////////////////////////////
/////////////////////////////////6////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
  case 6://****** exp6
//exp6 := exp7 [^] exp6 | exp7
       {BOOL quit=false;
        Dis(6);
        Read_BegExp(7,val);
        while(!quit)
        {quit=true;

        if (lexeq("^"))
           {quit=false;
            IfNotOperator2Arg("^",6)
            {
            TwoParamInstrNoDouble(1,^,"^",opvcpuXOR,0,6,true);
            }
           }
        }
        break;
       }



///////////////////////////////////////////////////////////////////////////
/////////////////////////////////7////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
  case 7://****** exp7
//exp7 := exp8 [&] exp7 | exp8
       {BOOL quit=false;
        Dis(7);
        Read_BegExp(8,val);
        while(!quit)
        {quit=true;

        if (lexeq("&"))
           {quit=false;
            IfNotOperator2Arg("&",7)
            {
            TwoParamInstrNoDouble(1,&,"&",opvcpuAND,0xffffffff,7,false);
            }
           }
        }
        break;
       }



///////////////////////////////////////////////////////////////////////////
/////////////////////////////////8////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
  case 8://****** exp8
//exp8 := exp9 [==] exp8 | exp9      [==] =: ==,!=
       {BOOL quit=false;
        Dis(8);
        Read_BegExp(9,val);
        while(!quit)
        {quit=true;

        if (lexeq("=="))
           {quit=false;
            IfNotOperator2Arg("==",8)
            {
            TwoParamInstr(0,==,opvcpuCMPE,0,8,true,false);
            val->type.flags=typPREDEFINED;(valTYPE)val->type.main=valINT;
            }
           }else

        if (lexeq("!="))
           {quit=false;
            IfNotOperator2Arg("!=",8)
            {
            TwoParamInstr(0,!=,opvcpuCMPE,0,8,true,false);
            if(!scvalIsImmidiate(val))Gen_Opcode(opvcpuNOT,val,NULL);
            val->type.flags=typPREDEFINED;(valTYPE)val->type.main=valINT;
            }
           }
        }
        break;
       }



///////////////////////////////////////////////////////////////////////////
/////////////////////////////////9////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
  case 9://****** exp9
//exp9 := exp10 [<] exp9 | exp10      [<] =: <,<=,>,>=
       {BOOL quit=false;
        Dis(9);
        Read_BegExp(10,val);
        while(!quit)
        {quit=true;

        if (lexeq("<"))
           {quit=false;
            IfNotOperator2Arg("<",9)
            {
            TwoParamInstr(0,<,opvcpuCMPL,0,9,false,false);
            val->type.flags=typPREDEFINED;(valTYPE)val->type.main=valINT;
            }
           }else

        if (lexeq("<="))
           {quit=false;
            IfNotOperator2Arg("<=",9)
            {
            TwoParamInstr(0,<=,opvcpuCMPNG,0,9,false,false);
            val->type.flags=typPREDEFINED;(valTYPE)val->type.main=valINT;
            }
           }else

        if (lexeq(">"))
           {quit=false;
            IfNotOperator2Arg(">",9)
            {
            TwoParamInstr(0,>,opvcpuCMPG,0,9,false,false);
            val->type.flags=typPREDEFINED;(valTYPE)val->type.main=valINT;
            }
           }else

        if (lexeq(">="))
           {quit=false;
            IfNotOperator2Arg(">=",9)
            {
            TwoParamInstr(0,>=,opvcpuCMPNL,0,9,false,false);
            val->type.flags=typPREDEFINED;(valTYPE)val->type.main=valINT;
            }
           }
        }
        break;
       }



///////////////////////////////////////////////////////////////////////////
/////////////////////////////////10////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
  case 10://****** exp10
//exp10:= exp11 [<<] exp10 | exp11
       {BOOL quit=false;
        Dis(10);
        Read_BegExp(11,val);
        while(!quit)
        {quit=true;

        if (lexeq("<<"))
           {quit=false;
            IfNotOperator2Arg("<<",10)
            {
            TwoParamInstrNoDouble(1,<<,"<<",opvcpuSHL,0,10,false);
            }
           }else

        if (lexeq(">>"))
           {quit=false;
            IfNotOperator2Arg(">>",10)
            {
            TwoParamInstrNoDouble(1,>>,">>",opvcpuSHR,0,10,false);
            }
           }
        }
        break;
       }



///////////////////////////////////////////////////////////////////////////
/////////////////////////////////11////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
  case 11://****** exp11
//exp11:= exp12 [+] exp11 | exp12     [+] =: +,-
       {BOOL quit=false;
        Dis(11);
        Read_BegExp(12,val);
        while(!quit)
        {quit=true;

         if (lexeq("+"))
           {quit=false;
            IfNotOperator2Arg("+",12)
            {
            if (scvalGetValType(val)!=valPOINTER)
              {
              TwoParamInstr(1,+,opvcpuADD,0,12,true,false);
              }
              else
              {   _value vl=*val;
                  int mul=4;
                  deb0("[Pointer+num]");
                  Advance;
                  {_value vt=*val;
                   sctypGoDeeper(&vt.type);
                   mul=scvalSizeOf(&vt);
                  }
                  Read_MusExp(12,&vt1);
                  if (scvalGetValType(&vt1)!=valINT)
                     serr(scErr_Operand,"Invalid 2nd argument to operator+");

                  scvalArrayToPointer(val);
                  scvalPutToRegister(val,TRUE);
                  if (mul&&scvalIsImmidiate(&vt1))
                     vt1.adr.val.ival*=mul,mul=0;
                  if (mul)
                     {_value vt2;
                      scvalPutToRegister(&vt1,TRUE);
                      Gen_Opcode(opvcpuMUL,&vt1,scvalSetINT(&vt2,mul));
                     }
                  Gen_Opcode(opvcpuADD,val,&vt1);
                  scvalFreeValue(&vt1);
                  val->sym=vl.sym;
                  val->type=vl.type;
              }
            }
           }else

         if (lexeq("-"))
           {quit=false;
            IfNotOperator2Arg("-",12)
            {
            if (scvalGetValType(val)==valPOINTER)
              serr(scErr_Operand, "Invalid pointer operation");
            TwoParamInstr(1,-,opvcpuSUB,0,12,false,false);
            }
           }
        }
        break;
       }



///////////////////////////////////////////////////////////////////////////
/////////////////////////////////12////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
  case 12://****** exp12
//exp12:= exp13 [*] exp12 | exp13     [*] =: *,/,%
       {BOOL quit=false;
        Dis(12);
        Read_BegExp(13,val);

        while(!quit)
        {quit=true;

        if (lexeq("*"))
           {quit=false;
            IfNotOperator2Arg("*",12)
            {
            if (scvalGetValType(val)==valPOINTER)
              serr(scErr_Operand, "Invalid pointer operation");
            TwoParamInstr(1,*,opvcpuMUL,1,12,true,false);
            }
           }else

        if (lexeq("/"))
           {quit=false;
            IfNotOperator2Arg("/",12)
            {
            if (scvalGetValType(val)==valPOINTER)
              serr(scErr_Operand, "Invalid pointer operation");
            TwoParamInstr(1,/,opvcpuDIV,1,12,false,true);
            }
           }else

        if (lexeq("%"))
           {quit=false;
            IfNotOperator2Arg("%",12)
            {
            if (scvalGetValType(val)==valPOINTER)
              serr(scErr_Operand, "Invalid pointer operation");
            TwoParamInstrNoDouble(1,%,"%%",opvcpuMOD,MAXINT,12,false);
            }
           }
        }
        break;
       }




///////////////////////////////////////////////////////////////////////////
/////////////////////////////////13////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
  case 13://****** exp13
/*exp13:= ~special~ : !exp14 | -exp14 | ++exp14 | exp14++ |
        --exp14 | exp14-- | +exp14 | -exp14 | *exp14 |
        (type)exp14 | sizeof(type) | exp14*/
       {scSymbol *sc;
        Dis(13);
        if (lexeq("!"))
           {Advance;Read_MusExp(14,val);
            TryOperator1Arg("!");
//            if (scvalIsImmidiate(val)&&(scvalGetValType(val)==valDOUBLE))
//               serr3(scErr_Operand, invalid_operand_to,"","unary !");
            if (scvalIsImmidiate(val))
            {switch(scvalGetValType(val))
             {
              case valDOUBLE:
              case valFIXED:
              case valFLOAT:
                serr3(scErr_Operand, invalid_operand_to,"","unary !");
              case valCHAR:
              case valSHORT:
              case valINT:val->adr.val.ival=!val->adr.val.ival;break;
              default:scError(sciReadSubExp);
             }
            }
            else
            {
             scvalPutToRegister(val,TRUE);
             Gen_Opcode(opvcpuNOT,val,NULL);
            }
           }else
        if (lexeq("~"))
           {Advance;Read_MusExp(13,val);
            TryOperator1Arg("~");
            if ((scvalGetValType(val)==valDOUBLE))
               serr3(scErr_Operand, invalid_operand_to,"","unary ~");
            scvalPutToRegister(val,TRUE);
            Gen_Opcode(opvcpuXOR,val,scvalSetINT(&vt1,-1));
           }else
        if (lexeq("-"))
           {Advance;Read_MusExp(14,val);
            TryOperator1Arg("-");
            if (scvalIsImmidiate(val))
            {switch(scvalGetValType(val))
             {
              case valDOUBLE:val->adr.val.dval=-val->adr.val.dval;break;
              case valFIXED:val->adr.val.ival=-val->adr.val.ival;break;
              case valFLOAT:val->adr.val.fval=-val->adr.val.fval;break;
              case valCHAR:
              case valSHORT:
              case valINT:val->adr.val.ival=-val->adr.val.ival;break;
              default:scError(sciReadSubExp);
             }
            }
            else
            {
             scvalPutToRegister(val,TRUE);
             Gen_Opcode(opvcpuNEG,val,NULL);
            }
           }else
        if (lexeq("++"))//wazne!:(x+(++x)) dla x=1 da 2+2 zamiast 1+2,czyli zle!!!
           {Advance;Read_MusExp(14,val);
            valNotConst(val);
            TryOperator1Arg("++");
            if (!(val->adr.flags&adrPOINTER)) serr(scErr_Operand, "Invalid lvalue in increment");
            scvalSetINT(&vt1,1);
            Gen_Opcode(opvcpuADD,val,&vt1);
            if (!CallerIgnore)
               scvalPutToRegister(val,TRUE);//not lvalue
           }else
        if (lexeq("--"))
           {Advance;Read_MusExp(14,val);
            valNotConst(val);
            TryOperator1Arg("--");
            if (!(val->adr.flags&adrPOINTER)) serr(scErr_Operand, "Invalid lvalue in decrement");
            scvalSetINT(&vt1,1);
            Gen_Opcode(opvcpuSUB,val,&vt1);
            if (!CallerIgnore)
                scvalPutToRegister(val,TRUE);//not lvalue
           }else
        if (lexeq("+"))
           {Advance;Read_MusExp(14,val);//do nothing
           }else
        if (lexeq("-"))//?????????????????????
           {Advance;Read_MusExp(14,val);
            serr(scErr_Operand, "Unknown unary -");
           }else
        if (lexeq("&"))//unary
           {
            Advance;Read_MusExp(14,val);
            scvalGetAddress(val,0);
            return 1;
           }else
        if (lexeq("*"))//
           {int countaster=0;
            while(lexeq("*"))
            {countaster++;
             Advance;
            }//while "*"
//            countaster--;
            Read_MusExp(14,val);
            for(;countaster;countaster--)
            {
             if (!val->type.params&&val->type.params[0]!='['&&val->type.params[0]!='*')
                  serr(scErr_Operand, "Not a pointer or table");
             if ((val->type.params[0]=='*'/*||(val->type.params[0]=='['&&val->type.params[1]==']')*/)
                &&(val->adr.flags&adrPOINTER))//mov a0,[x]
               {
                vt1=*val;
                val->adr.address=scvalFindFreeRegister(val);
                val->adr.flags=adrREGISTER;
                Gen_Opcode(opvcpuMOV,val,&vt1);
                scvalFreeValue(&vt1);
               }

//             val->adr.flags-=val->adr.flags&adrPOINTER;
             val->adr.flags|=adrCPU;//make [a0]
             sctypGoDeeper(&val->type);
            }//for
           }else
        if (lexeq("typeof"))
           {char *styp=scAlloc(16384);
            Advance;
            if (!lexeq("(")) serr2(scErr_Parse, parse_error,ST->lexema);
            Advance;scget_back(ST);
            if ((sc=get_definition_on_typeof()))//sctypGetDefinition()))
             {
              sctypTypeOfStr(&sc->type,NULL,styp);
              scdicFreeFunctionSymbol(sc);
              Advance;
             }else
             {
              Read_Expression(val,true);
              sctypTypeOfStr(&val->type,NULL,styp);
              //Advance;
             }
            deb1("typeof=%s\n",styp);
            if (!styp||!styp[0])
               serr(scErr_Parse, "typeof() requires type or variable");
            scvalConstantString(val,styp);

            if (!lexeq(")"))serr2(scErr_Parse, "`%s' instead of `)'",ST->lexema);
            Advance;
            return 1;
           }else
        if (lexeq("sizeof"))
           {
            int i,j=0;
            Advance;
            if (!lexeq("(")) serr2(scErr_Parse, parse_error,ST->lexema);
            Advance;scget_back(ST);
            if ((sc=sctypGetDefinition()))
             {scType typ=sc->type;
              if (sctypIsReference(&typ))
               {sctypGoDeeper(&typ);
               }
              i=sctypSizeOf(&typ);
              scdicFreeFunctionSymbol(sc);
              Advance;
             }else//expression - sizeof return
             {
              Read_Expression(val,true);
              if (sctypIsReference(&val->type))
               {sctypGoDeeper(&val->type);
               }
              i=scvalSizeOf(val);
              j=1;
              //Advance;
             }
             scvalSetINT(val,i);
             if (!lexeq(")"))serr2(scErr_Parse, "`%s' instead of `)'",ST->lexema);
             Advance;
             return 1;
           }else
        if (lexeq("(")&&(sc=get_definition_on_cast()))//casting
           {
            if (!lexeq(")")) serr2(scErr_Parse, "`%s' instead of `)'",ST->lexema);
            Advance;Advance;
            deb1("<Casting>%s",ST->lexema);
            Read_MusExp(13,val);//14?
            scvalCast(val,sc);
            return 1;
           }else
           {
            Read_BegExp(14,val);
            if (lexeq("++")) {_value vt2;Advance;
                valNotConst(val);
                TryOperator1ArgRight("++");
                if (!(val->adr.flags&adrPOINTER))
                   serr(scErr_Operand, "Invalid lvalue in increment");
                vt2=*val;
                if (!CallerIgnore)
                {
                scvalPutToRegister(val,FALSE);
                }
                scvalSetINT(&vt1,1);
                Gen_Opcode(opvcpuADD,&vt2,&vt1);
               }else
            if (lexeq("--")) {_value vt2;Advance;
                valNotConst(val);
                TryOperator1ArgRight("--");
                if (!(val->adr.flags&adrPOINTER))
                   serr(scErr_Operand, "Invalid lvalue in decrement");
//                CopyVal(&vt2,val);
                vt2=*val;
                if (!CallerIgnore)
                {
                scvalPutToRegister(val,FALSE);
                }
                scvalSetINT(&vt1,1);
                Gen_Opcode(opvcpuSUB,&vt2,&vt1);
               }
            return 1;
           }
        break;
       }


///////////////////////////////////////////////////////////////////////////
/////////////////////////////////14////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
  case 14://****** exp14
//exp14:= (expr) | exp15[expr] | exp15 -> exp14 | exp14 . exp15 | exp15
       {int read=0;
        Dis(14);
        if (lexeq("(")){
           Advance;
           Read_Expression(val,true);
           if (!lexeq(")")) serr(scErr_Parse, "`)' expected");
           Advance;
           deb1("Here is %s\n",ST->lexema);
           read=1;
           if (!(lexeq(".")||lexeq("->")||lexeq("[")))
                      return 1;
           }
        do{//-------------------------------------------------
        scSymbol *sc,*father=NULL,*find_among;
        char name[64];//1024:TODO
        _value thisval;
        _value scriptval;
        int i=0;
        scriptval.sym=NULL;
        if (!read) Read_BegExp(15,val);
        read=1;
        thisval=*val;
        if (lexeq("->")||lexeq("."))
        {
         int w=0;
         if (!scriptval.sym&&scvalGetValType(val)==valSCRIPT)
         {
          scriptval=*val;
          deb1("Accessing script %s\n",scriptval.sym->name);
         }
         if (lexeq("->"))
           {w=1;
            scvalDisplayValue("this(->)=",&thisval);
            father=val->sym;
            Advance;
            if (!val->type.params||val->type.params[0]!='*')
                 serr(scErr_Operand, "Not a pointer in `->'");
            if (val->type.params[0]=='*'&&(val->adr.flags&adrPOINTER))//mov a0,[x]
              {
               vt1=*val;
               val->adr.address=scvalFindFreeRegister(val);
               val->adr.flags=adrREGISTER;
               Gen_Opcode(opvcpuMOV,val,&vt1);
               scvalFreeValue(&vt1);
              }
            val->adr.flags|=adrCPU;
            deb0("(->)deeper:");
            sctypGoDeeper(&val->type);
            if (scvalGetValType(val)!=valSTRUCT&&scvalGetValType(val)!=valSCRIPT)
             {serr(scErr_Operand, "not a struct or script in binary `->'");
             }
           }
         if (!w&&lexeq("."))
           {
            scvalDisplayValue("this(.)=",&thisval);
            father=val->sym;
            Advance;
            if (scvalGetValType(val)!=valSTRUCT&&scvalGetValType(val)!=valSCRIPT)
             {serr(scErr_Operand, "not a struct or script in binary `.'");
             }
           }

         if (lexeq("~"))
         {int i;
          Advance;
          for(i=strlen(ST->lexema);i>=0;i--) ST->lexema[i+1]=ST->lexema[i];
          ST->lexema[0]='~';
         }
         if (lexeq("operator"))
         {char *x=scReadOperatorName();
          strcpy(ST->lexema,x);scFree(x);
         }

//         scAssert(SubExpr::Struct,scvalGetValType(val)!=valSCRIPT||(sc->adr.flags&adrSCRIPT));

         /*some tweaking for script before...
         if (scvalGetValType(val)==valSCRIPT)
         {
          val->type.flags=typPREDEFINED;
          val->type.main=(scSymbol *)valINT;
         }*/

         find_among=val->type.main;
         strcpy(name,ST->lexema);
         Advance;
         if (lexeq("("))
         {
         //if function is imported and we use it for the first time, add it to the
         //list of used imported functions
//         sciAddImportedFunction(sc,father->type.main);
//         val->adr=sc->adr;
//         val->sym=sc;
//         val->type=sc->type;
//         if (father->type.flags&typCONST)
//           val->type.flags|=typCONST;
          int *regs;
          scSymbol *sym;
          _value *params;

          regs=scvalPushUsedRegisters();
          params=scvalReadParameters();
          {char *sname=scAlloc(1024);
          sprintf(sname,"%%s is not a function member of `%s'.%%s",father->name);
          sym=scsFind_Function(name,find_among,NULL,params,sname);
          scFree(sname);
          }
          if (scriptval.sym)
            scvalCallFunction(sym,val,father,&thisval,&scriptval,params,CallerIgnore);
           else
            scvalCallFunction(sym,val,father,&thisval,NULL,params,CallerIgnore);
          scvalPopUsedRegisters(regs);
          Advance;
         }
         else
         {
          sc=scdicFoundSymbol(val->type.main,name);
          if (!sc) serr3(scErr_Operand, "`%s' is not a member of `%s'",name,father->name);

          scAssert(SubExpr::Struct,scvalGetValType(val)!=valSCRIPT||(sc->adr.flags&adrSCRIPT));

          //some tweaking for script before...
          if (scvalGetValType(val)==valSCRIPT)
          {
           val->type.flags=typPREDEFINED;//done above
           val->type.main=(scSymbol *)valINT;
           //mov aX,*this (holds DS);
           scvalSetRegister(&vt1,valINT,scvalFindFreeRegister(val));
           Gen_Opcode(opvcpuMOV,&vt1,val);
          }

          i=sc->adr.address;//the offset in structure

          //read the member
          if (i&&(val->adr.flags&adrREGISTER))
           {_value v1,v2;
            Gen_Opcode(opvcpuADD,scvalSetRegister(&v1,valINT,val->adr.address),scvalSetINT(&v2,i));
           }
           else
            val->adr.address+=i;

          if (scvalGetValType(&thisval)==valSCRIPT)//some more tweaking
          {
           //add aX,val;
           Gen_Opcode(opvcpuADD,&vt1,val);
           vt1.adr.flags|=adrCPU;
           *val=vt1;
          }//script
#if DEBUGLEVEL==0
          deb1("adr.flags=%d\n",val->adr.flags);
#endif
          val->sym=sc;
          val->type=sc->type;
          //CONST?
          if (father->type.flags&typCONST)
            val->type.flags|=typCONST;
//          Advance;//scget_back(ST);
         }
        }else//-> || .
        if (lexeq("["))
           {
           while(lexeq("["))
           {
            _value vt;
            int mul=4;//mul==0=>immidiate

            Advance;
            Read_Expression(&vt1,true);
            if (!lexeq("]")) serr(scErr_Parse, "`]' expected");
            //Operator
            if (scvalGetValType(val)==valSTRUCT)
            {
             if (!scsCallOperator("operator[]",val,&vt1,NULL,NULL,CallerIgnore))
              serr3(scErr_Operand, invalid_operand_to,"s","[]");
            }
            else
            {
             if (!val->type.params&&val->type.params[0]!='['&&val->type.params[0]!='*')
               serr(scErr_Operand, "Indexing not table or pointer");

             scvalDisplayValue("operator[]<-",val);

             //calculate offset
             vt=*val;
             sctypGoDeeper(&vt.type);
             mul=scvalSizeOf(&vt);

             if (mul&&scvalIsImmidiate(&vt1))
                vt1.adr.val.ival*=mul,mul=0;
             if (mul)scvalPutToRegister(&vt1,TRUE);
             if (mul&&mul!=1)
                {_value vt2;
                 Gen_Opcode(opvcpuMUL,&vt1,scvalSetINT(&vt2,mul));
                }

             if ((val->adr.flags&adrPOINTER))//mov a0,[x+v]
                {
                 if (val->type.params[0]=='[')
                 {
                   scType typbuf;
                   scType typINT={typPREDEFINED,NULL,(scSymbol*)valINT};
                   int flg=val->adr.flags;
                   /* Make it only the address */
                   val->adr.flags-=val->adr.flags&adrPOINTER;
                   val->adr.val.ival=val->adr.address;
                   typbuf=val->type;
                   val->type=vt.type;//New
                   if (!scvalIsImmidiate(val)&&!mul)
                   {mul=1;scvalPutToRegister(&vt1,TRUE);
                   }
                   if (mul)
                   {
                      vt1.type=typINT;
                      val->type=typINT;
                      Gen_Opcode(opvcpuADD,&vt1,val);
                      if (flg&adrIMMIDIATE) flg-=adrIMMIDIATE;
                      flg|=adrREGISTER;
                      vt1.adr.flags=flg;
                   }
                   else
                   {  val->adr.val.ival+=vt1.adr.val.ival;
                      flg-=flg&adrREGISTER;
                      flg|=adrIMMIDIATE;
                      vt1.adr.address=val->adr.val.ival;
                      vt1.adr.flags=flg;
                   }
                   val->type=typbuf;
                 }//[]
                 else
                 {//not []
                   scvalPutToRegister(&vt1,TRUE);
                   vt1.adr.flags=adrREGISTER;
                   Gen_Opcode(opvcpuADD,&vt1,val);
                   vt1.adr.flags|=adrCPU;
                 }
                 val->adr=vt1.adr;
                }
                else
                {//!adrPOINTER
                 if (val->adr.flags&adrREGISTER)
                 {
                   scAssert(sciReadSubExp::operator[],val->type.params&&val->type.params[0]=='*');
                   Gen_Opcode(opvcpuADD,val,&vt1);
                   val->adr.flags|=adrCPU;
                 }
                 else
                   serr(scErr_Operand, "Indexing not table nor pointer");
 //                serr(scErr_Internal, "internal compiler error at []\n");
                }
              sctypGoDeeper(&val->type);
            }//!operator[]
           }//while
           Advance;
           }//[]
           else
           return 1;//quit at end
        } while(1);
        break;
       }




///////////////////////////////////////////////////////////////////////////
/////////////////////////////////15////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
  case 15://****** exp15
/*exp15:= terminal | error
lvalue:= exp13

terminal := symbol (paramlist) | symbol | number | string
paramlist := exp1 , paramlist | exp1 | ""*/
       {
        scSymbol *sym,*in_struct=NULL,*find_among=NULL;
        _value thisval;
//        _value vt2;
        Dis(15);
                val->type.params=NULL;

//*************** STRING
        if (ST->lexema[0]=='"')
           {//String - in consts is stored! DS<0
            char *data;
            data=scStrdup(ST->lexema+1);
            data[strlen(data)-1]=0;//erase "
            scvalConstantString(val,data);
            Advance;
            return 1;
           }else
//*************** CHAR
        if (ST->lexema[0]=='\'')
           {char c=ST->lexema[1];
            if (c=='\\')
            {char *t;
             c=ST->lexema[2];
             if (isdigit(ST->lexema[0]))
             {
              c=(char)strtol(ST->lexema+2,&t,8);
             }
             else
             {
              switch(c)
              {
               case 'n':c='\n';
               case 'r':c='\r';
               case 't':c='\t';
              }
             }
            }
             else
             if (ST->lexema[2]!='\'')
                serr(scErr_Parse,"too long character constant");
            val->adr.val.ival=c;
            val->adr.flags=adrIMMIDIATE;
            val->type.flags=typPREDEFINED|typCONST;
            (valTYPE)val->type.main=valINT;
            val->sym=NULL;
            Advance;
            return 1;
           }
           else
//*************** NUMBER
        if (isdigit(ST->lexema[0]))
           {
            char *endp;
            val->type.params=NULL;
            if (strstr(ST->lexema,"."))
             {
              val->adr.val.dval=strtod(ST->lexema, &endp);
              val->adr.flags=adrIMMIDIATE;
              val->type.flags=typPREDEFINED|typCONST;
              (valTYPE)val->type.main=valDOUBLE;
              val->sym=NULL;
             }
             else
             {
              val->adr.val.ival=strtoul(ST->lexema, &endp, 0);
              val->adr.flags=adrIMMIDIATE;
              val->type.flags=typPREDEFINED|typCONST;
              (valTYPE)val->type.main=valINT;
              val->sym=NULL;
             }
            if (*endp) serr(scErr_Parse, "nondigits in number and not hexadecimal");
            Advance;
            return 1;
           }

           else

//*************** MEMBER-SYMBOL
           if ((this_symbol.reserved)&&
               (sym=scdicFoundSymbol(this_symbol.type.main->type.main,ST->lexema)))
            {
              _value vt1,vt2;
              scvalSetSymbol(val,&this_symbol);
              find_among=this_symbol.type.main->type.main;
              scvalGetValType(val);
              in_struct=&this_symbol;
              thisval=*val;

              deb2("Member-symbol:%s at [BP+8]+%d\n",sym->name,sym->adr.address);

                        if (!sctypIsFunction(&sym->type))
                        {
                                vt1=*val;
                                val->adr.address=scvalFindFreeRegister(val);
                                val->adr.flags=adrREGISTER;
                                Gen_Opcode(opvcpuMOV,val,&vt1);
                                scvalFreeValue(&vt1);
                                val->adr.flags|=adrCPU;
                                sctypGoDeeper(&val->type);
                                if (sym->adr.address)
                                   Gen_Opcode(opvcpuADD,scvalSetRegister(&vt1,valINT,val->adr.address),scvalSetINT(&vt2,sym->adr.address));
                        }
                        val->sym=sym;
                        val->type=sym->type;
                        goto continue_with_symbol;
           }
         else

//*************** SYMBOL
        if ((sym=scdicFoundSymbol(SC_symbols->first,ST->lexema)))
           {
            if (!sctypIsFunction(&sym->type))
               sciAddImportedFunction(sym,in_struct);
            scvalSetSymbol(val,sym);
            deb1("Symbol:%s\n",sym->name);
continue_with_symbol:
            scvalAccessReference(val);
//            scvalGetValType(val);//?
            Advance;
            if (!lexeq("("))//non-function
             {
              if (sctypIsFunction(&val->type))
              {//address of function - it'll be changed to int!
//               if (val->adr.flags&adrIMPORTED){
//               }
                serr(scErr_Operand, "At the moment getting the address of a function is not implemented!");
                /*serr2(scErr_Operand, "To obtain function address, use &%s\n"
                "WARNING:obtained address is CS+offset, you cannot directly pass it to scCall_Instance"
                ,sym->name);*/
              }
              if (val->adr.flags&adrIMPORTED){
//                scSymbol *s=val->sym;
                scType t=val->type;
                val->type.flags=typPREDEFINED;
                val->type.params=NULL;
                (valTYPE)val->type.main=valINT;
                deb0("Convert imported\n");
                scvalPutToRegister(val,TRUE);//yep!
                val->type=t;

                //val->adr.flags|=adrCPU;
                //val->sym=s;
              }
             }
             else
             {//function call
              int *regs;
              _value *params;
              scSymbol *sym1;


              regs=scvalPushUsedRegisters();
              params=scvalReadParameters();

              if (!sctypIsFunction(&sym->type))
              {
               if (sctypGetValType(&sym->type)==valSTRUCT)
               {
                //call operator()
                in_struct=sym;
                scvalGetValType(val);
                thisval=*val;
                sym1=scsFind_Function("operator()",val->type.main,NULL,params,"Proper `%s' could not be found.%s");
               }
               else
               serr2(scErr_Operand, "`%s' is not a function",sym->name);
              }
              else
               sym1=scsFind_Function(sym->name,find_among,SC_symbols->first,params,"`%s' could not be found.%s");

              scvalCallFunction(sym1,val,in_struct,&thisval,NULL,params,CallerIgnore);

              scvalPopUsedRegisters(regs);
              Advance;
             }
            return 1;
           }
           else
           {scSymbol *sc,*_this;

            if ((sc=get_definition_on_construct()))
            {int siz;deb0("Constructor?\n");
             scAssert(scRead_SubExp::constructor,lexeq("("));
             Advance;
             deb1("name=%s\n",sc->type.main->name);
             siz=scsGenStruct(&sc->type.main->type,val,&sc->type,false);
             val->type=sc->type;
             _this=sc->type.main;
             {
              int *regs;
              _value val1,val2;
              _value *params;//scAlloc(sizeof(_value));
              scSymbol *fsym;
              deb0("Found constructor!\n");
              regs=scvalPushUsedRegisters();
              params=scvalReadParameters();
              Advance;
              fsym=scsFind_Function(_this->name,_this->type.main,NULL,params,"Constructor `%s' couldn't be found.%s");
              val2=*val;
              scvalCallFunction(fsym,&val1,sc,&val2,NULL,params,false);
              scvalPopUsedRegisters(regs);
             }
             return 1;
            }
            deb0("Not an expression\n");
            if (!ExpRequired) return 0;
            if (IsSymbol(ST->lexema[0]))
//                serr2(scErr_Parse,parse_error,ST->lexema);
                  serr2(scErr_Parse,"Invalid expression separator `%s'",ST->lexema);
            if (sciValidIdentifier(ST->lexema))
              serr2(scErr_Operand, "`%s' undeclared",ST->lexema);
            serr2(scErr_Parse,parse_error,ST->lexema);
}

        break;
       }
}//switch
return 1;

return 0;
}

///////////////////////////////////////////////////////////////////////////
////Read_Expression////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int Read_Expression(_value *val,BOOL RequiredExp)
{
//*********** expr := exp1,expr | exp1

 if (sciReadSubExp(1,val,RequiredExp))
 {
  if ((!lexeq(";"))&&(!lexeq(")"))&&(!lexeq(","))&&(!lexeq("]"))&&(!lexeq(":"))&&(!lexeq("}")))
    {if (IsSymbol(ST->lexema[0]))
      serr2(scErr_Parse,"Invalid expression separator :`%s'",ST->lexema);
     serr2(scErr_Parse,parse_error,ST->lexema);
    }
  if (lexeq(",")) {deb0("Next expr after comma\n");Advance;return Read_Expression(val,true);}
  return 1;
 }
 return 0;

 return 0;
}
#undef CallerIgnore

///////////////////////////////////////////////////////////////////////////
////scvalPushUsedRegisters/////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
// it pushes all the used registers.
int *scvalPushUsedRegisters()
{ int i;
  int *regs;
  regs=scAlloc(240*sizeof(int));
  for (i=0;i<240;i++)
  {regs[i]=0;
   if (act.registers[i].used) //push it
    {
//     int ptr;
     _value valt;
//     ptr=act.registers[i].used->adr.flags&adrPOINTER;
//     act.registers[i].used->adr.flags^=ptr;
     Gen_Opcode(opvcpuPUSH,scvalSetRegister(&valt,valINT,i),NULL);
//     act.registers[i].used->adr.flags^=ptr;
     regs[i]=1;
     }
  }
  return regs;
}

///////////////////////////////////////////////////////////////////////////
////scvalPopUsedRegisters//////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void scvalPopUsedRegisters(int *regs)
{int i;
 for (i=239;i>=0;i--)
  {
   if (regs[i]) //pop it
    {//int ptr;
     _value valt;
//     ptr=act.registers[i].used->adr.flags&adrPOINTER;
//     act.registers[i].used->adr.flags^=ptr;
     Gen_Opcode(opvcpuPOP,scvalSetRegister(&valt,valINT,i),NULL);
//     act.registers[i].used->adr.flags^=ptr;
    }
  }
  scFree(regs);
}

///////////////////////////////////////////////////////////////////////////
////scvalReadParameters////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
/***********************************
 Reads function parameters to an array
 of _value and returns this array.
 This array may be freed.
*/
_value* scvalReadParameters()
{_value *vx,*v;
 int num=0;
 v=scAlloc(sizeof(_value)*256);
 deb0("Reading function parameters.\n");
 scAssert(scvalReadParameters,lexeq("("));
 Advance;

 while(!lexeq(")"))
 {
  sciReadSubExp(1,&(v[num++]),true);

  if (!lexeq(",")&&!lexeq(")"))
      serr2(scErr_Parse, parse_error,ST->lexema);
  if (lexeq(",")) Advance;
 }
 //end marker
 v[num].adr.flags=adrIMPORTED&adrTYPEONLY;
 v[num].type.flags=typINVALID;
 vx=scAlloc((num+1)*sizeof(_value));
 memcpy(vx,v,(num+1)*sizeof(_value));
 scFree(v);
 return vx;
}
///////////////////////////////////////////////////////////////////////////
////scsTypesExact//////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//t1 is formal, t2 is actual
BOOL scsTypesExact(scType* t1,scType* t2)
{
 int gvt1,gvt2;

 if (sctypIsReference(t1)||sctypIsReference(t2))
 {scType ct1=*t1,ct2=*t2;
  if (sctypIsReference(t1)) sctypGoDeeper(&ct1);
  if (sctypIsReference(t2)) sctypGoDeeper(&ct2);
  return scsTypesExact(&ct1,&ct2);
 }

 gvt1=sctypGetValType(t1);
 gvt2=sctypGetValType(t2);

 if (gvt1==gvt2)
 {switch(gvt1)
  {
   case valINT:
   case valLONG:
   case valCHAR:
   case valFIXED:
   case valFLOAT:
   case valDOUBLE:
   case valSHORT:
      return true;

   case valPOINTER:
      {scType ct1=*t1,ct2=*t2;
       sctypGoDeeper(&ct1);
       sctypGoDeeper(&ct2);
       return scsTypesExact(&ct1,&ct2);
      }
      return true;

   case valSTRUCT:
   case valSCRIPT:
      if (sctypIsReference(t1)||sctypIsReference(t2))
      {scType ct1=*t1,ct2=*t2;
       if (sctypIsReference(t1)) sctypGoDeeper(&ct1);
       if (sctypIsReference(t2)) sctypGoDeeper(&ct2);
       return scsTypesExact(&ct1,&ct2);
      }

      if (sctypSizeOf(t1)!=sctypSizeOf(t2)) return false;
      //TODO:do more proper check
      return true;
  }
 }

//this seems also 'exact'
 switch(gvt1)
 {
  case valDOUBLE:
  case valFLOAT:
  case valFIXED:
   switch(gvt2)
   {
     case valDOUBLE:
     case valFLOAT:
     case valFIXED:
     case valINT:
       return true;
     break;
   }
   break;
 }
 return false;
}

///////////////////////////////////////////////////////////////////////////
////scsCanClassCast////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static BOOL scsCanClassCast(scType *t1,scType *t2)
{
 int gvt1;

 gvt1=sctypGetValType(t1);
 switch(gvt1)
 {case valSTRUCT:
    {
     _value *params=scAlloc(2*sizeof(_value));
     scSymbol *fsym;
     scSymbol *_this;
     _this=sctypGetStruct(t1);
     scAssert(scsCanClassCast,_this);
     params[0].type=*t2;
     params[1].adr.flags=adrIMPORTED&adrTYPEONLY;
     params[1].type.flags=typINVALID;
     fsym=scsFind_Function(_this->name,_this->type.main,NULL,params,NULL);
     scFree(params);
     if (fsym) return true;
    }
   break;

 }
 return false;
}

///////////////////////////////////////////////////////////////////////////
////scsDoClassCast/////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static BOOL scsDoClassCast(scType *t1,_value *v,BOOL doTemporary)
{
 int gvt1;

 gvt1=sctypGetValType(t1);
 switch(gvt1)
 {case valSTRUCT:
    {
     _value *params=scAlloc(2*sizeof(_value));
     scSymbol *fsym;
     scSymbol *_this;
     _this=sctypGetStruct(t1);
     scAssert(scsCanClassCast,_this);
     params[0]=*v;
     params[1].adr.flags=adrIMPORTED&adrTYPEONLY;
     params[1].type.flags=typINVALID;
     fsym=scsFind_Function(_this->name,_this->type.main,NULL,params,NULL);
     if (fsym)
     {
          int *regs;
          _value thisval,valx;
/*          scSymbol sym;

          sym.name=NULL;
          sym.type.flags=typUSERDEFINED;
          sym.type.main=t1;*/

//          Gen_Opcode(opvcpuSUB,scvalSetRegister(&val1,valINT,regSP),scvalSetINT(&val2,type_size));
          deb0("Call constructor(...) for conversion\n");
          //siz=
          scsGenStruct(&t1->main->type,v,t1,!doTemporary);
          regs=scvalPushUsedRegisters();
          v->type=*t1;
          valx=*v;
          thisval=*v;
//          scvalFreeValue(&thisval);//xr_free register, where 'this' is stored
//          thisval=*v;
          scvalCallFunction(fsym,&valx,v->sym,&thisval,NULL,params,false);
          scvalPopUsedRegisters(regs);
          return true;
     }
    }
   break;

 }
 return false;
}

///////////////////////////////////////////////////////////////////////////
////scsTypesAlmostExact////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//t1 is formal, t2 is actual
BOOL scsTypesAlmostExact(scType* t1,scType* t2)
{int gvt1,gvt2;

 if (sctypIsReference(t1)||sctypIsReference(t2))
 {scType ct1=*t1,ct2=*t2;
  if (sctypIsReference(t1)) sctypGoDeeper(&ct1);
  if (sctypIsReference(t2)) sctypGoDeeper(&ct2);
  return scsTypesAlmostExact(&ct1,&ct2);
 }

 if (scsTypesExact(t1,t2)) return true;

 gvt1=sctypGetValType(t1);
 gvt2=sctypGetValType(t2);

 if (gvt1==gvt2)
 {switch(gvt1)
  {
   case valPOINTER:
      {scType ct1=*t1,ct2=*t2;
       sctypGoDeeper(&ct1);
       sctypGoDeeper(&ct2);
       if (sctypGetValType(&ct1)==valVOID
         ||sctypGetValType(&ct2)==valVOID) return true;//void can play both
       return scsTypesExact(&ct1,&ct2);
      }
      return true;
  }
 }

 if (gvt1==valSTRUCT)//try a constructor
 {
      return (scsCanClassCast(t1,t2));
 }

 return false;
}

///////////////////////////////////////////////////////////////////////////
////sctypExactFunction/////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
BOOL sctypExactFunction(scType *t1,scType *t2)
{
 BOOL after3dots=false;
 char *x1=t1->params,*x2=t2->params;
 if (*x1!='('||*x2!='(') return false;
 x1++;x2++;
 while (*x1!=')')
 {
  scSymbol *s1=NULL,*s2=NULL;
  _value *v1=NULL,*v2=NULL;

  if ((x1[0]=='.')&&(x1[1]=='.')&&(x1[2]=='.'))
  {x1+=3;
   if ((x2[0]!='.')||(x2[1]!='.')||(x2[2]!='.')) return false;
   x2+=3;
   after3dots=true;
  }

  if (*x2==')') return false;

  if (!after3dots)
  {
   s1=(scSymbol*)strtoul(x1, &x1, 16);
   s2=(scSymbol*)strtoul(x2, &x2, 16);
   if (*x1=='=') v1=(_value*)strtoul(++x1, &x1, 16);
   if (*x2=='=') v2=(_value*)strtoul(++x2, &x2, 16);
   if (v1&&v2)
   {
    if (!v1||!v2) return false;//all have to have this initializer
    //TODO:compare default initializers v1 and v2
   }
   if (!s1||!s2) return false;
  }

  if (!after3dots) scAssert(scsFunctionOK,s1&&s2);
  if (*x1==',') {
     x1++;
     if (*x2!=',') return false;
     x2++;
  }
  if (!after3dots&&!scsTypesExact(&s1->type,&s2->type)) return false;
 }
 if (*x2!=')') return false;
 return true;
}

///////////////////////////////////////////////////////////////////////////
////scsFunctionOK//////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
BOOL scsFunctionOK(scSymbol *fun,_value* params, BOOL (*typeCompare)(scType* t1,scType* t2))
{
 BOOL after3dots=false;
 char *x=fun->type.params;
 int i=0;

 scAssert(scsFunctionOK,x&&*x=='(');
 x++;
 while((params[i].adr.flags!=(adrIMPORTED&adrTYPEONLY))||
        params[i].type.flags!=typINVALID)
 {
  scSymbol *xs=NULL;
  _value *v=NULL;

  if ((x[0]=='.')&&(x[1]=='.')&&(x[2]=='.'))
  {x+=3;
   after3dots=true;
  }

  if (!after3dots)
  {
   if (*x==')')
       return false;
   xs=(scSymbol*)strtoul(x, &x, 16);//TODO-check if not number serr(
   if (*x=='=') v=(_value*)strtoul(++x, &x, 16);
  }

  scAssert(scsFunctionOK,*x==','||*x==')');
  if (!after3dots) scAssert(scsFunctionOK,xs);
  if (*x==',') x++;
  if (!after3dots&&!typeCompare(&xs->type,&params[i].type)) return false;
  i++;
 }
 //skip autoinitializers...
 if (*x!='.' && *x!=')')
     {scSymbol *xs;
      _value *v;
      while (*x!=')'||*x=='.')
      {
       xs=(scSymbol*)strtoul(x, &x, 16);
       if (*x=='=') {v=(_value*)strtoul(++x, &x, 16);}
            else return false;
       scAssert(scsFunctionOK,*x==','||*x==')');
       if (*x==',') x++;
      }
     }

 if (x[0]=='.'&&x[1]=='.'&&x[2]=='.') x+=3;
 if (*x!=')') return false;

 return true;
}

///////////////////////////////////////////////////////////////////////////
////scsFindFunction1Pass///////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int scsFindFunction1Pass(char *name,scSymbol **found,scSymbol *find_among,scSymbol *also_find,_value *params,BOOL (*typeCompare)(scType* t1,scType* t2))
{ int fi=0,in_also=0;
  scSymbol *p;

  p=find_among;

  while(p)
   {

    if (strcmp(p->name,name)==0)
     {char buf[1024];
      sctypTypeOfStr(&p->type,name,buf);
      deb1("Try %s?",buf);
      if (scsFunctionOK(p,params,typeCompare))
      {
      found[fi]=p;fi++;
      deb0("Yes\n");
      }
       else
      deb0("No\n");
     }

    p=p->next;

    if (!p&&!in_also)
    {p=also_find;in_also=1;
    }
   }
 return fi;
}

///////////////////////////////////////////////////////////////////////////
////scsFind_Function///////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//desc==NULL - if not found returns NULL,
//else serr(...,desc,func_name,additional_ending
//so desc should have two '%s'
scSymbol *scsFind_Function(char *name,scSymbol *find_among,scSymbol *also_find,_value *params,char *desc)
{
  scSymbol *found[256];
  char func[256];
  int fi=0;

  if (!find_among)
     find_among=also_find,also_find=NULL;
  deb1("Searching function '%s'..\n",name);

//find the exact one
  fi=scsFindFunction1Pass(name,found,find_among,also_find,params,&scsTypesExact);
  if (!fi)
  fi=scsFindFunction1Pass(name,found,find_among,also_find,params,&scsTypesAlmostExact);
  if (fi==1) return found[0];

  //we didn't find exactly one passing to this, so generate error message:
  //get
  {
   int i=0;
   strcpy(func,name);
   strcpy(func+strlen(func),"(");

   while((params[i].adr.flags!=(adrIMPORTED&adrTYPEONLY))||
          params[i].type.flags!=typINVALID)
   {
    if (i) strcpy(func+strlen(func),",");
    sctypTypeOfStr(&params[i].type,NULL,func+strlen(func));
    i++;
   }
   strcpy(func+strlen(func),")");
  }

  if (fi>1)
   {int i=0;
    char buf[1024];
    for(i=0;i<fi;i++)
    {
        strcpy(buf+strlen(buf),"\n");
        sctypTypeOfStr(&found[i]->type,name,buf+strlen(buf));
    }
    serr4(scErr_Operand,"Unable to decide which of %d candidates for function '%s' to use:%s",scStrdup(func),fi,scStrdup(buf));
   }
  deb1("No function passing to '%s' has been found\n",func);
  if (!desc) return NULL;

  {
    int in_also=0;
    scSymbol *p;
    char buf[1024];
    buf[0]=0;

    p=find_among;

    while(p)
     {

      if (strcmp(p->name,name)==0)
       {
        if (!buf[0])
            strcpy(buf,"\nPossible candidates:");
        strcpy(buf+strlen(buf),"\n");
        sctypTypeOfStr(&p->type,name,buf+strlen(buf));
       }

      p=p->next;

      if (!p&&!in_also)
      {p=also_find;in_also=1;
      }
     }
    if (!buf[0])
    {serr3(scErr_Operand,desc,func,"");}
    else
    {
     serr3(scErr_Operand,desc,func,buf);}
  }
  return NULL;
}

///////////////////////////////////////////////////////////////////////////
////sctypFind_Function/////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
scSymbol *sctypFindExactFunction(char *name,scSymbol *find_among,scSymbol *also_find,scType *params)
{ scSymbol *p;
  scSymbol *found=NULL;
  int in_also=0;
  char buf[1024];
  if (!find_among)
     find_among=also_find,also_find=NULL;
  p=find_among;
  sctypTypeOfStr(params,name,buf);
  deb1("Searching function %s..\n",buf);
//find the exact one
  while(p)
   {
    if (strcmp(p->name,name)==0)
     {
      sctypTypeOfStr(&p->type,name,buf);
      deb1("Try %s?",buf);
      if (sctypExactFunction(&p->type,params))
      {if (found)
        serr2(scErr_Operand,"Too many same functions '%s'",name);
       found=p;deb0("Yes\n");
      }else deb0("No\n");
     }

    p=p->next;

    if (!p&&!in_also)
    {p=also_find;in_also=1;
    }
   }
  return found;
}

///////////////////////////////////////////////////////////////////////////
////scvalPushParameters////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
#define MAX_PARAMS 256
static void scvalPushParameters(_value* fun,_value *params,int *size,char **param_end)
{int i=0,k,l;
 scSymbol* xs=NULL;
 _value *xv=NULL;
 _value temp;
 scType formal[MAX_PARAMS];
 _value *def_val[MAX_PARAMS];
 int after3dots=0;
 char *x=fun->type.params;

 deb0("Pushing parameters.\n");
 scAssert(scvalPushParameters1,*x=='(');
 x++;
 *size=0;
 while((params[i].adr.flags!=(adrIMPORTED&adrTYPEONLY))||
        params[i].type.flags!=typINVALID)
 {
  xs=NULL;xv=NULL;
  formal[i].flags=typINVALID;

  if ((x[0]=='.')&&(x[1]=='.')&&(x[2]=='.'))
  {x+=3;
   after3dots=i+1;
  }

  if (!after3dots)
  {
   if (*x==')')
       serr2(scErr_Operand, "Too many parameters to function `%s'",fun->sym->name);
   xs=(scSymbol*)strtoul(x, &x, 16);//TODO-check if not number serr(
   if (*x=='=') {
     xv=(_value*)strtoul(++x, &x, 16);
     def_val[i]=xv;
     }
     else
     def_val[i]=NULL;
   formal[i]=xs->type;

  }

  scAssert(scvalPushParameters,*x==','||*x==')');
  if (*x==',') x++;
  i++;
 }
// l=i?(--i):i;
 l=--i;//number of actual agruments
// deb2("Arguments:%d:%d\n",l,i);

 //count autoinitializers...
 if (*x!='.' && *x!=')')
     {scSymbol *xs;
      _value *v;
      while (*x!=')'||*x=='.')
      {
       xs=(scSymbol*)strtoul(x, &x, 16);
       i++;
       scAssert(scvalPushParameters,*x=='=');
       v=(_value*)strtoul(++x, &x, 16);
       def_val[i]=v;
       formal[i]=xs->type;
       scAssert(scvalPushParameters,*x==','||*x==')');
       if (*x==',') x++;
      }
     }

//here we assume arguments are OK. No check (why duplicate it, it was done
//before in scsFunctionOK).
// if (x[0]=='.'&&x[1]=='.'&&x[2]=='.') x+=3;
// if (*x!=')')
//     serr2(scErr_Operand, "Too few parameters to function `%s'",fun->sym->name);

//convert params:
 {
  int z;
  for(z=0;z<=i;z++)
  {_value *v=&params[z];
   if (z>l) params[z]=*def_val[z];
   if (!(after3dots&&z+1>=after3dots)&&!scsTypesExact(&formal[z],&v->type))
      {
         if (sctypGetValType(&formal[z])==valSTRUCT&&sctypIsReference(&formal[z]))
             {//non-reference should be converted while pushing
              scAssert(scvalPushParameters,scsDoClassCast(&formal[z],v,true));//create temporary for further delete
             }
      }
  }
 }

//push params
// deb3("With autos:%d:%d,after3dots=%d\n",l,i,after3dots);
 for(;i>=0;i--)
 {   _value *v=&params[i];
//     deb1("Arg=%d\n",i);
     if (i>l) v=def_val[i];

     scAssert(scvalPushParameters,v);

     if (scvalGetValType(v)==valFLOAT)
        {scvalConvToDouble(v);}
     if (scvalGetValType(v)==valPOINTER)
        {scvalArrayToPointer(v);}

     if (after3dots&&i+1>=after3dots)
        k=scvalSizeOf(v);
     else
        k=sctypSizeOf(&formal[i]);

     if (k<4) k=4;
     *size+=k;

     if (!(after3dots&&i+1>=after3dots))
     {scSymbol xs;xs.type=formal[i];
     xs.name=NULL;
     if (!scvalSameType(v,scvalSetSymbol(&temp,&xs)))
        serr3(scErr_Operand, "Incompatible type for parameter %d to function %s",i+1,fun->sym->name);
     if (sctypIsReference(&formal[i]))
        {//scvalGetAddress(v,2);
         v->adr.flags|=adrADDRESS;
        }
     }

/*     if ( !(after3dots&&i+1>=after3dots) &&
         sctypGetValType(&formal[i])==valSTRUCT &&
         !scsTypesExact(&formal[i],&v->type)
     {//try to cast to structure
      scAssert(scvalPushParameters,scsDoClassCast(&formal[i],v));
     }*/

     if (((after3dots&&i+1>=after3dots)||!sctypIsReference(&formal[i])))
     {_value v1,v2;
      int type_size=scvalSizeOf(v);
      deb1("Parameter [%d]\n",i);
      //try dedicated copy constructor
      if ((after3dots&&i+1>=after3dots) ||
         !scsDoClassCast(&formal[i],v,false))
      {
      //default copy constructor
      if (scvalSizeOf(v)>4&&scvalSizeOf(v)!=8)//push something bigger
      {
       Gen_Opcode(opvcpuMOV,scvalSetRegister(&v1,valINT,regCX),scvalSetINT(&v2,type_size));
       Gen_Opcode(opvcpuSUB,scvalSetRegister(&v1,valINT,regSP),scvalSetRegister(&v2,valINT,regCX));
       scvalSetRegister(&v1,valINT,regSP);
       scvalPutToRegister(&v1,true);
       scvalSetRegister(&v2,valINT,regSS);
       Gen_Opcode(opvcpuADD,&v1,&v2);
       v1.adr.flags|=adrCPU;
       Gen_Opcode(opvcpuCOPY,&v1,v);
       scvalFreeValue(&v1);
      }
      else
        Gen_Opcode(opvcpuPUSH,v,NULL);
      }//!dedicated
      else
      {//dedicated, creation of temporary changed BP, now fix it
        int rv=sctypSizeOf(&formal[i]);
        if (rv%4) rv+=4-rv%4;
        act.afterbp+=rv;
      }
     }
     else
      Gen_Opcode(opvcpuPUSH,v,NULL);
     scvalFreeValue(v);
 }
 scFree(params);
 //find end of (..) in type
 *param_end=x+1;
 /*sym->type.params;
 while(**param_end!=')') (*param_end)++;
 (*param_end)++;*/
 if (!**param_end) *param_end=NULL;
}

///////////////////////////////////////////////////////////////////////////
////scvalCallFunction//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
/************************************************
sym-function symbol
val-return value will be stored here
in_struct-structure the function is member of
thisval  -'this' of the function
scriptval-script of the function
 Before calling this function, you should
 scvalPushUsedRegisters
 scvalReadParameters()
 scvalPopUsedRegisters
*/
void scvalCallFunction(scSymbol *sym,_value *val,scSymbol* in_struct,_value *thisval,_value *scriptval,_value *params,BOOL CallerIgnore)
{
  int siz=0;
  int ret_struct=0;
  _value ret_struct_val;
  scSymbol *in_script=NULL;
  _value vt1,vt2;
  int i;
  _value val2;
  _value this_val=*thisval;
  char *k;
  scAssert(scvalCallFunction,sym);
  sciAddImportedFunction(sym,in_struct);
  scvalSetSymbol(val,sym);
  if (!sctypIsFunction(&val->type))
      serr2(scErr_Operand, "Symbol `%s' is not a function",sym->name);

//push

  if (!sctypIsReference(&sym->type)&&sctypGetValType(&sym->type)==valSTRUCT)
  ret_struct=scsGenStruct(&sym->type.main->type,&ret_struct_val,&sym->type,false);


  scvalPushParameters(val,params,&siz,&k);

  scvalSetINT(&vt1,sym->adr.address);

  if (in_struct)
   {
        //check if it is from script
        scSymbol *is=in_struct;//HERE WE ARE
        if (!(is->adr.address&adrTYPEONLY)) is=is->type.main;
        while(is&&!in_script)
        {if ((is->type.flags&typTYPE)==typSCRIPT) in_script=is;
         is=is->parent;
        }
        if (
            ((in_struct->adr.address&adrTYPEONLY)&&(in_struct->type.flags&typSCRIPT))
            ||
            ((in_struct->type.main->adr.address&adrTYPEONLY)&&(in_struct->type.main->type.flags&typSCRIPT))
           )
        {deb0("I don't send 'this'.\n");
        }
        else
        {
        deb1("Here I push \"this\" (of %s) to the member function.\n",in_struct->name);
        if (sctypGetValType(&in_struct->type)==valSTRUCT)//not pointer
        {
        //scvalGetAddress(&this_val,1);
        this_val.adr.flags|=adrADDRESS;
        Gen_Opcode(opvcpuPUSH,&this_val,NULL);
        this_val.adr.flags^=adrADDRESS;
        }
        else
        Gen_Opcode(opvcpuPUSH,&this_val,NULL);
        siz+=4;
        }
   }

  if (ret_struct)
  {//call to function returning struct
   _value v1=ret_struct_val;
   deb0("Push the return address\n");
   //scvalGetAddress(&v1,1);
   v1.adr.flags|=adrADDRESS;
   Gen_Opcode(opvcpuPUSH,&v1,NULL);
   v1.adr.flags^=adrADDRESS;
   siz+=4;
  }

  if (in_script)
  {
   deb1("call from other script '%s'\n",in_script->name);
   {
    if (!(scriptval&&scriptval->sym))
       serr2(scErr_Operand, "Invalid script '%s' access",in_script->name);

    deb1("Pushing script '%s' descriptor\n",scriptval->sym->name);
    //scvalGetAddress(scriptval,1);
    scriptval->adr.flags|=adrADDRESS;
    Gen_Opcode(opvcpuPUSH,scriptval,NULL);
    scriptval->adr.flags^=adrADDRESS;
    siz+=4;
   }
  }

/* do the call */
  deb1("Calling function '%s'\n",sym->name);
  if (sym->adr.flags&adrIMPORTED)
  {unsigned int opt=siz;
   int fl=scvalGetValType(val);
   if (fl==valVOID)
        opt|=scDispatch_Void;
   if (fl==valDOUBLE||fl==valFLOAT)
        opt|=scDispatch_Double;
   if (in_struct)
        opt|=scDispatch_Member;
   if (ret_struct)
   {switch(ret_struct)
    {case 1:opt|=2<<22;break;
     case 2:opt|=3<<22;break;
     case 4:opt|=4<<22;break;
     case 8:opt|=5<<22;break;
     default:opt|=1<<22;
    }
   }
   opt|=((sym->adr.val.ival)<<28);//dispatcher nr.
   Gen_Opcode(opvcpuCALLEX,&vt1,scvalSetINT(&val2,opt));
  }
  else
  if (in_script)
  {unsigned int opt=siz;
   opt|=scDispatch_Script;
   Gen_Opcode(opvcpuCALLEX,&vt1,scvalSetINT(&val2,opt));
   //callex does himself add sp,paramcount
  }
  else
  {int i;
   i=act.ip;
   Gen_Opcode(opvcpuCALL,&vt1,NULL);
   if ((sym->adr.address<0)&&(!act.unreal)) sym->adr.address=-i;
   if (siz)//add sp,paramcount
   {
    scvalSetRegister(&vt1,valINT,regSP);
    scvalSetINT(&vt2,siz);
    Gen_Opcode(opvcpuADD,&vt1,&vt2);
   }
  }
  if (ret_struct)
  {
   val->sym=sym;
   val->type=sym->type;
   val->type.params=k;
   val->adr=ret_struct_val.adr;
  }
  else
  {
   val->type.params=k;

   if (sctypIsReference(&val->type)||scvalGetValType(val)!=valVOID)
   {
    if (scvalSizeOf(val)==8)
     {//deb0("dbl\n");
//      act.registers[1].used=val;
      i=scvalFindFreeDoubleRegister(val);
//      act.registers[1].used=NULL;
     }
    else
     {
      i=scvalFindFreeRegister(val);
     }

    scvalSetRegister(val,valINT,i);
    scvalSetRegister(&vt1,valINT,0);

    val->sym=sym;
    val->type=sym->type;
    val->type.params=k;
    vt1.type=val->type;


    if (i)
    {
     if (!CallerIgnore)
       Gen_Opcode(opvcpuMOV,val,&vt1);
    }
   }
   else
   {
    val->sym=sym;
    val->type=sym->type;
    val->type.params=k;
   }//void
  }
//pop
//  if (!CallerIgnore)
    scvalAccessReference(val);
}

///////////////////////////////////////////////////////////////////////////
////scsCallOperator////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static BOOL scsCallOperator(char *operatorX,_value *val,_value *v1,_value *v2,_value *v3,BOOL CallerIgnore)
{
  int vnum=0;
  scSymbol *father;

  father=val->sym;
  scAssert(scsCallOperator,father);

  if (scdicFoundSymbol(val->type.main,operatorX))
  {//call it
    int *regs;
    _value thisval;
    _value *params=scAlloc(5*sizeof(_value));
    scSymbol *fsym;

    deb1("call %s\n",operatorX);

    if (v1)
      vnum++,params[0]=*v1;
    if (v2)
      vnum++,params[1]=*v2;
    if (v3)
      vnum++,params[2]=*v3;
    params[vnum].adr.flags=adrIMPORTED&adrTYPEONLY;
    params[vnum].type.flags=typINVALID;

    thisval=*val;

    regs=scvalPushUsedRegisters();

    fsym=scsFind_Function(operatorX,val->type.main,NULL,params,"Suitable `%s' could not be found.%s");
    if (!fsym)
       serr2(scErr_Operand,"Unable to find suitable %s",operatorX);

    scvalCallFunction(fsym,val,father,&thisval,NULL,params,CallerIgnore);
    {
      char buf[1024];
      sctypTypeOfStr(&val->type,NULL,buf);
      deb1("Result of operator is %s?",buf);
    }
    scvalPopUsedRegisters(regs);
    return true;
  }

  return false;
}


///////////////////////////////////////////////////////////////////////////
////scsGenStruct///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//does not affect val->type
//temptype=stype->type.main;
static int scsGenStruct(scType *stype,_value *val,scType *temptype,BOOL toStack)
{_value v1,v2;
 int ret_struct;
 int rv;
 scSymbol *tempo;
 rv=ret_struct=sctypSizeOf(stype);
 if (rv%4) rv+=4-rv%4;

 //subl ret_struct,%esp
 deb1("Generating space for struct at bp+%d.\n",act.afterbp-rv);
 if (rv==4)
 Gen_Opcode(opvcpuPUSH,scvalSetINT(&v1,0),NULL);
 else
 Gen_Opcode(opvcpuSUB,scvalSetRegister(&v1,valINT,regSP),scvalSetINT(&v2,rv));
 scvalSetINT(val,0);

// if (!toStack)
 {
     val->adr.address=act.afterbp-rv;
     act.afterbp-=rv;
     val->adr.flags=adrBP+adrIMMIDIATE;
 }
/* else
 {//TODO:
     _value vt1;

     scvalSetRegister(val,valINT,regSP);
     scvalPutToRegister(val,TRUE);
     Gen_Opcode(opvcpuADD,val,scvalSetRegister(&vt1,valINT,regSS));
 }*/

 tempo=New(scSymbol);
 tempo->name=NULL;
 tempo->adr=val->adr;
 tempo->type=*temptype;//sym->type
 val->sym=tempo;
 if (!toStack)
   scdicAddSymbol_Ex(SC_temporary,tempo);
 return ret_struct;
}
