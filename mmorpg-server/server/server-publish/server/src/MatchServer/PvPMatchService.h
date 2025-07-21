#ifndef __PVP_MATCH_SERVICE_H__
#define __PVP_MATCH_SERVICE_H__

#include "MatchService.h"
#include "PvPMatchRace.h"
#include <CLSeasonDefine.h>

class PvPMatchService : public MatchService
{
	// ��ౣ�����ʷ���ּ�¼
	const UInt32 MAX_PVP_HISTORY_OPPONENT = 3;
public:
    PvPMatchService(bool useRobot = false);
	virtual ~PvPMatchService();

	virtual bool OnTick(const Avalon::Time& now);
    virtual bool Init();
    virtual bool OnTeamStartMatch(MatchTeam* team);
    virtual bool OnJoinMatchPool(MatchTeam* team);
    virtual UInt32 OnTeamCancelMatch(ObjID_t teamId);
    virtual bool OnMatchSuccess(VMatchTeam allTeams);
    virtual void OnQueryPlayerRaceInfoRes(ObjID_t sessionId, ObjID_t roleId, UInt8 result, RacePlayerInfo& info);

	virtual void OnRaceStarted(ObjID_t sessionId) {}

    /**
    *@brief RelayServer��ʼս������
    */
    virtual void OnRelayServerStartResponse(UInt32 result, ObjID_t gamesessionID, UInt32 abality, const Avalon::SockAddr& address);

protected:
	bool OnStartRaceWithRobot(MatchTeam* team);
	RobotAIHardType GenRobotAIHard(MatchTeamMember* member, UInt16 level);
	RobotAttrHardType GenRobotAttrHard(MatchTeamMember* member, UInt16 level);
	UInt32 GenRobotMatchScore(UInt32 score);

protected:
	/**
	*@brief ���ƥ����ʷ
	*/
	void AddHistory(ObjID_t player1, ObjID_t player2);

	/**
	*@brief ��ȡ��һ������
	*/
	std::vector<ObjID_t> GetLastOpponent(ObjID_t id);

	/**
	*@brief ���һ��PK����
	*/
	virtual UInt32 GenPkDungeon();

	/**
	*@brief �������PK��λ
	*/
	UInt32 _GenRobotPkValue(UInt32 pkValue);

	/**
	*@brief �������������λ
	*/
	UInt32 _GenRobotSeasonLevel(UInt32 seasonLevel);

protected:
    virtual MatchRace* CreateRace() { return new PvPMatchRace(); }
	virtual MatchQueryType GetMatchQueryType() { return MATCH_QUERY_RACE_INFO; }
	virtual RaceType GetRaceType() { return RT_PK; }

protected:
	virtual UInt32 GetWaitMatchUntilRobotSec(SeasonMainLevel level = SML_BRONZE) const;

protected:
	bool		m_useRobot;

	// ƥ����ʷ
	HashMap<ObjID_t, std::list<ObjID_t>>		m_mathchHistory;

	// pk���³��б�
	std::vector<UInt32>							m_pkDungeonIdList;
};

#endif