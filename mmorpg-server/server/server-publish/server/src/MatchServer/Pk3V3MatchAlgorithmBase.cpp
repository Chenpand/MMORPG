#include "PK3V3MatchAlgorithmBase.h"
#include "MatchAlgorithmBase.h"

// namespace start
namespace PK3V3 {

	Pk3V3MatchAlgorithmBase::Pk3V3MatchAlgorithmBase()
	{
		m_iMatchUnitID = 0;
		m_iFullPlayerNum = 0;
		m_iStep = 0;
	}

	Pk3V3MatchAlgorithmBase::~Pk3V3MatchAlgorithmBase()
	{

	}

	bool Pk3V3MatchAlgorithmBase::Initialize(int iFullPlayerNum, int iCampNum)
	{
		if (iFullPlayerNum <= 0)
		{
			return false;
		}

		m_iFullPlayerNum = iFullPlayerNum;
		m_iCampNum = iCampNum;

		m_kFindTeammateTimer.SetInterValue(500);
		m_kMatchTimer.SetInterValue(1000);
		m_kDissmissTimer.SetInterValue(2000);

		return true;
	}

	void Pk3V3MatchAlgorithmBase::Update(UInt64 dwTime)
	{
		if (m_iFullPlayerNum <= 0)
		{
			return;
		}

		m_curTime = (UInt32)(dwTime / 1000);

		m_kMatchWaitTimer.Update(dwTime);
		m_kFindTeammateTimer.Update(dwTime);
		m_kMatchTimer.Update(dwTime);
		m_kDissmissTimer.Update(dwTime);

		{
			_PushAllTeamToMatchPool();
		}

		if (m_kFindTeammateTimer.IsPassed())
		{
			if (m_iFullPlayerNum > 1)
			{
				FindTeamMates(m_iStep + 1);
				m_iStep = (m_iStep + 1) % (m_iFullPlayerNum - 1);
			}
		}

		if (m_kMatchTimer.IsPassed())
		{
			DoMatch();
		}

		if (m_kDissmissTimer.IsPassed())
		{
			TryDismissMatchUnitsToRematch();
		}
	}

	const ObjID_t Pk3V3MatchAlgorithmBase::GenID()
	{
		return ++m_iMatchUnitID;
	}

	SeMatchKey Pk3V3MatchAlgorithmBase::MakeMatchKey(int iPlayerNum, int iScore)
	{
		return std::make_pair(iPlayerNum, iScore);
	}

	void Pk3V3MatchAlgorithmBase::_PushAllTeamToMatchPool()
	{
		std::vector<SeMatchTeam*> apkTeam;
		for (MapMatchTeam::iterator kItr = m_akMatchWaitTeam.begin(); kItr != m_akMatchWaitTeam.end(); ++kItr)
		{
			apkTeam.push_back(kItr->second);
		}

		m_akMatchWaitTeam.clear();

		// 打乱队伍顺序，把所有等待中的队伍放到匹配池中
		// 随机排列数组
		std::random_shuffle(apkTeam.begin(), apkTeam.end());
		for (UInt32 i = 0; i < apkTeam.size(); i++)
		{
			SeMatchTeam* pkTeam = apkTeam[i];
			_PushTeamToMatchPool(pkTeam);
		}
	}

	bool Pk3V3MatchAlgorithmBase::_PushTeamToMatchPool(SeMatchTeam* pkTeam)
	{
		if (!pkTeam)
		{
			return false;
		}
		ObjID_t iTeamIndex = pkTeam->GetTeamID();

		VMatchTeam akTeams;
		akTeams.push_back(pkTeam);

		m_akMatchTeam[iTeamIndex] = pkTeam;
		if (!AddMatchUnitByTeams(akTeams))
		{
			m_akMatchTeam.erase(iTeamIndex);
			_DestoryMatchTeam(pkTeam);
			return false;
		}


		return true;
	}

