#ifndef _WS_PLAYER_H_
#define _WS_PLAYER_H_

#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <CLGameDefine.h>
#include <CLRequest.h>
#include <AvalonNetConnection.h>
#include <AvalonProtocol.h>
#include <AvalonSimpleTimer.h>
#include <CLVipDataEntry.h>
#include <CLMatchProtocol.h>
#include <CLSysNotify.h>
#include <CLChatProtocol.h>
#include <CLAuctionDefine.h>
#include <CLTeamDefine.h>
#include <CLDungeonDefine.h>
#include <kingnet_analyzer.h>
#include <CLSceneDefine.h>
#include <CLGuildDefine.h>
#include <CLActivityInfo.h>
#include <CLCheatDefine.h>

#include "MailBox.h"
#include "WSRelationMgr.h"
#include "OfflineEventMgr.h"
#include "NewAuctionMgr.h"
#include "AccountChargeRecord.h"
#include "AccountInfo.h"
#include "MallGiftPackMgr.h"
#include "CondActivateMgr.h"
#include "Room.h"
#include "WSMasterSystem.h"
#include "Monopoly.h"
#include "NewTitle.h"
#include "DailyTodo.h"

class WSScene;
class WSTeam;
class Tribe;

class ItemTransaction;

class Pasture;

class AdventurePassInfo;

typedef std::function<void(WSPlayer*)> PlayerOfflineLoadCallBack;

/**
 *@brief 玩家状态
 */
enum PlayerStatus
{
	PS_INIT = 0,	    //初始状态
	PS_NORMAL = 1,	    //正常状态
    PS_LEAVEGAME = 2,   // 离开比赛，等待重连状态
	PS_FINAL = 3,	    //待销毁状态
};

//拉取玩家数据类型
enum LoadType
{
	LT_NONE = 0,		
	LT_MAKE_FRIEND = 1 << 0,		//同意添加好友
	LT_MAKE_MASTER = 1 << 1,		//同意拜师
	LT_MAKE_DISCIPLE = 1 << 2,		//同意收徒
	LT_ADDTO_BLACKLIST = 1 << 3,    //被拉黑
	LT_MAKE_MASTERSECT_BROTHER = 1 << 4, //建立同门师兄关系
	LT_MAKE_MASTERSECT_MASTER = 1 << 5,  //建立同门师傅关系，老号刷已存在关系
	LT_MAKE_MASTERSECT_DISCIPLE = 1 << 6, //建立同门徒弟关系
	LT_REMOVE_MASTERSECT_DISCIPLE = 1 << 7, //移除徒弟
	LT_PLAYER_SHAREDATA_GETMASTERTASK = 1 << 8,  //查看玩家师门任务
	LT_ONREMOVE_FRIEND = 1 << 9,	//被对方移除好友
	LT_AUCTION_QUERY_ITEM = 1 << 10, //拍卖行查询道具
	LT_2V2_SORTLIST_REWARD = 1 << 11, //2v2排行榜头衔奖励
	LT_FRIEND_PRSENT = 1 << 12, //好友赠送
};

#define PLAYERINFO(T) \
	"accid(" << T->GetAccId() << ") playerid(" << T->GetID() << ") occu(" << T->GetOccu() << ")"

// 角色基础信息
struct RoleBaseInfo
{
	ObjID_t roleid;
	UInt16	level;
};

/**
 *@brief 玩家  Noted by aprilliu, 收到scene server发来的WorldRegisterPlayerReq 进行创建， 其ObjID 就是角色id（一个guid）
 */
class WSPlayer : public CLSyncObject
{
	//道具事务超时时间
	const static UInt32 ITEM_TRANS_TIMEOUT = 5;
	//离开游戏超时
	const static UInt32	LEAVEGAME_TIMEOUT = 30;

public:
	WSPlayer();
	~WSPlayer();

public:
	/**
	*@brief 初始化
	*@param id 玩家的id
	*@param name 玩家名
	*@param sceneid 所在场景id
	*@param gateid 登录网关id
	*/
	bool Init(ObjID_t id, UInt32 accid, const std::string& name, UInt32 sceneid, UInt32 gateid);

	/**
	*@brief 获取网关节点id
	*/
	UInt32 GetGateID() const;
	void SetGateID(UInt32 nodeId);

	/**
	*@brief 账号信息
	*/
	AccountInfo* GetAccountInfo() const;
	void SetAccountInfo(AccountInfo* info);

	/**
	 *@brief 设置获取状态
	 */
	void SetGameStatus(UInt8 status)
	{ 
		m_Status = status; 
		m_StatusTime = (UInt32)CURRENT_TIME.Sec();
	}
	UInt8 GetGameStatus() const{ return m_Status; }
	UInt32 GetStatusTime() const { return m_StatusTime; }

	/**
	 *@brief 设置获取区id
	 */
	void SetZoneId(UInt16 zoneid){ m_ZoneId = zoneid; }
	UInt16 GetZoneId() const{ return m_ZoneId; }

	/**
	*@brief 设置获取出生区id
	*/
	void SetBornZoneId(UInt32 zoneId) { m_BornZoneId = zoneId; }
	UInt32 GetBornZoneId() const { return m_BornZoneId; }

	/**
	 *@brief 设置获取原区id
	 */
	void SetSrcZoneId(UInt32 srcZoneId){ m_SrcZoneId = srcZoneId; }
	UInt32 GetSrcZoneId() const{ return m_SrcZoneId; }

	/**
	 *@brief 设置获取等级
	 */
	void SetLevel(UInt16 level){ m_Level = level; }
	UInt16 GetLevel() const{ return m_Level; }

	/**
	 *@brief 设置获取职业
	 */
	void SetOccu(UInt8	occu){ m_Occu = occu; }
	UInt8 GetOccu() const{ return m_Occu; }

	/**
	*@brief 设置获取觉醒
	*/
	void SetAwaken(UInt8 awaken) { m_Awaken = awaken; }
	UInt8 GetAwaken() const { return m_Awaken; }

	/**
	*@brief 拍卖行刷新时间
	*/
	void SetAuctionRefreshTime(UInt32 time) { m_AuctionRefreshTime = time; }
	UInt32 GetAuctionRefreshTime();
	void RefreshAuctionRefreshTime();

	/**
	*@brief 拍卖行额外购买的栏位
	*/
	void SetAuctionAdditionBooth(UInt32 num) { m_AuctionAdditionBooth = num; }
	UInt32 GetAuctionAdditionBooth() const { return m_AuctionAdditionBooth; }

	/**
	 *@brief 设置获取性别
	 */
	void SetSex(UInt8 sex){ m_Sex = sex; }
	UInt8 GetSex() const{ return m_Sex; }

	/**
	*@brief 设置获取头像框
	*/
	void SetHeadFrame(UInt32 headFrameId){ m_HeadFrame = headFrameId; }
	UInt32 GetHeadFrame() const{ return m_HeadFrame; }
	void OnHeadFrameChanged(UInt32 headFrameId);

	void SetSceneID(UInt32 id);
	UInt32 GetSceneID() const { return m_SceneId; }

	void SetSceneType(SceneType type) { m_SceneType = type; }
	SceneType GetSceneType() const { return m_SceneType; }

	bool IsInTown() const;

	/**
	 *@brief 设置获取vip等级
	 */
	void SetVipLvl(UInt8 vip);
	UInt8 GetVipLvl() const;

	/**
	*@brief 获取上线时间
	*/
	UInt32 GetOnlineTime() const { return (UInt32)m_OnlineTime; }

	/**
	*@brief 设置获取vip经验
	*/
	void SetVipExp(UInt32 exp);
	UInt32 GetVipExp() const;

	/**
	*@brief 设置获取总充值金额
	*/
	void SetTotalChargeNum(UInt32 num);
	UInt32 GetTotalChargeNum() const;

	/**
	*@brief 设置获取角色总充值金额
	*/
	void SetTotalPlayerChargeNum(UInt32 num) { m_totalPlayerChargeNum = num; }
	UInt32 GetTotalPlayerChargeNum() const { return m_totalPlayerChargeNum; }

	/**
	*@brief 设置获取月卡到期时间
	*/
	void SetMonthCardExpireTime(UInt32 time);
	UInt32 GetMonthCardExpireTime() const;
	bool IsInMonthCard() const;

	/**
	*@brief 设置获取理财卡角色
	*/
	void SetMoneyManageStatus(UInt8 status);
	UInt8 GetMoneyManageStatus();

