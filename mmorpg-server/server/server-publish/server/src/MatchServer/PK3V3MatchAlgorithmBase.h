#ifndef __3V3_MATCH_ALGORITHM_BASE_H__
#define __3V3_MATCH_ALGORITHM_BASE_H__

#include <CLDefine.h>
#include <AvalonObjectPool.h>
#include <cassert>
#include <set>
#include <CLSeasonDataEntry.h>
#include "SeTimer.h"
#include "MatchAlgorithmBase.h"

// namespace start
namespace PK3V3 {
	typedef std::vector<Match::PK3V3MatchMember> VMatchMember;

	// ƥ�����
	struct SeMatchTeam
	{
		AVALON_DEFINE_OBJECTPOOL(SeMatchTeam, Avalon::NullMutex)

		SeMatchTeam()
		{
			wsId = 0;
			serverCreateTime = 0;
			iTeamID = 0;
			iPlayerNum = 0;
			iScore = 0;
			iBelongMatchUnitID = 0;
			startTime = (UInt32)CURRENT_TIME.Sec();
		}

		void SetTeamID(const ObjID_t iID) { iTeamID = iID; }
		const ObjID_t GetTeamID() const { return iTeamID; }

		void SetPlayerNum(const int iNum) { iPlayerNum = iNum; }
		int GetPlayerNum() const { return iPlayerNum; }

		void SetScore(const int iScore) { this->iScore = iScore; }
		int GetScore() const { return iScore; }

		void SetStartTime(UInt32 now) { startTime = now; }
		UInt32 GetStartTime() const { return startTime; }
		UInt32 GetPassSec(UInt32 now) const { return now >= startTime ? (now - startTime) : 0; }

		const ObjID_t GetBelongMatchUnitID() const { return iBelongMatchUnitID; }
		void SetBelongMatchUnitID(const ObjID_t iID) { iBelongMatchUnitID = iID; }

		void SetMembers(VMatchMember members) { akMembers = members; }
		const VMatchMember& GetMembers() const { return akMembers; }

		void SetWSID(UInt32 id) { wsId = id; }
		UInt32 GetWSID() const { return wsId; }

		void SetServerCreateTime(UInt32 time) { serverCreateTime = time; }
		UInt32 GetServerCreateTime() const { return serverCreateTime; }

	private:
		ObjID_t	iTeamID;
		UInt32  wsId;
		UInt32	serverCreateTime;
		int		iPlayerNum;
		int		iScore;
		UInt32	startTime;			// ��ʼʱ��
		ObjID_t iBelongMatchUnitID;
		VMatchMember akMembers;
	};
	typedef std::vector<SeMatchTeam*> VMatchTeam;

	// ƥ�䵥Ԫ
	typedef std::pair<int, int> SeMatchKey;
	struct SeMatchUnit
	{
		AVALON_DEFINE_OBJECTPOOL(SeMatchUnit, Avalon::NullMutex)

		SeMatchUnit()
		{
			wsId = 0;
			serverCreateTime = 0;
			this->iMatchUnitID = 0;
			this->iPlayerNum = 0;
			this->iMatchScore = 0;
			this->iLoop = 0;
			this->bDirty = false;
			this->iLevel = 0;
			this->startTime = (UInt32)CURRENT_TIME.Sec();
			mainLevel = SML_BRONZE;
			smallLevel = 0;
			isPromotion = false;
		}

		bool AddTeam(ObjID_t iTeamID)
		{
			// �����ظ�����һ���Ķ���
			if (std::find(aiTeamIDs.begin(), aiTeamIDs.end(), iTeamID) != aiTeamIDs.end())
			{
				return false;
			}

			aiTeamIDs.push_back(iTeamID);
			return true;
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

		inline void SetLevel(int level) { iLevel = level; }
		inline int GetLevel() const { return iLevel; }

		const SeMatchKey GetKey() const
		{
			return std::make_pair<int, int>(GetPlayerNum(), GetMatchScore());
		}

		int  GetLoop() const { return iLoop; }
		void IncLoop() { ++iLoop; }
		void ClearLoop() { iLoop = 0; }

		UInt32 GetStartTime() { return startTime; }
		void SetStartTime(UInt32 time) { startTime = time; }
		UInt32 GetPassSec(UInt32 now) const { return now >= startTime ? (now - startTime) : 0; }
		UInt32 GetTeamSize() const { return aiTeamIDs.size(); }

		void SetDirty(bool bDirty) { this->bDirty = bDirty; }
		bool IsDirty() { return bDirty; }

		void ClearMembers() { akMembers.clear(); }
		void AddMembers(const VMatchMember& members)
		{
			for (auto member : members)
			{
				akMembers.push_back(member);
			}

		}
		VMatchMember GetMembers() const { return akMembers; }

		SeasonMainLevel GetSeasonMainLevel() const { return mainLevel; }
		void SetSeasonMainLevel(SeasonMainLevel level) { mainLevel = level; }

		UInt32 GetSeasonSmallLevel() const { return smallLevel; }
		void SetSeasonSmallLevel(UInt32 level) { smallLevel = level; }

		bool IsPromotion() const { return isPromotion; }
		void SetIsPromotion(bool promotion) { isPromotion = promotion; }

		void SetWSID(UInt32 id) { wsId = id; }
		UInt32 GetWSID() const { return wsId; }

		void SetServerCreateTime(UInt32 time) { serverCreateTime = time; }
		UInt32 GetServerCreateTime() const { return serverCreateTime; }

	private:
		UInt32					wsId;
		UInt32					serverCreateTime;
		ObjID_t		            iMatchUnitID;		// ƥ�䵥ԪΨһID
		int			            iPlayerNum;			// ������
		int			            iMatchScore;		// �ܻ���
		int						iLevel;				// �ȼ�
		std::vector<ObjID_t>	aiTeamIDs;	        // ���ж���ID
		int			            iLoop;				// �ڼ���ƥ��
		bool		            bDirty;				// ����
		VMatchMember			akMembers;			// ��Ա
		UInt32					startTime;			// ��ʼʱ��
		SeasonMainLevel			mainLevel;			// ���λ
		UInt32					smallLevel;			// С��λ
		bool					isPromotion;		// �Ƿ�������վ
	};

