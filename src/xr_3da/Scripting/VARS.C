/************************************************************************
       -= SEER - C-Scripting engine v 0.3a =-
State      : under implementation
Author     : Przemyslaw Podsiadly
             email: ppodsiad@elka.pw.edu.pl
             WWW:   http://home.elka.pw.edu.pl/~ppodsiad
File:vars.c
Desc:working on `_value's
************************************************************************/
#define INTERNAL_FILE
#define EXACT_DEBUG
#include <stdio.h>
#include <seer.h>
#include "internal.h"
#include "code.h"

#define TYPESTR type->params

///////////////////////////////////////////////////////////////////////////
////sctypIsReference///////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
BOOL sctypIsReference(scType *type)
{
 scAssert(sctypIsReference,type);
 if (TYPESTR&&TYPESTR[0])
  { if (TYPESTR[0]=='&') return true;
    if (TYPESTR[0]=='(') //function
    {
         BOOL x;
         char *c=type->params;
         while(*type->params++!=')');//omit parameter list
         if (!*type->params) type->params=NULL;
         x=sctypIsReference(type);
         type->params=c;
         return x;
     }
    return false;
  }
  else
  {
     switch(type->flags&typTYPE)
     {
         case typUSERDEFINED:
          {
           return (sctypIsReference(&type->main->type));
          }
         default:return false;
     }
  }
}

///////////////////////////////////////////////////////////////////////////
////sctypSizeOf////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int sctypSizeOf(scType *type)
{
// deb("sctypSizeOf(where=%s)",TYPESTR);
 scAssert(sctypSizeOf,type);
// scIntAssert(sctypSizeOf,sym->add);
 if (TYPESTR&&TYPESTR[0])
    { if (TYPESTR[0]=='&') //reference
       return 4;
/*      {char *cq=TYPESTR;
       int i;
       TYPESTR++;
       i=sctypSizeOf(type);
       TYPESTR=cq;
       return i;
      }*/
      if (TYPESTR[0]=='(') return 4;//function
      if (TYPESTR[0]=='*'||(TYPESTR[0]=='['&&TYPESTR[1]==']')) return 4;//pointer
      if (TYPESTR[0]=='[')//TABLE
       {int i=0,c=1;
        char *cq;
        while (TYPESTR[c]!=']')
         {i=i*10+(TYPESTR[c]-'0');
          c++;
         }
        cq=TYPESTR;
        TYPESTR+=c+1;
        i*=sctypSizeOf(type);

        TYPESTR=cq;
        return i;
       }
    }
    {//no params
    //type is a struct
         switch(type->flags&typTYPE)
         {
          case typSCRIPT:
             {int size=12;//structs
              scSymbol *sc=type->main;
              do{
               if (!(sc->adr.flags&adrTYPEONLY)) size+=4;
               sc=sc->next;
              } while (sc);
              return size;
             }
          case typSTRUCT:
             {int size=0,j,k,l;//structs
              scSymbol *sc=type->main;
              l=sctypInsistsAlignment(&sc->type);
              while(sc){
               if (!sctypIsFunction(&sc->type)&&!(sc->adr.flags&adrTYPEONLY))
                {
                j=sctypSizeOf(&sc->type);
                k=sctypInsistsAlignment(&sc->type);
                if (k&&size%k) size+=k-size%k;

                }
                else j=0;
               size+=j;
               //unpack shorts and bytes
               sc=sc->next;
              };
              if (l&&size%l) size+=l-size%l;//DJGPP sizeof compatibility
              return size;
             }
         case typPREDEFINED:
             {
   //          deb("sizeof(PRE:%d)\n",sym->add->main);
              return Standard_Types[(int)type->main].size;
             }
         case typUSERDEFINED:
          {
//           deb("sizeof(user:%s)\n",sym->name);
           return (sctypSizeOf(&type->main->type));
          }
         }

    }
 serr(scErr_Internal, "internal compiler error");
}

///////////////////////////////////////////////////////////////////////////
////sctypInsistsAlignment//////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int sctypInsistsAlignment(scType *type)
{int i,j;
 i=sctypSizeOf(type);
 j=sctypGetValType(type);
 switch (j)
 {case valINT:
  case valDOUBLE:
  case valFLOAT:
       return 4;
  case valSHORT:
       return 2;
  case valSTRUCT:
       return sctypInsistsAlignment(&type->main->type);
  case valSCRIPT:
       return 4;
  case valPOINTER://depends on if it's array or pointer
       scAssert(sctypInsistsAlignment,TYPESTR);
       if (TYPESTR[0]=='*') return 4;//pointer requires 4
       //array:depends on of what does it consist
       {scType typ2;
        typ2=*type;
        sctypGoDeeper(&typ2);
        return sctypInsistsAlignment(&typ2);
       }
 }
 return 0;//doesn't need alignment
}

///////////////////////////////////////////////////////////////////////////
////scvalFreeValue/////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void scvalFreeValue(_value *v)
{scAssert(scvalFreeValue,v);
 if (v->adr.flags&adrREGISTER)
  {
   if (scvalSizeOf(v)==8)
   {
//   if (!(act.registers[v->val].used==v))
//     serr (scErr_Internal,"Internal compiler error:freevalue of a%d",v->val);
//   deb2(" ** DFree register a%d and a%d ** \n",v->adr.address,v->adr.address+1);

   act.registers[v->adr.address].used=NULL;
   act.registers[v->adr.address].locked=0;
   act.registers[v->adr.address+1].used=NULL;
   act.registers[v->adr.address+1].locked=0;
   }
   else{
//   if (!(act.registers[v->val].used==v))
//     serr (scErr_Internal,"Internal compiler error:freevalue of a%d and a%d",v->val,v->val+1);
//   deb1(" ** Free register a%d ** \n",v->adr.address);
   act.registers[v->adr.address].used=NULL;
   act.registers[v->adr.address].locked=0;
   }
  }
}

