#ifndef _WS_RELATION_MGR_H_
#define _WS_RELATION_MGR_H_

#include <AvalonSimpleTimer.h>
#include "WSRelation.h"
#include "WSApplication.h"
#include <functional>

class WSPlayer;
class CLRecordCallback;
namespace CLProtocol{
	class SceneSyncChat;
};

inline UInt64 GetUSecsNow()
{
#if defined(AVALON_OS_WINDOWS)
	return 0;
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (UInt64)tv.tv_sec * 1000000 + (UInt64)tv.tv_usec;
#endif
}

class UpdateRelationDebugLog
{
public:
	UpdateRelationDebugLog(const char* info) : m_info(info)
	{
		if (WSApplication::Instance()->IsCheckUpDateRelation())
		{
			m_startTime = GetUSecsNow();
			//DebugLogStream << "begin invoke  WSRelationMgr::UpdateRelation form " << m_info.c_str() << LogStream::eos;
		}
		
	}

	~UpdateRelationDebugLog()
	{
		if (WSApplication::Instance()->IsCheckUpDateRelation())
		{
			UInt64 endTime = GetUSecsNow();
			DebugLogStream << "end invoke  WSRelationMgr::UpdateRelation in " << m_info.c_str() << " used time = " << endTime - m_startTime << LogStream::eos;
		}
	}
protected:
private:
	std::string m_info;
	UInt64	m_startTime;
};

/**
*@brief 全局关系管理器
*/
class GlobalRelationMgr : public Avalon::Singleton<GlobalRelationMgr>
{
public:
	GlobalRelationMgr();
	~GlobalRelationMgr();

	void RemoveAllRelation(ObjID_t roleUid);
};

/**
*@brief 全局代付订单管理器
*/
class AddonPayOrderMgr : public Avalon::Singleton<AddonPayOrderMgr>
{
public:
	AddonPayOrderMgr();
	~AddonPayOrderMgr();

	AddonPayOrderData* GetOrder(UInt64 id);
	void AddOrder(AddonPayOrderData* data);
	void GetReqOrders(UInt64 reqId, std::vector<AddonPayOrderData*>& outDatas);
	void GetRepOrders(UInt64 repId, std::vector<AddonPayOrderData*>& outDatas);
private:

	std::map<UInt64, AddonPayOrderData*> m_AddonPayDatas;
	std::map<UInt64, std::vector<AddonPayOrderData*>>	m_RequestDatas;
	std::map<UInt64, std::vector<AddonPayOrderData*>>	m_ReplyDatas;
};

/** 
 *@brief 关系管理器
 */
class WSRelationMgr 
{
	typedef std::map<ObjID_t, WSRelation*>	RelationMap;
	typedef std::vector<ObjID_t> IdVec;

	//最大杀死次数
	const static UInt32	MAX_KILL_TIMES = 999;
	
public:
	WSRelationMgr();
	~WSRelationMgr();

public:
	/**
	 *@breif 设置获取拥有者
	 */
	void SetOwner(WSPlayer* owner){ m_pOwner = owner; }
	WSPlayer* GetOwner() const{ return m_pOwner; }

	/**
	 *@brief 判断某种类型的关系是否满了
	 */
	bool IsFull(UInt8 type) const;

	/**
	 *@brief 查找、移除、增加关系
	 */
	WSRelation* FindRelation(UInt8 type, ObjID_t id);
	WSRelation* FindRelation(UInt8 type, const std::string& name);
	// 此方法会回收relation！
	void RemoveRelation(WSRelation* relation);
	bool RemoveRelation(UInt8 type, ObjID_t id);
	bool AddRelation(WSRelation* relation);
	bool HasRelation(ObjID_t id);
	bool HasRelation(UInt8 type);
	WSRelation* GetRelation(ObjID_t id);

	WSRelation* GetRelation(UInt8 type);
	bool ChangeToStranger(UInt8 type, ObjID_t id);
	bool AddFriend(UInt8 type, ObjID_t id);
	void UpdateRelation(ObjID_t uid, WSRelation* oldRelation, UInt8 newType);

	void RemoveAllRelations();
	void OnDeletePlayer(WSRelation* relation);

	/**
	 *@brief 遍历一个类型的关系
	 */
	void VisitRelations(UInt8 type, RelationVisitor& visitor);
	void VisitRelations(RelationVisitor& visitor);