	/**
	 *@brief 设置qq特权信息
	 */
	void SetQQVipInfo(const QQVipInfo& info){ m_qqVipInfo = info; }
	const QQVipInfo& GetQQVipInfo() const{ return m_qqVipInfo; }
	UInt8 GetSuperBlue() const{ return m_qqVipInfo.IsSuperBlue() ? 1 : 0; }

	/**
	*@brief 设置获取战斗力
	*/
	void SetPower(UInt32 power) { m_Power = power; }
	UInt32 GetPower() const { return m_Power; }

	/**
	 *@brief 设置获取点劵
	 */
	void SetPoint(UInt32 point);
	UInt32 GetPoint() const;

	/**
	 *@brief 设置获取信用点劵
	 */
	void SetCreditPoint(UInt32 point);
	UInt32 GetCreditPoint() const;

	/**
	*@brief 设置获取公会
	*/
	void SetGuildID(ObjID_t id);
	ObjID_t GetGuildID() const { return m_GuildId; }

	void SetGuildLvl(UInt32 lvl) { m_GuildLvl = lvl; };
	UInt32 GetGuildLvl() const { return m_GuildLvl; }

	//获取权限的值
	void GetVipValue(VipPrivilegeType type, UInt32& value);
	UInt32 GetVipValue(VipPrivilegeType type);

	//获取权限的值
	void GetVipValue(VipPrivilegeType type, bool& value);

	/**
	 *@brief 设置获取创建时间
	 */
	void SetCreateTime(UInt32 time){ m_CreateTime = time; }
	UInt32 GetCreateTime() const{ return m_CreateTime; }
	UInt32 GetCreateFromNowOnDay();

	/**
	 *@brief 设置获取所在场景服务器
	 */
	bool SetSceneNode(UInt32 node);
	UInt32 GetSceneNode() const;

	//void IncExp(UInt32 exp); /******** 该接口未在使用 ********/

	/**
	*@brief world主动加经验
	*/
	void IncExpActive(const std::string& reason, UInt64 incExp);

	/**
	 *@brief 获取账号id
	 */
	UInt32	GetAccId() const{ return m_AccId; }

	/**
	*@brief 账号guid
	*/
	ObjID_t GetAccGuid() const { return m_AccGuid; }
	void SetAccGuid(ObjID_t id) { m_AccGuid = id; }

	/**
	 *@brief 设置openid
	 */
	void SetOpenId(const std::string& openid){ m_OpenId = openid; }
	const std::string& GetOpenId() const { return m_OpenId; }

	/**
	 *@brief 设置获取城市
	 */
	void SetCity(const std::string& city){ m_City = city; }
	const std::string& GetCity() const{ return m_City; }

	/**
	 *@brief 设置获取ip
	 */
	void SetIp(const std::string& ip);
	const std::string& GetIp() const{ return m_Ip; }
	UInt32 GetIpValue() const{ return m_IpValue; }

	/**
	 *@brief 设置获取平台id
	 */
	void SetPf(const std::string& pf){ m_Pf = pf; }
	const std::string& GetPf() const{ return m_Pf; }

	/**
	*@brief 设置获取设备id
	*/
	void SetDeviceId(const std::string& did) { m_DeviceId = did; }
	const std::string& GetDeviceId() const { return m_DeviceId; }

	/**
	*@brief 设置获取设备型号
	*/
	void SetModel(const std::string& model) { m_Model = model; }
	const std::string& GetModel() const { return m_Model; }

	/**
	*@brief 设置获取设备版本
	*/
	void SetDeviceVersion(const std::string& deviceVersion) { m_DeviceVersion = deviceVersion; }
	const std::string& GetDeviceVersion() const { return m_DeviceVersion; }

	/**
	*@brief 设置获取段位
	*/
	void SetSeasonLv(UInt32 seasonLv)	{ m_SeasonLv = seasonLv; }
	UInt32 GetSeasonLv()	{ return m_SeasonLv; }
	/**
	 *@brief 踢下线
	 */
	void KickOff(UInt32 errorCode, const char* msg = NULL);

	/**
	 *@brief 离开游戏
	 */
	void LeaveGame();

	/**
	 *@brief 设置获取离线时间
	 */
	void SetOfflineTime(UInt64 time){ m_OfflineTime = time; }
	UInt64 GetOfflineTime() const{ return m_OfflineTime; }
	UInt32 GetOfflineTimeSec() const { return (UInt32)(m_OfflineTime / 1000); }
	/**
	 *@brief 设置获取本服离线时间
	 */
	void SetLocalOfflineTime(UInt64 time){ m_LocalOfflineTime = time; }
	UInt64 GetLocalOfflineTime() const{ return m_LocalOfflineTime; }

	/**
	*@brief 设置获取领取离线邮件时间
	*/
	void SetOfflineMailTime(UInt64 time);
	void SaveOfflineMailTime(UInt64 time);
	UInt64 GetOfflineMailTime() const { return m_OfflineMailTime; }

	/**
	 *@brief 设置获取离线信息
	 */
	void SetOfflineScene(UInt32 sceneid){ m_OfflineScene = sceneid; }
	UInt32 GetOfflineScene() const{ return m_OfflineScene; }
	void SetOfflinePos(const CLPosition& pos){ m_OfflinePos = pos; }
	const CLPosition& GetOfflinePos() const{ return m_OfflinePos; }

	/**
	 *@brief 向场景同步社会关系数据
	 */
	void SyncRelationDataToScene();

	/**
	 *@brief 获取指定场景服务器人数
	 */
	static UInt32 GetServerPlayerNum(UInt32 nodeId);

	/**
	 *@brief 通知场景离开
	 */
	void NotifyLeaveGame(UInt8 isOfflineTimeOut = 0);

	/**
	 *@brief 判断能否再次招募
	 */
	bool CheckCopySceneRecruit();

	/**
	 *@brief 发送支付错误
	 */
	void SendBillingResult(UInt32 billingId, UInt8 result);

	/**
	*@brief 设置、活动商城道具购买信息
	*/
	void SetMallItemBuyedNum(UInt32 uId, UInt16 uNum);
	UInt16 GetMallItemBuyedNum(UInt32 uId);

	/**
	*@brief 设置、获得开服商城限量道具购买信息
	*/
	void SetGSMallBuyInfo(UInt32 uInfo) { m_uGSMallBuyInfo = uInfo; }
	UInt32 GetGSMallBuyInfo() const { return m_uGSMallBuyInfo; }

	/**
	 *@brief 加货币
	 */
	void AddMoneyItem(const char* reason, UInt8 type, UInt32 num);
	
	/**
	*@brief 加道具
	*/
	void AddItem(const std::string& reason, const ItemRewardVec& itemVec, bool isNotify = true);

	/**
	*@brief 通知客户端加了道具
	*/
	void NotifyClientAddItem(const std::string& reason, const ItemRewardVec& rewards, UInt8 notifyType = 1);

	/**
	*@brief 外观
	*/
	PlayerAvatar GetAvatar() const { return m_Avatar; }
	void SetAvatar(const PlayerAvatar& avatar);

	/**
	*@brief 玩家头像
	*/
	PlayerIcon GetIcon() const;

	/**
	*@brief oppo用户琥珀等级
	*/
	void SetOppoAmberLevel(UInt8 level) { m_OppoAmberLevel = level; }
	UInt8 GetOppoAmberLevel() const { return m_OppoAmberLevel; }

	/**
	*@brief 获取玩家标签信息
	*/
	void GetPlayerLabelInfo(PlayerLabelInfo& info) const;

	/*
	*@brief 通知scene查询待处理异常交易
	*/
	void NotifySceneQueryWaitHandleAbnormalTrans();

public://请求相关
	/**
	 *@brief 获取请求管理器
	 */
	RequestMgr& GetRequestMgr(){ return m_Requests; }

	/**
	 *@brief 同步请求
	 */
	void SyncRequest(UInt8 type, WSPlayer* pRequester, const std::string& param = "");

public://惩罚相关
	/**
	 *@brief 设置判断惩罚标志位
	 */
	void SetPunishFlag(UInt32 flag);
	UInt32 GetPunishFlag() const{ return m_PunishFlag; }
	bool CheckPunishFlag(int type) const{ return (m_PunishFlag & (1 << type)) != 0; }

	/**
	 *@brief 设置封号到期时间
	 */
	void SetForbitLoginDueTime(UInt32 duetime){ m_ForbitLoginDueTime = duetime; }

