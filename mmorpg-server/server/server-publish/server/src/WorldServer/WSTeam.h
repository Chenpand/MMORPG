#ifndef _WS_TEAM_H_
#define _WS_TEAM_H_

#include <AvalonSimpleTimer.h>
#include <CLDefine.h>
#include <CLObject.h>
#include <CLTeamProtocol.h>
#include <vector>
#include <CLGameDefine.h>
#include <CLObjectDefine.h>

class WSPlayer;

/**
 *@brief 队伍成员
 */
struct WSTeamMember
{
	WSTeamMember() :player(NULL), enterTime(0), id(0), level(0), occu(0), intimacyLevel(0), 
		remainTimes(0), isAssist(false), isAssistByPlayer(false), isRacing(false),
		guildId(0), vipLevel(0), resistMagic(0){}

    UInt8 GetStatusMask() 
    { 
        UInt8 mask = 0;
        if (player)
        {
            mask |= TEAM_MEMBER_MASK_ONLINE;
        }

		if (isAssist)
		{
			mask |= TEAM_MEMBER_MASK_ASSIST;
		}

		if (isRacing)
		{
			mask |= TEAM_MEMBER_MASK_RACING;
		}

        return mask;
    }
    
	void SetPlayer(WSPlayer* player);

	TeammemberBaseInfo GetBaseInfo()
	{
		TeammemberBaseInfo info;
		info.id = id;
		info.name = name;
		info.level = level;
		info.occu = occu;
		info.playerLabelInfo = playerLabelInfo;

		return info;
	}

	TeammemberInfo GetInfo()
	{
		TeammemberInfo info;
		info.id = id;
		info.name = name;
		info.level = level;
		info.occu = occu;
		info.statusMask = GetStatusMask();
		info.avatar = avatar;
		info.remainTimes = remainTimes;
		info.guildId = guildId;
		info.vipLevel = vipLevel;
		info.resistMagic = resistMagic;
		info.playerLabelInfo = playerLabelInfo;

		return info;
	}

	//玩家
	WSPlayer* player;
	//进入队伍时间
	UInt32	enterTime;
	//玩家id
	ObjID_t	id;
	//玩家名
	std::string	name;
	//等级
	UInt16	level;
	//职业
	UInt8	occu;
	//组队时亲密度加成级别
	UInt8	intimacyLevel;
	//外观
	PlayerAvatar avatar;
	//地下城剩余次数
	UInt32	remainTimes;
	// 是否助战
	bool	isAssist;
	// 玩家是否手动设置了助战状态
	bool	isAssistByPlayer;
	// 是否在战斗中
	bool	isRacing;
	// 公会ID
	ObjID_t	guildId;
	// vip等级
	UInt8	vipLevel;
	// 抗魔值
	UInt32  resistMagic;
	//玩家标签信息
	PlayerLabelInfo playerLabelInfo;
};

typedef std::vector<WSTeamMember> MemberVec;
typedef CLVisitor<WSTeamMember> WSTeamMemberVisitor;

// 组队开始地下城投票
struct TeamDungeonStartVote
{
	TeamDungeonStartVote()
	{
		Clear();
	}

	void Clear()
	{
		dungeonId = 0;
		agreePlayerIds.clear();
		startTime = 0;
	}

	bool IsTimeout()
	{
		return dungeonId > 0 && startTime + 20 < (UInt32)CURRENT_TIME.Sec();
	}

	bool HasPlayer(ObjID_t id)
	{
		return std::find(agreePlayerIds.begin(), agreePlayerIds.end(), id) != agreePlayerIds.end();
	}

	// 地下城ID
	UInt32					dungeonId;
	// 所有同意了的玩家
	std::vector<ObjID_t>	agreePlayerIds;
	// 开始时间
	UInt32					startTime;
};

enum TeamStatus
{
	// 空闲
	TEAM_STATUS_IDEL,
	// 比赛中
	TEAM_STATUS_RACING,
};

struct TeamInviteInfo
{
	TeamInviteInfo()
	{
		roleId = 0;
		endTime = 0;
	}

