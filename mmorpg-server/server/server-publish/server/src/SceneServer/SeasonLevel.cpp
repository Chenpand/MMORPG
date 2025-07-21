#include "SeasonLevel.h"
#include "Player.h"
#include "SeasonMgr.h"
#include <CLSortListProtocol.h>
#include "SSRouter.h"
#include "SortListMgr.h"
#include <CLSystemValueDataEntry.h>
#include <CLFunctionDataEntry.h>


UInt32 SeasonUplevelBattle::GetBattleWinSize()
{
	return std::count(m_BattleRecord.begin(), m_BattleRecord.end(), _resultBitmask::resultBitmask_win);
}

UInt32 SeasonUplevelBattle::GetBattleLoseSize()
{
	return GetBattleRecordSize() - GetBattleWinSize();
}

UInt32 SeasonUplevelBattle::GetBattleRecordSize()
{
	return m_BattleRecord.size();
}

void SeasonUplevelBattle::IncBattleRecord(_resultBitmask result)
{
	m_BattleRecord.push_back(result);
	SetDirty();
}

void SeasonUplevelBattle::Clear()
{
	m_BattleRecord.clear();
	SetDirty();
}

void SeasonUplevelBattle::Input(Avalon::NetInputStream& stream)
{
	Clear();
	UInt8 record = 0;
	stream & record;
	while (record != 0)
	{
		IncBattleRecord((_resultBitmask)record);
		stream & record;
	}
}
void SeasonUplevelBattle::Output(Avalon::NetOutputStream& stream)
{
	for (UInt32 i = 0; i < m_BattleRecord.size(); ++i)
	{
		stream & m_BattleRecord[i];
	}
	stream & UInt8(0);
}

void SeasonUplevelBattle::Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty)
{
	for (UInt32 i = 0; i < m_BattleRecord.size(); ++i)
	{
		stream & m_BattleRecord[i];
	}
	stream & UInt8(0);
}
void SeasonUplevelBattle::Decode(Avalon::NetInputStream& stream)
{
	Clear();
	UInt8 record = 0;
	stream & record;
	while (record != 0)
	{
		IncBattleRecord((_resultBitmask)record);
		stream & record;
	}
}

void SeasonUplevelBattle::DecodeString(std::istringstream& is)
{
	UInt32 record = 0;
	char split = 0;
	while ((is >> record) && record != 0)
	{
		is >> split;
		IncBattleRecord((_resultBitmask)record);
	}
}

void SeasonUplevelBattle::EncodeString(std::ostringstream& os)
{
	for (UInt32 i = 0; i < m_BattleRecord.size(); ++i)
	{
		UInt32 record = m_BattleRecord[i];
		os << record << ";";
	}
}

void SeasonLevel::Init()
{
}

void SeasonLevel::ResetSeasonPlayStatus()
{
	m_SeasonPlayStatus = SeasonPlayStatus::SPS_INVALID;
}

