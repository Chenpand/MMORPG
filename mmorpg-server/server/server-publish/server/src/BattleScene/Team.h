#ifndef _TEAM_H_
#define _TEAM_H_

#include <CLObject.h>
#include <CLChatDefine.h>
#include <CLSysNotify.h>
#include <vector>
#include <AvalonSocket.h>
#include <CLDungeonDefine.h>
#include <CLTeamDefine.h>
#include <CLObjectDefine.h>

class Player;
class TeamMemberVisitor
{
public:
	virtual ~TeamMemberVisitor(){}

	virtual bool operator()(Player* player) = 0;
};

/**
 *@brief 队伍
 */
class Team
{
	typedef std::vector<SceneTeamMember> MemberVec;

	//最大人数
	const static UInt32 MAX_TEAM_SIZE = 5;

public:
	Team(UInt32 id);
	~Team();

public:
    /**
    *@brief 组队开始地下城
    */
	bool OnPlayerRequestStartDungeonRace(Player* player, UInt32 dungeonId, const SceneNpc& cityMonster);

	/**
	*@brief world请求开始比赛返回
	*/
	bool OnWorldServerStartDungeonRes(UInt32 dungeonId, const SceneNpc& cityMonster);

    /**
    *@brief RelayServer创建比赛返回
    */
    bool OnRelayServerCreateRaceRet(UInt32 relayServerID, UInt32 dungeonId, UInt64 session, Avalon::SockAddr addr, bool isHell, const SceneNpc& cityMonster);

    /**
    *@brief 结算
    */
    bool OnDungeonRaceEnd(const DungeonRaceEndInfo& info);

    /**
    *@brief 结算
    */
    void OnPlayerLeaveDungeonRace(ObjID_t roleId);

	/**
	*@brief 检测队伍能否开始比赛
	*/
	bool CanStartDungeonRace(UInt32 dungeonId, bool checkTicket, bool sendNotice, bool checkLoading, const SceneNpc& cityMonster);

private:
    std::vector<Player*> _GetMemberPlayerInfo();

public:
	/**
	 *@brief 获取id
	 */
	UInt32 GetId() const{ return m_Id; }

    /**
    *@brief 比赛id
    */
    void SetRaceID(ObjID_t id) { m_raceId = id; }
    ObjID_t GetRaceID() const { return m_raceId; }

    /**
    *@brief RelayServer id
    */
    void SetRelayServerID(UInt32 id) { m_relayServerId = id; }
    UInt32 GetRelayServerID() const { return m_relayServerId; }

    /**
    *@brief session
    */
    void SetSession(ObjID_t session) { m_session = session; }
    ObjID_t GetSession() const { return m_session; }

	/**
	 *@brief 设置获取队长
	 */
	void SetMaster(ObjID_t master){ m_Master = master; }
	ObjID_t GetMaster() const{ return m_Master; }

	/**
	 *@brief 获取大小
	 */
	UInt8 GetSize() const{ return UInt8(m_Members.size()); }

	/**
	 *@brief 设置成员列表
	 */
	void SetMembers(const MemberVec& members);

	/**
	 *@brief 添加移除成员
	 */
	void AddMember(ObjID_t member);
	void RemoveMember(ObjID_t member);
	bool IsMember(ObjID_t member) const;
	SceneTeamMember* FindMember(ObjID_t id);

	/**
	 *@brief 设置获取上次获得道具的队友
	 */
	void SetLastItemOwner(ObjID_t owner){ m_LastItemOwner = owner; }
	ObjID_t GetLastItemOwner() const{ return m_LastItemOwner; }

	/**
	 *@brief 遍历范围内队友
	 */
	void VisitMembersAround(UInt32 sceneId, const CLPosition& pos, TeamMemberVisitor& visitor);
	void VisitSceneMembers(UInt32 sceneId, TeamMemberVisitor& visitor);

	/**
	 *@brief 获取本场景的成员数
	 */
	UInt32 GetMemberNumInScene();

	/** 
	 *@brief 获取周围同屏人数
	 */
	UInt32 GetMemberNumAround(UInt32 sceneId, UInt16 x, UInt16 y);

	/**
	 *@brief 检查队伍成员等级
	 */
	ObjID_t	CheckMembersLevel(UInt16 minLevel);

	/**
	 *@brief 获取最大等级
	 */
	UInt16 GetMembersMaxLevel();

	/**
	 *@brief 获取队伍平均等级
	 */
	UInt16 GetMembersAverageLevel();

	/**
	 *@brief 检查背包格子数
	 */
	ObjID_t	CheckPackGridNum(UInt32 num);

	/**
	 *@brief 检查队伍成员能否接取某个任务
	 */
	ObjID_t CheckTaskAcceptable(UInt32 id);

	/**
	 *@brief 接收任务
	 */
	void	AcceptTask(UInt32 id, UInt8 quality);

	/**
	 *@brief 提交任务
	 */
	void	SubmitTask(UInt32 id, UInt32 sceneId, UInt16 x, UInt16 y);

	/**
	 *@brief 更新信息
	 */
	void UpdateTeamInfo();

    /**
    *@brief 更新队员状态
    */
    void UpdateTeamMemberStatus(ObjID_t id, UInt8 status);

    /**
    *@brief 是否所有成员都准备好了
    */
    bool IsAllMemberReady();

	/**
	 *@brief 发送队伍通知
	 */
	//by huchenhui
	//void BroadcastNotify(UInt32 id, ... );

	template <typename... Args>
	void BroadcastNotify(UInt32 id, const Args & ... args)
	{
		UInt16 type = 0;
		UInt8 color = 0;
		char buffer[CL_MAX_CHAT_LEN + 1];
		memset(buffer, 0, sizeof(buffer));
		bool ret = SysNotifyMgr::Instance()->MakeNotify(id, type, color, buffer, CL_MAX_CHAT_LEN, args...);
		if (!ret) return;

		Notify(type, color, buffer);
	}

	void Notify(UInt16 type, UInt8 color, const char* word);

    /**
    *@brief 广播协议
    */
    void BroadcastProtocol(Avalon::Protocol& protocol);

	/**
	 *@brief 设置任务变量
	 */
	void SetTaskVar(UInt32 taskId, const char* name, Int64 val);
	/**
	 *@brief 是否为异性混合队伍 在同一场景中
	 */
	bool IsMixedTeamInScene(UInt32 sceneId);

public://事件
	/**
	 *@brief 进入场景
	 */
	void OnEnterScene(Player* player);

	/**
	 *@brief 离开场景
	 */
	void OnLeaveScene(Player* player);

	/**
	 *@brief 发送移动
	 */
	void OnMoved(Player* player);

private:
	/**
	 *@brief 同步位置
	 *@param bOther 是否同步其他人位置
	 */
	void SyncScenePos(Player* player, bool bOtherPos);

private:
	//队伍id
	UInt32	m_Id;
    //比赛ID
    ObjID_t m_raceId;
    //RelayServer的ID
    UInt32 m_relayServerId;
    //Session
    ObjID_t m_session;
	//队长
	ObjID_t	m_Master;
	//队员列表
	MemberVec	m_Members;
	//上次获得道具的队友
	ObjID_t	m_LastItemOwner;

	//队伍boss id
	UInt32	m_TeamBossId;
	//设置boss场景id
	UInt32	m_BossSceneId;
	//设置boss位置
	CLPosition	m_BossPos;
};

#endif