	/**
	 *@brief 设置获取禁言到期时间（废弃）
	 */
	void SetForbitTalkDueTime(UInt32 duetime){ m_ForbitTalkDueTime = duetime; }
	UInt32 GetForbitTalkDueTime() const{ return m_ForbitTalkDueTime; }

	void SetLeaveTimeOut(int leaveTimeout) { m_LeaveTimeout = leaveTimeout; }

public://道具相关
	/**
	 *@brief 准备道具事务
	 */
	bool BeginItemTransaction(ItemTransaction* transaction);

	/**
	 *@brief 获取当前的道具事务
	 */
	ItemTransaction* GetItemTransaction() const{ return m_pItemTrans; }

	/**
	 *@brief 解锁财产流程锁
	 */
	void UnLockAsserts();

	/**
	 *@brief 移除指定道具，在道具使用流程中用
	 */
	void RemoveItem(ObjID_t itemId);

	void OnCheckItemRet(UInt16 serial, UInt32 result, UInt32 counterTimes);


public://聊天相关
	/**
	 *@brief 通知
	 */
	//by huchenhui
	//void SendNotify(UInt32 id, ...);

	template <typename... Args>
	void SendNotify(UInt32 id, const Args & ... args)
	{
		UInt16 type = 0;
		UInt8 color = 0;
		char buffer[CL_MAX_CHAT_LEN + 1];
		bool ret = SysNotifyMgr::Instance()->MakeNotify(id, type, color, buffer, CL_MAX_CHAT_LEN, args...);

		if (!ret) return;

		CLProtocol::SysNotify notify;
		notify.type = type;
		notify.channel = 0;
		notify.color = color;
		notify.word = buffer;
		SendProtocol(notify);
	}

	void Notify(UInt16 type, UInt8 color, const char* word);

public://邮件相关
	/**
	 *@brief 获取邮箱
	 */
	MailBox& GetMailBox(){ return m_MailBox; }

public://离线事件
	/**
	 *@brief 获取离线事件管理器
	 */
	OfflineEventMgr& GetOfflineEventMgr(){ return m_OfflineEventMgr; }

	/**
	 *@brief 产生一个离线事件
	 */
	void PutOfflineEvent(ObjID_t playerId, OfflineEvent* offlineevent);
	bool CanPutOfflineEvent(UInt16 type, std::vector<OfflineEvent*>& events);

public://商城限时礼包
	MallGiftPackMgr& GetMallGiftPackMgr() { return m_MallGiftPackMgr; }

	/**
	*@brief 获取条件触发管理器
	*/
	CondActivateMgr& GetCondActivateMgr() { return m_CondActivateMgr; }

public://队伍相关

	/**
	*@brief 是否开放组队功能
	*/
	bool IsOpenTeamFunction();

	/**
	*@brief 是否在能加入队伍的场景
	*/
	bool IsSceneCanJoinTeam();

	/**
	 *@brief 设置获取队伍
	 */
	void SetTeam(WSTeam* team);
	WSTeam* GetTeam() const;
	UInt32 GetTeamID() const { return m_TeamId; }

	/**
	 *@brief 创建队伍
	 */
	void CreateTeam(UInt32 target, bool cancelMatch = true);

	/**
	 *@brief 邀请入队
	 */
	UInt32 InviteTeam(ObjID_t target);
	UInt32 ReplyInviteTeam(UInt32 teamId, bool agree);

	/**
	 *@brief 请求入队
	 */
	void RequestTeam(ObjID_t target, const std::string& passwd);
	void RequestJoinTeam(UInt32 teamId, const std::string& passwd);
	void ReplyRequestTeam(ObjID_t requester);

	/**
	*@brief 邀请玩家加入公会
	*/
	void InviteJoinGuild(ObjID_t targetId);

public://关系相关
	/**
	 *@brief 获取关系管理器
	 */
	WSRelationMgr& GetRelationMgr() { return m_RelationMgr; }

	/**
	 *@brief 邀请好友
	 */
	void InviteFriend(ObjID_t target, std::string tarName);
	void ReplyInviteFriend(ObjID_t requester);
	bool MakeFriend(WSPlayer* other);
	void RemoveFriend(ObjID_t target);
	bool AddToBlackList(WSPlayer* target);

	bool MakeFriendNew(WSPlayer* other);
	bool MakeFriendOfflineCb(WSPlayer* other);

	void OnRemovedFried(ObjID_t targetId);
	/**
	*@brief 师徒
	*/
	bool CanReqMaster(ObjID_t target, bool sendNotice);
	void ReqMaster(ObjID_t targetId, std::string tarName);
	void ReplyMaster(ObjID_t requesterId);

	bool CanReqDisciple(ObjID_t target, bool sendNotice);
	void ReqDisciple(ObjID_t targetId, std::string tarName);
	void ReplyDisciple(ObjID_t requesterId);

	bool MakeMasterDisciple(WSPlayer* master, WSPlayer* disciple, UInt8 type);
	void RemoveMaster(ObjID_t targetId);
	void RemoveDisciple(ObjID_t targetId);

	/**
	 *@brief 增加,获取扣除亲密度
	 */
	void AddIntimacy(ObjID_t friendId, UInt16 value);
	UInt16 GetIntimacy(ObjID_t friendId);
	UInt16 DecIntimacy(ObjID_t friendId, UInt16 value);

	/**
	*@biref 客户端计数器窗口设置 内存中
	*/
	void SetOpenFlag(UInt8 type, UInt8 flag) { m_mapOpenFlags[type] = flag; };
	UInt8 GetOpenFlag(UInt8 type) const;

	/**
	*@brief 获取师门系统
	*/
	WSMasterSystem& GetMasterSystem() { return m_MasterSys; }
	//师傅发布任务标记
	std::map<UInt64, UInt8> m_pubishTaskFlag;
	void SetPubishTasFlag(UInt64 discpleId);
	bool CheckPublishTaskFlag(UInt64 discpleId);
	void ResetPubishTasFlag(UInt64 discpleId);
public://宠物相关
	/**
	 *@brief 设置获取当前所在牧场
	 */
	void SetPasture(Pasture* pasture){ m_pPasture = pasture; }
	Pasture* GetPasture() const{ return m_pPasture; }

public:
	// 公会相关

	/**
	*@brief 添加公会技能
	*/
	void AddGuildSkill(UInt16 id, UInt8 level);

	/**
	*@brief 获取公会技能
	*/
	SkillBase* GetGuildSkill(UInt16 id);

	/**
	*@brief 获取公会技能等级
	*/
	UInt8 GetGuildSkillLevel(UInt16 id);

	/**
	*@brief 获取公会技能等级
	*/
	void SetGuildSkillLevel(UInt16 id, UInt8 level, UInt32 skillType = 1);

public://流程相关
	/**
	 *@brief 检查建立副本间隔
	 */
	bool CheckCreateCopyInterval(){ return m_CreateCopyTimer.IsTimeout(CURRENT_TIME); }

public:	// 数据中心日志相关
	/**
	*@brief 发送udplog
	*/
	//void SendUdpLog(const char *str1, const char *str2, const char *str3, const char *str4, const char *str5, const char *str6, const char *strType, Int32 nCount = 1);

	/**
	*@brief 发送udplog
	*/
	template <typename... Args>
	void SendUdpLog(const char* typeInfo, const char* format, const Args & ... args)
	{
		if (typeInfo == NULL || format == NULL) return;
		CUserLogger	*pLogger = _analyzer.GetInstance(UDPLOG_WORLD_UID);
		if (NULL == pLogger) return;

		char userLogBuffer[MAX_USERMSG_LENGTH];
		memset(userLogBuffer, 0, MAX_USERMSG_LENGTH);
		UInt32 nTime = (UInt32)Avalon::Time::CurrentTime().Sec();
		UInt16 zoneId = (GetSrcZoneId() != 0 ? GetSrcZoneId() : ZONE_ID);
		UInt32 monthCardExpireTime = m_MonthCardExpireTime;
		UInt32 bronZoneId = GetBornZoneId();
		UInt32 isReturn = IsVeteranReturn() ? 1 : 0;

#ifndef AVALON_OS_WINDOWS
		snprintf(userLogBuffer, sizeof(userLogBuffer), LOG_USERMSG_FORMAT, GAME_VERSION.c_str(), m_OpenId.c_str(), zoneId, m_Pf.c_str(), m_DeviceId.c_str(), m_Model.c_str(), m_DeviceVersion.c_str(), m_Ip.c_str(), GetID(), GetAccId(), (UInt32)GetLevel(), (UInt32)GetVipLvl(), (UInt32)GetOccu(), monthCardExpireTime, UInt32(m_CreateTime), nTime, bronZoneId, isReturn, 0, GetTotalPlayerChargeNum(), GetGuildLvl());
#else
		_snprintf_s(userLogBuffer, sizeof(userLogBuffer), LOG_USERMSG_FORMAT, GAME_VERSION.c_str(), m_OpenId.c_str(), zoneId, m_Pf.c_str(), m_DeviceId.c_str(), m_Model.c_str(), m_DeviceVersion.c_str(), m_Ip.c_str(), GetID(), GetAccId(), (UInt32)GetLevel(), (UInt32)GetVipLvl(), (UInt32)GetOccu(), monthCardExpireTime, UInt32(m_CreateTime), nTime, bronZoneId, isReturn, 0, GetTotalPlayerChargeNum(), GetGuildLvl());
#endif

		pLogger->SetUserIP(m_Ip.c_str());
		pLogger->SetUserMsg(userLogBuffer);
		pLogger->LogMsg(typeInfo, format, args...);

	}