void SeasonLevel::OnOnline()
{
	if (m_SeasonLevel == 0)
	{
		SeasonLevelDataEntry* minDataEntry = SeasonLevelDataEntryMgr::Instance()->GetMinSeasonLevel();
		if (minDataEntry == NULL) return;
		m_SeasonLevelDataEntry = SeasonLevelDataEntryMgr::Instance()->GetSeasonLevel(GetOwner()->GetMatchScore());
		if (m_SeasonLevelDataEntry == NULL) m_SeasonLevelDataEntry = minDataEntry;

		m_SeasonLevel = m_SeasonLevelDataEntry->id;
		m_SeasonStar = 0;

		m_SeasonId = SeasonMgr::Instance()->GetSeasonId();
		m_SeasonChangeTime = (UInt32)Avalon::Time().CurrentTime().Sec();
		
		//是否刚出生
		if (GetOwner()->GetSavebackTime() > 0)
		{
			m_SeasonPlayStatus = SeasonPlayStatus::SPS_NEW;
		}
		else
		{
			m_SeasonPlayStatus = SeasonPlayStatus::SPS_INVALID;
		}
		_SetMaxSeasonLevel();
		_UpdateSortList();
	}
	else
	{
		m_SeasonLevelDataEntry = SeasonLevelDataEntryMgr::Instance()->FindEntry(m_SeasonLevel);
		if (m_SeasonLevelDataEntry == NULL)
		{
			ErrorLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName()
				<< ")init season level(" << m_SeasonLevel << ") not find" << LogStream::eos;
		}
	}

	UInt32 nowSeasonId = SeasonMgr::Instance()->GetSeasonId();

	if (nowSeasonId == (UInt32)m_SeasonId)
	{
		UInt32 intervalDay = GetOwner()->GetIntervalDay((UInt64)m_SeasonChangeTime * Avalon::Time::MSECS_OF_SEC, Avalon::Time::CurrentTime().MSec(), 6);
		DayDownLevel(intervalDay);
	}

	/*
	if (nowSeasonId > (UInt32)m_SeasonId)
	{
		UpdateSeason(nowSeasonId);
	}
	else if (nowSeasonId == (UInt32)m_SeasonId)
	{
		UInt32 intervalDay = GetOwner()->GetIntervalDay((UInt64)m_SeasonChangeTime * Avalon::Time::MSECS_OF_SEC, Avalon::Time::CurrentTime().MSec(), 6);
		DayDownLevel(intervalDay);
	}
	else
	{
		ErrorLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName()
			<< ")OnOnline season backup (" << nowSeasonId << "|" << m_SeasonId << ") not find" << LogStream::eos;
	}
	*/

	UInt32 nextSeasonChangeAttrTime = SeasonMgr::Instance()->GetSeasonAttrStartTime();
	if (m_ChangeSeasonAttrTime < nextSeasonChangeAttrTime)
	{
		//设置赛季属性BUFF
		SetSeasonAttr(nextSeasonChangeAttrTime);
	}

	if (nowSeasonId >(UInt32)m_SeasonId)
	{
		UpdateSeason(nowSeasonId);
	}

	_SendSeasonInfo();
}

void SeasonLevel::OnTick(Avalon::Time now)
{
	UInt32 nextSeasonChangeAttrTime = SeasonMgr::Instance()->GetSeasonAttrStartTime();
	bool isSeasonAttr = false;
	if (m_ChangeSeasonAttrTime < nextSeasonChangeAttrTime)
	{
		//设置赛季属性BUFF
		SetSeasonAttr(nextSeasonChangeAttrTime);
		isSeasonAttr = true;
	}

	bool isSeasonId = false;
	UInt32 nowSeasonId = SeasonMgr::Instance()->GetSeasonId();
	if (nowSeasonId > (UInt32)m_SeasonId)
	{
		UpdateSeason(nowSeasonId);
		isSeasonId = true;
	}

	if (isSeasonAttr || isSeasonId)
	{
		_SendSeasonInfo();
	}
}

void SeasonLevel::OnDayChange()
{
	UInt64 now = Avalon::Time::CurrentTime().MSec();
	UInt32 intervalDay = GetOwner()->GetIntervalDay((UInt64)m_SeasonChangeTime * Avalon::Time::MSECS_OF_SEC, now, 6);
	DayDownLevel(intervalDay);
}

void SeasonLevel::OnRaceEnd(const CLProtocol::MatchServerRaceEnd& raceEnd, Int32& changeSeasonExp)
{
	if (_CheckSeasonUplevel((_resultBitmask)raceEnd.result.resultFlag, changeSeasonExp))
	{
		return;
	}

	if (raceEnd.result.resultFlag & _resultBitmask::resultBitmask_win)
	{
		UInt32 exp = CalculationSeasonExp(m_SeasonLevel, raceEnd.opponent.seasonLevel);
		_IncBattleCount();
		_IncSeasonWinStreak();
		
		//连胜奖励
		UInt32 winStreak = _GetSeasonWinStreak();
		UInt32 needWinStreak = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_SEASON_WIN_STREAK_COUNT);
		if (needWinStreak != 0 && winStreak != 0 && ((winStreak % needWinStreak) == 0))
		{
			if (m_SeasonLevelDataEntry != NULL && m_SeasonLevelDataEntry->IsUplevelBattle())
			{
				DebugLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName()
					<< ") Promotion lost resultFlag(" << raceEnd.result.resultFlag << ")" << LogStream::eos;
				return;
			}
			exp += SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_SEASON_WIN_STREAK_EXP);
			_ResetSeasonWinStreak();
		}
		changeSeasonExp = changeSeasonExp + exp;
		IncExp(exp);
	}
	else if (raceEnd.result.resultFlag & _resultBitmask::resultBitmask_lose)
	{
		if (m_SeasonLevelDataEntry->isDownLevel)
		{
			UInt32 exp = CalculationSeasonExp(raceEnd.opponent.seasonLevel, m_SeasonLevel);
			changeSeasonExp = changeSeasonExp - exp;
			DecExp(exp);
		}
		_IncBattleCount();
		_ResetSeasonWinStreak();
	}
	else
	{
		ErrorLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName()
			<< ") OnRaceEnd resultFlag(" << raceEnd.result.resultFlag << ")" << LogStream::eos;
		return;
	}
}

