// xrStatisticConvert.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Statistic.h"

int _tmain(int argc, _TCHAR* argv[])
{
	WeaponUsageStatistic WUS;

	for (int i=1; i<argc; i++)
	{
		printf("%s ", argv[i]);
		WUS.LoadFile(argv[i]);
	}
	//-----------------------------------------
	SYSTEMTIME Time;
	GetLocalTime(&Time);

	string1024 FileName = "";
	sprintf(FileName, "%s(%s)_(%s)_%02d.%02d.%02d_%02d.%02d.%02d.txt", FilePath, LevelName, GameType, Time.wMonth, Time.wDay, Time.wYear, Time.wHour, Time.wMinute, Time.wSecond);
	//-----------------------------------------
	WUS.SaveFile(FileName);
	return 0;
}

