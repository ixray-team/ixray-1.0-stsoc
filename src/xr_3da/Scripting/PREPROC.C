/************************************************************************
-= SEER - C-Scripting engine v 0.3a =-
State      : under implementation
Author     : Przemyslaw Podsiadly
email: ppodsiad@elka.pw.edu.pl
WWW:   http://home.elka.pw.edu.pl/~ppodsiad
File:preproc.c
Desc:preprocessor
************************************************************************/
#define INTERNAL_FILE
#include <seer.h>
#include "internal.h"
/*
.int..
unassert
assert
ident
warning
import +
export +
include+
endif  +
ifndef +
ifdef  +
if
define +
*/
//#undef debprintf
//#define debprintf deb
extern int parser_Comment;

Source_Text* scLoad_File(char *filename);
Source_Text* scLoad_Text(char *source);

void delete_Source_Text(Source_Text* ST)
{int i;
if (!ST->erase) return;
for (i=0;i<=ST->num_lines;i++) scFree(ST->lines[i]);
scFree(ST->lines);
scFree(ST);
}

Source_Text* new_Source_Text()
{Source_Text* temp=scAlloc(sizeof(Source_Text));
temp->erase=1;temp->act_line=temp->act_char=0;
return temp;
}
void AddBackSlashes(char *n)
{char t[1024];
unsigned int i=0,j=0;
strcpy(t,n);
while(t[i]==' ') i++;
for(;i<strlen(t);i++)
{
	if (t[i]=='"')
    {n[j++]='\\';n[j++]='"';
    }
    else n[j++]=t[i];
}
while(j>0 && n[j-1]==' ') j--;
n[j]=0;
}

