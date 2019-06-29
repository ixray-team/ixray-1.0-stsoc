/************************************************************************
       -= SEER - C-Scripting engine v 0.3a =-
State      : under implementation
Author     : Przemyslaw Podsiadly
             email: ppodsiad@elka.pw.edu.pl
             WWW:   http://home.elka.pw.edu.pl/~ppodsiad
File:parser.c
Desc:parsing - reading lexemas from input
************************************************************************/
#define INTERNAL_FILE
#include <seer.h>
#include "internal.h"

int parser_Comment=0;
///////////////////////////////////////////////////////////////////////////
////scScript_Error/////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//does work only for compile error, not runtime,
//linenum-in compiler code, not in script
void scScript_Error(Source_Text* ST,char* filename,int linenum,int errnum,char *er, ...)
{
  va_list arg;
//  if (scErrorNo) return;
#ifndef DEBUG
  linenum=0;
#endif
  va_start (arg, er);
  {
    if (scErrorNo)//show previous errors
     {
      sprintf (scErrorMsg,"%s,\n%s(%3d) Error",scErrorMsg,ST->name,ST->act_line+1+ST->linedif);
     }
     else
     sprintf (scErrorMsg,"%s(%3d) Error",ST->name,ST->act_line+1+ST->linedif);
    if (linenum)
     sprintf (scErrorMsg,"%s[%s:%d] :",scErrorMsg,filename,linenum);
     else
     sprintf (scErrorMsg,"%s :",scErrorMsg);
    scErrorNo=errnum;
    vsprintf (scErrorMsg+strlen(scErrorMsg),er, arg);
    if ((ST->act_line<ST->num_lines)&&(STALine))
    strncpy(scErrorLine,STALine,255);
    else
    strcpy(scErrorLine,"<unknown line>");
#ifdef DEBUG
   if (debug_flag)
   {
    fprintf (debfile,"\nScript(%3d) Error[%s:%d]: ",ST->act_line+1,filename,linenum);
    vfprintf (debfile,er, arg);
    if ((ST->act_line<ST->num_lines)&&(STALine))
    fprintf (debfile,"\nLine:%s\n",STALine);
    else
    fprintf (debfile,"\n");
   }
//    fclose(debfile);
#endif
//    exit (1);
  }
  va_end (arg);
}

///////////////////////////////////////////////////////////////////////////
////scRuntime_Error////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//does work only for runtime
void scRuntime_Error(char *er, ...)
{
  va_list arg;
//  if (scErrorNo) return;
  va_start (arg, er);
  {
    if (scErrorNo)//show previous errors
     {
      sprintf (scErrorMsg,"%s,\nRuntime error ",scErrorMsg);
     }
     else
     sprintf (scErrorMsg,"Runtime error ");
    if (scActual_Instance)
     {
      if (scActual_Instance->forked)
      sprintf(scErrorMsg,"%s in `%s'(%d):",scErrorMsg,scGet_Instance_Title(scActual_Instance),scActual_Instance->flags.forkno);
      else
      sprintf(scErrorMsg,"%s in `%s':",scErrorMsg,scGet_Instance_Title(scActual_Instance));
     }
     else
      sprintf(scErrorMsg,"%s in :",scErrorMsg);
    if (scErrorNo!=scErr_Violation)
       scErrorNo=scErr_Runtime;
    vsprintf (scErrorMsg+strlen(scErrorMsg),er, arg);
#ifdef DEBUG
   if (debug_flag)
   {
    fprintf (debfile,"\nRuntime Error: ");
    vfprintf (debfile,er, arg);
   }
#endif
  }
  va_end (arg);
}

///////////////////////////////////////////////////////////////////////////
////GetChar////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
char GetChar(Source_Text *ST)
{
 char c;
 if ((ST->act_char>0)&&(!STALine[ST->act_char-1]))
  {
   ST->act_line++;
   ST->act_char=0;
   ST->newline++;
  }
 if (ST->act_line>ST->num_lines)
    c=26;
    else
    {
     c=STALine[ST->act_char++];
    }
return c;
}

///////////////////////////////////////////////////////////////////////////
////UnGetChar//////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void UnGetChar()
{
if (ST->act_char>1||!ST->act_line)
 ST->act_char--;
  else
 {
  ST->act_line--;ST->newline--;
  if (ST->act_line<0)
     serr(scErr_Parse, "Internal parser error!");
  ST->act_char=strlen(ST->lines[ST->act_line])+1;
 }
}

