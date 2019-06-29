////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_primary_funcs.h
//	Created 	: 13.06.2003
//  Modified 	: 13.06.2003
//	Author		: Dmitriy Iassenev
//	Description : Primary function classes
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_base_funcs.h"

class CDistanceFunction : public CBaseFunction {
public:
	typedef CBaseFunction inherited;

	CDistanceFunction()
	{
		m_fMinResultValue = 3.0;
		m_fMaxResultValue = 20.0;
		strcat(m_caName,"Distance");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual ~CDistanceFunction(){}

	virtual float ffGetValue();

	virtual u32	dwfGetDiscreteValue(u32 dwDiscretizationValue)
	{
		float fTemp = ffGetValue();
		if (fTemp <= m_fMinResultValue)
			return(0);
		else
			if (fTemp >= m_fMaxResultValue)
				return(dwDiscretizationValue - 1);
			else
				return(1);
	}
};

class CPersonalHealthFunction : public CBaseFunction {
public:
	typedef CBaseFunction inherited;

	CPersonalHealthFunction()
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName,"PersonalHealth");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual float ffGetValue();
};

class CPersonalMoraleFunction : public CBaseFunction {
public:
	typedef CBaseFunction inherited;

	CPersonalMoraleFunction()
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName,"PersonalMorale");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual ~CPersonalMoraleFunction(){}

	virtual float ffGetValue();
};

class CPersonalCreatureTypeFunction : public CBaseFunction {
public:
	typedef CBaseFunction inherited;

	CPersonalCreatureTypeFunction()
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 21.0;
		strcat(m_caName,"PersonalCreatureType");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual ~CPersonalCreatureTypeFunction(){}

	virtual float ffGetValue();
};

class CPersonalWeaponTypeFunction : public CBaseFunction {
public:
	typedef CBaseFunction inherited;

	CPersonalWeaponTypeFunction()
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 11.0;
		strcat(m_caName,"PersonalWeaponType");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual ~CPersonalWeaponTypeFunction(){}

			u32	  dwfGetWeaponType	();
			float ffGetTheBestWeapon();
	virtual float ffGetValue		();
};

class CPersonalAccuracyFunction : public CBaseFunction {
public:
	typedef CBaseFunction inherited;

	CPersonalAccuracyFunction()
	{
		m_fMinResultValue =   0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName,"PersonalAccuracy");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual ~CPersonalAccuracyFunction(){}

	virtual float ffGetValue();
};

class CPersonalIntelligenceFunction : public CBaseFunction {
public:
	typedef CBaseFunction inherited;

	CPersonalIntelligenceFunction()
	{
		m_fMinResultValue =   0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName,"PersonalIntelligence");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual ~CPersonalIntelligenceFunction(){}

	virtual float ffGetValue();
};

class CPersonalRelationFunction : public CBaseFunction {
public:
	typedef CBaseFunction inherited;

	CPersonalRelationFunction()
	{
		m_fMinResultValue =   0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName,"PersonalRelation");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual ~CPersonalRelationFunction(){}

	virtual float ffGetValue();
};

class CPersonalGreedFunction : public CBaseFunction {
public:
	typedef CBaseFunction inherited;

	CPersonalGreedFunction()
	{
		m_fMinResultValue =   0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName,"PersonalGreed");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual ~CPersonalGreedFunction(){}

	virtual float ffGetValue();
};

class CPersonalAggressivenessFunction : public CBaseFunction {
public:
	typedef CBaseFunction inherited;

	CPersonalAggressivenessFunction()
	{
		m_fMinResultValue =   0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName,"PersonalAggressiveness");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual ~CPersonalAggressivenessFunction(){}

	virtual float ffGetValue();
};

class CEnemyHealthFunction : public CBaseFunction {
public:
	typedef CBaseFunction inherited;

	CEnemyHealthFunction()
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName,"EnemyHealth");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual ~CEnemyHealthFunction(){}

	virtual float ffGetValue();
};

class CEnemyMoraleFunction : public CBaseFunction {
public:
	typedef CBaseFunction inherited;

	CEnemyMoraleFunction()
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName,"EnemyMorale");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};
	
	virtual ~CEnemyMoraleFunction(){}

	virtual float ffGetValue();
};

class CEnemyCreatureTypeFunction : public CBaseFunction {
public:
	typedef CBaseFunction inherited;

	CEnemyCreatureTypeFunction()
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 21.0;
		strcat(m_caName,"EnemyCreatureType");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual ~CEnemyCreatureTypeFunction(){}

	virtual float ffGetValue();
};

class CEnemyWeaponTypeFunction : public CBaseFunction {
public:
	typedef CBaseFunction inherited;

	CEnemyWeaponTypeFunction()
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 11.0;
		strcat(m_caName,"EnemyWeaponType");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual ~CEnemyWeaponTypeFunction(){}

	virtual float ffGetValue();
};

class CEnemyEquipmentCostFunction : public CBaseFunction {
public:
	typedef CBaseFunction inherited;

	CEnemyEquipmentCostFunction()
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 12.0;
		strcat(m_caName,"EnemyEquipmentCost");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual ~CEnemyEquipmentCostFunction(){}

	virtual float ffGetValue();
};

class CEnemyRukzakWeightFunction : public CBaseFunction {
public:
	typedef CBaseFunction inherited;

