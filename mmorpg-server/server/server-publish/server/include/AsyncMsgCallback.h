#ifndef __ASYNC_MSG_CALLBACK_H__
#define __ASYNC_MSG_CALLBACK_H__

#include <CLDefine.h>
#include <AvalonProtocol.h>
#include <AvalonObjectPool.h>

enum MsgCallbackError
{
    MCE_TIMEOUT,
    MCE_RECV_INVALID_PACKET,
    MCE_NOT_SET_WAIT_MSG,
    MCE_DECODE_MSG_FAILED,
};

class AsyncMsgCallback 
{
	AVALON_DEFINE_OBJECTPOOL(AsyncMsgCallback, Avalon::NullMutex)
public:
    AsyncMsgCallback(Avalon::Protocol* waitMsg);
    virtual ~AsyncMsgCallback();

    /**
    *@brief ΨһID
    */
    inline void SetID(ObjID_t id) { m_id = id; }
    inline ObjID_t GetID() const { return m_id; }

    /**
    *@brief ���ó�ʱʱ��
    */
    inline void SetTimeOut(UInt32 timeout) { m_timeout = timeout; }

    /**
    *@brief �Ƿ�ʱ
    */
    bool IsTimeOut(const Avalon::Time& now) const ;

    /**
    *@brief ��ȡ�ȴ�����ϢID
    */
    UInt32 GetWaitMsgID() const;

    /**
    *@brief ��ȡ������Ϣ��
    */
    Avalon::Packet* GetRequestPacket(Avalon::Protocol& protocol);

public:
    /**
    *@brief �յ���Ϣ
    */
    void OnRecv(Avalon::Packet* packet);

    /**
    *@brief ִ�гɹ�
    */
    virtual void OnSuccess(Avalon::Protocol* msg) = 0;

    /**
    *@brief ִ��ʧ�ܣ���ȴ���ʱ
    */
    virtual void OnFailed(MsgCallbackError errorCode) = 0;

private:
    const static UInt32 DEFAULT_TIMEOUT_MS = 5 * 1000;

    ObjID_t             m_id;
    Avalon::Protocol*   m_waitMsg;
    UInt32              m_timeout;
    Avalon::Time        m_startTime;
};

#endif