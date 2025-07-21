#ifndef __MATCH_ALGORITHM_BASE_H__
#define __MATCH_ALGORITHM_BASE_H__

#include <CLDefine.h>
#include <AvalonObjectPool.h>
#include <cassert>
#include <set>
#include "SeTimer.h"

#define DIFF_TIME(a,b) ( (a) >= (b) ? (a - b) : (b - 1) )

// namespace start
namespace Match {



// ƥ�����
struct SeMatchTeam
{
    AVALON_DEFINE_OBJECTPOOL(SeMatchTeam, Avalon::NullMutex)

	SeMatchTeam()
	{
		iTeamID = 0;
		iPlayerNum = 0;
		iScore = 0;
		iBelongMatchUnitID = 0;
	}

	void SetTeamID(const ObjID_t iID) { iTeamID = iID; }
	const ObjID_t GetTeamID() const { return iTeamID; }

	void SetPlayerNum(const int iNum) { iPlayerNum = iNum; }
	int GetPlayerNum() const { return iPlayerNum;	}

	void SetScore(const int iScore) { this->iScore = iScore; }
	int GetScore() const { return iScore; }

	const ObjID_t GetBelongMatchUnitID() const { return iBelongMatchUnitID; }
	void SetBelongMatchUnitID(const ObjID_t iID) { iBelongMatchUnitID = iID; }

	void SetMembers(std::vector<ObjID_t> members) { akMemberIds = members; }
	std::vector<ObjID_t> GetMembers() const { return akMemberIds; }

	void SetBlackList(std::vector<ObjID_t> blackList) { akBlackList = blackList; }
	std::vector<ObjID_t> GetBlackList() const { return akBlackList; }
	bool IsInBlackList(ObjID_t id) { return std::find(akBlackList.begin(), akBlackList.end(), id) != akBlackList.end(); }

private:
    ObjID_t	iTeamID;
	int		iPlayerNum;
	int		iScore;
    ObjID_t iBelongMatchUnitID;
	std::vector<ObjID_t> akMemberIds;
	std::vector<ObjID_t> akBlackList;
};
typedef std::vector<SeMatchTeam*> VMatchTeam;

// ƥ�䵥Ԫ
typedef std::pair<int,int> SeMatchKey;
struct SeMatchUnit
{
    AVALON_DEFINE_OBJECTPOOL(SeMatchUnit, Avalon::NullMutex)

	SeMatchUnit()
	{
		this->iMatchUnitID = 0;
		this->iPlayerNum = 0;
		this->iMatchScore = 0;
		this->iLoop = 0;
		this->bDirty = false;
	}

	bool AddTeam(ObjID_t iTeamID)
	{
		// �����ظ�����һ���Ķ���
		if(std::find(aiTeamIDs.begin(),aiTeamIDs.end(),iTeamID) != aiTeamIDs.end())
		{
			return false;
		}

		aiTeamIDs.push_back(iTeamID);
		return true;
	}

	bool IsMatch(SeMatchUnit* pkOthers, int iMatchScoreDiff)
	{
		if(!pkOthers)
		{
			return false;
		}

		int iMinScore = GetMatchScore() - iMatchScoreDiff;
		int iMaxScore = GetMatchScore() + iMatchScoreDiff;
		if(pkOthers->GetMatchScore() >= iMinScore &&
			pkOthers->GetMatchScore() <= iMaxScore)
		{
			return true;
		}

		return false;
	}

	// ƥ�������ֵ
	int GetMatchScoreDiff() 
	{ 
		const int iMaxMatchScoreDiff = 5;

		int iDiff = iLoop > 10 ? (iLoop - 10) / 2 : 0;
		iDiff = iDiff > iMaxMatchScoreDiff ? iMaxMatchScoreDiff : iDiff;

		return iDiff;
	}

	// ��ȡ����ƥ��
	const std::vector<ObjID_t>& GetAllMatchTeams() const
	{ 
		return aiTeamIDs;
	}

	inline ObjID_t GetMatchUnitID() const { return iMatchUnitID; }
	inline void SetMatchUnitID(const ObjID_t iID) { iMatchUnitID = iID; }

	inline void SetPlayerNum(int iNum) { iPlayerNum = iNum; }
	inline int GetPlayerNum() const { return iPlayerNum; }

	inline void SetMatchScore(int iScore) { iMatchScore = iScore; }
	inline int GetMatchScore() const { return iMatchScore; }

	const SeMatchKey GetKey() const
	{
		return std::make_pair<int,int>(GetPlayerNum(),GetMatchScore());
	}

    int  GetLoop() { return iLoop; }
	void IncLoop() { ++iLoop; }
	void ClearLoop() { iLoop = 0; }

