/************************************************************************
       -= SEER - C-Scripting engine v 0.3a =-
State      : under implementation
Author     : Przemyslaw Podsiadly
             email: ppodsiad@elka.pw.edu.pl
             WWW:   http://home.elka.pw.edu.pl/~ppodsiad
File:code.h
Desc:opcodes of vcpu - declare opvcpu* constants
************************************************************************/

#define optionsvcpuUNSIGNED 1




/**************  VCPU ****************************************/
enum opcodes_vcpu{
opvcpuEXTENDED=0,//with this starts the extended opcodes(that has 4b.),e.g
opvcpuEXPR=0x12345600,//marks end of expression, here seer can be interrupted
//+here are all no-parameter opcodes, like NOP,CLI,STI,WAIT...
opvcpuPUSH=1,
opvcpuPUSHADR,
opvcpuDPUSH,
opvcpuPOP,
opvcpuNEG,//  bin:-1-ARG+1=-ARG
opvcpuDNEG,
opvcpuNOT,// (negacja logiczna)
opvcpuOPTIONS,//optionvcpu* set or not
opvcpuJMP,
opvcpuCALL,
opvcpuMOV,//copies 4bytes--------------------------------------
opvcpuXCHG,
opvcpuCALLEX,
opvcpuCOPY,//dest,src:copy CX-bytes from source to dest,CX=1
opvcpuADD,
opvcpuSUB,
opvcpuMUL,
opvcpuDIV,
opvcpuCMPE,//x,a := x=(x==a)?1:0
opvcpuCMPG,//x,a := x=(x>a)?1:0
opvcpuCMPL,//x,a := x=(x<a)?1:0
opvcpuCMPNG,//x,a := x=(x<=a)?1:0
opvcpuCMPNL,//x,a := x=(x>=a)?1:0
opvcpuMOD,
opvcpuAND,
opvcpuOR,
opvcpuXOR,
opvcpuANDL,//&&
opvcpuORL,//||
opvcpuSHL,
opvcpuSHR,
opvcpuJTRUE,//30
opvcpuJFALSE,
opvcpuDADD,//double:
opvcpuDSUB,
opvcpuDMUL,
opvcpuDDIV,
opvcpuDCMPE,//x,a := x=(x==a)?1:0
opvcpuDCMPG,//x,a := x=(x>a)?1:0
opvcpuDCMPL,//x,a := x=(x<a)?1:0
opvcpuDCMPNG,//x,a := x=(x<=a)?1:0
opvcpuDCMPNL,//x,a := x=(x>=a)?1:0
opvcpuFIXMUL,
opvcpuFIXDIV,
opvcpuCMOV,//copies 1 byte
opvcpuWMOV,//copies 2 bytes
opvcpuDMOV,//copies 8 bytes
opvcpuIDBL,
opvcpuDINT,
opvcpuFDBL,
opvcpuDFLT,
opvcpuDFIX,
opvcpuFIXDBL,
opvcpuFORK,
opvcpuRET,//55-----------------------------------------------------
opvcpuWAIT,// 56
opvcpuCLI,// \Turn OFF\ Multitasking
opvcpuSTI,// /     ON /
opvcpuENTER,//push bp;mov bp,sp
opvcpuLEAVE,//mov sp,bp; pop bp
opvcpuNOP
};//60/max 63 operations
/*
char+short+int  0
double          1
fixed           2
float           3
{
int2double double2int
float2double double2float
char x=double;
float x=int;ITD,DTF//IDBL,DFLT
fixed x=float;FDBL,DINT,SHR x,16
ITD IDBL
DTI DINT
FTD FDBL
DTF DFLT
}
conv_int_to_double 0+1*16
*/

/***************************** SPU *****************************/
/*
int:
1 - opcode nr.
2 - type byte_size
3 - adr : immidiate
4 - reserved
*/
enum opcodes_spu{
spuNOP,
spuCLI,
spuSTI,
spuPUSH,

};