void SeasonLevel::OnRoomRaceEnd(const Pk3V3PlayerRaceEndInfo& playerInfo, Int32& changeSeasonExp)
{
	if (_CheckSeasonUplevel((_resultBitmask)playerInfo.info.resultFlag, changeSeasonExp))
	{
		return;
	}

	if (playerInfo.info.resultFlag & _resultBitmask::resultBitmask_win)
	{
		_IncBattleCount();
		_IncSeasonWinStreak();

		//连胜奖励
		UInt32 winStreak = _GetSeasonWinStreak();
		UInt32 needWinStreak = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_SEASON_WIN_STREAK_COUNT);
		if (needWinStreak != 0 && winStreak != 0 && ((winStreak % needWinStreak) == 0))
		{
			if (m_SeasonLevelDataEntry != NULL && m_SeasonLevelDataEntry->IsUplevelBattle())
			{
				DebugLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName()
					<< ") Promotion lost resultFlag(" << playerInfo.info.resultFlag << ")" << LogStream::eos;
				return;
			}
			changeSeasonExp += SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_SEASON_WIN_STREAK_EXP);
			_ResetSeasonWinStreak();
		}
		IncExp(changeSeasonExp);
	}
	else if (playerInfo.info.resultFlag & _resultBitmask::resultBitmask_lose)
	{
		if (m_SeasonLevelDataEntry && m_SeasonLevelDataEntry->isDownLevel)
		{
			DecExp(changeSeasonExp);
			changeSeasonExp = changeSeasonExp * -1;
		}
		else
		{
			changeSeasonExp = 0;
		}
		_IncBattleCount();
		_ResetSeasonWinStreak();
	}
	else
	{
		ErrorLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName()
			<< ") OnRaceEnd resultFlag(" << playerInfo.info.resultFlag << ")" << LogStream::eos;
		return;
	}
}


void SeasonLevel::IncExp(UInt32 exp, bool isUplevelBattle)
{
	if (m_SeasonLevelDataEntry == NULL)
	{
		ErrorLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName()
			<< ") inc Exp season level(" << m_SeasonLevel << ") not find" << LogStream::eos;
		return;
	}

	UInt32 oldSeasonExp = m_SeasonExp;
	UInt32 oldSeasonLevel = m_SeasonLevel;
	UInt32 oldStar = m_SeasonStar;

	m_SeasonExp = m_SeasonExp + exp;
	while(m_SeasonExp >= m_SeasonLevelDataEntry->maxExp)
	{
		if (!isUplevelBattle && m_SeasonLevelDataEntry->IsUplevelBattle()) break;
		m_SeasonExp = m_SeasonExp - m_SeasonLevelDataEntry->maxExp;
		UpLevel();
	}

	DebugLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName()
		<< ") inc exp season level(" << oldSeasonLevel << " -> " << m_SeasonLevel << ") star(" << oldStar << "->"
		<< m_SeasonStar << ") exp(" << oldSeasonExp << " -> " << m_SeasonExp << "|" << exp << ")" << LogStream::eos;
}

void SeasonLevel::DecExp(UInt32 exp)
{
	if (m_SeasonLevelDataEntry == NULL)
	{
		ErrorLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName()
			<< ") dec Exp season level(" << m_SeasonLevel << ") not find" << LogStream::eos;
		return;
	}

	UInt32 oldSeasonExp = m_SeasonExp;
	UInt32 oldSeasonLevel = m_SeasonLevel;
	UInt32 oldStar = m_SeasonStar;

	while (exp > 0)
	{
		if (m_SeasonExp >= exp)
		{
			m_SeasonExp = m_SeasonExp - exp;
			exp = 0;
		}
		else
		{
			exp = exp - m_SeasonExp;
			DownLevel(1);
			m_SeasonExp = m_SeasonLevelDataEntry->maxExp;
		}
	}

	DebugLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName()
		<< ") dec exp season level(" << oldSeasonLevel << " -> " << m_SeasonLevel << ") star(" << oldStar << "->"
		<< m_SeasonStar << ") exp(" << oldSeasonExp << " -> " << m_SeasonExp << "|" << exp << ")" << LogStream::eos;
}