	ObjID_t	roleId;
	// 邀请截止时间
	UInt32	endTime;
};

/**
 *@brief 队伍
 */
class WSTeam
{
	typedef std::map<UInt32, WSPlayer*>	ScenePlayerMap;

public:
	//无效的队伍ID
	const static UInt32 INVALID_TEAM_ID = 0;
	//最大人数
	const static UInt32 MAX_TEAM_SIZE = 4;
	//无人解散等待时间
	const static UInt32 EMPTY_DISMISS_WAITTIME = 0;
    //名字最大长度
    const static UInt32 MAX_TEAM_NAME_LENGTH = 14;
    //密码最大长度
    const static UInt32 MAX_TEAM_PASSWD_LENGTH = 4;
	//队伍邀请有效时间
	const static UInt32 TEAM_INVITE_VALID_TIME = 60;

public:
	WSTeam(WSPlayer* master);
	WSTeam(WSPlayer* master, WSPlayer* member);
	~WSTeam();

	void OnTick(const Avalon::Time& now);

public:
	/**
	 *@brief 设置获取id
	 */
	void SetId(UInt32 id){ m_Id = id; }
	UInt32 GetId() const{ return m_Id; }

	/**
	*@brief 队伍编号
	*/
	void SetNumber(UInt16 number) { m_number = number; }
	UInt16 GetNumber() const { return m_number; }

	void SetStatus(TeamStatus status);
	TeamStatus GetStatus() const;

    /**
    *@brief 队伍目标
    */
    void SetTeamTarget(UInt32 target) { m_target = target; }
	UInt32 ChangeTeamTarget(UInt32 target);
    UInt32 GetTeamTarget() const { return m_target; }
	void SetTargetDungeonID(UInt32 id) { m_TargetDungeonId = id; }
	UInt32 GetTargetDungeonID() const { return m_TargetDungeonId; }

	/**
	*@brief 是否被匹配到
	*/
	void SetIsMatched(bool isMatched) { m_IsMatched = isMatched; }
	bool GetIsMatched() const { return m_IsMatched; }

	/** 
	 *@brief 队长
	 */
	void SetMaster(ObjID_t master){ m_Master = master; }
	ObjID_t GetMaster() const{ return m_Master; }

	/**
	*@brief 自动同意
	*/
	void SetAutoAgree(bool agree) { m_AutoAgree = agree; }
	bool IsAutoAgree() const { return m_AutoAgree; }

	/**
	*@brief 队伍标记
	*/
	void SetOption(TeamOption option) { m_OptionMask |= option; }
	void ClearOption(TeamOption option) { m_OptionMask &= (~option); }
	bool IsSetOption(UInt32 option) const { return (m_OptionMask & option) > 0; }

	/**
	 *@brief 判断队伍是否已满
	 */
	bool IsFull() const{ return GetSize() >= GetMaxTeamMemberNum(); }

	/**
	 *@brief 判断队伍是否投票中
	 */
	bool IsInVote() { return m_Vote.dungeonId > 0; }

	/**
	 *@brief 添加移除成员
	 */
	void AddMember(WSPlayer* player, bool sendNotify, bool cancelMatch = true);
	void RemoveMember(ObjID_t playerId);

	void VisitMembers(WSTeamMemberVisitor& visitor);
	std::vector<WSPlayer*> GetAllMembers();

    /**
    *@brief 获取队伍基础信息, 如果参数为0，代表队长
    */
    TeamBaseInfo GetBaseInfo(UInt64 fromId = 0);

	/**
	*@brief 修改外观信息
	*/
	void UpdateAvatar(ObjID_t id, const PlayerAvatar& avatar);

	/**
	*@brief 修改助战状态
	*/
	void ChangeMemberAssistStatus(ObjID_t id, bool isAssist, bool modifyByPlayer = false);

	/**
	*@brief 修改公会ID
	*/
	void ChangeMemberGuildID(ObjID_t id, ObjID_t guildId);

