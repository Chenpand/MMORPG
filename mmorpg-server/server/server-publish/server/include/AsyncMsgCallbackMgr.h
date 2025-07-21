#ifndef __ASYNC_MSG_CALLBACK_MGR_H__
#define __ASYNC_MSG_CALLBACK_MGR_H__

#include <CLDefine.h>
#include <AvalonDefine.h>
#include <AvalonSingleton.h>
#include <AvalonPacket.h>
#include <CLPacketExecutor.h>
#include <CLSysProtocol.h>
#include "AsyncMsgCallback.h"

class AsyncMsgCallbackMgr : public Avalon::Singleton<AsyncMsgCallbackMgr>
{
    typedef HashMap<ObjID_t, AsyncMsgCallback*> MapMsgCallback;
public:
    AsyncMsgCallbackMgr();
    ~AsyncMsgCallbackMgr();

    void Tick(const Avalon::Time& now);

public:
    /**
    *@brief 新增异步消息
    */
    bool Add(AsyncMsgCallback* cb);

    /**
    *@brief 收到回复的消息
    */
    void OnRecvReturnMsg(ObjID_t id, Avalon::Packet* packet);

    /**
    *@brief 收到消息
    */
    AsyncMsgCallback* FindCallback(ObjID_t id);

    /**
    *@brief 获取回复消息的packet
    */
    bool SendBackReturnMsg(ObjID_t id, Avalon::Protocol& req, Avalon::Protocol& ret);

	/**
	*@brief 获取回复消息的packet
	*/
	bool SendBackReturnMsg(ObjID_t id, UInt32 connid, Avalon::Protocol& ret);

public:

    /**
    *@brief 注册消息回调
    */
    void RegisterMsgHandler(Avalon::UInt32 id, CLParamPacketFunction<ObjID_t>* func)
    {
        m_Executor.Bind(id, func);
    }

    /**
    *@brief 执行一个消息包
    */
    void ExecutePacket(ObjID_t id, Avalon::Packet* packet);

private:

    /**
    *@brief 生成唯一ID
    */
    ObjID_t _GenID();

private:
    ObjID_t                         m_idSeed;
    MapMsgCallback                  m_msgCbs;

    // 消息包执行器
    CLParamPacketExecutor<ObjID_t>	m_Executor;
};

#define SEND_ASYNC_MSG(handle, packet) \
                do { \
                    Avalon::Packet* __packet = packet; \
                    handle(__packet); \
                    Avalon::Packet::Destroy(__packet); \
                } while(0)
            
#define SEND_ASYNC_MSG2(handle, cb, protocol) \
                do { \
					auto tmpCb = (cb); \
					AsyncMsgCallbackMgr::Instance()->Add(tmpCb); \
					Avalon::Packet* __packet = tmpCb->GetRequestPacket(protocol); \
                    handle(__packet); \
                    Avalon::Packet::Destroy(__packet); \
                } while(0)

#endif