///////////////////////////////////////////////////////////////////////////
////sciValidIdentifier/////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
BOOL sciValidIdentifier(char * name)
{int i;
 if (isdigit(name[0])) return false;
 if (strstr(name,"operator")==name) return true;
 for (i=0;Special_Symbols[i];i++)
  if (strstr(name,Special_Symbols[i])) return false;
 for (i=0;Keywords[i];i++)
  if (strcmp(name,Keywords[i])==0) return false;
 for (i=0;Standard_Types[i].name;i++)
  if (strcmp(name,Standard_Types[i].name)==0) return false;
 return true;
}

///////////////////////////////////////////////////////////////////////////
////scvalFindFreeRegister//////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int scvalFindFreeRegister(_value *val)
{int il;
 for(il=2;il<240;il++)//FIX:0 and 1 are often needed elsewhere
  {if (!act.registers[il].used)
   {
//    deb1(" ** Allocate register a%d\n",il);
    act.registers[il].used=val;
    return il;
   }
//   act.registers[il].locked=0;
  }
 serr(scErr_Internal,"Compiler error : expression too complex (register overuse)!");
 return 239;
}

///////////////////////////////////////////////////////////////////////////
////scvalFindFreeDoubleRegister////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//Find xr_free double-register
int scvalFindFreeDoubleRegister(_value *val)
{int il;
 for(il=2;il<239;il++)
  {if (act.registers[il].used==NULL&&act.registers[il+1].used==NULL)
   {act.registers[il].used=val;
    act.registers[il+1].used=val;
//    deb2(" ** Allocate registers a%d and a%d\n",il,il+1);
    return il;
   }
  }
 serr(scErr_Internal, "Compiler error : expression too complex (register overuse)!");
}

///////////////////////////////////////////////////////////////////////////
////scvalArrayToPointer////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void scvalArrayToPointer(_value *v)
{int r=0;
 _value vt1;
 int imp;
 if (scvalGetValType(v)!=valPOINTER) return;
 sctypSimplifyType(&v->type);
 if (v->type.params[0]=='*') return;
 if (!v->sym) return;
 imp = (v->sym->adr.flags&adrIMPORTED);
 //so it's an array;
 deb0("[ArrayToPointer]");
 if (imp)
  {deb0("[Imported]");
   //scvalPutToRegister(v,TRUE);
  }
  //else
//[DS+128] -> mov a0,128;add a0,ds
//[[128]]  -> mov a0,[128];
//   v->flags^=valPointer;//erase pointer
//   v->sym=NULL;//erase sym
//   v->where=NULL;

 if ((v->adr.flags&adrPOINTER)==adrDS) r=regDS;
 if ((v->adr.flags&adrPOINTER)==adrBP) r=regBP;
 if ((v->adr.flags&adrPOINTER)==adrCS) r=regCS;
 v->adr.flags-=v->adr.flags&adrPOINTER;//not a pointer
 v->adr.val.ival=v->adr.address;
 scAssert(ArrayToPointer,imp||!(v->adr.flags&adrPOINTER))
 if (r)
 {
  scvalPutToRegister(v,TRUE);
  scvalSetRegister(&vt1,valINT,r);
  Gen_Opcode(opvcpuADD,v,&vt1);
 }
 v->type.flags=typPREDEFINED;
 (valTYPE)v->type.main=valINT;
 v->type.params=NULL;
 scAssert(ArrayToPointer,scvalSizeOf(v)==4);
}

///////////////////////////////////////////////////////////////////////////
////sctypSimplifyType//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void sctypSimplifyType(scType *type)
{
 if (TYPESTR&&TYPESTR[0]) return;
 if ((type->flags&typTYPE)==typUSERDEFINED)
 {
   *type=type->main->type;
   sctypSimplifyType(type);
   return;
 }
}

///////////////////////////////////////////////////////////////////////////
////sctypGetValType////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
valTYPE sctypGetValType(scType *type)
{
// deb("sctypSizeOf(where=%s)",TYPESTR);
 scAssert(sctypGetValType,type);
 if (TYPESTR&&TYPESTR[0])
    {
/*      if (TYPESTR[0]=='&')//reference
      {int i=0;i=1/i;}
      scAssert(sctypGetValType,TYPESTR[0]!='&');*/
      if (TYPESTR[0]=='&')//reference
      {char *cq=TYPESTR;
       valTYPE i;
       TYPESTR++;
       i=sctypGetValType(type);
       TYPESTR=cq;
       return i;
      }
      if (TYPESTR[0]=='(') //function
      {
           int x;
           char *c=type->params;
           while(*type->params++!=')');//omit parameter list
           if (!*type->params) type->params=NULL;
           x=sctypGetValType(type);
           type->params=c;
           return x;
       }
      if (TYPESTR[0]=='*'||TYPESTR[0]=='[') return valPOINTER;//pointer
    }
    //no params
         switch(type->flags&typTYPE)
         {
          case (typSTRUCT):return valSTRUCT;
          case (typSCRIPT):return valSCRIPT;
          case (typPREDEFINED):return (int)type->main;
          case (typUSERDEFINED):
//                       *type=type->main->type;
//                       return sctypGetValType(type);
                       return sctypGetValType(&type->main->type);

         }
         deb1("flags=%d",type->flags&typTYPE);
         deb1("flags=%d",type->flags);
//         type->flags/=0;
 scError(sctypGetValType);
}

///////////////////////////////////////////////////////////////////////////
////scvalGetValType////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
valTYPE scvalGetValType(_value* v)
{if (!v) return 0;
//if (!v->sym) {deb0("GVT:Non sym\n");}
//  else {deb1("GVT:Var:%s\n",v->sym->name);}
 sctypSimplifyType(&v->type);
 return sctypGetValType(&v->type);
}

