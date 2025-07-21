#include "AccountInfo.h"
#include <CLRecordClient.h>
#include "AccountMgr.h"
#include <CLSecurityLockDefine.h>
#include <CLSystemValueDataEntry.h>
#include <CLRoleValueTotalScoreCalcDataEntry.h>
#include <CLGateProtocol.h>
#include <CLNetwork.h>
#include <CLSyncObjectProtocol.h>
#include <CLAdventureTeamDataEntry.h>
#include <CLAdventureTeamGradeDataEntry.h>


#include "AccountLoadDataCallback.h"
#include "AccountCounter.h"
#include "AccountAchievementTaskMgr.h"
#include "WSPlayer.h"
#include "WSServiceSwitchMgr.h"
#include "WSSortListMgr.h"
#include "WSApplication.h"
#include "WSPlayerMgr.h"

AccountInfo::AccountInfo()
{
	m_Accid = 0;
	m_Point = 0;
	m_CreditPoint = 0;
	m_MallPoint = 0;
	m_VipLevel = 0;
	m_VipExp = 0;
	m_TotalChargeNum = 0;
	m_MonthCardExpireTime = 0;

	m_SaveTimer.SetInterval(1000);
	m_LoadFinishCallback = NULL;
	m_LoadDataCount = 0;
	m_LoadDataSerial = 0;
	m_StorageSize = 0;
	m_MoneyManageStatus = MoneyManageStatus::MMS_NOT_BUY;
	m_SecurityState = 0;
	m_SecurityFreezeTime = 0;
	m_SecurityUnFreezeTime = 0;
	m_PasswdErrorNum = 0;
	m_PasswdErrorNumResetTime = 0;

	m_UnlockedExtensibleRoleField = 0;
	m_GnomeCoinNum = 0;
	m_GnomeCoinRefreshTime = 0;
	m_AdventureTeamLevel = 0;
	m_AdventureTeamExp = 0;
	m_AdventureTeamGrade = 0;
	m_TotalScoreOfAllRoleValue = 0;
	m_QueryUnlockedNewOccusTime = 0;
	m_TitleMgr.SetOwner(this);
	m_OfflineTime = 0;

	m_AchievementTasks.SetOwner(this);
	m_WeeklyTaskMgr.SetOwner(this);
	m_OfflineTime = 0;

	m_Monopoly.SetOwner(this);
}

AccountInfo::~AccountInfo()
{
	m_RoleIdSet.clear();
	for (auto itr : m_DungeonHardInfoes)
	{
		if (itr.second)
		{
			CL_SAFE_DELETE(itr.second);
		}
	}
	m_DungeonHardInfoes.clear();

	for (auto iter : m_AllRoleValueScores)
	{
		if (iter)
		{
			CL_SAFE_DELETE(iter);
		}
	}
	m_AllRoleValueScores.clear();

	for (auto iter : m_AccountCounter)
	{
		if (iter)
		{
			CL_SAFE_DELETE(iter);
		}
	}
	m_AccountCounter.clear();
}

void AccountInfo::OnTick(const Avalon::Time& now)
{
	if (m_SaveTimer.IsTimeout(now))
	{
		SaveToDB();
		GetTitleMgr().OnTick(now);
		m_ExpeditionMgr.OnTick(now);

		do 
		{
			// 没有玩家在线
			if (WSApplication::Instance()->IsAccountDelayedRefresh() && m_RoleIdSet.size() <= 0)
			{
				break;
			}

			if (m_LogicWeekChangeTime == 0 || m_LogicWeekChangeTime > now.MSec())
			{
				break;
			}

			UInt64 nextRefreshTimestamp = 0;
			TimeUtil::RefreshDate refreshDate;
			refreshDate.weekDay = 1;
			refreshDate.hour = 6;
			if (!TimeUtil::CalculateNextRefreshTimestamp(TimeUtil::REFRESH_TYPE_PER_WEEK, refreshDate, nextRefreshTimestamp))
			{
				break;
			}

			if (m_LogicWeekChangeTime == nextRefreshTimestamp)
			{
				break;
			}

			m_LogicWeekChangeTime = nextRefreshTimestamp;
			SetAccountCounterNumNoSync(ACC_COUNTER_LOGIC_WEEK_REFRESH, m_LogicWeekChangeTime);
			OnLogicWeekChange(now);

		} while (0);
	}

	GetMonopolyInfo().OnTick(now);
}

void AccountInfo::OnLogicWeekChange(const Avalon::Time& now)
{
	InfoLogStream << ACCOUNTINFO(this) << " on logic week change..." << LogStream::eos;

	m_WeeklyTaskMgr.OnLogicWeekChange();
}

void AccountInfo::SetGateConn(UInt32 nodeId)
{ 
	m_Conn = WSNetwork::Instance()->FindConnection(nodeId);
}

WSPlayer* AccountInfo::GetCurrentRole()
{
	return WSPlayerMgr::Instance()->FindPlayer(m_RoleId);
}

bool AccountInfo::CanBuyMonthCard()
{
	return true;
	/*UInt32 curSec = (UInt32)CURRENT_TIME.Sec();
	if (curSec >= GetMonthCardExpireTime() || GetMonthCardExpireTime() - curSec <= 3 * Avalon::Time::SECS_OF_DAY)
	{
		return true;
	}

	return false;*/
}

bool AccountInfo::CanBuyMoneyManageCard()
{
	/*
	if (m_MoneyManageStatus != MoneyManageStatus::MMS_NOT_BUY)
	{
		return false;
	}
	*/
	return true;
}

void AccountInfo::SetMoneyManageStatus(UInt8 status) 
{
	if ((MoneyManageStatus)status == MoneyManageStatus::MMS_PLAYER_BUY || (MoneyManageStatus)status == MoneyManageStatus::MMS_NEW_PLAYER_BUY)
	{
		m_MoneyManageStatus = MoneyManageStatus::MMS_ACCOUNT_BUY;
	}
}

void AccountInfo::AddCreditPoint(UInt32 num)
{
	if (m_CreditPoint + num >= MAX_MONEY)
	{
		m_CreditPoint = MAX_MONEY;
	}
	else
	{
		m_CreditPoint = m_CreditPoint + num;
	}
}

void AccountInfo::RemoveCreditPoint(UInt32 num)
{
	if (num >= m_CreditPoint)
	{
		m_CreditPoint = 0;
	}
	else
	{
		m_CreditPoint = m_CreditPoint - num;
	}
}

void AccountInfo::LoadAccountCounter(CLRecordCallback* callback)
{
	if (!callback) return;

	while (callback->NextRow())
	{
		UInt8 counterType = callback->GetData("counter_type");

		AccountCounter* counter = FindAccountCounter((AccountCounterType)counterType);
		if (!counter)
		{
			counter = new AccountCounter();
			counter->SetID(callback->GetKey());
			counter->SetDBData(callback);
			AddAccountCounter(counter);
		}
		else
		{	 // 已经找到，属于合服问题
			switch (counterType)
			{
			case ACC_COUNTER_ENERGY_COIN:
			{  
				// 加起来, 删除原来的
				UInt64 num = callback->GetData("counter_num");
				counter->SetCounterNum(counter->GetCounterNum() + num);

				CLDeleteCommand* del = CLRecordClient::Instance()->PrepareDeleteCommand("t_account_counter", callback->GetKey());
				CLRecordClient::Instance()->SendCommand(del);

				break;
			}
			default:
				break;
			}
		}
	}
}

