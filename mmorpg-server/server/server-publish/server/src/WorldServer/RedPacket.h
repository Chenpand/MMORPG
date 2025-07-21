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
	*@brief 拥有者ID
	*/
	inline ObjID_t GetOwner() const { return m_OwnerId; }

	std::string GetOwnerName() const { return  m_OwnerName; }

	/**
	*@brief 红包类型
	*/
	inline RedPacketType GetType() const { return (RedPacketType)(UInt8)m_Type; }

	/**
	*@brief 状态
	*/
	inline RedPacketStatus GetStatus() const { return (RedPacketStatus)(UInt8)m_Status; }
	void ChangeStatus(RedPacketStatus status);
	inline UInt32 GetStatusTime() const { return m_StatusTime; }
	RedPacketStatus GetStatus(ObjID_t playerId, UInt32 accid);
	RedPacketStatus GetGuildRedPacketStatus(ObjID_t playerId, UInt32 accid);

	/**
	*@brief 来源
	*/
	inline UInt16 GetReason() const { return m_Reason; }

	/**
	*@brief 总金额
	*/
	inline UInt32 GetTotalMoney() const { return m_TotalMoney; }
	inline void SetTotalMoney(UInt32 money) { m_TotalMoney = money; }

	/**
	*@brief 红包领取条件
	*/
	inline UInt64 GetCond1() const { return m_Cond1; }
	inline void SetCond1(UInt64 cond) { m_Cond1 = cond; }

	inline UInt64 GetCond2() const { return m_Cond2; }
	inline void SetCond2(UInt64 cond) { m_Cond2 = cond; }

	/**
	*@brief 红包个数
	*/
	inline void SetNum(UInt8 num) { m_Num = num; }
	inline UInt8 GetNum() { return m_Num; }
	/**
	*@brief 红包是否已经领完
	*/
	bool IsEmpty() const;

	/**
	*@brief 添加领取者
	*/
	void AddReceiver(RedPacketReceiver* receiver);
	RedPacketReceiver* FindReceiver(ObjID_t playerId);

	/**
	*@brief 获取基础信息
	*/
	RedPacketBaseEntry GetBaseEntry(ObjID_t roleId, UInt32 accid);

	/**
	*@brief 获取详细信息
	*/
	RedPacketDetail GetDetailInfo(ObjID_t roleId, UInt32 accid);

	UInt8 GetRemainNum();
public:
	/**
	*@brief 玩家打开红包
	*/
	UInt32 OnPlayerOpen(WSPlayer* player);

	/**
	*@brief 玩家是否满足红包打开条件
	*/
	bool IsMatchCond(WSPlayer* player);

	/**
	*@brief 玩家是否已经打开过红包了
	*/
	bool IsPlayerAlreadyOpened(ObjID_t id) const;

	/**
	*@brief 账号是否已经打开过红包
	*/
	bool IsAccountAlreadyOpend(UInt32 accid) const;

	/**
	*@brief 账号打开红包是否超出上限
	*/
	bool IsAccountOpenRedPacketReachMax(WSPlayer* player);
protected:
	/**
	*@brief 生成红包金额
	*/
	UInt32 _GenMoney();

	/**
	*@brief 获取剩余金钱
	*/
	UInt32 _GetRemainMoney() const;

	/**
	*@brief 红包已空
	*/
	void _OnEmpty(bool first = false);

	/**
	*@brief 获取手气最佳的人
	*/
	RedPacketReceiver* GetBestReceiver();

	/**
	*@brief 生成红包金额数组
	*/
	void _GenMoneyVec();

public:
	/* 数据库相关 */
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
	//保存定时器
	Avalon::SimpleTimer	m_SaveTimer;
	// 拥有者
	ObjUInt64			m_OwnerId;
	// 拥有者名字
	ObjString			m_OwnerName;
	// 拥有者等级
	ObjUInt16			m_OwnerLevel;
	// 拥有者职业
	ObjUInt8			m_OwnerOccu;
	// 类型(RedPacketType)
	ObjUInt8			m_Type;
	// 状态(RedPacketStatus)
	ObjUInt8			m_Status;
	// 来源(对应红包来源表中的ID)
	ObjUInt16			m_Reason;
	// 领取条件（根据红包类型有不同的含义，如公会红包，就代表公会ID）
	ObjUInt64			m_Cond1;
	// 领取条件2（根据红包类型有不同的含义，如公会战红包，就代表公会战时间）
	ObjUInt64			m_Cond2;
	// 总金额
	ObjUInt32			m_TotalMoney;
	// 剩余金额
	ObjUInt32			m_RemainMoney;
	// 红包个数
	ObjUInt8			m_Num;
	// 修改状态时间
	ObjUInt32			m_StatusTime;
	// 已经领取过的人
	std::vector<RedPacketReceiver*>		m_Receivers;
	// 红包金额数组（创建红包或数据库加载后生成，平时只从数组里取对应下标的金额）
	std::vector<UInt32> m_MoneyVec;
	bool				m_GenMoneyVec;
};

#endif