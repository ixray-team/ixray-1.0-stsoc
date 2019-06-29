/************************************************************************
-= SEER - C-Scripting engine v 0.3a =-
State      : under implementation
Author     : Przemyslaw Podsiadly
email: ppodsiad@elka.pw.edu.pl
WWW:   http://home.elka.pw.edu.pl/~ppodsiad
File:vcpudeco.c
Desc:decoding and running script
************************************************************************/
#define INTERNAL_FILE
#include <stdio.h>
#include <seer.h>
#include "internal.h"
#include "code.h"
#include "fixed.h"

#define ToINT(ch) ((int*)(ch))
#define ReadINT(ptr,x) memcpy(&x,ptr,4)
#define WriteINT(ptr,x) memcpy(ptr,&x,4)
#define ToCodeINT ((int*)Ins->code)
#define Register ((int*)Ins->data_stack)
#define CodeSeg ((char *)(Ins->code+Register[regCS]+Register[regIP]))
#define GetAtIP(typ) ((typ *)(Ins->code+Register[regCS]+(Register[regIP]+=sizeof(typ))-sizeof(typ)))

#define DoOper(typ1,typ2,op) *((typ1*)v1.what)##op*((typ2*)v2.what);
#define DoOper2(typ1,typ2,op) *((typ1*)v1.what)=(*((typ1*)v1.what))##op(*((typ2*)v2.what));
#define DoOper3(typ1,typ2,op) *((int*)v1.what)=(*((typ1*)v1.what))##op(*((typ2*)v2.what));

#define DOUBLEFix                                    \
	if(v2.what==&imINT2)                          \
        {int *x=(int*)(&imDOUBLE2);                  \
		imINT3=*GetAtIP(int);                       \
		*x=imINT2;                                  \
		*(x+1)=imINT3;                              \
		deb1("(2nd=$%2.2f)",imDOUBLE2);             \
		v2.what=(int*)&imDOUBLE2;                   \
        }
#define DOUBLEFix1                                   \
	if(v1.what==&imINT1)                          \
        {int *x=(int*)(&imDOUBLE1);                  \
		imINT3=*GetAtIP(int);                       \
		*x=imINT1;                                  \
		*(x+1)=imINT3;                              \
		deb1("(BOTH$%2.2f)",imDOUBLE1);             \
		v1.what=(int*)&imDOUBLE1;                   \
        }

#define DOUBLEdeb0(c)                                 \
        {                                             \
		deb2("<%c%f",c,                              \
		*((double*)v2.what));           \
        }

#define DOUBLEdeb1(c)                                 \
        {                                             \
		deb3("%f%c%f",*((double*)v1.what),c,         \
		*((double*)v2.what));           \
        }

#define DOUBLEdeb2                                    \
        {                                             \
		deb1("=>%f\n",*((double*)v1.what));          \
        }

#define DOUBLEint2                                    \
        {                                             \
		deb1("=>%d\n",*((int*)v1.what));             \
        }

#define PushInt(co)\
        {\
		Register[regSP]-=4;\
		*((int *)(Register[regSS]+Register[regSP]))=co;\
        }

#define PopInt(co)\
        {\
		co=*((int *)(Register[regSS]+Register[regSP]));\
		Register[regSP]+=4;\
        }

typedef struct eval{
	int* what;
	//        char type;//=0 - immidiate
	//        char ptr;
}eval;