///////////////////////////////////////////////////////////////////////////
////sciValTypeFloating/////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
BOOL sciValTypeFloating(valTYPE fl)
 {
  if ((fl==valDOUBLE)||(fl==valFLOAT))
//  )||(fl==valFIXED))
  return true;
  return false;
 }

//MAKE THEM COMPATIBLE IF POSSIBLE

///////////////////////////////////////////////////////////////////////////
////scvalBothTypes/////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void scvalBothTypes(_value* v1,_value* v2)
{//int typ;
 if (v1->adr.flags&adrPOINTER||v2->adr.flags&adrPOINTER) return;
 if (scvalGetValType(v1)==valDOUBLE){
  scvalConvToDouble(v2);
 }
 else
 if (scvalGetValType(v2)==valDOUBLE){
  scvalConvToDouble(v1);
 }

}

///////////////////////////////////////////////////////////////////////////
////scvalPutToRegister/////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void scvalPutToRegister(_value *val,int freeit)
{
 if (val->adr.flags&adrADDRESS)
  {//address we have here
    scvalGetAddress(val,3);//get rid of adrADDRESS
    return;
  }
 if (val->adr.flags&adrPOINTER)
  {
//   _value v2={{/*ADDRESS*/0,adrREGISTER,{0}},val->type,val->sym};
        _value v2={{/*ADDRESS*/0,
                adrREGISTER,{0}},
        {val->type.flags,val->type.params,val->type.main},
                val->sym};
   if (val->adr.flags&adrIMPORTED)
    {
     v2.adr.address=scvalFindFreeRegister(val);
     Gen_Opcode(opvcpuMOV,&v2,val);
//     val->adr.flags-=adrIMPORTED;
//     val->adr.flags|=adrCPU;
     val->adr=v2.adr;
     val->adr.flags|=adrCPU;
     return;
    }
   switch (scvalGetValType(val))
   {
   case valCHAR:
   case valSHORT:
   case valINT:
   case valFIXED:
   case valPOINTER:
     v2.adr.address=scvalFindFreeRegister(val);
     Gen_Opcode(opvcpuMOV,&v2,val);//???

     break;
   case valDOUBLE:
     v2.adr.address=scvalFindFreeDoubleRegister(val);
//     (int)v2.type.main=valDOUBLE;
     Gen_Opcode(opvcpuMOV,&v2,val);//DMOV ???

     break;

   case valFLOAT:
     v2.adr.address=scvalFindFreeDoubleRegister(val);
     (int)v2.type.main=valDOUBLE;
     Gen_Opcode(opvcpuFDBL,&v2,val);

     break;

   default:

     scError(scvalPutToRegister);
     v2.adr.address=scvalFindFreeRegister(val);
//     v2.type.main=valINT;
     Gen_Opcode(opvcpuMOV,&v2,val);

   }

   if (freeit) scvalFreeValue(val);
   *val=v2;
  } else
 if (val->adr.flags&adrREGISTER)//already register
   {
    if (val->adr.address>=240)//special register
    {_value v2;
     scvalSetRegister(&v2,valINT,scvalFindFreeRegister(val));
     Gen_Opcode(opvcpuMOV,&v2,val);
     *val=v2;
     return;
    }
    if (!act.registers[val->adr.address].used)//apply changes to the register list
       act.registers[val->adr.address].used=val;
    if (scvalSizeOf(val)==8)
     if (!act.registers[val->adr.address+1].used)
        act.registers[val->adr.address+1].used=val;
    return;
   }
    else
 if (val->adr.flags&adrIMMIDIATE)//numer
  {
   _value v2={{/*ADDRESS*/0,adrREGISTER,{0}},{/*type*/typPREDEFINED,NULL,(scSymbol*)valINT},NULL};
   switch (scvalGetValType(val))
   {
   case valFIXED:
      v2.adr.address=scvalFindFreeRegister(val);
      v2.type.flags=typPREDEFINED;
      (valTYPE)v2.type.main=valFIXED;
      Gen_Opcode(opvcpuMOV,&v2,val);

      break;
   case valDOUBLE:
      v2.adr.address=scvalFindFreeDoubleRegister(val);
      v2.type.flags=typPREDEFINED;
      (valTYPE)v2.type.main=valDOUBLE;
//      v2.type.flags=valDOUBLE;
      Gen_Opcode(opvcpuMOV,&v2,val);//DMOV ???

      break;
   default:
      v2.adr.address=scvalFindFreeRegister(val);
      v2.type.flags=typPREDEFINED;
      (valTYPE)v2.type.main=valINT;
//      v2.type.flags=valINT;
      Gen_Opcode(opvcpuMOV,&v2,val);

   }

   val->adr=v2.adr;
  }
}
/*************************Type information*****************************/

///////////////////////////////////////////////////////////////////////////
////sctypIsFunction////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
BOOL sctypIsFunction(scType *type)
{
 scAssert(sctypIsFunction,type);
 if (!TYPESTR) return false;
 if (TYPESTR[0]=='(') return true;
 return false;
}

///////////////////////////////////////////////////////////////////////////
////scvalIsOfType//////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
BOOL scvalIsOfType(_value *v,valTYPE vt)
{return (scvalGetValType(v)==vt);
}

///////////////////////////////////////////////////////////////////////////
////scvalIsImmidiate///////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
BOOL scvalIsImmidiate(_value *v)
{return ((v->adr.flags&adrIMMIDIATE)&&!(v->adr.flags&adrPOINTER));
}

///////////////////////////////////////////////////////////////////////////
////scvalIsImmidiateOfType/////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
BOOL scvalIsImmidiateOfType(_value *v,valTYPE vt)
{return (scvalIsImmidiate(v)&&scvalIsOfType(v,vt));
}

///////////////////////////////////////////////////////////////////////////
////scvalImmidiateValue////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
double scvalImmidiateValue(_value *v)
{switch((int)scvalGetValType(v))
 {
  case valINT:case valSHORT:case valCHAR:
    return (double)v->adr.val.ival;
  case valFLOAT:case valDOUBLE:case valFIXED:
    return v->adr.val.dval;
 }
 scError(scvalImmidiateValue);
}

