#ifndef _GS_PLAYER_H_
#define _GS_PLAYER_H_

#include <CLSceneObject.h>
#include <AvalonProtocol.h>
#include <AvalonNetConnection.h>
#include <AvalonSimpleTimer.h>
#include <set>
#include <kingnet_analyzer.h>
#include <CLDefine.h>
#include <CLGameDefine.h>
#include <CLMaskProperty.h>

#include "PlayerPacketBuffer.h"

/**
 *@brief 网关玩家状态
 */
enum GatePlayerStatus
{
	GPS_VERIFY,			//验证阶段
	GPS_QUEUE,			//排队等待中
	GPS_WAITROLEINFO,	//等待角色信息
	GPS_CREATEROLE,		//创建角色过程中
	GPS_SELECTROLE,		//选择角色
	GPS_ENTERGAME,		//进入游戏阶段
	GPS_NORMAL,			//正常游戏阶段
    GPS_OFFLINE,        //离线阶段
	GPS_RECONNECT,		//断线重连
	GPS_REPLACE,		//顶号
	GPS_FINAL,			//终止阶段
};

/**
*@brief 进入游戏方式
*/
enum EnterGameMode
{
	EGM_NORMAL = 0,		//一般流程
	EGM_ROLE_SWITCH,	//角色切换
};

#define PLAYERINFO(T) "accid(" << T->GetAccID() << ")"

/**
 *@brief  角色信息
 */
struct GSRoleInfo
{
	GSRoleInfo()
		:id(0), mapid(0), sceneid(0), sex(0), occu(0), level(0), tourZoneId(0), bornZoneId(0), createtime(0), offlineTime(0), deleteTime(0), newBoot(0), preOccu(0), isAppointmentOccu(0), returnTime(0), returnLevel(0), addPreferencesTime(0),delPreferencesTime(0){}

	// 角色是否已经永久性删除
	bool IsForeverDeleted()
	{
		if (deleteTime == 0)
		{
			return false;
		}

		if (level < DIRECT_DELETE_PLAYER_LEVEL || deleteTime + DELETE_PLAYER_SAVE_TIME < CURRENT_TIME.Sec())
		{
			return true;
		}

		return false;
	}

	//唯一id
	ObjID_t		id;
	//角色名
	std::string	name;
	//地图id
	UInt32		mapid;
	//场景id
	UInt32		sceneid;
	//位置x
	CLPosition  pos;
	//性别
	UInt8		sex;
	//职业
	UInt8		occu;
	//等级
	UInt16		level;
	//旅游区
	UInt32		tourZoneId;
	//原区
	UInt32		bornZoneId;
	//创建时间
	UInt32		createtime;
	//离线时间
	UInt32		offlineTime;
	//删号时间
	UInt32		deleteTime;
	//外观
	PlayerAvatar avatar;
	//新手引导
	UInt32		newBoot;
	//预转职
	UInt8		preOccu;
	//是否是预约角色
	UInt8		isAppointmentOccu;
	//回归时间
	UInt32		returnTime;
	//回归等级
	UInt32		returnLevel;
	//玩家标签信息
	PlayerLabelInfo playerLabelInfo;
	//设置偏好时间
	UInt32 addPreferencesTime;
	//取消偏好时间
	UInt32 delPreferencesTime;
};

class GSScene;

/**
 *@brief 网关玩家  noted by aprilliu, GSPlayer的 object id 就是创建角色的id，一个guid
 */
class GSPlayer : public CLSceneObject
{
public:
	typedef std::list<GSRoleInfo>	RoleList; 
	typedef std::set<ObjID_t>		ObjIDSet;

public:
	GSPlayer();
	~GSPlayer();

	/**
	 *@brief 初始化
	 *@param accid 账号id
	 */
	bool Init(UInt32 accid, UInt8 gmauthority, const std::string& openid, 
		const QQVipInfo& info, const std::string& source1, const std::string& source2, const std::string& pf, const std::string& did,
		const std::string& model, const std::string& deviceVersion);

