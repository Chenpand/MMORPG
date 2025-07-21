#pragma once
#include "BSBaseRace.h"
#include <CLGameSessionProtocol.h>
#include <CLLostDungeonDefine.h>

class BSLostBattle;
class BSPlayer;

// ��ӿ�ʼ���³�ͶƱ
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

	// ���³�ID
	UInt32					dungeonId;
	// ����ͬ���˵����
	std::vector<ObjID_t>	agreePlayerIds;
	// ��ʼʱ��
	UInt32					startTime;
};


//��ʧ������Ӵ�ؿ���������
class BSLostTeamDungeonRace : public BSBaseRace
{
public:
	BSLostTeamDungeonRace();
	BSLostTeamDungeonRace(UInt8 raceType);
	~BSLostTeamDungeonRace();

	void OnCreate(CLProtocol::DungenServerTeamStartRaceReq& teamStartRaceReq);

	inline void SetLostBattle(BSLostBattle*  battle) { m_ownerBattle = battle; }
	BSLostBattle* GetLostBattle();

	inline UInt32 GetTeamId() { return m_TeamId; }
	inline UInt64 GetMaster() { return m_Master; }

	inline UInt8 GetMemberSize() { return m_teamMembers.size(); }

	/**
	 *@brief �㲥�����г�Ա
	 */
	void Broadcast(Avalon::Protocol& protocol);

	/**
	*@brief ��ʼ���³�ͶƱ
	*/
	UInt32 OnStartRaceVote(UInt32 dungeonId);
	
	/**
	*@brief ����ϴ���ʼ���³�ͶƱ
	*/
	void OnPlayerReportVote(BSPlayer* player, bool agree);

	/**
	*@brief ������Ҷ�ͬ��
	*/
	void OnStartRaceVoteAllAgree();

	/**
	*@brief ����Ҿܾ�
	*/
	void OnStartRaceVoteFailure(ObjID_t roleId = 0, UInt32 noticeId = 0);

	/**
	 *@brief �㲥��������г�Ա
	 */
	void Notify(UInt16 type, UInt8 color, const char* word);

	void SyncTeamState(LostDungTeamBattleSt st, UInt8 isEnd = 0);

	virtual UInt32 StartRace();
	virtual void OnStartRaceFail();
	virtual void OnStartDungeonRes(UInt32 result, UInt64 gamesessionID);
	virtual void OnDungeonEnd(UInt8 result, DungeonRaceEndInfo& endInfo);
	virtual void OnTick(const Avalon::Time& now);
	virtual BSBaseBattle* GetBattle();

private:
	std::vector<LostDungRaceMember> m_teamMembers;
	BSLostBattle*  m_ownerBattle; //����ս��
	//��ʼ����ͶƱ
	TeamDungeonStartVote	m_Vote;
	UInt64	m_Master;
	UInt32  m_TeamId;
	CLProtocol::DungenServerTeamStartRaceReq m_teamStartRaceReq;
};