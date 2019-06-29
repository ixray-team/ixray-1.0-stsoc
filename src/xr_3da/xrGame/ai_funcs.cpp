////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_funcs.cpp
//	Created 	: 25.03.2002
//  Modified 	: 11.10.2002
//	Author		: Dmitriy Iassenev
//	Description : Pattern functions class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_funcs.h"
#include "ai_primary_funcs.h"
#include "ai_space.h"
#include "ai_alife_templates.h"
#include "ai_console.h"

CPatternFunction::CPatternFunction() : CBaseFunction()
{
	m_dwPatternCount		= 0;
	m_dwVariableCount		= 0;
	m_dwParameterCount		= 0;
	m_dwaVariableTypes		= 0;
	m_dwaAtomicFeatureRange = 0;
	m_dwaPatternIndexes		= 0;
	m_tpPatterns			= 0;
	m_faParameters			= 0;
	m_dwaVariableValues		= 0;
}

CPatternFunction::CPatternFunction(LPCSTR caFileName, CAI_DDD *tpAI_DDD) : CBaseFunction()
{
	m_dwPatternCount		= 0;
	m_dwVariableCount		= 0;
	m_dwParameterCount		= 0;
	m_dwaVariableTypes		= 0;
	m_dwaAtomicFeatureRange = 0;
	m_dwaPatternIndexes		= 0;
	m_tpPatterns			= 0;
	m_faParameters			= 0;
	m_dwaVariableValues		= 0;
	vfLoadEF				(caFileName, tpAI_DDD);
}

CPatternFunction::~CPatternFunction()
{
	xr_free			(m_dwaVariableTypes);
	xr_free			(m_dwaAtomicFeatureRange);
	xr_free			(m_dwaPatternIndexes);
	for (u32 i=0; i<m_dwPatternCount; i++)
		xr_free		(m_tpPatterns[i].dwaVariableIndexes);
	xr_free			(m_tpPatterns);
	xr_free			(m_faParameters);
	xr_free			(m_dwaVariableValues);
}

void CPatternFunction::vfLoadEF(LPCSTR caFileName, CAI_DDD *tpAI_DDD)
{
	string256		caPath;
	if (!FS.exist(caPath,"$game_ai$",caFileName)) {
		Msg			("! Evaluation function : File not found \"%s\"",caPath);
		R_ASSERT	(false);
		return;
	}
	
	IReader			*F = FS.r_open(caPath);
	F->r			(&m_tEFHeader,sizeof(SEFHeader));

	if (m_tEFHeader.dwBuilderVersion != EFC_VERSION) {
		FS.r_close	(F);
		Msg			("! Evaluation function (%s) : Not supported version of the Evaluation Function Contructor",caPath);
		R_ASSERT	(false);
		return;
	}

	F->r			(&m_dwVariableCount,sizeof(m_dwVariableCount));
	m_dwaAtomicFeatureRange = (u32 *)xr_malloc(m_dwVariableCount*sizeof(u32));
	ZeroMemory		(m_dwaAtomicFeatureRange,m_dwVariableCount*sizeof(u32));
	u32				*m_dwaAtomicIndexes = (u32 *)xr_malloc(m_dwVariableCount*sizeof(u32));
	ZeroMemory		(m_dwaAtomicIndexes,m_dwVariableCount*sizeof(u32));

	for (u32 i=0; i<m_dwVariableCount; i++) {
		F->r(m_dwaAtomicFeatureRange + i,sizeof(u32));
		if (i)
			m_dwaAtomicIndexes[i] = m_dwaAtomicIndexes[i-1] + m_dwaAtomicFeatureRange[i-1];
	}

	m_dwaVariableTypes = (u32 *)xr_malloc(m_dwVariableCount*sizeof(u32));
	F->r			(m_dwaVariableTypes,m_dwVariableCount*sizeof(u32));

	F->r			(&m_dwFunctionType,sizeof(u32));

	F->r			(&m_fMinResultValue,sizeof(float));
	F->r			(&m_fMaxResultValue,sizeof(float));

	F->r			(&m_dwPatternCount,sizeof(m_dwPatternCount));
	m_tpPatterns	= (SPattern *)xr_malloc(m_dwPatternCount*sizeof(SPattern));
	m_dwaPatternIndexes = (u32 *)xr_malloc(m_dwPatternCount*sizeof(u32));
	ZeroMemory		(m_dwaPatternIndexes,m_dwPatternCount*sizeof(u32));
	m_dwParameterCount = 0;
	for ( i=0; i<m_dwPatternCount; i++) {
		if (i)
			m_dwaPatternIndexes[i] = m_dwParameterCount;
		F->r		(&(m_tpPatterns[i].dwCardinality),sizeof(m_tpPatterns[i].dwCardinality));
		m_tpPatterns[i].dwaVariableIndexes = (u32 *)xr_malloc(m_tpPatterns[i].dwCardinality*sizeof(u32));
		F->r		(m_tpPatterns[i].dwaVariableIndexes,m_tpPatterns[i].dwCardinality*sizeof(u32));
		u32			m_dwComplexity = 1;
		for (int j=0; j<(int)m_tpPatterns[i].dwCardinality; j++)
			m_dwComplexity *= m_dwaAtomicFeatureRange[m_tpPatterns[i].dwaVariableIndexes[j]];
		m_dwParameterCount += m_dwComplexity;
	}
	
	m_faParameters	= (float *)xr_malloc(m_dwParameterCount*sizeof(float));
	F->r			(m_faParameters,m_dwParameterCount*sizeof(float));
	FS.r_close		(F);

	m_dwaVariableValues = (u32 *)xr_malloc(m_dwVariableCount*sizeof(u32));
	
	xr_free			(m_dwaAtomicIndexes);
    
	tpAI_DDD->m_fpaBaseFunctions[m_dwFunctionType] = this;
	
	_splitpath		(caPath,0,0,m_caName,0);

	// Msg			("* Evaluation function \"%s\" is successfully loaded",m_caName);
}