	void VisitRelationsNew(UInt8 type, const std::function<bool(WSRelation*)>& ff);
	/**
	 *@brief 加载所有关系
	 */
	void LoadRelations();

	/**
	 *@brief 同步关系列表到场景
	 */
	void SyncRelationListToScene();

	/**
	*@brief 查询好友的后院操作次数
	*/
	//void QueryBKOpRecord(UInt16 uStartNo, UInt16 uNum, Avalon::NetOutputStream &stream);

	//重置好友关系每日送礼数量
	void ResetFriendDayGiftNum();

	//更新关系
	void UpdateRelation();

	void SetMasterNote(std::string note, bool syncDB = false);
	inline std::string GetMasterNote()	{ return m_MasterNote; }

	void SetIsRecvDisciple(bool isRecv, bool syncDB = false);
	inline bool GetIsRecvDisciple()	{ return m_IsRecvDisciple; }

public: //事件
	/**
	 *@brief 加载数据返回
	 */
	void OnLoadDataRet(CLRecordCallback* callback);

	/**
	 *@brief 玩家上线
	 */
	void OnOnline(bool dayChanged, bool isReconnect = false);

	//逻辑日更
	void OnLogicDayChange();

	/**
	 *@brief 玩家下线
	 */
	void OnOffline();

	/**
	 *@brief 被杀
	 */
	void OnKilled(WSPlayer* killer);

	/**
	 *@brief 等级改变
	 */
	void OnLevelChanged();

	/**
	*@brief vip等级改变
	*/
	void OnVipLvlChanged();

	/**
	*@brief 职业改变
	*/
	void OnOccuChanged();

	/**
	*@brief 段位改变
	*/
	void OnSeasonLvChanged();

	/**
	 *@brief 切换场景服务器
	 */
	void OnServerChanged();

	/**
	*@brief 战斗力改变
	*/
	void OnPowerChanged();

	/**
	*@brief 状态改变
	*/
	void OnStatusChanged(UInt8 status);

	/**
	*@brief 头像框改变
	*/
	void OnHeadFrameChanged();

	/**
	*@brief 公会改变
	*/
	void OnGuildChanged();

	/**
	*@brief 回归周年称号
	*/
	void OnReturnYearTitle();
	
	/**
	 *@brief 心跳
	 */
	void OnHeartbeat(const Avalon::Time& now);
	void FiveMinHb();

	/**
	 *@brief 名字改变
	 */
	static void OnNameChanged(ObjID_t playerId, const std::string& name);

	/**
	 *@brief 性别改变                                                                     
	 */
	static void OnSexChanged(ObjID_t playerId, UInt8 sex);

	/**
	*@brief 关系改变
	*/
	void OnRelationTypeChanged();

	/**
	*@brief 同步关系到数据库
	*/
	static void SyncRelationTypeToDB(ObjID_t owner, ObjID_t tar, UInt8 type);

	/**
	*@brief 检查关系优先级
	*/
	bool CheckRelationTypePriority(RelationType typeNew, RelationType typeOld);

	/**
	*@brief 检查关系开放等级
	*/
	bool CheckRelationOpenLv(RelationType type);

	//请求关系玩家
	void OnRequestPlayer(RelationType type, ObjID_t tarId);

	//清除请求玩家
	void ClearRequestPlayer(RelationType type);

	//查找请求玩家
	bool FindRequestPlayer(RelationType type, ObjID_t tarId);

	/**
	*@处理添加好友
	*/
	void OnAddFriend(WSRelation* relation);

	/**
	*@brief 处理变成回归玩家 bRegress [false]:不是回归玩家，[true]是回归玩家
	*/
	void OnChangeRegression(bool bRegress);

public:	// 消息包
	/**
	*@brief 同步玩家
	*/
	void SendNotifyDelBlackList(ObjID_t myId, ObjID_t tarId);
	
	void SendPlayerDelRelation(RelationType type, ObjID_t tarId);
	/**
	*@brief 同步场景
	*/
	void SendNotifyNewRelation(RelationType type, ObjID_t tarId);
	void SendNotifySceneDel(ObjID_t myId, RelationType type, ObjID_t tarId);

public:	// 亲密付
	void RequestAddonPayData(UInt32 goodId, UInt64 tarId);

	// 同步代付信息给玩家
	void SyncAddonPayData(AddonPayOrderData* order);

