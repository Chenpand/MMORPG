#ifndef __RED_PACKET_RECEIVER_H__
#define __RED_PACKET_RECEIVER_H__

/**
*@brief 红包领取者结构
*/

#include <CLDefine.h>
#include <CLSceneObject.h>
#include <CLRedPacketDefine.h>

class WSPlayer;
class RedPacket;

class RedPacketReceiver : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(RedPacketReceiver, Avalon::NullMutex)
public:
	RedPacketReceiver();
	~RedPacketReceiver();
	
	/**
	*@brief 领取者ID
	*/
	inline ObjID_t GetReceiverID() const { return m_ReceiverId; }

	/**
	*@brief 红包ID
	*/
	inline ObjID_t GetRedPacketID() const { return m_RedPacketId; }

	/**
	*@brief 领取者职业
	*/
	inline UInt8 GetReceiverOccu() const { return m_Occu; }

	/**
	*@brief 领取者等级
	*/
	inline UInt32 GetReceiverLevel() const { return m_Level; }

	/**
	*@brief 初始化领取者
	*/
	bool Init(WSPlayer* player, ObjID_t redPacketId, UInt32 money, RedPacket& redPacket);

	/**
	*@brief 获取金钱
	*/
	inline UInt32 GetMoney() const { return m_Money; }

	/**
	*@brief 领取者账号id
	*/
	inline ObjID_t GetReceiverAccID() const { return m_reciver_accid; }

	/**
	*@brief 获取信息
	*/
	RedPacketReceiverEntry GetEntry() const;

	RedPacketRecord ToClient() const;

	void SetBset();
public:
	/* 数据库相关 */
	void InsertToDB();
	void DelFromDB();
	void Update();

	CL_DBPROPERTY_BEGIN(CLSyncObject)
	CL_DBPROPERTY("receiver_id", m_ReceiverId)
	CL_DBPROPERTY("red_packet_id", m_RedPacketId)
	CL_DBPROPERTY("occu", m_Occu)
	CL_DBPROPERTY("level", m_Level)
	CL_DBPROPERTY("money_id", m_MoneyId)
	CL_DBPROPERTY("money", m_Money)
	CL_DBPROPERTY("time", m_Time)
	CL_DBPROPERTY("red_packet_owner_name", m_RedPacketOwnerName)
	CL_DBPROPERTY("is_best", m_isBest)
	CL_DBPROPERTY("reciver_accid", m_reciver_accid)
	CL_DBPROPERTY_END()
private:
	// 领取者ID
	ObjUInt64		m_ReceiverId;
	// 红包ID
	ObjUInt64		m_RedPacketId;
	// 职业
	ObjUInt8		m_Occu;
	// 等级
	ObjUInt32		m_Level;
	// 获得奖励
	ObjUInt32		m_Money;
	// 领取时间
	ObjUInt32		m_Time;
	// 红包发送者名字
	ObjString		m_RedPacketOwnerName;
	// 奖励id
	ObjUInt32		m_MoneyId;
	// 是否最佳
	ObjUInt8        m_isBest;
	// 账号id
	ObjUInt32		m_reciver_accid;
};

#endif