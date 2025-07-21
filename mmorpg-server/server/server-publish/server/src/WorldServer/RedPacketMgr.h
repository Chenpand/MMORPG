#ifndef __RED_PACKET_MGR_H__
#define __RED_PACKET_MGR_H__

#include <AvalonSingleton.h>
#include <AvalonDailyTimer.h>
#include "RedPacket.h"

class RedPacketMgr : public Avalon::Singleton<RedPacketMgr>
{
	// ��ʼ״̬��ౣ��ʱ��
	const UInt32 RED_PACKET_INIT_TIME_OUT = 24 * 60 * 60;
	// �����˵ĺ������ʱ��
	const UInt32 RED_PACKET_WAIT_RECEIVE_TIME = 24 * 60 * 60;
	// �������󳤶�
	const UInt32 RED_PACKET_NAME_MAX_WIDTH = 20;
public:
	RedPacketMgr();
	~RedPacketMgr();

public:
	/**
	*@brief ��ʼ��
	*/
	bool Init();

	/**
	*@brief Tick
	*/
	bool OnTick(const Avalon::Time& now);

	/**
	*@brief ����
	*/
	void Final();

	/**
	*@brief ��ȡ���
	*/
	RedPacket* FindRedPacketByID(ObjID_t id) const;

	/**
	*@brief ɾ�����
	*/
	void DelRedPacketByID(ObjID_t id);

public:
	/**
	*@brief �������
	*/
	void OnPlayerOnline(WSPlayer* player);

	/**
	*@brief ����޸Ĺ��ᣨ�˻���߼��빫�ᣩ
	*/
	void OnPlayerChangeGuild(ObjID_t roleId, ObjID_t oldGuildId, ObjID_t newGuildId);

public:
	/**
	*@brief ��ҷ����ԷѺ��
	*/
	UInt32 OnPlayerSendCustomRedPacket(WSPlayer* player, UInt16 reason, std::string name, UInt8 num);

	/**
	*@brief �����Ӻ��
	*/
	UInt32 OnPlayerAddRedPacket(WSPlayer* player, UInt16 reason, std::string name, ObjID_t& redPacketId);

	/**
	*@brief ��ҷ��ͺ��
	*/
	UInt32 OnPlayerSendRedPacket(ObjID_t roleId, ObjID_t redPacketId, UInt8 num = 0);

	/**
	*@brief ��Ҵ򿪺��
	*/
	bool OnPlayerOpenRedPacket(WSPlayer* player, ObjID_t redPacketId);

	/**
	*@brief �Ƿ������������ʸ�
	*/
	bool IsCanReciveGuildRedPacket(RedPacket& redPacket, ObjID_t palyerId);
	
	/**
	*@brief ��ȡ�������սʱ��
	*/
	UInt32 GetGuildBattleLastTime(UInt8 type);

	/**
	*@brief ����ת��
	*/
	UInt8 RedPackThreTpCoverToPlayerJoinTp(RedPacketThreeType type);

	/**
	*@breif ��ȡ��������Ϣ
	*/
	UInt32 GetGuildRedPacketInfo(WSPlayer* player, std::vector<GuildRedPacketSpecInfo>& infos);
public:
	/**
	*@brief �ԷѺ�����񷵻�
	*/
	UInt32 OnCheckCustomRedPacketRet(WSPlayer* player, UInt16 reason, std::string name, UInt8 num);

public:
	// ���ݿ����

	/**
	*@brief ���غ������
	*/
	bool OnSelectRedPacketRet(CLRecordCallback* callback);

	/**
	*@brief ���غ����ȡ�߷���
	*/
	bool OnSelectRedPacketReceiverRet(CLRecordCallback* callback);

protected:
	/**
	*@brief ����Ƿ��ע������
	*/
	bool _IsPlayerSubscribe(WSPlayer* player, RedPacket* redPacket);

	/**
	*@brief ��Ӻ������������
	*/
	void _AddRedPacket(RedPacket* redPacket);

	/**
	*@brief ɾ�����
	*/
	void _DelRedPacket(RedPacket* redPacket);

private:
	HashMap<ObjID_t, RedPacket*>		m_RedPackets;
	Avalon::DailyTimer					m_DailyTimer;
};

#endif