	/**
	*@brief 修改玩家地下城剩余次数
	*/
	void OnChangeRemainTimes(ObjID_t id, UInt32 targetId, UInt32 remainTimes);
	void SetMemberRemainTimes(ObjID_t id, UInt32 remainTimes);

	/**
	*@brief 修改玩家正在战斗中的状态
	*/
	void OnChangePlayerRacingStatus(ObjID_t id, bool racing);

	/**
	*@brief 队伍目标是否有次数限制
	*/
	bool IsTargetTimesLimit();

	/**
	*@brief 获取队伍最低等级
	*/
	UInt16 GetMinLevelLimit();

	/**
	*@brief 更新抗魔值
	*/
	void OnPlayerResistMageicChanged(WSPlayer* player);
public:
	/**
	*@brief 请求加入队伍
	*/
	UInt32 RequestJoinTeam(WSPlayer* player, const std::string& passwd);

	/**
	 *@brief 解散队伍
	 */
	bool Dismiss(WSPlayer* player);
	void Dismiss();

	/**
	 *@brief 获取解散时间
	 */
	UInt32 GetDismissTime() const{ return m_DismissTime; }

	/**
	 *@brief 获取人数
	 */
	UInt32 GetSize() const{ return UInt32(m_Members.size()); }

	/**
	 *@brief 转移队长
	 */
	void TransferMaster(WSPlayer* player, ObjID_t id);

	/**
	 *@brief 查找成员
	 */
	WSTeamMember* FindMember(ObjID_t id);

    /**
    *@brief 玩家修改队伍选项
    */
    void OnPlayerChangeTeamOption(WSPlayer* player, TeamOptionOperType type, std::string str, UInt32 param1 = 0, UInt32 param2 = 0);

	/**
	*@brief 玩家修改队伍助战选项
	*/
	void OnPlayerChangeAssist(WSPlayer* player, bool isAssist);

public: // 地下城投票
	/**
	*@brief 开始地下城投票
	*/
	UInt32 OnStartRaceVote(UInt32 dungeonId);

	/**
	*@brief 玩家上传开始地下城投票
	*/
	void OnPlayerReportVote(WSPlayer* player, bool agree);

	/**
	*@brief 所有玩家都同意
	*/
	void OnStartRaceVoteAllAgree();

	/**
	*@brief 有玩家拒绝
	*/
	void OnStartRaceVoteFailure(ObjID_t roleId = 0, UInt32 noticeId = 0);

	/**
	*@brief 向scene发送开始比赛请求
	*/
	void SendStartRaceToScene(UInt32 dungeonId, SceneNpc cityMonster);

public:
	void UpdateSceneInfo();
	void SyncTeamInfo();
	void SyncTeamBaseToScene();

public: // 请求列表相关
	/**
	*@brief 添加申请者
	*/
	void AddRequester(WSPlayer* player);

	/**
	*@brief 获取申请者
	*/
	TeamRequester* FindRequester(ObjID_t id);

	/**
	*@brief 删除申请者
	*/
	void DelRequester(ObjID_t id);

	/**
	*@brief 清空申请列表
	*/
	void ClearRequesterList(WSPlayer* player);

	/**
	*@brief 回复加入请求
	*/
	void ReplyJoinRequest(WSPlayer* player, ObjID_t targetId, bool agree);

	/**
	*@brief 获取请求者列表
	*/
	const std::vector<TeamRequester>& GetRequesterList() const { return m_Requesters; }

	/**
	*@brief 获取队伍成员列表
	*/
	const MemberVec& GetMemberVec() { return m_Members; }

public: // 队伍邀请相关
	/**
	*@brief 添加到邀请列表
	*/
	void AddInvitePlayer(WSPlayer* player);

	/**
	*@brief 从邀请列表中删除
	*/
	void DelInvitePlayer(ObjID_t id);

	/**
	*@brief 玩家是否在邀请列表中
	*/
	bool IsInInviteList(ObjID_t id);

	/**
	*@brief 清空邀请列表
	*/
	void ClearInviteList(bool notify = true);