	void SetDirty(bool bDirty) { this->bDirty = bDirty; }
	bool IsDirty() { return bDirty; }

	void AddMembers(std::vector<ObjID_t> members) 
	{
		for (auto id : members)
		{
			akMemberIds.push_back(id);
		}
		
	}
	std::vector<ObjID_t> GetMembers() const { return akMemberIds; }

	void AddBlackList(std::vector<ObjID_t> blackList) 
	{
		for (auto id : blackList)
		{
			akBlackList.insert(id);
		}
	}

	bool IsInBlackList(ObjID_t id)
	{
		return akBlackList.find(id) != akBlackList.end();
	}

	bool IsInBlackList(SeMatchUnit* pkUnit) 
	{ 
		if (!pkUnit)
		{
			return true;
		}

		// �Է����ҵĺ�������
		for (auto id : pkUnit->GetMembers())
		{
			if (IsInBlackList(id))
			{
				return true;
			}
		}

		// ���ڶԷ��ĺ�������
		for (auto id : GetMembers())
		{
			if (pkUnit->IsInBlackList(id))
			{
				return true;
			}
		}

		return false;
	}

private:
    ObjID_t		            iMatchUnitID;		// ƥ�䵥ԪΨһID
	int			            iPlayerNum;			// ������
	int			            iMatchScore;		// �ܻ���
	std::vector<ObjID_t>	aiTeamIDs;	        // ���ж���ID
	int			            iLoop;				// �ڼ���ƥ��
	bool		            bDirty;				// ����
	std::vector<ObjID_t>	akMemberIds;		// ��ԱID
	std::set<ObjID_t>		akBlackList;		// ������
};

// ƥ���㷨
class SeMatchAlorithm
{
public:
	typedef std::map<ObjID_t,SeMatchTeam*>				MapMatchTeam;
	typedef std::map<ObjID_t,SeMatchUnit*>				MapMatchUnit;
	typedef std::map<SeMatchKey,MapMatchUnit >			MapMatchPool;
	typedef std::vector<SeMatchUnit*>					MatchResult;		// ƥ����
	typedef std::vector<MatchResult >					VMatchResult;

public:
	SeMatchAlorithm();
	~SeMatchAlorithm();

    inline int GetFullPlayerNum() { return m_iFullPlayerNum; }

	bool Initialize(int iFullPlayerNum, int iCampNum, int iBaseMatchScoreDiff = 0, int iMaxMatchScoreDiff = 0, int iMatchScoreExpansionMinLoop = 0,
        int iMatchScoreExpansionScore = 0, int iMatchScoreExpansionInterval = 0);

	void Update(UInt64 dwTime);

	bool AddMatchTeam(ObjID_t iTeamIndex, int iPlayerNum, int iScore, std::vector<ObjID_t> akMemberes = std::vector<ObjID_t>(), std::vector<ObjID_t> akBlackList = std::vector<ObjID_t>());
	bool DelMatchTeam(ObjID_t iTeamIndex);

	// ��ȡ����ƥ����������
	int GetMatchingPlayerNum();

	SeMatchKey MakeMatchKey(int iPlayerNum,int iScore);

	VMatchResult& GetAllMatcheResult();
	void DelAllMatchResult();

private:
	const ObjID_t GenID();
	
    inline SeMatchTeam* _CreateMatchTeam() { return new SeMatchTeam(); }
    inline void _DestoryMatchTeam(SeMatchTeam* team) { delete team; }

    inline SeMatchUnit* _CreateMatchUnit() { return new SeMatchUnit(); }
    inline void _DestoryMatchUnit(SeMatchUnit* unit) { delete unit; }

    void _PushAllTeamToMatchPool();
    bool _PushTeamToMatchPool(SeMatchTeam* pkTeam);

	const SeMatchUnit* AddMatchUnitByTeams(VMatchTeam& rakTeam);
	SeMatchTeam* GetMatchTeamByID(ObjID_t iTeamID);

	// Ϊ����ΪiSrcPlayerNum�Ķ���Ѱ�Ҷ���
	void FindTeamMates(int iSrcPlayerNum);

	// Ѱ�Ҷ���,����ΪiSrcPlayerNum�Ķ���������ΪiDstPlayerNum�Ķ���
	void FindTeamMates(int iSrcPlayerNum,int iDstPlayerNum);

	// ƥ�����
	void DoMatch();

	// �ҵ���һ����ƥ���ƥ�䵥Ԫ
	SeMatchUnit* GetFirstMatchUnit(int iPlayerNum,SeMatchUnit* pkSrcUnit);

	bool EraseMatchUnitFromMatchPool(const SeMatchUnit* pkUnit);
	void DelMatchUnit(SeMatchUnit* pkUnit);