void SeasonLevel::UpLevel()
{
	if (m_SeasonLevelDataEntry == NULL)
	{
		ErrorLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName()
			<< ") up level season level(" << m_SeasonLevel << ") not find" << LogStream::eos;
		return;
	}
	UInt32 oldLevel = m_SeasonLevel;
	UInt32 oldStar = m_SeasonStar;
	if (m_SeasonLevelDataEntry->afterId == 0)
	{
		m_SeasonStar = m_SeasonStar + 1;
	}
	else
	{
		SeasonLevelDataEntry* dataEntry = SeasonLevelDataEntryMgr::Instance()->FindEntry(m_SeasonLevelDataEntry->afterId);
		if (dataEntry == NULL)
		{
			ErrorLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName()
				<< ") up level afterId season level(" << m_SeasonLevel << ") not find" << LogStream::eos;
			return;
		}
		m_SeasonLevel = dataEntry->id;
		m_SeasonLevelDataEntry = dataEntry;
		//没有零星
		if (m_SeasonLevelDataEntry->afterId == 0)
		{
			m_SeasonStar = 1;
		}
		m_SeasonChangeTime = (UInt32)Avalon::Time().CurrentTime().Sec();
		_ResetBattleCount();
	}

	_ChangeMaxSeasonLevel();
	_UpdateSortList();
	_OnChangeSeasonLevel(oldLevel, oldStar);
	GetOwner()->OnSeasonLevelUp();

	DebugLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName()
		<< ") up level  season level(" << oldLevel << " -> " << m_SeasonLevel << ") star(" << oldStar << "->" << m_SeasonStar << ")" << LogStream::eos;
}

void SeasonLevel::DownLevel(UInt32 level)
{
	if (m_SeasonLevelDataEntry == NULL)
	{
		ErrorLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName()
			<< ") down level season level(" << m_SeasonLevel << ") not find" << LogStream::eos;
		return;
	}

	UInt32 oldLevel = m_SeasonLevel;
	UInt32 oldStar = m_SeasonStar;

	if ((UInt32)m_SeasonStar > level)
	{
		m_SeasonStar = m_SeasonStar - level;
		level = 0;
	}
	else
	{
		level = level - m_SeasonStar;
		m_SeasonStar = 0;
		//没有零星
		if (m_SeasonLevelDataEntry->afterId == 0)
		{
			level = level + 1;
		}
	}

	SeasonLevelDataEntry* dataEntry = m_SeasonLevelDataEntry;
	while (level > 0)
	{
		//dataEntry = SeasonLevelDataEntryMgr::Instance()->GetPreSeason(dataEntry->id);
		dataEntry = SeasonLevelDataEntryMgr::Instance()->FindEntry(dataEntry->preId);
		if (dataEntry == NULL)
		{
			dataEntry = SeasonLevelDataEntryMgr::Instance()->GetMinSeasonLevel();
			break;
		}
		level--;
	}
	m_SeasonLevelDataEntry = dataEntry;
	m_SeasonLevel = m_SeasonLevelDataEntry->id;
	m_SeasonChangeTime = (UInt32)Avalon::Time().CurrentTime().Sec();
	_ResetBattleCount();

	_UpdateSortList();
	_OnChangeSeasonLevel(oldLevel, oldStar);
	

	DebugLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName()
		<< ") down level season level(" << oldLevel << " -> " << m_SeasonLevel << ") star(" << oldStar << "->" << m_SeasonStar << ")" << LogStream::eos;

}

