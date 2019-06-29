////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_news_registry_inline.h
//	Created 	: 15.01.2003
//  Modified 	: 13.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife news registry inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	ALife::_NEWS_ID CALifeNewsRegistry::add		(const CALifeNews &news)
{
	CALifeNews						*_news = xr_new<CALifeNews>(news);
	_news->m_news_id				= m_last_id++;
	m_news.insert					(std::make_pair(_news->m_news_id,_news));
	return							(_news->m_news_id);
}

IC	void CALifeNewsRegistry::remove		(const ALife::_NEWS_ID &news_id)
{
	ALife::NEWS_REGISTRY::iterator	I = m_news.find(news_id);
	VERIFY							(news().end() != I);
	m_news.erase					(I);
}

IC	const ALife::NEWS_REGISTRY &CALifeNewsRegistry::news	() const
{
	return							(m_news);
}

IC	const CALifeNews *CALifeNewsRegistry::news	(const ALife::_NEWS_ID &news_id) const
{
	ALife::NEWS_REGISTRY::const_iterator	I = news().find(news_id);
	if (news().end() == I)
		return						(0);
	return							((*I).second);
}