	void SendLoginUdpLog();

	void SendLogoutUdpLog();

	/**
	*@brief 发送拍卖行上架的日志
	*/
	void SendAuctionUpLog(AuctionType type, UInt64 itemId, UInt32 itemDataId, UInt32 itemNum, MoneyType moneyType, UInt32 price, UInt32 deposit, UInt32 duetime);

	/**
	*@brief 发送拍卖行下架的日志
	*/
	void SendAuctionDownLog(zjy_auction::AuctionSendItemType type, AuctionType auctionType, UInt64 itemId, UInt32 itemDataId, UInt32 itemNum);

	/**
	*@brief 发送拍卖行出售的日志
	*/
	void SendAuctionBuyLog(UInt64 targetId, UInt32 ownerVipLev, AuctionType type, bool abnormalDeal, ObjID_t srcItemId, UInt64 itemId,
		zjy_auction::AuctionMainItemType mainType, UInt32 itemDataId, UInt32 itemNum, MoneyType moneyType, UInt32 price,
		UInt32 tax, UInt8 strengthen, UInt32 averPrice, UInt32 onSealTime, ItemQuality itemQuality, std::string sellerIp,
		std::string sellerDeviceId, UInt32 targetAccId, UInt8 isTreas, UInt32 sysRecomPrice, UInt32 minPriceOnSale, UInt64 transGuid,
		UInt8 isAttented, UInt32 itemAttentNum, UInt32 mountMagicCardId, UInt32 mountMagicCardAverPrice, UInt32 mountBeadId, UInt32 mountBeadAverPrice, UInt32 eqQualityLv, UInt32 remainSealCount,
		UInt64 minPriceGuid, UInt32 minPriceOnsalePrice, UInt32 minPriceQualityLv, UInt32 minPriceMountBeadId, UInt32 minPriceMountBeadAverPrice, UInt32 minPriceMountMagicId, UInt32 minPriceMountMagicAverPrice
		, UInt32 auctionPrice, UInt32 auctionTransNum, UInt32 auctionTransMax);

	/**
	*@brief 发送玩家复活的日志
	*/
	void SendReviveUdpLog(UInt32 dungeonId, UInt64 targetId, UInt32 areaId);

	/**
	*@brief 发送开始地下城的日志
	*/
	void SendDungeonStartUdpLog(UInt32 dungeonId, DungeonType type, std::string dungeonName, DungeonHellMode hellmode, UInt8 isTeam, UInt8 isHaveFriend, UInt8 hellAutoClose);

	/**
	*@brief 发送清房间日志
	*/
	void SendDungeonClearAreaUdpLog(UInt32 dungeonId, DungeonType type, std::string dungeonName, DungeonHellMode hellmode, UInt32 areaId, UInt32 usedTime);

	/**
	*@brief 发送地下城吃药协议
	*/
	void SendDungeonUsePotionUdpLog(UInt64 raceId, UInt32 dungeonId, UInt32 areaId, UInt32 itemTypeId, UInt8 isTeam);

	/**
	*@brief 发送地下城掉落
	*/
	void SendDungeonDropUdpLog(UInt32 dungeonId, UInt32 itemId, UInt32 color, UInt32 itemLevel);

	/**
	*@brief 发送公会战事件日志
	*/
	void SendGuildPvpUdpLog(UInt64 guildId, std::string guildName, UInt64 targetId, std::string targetName, PkType type, GuildBattleType battleType, UInt32 result, UInt32 oldGuildScore, UInt32 newGuildScore);

	/**
	*@brief 发送匹配成功日志
	*/
	void SendMatchSuccessLog(UInt32 zoneId, ObjID_t opponentId, UInt8 pkType, UInt32 seasonLevel, UInt32 matchScore, UInt32 usedTime);

	/**
	*@brief 发送取消匹配日志
	*/
	void SendMatchCancelLog(UInt8 pkType, UInt32 seasonLevel, UInt32 matchScore, UInt32 matchTime);

	/**
	*@brief 上报功能日志，玩家使用各种功能之后上报
	*/
	void SendFunctionUdpLog(FunctionLogType type, UInt32 p1, UInt32 p2);

	/**
	*@brief 决斗结束
	*/
	void SendPvpEndUdpLog(UInt32 targetZoneId, UInt64 targetId, PkType type, PkRaceResult result, UInt32 oldMatchScore, UInt32 newMatchScore, UInt32 oldPkValue, UInt32 newPkValue, UInt32 oldSeasonLevel, UInt32 newSeasonLevel, UInt32 oldSeasonStar, UInt32 newSeasonStar, UInt32 oldSeasonExp, UInt32 newSeasonExp, UInt32 seasonId, UInt32 dungeonId, UInt32 usedTime);

	//void SendGuildStorageUdpLog(std::string reason, UInt64 itemId, UInt32 itemDataId, UInt32 num);

	/**
	*@brief 创建房间事件
	*/
	void SendRoomCreateUdpLog(UInt32 roomId, UInt32 roomType, UInt32 createTime);

	/**
	*@brief 删除房间事件
	*/
	void SendRoomDeleteUdpLog(UInt32 roomId, UInt32 roomType, UInt32 createTime);

	/**
	*@brief 进入房间事件
	*/
	void SendRoomJoinUdpLog(UInt32 roomId, UInt32 roomType, UInt32 createTime);

	/**
	*@brief 退出房间事件
	*/
	void SendRoomQuitUdpLog(UInt32 roomId, UInt32 roomType, UInt32 createTime);

	/**
	*@brief 匹配开始
	*/
	void SendRoomMatchStartUdpLog(UInt32 roomId, UInt32 roomType, UInt32 roomPlayerSize, UInt32 createTime);

	/**
	*@brief 匹配成功
	*/
	void SendRoomBattleStartUdpLog(UInt32 roomId, UInt32 roomType, UInt32 roomPlayerSize, UInt32 createTime);

	/**
	*@brief 道具
	*/
	void SendItemUdpLog(const char* reason, UInt64 itemId, UInt32 itemDataId, ItemType itemType, ItemSubType itemSubType, std::string itemName, UInt32 itemQuality, Int32 number, UInt32 oldNumber);

	/**
	*@brief 发送拜师log
	*/
	void SendMakeMasterDiscipleUdpLog(ObjID_t discipleId, ObjID_t masterId, UInt32 discipleLevel, UInt32 masterLevel, UInt32 discipleViplev, UInt32 masterViplev, UInt32 timestamp, UInt8 eventType, UInt8 masterOccu, UInt8 discipleOccu);

	/**
	*@brief 发送出师log
	*/
	void SendFinishSchoolUdpLog(UInt8 type, ObjID_t discipleId, ObjID_t masterId, UInt32 discipleLevel, UInt32 masterLevel, UInt32 discipleGrowth, UInt32 apprentTimeSec, UInt32 finishTimeSec);

	/**
	*@brief 发送师傅发布师门任务log
	*/
	void SendMasterPublishDailyTaskLog(ObjID_t discipleId, ObjID_t masterId, UInt32 timestamp);

	/**
	*@brief 发送领取师门日常任务礼包log
	*/
	void SendReceiveDailyTaskGiftLog(UInt32 type, ObjID_t uid, UInt32 timestamp);

	/**
	*@brief 发送师徒互动log
	*/
	void SendMasterDiscipleInteractionLog(ObjID_t discipleId, ObjID_t masterId, UInt32 dungeonId, UInt32 timestamp);