void SeasonLevel::UpdateSeason(UInt32 nowSeasonId)
{
	if (m_SeasonLevelDataEntry == NULL)
	{
		ErrorLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName()
			<< ") down level season level(" << m_SeasonLevel << ") not find" << LogStream::eos;
		return;
	}

	UInt32 oldSeasonLevel = m_SeasonLevel;
	UInt32 oldSeasonStar = m_SeasonStar;

	//发放奖励
	_SendSeasonReward();

	//DownLevel(m_SeasonLevelDataEntry->downLevel);

	//增加王者印记
	if (m_SeasonLevelDataEntry->afterId == 0)
	{
		_IncSeasonKingMark();
	}

	SeasonLevelDataEntry* dataEntry = SeasonLevelDataEntryMgr::Instance()->FindEntry(m_SeasonLevelDataEntry->downLevel);
	if (dataEntry != NULL)
	{
		m_SeasonLevelDataEntry = dataEntry;
		m_SeasonLevel = dataEntry->id;
		m_SeasonStar = 0;
		m_SeasonExp = 0;
		m_SeasonChangeTime = (UInt32)Avalon::Time().CurrentTime().Sec();
	}

	m_SeasonId = nowSeasonId;
	if (GetOwner()->CheckFuncFlag(FUNC_PK))
	{
		m_SeasonPlayStatus = SeasonPlayStatus::SPS_NEW;
	}
	_ResetBattleCount();
	_SetMaxSeasonLevel();
	_UpdateSortList();

	UInt32 newScore = _GetResetMatchScore(GetOwner()->GetMatchScore());
	GetOwner()->SetMatchScore(newScore);

	//清空晋级赛战绩
	m_SeasonUplevelBattle.Clear();

	DebugLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName()
		<< ") UpdateSeason level(" << oldSeasonLevel << " -> " << m_SeasonLevel << ") star(" << oldSeasonStar << "->" << m_SeasonStar << ")" << LogStream::eos;
}


void SeasonLevel::DayDownLevel(UInt32 intervalDay)
{
	if (m_SeasonLevelDataEntry == NULL)
	{
		ErrorLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName()
			<< ") down level season level(" << m_SeasonLevel << ") not find" << LogStream::eos;
		return;
	}

	UInt32 oldSeasonLevel = m_SeasonLevel;
	UInt32 oldStar = m_SeasonStar;

	UInt32 battleCount = _GetBattleCount();
	if (intervalDay > m_SeasonLevelDataEntry->battleDayNumber && m_SeasonLevelDataEntry->battleDayNumber != 0)
	{
		if (battleCount >= m_SeasonLevelDataEntry->battleCount)
		{
			intervalDay = intervalDay - m_SeasonLevelDataEntry->battleDayNumber;
		}
		_ResetBattleCount();
	}

	while (intervalDay > m_SeasonLevelDataEntry->battleDayNumber && m_SeasonLevelDataEntry->battleDayNumber != 0)
	{
		intervalDay = intervalDay - m_SeasonLevelDataEntry->battleDayNumber;
		//DownLevel(1);
		DecExp(100);
	}

	if (oldSeasonLevel != m_SeasonLevel || oldStar != m_SeasonStar)
	{
		CLProtocol::SceneSyncSeasonLevel syncInfo;
		syncInfo.oldSeasonLevel = oldSeasonLevel;
		syncInfo.oldSeasonStar = oldStar;
		syncInfo.seasonLevel = m_SeasonLevel;
		syncInfo.seasonStar = m_SeasonStar;
		GetOwner()->SendProtocol(syncInfo);
	}

	DebugLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName()
		<< ") DayDownLevel level(" << oldSeasonLevel << " -> " << m_SeasonLevel << ") star(" << oldStar << "->" << m_SeasonStar << ")" << LogStream::eos;
}

bool SeasonLevel::SetSeasonLevel(UInt32 seasonLevel, UInt32 seasonStar)
{
	UInt32 oldLevel = (UInt32)m_SeasonLevel;
	UInt32 oldStar = (UInt32)m_SeasonStar;
	SeasonLevelDataEntry* dataEntry = SeasonLevelDataEntryMgr::Instance()->FindEntry(seasonLevel);
	if (dataEntry == NULL)
	{
		ErrorLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName()
			<< ")  SeasonLevel (" << m_SeasonLevel << ") SetSeasonLevel(" << seasonLevel << ") not find" << LogStream::eos;
		return false;
	}
	m_SeasonLevel = seasonLevel;
	m_SeasonStar = seasonStar;
	m_SeasonChangeTime = (UInt32)Avalon::Time().CurrentTime().Sec();
	m_SeasonLevelDataEntry = dataEntry;
	_ResetBattleCount();
	_UpdateSortList();
	_OnChangeSeasonLevel(oldLevel, oldStar);
	_ChangeMaxSeasonLevel();

	DebugLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName()
		<< ") SetSeasonLevel oldLevel(" << oldLevel << " -> " << m_SeasonLevel << ") star(" << oldStar << "->" << m_SeasonStar << ")" << LogStream::eos;

	return true;
}

