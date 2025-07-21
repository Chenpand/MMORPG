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

	// 匹配队伍
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
		UInt32	startTime;			// 开始时间
		ObjID_t iBelongMatchUnitID;
		VMatchMember akMembers;
	};
	typedef std::vector<SeMatchTeam*> VMatchTeam;

	// 匹配单元
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
			// 不能重复加入一样的队伍
			if (std::find(aiTeamIDs.begin(), aiTeamIDs.end(), iTeamID) != aiTeamIDs.end())
			{
				return false;
			}

			aiTeamIDs.push_back(iTeamID);
			return true;
		}

		// 获取所有匹配
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
		ObjID_t		            iMatchUnitID;		// 匹配单元唯一ID
		int			            iPlayerNum;			// 总人数
		int			            iMatchScore;		// 总积分
		int						iLevel;				// 等級
		std::vector<ObjID_t>	aiTeamIDs;	        // 所有队伍ID
		int			            iLoop;				// 第几轮匹配
		bool		            bDirty;				// 脏标记
		VMatchMember			akMembers;			// 成员
		UInt32					startTime;			// 开始时间
		SeasonMainLevel			mainLevel;			// 大段位
		UInt32					smallLevel;			// 小段位
		bool					isPromotion;		// 是否是升段站
	};

	// 匹配算法
	class Pk3V3MatchAlgorithmBase
	{
	public:
		typedef std::map<ObjID_t, SeMatchTeam*>				MapMatchTeam;
		typedef std::map<ObjID_t, SeMatchUnit*>				MapMatchUnit;
		typedef std::map<SeMatchKey, MapMatchUnit >			MapMatchPool;
		typedef std::vector<SeMatchUnit*>					MatchResult;		// 匹配结果
		typedef std::vector<MatchResult >					VMatchResult;

	public:
		Pk3V3MatchAlgorithmBase();
		~Pk3V3MatchAlgorithmBase();

		inline int GetFullPlayerNum() { return m_iFullPlayerNum; }

		bool Initialize(int iFullPlayerNum, int iCampNum);

		void Update(UInt64 dwTime);

		bool AddMatchTeam(UInt32 wsId, UInt32 serverCreateTime, ObjID_t iTeamIndex, int iPlayerNum, VMatchMember akMemberes);
		bool DelMatchTeam(ObjID_t iTeamIndex);

		// 获取正在匹配的玩家数量
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

		// 为人数为iSrcPlayerNum的队伍寻找队友
		void FindTeamMates(int iSrcPlayerNum);

		// 寻找队友,人数为iSrcPlayerNum的队伍找人数为iDstPlayerNum的队伍
		void FindTeamMates(int iSrcPlayerNum, int iDstPlayerNum);

		// 匹配队伍
		void DoMatch();

		// 解散长时间未匹配到的队伍，重新放入匹配池中
		void TryDismissMatchUnitsToRematch();

		// 找到第一个可匹配的匹配单元
		SeMatchUnit* GetFirstMatchUnit(int iPlayerNum, SeMatchUnit* pkSrcUnit);

		bool EraseMatchUnitFromMatchPool(const SeMatchUnit* pkUnit);
		void DelMatchUnit(SeMatchUnit* pkUnit);

		// MatchUnit的操作
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
		// 是否满足要求
		virtual bool _IsMatch(SeMatchUnit* a, SeMatchUnit* b);
		// 是否滿足积分要求
		virtual bool _IsMatchScore(SeMatchUnit* a, SeMatchUnit* b);
		// 是否满足等级要求
		virtual bool _IsMatchLevel(SeMatchUnit* a, SeMatchUnit* b);
		virtual int GetMaxMatchSec() const { return 151; }

		/**
		*@brief 获取满足段位要求
		*/
		virtual bool _IsMatchSeasonLevel(SeMatchUnit* a, SeMatchUnit* b);

		/**
		*@brief 获取匹配段位范围
		*/
		void _GetMatchLevelRange(SeMatchUnit* unit, SeasonMainLevel& minMainLevel, SeasonMainLevel& maxMainLevel);

	private:
		ObjID_t					m_iMatchUnitID;

		// 匹配使用的数据结构
		MapMatchTeam            m_akMatchWaitTeam;      // 等待进入匹配池的队伍
		MapMatchTeam			m_akMatchTeam;			// 所有匹配队伍
		MapMatchUnit			m_akMatchUnit;			// 所有匹配单元
		MapMatchPool			m_akMatchPool;			// 匹配池
		VMatchResult			m_akMatchResult;		// 匹配结构

		// 匹配元素
		int						m_iFullPlayerNum;		// 一个完整的匹配单元需要的玩家数(PS:5V5时就是5)
		int                     m_iCampNum;             // 阵营数量(PS:5人的房间就是1,5v5的就是2)

		// 匹配定时器
		UInt32					m_curTime;
		int						m_iStep;
		Match::SeTimer			m_kFindTeammateTimer;	// 寻找队友定时器
		Match::SeTimer			m_kMatchTimer;			// 匹配定时器
		Match::SeTimer          m_kMatchWaitTimer;      // 等待进入匹配池定时器
		Match::SeTimer			m_kDissmissTimer;		// 解散队伍定时器
	};
	// namespace end
};

#endif