	/**
	*@brief 发送作弊原因log
	*/
	void SendCheatLog(PlayerCheatReason reason, ObjID_t raceId, UInt32 dungeonId);

	/**
	*@brief 发送资产相关log
	*/
	void SendAssetUdpLog(AssetType type, const char* reason, UInt64 oldAsset, Int64 assetNum, Int64 nowAssetNum, UInt64 nowAsset);

	/**
	*@brief 发送冒险队名字改变log
	*/
	void SendAdventureTeamNameChangeUdpLog(const std::string& oldName, const std::string& newName);

	/**
	*@brief 发送派遣远征log
	*/
	void SendDispatchExpeditionTeamUdpLog(UInt8 mapId, UInt32 duration, UInt32 memberNum);

	/**
	*@brief 发送取消远征log
	*/
	void SendCancelExpeditionTeamUdpLog(UInt8 mapId);

	/**
	*@brief 发送领取远征奖励log
	*/
	void SendGetExpeditionTeamRewardsUdpLog(UInt8 mapId, UInt32 duration, const std::vector<UInt32>& condIds);
	
	/**
	**@brief 发生黑市商人交易log
	*/
	void SendBlackMarketTransUdpLog(UInt8 opType, UInt64 transid, UInt64 auctId, UInt8 auctType, UInt32 auctItemDataId, UInt64 auctItemId, UInt32 auctPrice);

	/**
	**@brief 发生冒险队信息log
	*/
	void SendAdventureTeamInfoUdpLog();

	/**
	**@brief 发生充值推送log
	*/
	void SendRechargePushUdpLog(UInt32 pushId, UInt32 price);

	/**
	**@brief 发生公会加入或退出log
	*/
	void SendGuildJoinExitUdpLog(UInt32 opType);

	/**
	*@brief 发送地下城roll日志
	*/
	void SendGungeonRollUdpLog(UInt32 dungeonId, UInt32 rollIndex, UInt32 rollItem, UInt32 rollItenNum, UInt32 rollType, UInt32 point);


	/**
	*@brief 发送冒险通行证日志
	*/
	void SendAdventurePassUdpLog(UInt32 level, UInt32 passType);

	/**
	*@brief发送添加好友请求日志
	*/
	void SendAddFriendUdpLog(UInt32 guildId, UInt32 tLevel, UInt32 tVipLev, UInt32 tOccu, UInt32 tGuildId);

	/**
	*@brief发送货币存量日志
	*/
	void SendMoneyStockUdpLog();

public://数据库相关
	/**
	 *@brief 增加加载项个数，返回本次加载序号
	 */
	UInt8 IncLoadDataCount();

	/**
	 *@brief 检查加载项
	 */
	bool CheckLoadDataSerial(UInt8 serial);
	void SetLoadDataFinished(UInt8 serial);

public://事件
	/**
	 *@brief 上线
	 */
	void OnOnline(bool isReconnect = false);

	/**
	 *@brief 下线
	 */
	void OnOffline(bool serverQuit = false);

	/**
	*@brief 与Gate断开连接（闪断）
	*/
	void OnDisconnect();

	/**
	*@brief 重连成功
	*/
	void OnReconnectSuccess();

	/**
	 *@brief 场景服务器切换
	 */
	void OnServerChanged(UInt32 oldNodeId);

	/**
	 *@brief 等级改变
	 */
	void OnLevelChanged(UInt16 oldLv);

	/**
	*@brief 职业改变
	*/
	void OnOccuChanged();

	/**
	 *@brief vip等级该表
	 */
	void OnVipLvlChanged();

	/**
	*@brief 赛季等级改变
	*/
	void OnSeasonLvChanged();

	/**
	*@brief 经验增加
	*/
	void OnExpIncrease(UInt64 addExp, const std::string& reason);

	/**
	 *@brief 被杀死
	 */
	void OnKilled(WSPlayer* attacker);

	/**
	 *@brief 心跳
	 */
	void OnHeartbeat(const Avalon::Time& now);

	/**
	 *@brief 加载数据完成
	 */
	void OnLoadDataFinished(bool isReconnect = false);

	/**
	*@brief 加载玩家全局信息回调
	*/
	void OnLoadGlobalInfo(CLRecordCallback *pCallback);

	/**
	*@brief 当天第一次登录
	*/
	void OnFirstLogin();

	/**
	 *@brief 战力改变
	 */
	void OnPowerChanged();

	/**
	*@brief 好友赠送
	*/
	void HandlerFriendGive(WSPlayer* target, WSRelation* relation);
	void OnFriendGive(WSPlayer* target);

	/**
	*@brief 拍卖行 上架
	*/
	void OnAddAuction(zjy_auction::AuctionInfo* auctionInfo);

	/**
	*@brief 拍卖行 下架
	*/
	void OnDownAuction(zjy_auction::AuctionSendItemType downType, AuctionType type, UInt64 itemId, UInt32 itemDataId, UInt32 num);

	/**
	*@brief 拍卖行 卖出
	*/
	void OnSellAuction(UInt32 money);

	/**
	*@brief 拍卖行 购买
	*/
	void OnBuyAuction(AuctionType type, bool abnormalDeal, ObjID_t owner, UInt32 ownerVipLev, ObjID_t srcItemId,
		ObjID_t itemId, zjy_auction::AuctionMainItemType mainType, UInt32 itemDataId, UInt32 num,
		MoneyType priceType, UInt32 price, UInt32 counterFee, UInt8 strengthen, UInt32 averPrice,
		UInt32 onSealTime, ItemQuality itemQuality, std::string sellerIp, std::string sellerDeviceId, UInt32 ownerAccount,
		UInt8 isTreas, UInt32 sysRecomPrice, UInt32 minPriceOnSale, UInt64 transGuid, UInt8 isAttented, UInt32 itemAttentNum,
		UInt32 mountMagicCardId, UInt32 mountMagicCardAverPrice, UInt32 mountBeadId, UInt32 mountBeadAverPrice, UInt32 eqQualityLv, UInt32 remainSealCount,
		UInt64 minPriceGuid, UInt32 minPriceOnsalePrice, UInt32 minPriceQualityLv, UInt32 minPriceMountBeadId, UInt32 minPriceMountBeadAverPrice,
		UInt32 minPriceMountMagicId, UInt32 minPriceMountMagicAverPrice, UInt32 auctionPrice, UInt32 auctionTransNum, UInt32 auctionTransMax);

	/**
	*@brief 城镇事件
	*/
	void OnSyncSceneEvent(SyncEventType type, ObjID_t param1, ObjID_t param2, ObjID_t param3);

	/**
	*@brief 团本事件
	*/
	void OnTeamCopyEvent(ObjID_t param1, ObjID_t param3);

	/**
	*@brief 公会改变
	*/
	void OnGuildChanged();

	/**
	*@brief 加入工会
	*/
	void OnJoinGuild();

	/**
	*@brief 请求GlobalServer信息
	*/
	void RequestGlobalServerInfo();

	/**
	*@brief 增加工会战积分
	*/
	void OnIncGuildBattleScore(UInt32 score);

	/**
	*@brief 占领领地
	*/
	void OnGuildOccupyTerritory(UInt8 terrId);

	/**
	*@brief 商城购买
	*/
	void OnMallItemBuy(UInt32 mallItemId, UInt32 buyNum);

	/**
	*@brief 杀怪
	*/
	void OnOwnedNpcDied(UInt32 npcid, UInt8 type, UInt32 dungeon);

	/**
	*@brief 玩家通关
	*/
	void OnPlayerClearDungeon(const DungeonDataEntry* dungeonData, LevelScore score, UInt32 usedTime, UInt8 isAsist);

	/**
	*@brief 疲劳消耗
	*/
	void OnRemoveFatigue(UInt32 removeFatigue);

public://匹配相关

	/**
	*@brief 正在匹配中
	*/
	void SetMatching(bool matching) { m_IsMatching = matching; }
	bool IsMatching() const { return m_IsMatching; }

	/**
	*@brief 能否开始匹配
	*/
	bool CanStartMatch();

	/**
	*@brief 开始匹配
	*/
	void OnStartMatch();

	/**
	*@brief 取消匹配
	*/
	void OnCancleMatch();

	/**
	*@brief 开始pk战斗
	*/
	void OnStartPvPRace();

	/**
	*@brief 获取战斗需要的玩家信息
	*/
	RacePlayerInfo GetRacePlayerInfo();

	/**
	*@brief 请求挑战玩家
	*/
	void RequestPkChallenge(ObjID_t otherId, RequestType type = REQUEST_CHALLENGE_PK);

