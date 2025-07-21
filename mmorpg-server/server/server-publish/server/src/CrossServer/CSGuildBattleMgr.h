#ifndef _CS_GUILD_BATTLE_MGR_H_
#define _CS_GUILD_BATTLE_MGR_H_

#include <AvalonSingleton.h>
#include <CLDefine.h>
#include <CLGuildDefine.h>
#include <CLGameSessionProtocol.h>
#include "CSGuildBattleTerritory.h"
#include "GameZoneMgr.h"

class CSGuildBattleMgr : public Avalon::Singleton<CSGuildBattleMgr>
{
public:
	CSGuildBattleMgr() : m_Status(GuildBattleStatus::GBS_INVALID), m_TargetStatus(GuildBattleStatus::GBS_INVALID), m_CheckWaitTime(0), m_IsByTime(true){}
	~CSGuildBattleMgr(){}

	bool Init();

public:
	/**
	*@brief 加载报名信息
	*/
	void LoadEnroll(std::map<UInt8, std::vector<CSEnrollInfo>> enrollInfo);

	/**
	*@brief 增加报名信息
	*/
	UInt32 AddEnroll(CSEnrollInfo info);

	/**
	*@brief 设置领地信息
	*/
	UInt32 SetTerritoryBaseInfo(std::vector<GuildTerritoryBaseInfo> info);

	/**
	*@brief 获取领地信息
	*/
	UInt32 GetTerritoryBaseInfo(UInt8 terrId, GuildTerritoryBaseInfo& info);

	/**
	*@brief 获取所有领地信息
	*/
	UInt32 GetTerritoryBaseInfos(std::vector<GuildTerritoryBaseInfo>& infos);

	/**
	*@brief 一场公会战结果
	*/
	UInt32 OnCrossBattleRaceEnd(CLProtocol::MatchServerGuildBattleRaceEnd& raceEnd);

	/**
	*@brief 请求公会战记录
	*/
	UInt32 GetBattleRecord(UInt8 terrId, std::vector<GuildBattleRecord>& records, Int32 startIndex, UInt32 count);
	UInt32 GetBattleSelfRecrod(UInt8 terrId, ObjID_t playerId, std::vector<GuildBattleRecord>& records);

	/**
	*@brief 更新排行榜
	*/
	UInt32 UpdateSortList(UInt8 terrId, TerritorySortListEntry& entry);

	/**
	*@brief 获取排行榜
	*/
	UInt32 GetSortList(GameZone* zone, UInt8 terrId, ObjID_t playerId, ObjID_t guildId, SortListType sortType, UInt16 start, UInt16 count);

public:
	void OnTick(const Avalon::Time& now);

	void OnConnected(UInt32 id);

	void OnDisconnected(UInt32 id);

	/**
	*@brief 报名开始
	*/
	void OnEnrollBegin();

	/**
	*@brief 报名结束
	*/
	void OnEnrollEnd();

	/**
	*@brief 战斗准备
	*/
	void OnBattlePrepare();

	/**
	*@brief 战斗开始
	*/
	void OnBattleBegin();

	/**
	*@brief 战斗结束
	*/
	void OnBattleEnd();

	/**
	*@brief 领奖
	*/
	void OnBattleReward();

	/**
	*@brief 转换到目标公会战状态
	*/
	void ChangeTargetStatus(GuildBattleStatus targetStatus);

public:
	/**
	*@brief 是否跨服工会战
	*/
	bool IsCrossBattle();

	/**
	*@brief 移除不需要等待报名消息的ID
	*/
	void RemoveWaitLoadEnrollConnId(UInt32 id);

	/**
	*@brief 获取报名数量
	*/
	UInt32 GetEnrollSize(UInt32 terrId);

	/**
	*@brief 公会战是否处于开启状态
	*/
	bool IsGuildBattle() { return m_Status == GuildBattleStatus::GBS_BATTLE; }

	/**
	*@brief 公会战是否可以报名
	*/
	bool IsGuildBattleEnroll() { return m_Status == GuildBattleStatus::GBS_ENROLL; }

	/**
	*@brief 设置公会战类型
	*/
	void SetGuildBattleType(GuildBattleType type) { m_Type = type; };

	/**
	*@brief 不根据自然时间变化状态
	*/
	void SetIsByTime(bool isByTime) { m_IsByTime = isByTime; }

	/**
	*@brief 设置公会战时间信息
	*/
	void SetGuildBattleTimeInfo(GuildBattleTimeInfo info);

	/**
	*@brief 获取排名
	*/
	UInt32 GetRanking(UInt8 terrId, SortListType sortType, ObjID_t playerId, ObjID_t guildId);

private:
	bool _IsEnroll(UInt64 guildId);

	/**
	*@brief 清理领地信息
	*/
	void _ClearTerritory();

	/**
	*@brief 清理领地信息
	*/
	void _ClearTerritory(UInt8 terrId);

	/**
	*@brief 获取领地信息
	*/
	CSGuildBattleTerritory* _GetGuildBattleTerritory(UInt8 terrId);

	/**
	*@brief 初始化公会战状态
	*/
	void _InitGuildBattleStatus();

	/**
	*@brief 设置公会战状态
	*/
	void _SetGuildBattleStatus(GuildBattleStatus status);

	/**
	*@brief 执行状态转换
	*/
	void _ChangeToTargetStatus();

private:
	//报名信息
	HashMap<UInt8, std::vector<CSEnrollInfo>>		m_Enroll;

	//各个领地信息
	HashMap<UInt8, CSGuildBattleTerritory>		m_Territory;

	//等待加载报名信息的服务器
	std::vector<UInt32>							m_WaitLoadEnrollConnIds;

	//当前工会战状态
	GuildBattleStatus							m_Status;

	//目标状态
	GuildBattleStatus							m_TargetStatus;

	//当前公会战类型
	GuildBattleType								m_Type;

	//公会战时间
	GuildBattleTimeInfo							m_GuildBattleTimeInfo;

	//公会战结果
	std::vector<GuildBattleEndInfo>				m_GuildBattleEndInfo;

	//最后Tick时间
	UInt32										m_CheckWaitTime;

	//是否根据自然时间
	bool										m_IsByTime;
};


#endif //_CS_GUILD_BATTLE_H_