	/**
	*@brief 设置账号guid
	*/
	void SetAccountGuid(DBKey_t guid) { m_AccountGuid = guid; }
	DBKey_t GetAccountGuid() { return m_AccountGuid; }

	/**
	 *@brief 获取账号id
	 */
	UInt32 GetAccID() const { return m_Accid; }

	/**
	 *@brief 获取gm权限
	 */
	UInt8 GetGmAuthority() const { return m_GmAuthority; }
	void SetGmAuthority(UInt8 flag) { m_GmAuthority = flag; }

	/**
	*@brief 手机绑定的角色ID
	*/
	ObjID_t GetPhoneBindRoleID() const { return m_PhoneBindRoleId; }
	void SetPhoneBindRoleID(ObjID_t id) { m_PhoneBindRoleId = id; }

	/**
	 *@brief 设置角色删除时间
	 */
	void SetRoleDeleteTime(UInt32 time) { m_RoleDeleteTime = time; }

	/**
	*@brief 设置角色恢复时间
	*/
	void SetRoleRecoverTime(UInt32 time) { m_RoleRecoverTime = time; }

	/**
	*@brief 获取角色删除/恢复 限制消息
	*/
	std::string GetRoleUpdateLimitInfo() const { return m_RoleUpdateLimitInfo; }

	/**
	 *@brief 获取object类型
	 */
	SceneObjectType GetType() const{ return SOT_PLAYER; }

	/**
	 *@brief 设置源区id                                                                     
	 */
	void SetSrcZoneId(UInt32 srcZoneId){ m_SrcZoneId = srcZoneId; }
	UInt32 GetSrcZoneId() const{ return m_SrcZoneId; }

	/**
	 *@brief 设置获取等级
	 */
	void SetLevel(UInt16 level){ m_Level = level; }
	UInt16 GetLevel() const{ return m_Level; }

	/**
	 *@brief 获取openid                                                                     
	 */
	const std::string& GetOpenId() const{ return m_OpenId; }

	/**
	 *@brief 获取qq黄钻等级                                                                     
	 */
	const QQVipInfo& GetQQVip() const{ return m_qqVipInfo; }

	/**
	 *@brief 获取ip                                                                     
	 */
	const std::string& GetIp() const{ return m_Ip; }

	/**
	*@brief 能否进入游戏
	*/
	bool CanEnterGame() const { return m_CanEnterGame; }
	void SetCanEnterGame(bool can) { m_CanEnterGame = can; }

	/**
	 *@brief 获取来源                                                                     
	 */
	const std::string& GetSource1() const{ return m_Source1; }
	const std::string& GetSource2() const{ return m_Source2; }

	const std::string& GetPf() const{ return m_Pf; }

	const UInt8 GetStorageSize() const { return m_StorageSize; }
	void SetStorageSize(UInt8 sz) { m_StorageSize = sz; }

    inline void SetConn(Avalon::NetConnectionPtr conn) { m_Conn = conn; }
	inline Avalon::NetConnectionPtr GetConn() const { return m_Conn; }

	void SetLastRecvSequence(UInt32 sequence);
	UInt32 GetLastRecvSequence() const { return m_LastRecvSequence; }

	void ClearPacketBuffer() { m_packetBuffer.Clear(); }
	const PlayerPacketBuffer& GetPlayerPacketBuffer() const { return m_packetBuffer; }

	void SetMonitored(bool monitored);
	bool IsMonitored() const { return m_IsMonitored; }

	/**
	*@brief 冒险队
	*/
	void SetAdventureTeamInfo(const std::string& name, UInt16 level, UInt64 exp);
	const AdventureTeamInfo& GetAdventureTeamInfo() const { return m_AdventureTeamInfo; }

	/**
	*@brief 可扩展角色栏位解锁数量
	*/
	void SetUnlockedExtensibleRoleField(UInt32 num) { m_UnlockeExtensibleRoleField = num; }
	UInt32 GetUnlockedExtensiblRoleField() const { return m_UnlockeExtensibleRoleField; }

