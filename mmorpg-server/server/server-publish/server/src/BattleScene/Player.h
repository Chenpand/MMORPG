#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <CLPlayerDataEntry.h>
#include <CLVipDefine.h>
#include <CLVipDataEntry.h>
#include <CLChatDefine.h>
#include <CLMaskProperty.h>
#include <CLRequest.h>
#include <map>
#include <CLMatchProtocol.h>
#include <CLSysNotify.h>
#include <CLPkLevelDataEntry.h>
#include <kingnet_analyzer.h>
#include <CLDeathTowerAwardDataEntry.h>
#include <AvalonDailyTimer.h>
#include <CLGuildDefine.h>
#include <CLGameSessionProtocol.h>
#include <CLBillingProtocol.h>
#include <CLRelationDefine.h>
#include <CLFigureStatueDefine.h>
#include <CLMoneyManageDefine.h>
#include <CLScoreWarDefine.h>
#include <CLActivityInfo.h>

#include "Creature.h"
#include "ItemMgr.h"
#include "TaskMgr.h"
#include "QuickBar.h"
#include "ItemCD.h"
#include "CounterMgr.h"
#include "ActionCounterMgr.h"
#include "RoutineMgr.h"
#include "InviteMgr.h"
#include "EquipShopMgr.h"
#include "DungeonMgr.h"
#include "Fatigue.h"
#include "ShopMgr.h"
#include "PkStatisticMgr.h"
#include "RetinueMgr.h"
#include "Vip.h"
#include "OpenJarMgr.h"
#include "DungeonMgr.h"
#include "SeasonLevel.h"
#include "PetMgr.h"
#include "MallCheckBuyMgr.h"
#include "AbnormalTransactionMgr.h"
#include "ActivityAttributeMgr.h"
#include "HeadFrameMgr.h"
#include <CLLostDungeonDefine.h>

class Trade;
class Team;
class QuickBuyTransaction;
class PlayerLoadingCallback;
class PlayerSenceShareData;

/**
 *@brief 玩家状态
 */
enum PlayerStatus
{
	PS_INIT,		//刚创建等待数据或注册，此状态不在world或场景
	PS_NORMAL,		//正常状态，即在场景和world中
	PS_LEAVEGAME,	//离开游戏计时阶段
    PS_RECONNECT,   //重连
	PS_FINAL,		//存档
	PS_DESTORY,		//待摧毁
	PS_NUM
};

/**
*@brief 武道大会状态
*/
enum WudaoStatus
{
	// 初始状态（未参加）
	WUDAO_STATUS_INIT,
	// 进行中
	WUDAO_STATUS_PLAYING,
	// 已完成
	WUDAO_STATUS_COMPLETE,
};

/**
*@brief 开始判断触发条件的玩家等级
*/
#define PLAYER_LEVEL_ON_BEGIN_CHECK_ACTIVATE_COND 7

/**
*@brief 玩家视野优先级
*/
struct PlayerSightPriority
{
	ObjID_t	id;
	UInt16	level;
	bool	isFriend;

	bool operator<(const PlayerSightPriority& other) const
	{
		if (isFriend != other.isFriend)
		{
			return isFriend ? true : false;
		}

		if (level != other.level)
		{
			return level > other.level;
		}

		return id < other.id;
	}
};

#define PLAYERINFO(T) \
	"accid(" << T->GetAccID() << ") playerid(" << T->GetID() << ") occu(" << T->GetOccu() << ")"

/**
 *#brief 玩家类  notedy by aprilliu, Player是在scene server收到world server发来的SceneEnterGameReq时进行创建的， Object_ID就是创建的角色id (一个guid)
 */
class Player : public Creature
{
	AVALON_DEFINE_OBJECTPOOL(Player, Avalon::NullMutex)  //by aprilliu,底层使用Avalon::ObjectPool来存储Player对象，同时使用PlayerMgr 来索引Player对象（std::map 以Object_t为key）

	typedef std::map<ObjID_t, UInt16>	FriendMap;

public:
	//玩家下线延迟, 30秒钟
	const static UInt32 LEAVEGAME_TIMEOUT = 3 * 1000;
	//财产锁定时间
	const static UInt8	ASSET_LOCK_TIME = 5;
	//逻辑跨天小时
	const static UInt8 LOGIC_DAYCHANGE_HOUR = 6;

public:
	Player();
	~Player();

public:
	/**
	 *@brief 初始设置
	 */
	bool SetupBase();

	/**
	 *@brief 设置获取状态
	 */
	void SetGameStatus(UInt8 status)
	{ 
		m_GameStatus = status;
		m_StatusTime = (UInt32)CURRENT_TIME.Sec();
	}
	UInt8 GetGameStatus() const { return m_GameStatus; }
	UInt32 GetStatusTime() const { return m_StatusTime; }

	/**
	 *@brief 设置获取网关id
	 */
	bool SetGateID(UInt32 nodeid);
	UInt32 GetGateID() const;

	/**
	 *@brief 设置获取账号id
	 */
	void SetAccID(UInt32 accid){ m_AccId = accid; }
	UInt32 GetAccID() const{ return m_AccId; }

	/**
	*@brief 是否手机绑定
	*/
	void SetPhoneBind(bool bind){ m_IsPhoneBind = bind; }
	bool IsPhoneBind() const{ return m_IsPhoneBind; }

	/**
	*@brief 设置获取账号guid
	*/
	void SetAccGuid(ObjID_t guid){ m_AccountGuid = guid; }
	ObjID_t GetAccGuid() const{ return m_AccountGuid; }

	/**
	*@brief 主城id（赛丽亚房间）
	*/
	void SetMajorCityID(UInt32 id){ m_MajorCityId = id; }
	UInt32 GetMajorCityID() const{ return m_MajorCityId; }


	/**
	 *@brief 获取base64编码
	 */
	const char* GetBase64ID() const;

	SceneObjectType GetType() const { return SOT_PLAYER; }

	/**
	 *@brief 经验相关
	 */
	UInt64 GetExp() const { return m_Exp; }
	UInt64 DecExp(const char* reason, UInt64 exp);
	void   IncExp(const char* reason, UInt64 exp);
	bool   AddExpFromRewardAdapter(UInt32 key, UInt16 subKey);

	/**
	*@brief 根据当前等级的经验添加百分比
	*/
	void   AddExpByLevelPercent(const char* reason, UInt32 percent);

	/**
	*@brief 根据等级查看需要的经验
	*/
	UInt64 GetLevelExp(UInt8 level);

	/**
	 *@brief 判断是否满级(经验都满了)
	 */
	bool IsFullLevel() const;

	/**
	*@brief 设置、获得经验加成buff
	*/
	void SetExpAddBuff(Buff *pBuff) { m_pExpAddBuff = pBuff; }
	Buff* GetExpAddBuff() { return m_pExpAddBuff; }

	/**
	 *@brief 获取职业
	 */
	UInt8 GetOccu() const { return m_Occu; }
	UInt8 GetPreOccu() const { return m_PreOccu; }
	UInt8 GetMainOccu() const { return m_Occu / 10; }
	UInt8 GetOrigOccu() const { return GetMainOccu() * 10; }
	bool IsSameOccu(int occu) { return m_Occu / 10 == occu / 10; }	//by huchenhui
	void ChangeOccu(UInt8 occu);		//by huchenhui
	bool IsInitOccu() { return m_Occu % 10 == 0; }		//by huchenhui 是否初始职业
	Occupation GetBaseOccu() const { return (Occupation)((m_Occu / 10) * 10); }
	Occupation GetOccupation() { return (Occupation)(m_Occu.GetDBValue()); }
	bool IsChangeOccu(UInt8 occu)	{ return (occu % 10) == 0 ? false : true; }			//是否已转职

	/**
	*@brief 觉醒相关
	*/
	UInt32 Awaken();
	void SetAwaken(AwakenStatus awaken) { m_Awaken = awaken; }
	UInt8 GetAwaken() { return m_Awaken; }

	/**
	*@brief 计算出生城市（赛丽亚房间）
	*/
	UInt32 GetBirthCity();

	/**
	 *@brief 判断是否拥有某种gm权限
	 */
	void SetGmAuthority(UInt8 authority){ m_GmAuthority = authority; }
	bool HasGmAuthority(UInt8 authority) const{ return m_GmAuthority >= authority; }

	/**
	 *@brief 设置判断客户端选项
	 */
	void SetClientOption(UInt8 option){ m_ClientOption = option; }
	bool HasClientOption(UInt8 flag) const{ return (m_ClientOption & flag) != 0; }

	/**
	*@brief 拍卖行刷新时间
	*/
	void SetAuctionRefreshTime(UInt32 time) { m_AuctionRefreshTime = time; }
	UInt32 GetAuctionRefreshTime() const { return m_AuctionRefreshTime; }

	/**
	*@brief 拍卖行额外购买的栏位
	*/
	void SetAuctionAdditionBooth(UInt32 num) { m_AuctionAdditionBooth = num; }
	UInt32 GetAuctionAdditionBooth() const { return m_AuctionAdditionBooth; }

	/**
	*@brief Oppo Vip等级
	*/
	UInt8 GetOppoVipLevel() const { return m_OppoVipLevel; }
	void SetOppoVipLevel(UInt8 level) { m_OppoVipLevel = level; }

	/**
	 *@brief 获取上线时间
	 */
	UInt64 GetOnlineTime() const{ return m_OnlineTime; }
	UInt64 GetOfflineTime();

	//获得离线的天数差,不满一天为一天,例如:1号下线,4号上线,结果为3天
	UInt32 GetOfflineDay();

	//获得指定时刻的离线天数,不满一天为零天
	UInt32 GetOfflineDay(UInt32 Hour);

	//获得指定时间段内的离线天数,不满一天为零天
	UInt32 GetIntervalDay(UInt64 startTime, UInt64 endTime, UInt32 hour);

	////获得指定时刻的离线天数,不满一天为零天, updateTime:上一次更新的时间
	UInt32 GetOfflineDay(UInt32 Hour, UInt32 updateTime);

	UInt32 GetDayOnlineTime();
	/**
	 *@brief 设置获取本服存档时间
	 */
	void SetLocalSavebackTime(UInt64 time){ m_LocalSavebackTime = time; }
	UInt64 GetLocalSavebackTime() const{ return m_LocalSavebackTime; }

	/**
	 *@brief 同步属性变化
	 */
	void SyncProperty();

	/**
	 *@brief 设置提升等级
	 */
	void SetTestLevel(UInt16 level);
	void Levelup();
	void LevelDown();

	/**
	 *@brief 设置获取城市
	 */
	void SetCity(const std::string& city){ m_City = city; }
	const std::string& GetCity() const{ return m_City; }

	/**
	 *@brief 设置获取ip
	 */
	void SetIp(const std::string& ip){ m_Ip = ip; }
	const std::string& GetIp() const{ return m_Ip; }

	/**
	 *@brief 设置繁忙时间
	 */
	void SetBusyTime(){ m_BusyTime = CURRENT_TIME.MSec(); }
	UInt64 GetBusyTime(){ return m_BusyTime; }

	/**
	 *@brief 获取创建时间
	 */
	UInt32 GetCreateTime() const{ return m_CreateTime; }
	UInt32 GetCreateFromNowOn();
	UInt32 GetCreateFromNowOnDay();
	//创建到现在的天数，按6点区分
	UInt32 GetCreateFromNowOnLogicDay();

	/**
	 *@brief 设置来源
	 */
	void SetSource1(const std::string& source1){ m_Source1 = source1; }
	void setSource2(const std::string& source2){ m_Source2 = source2; }

	/**
	 *@brief 获取区id
	 */
	UInt16 GetZoneId() const{ return m_ZoneId; }

	/**
	 *@brief 设置获取原区id
	 */
	void SetSrcZoneId(UInt16 zoneid){ m_SrcZoneId = zoneid; }
	UInt16 GetSrcZoneId() const{ return m_SrcZoneId; }

	/**
	*@brief 设置获取出生区id
	*/
	void SetBornZoneId(UInt32 zoneId) { m_BornZoneId = zoneId; }
	UInt32 GetBornZoneId() const { return m_BornZoneId; }

	/**
	 *@brief 离开游戏, 注意调用此接口会释放对象
	 */
	void OnLeaveTimeout();
	void LeaveGame();
	void UnRegisterWorldPlayer();
	void LeaveGameForTour(UInt32 dstZoneId, UInt32 mapId, const Avalon::SockAddr& addr, UInt8* key);

	//逻辑下线,玩家请求下线还没有真正释放时的处理
	void OnLogicOffLine();

	/**
	 *@brief 踢掉网关连接
	 */
	void KickOff(UInt32 erroCode = 0);

	/**
	 *@brief 设置行为标志
	 *@return true 首次设置 false 原来已设置
	 */
	bool SetActionFlag(UInt32 flag);
	bool TestActionFlag(UInt32 flag) const;

	/**
	*@brief 增加玩家等级
	*/
	void IncLevel();

	/**
	 *@brief 同步请求
	 */
	void SyncRequest(UInt8 type, Player* pRequester, const std::string& param = "");

	/**
	 *@brief 设置获取openid
	 */
	void SetOpenId(const std::string& openid){ m_OpenId = openid; }
	const std::string& GetOpenId() const{ return m_OpenId; }
	void ClearOpenIdDirtyFlag() { m_OpenId.ClearDirty(DB_SYNC_TYPE); }

	/**
	 *@brief 设置邀请者
	 */
	void SetInviter(UInt32 inviter){ m_Inviter = inviter; }

	/**
	 *@brief 设置获取平台id
	 */
	void SetPf(const std::string& pf){ m_Pf = pf; }
	const std::string& GetPf() const{ return m_Pf; }
	const char* ScriptGetPf() const{ return m_Pf.c_str(); }
	void ClearPfDirtyFlag() { m_Pf.ClearDirty(DB_SYNC_TYPE); }

	/**
	*@brief 设置获取设备id
	*/
	void SetDeviceId(const std::string& did) { m_DeviceId = did; }
	const std::string& GetDeviceId() const{ return m_DeviceId; }
	const char* ScriptGetDeviceId() const { return m_DeviceId.c_str(); }

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
	*@brief 客户端是否在loading中
	*/
	void SetClientLoading(bool loading) { m_IsClientLoading = loading; }
	bool IsClientLoading() const { return m_IsClientLoading; }

	/**
	 *@brief 获取创角后连续登陆天数                                                                     
	 */
	UInt32 GetContinuousOnlineDays() const{ return m_ContinuousDays; }

	/**
	 *@brief 跨服                                                                     
	 */
	void ChangeZone(UInt32 zoneId, UInt32 mapId);
	void ReturnZone();

	/**
	*@brief 通知world一些事件
	*/
	void SyncEventToWorld(SyncEventType type, ObjID_t param1);

	/**
	*@brief 获取玩家标签信息
	*/
	void GetPlayerLabelInfo(PlayerLabelInfo& info);

	/**
	*@brief 设置获取冒险队名
	*/
	void SetAdventureTeamName(const std::string& name) { m_AdventureTeamName = name; }
	
public: //惩罚相关
	/**
	 *@brief 设置判断惩罚标志
	 */
	void SetPunishFlag(UInt32 flag){ m_PunishFlag |= (1 << flag); }
	bool CheckPunishFlag(int type) const{ return (m_PunishFlag & ( 1 << type)) != 0; }

	/** 
	 *@brief 设置禁言到期时间
	 */
	void	SetForbitTalkDueTime(UInt32 duetime){ 
		if (m_ForbitTalkDueTime < duetime)
		{
			m_ForbitTalkDueTime = duetime;
		}
	}
	UInt32	GetForbitTaleDueTime() const{ return m_ForbitTalkDueTime; }
	void	ClearForbitTalkDueTime()	{ m_ForbitTalkDueTime = 0; }

public:	//vip相关
	/**
	 *@brief 设置获取vip等级
	 */
	void SetVipLvl(UInt8 level);
	UInt8 GetVipLvl();
	UInt32 GetVipExp();

	/**
	*@brief 总的充值金额
	*/
	void InitTotalChargeNum(UInt32);
	void AddTotalChargeNum(UInt32 num);
	
	/**
	*@brief 总的充值金额（角色）
	*/
	UInt32 GetTotalChargeNumOnRole() const { return m_TotleChargeNum; }

	/**
	 *@brief 设置vip体验buff                                                                     
	 */
	void SetVipBuff(Buff* pVipBuff){ m_pVipBuff = pVipBuff; }
	Buff* GetVipBuff() const{ return m_pVipBuff; }

	/**
	 *@brief 设置获取vip到期时间
	 */
	void SetVipDueTime(UInt32 dueTime){ m_VipDueTime = dueTime; }
	UInt32 GetVipDueTime() const{ return m_VipDueTime; }

	/**
	 *@brief 设置qq特权信息
	 */
	void SetQQVipInfo(const QQVipInfo& info){ m_qqVipInfo = info; }
	const QQVipInfo& GetQQVipInfo() const{ return m_qqVipInfo; }

	//增加VIP经验
	void AddVipExp(const char* reason, UInt32 exp) { m_Vip.AddExp(reason, exp); }

	//获取权限的值
	void GetVipValue(VipPrivilegeType type, UInt32& value) { m_Vip.GetValue(type, value); }

	//获取权限的值
	UInt32 GetVipValue(VipPrivilegeType type) { UInt32 value = 0; m_Vip.GetValue(type, value); return value; }

	//获取权限的值
	void GetVipValue(VipPrivilegeType type, bool& value) { m_Vip.GetValue(type, value); }

	//获取权限的值
	UInt32 GetVipValueById(UInt32 id) { return m_Vip.GetValueById(id); }


	//购买礼包
	UInt32 BuyGiftItem(UInt8 vipLevel) { return m_Vip.BuyGiftItem(vipLevel); };

	//初始化服务器
	void InitVip(UInt8 level, UInt32 exp) { return m_Vip.Init(level, exp); }
	void AdjestChargeInfo(UInt8 vipLevel, UInt32 vipExp, UInt32 point, UInt32 totalChargeNum);

public://切换场景相关
	/**
	*@brief 能否进入场景
	*/
	bool CanEnterMap(UInt32 uMapId);

	bool IsInDungeon();

	bool IsInPK();

    /**
    *brief 场景坐标
    */
    const ScenePos GetScenePos() const;
    void SetScenePos(UInt32 x, UInt32 y);
    void UpdateScenePosition();
    void RequestStopMoving(ScenePos pos);
    void AdjuestScenePosition(int &x, int &y);

    /**
    *brief 请求切换场景
    */

    void OnSceneChangeReq(UInt32 curDoorId, UInt32 dstSceneId, UInt32 dstDoorId);

    /**
    *brief 运动方向
    */
    const SceneDir GetSceneDir() const { return m_SceneDir; }
    void SetSceneDir(SceneDir dir) { m_SceneDir = dir; }

    /**
    *brief 运动
    */
    bool CanMoveToTarget(const ScenePos& pos) const;
    const bool IsRuning() const { return m_Runing; }
    void SetRuning(bool runing) { m_Runing = runing; }

    /**
    *brief 上一次移动时间
    */
    void SetLastMoveTime(Avalon::Time now) { m_LastMoveTime = now; }
    Avalon::Time GetLastMoveTime() const { return m_LastMoveTime; }

public://道具相关

	//加载道具数据
	bool LoadItem(Item* item, bool newPos);

	//使用道具
	UInt32 UseItem(ObjID_t uid, UInt8 useAll, UInt32 param1, UInt32 param2);
	//快捷使用道具
	UInt32 QuickUseItem(UInt8 idx, UInt32 dungenID);

	//出售道具
	UInt32 SellItem(ObjID_t id, UInt16 num = 1);
	//批量出售道具
	UInt32 SellItemBat(std::vector<UInt64> itemUids);
	//存仓库
	UInt32 PushStorage(ObjID_t id, UInt16 num = 1, UInt8 storageType = PACK_STORAGE);
	//取仓库
	UInt32 PullStorage(ObjID_t id, UInt16 num = 1);
	//一键存仓库
	UInt32 OneKeyPushStorage();

	Item* FindCarryOnItem(ObjID_t id);
	Item* FindItem(ObjID_t id);
	Item* FindItem(const ItemPos &pos);
	
	void GetEquips(std::vector<Item*>& equips) const;
	void GetSecondWeapons(std::vector<Item*>& weapons);
	Item* GetEquip(UInt8 subType);

	void GetRaceItem(std::vector<RaceItem>& items);
	void GetRaceBuffs(std::vector<RaceBuffInfo>& buffs);
	void GetRaceRetinues(std::vector<RaceRetinue>& retinues);
	void GetRacePets(std::vector<RacePet>& pets);
	
	//减少道具数量 当叠加数为0时 对象删除
	UInt32 ReduceItemNum(const char* reason, Item* item, UInt16 num=1);
	void VisitItems(UInt8 pack, ItemVisitor& visitor);
	
	//获取剩余背包格子
	UInt32 GetPackGridLeft() const{ return m_ItemMgr.GetPackage(PACK_MATERIAL)->GetCapacity(); }
	UInt32 GetPackGridLeftByPack(PackType pack) const{ return m_ItemMgr.GetPackage(pack)->GetCapacity(); }
	UInt32 GetPackItemNum(UInt8 pack) const;