bool SeasonLevel::SetSeasonUplevelBattleRecord(std::vector<UInt8> vecRecords)
{
	m_SeasonUplevelBattle.SetBattleRecrods(vecRecords);
	return true;
}

void SeasonLevel::SetSeasonAttr(UInt32 changeTime, UInt8 attr)
{
	UInt32 oldSeasonAttr = m_SeasonAttr;
	UInt32 oldTime = m_ChangeSeasonAttrTime;

	if (attr != 0)
	{
		m_SeasonAttr = attr;
	}
	else if (m_SeasonLevelDataEntry != NULL)
	{
		m_SeasonAttr = m_SeasonLevelDataEntry->attrId;
	}
	if (GetOwner()->CheckFuncFlag(FUNC_PK))
	{
		m_SeasonPlayStatus = SeasonPlayStatus::SPS_NEW_ATT;
	}

	m_ChangeSeasonAttrTime = changeTime;

	_SetSeasonAttrLevel();

	DebugLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName()
		<< ") SetSeasonAttr oldAttr(" << oldSeasonAttr << " -> " << m_SeasonAttr << ") changeTime(" << oldTime << "->" << m_ChangeSeasonAttrTime << ")" << LogStream::eos;
}

void SeasonLevel::UpdateSeasonInfo()
{
	UInt32 nextSeasonChangeAttrTime = SeasonMgr::Instance()->GetSeasonAttrStartTime();
	bool isSeasonAttr = false;
	if (m_ChangeSeasonAttrTime < nextSeasonChangeAttrTime)
	{
		//设置赛季属性BUFF
		SetSeasonAttr(nextSeasonChangeAttrTime);
		isSeasonAttr = true;
	}

	bool isSeasonId = false;
	UInt32 nowSeasonId = SeasonMgr::Instance()->GetSeasonId();
	if (nowSeasonId >(UInt32)m_SeasonId)
	{
		UpdateSeason(nowSeasonId);
		isSeasonId = true;
	}

	if (isSeasonAttr || isSeasonId)
	{
		_SendSeasonInfo();
	}
}

void SeasonLevel::_ResetBattleCount()
{
	GetOwner()->SetCounter(SEASON_BATTLE_NUM, 0);
}


void SeasonLevel::_IncBattleCount()
{
	GetOwner()->IncCounter(SEASON_BATTLE_NUM, 1);
}

UInt32 SeasonLevel::_GetBattleCount()
{
	return GetOwner()->GetCounter(SEASON_BATTLE_NUM);
}

void SeasonLevel::_UpdateSortList()
{
	SortListMgr::Instance()->OnSortValueChanged(SORTLIST_1V1_SEASON, GetOwner()->GetID(), GetOwner()->GetName(), 0, GetOwner()->GetOccu(), 0, GetOwner()->GetLevel(), (UInt32)m_SeasonLevel, (UInt32)m_SeasonStar, 0);
	SortListMgr::Instance()->OnActivitySortValueChanged(SORTLIST_1V1_SEASON, GetOwner()->GetPf(), GetOwner()->GetOpenId(), ZONE_ID, SERVER_NAME, GetOwner()->GetID(), GetOwner()->GetAccID(), GetOwner()->GetName(), GetOwner()->GetOccu(), GetOwner()->GetLevel(), (UInt32)m_SeasonLevel, (UInt32)m_SeasonStar, 0);
	SortListType sortListType = Get1v1SeasonSortListTypeByOccu(GetOwner()->GetOccu());
	if (sortListType != SORTLIST_INVALID)
	{
		SortListMgr::Instance()->OnSortValueChanged(sortListType, GetOwner()->GetID(), GetOwner()->GetName(), 0, GetOwner()->GetOccu(), 0, GetOwner()->GetLevel(), (UInt32)m_SeasonLevel, (UInt32)m_SeasonStar, 0);
	}
}

void SeasonLevel::_SendSeasonInfo()
{
	CLProtocol::SceneSyncSeasonInfo syncInfo;
	syncInfo.endTime = SeasonMgr::Instance()->GetEndTime();
	syncInfo.seasonId = m_SeasonId;
	syncInfo.seasonLevel = m_SeasonLevel;
	syncInfo.seasonStar = m_SeasonStar;
	syncInfo.seasonExp = m_SeasonExp;
	syncInfo.seasonStatus = m_SeasonPlayStatus;
	syncInfo.seasonAttrEndTime = SeasonMgr::Instance()->GetSeasonAttrEndTime();
	syncInfo.seasonAttrLevel = _GetSeasonAttrLevel();
	syncInfo.seasonAttr = GetSeasonAttr();
	GetOwner()->SendProtocol(syncInfo);
}

