#ifndef _MSG_CONFIRM_MGR_H_
#define _MSG_CONFIRM_MGR_H_

#include <CLDefine.h>

namespace Avalon
{
	class Packet;
};

namespace CLProtocol
{
	class SysMsgConfirmReq;
}

struct MsgConfirm
{
	MsgConfirm() : guid(0), zoneId(0), roleId(0), msgId(0), sendTime(0) {}

	UInt64 guid;
	UInt32 zoneId;
	UInt64 roleId;
	UInt32 msgId;
	UInt32 sendTime;
	Avalon::Packet* packet;
};

typedef void (*SendMsgToGameZone)(Avalon::UInt32, Avalon::Packet*);

/**
 *@brief 一种消息确认机制
 */
class MsgConfirmMgr : public Avalon::Singleton<MsgConfirmMgr>
{
public:

	MsgConfirmMgr();
	~MsgConfirmMgr();

	/**
	 *@brief 创建一个未确认的消息
	 */
	CLProtocol::SysMsgConfirmReq* CreateNoConfirmMsg(Avalon::Packet* packet);

	/**
	 *@brief 注册消息发送的接口
	 */
	bool RegisterSendMsgFun(SendMsgToGameZone sendFun);

	/**
	 *@brief tick
	 */
	void OnTick(const Avalon::Time& now);

	/**
	 *@brief 增加一个未确认消息
	 */
	void AddNoConfirmMsg(UInt64 roleId, UInt32 zoneId, Avalon::Packet* packet);

	/**
	 *@brief 回复确认消息
	 */
	void ReplyConfirmMsg(UInt64 guid);

private:

	/**
	 *@brief 再次发送未确认的消息
	 */
	void _AgainSendNoConfirmMsg();

private:

	// 发消息到区服的接口，提供接口到上层注册进来
	SendMsgToGameZone m_SendFun;

	// 未确认的消息( key->guid )
	std::map<UInt64, MsgConfirm> m_NoConfirmMsgMap;
};

#endif // !_MSG_CONFIRM_MGR_H_

