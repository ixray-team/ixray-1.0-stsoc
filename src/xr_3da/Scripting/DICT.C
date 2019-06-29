/************************************************************************
       -= SEER - C-Scripting engine v 0.1a =-
State      : under implementation
Author     : Przemyslaw Podsiadly
             email: ppodsiad@elka.pw.edu.pl
             WWW:   http://home.elka.pw.edu.pl/~ppodsiad
File:dict.c
Desc:dictionary struct - for linked list of symbols
************************************************************************/
#define INTERNAL_FILE
#include <seer.h>
#include "internal.h"

///////////////////////////////////////////////////////////////////////////
////scdicAddSymbol_Ex//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void scdicAddSymbol_Ex(scDictionary *dict,scSymbol* s)
{s->next=dict->first;
 dict->first=s;
}

///////////////////////////////////////////////////////////////////////////
////scdicAddSymbolToEnd_Ex/////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void scdicAddSymbolToEnd_Ex(scDictionary *dict,scSymbol* s)
{scSymbol *p=dict->first;
 if(!p)
 {
  s->next=dict->first;
  dict->first=s;
 }
 else
 {while(p->next) p=p->next;
  p->next=s;
  s->next=NULL;
 }
}

///////////////////////////////////////////////////////////////////////////
////scdicAddSymbol/////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void scdicAddSymbol(scDictionary *dict,char *nam,int addr,int typeflags,void *reserved)
{if (!dict->first)
    {
     dict->first=New(scSymbol);
     dict->first->next=NULL;
    } else
    {scSymbol* temp=New(scSymbol);
     temp->next=dict->first;
     dict->first=temp;
    }
 dict->first->adr.address=addr;
 dict->first->type.flags=typeflags;
 dict->first->name=scStrdup(nam);//xr_malloc(strlen(nam)+1);
 dict->first->reserved=reserved;
}

///////////////////////////////////////////////////////////////////////////
////scdicRemoveSymbol//////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void scdicRemoveSymbol(scDictionary *dict,char *nam)
{scSymbol *t=dict->first,*p=NULL;
 while(t)
  {if (strcmp(nam,t->name)==0)
      {
       if (p) {p->next=t->next;}else dict->first=t->next;
//       scFree(t);
       return;
      }
   p=t;
   t=t->next;
  }
}

///////////////////////////////////////////////////////////////////////////
////scdicStoreDictionary///////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
scSymbol* scdicStoreDictionary(scDictionary *dict)
{ return dict->first;
}

///////////////////////////////////////////////////////////////////////////
////scdicRestoreDictionary/////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void scdicRestoreDictionary(scDictionary *dict,scSymbol *frst,void Remove(scSymbol *))
{ scSymbol *x=dict->first;
 while(dict->first!=frst)
 {x=dict->first->next;
  if (&Remove)
   Remove(dict->first);
  dict->first=x;
 }
}


///////////////////////////////////////////////////////////////////////////
////scdicFreeFunctonSymbol/////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void scdicFreeFunctionSymbol(scSymbol* sym)
{
 deb1("Removing %s\n",sym->name?sym->name:"anonymous");
 if (sym->name)scFree(sym->name);
 if (sym->type.params)
   {char *x=sym->type.params;
     scSymbol* t;
     while (*x)
      {
       if (*x=='(')//parameters:
          {
           x++;
           while (*x&&*x!=')')
           {
            if (x[0]=='.'&&x[1]=='.'&&x[2]=='.')
               {x+=3;
               }
             else
               {
                 t=(scSymbol*)strtoul(x,&x,16);
                 scdicFreeFunctionSymbol(t);
                 if (*x=='=')//defaultinitializer
                 {_value *v=(_value*)strtoul(++x,&x,16);
                  scFree(v);
                 }

               }
            if (*x==',') x++;
             else if (*x!=')') serr(scErr_Internal, "Internal compiler error at function params!");
           }
          }
       x++;
      }
     scFree(sym->type.params);
   }
 scFree(sym);
}

///////////////////////////////////////////////////////////////////////////
////scdicFoundSymbol///////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
scSymbol* scdicFoundSymbol(scSymbol *first,char *nam)
{scSymbol *t=first;
 while(t)
  {if (strcmp(nam,t->name)==0) return t;
   t=t->next;
  }
 return NULL;
}

///////////////////////////////////////////////////////////////////////////
////scdicNewDictionary/////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
scDictionary *scdicNewDictionary()
{scDictionary *temp=New(scDictionary);
 temp->first=NULL;
 return temp;
}

///////////////////////////////////////////////////////////////////////////
////scdicDeleteDictionary//////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
scDictionary* scdicDeleteDictionary(scDictionary *l)
{scSymbol *t=l->first,*o;
 while(t)
 {scFree(t->name);
  o=t;
  t=t->next;
  scFree(o);
 }
 scFree(l);
 return l;
}
