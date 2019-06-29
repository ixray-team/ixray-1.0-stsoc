/************************************************************************
       -= SEER - C-Scripting engine v 0.62a =-
State      : under implementation
Author     : Przemyslaw Podsiadly
             email: ppodsiad@elka.pw.edu.pl
             WWW:   http://home.elka.pw.edu.pl/~ppodsiad
File:test.c
Desc:testing program
************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "seer.h"
#include "internal.h"

#define SeeRC_STR "SeeR compiler version "SeeR_VERSION_STR" by Theur(*Przemyslaw Podsiadly*) "SeeR_DATE_STR"\n"

int random_(int x)
{
 return (rand()%x);
}

int compiler_debug=0;
//char *compiler_debfname;
char *help=
"Options:\n\
  -a, -author {author-name}\n\
  -d, -debug\n\
  -h, -help\n\
  -i, -info\n\
  -o {out-file-name}\n\
  -r, -run {function-name(parameters)}\n\
  -s, -show\n\
  -t, -title {title-name}\n\
  -v, -version\n\
\n\
  SeeRC defines internal header named stdio,that contains some standard C\n\
 functions (see readme.txt)\n\
";

//script imports these:
#define MAXINPUTFILENAMES 256

char *FNOutput=NULL,*DEBOutput=NULL,*FNInput[MAXINPUTFILENAMES];
int InputFiles=0;
int FLShow=0;
enum {FLCompile,FLRun,FLInfo};
int FLOp=FLCompile;

#define SCError(i,c) {if (!FLShow)ShowName();printf("SeeRC:");printf(c);exit(i);}
void ShowName()
{
 printf(SeeRC_STR);
}

void Compilation();
void Run();
void Info();
/*
int main(int argc,char *argv[])
{
 {
  int i;
        scAdd_Internal_Header("stdio",
        "import int printf(char *,...);\n"
        "import int sprintf(char*,char *,...);\n"
        "import int scanf(char *,...);\n"
        "import int getchar();\n"
        "import int scrandom(int);\n"
        "import int rand();\n"
        "import int srand(int seed);\n"
        "import int strcpy(char *,char *);\n"
        "import int strtol(char *,char **,int);\n"
        "import int atoi(char *);\n"
        "import double atof(char *);\n"
        "import unsigned int strtoul(char *,char **,int);\n"
        "import double strtod(char *,char **);\n"
        "import char *strdup(char *);\n"
        "import double sqrt(double);\n"
        "import double sin(double);\n"
        "import double cos(double);\n"
        "import void* malloc(int);\n"
        "import void free(void *);\n"
        );
        scAdd_External_Symbol("scrandom",random_);
        scAddExtSym(printf);
        scAddExtSym(sprintf);
        scAddExtSym(rand);
        scAddExtSym(srand);
        scAddExtSym(scanf);
        scAddExtSym(getchar);
        scAddExtSym(strcpy);
        scAddExtSym(strdup);
        scAddExtSym(strtoul);
        scAddExtSym(strtol);
        scAddExtSym(strtod);
        scAddExtSym(atoi);
        scAddExtSym(atof);
        scAddExtSym(sqrt);
        scAddExtSym(sin);
        scAddExtSym(cos);
        scAddExtSym(malloc);
        scAddExtSym(free);

  for (i=1;i<argc;i++)
  {if (argv[i][0]!='-')//FileName;
   {
    if (InputFiles+1>MAXINPUTFILENAMES) SCError(-1,"Too many input files!\n");
//    printf("%d:%s\n",InputFiles,argv[i]);
    if (strcmp("/?",argv[i])==0)
      SCError(3,"Invalid file specification\n");
    FNInput[InputFiles++]=argv[i];
   }
   else
   {//options
    if (strcmp(argv[i],"-s")==0||strcmp(argv[i],"-show")==0 )
      {
       FLShow=1;
      }
      else
    if (strcmp(argv[i],"-d")==0||strcmp(argv[i],"-debug")==0 )
      {
       compiler_debug=1;
      }
      else
    if (strcmp(argv[i],"-r")==0||strcmp(argv[i],"-run")==0 )
      {
       FLOp=FLRun;
       FNOutput=strdup(argv[++i]);
      }
      else
    if (strcmp(argv[i],"-i")==0||strcmp(argv[i],"-info")==0 )
      {
       FLOp=FLInfo;
      }
      else
    if (strcmp(argv[i],"-v")==0||strcmp(argv[i],"-version")==0)
      {
       ShowName();return 0;
      }
      else
    if (strcmp(argv[i],"-h")==0||strcmp(argv[i],"-help")==0 )
      {ShowName();
       printf("%s",help);
       return 0;
      }
      else
    if (strcmp(argv[i],"-t")==0||strcmp(argv[i],"-title")==0)
      {
//       scScript_Title=strdup(argv[++i]);
        strcpy(scScript_Title,argv[++i]);
      }
      else
    if (strcmp(argv[i],"-a")==0||strcmp(argv[i],"-author")==0)
      {
        strcpy(scScript_Author,argv[++i]);
//       scScript_Author=strdup(argv[++i]);
      }
      else
    if (strcmp(argv[i],"-o")==0)
      {
       FNOutput=strdup(argv[++i]);
      }
      else
      SCError(3,"Unrecognized option `%s'.\n");
   }
  }
 }

 if (!InputFiles) SCError(2,"No input files (-h for help)\n");
 switch (FLOp){
 case FLRun:
   Run();
   break;
 case FLInfo:
   Info();
   break;
 case FLCompile:
   Compilation();
   break;
 default:
   SCError(-1,"Internal error:unknown job\n");
 }

 return 0;
}

/*************Worx***************************************/
#define ReadINT(ptr,x) memcpy(&(x),ptr,4)

