#ifndef _MALL_GIFT_PACK_H_
#define _MALL_GIFT_PACK_H_


#include <CLMallDefine.h>
#include <CLSyncObject.h>
#include <CLObjectProperty.h>

#include "WSPlayer.h"


//商城限时礼包触发状态
enum MallGiftPackActivateState
{
	MGPAT_NONE = 0,			//未触发
	MGPAT_OPEN,				//已开启
	MGPAT_CLOSED,			//已关闭
};


// 商城限时礼包
class MallGiftPack : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(MallGiftPack, Avalon::NullMutex);

public:
	MallGiftPack();
	~MallGiftPack();

public:
	static MallGiftPack* Create();
	static void Destroy(MallGiftPack*& item);

public:
	//获取设置账号id
	UInt32 GetAccId() { return m_AccId; }
	void SetAccId(UInt32 accid) { m_AccId = accid; }

	//获取设置角色id
	UInt64 GetPlayerId() { return m_PlayerId; }
	void SetPlayerId(UInt64 playerId) { m_PlayerId = playerId; }

	//获取设置限时礼包ID
	UInt32 GetGiftPackID() { return m_GiftPackID; }
	void SetGiftPackID(UInt32 giftPackId) { m_GiftPackID = giftPackId; }
	
	//获取设置限时礼包开始时间
	UInt32 GetActivateStartTime() { return m_ActivateStartTime; }
	void SetActivateStartTime(UInt32 startTime) { m_ActivateStartTime = startTime; }

	//获取设置限时礼包结束时间
	UInt32 GetActivateEndTime() { return m_ActivateEndTime; }
	void SetActivateEndTime(UInt32 endTime) { m_ActivateEndTime = endTime; }

	//获取设置限时礼包剩余数量
	UInt16 GetRestNum() { return m_RestNum; }
	void SetRestNum(UInt16 num) { m_RestNum = num; }

	//获取设置限时礼包激活条件
	UInt8 GetActivateCond() { return m_ActivateCond; }
	void SetActivateCond(UInt8 activateCond) { m_ActivateCond = activateCond; }

	//获取设置限时礼包激活状态
	UInt8 GetActivateState() { return m_ActivateState; }
	void SetActivateState(MallGiftPackActivateState actState) { m_ActivateState = (UInt8)actState; }

	//设置获取限时礼包激活计数
	UInt16 GetActivateCounter() { return m_ActivateCounter; }
	void SetActivateCounter(UInt16 actCounter);

public:
	/**
	*@brief 检查下限时礼包是否过期
	*/
	bool IsExpired(UInt32 curTime);

public:
	//数据库相关
	void UpdateToDB(WSPlayer* player, bool bFlush = true);
	void InsertProperty(WSPlayer* player);

	CL_DBPROPERTY_BEGIN(CLSyncObject)
		CL_DBPROPERTY("accid", m_AccId)
		CL_DBPROPERTY("roleid", m_PlayerId)
		CL_DBPROPERTY("gift_pack_id", m_GiftPackID)
		CL_DBPROPERTY("activate_start_time", m_ActivateStartTime)
		CL_DBPROPERTY("activate_end_time", m_ActivateEndTime)
		CL_DBPROPERTY("rest_num", m_RestNum)
		CL_DBPROPERTY("activate_cond", m_ActivateCond)
		CL_DBPROPERTY("activate_state", m_ActivateState)
		CL_DBPROPERTY("activate_counter", m_ActivateCounter)
	CL_DBPROPERTY_END()

private:
	//账号id
	ObjUInt32 m_AccId;

	//角色id
	ObjUInt64 m_PlayerId;

	//限时礼包id
	ObjUInt32 m_GiftPackID;

	//激活开始时间
	ObjUInt32 m_ActivateStartTime;

	//激活截止时间
	ObjUInt32 m_ActivateEndTime;

	//剩余数量
	ObjUInt16 m_RestNum;

	//激活条件
	ObjUInt8 m_ActivateCond;

	//激活状态
	ObjUInt8 m_ActivateState;

	//激活计数
	ObjUInt16 m_ActivateCounter;
};

#endif