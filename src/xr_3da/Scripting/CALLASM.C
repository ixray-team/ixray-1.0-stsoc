/************************************************************************
       -= SEER - C-Scripting engine v 0.93a =-
State      : complete
Author     : Przemyslaw Podsiadly
             email: ppodsiad@elka.pw.edu.pl
             WWW:   http://home.elka.pw.edu.pl/~ppodsiad
File:callasm.c
Desc:calls functions with parameters in void* params and size count
     returning int or double.

[99.08.27]
Implementing functions returning struct....
---VC:switch(sizeof(struct))
1 via al,2 via ax,4 via eax,8 via eax:edx, else :push adr
---DJGPP:switch(sizeof(struct))
4 via eax, else :push adr
---SeeR:4 always push adr,
so:
push params...
push this
push return_adr
call

[99.04.02]
Dedicated versions for
          GCC on intel,MS Visual C
Portable version for others (limited to 12*4 bytes long arguments)

DJGPP:Don't use `-fomit-frame-pointer' while compiling it !!!


Problems (after reading GCC info on function passing - gcc:Interface):
1.Returning struct and union: 1, 2, 4, or 8 bytes - returned normally,
other - a pointer to dest is passed (other compilers may do it also for
1-8 long structs, RISC other than GCC).
2. On some machines, the first few arguments are passed in registers; in
others, all are passed on the stack [ibidem].
    -=>hope "portable" is OK in that metter (at least with Sparc)
3. On some machines (particularly the Sparc), certain types of arguments
are passed "by invisible reference".  This means that the value is
stored in memory, and the address of the memory location is passed to
the subroutine [ibidem].
    -=>so SPARC may need a dedicated callasm implementation. More - it should
       know the argument types. Where from? Script should give this info.
       How? How to avoid this? Maybe disallow passing those "certain types".
************************************************************************/
#define INTERNAL_FILE
#include <seer.h>
#include "internal.h"
#define USE_DEDICATED 1

#if defined(SC_MSVC) && USE_DEDICATED
int _msvc_regEDX;
//**************** MS Visual C++ 5 version **************
//#pragma optimize( "y", on )

int callfunc(void *func,void *params,int count)
{
   int reti;

   if (count%4) //error
    ;
   if (!count)
    {
           __asm {
                   call func
                   mov reti,eax
           }
    }
    else
    {
      __asm {
        sub esp,count
        mov esi,params
        mov ecx,count
        shr ecx,2
        jcxz endl1
                }
    loop1:
      __asm {
        mov eax,[esi]
        mov [esp],eax
        add esp,4
        add esi,4
        loop loop1
        sub esp,count
        }
    endl1:
      __asm {
        call func
	mov reti,eax
	mov [_msvc_regEDX],edx
	add esp,count
        }
   }

   return reti;
}

int callmember(void *func,void *params,int count)
{
   int reti;

   if (count%4) //error
    ;
   if (!count) //impossible
    {
           __asm {
                   call func
                   mov reti,eax
           }
    }
    else
    {
      __asm {
        sub esp,count
        mov esi,params
        mov ecx,count
        shr ecx,2

        mov edx,[esi] //copy 'this' to edx
        add esi,4
        dec ecx
        sub count,4
        jcxz endl1m
                }
    loop1m:
      __asm {
        mov eax,[esi]
        mov [esp],eax
        add esp,4
        add esi,4
        loop loop1m
        sub esp,count
        }
    endl1m:
      __asm {
        mov ecx,edx     //'this' should be in ecx
        call func
        mov reti,eax
	mov [_msvc_regEDX],edx
        add esp,count
        }
   }

   return reti;
}

double callfunc_d(void *func,void *params,int count)
{double retd;
//   if (count%4) //error apeared
//    ;
   if (!count)
    {//pop edi,esi,ebx, fld laduje - fsv
   __asm {
        call func
                fst retd
                ;pop ebx
        ;pop esi
        ;pop edi
        ;leave
        ;ret
     }
    }
    else
    {
    __asm {
        mov esi,params
        sub esp,count
        mov ecx,count
        shr ecx,2
        jcxz endl2
        }
        loop2:
    __asm {
        mov eax,[esi]
        mov [esp],eax
        add esp,4
        add esi,4
        loop loop2
        sub esp,count
        }
        endl2:
    __asm {
        call func
        add esp,count
        fst retd
        ;pop ebx
        ;pop esi
        ;pop edi
        ;leave
        ;ret
        }
   }

  return retd;//ret was earlier, it's only to disable a warning
}