	/**
	*@brief 账号回归时间
	*/
	void SetAccountReturnTime(UInt32 time) { m_AccountReturnTime = time; }
	UInt32 GetAccountReturnTime() const { return m_AccountReturnTime; }

	/**
	*@brief 角色回归时间
	*/
	void SetRoleReturnTime(UInt32 time) { m_RoleReturnTime = time; }
	UInt32 GetRoleReturnTime() const { return m_RoleReturnTime; }

	/**
	*@brief 角色回归等级
	*/
	void SetRoleReturnLevel(UInt32 lvl) { m_RoleReturnLevel = lvl; }
	UInt32 GetRoleReturnLevel() const { return m_RoleReturnLevel; }

	/**
	*@brief 是否再次回归
	*/
	void SetAgainReturn(bool isAgain) { m_AgainReturn = isAgain ? 1:0; }
	UInt8 GetAgainReturn() const { return m_AgainReturn; }

	/**
	*@brief 是否在回归期间
	*/
	bool IsVeteranReturn();

	/**
	*@brief 角色列表
	*/
	const RoleList& GetRoleList() { return m_Roles; }

	/**
	*@brief 关闭监控记录的文件
	*/
	void CloseMonitorFile();

public:
	/**
	 *@brief 添加一个角色
	 */
	bool AddRole(const GSRoleInfo& info);

	/**
	 *@brief 删除一个角色                                                                     
	 */
	UInt32 DeleteRole(ObjID_t roleId);

	/**
	 *@brief 恢复一个角色                                                                     
	 */
	UInt32 RecoverRole(ObjID_t roleId);

	/**
	 *@brief 获取角色数
	 */
	UInt32 GetRoleNum() const;

	/**
	*@brief 获取已经删除的角色（不包括已经永久删除的）
	*/
	UInt32 GetDeletedRoleNum();

	/**
	*@brief 获取今日创建角色数量
	*/
	UInt32 GetTodayCreatedRoleNum();

	/**
	*@brief 能否创建新的角色
	*/
	UInt32 CanCreateNewRole();

	/**
	*@brief 检测角色栏位是否达到上限
	*/
	bool RoleFieldReachUpperLimit();
	/**
	 *@brief 设置偏好
	 */
	UInt32 SetPreferences(ObjID_t roleId, RolePreferences flag);
	/**
	 *@brief 获取偏好玩家数
	 */
	UInt32 GetPreferencesRoleNum();
public:
	void StartToQueryRoleInfo(UInt8 enterGameMode = 0);

	/**
	 *@brief 设置获取所在的场景
	 */
	void SetScene(GSScene* scene){ m_pScene = scene; }
	GSScene* GetScene() const { return m_pScene; }

	/**
	 *@brief 设置获取状态
	 */
	void SetStatus(GatePlayerStatus status);
	GatePlayerStatus GetStatus() const { return m_Status; }

	/**
	*@brief 查询账号信息
	*/
	void BeginQueryAccountInfo(UInt8 enterGameMode = 0);

	/**
	 *@brief 开始进入游戏
	 */
	bool BeginEnterGame(ObjID_t roleId, UInt8 option, const std::string& city, UInt32 inviter);
	bool BeginEnterGameForTour(UInt8 option, const std::string& city, UInt32 inviter);

	/**
	*@brief 完成新手引导关卡
	*/
	void OnFinishNewbeeGuide(ObjID_t roleId, UInt32 id);

	/**
	 *@brief 结束进入游戏
	 */
	void EndEnterGame();

	/**
	 *@brief 设置获取上次下线场景和位置
	 */
	void SetLastMapID(UInt32 mapId){ m_LastMapID = mapId; }
	UInt32 GetLastMap() const{ return m_LastMapID; }
	UInt32 GetLastScene() const{ return m_LastSceneID; }
	const CLPosition& GetLastPos() const{ return m_LastPos; }