	//整理道具
	void TrimItems(UInt8 pack){ m_ItemMgr.TrimItems(pack); }
	//批量分解装备
	UInt32 DecomposeEquip(const char* reason, std::vector<ObjID_t>& uidList, ItemRewardVec& getItems, bool sendMail = false);
	//单个分解装备
	UInt32 DecomposeEquip(ObjID_t uid);
	//强化装备
	UInt32 StrengthenEquip(ObjID_t equipUid, bool useUnbreak, ObjID_t strTicket);
	//模拟强化for test
	bool SimStrength(UInt32& strLv, UInt32 equipLv, UInt32 color, UInt32 subType, UInt32 destStrLv, 
		UInt32& fix1Ct, UInt32& fix2Ct, UInt32& strNum, UInt32& failNum, UInt32 lv2failNum[], 
		UInt32& useProtectNum, UInt32& costGold, UInt32& costColor, UInt32& costColorless);

	//装备封装
	UInt32 CheckSealEquip(ObjID_t equipUid, UInt16& needNum, UInt32& needId);
	UInt32 SealEquip(ObjID_t equipUid);
	//装备品级重置
	UInt32 ReMakeEquipQlv(ObjID_t equipUid, UInt8 autoUsePoint, UInt8 usePerfect); 
	//装备转移
	UInt32 TransferEquip(ObjID_t equipUid, ObjID_t transferId);
	//时装合成
	UInt32 FashionCompose(ObjID_t fashionA, ObjID_t fashionB);
	void SendFashionComposeBack(UInt32 code, UInt8 type, UInt32 commonId, UInt32 advanceId, UInt32 wingId);
	//时装节时装合成返回
	void SendFashionCompBackChangeActive(UInt32 ret, UInt8 type, UInt32 commonId, UInt32 advanceId, bool allMerged);

	//装备制作
	UInt32 EuqipMake(UInt32 equipId);

	/**
	 *@brief 设置获取道具组使用时间
	 */
	void SetItemUseTime(UInt8 id, UInt64 usetime, UInt32 maxTime){ m_ItemCD.SetUseTime(id, usetime, maxTime); }
	UInt64 GetItemUseTime(UInt8 id) const{ return m_ItemCD.GetUseTime(id); }

	/**
	 *@brief 检查删除道具
	 */
	UInt32 GetItemNum(UInt32 id);
	UInt32 GetItemNumWithEqualItem(UInt32 id);  
	bool RemoveItem(const char* reason, UInt32 id, UInt32 num);
	Int32 RemoveItemWithEqualItem(const char* reason, UInt32 id, UInt32 num);
	UInt32 GetBindItemNum(UInt32 id);
	UInt32 GetNoBindItemNum(UInt32 id);
	void RemoveNoBindItem(const char* reason, UInt32 id, UInt32 num);

	/**
	*@brief 优先移除评分较低的道具
	*/
	bool RemoveLessScoreItem(const char* reason, UInt32 id, UInt32 num);

	/**
	 *@brief 移除道具，此接口不会删除item对象
	 */
	void RemoveItemDirect(const char* reason, Item* item);

	/**
	 *@brief 检查添加道具
	 */
	bool CheckAddItem(UInt32 id, UInt16 num);


	UInt16 AddItem(const char* reason, UInt32 id, UInt32 num, UInt8 qualityLv = 0, UInt8 strenth = 0, bool getNotify = true);

	UInt16 AddItemWithCallBack(const char* reason, UInt32 id, UInt32 num,
		const std::function<bool(Item&)>& initCallback,
		const std::function<void(Item&)>& makedCallback,
		UInt8 qualityLv = 0, UInt8 strenth = 0, bool getNotify = true);

	UInt16 AddItemDirect(const char* reason, Item* item, bool notify = true, bool mailGive = false);


	//for test
	void EquipItem(UInt32 id);

	/**
	 *@brief 检查给予奖励
	 */
	RewardGroup* GetRewardGroup();
	RewardGroup* GetRewardGroupNoClear();
	inline void ClearRewardGroup() { m_RewardGroup.ClearRewards(); };

	//检查背包是否能加的下,srcItem是用于类似礼包逻辑,表示生成源道具, srcNum是对应数量,用来检测源道具被删除后添加的边界情况
	//注意该方法检查过后要先删除再添加,否则会有可能在边界情况下添加失败
	UInt32 CheckAddRewards(RewardGroup* group, Item* srcItem = NULL, UInt16 srcNum = 0);
	UInt32 CheckAddLimitItems(RewardGroup* group);
	//背包满需要走邮件的可以直接使用AddRewards
	void AddRewardsBase(const char* reason, RewardGroup* group, bool notice, UInt32 validDay, bool sendMail, bool checkGetLimit, bool openJar, bool mailGive = false, bool notifyNewItem = true);
	void AddRewards(const char* reason, RewardGroup* group, bool notice = false, UInt32 validDay = MAIL_VALID_DAY, bool sendMail = true, bool checkGetLimit = false);
	void AddJarRewards(const char* reason, RewardGroup* group, bool notice = false, UInt32 validDay = MAIL_VALID_DAY);
	void AddRewardsByMailGive(const char* reason, RewardGroup* group, bool notice = false, UInt32 validDay = MAIL_VALID_DAY, bool sendMail = true, bool checkGetLimit = false);
	UInt32 CheckAddRewardsScript(RewardGroup* group);
	void AddRewardsNotNotify(const char* reason, RewardGroup* group, bool notice = false, UInt32 validDay = MAIL_VALID_DAY, bool sendMail = true, bool checkGetLimit = false);
	/**
	*@brief 根据来源获取邮件信息
	*/
	void GetMailInfoByReason(const char* reason, std::string& senderName, std::string& title, std::string& content);

	/**
	 *@brief 金币相关
	 */
	void AddGold(const char* reason, UInt32 num);
	UInt32 GetGold() const { return m_Gold; }
	UInt32 RemoveGold(const char* reason, UInt32 num);

    /**
    *@brief 勇者之魂相关
    */
    void AddWarriorSoul(const char* reason, UInt32 num);
    UInt32 GetWarriorSoul() const { return m_WarriorSoul; }
    void RemoveWarriorSoul(const char* reason, UInt32 num);

	/**
	*@brief 决斗币相关
	*/
	void AddPkCoin(const char* reason, UInt32 num);
	UInt32 GetPkCoin() const { return m_PkCoin; }
	void RemovePkCoin(const char* reason, UInt32 num);
	UInt32 AddPkCoinFromRace(UInt32 num);
	UInt32 AddPkCoinFromActivity(UInt32 num);

	/**
	*@brief 罐子积分
	*/
	void AddGoldJarPoint(const char* reason, UInt32 num);
	void AddMagJarPoint(const char* reason, UInt32 num);
	inline UInt32 GetGoldJarPoint()	{ return m_GoldJarPoint.GetDBValue(); }
	inline UInt32 GetMagJarPoint()	{ return m_MagJarPoint.GetDBValue(); }
	void RemoveGoldJarPoint(const char* reason, UInt32 num);
	void RemoveMagJarPoint(const char* reason, UInt32 num);

	/**
	*@brief 每日从战斗中获得决斗币的最大数量
	*/
	UInt32 DailyMaxPkCoinFromRace();

	/**
	*@brief 每日从活动中获得决斗币的最大数量
	*/
	UInt32 DailyMaxPkCoinFromActivity();

	/**
	*@brief 决斗币相关
	*/
	void AddReviveCoin(const char* reason, UInt32 num);
	UInt32 GetReviveCoin() const { return m_reviveCoin; }
	void RemoveReviveCoin(const char* reason, UInt32 num);

	/**
	*@brief SP相关
	*/
	void AddSP(const char* reason, UInt32 num);
	void AddPveSP(const char* reason, UInt32 num);
	void AddPvpSP(const char* reason, UInt32 num);
	UInt32 GetPveSP() const { return m_SP; }
	UInt32 GetPvpSP() const { return m_PvpSP; }
	void SetPveSP(UInt32 num) { m_SP = num; }
	void SetPvpSP(UInt32 num) { m_PvpSP = num; }
	bool RemovePveSP(const char* reason, UInt32 num);
	bool RemovePvpSP(const char* reason, UInt32 num);
	UInt32 GetAllSP();

	/**
	*@brief 疲劳值相关
	*/
	void AddFatigue(const char* reason, UInt16 num, bool notify = false) { m_Fatigue.AddFatigue(reason, num, notify); }
	void RemoveFatigue(const char* reason, UInt16 num) { m_Fatigue.RemoveFatigue(reason, num); }
	UInt16 GetFatigue() { return m_Fatigue.GetFatigue(); }
	UInt16 GetUseFatigue() { return m_Fatigue.GetUseFatigue(); }
	UInt16 GetLostFatigue() { return m_Fatigue.GetLostFatigue(); }
	UInt32 GetLastFatigue()	{ return m_Fatigue.GetLastFatigue(); }
	UInt16 GetMaxFatigue()	{ return m_Fatigue.GetMaxFatigue(); }

	/**
	 *@brief 绑金相关
	 */
	void AddBindGold(const char* reason, UInt32 num);
	UInt32 GetBindGold() const { return m_BindGold; }
	void RemoveBindGold(const char* reason, UInt32 num, UInt32 itemId = 0, UInt32 itemNum = 0);

	/**
	 *@brief 点券相关
	 */
	void AddPoint(const char* reason, UInt32 num);
	UInt32 GetPoint() const { return  m_Point; }
	void RemovePoint(const char* reason, UInt32 num, UInt32 itemId = 0, UInt32 itemNum = 0);
	void InitPoint(UInt32 point) { m_Point = point; }

	/**
	 *@brief  绑定点券相关                                                                 
	 */
	void AddBindPoint(const char* reason, UInt32 num);
	UInt32 GetBindPoint() const { return m_BindPoint; }
	void RemoveBindPoint(const char* reason, UInt32 num);

	/**
	 *@brief 增加钱类道具
	 */
	bool CheckAddMoneyItem(UInt8 type, UInt32 num);
	void AddMoneyItem(const char *reason, UInt8 type, UInt32 num);

	void SetCustomField(UInt32 field) { m_CustomField = field; }

	/**
	 *@brief 获取金币（包括绑定金币）
	 */
	UInt32 GetGoldBoth() const { return  m_BindGold + m_Gold; }
	/*
	 *@brief 移除金币（优先绑定金比）
	 *@return true 移除了绑定金币 false 没有
	 */
	bool RemoveGoldBoth(const char* reason, UInt32 num);

	//判断能否扣钱
	bool CheckRmMoney(ItemSubType moneyType, UInt32 cost);
	bool CheckRmMoney(ItemDataEntry* data, UInt32 cost);
	bool RemoveMoney(const char* reason, ItemSubType moneyType, UInt32 num);

	//点券相关
	UInt32 GetPointBoth() const { return m_BindPoint + m_Point; }
	bool RemovePointBoth(const char* reason, UInt32 num);

	/**
	 *@brief 发送支付结果
	 */
	void SendBillingResult(UInt32 billingId, UInt8 result);

	/**
	 *@brief 扩充包裹大小
	 */
	UInt32 EnlargePackage();
	UInt32 EnlargeStorage(UInt8 count = 0);
	void InitPackage();
	void InitStoragePack(UInt8 sz);

	// 已经扩展的背包格子
	UInt32 GetEnlargePackageSize();

	// 已经扩展的仓库格子
	UInt32 GetEnlargeStoragePackSize();

	/**
	 *@brief 检查是否可进行仓库操作
	 */
	bool CheckUseStorage();

	//开罐子
	void SendMagicJarResult(const char* bonusPool, UInt32 getBonusID, UInt16 getNum);

	//附魔
	UInt32 AddMagic(Item* card, Item* item);
	UInt32 ComposeMagicCard(ObjID_t card_1, ObjID_t card_2, UInt32& cardId);

	/**
	*@brief 宝珠
	*/
	UInt32 AddPreciousBeadToItem(Item* preciousBead, Item* item);

	//获取装备数量 by huchenhui 2016.09.13
	UInt32 GetEquipSize(UInt16 level, UInt8 quality, ItemSubType subType = ItemSubType::ST_ITEM_INVALID) { return m_ItemMgr.GetEquipSize(level, quality, subType); }

	//获取传承装备数量 by huchenhui 2016.09.13
	UInt32 GetGreenEquipSize(UInt16 level) { return m_ItemMgr.GetGreenEquipSize(level); }

	//获取传承装备数量 by huchenhui 2018.03.20
	UInt32 GetFashionSize(UInt8 quality, ItemSubType subType = ItemSubType::ST_ITEM_INVALID) { return m_ItemMgr.GetFashionSize(quality, subType); }

	inline ItemMgr& GetItemMgr() { return m_ItemMgr; }
	inline OpenJarMgr& GetJarMgr()	{ return m_JarMgr; }

	// 计算移速
	void CountMoveSpeed();

	// 开罐子
	UInt32 OpenJar(UInt32 jarId, UInt8 comboNum, UInt32 opActId);

	// 记录获得天空套
	void RecordGetSkyFashion(UInt32 fashionId);

	// 重置获得天空套
	void ResetGetSkyRecord(UInt32 fashionId);

	// 是否获得过该天空套
	bool HasGetSkyFashion(UInt32 fashionId);

	/**
	*@brief 安全锁
	*/
	void SetSecurityStatus(UInt32 _status) { m_SecurityStatus = _status; }
	UInt32 GetSecurityStatus() { return m_SecurityStatus; }

	bool SecurityLockCheck(bool _isNotify = true);
	bool SecurityLockCheckByColor(std::vector<ObjID_t>& uidList, UInt32 _color);

private:
	UInt32 GetIncome(UInt32 type);

public://开罐子
	bool LoadJarItem(JarItem* jarItem);
	void InitJarMgr();
	void OpenJarSample(UInt32 type, UInt32 combo, UInt32 num);

	inline UInt32 GetLastGoldJarPtRstTime()	{ return m_LastResetGoldJarPointTime.GetDBValue(); }
	inline void SetLastGoldJarPtRstTime(UInt32 time)	{ m_LastResetGoldJarPointTime = time; }
	inline UInt32 GetLastMagJarPtRstTime()		{ return m_LastResetMagJarPointTime.GetDBValue(); }
	inline void SetLastMagJarPtRstTime(UInt32 time)		{ m_LastResetMagJarPointTime = time; }


public://buff相关
	bool AddBuff(UInt32 buffId);

public: //活动任务相关
	UInt32	GetAllEquipLvMin();							//获得全身装备等级下限(1-9号部位,不包含称号)
	UInt32	GetAllEquipQualityMin();					//获得全身装备品质下限(1-9号部位,不包含称号)
	UInt32	GetWearFashionNum();						//获得已穿戴时装数量
	UInt8	GetEquipNumByStrLv(UInt32 strLv);			//获得指定强化等级装备数量(1-9号部位,不包含称号)
	UInt8	GetStrMaxLv(UInt8 num, UInt8 needStrLv);								//获得装备中的最大强化等级
	UInt8	GetStrMaxLvAll();								//获得装备中和背包里的最大强化等级
	UInt8	GetEquipNumByLv(UInt32 lv);					//获得指定等级装备数量(1-9号部位,不包含称号)
	UInt32	GetTitleNum();								//获得称号数量
	UInt32	GetWearEquipNumByQua(UInt8 color);			//获得指定品质以上的已穿戴装备数量
	UInt32	GetSignInCount();							//获取累积签到次数

public://账号相关
	void SyncAccountInfo(AccountInfoType type, UInt32 value);

public://任务相关
	/**
	 *@brief 查找
	 */
	Task* FindTask(UInt32 id);

	Task* FindAchievementTaskTask(UInt32 id);
	/**
	 *@brief 接取、提交、放弃一个任务
	 */
	bool AcceptTask(UInt32 id, UInt8 quality, bool bForce = false);
	bool ScriptAcceptTask(UInt32 id){ return AcceptTask(id, 0); }
	bool SubmitTask(UInt32 id, bool force = false);
	void AbandonTask(UInt32 id);

	/**
	*@brief 活动任务
	*/
	bool SubmitActiveTask(UInt32 id);
	bool AcceptActiveTask(UInt32 id);
	bool RemoveActiveTask(UInt32 id);
	void OnActiveClosed(UInt32 id);

	/**
	 *@brief 传奇之路
	 */
	void GetLegendTaskList(std::vector<TaskBriefInfo>& tasks) { m_LegendTaskMgr.GetLegendTasks(tasks); }
	bool SubmitLegendTask(UInt32 taskId);

	/**
	 *@brief 选取合适的日常任务
	 */
	//UInt32 SelectDailyTask(UInt8 type);

	void ResetTask();
	TaskMgr& GetTaskMgr(){ return m_TaskMgr; }
	DailyTaskMgr& GetDailyTaskMgr() { return m_DailyTaskMgr; }
	AchievementTaskMgr& GetAchievementTaskMgr() { return m_AchievementTaskMgr; }
	CycleTaskMgr& GetCycleTaskMgr() { return m_CycleTaskMgr; }
	RedPacketTaskMgr& GetRedPacketTaskMgr() { return m_RedPacketTaskMgr; }
	LegendTaskMgr& GetLegendTaskMgr() { return m_LegendTaskMgr; }
	MasterDailyTaskMgr& GetMasterDailyTaskMgr() { return m_MasterDailyTaskMgr; }
	MasterAcademicGrowthTaskMgr& GetMasterAcademicGrowthTaskMgr() { return m_MasterAcademicGrowthTaskMgr; }
	bool CheckTaskFinished(UInt32 id);
	void SetTaskFinished(UInt32 id);

	bool AcceptLegendTask(UInt32 taskId);
	bool CheckLegendTaskFinished(UInt32 taskId);

	bool CheckAchievementTaskFinished(UInt32 taskId);

	ActivityAttributeMgr& GetActivityAttributeMgr() { return m_ActivityAttributeMgr; }

	/**
	 *@brief 根据等级和前置任务判断任务是否可接
	 */
	bool IsTaskAcceptable(UInt32 id, bool bNotifyErr = false);
	//bool IsDailyTaskAcceptable(UInt8 type, bool bNotifyErr);
	//UInt32 GetDailyTaskMaxNum(UInt8 type);
	//UInt32 GetDailyTaskBaseMaxNum(UInt8 type);

	/**
	*@brief 设定好的日常人数数量
	*/
	UInt32 GetDailyTaskSelectorNum();

	/**
	 *@brief 获取日常任务数
	 */
	UInt32 GetDailyTaskNum(UInt8 type){ return m_DailyTaskMgr.GetTaskNum(); }

	/**
	*@brief 获取日常任务选择器
	*/
	const std::vector<DailyTaskSelector>& GetDailyTaskSelector() { return m_pLevelDataEntry->dailyTaskPool; }

	/**
	 *@brief 设置获取区域任务
	 */
	void SetZoneTask(UInt32 task){ m_ZoneTask = task; }
	UInt32 GetZoneTask() const{ return m_ZoneTask; }

	/**
	*@brief 加载任务
	*/
	void LoadTask(UInt64 guid, UInt32 dataId, TaskStatus status, std::string parameter, UInt32 finTime, UInt8 submitCount);

	/**
	*@brief 加载任务完成
	*/
	void OnLoadTaskFinished();

	/**
	*@brief 根据id获取task对象
	*/
	BaseTask* GetTask(UInt32 taskId);

	/**
	*@brief 上交任务物品
	*/
	UInt32 SetTaskItem(UInt32 taskId, std::vector<UInt64> items);

	/**
	*@brief 获得悬赏任务列表大小
	*/
	//UInt32 GetXuanShangListSize() { return m_TaskMgr.GetXuanShangListSize(); }

	UInt32 FinishAllTask(TaskType taskType = TaskType::TASK_MAX, UInt32 taskId = 0);

	/**
	*@brief 每日任务积分
	*/
	void IncDailyTaskScore(UInt32 score);
	UInt32 GetDailyTaskScore() { return m_DailyTaskScore; }
	void ResetDailyTaskScore() { m_DailyTaskScore = 0; }

	/**
	*@brief 每日任务积分奖励
	*/
	inline bool CheckDailyTaskRewardMask(UInt32 boxId) const { return m_DailyTaskRewardMask.CheckMask(boxId); }
	inline void SetDailyTaskRewardMask(UInt32 boxId){ m_DailyTaskRewardMask.SetMask(boxId); }
	inline void ResetDailyTaskRewardMask() { m_DailyTaskRewardMask.ClearMask(); }

	/**
	*@brief 成就积分奖励
	*/
	inline bool CheckAchievementRewardMask(UInt32 rewardId) const { return m_AchievementTaskRewardMask.CheckMask(rewardId); }
	inline void SetAchievementRewardMask(UInt32 rewardId) { m_AchievementTaskRewardMask.SetMask(rewardId); }

	/**
	*@brief 领取每日任务积分奖励
	*/
	UInt32 ReceiveDailyTaskScoreReward(UInt32 boxId);

	/**
	*@brief 领取成就积分奖励
	*/
	UInt32 ReceiveAchievementScoreReward(UInt32 rewardId);

	/**
	*@brief 成就积分
	*/
	void AddAchievementScore(UInt32 score);
	inline UInt32 GetAchievementScore() const { return m_AchievementScore; }

public://剧情
	/**
	 *@brief 开始一个剧情
	 */
	bool StartStory(UInt32 id);

	/**
	 *@brief 客户端通知结束一个剧情
	 */
	void StopStory(UInt32 id);

	/**
	*@brief 设置一个引导
	*/
	void SetBootFlag(UInt32 id);

