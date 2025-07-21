#ifndef _CL_RL_CHANNELS_H_
#define _CL_RL_CHANNELS_H_
#include <AvalonSingleton.h>
#include <CLDefine.h>
#include "CLPacketExecutor.h"
//#include <AvalonCodeQueue.h>

namespace Avalon
{
	class CShmMemory;
	class CCodeQueue;
	class PropertySet;
	class Packet;
}

class CLPacketFunction;

class RLChannels : public Avalon::Singleton<RLChannels>
{
public:
	typedef HashMap<Avalon::UInt32, CLPacketFunction*> RLFunctionMap;
public:
	RLChannels();
	~RLChannels();

	bool Init(Avalon::PropertySet* prop);

	//send to client
	Int32 send_data(char* data, size_t size);

	//recv from client
	Int32 recv_data(char* data, size_t& size);

	//检查channel中的数据情况
	void OnTick(const Avalon::Time& time);
	//
	void OnQuit();

	//注册消息
	void RegisterProtocols();

protected:
	void process_packets();
	void check_channels(const Avalon::Time& now);

	//
	void RegisterProtocol(UInt32 id, CLPacketFunction* func){ /*m_Executor.Bind(id, func);*/ }

	/**
	*@brief 执行一个消息包
	*/
	bool ExecutePacket(ObjID_t roleId, Avalon::Packet* packet);

	//解包生成Avalon::Packet
	bool decode_to_packet(char* buff, size_t size, Avalon::Packet*& packet, ObjID_t& roleId);
protected:

	bool make_channels(Avalon::PropertySet* prop);


private:
	Avalon::CCodeQueue* m_pstCSChannel;
	Avalon::CCodeQueue* m_pstSCChannel;

	//
	//RLFunctionMap m_Executor;
    CLParamPacketExecutor<ObjID_t> m_Executor;
};

extern Avalon::CShmMemory* g_pstCSChannelShm;
extern Avalon::CShmMemory* g_pstSCChannelShm;

#endif