	/**
	*@brief 挑战玩家请求返回
	*/
	void ReplyPkChallenge(ObjID_t requesterId, bool agree, RequestType type = REQUEST_CHALLENGE_PK);

	/**
	*@brief 能否开始挑战赛 type用于区分正常pk邀请和公平竞技场pk邀请
	*/
	UInt32 CanStartPkChallenge(bool isChallenger, RequestType type = REQUEST_CHALLENGE_PK);

	/**
	*@brief 获取战斗状态（空闲、忙碌）
	*/
	FriendMatchStatus GetMatchStatus();
	void SetMatchStatus(FriendMatchStatus status, bool forceSync = false);

	/**
	*@brief 计算战斗状态（空闲、忙碌）
	*/
	FriendMatchStatus CountMatchStatus();

	/**
	*@brief 更新战斗状态
	*/
	void UpdateMatchStatus();

public:// 拍卖相关
	/**
	*@brief 获取正在拍卖的物品数量
	*/
	UInt8 GetAuctionNum(AuctionType type);

	/**
	*@brief	清空拍卖物品
	*/
	void ClearAuction();

	/**
	*@brief 添加拍卖物品
	*/
	void AddAuction(AuctionType type, ObjID_t id);

	/**
	*@brief 删除拍卖物品
	*/
	void DelAuction(AuctionType type, ObjID_t id);

	/**
	*@brief 获取拍卖列表
	*/
	std::vector<ObjID_t> GetAuctionList(AuctionType type);

public: //房间相关
	//获得房间ID
	UInt32 GetRoomId() { return m_RoomId; }
	//设置房间ID
	void SetRoomId(UInt32 roomId) { m_RoomId = roomId; }

	//获取退出房间id
	UInt32 GetQuitRoomId() { return m_QuitRoomId; }
	//设置退出房间id
	void SetQuitRoomId(UInt32 roomId) { m_QuitRoomId = roomId; }

	//获取退出房间的时间
	UInt32 GetQuitRoomTime() { return m_QuitRoomTime; }
	//设置退出房间的时间
	void SetQuitRoomTime(UInt32 quitRoomTime) { m_QuitRoomTime = quitRoomTime; }

	UInt16 GetRoomLimitLevel();

	UInt32 GetRoomLimitSeasonLevel();

	void OnStartRoomRace();

	void OnCancleRoomMatch();

	bool IsOpenRoomFunction();

public: //33活动
	UInt32 GetScoreWarLoseStreak() { return m_ScoreWarLoseStreak; }
	void SetScoreWarLoseStreak(UInt32 loseStreak) { m_ScoreWarLoseStreak = loseStreak; }

	UInt32 GetScoreWarWinStreak() { return m_ScoreWarWinStreak; }
	void SetScoreWarWinStreak(UInt32 winStreak){ m_ScoreWarWinStreak = winStreak; }

	UInt32 GetMatchScore() { return m_MatchScore; }
	void SetMatchScore(UInt32 matchScore) { m_MatchScore = matchScore; }

	UInt32 GetScoreWarBattleCount() { return m_ScoreWarBattleCount; }
	void SetScoreWarBattleCount(UInt32 battleCount) { m_ScoreWarBattleCount = battleCount; }

public: //挖宝相关
	//获取挖宝地图ID
	UInt32 GetDigMapId() { return m_DigMapId; }
	//设置挖宝地图ID
	void SetDigMapId(UInt32 mapId) { m_DigMapId = mapId; }

public://充值相关
	/**
	*@brief 获得某个商品充值次数
	*/
	UInt32 GetChargeGoodsTimes(ChargeMallType mallType, UInt32 goodsId, ObjID_t roleId);

	/**
	*@brief 获得商品今天的购买次数
	*/
	UInt32 GetTodayChargeTimes(ChargeMallType mallType, UInt32 goodsId, ObjID_t roleId);

	/**
	*@brief 充值成功回调
	*/
	void AddChargeGoodsTimes(UInt32 accid, ObjID_t roleId, ChargeMallType mallType, UInt32 goodsId, UInt32 times);

	/**
	*@brief 重置充值记录
	*/
	void ResetChargeGoodsTimes();

	/**
	*@brief 能否购买月卡
	*/
	bool CanBuyMonthCard();

	/**
	*@brief 能否购买理财卡
	*/
	bool CanBuyMoneyManageCard();

public:// 战斗相关
	/**
	*@brief 当前进行的比赛ID
	*/
	ObjID_t	GetRaceID() const { return m_RaceID; }
	void SetRaceID(ObjID_t id) { m_RaceID = id; }

	/**
	*@brief 获取relay地址
	*/
	const Avalon::SockAddr& GetRelayAddr() const { return m_RelayAddr; }
	void SetRelayAddr(const Avalon::SockAddr& addr) { m_RelayAddr = addr; }

public:// 红包
	/**
	*@brief 发送红包成功
	*/
	void OnSendRedPacketSuccess(ObjID_t redPacketId, UInt16 reason, UInt8 num, const std::string& name);

	/**
	*@brief 发送红包失败
	*/
	void OnSendRedPacketFailure(UInt32 errorCode);

public: //抗魔值
	/**
	*@brief 抗魔值
	*/
	void SetResistMagic(UInt32 value) { m_ResistMagic = value; }
	UInt32 GetResistMagic() { return m_ResistMagic; }

	void HandleSceneSyncResistMagic(UInt32 value);

public://个人活动
	PlayerActivityInfo<WSPlayer>& GetPlayerActivityInfo() { return m_PlayerActivityInfo; }

public:// 冒险队
	/**
	*@brief 冒险队
	*/
	void TryCreateAdventureTeam();
	bool HasAdventureTeam();

	/**
	*@brief 冒险队名字
	*/
	const std::string GetAdventureTeamName() const;
	bool RenameAdventureTeam(const std::string& newName);

	/**
	*@brief 冒险队等级
	*/
	UInt16 GetAdventureTeamLevel() const;

	/**
	*@brief 冒险队经验
	*/
	UInt64 GetAdventureTeamExp() const;

	/**
	*@brief 冒险队评级
	*/
	const std::string& GetAdventureTeamGrade() const;

	UInt8 GetAdventureTeamGradeNum() const;

	/**
	*@brief 冒险队角色总评分
	*/
	UInt32 GetAdventureRoleScore() const;

	/**
	*@brief 同步冒险队信息
	*/
	void SyncAdventureTeamInfo(bool create = false);

	/**
	*@brief 添加解锁的新职业
	*/
	void TryAddUnlockedNewOccu();

	/**
	*@brief 同步解锁的新职业
	*/
	void SyncUnlockedNewOccus(bool isOnOnline = false);

	/**
	*@brief 获取冒险队地下城加成
	*/
	void GetAdventureTeamDungeonAddition(DungeonAddition& addition, const DungeonDataEntry* dungeonData);

	/**
	*@brief 赐福水晶经验
	*/
	UInt64 GetBlessCrystalExp() const;

	/**
	*@brief 赐福水晶数量
	*/
	UInt32 GetBlessCrystalNum() const;

	/**
	*@brief 传承祝福经验
	*/
	UInt64 GetInheritBlessExp() const;

	/**
	*@brief 传承祝福数量
	*/
	UInt32 GetInheritBlessNum() const;

	/**
	*@brief 使用传承祝福
	*/
	UInt32 UseInheritBless(UInt32 num = 1);

	/**
	*@brief 赏金
	*/
	UInt32 GetBounties() const;

	/**
	*@brief 同步冒险队相关货币
	*/
	void SyncAdventureTeamMoney();

public://角色评分
	/**
	*@brief 角色评分同步
	*/
	void OnRoleValueScoreSync(UInt32 roleValueScore);

	/**
	*@brief 所有角色总评分
	*/
	UInt32 GetTotalScoreOfAllRoleValue() const;

public://角色栏位
	/**
	*@brief 解锁的可扩展角色栏位
	*/
	void SetUnlockedExtensibleRoleField(UInt32 num);
	UInt32 GetUnlockedExtensibleRoleField();

	/**
	*@brief 增加解锁的可扩展角色栏位
	*/
	bool IncUnlockedExtensibleRoleField(UInt32 incNum = 1);

	/**
	*@brief 解锁可扩展角色栏位
	*/
	void UnlockExtensibleRoleField(ObjID_t costItemUId, UInt32 costItemDataId);