char *render_macro(Source_Text* ST,scSymbol* sym)
{
#ifdef rm_rets
#error rm_rets redeclaration
#else
#define rm_rets  ST->act_line=j; ST->num_lines=numl;
#endif
	int j=ST->act_line,numl=ST->num_lines;
	int dont_end=0;
	int achr,mchr=1;
	char *temp1;
	char t1[4096],t2[4096];
	char *st1=t1,*st2=t2;
	char nm1[256],nm2[256];
	char dm1,dm2,prev=' ';
	// debprintf("Rendering\n");
	//actual,formal parameter
	// ST->num_lines=ST->act_line;
	
	strcpy(t1,((char *)sym->reserved)+sym->adr.address+1);
	// deb1("\nMaking macro :%s\n",t1);
	// deb1("%s<-\n",STALine);
	Advance;
	{int li,brik;
	if (!lexeq("(")) serr(scErr_Internal,"Internal preprocessor error!");//shouldn't happen
	//Advance;
	//if (!lexeq("(")) {rm_rets;serr2(scErr_Parse,parse_error,ST->lexema);}
	do{
		li=1;//number of (
		brik=0;
		nm1[0]=' ';nm1[1]=0;
		//prepare actual
		do{dont_end=0;
		Advance;//next readed
		//    deb2("`%s':%d;",ST->lexema,li);
		if (ST->whitespaces)
			strcpy(nm1+strlen(nm1)," ");
		if (lexeq("{")) brik++;
		if (lexeq("}")) brik--;
		if (lexeq("(")) li++;
		if (lexeq(")")) (li--,(li>0?dont_end=1:0));
		
		strcpy(nm1+strlen(nm1),ST->lexema);//actual parameter
		//    deb1("<%s>\n",nm1);
		dm1=ST->lexema[0];
		}
		while ((brik)||(li>1) || (! (lexeq(",")||lexeq(")"))) || dont_end );
		nm1[strlen(nm1)-1]=' ';
		//  nm1[strlen(nm1)]=' ';
		//now have to change all instances of the param in t1
		//Read param name:
		//make as if we operate on source:
		
		//prepare formal
		temp1=STALine;achr=ST->act_char;
		
		ST->act_char=mchr;STALine=(char *)sym->reserved;
		Advance;//read
		//  if (IsSymbol(ST->lexema[0])) serr( parse_error);
		strcpy(nm2,ST->lexema);//actual parameter
		Advance;//read symbol
		if (ST->lexema[1])
		{
			rm_rets;STALine=temp1;
			serr2(scErr_Parse, parse_error,ST->lexema);
		}
		dm2=ST->lexema[0];//formal parameter
		mchr=ST->act_char;
		if (IsSymbol(nm2[0])||/*IsSymbol(nm1[0])||*/!nm1[0]||!nm2[0])
		{rm_rets;STALine=temp1;serr2(scErr_Parse, parse_error,sym->name);}
		
		//  deb2("MCR>%s %s ",nm1,nm2);
		//  deb2("%c %c",dm1,dm2);
		
		//actualize t1
		STALine=st1;ST->act_char=0;
		//  deb1("<%s>\n",st1);
		while(scget_lexema(ST)&&!ST->newline)
		{
			if (lexeq(nm2))
			{//exchange
				char *cxs;
				if (prev=='#')
				{
					scSwap(st2,st1,"\"\"",ST->lexstart-1,ST->act_char);
					cxs=st1;st1=st2;st2=cxs;//exchange t1 and t2
					STALine=st1;
					AddBackSlashes(nm1);
					scSwap(st2,st1,nm1,ST->lexstart,ST->lexstart);
				}
				else
                {
					if (prev=='+')
                        scSwap(st2,st1,nm1+1,ST->lexstart,ST->act_char);
					else
                        scSwap(st2,st1,nm1,ST->lexstart,ST->act_char);
                }
				//       deb1("-%s-",st2);
				cxs=st1;st1=st2;st2=cxs;//exchange t1 and t2
				STALine=st1;
				ST->act_char=ST->lexstart+strlen(nm1)+(prev=='#'?2:0)-(prev=='+'?1:0);
				
				//       deb1("MCR>-%s\n",st1);
			}else
				if (dm1==')'&&lexeq("##"))
				{int a=0;
				char *cxs;
				if (st1[ST->lexstart-1]==' ') a=1;
				scSwap(st2,st1,"",ST->lexstart-a,ST->act_char);
				
				cxs=st1;st1=st2;st2=cxs;//exchange t1 and t2
				STALine=st1;
				ST->act_char=ST->lexstart;
				}
				prev=' ';
				if (!ST->lexema[1]) prev=ST->lexema[0];
				if (lexeq("##")) prev='+';
		}
		
		ST->act_line=j;
		STALine=temp1;ST->act_char=achr;
		//check if equal nr of params
		if (dm1!=')'&&dm2==')') {rm_rets;serr2(scErr_Parse,"Too many parameters for macro `%s'",sym->name);}
		if (dm1==')'&&dm2!=')') {rm_rets;serr2(scErr_Parse,"Too few parameters for macro `%s'",sym->name);}
		
	}while (dm1!=')');
	
	ST->act_line=j;
	ST->num_lines=numl;
	if (li)
	{Advance;
    if (!lexeq(")"))
	{rm_rets;serr2(scErr_Parse,"`)' expected instead of `%s'",ST->lexema);}
	}
 }
 // if (st1[strlen(st1)-2]=='\n')st1[strlen(st1)-2]=0;
 // deb1("MACRO=`%s'",st1);
 return scStrdup(st1);
#undef rm_rets
}