	/**
	*@brief 重置一个引导
	*/
	void ResetBootFlag(UInt32 id);

	/**
	 *@brief 终止剧情
	 */
	void CancelStory();

	/**
	*@brief 设置新手引导
	*/
	void SetNewBoot(UInt32 id);
	UInt32 GetNewBoot();

	void CheckNewBoot();

	/**
	*@brief 设置赛季播放状态
	*/
	void ResetSeasonPlayStatus();

	/**
	*@brief 获取赛季播放状态
	*/
	UInt8 GetSeasonPlayStatus() { return m_SeasonLevel.GetSeasonPlayStatus(); }

	//设置药水配置
	bool SetPotionPos(UInt8 pos, UInt32 potionId)	{ return m_potionSet.SetPotion(pos, potionId); }
	std::vector<UInt32> GetPotionPos()	{ return m_potionSet.GetPosionPos(); }

public://交易相关
	/**
	 *@brief 邀请和应答交易
	 */
	bool InviteTrade(ObjID_t id);
	void AcceptTrade(ObjID_t id);

	/**
	 *@brief 设置获取交易
	 */
	Trade* GetTrade() const{ return m_pTrade; }
	void SetTrade(Trade* trade){ m_pTrade = trade; }

	/**
	 *@brief 退出交易
	 */
	void CancelTrade();

public: //邮件相关
	/**
	 *@brief 发送系统邮件
	 */
	void SendSysMail(const char* senderName, const char* title, const char* content, const char* reason, RewardGroup* rewards = NULL);

public:
	/**
	 *@brief 发送观察数据（废弃）
	 */
	void SendWatchData(ObjID_t watcher, const std::string& tribe);

public://关系相关

	/** 
	 *@brief 是否队友
	 */
	bool IsTeammate(Creature* creature) const;

	/**
	 *@brief 设置关系列表
	 */
	void SetRelationList(RelationType type, std::map<ObjID_t, UInt16>& list);

	/**
	 *@brief 增加删除关系
	 */
	void AddRelation(RelationType type, ObjID_t tarId);
	void RemoveRelation(RelationType type, ObjID_t tarId);

	// 和指定玩家是否有关系
	bool HasRelation(RelationType type, ObjID_t tarId);
	// 是否有指定关系的玩家
	bool HasRelation(RelationType type);
	// 获得师傅
	Player* GetOnlineMaster();
	// 师傅是师傅
	bool IsMaster(ObjID_t roleId);
	ObjID_t GetMasterId();

	// 师徒疲劳返利（消耗疲劳时调用）
	void AddFatigueToMaster(UInt32 usedFatigue);

	// 师徒类型
	UInt8 GetMasterDiscipleType();
	/**
	 *@brief 设置亲密度
	 */
	void OnSyncIntimacy(ObjID_t id, UInt16 intimacy, UInt16 incIntimacy);
	void AddIntimacy(ObjID_t id, UInt16 intimacy);
	UInt16 GetIntimacy(ObjID_t id) const;
	UInt16 GetTeamIntimacyBuff() const{ return m_TeamIntimacyBuff; }
	bool IsFriend(ObjID_t id) const{ return m_Friends.find(id) != m_Friends.end(); }

	/**
	 *@brief 同步祝福好友次数                                                                     
	 */
	void SyncBlessTimes();

	/**
	 *@brief 祝福经验相关
	 */
	UInt32 GetBlessExp() const{ return m_BlessExp; }
	void DecBlessExp(UInt32 exp);
	void IncBlessExp(UInt32 exp);

	/**
	 *@brief 发送好友相赠信息                                                                     
	 */
	void SendFriendPresentInfo();

	/**
	 *@brief 好友给礼物                                                                     
	 */
	bool FriendGiveGift(const std::string& friendName, UInt32 itemId);
	void OnGiveFriendGiftRet(UInt32 gamestartDay, UInt8 type, UInt8 result);

public://聊天相关
	/**
	 *@brief 聊天
	 */
	void Speak(UInt8 channel, const std::string& word, const std::string voiceKey = "", UInt8 voiceDur = 0, ObjID_t targetId = 0, UInt8 isShare = 0);
	UInt32 ParseSpeakWords(const std::string inWord, std::string& outWords, std::set<Item*>& linkItems, std::set<ObjID_t>& linkAuctionItems,
		bool& isLink, UInt32 maxShowWidth = CL_MAX_SHOW_WIDTH);
	void ScriptSpeak(UInt8 channel, const char* word);

	/**
	*@brief 发送喇叭
	*/
	UInt32 SendHorn(const std::string& content, UInt8 num);

	/**
	*@brief 上报自定义日志
	*/
	void ReportCustomLog(CustomLogReportType type, const std::string& param);

	/**
	 *@brief 设置聊天时间
	 */
	void SetChatTime(UInt8 channel, UInt64 time){ m_ChatTimer.SetChatTime(channel, time); }

	/**
	 *@brief 发送玩家位置
	 */
	void SendPos(UInt8 channel, ObjID_t targetId);
	void SendNotifyById(UInt32 id);

	/**
	 *@brief 发送通知
	 */
	// added by huchenhui
	template <typename... Args>
	void SendNotify(UInt32 id, const Args & ... args)
	{
		UInt16 type = 0;
		UInt8 color = 0;
		char buffer[CL_MAX_CHAT_LEN + 1];
		memset(buffer, 0, sizeof(buffer));
		bool ret = SysNotifyMgr::Instance()->MakeNotify(id, type, color, buffer, CL_MAX_CHAT_LEN, args...);

		if (!ret) return;

		Notify(type, color, buffer);
	}

	template <typename... Args>
	void SendNotify(const char* ctx, const Args & ... args)
	{
		char buffer[CL_MAX_CHAT_LEN + 1];
		memset(buffer, 0, sizeof(buffer));
		bool ret = SysNotifyMgr::Instance()->MakeNotify(ctx, buffer, CL_MAX_CHAT_LEN, args...);
		if (!ret) return;
		Notify(20, 0, buffer);
	}

	void Notify(UInt16 type, UInt8 color, const char* word);

public://队伍相关
	/**
	 *@brief 创建队伍
	 */
	void CreateTeam();

	/**
	 *@brief 设置获取队伍
	 */
	void SetTeam(Team* team);
	Team* GetTeam() const{ return m_pTeam; }
	void UpdateTeamInfo();

	/**
	 *@brief 判断是否队长
	 */
	bool IsTeamMaster() const;

	/**
	*@brief 获取队伍人数
	*/
	UInt8 GetTeamSize() const;

public://设置相关

	PvpSkillMgr& GetPvpSkillMgr() { return m_PvpSkillMgr; }

	/**
	 *@brief 获取技能栏
	 */
	SkillBar& GetSkillBar(){ return m_SkillBar; }

	/**
	*@brief 获取PVP技能栏
	*/
	SkillBar& GetPvpSkillBar() { return m_PvpSkillBar; }

	/**
	*@brief 获取预先转职技能
	*/
	std::vector<UInt16> GetPreOccuSkillId();

	/**
	*@brief 清除预转职技能
	*/
	void ClearPreOccuSkills() { GetSkillMgr().ClearPreOccuSkills(); GetPvpSkillMgr().ClearPreOccuSkills(); }

	/**
	*@brief 获取技能在skillbar中的位置
	*/
	UInt8 GetSkillSlot(UInt16 skillId) { return m_SkillBar.GetSlot(skillId); }

	/**
	*@brief 获取PVP技能在skillbar中的位置
	*/
	UInt8 GetPvpSkillSlot(UInt16 skillId) { return m_PvpSkillBar.GetSlot(skillId); }

	/**
	*@brief 获取物品栏
	*/
	ItemBar& GetItemBar() { return m_ItemBar; }

	/**
	*@brief 设置物品栏
	*/
	void SetItemBar(UInt8 slot, UInt32 id);

	/**
	*@brief 获取技能栏位数 huchenhui
	*/
	UInt32 GetSkillGridSize() { return m_pLevelDataEntry->skillGridSize; }

	/**
	*@brief 获取物品栏位数 huchenhui
	*/
	UInt32 GetItemGridSize() { return m_pLevelDataEntry->itemGridSize; }

	/**
	 *@brief 设置选项
	 */
	void SetOptions(UInt32 options){ m_Options = options; }

	/**
	*@brief 获取战斗技能信息
	*/
	void GetRaceSkills(std::vector<RaceSkillInfo>& raceSkills, bool isPvp);

	/**
	*@brief 让world缓存物品数据
	*/
	void WorldCacheItemData(Item* item);

	/**
	*@brief 让world缓存随从数据
	*/
	void WorldCacheRetinueData(Retinue* retinue);

	/**
	*@brief 是否检查过SP
	*/
	bool IsCheckSP() { return m_IsCheckSp == 1; }

	/**
	*@brief 完成SP检查
	*/
	void FinishCheckSP() { m_IsCheckSp = 1; }

	/**
	*@brief 装备栏是否有效
	*/
	bool IsWeaponInvalid() { return m_WeaponBar.IsInvalid(); }

	/**
	*@brief 装备栏是否存在该装备
	*/
	bool IsWeaponExist(ObjID_t id) { return m_WeaponBar.IsExist(id); }

	/**
	*@brief 设置装备栏
	*/
	UInt32 SetWeaponBarById(ObjID_t oldId, ObjID_t newId) { return m_WeaponBar.SetBarById(oldId, newId); }

	/**
	*@brief 设置装备栏
	*/
	UInt32 SetWeaponBarByIndex(UInt32 index, ObjID_t newId);

public:	//计数相关
	/**
	 *@brief 增加获取计数
	 */
	UInt32 IncCounter(const char* name, UInt32 value){ return m_CounterMgr.IncCounter(name, value); }
	UInt32 GetCounter(const char* name) { return m_CounterMgr.GetCounter(name); }
	UInt32 GetCounterWithoutCycle(const char* name) { return m_CounterMgr.GetCounterWithoutCycle(name); }
	void SetCounter(const char* name, UInt32 value){ m_CounterMgr.SetCounter(name, value); }

	/**
	 *@brief 获取计数管理器
	 */
	CounterMgr& GetCounterMgr(){ return m_CounterMgr; }

	/**
	 *@brief 获取行为计数管理器                                                                     
	 */
	UInt32 IncActionCounter(int type) { return m_ActionCounterMgr.IncCounter(type); }
	ActionCounterMgr& GetActionCounterMgr() { return m_ActionCounterMgr; }

	/**
	 *@brief 获取副本进入场景和位置                                                                     
	 */
	UInt32 GetEnterSceneID() const{ return m_EnterScene; }
	const CLPosition& GetEnterPos() const{ return m_EnterPos; }
	UInt64 GetEnterDyncSceneID() const { return m_EnterSceneDyncId; }
	/**
	*@brief 获取进入武道大会前的场景和位置
	*/
	void SetWudaoPrevSceneID(UInt32 id) { m_WudaoPrevSceneId = id; }
	UInt32 GetWudaoPrevSceneID() const{ return m_WudaoPrevSceneId; }
	const CLPosition& GetWudaoPrevPos() const{ return m_WudaoPrevScenePos; }

	/**
	*@brief 跨服场景服务器ID
	*/
	void SetCrossSceneNodeID(UInt32 id) { m_CrossSceneNodeId = id; }
	UInt32 GetCrossSceneNodeID() const { return m_CrossSceneNodeId; }

	/**
	*@brief 原场景服务器ID
	*/
	void SetSrcSceneNodeID(UInt32 id) { m_SrcSceneNodeId = id; }
	UInt32 GetSrcSceneNodeID() const { return m_SrcSceneNodeId; }

	/**
	*@brief 战场场景服务器ID
	*/
	void SetBattleSceneNodeID(UInt32 id) { m_BattleSceneNodeId = id; }
	UInt32 GetBattleSceneNodeID() const { return m_BattleSceneNodeId; }

	/**
	*@brief 原场景服务器ID
	*/
	void SetNotifyChangeScene(bool notify) { m_NotifyChangeScene = notify; }
	bool IsNotifyChangeScene() const { return m_NotifyChangeScene; }

public: //日常相关
	/**
	 *@brief 获取日常管理器
	 */
	RoutineMgr& GetRoutineMgr() { return m_RoutineMgr; }

	/**
	 *@brief 活跃度事件完成
	 */
	void	OnActiveEventFinished(UInt32 eventId){ GetRoutineMgr().OnActiveEventFinished(eventId); }

	/**
	*@brief 获取每日在线时间
	*/
	UInt32 GetDailyOnlineTime() const { return m_RoutineMgr.GetDailyOnlineTime(); }

public: //邀请好友相关

	InviteMgr& GetInviteMgr() { return m_InviteMgr; }

public:  //adder by huchenhui 2016.07.24

	//获取随从管理器
	RetinueMgr& GetRetinueMgr() { return m_RetinueMgr; }

	//添加一个随从
	UInt32 AddRetinue(UInt32 dataId) { return m_RetinueMgr.AddRetinue(dataId); }

	//根据ItemID添加一个随从
	UInt32 AddRetinueByItemId(UInt32 itemId);

	//根据ItemID检查添加随从
	UInt32 CheckAddRetinueByItemId(UInt32 itemId);

	//获取随从个数
	UInt32 GetRetinueSize() { return m_RetinueMgr.GetRetinueSize(); }

	//获取指定品阶以上的数量
	UInt32 GetRetinueSizeByQuality(RetinueQuality quality) { return m_RetinueMgr.GetRetinueSizeByQuality(quality); }

	//获取历史洗练次数
	UInt32 GetChangeSkillTotal() { return m_RetinueMgr.GetChangeSkillTotal() + 1; }

	//获取技能个数
	UInt32 GetSkillCount(UInt32 id, UInt8 level) { return m_RetinueMgr.GetSkillCount(id, level); }

	//根据技能ID获取技能个数
	UInt32 GetSkillCountById(UInt32 id) { return m_RetinueMgr.GetSkillCount(id, 0); }

	//根据技能Level
	UInt32 GetSkillCountByLevel(UInt8 level) { return m_RetinueMgr.GetSkillCount(0, level); }

	//获取随从槽位数量
	UInt32 GetMaxRetinueNum() { return m_pLevelDataEntry->retinueSize; }

	//获取最大随从等级
	UInt32 GetMaxRetinueLevel() { return m_RetinueMgr.GetMaxRetinueLevel(); }

	//获取最大星级
	UInt32 GetMaxRetinueStar() { return m_RetinueMgr.GetMaxRetinueStar(); }

public:

	//获取宠物管理器
	PetMgr& GetPetMgr() { return m_PetMgr; }

	/**
	*@brief 增加宠物
	*/
	UInt32 AddPet(UInt32 itemId) { return m_PetMgr.AddPet(itemId); }

	/**
	*@brief 设置宠物槽位
	*/
	UInt32 SetPetSlot(PetType petType, UInt64 petId) { return m_PetMgr.SetPetSlot(petType, petId); }

	/**
	*@brief 获取宠物槽位列表
	*/
	std::vector<UInt64> GetBattlePets() { return m_PetMgr.GetBattlePets(); }

	/**
	*@brief 喂食宠物
	*/
	UInt32 FeedPet(ObjID_t id, PetFeedType petFeedType, UInt8& isCritical, UInt32& value) { return m_PetMgr.FeedPet(id, petFeedType, isCritical, value); }

	/**
	*@brief 出售宠物
	*/
	UInt32 SellPet(ObjID_t id) { return m_PetMgr.SellPet(id); }

	/**
	*@brief 变更宠物技能
	*/
	UInt32 ChangePetSkill(ObjID_t id, UInt8 skillIndex){ return m_PetMgr.ChangePetSkill(id, skillIndex); }

	/**
	*@brief 设置跟随的宠物
	*/
	UInt32 SetPetFollow(ObjID_t id) { return m_PetMgr.SetPetFollow(id); }

	/**
	*@brief 获取跟随的宠物
	*/
	UInt64 GetPetFollow() { return m_PetMgr.GetFollowPetId(); }

	/**
	*@brief 吞食宠物
	*/
	UInt32 DevourPet(ObjID_t id, std::vector<ObjID_t> petIds, UInt32& exp) { return m_PetMgr.DevourPet(id, petIds, exp); }

	/**
	*@brief 获取宠物基本信息
	*/
	void GetPetBaseInfos(std::vector<PetBaseInfo>& pets) { return m_PetMgr.GetPetBaseInfos(pets); }

	/**
	*@brief 扣除宠物饥饿值
	*/
	void RemovePetHunger(UInt16 hunger) { return m_PetMgr.RemovePetHunger(hunger); }

	/**
	*@brief 增加宠物饥饿值
	*/
	void AddPetHunger(UInt16 hunger) { return m_PetMgr.AddPetHunger(hunger); }

	/**
	*@brief 获取宠物数量
	*/
	UInt32 GetPetSize(PetType petType, PetQuality petQualiy, UInt16 level) { return m_PetMgr.GetPetSize(petType, petQualiy, level); }

public:	//赛季段位
	//获取赛季段位
	UInt32 GetSeasonLevel() { return m_SeasonLevel.GetSeasonLevel(); }

	//获取赛季星级
	UInt32 GetSeasonStar() { return m_SeasonLevel.GetSeasonStar(); }

	//获取赛季经验
	UInt32 GetSeasonExp() { return m_SeasonLevel.GetSeasonExp(); }

	//获取赛季属性
	UInt8 GetSeasonAttr() { return m_SeasonLevel.GetSeasonAttr(); }

	//获取王者印记
	UInt32 GetSeasonKingMarkCount() { return m_SeasonLevel.GetSeasonKingMarkCount(); }

	//获取赛季晋级赛记录
	std::vector<UInt8> GetSeasonUplevelBattle() { return m_SeasonLevel.GetSeasonUplevelBattle().GetBattleRecords(); }

	//赛季信息变更
	void UpdateSeasonInfo() { return m_SeasonLevel.UpdateSeasonInfo(); }

	//设置赛季段位和星星
	bool SetSeasonLevel (UInt32 level, UInt32 star = 0) { return m_SeasonLevel.SetSeasonLevel(level, star); }

	//设置晋级赛战绩
	bool SetSeasonUplevelBattleRecord(std::vector<UInt8> vecRecords) { return m_SeasonLevel.SetSeasonUplevelBattleRecord(vecRecords); }

	//设置赛季属性
	void SetSeasonAttr(UInt32 time, UInt8 attr = 0) { m_SeasonLevel.SetSeasonAttr(time, attr); }

	//获取赛季到期时间
	UInt32 GetChangeSeasonAttrTime() { return m_SeasonLevel.GetChangeSeasonAttrTime(); }

	//增加赛季经验
	void AddSeasonExp(UInt32 exp) { m_SeasonLevel.IncExp(exp); }

	//扣除赛季经验
	void DecSeasonExp(UInt32 exp) { m_SeasonLevel.DecExp(exp); }

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
		CUserLogger	*pLogger = _analyzer.GetInstance(UDPLOG_SCENE_UID);
		if (NULL == pLogger) return;

		char userLogBuffer[MAX_USERMSG_LENGTH];
		memset(userLogBuffer, 0, MAX_USERMSG_LENGTH);
		UInt32 nTime = (UInt32)Avalon::Time::CurrentTime().Sec();
		UInt16 zoneId = (GetSrcZoneId() != 0 ? GetSrcZoneId() : ZONE_ID);
		UInt32 monthCardExpireTime = m_MonthCardExpireTime;
		UInt32 bronZoneId = GetBornZoneId();
		UInt32 isReturn = IsVeteranReturn() ? 1 : 0;

#ifndef AVALON_OS_WINDOWS
		snprintf(userLogBuffer, sizeof(userLogBuffer), LOG_USERMSG_FORMAT, GAME_VERSION.c_str(), m_OpenId.c_str(), zoneId, m_Pf.c_str(), m_DeviceId.c_str(),
			m_Model.c_str(), m_DeviceVersion.c_str(), m_Ip.c_str(), GetID(), GetAccID(), (UInt32)GetLevel(), (UInt32)GetVipLvl(), (UInt32)GetOccu(), monthCardExpireTime, UInt32(m_CreateTime), nTime, bronZoneId, isReturn, 0, 0, 0);
#else
		_snprintf_s(userLogBuffer, sizeof(userLogBuffer), LOG_USERMSG_FORMAT, GAME_VERSION.c_str(), m_OpenId.c_str(), zoneId, m_Pf.c_str(), m_DeviceId.c_str(),
			m_Model.c_str(), m_DeviceVersion.c_str(), m_Ip.c_str(), GetID(), GetAccID(), (UInt32)GetLevel(), (UInt32)GetVipLvl(), (UInt32)GetOccu(), monthCardExpireTime, UInt32(m_CreateTime), nTime, bronZoneId, isReturn, 0, 0, 0);
#endif