	/**
	*@brief 解锁可扩展角色栏位返回
	*/
	void SendUnlockExtensibleRoleFieldRespose(UInt32 errorCode);

	/**
	*@brief 获取拥有的角色栏位数
	*/
	UInt32 GetRoleFileds();

public:// 地精纪念币
	/**
	*@brief 设置获取地精纪念币数量
	*/
	void SetGnomeCoinNum(UInt32 num);
	UInt32 GetGnomeCoinNum() const;

	/**
	*@brief 增加地精纪念币数量
	*/
	void IncGnomeCoinNum(const std::string& reason, UInt32 incNum, bool checkCond = true);

public:// 账号相关金钱

	/*
	*@brief 判断能否扣钱
	*/
	bool CheckRmAccountMoney(ItemSubType moneyType, UInt32 cost);
	bool RemoveAccountMoney(const std::string& reason, ItemSubType moneyType, UInt32 num);

	/*
	*@brief 增加账号金钱
	*/
	bool IncAccountMoney(const std::string& reason, ItemSubType moneyType, UInt32 incNum);

	/*
	*@brief 判断能否扣账号计数
	*/
	bool CheckRmAccountCounterNum(AccountCounterType type, UInt64 cost);
	bool RemoveAccountCounterNum(const std::string& reason, AccountCounterType type, UInt64 num);

public:// 商城
	/**
	*@brief 商城购买获得物信息同步
	*/
	void MallBuyGotInfoSync(MallBuyGotType type);
	void MallBuyGotInfoSync();

public:// 运营活动
	/**
	*@brief 运营活动关闭
	*/
	void OnOpActivityClose(const OpActivityRegInfo& opAct);

public://网络
	/**
	 *@brief 发送到网关
	 */
	void SendToGate(Avalon::Protocol& protocol);

	/**
	 *@brief 发送到客户端
	 */
	void SendProtocol(Avalon::Protocol& protocol);
	void SendPacket(Avalon::Packet* packet);

	/**
	 *@brief 发送到所在场景
	 */
	void SendToScene(Avalon::Protocol& protocol);
	void SendToScene(Avalon::Packet* packet);

	/**
	*@brief 同步事件到Scene
	*/
	void SyncEventToScene(SyncEventType type, ObjID_t param1 = 0, ObjID_t param2 = 0);

public://数据库相关
	void LoadData();
	void Saveback();

	void LoadGlobalInfo();
	void SaveTribeSkill();
	void SaveMallBuyInfo();
	void LoadAccountInfo();
	void LoadMallGiftPack();
	void LoadAccountActivityTask();
	void LoadMallItem();
	void LoadAccountShopItemRoleBuyRecord();

public:
	inline void SetRefTime(UInt64 time)	{ m_RefTime = time; }
	inline UInt64 GetRefTime()	{ return m_RefTime; }

public: //离线加载上下文
	void SetOfflineReplyCtx(WSPlayer* player, LoadType lt, PlayerOfflineLoadCallBack cb = nullptr);
	void ClearOfflineReplyCtx();
	void OfflineLoadCb();
	bool IsOfflineLoad();

public:
	// 安全锁
	void HandlerSecurityLockDataReq(const std::string& deviceID);
	void HandlerSecurityLockOpReq(UInt32 _type, const std::string& _passwd);
	void ChangeSecurityPasswdReq(const std::string& _oldPasswd, const std::string& _newPasswd);
	void HandlerCommonDeviceReq(UInt32 type, const std::string& deviceID);

	void SyncSecurityStatusToScene();
	void SyncSecurityStatusToScene(UInt32 _state);

	void SecurityLockForceCheck();
	void ClearSecurityLockNotifyClient();
	void NotifySecurityPasswdErrorNum();

	// 如果安全锁锁住，返回false，并通知客户端
	bool SecurityLockCheck();

public:
	// 设置当前可购买拍卖商品id
	void SetCanBuyAuctionGuids(const std::vector<ObjID_t>& guids);
	bool CheckCanBuyAuction(const ObjID_t& guid);
	void SetAuctionProhibitTimeStamp(UInt64 timeStamp) { m_AuctionProhibitTimeStamp = timeStamp; }
	UInt64 GetAuctionProhibitTimeStamp() { return m_AuctionProhibitTimeStamp; }

	void SetAuctionProhibitQueryPriceTimeStamp(UInt64 timeStamp) { m_AuctionProhibitQueryPriceListTimeStamp = timeStamp; }
	UInt64 GetAuctionProhibitProhibitTimeStamp() { return m_AuctionProhibitQueryPriceListTimeStamp; }
public:
	void SetGuildDungeonCD() { m_GuildDungeonCD = CURRENT_TIME.Sec(); }
	UInt64 GetGuildDungeonCD() { return m_GuildDungeonCD; }

	void SetGuildDungeonDebugDamage(UInt64 val) { m_DebugGuildDungeonMaxDamage = val; }
	UInt64 GetGuildDungeonDebugDamage() { return m_DebugGuildDungeonMaxDamage; }

public: // 游戏设置
	
	// 获取游戏设置的数据
	template<typename T>
	T GetGameSetValue(UInt32 type, UInt32 subType = 0);
	// 解析游戏设置数据
	void ParseGameSet(const std::string& gameSet);

public:	// 回归

	// 回归重置数据
	void ReturnRset();

	// 是否老兵回归
	bool IsVeteranReturn() const;
	// 老兵回归检查
	void OnCheckVeteranReturn();

	// 角色回归时间
	void SetRoleReturnTime(UInt32 time) { m_RoleReturnTime = time; }
	UInt32 GetRoleReturnTime() const { return m_RoleReturnTime; }

	// 回归首次登陆
	void SetReturnFirstLogin(UInt32 firstLogin) { m_ReturnFirstLogin = firstLogin; }
	bool IsReturnFirstLogin() { return m_ReturnFirstLogin != 0; }
	void UpdateReturnFirstLogin();

	// 回归周年称号
	void SetReturnYearTitle(UInt32 title) { m_ReturnYearTitle = title; }
	UInt32 GetReturnYearTitle() const { return m_ReturnYearTitle; }

	// 套装解锁情况
	void SetSuit(bool bo) { m_HaveSuit = bo; }
	bool GetSuit() const { return m_HaveSuit; }

	void SendAdventurePassStatus();

public: // 账号计数

	// 同步账号计数到
	void SyncAccountCounterToClient(UInt32 type);
	void SyncAccountCounterToScene(UInt32 type);
	void SyncAllAccountCounterToClient();
	void SyncCustomerAccountCounterToClient(const std::vector<UInt32>& types);

	// 同步账号数据到scene
	void SyncAccountDataToScene(UInt32 type);


public: //头衔
	PlayerTitleRoleMgr& GetTitleMgr() { return m_TitleMgr; }
	// 设置当前穿戴的头衔id
	void SetCurNewTitleGuid(UInt64 titleGuid) { m_curWearTitleGuid = titleGuid; }
	// 获取当前穿戴的头衔id
	UInt64 GetCurNewTitleGuid() { return m_curWearTitleGuid; }

public: //大富翁
	MonopolyInfo& GetMonopolyInfo();

public: //每日必做
	
	DailyTodoMgr& GetDailyTodoMgr() { return m_DailyTodoMgr; }

	DailyTodoMgr	m_DailyTodoMgr;

	/**
	*@brief 礼遇特权
	*/
	bool HaveGiftRightCard() const { return m_HaveGiftRight; }
	void SetHaveGiftRightCard(bool bo) { m_HaveGiftRight = bo; }

	/**
	*@brief 能否买礼遇特权卡
	*/
	bool CanBuyGiftRightCard() const { return m_CanBuyGiftRight; }
	void SetCanBuyGiftRightCard(bool bo) { m_CanBuyGiftRight = bo; }

	/**
	*@brief 能否买深渊特权卡
	*/
	bool CanBuyDungeonRightCard() const { return m_CanBuyDungeonRight; }
	void SetCanBuyDungeonRightCard(bool bo) { m_CanBuyDungeonRight = bo; }

public: // 其他角色基础信息
	 /**
	 *@brief 设置其他角色基础信息
	 */
	void SetOtherRoleBaseInfo(const std::vector<RoleBaseInfo>& info) { m_OtherRoleInfo = info; }
	const std::vector<RoleBaseInfo>& GetOtherRoleBaseInfo() const { return m_OtherRoleInfo; }

	/**
	 *@brief 是否有角色满足等级要求(包括自己)
	 */
	bool HasRoleLevelLimit(UInt16 level) const;