AccountCounter* AccountInfo::FindAccountCounter(AccountCounterType type)
{
	for (auto elem : m_AccountCounter)
	{
		if (!elem) continue;

		if (elem->GetCounterType() == type)
		{
			return elem;
		}
	}
	return NULL;
}

void AccountInfo::AddAccountCounter(AccountCounter* counter)
{
	m_AccountCounter.push_back(counter);
}

void AccountInfo::SetAccountCounterNum(WSPlayer* player, AccountCounterType type, UInt64 num)
{
	auto counter = FindAccountCounter(type);
	if (!counter)
	{
		counter = new AccountCounter();
		counter->SetAccGUID(GetID());
		counter->SetOwner(GetAccID());
		counter->SetCounterType(type);
		counter->TryRefresh();
		AddAccountCounter(counter);
	}

	counter->SetCounterNum(num);

	if (player)
	{
		CLProtocol::WorldAccountCounterNotify protocol;
		CLProtocol::AccountCounter countInfo;
		countInfo.type = type;
		countInfo.num = num;
		protocol.accCounterVec.push_back(countInfo);
		player->SendProtocol(protocol);
	}
}

void AccountInfo::SetAccountCounterNumNoSync(AccountCounterType type, UInt64 num)
{
	auto counter = FindAccountCounter(type);
	if (!counter)
	{
		counter = new AccountCounter();
		counter->SetAccGUID(GetID());
		counter->SetOwner(GetAccID());
		counter->SetCounterType(type);
		counter->TryRefresh();
		AddAccountCounter(counter);
	}

	counter->SetCounterNum(num);
}

UInt64 AccountInfo::GetAccountCounterNum(AccountCounterType type)
{
	auto counter = FindAccountCounter(type);
	return counter ? counter->GetCounterNum() : 0;
}

bool AccountInfo::CheckRmAccountMoney(ItemSubType moneyType, UInt32 cost)
{
	switch (moneyType)
	{
	case ST_BLESS_CRYSTAL: return (UInt32)GetAccountCounterNum(ACC_COUNTER_BLESS_CRYSTAL) >= cost;
	case ST_INHERIT_BLESS: return (UInt32)GetAccountCounterNum(ACC_COUNTER_INHERIT_BLESS) >= cost;
	case ST_BOUNTY: return (UInt32)GetAccountCounterNum(ACC_COUNTER_BOUNTY) >= cost;
	case ST_HIRE_COIN: return (UInt32)GetAccountCounterNum(ACC_COUNTER_HIRE_COIN) >= cost;
	default:
		ErrorLogStream << "Can't find item subtype(" << (UInt32)moneyType << ")!" << LogStream::eos;
		return false;
	}
}

bool AccountInfo::RemoveAccountMoney(WSPlayer* player, const std::string& reason, ItemSubType moneyType, UInt32 num)
{
	if (!player) return false;

	std::string moneyLogName;
	AssetType assetType = ASSET_INVALID;
	AccountCounterType counterType = ACC_COUNTER_INVALID;
	switch (moneyType)
	{
	case ST_BLESS_CRYSTAL: 
		moneyLogName = "blessCrystal";
		assetType = ASSET_BLESS_CRYSTAL;
		counterType = ACC_COUNTER_BLESS_CRYSTAL;
		break;

	case ST_INHERIT_BLESS:
		moneyLogName = "inheritBless";
		assetType = ASSET_INHERIT_BLESS;
		counterType = ACC_COUNTER_INHERIT_BLESS;
		break;

	case ST_BOUNTY: 
		moneyLogName = "bounty";
		assetType = ASSET_BOUNTY;
		counterType = ACC_COUNTER_BOUNTY;
		break;

	case ST_HIRE_COIN:
		moneyLogName = "hirecoin";
		assetType = ASSET_HIRE_COIN;
		counterType = ACC_COUNTER_HIRE_COIN;
		break;

	default:
		ErrorLogStream << PLAYERINFO(player) << " remove account money(" << (UInt32)moneyType << ") failed!" << LogStream::eos;
		return false;
	}

	UInt32 newNum = 0;
	UInt32 oldNum = (UInt32)GetAccountCounterNum(counterType);

	if (num < oldNum)
	{
		newNum = oldNum - num;
	}

	if (oldNum > newNum)
	{
		SetAccountCounterNum(player, counterType, newNum);

		GameInfoLogStream("Property") << "player(" << player->GetAccId() << "," << player->GetID() << "," << player->GetName() << ") remove "
			<< moneyLogName << ":" << reason << "|" << oldNum - newNum << "|" << newNum << LogStream::eos;

		player->SendAssetUdpLog(assetType, reason.c_str(), (UInt64)oldNum, (Int64)num * -1, (Int64)newNum - (Int64)oldNum, (Int64)newNum);
		return true;
	}

	return false;
}

bool AccountInfo::IncAccountMoney(WSPlayer* player, const std::string& reason, ItemSubType moneyType, UInt32 incNum)
{
	if (!player) return false;

	std::string moneyLogName;
	AssetType assetType = ASSET_INVALID;
	AccountCounterType counterType = ACC_COUNTER_INVALID;
	switch (moneyType)
	{
	case ST_BOUNTY:
		moneyLogName = "bounty";
		assetType = ASSET_BOUNTY;
		counterType = ACC_COUNTER_BOUNTY;
		break;

	case ST_HIRE_COIN:
		moneyLogName = "hirecoin";
		assetType = ASSET_HIRE_COIN;
		counterType = ACC_COUNTER_HIRE_COIN;
		break;

	default:
		ErrorLogStream << PLAYERINFO(player) << " remove account money(" << (UInt32)moneyType << ") failed!" << LogStream::eos;
		return false;
	}

	UInt32 oldNum = (UInt32)GetAccountCounterNum(counterType);
	UInt32 newNum = 0;
	if (oldNum + incNum > MAX_MONEY) newNum = MAX_MONEY;
	else newNum = oldNum + incNum;

	if (newNum > oldNum)
	{
		SetAccountCounterNum(player, counterType, newNum);

		GameInfoLogStream("Property") << "player(" << player->GetAccId() << "," << player->GetID() << "," << player->GetName() << ") add " << moneyLogName << ":"
			<< reason << "|" << newNum - oldNum << "|" << newNum << LogStream::eos;
		player->SendAssetUdpLog(assetType, reason.c_str(), (UInt64)oldNum, (Int64)incNum, (Int64)newNum - (Int64)oldNum, (UInt64)newNum);
		return true;
	}
	return false;
}

bool AccountInfo::CheckRmAccountCounterNum(AccountCounterType type, UInt64 cost)
{
	return GetAccountCounterNum(type) >= cost;
}