	/**
	*@brief 同步队伍属性给被邀请者
	*/
	void SyncTeamAttrToInvitePlayer();

	/**
	*@brief 广播到所有邀请玩家
	*/
	void BroadcastToInvitePlayer(Avalon::Protocol& protocol);

public: //消息相关
	/**
	 *@brief 广播到成员所在场景
	 */
	void BroadcastToScene(Avalon::Protocol& protocol);

	/** 
	 *@brief 广播给所有成员
	 */
	void Broadcast(Avalon::Protocol& protocol);

	/**
	*@brief 广播给除了队长之外的成员
	*/
	void BroadcastExceptMaster(Avalon::Protocol& protocol);

	/**
	 *@brief 广播公告给所有成员
	 */
	void Notify(UInt16 type, UInt8 color, const char* word);

	/**
	*@brief 广播成员状态
	*/
	void BroadcastMemberStatus(ObjID_t id);

public: //事件
	/**
	 *@brief 玩家上下线
	 */
	bool OnPlayerOnline(WSPlayer* player);
	void OnPlayerOffline(WSPlayer* player);

	/**
	 *@brief 场景服务器切换
	 */
	void OnSceneServerChanged(WSPlayer* player);

	/**
	 *@brief 等级改变
	 */
	void OnPlayerLevelChanged(WSPlayer* player);

	/**
	*@brief 职业改变
	*/
	void OnPlayerOccuChanged(WSPlayer* player);

	/**
	*@brief vip等级改变
	*/
	void OnPlayerVipLevelChanged(WSPlayer* player);

	/**
	 *@brief 解散
	 */
	void OnDismissed();

	/**
	*@brief 新的一天
	*/
	void OnDayChanged();

	/**
	*@brief 获取队伍最大人数
	*/
	UInt8 GetMaxTeamMemberNum() const;
	void SetMaxTeamMemberNum(UInt8 num) { m_maxMemberNum = num; }

	/**
	*@brief 获取剩余次数
	*/
	void QueryRemainTimes(ObjID_t roleId, UInt32 targetId);
	void QueryAllMemberRemainTimes(UInt32 targetId);

	/**
	*@brief 通知所以队员解散倒计时状态
	*/
	void BroadcastTeamKick(bool status);

	/**
	*@brief 控制解散开关
	*/
	void ControlKickMaster(bool bo);

private:
	void FillTeamInfo(CLProtocol::WorldSyncTeamInfo& sync);
	void FillSceneTeamInfo(CLProtocol::WorldSyncTeamInfoToScene& sync);
	void JudgeMaster();
	void ResetKickMasterTimer();
	
private:
	//队伍id
	UInt32	m_Id;
	//队伍编号（用于客户端显示）
	UInt16	m_number;
	//队伍状态
	TeamStatus m_Status;
	//队长
	ObjID_t	m_Master;
    // 队伍目标
    UInt32	m_target;
	// 最大人数
	UInt32	m_maxMemberNum;
	//解散时间
	UInt32	m_DismissTime;
	//队伍成员列表
	MemberVec	m_Members;
	//场景连接列表
	ScenePlayerMap	m_ScenePlayers;
	//申请列表
	std::vector<TeamRequester>	m_Requesters;
	//是否自动同意
	bool		m_AutoAgree;
	//队伍选项(对应TeamOption的位运算)
	UInt32						m_OptionMask;
	//开始比赛投票
	TeamDungeonStartVote	m_Vote;
	//邀请列表
	std::vector<TeamInviteInfo> m_InviteList;
	//目标地下城
	UInt32						m_TargetDungeonId;
	//是否已经被匹配到
	bool                        m_IsMatched;
	//队伍解散转移队长定时器
	Avalon::SimpleTimer         m_KickMasterTimer;
	//队伍解散转移队长控制
	bool						m_KickMasterControl;
	//队伍解散广播定时器
	Avalon::SimpleTimer         m_KickMasterBroadcastTimer;
};

typedef CLVisitor<WSTeam> WSTeamVisitor;

#endif
