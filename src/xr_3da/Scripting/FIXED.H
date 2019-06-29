/************************************************************************
       -= SEER - C-Scripting engine v 0.94a =-
State      : under implementation
Author     : Przemyslaw Podsiadly
             email: ppodsiad@elka.pw.edu.pl
             WWW:   http://home.elka.pw.edu.pl/~ppodsiad
File:fixed.h
Desc:fixed arythmetics, included from vcpudeco.c
************************************************************************/
#ifndef __INLINE__

#ifdef SC_MSVC
#define __INLINE__      __inline
#else
#define __INLINE__      inline
#endif

#endif


#ifdef SC_MSVC
//portable

__INLINE__ int fixedmul(int x,int y)
{int64 d=x;
 d*=y;
 d>>=16;
 return (int)d;
}

__INLINE__ int fixeddiv(int x,int y)
{int64 d=x;
 d<<=16;
 if (y) d/=y;
 return (int)d;
}

#else
#if (defined(SC_GCC) && defined(SC_INTEL))
//Copied from original Allegro's

__INLINE__ int fixedmul(int x, int y)
{
   int result;

   asm (
      "  movl %1, %0 ; "
      "  imull %2 ; "                     /* do the multiply */
      "  shrdl $16, %%edx, %0 ; "

      "  shrl $16, %%edx ; "              /* check for overflow */
      "  jz 0f ; "
      "  cmpw $0xFFFF, %%dx ; "
      "  je 0f ; "

      "  movl $2, _errno ; "              /* on overflow, set errno */
      "  movl $0x7fffffff, %0 ; "         /* and return MAXINT */
      "  cmpl $0, %1 ; "
      "  jge 1f ; "
      "  negl %0 ; "
      " 1: "
      "  cmpl $0, %2 ; "
      "  jge 0f ; "
      "  negl %0 ; "

      " 0: "                              /* finished */

   : "=&a" (result)                       /* the result has to go in eax */

   : "mr" (x),                            /* x and y can be regs or mem */
     "mr" (y)

   : "%edx", "%cc"                        /* clobbers edx and flags */
   );

   return result;
}

__INLINE__ int fixeddiv(int x, int y)
{
   int result;

   asm (
      "  movl %2, %%ecx ; "
      "  xorl %%ebx, %%ebx ; "

      "  orl %0, %0 ; "                   /* test sign of x */
      "  jns 0f ; "

      "  negl %0 ; "
      "  incl %%ebx ; "

      " 0: "
      "  orl %%ecx, %%ecx ; "             /* test sign of y */
      "  jns 1f ; "

      "  negl %%ecx ; "
      "  incb %%ebx ; "

      " 1: "
      "  movl %0, %%edx ; "               /* check the range is ok */
      "  shrl $16, %%edx ; "
      "  shll $16, %0 ; "
      "  cmpl %%ecx, %%edx ; "
      "  jae 2f ; "

      "  divl %%ecx ; "                   /* do the divide */
      "  orl %0, %0 ; "
      "  jns 3f ; "

      " 2: "
      "  movl $2, _errno ; "              /* on overflow, set errno */
      "  movl $0x7fffffff, %0 ; "         /* and return MAXINT */

      " 3: "
      "  testl $1, %%ebx ; "              /* fix up the sign of the result */
      "  jz 4f ; "

      "  negl %0 ; "

      " 4: "                              /* finished */

   : "=a" (result)                        /* the result has to go in eax */

   : "0" (x),                             /* x in eax */
     "g" (y)                              /* y can be anywhere */

   : "%ebx", "%ecx", "%edx", "%cc"        /* clobbers ebx, ecx, edx + flags */
   );

   return result;
}

#else

#error Fixed operations not implemented for machine/compiler

#endif

#endif
