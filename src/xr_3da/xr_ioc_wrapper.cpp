#include "stdafx.h"
#include "xr_ioc_wrapper.h"
#include "xr_IOConsole.h"

	//execute
void CConsoleWrapper::Execute				(LPCSTR cmd)
{
	Console->Execute(cmd);
}

void CConsoleWrapper::ExecuteScript		(LPCSTR name)
{
	Console->ExecuteScript(name);
}

	//get
BOOL CConsoleWrapper::GetBool				(LPCSTR cmd, BOOL& val)
{
	Console->GetBool(cmd, val);
	return val;
}
void CConsoleWrapper::SetBool				(LPCSTR cmd, BOOL val)
{
	sprintf		(buff,"%s %s",cmd,(val)?"on":"off" );
	Console->Execute(buff);
}

float CConsoleWrapper::GetFloat			(LPCSTR cmd, float& val, float& min, float& max)
{
	return Console->GetFloat(cmd, val, min, max);
}

void CConsoleWrapper::SetFloat				(LPCSTR cmd, float val)
{
	sprintf		(buff,"%s %f", cmd, val);
	Console->Execute(buff);
}

int CConsoleWrapper::GetInteger				(LPCSTR cmd, int& val, int& min, int& max)
{
	return Console->GetInteger(cmd, val, min, max);
}

void CConsoleWrapper::SetInteger				(LPCSTR cmd, int val)
{
	sprintf		(buff,"%s %d", cmd, val);
	Console->Execute(buff);
}

char * CConsoleWrapper::GetString			(LPCSTR cmd)
{
	return Console->GetString(cmd);
}

void CConsoleWrapper::SetString				(LPCSTR cmd, LPCSTR val)
{
	sprintf		(buff,"%s %s", cmd, val);
	Console->Execute(buff);
}
/*
char * CConsoleWrapper::GetNextValue		(LPCSTR cmd)
{
	return Console->GetNextValue(cmd);
}

char * CConsoleWrapper::GetPrevValue		(LPCSTR cmd)
{
	return Console->GetPrevValue(cmd);
}*/
