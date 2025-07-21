#ifndef __SEASON_LEVEL_MATCH_ALGORITHM_H__
#define __SEASON_LEVEL_MATCH_ALGORITHM_H__

#include "MatchAlgorithmBase.h"
#include <CLSeasonDefine.h>

struct GuildLevelMatchUnit
{
	GuildLevelMatchUnit()
	{
		wsId = 0;
		serverCreateTime = 0;
		roleId = 0;
		teamId = 0;
		level = 0;
		mainLevel = SML_BRONZE;
		smallLevel = 0;
		matchScore = 0;
		isPromotion = false;
		isWudaodahui = false;
		startTime = 0;
		dirty = false;
	}

	// ������ID
	UInt32	wsId;
	// ����������ʱ��
	UInt32  serverCreateTime;
	// ��ɫID
	ObjID_t	roleId;
	// ����ID
	ObjID_t teamId;
	// �ȼ�
	UInt16	level;
	// ���λ
	SeasonMainLevel mainLevel;
	// С��λ
	UInt32	smallLevel;
	// ƥ�����
	UInt32  matchScore;
	// ������
	std::vector<ObjID_t> blackList;
	// �Ƿ�������վ
	bool	isPromotion;
	// �Ƿ���������
	bool	isWudaodahui;
	// ��ʼʱ��(s)
	UInt32	startTime;
	// �Ƿ��Ѿ�ƥ�䵽
	bool	dirty;
};

class SeasonLevelMatchAlgorithm : public Match::MatchAlgorithm
{
	typedef std::vector<GuildLevelMatchUnit*> MatchPool;
public:
	SeasonLevelMatchAlgorithm();
	~SeasonLevelMatchAlgorithm();

public:
	virtual bool Initialize();
	virtual void OnTick(const Avalon::Time& now);

	virtual bool AddTeam(ObjID_t teamId, UInt8 playerNum, Match::MatchKey& key);
	virtual bool DelTeam(ObjID_t teamId);
	virtual std::vector<Match::MatchTeamResult> GetMatchTeamResult();

protected:
	/**
	*@brief �����ƥ��Ļ��ֲ�
	*/
	UInt32 _GetMatchScoreDiff(GuildLevelMatchUnit* unit);

	/**
	*@brief ��ȡƥ���λ��Χ
	*/
	void _GetMatchLevelRange(GuildLevelMatchUnit* unit, SeasonMainLevel& minMainLevel, SeasonMainLevel& maxMainLevel);

	/**
	*@brief ��ȡ��ƥ������ȼ�
	*/
	UInt16 _GetMatchMaxLevel(UInt16 level);

	GuildLevelMatchUnit* _FindOpponent(GuildLevelMatchUnit* unit);
	bool IsMatch(GuildLevelMatchUnit* a, GuildLevelMatchUnit* b);
	void _DelFromPool(GuildLevelMatchUnit* unit);

private:
	UInt64										m_lastUpdateTime;

	std::map<ObjID_t, GuildLevelMatchUnit*>		m_matchUnits;
};

#endif