bool AccountInfo::RemoveAccountCounterNum(WSPlayer* player, const std::string& reason, AccountCounterType type, UInt64 num)
{
	if (!player) return false;

	std::string logName;
	AssetType assetType = ASSET_INVALID;
	switch (type)
	{
	case ACC_COUNTER_BLESS_CRYSTAL_EXP:
		logName = "blessCrystalExp";
		assetType = ASSET_BLESS_CRYSTAL_EXP;
		break;

	case ACC_COUNTER_INHERIT_BLESS_EXP:
		logName = "inheritBlessExp";
		assetType = ASSET_INHERIT_BLESS_EXP;
		break;

	default:
		ErrorLogStream << PLAYERINFO(player) << " remove account type(" << (UInt32)type << ") failed!" << LogStream::eos;
		return false;
	}

	UInt64 newNum = 0;
	UInt64 oldNum = GetAccountCounterNum(type);

	if (num < oldNum)
	{
		newNum = oldNum - num;
	}

	if (oldNum > newNum)
	{
		SetAccountCounterNum(player, type, newNum);

		GameInfoLogStream("Property") << "player(" << player->GetAccId() << "," << player->GetID() << "," << player->GetName() << ") remove "
			<< logName << ":" << reason << "|" << oldNum - newNum << "|" << newNum << LogStream::eos;

		player->SendAssetUdpLog(assetType, reason.c_str(), (UInt64)oldNum, (Int64)num * -1, (Int64)newNum - (Int64)oldNum, (Int64)newNum);
		return true;
	}

	return false;
}

bool AccountInfo::UnlockExtensibleRoleField(UInt32 incNum)
{
	SetUnlockedExtensibleRoleField(GetUnlockedExtensibleRoleField() + incNum);
	InfoLogStream << "Account(" << GetAccID() << ") increase extensible role field(" << incNum << "), now field=" << GetUnlockedExtensibleRoleField() << ")." << LogStream::eos;
	return true;
}

bool AccountInfo::HasAdventureTeam()
{
	if (!WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_ADVENTURE_TEAM))
	{
		return false;
	}
	return !GetAdventureTeamName().empty(); 
}

void AccountInfo::SetAdventureTeamLevel(UInt16 level) 
{
	m_AdventureTeamLevel = level; 

}

void AccountInfo::TryIncAdventureTeamExp(WSPlayer* player, const std::string& reason, UInt64 incExp)
{
	if (!player || incExp == 0) return;

	// 当前冒险队等级
	UInt32 oldLevel = (UInt32)GetAdventureTeamLevel();
	// 最大值
	UInt32 maxLevel = AdventureTeamDataEntryMgr::Instance()->GetMaxLevel();

	if (oldLevel >= maxLevel)
	{
		return;
	}

	// 计算值
	UInt32 calcLevel = (UInt32)GetAdventureTeamLevel();
	UInt64 oldExp = GetAdventureTeamExp();
	UInt64 newExp = oldExp;
	if (oldExp + incExp > MAX_MONEY) newExp = MAX_MONEY;
	else newExp = oldExp + incExp;

	while (calcLevel < maxLevel)
	{
		auto adventureTeamData = AdventureTeamDataEntryMgr::Instance()->FindEntry(calcLevel);
		if (!adventureTeamData)
		{
			ErrorLogStream << PLAYERINFO(player) << " can't find adventure team data(" << calcLevel << ")!" << LogStream::eos;
			return;
		}

		if (newExp < adventureTeamData->levelUpExp)
		{
			break;
		}

		calcLevel++;
		newExp -= adventureTeamData->levelUpExp;
	}

	// 如果等级达到最大
	if (calcLevel >= maxLevel)
	{
		newExp = 0;
	}

	if (oldExp != newExp)
	{
		SetAdventureTeamExp(newExp);
		GameInfoLogStream("Property") << "player(" << player->GetAccId() << "," << player->GetID() << "," << player->GetName() << ") add adventureTeamExp: "
			<< reason << "|" << (Int64)newExp - (Int64)oldExp << "|" << newExp << LogStream::eos;
		player->SendAssetUdpLog(ASSET_ADVENTURE_TEAM_EXP, reason.c_str(), (UInt64)oldExp, (Int64)incExp, (Int64)newExp - (Int64)oldExp, (UInt64)newExp);
	}

	if (calcLevel <= maxLevel && oldLevel < calcLevel)
	{
		// 等级改变

		UInt16 newLevel = (UInt16)calcLevel;
		SetAdventureTeamLevel(newLevel);
		GameInfoLogStream("Property") << "player(" << player->GetAccId() << "," << player->GetID() << "," << player->GetName() << ") up adventureTeamLevel: "
			<< reason << "|" << newLevel - oldLevel << "|" << newLevel << LogStream::eos;
		player->SendAssetUdpLog(ASSET_ADVENTURE_TEAM_LEVEL, reason.c_str(), (UInt64)oldLevel, (Int64)newLevel - (Int64)oldLevel, (Int64)newLevel - (Int64)oldLevel, (UInt64)newLevel);

		OnAdventureTeamLevelChanged();

		player->SyncAdventureTeamInfo();
	}
}