	/**
	 *@brief 账号下角色最高等级(包括自己)
	 */
	UInt32 MaxRoleLevel() const;

private:
	//所在场景连接
	Avalon::NetConnectionPtr	m_SceneConn;
	//网关连接id
	Avalon::NetConnectionPtr	m_Conn;

	//各场景服务器人数
	static std::map<UInt32, UInt32>	m_ScenePlayerNum;

	//账号id
	UInt32	m_AccId;
	//账号表guid
	ObjID_t	m_AccGuid;
	// 账号信息
	AccountInfo*	m_AccountInfo;

	//玩家状态
	UInt8	m_Status;
	//修改状态的时间
	UInt32	m_StatusTime;

	//是否通知场景离开游戏
	UInt16	m_LeaveTimeout;
	//上线时间(s)
	UInt64	m_OnlineTime;
	//下线时间
	UInt64	m_OfflineTime;
	//本服下线时间
	UInt64	m_LocalOfflineTime;
	//离线场景
	UInt32	m_OfflineScene;
	//离线位置
	CLPosition	m_OfflinePos;
	
	//领取离线邮件时间
	UInt64	m_OfflineMailTime;

	//区id
	UInt16	m_ZoneId;
	//原区id
	UInt32	m_SrcZoneId;
	//出生区
	UInt32	m_BornZoneId;

	//等级
	UInt16	m_Level;
	//职业
	UInt8	m_Occu;
	//觉醒
	UInt8	m_Awaken;
	//性别
	UInt8	m_Sex;
	// 头像框
	UInt32	m_HeadFrame;
	//qq特权信息;
	QQVipInfo	m_qqVipInfo;
	//战斗力
	UInt32	m_Power;

	//拍卖行刷新时间
	UInt32		m_AuctionRefreshTime;

	//拍卖行额外购买的栏位
	UInt32		m_AuctionAdditionBooth;

	//openid
	std::string	m_OpenId;
	//城市
	std::string	m_City;
	//ip
	std::string	m_Ip;
	//数值ip
	UInt32		m_IpValue;
	//平台
	std::string	m_Pf;
	//设备id
	std::string m_DeviceId;
	//设备型号
	std::string m_Model;
	//设备版本
	std::string m_DeviceVersion;

	//惩罚标志位
	UInt32	m_PunishFlag;
	//封号到期时间
	UInt32	m_ForbitLoginDueTime;
	//禁言到期时间
	UInt32	m_ForbitTalkDueTime;

	//角色创建时间
	UInt32	m_CreateTime;

	//加载项数
	UInt8	m_LoadDataCount;
	//加载序号
	UInt32	m_LoadDataSerial;

	//道具事务
	ItemTransaction*	m_pItemTrans;
	//道具事务超时计时
	UInt8			m_ItemTransTimeout;

	//邮箱
	MailBox			m_MailBox;
	//队伍ID
	UInt32			m_TeamId;

	//创建副本定时器（防异常）
	Avalon::SimpleTimer m_CreateCopyTimer;
	//招募间隔时间
	Avalon::SimpleTimer	m_RecruitTimer;

	//请求管理器
	RequestMgr		m_Requests;

	//关系管理器
	WSRelationMgr	m_RelationMgr;

	//同门关系管理器
	WSMasterSystem	m_MasterSys;

	//离线事件管理器
	OfflineEventMgr	m_OfflineEventMgr;

	//商城限时礼包管理器
	MallGiftPackMgr	m_MallGiftPackMgr;

	// 条件触发管理器
	CondActivateMgr m_CondActivateMgr;

	// 部落技能
	std::map<UInt16, UInt8>	m_mapTribeSkills;

	// 商城道具购买信息
	std::map<UInt32, UInt16>	m_mapMallBuyInfo;
	UInt32						m_uGSMallBuyInfo;

	//全服counter窗口是否打开
	std::map<UInt8, UInt8> m_mapOpenFlags;

	//逻辑定时事件
	UInt64		m_TickTime[LOGICTICK_MAX];

	//当前所在牧场
	Pasture*	m_pPasture;

    // 拍卖物品列表
    std::vector<ObjID_t>    m_auctoinList[AT_NUM];

	// 玩家所在场景
	SceneType	m_SceneType;
	UInt32		m_SceneId;

	UInt8		m_bDayChanged;
	UInt8		m_bLogicDayChanged;

	ObjID_t		m_GuildId;
	UInt32		m_GuildLvl;

	// 公会技能
	std::vector<SkillBase>	m_GuildSkills;

	// 外观
	PlayerAvatar			m_Avatar;

	// 拉取离线玩家数据类型 LoadType
	UInt32					m_LoadPlayerType;

	// 请求离线玩家的人
	ObjID_t					m_RequestTarget;

	// 离线加载完成回调函数
	PlayerOfflineLoadCallBack	m_playerOfflineLoadCb;

	//段位
	UInt32					m_SeasonLv;

	//是否匹配中
	bool					m_IsMatching;

	//匹配状态
	FriendMatchStatus		m_MatchStatus;

	//月卡过期时间
	UInt32					m_MonthCardExpireTime;

	// 数据被引用的时间
	UInt64					m_RefTime;

	// 师傅公告
	std::string				m_MasterNote;

	// 师傅是否收徒
	bool					m_IsRecvDisciple;

	//所在房间ID
	UInt32					m_RoomId;
	//上一次退出房间ID
	UInt32					m_QuitRoomId;
	//上一次退出房间时间
	UInt32					m_QuitRoomTime;

	//玩家关注的挖宝地图
	UInt32					m_DigMapId;

	//oppo用户琥珀等级
	UInt8					m_OppoAmberLevel;

	//角色充值总金额
	UInt32					m_totalPlayerChargeNum;

	//离线加载是否失败
	bool					m_loadOfflineDataFail;

	//是否在跨服场景
	bool					m_inCrossScene;

	//抗魔值
	UInt32					m_ResistMagic;

	// 战斗相关
	ObjID_t					m_RaceID;
	Avalon::SockAddr		m_RelayAddr;

	//个人活动
	PlayerActivityInfo<WSPlayer> m_PlayerActivityInfo;

	//33积分
	UInt32 m_ScoreWarLoseStreak;
	UInt32 m_ScoreWarWinStreak;
	UInt32 m_MatchScore;
	UInt32 m_ScoreWarBattleCount;

	// 当前在线的设备
	std::string m_curOnlineDev;

	// 当前可见购买的拍卖物品id
	std::vector<ObjID_t> m_AuctionIds;

	//拍卖行查询禁止时间戳
	UInt64				 m_AuctionProhibitTimeStamp;

	// 公会地下城挑战CD
	UInt64				 m_GuildDungeonCD;

	// 设置的公会地下城最大伤害值
	UInt64				 m_DebugGuildDungeonMaxDamage;

	// 角色回归时间
	UInt32				m_RoleReturnTime;
	// 回归首次登陆
	UInt32				m_ReturnFirstLogin;
	// 回归周年称号
	UInt32				m_ReturnYearTitle;
	//套装解锁情况
	bool m_HaveSuit;

	// 游戏设置
	std::map<UInt32, std::string>	m_GameSetMap;

	//角色头衔
	PlayerTitleRoleMgr	m_TitleMgr;

	//当前穿戴头衔GUID
	UInt64		m_curWearTitleGuid;

	//拍卖行查询禁止查询价格列表时间戳
	UInt64				 m_AuctionProhibitQueryPriceListTimeStamp;

	//礼遇特权
	bool					m_HaveGiftRight = false;

	//能否买礼遇特权卡
	bool					m_CanBuyGiftRight = false;	
	
	//能否买深渊特权卡
	bool					m_CanBuyDungeonRight = false;

	//其他角色基础信息
	std::vector<RoleBaseInfo>			m_OtherRoleInfo;


};

typedef CLVisitor<WSPlayer> WSPlayerVisitor;


/**
 *@brief 查询玩家数据返回
 */
class SelectRoleInfoCallback : public CLRecordCallback
{
public:
	SelectRoleInfoCallback(WSPlayer* player)
		:m_PlayerId(player->GetID()), m_LoadSerial(player->IncLoadDataCount()){}
	~SelectRoleInfoCallback(){}

public:
	virtual void OnFinished(WSPlayer* player) = 0;
	virtual void OnFailed(WSPlayer* player) = 0;

private:
	void OnFinished();

	void OnLoadFailed();

private:
	//玩家id
	ObjID_t m_PlayerId;
	//加载序号
	UInt8	m_LoadSerial;
};

#endif
