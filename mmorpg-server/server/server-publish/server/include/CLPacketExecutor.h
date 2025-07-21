#ifndef _CL_PACKET_EXECUTOR_H_
#define _CL_PACKET_EXECUTOR_H_

#include <AvalonDefine.h>
#include <AvalonPacket.h>

#include <CLNetworkPerformance.h>

/**
 *@brief ��Ϣ������
 */
class CLPacketFunction
{
public:
	virtual ~CLPacketFunction(){}

	virtual void operator()(Avalon::Packet* packet) const = 0;
};


/**
 *@brief ��Ϣ������
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
	 *@brief ����Ϣ������
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
	 *@brief ����һ����Ϣִ�к���                                                                     
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
	 *@brief ִ��һ����Ϣ Ҳ���Ǹ���Э��id �ҵ���Э��Ĵ��������д���
	 */
	bool ExecutePacket(Avalon::Packet* packet) const
	{
		auto startTime = CLNetworkPerfMgr::Instance()->GetUSecsNow();
		CLPacketFunction* func = FindFunction(packet->GetID());
		if(func == NULL) return false;

		(*func)(packet);

		// ����ͳ��
		auto endTime = CLNetworkPerfMgr::Instance()->GetUSecsNow();
		CLNetworkPerfMgr::Instance()->OnProcessPacket(packet->GetConnID(), packet->GetID(), endTime - startTime);
		return true;
	}

private:
	//��Ϣ�������б�
	FunctionMap m_Functions;
};



/**
 *@brief ��������Ϣ������
 */
template<typename T>
class CLParamPacketFunction
{
public:
	virtual ~CLParamPacketFunction(){}

	virtual void operator()(Avalon::Packet* packet,T param) const = 0;
};

/**
 *@brief ��������Ϣ������
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
	 *@brief ��һ����Ϣ������
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
	 *@brief ִ��һ����Ϣ
	 */
	bool ExecutePacket(Avalon::Packet* packet,T param) const
	{
		auto startTime = CLNetworkPerfMgr::Instance()->GetUSecsNow();
		typename FunctionMap::const_iterator iter = m_Functions.find(packet->GetID());
		if(iter != m_Functions.end())
		{
			(*iter->second)(packet,param);

			// ����ͳ��
			auto endTime = CLNetworkPerfMgr::Instance()->GetUSecsNow();
			CLNetworkPerfMgr::Instance()->OnProcessPacket(packet->GetConnID(), packet->GetID(), endTime - startTime);
			return true;
		}
		return false;
	}

private:
	//��Ϣ�������б�
	FunctionMap m_Functions;
};

#endif