	// 同步所有代付信息给玩家
	void SyncAllAddonPayDatas(std::vector<AddonPayOrderData*>& orders);

private: // 亲密付
	bool TransOrderToNetData(AddonPayOrderData* order, AddonPayData& netData);

private:
	
	/**
	 *@brief 同步到数据库
	 */
	void SyncToDB();

	/**
	 *@brief 同步等级到数据库
	 */
	static void SyncLevelToDB(ObjID_t playerId, UInt16 level);

	/**
	*@brief 同步头像框到数据库
	*/
	static void SyncHeadFrameToDB(ObjID_t playerId, UInt32 id);

	/**
	*@brief 同步公会ID到数据库
	*/
	static void SyncGuildIdToDB(ObjID_t playerId, UInt64 id);

	/**
	*@brief 同步公会ID到数据库
	*/
	static void SyncReturnYearTitleToDB(ObjID_t playerId, UInt32 title);

	/**
	*@brief 同步vip等级到数据库
	*/
	static void SyncVipLevelToDB(ObjID_t playerId, UInt16 vipLvl);

	/**
	*@brief 同步职业到数据库
	*/
	static void SyncOccuToDB(ObjID_t playerId, UInt8 occu);

	/**
	*@brief 同步段位到数据库
	*/
	static void SyncSeasonLvToDB(ObjID_t playerId, UInt32	seasonLv);

	/**
	 *@brief 同步名字到数据库
	 */
	static void SyncNameToDB(ObjID_t playerId, const std::string& name);

	/**
	 *@brief 同步性别到数据库                                                                     
	 */
	static void SyncSexToDB(ObjID_t playerId, UInt8 sex);


	/**
	*@brief 同步qq vip等级到数据库
	*/
	static void SyncQQvipToDB(ObjID_t playerId, UInt8 qqVipLvl, UInt8 bSuperBlue);

	/**
	*@brief 好友祝福等级限制
	*@param 玩家等级
	*@return 是否受限 true:是 false:否
	*/
	bool IsBlessLevelLimit(UInt16 level);

	/**
	*@brief  同步好友回归字段
	*/
	static void SyncRegressionMarkToDB(ObjID_t playerId, UInt8 IsRegress);

	/**
	*@brief  同步下线时间
	*/
	static void SyncOfflineTimeToDB(ObjID_t playerId, UInt32 offlineTime);
public:
	/**
	*@brief 判定两玩家是否为好友
	*/
	static bool IsMutualFriend(WSPlayer* player1, WSPlayer* player2);

	/**
	*@brief 判断两玩家是否是师徒
	*/
	static bool IsMasterDisciple(WSPlayer* player1, WSPlayer* player2);

	/**
	*@brief 修改师门每日状态任务
	*/
	static void SyncMasterDailyTaskState(ObjID_t owner, ObjID_t tar, UInt8 type, UInt8 state);
	
	/**
	*@brief 更新师门每日任务状态
	*/
	void UpdateMasterDailyTaskStateOnLogicDayChange();

	/**
	*#brief 修改关系
	*/
	static void UpdateRelationTypeToDb(ObjID_t owner, UInt8 type, ObjID_t id, UInt8 newType);

	/**
	*@brief  设置备注
	*/
	UInt32 SetPlayerRemark(ObjID_t roleId, std::string& remark);

	/**
	*@brief 通知好友，师徒 我回归了
	*/
	void NotifyRelationReturn();
	void NotifyRelationReturnMsg(UInt32 type, CLProtocol::SceneSyncChat& chat);

	/**
	*@brief 获取师傅名字
	*/
	std::string GetMasterName();
private:
	//拥有者
	WSPlayer*	m_pOwner;
	//关系列表
	RelationMap	m_Relations[RELATION_MAX];
	//简单定时器
	Avalon::SimpleTimer	m_SyncDBTimer;
	bool		m_bDayGiftChxsanged;
	//师傅公告
	std::string	m_MasterNote;
	//是否收徒
	bool		m_IsRecvDisciple;
	//请求别人代付列表
	std::vector<AddonPayData>	m_RequestAddonPayData;
	//别人请求自己代付列表
	std::vector<AddonPayData>	m_ReplyAddonPayData;
	//申请过的玩家 5分种清一次
	std::set<ObjID_t>	m_RequstedPlayerID[RelationType::RELATION_MAX];
};

#endif
