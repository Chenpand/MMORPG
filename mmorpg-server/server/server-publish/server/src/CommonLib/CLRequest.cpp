#include "CLRequest.h"

#include <CLGrid.h>


RequestMgr::RequestMgr()
{
}

RequestMgr::~RequestMgr()
{
	for(Int32 i = 0; i < REQUEST_MAX; ++i)
	{
		m_Requests[i].clear();
	}
}

void RequestMgr::AddRequest(Int32 type, ObjID_t target, UInt32 validTime, UInt32 param)
{
	if(type >= REQUEST_MAX) return;
	
	RequestMap& requests = m_Requests[type];

	RequestMap::iterator iter = requests.find(target);
	if(iter != requests.end())
	{
		iter->second.leftTime = validTime;
	}
	else
	{
		Request request;
		request.target = target;
		request.leftTime = validTime;
		request.param = param;
	
		requests.insert(std::make_pair(target, request));
	
		//超时数量限制，删除最早请求
		if(requests.size() > MAX_REQUEST_NUM)
		{
			UInt16 leftTime = UInt16(-1);
			RequestMap::iterator fistRequest = requests.end();
			for(RequestMap::iterator iter = requests.begin();
				iter != requests.end(); ++iter)
			{
				if(iter->second.leftTime < leftTime)
				{
					leftTime = iter->second.leftTime;
					fistRequest = iter;
				}
			}
			if(fistRequest != requests.end())
			{
				requests.erase(fistRequest);
			}
		}
	}
}

bool RequestMgr::HasRequest(Int32 type, ObjID_t target)
{
	if(type >= REQUEST_MAX) return false;

	RequestMap& requests = m_Requests[type];
	return (requests.find(target) != requests.end());
}

void RequestMgr::RemoveRequest(Int32 type, ObjID_t target)
{	
	if(type >= REQUEST_MAX) return;

	m_Requests[type].erase(target);
}

UInt32 RequestMgr::GetRequestLeftTime(Int32 type, ObjID_t target)
{
	if (type >= REQUEST_MAX)
	{
		return 0;
	}

	RequestMap& requests = m_Requests[type];
	auto itr = requests.find(target);
	if (itr == requests.end())
	{
		return 0;
	}

	auto request = itr->second;
	return request.leftTime;
}

void RequestMgr::ClearRequests(Int32 type)
{
	if(type >= REQUEST_MAX) return;

	m_Requests[type].clear();
}

UInt32 RequestMgr::GetRequestParam(Int32 type, ObjID_t target)
{
	if(type >= REQUEST_MAX) return 0;

	RequestMap& requests = m_Requests[type];
	RequestMap::iterator	iter = requests.find(target);
	if(iter == requests.end())
		return 0;

	return iter->second.param;
}

void RequestMgr::OnTick(const Avalon::Time& now)
{
	for(Int32 i = 0; i < REQUEST_MAX; ++i)
	{
		RequestMap& requests = m_Requests[i];
		for(RequestMap::iterator iter = requests.begin();
			iter != requests.end();)
		{
			Request& request = iter->second;
			if(request.leftTime > 0) --request.leftTime;

			if(request.leftTime == 0) requests.erase(iter++); 
			else ++iter; 
		}
	}
}




