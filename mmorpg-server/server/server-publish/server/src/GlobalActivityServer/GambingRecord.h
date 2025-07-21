#ifndef _GAMBING_RECORD_H_
#define _GAMBING_RECORD_H_


#include <AvalonObjectPool.h>
#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <CLItemDefine.h>
#include <CLObjectProperty.h>


/**
*@brief 夺宝记录
*/
class GambingRecord : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(GambingRecord, Avalon::NullMutex)

public:
	GambingRecord();
	~GambingRecord();

public:
	/**
	*@brief 设置获取玩家唯一id
	*/
	void SetOwnerUId(const std::string& uid) { m_OwnerUId = uid; }
	const std::string GetOwnerUId() const { return m_OwnerUId; }

	/**
	*@brief 设置获取角色名
	*/
	void SetOwnerName(const std::string& name) { m_OwnerName = name; }
	const std::string& GetOwnerName() const { return m_OwnerName; }

	/**
	*@brief 设置获取玩家所在服名字
	*/
	void SetOwnerServerName(const std::string& name) { m_OwnerServerName = name; }
	const std::string GetOwnerServerName() const { return m_OwnerServerName; }

	/**
	*@brief 设置获取玩家所在中心服节点
	*/
	void SetOwnerSourceCenterNode(UInt32 node) { m_OwnerSourceCenterNode = node; }
	UInt32 GetOwnerSourceCenterNode() const { return m_OwnerSourceCenterNode; }

	/**
	*@brief 设置获取玩家所在管理服节点
	*/
	void SetOwnerSourceAdminNode(UInt32 node) { m_OwnerSourceAdminNode = node; }
	UInt32 GetOwnerSourceAdminNode() const { return m_OwnerSourceAdminNode; }

	/**
	*@brief 设置获取夺宝商品id
	*/
	void SetGambingItemId(UInt32 itemid) { m_GambingItemId = itemid; }
	UInt32 GetGambingItemId() const { return m_GambingItemId; }

	/**
	*@brief 设置获取夺宝商品数量
	*/
	void SetGambingItemNum(UInt32 num) { m_GambingItemNum = num; }
	UInt32 GetGambingItemNum() const { return m_GambingItemNum; }

	/**
	*@brief 设置获取道具表id
	*/
	void SetItemDataId(UInt32 id) { m_ItemDataId = id; }
	UInt32 GetItemDataId() const { return m_ItemDataId; }

	/**
	*@brief 设置获取夺宝组id
	*/
	void SetGambingGroupId(UInt16 groupid) { m_GroupId = groupid; }
	UInt16 GetGambingGroupId() const { return m_GroupId; }

	/**
	*@brief 增加获取投入份数
	*/
	void AddInvestCopies(UInt32 copies) { m_InvestCopies = m_InvestCopies + copies; }
	UInt32 GetInvestCopies() const { return m_InvestCopies; }

	/**
	*@brief 设置获取投入货币id
	*/
	void SetInvestCurrencyId(UInt32 id) { m_InvestCurrencyId = id; }
	UInt32 GetInvestCurrencyId() const { return m_InvestCurrencyId; }

	/**
	*@brief 增加获取投入货币数量
	*/
	void AddInvestCurrencyNum(UInt32 num) { m_InvestCurrencyNum = m_InvestCurrencyNum + num; }
	UInt32 GetInvestCurrencyNum() const { return m_InvestCurrencyNum; }

	/**
	*@brief 设置获取夺宝权重
	*/
	void SetGambingWeight(UInt32 weight) { m_GambingWeight = weight; }
	UInt32 GetGambingWeight() const { return m_GambingWeight; }

	/**
	*@brief 设置获取创建时间
	*/
	void SetCreateTimeSec(UInt32 time) { m_CreateTimestamp = time; }
	UInt32 GetCreateTimeSec() const { return m_CreateTimestamp; }

	/**
	*@brief 计算出夺宝几率(客户端显示)
	*/
	std::string CalculateGambingRate(UInt32 baseValue);

public:
	//数据库相关
	void UpdateToDB();
	void InsertToDB();

	CL_DBPROPERTY_BEGIN(CLSyncObject)
		CL_DBPROPERTY("owner_uid", m_OwnerUId)
		CL_DBPROPERTY("owner_name", m_OwnerName)
		CL_DBPROPERTY("owner_server_name", m_OwnerServerName)
		CL_DBPROPERTY("owner_source_center_node", m_OwnerSourceCenterNode)
		CL_DBPROPERTY("owner_source_admin_node", m_OwnerSourceAdminNode)
		CL_DBPROPERTY("gambing_item_id", m_GambingItemId)
		CL_DBPROPERTY("gambing_item_num", m_GambingItemNum)
		CL_DBPROPERTY("item_data_id", m_ItemDataId)
		CL_DBPROPERTY("group_id", m_GroupId)
		CL_DBPROPERTY("invest_currency_id", m_InvestCurrencyId)
		CL_DBPROPERTY("invest_currency_num", m_InvestCurrencyNum)
		CL_DBPROPERTY("invest_copies", m_InvestCopies)
		CL_DBPROPERTY("gambing_weight", m_GambingWeight)
		CL_DBPROPERTY("create_time_sec", m_CreateTimestamp)

		CL_DBPROPERTY_END()