	void SetSession(std::vector<UInt8> session) { m_Session = session; }
	std::vector<UInt8> GetSession() { return m_Session; }

	/**
	 *@brief 发送角色信息
	 */
	void SendRoleInfo();

	/**
	*@brief 发送角色信息到Center
	*/
	void SendRoleInfoToCenter();

	/**
	 *@brief 检查跨区                                                                     
	 */
	void CheckChangeZone(UInt32 dstZoneId, UInt32 dstMapId);

    /**
    *@brief 设置离线定时器
    */
    void SetOfflineTimer(UInt32 sec);

    /**
    *@brief 是否离线超时
    */
    bool IsOfflineTimeout(const Avalon::Time& now);

	/**
	*@brief 通知world server玩家离开游戏,由world 通知scene server 走下线流程逻辑
	*/
	void NotifyLeaveGame(ObjID_t roleid, UInt8 isOfflineTimeout = 0);

	/**
	*@brief 通知world server玩家离开游戏
	*/
	void NotifyDisconnect();

	/**
	*@brief 通知world server重连成功
	*/
	void NotifyReconnectSuccess();

	/**
	*@brief 同步冒险队信息
	*/
	void SyncAdventureTeamInfo();

	// 同步系统开关
	void NotifySysSwitch();

public://关系相关
	/**
	 *@brief 设置黑名单
	 */
	void SetBlackList(const std::vector<ObjID_t>& blacklist){ m_BlackList.insert(blacklist.begin(), blacklist.end()); }

	/**
	 *@brief 增加删除黑名单
	 */
	void AddBlackList(ObjID_t id){ m_BlackList.insert(id); }
	void RemoveBlackList(ObjID_t id){ m_BlackList.erase(id); }

	/**
	 *@brief 判断是否在黑名单中
	 */
	bool IsInBlackList(ObjID_t id) const{ return m_BlackList.find(id) != m_BlackList.end(); }

public:// 预约职业相关
	/**
	*@brief 检查角色是否还处于预约状态下
	*/
	void CheckRoleAlreadyInAppointment(GSRoleInfo& info);

	/**
	*@brief 获取预约角色数量
	*/
	UInt32 GetAppointmentRoleNum();

public:// 回归活动

	/**
	*@brief 更新回归时间
	*/
	void UpdateRoleReturnInfo(UInt64 roleId, UInt32 time);
	void UpdateAccountReturnTime();

public:// 冒险队(佣兵团)

	/**
	*@brief 是否需要弹出新手引导选择
	*/
	bool NeedPopupNoviceGuideChoose();

	/**
	*@brief 设置新手引导选择标志
	*/
	void SetNoviceGuideChooseFlag(ObjID_t roleid, UInt8 chooseFlag);

	/**
	*@brief 通知重新计算角色总评分
	*/
	void NotifyRecalculateTotalRoleValueScore(const std::string& reason);

public://事件
	
	void OnTick(const Avalon::Time& now);

	void NotifyNodeRoleConnect(bool isLeave = true);
	
public: //日志