	const SeMatchUnit* Pk3V3MatchAlgorithmBase::AddMatchUnitByTeams(VMatchTeam& rakTeam)
	{
		if (rakTeam.empty())
		{
			return NULL;
		}

		SeMatchUnit* pkMatchUnit = _CreateMatchUnit();
		if (!pkMatchUnit)
		{
			return NULL;
		}

		// 分配ID
		pkMatchUnit->SetMatchUnitID(GenID());

		for (UInt32 i = 0; i < rakTeam.size(); i++)
		{
			if (!AddMatchTeamToUnit(pkMatchUnit, rakTeam[i]))
			{
				_DestoryMatchUnit(pkMatchUnit);
				return NULL;
			}
		}

		if (pkMatchUnit->GetPlayerNum() <= 0)
		{
			_DestoryMatchUnit(pkMatchUnit);
			return NULL;
		}

		m_akMatchPool[pkMatchUnit->GetKey()][pkMatchUnit->GetMatchUnitID()] = pkMatchUnit;
		m_akMatchUnit[pkMatchUnit->GetMatchUnitID()] = pkMatchUnit;
		return pkMatchUnit;
	}

	bool Pk3V3MatchAlgorithmBase::AddMatchTeam(UInt32 wsId, UInt32 serverCreateTime, ObjID_t iTeamIndex, int iPlayerNum, VMatchMember akMemberes)
	{
		if (iPlayerNum <= 0 || iPlayerNum > m_iFullPlayerNum)
		{
			return false;
		}

		DelMatchTeam(iTeamIndex);
		SeMatchTeam* pkTeam = _CreateMatchTeam();
		if (!pkTeam)
		{
			return false;
		}

		pkTeam->SetTeamID(iTeamIndex);
		pkTeam->SetPlayerNum(iPlayerNum);
		pkTeam->SetScore(CountMatchScore(akMemberes));
		pkTeam->SetMembers(akMemberes);
		pkTeam->SetWSID(wsId);
		pkTeam->SetServerCreateTime(serverCreateTime);

		m_akMatchWaitTeam[iTeamIndex] = pkTeam;
		return true;
	}

	bool Pk3V3MatchAlgorithmBase::DelMatchTeam(ObjID_t iTeamIndex)
	{
		SeMatchTeam* pkTeam = NULL;

		MapMatchTeam::iterator kItrTeam = m_akMatchWaitTeam.find(iTeamIndex);
		if (kItrTeam != m_akMatchWaitTeam.end())
		{
			pkTeam = kItrTeam->second;
		}

		kItrTeam = m_akMatchTeam.find(iTeamIndex);
		if (kItrTeam != m_akMatchTeam.end())
		{
			if (pkTeam)
			{
				assert(0 && "Team Pointer In Two Pool.");
			}
			pkTeam = kItrTeam->second;
		}

		if (!pkTeam)
		{
			return false;
		}

		MapMatchUnit::iterator kItrMatchUnit = m_akMatchUnit.find(pkTeam->GetBelongMatchUnitID());
		if (kItrMatchUnit == m_akMatchUnit.end())
		{
			// 找不到说明还没有放到匹配池中

			m_akMatchWaitTeam.erase(pkTeam->GetTeamID());
			m_akMatchTeam.erase(pkTeam->GetTeamID());
			_DestoryMatchTeam(pkTeam);

			return false;
		}

		SeMatchUnit* pkMatchUnit = kItrMatchUnit->second;
		if (!pkMatchUnit)
		{
			assert(0 && "MatchUnit Pointer Is Null.");
			return false;
		}

		// 如果已经匹配好，就要把原先匹配好的队伍放回到匹配池中
		VMatchResult::iterator kItrMatchResult = m_akMatchResult.begin();
		for (; kItrMatchResult != m_akMatchResult.end(); ++kItrMatchResult)
		{
			const MatchResult& rkMatchResult = *kItrMatchResult;

			bool bFind = false;

			for (UInt32 j = 0; j < rkMatchResult.size(); j++)
			{
				const SeMatchUnit* pkResultUnit = rkMatchResult[j];
				if (pkResultUnit == pkMatchUnit)
				{
					bFind = true;
					break;
				}
			}

			if (!bFind)
			{
				continue;
			}

			for (UInt32 j = 0; j < rkMatchResult.size(); j++)
			{
				SeMatchUnit* pkResultUnit = rkMatchResult[j];
				if (pkResultUnit && pkResultUnit != pkMatchUnit)
				{
					pkResultUnit->SetDirty(false);
					pkResultUnit->ClearLoop();
					m_akMatchPool[pkResultUnit->GetKey()][pkResultUnit->GetMatchUnitID()] = pkResultUnit;
				}
			}

			m_akMatchResult.erase(kItrMatchResult);
			break;
		}

		// 把这个队伍从原先匹配好的队伍中移除，再把其他的队伍放回到匹配池中
		VMatchTeam akTeams;
		GetAllMatchTeams(pkMatchUnit, akTeams);

		VMatchTeam::iterator kItr = std::find(akTeams.begin(), akTeams.end(), pkTeam);
		if (kItr == akTeams.end())
		{
			assert(0 && "Team Not In Found MatchUnit.");
		}

		akTeams.erase(kItr);
		AddMatchUnitByTeams(akTeams);

		DelMatchUnit(pkMatchUnit);
		m_akMatchWaitTeam.erase(pkTeam->GetTeamID());
		m_akMatchTeam.erase(pkTeam->GetTeamID());
		_DestoryMatchTeam(pkTeam);

		return true;
	}