///////////////////////////////////////////////////////////////////////////
////scvalSetImiidiate//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
_value* scvalSetImmidiate(_value *v,valTYPE vt,int ival,double dval)
{
 if (!v) v=New(v);
 //address
 v->adr.flags=vt|adrIMMIDIATE;
 v->adr.address=-1;
 if (vt==valDOUBLE||vt==valFLOAT)
   v->adr.val.dval=dval;
   else
   v->adr.val.ival=ival;
 //type
 v->type.flags=typPREDEFINED;
 (valTYPE)v->type.main=vt;
 v->type.params=NULL;
 //symbol reference
 v->sym=NULL;
 return v;
}

///////////////////////////////////////////////////////////////////////////
////scvalSetINT////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
_value* scvalSetINT(_value *v,int ival)
{return scvalSetImmidiate(v,valINT,ival,ival);
}

///////////////////////////////////////////////////////////////////////////
////scvalSetRegister///////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
_value*  scvalSetRegister(_value *v,valTYPE vt,int reg)
{
 if (!v) v=New(v);
 //address
 v->adr.flags=vt|adrREGISTER;
 v->adr.address=reg;
 v->adr.val.ival=0;//don't care
 //type
 v->type.flags=typPREDEFINED;
 (valTYPE)v->type.main=vt;
 v->type.params=NULL;
 //symbol reference
 v->sym=NULL;
 return v;
}

///////////////////////////////////////////////////////////////////////////
////scvalSetSymbol/////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
_value* scvalSetSymbol(_value *v,scSymbol *s)
{
 if (!v) v=New(v);
 //address
 v->adr=s->adr;
 //type
 v->type=s->type;
 //symbol reference
 v->sym=s;
 return v;
}

///////////////////////////////////////////////////////////////////////////
////sctypGoDeeper//////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
// Changes type of val to next in unary * operator
void sctypGoDeeper(scType* typ)
{
            if (sctypIsFunction(typ))
            {return;//don't change with functions
            }
/*             if ((!val->type.params)||!(val->type.params[0]))//end of type string
              {//go deeper, if possible
               if (((val->type.flags&typTYPE)!=typUSERDEFINED))//its standard or struct type:-(
                 serr3(scErr_Operand, invalid_operand_to,"","unary `*' or `->'");
//               val->sym=((scSymbol *)val->sym->add->main);
//               val->where=val->sym->add->queue;
               *typ=type->main->type;
               sctypGoDeeper(typ);

              }
              else*/
             sctypSimplifyType(typ);

             if (*(typ->params)=='*') (typ->params)++;
                else
             if (*(typ->params)=='&') (typ->params)++;
                else
             if (*(typ->params)=='[')
                { while (typ->params&&*typ->params!=']') (typ->params)++;
                  scAssert(scvalGoDeeper,*typ->params==']');
                  (typ->params)++;
                }
                else
                 serr3(scErr_Operand, invalid_operand_to,"","unary `*' (not a pointer)");
}

///////////////////////////////////////////////////////////////////////////
////scvalSizeOf////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int scvalSizeOf(_value *val)
{ return sctypSizeOf(&val->type);
}

///////////////////////////////////////////////////////////////////////////
////sciValTypeStr//////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
char *sciValTypeStr(valTYPE v)
{
 switch (v)
 {
           case valINT:return "int";
           case valCHAR:return "char";
           case valFIXED:return "fixed";
           case valFLOAT:return "float";
           case valDOUBLE:return "double";
           case valSHORT:return "short";
           case valPOINTER:return "pointer";
           case valVOID:return "void";
           case valSTRUCT:return "struct";
           case valSCRIPT:return "script";
 default:return "union or enum";
 }
}

///////////////////////////////////////////////////////////////////////////
////scvalSameType//////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
BOOL scvalSameType(_value *val1,_value *val2)
{
 int v1,v2;

 v1=scvalGetValType(val1);
 v2=scvalGetValType(val2);
 if (v1==v2) return true;
 if (v1!=v2){
 if (
   (v1==valINT&&v2==valPOINTER)||
   (v2==valINT&&v1==valPOINTER)
   ) return true;
// if (scvalIsImmidiate(val1))
  {
   switch(v2)
   {
    case valSHORT:
    case valCHAR:
    case valINT:
         scvalConvToInt(val1);return true;
    case valFLOAT:
         scvalConvToFloat(val1);return true;
    case valFIXED:
         scvalConvToFixed(val1);return true;
    case valDOUBLE:
         scvalConvToDouble(val1);return true;
   }
  }
 deb2("Types between `%s' and `%s'\n",sciValTypeStr(v1),sciValTypeStr(v2));
 return false;
 }
//  serr( "Incompatible type between `%s' and `%s'",sciValTypeStr(v1),sciValTypeStr(v2));
 return true;
}


