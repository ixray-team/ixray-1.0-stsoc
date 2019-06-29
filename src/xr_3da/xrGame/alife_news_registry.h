////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_news_registry.h
//	Created 	: 15.01.2003
//  Modified 	: 13.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife news registry
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_news.h"
#include "alife_space.h"

class CALifeNewsRegistry {
protected:
	ALife::NEWS_REGISTRY				m_news;
	ALife::_NEWS_ID						m_last_id;

public:
										CALifeNewsRegistry	(LPCSTR section);
	virtual								~CALifeNewsRegistry	();
	virtual	void						save				(IWriter &memory_stream);
	virtual	void						load				(IReader &file_stream);
			void						clear				();
	IC		ALife::_NEWS_ID				add					(const CALifeNews &news);
	IC		void						remove				(const ALife::_NEWS_ID &news_id);
	IC		const ALife::NEWS_REGISTRY	&news				() const;
	IC		const CALifeNews			*news				(const ALife::_NEWS_ID &news_id) const;
};

#include "alife_news_registry_inline.h"