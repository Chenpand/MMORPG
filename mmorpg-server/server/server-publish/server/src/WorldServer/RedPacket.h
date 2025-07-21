#ifndef __RED_PACKET_H__
#define __RED_PACKET_H__

#include <AvalonSimpleTimer.h>
#include <CLDefine.h>
#include <CLSceneObject.h>
#include "RedPacketReceiver.h"

class WSPlayer;
class RedPacket : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(RedPacket, Avalon::NullMutex)
public:
	RedPacket();
	RedPacket(WSPlayer* player, RedPacketType type, UInt16 reason, UInt32 money);
	~RedPacket();

public:
	/**
	*@brief Tick
	*/
	bool OnTick(const Avalon::Time& now);

public:
	/**
	*@brief ӵ����ID
	*/
	inline ObjID_t GetOwner() const { return m_OwnerId; }

	std::string GetOwnerName() const { return  m_OwnerName; }

	/**
	*@brief �������
	*/
	inline RedPacketType GetType() const { return (RedPacketType)(UInt8)m_Type; }

	/**
	*@brief ״̬
	*/
	inline RedPacketStatus GetStatus() const { return (RedPacketStatus)(UInt8)m_Status; }
	void ChangeStatus(RedPacketStatus status);
	inline UInt32 GetStatusTime() const { return m_StatusTime; }
	RedPacketStatus GetStatus(ObjID_t playerId, UInt32 accid);
	RedPacketStatus GetGuildRedPacketStatus(ObjID_t playerId, UInt32 accid);

	/**
	*@brief ��Դ
	*/
	inline UInt16 GetReason() const { return m_Reason; }

	/**
	*@brief �ܽ��
	*/
	inline UInt32 GetTotalMoney() const { return m_TotalMoney; }
	inline void SetTotalMoney(UInt32 money) { m_TotalMoney = money; }

	/**
	*@brief �����ȡ����
	*/
	inline UInt64 GetCond1() const { return m_Cond1; }
	inline void SetCond1(UInt64 cond) { m_Cond1 = cond; }

	inline UInt64 GetCond2() const { return m_Cond2; }
	inline void SetCond2(UInt64 cond) { m_Cond2 = cond; }

	/**
	*@brief �������
	*/
	inline void SetNum(UInt8 num) { m_Num = num; }
	inline UInt8 GetNum() { return m_Num; }
	/**
	*@brief ����Ƿ��Ѿ�����
	*/
	bool IsEmpty() const;

	/**
	*@brief �����ȡ��
	*/
	void AddReceiver(RedPacketReceiver* receiver);
	RedPacketReceiver* FindReceiver(ObjID_t playerId);

	/**
	*@brief ��ȡ������Ϣ
	*/
	RedPacketBaseEntry GetBaseEntry(ObjID_t roleId, UInt32 accid);

	/**
	*@brief ��ȡ��ϸ��Ϣ
	*/
	RedPacketDetail GetDetailInfo(ObjID_t roleId, UInt32 accid);

	UInt8 GetRemainNum();
public:
	/**
	*@brief ��Ҵ򿪺��
	*/
	UInt32 OnPlayerOpen(WSPlayer* player);

	/**
	*@brief ����Ƿ�������������
	*/
	bool IsMatchCond(WSPlayer* player);

	/**
	*@brief ����Ƿ��Ѿ��򿪹������
	*/
	bool IsPlayerAlreadyOpened(ObjID_t id) const;

	/**
	*@brief �˺��Ƿ��Ѿ��򿪹����
	*/
	bool IsAccountAlreadyOpend(UInt32 accid) const;

	/**
	*@brief �˺Ŵ򿪺���Ƿ񳬳�����
	*/
	bool IsAccountOpenRedPacketReachMax(WSPlayer* player);
protected:
	/**
	*@brief ���ɺ�����
	*/
	UInt32 _GenMoney();

	/**
	*@brief ��ȡʣ���Ǯ
	*/
	UInt32 _GetRemainMoney() const;

	/**
	*@brief ����ѿ�
	*/
	void _OnEmpty(bool first = false);

	/**
	*@brief ��ȡ������ѵ���
	*/
	RedPacketReceiver* GetBestReceiver();

	/**
	*@brief ���ɺ���������
	*/
	void _GenMoneyVec();

public:
	/* ���ݿ���� */
	void InsertToDB();
	void SaveToDB();
	void DelFromDB();

	CL_DBPROPERTY_BEGIN(CLSyncObject)
	CL_DBPROPERTY("owner_id", m_OwnerId)
	CL_DBPROPERTY("owner_name", m_OwnerName)
	CL_DBPROPERTY("owner_level", m_OwnerLevel)
	CL_DBPROPERTY("owner_occu", m_OwnerOccu)
	CL_DBPROPERTY("type", m_Type)
	CL_DBPROPERTY("status", m_Status)
	CL_DBPROPERTY("reason", m_Reason)
	CL_DBPROPERTY("cond1", m_Cond1)
	CL_DBPROPERTY("total_money", m_TotalMoney)
	CL_DBPROPERTY("remain_money", m_RemainMoney)
	CL_DBPROPERTY("num", m_Num)
	CL_DBPROPERTY("status_time", m_StatusTime)
	CL_DBPROPERTY("cond2", m_Cond2)
	
	CL_DBPROPERTY_END()

protected:
	//���涨ʱ��
	Avalon::SimpleTimer	m_SaveTimer;
	// ӵ����
	ObjUInt64			m_OwnerId;
	// ӵ��������
	ObjString			m_OwnerName;
	// ӵ���ߵȼ�
	ObjUInt16			m_OwnerLevel;
	// ӵ����ְҵ
	ObjUInt8			m_OwnerOccu;
	// ����(RedPacketType)
	ObjUInt8			m_Type;
	// ״̬(RedPacketStatus)
	ObjUInt8			m_Status;
	// ��Դ(��Ӧ�����Դ���е�ID)
	ObjUInt16			m_Reason;
	// ��ȡ���������ݺ�������в�ͬ�ĺ��壬�繫�������ʹ�����ID��
	ObjUInt64			m_Cond1;
	// ��ȡ����2�����ݺ�������в�ͬ�ĺ��壬�繫��ս������ʹ�����սʱ�䣩
	ObjUInt64			m_Cond2;
	// �ܽ��
	ObjUInt32			m_TotalMoney;
	// ʣ����
	ObjUInt32			m_RemainMoney;
	// �������
	ObjUInt8			m_Num;
	// �޸�״̬ʱ��
	ObjUInt32			m_StatusTime;
	// �Ѿ���ȡ������
	std::vector<RedPacketReceiver*>		m_Receivers;
	// ���������飨������������ݿ���غ����ɣ�ƽʱֻ��������ȡ��Ӧ�±�Ľ�
	std::vector<UInt32> m_MoneyVec;
	bool				m_GenMoneyVec;
};

#endif