	/**
	*@brief 发送udplog
	*/
	template <typename... Args>
	void SendUdpLog(const char* typeInfo, const char* format, const Args & ... args)
	{
		if (typeInfo == NULL || format == NULL) return;
		CUserLogger	*pLogger = _analyzer.GetInstance(UDPLOG_GATE_UID);
		if (NULL == pLogger) return;

		char userLogBuffer[MAX_USERMSG_LENGTH];
		memset(userLogBuffer, 0, MAX_USERMSG_LENGTH);
		UInt32 nTime = (UInt32)Avalon::Time::CurrentTime().Sec();
		UInt16 zoneId = (GetSrcZoneId() != 0 ? GetSrcZoneId() : ZONE_ID);
		UInt32 bronZoneId = m_BornZoneId;
		UInt32 isReturn = IsVeteranReturn() ? 1 : 0;

#ifndef AVALON_OS_WINDOWS
		snprintf(userLogBuffer, sizeof(userLogBuffer), LOG_USERMSG_FORMAT, GAME_VERSION.c_str(), m_OpenId.c_str(), zoneId, m_Pf.c_str(), m_DeviceId.c_str(), m_Model.c_str(), m_DeviceVersion.c_str(), m_Ip.c_str(), GetID(), GetAccID(), (UInt32)GetLevel(), 0, (UInt32)m_Occu, 0, 0, nTime, bronZoneId, isReturn, 0, 0, 0);
#else
		_snprintf_s(userLogBuffer, sizeof(userLogBuffer), LOG_USERMSG_FORMAT, GAME_VERSION.c_str(), m_OpenId.c_str(), zoneId, m_Pf.c_str(), m_DeviceId.c_str(), m_Model.c_str(), m_DeviceVersion.c_str(), m_Ip.c_str(), GetID(), GetAccID(), (UInt32)GetLevel(), 0, (UInt32)m_Occu, 0, 0, nTime, bronZoneId, isReturn, 0, 0, 0);
#endif

		pLogger->SetUserIP(m_Ip.c_str());
		pLogger->SetUserMsg(userLogBuffer);
		pLogger->LogMsg(typeInfo, format, args...);

	}

	void SendDeleteUdpLog(UInt32 deleteTime);

	void SendCreateUdpLog(std::string name, UInt8 occu, ObjID_t roleId, UInt8 preOccu, UInt8 isAppointMentOccu);

	void SendAccountLoginUdpLog();

	void SendRolePreferencesUdpLog(UInt32 isPreferences, UInt32 roleLocation);
public://网络

	/**
	 *@brief 发送一个协议
	 */
	void SendProtocol(Avalon::Protocol& protocol);

	/**
	 *@brief 发送一个包
	 */
	void SendPacket(Avalon::Packet* packet);

	/**
	 *@brief 发送到所在场景
	 */
	void SendToSceneServer(Avalon::Protocol& protocol);

	/**
	 *@brief 转发到场景
	 */
	void TransToSceneServer(Avalon::Protocol& protocol);

	/**
	 *@brief 转发到world
	 */
	void TransToWorldServer(Avalon::Protocol& protocol);

	/**
	*@brief 转发到globalActivity
	*/
	void SendToGlobalActivity(Avalon::Protocol& protocol);
	void SendToGlobalActivity(Avalon::Packet* packet);

	/**
	*@brief 转发到Battle
	*/
	void SendToBattle(Avalon::Protocol& protocol);
	void SendToBattle(Avalon::Packet* packet);

	/**
	*@brief 转发到TeamCopy
	*/
	void SendToTeamCopy(Avalon::Protocol& protocol);
	void SendToTeamCopy(Avalon::Packet* packet);

	/**
	*@brief 转发到Union
	*/
	void SendToUnion(Avalon::Protocol& protocol);
	void SendToUnion(Avalon::Packet* packet);

	

	/**
	 *@brief 断开连接
	 */
	void Disconnect();

	/**
	*@brief 重连
	*/
	UInt32 BeginReconnect(Avalon::NetConnectionPtr conn, std::vector<UInt8> session, UInt32 sequence);

	/**
	*@brief 把消息记录到文件
	*/
	void RecordRecord(Avalon::Packet* packet, bool isRecv);

private:
	void EnterGame();
	UInt32 GenPacketSequence() { return ++m_packetSequenceSeed; }

	/**
	*@brief 获取当前存在的角色中最高等级
	*/
	UInt32 GetRoleMaxLevel();

	/**
	*@brief 判断是否限制角色删除或恢复
	*/
	bool IsLimitRoleUpdate(UInt32 updateType, UInt32 intervalTime, UInt32 roleUpdateLimitTime);

	/**
	*@brief 将时间转换为字符串
	*/
	std::string TimeToStr(UInt32 secs);