		pLogger->SetUserIP(m_Ip.c_str());
		pLogger->SetUserMsg(userLogBuffer);
		pLogger->LogMsg(typeInfo, format, args...);

	}

	/**
	*@brief 发送登录log
	*/
	void SendLoginUdpLog();

	/**
	*@brief 发送登出log
	*/
	void SendLogoutUdpLog();

	/**
	*@brief 发送道具库存log
	*/
	void SendItemUdpLog();

	/**
	*@brief 发送道具相关log
	*/
	void SendItemUdpLog(const char* reason, UInt64 itemId, UInt32 itemDataId, ItemType itemType, ItemSubType itemSubType, std::string itemName, UInt32 itemQuality, Int32 number, UInt32 oldNumber);

	/**
	*@brief 发送装备登陆
	*/
	void SendEquipLoginUdpLog(UInt64 itemId, UInt32 itemDataId, ItemType itemType, ItemSubType itemSubType, ItemQuality quality, UInt32 strengthen, UInt32 qualityLevel, std::string itemName
		, UInt32 preciousId, ItemQuality preciousQuality, UInt32 cardId, ItemQuality cardQuality, UInt32 slot);

	/**
	*@brief 发送道具登陆
	*/
	void SendItemLoginUdpLog(UInt64 itemId, UInt32 itemDataId, UInt32 itemNum, ItemType itemType, ItemSubType itemSubType, ItemQuality quality, UInt32 strengthen, UInt32 qualityLevel, std::string itemName
		, UInt32 preciousId, ItemQuality preciousQuality, UInt32 cardId, ItemQuality cardQuality, UInt32 slot);

	/**
	*@brief 发送开罐子相关log
	*/
	void SendOpenJarUdpLog(UInt32 jarId, UInt32 combo, ItemDataEntry* itemData, UInt32 itemNum);

	/**
	*@brief 发送资产相关log  added by huchenhui 2016.08.11
	*/
	void SendAssetUdpLog(AssetType type, const char* reason, UInt64 oldAsset, Int64 assetNum, Int64 nowAssetNum, UInt64 nowAsset);

	/**
	*@brief 发送商城道具变化log
	*/
	void SendItemChangeUdpLog(const char *strType, UInt32 uItemId, UInt32 uPrice, UInt16 uNum);

	/**
	*@brief 上报功能日志，玩家使用各种功能之后上报
	*/
	void SendFunctionUdpLog(FunctionLogType type, UInt32 p1 = 0, UInt32 p2 = 0);

	/**
	*@brief 技能变化UDP日志
	*/
	void SendChangeSkillUdpLog(UInt16 skillId, std::string skillName, Int8 changeLevel, UInt8 sourceLevel, UInt8 newLevel);

	/**
	*@brief 随从解锁UDP日志
	*/
	void SendRetinueUnlockUdpLog(UInt64 retinueId, UInt32 dataId);

	/**
	*@brief 随从升级UDP日志
	*/
	void SendRetinueUpLevelUdpLog(UInt64 retinueId, UInt32 dataId, UInt8 level);

	/**
	*@brief 随从升星UDP日志
	*/
	void SendRetinueUpStarUdpLog(UInt64 retinueId, UInt32 dataId, UInt8 starLevel);

	/**
	*@brief 随从洗练UDP日志
	*/
	void SendRetinueChangeSkillUdpLog(UInt64 retinueId, UInt32 dataId, std::string lockSkills, std::string oldSkills, std::string newSkills);

	/**
	*@brief 活动任务完成日志
	*/
	void SendActiveTaskFinishUdpLog(UInt32 activeId, UInt32 taskId);

	/**
	*@brief 商店购买日志
	*/
	void SendShopBuyUdpLog(UInt32 shopId, UInt32 shopItemId, UInt32 num, UInt32 newNum, UInt32 dungeonId);

	/**
	*@brief 商店商品日志
	*/
	void SendShopItemUdpLog(UInt32 shopId, UInt32 shopItemId, UInt32 costNum);

	/**
	*@brief 商城购买日志
	*/
	void SendMallBuyUdpLog(UInt32 mallItemId, UInt32 num);

	/**
	*@brief 新手引导日志
	*/
	void SendNewBootUdpLog(UInt32 bootId);

	/**
	*@brief 强化装备日志
	*/
	void SendStrengthEquipUdpLog(UInt64 itemId, UInt32 itemDataId, UInt32 oldStrength, UInt32 newStrength, UInt32 errorcode, UInt32 packType, UInt32 useUnbreak, UInt32 isTicket, UInt32 probability);

	/**
	*@brief 首次过关死亡之塔
	*/
	void SendFirstClearDeathTowerUdpLog(UInt32 dungeonId, UInt32 floor, UInt32 usedSec);

	/**
	*@brief 匹配开始日志
	*/
	void SendMatchStartUdpLog(PkType type);

	/**
	*@brief 决斗结束
	*/
	void SendPvpEndUdpLog(UInt32 targetZoneId, UInt64 targetId, PkType type, PkRaceResult result, UInt32 oldMatchScore, UInt32 newMatchScore, UInt32 oldPkValue, UInt32 newPkValue, UInt32 oldSeasonLevel, UInt32 newSeasonLevel, UInt32 oldSeasonStar, UInt32 newSeasonStar, UInt32 oldSeasonExp, UInt32 newSeasonExp, UInt32 seasonId, UInt32 dungeonId, UInt32 usedTime);

	/**
	*@brief 福利卡日志
	*/
	void SendWelfareCardUdpLog(ItemSubType type);

	/**
	*@brief 附魔卡合成
	*/
	void SendComposeMagicCardUdpLog(UInt32 card1, UInt32 card2, UInt32 targetCard, UInt32 code);

	/**
	*@brief 时装合成
	*/
	void SendFashionComposeUdpLog(UInt32 fashion1, UInt32 fashion2, UInt32 commonFashionId, UInt32 skyFashionId);

	/**
	*@brief 装备品阶调整
	*/
	void SendReMakeEquipQlvUdpLog(UInt64 itemId, UInt32 itemDataId, UInt8 oldLv, UInt8 nowLv, UInt32 oldMatNum, UInt32 newMatNum, UInt32 oldPoint, UInt32 newPoint, UInt8 isFrenzy, UInt8 eqColor, UInt8 eqPost);

	/**
	*@brief 订单完成
	*/
	void SendPayEndUdpLog(std::string orderId, UInt8 mallType, UInt32 chargeGoodsId, UInt32 chargeItemId, UInt32 chargeItemNum, UInt32 vipScore, UInt32 chargeMoney, UInt32 time, UInt32 sendGoodTimeoutTime);

	/**
	*@brief 转职
	*/
	void SendChangeOccuUdpLog(UInt8 mainOccu, UInt8 preOccu, UInt8 occu);

	/**
	*@brief 宠物登陆
	*/
	void SendPetLoginUdpLog(UInt64 petId, UInt32 dataId, UInt32 level, UInt32 exp, PetType petType, UInt32 slot);

	/**
	*@brief 增加宠物
	*/
	void SendPetAddUdpLog(UInt64 petId, UInt32 dataId, UInt32 itemId, PetType petType, PetQuality petQuality);

	/**
	*@brief 宠物出阵
	*/
	void SendPetBattleUdpLog(UInt64 petId, UInt32 dataId, UInt64 oldPetId, UInt32 oldDataId);

	/**
	*@brief 吞噬宠物
	*/
	void SendPetDevourUdpLog(UInt64 petId, UInt32 dataId, UInt16 oldLevel, UInt16 newLevel, UInt32 oldExp, UInt32 newExp, UInt32 exp, UInt64 devourId, UInt32 devourDataId, UInt16 devourLevel, UInt32 devourExp);

	/**
	*@brief 宠物属性重选
	*/
	void SendPetChangeSkillUdpLog(UInt64 petId, UInt32 dataId, UInt8 oldIndex, UInt8 newIndex);

	/**
	*@brief 上报实时语音日志
	*/
	void SendLiveVoiceDataUdpLog(UInt8 type, const std::string& roomId, const std::string& time);

	/**
	*@brief 上报聊天语音日志
	*/
	void SendChatVoiceDataUdpLog(UInt8 type, const std::string& voiceId);

	/*
	*@brief 发送徒弟完成师门任务log
	*/
	void SendDiscipleCompDailyTaskLog(ObjID_t discipleId, ObjID_t masterId, UInt32 timestamp);

	/*
	*@brief 发送宝珠日志
	*/
	void SendPreciousBeadUdpLog(PreciousBeadOpType opType, ObjID_t itemObjectId, UInt32 itemId, UInt32 srcBeadId, UInt32 newBeadId, UInt32 costItemId, UInt32 costItemNum, UInt32 isSuccess, UInt32 param1, UInt32 param2);

	/*
	*@brief 发送抽神器罐子折扣日志
	*/
	void SendExtractArtifactJarDiscountUdpLog(UInt32 opActId, UInt32 rate, UInt32 times);

	/*
	*@brief 发送吃鸡结算日志
	*/
	void SendChiJiBalanceUdpLog(UInt32 rank, UInt32 survivalTime, UInt32 reason);

	/**
	*@brief 计算玩家外观
	*/
	void CountAvatar();
	PlayerAvatar GetAvatar() const { return m_avatar; }
	void SetAvatar(const PlayerAvatar& avatar) { m_avatar = avatar; }

	// 控制时限道具是否通知客户端
	inline bool IsFirstTimeItemTick()	{ return m_bFirstTimeItemTick; }
	inline void SetFirstTimeItemTick(bool b)	{ m_bFirstTimeItemTick = b; }

public://数据库相关

    /**
	 *@brief 开始加载玩家数据(这里必须指定一个回调)
	 */
	void StartLoadData(PlayerLoadingCallback* cb);

	/**
	*@brief 开始加载账号信息
	*/
	void LoadAccountInfo(PlayerLoadingCallback* cb);

	/**
	*@brief 开始加载账号仓库信息
	*/
	void LoadAccountStorage();

    /**
	 *@brief 添加加载数据回调
	 */
	void AddLoadingCallback(PlayerLoadingCallback* cb);

	/**
	 *@brief 保存所有
	 */
	void Saveback(CLRecordCallback* callback);

	/**
	*@brief 保存上线数据
	*/
	void SaveOnlineData();

	/**
	 *@brief 增加加载项个数，返回本次加载序号
	 */
	UInt8 IncLoadDataCount();

	/**
	 *@brief 检查加载项
	 */
	bool CheckLoadDataSerial(UInt8 serial);
	void SetLoadDataFinished(UInt8 serial);

    /**
    *@brief 加载数据完成
    */
    void LoadingFinish();

	/**
	*@brief 加载数据完成
	*/
	void LoadingFailed();

public://功能开启相关
	/**
	 *检查、设置功能开启标志位
	 */
	bool CheckFuncFlag(UInt32 id) const{ return m_FuncFlag.CheckMask(id); }
	void SetFuncFlag(UInt32 id){ m_FuncFlag.SetMask(id); }

	/**
	 *@brief 判断预告功能                                                                     
	 */
	void UpdateFuncNotify();

	void OpenFunction(UInt32 id);

public://事件
	/**
	*@brief 心跳
	*/
	void OnTick(const Avalon::Time& now);

	/**
	 *@brief 心跳
	 */
	void OnHeartbeat(const Avalon::Time& now);

	/**
	*@brief 上线注册监听事件
	*/
	void OnRegistGameEvent();

	/**
	*@brief 离线注册监听事件
	*/
	void OnUnRegistGameEvent();

	/**
	 *@brief 上线
	 */
	void OnOnline();
	void OnOnlineBattle();

	/**
	 *@brief 下线
	 *@param bShutdown 是否维护
	 */
	void OnOffline(bool bShutdown = false);

	/**
	 *@brief 处理脚本上线
	 */
	void ScriptOnOnline(UInt64 offlineTime);

	/** 
	 *@brief 出生
	 */
	void OnBirth();

	/**
	 *@brief 即将进入场景事件
	 */
	void OnBeforeEnterScene();

	/**
	 *@brief 进入游戏
	 */
	void OnEnterScene();

	/**
	 *@brief 离开游戏
	 */
	void OnLeaveScene();

	/**
	*@brief 刷新疲劳值
	*/
	void OnRefreshFatigue();

	/**
	 *@brief 格子发生变化
	 */
	void OnGridChanged();

	/**
	 *@brief 升级
	 */
	void OnLevelup();

	/**
	 *@brief 使用道具
	 */
	void OnUseItem(UInt32 itemid);

	/**
	 *@brief 获得道具
	 */
	void OnGetItem(const char* reason, ItemDataEntry* data, UInt8 qualityLv, UInt8 strenth, UInt16 num,
		UInt16 oldNum, Item* pItem = NULL, bool getNotify = true, bool mailGive = false);
	void OnGetRewardGroup(const char* reason, RewardGroup* group, bool mailGive = false);
	/**
	 *@brief 消耗道具
	 */
	void OnCostItem(const char* reason, ItemDataEntry* data, UInt8 strengthen, UInt16 newNum, UInt16 oldNum, Item* pItem = NULL);

	/**
	*@brief 仓库操作
	*/
	void OnPushStorage(UInt32 itemId, UInt8 quality, UInt8 strengthen, UInt16 num);
	void OnPullStorage(UInt32 itemId, UInt8 quality, UInt8 strengthen, UInt16 num);

	/**
	 *@brief 具有归属权的npc死亡
	 */
	void OnOwnedNpcDied(Npc* npc, bool bDeathOwner = true);

	/*
	*@ 怪物死亡事件回调  added by aprilliu, 2016.04.13
	*/
	void OnNpcDead(UInt32 npcId, UInt32 dungeon);

	/**
	 *@brief 移动事件
	 */
	void OnMoved(const CLPosition& oldpos);

	/**
	 *@brief 场景切换
	 */
	void OnSceneChanged();

	/**
	 *@brief 进入第二天
	 */
	void OnDayChanged();

	/**
	*@brief 进入逻辑第二天(6点)
	*/
	void OnLogicDayChanged();

	/**
	 *@brief 进入下一逻辑周
	 */
	void OnLogicWeekChanged();

	/**
	 *@brief 加载数据完成
	 */
	void OnDataLoaded();

	/**
	*@brief 开始副本
	*/
	void OnBeginDungeon(UInt32 dungeonId, bool isHell);

	/**
	*@brief 副本通关
	*/
	void OnClearDungeon(UInt32 dungeonId, UInt16 beated, UInt16 areas, LevelScore score, UInt32 usedTime, UInt32 reviveCount, UInt32 endAreaId, std::vector<ObjID_t> teammateIds);

	/**
	 *@brief 消费
	 */
	void OnConsumed(UInt32 itemId, UInt16 num, UInt32 cost);

	/**
	 *@brief 名字改变
	 */
	void OnNameChanged();

	/**
	 *@brief vip等级改变
	 */
	void OnVipLvlChanged();

	/**
	*@brief vip经验改变
	*/
	void OnVipExpChanged();

	/**
	 *@brief 性别改变
	 */
	void OnSexChanged();

	/**
	 *@brief 任务成功
	 */
	void OnTaskSucced(BaseTask* task);

	/**
	 *@brief 任务失败
	 */
	void OnTaskFailed(UInt32 taskId);

	/**
	*@brief 强化装备 Adder by huchenhui 2016.06.24
	*/
	void OnStrengthenEquip(UInt32 code, UInt8 oldStrengthen, UInt64 itemId, UInt32 itemDataId, UInt32 oldPack, UInt32 useUnbreak, UInt32 isTicket, UInt32 probability);

	/**
	*@brief 分解装备 Adder by huchenhui 2016.06.24
	*/
	void OnDecomposeEquip();

	/**
	*@brief 游戏币变化  num为正数表示增加, 负数表示消耗   added by huchenhui 2016.06.28
	*/
	void OnChangeMoney(UInt32 type, Int32 num);

	/**
	*@brief 穿戴装备 Adder by huchenhui 2016.06.29
	*/
	void OnPutEquip(UInt16 level, UInt32 subType, UInt8 color, UInt32 strengthen);

	/**
	*@brief 穿戴时装 Adder by huchenhui 2018.03.20
	*/
	void OnPutFashion(ItemSubType subType, UInt8 color);

	/**
	*@brief 添加随从 Adder by huchenhui 2016.08.04
	*/
	void OnAddRetinue(Retinue* retinue);

	/**
	*@brief 随从升级 Adder by huchenhui 2016.10.19
	*/
	void OnRetinueUpLevel(Retinue* retinue);

	/**
	*@brief 随从升星 Adder by huchenhui 2016.11.28
	*/
	void OnRetinueUpStar(Retinue* retinue);

	/**
	*@brief 随从洗练 Adder by huchenhui 2016.08.20
	*/
	void OnRetinueChangeSkill(Retinue* retinue, std::vector<RetinueSkill> lockSkills, std::vector<RetinueSkill> oldSkills, std::vector<RetinueSkill> newSkills);

	/**
	*@brief pvp事件 Adder by huchenhui 2016.08.05
	*/
	void OnPVP(const CLProtocol::MatchServerRaceEnd& end);

	/**
	*@brief 房间pvp事件 Adder by huchenhui 2017.12.25
	*/
	void OnRoomPvp(const CLProtocol::MatchServerPkRoomRaceEnd& raceEnd, const Pk3V3PlayerRaceEndInfo& playerInfo);

	/**
	*@brief 转职事件 Adder by huchenhui 2016.08.20
	*/
	void OnChangeOccu(JobDataEntry* targetJob);

	/**
	*@brief 附魔事件 Adder by huchenhui 2016.08.20
	*/
	void OnAddMagic(ItemDataEntry* data, Item* equip);

	/**
	*@brief 合成附魔卡事件 Adder by huchenhui 2016.09.13
	*/
	void OnAddMagicCard();

	/**
	*@brief 开罐子事件 Adder by huchenhui 2016.08.20
	*/
	void OnOpenMagicJar(JarType type, UInt32 combo);

	/**
	*@brief 好友赠送事件 Adder by huchenhui 2016.08.22
	*/
	void OnFriendGive();

	/**
	*@brief 刷新商店事件 Adder by huchenhui 2016.08.23
	*/
	void OnRefreshShop();

	/**
	*@brief 购买商品事件 Adder by huchenhui 2016.11.23
	*/
	void OnShopBuy(UInt32 shopId, UInt32 shopItemId, UInt32 num, UInt32 newNum, UInt32 dungeonId);

	/**
	*@brief 商城购买事件 Adder by huchenhui 2016.11.23
	*/
	void OnBuyMallItem(UInt32 mallItemId, UInt32 num);

	/**
	*@brief 消耗疲劳事件 Adder by huchenhui 2016.08.29
	*/
	void OnRemoveFatigue(UInt16 usedFatigue);

	/**
	*@brief 通关深渊 Adder by huchenhui 2016.08.30
	*/
	void OnClearHell(UInt32 dungeonId, UInt32 usedTimec, LevelScore score, std::vector<ObjID_t> teammateIds);

	/**
	*@brief 死亡之塔开始 Adder by huchenhui 2016.10.22
	*/
	void OnDeathTowerBegin(UInt32 towerId);

	/**
	*@brief 死亡之塔 Adder by huchenhui 2016.08.30
	*/
	void OnDeathTower(UInt32 towerId, UInt32 layer, UInt32 usedTime);

	/**
	*@brief 觉醒 Adder by huchenhui 2016.08.31
	*/
	void OnAwaken();

	/**
	*@brief 复活币 Adder by huchenhui 2016.09.06
	*/
	void OnReviveCoin(bool isSelf);

	/**
	*@brief 拍卖行上架 Adder by huchenhui 2016.09.14
	*/
	void OnAddAuction();

	/**
	*@brief 拍卖行 售出 Adder by huchenhui 2016.09.14
	*/
	void OnSellAuction(UInt32 money);

	/**
	*@brief 拍卖行 售出 Adder by huchenhui 2018.03.17
	*/
	void OnBuyAuction(UInt32 money);

	/**
	*@brief 接受任务事件
	*/
	void OnAcceptTask(UInt32 taskId, TaskType type);

	/**
	*@brief 放弃任务事件
	*/
	void OnAbandonTask(UInt32 taskId, TaskType type);

	/**
	*@brief 完成日常任务 Adder by huchenhui 2016.09.14
	*/
	void OnDailyTaskSubmitted(UInt32 taskId, TaskType type, TaskSubType subType);

	/**
	*@brief 完成任务 Adder by huchenhui 2016.09.14
	*/
	void OnSubmitTaskSucceed(UInt32 taskId, TaskType type);

	/**
	*@brief 完成活动任务 Adder by huchenhui 2016.10.22
	*/
	void OnSubmitActiveTaskSucceed(UInt32 templateId, UInt32 taskId);

	/**
	*@brief 完成循环任务 Adder by huchenhui 2016.11.07
	*/
	void OnSubmitCycleTaskSucceed(UInt32 taskId);

	/**
	*@brief 完成传奇之路任务 Adder by huchenhui 2017.08.23
	*/
	void OnSubmitLegendTaskSucceed(BaseTask* task);

	/**
	*@brief 创建物品事件 Adder by huchenhui 2016.11.14
	*/
	void OnMakeItem(const char* reason, Item* item, Int32 num, bool isSendLog = true);

	/**
	*@brief 删除物品事件 Adder by huchenhui 2016.11.14
	*/
	void OnRemoveItem(const char* reason, Item* item, Int32 num, UInt32 oldNum);

	/**
	*@brief 合并物品事件 Adder by huchenhui 2016.11.14
	*/
	void OnMerge(const char* reason, Item* item, Int32 num, UInt32 oldNum);

	/**
	*@brief 新技能事件 Adder by huchenhui 2016.11.14
	*/
	void OnNewSkill(UInt16 skillId, bool isSetting );

	/**
	*@brief 技能等级变化事件 Adder by huchenhui 2016.11.14
	*/
	void OnChangeSkill(UInt16 skillId, std::string skillName, Int8 changeLevel, UInt8 sourceLevel, UInt8 newLevel, bool isSetting);

	/**
	*@brief 赛季等级变化事件 Adder by huchenhui 2017.04.24
	*/
	void OnSeasonLevelUp();

	/**
	*@brief 宠物喂食 Adder by huchenhui 2017.04.24
	*/
	void OnFeedPet(PetFeedType type);

	/**
	*@brief 获得宠物 Adder by huchenhui 2018.03.15
	*/
	void OnAddPet(PetType petType, PetQuality petQuality);

	/**
	*@brief 宠物升级 Adder by huchenhui 2018.03.21
	*/
	void OnPetUpLevel(PetType petType, PetQuality petQuality, UInt16 level);

	/**
	*@brief 增加社会关系 Adder by huchenhui 2017.10.23
	*/
	void OnAddRelation(RelationType type);

	/**
	*@brief 品阶调整 Adder by huchenhui 2018.03.14
	*/
	void OnRemakeEquip(UInt32 itemId, UInt8 qualityLevel);

	/**
	*@brief 装备分计算 Adder by huchenhui 2018.03.14
	*/
	void OnCalculateValuedScore(ItemSubType subType, ItemQuality quality, UInt32 score);

	/**
	*@brief 镶嵌宝珠 Adder by huchenhui 2018.03.15
	*/
	void OnAddPreciousBead();

	/**
	*@brief 镶嵌宝珠 Adder by huchenhui 2018.03.17
	*/
	void OnSendHorn();

	/**
	*@brief 加入工会 Adder by huchenhui 2018.03.17
	*/
	void OnJoinGuild();

	/**
	*@brief 加入工会 Adder by huchenhui 2018.06.06
	*/
	void OnIncGuildBattleScore(UInt32 score);

	/**
	*@brief 占领领地 Adder by huchenhui 2018.06.06
	*/
	void OnGuildOccupyTerritory(UInt8 terrId);

	/**
	*@brief 解除了和师傅的关系
	*/
	void OnRelationRelieveMaster(ObjID_t masterId);

	/**
	*@brief 拜师成功
	*/
	void OnRelationApprenticeMaster(ObjID_t masterId);

	/**
	*@brief 徒弟出师
	*/
	void OnRelationDiscipleFinishSchool(ObjID_t masterId);

	/**
	*@brief 增加良师值
	*/
	void OnAddGoodTeacherValueByFinishSchool(UInt64 addValue, ObjID_t discipleId);

	/**
	*@brief 武道大会胜利 Adder by huchenhui 2018.03.20
	*/
	void OnWinWudaodahuiPkEnd(UInt32 winNum);

	/**
	*@brief 扩展包裹 Adder by huchenhui 2018.06.11
	*/
	void OnEnlargePackage(UInt32 size);

	/**
	*@brief 扩展仓库 Adder by huchenhui 2018.06.11
	*/
	void OnEnlargeStorage(UInt32 size);

	/**
	*@brief 周常深渊失败
	*/
	void OnWeekHellFail(DungeonDataEntry& dungeonData);