double callmember_d(void *func,void *params,int count)
{double retd;
//   if (count%4) //error apeared
//    ;
   if (!count)//also imposible
    {//pop edi,esi,ebx, fld laduje - fsv
   __asm {
        call func
                fst retd
     }
    }
    else
    {
    __asm {
        mov esi,params
        sub esp,count
        mov ecx,count
        shr ecx,2

        mov edx,[esi] //copy 'this' to edx
        add esi,4
        dec ecx
        sub count,4
        jcxz endl2m
        }
        loop2m:
    __asm {
        mov eax,[esi]
        mov [esp],eax
        add esp,4
        add esi,4
        loop loop2m
        sub esp,count
        }
        endl2m:
    __asm {
        mov ecx,edx     //'this' should be in ecx
        call func
        add esp,count
        fst retd
        }
   }

  return retd;
}


#elif (defined(SC_GCC) && defined(SC_INTEL)) && USE_DEDICATED
//**************** Intel GCC version **************
int callfunc(void *func,void *params,int count)
{
   int ret;

   if (count%4) //error
    ;
   if (!count)
    {
   asm (
      "  call *%%edi ; "
   : "=&a" (ret)                       /* return value in eax */
   : "D" (func)                        /* function in edi */
   : "memory"                          /* assume everything is clobbered */
   );
    }
    else
    {
    asm (
      "  subl %2,%%esp ; "
      "  movl %2,%%ecx ; "
      "  shrl $2,%%ecx ; "
      "  jcxz endl1 ; "
      "  loop1: "
      "  movl (%%esi),%%eax ; "
      "  movl %%eax,(%%esp) ; "
      "  addl $4,%%esp ; "
      "  addl $4,%%esi ; "
      "  loop loop1 ; "
      "  subl %2,%%esp ; "
      "  endl1: "
      "  call *%%edi ; "
      "  addl %2,%%esp  "
   : "=&a" (ret)                       /* return value in eax */
   : "D" (func),                       /* function in edi */
     "d" (count),                      /* count in edx */
     "S" (params)                      /* params in esi */
   : "memory"                          /* assume everything is clobbered */
   );
   }

   return ret;
}

int callmember(void *func,void *params,int count)
{//here it is fun - same as callfunc -
 //this is normally the first parameter
 return callfunc(func,params,count);
}

double callfunc_d(void *func,void *params,int count)
{  double dret=111.12;
//   if (count%4) //error apeared
//    ;
   if (!count)
    {
   asm (
      "  call *%%edi ; "
//      "  popl %%esi;"
//      "  popl %%edi;"
      "  fstpl -8(%%ebp);"
//      "  fstpl (dret);"
      "  leave;"
      "  ret ; "
//      "  fstpl -8(%%ebp);"
   :
   : "D" (func)                        /* function in edi */
   : "memory"                          /* assume everything is clobbered */
   );
    }
    else
    {
    asm (
      "  subl %1,%%esp ; "
      "  movl %1,%%ecx ; "
      "  shrl $2,%%ecx ; "
      "  jcxz endl2 ; "
      "  loop2: "
      "  movl (%%esi),%%eax ; "
      "  movl %%eax,(%%esp) ; "
      "  addl $4,%%esp ; "
      "  addl $4,%%esi ; "
      "  loop loop2 ; "
      "  subl %1,%%esp ; "
      "  endl2: "
      "  call *%%edi ; "
      "  addl %1,%%esp;  "
//      "  popl %%esi;"
//      "  popl %%edi;"
//      "  fstpl -8(%%ebp);"
//      "  fstpl (dret);"
//      "  movl %%ebp,%%esp;"
//      "  popl %%ebp;"

      "  leave;"
      "  ret; "
   :
   : "D" (func),                       /* function in edi */
     "d" (count),                      /* count in edx */
     "S" (params)                      /* params in esi */
   : "memory"                     /* assume everything is clobbered */
   );
   }
  printf("LONG=%f\n",dret);fflush(stdout);
  return dret;//ret was earlier, it's only to disable a warning
}

double callmember_d(void *func,void *params,int count)
{//here it is also fun - same as callmember !
 return callfunc_d(func,params,count);
}

#else

//**************** portable(?) version **************
#define _I_PA(x) (((int*)params)[x])
#define _INT_FUNC(x) ((int (*)x)func)
#define _DBL_FUNC(x) ((double (*)x)func)