/*************************************************************************
          CONVERSION ROUTINES..........
*************************************************************************/
///////////////////////////////////////////////////////////////////////////
////scvalConvToDouble//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void scvalConvToDouble(_value *val)
{

 if (scvalGetValType(val)==valDOUBLE) return;

 if (scvalIsImmidiate(val))
  {
   switch((int)scvalGetValType(val))
   {
    case valINT:

     val->adr.val.dval=val->adr.val.ival;
     break;
    case valFIXED:
     val->adr.val.dval=val->adr.val.ival/65536.0;
     break;
    case valFLOAT:
     val->adr.val.dval=val->adr.val.fval;
     break;
    }
    scAssert(scvalConvToDouble,(val->type.flags&typTYPE)==typPREDEFINED);
    (valTYPE)val->type.main=valDOUBLE;
  }
  else//-----------------------------------------------------------
  {
   _value v1={{/*ADDRESS*/0,adrREGISTER,{0}},{/*type*/typPREDEFINED,NULL,(void*)valDOUBLE},NULL};
   switch(scvalGetValType(val))
   {
    case valINT:

      v1.adr.address=scvalFindFreeDoubleRegister(val);
      Gen_Opcode(opvcpuIDBL,&v1,val);
      *val=v1;
      break;
    case valSHORT:
    case valCHAR:
      scvalConvToInt(val);
      scvalConvToDouble(val);
      return;
      break;
    case valFLOAT:

      v1.adr.address=scvalFindFreeDoubleRegister(val);
      Gen_Opcode(opvcpuFDBL,&v1,val);
      *val=v1;
      break;
    case valFIXED:

      v1.adr.address=scvalFindFreeDoubleRegister(val);
      Gen_Opcode(opvcpuFIXDBL,&v1,val);
      *val=v1;
      break;
     default:serr2(scErr_Cast,illegal_type_conversion," to double");
   }
  }
}

///////////////////////////////////////////////////////////////////////////
////scvalConvToFloat///////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void scvalConvToFloat(_value *val)
{
 if (scvalGetValType(val)==valFLOAT) return;

 deb0("ToFloat\n");
 if (scvalIsImmidiate(val))
  {
   switch(scvalGetValType(val))
   {
    case valDOUBLE:
     val->adr.val.fval=(float)val->adr.val.dval;
    break;
    case valINT:
     val->adr.val.fval=(float)val->adr.val.ival;
    break;
    case valFIXED:
      val->adr.val.fval=(float)(val->adr.val.ival/65536.0);
    break;
    default:

//      (float)val->val=val->val;
//      val->vald=val->val;
    scError(scvalConvToFloat);
   }

    scAssert(scvalConvToFloat,(val->type.flags&typTYPE)==typPREDEFINED);
    (valTYPE)val->type.main=valFLOAT;
  }
  else
  {
   _value v1={{/*ADDRESS*/0,adrREGISTER,{0}},{/*type*/typPREDEFINED,NULL,(void*)valFLOAT},NULL};
   switch(scvalGetValType(val))
    {
     case valDOUBLE:
      Gen_Opcode(opvcpuDFLT,&v1,val);
      *val=v1;
      break;
     case valSHORT:
     case valCHAR:
     case valFIXED:
     case valINT:

      scvalConvToDouble(val);
      scvalConvToFloat(val);
      return;
      break;
     default:serr2(scErr_Cast,illegal_type_conversion," to float");
    }

  }
}

///////////////////////////////////////////////////////////////////////////
////scvalConvToInt/////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void scvalConvToInt(_value *val)
{
 if (scvalGetValType(val)==valINT
   ||scvalGetValType(val)==valPOINTER)
         return;

 deb0("ToINT\n");
 if (scvalIsImmidiate(val))
  {
   switch(scvalGetValType(val))
   {
    case valDOUBLE:
       val->adr.val.ival=(int)val->adr.val.dval;
       break;
    case valFLOAT:
       val->adr.val.fval=(float)val->adr.val.dval;//???
      break;
    case valFIXED:
      val->adr.val.ival>>=16;
      break;
    default:

    scError(scvalConvToInt);
   }
    scAssert(scvalConvToInt,(val->type.flags&typTYPE)==typPREDEFINED);
    (valTYPE)val->type.main=valINT;
  }
  else
  {_value v1={{/*ADDRESS*/0,adrREGISTER,{0}},{/*type*/typPREDEFINED,NULL,(scSymbol*)valINT},NULL};
   switch(scvalGetValType(val))
    {
    case valSHORT:
    case valCHAR:
      scvalPutToRegister(val,TRUE);
      (valTYPE)val->type.main=valINT;
      return;
      break;
     case valFIXED:
      scAssert(scvalConvToInt(2),(val->type.flags&typTYPE)==typPREDEFINED);
      scvalPutToRegister(val,TRUE);
      (valTYPE)val->type.main=valINT;
      Gen_Opcode(opvcpuSHR,val,scvalSetImmidiate(&v1,valINT,16,0));
      break;
     case valDOUBLE:
      v1.adr.address=scvalFindFreeRegister(val);
      Gen_Opcode(opvcpuDINT,&v1,val);
      *val=v1;
      return;
      break;
     case valFLOAT:
      scvalPutToRegister(val,TRUE);
      scvalConvToInt(val);
      return;
      break;
     default:serr2(scErr_Cast,illegal_type_conversion," to int");
    }

   scAssert(scvalConvToInt,(val->type.flags&typTYPE)==typPREDEFINED);
   scAssert(scvalConvToInt,(valTYPE)val->type.main==valINT);
  }

}

///////////////////////////////////////////////////////////////////////////
////scvalConvToFixed///////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void scvalConvToFixed(_value *val)
{
 if (scvalGetValType(val)==valFIXED) return;
 deb1("<%s to fixed>",sciValTypeStr(scvalGetValType(val)));

 deb0("ToFIXED\n");
 if (scvalIsImmidiate(val))
  {
   switch(scvalGetValType(val))
   {
    case valDOUBLE:
       val->adr.val.ival=(int)(val->adr.val.dval*65536);
       break;
    case valFLOAT:
       val->adr.val.ival=(int)(val->adr.val.fval*65536);
      break;
    case valINT:
       val->adr.val.ival=val->adr.val.ival<<16;
      break;
    default:
      deb1("val=%d\n",scvalGetValType(val));

    scError(scvalConvToFixed);
   }
    scAssert(scvalConvToFixed,(val->type.flags&typTYPE)==typPREDEFINED);
    (valTYPE)val->type.main=valFIXED;
  }
  else
  {_value v1={{/*ADDRESS*/0,adrREGISTER,{0}},{/*type*/typPREDEFINED,NULL,(void*)valFIXED},NULL};
   switch((int)scvalGetValType(val))
    {
    case valSHORT:
    case valCHAR:
    case valINT:

      scAssert(scvalConvToFixed(2),(val->type.flags&typTYPE)==typPREDEFINED);
      scvalPutToRegister(val,TRUE);
      Gen_Opcode(opvcpuSHL,val,scvalSetImmidiate(&v1,valINT,16,0));
      (valTYPE)val->type.main=valFIXED;
      deb1("val=%s\n",sciValTypeStr(scvalGetValType(val)));
      break;
     case valDOUBLE:
      v1.adr.address=scvalFindFreeRegister(val);
      Gen_Opcode(opvcpuDFIX,&v1,val);
      *val=v1;
      break;
     case valFLOAT:
      scvalPutToRegister(val,TRUE);
      scvalConvToFixed(val);
      return;
      break;
    }
//   val->type.flags=typPREDEFINED;

  }

}