public://网络
	/**
	 *@brief 发送到网关
	 */
	void SendToGate(Avalon::Protocol& protocol);

	/**
	 *@brief 发送到客户端
	 */
	void SendProtocol(Avalon::Protocol& protocol); 

	/**
	 *@brief 发送消息包到客户端
	 */
	void SendPacket(Avalon::Packet* packet);

	/**
	*@brief 转发到globalActivity
	*/
	void SendToGlobalActivity(Avalon::Protocol& protocol);
	void SendToGlobalActivity(Avalon::Packet* packet);

	void SendToSelfScene(Avalon::Protocol& protocol);

public:
	/**
	 *@brief 同步自身数据
	 *@param bDirty 是否只同步脏数据
	 */
	void SyncSelfData(bool bDirty);
	void SyncSelfData(UInt32 type, bool bDirty);

	/**
	 *@brief 同步自身数据到周围队友
	 */
	void SyncTeamAround(bool bDirty);
	void SyncTeamAroundToMe();
	void SyncTeamNewZone();

	/**
	*@brief 同步外观信息
	*/
	void SyncAvatarToTeam();

	/**
	*@brief 同步外观信息
	*/
	void SyncAvatarToWorld();

	/**
	 *@brief 同步周围到自身                
             isForceSendMsg表示是否强制发送
	 */
	void SyncAroundToMe(bool isForceSendMsg = false);

	/**
	 *@brief 通知客户端进入场景
	 */
	void NotifyGateEnterScene();

	/**
	 *@brief 通知客户端加载场景
	 */
	void NotifyLoadScene();

	/**
	*@brief world同步过来的事件处理 by huchenhui 2016.08.22
	*/
	void EventProcess(SyncEventType type, ObjID_t param1, ObjID_t param2);

public:
	/**
	*@breif 设置更新版本号
	*/
	void SetUpdateBulletin(UInt32 updateBulletin) { m_UpdateBulletin = updateBulletin; }

public://一元抢购
	/**
	*@brief 检查兑换商城道具
	*@param num:数量
	*@return 是否满足
	*/
	bool CheckMallEx(UInt32 num);

	/**
	*@brief 扣除兑换商城道具
	*@param num:数量
	*/
	void RemoveMallEx(UInt32 num);

	/**
	*@brief 判断是否富贵达人商城道具
	*@param mallId:商城道具ID
	*/
	bool IsMallPanicBuyId(UInt32 mallId);

public:// 拍卖行相关
	/**
	*@brief 手动刷新拍卖行
	*/
	UInt32 OnPlayerRefreshAuction();

	/**
	*@brief 购买栏位
	*/
	UInt32 OnPlayerBuyAuctionBooth();

	/**
	*@brief 获取异常交易管理器
	*/
	AbnormalTransactionMgr& GetAbnormalTransactionMgr() { return m_AbnormalTransactionMgr; }

public:	// 随机装备商店
	/**
	*@brief 获得随机装备商店管理器
	*/
	EquipShopMgr& GetEquipShopMgr() { return m_EquipShopMgr; }

public:
    // 关卡相关
    inline DungeonMgr& GetDungeonMgr() { return m_dungeonMgr; }

	// 通知离开地下城
	void NotifyLeaveDungeon(bool isDisconnect);

	/**
	*@brief 是否通关地下城的任意难度
	*/
	bool IsDungeonAnyHardPassed(UInt32 dungeonId);

	/**
	*@brief 是否通关地下城
	*/
	bool IsDungeonPassed(UInt32 dungeonId);

	/**
	*@brief 请求购买次数
	*/
	UInt32 RequestBuyDungeonTimes(DungeonSubType subType);
	
	/**
	*@brief 获取地下城次数
	*/
	UInt32 GetDungeonDailyCount(UInt32 dungeonId){ return m_dungeonMgr.GetDailyCount(dungeonId); }

	/**
	*@breif 增加地下城次数
	*/
	void AddDungeonTimes(UInt8 subType, UInt32 times);
	/**
	*@breif 获取地下城最好成绩
	*/
	UInt32 GetDungeonBestScore(UInt32 dungeonId);

public:
	/**
	*@brief 请求购买死亡之塔次数
	*/
	UInt32 RequestBuyTowerTimes();

	// 死亡之塔
	void OnClearDeathTowerFloorMonsters(UInt32 dungeonId, UInt32 areaId, UInt32 usedTime, UInt32 remainHp, UInt32 remainMp);

	// 获取死亡之塔层数
	static UInt32 GetTowerFloorByAreaID(UInt32 dungeonId, UInt32 areaId);

	// 计算死亡之塔过层奖励
	DeathTowerAwardDataEntry* GetDeathTowerFloorAward(UInt32 floor);

	/**
	*@brief 死亡之塔最高层
	*/
	UInt32 GetDeathTowerTopFloor();
	void SetDeathTowerTopFloor(UInt32 floor);

	/**
	*@brief 死亡之塔最高层用时
	*/
	UInt32 GetDeathUsedTime();
	void SetDeathUsedTime(UInt32 usedTime);

	/**
	*@brief 死亡之塔计数
	*/
	bool CheckRefreshDeathTowerCount();
	void RefreshDeathTowerCount();

	/**
	*@brief 重置死亡之塔
	*/
	UInt32 OnResetDeathTower();

	/**
	*@brief 开始扫荡死亡之塔
	*/
	UInt32 OnBeginWipeoutDeathTower();

	/**
	*@brief 获取死亡之塔扫荡奖励
	*/
	UInt32 OnRequestDeathTowerWipeoutResult();

	/**
	*@brief 获取制定层数死亡之塔扫荡奖励
	*/
	UInt32 OnRequestDeathTowerWipeouttQueryResult(UInt32 beginFloor, UInt32 endFloor);

	/**
	*@brief 快速完成死亡之塔扫荡
	*/
	UInt32 OnQuickFinishDeathTowerWipeout();

	/**
	*@brief 死亡之塔首通奖励
	*/
	UInt32 OnRequestDeathTowerPassFloorAward(UInt32 floor);

	/**
	*@brief 重置死亡之塔记录
	*/
	UInt32 ResetTowerCounter();

public:	// 包裹
	inline UInt32 GetPackSize() { return m_PackSize; }
	void SetPackSize(UInt32 pksz);
	inline UInt8 GetStorageSize() { return m_StorageSize; }
	void SetStorageSize(UInt8 sz);
	// 更新新道具标识
	void UpdateNewItem(UInt8 packageType);

	/**
	*@brief 获取包裹容量加成
	*/
	UInt32 GetPackageAdditionSize(PackType type);

public:
	/**
	*@brief 能否进行匹配
	*/
	UInt32 CanStartPkMatch(PkType type);

	/**
	*@brief 玩家战斗信息
	*/
    RacePlayerInfo GetMatchRacePlayerInfo(bool pvp);

    /**
    *@brief 匹配积分
    */
    UInt32 GetMatchScore() const { return m_MatchScore; }
	void SetMatchScore(UInt32 score);

	/**
	*@brief 最高匹配积分
	*/
	UInt32 GetBestMatchScore();
	void SetBestMatchScore(UInt32 score);

	/**
	*@brief 今日赛季连胜、连败纪录
	*/
	void UpdateSeasonPkStraightCount(PkRaceResult result);
	UInt32 GetSeasonPkWinStraight();
	UInt32 GetSeasonPkLoseStraight();

    /**
    *@brief 获取pk值
    */
    UInt32 GetPkValue() const;
	PkLevelType GetPkLevelType() const;
	UInt8 GetPkLevel() const;

    /**
    *@brief 增加pk值
    */
    void IncPkValue(UInt32 value);

    /**
    *@brief 增加pk值
    */
    void DecPkValue(UInt32 value);

    PkStatisticMgr& GetPkStatisticMgr() { return m_pkStatisticMgr; }

	/**
	*@brief 获取最大连胜
	*/
	UInt32 GetMaxWinStreak(PkType type);

	/**
	*@brief 获取当前连胜
	*/
	UInt32 GetCurWinStreak(PkType type);

	/**
	*@brief 获取胜利场次
	*/
	UInt32 GetTotalWinNum(PkType type);

    /**
    *@brief 设置当前使用的称呼
    */
    void SetActiveTitle(UInt32 titleId);

	/**
	*@breif 获取当前使用的称号
	*/
	UInt32 GetActiveTitle() { return m_titleId; }
public:
	// 武道大会

	/**
	*@brief 武道大会状态
	*/
	WudaoStatus GetWudaoStatus() const { return (WudaoStatus)(UInt8)m_WudaoStatus; }
	void SetWudaoStatus(WudaoStatus status) { m_WudaoStatus = (UInt8)status; }

	/**
	*@brief 参加武道大会
	*/
	UInt32 JoinWudaodahui();

	/**
	*@brief 武道大会结算
	*/
	void OnWudaodahuiPkEnd(PkRaceResult result);

	/**
	*@brief 获取武道大会奖励
	*/
	UInt32 RequestWudaodahuiReward();

	/**
	*@brief 检查武道大会状态
	*/
	void CheckWudaodahuiStatus(bool dayChanged);

public:
	// 公会相关

	/**
	*@brief 公会ID
	*/
	inline void SetGuildId(ObjID_t guildId) { m_GuildId = guildId; }
	inline ObjID_t GetGuildId() { return m_GuildId; }

	/**
	*@brief 公会名
	*/
	inline void SetGuildName(const std::string& name) { m_GuildName = name; }
	inline std::string GetGuildName() const { return m_GuildName; }

	/**
	*@brief 公会(对应枚举GuildPost)
	*/
	inline void SetGuildPost(UInt8 post) { m_GuildPost = post; }
	inline UInt8 GetGuildPost() const { return m_GuildPost; }

	/**
	*@brief 加入公会时间
	*/
	inline void SetGuildEnterTime(UInt32 time) { m_GuildEnterTime = time; }
	inline UInt32 GetGuildEnterTime() { return m_GuildEnterTime; }

	/**
	*@brief 公会贡献
	*/
	void AddGuildContribution(const char* reason, UInt32 num, bool syncToWorld = false);
	void RemoveGuildContribution(const char* reason, UInt32 num);
	inline UInt32 GetGuildContribution() const { return m_GuildContri; }

	/**
	*@brief 请求公会兑换
	*/
	void RequestGuildExchange();

	/**
	*@brief 公会商店
	*/
	inline UInt8 GetGuildShopID() const { return m_GuildShopId; };
	inline void SetGuildShopID(UInt8 id) { m_GuildShopId = id; }

	/**
	*@brief 公会建筑
	*/
	void SetGuildBuildingLevel(GuildBuildingType type, UInt8 level);
	UInt8 GetGuildBuildingLevel(GuildBuildingType type);

	/**
	*@brief 工会跨服领地
	*/
	void SetGuildOccupyCrossTerrId(UInt8 terrId) { m_GuildOccupyCrossTerrId = terrId; }
	UInt8 GetGuildOccupyCrossTerrId() { return m_GuildOccupyCrossTerrId; }
	

	/**
	*@brief 红点
	*/
	void SetRedPointFlag(UInt32 flag) { m_RedPointFlag = flag; }
	UInt32 GetRedPointFlag() const { return m_RedPointFlag; }
	void AddRedPoint(RedPointFlag flag) { m_RedPointFlag = m_RedPointFlag | (1 << flag); }
	void ClearRedPoint(RedPointFlag flag) { m_RedPointFlag = m_RedPointFlag & (~(1 << flag)); }

	/**
	*@brief 设置公会战次数
	*/
	void SetGuildBattleNumber(UInt32 num) { m_GuildBattleNumber = num; }

	/**
	*@brief 设置公会战积分
	*/
	void SetGuildBattleScore(UInt32 score) { m_GuildBattleScore = score; }

	/**
	*@brief 设置公会抽奖状态
	*/
	void SetGuildBattleLotteryStatus(UInt8 status) { m_GuildBattleLotteryStatus = status; }

	/**
	*@brief 设置公会战奖励领取情况
	*/
	void SetGuildBattleReward(std::string mask);

	/**
	*@brief 公会徽记
	*/
	void SetGuildEmblem(UInt32 lvl);
	UInt32 GetGuildEmblem() { return m_GuildEmblem; }

public:
	// 雕像
	void OnRemoveFigureStatus(FigureStatueInfo info);

	void OnCreateFigureStatus(FigureStatueInfo info);

public:
	// 城镇筛选相关

	/**
	*@brief 广播到新区域
	*/
	virtual void BroadcastNewZone(Avalon::Packet* packet);
	virtual void BroadcastOldZone(Avalon::Packet* packet);

	virtual void Broadcast(Avalon::Protocol& protocol);
	virtual void Broadcast(Avalon::Packet* packet);
	virtual void BroadcastExceptMe(Avalon::Protocol& protocol);
	virtual void BroadcastExceptMe(Avalon::Packet* packet);

	/**
	*@brief 添加周围的玩家
	*/
	void AddAroundPlayer(Player* player, bool notify);

	/**
	*@brief 添加周围所有的玩家
	*/
	void AddAllAroundPlayers();

	/**
	*@brief 添加周围的玩家
	*/
	void DelAroundPlayer(Player* player, bool notify);

	/**
	*@brief 删除周围所有的玩家
	*/
	void DelAllAroundPlayers();

	/**
	*@brief 获取周围观察自己的所有玩家列表
	*/
	void GetWatchPlayerList(std::vector<ObjID_t>& roleIdList, bool containSelf);

	/**
	*@brief 是否观察该目标
	*/
	bool IsWatchThisPlayer(ObjID_t id);

	/**
	*@brief 计算新的观察列表
	*/
	void CountNewWatchList(ObjID_t changedRoleId, bool notify);

	/**
	*@brief 计算新的观察列表
	*/
	void CountNewWatchList2(bool sync);

	/**
	*@brief 计算新的观察列表(3v3场景下的同步方式)
	*/
	void CountNewWatchList_3V3(bool sync);

	/**
	*@brief 获取最多观察玩家数量
	*/
	UInt32 GetMaxWatchPlayerNum();

public:
	// 快速购买相关

	/**
	*@brief 开始快速购买事务
	*/
	UInt32 BeginQuickBuyTransaction(QuickBuyTransaction* trans);

	/**
	*@brief 快速购买事务返回
	*/
	void OnQuickBuyTransactionRes(UInt32 id, UInt32 result);

public:
	/**
	*@brief 其它玩家查看自己的信息
	*/
	void OnOhterPlayerWatch(PlayerWatchInfo& info);

public: //added by aprilliu, 2016.04.12
	void HandleProtocol(CLProtocol::SceneAcceptTask& request);
	void HandleProtocol(CLProtocol::SceneSubmitTask& request);

public: //商店
	inline ShopMgr& GetShopMgr() { return m_ShopMgr; }

public: //商城
	//购买商城商品
	void OnBuyMallGoods(CLProtocol::WorldMallCheckBuy& req);

	//装备碎掉后查询激活条件
	MallGiftPackActivateCond CheckEquipBrokenActivateCond();

	//装备碎掉后获取激活条件
	MallGiftPackActivateCond GetActivateCond(PlayerLevel level);

	//激活商城限时礼包请求
	void ActivateMallGiftPackReq(MallGiftPackActivateCond cond);

	//玩家升级后检查激活礼包
	void CheckMallGiftPackOnLevelUp();

	/**
	*@brief 设置条件触发数据库加载状态
	*/
	void SetConditionActivateDBLoadState(UInt8 loadState) { m_ConditionActivateDBLoadState = loadState; }

public: 
	// 红包
	void AddRedPacket(UInt16 reason);

	void OnNewYearRedPacket(UInt32 num);

	void SyncRedPacketInfoToCenter();

public:
	// 赏金联赛

	/**
	*@brief 赏金联赛报名成功
	*/
	void OnPremiumLeagueEnroll();

public:
	// 充值

	/**
	*@brief 发送充值货物
	*/
	UInt32 GiveChargeGoods(const CLProtocol::SceneBillingSendGoods& goods);

	/**
	*@brief 月卡
	*/
	void OnBuyMonthCard(const char* reason);

	/**
	*@brief 添加月卡福利
	*/
	void OnMonthCardWelfare(UInt32 remainDays, bool isRenew, bool bDayChange = false, bool isOnline = false);

	/**
	*@brief 初始化月卡到期时间
	*/
	void InitMonthCardExpireTime(UInt32 time);

	/**
	*@brief 有没有月卡
	*/
	bool HasMonthCard() const;

	/**
	*@brief 获取月卡剩余天数
	*/
	UInt32 GetMonthCardRemainDays() const;

	UInt32 GetTotChargeNum() const;
	UInt32 GetDayChargeNum() const;
	void AddPlayerChargeTotal(UInt32 deltaCharge);
	UInt32 GetPlayerChargeTotal() const;

	/**
	*@brief 理财卡
	*/
	void OnBuyMoneyManageCard(const char* reason, bool isSendPoint = true);

	/**
	*@brief 初始化理财管理
	*/
	void InitMoneyManage(UInt8 status);

	/**
	*@brief 获取理财管理奖励
	*/
	UInt32 GiveMoneyManageReward(UInt32 rewardId);

	/**
	*@brief 理财卡登陆处理
	*/
	void UpdateMoneyManage();

public:
	//房间
	UInt32 GetRoomId() { return m_RoomId; }
	void SetRoomId(UInt32 roomId) { m_RoomId = roomId; }

public:
	// 预约角色
	void CheckAppointmentOccu();
	void OnAppointmentActivityClose();
	bool IsAppointmentOccu() const { return m_AppointmentOccu != 0; }
	void ClearAppointmentOccu() { m_AppointmentOccu = 0; }


	void AddAppointmentCoin(const char* reason, UInt32 num);
	void RemoveAppointmentCoin(const char* reason, UInt32 num);
	UInt32 GetAppointmentCoin();

public:
	// 城镇npc
	void SummonTaskNpc(Task* task, bool notify);

	void OnKillCityMonster(ObjID_t raceId, const SceneNpc& cityMonster, std::vector<ObjID_t>& teammateIds);

	UInt32 GetDailyCityMonsterRemainTimes();
	
	void IncDailyCityMonsterTimes();

	UInt32 GetDailyCityMonsterTimes();

	void SyncCityMonsterRemainTimesToWorld();

public:
	//装备回收
	//处理装备回收提交请求
	UInt32 handleEqRecSubcmt(std::vector<UInt64>& eqUids, std::vector<EqRecScoreItem>& scoreItems, UInt32& score);

	//装备赎回请求
	UInt32 handleEqRecRedeem(UInt64 equid, UInt32& consScore);

	//装备回收积分提升请求
	UInt32 handleEqRecUpscore(UInt64 equid, UInt32& upscore);

