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
 *@brief һ����Ϣȷ�ϻ���
 */
class MsgConfirmMgr : public Avalon::Singleton<MsgConfirmMgr>
{
public:

	MsgConfirmMgr();
	~MsgConfirmMgr();

	/**
	 *@brief ����һ��δȷ�ϵ���Ϣ
	 */
	CLProtocol::SysMsgConfirmReq* CreateNoConfirmMsg(Avalon::Packet* packet);

	/**
	 *@brief ע����Ϣ���͵Ľӿ�
	 */
	bool RegisterSendMsgFun(SendMsgToGameZone sendFun);

	/**
	 *@brief tick
	 */
	void OnTick(const Avalon::Time& now);

	/**
	 *@brief ����һ��δȷ����Ϣ
	 */
	void AddNoConfirmMsg(UInt64 roleId, UInt32 zoneId, Avalon::Packet* packet);

	/**
	 *@brief �ظ�ȷ����Ϣ
	 */
	void ReplyConfirmMsg(UInt64 guid);

private:

	/**
	 *@brief �ٴη���δȷ�ϵ���Ϣ
	 */
	void _AgainSendNoConfirmMsg();

private:

	// ����Ϣ�������Ľӿڣ��ṩ�ӿڵ��ϲ�ע�����
	SendMsgToGameZone m_SendFun;

	// δȷ�ϵ���Ϣ( key->guid )
	std::map<UInt64, MsgConfirm> m_NoConfirmMsgMap;
};

#endif // !_MSG_CONFIRM_MGR_H_

