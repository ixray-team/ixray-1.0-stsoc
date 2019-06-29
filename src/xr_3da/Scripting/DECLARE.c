/************************************************************************
       -= SEER - C-Scripting engine v 0.93a =-
State      : under implementation
Author     : Przemyslaw Podsiadly
             email: ppodsiad@elka.pw.edu.pl
             WWW:   http://home.elka.pw.edu.pl/~ppodsiad
File:declare.c
Desc:reading declaration of symbols(types,functions and variables)
99.08.04 Script support started
99.07.05 Classes support started
************************************************************************/
#define INTERNAL_FILE
#include <seer.h>
#include "internal.h"
#include "code.h"

scSymbol *sctypGetShortDefinition(scType* typ);
void sciReadDefinition(scSymbol *deli,scSymbol *sym,scSymbol *in_struct);

#define __MAX_FUNCTION_PARAMETERS 200


///////////////////////////////////////////////////////////////////////////
////scReadOperatorName/////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
char *scReadOperatorName()
{char *name;
 Advance;
 if (ST->lexema[1]==0)
 {
  switch(ST->lexema[0])
  {case '+':
   case '-':
   case '*':
   case '/':
   case '%':
   case '|':
   case '&':
   case '^':
   case '=':
   case '<':
   case '>':
   case '!':
   case '~':
    name=scStrdup("operator?");
    name[8]=ST->lexema[0];
    return name;
   default:;
  }
 }
 if (lexeq("==")||lexeq("<=")||lexeq(">=")||lexeq("+=")||lexeq("-=")||
     lexeq("/=")||lexeq("*=")||lexeq("%=")||lexeq("++")||lexeq("--")||
     lexeq("!=")||lexeq("<<")||lexeq(">>")||lexeq("[]")||lexeq("^=")||
     lexeq("&=")||lexeq("|="))
 {name=scStrdup("operator??");
  name[8]=ST->lexema[0];
  name[9]=ST->lexema[1];
  return name;
 }
 else
 if (lexeq("<<=")||lexeq(">>="))
 {name=scStrdup("operator???");
  name[8]=ST->lexema[0];
  name[9]=ST->lexema[1];
  name[10]=ST->lexema[2];
  return name;
 }
 else
 if (lexeq("("))
 {Advance;
  if (lexeq(")"))
   return scStrdup("operator()");
  if (lexeq("int"))
   {Advance;if (lexeq(")"))return scStrdup("operator(int)");}
  if (lexeq("double"))
   {Advance;if (lexeq(")"))return scStrdup("operator(double)");}
  if (lexeq("char"))
   {Advance;
    if (lexeq("*"))
     {Advance;if (lexeq(")"))return scStrdup("operator(char*)");}
   }
 }
 else
 if (lexeq("["))
 {Advance;
  if (lexeq("]"))
   return scStrdup("operator[]");
 }
 serr( scErr_Declaration,"invalid operator function");
}

///////////////////////////////////////////////////////////////////////////
////scsGetFunctionParams///////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
static char *scsGetFunctionParams()
{//read till `)'
 scPosition sp;
 char temp[1024]="(";
 BOOL was3dots=false;
 scSymbol* sm[__MAX_FUNCTION_PARAMETERS];
 int actu=0;
 scSymbol* ts;
 scget_back(ST);
 scGetPosition(ST,&sp);
 Advance;
#if DEBUGLEVEL==0
 deb0("of function with parameters:\n");
#endif
 while(1)
 {
  Advance;
  if lexeq(")")
    {
     strcpy(temp+strlen(temp),")");
     Advance;
     return scStrdup(temp);
    }
  else
  {int it;
   char s[12];
   if (was3dots) serr2(scErr_Parse, parse_error,ST->lexema);
   scget_back(ST);
   if lexeq("...")
    {was3dots=true;
     strcpy(temp+strlen(temp),ST->lexema);
     Advance;
    }
    else
    {
#if DEBUGLEVEL>0
     deb0("Function Parameter ");
#endif
     ts=sctypGetDefinition();
     scget_back(ST);
     if (!ts&&actu) serr2(scErr_Declaration, "Parameter declaration expected at %s",ST->lexema);
     if (!ts) {
     scSetPosition(sp);Advance;scget_back(ST);
#if DEBUGLEVEL==0
     deb1("Maybe a constructor%s)?\n",ST->lexema);
#endif
     return NULL;//maybe a constructor?
     }
     actu++;sm[actu-1]=ts;
     if (ts->name)//check if duplicated names
     for (it=0;it<actu-1;it++)
       if (sm[it]->name&&strcmp(sm[it]->name,ts->name)==0)
        serr4( scErr_Declaration,"Parameters %d and %d has the same name '%s'",it+1,actu,ts->name);
//     deb1("<%x>\n",(int)ts);
     sprintf(s,"%x",(int)ts);//TS should be freed
     strcpy(temp+strlen(temp),s);
    }
   Advance;
   if (lexeq("="))
   {//serr( scErr_Parse,"SeeR now doesn't allow default initializers in function parameters")
    int ip;
    char s[12];
    _value *val=New(_value);
    ip=act.ip;
    Advance;scget_back(ST);
    sciReadSubExp(1,val,true);
    if (ip!=act.ip)
       serr( scErr_Declaration,"Invalid default initializer")
    sprintf(s,"=%x",(int)val);
    strcpy(temp+strlen(temp),s);
   }
   if (!lexeq(",")) {if(!lexeq(")")) serr2( scErr_Parse,parse_error,ST->lexema)
                      else scget_back(ST);//put back ")"
                    }
                   else
   strcpy(temp+strlen(temp),",");

  }
 }
#if DEBUGLEVEL==0
 deb0("End of parameters ");
#endif
 return NULL;
}