public:
	//33积分赛
	UInt32 GetScoreWarScore() { return m_ScoreWarScore; }
	void SetScoreWarScore(UInt32 score) { m_ScoreWarScore = score; }

	UInt8 GetScoreWarBattleCount() { return m_ScoreWarBattleCount; }
	void SetScoreWarBattleCount(UInt8 count) { m_ScoreWarBattleCount = count; }

	UInt8 GetScoreWarWinBattleCount() { return m_ScoreWarWinBattleCount; }
	void SetScoreWarWinBattleCount(UInt8 count) { m_ScoreWarWinBattleCount = count; }
	void IncScoreWarWinBattleCount() { m_ScoreWarWinBattleCount = m_ScoreWarWinBattleCount + 1; }

	UInt32 GetScoreWarLastBattleTime() { return m_ScoreWarLastBattleTime; }
	void SetScoreWarLastBattleTime(UInt32 time) { m_ScoreWarLastBattleTime = time; }

	bool CheckScoreWarReward(UInt32 rewardId) { return m_ScoreWarRewardMask.CheckMask(rewardId); }
	void SetScoreWarReward(UInt32 rewardId) { m_ScoreWarRewardMask.SetMask(rewardId); }
	void ClearScoreWarReward() { m_ScoreWarRewardMask.ClearMask(); }

	void UpdateScoreWarStraightCount(PkRaceResult result);
	UInt32 GetScoreWarWinStraight();
	UInt32 GetScoreWarLoseStraight();
	void InitScoreWarStraightCount();

	UInt32 GiveScoreWarReward(UInt32 rewardId);

	void SendScoreWarSortListInfo();

public: // 抽奖
	/**
	*@brief 深渊票
	*/
	UInt32 DrawPrizeForHellTicket(UInt32 drawPrizeId, UInt32& rewardId);

	/**
	*@brief 抽奖券
	*/
	UInt32 DrawPrizeForRaffleTicket(UInt32 drawPrizeId, UInt32& rewardId);

public: // 神器罐子活动

	/**
	*@brief 神器罐子折扣信息同步
	*/
	void SyncArtifactJarDiscountInfo(UInt32 opActId, UInt8 extractDiscountStatus = 0, UInt32 discountRate = 0, UInt32 discountEffectTimes = 0);

public: //计数
	inline UInt32 GetCount(const char* name)	{ return GetCounterMgr().GetCounter(name); }

	void OnSysRecordDataRes(const CLProtocol::SceneSysRecordDataRes& protocol);

	// 同步sys record
	void SyncSysRecordData(UInt32 rareType, UInt32 parm1, int num);

	// 请求sys record数据
	void RequestSysRecordData(UInt8 sysRecordEvent, UInt8 rareControlType, const std::vector<UInt32>& params, const std::vector<UInt32>& extraParam);

	/**
	*@brief 同步基本信息到world
	*/
	void SyncBaseInfoToWorld();
	void SyncBaseInfoToWorld(UInt16 oldLv);
	void SyncBaseInfoToWorld(const char* incExpReason, UInt64 incExp);

	inline void SetRefTime(UInt64 time)	{ m_RefTime = time; }
	inline UInt64 GetRefTime()	{ return m_RefTime; }

	inline void SetTotalOnlineTime(UInt32 time)	{ m_totOnlineTime = time; }
	inline UInt32 GetTotelOnlineTime()	{ return m_totOnlineTime; }

public:
	void OnGetMallItem(IEventParam* param);
	inline ObjID_t GetMallGetItemID()	{ return m_MallItemId; }
	inline void ClearMallGetItem()	{ m_MallItemId = 0; }

public:
	bool IsReadedNotify(UInt32 type, UInt32 param);
	void ReadNotify(UInt32 type, UInt64 param);
	void ClearReadNotify(UInt32 type, UInt32 param);

	void SendNotifyTimeItemState(std::vector<Item*>& items);

	// 头像框
public:
	HeadFrameMgr& GetHeadFrameMgr() { return m_HeadFrameMgr; }
	void SetHeadFrameId(UInt32 id) { m_HeadFrameId = id; }
	UInt32 GetHeadFrameId() { return m_HeadFrameId; }

#ifdef _DEBUG
public:
	void SyncDebugChat(std::string word);
#endif

private:
	bool CheckPublishTask(UInt32 npcID, UInt32 taskID);
	bool CheckSubmitTask(UInt32 npcID, UInt32 taskID);

public: //编解码相关
	
	CL_SYNC_PROPERTY_BEGIN(Creature)
	CL_SYNC_PROPERTY(m_ZoneId, SOA_ZONEID, SST_SCENE | SST_AROUND | SST_TEST | SST_BATTLE | SST_LOSTDUNGEON)
	CL_SYNC_PROPERTY(m_Exp, SOA_EXP, SST_SELF | SST_SCENE | SST_TEST)
	CL_SYNC_PROPERTY(m_BlessExp,	SOA_BLESSEXP,	SST_SELF|SST_SCENE)
	CL_SYNC_PROPERTY(m_Occu, SOA_OCCU, SST_SELF | SST_AROUND | SST_WATCH | SST_SCENE | SST_CHALLENGE | SST_TEST | SST_BATTLE | SST_LOSTDUNGEON)
	CL_SYNC_PROPERTY(m_ScenePos, SOA_SCENE_POS, SST_SCENE_ENTER)
	CL_SYNC_PROPERTY(m_SceneDir, SOA_SCENE_DIR, SST_SCENE_ENTER)
	CL_SYNC_PROPERTY(m_VipLvl, SOA_VIPLVL, SST_SELF | SST_WATCH | SST_SCENE | SST_CHALLENGE | SST_AROUND | SST_BATTLE | SST_LOSTDUNGEON)
	CL_SYNC_PROPERTY(m_qqVipInfo,	SOA_QQVIPINFO,		SST_SELF|SST_WATCH|SST_SCENE|SST_CHALLENGE)
	CL_SYNC_PROPERTY(m_Gold,		SOA_GOLD,		SST_SELF|SST_SCENE|SST_TEST)
	CL_SYNC_PROPERTY(m_BindGold,	SOA_BINDGOLD,	SST_SELF|SST_SCENE|SST_TEST)
	CL_SYNC_PROPERTY(m_Point,		SOA_POINT,		SST_SELF|SST_SCENE|SST_TEST)
	CL_SYNC_PROPERTY(m_BindPoint,	SOA_BINDPOINT, SST_SELF | SST_SCENE|SST_TEST)
	//CL_SYNC_PROPERTY(m_SpiritStone, SOA_SPIRITSTONE,	SST_SELF|SST_SCENE)
	CL_SYNC_PROPERTY(m_Honour,		SOA_HONOUR,		SST_SELF|SST_WATCH|SST_SCENE)
	CL_SYNC_PROPERTY(m_PackSize,	SOA_PACKSIZE,	SST_SELF|SST_SCENE|SST_TEST)
	CL_SYNC_PROPERTY(m_StorageSize,	SOA_STORAGESIZE,	SST_SELF|SST_SCENE| SST_TEST)
	CL_SYNC_PROPERTY(m_SuitEffect, SOA_SUITEFFECT, SST_SELF | SST_WATCH | SST_CHALLENGE)
	CL_SYNC_PROPERTY(m_SkillBar,	SOA_SKILLBAR,	SST_SELF|SST_SCENE|SST_TEST)
	//CL_SYNC_PROPERTY(m_ItemBar,		SOA_ITEMBAR,	SST_SELF|SST_SCENE|SST_TEST)
	CL_SYNC_PROPERTY(m_Options,		SOA_GAMEOPTIONS,	SST_SELF|SST_SCENE)
	CL_SYNC_PROPERTY(m_ItemCD,		SOA_ITEMCD,		SST_SELF|SST_SCENE|SST_TEST)
	CL_SYNC_PROPERTY(m_isTeamMaster,	SOA_TEAMMASTER,	SST_SELF)
	CL_SYNC_PROPERTY(m_TeamSize,	SOA_TEAMSIZE,	SST_SELF)
	CL_SYNC_PROPERTY(m_StoryId,		SOA_STORY,		SST_SELF|SST_SCENE)
	CL_SYNC_PROPERTY(m_titleId, SOA_TITLE, SST_SELF | SST_AROUND | SST_SCENE | SST_CHALLENGE | SST_TEST)
	CL_SYNC_PROPERTY(m_UpdateBulletin, SOA_UPDATEBULLETIN,	SST_SELF|SST_SCENE)
	CL_SYNC_PROPERTY(m_FuncNotify, SOA_FUNCNOTIFY, SST_SELF|SST_SCENE|SST_TEST)
	CL_SYNC_PROPERTY(m_ActionFlag,	SOA_ACTIONFLAG, SST_SELF|SST_SCENE)
	CL_SYNC_PROPERTY(m_FuncFlag,	SOA_FUNCFLAG,	SST_SELF|SST_SCENE|SST_TEST)
	CL_SYNC_PROPERTY(m_SrcZoneId,	SOA_SRCZONEID,	SST_SELF|SST_SCENE)
	CL_SYNC_PROPERTY(m_DayChargeNum, SOA_DAYCHARGENUM, SST_SELF | SST_SCENE | SST_TEST)
	CL_SYNC_PROPERTY(m_NetSp, SOA_SP, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_Awaken, SOA_AWAKEN, SST_SELF | SST_SCENE | SST_TEST)
    CL_SYNC_PROPERTY(m_pkValue, SOA_PKVALUE, SST_SELF | SST_TEST | SST_SCENE | SST_AROUND)
	CL_SYNC_PROPERTY(m_Fatigue, SOA_FATIGUE, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_WarriorSoul, SOA_WARRIOR_SOUL, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_MatchScore, SOA_MATCH_SCORE, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_CounterMgr, SOA_COUNTER, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_PkCoin, SOA_PK_COIN, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_RetinueMgr.GetSceneRetinueInfo(), SOA_RETINUE, SST_SELF | SST_TEST | SST_SCENE | SST_AROUND)
	CL_SYNC_PROPERTY(m_avatar, SOA_AVATAR, SST_SELF | SST_TEST | SST_SCENE | SST_AROUND | SST_LOSTDUNGEON)
	CL_SYNC_PROPERTY(m_reviveCoin, SOA_REVIVE_COIN, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_Vip, SOA_VIP, SST_SELF | SST_TEST | SST_SCENE | SST_AROUND | SST_BATTLE | SST_LOSTDUNGEON)
	CL_SYNC_PROPERTY(m_CreateTime, SOA_CREATE_TIME, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_NewBoot, SOA_NEW_BOOT, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_BootFlag, SOA_BOOT_FLAG, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_DeathTowerWipeoutEndTime, SOA_TOWER_WIPEOUT_END_TIME, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_GuildName, SOA_GUILD_NAME, SST_SELF | SST_TEST | SST_SCENE | SST_AROUND)
	CL_SYNC_PROPERTY(m_GuildPost, SOA_GUILD_POST, SST_SELF | SST_TEST | SST_SCENE | SST_AROUND)
	CL_SYNC_PROPERTY(m_GuildContri, SOA_GUILD_CONTRI, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_GuildShopId, SOA_GUILD_SHOP, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_RedPointFlag, SOA_RED_POINT, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_GuildBattleNumber, SOA_GUILD_BATTLE_NUMBER, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_GuildBattleScore, SOA_GUILD_BATTLE_SCORE, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_GuildBattleRewardMask, SOA_GUILD_BATTLE_REWARD, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_DailyTaskScore, SOA_DAILY_TASK_SCORE, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_DailyTaskRewardMask, SOA_DAILY_TASK_REWARD_MASK, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_WudaoStatus, SOA_WUDAO_STATUS, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_MonthCardExpireTime, SOA_MONTH_CARD_EXPIRE_TIME, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_CustomField, SOA_CUSTOM_FIELD, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_SeasonLevel.GetSeasonLevel(), SOA_SEASON_LEVEL, SST_SELF | SST_TEST | SST_SCENE | SST_AROUND)
	CL_SYNC_PROPERTY(m_SeasonLevel.GetSeasonStar(), SOA_SEASON_STAR, SST_SELF | SST_TEST | SST_SCENE | SST_AROUND)
	CL_SYNC_PROPERTY(m_SeasonLevel.GetSeasonExp(), SOA_SEASON_EXP, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_SeasonLevel.GetSeasonUplevelBattle(), SOA_SEASON_UPLEVEL_RECORD, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_SeasonLevel.GetSeasonAttr(), SOA_SEASON_ATTR, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_SeasonLevel.GetSeasonKingMarkCount(), SOA_SEASON_KING_MARK, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_AuctionRefreshTime, SOA_AUCTION_REFRESH_TIME, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_AuctionAdditionBooth, SOA_AUCTION_ADDITION_BOOTH, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_GoldJarPoint, SOA_GOLDJAR_POINT, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_MagJarPoint, SOA_MAGJAR_POINT, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_PetMgr.GetFollowPetDataId(), SOA_PET_FOLLOW, SST_SELF | SST_TEST | SST_SCENE | SST_AROUND | SST_LOSTDUNGEON)
	CL_SYNC_PROPERTY(m_potionSet, SOA_POTION_SET, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_PreOccu, SOA_PRE_OCCU, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_GuildBattleLotteryStatus, SOA_GUILD_BATTLE_LOTTERY_STATUS, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_PvpSkillMgr, SOA_PVP_SKILLS, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_NetSp, SOA_PVP_SP, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_PvpSkillBar, SOA_PVP_SKILLBAR, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_AchievementScore, SOA_ACHIEVEMENT_SCORE, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_AchievementTaskRewardMask, SOA_ACHIEVEMENT_TASK_REWARD_MASK, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_WeaponBar, SOA_WEAPON_BAR, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_AppointmentOccu, SOA_APPOINTMENT_OCCU, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_PackSizeAddition, SOA_PACKAGE_SIZE_ADDITION, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_MoneyManageStatus, SOA_MONEY_MANAGE_STATUS, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_MoneyManageRewardMask, SOA_MONEY_MANAGE_REWARD_MASK, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_ScoreWarScore, SOA_SCORE_WAR_SCORE, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_ScoreWarBattleCount, SOA_SCORE_WAR_BATTLE_COUNT, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_ScoreWarRewardMask, SOA_SCORE_WAR_REWARD_MASK, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_ScoreWarWinBattleCount, SOA_SCORE_WAR_WIN_BATTLE_COUNT, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_AcademicTotalGrowth, SOA_ACADEMIC_TOTAL_GROWTH_VALUE, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_MasterDailyTaskGrowth, SOA_MASTER_DAILYTASK_GROWTH, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_MasterAcademicTaskGrowth, SOA_MASTER_ACADEMICTASK_GROWTH, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_MasterUplevelGrowth, SOA_MASTER_UPLEVEL_GROWTH, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_MasterGiveEquipGrowth, SOA_MASTER_GIVEEQUIP_GROWTH, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_MasterGiveGiftGrowth, SOA_MASTER_GIVEGIFT_GROWTH, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_MasterTeamClearDungeonGrowth, SOA_MASTER_TEAMCLEARDUNGON_GROWTH, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_GoodTeacherValue, SOA_MASTER_GOODTEACHER_VALUE, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_RoomId, SOA_ROOM_ID, SST_CHALLENGE)
	CL_SYNC_PROPERTY(m_ShowFashionWeapon, SOA_SHOW_FASHION_WEAPON, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_weaponLeaseTickets, SOA_WEAPON_LEASE_TICKETS, SST_SELF | SST_SCENE | SST_TEST)
	CL_SYNC_PROPERTY(m_GameSet, SOA_GAME_SET, SST_SELF | SST_SCENE | SST_TEST)
	CL_SYNC_PROPERTY(m_AdventureTeamName, SOA_ADVENTURE_TEAM_NAME, SST_SELF | SST_AROUND | SST_WATCH | SST_SCENE | SST_CHALLENGE | SST_TEST)
	CL_SYNC_PROPERTY(m_HeadFrameId, SOA_HEAD_FRAME, SST_SELF | SST_SCENE | SST_TEST)
	CL_SYNC_PROPERTY(m_NewTitleName, SOA_NEW_TITLE_NAME, SST_SELF | SST_AROUND | SST_WATCH | SST_SCENE | SST_CHALLENGE | SST_TEST)
	CL_SYNC_PROPERTY(m_NewTitleGuid, SOA_NEW_TITLE_GUID, SST_SELF | SST_AROUND | SST_WATCH | SST_SCENE | SST_CHALLENGE | SST_TEST)
	CL_SYNC_PROPERTY(m_ChijiHp, SOA_CHIJI_HP, SST_SELF | SST_TEST)
	CL_SYNC_PROPERTY(m_ChijiMp, SOA_CHIJI_MP, SST_SELF | SST_TEST)
	CL_SYNC_PROPERTY(m_PackageTypeStr, SOA_PACKAGE_TYPE, SST_SELF | SST_SCENE | SST_TEST)
	CL_SYNC_PROPERTY(m_GuildEmblem, SOA_GUILD_EMBLEM, SST_SELF | SST_AROUND | SST_WATCH | SST_SCENE | SST_TEST)
	CL_SYNC_PROPERTY(m_OppoVipLevel, SOA_OPPO_VIP_LEVEL, SST_SELF | SST_SCENE | SST_TEST)
	CL_SYNC_PROPERTY(m_ChijiScore, SOA_CHIJI_SCORE, SST_SELF | SST_SCENE | SST_TEST)
	CL_SYNC_PROPERTY(m_LostDungeonHp, SOA_LOSTDUNGEON_HP, SST_SELF | SST_TEST | SST_LOSTDUNG_ENTER_SYNC | SST_AROUND)
	CL_SYNC_PROPERTY(m_LostDungeonMp, SOA_LOSTDUNGEON_MP, SST_SELF | SST_TEST | SST_LOSTDUNG_ENTER_SYNC | SST_AROUND)
	CL_SYNC_PROPERTY(m_LostDungeonScore, SOA_LOSTDUNGEON_SCORE, SST_SELF | SST_SCENE | SST_TEST | SST_AROUND)
	CL_SYNC_PROPERTY(m_lostDungBatSt, SOA_LOSTDUNGEON_BATTLE_ST, SST_SELF | SST_SCENE | SST_TEST | SST_AROUND)
		
	CL_SYNC_PROPERTY_END()

	CL_DBPROPERTY_BEGIN(Creature)
	CL_DBPROPERTY("zoneid",		m_ZoneId)
	CL_DBPROPERTY("exp",		m_Exp)
	CL_DBPROPERTY("blessexp",	m_BlessExp)
	CL_DBPROPERTY("occu",		m_Occu)
	CL_DBPROPERTY("pre_occu",	m_PreOccu)
	CL_DBPROPERTY("tourzone",	m_TourZoneId)
	//CL_DBPROPERTY("viplvl",		m_VipLvl)
	//CL_DBPROPERTY("viplvl", m_Vip.GetLevel())
	//CL_DBPROPERTY("vipexp", m_Vip.GetExp())
	CL_DBPROPERTY("vipduetime",	m_VipDueTime)
	CL_DBPROPERTY("createtime",	m_CreateTime)
	CL_DBPROPERTY("gold",		m_Gold)
	CL_DBPROPERTY("bindgold",	m_BindGold)
	CL_DBPROPERTY("bindpoint",	m_BindPoint)
	CL_DBPROPERTY("totlechargenum", m_TotleChargeNum)
	CL_DBPROPERTY("honour",		m_Honour) 
	CL_DBPROPERTY("packsize",	m_PackSize)
	//CL_DBPROPERTY("storagesize",	m_StorageSize)
	CL_DBPROPERTY("skillbar",	m_SkillBar)
	CL_DBPROPERTY("pvp_skillbar", m_PvpSkillBar)
	CL_DBPROPERTY("options",	m_Options)
	CL_DBPROPERTY("taskmask",	m_TaskMask)
	CL_DBPROPERTY("storymask",	m_StoryMask)
	CL_DBPROPERTY("story",		m_StoryId)
	CL_DBPROPERTY("itemcd",		m_ItemCD)
	CL_DBPROPERTY("routine",	m_RoutineMgr)
	CL_DBPROPERTY("actionflag",	m_ActionFlag)
	CL_DBPROPERTY("updatebulletin",	m_UpdateBulletin)
	CL_DBPROPERTY("revivecost",	m_ReviveCost)
	CL_DBPROPERTY("revivetime",	m_ReviveTime)
	CL_DBPROPERTY("continuousdays", m_ContinuousDays)
	CL_DBPROPERTY("funcflag",	m_FuncFlag)
	CL_DBPROPERTY("actioncount",	m_ActionCounterMgr)
	//CL_DBPROPERTY("totleonlinetime", m_totOnlineTime)	//用来存总的在线时间
	CL_DBPROPERTY("equipshop", m_EquipShopMgr)
	CL_DBPROPERTY("sp", m_SP)
	CL_DBPROPERTY("pvp_sp", m_PvpSP)
	CL_DBPROPERTY("awaken", m_Awaken)
    CL_DBPROPERTY("pkvalue", m_pkValue)
	CL_DBPROPERTY("fatigue", m_Fatigue)
    CL_DBPROPERTY("warrior_soul", m_WarriorSoul)
	CL_DBPROPERTY("pk_coin", m_PkCoin)
    CL_DBPROPERTY("match_score", m_MatchScore)
	CL_DBPROPERTY("retinuefighting", m_RetinueMgr.GetRetinueFighting())
	CL_DBPROPERTY("avatar", m_avatar);
	CL_DBPROPERTY("revive_coin", m_reviveCoin);
	CL_DBPROPERTY("offretinuelist", m_RetinueMgr.GetOffRetinueList());
	CL_DBPROPERTY("bootflag", m_BootFlag);
	CL_DBPROPERTY("newboot", m_NewBoot);
	CL_DBPROPERTY("tower_wipeout_end_time", m_DeathTowerWipeoutEndTime);
	CL_DBPROPERTY("guild_contri", m_GuildContri);
	CL_DBPROPERTY("redpoint", m_RedPointFlag);
	CL_DBPROPERTY("daily_task_score", m_DailyTaskScore);
	CL_DBPROPERTY("daily_task_reward_mask", m_DailyTaskRewardMask);
	CL_DBPROPERTY("wudao_status", m_WudaoStatus);
	CL_DBPROPERTY("forbid_talk_duetime", m_ForbitTalkDueTime);
	CL_DBPROPERTY("custom_field", m_CustomField);
	CL_DBPROPERTY("season_id", m_SeasonLevel.GetSeasonId());
	CL_DBPROPERTY("season_level", m_SeasonLevel.GetSeasonLevel());
	CL_DBPROPERTY("season_star", m_SeasonLevel.GetSeasonStar());
	CL_DBPROPERTY("season_exp", m_SeasonLevel.GetSeasonExp());
	CL_DBPROPERTY("season_change_time", m_SeasonLevel.GetSeasonChangeTime());
	CL_DBPROPERTY("season_play_status", m_SeasonLevel.GetSeasonPlayStatus());
	CL_DBPROPERTY("season_uplevel_records", m_SeasonLevel.GetSeasonUplevelBattle());
	CL_DBPROPERTY("season_attr", m_SeasonLevel.GetSeasonAttr());
	CL_DBPROPERTY("season_king_mark_count", m_SeasonLevel.GetSeasonKingMarkCount());
	CL_DBPROPERTY("season_attr_end_time", m_SeasonLevel.GetChangeSeasonAttrTime());

	CL_DBPROPERTY("auction_refresh_time", m_AuctionRefreshTime);
	CL_DBPROPERTY("auction_addition_booth", m_AuctionAdditionBooth);
	CL_DBPROPERTY("platform", m_Pf);
	CL_DBPROPERTY("openid", m_OpenId);
	CL_DBPROPERTY("last_reset_gjarpoint", m_LastResetGoldJarPointTime);
	CL_DBPROPERTY("last_reset_mjarpoint", m_LastResetMagJarPointTime);
	CL_DBPROPERTY("goldjar_point", m_GoldJarPoint);
	CL_DBPROPERTY("magjar_point", m_MagJarPoint);

	CL_DBPROPERTY("pet_battle", m_PetMgr.GetBattlePetMap());
	CL_DBPROPERTY("pet_follow", m_PetMgr.GetFollowPetId());
	//CL_DBPROPERTY("read_notify", m_readNotify);
	CL_DBPROPERTY("potion_set", m_potionSet);
	CL_DBPROPERTY("month_card_expire_time", m_MonthCardExpireTime);
	CL_DBPROPERTY("is_check_sp", m_IsCheckSp);
	CL_DBPROPERTY("pvp_skills", m_PvpSkillMgr);
	CL_DBPROPERTY("sky_fashion_get", m_FashionSkyGetBit);
	CL_DBPROPERTY("achievement_score", m_AchievementScore);
	CL_DBPROPERTY("daychargenum", m_DayChargeNum)
	CL_DBPROPERTY("achievement_task_reward_mask", m_AchievementTaskRewardMask);
	CL_DBPROPERTY("weapon_bar", m_WeaponBar);
	CL_DBPROPERTY("is_appointment_occu", m_AppointmentOccu);
	CL_DBPROPERTY("money_manage_reward_mask", m_MoneyManageRewardMask)
	CL_DBPROPERTY("money_manage_status", m_MoneyManageStatus)
	
	CL_DBPROPERTY("score_war_score", m_ScoreWarScore)
	CL_DBPROPERTY("score_war_battle_count", m_ScoreWarBattleCount)
	CL_DBPROPERTY("score_war_win_battle_count", m_ScoreWarWinBattleCount)
	CL_DBPROPERTY("score_war_reward_mask", m_ScoreWarRewardMask)
	CL_DBPROPERTY("score_war_last_battle_time", m_ScoreWarLastBattleTime)

	CL_DBPROPERTY("academic_total_growth", m_AcademicTotalGrowth)
	CL_DBPROPERTY("master_dailytask_growth", m_MasterDailyTaskGrowth)
	CL_DBPROPERTY("master_academictask_growth", m_MasterAcademicTaskGrowth)
	CL_DBPROPERTY("master_giveequip_growth", m_MasterGiveEquipGrowth)
	CL_DBPROPERTY("master_uplevel_growth", m_MasterUplevelGrowth)
	CL_DBPROPERTY("master_givegift_growth", m_MasterGiveGiftGrowth)
	CL_DBPROPERTY("master_teamcleardungeon_growth", m_MasterTeamClearDungeonGrowth)
	CL_DBPROPERTY("good_teacher_value", m_GoodTeacherValue)
	CL_DBPROPERTY("fin_sch_disciple_sum", m_FinSchDiscipleSum)

	CL_DBPROPERTY("score_war_score", m_ScoreWarScore)
	CL_DBPROPERTY("score_war_battle_count", m_ScoreWarBattleCount)
	CL_DBPROPERTY("score_war_reward_mask", m_ScoreWarRewardMask)
	CL_DBPROPERTY("score_war_last_battle_time", m_ScoreWarLastBattleTime)

	CL_DBPROPERTY("show_fashion_weapon", m_ShowFashionWeapon)
	CL_DBPROPERTY("game_set", m_GameSet)
	CL_DBPROPERTY("role_value_score", m_RoleValueScore)
	CL_DBPROPERTY("head_frame_id", m_HeadFrameId)
	CL_DBPROPERTY("new_title_guid", m_NewTitleGuid)
	CL_DBPROPERTY("chiji_score", m_ChijiScore)
	CL_DBPROPERTY("chiji_score_reset_time", m_ChijiScoreResetTime)

	CL_DBPROPERTY_END()
	

	DEFINE_CHANGESCENE_SERIALIZATION(Creature)
	{
		stream.SeriaArray(m_TickTime, LOGICTICK_MAX);

		stream & m_TourZoneId;
		stream & m_OpenId;
		stream & m_Source1 & m_Source2;
		stream & m_Inviter;
		stream & m_Pf;
		stream & m_DeviceId;
		stream & m_Model;
		stream & m_DeviceVersion;
		stream & m_LocalSavebackTime;
		stream & m_VipDueTime;
		stream & m_CreateTime;
		stream & m_OnlineTime;
		stream & m_AssetsLockTime & m_RecoverTime & m_BusyTime;

		stream & m_EnterScene & m_EnterPos;
		stream & m_ReviveTime & m_ReviveCost & m_ReviveTimes;

		stream & m_GmAuthority & m_ClientOption & m_ChatTimer;
		stream & m_TaskMask & m_TaskMgr;
		stream & m_StoryMask;
		stream & m_ItemMgr;

		stream & m_CounterMgr & m_ActionCounterMgr;
		stream & m_RoutineMgr;

		stream & m_WallowFactor;

		stream & m_ContinuousDays;

		stream & m_EquipShopMgr;
		stream & m_SP;
		stream & m_Awaken;
        stream & m_dungeonMgr;
        stream & m_pkValue;
        stream & m_MatchScore;
        stream & m_titleId;
        stream & m_WarriorSoul;
		stream & m_PkCoin;
		stream & m_avatar;

		//stream & m_SkillBar;
		//stream & m_ItemBar;
	}

	void SyncToRecord(bool bPos = false);

	//清除无效数据
	void ClearInvalidData();

