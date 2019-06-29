////////////////////////////////////////////////////////////////////////////
//	Module 		: enemy_location_predictor.cpp
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Enemy location predictor
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "enemy_location_predictor.h"

CEnemyLocationPredictor::CEnemyLocationPredictor		()
{
}

CEnemyLocationPredictor::~CEnemyLocationPredictor		()
{
}

void CEnemyLocationPredictor::reinit					(CRestrictedObject *object)
{
}

void CEnemyLocationPredictor::predict_enemy_locations	()
{
}

bool CEnemyLocationPredictor::enemy_prediction_actual	() const
{
	return				(true);
}

void CEnemyLocationPredictor::select_enemy_location		()
{
}

bool CEnemyLocationPredictor::prediction_failed			() const
{
	return				(false);
}
