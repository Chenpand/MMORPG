#ifndef __MATCH_PLAYER_H__
#define __MATCH_PLAYER_H__

#include <CLDefine.h>
#include <CLObject.h>
#include <AvalonObjectPool.h>

enum MatchState
{
    MS_IDEL,
	MS_MATCHING,		// ����ƥ��
    MS_WAIT_WORLD_REP,  // �ȴ�WorldServer�������ս����Ϣ
	MS_WAIT_RELAY_REP,	// �ȴ�RelayServer����
	MS_RACING,			// ����ս��
};

class MatchPlayer : public CLObject
{
	AVALON_DEFINE_OBJECTPOOL(MatchPlayer, Avalon::NullMutex)

	MatchPlayer()
	{
		SetMatchState(MS_IDEL);
		m_AccountID = 0;
        m_ServerID = 0;
		m_MatchScore = 0;
	}

    inline UInt32 GetServerID() const { return m_ServerID; }
    inline void SetServerID(UInt32 id) { m_ServerID = id; }

	inline MatchState GetMatchState() const { return m_MatchState; }
	inline void SetMatchState(MatchState state) { m_MatchState = state; }

	inline UInt32 GetAccID() const { return m_AccountID; }
	inline void SetAccId(UInt32 accID) { m_AccountID = accID; }

	inline UInt32 GetMatchScore() const { return m_MatchScore; }
	inline void SetMatchScore(UInt32 score) { m_MatchScore = score; }

	inline Avalon::Time GetStartTime() const { return m_StartTime; }
	inline void SetStartTime(Avalon::Time startTime) { m_StartTime = startTime; }

    inline ObjID_t GetSessionID() const { return m_SessionID; }
    inline void SetSessionID(ObjID_t id) { m_SessionID = id; }

    bool IsMatchTimeOut(Avalon::Time now) const {
        if (GetMatchState() != MS_MATCHING) {
            return false;
        }

        if (now.MSec() - GetStartTime().MSec() > 30 * Avalon::Time::MSECS_OF_SEC) {
            return true;
        }

        return false;
    }

	UInt32					m_AccountID;		// �˺�ID
    UInt32                  m_ServerID;         // WorldServer��ID
	MatchState				m_MatchState;		// ƥ��״̬
	UInt32					m_MatchScore;		// ƥ���õĻ���
	Avalon::Time			m_StartTime;		// ��ʼƥ��ʱ��
    ObjID_t                 m_SessionID;        // ƥ������SessionID;
};

typedef std::vector<MatchPlayer*> VMatchPlayer;

#endif