int callfunc(void *func,void *params,int count)
{switch(count/4)
 {
  case 0:
        return _INT_FUNC(())
                ();
  case 1:
        return _INT_FUNC((int))
                (_I_PA(0));
  case 2:
        return _INT_FUNC((int,int))
                (_I_PA(0),_I_PA(1));
  case 3:
        return _INT_FUNC((int,int,int))
                (_I_PA(0),_I_PA(1),_I_PA(2));
  case 4:
        return _INT_FUNC((int,int,int,int))
                (_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3));
  case 5:
        return _INT_FUNC((int,int,int,int,int))
                (_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3),_I_PA(4));
  case 6:
        return _INT_FUNC((int,int,int,int,int,int))
                (_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3),_I_PA(4),_I_PA(5));
  case 7:
        return _INT_FUNC((int,int,int,int,int,int,int))
                (_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3),_I_PA(4),_I_PA(5),_I_PA(6));
  case 8:
        return _INT_FUNC((int,int,int,int,int,int,int,int))
                (_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3),_I_PA(4),_I_PA(5),_I_PA(6),_I_PA(7));
  case 9:
        return _INT_FUNC((int,int,int,int,int,int,int,int,int))
                (_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3),_I_PA(4),_I_PA(5),_I_PA(6),_I_PA(7),_I_PA(8));
  case 10:
        return _INT_FUNC((int,int,int,int,int,int,int,int,int,int))
                (_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3),_I_PA(4),_I_PA(5),_I_PA(6),_I_PA(7),_I_PA(8),_I_PA(9));
  case 11:
        return _INT_FUNC((int,int,int,int,int,int,int,int,int,int,int))
                (_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3),_I_PA(4),_I_PA(5),_I_PA(6),_I_PA(7),_I_PA(8),_I_PA(9),_I_PA(10));
  case 12:
        return _INT_FUNC((int,int,int,int,int,int,int,int,int,int,int,int))
                (_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3),_I_PA(4),_I_PA(5),_I_PA(6),_I_PA(7),_I_PA(8),_I_PA(9),_I_PA(10),_I_PA(11));
  default:
        return -1;//set error also (unhandled number of paramters)
 }
}

double callfunc_d(void *func,void *params,int count)
{
 switch(count/4)
 {
  case 0:
        return _DBL_FUNC(())
                ();
  case 1:
        return _DBL_FUNC((int))
                (_I_PA(0));
  case 2:
        return _DBL_FUNC((int,int))
                (_I_PA(0),_I_PA(1));
  case 3:
        return _DBL_FUNC((int,int,int))
                (_I_PA(0),_I_PA(1),_I_PA(2));
  case 4:
        return _DBL_FUNC((int,int,int,int))
                (_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3));
  case 5:
        return _DBL_FUNC((int,int,int,int,int))
                (_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3),_I_PA(4));
  case 6:
        return _DBL_FUNC((int,int,int,int,int,int))
                (_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3),_I_PA(4),_I_PA(5));
  case 7:
        return _DBL_FUNC((int,int,int,int,int,int,int))
                (_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3),_I_PA(4),_I_PA(5),_I_PA(6));
  case 8:
        return _DBL_FUNC((int,int,int,int,int,int,int,int))
                (_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3),_I_PA(4),_I_PA(5),_I_PA(6),_I_PA(7));
  case 9:
        return _DBL_FUNC((int,int,int,int,int,int,int,int,int))
                (_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3),_I_PA(4),_I_PA(5),_I_PA(6),_I_PA(7),_I_PA(8));
  case 10:
        return _DBL_FUNC((int,int,int,int,int,int,int,int,int,int))
                (_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3),_I_PA(4),_I_PA(5),_I_PA(6),_I_PA(7),_I_PA(8),_I_PA(9));
  case 11:
        return _DBL_FUNC((int,int,int,int,int,int,int,int,int,int,int))
                (_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3),_I_PA(4),_I_PA(5),_I_PA(6),_I_PA(7),_I_PA(8),_I_PA(9),_I_PA(10));
  case 12:
        return _DBL_FUNC((int,int,int,int,int,int,int,int,int,int,int,int))
                (_I_PA(0),_I_PA(1),_I_PA(2),_I_PA(3),_I_PA(4),_I_PA(5),_I_PA(6),_I_PA(7),_I_PA(8),_I_PA(9),_I_PA(10),_I_PA(11));
  default:
        return -1;//set error also (unhandled number of paramters)
 }
}

//how to write a portable member caller, while in pure C?
//assume this is passed via the stack (like gcc does it)
int callmember(void *func,void *params,int count)
{//here it is fun - same as callfunc -
 //this is normally the first parameter
 return callfunc(func,params,count);
}
double callmember_d(void *func,void *params,int count)
{//here it is fun - same as callfunc -
 //this is normally the first parameter
 return callfunc_d(func,params,count);
}

#endif
