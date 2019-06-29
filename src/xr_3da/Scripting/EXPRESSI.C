/************************************************************************
       -= SEER - C-Scripting engine v 0.9a =-
State      : under implementation
Author     : Przemyslaw Podsiadly
             email: ppodsiad@elka.pw.edu.pl
             WWW:   http://home.elka.pw.edu.pl/~ppodsiad
File:expressi.c
Desc:calculating expressions
************************************************************************/
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
exp13:= ~special~ : !exp14 | -exp14 | ++exp14 | exp14++ |
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
//{debprintf("%d;",x);}
//getchar();}
char *asterisk="*";
scType CharAsteriskType={
typPREDEFINED,"*",(scSymbol *)valCHAR
};
scSymbol CharAsterisk={
{0,0,{0}},NULL,0,NULL,
{typPREDEFINED,"*",(scSymbol *)valCHAR},
NULL,NULL
};
///////////////////////////////////////////////////////////////////////////

//przemienne-

#define TwoParamInstr(isneutral,operand,opvcpuCODE,neutral,ThisSubExpr,przemienne,divs)\
            Advance;                                                     \
            if (!valIsImmidiate(val)) valPutToRegister(val,TRUE);        \
            Read_MusExp(ThisSubExpr,&vt1);cerr;                          \
            if (valIsImmidiate(val)&&valIsImmidiate(&vt1))               \
            {                                                            \
             BothTypesVal(val,&vt1);cerr;                                \
             if (GetValType(val)==valINT)                                \
             {                                                           \
             if (divs&&vt1.adr.val.ival==0)                              \
                serr(scErr_Operand,"Division by zero");                  \
             val->adr.val.ival=val->adr.val.ival operand vt1.adr.val.ival;\
             }                                                           \
             else                                                        \
             {                                                           \
             if (divs&&vt1.adr.val.dval==0)                              \
                serr(scErr_Operand,"Division by zero");                  \
             val->adr.val.dval=val->adr.val.dval operand vt1.adr.val.dval;\
             }                                                           \
             FreeValue(&vt1);                                            \
             return 1;                                                   \
            }                                                            \
            deb0("x\n");\
            /*Optimize*/                                                 \
            /*if ((!(vt1.adr.flags&adrPointer) &&vt1.adr.flags&adrRegister)\
               ||valIsImmidiate(val))                                    \
              if (przemienne) XChgVal(val,&vt1);*/                       \
            if (isneutral&&valIsImmidiate(&vt1)&&                        \
               ((valImmidiateValue(&vt1)==neutral)))                     \
               return 1;                                                 \
            valPutToRegister(val,TRUE);cerr;                             \
            Gen_Opcode(opvcpuCODE,val,&vt1);cerr;                        \
            FreeValue(&vt1);

#define TwoParamInstrWithSkip(isneutral,operand,opvcpuCODE,neutral,skipper,ThisSubExpr)  \
           {                                                             \
            _value vt2;/*Check if skip*/                                 \
            int oip,tip;                                                 \
            oip=act.ip;                                                  \
            if (!valIsImmidiate(val)) valPutToRegister(val,TRUE);        \
            valSetINT(&vt2,0);                                           \
            if (skipper)                                                 \
            Gen_Opcode(opvcpuJTRUE,val,&vt2);/*order to skip*/           \
            else                                                         \
            Gen_Opcode(opvcpuJFALSE,val,&vt2);/*order to skip*/          \
            cerr;Advance;                                                \
            Read_MusExp(ThisSubExpr,&vt1);cerr;                          \
            valConvTo_Int(&vt1);cerr;                                    \
            if (valIsImmidiate(val)&&valIsImmidiate(&vt1))               \
            {BothTypesVal(val,&vt1);                                     \
             if (GetValType(val)==valINT)                                \
             val->adr.val.ival=val->adr.val.ival operand vt1.adr.val.ival;           \
             else                                                        \
             val->adr.val.dval=val->adr.val.dval operand vt1.adr.val.dval;           \
             FreeValue(&vt1);                                            \
             return 1;                                                   \
            }                                                            \
                                                                         \
            if (!(isneutral&&valIsImmidiate(&vt1)&&(valImmidiateValue(&vt1)==neutral)))    \
            {                                                            \
            Gen_Opcode(opvcpuCODE,val,&vt1);cerr;                        \
            }                                                            \
            tip=act.ip;act.ip=oip;                                       \
            valSetINT(&vt2,tip);                                         \
            if (skipper)                                                 \
            Gen_Opcode(opvcpuJTRUE,val,&vt2);/*order to skip*/           \
            else                                                         \
            Gen_Opcode(opvcpuJFALSE,val,&vt2);/*order to skip*/          \
            cerr;act.ip=tip;                                             \
            FreeValue(&vt1);                                             \
           }


