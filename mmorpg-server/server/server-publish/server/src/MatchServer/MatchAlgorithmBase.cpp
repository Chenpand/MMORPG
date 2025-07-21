#include "MatchAlgorithmBase.h"

// namespace start
namespace Match {

SeMatchAlorithm::SeMatchAlorithm()
{
	m_iMatchUnitID = 0;
	m_iFullPlayerNum = 0;
	m_iStep = 0;
}

SeMatchAlorithm::~SeMatchAlorithm()
{
	
}

bool SeMatchAlorithm::Initialize(int iFullPlayerNum, int iCampNum, int iBaseMatchScoreDiff, int iMaxMatchScoreDiff, int iMatchScoreExpansionMinLoop,
                                    int iMatchScoreExpansionScore, int iMatchScoreExpansionInterval)


{
	if(iFullPlayerNum <= 0)
	{
		return false;
	}

	m_iFullPlayerNum = iFullPlayerNum;
	m_iCampNum = iCampNum;

	m_kFindTeammateTimer.SetInterValue(500);
	m_kMatchTimer.SetInterValue(1000);

    m_kMatchWaitTimer.SetInterValue(20 * 1000);

    m_iBaseMatchScoreDiff = iBaseMatchScoreDiff;
    m_iMaxMatchScoreDiff = iMaxMatchScoreDiff;
    m_iMatchScoreExpansionMinLoop = iMatchScoreExpansionMinLoop;
    m_iMatchScoreExpansionScore = iMatchScoreExpansionScore;
    m_iMatchScoreExpansionInterval = iMatchScoreExpansionInterval;

	return true;
}

void SeMatchAlorithm::Update(UInt64 dwTime)
{
	if(m_iFullPlayerNum <= 0)
	{
		return;
	}

    m_kMatchWaitTimer.Update(dwTime);
	m_kFindTeammateTimer.Update(dwTime);
	m_kMatchTimer.Update(dwTime);
    
    //if(m_kMatchWaitTimer.IsPassed())
    {
        _PushAllTeamToMatchPool();
    }

	if(m_kFindTeammateTimer.IsPassed())
	{
		if(m_iFullPlayerNum > 1)
		{
			FindTeamMates(m_iStep + 1);
			m_iStep = (m_iStep + 1) % (m_iFullPlayerNum - 1);
		}
	}

	if(m_kMatchTimer.IsPassed())
	{
		DoMatch();
	}
}

const ObjID_t SeMatchAlorithm::GenID()
{
	return ++m_iMatchUnitID;
}

SeMatchKey SeMatchAlorithm::MakeMatchKey(int iPlayerNum,int iScore)
{
	return std::make_pair(iPlayerNum, iScore);
}

void SeMatchAlorithm::_PushAllTeamToMatchPool()
{
    std::vector<SeMatchTeam*> apkTeam;
    for(MapMatchTeam::iterator kItr = m_akMatchWaitTeam.begin(); kItr != m_akMatchWaitTeam.end(); ++kItr)
    {
        apkTeam.push_back(kItr->second);
    }

    m_akMatchWaitTeam.clear();

    // 打乱队伍顺序，把所有等待中的队伍放到匹配池中
    // 随机排列数组
    std::random_shuffle(apkTeam.begin(),apkTeam.end());
    for(UInt32 i = 0; i < apkTeam.size(); i++)
    {
        SeMatchTeam* pkTeam = apkTeam[i];
        _PushTeamToMatchPool(pkTeam);
    }
}

bool SeMatchAlorithm::_PushTeamToMatchPool(SeMatchTeam* pkTeam)
{
    if(!pkTeam)
    {
        return false;
    }
    ObjID_t iTeamIndex = pkTeam->GetTeamID();

    VMatchTeam akTeams;
    akTeams.push_back(pkTeam);

    m_akMatchTeam[iTeamIndex] = pkTeam;
    if(!AddMatchUnitByTeams(akTeams))
    {
        m_akMatchTeam.erase(iTeamIndex);
        _DestoryMatchTeam(pkTeam);
        return false;
    }

    
    return true;
}

const SeMatchUnit* SeMatchAlorithm::AddMatchUnitByTeams(VMatchTeam& rakTeam)
{
	if(rakTeam.empty())
	{
		return NULL;
	}

	SeMatchUnit* pkMatchUnit = _CreateMatchUnit();
	if(!pkMatchUnit)
	{
		return NULL;
	}

	// 分配ID
	pkMatchUnit->SetMatchUnitID(GenID());

	for(UInt32 i = 0; i < rakTeam.size(); i++)
	{
		if(!AddMatchTeamToUnit(pkMatchUnit,rakTeam[i]))
		{
			_DestoryMatchUnit(pkMatchUnit);
			return NULL;
		}
	}

	if(pkMatchUnit->GetPlayerNum() <= 0)
	{
		_DestoryMatchUnit(pkMatchUnit);
		return NULL;
	}

	m_akMatchPool[pkMatchUnit->GetKey()][pkMatchUnit->GetMatchUnitID()] = pkMatchUnit;
	m_akMatchUnit[pkMatchUnit->GetMatchUnitID()] = pkMatchUnit;
	return pkMatchUnit;
}

bool SeMatchAlorithm::AddMatchTeam(ObjID_t iTeamIndex, int iPlayerNum, int iScore, std::vector<ObjID_t> akMemberes, std::vector<ObjID_t> akBlackList)
{
	if(iPlayerNum <= 0 || iPlayerNum > m_iFullPlayerNum)
	{
		return false;
	}

	DelMatchTeam(iTeamIndex);
	SeMatchTeam* pkTeam = _CreateMatchTeam();
	if(!pkTeam)
	{
		return false;
	}

	pkTeam->SetTeamID(iTeamIndex);
	pkTeam->SetPlayerNum(iPlayerNum);
	pkTeam->SetScore(iScore);
	pkTeam->SetMembers(akMemberes);
	pkTeam->SetBlackList(akBlackList);
	
    m_akMatchWaitTeam[iTeamIndex] = pkTeam;
	return true;
}

bool SeMatchAlorithm::DelMatchTeam(ObjID_t iTeamIndex)
{
	SeMatchTeam* pkTeam = NULL;

    MapMatchTeam::iterator kItrTeam = m_akMatchWaitTeam.find(iTeamIndex);
    if(kItrTeam != m_akMatchWaitTeam.end())
    {
        pkTeam = kItrTeam->second;
    }

    kItrTeam = m_akMatchTeam.find(iTeamIndex);
    if(kItrTeam != m_akMatchTeam.end())
    {
        if(pkTeam)
        {
            assert(0 && "Team Pointer In Two Pool.");
        }
        pkTeam = kItrTeam->second;
    }

	if(!pkTeam)
	{
		return false;
	}

	MapMatchUnit::iterator kItrMatchUnit = m_akMatchUnit.find(pkTeam->GetBelongMatchUnitID());
	if(kItrMatchUnit == m_akMatchUnit.end())
	{
        // 找不到说明还没有放到匹配池中

        m_akMatchWaitTeam.erase(pkTeam->GetTeamID());
        m_akMatchTeam.erase(pkTeam->GetTeamID());
        _DestoryMatchTeam(pkTeam);

		return false;
	}

	SeMatchUnit* pkMatchUnit = kItrMatchUnit->second;
	if(!pkMatchUnit)
	{
		assert(0 && "MatchUnit Pointer Is Null.");
		return false;
	}

	// 如果已经匹配好，就要把原先匹配好的队伍放回到匹配池中
	VMatchResult::iterator kItrMatchResult = m_akMatchResult.begin();
	for(; kItrMatchResult != m_akMatchResult.end(); ++kItrMatchResult)
	{
		const MatchResult& rkMatchResult = *kItrMatchResult;

		bool bFind = false;

		for(UInt32 j = 0; j < rkMatchResult.size(); j++)
		{
			const SeMatchUnit* pkResultUnit = rkMatchResult[j];
			if(pkResultUnit == pkMatchUnit)
			{
				bFind = true;
				break;
			}
		}

		if(!bFind)
		{
			continue;
		}

		for(UInt32 j = 0; j < rkMatchResult.size(); j++)
		{
			SeMatchUnit* pkResultUnit = rkMatchResult[j];
			if(pkResultUnit && pkResultUnit != pkMatchUnit)
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
	GetAllMatchTeams(pkMatchUnit,akTeams);

	VMatchTeam::iterator kItr = std::find(akTeams.begin(),akTeams.end(),pkTeam);
	if(kItr == akTeams.end())
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

int SeMatchAlorithm::GetMatchingPlayerNum()
{
	int iNum = 0;
	MapMatchTeam::iterator kItr = m_akMatchTeam.begin();
	for(; kItr != m_akMatchTeam.end(); ++kItr)
	{
		SeMatchTeam* pkTeam = kItr->second;
		if(!pkTeam)
		{
			continue;
		}

		iNum += pkTeam->GetPlayerNum();
	}

	return iNum;
}

void SeMatchAlorithm::FindTeamMates(int iSrcPlayerNum)
{
	for(int iDstPlayerNum = m_iFullPlayerNum - iSrcPlayerNum; iDstPlayerNum > 0; iDstPlayerNum--)
	{
		FindTeamMates(iSrcPlayerNum,iDstPlayerNum);
	}
}

void SeMatchAlorithm::FindTeamMates(int iSrcPlayerNum,int iDstPlayerNum)
{
	SeMatchKey kSrcBeginKey = MakeMatchKey(iSrcPlayerNum,0);
	MapMatchPool::iterator kItrMatchPool = m_akMatchPool.lower_bound(kSrcBeginKey);

	VMatchResult akMatchResult;
	for(; kItrMatchPool != m_akMatchPool.end(); ++kItrMatchPool)
	{
		SeMatchKey kKey = kItrMatchPool->first;
		if(kKey.first != iSrcPlayerNum)
		{
			break;
		}

		MapMatchUnit& rakMatchUnit = kItrMatchPool->second;
		for(MapMatchUnit::iterator kItrMatchUnit = rakMatchUnit.begin();
			kItrMatchUnit != rakMatchUnit.end(); ++kItrMatchUnit)
		{
			SeMatchUnit* pkMatchUnit = kItrMatchUnit->second;
			if(!pkMatchUnit || pkMatchUnit->IsDirty())
			{
				continue;
			}
			
			SeMatchUnit* pkFirstMatchUnit = GetFirstMatchUnit(iDstPlayerNum,pkMatchUnit);
			if(!pkFirstMatchUnit)
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

	for(UInt32 i = 0; i < akMatchResult.size(); i++)
	{
		const MatchResult& rkResult = akMatchResult[i];
		VMatchTeam akTeams;
		for(UInt32 j = 0; j < rkResult.size(); j++)
		{
			SeMatchUnit* pkUnit = rkResult[j];
			if(!pkUnit)
			{
				continue;
			}

			GetAllMatchTeams(pkUnit,akTeams);
			DelMatchUnit(pkUnit);
		}

		AddMatchUnitByTeams(akTeams);
	}
}

SeMatchUnit* SeMatchAlorithm::GetFirstMatchUnit(int iPlayerNum,SeMatchUnit* pkSrcUnit)
{
	if(!pkSrcUnit)
	{
		return NULL;
	}

	pkSrcUnit->IncLoop();

    int iDiff = GetMatchScoreDiff(pkSrcUnit);
	int iMinScore = pkSrcUnit->GetMatchScore() - iDiff;
	SeMatchKey kMinKey = MakeMatchKey(iPlayerNum,iMinScore);

	int iMaxScore = pkSrcUnit->GetMatchScore() + iDiff;
	SeMatchKey kMaxKey = MakeMatchKey(iPlayerNum,iMaxScore);

	MapMatchPool::iterator kItrStart = m_akMatchPool.lower_bound(kMinKey);
	MapMatchPool::iterator kItrEnd = m_akMatchPool.upper_bound(kMaxKey);

	MapMatchPool::iterator kItrMatchPool = kItrStart;
	for(; kItrMatchPool != kItrEnd; ++kItrMatchPool)
	{
		MapMatchUnit& rakMatchUnit = kItrMatchPool->second;
		for(MapMatchUnit::iterator kItrMatchUnit = rakMatchUnit.begin();
			kItrMatchUnit != rakMatchUnit.end(); ++kItrMatchUnit)
		{
			// 找到第一个满足条件的，一定要两边都符合匹配要求
			SeMatchUnit* pkDstUnit = kItrMatchUnit->second;
			if(!pkDstUnit || pkDstUnit->GetMatchUnitID() == pkSrcUnit->GetMatchUnitID() || 
				pkDstUnit->IsDirty() || !pkDstUnit->IsMatch(pkSrcUnit, GetMatchScoreDiff(pkDstUnit)) ||
				pkSrcUnit->IsInBlackList(pkDstUnit))
			{
				continue;
			}

			return pkDstUnit;
		}
	}

	return NULL;
}

void SeMatchAlorithm::DoMatch()
{
	SeMatchKey kSrcBeginKey = MakeMatchKey(m_iFullPlayerNum,0);
	MapMatchPool::iterator kItrMatchPool = m_akMatchPool.lower_bound(kSrcBeginKey);

	VMatchResult akMatchResult;
	for(; kItrMatchPool != m_akMatchPool.end(); ++kItrMatchPool)
	{
		SeMatchKey kKey = kItrMatchPool->first;
		if(kKey.first != m_iFullPlayerNum)
		{
			break;
		}

		MapMatchUnit& rakMatchUnit = kItrMatchPool->second;
		for(MapMatchUnit::iterator kItrMatchUnit = rakMatchUnit.begin();
			kItrMatchUnit != rakMatchUnit.end(); ++kItrMatchUnit)
		{
			SeMatchUnit* pkMatchUnit = kItrMatchUnit->second;
			if(!pkMatchUnit || pkMatchUnit->IsDirty())
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

	for(UInt32 i = 0; i < akMatchResult.size(); i++)
	{
		const MatchResult& rkResult = akMatchResult[i];
		for(UInt32 j = 0; j < rkResult.size(); j++)
		{
			const SeMatchUnit* pkUnit = rkResult[j];
			if(!pkUnit)
			{
				continue;
			}
            
			EraseMatchUnitFromMatchPool(pkUnit);
		}

		m_akMatchResult.push_back(rkResult);
	}
}

bool SeMatchAlorithm::EraseMatchUnitFromMatchPool(const SeMatchUnit* pkUnit)
{
	if(!pkUnit)
	{
		return false;
	}

	SeMatchKey kKey = pkUnit->GetKey();
	MapMatchPool::iterator kItrMatchPool = m_akMatchPool.find(kKey);
	if(kItrMatchPool == m_akMatchPool.end())
	{
		//assert(0 && "Can't Find MatchUnit From MatchPool");
		//return false;
		return true;
	}

	MapMatchUnit& rakMatchUnit = kItrMatchPool->second;
	MapMatchUnit::iterator kItrMatchUnit = rakMatchUnit.find(pkUnit->GetMatchUnitID());
	if(kItrMatchUnit == rakMatchUnit.end())
	{
		//assert(0 && "Can't Find MatchUnit From MatchUnitPool");
		//return false;
		return true;
	}

	rakMatchUnit.erase(kItrMatchUnit);

	if(rakMatchUnit.empty())
	{
		m_akMatchPool.erase(kItrMatchPool);
	}

	return true;
}

SeMatchTeam* SeMatchAlorithm::GetMatchTeamByID(ObjID_t iTeamID)
{
	MapMatchTeam::iterator kItr = m_akMatchTeam.find(iTeamID);
	return kItr == m_akMatchTeam.end() ? NULL : kItr->second;
}

void SeMatchAlorithm::DelMatchUnit(SeMatchUnit* pkUnit)
{
	if(!pkUnit)
	{
		return;
	}

	EraseMatchUnitFromMatchPool(pkUnit);
	m_akMatchUnit.erase(pkUnit->GetMatchUnitID());
	_DestoryMatchUnit(pkUnit);
}

bool SeMatchAlorithm::AddMatchTeamToUnit(SeMatchUnit* pkUnit,SeMatchTeam* pkTeam)
{
	if(!pkUnit || !pkTeam)
	{
		return false;
	}

	if(!pkUnit->AddTeam(pkTeam->GetTeamID()))
	{
		return false;
	}

	pkUnit->AddMembers(pkTeam->GetMembers());
	pkUnit->AddBlackList(pkTeam->GetBlackList());
	pkTeam->SetBelongMatchUnitID(pkUnit->GetMatchUnitID());
	CountMatchScore(pkUnit);
	return true;
}

void SeMatchAlorithm::GetAllMatchTeams(SeMatchUnit* pkUnit,VMatchTeam& rakMatchTeam)
{
	if(!pkUnit)
	{
		return;
	}

	const std::vector<ObjID_t>& raiTeamID = pkUnit->GetAllMatchTeams();
	for(UInt32 i = 0; i < raiTeamID.size(); i++)
	{
		SeMatchTeam* pkTeam = GetMatchTeamByID(raiTeamID[i]);
		if(!pkTeam)
		{
			continue;
		}

		rakMatchTeam.push_back(pkTeam);
	}
}

void SeMatchAlorithm::CountMatchScore(SeMatchUnit* pkUnit)
{
	if(!pkUnit)
	{
		return;
	}

	int iMatchScore = 0;
	int iPlayerNum = 0;

	int iTotalScore = 0;
	int iTotalPlayerNum = 0;
	VMatchTeam akTeams;
	GetAllMatchTeams(pkUnit,akTeams);
	for(UInt32 i = 0; i < akTeams.size(); i++)
	{
		const SeMatchTeam* pkTeam = akTeams[i];
		if(!pkTeam)
		{
			continue;
		}

		iTotalPlayerNum += pkTeam->GetPlayerNum();
		iTotalScore += pkTeam->GetScore() * pkTeam->GetPlayerNum();
	}

	iPlayerNum = iTotalPlayerNum;
	if(iTotalPlayerNum > 0)
	{
		iMatchScore = iTotalScore / iTotalPlayerNum;
	}

	pkUnit->SetPlayerNum(iPlayerNum);
	pkUnit->SetMatchScore(iMatchScore);
}

SeMatchAlorithm::VMatchResult& SeMatchAlorithm::GetAllMatcheResult()
{
	return m_akMatchResult;
}

void SeMatchAlorithm::DelAllMatchResult()
{
	for(UInt32 i = 0; i < m_akMatchResult.size(); i++)
	{
		for(UInt32 j = 0; j < m_akMatchResult[i].size(); j++)
		{
			SeMatchUnit* pkMatchUnit = m_akMatchResult[i][j];
			if(!pkMatchUnit)
			{
				continue;
			}

			VMatchTeam akTeams;
			GetAllMatchTeams(pkMatchUnit,akTeams);

			for(UInt32 k = 0; k < akTeams.size(); k++)
			{
				SeMatchTeam* pkTeam = akTeams[k];
				if(!pkTeam)
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

int SeMatchAlorithm::GetMatchScoreDiff(SeMatchUnit* pkUnit)
{
    if (!pkUnit)
    {
        return 0;
    }

    if (m_iMatchScoreExpansionScore == 0 || m_iMatchScoreExpansionInterval == 0)
    {
        return m_iBaseMatchScoreDiff;
    }

    int iDiff = m_iBaseMatchScoreDiff;
    if (pkUnit->GetLoop() > m_iMatchScoreExpansionMinLoop)
    {
        iDiff += (pkUnit->GetLoop() - m_iMatchScoreExpansionMinLoop) / m_iMatchScoreExpansionInterval * m_iMatchScoreExpansionScore;
    }
    iDiff = iDiff > m_iMaxMatchScoreDiff ? m_iMaxMatchScoreDiff : iDiff;

    return iDiff;
}

// namespace end
};