	/**
	*@brief 输出消息统计到文件
	*/
	void RecordPacketCountToFile();

	/**
	*@brief 统计消息包
	*/
	void PacketCount(UInt32 packetId, UInt32 packetSize);

private:
	//账号guid
	DBKey_t						m_AccountGuid;
	//账号id
	UInt32						m_Accid;
	//gm权限
	UInt8						m_GmAuthority;
	//城市
	std::string					m_City;
	//ip地址
	std::string					m_Ip;
	//openid
	std::string					m_OpenId;
	//qq特权信息
	QQVipInfo					m_qqVipInfo;
	//来源1
	std::string					m_Source1;
	//来源2
	std::string					m_Source2;
	//邀请者
	UInt32						m_Inviter;
	//平台
	std::string					m_Pf;
	//设备ID
	std::string					m_DeviceId;
	//设备型号
	std::string					m_Model;
	//设备版本
	std::string					m_DeviceVersion;

	//原区id
	UInt32						m_SrcZoneId;
	//旅游区id
	UInt32						m_TourZoneId;
	//原区id
	UInt32						m_BornZoneId;
	//等级
	UInt16						m_Level;
	//职业
	Occupation					m_Occu;

	//连接
	Avalon::NetConnectionPtr	m_Conn;
	//状态
	GatePlayerStatus			m_Status;
	//角色列表
	RoleList					m_Roles;
	//客户端选项
	UInt8						m_ClientOption;
	//上次下线地图
	UInt32						m_LastMapID;
	//上次下线场景
	UInt32						m_LastSceneID;
	//上次下限位置
	CLPosition					m_LastPos;
	//进入游戏尝试次数
	UInt8						m_TryTimes;
	//秒定时器
	Avalon::SimpleTimer			m_SecTimer;

    //离线定时器
    Avalon::SimpleTimer			m_offlineTimer;

	//10秒定时器
	Avalon::SimpleTimer			m_TenSecTimer;

	//分钟定时器
	Avalon::SimpleTimer			m_MinSecTimer;

	//所在场景
	GSScene*					m_pScene;

	//黑名单
	ObjIDSet					m_BlackList;

	//仓库大小
	UInt8						m_StorageSize;

	//消息序号种子
	UInt32						m_packetSequenceSeed;

	//消息Buffer
	PlayerPacketBuffer			m_packetBuffer;

	// 消息计数
	HashMap<UInt32, PacketStatistics>		m_packetCountMap;

	//Session
	std::vector<UInt8>			m_Session;

	//最后一个收到的消息序号
	UInt32						m_LastRecvSequence;

	//是否被监控
	bool						m_IsMonitored;

	//记录玩家消息的基本信息的文件
	FILE*						m_NetSimpleFile;
	//记录玩家具体消息的文件
	FILE*						m_NetDetailFile;
	//记录玩家的消息统计
	FILE*						m_PacketCountFile;

	//角色删除时间
	UInt32						m_RoleDeleteTime;

	//角色恢复时间
	UInt32						m_RoleRecoverTime;

	//角色删除/恢复 时限制消息
	std::string					m_RoleUpdateLimitInfo;
	
	//手机绑定的角色ID
	ObjID_t						m_PhoneBindRoleId;

	//能否进入游戏
	bool						m_CanEnterGame;

	//登录推送信息
	std::vector<LoginPushInfo>	m_LoginPushInfos;

	//冒险队信息
	AdventureTeamInfo			m_AdventureTeamInfo;

	//可扩展角色栏位解锁数量
	UInt32						m_UnlockeExtensibleRoleField;

	// 账号回归时间
	UInt32						m_AccountReturnTime;

	// 角色回归时间
	UInt32						m_RoleReturnTime;

	// 角色回归等级
	UInt32						m_RoleReturnLevel;

	// 是否再次回归
	UInt8						m_AgainReturn;

};

/**
 *@brief 访问器
 */
typedef CLVisitor<GSPlayer> GSPlayerVisitor;

#endif