#define TwoParamInstrNoDouble(isneutral,operand,operstr,opvcpuCODE,neutral,ThisSubExpr,przemienne) \
            Advance;                                                     \
            if (!valIsImmidiate(val)) valPutToRegister(val,TRUE);        \
            Read_MusExp(ThisSubExpr,&vt1);cerr;                          \
            if (floating(GetValType(val))||floating(GetValType(&vt1)))   \
                serr3(scErr_Operand,invalid_operand_to,"s","binary "operstr);\
            if (valIsImmidiate(val)&&valIsImmidiate(&vt1))               \
            {BothTypesVal(val,&vt1);                                     \
             if (GetValType(val)==valDOUBLE)                             \
                serr3(scErr_Operand,invalid_operand_to,"s","binary "operstr);\
             val->adr.val.ival##operand##=vt1.adr.val.ival;                      \
             FreeValue(&vt1);                                            \
             return 1;                                                   \
            } /*Optimize*/                                               \
            /*if ((!(vt1.flags&valPointer)&&vt1.flags&valRegister)       \
               ||valIsANumber(val))                                      \
              if (przemienne) XChgVal(val,&vt1);*/                       \
            if (isneutral&&valIsImmidiate(&vt1)&&(vt1.adr.val.ival==neutral))\
               return 1;                                                 \
            valPutToRegister(val,TRUE);cerr;                             \
            Gen_Opcode(opvcpuCODE,val,&vt1);                             \
            cerr;FreeValue(&vt1);

///////////////////////////////////////////////////////////////////////////
int Read_Expression(_value *val,bool RequiredExp);

scSymbol* get_definition_on_cast()
{scSymbol *t=NULL;
 scPosition p;
 scget_back(ST);
 scGetPosition(ST,&p);
 Advance;
 t=get_definition();
 if (!t) {
  scSetPosition(p);
  Advance;
 }

scError_Exit:;
 return t;
}