	// ƥ���㷨
	class Pk3V3MatchAlgorithmBase
	{
	public:
		typedef std::map<ObjID_t, SeMatchTeam*>				MapMatchTeam;
		typedef std::map<ObjID_t, SeMatchUnit*>				MapMatchUnit;
		typedef std::map<SeMatchKey, MapMatchUnit >			MapMatchPool;
		typedef std::vector<SeMatchUnit*>					MatchResult;		// ƥ����
		typedef std::vector<MatchResult >					VMatchResult;

	public:
		Pk3V3MatchAlgorithmBase();
		~Pk3V3MatchAlgorithmBase();

		inline int GetFullPlayerNum() { return m_iFullPlayerNum; }

		bool Initialize(int iFullPlayerNum, int iCampNum);

		void Update(UInt64 dwTime);

		bool AddMatchTeam(UInt32 wsId, UInt32 serverCreateTime, ObjID_t iTeamIndex, int iPlayerNum, VMatchMember akMemberes);
		bool DelMatchTeam(ObjID_t iTeamIndex);

		// ��ȡ����ƥ����������
		int GetMatchingPlayerNum();

		SeMatchKey MakeMatchKey(int iPlayerNum, int iScore);

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
		void FindTeamMates(int iSrcPlayerNum, int iDstPlayerNum);

		// ƥ�����
		void DoMatch();

		// ��ɢ��ʱ��δƥ�䵽�Ķ��飬���·���ƥ�����
		void TryDismissMatchUnitsToRematch();

		// �ҵ���һ����ƥ���ƥ�䵥Ԫ
		SeMatchUnit* GetFirstMatchUnit(int iPlayerNum, SeMatchUnit* pkSrcUnit);

		bool EraseMatchUnitFromMatchPool(const SeMatchUnit* pkUnit);
		void DelMatchUnit(SeMatchUnit* pkUnit);

		// MatchUnit�Ĳ���
		bool AddMatchTeamToUnit(SeMatchUnit* pkUnit, SeMatchTeam* pkTeam);
		void GetAllMatchTeams(SeMatchUnit* pkUnit, VMatchTeam& rakMatchTeam);
		void CountMatchScore(SeMatchUnit* pkUnit);
		UInt32 CountMatchScore(const VMatchMember& members);
		UInt32 CountMatchLevel(const VMatchMember& members);
		void CountSeasonLevel(SeMatchUnit* pkUnit);

	protected:
		virtual int GetMatchScoreDiff(SeMatchUnit* pkUnit);
		virtual int GetMaxMatchLevel(SeMatchUnit* pkUnit);

	protected:
		// �Ƿ�����Ҫ��
		virtual bool _IsMatch(SeMatchUnit* a, SeMatchUnit* b);
		// �Ƿ�M�����Ҫ��
		virtual bool _IsMatchScore(SeMatchUnit* a, SeMatchUnit* b);
		// �Ƿ�����ȼ�Ҫ��
		virtual bool _IsMatchLevel(SeMatchUnit* a, SeMatchUnit* b);
		virtual int GetMaxMatchSec() const { return 151; }

		/**
		*@brief ��ȡ�����λҪ��
		*/
		virtual bool _IsMatchSeasonLevel(SeMatchUnit* a, SeMatchUnit* b);

		/**
		*@brief ��ȡƥ���λ��Χ
		*/
		void _GetMatchLevelRange(SeMatchUnit* unit, SeasonMainLevel& minMainLevel, SeasonMainLevel& maxMainLevel);

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
		UInt32					m_curTime;
		int						m_iStep;
		Match::SeTimer			m_kFindTeammateTimer;	// Ѱ�Ҷ��Ѷ�ʱ��
		Match::SeTimer			m_kMatchTimer;			// ƥ�䶨ʱ��
		Match::SeTimer          m_kMatchWaitTimer;      // �ȴ�����ƥ��ض�ʱ��
		Match::SeTimer			m_kDissmissTimer;		// ��ɢ���鶨ʱ��
	};
	// namespace end
};

#endif