	int Pk3V3MatchAlgorithmBase::GetMatchingPlayerNum()
	{
		int iNum = 0;
		MapMatchTeam::iterator kItr = m_akMatchTeam.begin();
		for (; kItr != m_akMatchTeam.end(); ++kItr)
		{
			SeMatchTeam* pkTeam = kItr->second;
			if (!pkTeam)
			{
				continue;
			}

			iNum += pkTeam->GetPlayerNum();
		}

		return iNum;
	}

	void Pk3V3MatchAlgorithmBase::FindTeamMates(int iSrcPlayerNum)
	{
		for (int iDstPlayerNum = m_iFullPlayerNum - iSrcPlayerNum; iDstPlayerNum > 0; iDstPlayerNum--)
		{
			FindTeamMates(iSrcPlayerNum, iDstPlayerNum);
		}
	}

	void Pk3V3MatchAlgorithmBase::FindTeamMates(int iSrcPlayerNum, int iDstPlayerNum)
	{
		SeMatchKey kSrcBeginKey = MakeMatchKey(iSrcPlayerNum, 0);
		MapMatchPool::iterator kItrMatchPool = m_akMatchPool.lower_bound(kSrcBeginKey);

		VMatchResult akMatchResult;
		for (; kItrMatchPool != m_akMatchPool.end(); ++kItrMatchPool)
		{
			SeMatchKey kKey = kItrMatchPool->first;
			if (kKey.first != iSrcPlayerNum)
			{
				break;
			}

			MapMatchUnit& rakMatchUnit = kItrMatchPool->second;
			for (MapMatchUnit::iterator kItrMatchUnit = rakMatchUnit.begin();
				kItrMatchUnit != rakMatchUnit.end(); ++kItrMatchUnit)
			{
				SeMatchUnit* pkMatchUnit = kItrMatchUnit->second;
				if (!pkMatchUnit || pkMatchUnit->IsDirty())
				{
					continue;
				}

				SeMatchUnit* pkFirstMatchUnit = GetFirstMatchUnit(iDstPlayerNum, pkMatchUnit);
				if (!pkFirstMatchUnit)
				{
					continue;
				}

				pkMatchUnit->SetDirty(true);
				pkFirstMatchUnit->SetDirty(true);

				MatchResult kResult;
				kResult.push_back(pkMatchUnit);
				kResult.push_back(pkFirstMatchUnit);

				akMatchResult.push_back(kResult);
			}
		}

		for (UInt32 i = 0; i < akMatchResult.size(); i++)
		{
			const MatchResult& rkResult = akMatchResult[i];
			VMatchTeam akTeams;
			for (UInt32 j = 0; j < rkResult.size(); j++)
			{
				SeMatchUnit* pkUnit = rkResult[j];
				if (!pkUnit)
				{
					continue;
				}

				GetAllMatchTeams(pkUnit, akTeams);
				DelMatchUnit(pkUnit);
			}

			AddMatchUnitByTeams(akTeams);
		}
	}

	void Pk3V3MatchAlgorithmBase::TryDismissMatchUnitsToRematch()
	{
		std::vector<SeMatchUnit*> needDissmissUnits;

		for (auto itr1 : m_akMatchPool)
		{
			for (auto itr : itr1.second)
			{
				auto matchUnit = itr.second;
				// 一个队伍就不需要解散了
				if (!matchUnit || matchUnit->GetTeamSize() == 1)
				{
					continue;
				}

				auto secs = matchUnit->GetPassSec(m_curTime);
				if (secs <= (UInt32)GetMaxMatchSec())
				{
					continue;
				}

				needDissmissUnits.push_back(matchUnit);
			}
		}
		
		// 把需要解散的队伍解散
		for (auto unit : needDissmissUnits)
		{
			if (!unit)
			{
				continue;
			}

			for (auto teamId : unit->GetAllMatchTeams())
			{
				auto team = GetMatchTeamByID(teamId);
				if (!team)
				{
					assert(0 && "can't find team");
					continue;
				}

				if (team->GetPassSec(m_curTime) >= 50 && m_curTime >= 50)
				{
					team->SetStartTime(m_curTime - 50);
				}

				_PushTeamToMatchPool(team);
				DebugLogStream << "re push team(" << team->GetTeamID() << ") to match pool, startTime(" << team->GetStartTime() << ")." << LogStream::eos;
			}

			DelMatchUnit(unit);
		}
	}