//until: 0-end of file 1-if
//
#define MAX_DEEP 100
Source_Text* sc_prepro(Source_Text* ST,int until,int skip,char *temp,scDictionary* macros,int init)
{
	static int included[MAX_DEEP],in_include=-1;
	static char incfile[256];
	char *tmp;
	scSymbol* sym;
	
	if (init)
	{
		in_include=-1;
		incfile[0]=0;
	}
	if (in_include==-1)
	{
		memset(&included,0,MAX_DEEP*sizeof(int));
		in_include=0;
		deb0("Cleaning include data\n");
	}
	// debprintf("sc_prepro(ST,%d,%d,...);\n",until,skip);
	while (scget_lexema(ST))
	{
		//  debprintf("%s_",ST->lexema);
		if (!ST->lexema[1]&&ST->lexema[0]=='#')
		{if (!ST->newline) serr2(scErr_Parse,parse_error,"# directive");
		Advance;
		{STALine[ST->lexstart-1]=0;//erase line with macro
		if (lexeq("!"))
		{//dont erase the line - this macro is for line numbering
			if (!skip)
			{int t;
			STALine[ST->lexstart-1]='#';
			Advance;
			//ST->lexema[0]>='0'&&ST->lexema[0]<='9'
			if (ST->whitespaces&&(ST->lexema[1]||!(IsSymbol(ST->lexema[0]))))
			{
				//        difline=ST->act_line-
				t=atoi(ST->lexema);
				Advance;
				strcpy(incfile,ST->lexema);
				if (strcmp(incfile,"\"!!_internal_Script\"")==0) in_include--;
				//        if (incfile[0]=='\'') in_include--;
				else in_include++;
				included[in_include]=ST->act_line-t+1;
				//        deb3("<I %s %d,%d>\n",incfile,in_include,included[in_include]);
			}
			}
		}
		else
			if (lexeq("title"))
			{char *s;
			Advance;
			s=ST->lexema;
			if (s[0]=='"')//erase "
			{s++;s[strlen(s)-1]=0;
			}
			//        scScript_Title=xr_strdup(s);
			strcpy(scScript_Title,s);
			strcpy(ST->name,s);//????
			}
			else
				if (lexeq("author"))
				{char *s;
				Advance;
				s=ST->lexema;
				if (s[0]=='"')//erase "
				{s++;s[strlen(s)-1]=0;
				}
				strcpy(scScript_Author,s);
				//        scScript_Author=xr_strdup(s);
				}
				else
					if (lexeq("define"))
					{
						Advance;
						if (!skip)
						{
							int i,flags=0,addr=0;
							if (IsSymbol(ST->lexema[0])) {serr2(scErr_Parse,parse_error," macro definition");}
							//         debprintf("\ndefine ");
							if (scdicFoundSymbol(macros->first,ST->lexema))
							{serr2(scErr_Parse, "Macro `%s' redefined",ST->lexema);}
							
							if (STALine[ST->act_char]=='(')
							{//macro with parameters
								char *p=STALine+ST->act_char;
								if (*p=='(')
								{
									while (*p!=')')p++;
									p++;
								}
								else
									while (*p!=9&&*p!=' '&&*p!='\\')p++;
									flags=1;addr=p-STALine-ST->act_char;//addr- adres separatora parametrow od definicji
							}
							if (!scomit_whitespaces(ST));
							
							if (!ST->newline)
							{
								i=ST->act_line+1;
								tmp=scget_line(ST,1);
								//remove it
								for(;i<=ST->act_line;i++) ST->lines[i][0]=0;
							}
							else tmp=" ";
							if (addr>0)tmp[addr]=':';
							scdicAddSymbol(macros,ST->lexema,addr,flags,tmp);//addr,uflags
							//         deb2("Macro `%s' with `%s'\n",ST->lexema,tmp);
							
						}
					}
					else
						if (lexeq("ifdef"))
						{Advance;
						if (!skip)
						{//debprintf("ifdef %s\n",ST->lexema);
							if (scdicFoundSymbol(macros->first,ST->lexema))
								ST=sc_prepro(ST,1,0,temp,macros,0);//don't skip
							else
								ST=sc_prepro(ST,1,1,temp,macros,0);//skip
						}
						else
						{
							ST=sc_prepro(ST,1,1,temp,macros,0);//skip
						}
						
						if (!lexeq("endif"))
							serr(scErr_Parse, "#endif missing after #if");
						}
						else
							if (lexeq("ifndef"))
							{Advance;
							if (!skip)
							{//debprintf("ifndef %s\n",ST->lexema);
								if (!scdicFoundSymbol(macros->first,ST->lexema))
								{//debprintf("don't skip\n");
									ST=sc_prepro(ST,1,0,temp,macros,0);//don't skip
								}
								else
									ST=sc_prepro(ST,1,1,temp,macros,0);//skip
								
							}
							else
							{
								ST=sc_prepro(ST,1,1,temp,macros,0);//skip
							}
							
							if (!lexeq("endif"))
								serr(scErr_Parse, "#endif missing after #if");
							}
							else
								if (lexeq("if"))
								{int _not=0;
								Advance;
								if (lexeq("!")) {_not=1;Advance;}
								if (lexeq("defined")){
									if(!skip)
									{
										Advance;
										if (scdicFoundSymbol(macros->first,ST->lexema))
											ST=sc_prepro(ST,1,_not,temp,macros,0);
										else
											ST=sc_prepro(ST,1,!_not,temp,macros,0);
									}
									else
									{
										ST=sc_prepro(ST,1,1,temp,macros,0);//skip
									}
									
									if (!lexeq("endif"))
										serr(scErr_Parse, "#endif missing after #if");
								}//defined
								else serr2(scErr_Parse, "Unknown #%s directive",ST->lexema);
								}
								else
									if (lexeq("endif"))
									{//debprintf("endif");
										if (until!=1) serr(scErr_Parse, "#endif without #if");
										return ST;
									}
									else
										if (lexeq("else"))
										{if (until!=1) serr(scErr_Parse, "#else without #if");
										skip=!skip;
										}
										else
											if (lexeq("elif"))
											{if (until!=1) serr(scErr_Parse, "#elif without #if");
											Advance;
											if (!skip){
												if (scdicFoundSymbol(macros->first,ST->lexema))
													ST=sc_prepro(ST,1,0,temp,macros,0);//show
												else
													ST=sc_prepro(ST,1,1,temp,macros,0);//dont show
												
											} else
												ST=sc_prepro(ST,1,1,temp,macros,0);//dont show
											
											return ST;
											}//elif
											else
												if (lexeq("include")&&!skip)
												{char sname[255];
												int last=ST->act_line;
												Source_Text* ST1=NULL;
												Advance;
												//load
												if (ST->lexema[0]=='"')
												{//FILE
													strcpy(sname,ST->lexema+1);
													sname[strlen(sname)-1]=0;
													//          printf(" include from file %s!\n",sname);
													ST1=scLoad_File(sname);
													
													if (!ST1) serr2(scErr_Parse, "Unable to include file `%s'",sname);
												}
												else
												{//Chars
													scSymbol* sym=NULL;
													char *x=NULL;
													strcpy(sname,ST->lexema);
													if (scInternalHeaders)
														sym=scdicFoundSymbol(scInternalHeaders->first,ST->lexema);
													if (sym) x=(char *)sym->adr.address;
													if (x) ST1=scLoad_Text(x);
													else
														serr2(scErr_Parse, "Unable to include internal `%s'",ST->lexema);
													
												}
												//         debprintf("include %s...\n",sname);
												//render include
												{int i,prev_lines=ST->num_lines;
												ST->num_lines+=ST1->num_lines+1;
												if (ST->num_lines+2>=ST->alloced_lines)
												{ST->alloced_lines=ST->num_lines+10;
												ST->lines=scRealloc(ST->lines,ST->alloced_lines*sizeof(char *));
												}               //=
												for (i=prev_lines;i>ST->act_line;i--)
												{
													ST->lines[i+ST1->num_lines+1]=ST->lines[i];
												}
												for (i=ST->act_line;i<=ST->act_line+ST1->num_lines;i++)
													ST->lines[i+1]=ST1->lines[i-ST->act_line];
												{char st[256];
												sprintf(st,"#! %d \"%s\"",0,sname);
												ST->lines[ST->act_line]=scStrdup(st);
												//           if (in_include)
												//           sprintf(st,"#! %d %s",ST->act_line+1,incfile);
												//           else
												//           deb3("Inc[%d]=%d-%d",in_include,ST->act_line+1,included[in_include]);
												if (in_include)
													sprintf(st,"#! %d %s",ST->act_line+1-included[in_include],incfile);
												else
													sprintf(st,"#! %d \"%s\"",ST->act_line+1-included[in_include],"!!_internal_Script");
												ST->lines[ST->act_line+ST1->num_lines+1]=scStrdup(st);
												{int l;
												for(l=0;l<in_include;l++)included[l]+=ST1->num_lines+1;
												}
												}
												scFree(ST1->lines);
												scFree(ST1);
												ST->act_line=last;ST->act_char=0;
												ST->newline=-1;
												/*                 {int l;//It's stops to see if including was OK.
												for(l=0;l<ST->num_lines;l++) deb2("%4d: %s\n",l,ST->lines[l]);
												deb1("ST->act_line=%d\n",ST->act_line);
												scClose_Debug();exit(52);
												}*/
												
												}
												}
												else
													if (lexeq("undef")&&!skip)
													{Advance;
													{//debprintf("Undefine %s!\n",ST->lexema);
														if (!scdicFoundSymbol(macros->first,ST->lexema)) serr(scErr_Parse, "Undefining not defined macro");
														scdicRemoveSymbol(macros,ST->lexema);
														
													}
													}
													else
													{
														if (!skip) serr(scErr_Parse, "Undefined or invalid # directive");
													}
   }
  }
  else
	  if (skip)
	  {ST->act_char=0;STALine[0]=0;//remove line
	  }
	  else
		  if (lexeq("\""))
		  { while(scget_lexema(ST)&&(!lexeq("\"")))
		  {}
		  }
		  else
			  if (lexeq("'"))
			  { while(scget_lexema(ST)&&(!lexeq("'")))
			  {}
			  }
			  else
				  if (lexeq("//"))
				  { STALine[ST->lexstart]=0;//erase the rest of line
				  ST->act_line++;ST->act_char=0;ST->newline=-1;
				  //      debprintf("Komentarz!\n");
				  }
				  else
					  if (lexeq("/*"))
					  {//read til */
						  int cl;
						  int ll=ST->act_line;
						  cl=ST->lexstart;
						  parser_Comment=1;
						  while(scget_lexema(ST)&&(!lexeq("*/")))
						  {}
						  parser_Comment=0;
						  if (ll==ST->act_line)//the same line - insert whitespace!
						  {scSwap(temp,STALine," ",cl,ST->act_char);
						  scFree(STALine);
						  STALine=scAlloc(strlen(temp)+1);
						  strcpy(STALine,temp);
						  ST->act_char=cl;
						  }
						  else
						  {int i;
						  ST->lines[ll][cl]=0;//first line
						  for (i=ll+1;i<ST->act_line;i++) ST->lines[i][0]=0;//all between
						  scSwap(temp,STALine,"",0,ST->act_char);
						  scFree(STALine);
						  STALine=scAlloc(strlen(temp)+1);
						  strcpy(STALine,temp);
						  ST->act_char=0;
						  }
					  }
					  else
						  if (!IsSymbol(ST->lexema[0])&&(sym=scdicFoundSymbol(macros->first,ST->lexema)))
						  {
							  int j=ST->lexstart;//ST->act_char-strlen(ST->lexema);
							  
							  if (sym->type.flags==1)
							  {char *tempo;
							  if (STALine[ST->act_char]!='(')
								  serr2(scErr_Parse, "Macro `%s' requires parameters",ST->lexema);
							  tempo=render_macro(ST,sym);
							  
							  scSwap(temp,STALine,tempo,j,ST->act_char);
							  scFree(tempo);
							  }
							  else
							  {
								  scSwap(temp,STALine,(char *)sym->reserved,ST->lexstart,ST->act_char);
							  }
							  scFree(STALine);
							  STALine=scStrdup(temp);
							  ST->act_char=j;//do this macro again!
						  }
 }
 
 deb0("End of preprocessing\n");
 return ST;
}