//return NULL,if not defi1 , eg int x;
#define ADD_to_end(into) strcpy(into+strlen(into),ST->lexema)

#define ADD_to_start(into,what) {char *t;\
                           strcpy(otemp,what);\
                           strcpy(otemp+strlen(otemp),temp);\
                           t=otemp;otemp=temp;temp=t;\
                           }
//****************************************************
/*
#ifndef DJGPP
char* itoa(int v,char *s, int base)//base =10
{
 sprintf(s,"%d",v);
 return s;
}
#endif
*/
///////////////////////////////////////////////////////////////////////////
////scsGetDefiParams///////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//name is read, and parameters, so all of type
// *(A[2])(c)
static char* scsGetDefiParams(char **name)
{
 int eod=0;
 char temp1[256];
 char temp2[256];
 char *temp=temp1,*otemp=temp2;
 char *nested=NULL;
 temp1[0]=temp2[0]=0;
// debprintf("Reading defi\n");
 while(!eod)
 {
  Advance;
  if(lexeq(",")||lexeq(";")||lexeq("{")||lexeq(")")||lexeq("="))
  {eod=1;
   scget_back(ST);
  }
  else{
  if (lexeq("*")) ADD_to_start(temp,ST->lexema)
  else
  if (lexeq("&")) ADD_to_start(temp,ST->lexema)
  else
  if (lexeq("["))
   {int i;
    char tmp[256];
    tmp[0]=0;
    while(lexeq("["))
    {
     ADD_to_end(tmp);
     Advance;
     if (lexeq("]")) //ADD_to_end(tmp);
     serr(scErr_Parse,"[] is illegal in SeeR. Sorry :-(");
     {int ip=act.ip;
      _value val;
      //maybe we should make dynamic arrays someday? like int y=12,x[y];=>[&1a102aa]
      sciReadSubExp(1,&val,true);
      if (!scvalIsImmidiateOfType(&val,valINT)||(act.ip!=ip))
         serr2(scErr_Declaration, "Constant integer array size expected",ST->lexema);
      i=val.adr.val.ival;
      if (!lexeq("]")) serr2(scErr_Declaration, "`]' expected instead of `%s'",ST->lexema);
      itoa(i,ST->lexema,10);
     }
     ADD_to_end(tmp);
     strcpy(ST->lexema,"]");
     ADD_to_end(tmp);
     Advance;
    }
    scget_back(ST);
    strcpy(ST->lexema,"]");
//    deb1("add %s\n",tmp);
    ADD_to_start(temp,tmp);
//    deb1("temp= %s\n",temp);
   }//'['
  else
  if (lexeq("(")) {
     if (*name)//name was already found
     { char *funpar;
       funpar=scsGetFunctionParams();//containing: (10E3A1B2)
       if (funpar)
       {
        scget_back(ST);
        ADD_to_start(temp,funpar);
        scFree(funpar);
       }
       eod=1;
     } else
     {//nested definition
       if (nested) serr2(scErr_Parse, parse_error,"`('");
       nested=scsGetDefiParams(name);

       if (!lexeq(")")) serr2( scErr_Parse,parse_error,"`)'");
       Advance;
     }
  }//"("
  else
  if (lexeq(".")||lexeq("::"))
  {scSymbol *structure;
   if (!(structure=scdicFoundSymbol(SC_types->first,*name)))
        serr2( scErr_Declaration,"Structure `%s' does not exist",*name);
   if ((sctypGetValType(&structure->type)!=valSTRUCT&&
       sctypGetValType(&structure->type)!=valSCRIPT)||!(structure->adr.flags&adrTYPEONLY))
        serr2( scErr_Declaration,"`%s' is not a structure",*name);
  while(lexeq(".")||lexeq("::"))
  {
   scSymbol *newsym;
   if (!*name) serr2(scErr_Parse,parse_error,ST->lexema);
   if (!structure) serr2(scErr_Parse,parse_error,ST->lexema);
   if (!structure->type.main) //this was a fully defined structure
        serr2( scErr_Declaration,"Structure `%s' was not fully specified",*name);
   Advance;
   if (lexeq("~"))
   {int i;
    Advance;
    if (!sciValidIdentifier(ST->lexema))
          serr2(scErr_Parse, parse_error,ST->lexema);
    for(i=strlen(ST->lexema);i>=0;i--) ST->lexema[i+1]=ST->lexema[i];
    ST->lexema[0]='~';
   }
   else
   if (!sciValidIdentifier(ST->lexema))
           serr2(scErr_Parse, parse_error,ST->lexema);
   if (lexeq("operator"))
   {char *x=scReadOperatorName();
    strcpy(ST->lexema,x);scFree(x);
   }
   if (!(newsym=scdicFoundSymbol(structure->type.main,ST->lexema)))
           serr3(scErr_Declaration,"`%s' is not a member of %s",ST->lexema,structure->name);
   {//add to name
        char *x=scAlloc(strlen(*name)+strlen(ST->lexema)+3);
        strcpy(x,*name);
        strcpy(x+strlen(x),".");
        strcpy(x+strlen(x),ST->lexema);
        scFree(*name);
        *name=x;
#if DEBUGLEVEL==0
        deb1("declare:%s\n",*name);
#endif
   }
   structure=(((newsym->type.flags&typTYPE)==typSTRUCT||
               (newsym->type.flags&typTYPE)==typSCRIPT) &&
               (newsym->adr.flags&adrTYPEONLY))  ?  newsym:NULL;
   Advance;
  }//while
  scget_back(ST);
  }//if
  else
  //****************************NAME
  {
   if (IsSymbol(ST->lexema[0])) serr2( scErr_Parse,parse_error,ST->lexema);
   if (*name) serr2( scErr_Parse,parse_error,ST->lexema);
   if (lexeq("operator"))
   {*name=scReadOperatorName();
   }
   else
   *name=scStrdup(ST->lexema);//name read!
  }
 }//not ending
 }//while
 if (nested) {ADD_to_start(temp,nested);scFree(nested);}
 if (strlen(temp)) nested=scStrdup(temp);
     else nested=NULL;
 //test if there is no pointer to reference:
 if (nested&&*nested)
 {char *x=nested;
//  if (*x=='&') x++;
  x++;
  while(*x)
    {
     if (*x=='('&&*nested=='&')
           serr( scErr_Declaration,"invalid reference");
     if (*x++=='&'&&*(x-2)!=')')//
        {
#if DEBUGLEVEL==0
         deb1("type=%s\n",nested);
#endif
           serr( scErr_Declaration,"invalid reference");
        }
    }
 }
 return nested;
}