	CEnemyRukzakWeightFunction()
	{
		m_fMinResultValue = 1.0;
		m_fMaxResultValue = 12.0;
		strcat(m_caName,"EnemyRukzakWeight");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual ~CEnemyRukzakWeightFunction(){}

	virtual float ffGetValue();
};

class CEnemyAnomalityFunction : public CBaseFunction {
public:
	typedef CBaseFunction inherited;

	CEnemyAnomalityFunction()
	{
		m_fMinResultValue = 1.0;
		m_fMaxResultValue = 12.0;
		strcat(m_caName,"EnemyAnomality");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual ~CEnemyAnomalityFunction(){}

	virtual float ffGetValue();
};

class CGraphPointType0 : public CBaseFunction {
public:
	typedef CBaseFunction inherited;

	CGraphPointType0()
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName,"GraphPointType0");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual ~CGraphPointType0(){}

	virtual float ffGetValue();
};

class CPersonalEyeRange : public CBaseFunction {
public:
	typedef CBaseFunction inherited;

	CPersonalEyeRange()
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName,"PersonalEyeRange");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual ~CPersonalEyeRange(){}

	virtual float ffGetValue();
};

class CPersonalMaxHealth : public CBaseFunction {
public:
	typedef CBaseFunction inherited;

	CPersonalMaxHealth()
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 1000.0;
		strcat(m_caName,"PersonalMaxHealth");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual ~CPersonalMaxHealth(){}

	virtual float	ffGetValue();
	virtual u32		dwfGetDiscreteValue(u32 dwDiscretizationValue = 1);
};

class CEnemyEyeRange : public CBaseFunction {
public:
	typedef CBaseFunction inherited;

	CEnemyEyeRange()
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName,"EnemyEyeRange");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual ~CEnemyEyeRange(){}

	virtual float ffGetValue();
};

class CEnemyMaxHealth : public CBaseFunction {
public:
	typedef CBaseFunction inherited;

	CEnemyMaxHealth()
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 1000.0;
		strcat(m_caName,"EnemyMaxHealth");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual ~CEnemyMaxHealth(){}

	virtual float	ffGetValue();
	virtual u32		dwfGetDiscreteValue(u32 dwDiscretizationValue = 1);
};

class CEquipmentType : public CBaseFunction {
public:
	typedef CBaseFunction inherited;

	CEquipmentType()
	{
		m_fMinResultValue = 1.0;
		m_fMaxResultValue = 5.0;
		strcat(m_caName,"EquipmentType");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual ~CEquipmentType(){}

	virtual float	ffGetValue();
};

class CItemDeterioration : public CBaseFunction {
public:
	typedef CBaseFunction inherited;

	CItemDeterioration()
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 100.0;
		strcat(m_caName,"ItemDeterioration");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual ~CItemDeterioration(){}

	virtual float	ffGetValue();
};

class CEquipmentPreference : public CBaseFunction {
public:
	typedef CBaseFunction inherited;

	CEquipmentPreference()
	{
		m_fMinResultValue = 1.0;
		m_fMaxResultValue = 3.0;
		strcat(m_caName,"EquipmentPreference");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual ~CEquipmentPreference(){}

	virtual float	ffGetValue();
};

class CMainWeaponType : public CBaseFunction {
public:
	typedef CBaseFunction inherited;

	CMainWeaponType()
	{
		m_fMinResultValue = 1.0;
		m_fMaxResultValue = 4.0;
		strcat(m_caName,"MainWeaponType");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual ~CMainWeaponType(){}

	virtual float	ffGetValue();
};

class CMainWeaponPreference : public CBaseFunction {
public:
	typedef CBaseFunction inherited;

	CMainWeaponPreference()
	{
		m_fMinResultValue = 1.0;
		m_fMaxResultValue = 3.0;
		strcat(m_caName,"MainWeaponPreference");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual ~CMainWeaponPreference(){}

	virtual float	ffGetValue();
};

class CItemValue : public CBaseFunction {
public:
	typedef CBaseFunction inherited;

	CItemValue()
	{
		m_fMinResultValue = 100.0;
		m_fMaxResultValue = 2000.0;
		strcat(m_caName,"ItemValue");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual ~CItemValue(){}

	virtual float	ffGetValue();
};

class CWeaponAmmoCount : public CBaseFunction {
public:
	typedef CBaseFunction inherited;

	CWeaponAmmoCount()
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 10.0;
		strcat(m_caName,"WeaponAmmoCount");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual ~CWeaponAmmoCount(){}

	virtual float	ffGetValue();
	virtual u32		dwfGetDiscreteValue(u32 dwDiscretizationValue = 1);
};

class CEnemyAnomalyType : public CBaseFunction {
public:
	typedef CBaseFunction inherited;

	CEnemyAnomalyType()
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 7.0;
		strcat(m_caName,"EnemyAnomalyType");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual ~CEnemyAnomalyType(){}

	virtual float	ffGetValue();
};

class CDetectorType : public CBaseFunction {
public:
	typedef CBaseFunction inherited;

	CDetectorType()
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 2.0;
		strcat(m_caName,"DetectorType");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual ~CDetectorType(){}

	virtual float	ffGetValue();
};

class CEnemyDistanceToGraphPoint : public CBaseFunction {
public:
	typedef CBaseFunction inherited;

	CEnemyDistanceToGraphPoint()
	{
		m_fMinResultValue = 0.0;
		m_fMaxResultValue = 4.0;
		strcat(m_caName,"EnemyDistanceToGraphPoint");
		OUT_MESSAGE("* Evaluation function \"%s\" is successfully initalized",m_caName);
	};

	virtual ~CEnemyDistanceToGraphPoint(){}

	virtual float	ffGetValue();
};