///////////////////////////////////////////////////////////////////////////
////scvalCast//////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void scvalCast(_value *val,scSymbol *sctype)
{int id,nid,size,nsiz;
 _value vl;
 vl=*val;
 id=scvalGetValType(val);
 size=scvalSizeOf(val);
 vl.type=sctype->type;
 nsiz=scvalSizeOf(&vl);
 nid=scvalGetValType(&vl);
 deb2("cast %d,%d\n",id,nid);
 if (sctype->type.flags&typUNSIGNED)
  val->type.flags|=typUNSIGNED;
 if (id==nid) {
  val->type=sctype->type;
  return;
 }
//  if (size!=nsiz||sciValTypeFloating(nid)!=sciValTypeFloating(id))
   {switch(nid)//destination type
    {
     case valPOINTER:
          if (sciValTypeFloating(nid))
             serr2(scErr_Operand,illegal_cast,"from floating to pointer");
          if (size!=4)
             serr2(scErr_Operand,illegal_cast,"to pointer");
          *val=vl;
          return;
     case valINT:
          switch (id)
          {
           case valCHAR:case valFIXED:case valFLOAT:case valDOUBLE:case valPOINTER:
                scvalConvToInt(val);return;
          }
          break;
     case valFIXED:
          switch (id)
          {
           case valCHAR:case valINT:case valFLOAT:case valDOUBLE:
                scvalConvToFixed(val);return;
          }
          break;
     case valFLOAT:
          switch (id)
          {
           case valCHAR:case valFIXED:case valINT:case valDOUBLE:
                scvalConvToFloat(val);return;
          }
          break;
     case valDOUBLE:
          switch (id)
          {
           case valCHAR:case valINT:case valFLOAT:case valFIXED:
                scvalConvToDouble(val);return;
          }
          break;
    }
    serr2(scErr_Operand,illegal_cast,"");
   }

}

///////////////////////////////////////////////////////////////////////////
////sciAddImportedFunction/////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void sciAddImportedFunction(scSymbol* sym,scSymbol* in_struct)
{int flags=0;
 char temp[1024];
 char *use_name=sym->name;

 //special name for importing was specified with import(name) ...
 if (sym->import_name) use_name=sym->import_name;

 if (sym->adr.address!=-1) return;
 if (!(sym->adr.flags&adrIMPORTED)) return;
 if (in_struct)
 {
 while (!(in_struct->adr.flags&adrTYPEONLY))
      in_struct=in_struct->type.main;
  strcpy(temp,in_struct->name);
  strcpy(temp+strlen(temp),".");
  strcpy(temp+strlen(temp),use_name);
 }
 else strcpy(temp,use_name);

 //check, if any previous import does have this name
 {
  char *_pmem=act.imports.mem;
  char namer[1024];
  do{
  if ((int)_pmem-(int)act.imports.mem<act.imports.pos)
  {
  strcpy(namer,_pmem);
  _pmem+=strlen(namer)+1;
  ALIGN_INT((int)_pmem);
  if (strcmp(namer,temp)==0)
    serr3(scErr_Declaration,"Duplicate import name '%s' for symbol '%s'",temp,sym->name);
  _pmem+=8;
  }else namer[0]=0;
  } while (namer[0]);
 }

//do the job
 deb2("Adding import %s with %d params\n",temp,sym->params);
 deb1("Type is:%s\n",sciValTypeStr(sctypGetValType(&sym->type)));
 pushstr_pmem(&act.imports,temp);
 align_pmem(&act.imports);
 sym->adr.address=act.imports.pos;
 deb1("at %d\n",sym->adr.address);
 push_pmem(&act.imports,4,&flags);
 flags=sym->params;// function
 if (!sctypIsFunction(&sym->type)) flags=-1;// variable!
 push_pmem(&act.imports,4,&flags);
}

///////////////////////////////////////////////////////////////////////////
////sciReadIntegerInBrackets///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int sciReadIntegerInBrackets(int def)
{int imp=def;
  if (lexeq("("))
    {
      int ip=act.ip;
      _value val;
      Advance;Advance;
      sciReadSubExp(1,&val,true);
      if (!scvalIsImmidiateOfType(&val,valINT)||(act.ip!=ip))
         serr2(scErr_Declaration, "Constant integer expected instead of %s",ST->lexema);
      imp=val.adr.val.ival;
      if (!lexeq(")")) serr2(scErr_Declaration, "`)' expected instead of `%s'",ST->lexema);
    }
  return imp;
}

///////////////////////////////////////////////////////////////////////////
////getStructMemberFromName////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int getStructMemberFromName(char *name,scSymbol **in_struct)
{scSymbol *list=SC_types->first;
 while (strstr(name,"."))
 {      char *c=strstr(name,".");
        *(c++)=0;
        *in_struct=scdicFoundSymbol(list,name);
        scAssert(getStructMemberFromName,*in_struct);
        strcpy(name,c);
        list=((((*in_struct)->type.flags&typTYPE)==typSTRUCT
               ||((*in_struct)->type.flags&typTYPE)==typSCRIPT)
               &&
              ((*in_struct)->adr.flags&adrTYPEONLY))?
               (*in_struct)->type.main:NULL;
 }
 return 1;
}

