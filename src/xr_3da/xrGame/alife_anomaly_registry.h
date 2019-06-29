////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_anomaly_registry.h
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife anomaly registry
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_known_anomaly.h"
#include "alife_space.h"

class CALifeAnomalyRegistry {
public:
	typedef ALife::ANOMALY_P_VECTOR_VECTOR	ANOMALIES;
	typedef ALife::ANOMALY_P_VECTOR_SVECTOR CROSS_ANOMALIES;

protected:
	ANOMALIES						m_anomalies;
	CROSS_ANOMALIES					m_anomaly_cross_table;

public:
									CALifeAnomalyRegistry	(LPCSTR section);
	virtual							~CALifeAnomalyRegistry	();
	virtual	void					save					(IWriter &memory_stream);
	virtual	void					load					(IReader &file_stream);
			void					clear					();
	IC		void					add						(CALifeKnownAnomaly *anomaly);
	IC		const CROSS_ANOMALIES	&cross					() const;
};

#include "alife_anomaly_registry_inline.h"