void AccountInfo::TryIncBlessExp(WSPlayer* player, const std::string& reason, UInt64 incAdventureTeamExp, AccountCounterType counterType)
{
	if (!player || incAdventureTeamExp == 0) return;

	// 增加的经验
	UInt64 incBlessExp = 0;
	// 最大数量
	UInt32 maxBlessNum = 0;
	// 最大经验
	UInt64 maxBlessExp = 0;
	// 现在拥有的数量
	UInt32 oldBlessNum = 0;
	// 现在拥有的经验
	UInt64 oldBlessExp = 0;
	// 数量资产类型
	AssetType numAssetType = ASSET_INVALID;
	// 经验资产类型
	AssetType expAssetType = ASSET_INVALID;
	// 数量日志格式
	std::string numLogName;
	// 经验日志格式
	std::string expLogName;
	// 数量counter
	AccountCounterType numCounterType = ACC_COUNTER_INVALID;

	switch (counterType)
	{
	case ACC_COUNTER_BLESS_CRYSTAL_EXP:
		incBlessExp = incAdventureTeamExp;
		maxBlessNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_BLESS_CRYSTAL_OWN_MAX_NUM);
		maxBlessExp = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_BLESS_CRYSTAL_OWN_MAX_EXP);
		oldBlessNum = (UInt32)GetAccountCounterNum(ACC_COUNTER_BLESS_CRYSTAL);
		oldBlessExp = GetAccountCounterNum(ACC_COUNTER_BLESS_CRYSTAL_EXP);
		numAssetType = ASSET_BLESS_CRYSTAL;
		expAssetType = ASSET_BLESS_CRYSTAL_EXP;
		numLogName = "blessCrystal";
		expLogName = "blessCrystalExp";
		numCounterType = ACC_COUNTER_BLESS_CRYSTAL;
		break;
	case ACC_COUNTER_INHERIT_BLESS_EXP:
		incBlessExp = incAdventureTeamExp;
		maxBlessNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_INHERIT_BLESS_OWN_MAX_NUM);
		maxBlessExp = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_INHERIT_BLESS_OWN_MAX_EXP);
		oldBlessNum = (UInt32)GetAccountCounterNum(ACC_COUNTER_INHERIT_BLESS);
		oldBlessExp = GetAccountCounterNum(ACC_COUNTER_INHERIT_BLESS_EXP);
		numAssetType = ASSET_INHERIT_BLESS;
		expAssetType = ASSET_INHERIT_BLESS_EXP;
		numLogName = "inheritBless";
		expLogName = "inheritBlessExp";
		numCounterType = ACC_COUNTER_INHERIT_BLESS;
		break;
	default:
		ErrorLogStream << PLAYERINFO(player) " can't increase invalid type(" << (UInt32)counterType << ")!" << LogStream::eos;
		return;
	}

	if (maxBlessNum == 0 || maxBlessExp == 0)
	{
		ErrorLogStream << PLAYERINFO(player) << " system value data is zero!" << LogStream::eos;
		return;
	}

	UInt64 newBlessExp = oldBlessExp;
	UInt32 newBlessNum = oldBlessNum;

	// 增加后的经验没有达到阈值直接增加
	if ((oldBlessExp + incBlessExp) < maxBlessExp)
	{
		newBlessExp = oldBlessExp + incBlessExp;
		if (newBlessExp > oldBlessExp)
		{
			SetAccountCounterNum(player, counterType, newBlessExp);
		}
	}
	else
	{
		// 经验临近值
		UInt64 expNearThreshold = maxBlessExp - 1;

		// 拥有的数量达到最大值
		if (oldBlessNum >= maxBlessNum)
		{
			if (oldBlessNum > maxBlessNum)
			{
				ErrorLogStream << PLAYERINFO(player) << " own bless crystal(" << oldBlessNum << ") is more than threshold(" << maxBlessNum << ")!" << LogStream::eos;
			}

			// 经验只增加到临近值
			if (oldBlessExp == expNearThreshold)
			{
				DevLogStream << PLAYERINFO(player) << " own bless crystal(" << oldBlessNum << ") is reach threshold(" << maxBlessNum << "), current exp(" << oldBlessExp << ") is near reach max(" << expNearThreshold << ")!" << LogStream::eos;
				return;
			}

			newBlessExp = expNearThreshold;
			InfoLogStream << PLAYERINFO(player) << " own bless crystal(" << oldBlessNum << ") is reach threshold(" << maxBlessNum << ") while increase adventure team exp(" << incBlessExp << ")." << LogStream::eos;
		}
		else
		{
			UInt64 sumExp = oldBlessExp + incBlessExp;
			// 累积值
			UInt64 cumulateValue = sumExp / maxBlessExp;

			UInt64 restExp = sumExp % maxBlessExp;
			if (restExp >= expNearThreshold)
			{
				restExp = expNearThreshold;
			}
			newBlessExp = restExp;

			// 祝福数量相加值
			UInt32 sumBlessNum = (UInt32)cumulateValue + oldBlessNum;
			if (sumBlessNum >= maxBlessNum)
			{
				sumBlessNum = maxBlessNum;
			}
			newBlessNum = sumBlessNum;

			if (newBlessNum > oldBlessNum)
			{
				SetAccountCounterNum(player, numCounterType, newBlessNum);
				GameInfoLogStream("Property") << "player(" << player->GetAccId() << "," << player->GetID() << "," << player->GetName() << ") add " << numLogName << ": "
					<< reason << "|" << newBlessNum - oldBlessNum << "|" << newBlessNum << LogStream::eos;
				player->SendAssetUdpLog(numAssetType, reason.c_str(), (UInt64)oldBlessNum, (Int64)cumulateValue, (Int64)newBlessNum - (Int64)oldBlessNum, (UInt64)newBlessNum);
			}
		}
	}

	if (newBlessExp != oldBlessExp)
	{
		SetAccountCounterNum(player, counterType, newBlessExp);
		GameInfoLogStream("Property") << "player(" << player->GetAccId() << "," << player->GetID() << "," << player->GetName() << ") add " << expLogName << ": "
			<< reason << "|" << (Int64)newBlessExp - (Int64)oldBlessExp << "|" << newBlessExp << LogStream::eos;
		player->SendAssetUdpLog(expAssetType, reason.c_str(), (UInt64)oldBlessExp, (Int64)incBlessExp, (Int64)newBlessExp - (Int64)oldBlessExp, (UInt64)newBlessExp);
	}
}

const std::string& AccountInfo::GetAdventureTeamGradeStr() const
{
	AdventureTeamGradeDataEntry* adventureTeamGradeData = AdventureTeamGradeDataEntryMgr::Instance()->FindEntry(GetAdventureTeamGrade());
	if (!adventureTeamGradeData)
	{
		static std::string s_EmptyStr = "";
		ErrorLogStream << "Account(" << GetAccID() << ") can't find adventure team grade(" << GetAdventureTeamGrade() << ")!" << LogStream::eos;
		return s_EmptyStr;
	}
	return adventureTeamGradeData->adventureTeamGrade;
}

void AccountInfo::SetUnlockedNewOccus(const std::map<UInt8, UInt8>& occus)
{
	if (occus.empty())
	{
		m_UnlockedNewOccus = "";
		return;
	}
	std::ostringstream oss;
	for (auto occu : occus)
	{
		if (!oss.str().empty())
		{
			oss << ",";
		}
		oss << (UInt32)occu.first << ":" << (UInt32)occu.second;

		if (oss.str().size() >= 1024)
		{
			ErrorLogStream << "Account(" << GetAccID() << ")'s unlocked_new_occus size is more than 1024." << LogStream::eos;
			return;
		}
	}
	m_UnlockedNewOccus = oss.str();
}

void AccountInfo::GetUnlockedNewOccus(std::map<UInt8, UInt8>& newOccus)
{
	if (m_UnlockedNewOccus.GetDBValue().empty()) return;
	std::vector<std::string> strVec;
	Avalon::StringUtil::Split(m_UnlockedNewOccus.GetDBValue(), strVec, ",");
	for (const auto& elem : strVec)
	{
		std::vector<std::string> strVec2;
		Avalon::StringUtil::Split(elem, strVec2, ":");
		if(strVec2.size() != 2) continue;
		UInt8 key = Avalon::StringUtil::ConvertToValue<UInt8>(strVec2[0]);
		UInt8 val = Avalon::StringUtil::ConvertToValue<UInt8>(strVec2[1]);
		newOccus[key] = val;
	}
}

bool AccountInfo::AddUnlockedNewOccu(UInt8 newOccu)
{
	std::map<UInt8, UInt8> occus;
	GetUnlockedNewOccus(occus);
	auto itr = occus.find(newOccu);
	if (itr == occus.end())
	{
		occus[newOccu] = 1;
		SetUnlockedNewOccus(occus);
		return true;
	}
	return false;
}