//scInstance *Ins;
//if speed<0 - til end, else speed tacts
int SAPI Executor(scInstance *Ins,int speed)
{
	int opcode,i;
	unsigned char *opc=(unsigned char *)&opcode;
	int imINT1,imINT2,imINT3;
	double imDOUBLE1,imDOUBLE2;
	int c,actspeed=speed;
	BOOL immi1=false,immi2=false;
	eval v1,v2;
	scActual_Instance=Ins;//Set Global Variable...
	
	if (Ins->flags.forkno>0)
	{
		deb2("\n\nExecuting function from %s (fork:%d)!\n",scGet_Instance_Title(Ins),Ins->flags.forkno);
	} else {
		deb1("\n\nExecuting function from %s!\n",scGet_Instance_Title(Ins));
	}
	
	do{
		deb1(" %d:",Register[regIP]);
		opcode=*GetAtIP(int);
		
		while (opcode==opvcpuEXPR)//EXTENDED1 - EXPR opcode is normally ignored
		{
			//only here the execution can break
			if (actspeed>0)	
			{
				actspeed--;
			}
			if ((!actspeed)&&!Register[regIE])//every number of instructions wait
			{
				scActual_Instance=NULL;
				actspeed=speed;
				return 0;
			}
			deb1("EXPR ---- \n %d:",Register[regIP]);
			//get next opcode
			opcode=*GetAtIP(int);
		}
		
		immi1=immi2=false;
		if ((opc[0]&63)>=opvcpuRET)//4 instructions with no params:
		{
			for(i=0;i<4;i++)
			{
				c=opc[i]&0xff;
				switch(c)
				{
				case opvcpuRET:
					deb0("RET ");
					deb2("SP=%d,CP=%d\n",Register[regSP],Register[regCP]);
					if (Register[regSP]==Register[regCP])
					{
						scActual_Instance=NULL;
						deb0("EXIT\n");
						return 1;//end of instance
					}
					PopInt(Register[regIP]);
					deb1(" a0=%d ",Register[0]);
					i=3;//exit
					break;
					
				case opvcpuCLI:
					deb0("CLI ");
					Register[regIE]++;
					break;// \Turn OFF\ Multitasking
					
				case opvcpuSTI:
					if (Register[regIE]>0)Register[regIE]--;//if 0, interrupts enabled
					else rerr( "multitasking violation");
					deb0("STI ");
					break;// /     ON /
					
				case opvcpuWAIT:
					deb0("WAIT ");
					if (speed>0)
					{
						scActual_Instance=NULL;
						return 0;
					}
					break;// -
				case opvcpuENTER:
					deb0("ENTER ");
					PushInt(Register[regBP]-Register[regSS]);
					Register[regBP]=Register[regSP]+Register[regSS];
					deb1("%d ",Register[regBP]);
					break;//push bp;mov bp,sp
					
				case opvcpuLEAVE:
					Register[regSP]=Register[regBP]-Register[regSS];
					PopInt(Register[regBP]);
					Register[regBP]+=Register[regSS];
					deb0("LEAVE ");
					break;//mov sp,bp; pop bp
					
				case opvcpuNOP:
					deb0("NOP ");
					break;
					
				default:
					rerr3( "Unknown opcode %d in the script at %d!\n",opc[i]&0xff,Register[regIP]-4);
				}
				deb0("\n");
			}//for
		}
		else
			if((opc[0]&63)<opvcpuMOV)//1 arg
			{
				immi1=false;
				//EXTENDED opcode
				if (!opc)
				{	
					//other extended opcode?
					continue;
				}
				if (opc[1]&1 ) //pointer
				{
					if (opc[1]&2)	//register
					{
						v1.what=(int *)Register[opc[2]];
						deb1("[a%d]",opc[2]);
					}
					else			//immidiate
					{
						v1.what=(int *)(*GetAtIP(int));
						deb1("[%d]",(int)v1.what);
						immi1=true;
					}
					//make register shift
					switch ((opc[1]>>2)&adrSTOREPOINTER)//base register
					{
					case adrSTORECPU:
						if (immi1)//import
						{
							ReadINT((char *)Ins->code+ToCodeINT[hdrImportedOffset]+(int)v1.what,imINT1);
							v1.what=&imINT1;
						}
						break;
					case adrES:	(char *)(v1.what)+=Register[regES];deb0("ES"); break;
					case adrDS: (char *)(v1.what)+=Register[regDS];deb0("DS"); break;
					case adrBP:	(char *)(v1.what)+=Register[regBP];deb0("BP"); break;
					}
					// debprintf("(value=%d)\n",*((int*)v1.what));
				} else { //imidiate
					if (opc[1]&2) //register
					{
						v1.what=Register+opc[2];
						deb1("a%d",opc[2]);
					} else {
						imINT1=*GetAtIP(int);deb1("$%04x",imINT1);
						v1.what=&imINT1;
					}
				}
				deb1("(=$%d) ",*((int *)v1.what));
				switch(opc[0]&63)
				{
				case opvcpuPUSH:deb0("PUSH ");
					PushInt(*((int *)v1.what));
					deb3("(SP=%d,@%d,BP=%d) ",Register[regSP],Register[regSS]+Register[regSP],Register[regBP]);
					break;
					
				case opvcpuPUSHADR:deb0("PUSHADR ");
					PushInt((int)v1.what);
					deb3("(SP=%d,@%d,BP=%d) ",Register[regSP],Register[regSS]+Register[regSP],Register[regBP]);
					break;
					
				case opvcpuDPUSH:deb0("DPUSH ");
					DOUBLEFix1;
					PushInt(*(((int *)v1.what)+1));
					PushInt(*(((int *)v1.what)));
					deb2("of %08x%08x",*(((int *)v1.what)),*(((int *)v1.what)+1));
					deb2(" or %f",*(((int *)v1.what)),*(((int *)v1.what)+1));
					deb1("(SP=%d) ",Register[regSP]);
					break;
					
				case opvcpuDNEG:deb0("DNEG ");
					DOUBLEFix1;
					*((double *)v1.what)=-*((double *)v1.what);
					break;
					
				case opvcpuPOP:deb0("POP ");
					PopInt(*((int *)v1.what));
					deb1("(SP=%d) ",Register[regSP]);
					break;
					
				case opvcpuCALL:deb0("CALL ");
					PushInt(Register[regIP]);
					if (*((int *)v1.what)==-1)
					{
						rerr("not defined function called");
					}
					Register[regIP]=*((int *)v1.what);//call
					break;
					
				case opvcpuJMP:deb0("JMP ");
					Register[regIP]+=*v1.what;
					break;
					
				case opvcpuNEG:deb0("NEG ");
					*((int *)v1.what)=-*((int *)v1.what);
					break;
					
				case opvcpuNOT:deb0("NOT ");
					*((int *)v1.what)=!*((int *)v1.what);
					deb1("(%d)",*((int *)v1.what));
					break;
					
				case opvcpuOPTIONS:deb0("OPTIONS ");
					Register[regOPTIONS]=*((int *)v1.what);
					break;
					
				default:
					rerr3( "Unknown opcode:%d in the script at %d!\n",opc[0]&63,Register[regIP]-4);
				}
				deb0("\n");
			}
			else //2 args
			{
				//read params to v1 and v2
				//1st
				deb0("{");
				
				if (opc[0]&64 )//pointer
				{
					if (opc[1]&1)//register
					{
						v1.what=(int *)Register[opc[2]];
						deb1("[a%d]",opc[2]);
					} 
					else//immidiate
					{
						v1.what=(int *)(*GetAtIP(int));
						deb1("[%d]",(int)v1.what);
						immi1=true;
					}
					//make register shift
					switch ( ( (opc[1]>>1)&adrSTOREPOINTER))//base register
					{
					case adrSTORECPU:
						if (immi1)//import
                        {
							ReadINT((char *)Ins->code+ToCodeINT[hdrImportedOffset]+(int)v1.what,imINT1);
							v1.what=&imINT1;
                        }
						break;
					case adrES:(char *)(v1.what)+=Register[regES];deb0("ES");break;
					case adrDS:(char *)(v1.what)+=Register[regDS];deb0("DS");break;
					case adrBP:(char *)(v1.what)+=Register[regBP];deb1("{%d}",(int)v1.what); deb0("BP");break;
					}
					//  debprintf("(value=%d)\n",*((int*)v1.what));
				}
				else//imidiate
				{
					if (opc[1]&1) //register
					{
						v1.what=Register+opc[2];
						deb1("a%d",opc[2]);
						switch(opc[0]&63)
						{
						case opvcpuWMOV:
						case opvcpuCMOV:
							*v1.what=0;
							deb0(" CLEARED ");
							break;
						}
					}
					else
					{
						imINT1=*GetAtIP(int);
						deb1("$%04x",imINT1);
						v1.what=&imINT1;
					}
				}
				deb1("(=$%d)",*((int *)v1.what));
				deb0(",");
				
				//2nd parameter
				if (opc[0]&128)//pointer
				{
					if (opc[1]&(1<<4))//register
					{
						v2.what=(int *)Register[opc[3]];
						deb1("[a%d]",opc[3]);
					}
					else//immidiate
					{
						v2.what=(int *)(*GetAtIP(int));
						deb1("[%d]",(int)v2.what);
						immi2=true;
					}
					//make register shift
					switch ((opc[1]>>5)&adrSTOREPOINTER)//base register
					{
					case adrSTORECPU:
						if (immi2)//import
						{
							ReadINT((char *)Ins->code+ToCodeINT[hdrImportedOffset]+(int)v2.what,imINT2);
							v2.what=&imINT2;
						}
						break;
					case adrES:(char *)(v2.what)+=Register[regES];deb0("ES");break;
					case adrDS:(char *)(v2.what)+=Register[regDS];deb0("DS");break;
					case adrBP:(char *)(v2.what)+=/*Register[regSS]+*/Register[regBP];deb1("{%d}",(int)v2.what);deb0("BP");break;
					}
				}
				else//imidiate
				{
					if (opc[1]&(1<<4))//register
					{
						v2.what=Register+opc[3];
						deb1("a%d",opc[3]);
					}
					else
					{
						imINT2=*GetAtIP(int);
						deb1("$%04x",imINT2);
						v2.what=&imINT2;
					}
				}
				
				deb1("(=$%d)",*((int *)v2.what));
				deb0("}");
				
				switch(opc[0]&63)
				{
				case opvcpuMOV:
					DoOper(int,int,=);
					deb0("MOV");
					break;
					
				case opvcpuCMOV:
					DoOper(char,char,=);
					deb0("CMOV");
					break;
					
				case opvcpuWMOV:
					DoOper(short,short,=);
					deb0("WMOV");
					break;
					
				case opvcpuDMOV:
					DOUBLEFix;
					DOUBLEdeb0('=');
					DoOper(double,double,=);
					DOUBLEdeb2;
					deb0("DMOV");
					break;
					
				case opvcpuCALLEX://also add sp,paramcount
					deb0("CALLEX");
					{
						int x,nr,num,*result;
						x=ToCodeINT[hdrImportedOffset]+*((int*)v1.what);
						ReadINT((char *)Ins->code+x,x);
						scErrorNo=scOK;
						debflush();
						/*
						(1-16) First 16 bits of v2.what for param count,
						(17-22)next 9 bits reserved,
						(22)   1 bit for result (0-non-void, 1-void)
						(23,24,25)3 bits for struct:0-none,1-normal,2-7(size=1<<(bits-2))
						(26)   1 bit for calling script function from other instance
						(27)   1 bit for result type (0-int, 1-double)
						(28)   1 bit for member function (0-not a member)
						(29-32)   last 4 bits for function dispatcher number
						*/
						nr=(*((unsigned int*)v2.what))>>28;
						num=(*(unsigned int*)v2.what)&0x0ffff;
						
						if (!scUserFunctionDispatcher[nr])
							rerr2( "Undefined dispatcher (%d) used!\n",nr);
						scParamCount=num;
						
						//another script
						//TODO:if instance is running, call forked instance, else call instance
						if (*((unsigned int*)v2.what)&scDispatch_Script)
						{
							scScriptTypeHeader* h=*(void**)(Register[regSS]+Register[regSP]);
							
							deb1("\nCalling another script with %d params\n\n",num);
							
							scVCall_Instance(h->ins,((int*)h)[*v1.what/4],num,((int*)(Register[regSS]+Register[regSP]))+1);
							
							deb0("\nEnd of calling another script\n\n");
							
							if (scErrorNo) goto scError_Exit;
							
							if (*((unsigned int*)v2.what)&scDispatch_Double)
								*((double*)Ins->data_stack)=*((double*)h->ins->data_stack);
							else
								*((int*)Ins->data_stack)=*((int*)h->ins->data_stack);
							
						} 
						else	//imported function
						{
							if ((*((unsigned int*)v2.what))&scDispatch_Void)
								result=&x;
							else
								result=&Register[0];
							scUserFunctionDispatcher[nr](
								result,
								(void *)x,
								(int *)(Register[regSS]+Register[regSP]),
								num,
								*((unsigned int*)v2.what));
						}
						scActual_Instance=Ins;//in case it changes Actual_Instance (like Kernel call)
						Register[regSP]+=num;//add sp,num
						if (scErrorNo==scErr_Violation)
							rerr("Priority violation!");
						scErrorNo=scOK;
						
						if (Ins->flags.paused)
						{	
							//somebody paused us
							scActual_Instance=NULL;
							actspeed=speed;
							return 0;
						}
						
					}
					break;
					
				case opvcpuCOPY:
					if (Register[regCX]>0)
						memcpy(v1.what,v2.what,Register[regCX]);
					deb0("COPY");
					break;
					
				case opvcpuADD:
					DoOper(int,int,+=);
					deb0("ADD");
					break;
					
				case opvcpuDADD:
					DOUBLEFix;
					DOUBLEdeb1('+');
					DoOper(double,double,+=);
					DOUBLEdeb2;
					deb1(" {%f} ",(*((double*)v1.what)));
					deb0("DADD");
					break;
					
				case opvcpuSUB:
					DoOper(int,int,-=);
					deb0("SUB");
					break;
					
				case opvcpuDSUB:
					DOUBLEFix;
					DOUBLEdeb1('-');
					DoOper(double,double,-=);
					deb0("DSUB");
					DOUBLEdeb2;
					break;
					
				case opvcpuCMPE:
					DoOper2(int,int,==);
					deb0("CMPE");
					break;
					
				case opvcpuCMPG:
					if (Register[regOPTIONS]&optionsvcpuUNSIGNED)
					{DoOper2(unsigned,unsigned,>);
					Register[regOPTIONS]^=optionsvcpuUNSIGNED;
					}
					else
						DoOper2(int,int,>);
					deb0("CMPG");
					break;
					
				case opvcpuCMPL:
					if (Register[regOPTIONS]&optionsvcpuUNSIGNED)
					{DoOper2(unsigned,unsigned,<);
					Register[regOPTIONS]^=optionsvcpuUNSIGNED;
					}
					else
						DoOper2(int,int,<);
					deb0("CMPL");
					break;
					
				case opvcpuCMPNG:
					if (Register[regOPTIONS]&optionsvcpuUNSIGNED)
					{DoOper2(unsigned,unsigned,<=);
					Register[regOPTIONS]^=optionsvcpuUNSIGNED;
					}
					else
						DoOper2(int,int,<=);
					deb0("CMPNG");
					break;
					
				case opvcpuCMPNL:
					if (Register[regOPTIONS]&optionsvcpuUNSIGNED)
					{DoOper2(unsigned,unsigned,>=);
					Register[regOPTIONS]^=optionsvcpuUNSIGNED;
					}
					else
						DoOper2(int,int,>=);
					deb0("CMPNL");
					break;
					
				case opvcpuMUL:
					DoOper(int,int,*=);
					deb0("MUL");
					break;
					
				case opvcpuDMUL:
					DOUBLEFix;
					DOUBLEdeb1('*');
					DoOper(double,double,*=);
					DOUBLEdeb2;
					deb0("DMUL");
					break;
					
				case opvcpuDIV:
					if (Register[regOPTIONS]&optionsvcpuUNSIGNED)
					{DoOper(unsigned,unsigned,/=);
					Register[regOPTIONS]^=optionsvcpuUNSIGNED;
					}
					else
						DoOper(int,int,/=);
					deb0("DIV");
					break;
					
				case opvcpuFIXMUL:
					*((int*)v1.what)=fixedmul(*((int*)v1.what),*((int*)v2.what));
					deb0("FIXMUL");
					break;
					
				case opvcpuFIXDIV:
					*((int*)v1.what)=fixeddiv(*((int*)v1.what),*((int*)v2.what));
					deb0("FIXDIV");
					break;
					
				case opvcpuDFIX:
					DOUBLEFix;
					deb1(" {%f} ",(*((double*)v2.what)));
					*((int*)v1.what)=(int)((*((double*)v2.what))*65536);
					deb0("DFIX");
					break;
					
				case opvcpuFIXDBL:
					*((double*)v1.what)=*((int*)v2.what)/65536.0;
					deb1(" {%f} ",(*((double*)v1.what)));
					deb0("FIXDBL");
					break;
					
				case opvcpuDDIV:
					DOUBLEFix;
					DOUBLEdeb1('/');
					DoOper(double,double,/=);
					DOUBLEdeb2;
					deb0("DDIV");
					break;
					
				case opvcpuDCMPE:
					DOUBLEFix;
					DOUBLEdeb1('=');
					DoOper3(double,double,==);
					DOUBLEint2;
					deb0("DCMPE");
					break;
					
				case opvcpuDCMPG:
					DOUBLEFix;
					DOUBLEdeb1('^');
					DoOper3(double,double,>);
					DOUBLEint2;
					deb0("DCMPG");
					break;
					
				case opvcpuDCMPL:
					DOUBLEFix;
					DOUBLEdeb1('_');
					DoOper3(double,double,<);
					DOUBLEint2;
					deb0("DCMPL");
					break;
					
				case opvcpuDCMPNG:
					DOUBLEFix;
					DOUBLEdeb1('#');
					DoOper3(double,double,<=);
					DOUBLEint2;
					deb0("DCMPNG");
					break;
					
				case opvcpuDCMPNL:
					DOUBLEFix;
					DOUBLEdeb1('#');
					DoOper3(double,double,>=);
					DOUBLEint2;
					deb0("DCMPNL");
					break;
					
				case opvcpuMOD:
					if (Register[regOPTIONS]&optionsvcpuUNSIGNED)
					{
						DoOper(unsigned,unsigned,%=);
						Register[regOPTIONS]^=optionsvcpuUNSIGNED;
					}
					else
						DoOper(int,int,%=);
					deb0("MOD");
					break;
					
				case opvcpuAND:
					DoOper(int,int,&=);
					deb0("AND");
					break;
					
				case opvcpuOR:
					DoOper(int,int,|=);
					deb0("OR");
					break;
					
				case opvcpuXOR:
					DoOper(int,int,^=);
					deb0("XOR");
					break;
					
				case opvcpuANDL:
					DoOper2(int,int,&&);
					deb0("ANDL");
					break;
					
				case opvcpuORL:
					DoOper2(int,int,||);
					deb0("ORL");
					break;
					
				case opvcpuSHR:
					if (Register[regOPTIONS]&optionsvcpuUNSIGNED)
					{
						DoOper(unsigned,unsigned,>>=);
						Register[regOPTIONS]^=optionsvcpuUNSIGNED;
					}
					else
						DoOper(int,int,>>=);
					deb0("SHR");
					break;
					
				case opvcpuSHL://does not care if unsigned
					DoOper(int,int,<<=);
					deb0("SHL");
					break;
					
				case opvcpuIDBL:
					DoOper(double,int,=);
					deb0("IDBL");
					break;
					
#pragma warning(disable:4244)
				case opvcpuDINT:
					DoOper(int,double,=);
					deb0("DINT");
					break;
					
				case opvcpuFDBL:
					DoOper(double,float,=);
					deb0("FDBL");
					break;
					
				case opvcpuDFLT:
					DoOper(float,double,=);
					deb0("DFLT");
					break;
#pragma warning(default:4244)
					
				case opvcpuJTRUE:
					if (*v1.what)
						Register[regIP]+=*v2.what;
					deb0("JTRUE");
					break;
					
				case opvcpuJFALSE:
					if (!*v1.what)
						Register[regIP]+=*v2.what;
					deb0("JFALSE");
					break;
					
				case opvcpuFORK:
					if ((!(opc[0]&64))&&*v1.what==-1)//exit the forked instance.
					{
						/*delete instance*/
						scFree_Instance(scActual_Instance);
						scActual_Instance=NULL;
						deb0("EXIT");
						return 1;//end of instance
					}
					else//fork this instance.
					{
						scInstance* i;//TODO:
						i=scFork_Instance(Ins);
						scLaunch_Instance_GOTO(i,speed,Register[regIP]);
						*v1.what=i->flags.forkno;
						Register[regIP]=*v2.what;
					} 
					deb0("FORK");
					break;
					
				default:
					deb0("Unknown!");
					rerr3( "Unknown opcode:%d in the script at %d!\n",opc[0]&63,Register[regIP]-4);
					
				}
				deb0("\n");
			}
	}	while(1);

	scError_Exit:
	scActual_Instance=NULL;
	return -1;//error happened
}
