////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_task.cpp
//	Created 	: 02.06.2004
//  Modified 	: 02.06.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife task class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_task.h"

CALifeTask::~CALifeTask		()
{
}

void CALifeTask::load		(IReader &stream)
{
	stream.r				(&m_tTaskID,	sizeof(m_tTaskID));
	stream.r				(&m_tTimeID,	sizeof(m_tTimeID));
	stream.r				(&m_tCustomerID,sizeof(m_tCustomerID));
	m_fCost					= stream.r_float();
	stream.r				(&m_tTaskType,	sizeof(m_tTaskType));
	switch (m_tTaskType) {
		case ALife::eTaskTypeSearchForItemCL : {
			stream.r_stringZ(m_caSection,sizeof(m_caSection));
			stream.r		(&m_tLocationID,sizeof(m_tLocationID));
			break;
		}
		case ALife::eTaskTypeSearchForItemCG : {
			stream.r_stringZ(m_caSection,sizeof(m_caSection));
			stream.r		(&m_tGraphID,	sizeof(m_tGraphID));
			break;
		}
		case ALife::eTaskTypeSearchForItemOL : {
			stream.r		(&m_tObjectID,	sizeof(m_tObjectID));
			stream.r		(&m_tLocationID,sizeof(m_tLocationID));
			break;
		}
		case ALife::eTaskTypeSearchForItemOG : {
			stream.r		(&m_tObjectID,	sizeof(m_tObjectID));
			stream.r		(&m_tGraphID,	sizeof(m_tGraphID));
			break;
		}
		default : NODEFAULT;
	};
	stream.r				(&m_dwTryCount,sizeof(m_dwTryCount));
}

void CALifeTask::save		(IWriter &stream)
{
	stream.w				(&m_tTaskID,		sizeof(m_tTaskID));
	stream.w				(&m_tTimeID,		sizeof(m_tTimeID));
	stream.w				(&m_tCustomerID,	sizeof(m_tCustomerID));
	stream.w_float			(m_fCost);
	stream.w				(&m_tTaskType,		sizeof(m_tTaskType));
	switch (m_tTaskType) {
		case ALife::eTaskTypeSearchForItemCL : {
			stream.w_stringZ(m_caSection);
			stream.w		(&m_tLocationID,	sizeof(m_tLocationID));
			break;
		}
		case ALife::eTaskTypeSearchForItemCG : {
			stream.w_stringZ(m_caSection);
			stream.w		(&m_tGraphID,		sizeof(m_tGraphID));
			break;
		}
		case ALife::eTaskTypeSearchForItemOL : {
			stream.w		(&m_tObjectID,		sizeof(m_tObjectID));
			stream.w		(&m_tLocationID,	sizeof(m_tLocationID));
			break;
		}
		case ALife::eTaskTypeSearchForItemOG : {
			stream.w		(&m_tObjectID,		sizeof(m_tObjectID));
			stream.w		(&m_tGraphID,		sizeof(m_tGraphID));
			break;
		}
		default : NODEFAULT;
	}
	stream.w				(&m_dwTryCount,sizeof(m_dwTryCount));
}