	// MatchUnit�Ĳ���
	bool AddMatchTeamToUnit(SeMatchUnit* pkUnit,SeMatchTeam* pkTeam);
	void GetAllMatchTeams(SeMatchUnit* pkUnit,VMatchTeam& rakMatchTeam);
	void CountMatchScore(SeMatchUnit* pkUnit);

protected:
    virtual int GetMatchScoreDiff(SeMatchUnit* pkUnit);
	
private:
    ObjID_t					m_iMatchUnitID;

	// ƥ��ʹ�õ����ݽṹ
    MapMatchTeam            m_akMatchWaitTeam;      // �ȴ�����ƥ��صĶ���
	MapMatchTeam			m_akMatchTeam;			// ����ƥ�����
	MapMatchUnit			m_akMatchUnit;			// ����ƥ�䵥Ԫ
	MapMatchPool			m_akMatchPool;			// ƥ���
	VMatchResult			m_akMatchResult;		// ƥ��ṹ

	// ƥ��Ԫ��
	int						m_iFullPlayerNum;		// һ��������ƥ�䵥Ԫ��Ҫ�������(PS:5V5ʱ����5)
	int                     m_iCampNum;             // ��Ӫ����(PS:5�˵ķ������1,5v5�ľ���2)

	// ƥ�䶨ʱ��
	int						m_iStep;	
	SeTimer					m_kFindTeammateTimer;	// Ѱ�Ҷ��Ѷ�ʱ��
	SeTimer					m_kMatchTimer;			// ƥ�䶨ʱ��
    SeTimer                 m_kMatchWaitTimer;      // �ȴ�����ƥ��ض�ʱ��

    // ƥ��ϵ��
    int                     m_iBaseMatchScoreDiff;              // �����ֲ�
    int                     m_iMaxMatchScoreDiff;               // ���ֲ�
    int                     m_iMatchScoreExpansionMinLoop;      // �ӵڼ��ֿ�ʼ����ֲ�
    int                     m_iMatchScoreExpansionScore;        // ÿ������ķ���
    int                     m_iMatchScoreExpansionInterval;     // ��������������
};

struct MatchTeamResult
{
    std::vector<ObjID_t>    teams;
};

struct MatchKey
{
public:
	// ������ID
	UInt32 wsId;
	// ����ʱ��
	UInt32 serverCreateTime;
	// ����
	UInt32 score;
};

struct PK2V2MatchKey : public MatchKey
{
public:
	UInt32 lv = 0;
};

struct DungeonMatchKey : public MatchKey
{
	UInt32	id;
	UInt32	level;
};

struct SeasonMatchKey : public MatchKey
{
	ObjID_t	id;
	UInt16	level;
	UInt32	seasonLevelId;
	bool	isWudaodahui;
	// ����Ķ���
	std::vector<ObjID_t> lastOpponentList;
};

struct GuildBattleMatchKey : public MatchKey
{
	ObjID_t	guildId;
	UInt8	terrId;
	UInt32	winSteak;
};

struct PremiumLeaguePreliminayMatchKey : public MatchKey
{
	UInt32	seasonLevelId;
	UInt32	winSteak;
};

struct PK3V3MatchMember
{
	ObjID_t roleId;
	UInt16	level;
	UInt32	score;
	// �����ȼ�
	UInt32 seasonLevel;
	// ���λ
	UInt32 mainLevel;
	// С��λ
	UInt32	smallLevel;
	// �Ƿ�������վ
	bool	isPromotion;
};

struct PK3V3MatchKey : public MatchKey
{
	std::vector<PK3V3MatchMember> members;
};

/*
union MatchKey
{
	UInt32		score;
	struct DungeonMatchKey
	{
		UInt32	id;
		UInt32	level;
	}			dungeon;

	struct Season
	{
		ObjID_t	id;
		UInt32	score;
		UInt16	level;
		UInt32	seasonLevelId;
		ObjID_t	lastOpponent;
	}			season;

	struct GuildBattle
	{
		ObjID_t	guildId;
		UInt8	terrId;
		UInt32	winSteak;
	}			guildBattle;
};*/

class MatchAlgorithm
{
public:
    virtual bool Initialize() = 0;
    virtual void OnTick(const Avalon::Time& now) = 0;

	virtual bool AddTeam(ObjID_t teamId, UInt8 playerNum, MatchKey& key) = 0;
    virtual bool DelTeam(ObjID_t teamId) = 0;
    virtual std::vector<MatchTeamResult> GetMatchTeamResult() = 0;
	virtual UInt32 GetMatchPlayerNum() { return 0; }

	virtual ~MatchAlgorithm() {}
};

// namespace end
};

#endif