	SeMatchUnit* Pk3V3MatchAlgorithmBase::GetFirstMatchUnit(int iPlayerNum, SeMatchUnit* pkSrcUnit)
	{
		if (!pkSrcUnit)
		{
			return NULL;
		}

		std::vector<SeMatchUnit*> matchUnits;

		pkSrcUnit->IncLoop();

		int iDiff = GetMatchScoreDiff(pkSrcUnit);
		int iMinScore = pkSrcUnit->GetMatchScore() - iDiff;
		SeMatchKey kMinKey = MakeMatchKey(iPlayerNum, iMinScore);

		int iMaxScore = pkSrcUnit->GetMatchScore() + iDiff;
		SeMatchKey kMaxKey = MakeMatchKey(iPlayerNum, iMaxScore);

		MapMatchPool::iterator kItrStart = m_akMatchPool.lower_bound(kMinKey);
		MapMatchPool::iterator kItrEnd = m_akMatchPool.upper_bound(kMaxKey);

		MapMatchPool::iterator kItrMatchPool = kItrStart;
		for (; kItrMatchPool != kItrEnd; ++kItrMatchPool)
		{
			MapMatchUnit& rakMatchUnit = kItrMatchPool->second;
			for (MapMatchUnit::iterator kItrMatchUnit = rakMatchUnit.begin();
				kItrMatchUnit != rakMatchUnit.end(); ++kItrMatchUnit)
			{
				// 找到第一个满足条件的，一定要两边都符合匹配要求
				SeMatchUnit* pkDstUnit = kItrMatchUnit->second;
				if (!pkDstUnit || pkDstUnit->GetMatchUnitID() == pkSrcUnit->GetMatchUnitID() ||
					pkDstUnit->IsDirty() || !_IsMatch(pkSrcUnit, pkDstUnit))
				{
					continue;
				}

				//return pkDstUnit;
				matchUnits.push_back(pkDstUnit);
			}
		}

		struct MatchComper
		{
			MatchComper(int score_) : score(score_) {}
			bool operator()(const SeMatchUnit* a, const SeMatchUnit* b)
			{
				if (!a || !b)
				{
					return false;
				}

				int diffA = abs(a->GetMatchScore() - score);
				int diffB = abs(b->GetMatchScore() - score);
				if (diffA != diffB)
				{
					return diffA < diffB;
				}

				return a->GetLoop() > b->GetLoop();
			}
			int score;
		};
		MatchComper comper(pkSrcUnit->GetMatchScore());
		std::sort(matchUnits.begin(), matchUnits.end(), comper);

		if (!matchUnits.empty())
		{
			return matchUnits[0];
		}

		return NULL;
	}