void SeasonLevel::_SendSeasonReward()
{
	//if (m_SeasonLevelDataEntry == NULL) return;
	//std::vector<ItemReward> itemReward = m_SeasonLevelDataEntry->seasonRewards;

	if (!GetOwner()->CheckFuncFlag(FUNC_PK))
	{
		return;
	}

	UInt32 maxSeasonLevel = _GetMaxSeasonLevel();
	SeasonLevelDataEntry* entry = SeasonLevelDataEntryMgr::Instance()->FindEntry(maxSeasonLevel);
	if (entry == NULL)
	{
		ErrorLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName()
			<< ") _SendSeasonReward maxLevel(" << maxSeasonLevel << ") find entry error" << LogStream::eos;
		return;
	}

	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));

	// 发送者
	SysNotifyMgr::Instance()->MakeNotify(2023, buffer, sizeof(buffer));
	std::string	strSendName(buffer);

	//标题
	memset(buffer, 0, sizeof(buffer));
	SysNotifyMgr::Instance()->MakeNotify(2024, buffer, sizeof(buffer));
	std::string	strTitle(buffer);

	//内容
	memset(buffer, 0, sizeof(buffer));
	SysNotifyMgr::Instance()->MakeNotify(2025, buffer, sizeof(buffer));
	std::string	strContent(buffer);

	std::string strReason = GetReason(SOURCE_TYPE_SEASON_REWARD);

	UInt32 sendMailItemSize = entry->seasonRewards.size();
	UInt32 round = sendMailItemSize / MAX_MAILITEM_NUM;
	UInt32 remain = sendMailItemSize % MAX_MAILITEM_NUM;

	for (UInt32 i = 0; i < round; ++i)
	{
		std::vector<ItemReward> itemReward;
		itemReward.insert(itemReward.end(), entry->seasonRewards.begin() + (i * MAX_MAILITEM_NUM), entry->seasonRewards.begin() + ((i * MAX_MAILITEM_NUM) + MAX_MAILITEM_NUM));
		RewardGroup mailGroup;
		mailGroup.AddRewards(itemReward);
		GetOwner()->SendSysMail(strSendName.c_str(), strTitle.c_str(), strContent.c_str(), strReason.c_str(), &mailGroup);
	}
	
	if (remain > 0)
	{
		std::vector<ItemReward> itemReward;
		itemReward.insert(itemReward.end(), entry->seasonRewards.begin() + (round * MAX_MAILITEM_NUM), entry->seasonRewards.begin() + ((round * MAX_MAILITEM_NUM) + remain));
		RewardGroup mailGroup;
		mailGroup.AddRewards(itemReward);
		GetOwner()->SendSysMail(strSendName.c_str(), strTitle.c_str(), strContent.c_str(), strReason.c_str(), &mailGroup);
	}

}

void SeasonLevel::_SetMaxSeasonLevel()
{
	GetOwner()->SetCounter(SEASON_MAX_LEVEL, (UInt32)m_SeasonLevel);
}

void SeasonLevel::_ChangeMaxSeasonLevel()
{
	UInt32 maxSeasonLevel = _GetMaxSeasonLevel();
	if ((UInt32)m_SeasonLevel > maxSeasonLevel)
		_SetMaxSeasonLevel();
}

UInt32 SeasonLevel::_GetMaxSeasonLevel()
{
	return GetOwner()->GetCounter(SEASON_MAX_LEVEL);
}

void SeasonLevel::_IncSeasonWinStreak()
{
	GetOwner()->IncCounter(SEASON_WIN_STREAK, 1);
}

void SeasonLevel::_ResetSeasonWinStreak()
{
	if (_GetSeasonWinStreak() == 0) return;
	GetOwner()->SetCounter(SEASON_WIN_STREAK, 0);
}

UInt32 SeasonLevel::_GetSeasonWinStreak()
{
	/*
	PkStatistic* statistic = GetOwner()->GetPkStatisticMgr().FindPkStatistic(PkType::PK_SEASON_1V1);
	if (statistic == NULL) return 0;
	return statistic->GetCurWinStreak();
	*/
	return GetOwner()->GetCounter(SEASON_WIN_STREAK);
}

