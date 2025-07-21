#ifndef __TRADITION_MATCH_ALGORITHM_H__
#define __TRADITION_MATCH_ALGORITHM_H__

#include "MatchAlgorithmBase.h"
#include "CLPkLevelDataEntry.h"

struct TraditionMatchResult
{
    std::vector<ObjID_t> roleIds;
};
typedef std::vector<TraditionMatchResult> VTraditionMatchResult;

class TraditionMatchAlgorithm : public Match::MatchAlgorithm
{
public:
    TraditionMatchAlgorithm();
    ~TraditionMatchAlgorithm();

    bool AddPlayerToPool(ObjID_t roleId, UInt32 pkValue);
    bool RemovePlayer(ObjID_t roleId);

    void Update();

    VTraditionMatchResult GetMatchResult();

public:
    virtual bool Initialize();
    virtual void OnTick(const Avalon::Time& now);

	virtual bool AddTeam(ObjID_t teamId, UInt8 playerNum, Match::MatchKey& key);
    virtual bool DelTeam(ObjID_t teamId);
    virtual std::vector<Match::MatchTeamResult> GetMatchTeamResult();

private:
    PkLevelInfo _GetPkLevelInfo(UInt32 pkValue);

private:
    UInt64                          m_lastUpdateTime;
    Match::SeMatchAlorithm          m_algorithms[PKLV_NUM];
};

#endif