private:	//游戏系统事件注册
	void _RegistOpActEvent();
	void _UnRegistOpActEvent();

private:
	/**
	 *@brief 获取删除装备改造中的材料
	 */
	bool GetMaterials(const std::vector<ObjID_t>& ids, std::map<Item*,UInt32>& items, UInt8 type);
	void RemoveMaterials(const std::map<Item*,UInt32>& items);
	UInt32 RemoveMaterials(const char* reason, std::vector<Item*>& items, UInt32 num);

	/**
	 *@brief 同步基本信息到gate
	 */
	void SyncBaseInfoToGate();

	//充值统计
	void _OnCharge(const char* reason, UInt32 vipExp, UInt32 chargeNum);

public:
	void OnChargeMakeup(const char* reason, UInt32 vipExp, UInt32 chargeNum);

private:
	//检查是否逻辑跨天
	bool CheckLogicDayChange(const Avalon::Time& now, bool isOnline);
	//新的跨天判断
	bool _NewCheckLogicDayChange(const Avalon::Time& now);
	//老的跨天判断
	bool _OldCheckLogicDayChange();

	//继承老数据跨天判断
	bool CheckLogicDayChangeOld();

	//检查是否自然日跨天
	bool CheckDayChange(const Avalon::Time& now);
	//继承老数据跨天判断
	bool CheckDayChangeOld();
	//新的自然跨天判断
	bool _NewCheckDayChange(const Avalon::Time& now);
	//老的自然跨天判断
	bool _OldCheckDayChange();

	//逻辑跨周判断
	bool _CheckLogicWeekChange(const Avalon::Time& now);
	
public: //师徒
	void	SetActiveTimeType(UInt8 type) { m_activeTimeType = type; }
	UInt8	GetActiveTimeType() { return m_activeTimeType; }

	void	SetMasterType(UInt8 type) { m_masterType = type; }
	UInt8   GetMasterType()	{ return m_masterType; }

	void	SetRegionId(UInt8 regionId) { m_regionId = regionId; }
	UInt8   GetRegionId() { return m_regionId; }

	void	SetDeclaration(std::string str) { m_declaration = str; }
	std::string  GetDeclaration() { return m_declaration; }

	/**
	*@brief 师徒总成长值
	*/
	UInt32 GetMasterSysAcademicGrowth() { return m_AcademicTotalGrowth; }
	void AddMasterSysAcademicGrowth(const char* reason, UInt32 num);
	
	/**
	*@brief 良师值
	*/
	UInt32 GetMasterSysGoodTeachValue() { return m_GoodTeacherValue; }
	void AddMasterSysGoodTeachValue(const char* reason, UInt32 num);
	void RemoveSysGoodTeachValue(const char* reason, UInt32 num);

	/**
	*@brief 师门日常任务总成长值
	*/
	UInt32 GetMasterDailyTaskGrowth() { return m_MasterDailyTaskGrowth; }
	void AddMasterDailyTaskGrowth(const char* reason, UInt32 num);

	/**
	*@brief 师门学业成长任务总成长值
	*/
	UInt32 GetMasterAcademicTaskGrowth() { return m_MasterAcademicTaskGrowth; }
	void AddMasterAcademicTaskGrowth(const char* reason, UInt32 num);

	/**
	*@brief 师门师徒组队通关地下城成长值
	*/
	UInt32 GetMasterTeamClearDungeonGrowth() { return m_MasterTeamClearDungeonGrowth; }
	void AddMasterTeamClearDungeonGrowth(const char* reason, UInt32 num, UInt32 dungeonId);

	/**
	*@brief 师门徒弟升级成长值
	*/
	UInt32 GetMasterUplevelGrowth() { return m_MasterUplevelGrowth; }
	void AddMasterUplevelGrowth(const char* reason, UInt32 num);

	/**
	*@brief 师傅赠送装备成长值
	*/
	UInt32 GetMasterGiveEquipGrowth() { return m_MasterGiveEquipGrowth; }
	void AddMasterGiveEquipGrowth(const char* reason, UInt32 num);

	/**
	*@brief 出师徒弟数量
	*/
	void SetFinSchDiscipleSum(UInt32 value);
	UInt32 GetFinSchDiscipleSum() { return m_FinSchDiscipleSum; }

	/**
	*@brief 获取徒弟数量
	*/
	UInt32 GetDiscipleNum();

	/**
	*@brief 获取玩家师门任务数据
	*/
	void GetMasterTaskShareData(MasterTaskShareData& masterTaskShareData);

	/**
	*@brief 处理师徒关系离线解除
	*/
	static void HandleOfflineRelieveMaster(ObjID_t discipleId);

	/**
	*@brief 发放师门系统礼包
	*/
	UInt32 OnMasterSysGiveGift(UInt8 type, UInt32 giftDataId);

	/**
	*@brief 升级处理师门相关值
	*/
	void OnLevelUpMasterSys();

	/**
	*@brief 处理师傅装备赠送
	*/
	void OnGiveEquipMasterSys(UInt32 num);

public://个人活动
	PlayerActivityInfo<Player>& GetPlayerActivityInfo() { return m_PlayerActivityInfo; }

public://吃鸡
	/**
	*@brief 吃鸡血量
	*/
	Int32 GetChijiHp() const;
	void SetChijiHp(Int32 hp);

	/**
	*@brief 增加、减少血量
	*/
	void DecChijiHp(Int32 value);
	void IncChijiHp(Int32 value);

	/**
	*@brief 吃鸡蓝量
	*/
	Int32 GetChijiMp() const;
	void SetChijiMp(Int32 mp);

	/**
	*@brief 增加、减少蓝量
	*/
	void DecChijiMp(Int32 value);
	void IncChijiMp(Int32 value);

	/**
	*@brief 吃鸡积分
	*/
	void SetChijiScore(UInt32 val) { m_ChijiScore = val; }
	UInt32 GetChijiScore() { return m_ChijiScore; }

	/**
	*@brief 初始化吃鸡包裹
	*/
	void InitChijiPackage();

	/**
	*@brief 学习吃鸡技能
	*/
	void StudyChiJiSkill(UInt32 skillId, UInt32 skillLvl);

	/**
	*@brief 扣除吃鸡商店货币
	*/
	void RemoveChiJiShopCoin(const char* reason, UInt32 num);

	/**
	*@brief 吃鸡免战
	*/
	void SetChijiNoWarInfo(UInt64 enemyRoleId, UInt32 battleId, UInt32 dungeonId, UInt32 choiceTime);
	UInt64 GetChijiNoWarEnemyId() { return m_ChijiNoWarEnemyId; }
	UInt32 GetChijiNoWarBattleId() { return m_ChijiNoWarBattleId; }
	UInt32 GetChijiNoWarChoiceTime() { return m_ChijiNoWarChoiceTime; }
	UInt32 GetChijiPKDungeonId() { return m_ChijiPKDungeonId; }
	void SendNoWarToBattle();

	/**
	*@brief 吃鸡职业列表
	*/
	void CreateChijiOccu();
	bool IsHasChijiOccu(UInt32 occu);

private:
	/**
	*@brief 获取离线数据
	*/
	void GetShareData(PlayerSenceShareData& sharedata);

	/**
	*@brief 同步离线数据到world
	*/
	void SyncShareDataToWorld();

	/**
	*@brief 同步师门系统数据
	*/
	void SyncMasterSysDataToWorld();

public: //抗磨值
	/**
	*@brief 抗魔值
	*/
	void SetResistMagic(UInt32 value) { m_ResistMagic = value; }
	UInt32 GetResistMagic() { return m_ResistMagic; }

	/**
	*@brief 同步抗魔值到world
	*/
	void SyncResistMagicToWorld();

	/**
	*@brief 处理玩家对物品加锁解锁
	*/
	UInt32 HandlerItemOpLock(UInt32 opType, ObjID_t itemId);

	/**
	*@brief 时装武器显示设置
	*/
	UInt32 SetShowFashionWeapon(UInt8 isShow);
	UInt8 GetShowFashionWeapon() { return m_ShowFashionWeapon; }

public: //百变时装活动换装积分
	void AddChangeFashionScore(const char* reason, UInt32 num);
	void RemoveChangeFashionScore(const char* reason, UInt32 num);
	UInt32 GetChangeFashionScore();

	//武器租赁好运符
	void InitWeaponLeaseTickets(UInt32 num) { m_weaponLeaseTickets = num; }
	void AddWeaponLeaseTickets(const char* reason, UInt32 num);
	void RemoveWeaponLeaseTickets(const char* reason, UInt32 num);
	UInt32 GetWeaponLeaseTickets();


	// 是否老兵回归
	bool IsVeteranReturn();
	// 老兵回归检查
	void OnCheckVeteranReturn();

	// 角色回归时间
	void SetRoleReturnTime(UInt32 time) { m_RoleReturnTime = time; }
	UInt32 GetRoleReturnTime() const { return m_RoleReturnTime; }
	UInt32 GetRoleReturnExpireTime();

	// 角色回归等级
	void SetRoleReturnLevel(UInt32 level) { m_RoleReturnLevel = level; }
	UInt32 GetRoleReturnLevel() const { return m_RoleReturnLevel; }

	// 是否再次回归
	void SetAgainReturn(UInt8 isAgain) { m_AgainReturn = isAgain; }
	UInt8 GetAgainReturn() const { return m_AgainReturn; }

	// 精力币
	void SetEnergyCoin(UInt32 num) { m_EnergyCoin = num; }
	UInt32 GetEnergyCoin() const { return m_EnergyCoin; }
	void RefreshEnergyCoin(UInt32 num);

	// 同步账号计数
	void SyncAccountCounterToWorld(UInt32 type);

	// 包裹类型大小
	void UpdatePackageTypeSize();

public: // 游戏设置

	void SetGameSet(UInt32 setType, const std::string& setValue);
	
	/**
	*@brief 解析游戏设置字符
	*/
	void ParseGameSet();
	
	/**
	*@brief 更新游戏设置字符
	*/
	void UpdateGameSet();

	/**
	*@brief 游戏设置同步到world
	*/
	void SyncGameSetWorld();

public:// 角色评分
	/**
	*@brief 设置获取角色评分
	*/
	void SetRoleValueScore(UInt32 score);
	UInt32 GetRoleValueScore() const { return m_RoleValueScore; }

	/**
	*@brief 同步角色评分
	*/
	void SyncRoleValueScore();

	/**
	*@brief 获取等级评分
	*/
	UInt32 GetPlayerLevelScore();

	/**
	*@brief 计算包裹内道具评分
	*/
	UInt32 CalcItemsScoreInPackage(PackType packType, bool reCalc);

	/**
	*@brief 计算宠物评分
	*/
	UInt32 CalcPetsSocre(bool reCalc);

	/**
	*@brief 计算角色评分
	*/
	void CalcRoleValueScore(bool reCalc = false);

public: // 冒险队

	/**
	*@brief 设置获取冒险队等级
	*/
	void SetAdventureTeamLevel(UInt16 level) { m_AdventureTeamLevel = level; }
	UInt16 GetAdventureTeamLevel() const { return m_AdventureTeamLevel; }

	/**
	*@brief 设置获取冒险队评级
	*/
	void SetAdventureTeamGrade(UInt8 grade) { m_AdventureTeamGrade = grade; }
	UInt8 GetAdventureTeamGrade() const { return m_AdventureTeamGrade; }

public: //吃鸡
	UInt32 GetBattleID(void) { return m_BattleID; }
	void SetBabttleID(UInt32 battleID) { m_BattleID = battleID; }
	
public:	//头衔
	/**
	*@breif 玩家头衔修改
	*/
	void OnNewTitleModify(UInt8 opType, UInt8 ownerType, UInt64 titleGuid, std::string titleName);

	void AddNewTitle(UInt32 titleId, UInt32 dueTime);