private:
	// 玩家唯一id
	ObjString m_OwnerUId;
	// 角色名
	ObjString m_OwnerName;
	// 玩家所在服名字
	ObjString m_OwnerServerName;
	// 玩家所在的中心服节点
	ObjUInt32 m_OwnerSourceCenterNode;
	// 玩家所在的管理服节点
	ObjUInt32 m_OwnerSourceAdminNode;
	// 夺宝商品id
	ObjUInt32 m_GambingItemId;
	// 商品数量
	ObjUInt32 m_GambingItemNum;
	// 道具表id
	ObjUInt32 m_ItemDataId;
	// 夺宝组id
	ObjUInt16 m_GroupId;
	// 投入货币id
	ObjUInt32 m_InvestCurrencyId;
	// 投入货币数量
	ObjUInt32 m_InvestCurrencyNum;
	// 投入份数
	ObjUInt32 m_InvestCopies;
	// 夺宝权重
	ObjUInt32 m_GambingWeight;
	// 创建时间
	ObjUInt32 m_CreateTimestamp;
};

/**
*@brief 夺宝开奖状态
*/
enum GambingLotteryStatus
{
	// 初始值
	GLS_INIT = 0,
	// 等待
	GLS_WAIT,
	// 已开奖
	GLS_OPENED,
};

/**
*@brief 夺宝商品开奖记录
*/
class GambingLotteryRecord : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(GambingLotteryRecord, Avalon::NullMutex)

public:
	GambingLotteryRecord();
	~GambingLotteryRecord();

public:
	/**
	*@brief 设置获取商品id
	*/
	void SetGambingItemId(UInt32 id) { m_GambingItemId = id; }
	UInt32 GetGambingItemId() const { return m_GambingItemId; }

	/**
	*@brief 设置获取夺宝商品id
	*/
	void SetGambingItemNum(UInt32 num) { m_GambingItemNum = num; }
	UInt32 GetGambingItemNum() const { return m_GambingItemNum; }

	/**
	*@brief 设置获取道具表id
	*/
	void SetItemDataId(UInt32 id) { m_ItemDataId = id; }
	UInt32 GetItemDataId() const { return m_ItemDataId; }

	/**
	*@brief 设置获取组id
	*/
	void SetGroupId(UInt16 id) { m_GroupId = id; }
	UInt16 GetGroupId() const { return m_GroupId; }

	/**
	*@brief 设置获取售罄时间
	*/
	void SetSoldOutTimeSec(UInt32 time) { m_SoldOutTimeSec = time; }
	UInt32 GetSoldOutTimeSec() const { return m_SoldOutTimeSec; }

	/**
	*@brief 设置获取开奖时间
	*/
	void SetLotteryTimeSec(UInt32 time) { m_LotteryTimeSec = time; }
	UInt32 GetLotteryTimeSec() const { return m_LotteryTimeSec; }

	/**
	*@brief 设置获取赢家id
	*/
	void SetGainerUId(const std::string& uid) { m_GainerUId = uid; }
	const std::string& GetGainerUId() const { return m_GainerUId; }

	/**
	*@brief 设置获取创建时间
	*/
	void SetCreateTimeSec(UInt32 time) { m_CreateTimeSec = time; }
	UInt32 GetCreateTimeSec() const { return m_CreateTimeSec; }

public:
	//数据库相关
	void UpdateToDB(bool bFlush = true);
	void InsertToDB();

	CL_DBPROPERTY_BEGIN(CLSyncObject)
		CL_DBPROPERTY("gambing_item_id", m_GambingItemId)
		CL_DBPROPERTY("gambing_item_num", m_GambingItemNum)
		CL_DBPROPERTY("item_data_id", m_ItemDataId)
		CL_DBPROPERTY("group_id", m_GroupId)
		CL_DBPROPERTY("soldout_time_sce", m_SoldOutTimeSec)
		CL_DBPROPERTY("lottery_time_sec", m_LotteryTimeSec)
		CL_DBPROPERTY("gainer_uid", m_GainerUId)
		CL_DBPROPERTY("create_time_sec", m_CreateTimeSec)

		CL_DBPROPERTY_END()

private:
	// 商品id
	ObjUInt32 m_GambingItemId;
	// 商品数量
	ObjUInt32 m_GambingItemNum;
	// 道具表id
	ObjUInt32 m_ItemDataId;
	// 组id
	ObjUInt16 m_GroupId;
	// 售罄时间
	ObjUInt32 m_SoldOutTimeSec;
	// 开奖时间
	ObjUInt32 m_LotteryTimeSec;
	// 赢家
	ObjString m_GainerUId;
	// 创建时间
	ObjUInt32 m_CreateTimeSec;
};

#endif