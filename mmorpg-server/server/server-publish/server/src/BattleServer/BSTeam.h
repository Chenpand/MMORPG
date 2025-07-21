#ifndef _BS_TEAM_H_
#define _BS_TEAM_H_

#include <CLDefine.h>
#include "GameZoneMgr.h"
#include "CLMatchDefine.h"
#include "RobotMgr.h"

class BSTeamMember
{
public:
	BSTeamMember();
	~BSTeamMember();

	bool HasRaceInfo(void) { return m_hasRaceInfo; }
	bool BeRobot(void) { return m_robot; }

	/**
	*@brief 设置获取状态
	*/
	void SetStatus(BattlePlayerState status) { m_playerStatus = status; }
	BattlePlayerState GetStatus(void) { return m_playerStatus; }

	/**
	*@brief 设置获取比赛id
	*/
	void SetRaceId(UInt64 raceId) { m_RaceId = raceId; }
	UInt64 GetRaceId() { return m_RaceId; }

	/**
	*@brief 设置获取状态
	*/
	void AddNoWarCD(UInt64 roleId);
	bool IsNoWarCD(UInt64 roleId);

	void SendProtocol(Avalon::Protocol& protocol);

	void SendToSelfScene(Avalon::Protocol& protocol);

public:
	//账号ID
	UInt32 m_AccId;
	//玩家ID
	UInt64 m_PlayerId;
	//玩家名字
	std::string m_PlayerName;
	//玩家职业
	UInt8 m_PlayerOccu;
	//ConnId
	UInt32 m_ConnId;
	//
	bool m_hasRaceInfo;

	UInt64 m_RaceId;
	//
	RacePlayerInfo m_raceInfo;
	//
	bool m_robot;
	//
	RobotAttrHardType hardType;
	//生命状态
	BattlePlayerState m_playerStatus;
	//
	UInt32 m_kills;
	//
	UInt32 m_protectTimestamp;

	UInt32 m_matchTimestamp;

	// 是否参与过pk
	UInt32 m_IsPk;

	// 免战CD(key->roleID，val->cd)
	std::map<UInt64, UInt64> m_noWarCDMap;
};

typedef std::vector<BSTeamMember> VBSTeamMember;

enum BattleTeamState
{
	//存活
	BTS_ALIVE	= 0,
	//匹配
	BTS_MATCH	= 1,
	//打架
	BTS_PK		= 2,
	//出局
	BTS_OUT		= 3,
};

enum
{
	MAX_MEMBER_IN_TEAM = 3,
};

class BSTeam
{
public:
	BSTeam();
	~BSTeam();

public:
	
	/**
	*@brief 设置获取id
	*/
	void SetId(UInt32 id){ m_Id = id; }
	UInt32 GetId() const{ return m_Id; }
	
	/**
	*@brief 设置获取状态
	*/
	void SetStatus(BattleTeamState status) { m_TeamStatus = status; }
	BattleTeamState GetStatus(void) { return m_TeamStatus; }

	/**
	*@brief 获取人数
	*/
	UInt32 GetSize() const{ return UInt32(m_Members.size()); }

	/**
	*@brief 判断队伍是否已满
	*/
	bool IsFull() const{ return m_Members.size() >= MAX_MEMBER_IN_TEAM; }

	/**
	*@brief 添加移除成员
	*/
	void AddMember(const BSTeamMember& member);
	void DelMember(UInt64 playerId);

	/**
	*@brief 查找成员
	*/
	BSTeamMember* FindMember(UInt64 playerId);
	const VBSTeamMember& GetTeamMembers() const { return m_Members; }

	/**
	*@brief 解散队伍
	*/
	void Dismiss();

	/**
	*@brief 进入匹配
	*/
	void OnMatch();

	void NotifyMsgToAll(Avalon::Protocol& protocol);

public:
	//队伍id
	UInt32	m_Id;
	//队伍状态
	BattleTeamState m_TeamStatus;
	//队伍成员
	VBSTeamMember m_Members;
};

typedef std::vector<BSTeam*> VBSTeam;

#endif //_BS_TEAM_H_