public: //迷失地牢
	RacePlayerInfo GetLostBattleRacePlayerInfo();
	void SetLostBattleRacePlayerInfo(const RacePlayerInfo& raceInfo) { m_lostBattleRacePlayerInfo = raceInfo; }
	inline void SetDungChageMode(UInt8 mode) { m_dungChageMode = mode; }
	inline UInt8 GetDungChageMode() { return m_dungChageMode; }
	inline void SetDungChageTeamId(UInt32 teamId) { m_dungChageTeamId = teamId; }
	inline UInt32 GetDungChageTeamId() { return m_dungChageTeamId;}
	
	LostDungeonTaskMgr& GetLostDungeonTaskMgr() { return m_LostDungeonTaskMgr; }

	inline void	SetLastLostBattleSceneId(UInt32 sceneId) { m_lastLostBattleSceneId = sceneId; }
	inline UInt32 GetLastLostBattleSceneId() { return m_lastLostBattleSceneId; }

	Int32 GetLostDungeonHp() const;
	void SetLostDungeonHp(Int32 hp);
	void DecLostDungeonHp(Int32 value);
	void IncLostDungeonHp(Int32 value);
	Int32 GetLostDungeonMp() const;
	void SetLostDungeonMp(Int32 mp);
	void DecLostDungenMp(Int32 value);
	void IncLostDungeonMp(Int32 value);

	void SetLostDungenScore(UInt32 val) { m_LostDungeonScore = val; }
	UInt32 GetLostDungenScore() { return m_LostDungeonScore; }

	void OnEnterLostDungeon(bool reconnect);

	bool IsCompLostDungeonTask();

	Int32 GetLostDungeonHpBack() const;
	void SetLostDungeonHpBack(Int32 hp);
	Int32 GetLostDungeonMpBack() const;
	void SetLostDungeonMpBack(Int32 mp);

	void SetLostDungeonSpeedBack(UInt32 speed);
	UInt32 GetLostDungeonSpeedBack();

	inline void SetLostDungBatSt(UInt8 st) { m_lostDungBatSt = st; }
	inline UInt8 GetLostDungBatSt() { return m_lostDungBatSt; }

	inline void SetLostDungProtTime(UInt32 time) { m_lostProtTime = time; }
	inline UInt32 GetLostDungProtTime() { return m_lostProtTime; }

	//玩家查看情报
	UInt32 OnPlayerSeeIntell(UInt64 targetId, std::vector<ItemReward>& artifacts);

	inline bool GetLostDungLeaveTag() { return m_lostDungLeaving; }
	inline void SetLostDungLeaveTag(bool tag) { m_lostDungLeaving = tag; }

	void GetLostDunegonTaskKillTargetPos(std::vector<UInt64>& players, std::vector<LostDungeonPlayerPos>& poss);
	//最近在的战场场景id(动态的)
	UInt32	m_lastLostBattleSceneId;

	// 地牢剩余血量(千分比)
	ObjInt32					m_LostDungeonHp;
	// 地牢剩余蓝量(千分比)
	ObjInt32					m_LostDungeonMp;
	// 地牢积分
	ObjInt32					m_LostDungeonScore;

	// 进入战场时的地牢剩余血量(千分比)
	Int32					m_LostDungHpBack;
	// 进入战场时的地牢剩余蓝量(千分比)
	Int32					m_LostDungMpBack;
	// 进入战场时候的移动速度
	UInt32					m_LostDungSpeedBack;

	//战斗数据
	RacePlayerInfo	m_lostBattleRacePlayerInfo;
	//地下城关卡挑战模式
	UInt8 m_dungChageMode;
	//地下城关卡队伍id
	UInt32 m_dungChageTeamId;
	
	//地牢任务
	LostDungeonTaskMgr m_LostDungeonTaskMgr;
	//玩家战斗状态
	ObjInt8  m_lostDungBatSt;
	//玩家保护时间戳
	UInt32   m_lostProtTime;
	//正在离开战场
	bool     m_lostDungLeaving;
private:
	//网关连接  noted by aprilliu  收到SceneEnterGameReq时进行了设置
	Avalon::NetConnectionPtr m_Conn;
	//账号id
	UInt32		m_AccId;
	// 账号表的GUID
	ObjID_t		m_AccountGuid;
	//所在主城id（赛丽亚房间）
	UInt32		m_MajorCityId;

	//状态
	Int32		m_GameStatus;
	//修改状态的时间
	UInt32		m_StatusTime;

	//gm权限
	UInt8		m_GmAuthority;
	//客户端选项
	UInt8		m_ClientOption;

	//openid
	ObjString	m_OpenId;

	//逻辑定时事件
	UInt64		m_TickTime[LOGICTICK_MAX];

	//oppo vip等级
	ObjUInt8	m_OppoVipLevel;

	//城市
	std::string	m_City;
	//ip地址
	std::string	m_Ip;

	//职业
	ObjUInt8	m_Occu;

	//预转职职业
	ObjUInt8	m_PreOccu;

	//vip等级
	ObjUInt8	m_VipLvl;

	//vip经验
	ObjUInt32	m_VipExp;

	//总充值金额
	UInt32		m_TotalChargeNum;

	//vip到期时间
	ObjUInt32	m_VipDueTime;
	//vip体验buff
	Buff*		m_pVipBuff;

	//qq特权信息;
	QQVipInfo	m_qqVipInfo;

	//来源1
	std::string	m_Source1;
	//来源2
	std::string m_Source2;
	//邀请者
	UInt32		m_Inviter;
	//平台
	ObjString	m_Pf;
	//设备id
	std::string m_DeviceId;
	//设备型号
	std::string m_Model;
	//设备版本
	std::string m_DeviceVersion;

	//区id
	ObjUInt32	m_ZoneId;

	//经验
	ObjUInt64	m_Exp;
	//祝福经验
	ObjUInt32	m_BlessExp;

	//上次原地复活时间
	ObjUInt32	m_ReviveTime;
	//复活消耗
	ObjUInt32	m_ReviveCost;
	//连续复活次数
	UInt32		m_ReviveTimes;

	//玩家升级表数据项
	PlayerDataEntry*	m_pLevelDataEntry;

	//聊天计时器
	ChatTimer	m_ChatTimer;

	//加载项个数
	UInt8		m_LoadDataCount;
	//加载进度
	UInt64		m_LoadDataSerial;

	//惩罚标志
	UInt32		m_PunishFlag;
	//禁言到期时间
	ObjUInt32	m_ForbitTalkDueTime;

	//本区存档时间
	UInt64		m_LocalSavebackTime;
	//角色创建时间
	ObjInt32	m_CreateTime;
	//上线时间
	UInt64		m_OnlineTime;
	//日在线时间(s)
	ObjUInt32		m_DayOnlineTime;
	//总在线时长(s)
	UInt32		m_totOnlineTime;
	//恢复时间
	UInt64		m_RecoverTime;
	//上次繁忙时间
	UInt64		m_BusyTime;

	//副本入口场景
	UInt32		m_EnterScene;
	//副本入口位置
	CLPosition	m_EnterPos;
	UInt64      m_EnterSceneDyncId;

	//道具
	ItemMgr		m_ItemMgr;
	//商店
	ShopMgr		m_ShopMgr;
	//道具组cd
	ItemCD		m_ItemCD;
	//奖励组
	RewardGroup	m_RewardGroup;
	//套装效果
	ObjUInt32	m_SuitEffect;

	//金币
	ObjUInt32	m_Gold;
	//绑定金币
	ObjUInt32	m_BindGold;
	//点券
	ObjUInt32	m_Point;
	//绑定点券
	ObjUInt32	m_BindPoint;
	//累充记录
	ObjUInt32	m_TotleChargeNum;
	//日充值记录
	ObjUInt32   m_DayChargeNum;
	//荣誉
	ObjUInt32	m_Honour;
    //勇者之魂
    ObjUInt32	m_WarriorSoul;
	//决斗币	
	ObjUInt32	m_PkCoin;
	//金罐积分
	ObjUInt32	m_GoldJarPoint;
	//魔罐积分
	ObjUInt32	m_MagJarPoint;

	//包裹大小
	ObjUInt32	m_PackSize;

	//包裹容量加成
	PackageSize<PACK_MAX> m_PackSizeAddition;

	//仓库大小
	ObjUInt8	m_StorageSize;
	//财产锁定时间
	UInt8		m_AssetsLockTime;

	//防沉迷收益
	UInt32		m_WallowFactor;

	//任务完成掩码, noted by aprilliu, 放在TaskMgr类中更合适
	//huchenhui 2016.06.28 添加成就
	CLMaskProperty<3 * MAX_TYPE_TASKNUM> m_TaskMask;
	//任务
	TaskMgr			m_TaskMgr;
	//日常任务
	DailyTaskMgr	m_DailyTaskMgr;
	//成就
	AchievementTaskMgr	m_AchievementTaskMgr;
	//活动属性
	ActivityAttributeMgr m_ActivityAttributeMgr;
	//循环任务
	CycleTaskMgr	m_CycleTaskMgr;
	//红包任务
	RedPacketTaskMgr	m_RedPacketTaskMgr;
	//传奇任务
	LegendTaskMgr		m_LegendTaskMgr;
	//师门日常任务
	MasterDailyTaskMgr		m_MasterDailyTaskMgr;
	//师门学业成长任务	
	MasterAcademicGrowthTaskMgr m_MasterAcademicGrowthTaskMgr;
	//当前所在区域归属任务
	UInt32		m_ZoneTask;

	//剧情掩码
	CLMaskProperty<MAX_ONCE_STORY_NUM>	m_StoryMask;
	//当前剧情
	ObjUInt16	m_StoryId;

	//技能栏	by huchenhui
	SkillBar	m_SkillBar;
	//PVP技能栏	by huchenhui
	PvpSkillBar	m_PvpSkillBar;
	//物品栏	by huchenhui
	ItemBar		m_ItemBar;
	//游戏选项
	ObjInt32	m_Options;

	//经验加成BUFF
	Buff*		m_pExpAddBuff;

	//请求管理器
	RequestMgr	m_Requests;

	//交易
	Trade*		m_pTrade;

	//队伍
	Team*		m_pTeam;
	//是否队长
	ObjUInt8	m_isTeamMaster;
	//队伍人数
	ObjUInt8	m_TeamSize;
	//组队时的亲密度级别
	UInt16		m_TeamIntimacyBuff;

	//好友列表
	FriendMap	m_Friends;

	//关系列表
	std::map<ObjID_t, UInt16>	m_Relations[RELATION_MAX];

	//计数
	CounterMgr		m_CounterMgr;
	//行为计数
	ActionCounterMgr	m_ActionCounterMgr;

	//日常
	RoutineMgr	m_RoutineMgr;

	//行为标识
	ObjUInt32	m_ActionFlag;

	//好友邀请
	InviteMgr		m_InviteMgr;

	//月卡到期时间(s)
	ObjUInt32		m_MonthCardExpireTime;

	//更新版本号
	ObjUInt32		m_UpdateBulletin;

	//预告功能  added by aprilliu, 主线任务完成是检查是否需要变更，变更后走自动同步的流程给前端
	ObjUInt8		m_FuncNotify;
	//开能开启位  主线任务解锁，也是主线任务完成后进行检查 (Player::UpdateFuncNotify())
	CLMaskProperty<MAX_FUNCTION_OPEN_NUM>	m_FuncFlag;

	//创角后连续登陆天数
	ObjUInt32		m_ContinuousDays;
	
	//原区号
	ObjUInt16		m_SrcZoneId;
	//旅游区号
	ObjUInt16		m_TourZoneId;
	//出生区
	ObjUInt32		m_BornZoneId;

	// 随机装备商店
	EquipShopMgr	m_EquipShopMgr;

	//是否在跑
    bool            m_Runing;

    //真正的在场景中的坐标
    ObjScenePos     m_ScenePos;

    //运动方向（360度）
    ObjSceneDir     m_SceneDir;

    //上一次走动时间
    Avalon::Time    m_LastMoveTime;

	//SP noted by huchenhui
	ObjUInt32	    m_SP;

	//pvpSP noted by huchenhui
	ObjUInt32		m_PvpSP;

	//觉醒
	ObjUInt8	    m_Awaken;

    // 地下城管理器
    DungeonMgr      m_dungeonMgr;

    // 决斗场积分
    ObjUInt32       m_pkValue;

    // 匹配积分
    ObjUInt32       m_MatchScore;

	// 赛季段位
	SeasonLevel		m_SeasonLevel;

	//疲劳值
	Fatigue			m_Fatigue;

    // 战斗记录
    PkStatisticMgr  m_pkStatisticMgr;

    ObjUInt32       m_titleId;

	//随从管理器
	RetinueMgr		m_RetinueMgr;

	//宠物管理器
	PetMgr			m_PetMgr;

	// 外观
	ObjPlayerAvatar	m_avatar;

	// 快速购买
	QuickBuyTransaction*	m_quickBuyTrans;

	// 复活币
	ObjUInt32		m_reviveCoin;

	//VIP
	Vip				m_Vip;

	//引导
	CLMaskProperty<MAX_BOOT_FLAG_NUM>	m_BootFlag;

	// 城镇中关注的玩家列表，按优先级排序
	std::set<PlayerSightPriority>	m_aroundPlayerList;
	std::vector<ObjID_t>			m_watchRoleIdList;
	std::vector<ObjID_t>			m_watchRoleIdSet;

	// 城镇中关注的玩家列表，按优先级排序
	std::set<ObjID_t>				m_watchRoleList;

	// 上一次同步位置的时间(ms)
	UInt64							m_LastUpdatePosTime;
	// 上一次计算观察列表的时间(ms)
	UInt64							m_LastUpdateWatchListTime;

	//发言禁言上限次数
	UInt8	m_chatHiFrqCount;

	// 每日定时器
	Avalon::DailyTimer	m_DailyTimer;

	// 死亡之塔扫荡结束时间
	ObjUInt32		m_DeathTowerWipeoutEndTime;

	//开罐子管理器
	OpenJarMgr		m_JarMgr;

	// 公会相关
	// 公会ID
	ObjUInt64		m_GuildId;
	// 公会名
	ObjString		m_GuildName;
	// 职位
	ObjUInt8		m_GuildPost;
	//加入公会时间
	ObjUInt32		m_GuildEnterTime;
	// 贡献
	ObjUInt32		m_GuildContri;

	// 公会商店
	ObjUInt8		m_GuildShopId;

	// 公会建筑信息
	UInt8			m_GuildBuildingLevel[GUILD_BUILDING_NUM];

	// 工会占领的跨服领地
	UInt8			m_GuildOccupyCrossTerrId;

	// 红点
	ObjUInt32		m_RedPointFlag;

	//公会战次数
	ObjUInt32		m_GuildBattleNumber;

	//公会战积分
	ObjUInt32		m_GuildBattleScore;

	//公会战抽奖状态
	ObjUInt8		m_GuildBattleLotteryStatus;

	// 公会徽记
	ObjUInt32			m_GuildEmblem;

	//奖励领取情况
	CLMaskProperty<GUILD_BATTLE_REWARD_SIZE> m_GuildBattleRewardMask;

	//每日任务积分
	ObjUInt32		m_DailyTaskScore;

	//成就积分
	ObjUInt32		m_AchievementScore;

	//每日任务奖励领取情况
	CLMaskProperty<DAILY_TASK_REWARD_SIZE> m_DailyTaskRewardMask;

	//成就奖励领取情况
	CLMaskProperty<ACHIEVEMENT_TASK_REWARD_SIZE> m_AchievementTaskRewardMask;

	//武道大会状态
	ObjUInt8		m_WudaoStatus;

	// 武道大会上一个场景
	UInt32			m_WudaoPrevSceneId;
	// 武道大会上一个场景坐标
	CLPosition		m_WudaoPrevScenePos;
	// 上一个场景
	Scene*			m_lastScene;

	ObjUInt32		m_NewBoot;

    // 加载完成回调
    std::vector<PlayerLoadingCallback*>    m_LoadingCallback;

	// 用户自定义字段
	ObjUInt32		m_CustomField;

	//拍卖行刷新时间
	ObjUInt32		m_AuctionRefreshTime;

	//拍卖行额外购买的栏位
	ObjUInt32		m_AuctionAdditionBooth;

	// 异常交易管理器
	AbnormalTransactionMgr m_AbnormalTransactionMgr;

	bool m_bLogicDayChanged;

	UInt64 m_RefTime;		// 对该玩家的引用时间,比如查询离线玩家,离线充值发货,作用是延迟玩家缓存销毁

	//上一次重置金罐子积分时间
	ObjUInt32 m_LastResetGoldJarPointTime;
	//上一次重置魔罐积分时间
	ObjUInt32 m_LastResetMagJarPointTime;

	//上一次发送喇叭的时间
	UInt32		m_LastSendHornTime;
	//当前喇叭连发数量
	UInt16		m_HornCombo;

	//商城购买获得的道具
	ObjID_t		m_MallItemId;

	// 是否绑定了手机
	bool		m_IsPhoneBind;

	// 已阅通知
	//ReadNotifyProperty	m_readNotify;

	// 药水设置
	DungeonPotionSet<3>	m_potionSet;

	// 旧的玩家等级（用于触发条件判断）
	UInt16				m_OldLevel;

	// 是否加载完毕条件触发数据库
	UInt8 m_ConditionActivateDBLoadState;

	//装备碎掉后推送的装备与玩家绑定的等级集
	std::vector<PlayerLevel> m_EquipBindPlayerLevel;

	
	bool m_bFirstTimeItemTick;

	//是否已经检查SP
	ObjUInt8 m_IsCheckSp;

	//PVP技能管理
	PvpSkillMgr m_PvpSkillMgr;

	// 时装合成天空套有损库信息
	ObjUInt64 m_FashionSkyGetBit;

	//房间ID
	ObjUInt32 m_RoomId;

	//同步账号在线时间的定时器
	Avalon::SimpleTimer		m_SyncAddictTimer;

	//武器栏
	WeaponBar		m_WeaponBar;

	//预约角色
	ObjUInt8		m_AppointmentOccu;

	//跨服场景服务器ID
	UInt32			m_CrossSceneNodeId;
	//原场景服务器ID
	UInt32			m_SrcSceneNodeId;

	//战场场景服务器ID
	UInt32			m_BattleSceneNodeId;

	//是否通知切场景(client,gate)
	bool			m_NotifyChangeScene;

	//角色理财状态
	ObjUInt8		m_MoneyManageStatus;

	//账号理财状态
	UInt8			m_MoneyManageAccountStatus;

	//理财管理奖励
	CLMaskProperty<MONEY_MANAGE_REWARD_SIZE> m_MoneyManageRewardMask;

	//积分赛积分
	ObjUInt32		m_ScoreWarScore;
	//积分赛参与赛奖励
	CLMaskProperty<SCORE_WAR_REWARD_SIZE> m_ScoreWarRewardMask;
	//积分赛战斗次数
	ObjUInt8		m_ScoreWarBattleCount;
	//积分赛战斗胜利次数
	ObjUInt8		m_ScoreWarWinBattleCount;
	//积分赛最后战斗次数(用于刷新积分赛信息)
	ObjUInt32		m_ScoreWarLastBattleTime;

	//是否在loading中(客户端加载场景)
	bool			m_IsClientLoading;

	//师徒问卷：
	//在线时间类型
	UInt8			m_activeTimeType;
	//师傅类型
	UInt8			m_masterType;
	//地区id
	UInt8			m_regionId;
	//宣言
	std::string		m_declaration;
	
	//师徒成长值总值
	ObjUInt32		m_AcademicTotalGrowth;
	//师门日常任务成长总值
	ObjUInt32		m_MasterDailyTaskGrowth;
	//师门学业成长总值
	ObjUInt32		m_MasterAcademicTaskGrowth;
	//师门赠送装备成长总值
	ObjUInt32		m_MasterGiveEquipGrowth;
	//师门升级成长总值
	ObjUInt32		m_MasterUplevelGrowth;
	//师门赠送礼包成长总值
	ObjUInt32		m_MasterGiveGiftGrowth;
	//师徒组队通过地下城成长值
	ObjUInt32		m_MasterTeamClearDungeonGrowth;
	//良师值
	ObjUInt32		m_GoodTeacherValue;
	//出师徒弟数量
	ObjUInt32		m_FinSchDiscipleSum;

	//抗魔值
	UInt32			m_ResistMagic;

	//个人活动
	PlayerActivityInfo<Player> m_PlayerActivityInfo;

	// 安全锁状态
	UInt32		m_SecurityStatus;

	// 武器时装显示选择
	ObjUInt8    m_ShowFashionWeapon;

	// 武器租赁好运符
	ObjUInt32		m_weaponLeaseTickets;
	// 冒险队队名
	ObjString			m_AdventureTeamName;
	// 角色评分
	ObjUInt32			m_RoleValueScore;
	// 游戏设置
	ObjString	m_GameSet;
	// 游戏设置map
	std::map<UInt32, std::string> m_GameSetMap;

	// 角色回归时间
	UInt32				m_RoleReturnTime;
	// 角色回归等级
	UInt32				m_RoleReturnLevel;
	// 是否再次回归
	UInt8				m_AgainReturn;
	// 精力币
	UInt32				m_EnergyCoin;

	// 冒险队等级
	UInt16				m_AdventureTeamLevel;
	// 冒险队评级
	UInt8				m_AdventureTeamGrade;


	// 包裹类型大小
	ObjString			m_PackageTypeStr;
	// 包裹类型map
	std::map<UInt32, UInt32> m_PackageTypeSizeMap;

	// 头像框
	HeadFrameMgr		m_HeadFrameMgr;
	// 当前头像框
	ObjUInt32				m_HeadFrameId;

	// 头衔名字
	ObjString			m_NewTitleName;

	// 头衔GUID
	ObjUInt64				m_NewTitleGuid;
	// 吃鸡专有数据
	// 吃鸡剩余血量(千分比)
	ObjInt32					m_ChijiHp;
	// 吃鸡剩余蓝量(千分比)
	ObjInt32					m_ChijiMp;
	// 吃鸡积分
	ObjInt32					m_ChijiScore;
	// 吃鸡积分重置时间
	ObjInt64					m_ChijiScoreResetTime;
	// 吃鸡免战敌人id
	UInt64						m_ChijiNoWarEnemyId;
	// 吃鸡免战战场id
	UInt32						m_ChijiNoWarBattleId;
	// 吃鸡免战选择时间
	UInt32						m_ChijiNoWarChoiceTime;
	// 吃鸡pk地下城
	UInt32						m_ChijiPKDungeonId;
	// 吃鸡技能列表
	std::vector<UInt32>			m_ChijiOccuVec;

	SpMgr m_NetSp;

public:
	// 吃鸡战场ID
	UInt32					m_BattleID;
	Avalon::SimpleTimer		m_CheckBattleTimer;

#ifdef _DEBUG
	//强化概率测试
	UInt8	m_testEquipStrengthFlag;
	//合成天空套概率测试
	UInt8	m_testHappySky;
#endif
};

/**
 *@brief 玩家访问器
 */
typedef CLVisitor<Player> PlayerVisitor;

#endif