///////////////////////////////////////////////////////////////////////////
////scsGetTypeDeli/////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//does mallocs scType
scSymbol *scsGetTypeDeli()//scType* type)
{scSymbol *structure=NULL;
 scPosition p;
 scSymbol *sym1;
 scType* WriteAlsoTo=NULL;

 scSymbol *sym;
 sym=New(scSymbol);
 sym->name=NULL;
 sym->import_name=NULL;
 sym->val=NULL;
 sym->adr.address=-1;
 sym->type.flags=0;
 sym->type.params=NULL;
 sym->type.main=NULL;
 Advance;
// printf("Searching for %s...\n",ST->lexema);
  if (IsSymbol(ST->lexema[0]))
   {scFree(sym);
    return NULL;
   }

  if (lexeq("script"))//---------------------------------------------------
  {
   int size=12;//first 3 ints are explained in internal.h at 'scScriptTypeHeader'
   scSymbol OldThisSymbol=this_symbol;
   scSymbol *sym1=NULL;
//   if (this_symbol.reserved)
//    serr(scErr_Declaration,"script cannot be a member of any function or structure");
   scAssert(scsGetTypeDeli,sym->type.flags==0);
   sym->type.flags=typSCRIPT;
   if (this_symbol.reserved&&
      (this_symbol.type.main->type.flags&typSCRIPT||
       this_symbol.type.main->adr.flags&adrSCRIPT))
           serr( scErr_Declaration,"script cannot be a member of script");
   Advance;
   if (!lexeq("{"))//name:
     {
      if (!sciValidIdentifier(ST->lexema))
         serr2(scErr_Declaration, "Invalid identifier `%s' in script declaration",ST->lexema);
      if ((structure=scdicFoundSymbol(SC_types->first,ST->lexema)))
        {
         if (sctypGetValType(&structure->type)!=valSCRIPT)
           serr2( scErr_Declaration,"`%s' already defined as non-script",ST->lexema);
         if (structure->type.main) //this was a fully defined structure
           serr2( scErr_Declaration,"duplicate definition of `%s'",ST->lexema);
        }
        else
        {
         structure=New(scSymbol);
         structure->name=scStrdup(ST->lexema);
         structure->adr.flags=adrTYPEONLY;
         deb1("Script %s created.\n",structure->name);
        }
        structure->adr.address=-1;
        structure->type.flags=typSCRIPT;
        structure->type.main=NULL;
        structure->params=0;
        structure->val=NULL;
        structure->parent=this_symbol.type.main;
      WriteAlsoTo=&structure->type;

      Advance;
     }else//TODO:if noname,we don't care
      serr2(scErr_Declaration, "Identifier required in script declaration",ST->lexema);

   if (lexeq(";")) //declaration of script
     {
      scget_back(ST);
      if (WriteAlsoTo) *WriteAlsoTo=sym->type;
      sym->type.flags=typUSERDEFINED;
      sym->type.params=NULL;
      sym->type.main=structure;
      return sym;
     }

   if (!lexeq("{")) serr2( scErr_Parse,parse_error,ST->lexema);
   Advance;scget_back(ST);
   sym1=NULL;
   this_symbol.reserved=this_symbol.type.main=structure;

   while (!lexeq("}"))
   {
    scSymbol* sym2;
    if (WriteAlsoTo) *WriteAlsoTo=sym->type;

    sym2=sctypGetDefinition();
    if (!sym2)
      {Advance;serr( scErr_Declaration,"not a type in type definition");}
//    if (sctypGetValType(&sym2->type)==valSCRIPT)
//      serr( scErr_Declaration,"script cannot be a member of any structure");
    if (!sym2->name)
      {scSymbol *x;
       if (sctypGetValType(&sym2->type)!=valSTRUCT&&sctypGetValType(&sym2->type)!=valSCRIPT)
        serr( scErr_Declaration,"member name not specified in struct declaration");
       scAssert(scsGetTypeDeli,sym2->type.flags==typUSERDEFINED);
       x=sym2;
       sym2=sym2->type.main;//sym2 means now the struct-type
       deb1("nested structure %s\n",sym2->name);
       sym2->parent=structure;
       if (WriteAlsoTo) sym->type=*WriteAlsoTo;
       //maybe they added something?
       if (!sym1)
         {sym1=sym->type.main;
          if (sym1) {size+=4;
#if DEBUGLEVEL==0
          deb1("script:Detected addition of %s at start.\n",sym1->name);
#endif
          }
         }
       while(sym1&&sym1->next)
        {sym1=sym1->next,size+=4;
#if DEBUGLEVEL==0
         deb1("script:Detected addition of %s.\n",sym1->name);
#endif
        }
//       scFree(x);
      }
    if (sctypGetValType(&sym2->type)==valSTRUCT||sctypGetValType(&sym2->type)==valSCRIPT)
    {
    if (sym2->type.flags==typUSERDEFINED&&sym2->type.main==structure&&(!sym2->type.params||!sym2->type.params[0]))
       serr2( scErr_Declaration,"recursive declaration (script '%s' inside itself)",structure->name);
    if (sym2->type.flags==typUSERDEFINED&&!sym2->type.main->type.main&&(!sym2->type.params||!sym2->type.params[0]))
       serr2( scErr_Declaration,"struct '%s' not fully specified",sym2->type.main->name);
    }
    if (!sciValidIdentifier(sym2->name))
       serr2( scErr_Declaration,"Invalid identifier `%s' in declaration",sym2->name);
    if (scdicFoundSymbol(sym->type.main,sym2->name))
       serr2( scErr_Declaration,"member `%s' already in script",sym2->name);


    if (!(sym2->adr.flags&adrTYPEONLY))
        {
        sym2->adr.address=size;
        sym2->adr.flags=adrSCRIPT;
        sym2->parent=structure;
        size+=4;
        }
    if (sym1) sym1->next=sym2;
       else {sym->type.main=sym2;if (structure) structure->type.main=sym2;}
    sym1=sym2;sym1->next=NULL;
    deb1("script:member %s added ",sym2->name);
    if (sym1->adr.flags&adrTYPEONLY) deb0("which was a type\n");
    deb0("\n\n");

    Advance;
    if (lexeq(",") && sctypIsFunction(&sym1->type))
        serr( scErr_Declaration,"function can be declared one at a time!");
    while (lexeq(","))
    {Advance;scget_back(ST);
     if (!(sym2=sctypGetShortDefinition(&sym1->type)))
       serr( scErr_Declaration,"not a type in type definition");

     if (!sym2->name)
        serr( scErr_Declaration,"type name not specified in type definition");
//    if (sctypGetValType(&sym2->type)==valSCRIPT)
//      serr( scErr_Declaration,"script cannot be a member of any structure");
    if (!sciValidIdentifier(sym2->name))
         serr2( scErr_Declaration,"Invalid identifier `%s' in declaration",sym2->name);
    if (sctypIsFunction(&sym2->type))
        serr( scErr_Declaration,"function can be declared one at a time!");

    if (!(sym2->adr.flags&adrTYPEONLY))
        {
        sym2->adr.address=size;
        sym2->adr.flags=adrSCRIPT;
        sym2->parent=structure;
        size+=4;
        }
     sym1->next=sym2;
     sym1=sym2;sym1->next=NULL;
     deb1("script:member %s added\n\n",sym2->name);
     Advance;
    }
    if (lexeq("{") && sctypIsFunction(&sym2->type))
    {
      serr2( scErr_Declaration,"What? You want to define function `%s'? It'll come in the script!",sym2->name);
    } else
    if (!lexeq(";")) serr2( scErr_Parse,parse_error,ST->lexema);
    Advance;scget_back(ST);
   }

   Advance;
   if (WriteAlsoTo) *WriteAlsoTo=sym->type;
   sym->type.flags=typUSERDEFINED;
   sym->type.params=NULL;
   sym->type.main=structure;
   this_symbol=OldThisSymbol;
#ifdef DEBUG
   {char memberlist[2048*4];
    sctypTypeOfStr(&structure->type,NULL,memberlist);
    deb1("Script %s\n",memberlist);
   }
#endif
   return sym;
  }
  else




  if (lexeq("struct")||lexeq("class"))//--------------------------------------------
  {//struct
   int size=0;
   int protection_type=0;//0-private,1-protected,2-public
   scSymbol OldThisSymbol=this_symbol;
   scSymbol *in_script=NULL;
   char prefix[1024];
   scSymbol *sym1=NULL;

   //structures by default are public
   if (lexeq("struct")) protection_type=2;

   scAssert(scsGetTypeDeli,sym->type.flags==0);
   sym->type.flags=typSTRUCT;
   prefix[0]=0;
   Advance;

   if (!lexeq("{"))//name:
     {
      if (!sciValidIdentifier(ST->lexema))
         serr2(scErr_Declaration, "Invalid identifier `%s' in struct declaration",ST->lexema);
      //WORK:
      if (
         (!this_symbol.type.main&&(structure=scdicFoundSymbol(SC_types->first,ST->lexema)))
         ||
         (this_symbol.type.main&&(structure=scdicFoundSymbol(this_symbol.type.main->type.main,ST->lexema)))
         )
        {
         if (sctypGetValType(&structure->type)!=valSTRUCT)
           serr2( scErr_Declaration,"`%s' already defined as non-structure",ST->lexema);
         if (structure->type.main) //this was a fully defined structure
           serr2( scErr_Declaration,"duplicate definition of `%s'",ST->lexema);
        }
        else
        {
         structure=New(scSymbol);
         structure->name=scStrdup(ST->lexema);
         structure->adr.flags=adrTYPEONLY;
         if (this_symbol.type.main&&
            (this_symbol.type.main->type.flags&typSCRIPT||
             this_symbol.type.main->adr.flags&adrSCRIPT))
            {scSymbol *p=this_symbol.type.main;
             strcpy(prefix,structure->name);
             strcpy(prefix+strlen(prefix),".");
             while(p&&!in_script)
             {
#if DEBUGLEVEL==0
              deb1(".%s\n",p->name);
#endif
              if (p->type.flags&typSCRIPT) in_script=p;
              if (!in_script)
               {char prefix2[2048];
                strcpy(prefix2,prefix);
                strcpy(prefix,p->name);
                strcpy(prefix+strlen(prefix),".");
                strcpy(prefix+strlen(prefix),prefix2);
               }
              p=p->parent;
             }
#if DEBUGLEVEL==0
             deb1("In script %s ",in_script->name);
#endif
             structure->adr.flags|=adrSCRIPT;
            }
         deb1("Struct %s created.\n",structure->name);
#if DEBUGLEVEL==0
         if (prefix[0]) deb1("With prefix = '%s'\n",prefix);
#endif
        }
        structure->adr.address=-1;
//        structure->adr.flags=0;//&4+8
        structure->type.flags=typSTRUCT;
        structure->params=0;
        structure->val=NULL;
        structure->parent=this_symbol.type.main;
        structure->type.main=NULL;
//        structure->next=NULL;
//      structure->add=type;
      WriteAlsoTo=&structure->type;

      Advance;
     }else//TODO:if noname,we don't care
      serr2(scErr_Declaration, "Identifier required in struct declaration",ST->lexema);

   if (lexeq(";")) //declaration of structure
     {
      scget_back(ST);
      if (WriteAlsoTo) *WriteAlsoTo=sym->type;
      sym->type.flags=typUSERDEFINED;
      sym->type.params=NULL;
      sym->type.main=structure;
      return sym;
     }

   if (!lexeq("{")) serr2( scErr_Parse,parse_error,ST->lexema);
   Advance;scget_back(ST);
   sym1=NULL;
   this_symbol.reserved=this_symbol.type.main=structure;

   while (!lexeq("}"))
   {int k,imported=0,ppp=1;
    char *iname=NULL;
    scSymbol* sym2;

    while(ppp)//read public/protected/private (hence ppp)
    {
     ppp=0;
     if (lexeq("public"))
      {Advance;Advance;
       if (!lexeq(":")) serr2( scErr_Parse,parse_error,ST->lexema);
       protection_type=2;
       Advance;scget_back(ST);ppp=1;
      }
      else
     if (lexeq("private"))
      {Advance;Advance;
       if (!lexeq(":")) serr2( scErr_Parse,parse_error,ST->lexema);
       protection_type=0;
       Advance;scget_back(ST);ppp=1;
      }
      else
     if (lexeq("protected"))
      {Advance;Advance;
       if (!lexeq(":")) serr2( scErr_Parse,parse_error,ST->lexema);
       protection_type=1;
       Advance;scget_back(ST);ppp=1;
      }
    }
    if (lexeq("import"))
     {
      imported=sciReadImport(&iname)+1;

//      Advance;Advance;scget_back(ST);
//      imported=sciReadIntegerInBrackets(0)+1;//dispatcher increased by 1
//      deb1("import read, next=%s\n",ST->lexema);
//      Advance;
//      scget_back(ST);
     }
    if (structure->name&&lexeq("~"))
     {
      Advance;Advance;
      scget_back(ST);
//      deb0("Destructor reading:\n");
      sym2=sctypGetDefinition();
      if (sym2)
      {char *x=scAlloc(strlen(sym2->name)+2);
//       deb1("Renaming to ~%s\n",sym2->name);
       x[0]='~';
       strcpy(x+1,sym2->name);
       scFree(sym2->name);
       sym2->name=x;
      }
//      else
     }
     else
      sym2=sctypGetDefinition();

    if (!sym2)
      {Advance;serr( scErr_Declaration,"not a type in type definition");}

    if (iname) sym2->import_name=iname;

    if (!sym2->name)
      {scSymbol *x;
       if (sctypGetValType(&sym2->type)!=valSTRUCT&&sctypGetValType(&sym2->type)!=valSCRIPT)
        serr( scErr_Declaration,"member name not specified in struct declaration");
       scAssert(scsGetTypeDeli,sym2->type.flags==typUSERDEFINED);
       x=sym2;
       sym2=sym2->type.main;//sym2 means now the struct-type
       deb1("nested structure %s\n",sym2->name);
       sym2->parent=structure;
       scFree(x);
      }
    if (sctypGetValType(&sym2->type)==valSTRUCT||sctypGetValType(&sym2->type)==valSCRIPT)
    {
    if (sym2->type.flags==typUSERDEFINED&&sym2->type.main==structure&&(!sym2->type.params||!sym2->type.params[0]))
       serr2( scErr_Declaration,"recursive declaration (struct '%s' inside itself)",structure->name);
    if (sym2->type.flags==typUSERDEFINED&&!sym2->type.main->type.main&&(!sym2->type.params||!sym2->type.params[0]))
       serr2( scErr_Declaration,"struct '%s' not fully specified",sym2->type.main->name);
    }
    if (sym2->name&&sym2->name[0]=='~'&&strcmp(sym2->name+1,structure->name)==0)
    {}
    else
    if (!sciValidIdentifier(sym2->name))
       serr2( scErr_Declaration,"Invalid identifier `%s' in declaration",sym2->name);
    if (scdicFoundSymbol(sym->type.main,sym2->name))
      {
       scSymbol *p=sym->type.main;
       if (sctypIsFunction(&sym2->type))
       {
        while(p)
        {
         if (strcmp(sym2->name,p->name)==0&&
           (!sctypIsFunction(&p->type)||sctypExactFunction(&p->type,&sym2->type)))
           serr2( scErr_Declaration,"member `%s' already in structure",sym2->name);
         p=p->next;
        }
       }
       else
        serr2( scErr_Declaration,"member `%s' already in structure",sym2->name);
      }


    if (!sctypIsFunction(&sym2->type)&&!(sym2->adr.flags&adrTYPEONLY))
        {
        if (imported)
                serr2( scErr_Declaration,"A member variable (%s) cannot be imported",sym2->name);
        k=sctypInsistsAlignment(&sym2->type);
        if (k&&size%k) size+=k-size%k;
        sym2->adr.address=size;
        size+=sctypSizeOf(&sym2->type);
        sym2->parent=structure;
        }
        else//function
        {if (sctypIsFunction(&sym2->type)&&in_script)//add also to script
         {scSymbol *x;
          scSymbol *t;
          int i=strlen(prefix);
          t=New(scSymbol);
          *t=*sym2;
          strcpy(prefix+i,sym2->name);
          deb2("script '%s':add function %s\n",in_script->name,prefix);
          t->name=scStrdup(prefix);
          t->next=NULL;
          prefix[i]=0;//return to prev. state
          t->adr.address=0;
          if (!in_script->type.main)
           in_script->type.main=t;
           else
           {
            x=in_script->type.main;
            while(x->next) x=x->next;
            t->adr.address=x->adr.address+4;
            x->next=t;
           }
          sym2->adr.address=t->adr.address;
         }
        }
    if (sym1) sym1->next=sym2;
       else {sym->type.main=sym2;if (structure) structure->type.main=sym2;}
    sym1=sym2;sym1->next=NULL;
    deb1("struct:member %s added ",sym2->name);
    if (sym1->adr.flags&adrTYPEONLY) deb0("which was a type\n");
    deb0("\n\n");

    Advance;
    if (lexeq(",") && sctypIsFunction(&sym1->type))
        serr( scErr_Declaration,"function can be declared one at a time!");
    while (lexeq(","))
    {Advance;scget_back(ST);
     if (!(sym2=sctypGetShortDefinition(&sym1->type)))
       serr( scErr_Declaration,"not a type in type definition");

     if (!sym2->name)
        serr( scErr_Declaration,"type name not specified in type definition");
    if (!sciValidIdentifier(sym2->name))
         serr2( scErr_Declaration,"Invalid identifier `%s' in declaration",sym2->name);
    if (sctypIsFunction(&sym2->type))
        serr( scErr_Declaration,"function can be declared one at a time!");

    if (!(sym2->adr.flags&adrTYPEONLY))
        {
        if (imported)
                serr2( scErr_Declaration,"A member variable (%s) cannot be imported",sym2->name);
        k=sctypInsistsAlignment(&sym2->type);
        if (k&&size%k) size+=k-size%k;
        sym2->adr.address=size;
        size+=sctypSizeOf(&sym2->type);
        sym2->parent=structure;
        }
     sym1->next=sym2;
     sym1=sym2;sym1->next=NULL;
     deb1("struct:member %s added\n\n",sym2->name);
     Advance;
    }
    if (lexeq("{") && sctypIsFunction(&sym2->type))
    { scSymbol X=this_symbol;
      if (imported)
        serr2( scErr_Declaration,"What? You want to define function `%s'? It's already imported!",sym2->name);
#if DEBUGLEVEL==0
      deb1("member function of type %d!\n",sctypGetValType(&sym2->type));
#endif
      //Warning:what if it is happening in the
      //function already? Have to store all the data! And destroy the
      //struct immidiately after function ends.
      //Or abolish this!

      if (WriteAlsoTo) *WriteAlsoTo=sym->type;
      scget_back(ST);
      sciReadDefinition(NULL,sym2,structure);
      deb0("End of member function\n\n");
      this_symbol=X;
    } else
    if (!lexeq(";")) serr2( scErr_Parse,parse_error,ST->lexema);
    if (imported)//has to be function
    {
        if (!sctypIsFunction(&sym2->type))
           serr( scErr_Declaration,"Member variables cannot be imported.");
        sym2->adr.flags=adrIMPORTED+adrCPU;
        sym2->adr.val.ival=imported-1;
        sciCountFunctionParams(sym2,structure,0);
        sym2->adr.address=-1;
        //sciAddImportedFunction(sym2,structure);//called when used for 1st time
    }
    Advance;scget_back(ST);
   }

   Advance;
   if (WriteAlsoTo) *WriteAlsoTo=sym->type;
   sym->type.flags=typUSERDEFINED;
   sym->type.params=NULL;
   sym->type.main=structure;
   this_symbol=OldThisSymbol;
#ifdef DEBUG
   {char memberlist[2048*4];
   sctypTypeOfStr(&structure->type,NULL,memberlist);
   deb1("STRUCT %s\n",memberlist);
   }
#endif
   return sym;
  }
  else
  if (lexeq("union"))
  {//union
   serr( scErr_Declaration,"Sorry.Unions not implemented yet");
  }
  else
  if (lexeq("enum"))
  {//enum
   serr( scErr_Declaration,"Sorry.Enums not implemented yet");
  }
  else //simple type:
  {int i=1;
   int special_was_signed=0;
//   char **((*KLS)()) (int);//=>**(int)()*//won't be able to be done!
//   KLS()();
   sym->type.flags=0;
   //prefixes
   while(i)
   {i=0;
    if (lexeq("static"))
     {Advance;sym->type.flags|=typSTATIC;i=1;
     }
    if (lexeq("const"))
     {Advance;sym->type.flags|=typCONST;i=1;
     }
    if (lexeq("unsigned"))
     {Advance;sym->type.flags|=typUNSIGNED;i=1;
      if (special_was_signed) serr(scErr_Declaration,"Declare both signed and unsigned?");
     }
    if (lexeq("signed"))
     {Advance;i=1;special_was_signed=1;
      if (sym->type.flags&typUNSIGNED) serr(scErr_Declaration,"Declare both signed and unsigned?");
     }
    if (lexeq("register"))
     {Advance;sym->type.flags|=typREGISTER;i=1;
     }
   }

//Store the position of starting of the real type or name if constructor/destr
   scget_back(ST);
   scGetPosition(ST,&p);
   Advance;

   for (i=0;Standard_Types[i].name;i++)
     if (lexeq(Standard_Types[i].name))
        {
#if DEBUGLEVEL==0
         deb1("Declaration:standard `%s' ",ST->lexema);
#endif
         if (i==valLONG) //i=0;//don't convert long to int
          {serr(scErr_Declaration, "Type long is a 64bit long, yet not supported. \n(if you need 32-bit long, use int instead)");
          }
         sym->type.flags|=typPREDEFINED;sym->type.main=(scSymbol *)i;
         if (WriteAlsoTo) *WriteAlsoTo=sym->type;
         if (sym->type.flags&typUNSIGNED)
          {
#if DEBUGLEVEL==0
          deb0("It is UNSIGNED!\n");
#endif
          switch(i)
           {case valINT:case valCHAR:case valSHORT:break;
            default:serr(scErr_Declaration,"Only char,short and int can be unsigned");
           }
          }
         return sym;
        }
   //only unsigned
   if (sym->type.flags&typUNSIGNED||special_was_signed)
   {
#if DEBUGLEVEL==0
    deb0("Declaration:pure `unsigned' or `signed' ");
#endif
    sym->type.flags|=typPREDEFINED;sym->type.main=(scSymbol *)valINT;//int
    if (WriteAlsoTo) *WriteAlsoTo=sym->type;
    return sym;
   }
   //predefined type
   if (sym->type.flags&typUNSIGNED||special_was_signed)
   {
    serr( scErr_Declaration,"invalid type modifier");
   }
   if (this_symbol.reserved)//we're in a struct
   {
#if DEBUGLEVEL==0
    deb1("Searching in parent '%s'...\n",this_symbol.type.main->name);
#endif
    if ((sym1=scdicFoundSymbol(this_symbol.type.main->type.main,ST->lexema))
       )
    {
    if (sym1->adr.flags&adrTYPEONLY)
     {
#if DEBUGLEVEL==0
      deb2("Declaration:user-defined `%s.%s' ",this_symbol.type.main->name,ST->lexema);
#endif
      sym->type.flags|=typUSERDEFINED;sym->type.main=sym1;
      Advance;
      goto STRUCT_DECLARE;
     }
#if DEBUGLEVEL==0
     deb0("Found, but not typeonly\n");
#endif
    }
    if (lexeq(this_symbol.type.main->name))
    {
#if DEBUGLEVEL==0
     deb1("Declaration:parent `%s' ",ST->lexema);
#endif
     sym->type.flags|=typUSERDEFINED;sym->type.main=this_symbol.type.main;
     Advance;
     if (lexeq("("))
     {
#if DEBUGLEVEL==0
      deb0("Constructor!!!\n");
#endif
      scSetPosition(p);Advance;
      sym->type.flags=typPREDEFINED;
      sym->type.main=(scSymbol*)valVOID;
      scget_back(ST);
      if (WriteAlsoTo) *WriteAlsoTo=sym->type;
      return sym;
     }
     goto STRUCT_DECLARE;
    }
   }
   if ((sym1=scdicFoundSymbol(SC_types->first,ST->lexema)))
   {
#if DEBUGLEVEL==0
    deb1("Declaration:user-defined `%s' ",ST->lexema);
#endif
    sym->type.flags|=typUSERDEFINED;sym->type.main=sym1;
    Advance;
    if (lexeq("::")||lexeq("."))
    {
    }
    goto STRUCT_DECLARE;
   }
  scget_back(ST);
  }
 scFree(sym);
 return NULL;

//disgusting gotos? without them, it wouldn't look better...
STRUCT_DECLARE:
 while(lexeq(".")||lexeq("::"))
 {scSymbol *n;
  Advance;
  if ((sctypGetValType(&sym->type.main->type)!=valSTRUCT&&
       sctypGetValType(&sym->type.main->type)!=valSCRIPT)||
     !(sym->type.main->adr.flags&adrTYPEONLY))
        serr2( scErr_Declaration,"`%s' is not a structure",sym->type.main->name);
  if (lexeq("~")||lexeq(sym->type.main->name))
  {
#if DEBUGLEVEL==0
      deb0("Con/Destructor!!!\n");
#endif
      scSetPosition(p);Advance;
      sym->type.flags=typPREDEFINED;
      sym->type.main=(scSymbol*)valVOID;
      scget_back(ST);
      if (WriteAlsoTo) *WriteAlsoTo=sym->type;
      return sym;
  }
  n=scdicFoundSymbol(sym->type.main->type.main,ST->lexema);
  if (!n)
    serr3(scErr_Declaration,"`%s' is not a member of %s",ST->lexema,sym->type.main->name);
  sym->type.main=n;
  Advance;
 }
 scget_back(ST);
 if (WriteAlsoTo) *WriteAlsoTo=sym->type;
 return sym;
}