float CPatternFunction::ffEvaluate()
{
	float fResult = 0.0;
	for (u32 i=0; i<m_dwPatternCount; i++)
		fResult += m_faParameters[dwfGetPatternIndex(m_dwaVariableValues,i)];
	return(fResult);
}

float CPatternFunction::ffGetValue()
{
	if (bfCheckForCachedResult())
		return(m_fLastValue);

	for (u32 i=0; i<m_dwVariableCount; i++)
		m_dwaVariableValues[i] = getAI().m_fpaBaseFunctions[m_dwaVariableTypes[i]]->dwfGetDiscreteValue(m_dwaAtomicFeatureRange[i]);

	
#ifdef DEBUG	
	if (psAI_Flags.test(aiFuncs)) {
		m_fLastValue = ffEvaluate();
		char caString[256];
		int j = sprintf(caString,"%32s (",m_caName);
		for ( i=0; i<m_dwVariableCount; i++)
			j += sprintf(caString + j," %3d",m_dwaVariableValues[i] + 1);
		sprintf(caString + j,") = %7.2f",m_fLastValue);
		Msg("- %s",caString);
		return(m_fLastValue);
	}
#endif
	
	return(m_fLastValue = ffEvaluate());
}

CAI_DDD::CAI_DDD()
{	
	m_tpGameObject						= 0;
	m_tpCurrentMember					= 0;
	m_tpCurrentEnemy					= 0;
	m_tpCurrentALifeObject				= 0;
	m_tpCurrentALifeMember				= 0;
	m_tpCurrentALifeEnemy				= 0;
	m_tpALife							= 0;

	Memory.mem_fill						(m_fpaBaseFunctions,0,sizeof(CBaseFunction*)*AI_MAX_EVALUATION_FUNCTION_COUNT);
	
	m_fpaBaseFunctions[0]				= m_pfDistance				= xr_new<CDistanceFunction>				();
	m_fpaBaseFunctions[1]				= m_pfGraphPointType0		= xr_new<CGraphPointType0>				();
	m_fpaBaseFunctions[2]				= m_pfEquipmentType			= xr_new<CEquipmentType>				();
	m_fpaBaseFunctions[3]				= m_pfItemDeterioration		= xr_new<CItemDeterioration>			();
	m_fpaBaseFunctions[4]				= m_pfEquipmentPreference	= xr_new<CEquipmentPreference>			();
	m_fpaBaseFunctions[5]				= m_pfMainWeaponType		= xr_new<CMainWeaponType>				();
	m_fpaBaseFunctions[6]				= m_pfMainWeaponPreference	= xr_new<CMainWeaponPreference>			();
	m_fpaBaseFunctions[7]				= m_pfItemValue				= xr_new<CItemValue>					();
	m_fpaBaseFunctions[8]				= m_pfWeaponAmmoCount		= xr_new<CWeaponAmmoCount>				();
	m_fpaBaseFunctions[9]				= m_pfDetectorType			= xr_new<CDetectorType>					();
										  
	m_fpaBaseFunctions[21]				= m_pfPersonalHealth		= xr_new<CPersonalHealthFunction>		();
	m_fpaBaseFunctions[22]				= m_pfPersonalMorale		= xr_new<CPersonalMoraleFunction>		();			
	m_fpaBaseFunctions[23]				= m_pfPersonalCreatureType	= xr_new<CPersonalCreatureTypeFunction>	();	
	m_fpaBaseFunctions[24]				= m_pfPersonalWeaponType	= xr_new<CPersonalWeaponTypeFunction>	();		
	m_fpaBaseFunctions[25]				= m_pfPersonalAccuracy		= xr_new<CPersonalAccuracyFunction>		();		
	m_fpaBaseFunctions[26]				= m_pfPersonalIntelligence	= xr_new<CPersonalIntelligenceFunction>	();	
	m_fpaBaseFunctions[27]				= m_pfPersonalRelation		= xr_new<CPersonalRelationFunction>		();		
	m_fpaBaseFunctions[28]				= m_pfPersonalGreed			= xr_new<CPersonalGreedFunction>		();			
	m_fpaBaseFunctions[29]				= m_pfPersonalAggressiveness= xr_new<CPersonalAggressivenessFunction>();	
	m_fpaBaseFunctions[30]				= m_pfPersonalEyeRange		= xr_new<CPersonalEyeRange>				();	
	m_fpaBaseFunctions[31]				= m_pfPersonalMaxHealth		= xr_new<CPersonalMaxHealth>			();	
										  
	m_fpaBaseFunctions[41]				= m_pfEnemyHealth			= xr_new<CEnemyHealthFunction>			();			
	m_fpaBaseFunctions[42]				= m_pfEnemyCreatureType		= xr_new<CEnemyCreatureTypeFunction>	();		
	m_fpaBaseFunctions[43]				= m_pfEnemyWeaponType		= xr_new<CEnemyWeaponTypeFunction>		();		
	m_fpaBaseFunctions[44]				= m_pfEnemyEquipmentCost	= xr_new<CEnemyEquipmentCostFunction>	();		
	m_fpaBaseFunctions[45]				= m_pfEnemyRukzakWeight		= xr_new<CEnemyRukzakWeightFunction>	();		
	m_fpaBaseFunctions[46]				= m_pfEnemyAnomality		= xr_new<CEnemyAnomalityFunction>		();			
	m_fpaBaseFunctions[47]				= m_pfEnemyEyeRange			= xr_new<CEnemyEyeRange>				();	
	m_fpaBaseFunctions[48]				= m_pfEnemyMaxHealth		= xr_new<CEnemyMaxHealth>				();	
	m_fpaBaseFunctions[49]				= m_pfEnemyAnomalyType		= xr_new<CEnemyAnomalyType>				();	
	m_fpaBaseFunctions[50]				= m_pfEnemyDistanceToGraphPoint= xr_new<CEnemyDistanceToGraphPoint>	();	

	m_pfWeaponEffectiveness				= xr_new<CPatternFunction>	("common\\WeaponEffectiveness.efd",		this);
	m_pfCreatureEffectiveness			= xr_new<CPatternFunction>	("common\\CreatureEffectiveness.efd",	this);
	m_pfIntellectCreatureEffectiveness	= xr_new<CPatternFunction>	("common\\IntCreatureEffectiveness.efd",this);
	m_pfAccuracyWeaponEffectiveness		= xr_new<CPatternFunction>	("common\\AccWeaponEffectiveness.efd",	this);
	m_pfFinalCreatureEffectiveness		= xr_new<CPatternFunction>	("common\\FinCreatureEffectiveness.efd",this);
	m_pfVictoryProbability				= xr_new<CPatternFunction>	("common\\VictoryProbability.efd",		this);
	m_pfEntityCost						= xr_new<CPatternFunction>	("common\\EntityCost.efd",				this);
	m_pfExpediency						= xr_new<CPatternFunction>	("common\\Expediency.efd",				this);
	m_pfSurgeDeathProbability			= xr_new<CPatternFunction>	("common\\SurgeDeathProbability.efd",	this);
	m_pfEquipmentValue					= xr_new<CPatternFunction>	("common\\EquipmentValue.efd",			this);
	m_pfMainWeaponValue					= xr_new<CPatternFunction>	("common\\MainWeaponValue.efd",			this);
	m_pfSmallWeaponValue				= xr_new<CPatternFunction>	("common\\SmallWeaponValue.efd",		this);
	m_pfTerrainType						= xr_new<CPatternFunction>	("alife\\TerrainType.efd",				this);
	m_pfWeaponAttackTimes				= xr_new<CPatternFunction>	("alife\\WeaponAttackTimes.efd",		this);
	m_pfWeaponSuccessProbability		= xr_new<CPatternFunction>	("alife\\WeaponSuccessProbability.efd",	this);
	m_pfEnemyDetectability				= xr_new<CPatternFunction>	("alife\\EnemyDetectability.efd",		this);
	m_pfEnemyDetectProbability			= xr_new<CPatternFunction>	("alife\\EnemyDetectProbability.efd",	this);
	m_pfEnemyRetreatProbability			= xr_new<CPatternFunction>	("alife\\EnemyRetreatProbability.efd",	this);
	m_pfAnomalyDetectProbability		= xr_new<CPatternFunction>	("alife\\AnomalyDetectProbability.efd",	this);
	m_pfAnomalyInteractProbability		= xr_new<CPatternFunction>	("alife\\AnomalyInteractProbability.efd",this);
	m_pfAnomalyRetreatProbability		= xr_new<CPatternFunction>	("alife\\AnomalyRetreatProbability.efd",this);
	m_pfBirthPercentage					= xr_new<CPatternFunction>	("alife\\BirthPercentage.efd",			this);
	m_pfBirthProbability				= xr_new<CPatternFunction>	("alife\\BirthProbability.efd",			this);
	m_pfBirthSpeed						= xr_new<CPatternFunction>	("alife\\BirthSpeed.efd",				this);
}

CAI_DDD::~CAI_DDD()
{
	for (int i=0; i<AI_MAX_EVALUATION_FUNCTION_COUNT; ++i)
		xr_delete						(m_fpaBaseFunctions[i]);
}