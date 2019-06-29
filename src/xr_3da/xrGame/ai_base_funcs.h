////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_base_funcs.h
//	Created 	: 13.06.2003
//  Modified 	: 13.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Base function classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#define OUT_MESSAGE(s1,s2)					//Msg(s1,s2);

class CEntityAlive;
class CSE_ALifeObject;

class CBaseFunction {
protected:
	u32				m_dwLastUpdate;
	float			m_fLastValue;
	CEntityAlive	*m_tpLastMonster;
	CSE_ALifeObject	*m_tpLastALifeObject;
	float			m_fMinResultValue;
	float			m_fMaxResultValue;
	char			m_caName[260];

public:

					CBaseFunction()
	{
		m_caName[0] = 0;
	};

	virtual			~CBaseFunction()
	{
	};

	virtual bool	bfCheckForCachedResult();

	virtual float	ffGetValue() = 0;

	virtual u32		dwfGetDiscreteValue(u32 dwDiscretizationValue = 2)
	{
		float fTemp = ffGetValue();
		if (fTemp <= m_fMinResultValue)
			return(0);
		else
			if (fTemp >= m_fMaxResultValue)
				return(dwDiscretizationValue - 1);
			else
				return(iFloor((fTemp - m_fMinResultValue)/(m_fMaxResultValue - m_fMinResultValue)*float(dwDiscretizationValue - 1) + .5f));
	}

	IC		float	ffGetMaxResultValue()
	{
		return	(m_fMaxResultValue);
	}

	IC		float	ffGetMinResultValue()
	{
		return	(m_fMinResultValue);
	}

	IC		LPCSTR	Name()
	{
		return	(m_caName);
	}
};