///////////////////////////////////////////////////////////////////////////
////scvalAccessReference///////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void scvalAccessReference(_value *val)
{ //only reference
 if (sctypIsFunction(&val->type)) return;
 if (!sctypIsReference(&val->type)) return;
 deb0("Accessing reference.\n");
 {
  _value vt1=*val;
  val->adr.address=scvalFindFreeRegister(val);
  val->adr.flags=adrREGISTER;
  Gen_Opcode(opvcpuMOV,val,&vt1);
  scvalFreeValue(&vt1);
 }
 val->adr.flags|=adrCPU;//make [a0]
 sctypGoDeeper(&val->type);//remove reference
}

void scvalDisplayValue(char *x,_value *v)
{
 deb1("%s{",x);
 deb1("adr:{flags=%d",v->adr.flags);
 switch (v->adr.flags&adrPOINTER)
 {
  case adrCS:deb0("(CS)");break;
  case adrDS:deb0("(DS)");break;
  case adrBP:deb0("(BP)");break;
  case adrES:deb0("(ES)");break;
  case adrCPU:deb0("(CPU)");break;
  default:deb0("(??)");
 }
 if (v->adr.flags&adrIMMIDIATE) deb0("(IMM)");
 if (v->adr.flags&adrREGISTER) deb0("(REG)");
 if (v->adr.flags&adrIMPORTED) deb0("(IMP)");
 deb1(",adres=%d},",v->adr.address);
 deb2("type:{flags=%d,params=%s},",v->type.flags,v->type.params);
 if (!v->sym)
 {deb0("sym=NULL");}
 else
 {deb1("sym='%s'",v->sym->name);}
 deb0("};\n");
}

///////////////////////////////////////////////////////////////////////////
////strrcpy////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static void strrcpy(char *dest,char *src)
{int n=strlen(src);
 for(;n>=0;n--) dest[n]=src[n];
}

#define strAddToBegin(str,x)                                    \
  {strrcpy((str)+strlen(x),str);memcpy(str,x,strlen(x));}
#define chrAddToBegin(str,x)                                    \
  {strrcpy((str)+1,str);str[0]=x;}
#define strAddToEnd(str,x) strcpy((str)+strlen(str),x);
#define strAddName(buf,space) \
 if (name) {if (space) strcpy(buf+strlen(buf)," ");strcpy(buf+strlen(buf),name);name=NULL;}

///////////////////////////////////////////////////////////////////////////
////sctypTypeOfStr/////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void sctypTypeOfStr(scType* type,char *name,char *buf)
{buf[0]=0;
 scAssert(scvalTypeOfStr,type&&buf);

 if (TYPESTR)
 {char *x=type->params;
  int addspace=1;
  char *pbuf;
  type->params=NULL;
  sctypTypeOfStr(type,NULL,buf);
  type->params=x;
  pbuf=buf+strlen(buf);
//  *(pbuf++)=' ';
//  *pbuf=0;
  while (*x)
  {
   while (*x=='*'||*x=='&')
     {
      chrAddToBegin(pbuf,*x);x++;
      addspace=0;
     }
   if (*x=='[')
     {char t[64];
      int i=0;
      while(*x!=']') t[i++]=*x++;
      t[i++]=*x++;
      t[i]=0;
      strAddName(pbuf,addspace);
//      strAddToBegin(pbuf,t);
      strAddToEnd(pbuf,t);
     }
   if (*x=='(')
     {
      if (pbuf[0])//something is already there
      {strAddName(pbuf,addspace);
       strAddToEnd(pbuf,")");
       chrAddToBegin(pbuf,'(');
      } else
      strAddName(pbuf,addspace);
      strAddToEnd(pbuf,"(");
      x++;
      while(*x!=')')
      {
       if (*x=='.')
        {x+=3;
         strAddToEnd(pbuf,"...");
        }
        else
        {scSymbol *xs=(scSymbol*)strtoul(x, &x, 16);
         sctypTypeOfStr(&xs->type,NULL,pbuf+strlen(pbuf));//to the end
         if (*x=='=')
          {
           x++;
           strtoul(x, &x, 16);//pointer to default-value
          }
        }//...
        if (*x==',') {x++;strAddToEnd(buf,",");}
      }//while
      strAddToEnd(buf,")");
      x++;
     }//'('
     strAddName(buf,1);
  }//while
 }
 else
 {    scSymbol *x;

      switch(type->flags&typTYPE)
      {
       case (typSTRUCT):
        strcpy(buf,"struct");//strAddName(buf);
        break;
       case (typSCRIPT):
        strcpy(buf,"script");
        break;
       case (typPREDEFINED):
        strcpy(buf,sciValTypeStr((valTYPE)type->main));
        strAddName(buf,1);
        return;
       case (typUSERDEFINED):
        if (sctypGetValType(&type->main->type)==valSTRUCT)
        {sprintf(buf,"struct %s",type->main->name);
         strAddName(buf,1);
         return;//do a short struct Joe, other than struct{list of members}
        }
        sctypTypeOfStr(&type->main->type,NULL,buf);
        strAddName(buf,1);
        return;
      }

      //script or struct
      strcpy(buf+strlen(buf),"{");
      x=type->main;
      while(x)
      {if (!(x->adr.flags&adrTYPEONLY))
       {
       strcpy(buf+strlen(buf),x->name);
       strcpy(buf+strlen(buf),";");
       }
       x=x->next;
      }
   strcpy(buf+strlen(buf),"}");
 }
}

