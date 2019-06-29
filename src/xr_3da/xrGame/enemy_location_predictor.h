////////////////////////////////////////////////////////////////////////////
//	Module 		: enemy_location_predictor.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Enemy location predictor
////////////////////////////////////////////////////////////////////////////

#pragma once

class CRestrictedObject;

class CEnemyLocationPredictor {
public:
					CEnemyLocationPredictor			();
	virtual			~CEnemyLocationPredictor		();
	virtual void	reinit							(CRestrictedObject *object);
			void	predict_enemy_locations			();
			bool	enemy_prediction_actual			() const;
			void	select_enemy_location			();
			bool	prediction_failed				() const;
};