bool AccountInfo::RemoveUnlockedNewOccus(const std::vector<UInt8>& occus)
{
	bool bRemoved = false;
	std::map<UInt8, UInt8> unlockedNewOccus;
	GetUnlockedNewOccus(unlockedNewOccus);
	if (unlockedNewOccus.empty()) return false;
	for (auto occu : occus)
	{
		auto itr = unlockedNewOccus.find(occu);
		if (itr != unlockedNewOccus.end())
		{
			unlockedNewOccus[occu] = 0;
			bRemoved = true;
		}
	}
	SetUnlockedNewOccus(unlockedNewOccus);
	return bRemoved;
}

void AccountInfo::LoadRoleValueScoreCallback(CLRecordCallback* callback)
{
	if (!callback) return;

	while (callback->NextRow())
	{
		ObjID_t roleid = callback->GetKey();

		RoleValueScoreInfo* scoreInfo = FindRoleValueScoreInfo(roleid);
		if (!scoreInfo)
		{
			scoreInfo = new RoleValueScoreInfo();
			scoreInfo->roleid = roleid;
			scoreInfo->roleValueScore = callback->GetData("role_value_score");
			AddRoleValueScoreInfo(scoreInfo);
		}
	}

	UInt32 score = CalcTotalScoreOfAllRoleValue();
	SetTotalScoreOfAllRoleValue(score);
	OnTotalScoreOfAllRoleValueChange(NULL, "");
}

RoleValueScoreInfo* AccountInfo::FindRoleValueScoreInfo(ObjID_t roleid)
{
	for (auto scoreInfo : m_AllRoleValueScores)
	{
		if (!scoreInfo) continue;

		if (scoreInfo->roleid == roleid)
		{
			return scoreInfo;
		}
	}
	return NULL;
}

void AccountInfo::AddRoleValueScoreInfo(RoleValueScoreInfo* scoreInfo)
{
	m_AllRoleValueScores.push_back(scoreInfo);
}

UInt32 AccountInfo::CalcTotalScoreOfAllRoleValue()
{
	if (!WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_ADVENTURE_TEAM_SORTLIST))
	{
		return 0;
	}

	if (m_AllRoleValueScores.empty())
	{
		ErrorLogStream << "Account(" << GetAccID() << ")'s value scores of all roles are empty!" << LogStream::eos;
		return 0;
	}

	struct CustomMoreRoleValueScoreHelper
	{
		bool operator()(const RoleValueScoreInfo* lhs, const RoleValueScoreInfo* rhs)
		{
			if (!lhs || !rhs) return false;
			return lhs->roleValueScore > rhs->roleValueScore;
		}
	};

	if (m_AllRoleValueScores.size() != 1)
	{
		std::sort(m_AllRoleValueScores.begin(), m_AllRoleValueScores.end(), CustomMoreRoleValueScoreHelper());
	}

	UInt32 idx = 0;
	UInt32 scoreSum = 0;
	for (auto scoreInfo : m_AllRoleValueScores)
	{
		if (!scoreInfo || scoreInfo->roleValueScore == 0)
		{
			idx++;
			continue;
		}

		if (idx < RoleValueTotalScoreCalcDataEntryMgr::Instance()->GetMaxKeyInDatas())
		{
			idx++;
		}

		auto scoreCalcData = RoleValueTotalScoreCalcDataEntryMgr::Instance()->FindEntry(idx);
		if (!scoreCalcData)
		{
			ErrorLogStream << "Account(" << GetAccID() << ") can't find role value score calc data(" << idx << ")!" << LogStream::eos;
			continue;
		}

		scoreSum += scoreCalcData->ConvertToTotalScoreBySingleScore(scoreInfo->roleValueScore);
	}

	return scoreSum;
}

bool AccountInfo::HasChangedOfRoleValueScore(ObjID_t roleid, UInt32 roleValueScore)
{
	RoleValueScoreInfo* scoreInfo = FindRoleValueScoreInfo(roleid);
	if (!scoreInfo)
	{
		scoreInfo = new RoleValueScoreInfo();
		scoreInfo->roleid = roleid;
		scoreInfo->roleValueScore = roleValueScore;
		AddRoleValueScoreInfo(scoreInfo);
		return true;
	}
	else
	{
		if (scoreInfo->roleValueScore != roleValueScore)
		{
			scoreInfo->roleValueScore = roleValueScore;
			return true;
		}
	}
	return false;
}

void AccountInfo::OnRoleValueScoreChange(WSPlayer* player, const std::string& reason, UInt32 roleValueScore)
{
	if (!player) return;

	UInt32 oldScore = GetTotalScoreOfAllRoleValue();
	UInt32 newScore = CalcTotalScoreOfAllRoleValue();
	if (oldScore == newScore)
	{
		return;
	}

	SetTotalScoreOfAllRoleValue(newScore);

	GameInfoLogStream("Property") << "player(" << player->GetAccId() << "," << player->GetID() << "," << player->GetName() << ") change totalRoleScore: "
		<< reason << "|" << (Int64)newScore - (Int64)oldScore << "|" << newScore << LogStream::eos;
	player->SendAssetUdpLog(ASSET_TOTAL_ROLE_SCORE, reason.c_str(), (UInt64)oldScore, (Int64)roleValueScore, (Int64)newScore - (Int64)oldScore, (UInt64)newScore);

	OnTotalScoreOfAllRoleValueChange(player, reason);
	player->SyncAdventureTeamInfo();
}

void AccountInfo::OnTotalScoreOfAllRoleValueChange(WSPlayer* player, const std::string& reason)
{
	// player指针会是空的！！！

	auto oldEntry = WSSortListMgr::Instance()->FindEntry(SORTLIST_ADVENTURE_TEAM_GRADE, GetAccID());

	if (AdventureTeamGradeDataEntryMgr::Instance()->CanJoinSortList(GetTotalScoreOfAllRoleValue()))
	{
		// 更新排行榜

		SortListEntry entry;
		entry.sortType = SORTLIST_ADVENTURE_TEAM_GRADE;
		entry.id = GetAccID();
		entry.name = GetAdventureTeamGradeStr();
		entry.level = GetAdventureTeamLevel();
		entry.ownerName = GetAdventureTeamName();
		entry.value1 = GetTotalScoreOfAllRoleValue();
		entry.value2 = GetAdventureTeamGrade();

		if (!oldEntry || oldEntry->GetSortFactor() != entry.GetSortFactor())
		{
			WSSortListMgr::Instance()->UpdateSortList(entry);
		}
	}
	else if (oldEntry != NULL)
	{
		WSSortListMgr::Instance()->DeleteEntry(SORTLIST_ADVENTURE_TEAM_GRADE, GetAccID());
	}

	UInt32 scoreRanking = WSSortListMgr::Instance()->GetRanking(SORTLIST_ADVENTURE_TEAM_GRADE, GetAccID());

	// 计算评级

	AdventureTeamGradeDataEntry* adventureTeamGradeData = AdventureTeamGradeDataEntryMgr::Instance()->CalcAdventureTeamGrade(GetTotalScoreOfAllRoleValue(), scoreRanking);
	if (!adventureTeamGradeData)
	{
		return;
	}

	UInt8 oldGrade = GetAdventureTeamGrade();
	UInt8 newGrade = adventureTeamGradeData->id;
	if (AdventureTeamGradeDataEntryMgr::Instance()->CanJoinSortList(GetTotalScoreOfAllRoleValue()))
	{
		auto oldEntry_ = WSSortListMgr::Instance()->FindEntry(SORTLIST_ADVENTURE_TEAM_GRADE, GetAccID());
		if (oldEntry_ && oldEntry_->value2 != newGrade)
		{
			WSSortListMgr::Instance()->OnAdventureTeamGradeChanged(SORTLIST_ADVENTURE_TEAM_GRADE, GetAccID(), newGrade);
		}
	}

	if (oldGrade != newGrade)
	{
		// 评级改变

		SetAdventureTeamGrade(newGrade);

		if (player)
		{
			GameInfoLogStream("Property") << "player(" << player->GetAccId() << "," << player->GetID() << "," << player->GetName() << ") change adventureTeamGrade: "
				<< reason << "|" << newGrade - oldGrade << "|" << newGrade << LogStream::eos;
			player->SendAssetUdpLog(ASSET_ADVENTURE_TEAM_GRADE, reason.c_str(), (UInt64)oldGrade, (Int64)newGrade - (Int64)oldGrade, (Int64)newGrade - (Int64)oldGrade, (UInt64)newGrade);
		}

		OnAdventureTeamGradeChanged();
	}
}

