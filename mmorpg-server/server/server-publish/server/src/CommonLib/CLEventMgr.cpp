#include "CLEventMgr.h"


CLEventMgr::CLEventMgr()
{

}


CLEventMgr::~CLEventMgr()
{

}


void CLEventMgr::RegistEventHandler(EventType t, IEventHandler* h)
{
	if (!h)
	{
		return;
	}

	HandlerVec& vec = m_handlers[t];
	for (size_t i = 0; i < vec.size(); ++i)
	{
		if (vec[i] == h)
		{
			return;
		}
	}

	vec.push_back(h);
}


void CLEventMgr::UnRegistEventHandler(EventType t, IEventHandler* h)
{
	if (!h)
	{
		return;
	}

	auto mapItr = m_handlers.find(t);
	if (mapItr == m_handlers.end())
	{
		return;
	}

	HandlerVec& vec = mapItr->second;
	HandlerVec::iterator itr = vec.begin();
	for (; itr != vec.end(); ++itr)
	{
		IEventHandler* tmpHand = *itr;
		if (!tmpHand)	continue;
		
		if (tmpHand == h)
		{
			CL_SAFE_DELETE(tmpHand);
			vec.erase(itr);
			break;
		}
	}

	if (vec.size() == 0)
	{
		m_handlers.erase(mapItr);
	}
}


void CLEventMgr::SendEvent(EventType t, IEventParam* param)
{
	if (!param)	return;
	
	auto mapItr = m_handlers.find(t);
	if (mapItr == m_handlers.end())
	{
		return;
	}

	HandlerVec& vec = mapItr->second;
	HandlerVec::iterator itr = vec.begin();
	for (; itr != vec.end(); ++itr)
	{
		(*itr)->HandleEvent(param);
	}
}

void CLEventMgr::RegistPlayerEventHandler(ObjID_t uid, EventType t, IEventHandler* h)
{
	if (!h)	return;

	HandlerMap& handlers = m_playerHandlers[uid];
	
	HandlerVec& vec = handlers[t];
	for (size_t i = 0; i < vec.size(); ++i)
	{
		if (*vec[i] == h)
		{
			CL_SAFE_DELETE(h);
			return;
		}
	}

	vec.push_back(h);
}

void CLEventMgr::UnRegistPlayerEventHandler(ObjID_t uid, EventType t, IEventHandler* h)
{
	if (!h)	return;

	auto playerMapItr = m_playerHandlers.find(uid);
	if (playerMapItr == m_playerHandlers.end())
	{
		return;
	}

	auto mapItr = playerMapItr->second.find(t);
	if (mapItr == playerMapItr->second.end())
	{
		return;
	}

	HandlerVec& vec = mapItr->second;
	HandlerVec::iterator itr = vec.begin();
	for (; itr != vec.end(); ++itr)
	{
		IEventHandler* tmpHand = *itr;
		if (!tmpHand)	continue;
		
		if (*tmpHand == h)
		{
			CL_SAFE_DELETE(tmpHand);
			vec.erase(itr);
			break;
		}
	}
}

void CLEventMgr::SendPlayerEvent(ObjID_t uid, EventType t, IEventParam* param)
{
	auto playerMapItr = m_playerHandlers.find(uid);
	if (playerMapItr == m_playerHandlers.end())
	{
		return;
	}

	auto mapItr = playerMapItr->second.find(t);
	if (mapItr == playerMapItr->second.end())
	{
		return;
	}

	if (param)
	{
		param->_eventType = t;
	}

	HandlerVec& vec = mapItr->second;
	HandlerVec::iterator itr = vec.begin();
	for (; itr != vec.end(); ++itr)
	{
		(*itr)->HandleEvent(param);
	}
}

void CLEventMgr::RemovePlayerEventsHandler(ObjID_t uid)
{
	auto playerMapItr = m_playerHandlers.find(uid);
	if (playerMapItr == m_playerHandlers.end())
	{
		return;
	}

	HandlerMap& handlerMap = playerMapItr->second;
	for (auto handlerItr = handlerMap.begin(); handlerItr != handlerMap.end(); ++handlerItr)
	{
		HandlerVec& handlers = handlerItr->second;
		for (auto funcItr = handlers.begin(); funcItr != handlers.end(); ++funcItr)
		{
			IEventHandler* funcPtr = *funcItr;
			CL_SAFE_DELETE(funcPtr);
		}

		handlers.clear();
	}

	handlerMap.clear();
	m_playerHandlers.erase(playerMapItr);
}