Source_Text* scPreprocessor(Source_Text* ST)
{char temp[4096];
scDictionary *macros=scdicNewDictionary();
/* Standard macros in SeeR scripting */
scdicAddSymbol(macros,"SeeR",0,0,STR_SeeR_Version);
scdicAddSymbol(macros,"WAIT",0,0,"__vcpu(56)");
scdicAddSymbol(macros,"c_import",0,0,"import(1)");

scdicAddSymbol(macros,"NULL",0,0,"(void*)0");
scdicAddSymbol(macros,"MAXINT",0,0,"(0x7fffffff)");

//BOOLean type
scdicAddSymbol(macros,"BOOL",0,0,"int");
scdicAddSymbol(macros,"true",0,0,"1");
scdicAddSymbol(macros,"false",0,0,"0");

//do the job for all the file without skipping
ST->newline=-1;
ST->linedif=0;
strcpy(ST->name,"script");
sc_prepro(ST,0,0,temp,macros,1/*init*/);

// getchar();
scdicDeleteDictionary(macros);
return ST;
}

//allegro.h:
#define PACKFILE FILE
#define pack_fopen fopen
#define pack_fclose fclose
#define pack_feof feof
#define pack_fgets fgets

static void scCutNL(char* s)
{
	int i=strlen(s)-1;
	if (!s||i<0) return;
	while (i>=0&&(s[i]==10||s[i]==13)) i--;
	s[i+1]=0;
}

