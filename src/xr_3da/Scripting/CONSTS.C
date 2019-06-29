/************************************************************************
       -= SEER - C-Scripting engine v 0.3a =-
State      : under implementation
Author     : Przemyslaw Podsiadly
             email: ppodsiad@elka.pw.edu.pl
             WWW:   http://home.elka.pw.edu.pl/~ppodsiad
File:consts.c
Desc:constants (but without vcpu opcodes)
************************************************************************/
#define INTERNAL_FILE
#include <seer.h>
#include "internal.h"

pmem scExternals={NULL,0,0};

char* parse_error= "Parse error at %s";
char* invalid_operand_to= "Invalid operand%s to %s";
char* illegal_cast= "Illegal cast %s";
char* internal_compiler_error= "Internal compiler error%s (%d)";
char* illegal_type_conversion= "Illegal type conversion %s";

//signed unsigned short long const volatile
NameNInt Standard_Types[]={
     {"int",4},
     {"long",8},//has to be 1st (not 0th, but after int, used in declare.c)
     {"char",1},
     {"fixed",4},
     {"float",4},
     {"double",8},
     {"short",2},
     {"register",4},
     {"void",0},
     {NULL,0}//end delimeter
     };//See expression.c:GetValType(...)-explicitly using& in expressi.c

char *Keywords[]={
/****C****/
     "if",
     "else",
     "for",
     "while",
     "switch",
     "case",
     "return",
     "do",
     "typedef",
     "struct",
     "union",
     "enum",
     "register",
     "const",
     "sizeof",
     "break",
     "default",
     "unsigned",
     "signed",
     "static",
/****C++**/
     "this",
     "class",
     "public","private","protected",
     "operator",
     "new","delete",
/****SeeR*/
     "import",
     "export",
     "secured",
     "inherit",
     "__vcpu",
     "typeof",
     "script",
     NULL //end delimeter
     };
char *Special_Symbols[]={
     "..",
     "\\\\",
     "<<=",">>=",
     "##",
     "\\'","\\\"",
     "->",
     "||","&&",
     "//",
     "/*","*/",
     "+=","-=","/=","*=","%=",
     ">>","<<",
     "==","!=","<=",">=",
     "++","--",
     "[]",
/*   new C++ operators */
     "::",
/*   SeeR special operators - now not implemented */
//     "=>","<->","<>","<?>",":)","(:)",...
/*   rest */
     "!","~",
     "{","}"
     "[","]",
     "(",")",
     "*","+","-","/","%",
     "&","|",
     ".",
     "\"","'",
     ";",
     "?",":",
     "#",
     "~",
     NULL//end delimeter
};
