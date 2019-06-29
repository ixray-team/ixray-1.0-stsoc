#pragma once

class ENGINE_API CConsoleWrapper
{
#define MAX_CMD_LEN 256
	char			buff[MAX_CMD_LEN];

public:
	//execute
	void			Execute				(LPCSTR cmd);
	void			ExecuteScript		(LPCSTR name);

	//get
	BOOL			GetBool				(LPCSTR cmd, BOOL& val);
	void			SetBool				(LPCSTR cmd, BOOL val);
	float			GetFloat			(LPCSTR cmd, float& val, float& min, float& max);
	void			SetFloat			(LPCSTR cmd, float val);
	int				GetInteger			(LPCSTR cmd, int& val, int& min, int& max);
	void			SetInteger			(LPCSTR cmd, int val);
	char *			GetString			(LPCSTR cmd);
	void			SetString			(LPCSTR cmd, LPCSTR val);
//	char *			GetNextValue		(LPCSTR cmd);
//	char *			GetPrevValue		(LPCSTR cmd);

};