///////////////////////////////////////////////////////////////////////////
////sciReadImport//////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int sciReadImport(char **import_name)
{     int imp=-1;
      *import_name=NULL;
      Advance;Advance;scget_back(ST);

      while (lexeq("("))
        {
          int ip=act.ip;
          _value val;
          Advance;Advance;
          if (isdigit(ST->lexema[0]))
          {
          sciReadSubExp(1,&val,true);
          if (!scvalIsImmidiateOfType(&val,valINT)||(act.ip!=ip))
             serr2(scErr_Declaration, "Constant integer expected instead of %s",ST->lexema);
          if (imp>=0)
             serr(scErr_Declaration, "Dispatcher descriptor repeated more than once");
          imp=val.adr.val.ival;
          if (imp>=SC_MAX_FUNCTION_DISPATCHERS)
              serr3(scErr_Declaration, "The number %d exceeded the maximum number (%d) of dispatchers",imp, SC_MAX_FUNCTION_DISPATCHERS);
          }
          else
          {char name[1024];
           int i=1;
           name[0]=0;
           if (*import_name)
             serr2(scErr_Declaration, "Function already has an import name '%s'",*import_name);
           while (i)
           {
           if (lexeq(")")) i--;
           if (lexeq("(")) i++;
           if (i)
           {
            strcpy(name+strlen(name),ST->lexema);
            Advance;//scget_back(ST);
            if (ST->newline||ST->whitespaces)
             serr(scErr_Declaration, "Import name cannot contain spaces nor new-line characters (possibly thou hast ')' omited)");
           }
           }
           *import_name=scStrdup(name);
          }
          if (!lexeq(")")) {serr2(scErr_Declaration, "`)' expected instead of `%s'",ST->lexema);}
          Advance;scget_back(ST);
        }
      if (imp<0) imp=0;
      return imp;
}

///////////////////////////////////////////////////////////////////////////
////sctypGetStruct/////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
scSymbol *sctypGetStruct(scType *typ)
{scSymbol *_this;
 scAssert(sctypGetStruct,sctypGetValType(typ)==valSTRUCT);
 _this=typ->main;
  while (_this&&(!(_this->adr.flags&adrTYPEONLY)
         ||!(_this->type.flags&typSTRUCT)))
        _this=_this->type.main;
  return _this;
}

static scType CharAsteriskType={
typPREDEFINED,"*",(scSymbol *)valCHAR
};
/*static scSymbol CharAsterisk={
{0,0,{0}},NULL,NULL,0,NULL,
{typPREDEFINED,"*",(scSymbol *)valCHAR},
NULL,NULL
};*/
//static char *asterisk="*";

///////////////////////////////////////////////////////////////////////////
////scvalConstantString////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void scvalConstantString(_value *val,char *s)
{int adr=0;

//search for exact string already specified
 if (act.consts.pos&&strlen(s)+1<=(unsigned)act.consts.pos)
 {char *a=act.consts.mem;
  int remain=act.consts.pos;
  while(remain&&!adr)
  {
   if (strncmp(s,a,remain)==0) {adr=(int)a-(int)act.consts.mem+1;
   deb2("Found before as %s at %d\n",a,adr);
   }
   //skip to next string....
   while (*a&&remain) {a++;remain--;}
   //if s is empty
   if (!*s && !*a) {adr=(int)a-(int)act.consts.mem+1;}
   //omit zeros
   while(!*a&&remain) {a++;remain--;}
  }
 }//returns adr+1

 if (!adr)//string wasn't specified before
 {
   adr=act.consts.pos;
   pushstr_pmem(&act.consts,s);
 }
 else adr--;
 scFree(s);
 scvalSetINT(val,adr);
 val->sym=NULL;
 val->type=CharAsteriskType;
 val->type.flags|=typCONST;
 val->adr.flags|=adrADDRESS+adrES;
 val->adr.address=val->adr.val.ival;
}

///////////////////////////////////////////////////////////////////////////
////scvalGetAddress////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//situ=0-unary&,1-this operator,2-reference,3-author
void scvalGetAddress(_value *val,int situ)
{
  int r=0;
  _value vt1;
  scAssert(scvalGetAddress,situ!=3||(val->adr.flags&adrADDRESS))
  deb2("adr.flags=%d&adrPOINTER=%d\n",val->adr.flags,val->adr.flags&adrPOINTER);
  if (!(val->adr.flags&adrPOINTER))
  {
   deb1("adr.flags=%d\n",val->adr.flags);
   switch(situ)
   {
    case 0:serr(scErr_Operand, "Not a lvalue to unary &");
    case 1:serr(scErr_Operand, "this is invalid(?)");
    case 2:serr(scErr_Operand, "Invalid reference initializer");
    case 3:serr(scErr_Operand, "Invalid ADDRESS value");
   }
  }
  if ((val->adr.flags&adrPOINTER)==adrDS) r=regDS;
  if ((val->adr.flags&adrPOINTER)==adrES) r=regES;
  if ((val->adr.flags&adrPOINTER)==adrBP) r=regBP;
  if ((val->adr.flags&adrPOINTER)==adrCS) r=regCS;//seems it's a function

  //erase pointer
  val->adr.val.ival=val->adr.address;
  val->adr.flags-=val->adr.flags&adrPOINTER;

  if (situ==3)
  {
   val->adr.flags^=adrADDRESS;
  }
  else
  {
//change type:
   if (!val->type.params||!val->type.params[0])
           val->type.params="*";//set to "*"
   else
    {char *t=scAlloc(strlen(val->type.params)+2);
     strcpy(t+1,val->type.params);
     *t='*';
     val->type.params=t;
    }
   //           if (val->flags&valPointer)
   //              serr(scErr_Internal, "internal compiler error at unary &\n");
  }

  if (r)
  {
          scvalPutToRegister(val,TRUE);
          scvalSetRegister(&vt1,valINT,r);
          Gen_Opcode(opvcpuADD,val,&vt1);
  }

}

///////////////////////////////////////////////////////////////////////////
////scvalNotConst//////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void valNotConst(_value *val)
{if (val->type.flags&typCONST)
      serr(scErr_Operand, "const value cannot change");
}