	void Pk3V3MatchAlgorithmBase::DoMatch()
	{
		SeMatchKey kSrcBeginKey = MakeMatchKey(m_iFullPlayerNum, 0);
		MapMatchPool::iterator kItrMatchPool = m_akMatchPool.lower_bound(kSrcBeginKey);

		VMatchResult akMatchResult;
		for (; kItrMatchPool != m_akMatchPool.end(); ++kItrMatchPool)
		{
			SeMatchKey kKey = kItrMatchPool->first;
			if (kKey.first != m_iFullPlayerNum)
			{
				break;
			}

			MapMatchUnit& rakMatchUnit = kItrMatchPool->second;
			for (MapMatchUnit::iterator kItrMatchUnit = rakMatchUnit.begin();
				kItrMatchUnit != rakMatchUnit.end(); ++kItrMatchUnit)
			{
				SeMatchUnit* pkMatchUnit = kItrMatchUnit->second;
				if (!pkMatchUnit || pkMatchUnit->IsDirty())
				{
					continue;
				}

				MatchResult kResult;
				kResult.push_back(pkMatchUnit);

				// 找对手
				for (int i = 1; i < m_iCampNum; i++)
				{
					SeMatchUnit* pkFirstMatchUnit = GetFirstMatchUnit(m_iFullPlayerNum, pkMatchUnit);
					if (!pkFirstMatchUnit)
					{
						break;
					}

					kResult.push_back(pkFirstMatchUnit);
				}

				if (kResult.size() != (size_t)m_iCampNum)
				{
					continue;
				}

				for (auto& pkUnit : kResult)
				{
					if (pkUnit)
					{
						pkUnit->SetDirty(true);
					}
				}

				akMatchResult.push_back(kResult);
			}
		}

		for (UInt32 i = 0; i < akMatchResult.size(); i++)
		{
			const MatchResult& rkResult = akMatchResult[i];
			for (UInt32 j = 0; j < rkResult.size(); j++)
			{
				const SeMatchUnit* pkUnit = rkResult[j];
				if (!pkUnit)
				{
					continue;
				}

				EraseMatchUnitFromMatchPool(pkUnit);
			}

			m_akMatchResult.push_back(rkResult);
		}
	}

	bool Pk3V3MatchAlgorithmBase::EraseMatchUnitFromMatchPool(const SeMatchUnit* pkUnit)
	{
		if (!pkUnit)
		{
			return false;
		}

		SeMatchKey kKey = pkUnit->GetKey();
		MapMatchPool::iterator kItrMatchPool = m_akMatchPool.find(kKey);
		if (kItrMatchPool == m_akMatchPool.end())
		{
			//assert(0 && "Can't Find MatchUnit From MatchPool");
			//return false;
			return true;
		}

		MapMatchUnit& rakMatchUnit = kItrMatchPool->second;
		MapMatchUnit::iterator kItrMatchUnit = rakMatchUnit.find(pkUnit->GetMatchUnitID());
		if (kItrMatchUnit == rakMatchUnit.end())
		{
			//assert(0 && "Can't Find MatchUnit From MatchUnitPool");
			//return false;
			return true;
		}

		rakMatchUnit.erase(kItrMatchUnit);

		if (rakMatchUnit.empty())
		{
			m_akMatchPool.erase(kItrMatchPool);
		}

		InfoLogStream << "[3v3] erase match unit(" << pkUnit->GetMatchUnitID() << ") from pool." << LogStream::eos;
		for (auto member : pkUnit->GetMembers())
		{
			InfoLogStream << "[3v3] match unit(" << pkUnit->GetMatchUnitID() << ") player(" << member.roleId << ") erase from pool." << LogStream::eos;
		}

		return true;
	}

	SeMatchTeam* Pk3V3MatchAlgorithmBase::GetMatchTeamByID(ObjID_t iTeamID)
	{
		MapMatchTeam::iterator kItr = m_akMatchTeam.find(iTeamID);
		return kItr == m_akMatchTeam.end() ? NULL : kItr->second;
	}

	void Pk3V3MatchAlgorithmBase::DelMatchUnit(SeMatchUnit* pkUnit)
	{
		if (!pkUnit)
		{
			return;
		}

		InfoLogStream << "[3v3]delete match unit(" << pkUnit->GetMatchUnitID() << ")." << LogStream::eos;

		EraseMatchUnitFromMatchPool(pkUnit);
		m_akMatchUnit.erase(pkUnit->GetMatchUnitID());
		_DestoryMatchUnit(pkUnit);
	}

	bool Pk3V3MatchAlgorithmBase::AddMatchTeamToUnit(SeMatchUnit* pkUnit, SeMatchTeam* pkTeam)
	{
		if (!pkUnit || !pkTeam)
		{
			return false;
		}

		bool emptyUnit = pkUnit->GetTeamSize() == 0 ? true : false;
		if (!pkUnit->AddTeam(pkTeam->GetTeamID()))
		{
			return false;
		}

		pkUnit->AddMembers(pkTeam->GetMembers());
		pkTeam->SetBelongMatchUnitID(pkUnit->GetMatchUnitID());
		CountMatchScore(pkUnit);

		if (emptyUnit)
		{
			pkUnit->SetWSID(pkTeam->GetWSID());
			pkUnit->SetServerCreateTime(pkTeam->GetServerCreateTime());
		}
		else if (pkUnit->GetWSID() != pkTeam->GetWSID())
		{
			pkUnit->SetWSID(0);
			pkUnit->SetServerCreateTime(0);
		}

		std::stringstream ss;
		for (auto member : pkTeam->GetMembers())
		{
			ss << member.roleId << ", ";
		}
		InfoLogStream << "[3v3]match unit(" << pkUnit->GetMatchUnitID() << ") add team(" << pkTeam->GetTeamID() << ") member(" << ss.str() << ")." << LogStream::eos;

		return true;
	}

