#pragma once

//---------------------------------------------------------------------------
#include "../xr_ioconsole.h"
#include "../xr_ioc_cmd.h"

#define CMD_ADD(cls,p1,p2,p3,p4,ctrl,p0)		{ if (!ctrl) {CMD4(cls,p1,p2,p3,p4); } \
												else {sprintf(p0,p1 " #set_%p", p2); Console->Execute(p0);}}
#define CMD_CLEAR(p1)							{Console->Execute(p1 " #set_0");}

class CCC_SV_Int : public CCC_Integer {
protected:
	int StoredValue;
public:
	CCC_SV_Int(LPCSTR N, int *pValue,int _min=0, int _max=999) :
	  CCC_Integer(N,pValue,_min,_max),
		  StoredValue(*pValue)
	  {};

	  virtual void	Execute	(LPCSTR args);

	  virtual void	Save	(IWriter *F)	{};
	  virtual void	Status	(TStatus& S)
	  {
		  CCC_Integer::Status(S);
	  }
};

class CCC_SV_Flt : public CCC_Float {
protected:
	float StoredValue;
public:
	CCC_SV_Flt(LPCSTR N, float *pValue,float _min=0, float _max=999) :
	  CCC_Float(N,pValue,_min,_max),
		  StoredValue(*pValue)
	  {};

	  virtual void	Execute	(LPCSTR args);

	  virtual void	Save	(IWriter *F)	{};
	  virtual void	Status	(TStatus& S)
	  {
		  CCC_Float::Status(S);
	  }
};
//---------------------------------------------------------------