///////////////////////////////////////////////////////////////////////////
///////////////Read_SubExp/////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
#define Read_BegExp(a,b) {if (!Read_SubExp(a,b,ExpRequired)) return 0;}
#define Read_MusExp(a,b)   {Read_SubExp(a,b,true);}
int Read_SubExp(int i,_value *val,bool ExpRequired)
{_value vt1={{/*ADDRESS*/0,adrREGISTER,{0}},{/*type*/typPREDEFINED,NULL,valINT},NULL};
 scPosition befpos;
//void scSetPosition(scPosition n);
//void scGetPosition(Source_Text *ST,scPosition *n);
 scget_back(ST);
 scGetPosition(ST,&befpos);
 Advance;
 cerr;
// scSetPosition(befpos);
 switch(i)
 {



#define AfterLValueRead if (!(val->adr.flags&adrPOINTER)) serr(scErr_Operand, "Invalid lvalue in assignment");\
                        Advance;Read_Expression(&vt1,true);cerr;
///////////////////////////////////////////////////////////////////////////
/////////////////////////////////1////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
  case 1://****** exp1
//exp1 := lvalue [=] expr | exp2      [=] =: =,+=,-=,*=,/=,%=,&=,^=,|=,<<=,>>=
       {Dis(1);
        Read_BegExp(2,val);cerr;//lvalue
        if (lexeq("=")){
           AfterLValueRead
           valArrayToPointer(&vt1);
           if (GetValType(&vt1)==valSTRUCT||GetValType(val)==valSTRUCT)
             {
             if (valSizeOf(&vt1)!=valSizeOf(val))
              {serr(scErr_Operand, "Incompatible struct in copy.");}
              else
              {int size;
               if ((size=valSizeOf(val))==4)
                  Gen_Opcode(opvcpuMOV,val,&vt1);
               else
                  {_value v1,v2;
                  Gen_Opcode(opvcpuMOV,valSetRegister(&v1,valINT,regCX),valSetINT(&v2,size));
                  Gen_Opcode(opvcpuCOPY,val,&vt1);cerr;
//                  serr(scErr_Operand, "Copying structure not implemented.");
                  }
               cerr;
               return 1;
              }
             }
           Gen_Opcode(opvcpuMOV,val,&vt1);cerr;
           FreeValue(&vt1);
           }else
        if (lexeq("+=")){
           AfterLValueRead
//           if (GetValType(&vt1)==valPointer)//werr-warning
//              werr(scErr_Operand, "Invalid pointer operation");
           if (valIsImmidiate(&vt1)&&(valImmidiateValue(&vt1)==0)) return 1;
           if (GetValType(&vt1)==valSTRUCT||GetValType(val)==valSTRUCT)
               serr3(scErr_Operand, invalid_operand_to,"s","+=");
           Gen_Opcode(opvcpuADD,val,&vt1);cerr;
           FreeValue(&vt1);
           }else
        if (lexeq("-=")){
           AfterLValueRead
           if (valIsImmidiate(&vt1)&&(valImmidiateValue(&vt1)==0)) return 1;
           if (GetValType(&vt1)==valSTRUCT||GetValType(val)==valSTRUCT)
               serr3(scErr_Operand, invalid_operand_to,"s","-=");
           Gen_Opcode(opvcpuSUB,val,&vt1);cerr;
           FreeValue(&vt1);
           }else
        if (lexeq("*=")){
           AfterLValueRead
           if (valIsImmidiate(&vt1)&&(valImmidiateValue(&vt1)==1)) return 1;
           if (GetValType(&vt1)==valSTRUCT||GetValType(val)==valSTRUCT)
               serr3(scErr_Operand, invalid_operand_to,"s","*=");
           Gen_Opcode(opvcpuMUL,val,&vt1);cerr;
           FreeValue(&vt1);
           }else
        if (lexeq("/=")){
           AfterLValueRead
           if (valIsImmidiate(&vt1)&&(valImmidiateValue(&vt1)==1)) return 1;
           if (GetValType(&vt1)==valSTRUCT||GetValType(val)==valSTRUCT)
               serr3(scErr_Operand, invalid_operand_to,"s","/=");
           Gen_Opcode(opvcpuDIV,val,&vt1);cerr;
           FreeValue(&vt1);
           }else
        if (lexeq("%=")){
           AfterLValueRead
           if (GetValType(&vt1)==valSTRUCT||GetValType(val)==valSTRUCT)
               serr3(scErr_Operand, invalid_operand_to,"s","*=");
           Gen_Opcode(opvcpuMOD,val,&vt1);cerr;
           FreeValue(&vt1);
           }else
        if (lexeq("&=")){
           AfterLValueRead
           Gen_Opcode(opvcpuAND,val,&vt1);cerr;
           FreeValue(&vt1);
           }else
        if (lexeq("|=")){
           AfterLValueRead
           if (valIsImmidiate(&vt1)&&(valImmidiateValue(&vt1)==0)) return 1;
           Gen_Opcode(opvcpuOR,val,&vt1);cerr;
           FreeValue(&vt1);
           }else
        if (lexeq("^=")){
           AfterLValueRead
           if (valIsImmidiateOfType(&vt1,valINT)&&(valImmidiateValue(&vt1)==0)) return 1;
           Gen_Opcode(opvcpuXOR,val,&vt1);cerr;
           FreeValue(&vt1);
           }else
        if (lexeq("<<=")){
           AfterLValueRead
           if (valIsImmidiateOfType(&vt1,valINT)&&(valImmidiateValue(&vt1)==1)) return 1;
           Gen_Opcode(opvcpuSHL,val,&vt1);cerr;
           FreeValue(&vt1);
           }else
        if (lexeq(">>=")){
           AfterLValueRead
           if (valIsImmidiateOfType(&vt1,valINT)&&(valImmidiateValue(&vt1)==1)) return 1;
           Gen_Opcode(opvcpuSHR,val,&vt1);cerr;
           FreeValue(&vt1);
           }
        break;
       }




///////////////////////////////////////////////////////////////////////////
/////////////////////////////////2////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
  case 2://****** exp2
//exp2 := exp3? exp3 : exp3 | exp3
       {Dis(2);
        Read_BegExp(3,val);cerr;
        if (lexeq("?"))
           {_value vt2;
            int tip,oip1,oip2;
              valSetINT(&vt2,0);
              Gen_Align();
              valConvTo_Int(val);//HERE
              valPutToRegister(val,TRUE);cerr;
              oip1=act.ip;
              Gen_Opcode(opvcpuJFALSE,val,&vt2);
            Advance;
            Read_MusExp(3,&vt1);cerr;
            valPutToRegister(&vt1,TRUE);cerr;
            if (!lexeq(":")) serr(scErr_Parse, "`:' expected");
              oip2=act.ip;
              Gen_Opcode(opvcpuJMP,&vt2,NULL);
              Gen_Align();
              tip=act.ip;act.ip=oip1;
              valSetINT(&vt2,tip);
              Gen_Opcode(opvcpuJFALSE,val,&vt2);
              act.ip=tip;
            Advance;
            Read_MusExp(3,&vt2);cerr;

            if (vt2.adr.flags&adrREGISTER&&vt2.adr.address==vt1.adr.address)//Same reg
            {}
            else
            {
             Gen_Opcode(opvcpuMOV,&vt1,&vt2);
             FreeValue(&vt2);
            }
              Gen_Align();
              tip=act.ip;act.ip=oip2;
              valSetINT(&vt2,tip);
              Gen_Opcode(opvcpuJMP,&vt2,NULL);
              act.ip=tip;
              *val=vt1;
            //teraz zrob, zeby i vt1 i vt2 wpisywaly do tego samego rejestru wynik
           }
        break;
       }



///////////////////////////////////////////////////////////////////////////
/////////////////////////////////3////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
  case 3://****** exp3
//exp3 := exp4 [||] exp3 | exp4
       {Dis(3);
        Read_BegExp(4,val);cerr;
        if (lexeq("||"))
           {//Advance;Read_SubExp(3,&vt1);
            valConvTo_Int(val);cerr;
            TwoParamInstrWithSkip(1,||,opvcpuORL,0,1,3);
           }
        break;
       }


///////////////////////////////////////////////////////////////////////////
/////////////////////////////////4////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
  case 4://****** exp4 (if first 0, second is not processed!,val =0)
//exp4 := exp5 [&&] exp4 | exp5
       {Dis(4);
        Read_BegExp(5,val);cerr;
        if (lexeq("&&"))
           {//Advance;Read_SubExp(4,&vt1);
            valConvTo_Int(val);cerr;
            TwoParamInstrWithSkip(1,&&,opvcpuANDL,1,0,4);
           }
        break;
       }



///////////////////////////////////////////////////////////////////////////
/////////////////////////////////5////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
  case 5://****** exp5
//exp5 := exp6 [|] exp5 | exp6
       {Dis(5);
        Read_BegExp(6,val);cerr;
        if (lexeq("|"))
           {//Advance;Read_SubExp(5,&vt1);
            TwoParamInstrNoDouble(1,|,"|",opvcpuOR,0,5,true);
           }
        break;
       }

///////////////////////////////////////////////////////////////////////////
/////////////////////////////////6////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
  case 6://****** exp6
//exp6 := exp7 [^] exp6 | exp7
       {Dis(6);
        Read_BegExp(7,val);cerr;
        if (lexeq("^"))
           {//Advance;Read_SubExp(6,&vt1);
            TwoParamInstrNoDouble(1,^,"^",opvcpuXOR,0,6,true);
           }
        break;
       }



///////////////////////////////////////////////////////////////////////////
/////////////////////////////////7////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
  case 7://****** exp7
//exp7 := exp8 [&] exp7 | exp8
       {Dis(7);
        Read_BegExp(8,val);cerr;
        if (lexeq("&"))
           {//Advance;Read_SubExp(7,&vt1);
            TwoParamInstrNoDouble(1,&,"&",opvcpuAND,0xffffffff,7,false);
           }
        break;
       }



///////////////////////////////////////////////////////////////////////////
/////////////////////////////////8////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
  case 8://****** exp8
//exp8 := exp9 [==] exp8 | exp9      [==] =: ==,!=
       {Dis(8);
        Read_BegExp(9,val);cerr;
        if (lexeq("=="))
           {//Advance;Read_SubExp(8,&vt1);
            TwoParamInstr(0,==,opvcpuCMPE,0,8,true,false);
            val->type.flags=typPREDEFINED;(valTYPE)val->type.main=valINT;
           }else
        if (lexeq("!="))
           {
            TwoParamInstr(0,!=,opvcpuCMPE,0,8,true,false);
            if(!valIsImmidiate(val))Gen_Opcode(opvcpuNOT,val,NULL);
            val->type.flags=typPREDEFINED;(valTYPE)val->type.main=valINT;
           }
        break;
       }



///////////////////////////////////////////////////////////////////////////
/////////////////////////////////9////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
  case 9://****** exp9
//exp9 := exp10 [<] exp9 | exp10      [<] =: <,<=,>,>=
       {Dis(9);
        Read_BegExp(10,val);cerr;
        if (lexeq("<"))
           {
            TwoParamInstr(0,<,opvcpuCMPL,0,9,false,false);
            val->type.flags=typPREDEFINED;(valTYPE)val->type.main=valINT;
           }else
        if (lexeq("<="))
           {
            TwoParamInstr(0,<=,opvcpuCMPNG,0,9,false,false);
            val->type.flags=typPREDEFINED;(valTYPE)val->type.main=valINT;
           }else
        if (lexeq(">"))
           {
            TwoParamInstr(0,>,opvcpuCMPG,0,9,false,false);
            val->type.flags=typPREDEFINED;(valTYPE)val->type.main=valINT;
           }else
        if (lexeq(">="))
           {
            TwoParamInstr(0,>=,opvcpuCMPNL,0,9,false,false);
            val->type.flags=typPREDEFINED;(valTYPE)val->type.main=valINT;
           }
        break;
       }



///////////////////////////////////////////////////////////////////////////
/////////////////////////////////10////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
  case 10://****** exp10
//exp10:= exp11 [<<] exp10 | exp11
       {Dis(10);
        Read_BegExp(11,val);cerr;
        if (lexeq("<<"))
           {//Advance;Read_SubExp(10,&vt1);
            TwoParamInstrNoDouble(1,<<,"<<",opvcpuSHL,0,10,false);
           }else
        if (lexeq(">>"))
           {//Advance;Read_SubExp(10,&vt1);
            TwoParamInstrNoDouble(1,>>,">>",opvcpuSHR,0,10,false);
           }
        break;
       }



///////////////////////////////////////////////////////////////////////////
/////////////////////////////////11////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
  case 11://****** exp11
//exp11:= exp12 [+] exp11 | exp12     [+] =: +,-
       {Dis(11);
        Read_BegExp(12,val);cerr;
        if (lexeq("+"))
           {
            if (GetValType(val)!=valPOINTER)
              {
              TwoParamInstr(1,+,opvcpuADD,0,11,true,false);
              }
              else
              {   _value vl=*val;
                  int mul=4;
                  deb0("[Pointer+num]");
                  Advance;
                  {_value vt=*val;
                   valGoDeeper(&vt);cerr;
                   mul=valSizeOf(&vt);cerr;
                  }
                  Read_MusExp(11,&vt1);cerr;
                  valArrayToPointer(val);
                  valPutToRegister(val,TRUE);cerr;
                  if (mul&&valIsImmidiate(&vt1))
                     vt1.adr.val.ival*=mul,mul=0;
                  if (mul)
                     {_value vt2;
                      valPutToRegister(&vt1,TRUE);cerr;
                      Gen_Opcode(opvcpuMUL,&vt1,valSetINT(&vt2,mul));
                     }
                  Gen_Opcode(opvcpuADD,val,&vt1);
                  FreeValue(&vt1);
                  val->sym=vl.sym;
                  val->type=vl.type;
            }
           }else
        if (lexeq("-"))
           {
            if (GetValType(val)==valPOINTER)
              serr(scErr_Operand, "Invalid pointer operation");
            TwoParamInstr(1,-,opvcpuSUB,0,11,false,false);
           }
        break;
       }



///////////////////////////////////////////////////////////////////////////
/////////////////////////////////12////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
  case 12://****** exp12
//exp12:= exp13 [*] exp12 | exp13     [*] =: *,/,%
       {Dis(12);
        Read_BegExp(13,val);cerr;
        if (lexeq("*"))
           {
            if (GetValType(val)==valPOINTER)
              serr(scErr_Operand, "Invalid pointer operation");
            TwoParamInstr(1,*,opvcpuMUL,1,12,true,false);
           }else
        if (lexeq("/"))
           {
            if (GetValType(val)==valPOINTER)
              serr(scErr_Operand, "Invalid pointer operation");
            TwoParamInstr(1,/,opvcpuDIV,1,12,false,true);
           }else
        if (lexeq("%"))
           {
            if (GetValType(val)==valPOINTER)
              serr(scErr_Operand, "Invalid pointer operation");
            TwoParamInstrNoDouble(1,%,"%%",opvcpuMOD,MAXINT,12,false);
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
           {Advance;Read_MusExp(14,val);cerr;
            if (valIsImmidiate(val)&&(GetValType(val)==valDOUBLE))
               serr3(scErr_Operand, invalid_operand_to,"","unary !");
            valPutToRegister(val,TRUE);cerr;
            Gen_Opcode(opvcpuNOT,val,NULL);
           }else
        if (lexeq("-"))
           {Advance;Read_MusExp(14,val);cerr;
            if (valIsImmidiate(val))
            {switch(GetValType(val))
             {
              case valDOUBLE:val->adr.val.dval=-val->adr.val.dval;break;
              case valFIXED:val->adr.val.ival=-val->adr.val.ival;break;
              case valFLOAT:val->adr.val.fval=-val->adr.val.fval;break;
              case valCHAR:
              case valSHORT:
              case valINT:val->adr.val.ival=-val->adr.val.ival;break;
              default:scError(Read_SubExp);
             }
            }
            else
            {
             valPutToRegister(val,TRUE);cerr;
             Gen_Opcode(opvcpuNEG,val,NULL);cerr;
            }
           }else
        if (lexeq("++"))//wazne!:(x+(++x)) dla x=1 da 2+2 zamiast 1+2,czyli zle!!!
           {Advance;Read_MusExp(14,val);cerr;
            if (!(val->adr.flags&adrPOINTER)) serr(scErr_Operand, "Invalid lvalue in increment");
            valSetINT(&vt1,1);
            Gen_Opcode(opvcpuADD,val,&vt1);
            valPutToRegister(val,TRUE);cerr;//not lvalue
           }else
        if (lexeq("--"))
           {Advance;Read_MusExp(14,val);cerr;
            if (!(val->adr.flags&adrPOINTER)) serr(scErr_Operand, "Invalid lvalue in decrement");
            valSetINT(&vt1,1);
            Gen_Opcode(opvcpuSUB,val,&vt1);
            valPutToRegister(val,TRUE);cerr;//not lvalue
           }else
        if (lexeq("+"))
           {Advance;Read_MusExp(14,val);cerr;//do nothing
           }else
        if (lexeq("-"))//?????????????????????
           {Advance;Read_MusExp(14,val);cerr;
             serr(scErr_Operand, "Unknown unary -");
           }else
        if (lexeq("&"))//unary
           {int r=0;
            Advance;Read_MusExp(14,val);cerr;
            if (!(val->adr.flags&adrPOINTER))
             serr(scErr_Operand, "Not a lvalue to unary &");
//            val->flags^=valPointer;//erase pointer
            if ((val->adr.flags&adrPOINTER)==adrDS) r=regDS;
            if ((val->adr.flags&adrPOINTER)==adrBP) r=regBP;
            if ((val->adr.flags&adrPOINTER)==adrCS) r=regCS;//seems it's a function
//            valPutToRegister(val,TRUE);
            val->sym=NULL;//erase sym
            val->adr.flags-=val->adr.flags&adrPOINTER;
            val->adr.val.ival=val->adr.address;
            if (!val->type.params||!val->type.params[0])
            val->type.params=CharAsteriskType.params;//set to "*"
            else
            {char *t=scAlloc(strlen(val->type.params)+2);
             strcpy(t+1,val->type.params);
             *t='*';
             val->type.params=t;
            }
//           if (val->flags&valPointer)
//              serr(scErr_Internal, "internal compiler error at unary &\n");
            if (r)
            {
             valPutToRegister(val,TRUE);
             valSetRegister(&vt1,valINT,r);
             Gen_Opcode(opvcpuADD,val,&vt1);cerr;
            }
            return 1;
           }else
        if (lexeq("*"))//TODO -OK!
           {int countaster=0;
            while(lexeq("*"))
            {countaster++;
             Advance;
            }//while "*"
//            countaster--;
            Read_MusExp(14,val);cerr;
            for(;countaster;countaster--)
            {
             if (!val->type.params&&val->type.params[0]!='['&&val->type.params[0]!='*')
                  serr(scErr_Operand, "Not a pointer or table");
             if ((val->type.params[0]=='*'||(val->type.params[0]=='['&&val->type.params[1]==']'))
                &&(val->adr.flags&adrPOINTER))//mov a0,[x]
               {
                vt1=*val;
                val->adr.address=FindFreeRegister(val);
                val->adr.flags=adrREGISTER;
                Gen_Opcode(opvcpuMOV,val,&vt1);
                FreeValue(&vt1);
               }
             val->adr.flags-=val->adr.flags&adrPOINTER;cerr;
             valGoDeeper(val);cerr;
             //rekurencja!!!
            }//for
           }else
        if (lexeq("sizeof"))//type TODO
           {
            int i,j=0;
            Advance;
            if (!lexeq("(")) serr2(scErr_Parse, parse_error,ST->lexema);
            Advance;scget_back(ST);
            if ((sc=get_definition()))
             {cerr;
              i=SizeOf(&sc->type);cerr;
              free_Function_Symbol(sc);
              Advance;
             }else//expression - sizeof return
             {
              Read_Expression(val,true);
              i=valSizeOf(val);cerr;
              j=1;
              //Advance;
             }
             valSetINT(val,i);
             if (!lexeq(")"))serr2(scErr_Parse, "`%s' instead of `)'",ST->lexema);
             Advance;
             return 1;
           }else
        if (lexeq("(")&&(sc=get_definition_on_cast()))//type TODO
           {
            if (!lexeq(")")) serr2(scErr_Parse, "`%s' instead of `)'",ST->lexema);
            Advance;Advance;
            deb1("<Casting>%s",ST->lexema);
            Read_MusExp(14,val);cerr;
            valCast(val,sc);cerr;
            return 1;
           }else
           {
            Read_BegExp(14,val);cerr;
            if (lexeq("++")) {_value vt2;Advance;
                if (!(val->adr.flags&adrPOINTER)) serr(scErr_Operand, "Invalid lvalue in increment");
                vt2=*val;
                valPutToRegister(val,FALSE);
                valSetINT(&vt1,1);cerr;
                Gen_Opcode(opvcpuADD,&vt2,&vt1);
               }else
            if (lexeq("--")) {_value vt2;Advance;
                if (!(val->adr.flags&adrPOINTER)) serr(scErr_Operand, "Invalid lvalue in decrement");
//                CopyVal(&vt2,val);
                vt2=*val;
                valPutToRegister(val,FALSE);
                valSetINT(&vt1,1);
                Gen_Opcode(opvcpuSUB,&vt2,&vt1);cerr;
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
           Read_Expression(val,true);cerr;
           if (!lexeq(")")) serr(scErr_Parse, "`)' expected");
           Advance;
           read=1;
           if (lexeq(".")||lexeq("->")||lexeq("["))
           {}
           else
           return 1;
           }
        do{//-------------------------------------------------
        if (!read) Read_BegExp(15,val);cerr;
        read=1;
        if (lexeq("->"))//TODO
           {scSymbol *sc,*father;
            int i=0;deb0("->!");
            Advance;//Read_SubExp(15,&vt1);
            {
             if (!val->type.params||val->type.params[0]!='*')
                  serr(scErr_Operand, "Not a pointer in `->'");
             if (val->type.params[0]=='*'&&(val->adr.flags&adrPOINTER))//mov a0,[x]
               {
                vt1=*val;
                val->adr.address=FindFreeRegister(val);
                val->adr.flags=adrREGISTER;
                Gen_Opcode(opvcpuMOV,val,&vt1);
                FreeValue(&vt1);cerr;
               }
             val->adr.flags|=adrCPU;
             deb0("(->)deeper:");
             valGoDeeper(val);
             //rekurencja!!!
             }
            deb1("struct:where=%s,",val->type.params);
            deb1("struct `%s'?",val->sym->name);
            if (GetValType(val)!=valSTRUCT)
             {serr(scErr_Operand, "not a struct or union in binary `->'");
             }
            deb0("yes\n");
            sc=val->type.main;
            val->type=val->type.main->type;//get to the type symbol
            father=val->sym;
//            val->sym=sc;
//            sc=sc->add->main;
            while (sc)
            {
             if (lexeq(sc->name))
               {deb2("to member %s at %d\n",sc->name,i);break;}
             i+=SizeOf(&sc->type);sc=sc->next;
            }
            if (!sc) serr3(scErr_Operand, "`%s' is not a member of `%s'",ST->lexema,father->name);
            if (val->adr.flags&adrREGISTER)
             {_value vt1,vt2;
              Gen_Opcode(opvcpuADD,valSetRegister(&vt1,valINT,val->adr.address),valSetINT(&vt2,i));
             }
             else
             {
              val->adr.address+=i;
             }
            val->sym=sc;
            val->type=sc->type;
//            deb("member where=%s\n",val->where);
            Advance;//scget_back(ST);
//            deb("next1=%s\n",ST->lexema);
//            return 1;
           }else



        if (lexeq("."))
           {scSymbol *sc,*father;
            int i=0;
            Advance;//Read_SubExp(15,&vt1);
//            deb("where=%s;",val->where);
//            deb("struct `%s'?",val->sym->name);
            if (GetValType(val)!=valSTRUCT)
             {serr(scErr_Operand, "not a struct or union in binary `.'");
             }
            sc=val->type.main;//get to the type symbol
            scAssert(Read_SubExp::Struct,sc);
            father=val->sym;
//            val->sym=sc;
//            sc=sc->add->main;
            while (sc)
            {
             if (lexeq(sc->name))
              {
//               deb("to member %s at %d\n",sc->name,i);
               break;
              }
             i+=SizeOf(&sc->type);sc=sc->next;
            }
            if (!sc) serr3(scErr_Operand, "`%s' is not a member of `%s'",ST->lexema,father->name);
            if (i&&(val->adr.flags&adrREGISTER))
             {_value vt1,vt2;
              Gen_Opcode(opvcpuADD,valSetRegister(&vt1,valINT,val->adr.address),valSetINT(&vt2,i));
             }
             else
             {
              val->adr.address+=i;
             }
            val->sym=sc;
            val->type=sc->type;
//            deb("member where=%s\n",val->where);
            Advance;//scget_back(ST);
//            deb("next2=%s\n",ST->lexema);
//            return 1;
           }else


        if (lexeq("["))//TODO
           {
           while(lexeq("["))
           {Advance;
           Read_Expression(&vt1,true);cerr;
           if (!lexeq("]")) serr(scErr_Parse, "`]' expected");

             if ((val->adr.flags&adrPOINTER))//mov a0,[x+v]
               {int mul=4;//mul==0=>immidiate
                _value vt=*val;
                valGoDeeper(&vt);
                mul=valSizeOf(&vt);cerr;

                if (mul&&valIsImmidiate(&vt1))
                   vt1.adr.val.ival*=mul,mul=0;
                if (mul)valPutToRegister(&vt1,TRUE);cerr;
                if (mul&&mul!=1)
                   {_value vt2;
                    Gen_Opcode(opvcpuMUL,&vt1,valSetINT(&vt2,mul));
                   }
                if (!val->type.params&&val->type.params[0]!='['&&val->type.params[0]!='*')
                  serr(scErr_Operand, "Indexing not table or pointer");
                if (val->type.params[0]=='[')
                {//HERE
//                  _value val1;
//                  Gen_Opcode(opvcpuADD,&vt1,valSetINT(&val1,val->adr.address));

                  scType typbuf;
                  scType typINT={typPREDEFINED,NULL,(scSymbol*)valINT};
                  int flg=val->adr.flags;
                  /* Make it only the address */
                  val->adr.flags-=val->adr.flags&adrPOINTER;
                  val->adr.val.ival=val->adr.address;
                  typbuf=val->type;
                  val->type=vt.type;//NEW
                  if (!valIsImmidiate(val)&&!mul)
                  {mul=1;valPutToRegister(&vt1,TRUE);cerr;
                  }
//                  val->type=typbuf;
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
                }
                else
                {
                  //if (!valIsImmidiate(val))
                  valPutToRegister(&vt1,TRUE);
                  vt1.adr.flags=adrREGISTER;
                  Gen_Opcode(opvcpuADD,&vt1,val);
                  vt1.adr.flags|=adrCPU;
                }
                val->adr=vt1.adr;
               }
             valGoDeeper(val);
             cerr;
           }//while
           Advance;
           }
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
        scSymbol *sym;
        _value vt2;
        Dis(15);
                val->type.params=NULL;

//*************** STRING
        if (ST->lexema[0]=='"')
           {//String - in consts is stored! DS<0
            char *data;
//            int adr;
            data=strdup(ST->lexema+1);
            data[strlen(data)-1]=0;//erase "
//            debprintf("[string=%s]",data);
            valSetINT(val,act.consts.pos);
            valPutToRegister(val,TRUE);cerr;
            val->sym=NULL;
            val->type=CharAsterisk.type;
            valSetRegister(&vt1,valINT,regES);
            Gen_Opcode(opvcpuADD,val,&vt1);
            pushstr_pmem(&act.consts,data);
            scFree(data);
            Advance;
            return 1;
           }else
//*************** CHAR
        if (ST->lexema[0]=='\'')
           {char c=ST->lexema[1];
            if (c=='\\')
             {char *t;
              c=ST->lexema[2];
              if (isdigit(ST->lexema[0])) c=(char)strtol(ST->lexema+2,&t,8);
             }
             else
             if (ST->lexema[2]!='\'')
                serr(scErr_Parse,"too long character constant");
            val->adr.val.ival=c;
            val->adr.flags=adrIMMIDIATE;
            val->type.flags=typPREDEFINED;
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
//            debprintf("Digit: %s ,",ST->lexema);//getchar();
//            deb("[Imidiate:%s]",ST->lexema);
                        val->type.params=NULL;
            if (strstr(ST->lexema,"."))
             {
              val->adr.val.dval=strtod(ST->lexema, &endp);
              val->adr.flags=adrIMMIDIATE;
              val->type.flags=typPREDEFINED;
              (valTYPE)val->type.main=valDOUBLE;
              val->sym=NULL;
              if (*endp) serr(scErr_Parse, "nondigits in floating number");
             }
             else
             {
              val->adr.val.ival=strtol(ST->lexema, &endp, 0);
              val->adr.flags=adrIMMIDIATE;
              val->type.flags=typPREDEFINED;
              (valTYPE)val->type.main=valINT;
              val->sym=NULL;
              if (*endp) serr(scErr_Parse, "nondigits in number and not hexadecimal");
             }
            Advance;
//            i=read_integer(ST,char *s,int *integ)
//            debprintf("next=`%s'\n",ST->lexema);
            return 1;
           }

           else

//*************** SYMBOL
        if ((sym=Found_Symbol(symbols,ST->lexema)))
           {
//            debprintf("Symbol:`%s'; ",sym->name);//getchar();
//            deb("[Symbol:%s at %d]",ST->lexema,sym->address);
//            val->adr.address=sym->address;
//            val->adr.flags=adrPOINTER|adrIMMIDIATE|(sym->flags&(4+8));
            val->type=sym->type;
            val->adr=sym->adr;
            val->sym=sym;
            deb1("Symbol:%s\n",sym->name);
            GetValType(val);cerr;
            Advance;
            if (!lexeq("("))//non-function
             {
              if (val->type.params&&val->type.params[0]=='(')
              {//address of function - it'll be changed to int!
//               if (val->adr.flags&adrIMPORTED){
//               }
                serr2(scErr_Operand, "To obtain function address, use &%s\n"
                "WARNING:obtained address is CS+offset, you cannot directly pass it to scCall_Instance"
                ,sym->name);
              }
              if (val->adr.flags&adrIMPORTED){
//                scSymbol *s=val->sym;
                deb0("IMPORTED!");
                valPutToRegister(val,TRUE);//yep!
                cerr;
                //val->adr.flags|=adrCPU;
                //val->sym=s;
              }
             }
             else
             {
              int parnum=0,siz=0;
              int regs[240],i;
              _value val2;
              char *k;
              if ((!val->type.params)||val->type.params[0]!='(')
                  serr2(scErr_Operand, "Symbol `%s' is not a function",sym->name);
              Advance;
              k=val->type.params+1;

              /*Push all registers*/
              for (i=0;i<240;i++)
              {regs[i]=0;
               if (act.registers[i].used) //push it
                {//scSymbol *sm;
//                 sm=act.registers[i].used->sym;
//                 act.registers[i].used->sym=NULL;
                 int ptr;
                 _value valt;
                 ptr=act.registers[i].used->adr.flags&adrPOINTER;
                 act.registers[i].used->adr.flags^=ptr;
//                 Gen_Opcode(opvcpuPUSH,act.registers[i].used,NULL);
                 Gen_Opcode(opvcpuPUSH,valSetRegister(&valt,valINT,i),NULL);
                 act.registers[i].used->adr.flags^=ptr;
//                 act.registers[i].used->sym=sm;
//                 Gen_Opcode(opvcpuPUSH,act.registers[i].used,NULL);
                 regs[i]=1;
                 }
              }
              PushParameters(val,&parnum,&k,&siz,sym);cerr;
              Advance;
              if ((!parnum)&&val->type.params[1]!=')')
                 serr2(scErr_Operand, "Function `%s' requires parameters",sym->name);
//              if (*k!=')')
//                 serr2( "Too few parameters to function `%s'",sym->name);
              valSetINT(&vt1,sym->adr.address);
              if (sym->adr.flags&adrIMPORTED)
              {int fl=GetValType(val);
               if (fl==valDOUBLE||fl==valFLOAT)
               Gen_Opcode(opvcpuDCALLEX,&vt1,valSetINT(&val2,siz));
               else
               Gen_Opcode(opvcpuCALLEX,&vt1,valSetINT(&val2,siz));
              }
              else
              {int i=act.ip;
               Gen_Opcode(opvcpuCALL,&vt1,NULL);
               if ((sym->adr.address<0)&&(!act.unreal)) sym->adr.address=-i;
              }
              val->type.params=k+1;
              if (valSizeOf(val)==8)
               {deb0("dbl\n");
                act.registers[1].used=val;
                i=FindFreeDoubleRegister(val);
               }
              else
               {
                i=FindFreeRegister(val);
               }
              cerr;
              valSetRegister(val,valINT,i);
              valSetRegister(&vt1,valINT,0);
              cerr;
//              vt1.sym=sym;
//              vt1.type=sym->type;
              val->sym=sym;
              val->type=sym->type;
              val->type.params=k+1;
              vt1.type=val->type;
              if (i)
                Gen_Opcode(opvcpuMOV,val,&vt1);
//              if (sym->add)
//              val->where=sym->add->queue;
//              deb("Type=%s\n",ValTypeStr(GetValType(val)));
//              deb("Function returning %s\n",val->where);
              if (siz)
              {
               valSetRegister(&vt1,valINT,regSP);
               valSetINT(&vt2,siz);
               Gen_Opcode(opvcpuADD,&vt1,&vt2);
              }
              for (i=239;i>=0;i--)
              {
               if (regs[i]) //pop it
                {int ptr;
                 _value valt;
                 ptr=act.registers[i].used->adr.flags&adrPOINTER;
                 act.registers[i].used->adr.flags^=ptr;
                 Gen_Opcode(opvcpuPOP,valSetRegister(&valt,valINT,i),NULL);
//                 Gen_Opcode(opvcpuPOP,act.registers[i].used,NULL);
//                 act.registers[i].used->sym=sm;
                 act.registers[i].used->adr.flags^=ptr;
                }
              }cerr;
             }
//            debprintf("next=`%s'\n",ST->lexema);
//            debif (!val->where) debprintf("!NULL!\n");
            return 1;
           }
           else
           {if (!ExpRequired) return 0;
            if (IsSymbol(ST->lexema[0]))
//                serr2(scErr_Parse,parse_error,ST->lexema);
                  serr2(scErr_Parse,"Invalid expression separator :`%s'",ST->lexema);
            serr2(scErr_Operand, "`%s' undeclared",ST->lexema);
           }

        break;
       }
}//switch
return 1;
scError_Exit:;
return 0;
}

///////////////////////////////////////////////////////////////////////////
////Read_Expression////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int Read_Expression(_value *val,bool RequiredExp)
{
//*********** expr := exp1,expr | exp1
 cerr;
 if (Read_SubExp(1,val,RequiredExp))
 {cerr;
  if ((!lexeq(";"))&&(!lexeq(")"))&&(!lexeq(","))&&(!lexeq("]"))&&(!lexeq(":"))&&(!lexeq("}")))
    {if (IsSymbol(ST->lexema[0]))
      serr2(scErr_Parse,"Invalid expression separator :`%s'",ST->lexema);
     serr2(scErr_Parse,parse_error,ST->lexema);
    }
  if (lexeq(",")) {Advance;return Read_Expression(val,true);}
  return 1;
 }
 return 0;
scError_Exit:;
 return 0;
}