	void Pk3V3MatchAlgorithmBase::GetAllMatchTeams(SeMatchUnit* pkUnit, VMatchTeam& rakMatchTeam)
	{
		if (!pkUnit)
		{
			return;
		}

		const std::vector<ObjID_t>& raiTeamID = pkUnit->GetAllMatchTeams();
		for (UInt32 i = 0; i < raiTeamID.size(); i++)
		{
			SeMatchTeam* pkTeam = GetMatchTeamByID(raiTeamID[i]);
			if (!pkTeam)
			{
				continue;
			}

			rakMatchTeam.push_back(pkTeam);
		}
	}

	void Pk3V3MatchAlgorithmBase::CountMatchScore(SeMatchUnit* pkUnit)
	{
		if (!pkUnit)
		{
			return;
		}
		pkUnit->ClearMembers();

		UInt32 minStartTime = (UInt32)CURRENT_TIME.Sec();
		int iTotalPlayerNum = 0;
		VMatchTeam akTeams;
		GetAllMatchTeams(pkUnit, akTeams);
		for (UInt32 i = 0; i < akTeams.size(); i++)
		{
			const SeMatchTeam* pkTeam = akTeams[i];
			if (!pkTeam)
			{
				continue;
			}

			pkUnit->AddMembers(pkTeam->GetMembers());
			iTotalPlayerNum += pkTeam->GetPlayerNum();

			if (pkTeam->GetStartTime() < minStartTime)
			{
				minStartTime = pkTeam->GetStartTime();
			}
		}

		pkUnit->SetPlayerNum(iTotalPlayerNum);
		pkUnit->SetMatchScore(CountMatchScore(pkUnit->GetMembers()));
		pkUnit->SetLevel(CountMatchLevel(pkUnit->GetMembers()));
		CountSeasonLevel(pkUnit);
		pkUnit->SetStartTime(minStartTime);
	}

	UInt32 Pk3V3MatchAlgorithmBase::CountMatchScore(const VMatchMember& members)
	{
		std::vector<UInt32> scores;
		for (auto& member : members)
		{
			scores.push_back(member.score);
		}
		std::sort(scores.begin(), scores.end());

		if (scores.size() == 0)
		{
			return 0;
		}
		else if (scores.size() == 1)
		{
			return scores[0];
		}
		else if (scores.size() == 2)
		{
			return (scores[0] * 20 + scores[1] * 80) / 100;
		}
		else
		{
			return (scores[0] * 10 + scores[1] * 20 + scores[2] * 70) / 100;
		}

		return 0;
	}

	UInt32 Pk3V3MatchAlgorithmBase::CountMatchLevel(const VMatchMember& members)
	{
		std::vector<UInt32> levels;
		for (auto& member : members)
		{
			levels.push_back(member.level);
		}
		std::sort(levels.begin(), levels.end());

		if (levels.size() == 0)
		{
			return 0;
		}
		else if (levels.size() == 1)
		{
			return levels[0];
		}
		else if (members.size() == 2)
		{
			return (levels[0] * 20 + levels[1] * 80) / 100;
		}
		else
		{
			return (levels[0] * 5 + levels[1] * 15 + levels[2] * 80) / 100;
		}

		return 0;
	}