UInt32 AccountInfo::GetGnomeCoinNum()
{ 
	if (m_GnomeCoinNum > 0 && m_GnomeCoinRefreshTime > 0 && m_GnomeCoinRefreshTime <= (UInt32)CURRENT_TIME.Sec())
	{
		InfoLogStream << "Account(" << GetAccID() << ") gnome coin refreshed, old num(" << m_GnomeCoinNum << ") refresh time(" << m_GnomeCoinRefreshTime << ")." << LogStream::eos;
		m_GnomeCoinNum = 0;
	}
	return m_GnomeCoinNum;
}

void AccountInfo::GetAllDungeonHardInfo(std::vector<SceneDungeonHardInfo>& infoes)
{
	for (auto itr : m_DungeonHardInfoes)
	{
		auto hard = itr.second;
		if (!hard)
		{
			continue;
		}

		infoes.push_back(hard->GetSceneDungeonHardInfo());
	}
}

void AccountInfo::UpdateDungeonHardInfo(UInt32 dungeonId, DungeonHardType type)
{
	bool isNewDungeon = false;
	DungeonHardInfo* hard = FindDungeonHardInfoById(dungeonId);
	if (!hard)
	{
		isNewDungeon = true;
		hard = DungeonHardInfo::CreateDungeonHardInfo(dungeonId);
		if (!hard)
		{
			ErrorLogStream << "account(" << GetAccID() << ") create dungeon(" << dungeonId << ") hard info instance failed." << LogStream::eos;
			return;
		}

		hard->SetID(CLRecordClient::Instance()->GenGuid());
		hard->SetDungeonID(dungeonId);
		m_DungeonHardInfoes[dungeonId] = hard;
	}

	if (type < hard->GetUnlockedHardType() && !isNewDungeon)
	{
		return;
	}

	hard->SetUnlockedHardType(type);

	CLSaveCommand* cmd = NULL;
	if (isNewDungeon)
	{
		cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_g_dungeon_hard", hard->GetID());
	}
	else
	{
		cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_g_dungeon_hard", hard->GetID(), true);
	}

	hard->GetDBData(cmd, false);
	cmd->PutData("account", GetAccID());
	CLRecordClient::Instance()->SendCommand(cmd);
}

bool AccountInfo::LoadDungeonHardInfo(DungeonHardInfo* hard)
{
	if (!hard)
	{
		return false;
	}

	if (FindDungeonHardInfoById(hard->GetDungeonID()))
	{
		ErrorLogStream << "account(" << GetAccID() << ") has repeat dungeon(" << hard->GetDungeonID() << ") hard data." << LogStream::eos;
		return false;
	}

	m_DungeonHardInfoes[hard->GetDungeonID()] = hard;
	return true;
}

DungeonHardInfo* AccountInfo::FindDungeonHardInfoById(UInt32 dungeonId)
{
	auto itr = m_DungeonHardInfoes.find(dungeonId);
	return itr == m_DungeonHardInfoes.end() ? NULL : itr->second;
}

void AccountInfo::LoadAccountTask(CLRecordCallback* callback)
{
	if (!callback) return;

	while (callback->NextRow())
	{
		UInt64 guid = callback->GetKey();
		// UInt32 accId = callback->GetData("accid");
		UInt32 dataId = callback->GetData("dataid");
		UInt8 status = callback->GetData("status");
		std::string parameter = callback->GetData("parameter").GetString();
		UInt32 finTime = callback->GetData("finished_time");
		// UInt8 submitCount = callback->GetData("submit_count");

		TaskDataEntry* dataEntry = TaskDataEntryMgr::Instance()->FindEntry(dataId);
		if (dataEntry == NULL || !dataEntry->open) continue;

		AccountTask* task = NULL;

		switch (dataEntry->type)
		{
		case TASK_ACCOUNT_ACHIEVEMENT:
		{
			task = new AccountAchievementTask(this, dataId, guid, (TaskStatus)status, parameter, finTime);
			if (!task->SetupBase() || !m_AchievementTasks.LoadTask(task))
			{
				ErrorLogStream << ACCOUNTINFO(this) << " load account task(" << dataId << ") failed." << LogStream::eos;
			}
			break;
		}
		case TASK_ADVENTURE_TEAM_ACCOUNT_WEEKLY:
		{
			task = new AdventureWeeklyTask(this, dataId, guid, (TaskStatus)status, parameter);
			if (!task->SetupBase() || !m_WeeklyTaskMgr.LoadTask(task))
			{
				ErrorLogStream << ACCOUNTINFO(this) << " load " << task->LogStr() << ") failed!" << LogStream::eos;
			}
			break;
		}
		default:
			break;
		}
	}
}

void AccountInfo::OnCheckPackageEnoughForTaskRewardsRes(UInt32 taskId, const ItemRewardVec& rewards)
{
	auto taskData = TaskDataEntryMgr::Instance()->FindEntry(taskId);
	if (!taskData)
	{
		ErrorLogStream << ACCOUNTINFO(this) << " can't find task data(" << taskId << ")!" << LogStream::eos;
		return;
	}

	switch (taskData->type)
	{
	case TASK_ACCOUNT_ACHIEVEMENT:
		m_AchievementTasks.OnCheckPackageEnoughForRewardsRes(taskId, rewards);
		break;

	case TASK_ADVENTURE_TEAM_ACCOUNT_WEEKLY:
		m_WeeklyTaskMgr.OnCheckPackageEnoughForRewardsRes(taskId, rewards);
		break;

	default:
		break;
}
}