void SeasonLevel::_SetSeasonAttrLevel()
{
	GetOwner()->SetCounter(SEASON_ATTR_LEVEL, m_SeasonLevel);
}

UInt32 SeasonLevel::_GetSeasonAttrLevel()
{
	return GetOwner()->GetCounter(SEASON_ATTR_LEVEL);
}

void SeasonLevel::_OnChangeSeasonLevel(UInt32 oldSeasonLevel, UInt32 oldSeasonStar)
{
	UInt32 oldSmallLevel = GetSeasonSmallLevel(oldSeasonLevel);
	UInt32 newSmallLevel = GetSeasonSmallLevel((UInt32)m_SeasonLevel);
	//小段位变化
	if (oldSmallLevel != newSmallLevel)
	{
		_OnChangeSmallSeasonLevel(oldSeasonLevel);
	}
}

void SeasonLevel::_OnChangeSmallSeasonLevel(UInt32 oldSeasonLevel)
{
	GetOwner()->SyncBaseInfoToWorld();
}

UInt32 SeasonLevel::_GetResetMatchScore(UInt32 oldScore)
{
	return 1000 + (UInt32)round((oldScore - 1000) * 0.35f);
}

bool SeasonLevel::_CheckSeasonUplevel(_resultBitmask resultFlag, Int32& changeSeasonExp)
{
	bool isUplevelBattle = false;
	if (m_SeasonLevelDataEntry == NULL)
	{
		ErrorLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName()
			<< ") _CheckSeasonUplevel level(" << m_SeasonLevel << ") not find" << LogStream::eos;
		return isUplevelBattle;
	}

	if (!m_SeasonLevelDataEntry->IsUplevelBattle())
	{
		return isUplevelBattle;
	}

	DebugLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName()
		<< ") _CheckSeasonUplevel level(" << m_SeasonLevel << ")" << LogStream::eos;

	isUplevelBattle = true;
	_ResetSeasonWinStreak();

	if (resultFlag & _resultBitmask::resultBitmask_invalid)
	{
		ErrorLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName()
			<< ") _CheckSeasonUplevel level(" << m_SeasonLevel << ") resultFlag(" << resultFlag << ") error" << LogStream::eos;
		return isUplevelBattle;
	}

	m_SeasonUplevelBattle.IncBattleRecord(resultFlag);
	if (m_SeasonUplevelBattle.GetBattleWinSize() >= m_SeasonLevelDataEntry->uplevelBattleWinCount)
	{
		//晋级成功
		IncExp(m_SeasonLevelDataEntry->uplevelBattleWinExp, true);
		changeSeasonExp = changeSeasonExp + m_SeasonLevelDataEntry->uplevelBattleWinExp;
		m_SeasonUplevelBattle.Clear();

		if (GetSeasonMainLevel(m_SeasonLevel) == SeasonMainLevel::SML_DIAMOND)
		{
			GetOwner()->SendGuildEventToWorld(SysNotifyMgr::Instance()->MakeString(14003, GetOwner()->GetName()));
		}
		else if (GetSeasonMainLevel(m_SeasonLevel) == SeasonMainLevel::SML_KING)
		{
			GetOwner()->SendGuildEventToWorld(SysNotifyMgr::Instance()->MakeString(14004, GetOwner()->GetName()));
		}
	}
	else if (m_SeasonUplevelBattle.GetBattleLoseSize() > m_SeasonLevelDataEntry->uplevelBattleCount - m_SeasonLevelDataEntry->uplevelBattleWinCount)
	{
		//晋级失败
		DecExp(m_SeasonLevelDataEntry->uplevelBattleLoseExp);
		changeSeasonExp = changeSeasonExp - m_SeasonLevelDataEntry->uplevelBattleLoseExp;
		m_SeasonUplevelBattle.Clear();
	}
	return isUplevelBattle;
}

void SeasonLevel::_IncSeasonKingMark()
{
	m_SeasonKingMarkCount = m_SeasonKingMarkCount + 1;
}

UInt32 SeasonLevel::CalculationSeasonExp(UInt32 winSeasonLevel, UInt32 loseSeasonLevel)
{
	UInt32 winSeasonMainLevel = GetSeasonMainLevel(winSeasonLevel);
	UInt32 loseSeasonMainLevel = GetSeasonMainLevel(loseSeasonLevel);
	return SeasonLevelDataEntryMgr::Instance()->GetSeasonExp(winSeasonMainLevel, loseSeasonMainLevel);
}
