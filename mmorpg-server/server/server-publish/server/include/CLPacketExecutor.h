#ifndef _CL_PACKET_EXECUTOR_H_
#define _CL_PACKET_EXECUTOR_H_

#include <AvalonDefine.h>
#include <AvalonPacket.h>

#include <CLNetworkPerformance.h>

/**
 *@brief 消息处理函数
 */
class CLPacketFunction
{
public:
	virtual ~CLPacketFunction(){}

	virtual void operator()(Avalon::Packet* packet) const = 0;
};


/**
 *@brief 消息处理器
 */
class CLPacketExecutor
{
	typedef HashMap<Avalon::UInt32,CLPacketFunction*> FunctionMap;

public:
	CLPacketExecutor(){}
	~CLPacketExecutor()
	{
		for(FunctionMap::iterator iter = m_Functions.begin();iter != m_Functions.end();++iter)
		{
			delete iter->second;
		}
		m_Functions.clear();
	}

	/**
	 *@brief 绑定消息处理函数
	 */
	void Bind(Avalon::UInt32 id,CLPacketFunction* func)
	{
		FunctionMap::iterator iter = m_Functions.find(id);
		if(iter != m_Functions.end())
		{
			delete iter->second;
			iter->second = func;
		}
		else
		{
			m_Functions.insert(std::make_pair(id,func));
		}
	}

	/**
	 *@brief 查找一个消息执行函数                                                                     
	 */ 
	CLPacketFunction* FindFunction(Avalon::UInt32 id) const
	{
		FunctionMap::const_iterator iter = m_Functions.find(id);
		if(iter != m_Functions.end())
		{
			return iter->second;
		}
		return NULL;
	}

	/**
	 *@brief 执行一个消息 也就是根据协议id 找到该协议的处理函数进行处理
	 */
	bool ExecutePacket(Avalon::Packet* packet) const
	{
		auto startTime = CLNetworkPerfMgr::Instance()->GetUSecsNow();
		CLPacketFunction* func = FindFunction(packet->GetID());
		if(func == NULL) return false;

		(*func)(packet);

		// 性能统计
		auto endTime = CLNetworkPerfMgr::Instance()->GetUSecsNow();
		CLNetworkPerfMgr::Instance()->OnProcessPacket(packet->GetConnID(), packet->GetID(), endTime - startTime);
		return true;
	}

private:
	//消息处理函数列表
	FunctionMap m_Functions;
};



/**
 *@brief 带参数消息处理函数
 */
template<typename T>
class CLParamPacketFunction
{
public:
	virtual ~CLParamPacketFunction(){}

	virtual void operator()(Avalon::Packet* packet,T param) const = 0;
};

/**
 *@brief 带参数消息处理器
 */
template<typename T>
class CLParamPacketExecutor
{
	typedef HashMap<Avalon::UInt32,CLParamPacketFunction<T>* > FunctionMap;

public:
	CLParamPacketExecutor(){}
	~CLParamPacketExecutor()
	{
		for(typename FunctionMap::iterator iter = m_Functions.begin();iter != m_Functions.end();++iter)
		{
			delete iter->second;
		}
		m_Functions.clear();
	}

	/**
	 *@brief 绑定一个消息处理函数
	 */
	void Bind(Avalon::UInt32 id,CLParamPacketFunction<T>* func)
	{
		typename FunctionMap::iterator iter = m_Functions.find(id);
		if(iter != m_Functions.end())
		{
			delete iter->second;
			iter->second = func;
		}
		else
		{
			m_Functions.insert(std::make_pair(id,func));
		}
	}

	/**
	 *@brief 执行一个消息
	 */
	bool ExecutePacket(Avalon::Packet* packet,T param) const
	{
		auto startTime = CLNetworkPerfMgr::Instance()->GetUSecsNow();
		typename FunctionMap::const_iterator iter = m_Functions.find(packet->GetID());
		if(iter != m_Functions.end())
		{
			(*iter->second)(packet,param);

			// 性能统计
			auto endTime = CLNetworkPerfMgr::Instance()->GetUSecsNow();
			CLNetworkPerfMgr::Instance()->OnProcessPacket(packet->GetConnID(), packet->GetID(), endTime - startTime);
			return true;
		}
		return false;
	}

private:
	//消息处理函数列表
	FunctionMap m_Functions;
};

#endif