void Info()
{int i,data1;
// FILE *file;
 scScript script;
 for(i=0;i<InputFiles;i++)
 {
//read:
 script=scLoad_Script(FNInput[i]);
/* size=STfilesize(FNInput[i]);script=malloc(size);
 file=fopen(FNInput[i],"rb");*/
 if (!script)
    printf("Unable to open or not a SeeR input file:%s.\n",FNInput[i]);
 else
 {
//info:
 printf("File:%s\n",FNInput[i]);
 if (*((int*)script)!=CHARS2INT('S','e','e','R'))
    printf("Not a SeeR script!\n");
 else
 if (*((int*)script+1)!=CHARS2INT('V','C','P','U'))
    printf("SeeR script not VCPU!\n");
   else
   {char *au,*ti;
    data1=*((int*)script+2);
    ti=script+0x3C;
    au=ti+strlen(ti)+1;
    printf("SeeR %d.%d script, \"%s\" by %s\n",data1>>16,data1&0xFFFF,ti,au);
    data1=*((int*)script+4);//symbols
    printf("Data size:%d\n",*((int*)script+10));
    printf(" Symbols:\n");
    {char namer[1000];
     int addr,cfg;
     char *x=(char *)script+data1;
     do{
     strcpy(namer,x);
     if (namer[0])
     {
      x+=strlen(namer)+1;
      ReadINT(x,addr);x+=4;
      ReadINT(x,cfg);x+=4;
      printf("  %-16s - ",namer);
      if (cfg>=0) printf(" function ");
          else printf(" variable ");
      printf(" at %04x \n",addr);
     }
     } while(namer[0]);
    }

    data1=*((int*)script+13);//imports
    printf(" Imports:\n");
    {char namer[1000];
     int addr,cfg;
     char *x=(char *)script+data1;
     do{
     strcpy(namer,x);addr=x-((char *)script+data1);
     if (namer[0])
     {
      x+=strlen(namer)+1;
      ReadINT(x,cfg);x+=4;
      ReadINT(x,cfg);x+=4;
      printf("  %-16s - ",namer);
      if (cfg>=0) printf(" function ");
          else printf(" variable ");
      printf(" at %04x \n",addr);
     }
     } while(namer[0]);
    }
    printf("\n");
   }//else
  }
 }
}

void Run()
{int i;
 scInstance *in;
 FILE *file;
 scScript script;
 if (InputFiles>1) SCError(5,"Unable to run script for many inputs\n");

//read
 i=STfilesize(FNInput[0]);script=malloc(i);
 file=fopen(FNInput[0],"rb");
 if (!file)
    SCError(6,"Unable to open input file.\n");
 fread(script,i,1,file);
 fclose(file);

//externals:
 scAdd_External_Symbol("printf",printf);
 scAdd_External_Symbol("scanf",scanf);

 in=scCreate_Instance(script,"");
 scCall_Instance(in,scGet_Symbol(in,"main"));

 scFree_Instance(in);
 free(script);
}

void Compilation()
{FILE *file;
 scScript scrpt;
 int i;
 if (InputFiles>1&&FNOutput) SCError(4,"Single output filename for many inputs\n");

  for(i=0;i<InputFiles;i++)
  {strcpy(scScript_Title,FNInput[i]);
   if (!FNOutput)
    {int j;
     FNOutput=malloc(strlen(FNInput[i])+10);
     for(j=strlen(FNInput[i])-1;j>0&&FNInput[i][j]!='.';j--);
     strcpy(FNOutput,FNInput[i]);
     if (!j) {j=strlen(FNOutput);FNOutput[j]='.';}
     FNOutput[j+1]='c';
     FNOutput[j+2]='s';
     FNOutput[j+3]=0;
    }

   if (compiler_debug)
    {int j;
     DEBOutput=malloc(strlen(FNInput[i])+10);
     for(j=strlen(FNInput[i])-1;j>0&&FNInput[i][j]!='.';j--);
     strcpy(DEBOutput,FNInput[i]);
     if (!j) {j=strlen(DEBOutput);DEBOutput[j]='.';}
     DEBOutput[j+1]='d';
     DEBOutput[j+2]='e';
     DEBOutput[j+3]='b';
     DEBOutput[j+4]=0;
    }

   if (FLShow) {
     ShowName();
     if (compiler_debug)
                printf("Compiling %s to %s (debug:%s)...",FNInput[i],FNOutput,DEBOutput);
       else
                printf("Compiling %s to %s...",FNInput[i],FNOutput);
                fflush(stdin);
               }
   if (compiler_debug) //compiler_debfname=DEBOutput;
   {
    scOpen_Debug(DEBOutput);
    scToggle_Debug(true);
   }

   scrpt=scCompile_File(FNInput[i]);
   if (scErrorNo)  SCError(scErrorNo,"\nIn file\n");
   if (!scrpt)
      SCError(5,"\nFile not found.\n");
   if (compiler_debug)
   {
    scClose_Debug();
   }
   file=fopen(FNOutput,"wb");
   fwrite(scrpt,scGet_Script_Size(scrpt),1,file);
   fclose(file);
   free(scrpt);
   if (FLShow) printf("\x8\x8\x8.  \rûCompiled \n");
   if (InputFiles>1) {
       free(FNOutput);FNOutput=NULL;
       if (compiler_debug) free(DEBOutput);
      }
  }
 }