#define IsWhitespace(c) (c==9 || c==32 || c==13 || c==10)

///////////////////////////////////////////////////////////////////////////
////scomit_whitespaces/////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
BOOL scomit_whitespaces(Source_Text* ST)
{ char c;
  if (ST->newline==-1) ST->newline=1; else ST->newline=0;
  ST->whitespaces=0;
  do
  {
    c=GetChar(ST);
    if (c==26) return false;  // end of text reached
    ST->whitespaces++;
  }
  while (IsWhitespace(c) || !c); // skip white spaces
  ST->whitespaces--;
  UnGetChar();  // put the first non-space character back

  return true;
}

///////////////////////////////////////////////////////////////////////////
////scget_symbol///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//gets symbol or word
BOOL scget_symbol (Source_Text* ST)
{
  char *p, c;
  int i;
  ST->lexema[0]='\0';
  if (!(scomit_whitespaces(ST))) return false;
  c=GetChar(ST);
  if (c==26) return false;
  ST->lexstart=ST->act_char-1;
  if ((c==';')||(c=='"')||(c=='\'')||(c==')')||(c=='(')||((unsigned)c>127))
   {ST->lexema[0]=c;
    ST->lexema[1]=0;
    return true;
   }
  p = ST->lexema;
  UnGetChar();
  //symbol
  if (IsSymbol(c))
  {
   c=GetChar(ST);
   *p = c;
   c=GetChar(ST);
   if (!IsSymbol(c)) {UnGetChar();*(p+1)=0;return true;}
   *(p+1) = c;
   c=GetChar(ST);
   if (IsSymbol(c)){
   *(p+2) = c;
   *(p+3) = 0;
   for (i=0;Special_Symbols[i];i++)
   if (strcmp(ST->lexema,Special_Symbols[i] )==0) return true;
   };
   UnGetChar();
   *(p+2) = 0;
   for (i=0;Special_Symbols[i];i++)
   if (strcmp(ST->lexema,Special_Symbols[i] )==0) return true;
   UnGetChar();
   *(p+1) = 0;
   return true;
  }
  //name
  for (p = ST->lexema; ; p++)
  {
    c=GetChar(ST);
    if ((c>32&&c<127)&&
       ((!IsSymbol(c))||((c=='.')&&isdigit(ST->lexema[0]))))  // printable char?
    {
       if (p-ST->lexema>=DIMENSION(ST->lexema)-1) //return false;
        serr2(scErr_Parse,"Too long lexema %s",ST->lexema);
        *p = c; *(p+1) = 0;
    }
    else  // white space
    {
      UnGetChar();
      return true;  // lexema loaded
    }
  }
   return true;
}

///////////////////////////////////////////////////////////////////////////
////scSetPosition//////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void scSetPosition(scPosition n)
     {
       n.ST->act_line=n.lin;
       n.ST->act_char=n.chr;
     }

///////////////////////////////////////////////////////////////////////////
////scGetPosition//////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void scGetPosition(Source_Text *ST,scPosition *n)
     {
       n->ST=ST;
       n->lin=n->ST->act_line;
       n->chr=n->ST->act_char;
     }

char *eofstring_err="End of file reached inside string definition";

///////////////////////////////////////////////////////////////////////////
////scget_back/////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void scget_back (Source_Text* ST)
{scSetPosition(ST->lastpos);}
/*
        if (c=='0') {                     \
                                          \
          temp[i++]=0;                    \
          }                               \
       else                               \
*/


#define READ_INSIDE_STRING                \
   c=GetChar(ST);                            \
   if (c=='\\')                           \
      {                                   \
       c=GetChar(ST);                        \
       if (act.during==duringCOMPILING)   \
       {                                  \
       if (c==26)                         \
          serr(scErr_Parse,eofstring_err);\
       if (c=='n') temp[i++]='\n';        \
       else                               \
       if (c=='r') temp[i++]='\r';        \
       else                               \
        if (c=='t') temp[i++]='\t';       \
       else                               \
        if (c=='\\') temp[i++]='\\';      \
       else                               \
        if (c=='"') {temp[i++]='"';c=' ';}\
       else                               \
        if(c=='\''){temp[i++]='\'';c=' ';}\
       else                               \
        {temp[i++]='\\';temp[i++]=c;}     \
       }                                  \
       else                               \
       {temp[i++]='\\';temp[i++]=c;c=' ';}\
      } else                              \
      {                                   \
       if (c==26)                         \
          serr(scErr_Parse,eofstring_err);\
       if (c==0||c==13||c==10) c='\n';    \
       temp[i++]=c;                       \
      }