	void Pk3V3MatchAlgorithmBase::CountSeasonLevel(SeMatchUnit* pkUnit)
	{
		if (!pkUnit)
		{
			return;
		}

		auto members = pkUnit->GetMembers();
		bool isPromotion = false;
		std::vector<UInt32> seasonExps;
		for (auto& member : members)
		{
			if (member.isPromotion)
			{
				isPromotion = true;
			}

			// 根据当前段位计算经验值
			UInt32 totalExp = SeasonLevelDataEntryMgr::Instance()->GetSeasonLevelSumExp(member.seasonLevel);
			seasonExps.push_back(totalExp);
		}

		pkUnit->SetIsPromotion(isPromotion);

		std::sort(seasonExps.begin(), seasonExps.end());

		UInt32 totalExp = 0;
		if (seasonExps.size() == 1)
		{
			totalExp = seasonExps[0];
		}
		else if (seasonExps.size() == 2)
		{
			totalExp = (seasonExps[0] * 20 + seasonExps[1] * 80) / 100;
		}
		else
		{
			totalExp = (seasonExps[0] * 5 + seasonExps[1] * 15 + seasonExps[2] * 80) / 100;
		}

		// 根据经验值计算段位
		pkUnit->SetSeasonMainLevel(SeasonLevelDataEntryMgr::Instance()->GetSeasonMainLevelBySumExp(totalExp));
	}

	Pk3V3MatchAlgorithmBase::VMatchResult& Pk3V3MatchAlgorithmBase::GetAllMatcheResult()
	{
		return m_akMatchResult;
	}

	void Pk3V3MatchAlgorithmBase::DelAllMatchResult()
	{
		for (UInt32 i = 0; i < m_akMatchResult.size(); i++)
		{
			for (UInt32 j = 0; j < m_akMatchResult[i].size(); j++)
			{
				SeMatchUnit* pkMatchUnit = m_akMatchResult[i][j];
				if (!pkMatchUnit)
				{
					continue;
				}

				VMatchTeam akTeams;
				GetAllMatchTeams(pkMatchUnit, akTeams);

				for (UInt32 k = 0; k < akTeams.size(); k++)
				{
					SeMatchTeam* pkTeam = akTeams[k];
					if (!pkTeam)
					{
						continue;
					}

					m_akMatchTeam.erase(pkTeam->GetTeamID());
					_DestoryMatchTeam(pkTeam);
				}

				m_akMatchUnit.erase(pkMatchUnit->GetMatchUnitID());
				_DestoryMatchUnit(pkMatchUnit);
			}
		}

		m_akMatchResult.clear();
	}

	int Pk3V3MatchAlgorithmBase::GetMatchScoreDiff(SeMatchUnit* pkUnit)
	{
		if (!pkUnit)
		{
			return 0;
		}

		auto secs = pkUnit->GetPassSec(m_curTime);
		if (secs <= 5)
		{
			return 50;
		}
		else if (secs <= 30)
		{
			return 50 + (secs - 5) * 14;
		}
		else if (secs <= 50)
		{
			return 400 + (secs - 30) * 5;
		}
		else if (secs <= 150)
		{
			return 500 + (secs - 50);
		}
		else
		{
			return 600;
		}
	}

	int Pk3V3MatchAlgorithmBase::GetMaxMatchLevel(SeMatchUnit* pkUnit)
	{
		if (!pkUnit)
		{
			return 0;
		}

		int level = pkUnit->GetLevel();
		int s_levelRange[] = { 15, 20, 25, 30, 35, 40, 45, 55, 60, 65};
		int s_maxLevel[] = { 24, 29, 34, 39, 44, 49, 54, 60, 65, 65};

		for (UInt32 i = 0; i < sizeof(s_levelRange) / sizeof(s_levelRange[0]); i++)
		{
			if (s_levelRange[i] >= level)
			{
				return s_maxLevel[i];
			}
		}

		return s_maxLevel[sizeof(s_maxLevel) / sizeof(s_maxLevel[0]) - 1];
	}

	bool Pk3V3MatchAlgorithmBase::_IsMatch(SeMatchUnit* a, SeMatchUnit* b)
	{
		if (!a || !b || a == b)
		{
			return false;
		}

		if (!_IsMatchScore(a, b) || !_IsMatchLevel(a, b) || !_IsMatchSeasonLevel(a, b))
		{
			return false;
		}

		/*
			跨服匹配条件：
			1. 20秒内只匹配自己服的玩家
			2. 20-40秒内只匹配开服时间相差15天内的玩家
			3. 40秒后匹配所有服的玩家
		*/
		if(a->GetWSID() != 0 || b->GetWSID() != 0)
		{
			// 跨服匹配
			UInt32 sec1 = (UInt32)CURRENT_TIME.Sec() - (UInt32)a->GetStartTime();
			UInt32 sec2 = (UInt32)CURRENT_TIME.Sec() - (UInt32)b->GetStartTime();
			if (sec1 <= 20 || sec2 <= 20)
			{
				if (a->GetWSID() != b->GetWSID())
				{
					return false;
				}
			}
			else if (sec1 <= 40 || sec2 <= 40)
			{
				UInt32 diffTime = DIFF_TIME(a->GetServerCreateTime(), b->GetServerCreateTime());
				UInt32 maxDiffTime = 15 * Avalon::Time::SECS_OF_DAY;
				if (diffTime >= maxDiffTime)
				{
					return false;
				}
			}
		}
		return true;
	}

