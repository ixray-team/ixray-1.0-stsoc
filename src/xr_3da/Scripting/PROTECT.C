/************************************************************************
-= SEER - C-Scripting engine v 0.7a =-
State      : under implementation
Author     : Przemyslaw Podsiadly
email: ppodsiad@elka.pw.edu.pl
WWW:   http://home.elka.pw.edu.pl/~ppodsiad
File:protect.c
Desc:priorities, "protected" calling exported functions in script
************************************************************************/
#include <stdio.h>
#define INTERNAL_FILE
#include <seer.h>
#include "internal.h"

void  SAPI scSet_Priority(scInstance *Ins, int priority_level)
{
	//<ERASED> scKernelOnly(;);
	if (Ins)
	{
		Ins->flags.priority=priority_level;
	}
}

int  SAPI scGet_Priority(scInstance *Ins)//if NULL returns scrights_TOP
{
	//<ERASED> scKernelOnly(scrights_USER);
	if (!Ins)
		return scrights_TOP;
	return Ins->flags.priority;
}

int  SAPI scGet_Actual_Priority()
{
	if (!scActual_Instance) return scrights_TOP;
	return scActual_Instance->flags.priority;
	return scGet_Priority(scActual_Instance);
}
