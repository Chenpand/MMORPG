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
	*@brief ���û�ȡ״̬
	*/
	void SetStatus(BattlePlayerState status) { m_playerStatus = status; }
	BattlePlayerState GetStatus(void) { return m_playerStatus; }

	/**
	*@brief ���û�ȡ����id
	*/
	void SetRaceId(UInt64 raceId) { m_RaceId = raceId; }
	UInt64 GetRaceId() { return m_RaceId; }

	/**
	*@brief ���û�ȡ״̬
	*/
	void AddNoWarCD(UInt64 roleId);
	bool IsNoWarCD(UInt64 roleId);

	void SendProtocol(Avalon::Protocol& protocol);

	void SendToSelfScene(Avalon::Protocol& protocol);

public:
	//�˺�ID
	UInt32 m_AccId;
	//���ID
	UInt64 m_PlayerId;
	//�������
	std::string m_PlayerName;
	//���ְҵ
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
	//����״̬
	BattlePlayerState m_playerStatus;
	//
	UInt32 m_kills;
	//
	UInt32 m_protectTimestamp;

	UInt32 m_matchTimestamp;

	// �Ƿ�����pk
	UInt32 m_IsPk;

	// ��սCD(key->roleID��val->cd)
	std::map<UInt64, UInt64> m_noWarCDMap;
};

typedef std::vector<BSTeamMember> VBSTeamMember;

enum BattleTeamState
{
	//���
	BTS_ALIVE	= 0,
	//ƥ��
	BTS_MATCH	= 1,
	//���
	BTS_PK		= 2,
	//����
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
	*@brief ���û�ȡid
	*/
	void SetId(UInt32 id){ m_Id = id; }
	UInt32 GetId() const{ return m_Id; }
	
	/**
	*@brief ���û�ȡ״̬
	*/
	void SetStatus(BattleTeamState status) { m_TeamStatus = status; }
	BattleTeamState GetStatus(void) { return m_TeamStatus; }

	/**
	*@brief ��ȡ����
	*/
	UInt32 GetSize() const{ return UInt32(m_Members.size()); }

	/**
	*@brief �ж϶����Ƿ�����
	*/
	bool IsFull() const{ return m_Members.size() >= MAX_MEMBER_IN_TEAM; }

	/**
	*@brief ����Ƴ���Ա
	*/
	void AddMember(const BSTeamMember& member);
	void DelMember(UInt64 playerId);

	/**
	*@brief ���ҳ�Ա
	*/
	BSTeamMember* FindMember(UInt64 playerId);
	const VBSTeamMember& GetTeamMembers() const { return m_Members; }

	/**
	*@brief ��ɢ����
	*/
	void Dismiss();

	/**
	*@brief ����ƥ��
	*/
	void OnMatch();

	void NotifyMsgToAll(Avalon::Protocol& protocol);

public:
	//����id
	UInt32	m_Id;
	//����״̬
	BattleTeamState m_TeamStatus;
	//�����Ա
	VBSTeamMember m_Members;
};

typedef std::vector<BSTeam*> VBSTeam;

#endif //_BS_TEAM_H_