	bool Pk3V3MatchAlgorithmBase::_IsMatchScore(SeMatchUnit* a, SeMatchUnit* b)
	{
		if (!a || !b || a == b)
		{
			return false;
		}

		auto scoceA = a->GetMatchScore();
		auto diffA = GetMatchScoreDiff(a);
		auto scoreB = b->GetMatchScore();
		auto diffB = GetMatchScoreDiff(b);

		// A在B的积分范围内
		if (scoceA < scoreB - diffB || scoceA > scoreB + diffB)
		{
			return false;
		}

		// B在A的积分范围内
		if (scoreB < scoceA - diffA || scoreB > scoceA + diffA)
		{
			return false;
		}

		return true;
	}

	bool Pk3V3MatchAlgorithmBase::_IsMatchLevel(SeMatchUnit* a, SeMatchUnit* b)
	{
		if (!a || !b || a == b)
		{
			return false;
		}

		auto levelA = a->GetLevel();
		auto maxLevelA = GetMaxMatchLevel(a);
		auto levelB = b->GetLevel();
		auto maxLevelB = GetMaxMatchLevel(b);

		// A在B的等级范围内
		if (levelA > maxLevelB)
		{
			return false;
		}

		// B在A的等级范围内
		if (levelB > maxLevelA)
		{
			return false;
		}

		return true;
	}

	bool Pk3V3MatchAlgorithmBase::_IsMatchSeasonLevel(SeMatchUnit* a, SeMatchUnit* b)
	{
		// 段位限制
		{
			// 对方段位是否在自己的可匹配范围内
			SeasonMainLevel minMainLevel, maxMainLevel;
			_GetMatchLevelRange(a, minMainLevel, maxMainLevel);
			if (minMainLevel > b->GetSeasonMainLevel() || maxMainLevel < b->GetSeasonMainLevel())
			{
				return false;
			}
		}

		{
			// 自己段位是否在对方的可匹配范围内
			SeasonMainLevel minMainLevel, maxMainLevel;
			_GetMatchLevelRange(b, minMainLevel, maxMainLevel);
			if (minMainLevel > a->GetSeasonMainLevel() || maxMainLevel < a->GetSeasonMainLevel())
			{
				return false;
			}
		}

		return true;
	}

	void Pk3V3MatchAlgorithmBase::_GetMatchLevelRange(SeMatchUnit* unit, SeasonMainLevel& minMainLevel, SeasonMainLevel& maxMainLevel)
	{
		minMainLevel = maxMainLevel = (SeasonMainLevel)0;
		if (!unit)
		{
			return;
		}

		if (!unit->IsPromotion())
		{
			// 正常匹配
			if (unit->GetSeasonMainLevel() == SML_BRONZE)
			{
				minMainLevel = SML_BRONZE;
				maxMainLevel = SML_SILVER;
				return;
			}
			else if (unit->GetSeasonMainLevel() == SML_SILVER)
			{
				minMainLevel = SML_BRONZE;
				maxMainLevel = SML_GOLD;
				return;
			}
			else if (unit->GetSeasonMainLevel() == SML_GOLD)
			{
				minMainLevel = SML_SILVER;
				maxMainLevel = SML_DIAMOND;
				return;
			}
			else
			{
				minMainLevel = SML_GOLD;
				maxMainLevel = SML_KING;
				return;
			}
		}

		/*
		升段战
		30秒内只能匹高一段位的，30秒后可以匹同段位或者高一段位
		*/
		UInt32 sec = unit->GetPassSec(m_curTime);
		if (sec <= 30)
		{
			minMainLevel = maxMainLevel = (SeasonMainLevel)(unit->GetSeasonMainLevel() + 1);
			return;
		}
		else
		{
			minMainLevel = (SeasonMainLevel)unit->GetSeasonMainLevel();
			maxMainLevel = (SeasonMainLevel)(unit->GetSeasonMainLevel() + 1);
			return;
		}

		return;
	}

	// namespace end
};
