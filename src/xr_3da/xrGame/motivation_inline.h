////////////////////////////////////////////////////////////////////////////
//	Module 		: motivation_inline.h
//	Created 	: 22.03.2004
//  Modified 	: 22.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Motivation class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

#define TEMPLATE_SPECIALIZATION template <typename _object_type>
#define CSMotivation			CMotivation<_object_type>

TEMPLATE_SPECIALIZATION
IC	CSMotivation::CMotivation		()
{
	m_object			= 0;
}

TEMPLATE_SPECIALIZATION
CSMotivation::~CMotivation			()
{
}

TEMPLATE_SPECIALIZATION
void CSMotivation::setup			(_object_type *object)
{
	m_object			= object;
}

TEMPLATE_SPECIALIZATION
float CSMotivation::evaluate		(u32 sub_motivation_id)
{
	return				(0.f);
}

TEMPLATE_SPECIALIZATION
IC	_object_type &CSMotivation::object	() const
{
	VERIFY				(m_object);
	return				(*m_object);
}

#undef TEMPLATE_SPECIALIZATION
#undef CSMotivation