void AccountInfo::SubmitAccounTask(UInt32 taskId)
{
	auto taskData = TaskDataEntryMgr::Instance()->FindEntry(taskId);
	if (!taskData)
	{
		ErrorLogStream << ACCOUNTINFO(this) << " can't find task data(" << taskId << ")!" << LogStream::eos;
		return;
	}

	switch (taskData->type)
	{
	case TASK_ACCOUNT_ACHIEVEMENT:
	{
		auto task = m_AchievementTasks.FindTask(taskId);
		if (!task)
	    {
			ErrorLogStream << ACCOUNTINFO(this) << " can't find task(" << taskId << ")!" << LogStream::eos;
			return;
		}
		m_AchievementTasks.SubmitTask(task);
		break;
	}

	case TASK_ADVENTURE_TEAM_ACCOUNT_WEEKLY:
	{
		auto task = m_WeeklyTaskMgr.FindTask(taskId);
		if (!task)
		{
			ErrorLogStream << ACCOUNTINFO(this) << " can't find task(" << taskId << ")!" << LogStream::eos;
			return;
		}
		m_WeeklyTaskMgr.SubmitTask(task);
		break;
	}

	default:
		break;
	}
}

UInt32 AccountInfo::GetOverAdventureTeamTasks()
{
	UInt32 totalNum = 0;
	totalNum += m_WeeklyTaskMgr.GetOverAdventureTeamTasks();
	return totalNum;
}

void AccountInfo::AddAccountAchievementScore(WSPlayer* player, UInt32 score)
{
	if (!player) return;
	if (score == 0) return;
	UInt32 oldScore = m_AccountAchievementScore;
	m_AccountAchievementScore = m_AccountAchievementScore + score;
	SendNotify(9057, score);
	
	player->SyncEventToScene(SyncEventType::SET_ACCOUT_ACHIVE_SCORE, m_AccountAchievementScore);

	InfoLogStream << PLAYERINFO(player) << " achievement score(" << oldScore << " | " << score << " >> " << m_AccountAchievementScore << ")" << LogStream::eos;
}

void AccountInfo::Notify(UInt16 type, UInt8 color, const char* word)
{
	if (word == NULL) return;

	CLProtocol::SysNotify notify;
	notify.type = type;
	notify.channel = 0;
	notify.color = color;
	notify.word = word;
	SendProtocol(notify);
}

void AccountInfo::OnPlayerOnline(WSPlayer* player)
{
	if (!player) return;

	InfoLogStream << PLAYERINFO(player) << " on player online..." << LogStream::eos;

	m_RoleIdSet.insert(player->GetID());

	SetRoleID(player->GetID());

	if (!HasAdventureTeam() && m_AdventureTeamLevel > 0)
	{
		m_AdventureTeamLevel = 0;
	}

	if (GetAccountCounterNum(ACC_COUNTER_LOGIC_WEEK_REFRESH) == 0)
	{
		UInt64 nextRefreshTimestamp = 0;
		TimeUtil::RefreshDate refreshDate;
		refreshDate.weekDay = 1;
		refreshDate.hour = 6;
		if (!TimeUtil::CalculateNextRefreshTimestamp(TimeUtil::REFRESH_TYPE_PER_WEEK, refreshDate, nextRefreshTimestamp))
		{
			ErrorLogStream << ACCOUNTINFO(this) << " calculate next refresh timestamp failed by ("
				<< (UInt32)TimeUtil::REFRESH_TYPE_PER_WEEK << ") and (" << refreshDate.LogStr() << ")!" << LogStream::eos;
		}
		else
		{
			SetAccountCounterNumNoSync(ACC_COUNTER_LOGIC_WEEK_REFRESH, nextRefreshTimestamp);
		}
	}
	m_LogicWeekChangeTime = GetAccountCounterNum(ACC_COUNTER_LOGIC_WEEK_REFRESH);

	m_AchievementTasks.OnPlayerOnline(player);
	m_WeeklyTaskMgr.OnPlayerOnline(player);
}

void AccountInfo::OnPlayerOffline(WSPlayer* player)
{
	if (!player) return;

	InfoLogStream << PLAYERINFO(player) << " on player offline..." << LogStream::eos;

	m_RoleIdSet.erase(player->GetID());

	GetMonopolyInfo().OnOffline();
}

void AccountInfo::OnOwnedNpcDied(UInt32 npcid, UInt8 type, UInt32 dungeon)
{
	m_AchievementTasks.OnOwnedNpcDied(npcid, type, dungeon);
}

void AccountInfo::OnPlayerLevelChanged(WSPlayer* player, UInt32 oldLvl)
{
	if (!player) return;

	InfoLogStream << PLAYERINFO(player) << " on player level changed..." << LogStream::eos;

	m_AchievementTasks.OnPlayerLevelChanged(player, oldLvl);
}

void AccountInfo::OnPlayerOccuChanged(WSPlayer* player)
{
	if (!player) return;

	InfoLogStream << PLAYERINFO(player) << " on player occupation changed..." << LogStream::eos;

	m_AchievementTasks.OnPlayerOccuChanged(player);
}

void AccountInfo::OnAdventureTeamLevelChanged()
{
	m_AchievementTasks.OnAdventureTeamLevelChanged();

	WSSortListMgr::Instance()->OnAdventureTeamLevelChanged(SORTLIST_ADVENTURE_TEAM_GRADE, GetAccID(), GetAdventureTeamLevel());
}

void AccountInfo::OnAdventureTeamGradeChanged()
{
	m_AchievementTasks.OnAdventureTeamGradeChanged();
}

void AccountInfo::OnPlayerClearDungeon(const DungeonDataEntry* dungeonData, LevelScore score, UInt32 usedTime)
{
	DebugLogStream << ACCOUNTINFO(this) << " on clear dungeon(" << dungeonData->id << ")..." << LogStream::eos;

	m_WeeklyTaskMgr.OnPlayerClearDungeon(dungeonData, score, usedTime);
}

void AccountInfo::OnRemoveFatigue(UInt32 removeFatigue)
{
	m_WeeklyTaskMgr.OnPlayerRemoveFatigue(removeFatigue);
}

UInt8 AccountInfo::IncLoadDataCount()
{
	++m_LoadDataCount;
	m_LoadDataSerial |= 1 << m_LoadDataCount;
	return m_LoadDataCount;
}

bool AccountInfo::CheckLoadDataSerial(UInt8 serial)
{
	if ((m_LoadDataSerial & (1 << serial)) != 0)
	{
		return true;
	}

	return false;
}

void AccountInfo::SetLoadDataFinished(UInt8 serial)
{
	if (CheckLoadDataSerial(serial))
	{
		m_LoadDataSerial = m_LoadDataSerial & (~(1 << serial));
	}
}

void AccountInfo::OnLoadFinished()
{
	if (m_LoadFinishCallback)
	{
		m_LoadFinishCallback->OnFinish(this);
	}
}

void AccountInfo::OnLoadFailure()
{

}

void AccountInfo::SendProtocol(Avalon::Protocol& protocol)
{
	if (m_Conn == NULL)
	{
		ErrorLogStream << ACCOUNTINFO(this) << " connection is null!" << LogStream::eos;
		return;
	}

	if (GetRoleID() == 0)
	{
		ErrorLogStream << ACCOUNTINFO(this) << " role id is zero!" << LogStream::eos;
		return;
	}

	CLProtocol::GateTransmitPacket req;
	req.id = GetRoleID();
	if (protocol.Encode(req.packet))
	{
		CL_SEND_PROTOCOL(m_Conn, req);
	}
}