///////////////////////////////////////////////////////////////////////////
////scget_lexema///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
BOOL scget_lexema (Source_Text* ST)
{
 BOOL t;
 int i=0;
 char c;
 scGetPosition(ST,&ST->lastpos);
 t=scget_symbol(ST);

 if (!t) return t;

 if (!parser_Comment&&strcmp(ST->lexema,"\"")==0)
   {//Read all string
    char temp[10000];
    temp[i++]='\"';
    do{
     READ_INSIDE_STRING
    }while(c!='\"');
    temp[i]=0;
    strcpy(ST->lexema,temp);
   }
 else
 if (!parser_Comment&&strcmp(ST->lexema,"'")==0)
   {//Read all string
    char temp[10000];
    temp[i++]='\'';
    do{
     READ_INSIDE_STRING
    }while(c!='\'');
    temp[i]=0;
    strcpy(ST->lexema,temp);
   }
 else
 if (strcmp(ST->lexema,"..")==0)
   {char c;
    c=GetChar(ST);
    if (c!='.')
     UnGetChar();
    else {ST->lexema[2]='.';ST->lexema[3]=0;}
    return t;
   }
 else
   return t;
 return t;
}

///////////////////////////////////////////////////////////////////////////
////scget_line/////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
char* scget_line(Source_Text *ST,int ignore_whitespaces)
{char temp[10000];
 int i=0;
 char c=0,last;
 if (!STALine[ST->act_char-1]) return "";
 do {
  last=c;
  c=GetChar(ST);
  if (c=='\\') {while(c!=0/*c!=10&&c!=0&&c!=13&&c!=26*/) c=GetChar(ST);
                if (ignore_whitespaces){
                 if(!IsWhitespace(last)) temp[i++]=' ';c=' ';
                }
                else temp[i++]='\n';
//                last=c;
//                c=GetChar(ST);
//                  printf("'#%c=%d'",c,c);

               }
  if (c=='"')
   {temp[i++]=c;c=GetChar(ST);
    while(c&&c!='"')
     {
      temp[i++]=c;c=GetChar(ST);
     }
   }
  if (c=='\'')
   {temp[i++]=c;c=GetChar(ST);
    while(c&&c!='\'')
     {
      temp[i++]=c;c=GetChar(ST);
     }
   }
  if (c=='/')
  {c=GetChar(ST);
   if (c=='/')//comment!
    {
     UnGetChar();
     UnGetChar();
     temp[i]=0;
     return scStrdup(temp);
    }
   temp[i++]='/';
  }
  if (ignore_whitespaces&&IsWhitespace(c))
  {
    if(!IsWhitespace(last)) temp[i++]=' ';
  }else
  if (c!=10&&c!=0&&c!=13&&c!=26) temp[i++]=c;

 } while(c!=10&&c!=0&&c!=13&&c!=26);
 temp[i]=0;
 return scStrdup(temp);
}

///////////////////////////////////////////////////////////////////////////
////scSwap/////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//    void main(){xylo}
//             beg^   ^end
void scSwap(char *temp,char *source,char *toins,int beg,int end)
{int i;
 char* p;
        for(i=0;i<beg;i++) temp[i]=source[i];
        for(p=toins;*p;p++) temp[i++]=*p;
        if (source[end-1])
        for(beg=end;source[beg];beg++)
          temp[i++]=source[beg];
        temp[i]=0;
}

///////////////////////////////////////////////////////////////////////////
////read_integer///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int read_integer(Source_Text *ST,char *s,int *integ)
{ char *endp;
  if (strstr (s, ".")) serr(scErr_Parse,"Integer expected instead of double!\n");
/*
    if (strlen (lexema) == 1) return false;  // only '.' ?
    double d = strtod (lexema, &endp);
    lexval = ftofix (d);
    lextyp = 1; // fixed
  }
  else {*/

  *integ = strtol (s, &endp, 0);   // automatic base selection
  if (endp==s) return 0; else return 1;
//  lextyp = 0; // integer
  return 0;
}