///////////////////////////////////////////////////////////////////////////
////sctypGetShortDefinition//////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//e.g. int x,y;
//           ^-short(without first
scSymbol *sctypGetShortDefinition(scType* typ)
{
 scSymbol *temp=New(scSymbol);
 temp->name=NULL;
 temp->val=NULL;
 temp->adr.flags=0;
 temp->adr.address=-1;
// temp->add=scsGetTypeDeli();
// temp->add=New(scType);
// memcpy(temp->add,typ,sizeof(scType));
 temp->type=*typ;//copy of typ
 temp->type.params=scsGetDefiParams(&temp->name);

 if (!temp->type.params)
 {
#if DEBUGLEVEL==0
  deb1("Short decl of `%s'...\n",temp->name?temp->name:"anonymous");
#endif
 }
 else
 {
#if DEBUGLEVEL==0
  deb2("Short decl of `%s' as %s...\n",temp->name?temp->name:"anonymous",temp->type.params);
#endif
 }
#ifdef DEBUG
 {char t[1024];
  sctypTypeOfStr(&temp->type,temp->name,t);
  deb1("ShortDecl %s\n",t);
 }
#endif
 return temp;
}

///////////////////////////////////////////////////////////////////////////
////sctypGetDefiniton//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
scSymbol *sctypGetDefinition()
{
/* scSymbol *temp=New(scSymbol);
 temp->name=NULL;
 temp->val=NULL;
 temp->adr.address=-1;*/
 scSymbol *temp;
// temp->imported=0;
 temp=scsGetTypeDeli();
 if (!temp) return NULL;
// if (!scsGetTypeDeli(temp)) {scFree(temp);return NULL;}
 temp->type.params=scsGetDefiParams(&temp->name);

 if (!temp->type.params)
 {
#if DEBUGLEVEL==0
  deb1(" of `%s'...\n",temp->name?temp->name:"anonymous");
#endif
 }
 else
 {
#if DEBUGLEVEL==0
  deb2(" of `%s' as <%s>...\n",temp->name?temp->name:"anonymous",temp->type.params);
#endif
 }
#ifdef DEBUG
 {char t[1024];
  sctypTypeOfStr(&temp->type,temp->name,t);
  deb1("Declare %s\n",t);
 }
#endif
 return temp;
}