Source_Text*  scLoad_Text(char *source)
{
	Source_Text* ST;
	int pos=0;
	char s[2048];
	s[0]=0;
	
	//Load ST:
	ST=new_Source_Text();
	ST->alloced_lines=1000;
	ST->lines=scAlloc(ST->alloced_lines*sizeof(char *));
	ST->num_lines=-1;
	if (!source) return NULL;
	while (source[pos])
	{s[0]=0;
	{int i;
	for(i=0;source[pos]&&source[pos]!=10&&source[pos]!=13;i++,pos++)
		s[i]=source[pos];
	if (source[pos+1]==10&&source[pos+1]!=source[pos]) pos++;
	s[i]=0;pos++;
	}
	scCutNL(s);
	ST->num_lines++;
	if (ST->num_lines+2>=ST->alloced_lines)
	{ST->alloced_lines+=20;
	ST->lines=scRealloc(ST->lines,ST->alloced_lines*sizeof(char *));
	}
	ST->lines[ST->num_lines]=scStrdup(s);
	}
	ST->lines[++ST->num_lines]=scStrdup(" /**/ ");
	return ST;
}

Source_Text*  scLoad_File(char *filename)
{
	Source_Text* ST;
char s[2048];
PACKFILE *file;

//Load ST:
ST=new_Source_Text();
ST->alloced_lines=20;
ST->lines=scAlloc(ST->alloced_lines*sizeof(char *));
ST->num_lines=-1;
file=pack_fopen(filename,"r");
if (!file) return NULL;
while (!pack_feof(file))
{s[0]=0;
pack_fgets(s,2047,file);
if (pack_feof(file))
{int i=0;
while (s[i]&&s[i]!=26&&(s[i]!=-1)) i++;//debprintf("%c(%d)",s[i],(unsigned int)s[i++]);
s[i]=0;
}
scCutNL(s);
ST->num_lines++;
if (ST->num_lines+2>=ST->alloced_lines)
{ST->alloced_lines+=20;
ST->lines=scRealloc(ST->lines,ST->alloced_lines*sizeof(char *));
}
ST->lines[ST->num_lines]=scStrdup(s);
}
ST->lines[++ST->num_lines]=scStrdup(" /**/ ");
return ST;
}

scScript  SAPI scCompile_File(char *filename)
{
	Source_Text* ST;
	scScript Scr=NULL;
	scInit_SeeR();
	scErrorNo=0;
	scInitAlloc();
	if (!setjmp(act.jump))
	{
		ST=scLoad_File(filename);
		
		if (!ST) return NULL;
		Scr=scCompile(ST);
		
	}
	// delete_Source_Text(ST);
	scCleanUp();
	return Scr;
}

scScript  SAPI scCompile_Text(char *source)
{
	Source_Text* ST;
	scScript Scr=NULL;
	scInit_SeeR();
	scErrorNo=0;
	scInitAlloc();
	if (!setjmp(act.jump))
	{
		ST=scLoad_Text(source);
		
		if (!ST) return NULL;
		Scr=scCompile(ST);
		
	}
	scCleanUp();
	return Scr;
}