void AccountInfo::SendPacket(Avalon::Packet* packet)
{
	if (m_Conn == NULL) return;

	CLProtocol::GateTransmitPacket req(packet);
	req.id = GetRoleID();
	CL_SEND_PROTOCOL(m_Conn, req);
	req.Detach();
}

void AccountInfo::SaveToDB()
{
	CLUpdateCommand* updateCmd = NULL;
	if (WSApplication::Instance()->IsAccountDataFlush())
	{
		updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_account", GetID(), false);
	}
	else
	{
		// 点券有变化，立马刷新到数据库
		if (m_Point.IsDirty(DB_SYNC_TYPE))
		{
			updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_account", GetID(), true);
			InfoLogStream << "account(" << GetAccID() << ") point changed, flush new value(" << m_Point << ") to db." << LogStream::eos;
		}
		else if (m_CreditPoint.IsDirty(DB_SYNC_TYPE))
		{
			// 信用点券有变化，立马刷新到数据库
			updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_account", GetID(), true);
			InfoLogStream << "account(" << GetAccID() << ") creditPoint changed, flush new value(" << m_CreditPoint << ") to db." << LogStream::eos;
		}
		else
		{
			updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_account", GetID(), false);
		}
	}
	
	GetDBData(updateCmd, true);
	if (updateCmd->GetDataNum() > 0)
	{
		CLRecordClient::Instance()->SendCommand(updateCmd);
	}
	else
	{
		CL_SAFE_DELETE(updateCmd);
	}
}

void AccountInfo::InitSecurityState()
{
	if (m_SecurityUnFreezeTime != 0)
	{
		m_SecurityState = SECURITY_STATE_FORCE;
		return;
	}

	if (std::string(m_SecurityPasswd).empty())
		m_SecurityState = SECURITY_STATE_UNLOCK;
	else
		m_SecurityState = SECURITY_STATE_LOCK;
}

void AccountInfo::ParseCommonDevMap()
{
	std::vector<std::string> param_1;
	Avalon::StringUtil::Split(m_CommonDevice, param_1, "|");
	for (auto& iter : param_1)
	{
		if (iter.empty())
			continue;

		std::vector<std::string> param_2;
		Avalon::StringUtil::Split(iter, param_2, ",");
		if (param_2.size() != 2)	// 如果不是由设备id和登录时间构成，则出错
		{
			ErrorLogStream << "account(" << GetAccID() << ") common device " << m_CommonDevice << "error" << LogStream::eos;
			continue;
		}

		std::string id = param_2[0];
		std::string loginTime = param_2[1];
		m_CommonDevMap[id] = loginTime;
	}
}

void AccountInfo::UpdateCommonDevStr()
{
	std::string dev_str;
	dev_str.reserve(1024);
	for (auto& iter : m_CommonDevMap)
	{
		dev_str.append(iter.first).append(",").append(iter.second).append("|");
	}

	m_CommonDevice = dev_str;
}

void AccountInfo::UpdateCommonDevLoginTime(const std::string& deviceID)
{
	auto iter = m_CommonDevMap.find(deviceID);
	if (iter == m_CommonDevMap.end())
		return;

	iter->second = std::to_string(CURRENT_TIME.Sec());
	UpdateCommonDevStr();
}

bool AccountInfo::IsCommonDev(const std::string& deviceID)
{
	auto iter = m_CommonDevMap.find(deviceID);
	if (iter == m_CommonDevMap.end())
		return false;

	return true;
}

void AccountInfo::ClearCommonDev()
{
	m_CommonDevMap.clear();
	m_CommonDevice = "";
}

void AccountInfo::AddCommonDev(const std::string& deviceID)
{
	// 绑定设备数量超过上限
	if (m_CommonDevMap.size() >= SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_BIND_DEVICE_MAX_NUM))
	{
		UInt32 min_time = std::atoi(m_CommonDevMap.begin()->second.c_str());
		std::string id = m_CommonDevMap.begin()->first;

		for (auto& iter : m_CommonDevMap)
		{
			UInt32 iter_time = std::atoi(iter.second.c_str());
			if (min_time > iter_time)
			{
				min_time = iter_time;
				id = iter.first;
			}
		}

		m_CommonDevMap.erase(id);
	}

	m_CommonDevMap[deviceID] = std::to_string(CURRENT_TIME.Sec());
	UpdateCommonDevStr();
}

void AccountInfo::DropCommonDev(const std::string& deviceID)
{
	auto iter = m_CommonDevMap.find(deviceID);
	if (iter == m_CommonDevMap.end())
		return;

	m_CommonDevMap.erase(iter);
	UpdateCommonDevStr();
}

void AccountInfo::GetDeviceCode(std::string& _devCode)
{
	// 设备码随机数
	UInt32 rand_num = Avalon::Random::RandBetween(9999, 99999999);

	_devCode.clear();
	_devCode.append(std::to_string(m_Accid)).append("_");
	_devCode.append(std::to_string(rand_num)).append("_");
	_devCode.append(std::to_string(UInt32(CURRENT_TIME.Sec())));
}

bool AccountInfo::CheckPasswdLength(const std::string& passwd)
{
	static UInt32 security_passwd_min = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_SECURITY_LOCK_PASSWD_MIN_LENGTH);
	static UInt32 security_passwd_max = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_SECURITY_LOCK_PASSWD_MAX_LENGTH);

	if (passwd.length() < security_passwd_min || passwd.length() > security_passwd_max)
		return false;

	return true;
}

bool AccountInfo::CheckSecurityLockCrossDay()
{
	Avalon::Time lastResetTime(GetPasswdErrorNumResetTime());
	UInt64 lastResetDay = GetDayBeginTime2(lastResetTime);
	UInt64 currentDay = GetDayBeginTime2(CURRENT_TIME);

	if (lastResetDay != currentDay)
	{
		SetPasswdErrorNumResetTime(CURRENT_TIME.MSec());
		ClearSecurityPasswdErrorNum();
		SaveToDB();
	}

	return true;
}

UInt32 AccountInfo::SetTaskItem(UInt32 taskId, std::vector<WorldSetTaskItemStruct>& items)
{
	auto taskData = TaskDataEntryMgr::Instance()->FindEntry(taskId);
	if (!taskData)
	{
		ErrorLogStream << ACCOUNTINFO(this) << " can't find task data(" << taskId << ")!" << LogStream::eos;
		return ErrorCode::TASK_NOT_EXIST;
	}

	switch (taskData->type)
	{
	case TASK_ADVENTURE_TEAM_ACCOUNT_WEEKLY:
	{
		return m_WeeklyTaskMgr.SetTaskItem(taskId, items);
	}

	default:
		break;
	}

	return ErrorCode::TASK_SET_ITEM_TASK_TYPE_ERROR;
}

