#ifndef __MATCH_SERVICE_PLAYER_H__
#define __MATCH_SERVICE_PLAYER_H__

#include <CLDefine.h>
#include <CLObject.h>
#include <AvalonObjectPool.h>
#include "MatchService.h"

class MatchServicePlayer : public CLObject
{
    AVALON_DEFINE_OBJECTPOOL(MatchServicePlayer, Avalon::NullMutex)
public:
    MatchServicePlayer();
    ~MatchServicePlayer();

    inline void SetWsID(UInt32 id) { m_wsId = id; }
    inline UInt32 GetWsID() const { return m_wsId; }

    inline void SetAccountID(UInt32 id) { m_accId = id; }
    inline UInt32 GetAccouontID() const { return m_accId; }

    inline void SetTeamID(ObjID_t id) { m_teamId = id; }
    inline ObjID_t GetTeamID() const { return m_teamId; }

    inline void SetMatchType(MatchServiceType type) { m_matchType = type; }
    inline MatchServiceType GetMatchType() const { return m_matchType; }

private:
    // WorldServer��ID
    UInt32                      m_wsId;
    // accountId
    UInt32                      m_accId;
    // ƥ������
    MatchServiceType            m_matchType;
    // ����ID
    ObjID_t                     m_teamId;
};

#endif