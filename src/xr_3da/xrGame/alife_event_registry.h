////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_event_registry.h
//	Created 	: 15.01.2003
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife event registry
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_event.h"
#include "object_broker.h"

class CALifeEventRegistry {
protected:
	template <typename _predicate>
	struct CEventLoader : public object_loader::detail::CEmptyPredicate {
		using object_loader::detail::CEmptyPredicate::operator();

		const _predicate			&m_predicate;

		IC							CEventLoader	(const _predicate &predicate) :
										m_predicate		(predicate)
		{
		}

		template <typename T1, typename T2>
		IC	bool operator()	(T1 &data, const T2 &value, bool first) const {return(!first);}
		
		template <typename T1, typename T2>
		IC	void operator()	(std::pair<T1,T2> &data) const
		{
			const_cast<object_type_traits::remove_const<T1>::type&>(data.first) = data.second->m_tEventID;
			m_predicate				(data.second);
		}

		IC CEventLoader<_predicate> &operator=		(const CEventLoader<_predicate> &T)
		{
			*this					= T;
			return					(*this);
		}
	};

protected:
	ALife::_EVENT_ID				m_id;
	ALife::EVENT_MAP				m_events;

public:
	IC								CALifeEventRegistry		(LPCSTR section);
	virtual							~CALifeEventRegistry	();
	virtual	void					save					(IWriter &memory_stream);
	template <typename _predicate>
	IC		void					load					(IReader &file_stream, const _predicate &predicate);
	IC		void					add						(CALifeEvent	*event);
	IC		void					remove					(const ALife::_EVENT_ID	&event_id, bool no_assert = false);
	IC		CALifeEvent				*event					(const ALife::_EVENT_ID	&event_id, bool no_assert = false) const;
	IC		const ALife::EVENT_MAP	&events					() const;
};

#include "alife_event_registry_inline.h"