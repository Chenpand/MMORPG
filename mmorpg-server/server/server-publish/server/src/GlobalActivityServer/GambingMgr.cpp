#include "GambingMgr.h"

#include <CLRecordClient.h>
#include <CLSystemValueDataEntry.h>
#include <CLDrawPrizeDataEntry.h>
#include <TimeUtil.h>
#include <CLActiveEventDataEntry.h>
#include <CLMsgParser.h>
#include <CLErrorCode.h>
#include <udplog_typedef.h>
#include <DropItemMgr.h>
#include <CLItemDataEntry.h>
#include <CLSysNotify.h>
#include <CLAnnouncementDataEntry.h>
#include <CLGlobalActivityServerDefine.h>
#include <CLSysProtocol.h>
#include <CLChannelNameSwitchDataEntry.h>

#include "ActivityMgr.h"
#include "GASPlayerMgr.h"
#include "GASItemTransaction.h"
#include "GASGameZoneMgr.h"
#include "GASAnnouncementMgr.h"
#include "Utility.h"


// 系统玩家
const static char* SYSTEM_PLAYER = "system";

/**
*@brief 夺宝购买商品扣除道具事务
*/
class GambingPayingRmItemTransaction : public GASItemTransaction
{
public:
	GambingPayingRmItemTransaction(GASPlayer *player, const PayingGambleContext& payingGambleContext)
		: GASItemTransaction(player), m_PayingGambleContext(payingGambleContext) {}
	~GambingPayingRmItemTransaction() {}

	void OnCheckSucced(GASPlayer *player)
	{
		if (GambingMgr::Instance()->GetGambingActStatus() == AS_END)
		{
			ErrorLogStream << PLAYERINFO(player) << "Gambing activity was end!" << LogStream::eos;
			m_PayingGambleContext.payingResProtocol.retCode = ErrorCode::ITEM_NO_REASON;
			UnLock();
			player->SendProtocol(m_PayingGambleContext.payingResProtocol);
			return;
		}

		if (m_PayingGambleContext.itemPool == NULL)
		{
			ErrorLogStream << PLAYERINFO(player) << "GambingItemPool ptr is null!" << LogStream::eos;
			m_PayingGambleContext.payingResProtocol.retCode = ErrorCode::ITEM_OFF_SALE;
			UnLock();
			player->SendProtocol(m_PayingGambleContext.payingResProtocol);
			return;
		}

		if (m_PayingGambleContext.groupInfo == NULL)
		{
			ErrorLogStream << PLAYERINFO(player) << "GambingItemGroupInfo ptr is null!" << LogStream::eos;
			m_PayingGambleContext.payingResProtocol.retCode = ErrorCode::ITEM_OFF_SALE;
			UnLock();
			player->SendProtocol(m_PayingGambleContext.payingResProtocol);
			return;
		}

		// 商品不可以出售
		if (m_PayingGambleContext.groupInfo->groupStatus != GIS_SELLING)
		{
			ErrorLogStream << PLAYERINFO(player) << m_PayingGambleContext.groupInfo->LogFormatStr() << "'s status is error!" << LogStream::eos;
			m_PayingGambleContext.payingResProtocol.retCode = ErrorCode::ITEM_NOT_SELL;
			UnLock();
			player->SendProtocol(m_PayingGambleContext.payingResProtocol);
			return;
		}

		// 商品没有份数了
		if (m_PayingGambleContext.groupInfo->restCopies == 0)
		{
			if (m_PayingGambleContext.itemPool->restGroups == 0)
			{
				m_PayingGambleContext.payingResProtocol.retCode = ErrorCode::ITEM_GAMBLE_ALL_SOLD_OUT;
			}
			else
			{
				m_PayingGambleContext.payingResProtocol.retCode = ErrorCode::ITEM_SOLD_OUT;
			}

			ErrorLogStream << PLAYERINFO(player) << m_PayingGambleContext.groupInfo->LogFormatStr() << "'s rest copies are empty!" << LogStream::eos;
			UnLock();
			player->SendProtocol(m_PayingGambleContext.payingResProtocol);
			return;
		}

		Int32 tmpCopies = (Int32)m_PayingGambleContext.groupInfo->restCopies;

		// 一键购买
		if (m_PayingGambleContext.payingReqProtocol.bBuyAll)
		{
			// 事务返回后，剩余份额只可能小于或者等于一键购买的份额
			if (m_PayingGambleContext.payingResProtocol.investCopies < (UInt32)tmpCopies)
			{
				ErrorLogStream << PLAYERINFO(player) << m_PayingGambleContext.groupInfo->LogFormatStr() << "'s rest copies(" << tmpCopies << ") are more than invest copies="
					<< m_PayingGambleContext.payingResProtocol.investCopies << ")!" << LogStream::eos;
				m_PayingGambleContext.payingResProtocol.retCode = ErrorCode::ITEM_NO_REASON;
				UnLock();
				player->SendProtocol(m_PayingGambleContext.payingResProtocol);
				return;
			}
			m_PayingGambleContext.payingResProtocol.investCopies = tmpCopies;
			SetItem(m_PayingGambleContext.itemPool->costCurrencyId, m_PayingGambleContext.itemPool->unitPrice * (UInt32)tmpCopies);
			DebugLogStream << PLAYERINFO(player) << m_PayingGambleContext.groupInfo->LogFormatStr() << " buy all copies(" << tmpCopies << ")." << LogStream::eos;
		}

		tmpCopies -= (Int32)m_PayingGambleContext.payingResProtocol.investCopies;

		if (tmpCopies < 0)
		{
			ErrorLogStream << PLAYERINFO(player) << m_PayingGambleContext.groupInfo->LogFormatStr() << "'s copies over flow!" << LogStream::eos;
			m_PayingGambleContext.payingResProtocol.retCode = ErrorCode::ITEM_COPIES_NOT_ENOUGH;
			UnLock();
			player->SendProtocol(m_PayingGambleContext.payingResProtocol);
			return;
		}

		m_PayingGambleContext.groupInfo->restCopies = (UInt32)tmpCopies;

		bool bNewRecord = false;
		// 记录
		GambingRecord* gambingRecord = GambingMgr::Instance()->FindGambingRecord(player->GetUId(), m_PayingGambleContext.groupInfo->gambingItemId, m_PayingGambleContext.groupInfo->groupId);
		if (!gambingRecord)
		{
			gambingRecord = new GambingRecord();
			
			gambingRecord->SetOwnerUId(player->GetUId());
			gambingRecord->SetOwnerName(player->GetPlayerName());
			gambingRecord->SetOwnerServerName(player->GetServerName());
			gambingRecord->SetOwnerSourceCenterNode(player->GetSourceNode(ST_CENTER));
			gambingRecord->SetOwnerSourceAdminNode(player->GetSourceNode(ST_ADMIN));
			gambingRecord->SetGambingItemId(m_PayingGambleContext.groupInfo->gambingItemId);
			gambingRecord->SetGambingItemNum(m_PayingGambleContext.itemPool->gambingItemNum);
			gambingRecord->SetItemDataId(m_PayingGambleContext.itemPool->itemDataId);
			gambingRecord->SetGambingGroupId(m_PayingGambleContext.groupInfo->groupId);
			gambingRecord->SetInvestCurrencyId(m_PayingGambleContext.itemPool->costCurrencyId);
			gambingRecord->SetCreateTimeSec((UInt32)Avalon::Time::CurrentTime().Sec());

			// 添加player的组记录
			if (!GambingMgr::Instance()->AddGambingRecord(gambingRecord))
			{
				ErrorLogStream << PLAYERINFO(player) << "Add gambing record(" << gambingRecord->GetGambingItemId() << ", " << gambingRecord->GetGambingGroupId() << ") faild!" << LogStream::eos;
				delete gambingRecord;
				m_PayingGambleContext.payingResProtocol.retCode = ErrorCode::ITEM_NOT_SELL;
				UnLock();
				player->SendProtocol(m_PayingGambleContext.payingResProtocol);
				return;
			}

			bNewRecord = true;
		}

		gambingRecord->AddInvestCurrencyNum(m_PayingGambleContext.payingResProtocol.investCopies * m_PayingGambleContext.itemPool->unitPrice);
		gambingRecord->AddInvestCopies(m_PayingGambleContext.payingResProtocol.investCopies);
		gambingRecord->SetGambingWeight(gambingRecord->GetInvestCopies());

		GambingMgr::Instance()->CalculateGambingWeight(m_PayingGambleContext.itemPool, gambingRecord);

		// 更新参与者信息
		GambingMgr::Instance()->UpdateParticipantInfo(gambingRecord);

		if (bNewRecord)
		{
			gambingRecord->InsertToDB();
		}
		else
		{
			gambingRecord->UpdateToDB();
		}

		RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_GAMBING_PAYING, gambingRecord->GetItemDataId(), m_PayingGambleContext.payingResProtocol.investCopies));

		GambingMgr::Instance()->GetGambingItemInfo(m_PayingGambleContext.itemPool, m_PayingGambleContext.groupInfo, player->GetUId(), m_PayingGambleContext.payingResProtocol.itemInfo);
		player->SendProtocol(m_PayingGambleContext.payingResProtocol);

		// 该组卖完了
		if (m_PayingGambleContext.groupInfo->restCopies == 0)
		{
			m_PayingGambleContext.groupInfo->soldOutTimestamp = (UInt32)Avalon::Time::CurrentTime().Sec();
			GambingMgr::Instance()->OnGambingItemGroupSoldOut(m_PayingGambleContext, player);
			GambingMgr::Instance()->OnGambingItemGroupLottery(m_PayingGambleContext.groupInfo);
		}

		InfoLogStream << PLAYERINFO(player) << "Pay for " << m_PayingGambleContext.groupInfo->LogFormatStr() << " success, buy "
			<< m_PayingGambleContext.payingResProtocol.investCopies << " copies." << LogStream::eos;
	}

	void OnCheckFailed(GASPlayer *player)
	{
		ErrorLogStream << PLAYERINFO(player) << "Check failed!" << LogStream::eos;
		m_PayingGambleContext.payingResProtocol.retCode = ErrorCode::ITEM_NOT_ENOUGH_MONEY;
		UnLock();
		player->SendProtocol(m_PayingGambleContext.payingResProtocol);
	}

private:
	PayingGambleContext m_PayingGambleContext;
};

/**
*@brief 夺宝记录查询
*/
class SelectGambingRecordCallBack : public CLRecordCallback
{
public:
	void OnFinished()
	{
		if (!GambingMgr::Instance()->OnLoadGambingRecord(this))
		{
			ErrorLogStream << "On load t_gambing_record failed!" << LogStream::eos;
			GambingMgr::Instance()->SetGambingActStatus(AS_END);
			return;
		}

		GambingMgr::Instance()->SetGambingActStatus(AS_IN);
	}

	void OnFailed(UInt32 errorcode)
	{
		ErrorLogStream << "Select database(t_gambing_record) failed, errorcode=" << errorcode << "!" << LogStream::eos;
	}
};

/**
*@brief 夺宝开奖记录查询
*/
class SelectGambingLotteryRecordCallBack : public CLRecordCallback
{
public:
	SelectGambingLotteryRecordCallBack(const OpActivityInfo* info) : m_GambingAct(info) {}

public:
	void OnFinished()
	{
		if (m_GambingAct->startTime == 0 || m_GambingAct->endTime == 0)
		{
			ErrorLogStream << "Gambing activity start time(" << m_GambingAct->startTime << ") or end time(" << m_GambingAct->endTime << ") is zero!" << LogStream::eos;
			GambingMgr::Instance()->SetGambingActStatus(AS_END);
			return;
		}

		if (!GambingMgr::Instance()->OnLoadGambingLotteryRecord(this))
		{
			ErrorLogStream << "On load t_gambing_lottery_record failed!" << LogStream::eos;
			GambingMgr::Instance()->SetGambingActStatus(AS_END);
			return;
		}

		CLSelectCommand* cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_gambing_record");
		std::ostringstream limitoss;
		limitoss << " WHERE (`create_time_sec` >= " << m_GambingAct->startTime << " AND `create_time_sec` <= " << m_GambingAct->endTime << ")";
		cmd->SetLimit(limitoss.str());
		CLRecordClient::Instance()->SendCommand(cmd, new SelectGambingRecordCallBack());
	}

	void OnFailed(UInt32 errorcode)
	{
		ErrorLogStream << "Select database(t_gambing_lottery_record) failed, errorcode=" << errorcode << "!" << LogStream::eos;
	}

private:
	const OpActivityInfo* m_GambingAct;
};

GambingMgr::GambingMgr()
{
	// 初始化记得放在活动管理初始化之前
	m_SecondPerTimer.SetInterval(1000);
	m_GambingActivityStatus = AS_END;
}

GambingMgr::~GambingMgr()
{
	_Destroy();
}

bool GambingMgr::OnInit(const OpActivityInfo* opAct)
{
	if (!opAct) return false;

	if (!LoadTable(opAct))
	{
		ErrorLogStream << "Load table failed!" << LogStream::eos;
		return false;
	}

	for (auto itemPool : m_GambingItemPool)
	{
		if (!itemPool) continue;

		_OnInitGambingFirstGroup(opAct, itemPool);
	}

	return true;
}

void GambingMgr::Final()
{
	_Destroy();
}

bool GambingMgr::LoadTable(const OpActivityInfo* opAct, bool bReload)
{
	if (!opAct) return false;

	InfoLogStream << "Start load table which activity(" << opAct->id << ")." << LogStream::eos;

	class GambingItemDataEntryVisitor : public Avalon::DataEntryVisitor<GambingItemDataEntry>
	{
	public:
		bool operator()(GambingItemDataEntry* data)
		{
			datas.push_back(data);
			return true;
		}

		std::vector<GambingItemDataEntry*> datas;
	};

	GambingItemDataEntryVisitor visitor;
	GambingItemDataEntryMgr::Instance()->Visit(visitor);

	if (visitor.datas.empty())
	{
		ErrorLogStream << "Table(GambingItemTable.txt) is empty!" << LogStream::eos;
		return false;
	}

	for (const auto data : visitor.datas)
	{
		if (!data) continue;

		if (data->actId != opAct->id) continue;

		GambingItemPool* itemPool = new GambingItemPool();
		itemPool->gambingItemId = data->gambingItemId;
		itemPool->gambingItemNum = data->gambingItemNum;
		itemPool->sortId = data->sortId;
		itemPool->itemDataId = data->itemDataId;
		itemPool->totalGroups = data->totalGroups;
		itemPool->copiesOfEachGroup = data->copiesOfEachGroup;
		itemPool->costCurrencyId = data->costCurrencyId;
		itemPool->unitPrice = data->unitPrice;
		itemPool->coolDownIntervalBetweenGroups = data->coolDownIntervalBetweenGroups;
		itemPool->rewardsPerCopy = data->rewardsPerCopy;
		itemPool->weightingStepSize = data->weightingStepSize;
		itemPool->weightingValue = data->weightingValue;

		itemPool->restGroups = data->totalGroups;
		itemPool->itemPoolStatus = GIS_INIT;

		if (bReload)
		{
			m_TmpGambingItemPool.push_back(itemPool);
		}
		else
		{
			if (FindGambingItemPool(itemPool->gambingItemId) != NULL)
			{
				delete itemPool;
				continue;
			}
			m_GambingItemPool.push_back(itemPool);
		}

		InfoLogStream << "Load from table success, " << itemPool->LogFormatStr() << LogStream::eos;
	}

	return true;
}

bool GambingMgr::ReloadTable()
{
	InfoLogStream << "Start reloading..." << LogStream::eos;

	m_TmpGambingItemPool.clear();

	const OpActivityInfo* opAct = NULL;
	ActivityMgr::Instance()->VisitOpActivities([&opAct](OpActivityInfo* elem)->bool{
		if (elem->type == OAT_GAMBING && elem->status != AS_END)
		{
			opAct = elem;
			return false;
		}
		return true;
	});

	if (!opAct)
	{
		ErrorLogStream << "Can not find open activity(" << (UInt32)OAT_GAMBING << ")!" << LogStream::eos;
		return false;
	}

	if (!LoadTable(opAct, true))
	{
		ErrorLogStream << "Load table failed!" << LogStream::eos;
		return false;
	}

	for (auto itemPool : m_GambingItemPool)
	{
		if (!itemPool) continue;

		bool bFindInTmpPool = false;

		for (auto tmpPoolIter = m_TmpGambingItemPool.begin(); tmpPoolIter != m_TmpGambingItemPool.end(); ++tmpPoolIter)
		{
			GambingItemPool* tmpPool = *tmpPoolIter;
			if (!tmpPool) continue;

			if (tmpPool->gambingItemId == itemPool->gambingItemId)
			{
				itemPool->itemPoolStatus = GIS_INIT;
				bFindInTmpPool = true;
				delete tmpPool;
				m_TmpGambingItemPool.erase(tmpPoolIter);
				break;
			}
		}

		if (!bFindInTmpPool)
		{
			InfoLogStream << "Can not find gambing item(" << itemPool->gambingItemId << ") in memory, set off sale!" << LogStream::eos;
			// 现有内存中的数据在重载后的表格中找不到,则下架
			itemPool->itemPoolStatus = GIS_OFF_SALE;
		}
	}

	// 说明有新加的数据
	if (!m_TmpGambingItemPool.empty())
	{
		for (auto elem : m_TmpGambingItemPool)
		{
			if (!elem) continue;

			m_GambingItemPool.push_back(elem);
			InfoLogStream << "Add a new gambing item(" << elem->gambingItemId << ")." << LogStream::eos;

			_OnInitGambingFirstGroup(opAct, elem);
		}
	}

	m_TmpGambingItemPool.clear();

	return true;
}

bool GambingMgr::LoadDB(const OpActivityInfo* opAct)
{
	if (!opAct) return false;

	if (opAct->startTime == 0 || opAct->endTime == 0)
	{
		ErrorLogStream << "Gambing activity start time(" << opAct->startTime << ") or end time(" << opAct->endTime << ") is zero!" << LogStream::eos;
		return false;
	}

	CLSelectCommand* cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_gambing_lottery_record");
	std::ostringstream limitoss;
	limitoss << " WHERE (`create_time_sec` >= " << opAct->startTime << " AND `create_time_sec` <= " << opAct->endTime << ")";
	cmd->SetLimit(limitoss.str());
	CLRecordClient::Instance()->SendCommand(cmd, new SelectGambingLotteryRecordCallBack(opAct));

	return true;
}

bool GambingMgr::OnLoadGambingRecord(CLRecordCallback* callback)
{
	if (!callback) return false;

	while (callback->NextRow())
	{
		GambingRecord* gambingRecord = new GambingRecord();
		gambingRecord->SetID(callback->GetKey());
		gambingRecord->SetDBData(callback);

		if (!_OnInitDataFromDB(gambingRecord))
		{
			ErrorLogStream << "Init data from database failed, gambing record(" << gambingRecord->GetGambingItemId() << ")!" << LogStream::eos;
			delete gambingRecord;
			continue;
		}

		// 添加player的组记录
		if (!AddGambingRecord(gambingRecord))
		{
			ErrorLogStream << "Add gambing record(" << gambingRecord->GetGambingItemId() << ", " << gambingRecord->GetGambingGroupId() << ") faild!" << LogStream::eos;
			delete gambingRecord;
			continue;
		}

		DebugLogStream << "Gambing record(" << gambingRecord->GetGambingItemId() << ", " << gambingRecord->GetGambingGroupId() << "), create time=" 
			<< TimeUtil::TimestampToStandardFormat(gambingRecord->GetCreateTimeSec()) << LogStream::eos;
	}

	if (m_GambingRecords.empty())
	{
		ErrorLogStream << "Gambing records are empty!" << LogStream::eos;
	}

	// 更新每个池子当前售卖的最后一组
	for (auto itemPool : m_GambingItemPool)
	{
		if (!itemPool) continue;

		if (itemPool->itemPoolStatus == GIS_OFF_SALE)
		{
			ErrorLogStream << itemPool->LogFormatStr() << " off sale!" << LogStream::eos;
			continue;
		}

		if (itemPool->restGroups == 0)
		{
			DebugLogStream << itemPool->LogFormatStr() << " rest groups are zero!" << LogStream::eos;
			continue;
		}

		if (itemPool->totalGroups < itemPool->restGroups)
		{
			ErrorLogStream << itemPool->LogFormatStr() << " total groups(" << itemPool->totalGroups << ") are less than rest groups(" << itemPool->restGroups << ")!" << LogStream::eos;
			continue;
		}

		UInt16 lastGroupId = itemPool->totalGroups - itemPool->restGroups;
		GambingItemGroupInfo* groupInfo = FindGambingGroupInfo(itemPool->gambingItemId, lastGroupId);
		if (!groupInfo)
		{
			ErrorLogStream << "Can not find gambing group info(" << itemPool->gambingItemId << ", " << lastGroupId << ")!" << LogStream::eos;
			continue;
		}

		if (groupInfo->restCopies != 0)
		{
			continue;
		}

		// 如果最后一组卖完了，需要生成下一组
		GambingItemGroupInfo* nextNewGroup = _CreateGambingItemGroup(itemPool);
		if (!nextNewGroup)
		{
			ErrorLogStream << "Create gambing item group failed!" << LogStream::eos;
			continue;
		}

		_CalculateGroupSellBeginTimeSec(itemPool, nextNewGroup);

		itemPool->restGroups -= 1;

		DebugLogStream << "Add the new group(" << nextNewGroup->LogFormatStr() << ")." << LogStream::eos;
	}

	for (auto itemPool : m_GambingItemPool)
	{
		if (!itemPool) continue;

		DebugLogStream << itemPool->LogFormatStr() << LogStream::eos;
	}

	return true;
}

bool GambingMgr::OnLoadGambingLotteryRecord(CLRecordCallback* callback)
{
	if (!callback) return false;

	while (callback->NextRow())
	{
		GambingLotteryRecord* dbRecord = new GambingLotteryRecord();
		dbRecord->SetID(callback->GetKey());
		dbRecord->SetDBData(callback);

		GambingLotteryRecord* memRecord = FindGambingLotteryRecord(dbRecord->GetGambingItemId(), dbRecord->GetGroupId());
		if (memRecord != NULL)
		{
			ErrorLogStream << "Add gambing lottery record(" << dbRecord->GetGambingItemId() << ", " << dbRecord->GetGroupId() << ") again!" << LogStream::eos;
			delete dbRecord;
			continue;
		}

		AddGambingLotteryRecord(dbRecord);

		DebugLogStream << "Gambing lottery(" << dbRecord->GetGambingItemId() << ", " << dbRecord->GetGroupId() << "), soldout time=" << TimeUtil::TimestampToStandardFormat(dbRecord->GetSoldOutTimeSec())
			<< ", create time=" << TimeUtil::TimestampToStandardFormat(dbRecord->GetCreateTimeSec()) << LogStream::eos;
	}

	if (m_GambingLotteryRecord.empty())
	{
		ErrorLogStream << "Gambing lottery record is empty!" << LogStream::eos;
	}

	return true;
}

bool GambingMgr::AddGambingRecord(GambingRecord* record)
{
	if (!record) return false;

	if (FindGambingRecord(record->GetOwnerUId(), record->GetGambingItemId(), record->GetGambingGroupId()) != NULL)
	{
		ErrorLogStream << "Player(" << record->GetOwnerUId() << ") want to add gambing record(" << record->GetGambingItemId() << ", " << record->GetGambingGroupId() << ") again!" << LogStream::eos;
		return false;
	}

	m_GambingRecords[record->GetOwnerUId()].push_back(record);
	return true;
}

bool GambingMgr::AddGambingLotteryRecord(const GambingItemPool* itemPool, const GambingItemGroupInfo* groupInfo, const std::string& gainer)
{
	if (!itemPool || !groupInfo) return false;

	GambingLotteryRecord* record = FindGambingLotteryRecord(groupInfo->gambingItemId, groupInfo->groupId);
	if (record != NULL)
	{
		ErrorLogStream << "Add " << groupInfo->LogFormatStr() << " lottery again!" << LogStream::eos;
		return false;
	}

	record = new GambingLotteryRecord();
	record->SetGambingItemId(groupInfo->gambingItemId);
	record->SetGambingItemNum(itemPool->gambingItemNum);
	record->SetItemDataId(itemPool->itemDataId);
	record->SetGroupId(groupInfo->groupId);
	record->SetSoldOutTimeSec(groupInfo->soldOutTimestamp);
	record->SetLotteryTimeSec((UInt32)Avalon::Time::CurrentTime().Sec());
	record->SetGainerUId(gainer);
	record->SetCreateTimeSec((UInt32)Avalon::Time::CurrentTime().Sec());

	record->InsertToDB();

	AddGambingLotteryRecord(record);

	InfoLogStream << "Gainer(" << gainer << ") got " << groupInfo->LogFormatStr() << LogStream::eos;

	return true;
}

void GambingMgr::AddGambingLotteryRecord(GambingLotteryRecord* record)
{
	m_GambingLotteryRecord.push_back(record);
}

bool GambingMgr::AddGambingItemGroupInfo(GambingItemGroupInfo* group)
{
	if (!group) return false;

	GambingItemGroupKey key(group->gambingItemId, group->groupId);

	auto iter = m_GambingItemGroups.find(key);
	if (iter != m_GambingItemGroups.end())
	{
		ErrorLogStream << "Add the same group(" << key.gambingItemId << ", " << key.groupId << ")!" << LogStream::eos;
		return false;
	}

	m_GambingItemGroups[key] = group;
	return true;
}

GambingItemPool* GambingMgr::FindGambingItemPool(UInt32 gambingItemId)
{
	for (auto pool : m_GambingItemPool)
	{
		if (!pool) continue;

		if (pool->gambingItemId == gambingItemId) return pool;
	}

	return NULL;
}

GambingItemGroupInfo* GambingMgr::FindGambingGroupInfo(UInt32 gambingItemId, UInt16 groupId)
{
	auto itr = m_GambingItemGroups.find(GambingItemGroupKey(gambingItemId, groupId));
	if (itr != m_GambingItemGroups.end())
	{
		return itr->second;
	}

	return NULL;
}

GambingRecord* GambingMgr::FindGambingRecord(const std::string& ownerUId, UInt32 gambingItemId, UInt16 groupId)
{
	auto iter = m_GambingRecords.find(ownerUId);
	if (iter == m_GambingRecords.end())
	{
		return NULL;
	}

	for (auto gambingRecord : iter->second)
	{
		if (!gambingRecord) continue;

		if (gambingRecord->GetGambingItemId() == gambingItemId &&
			gambingRecord->GetGambingGroupId() == groupId)
		{
			return gambingRecord;
		}
	}

	return NULL;
}

void GambingMgr::FindGambingRecords(const std::string& ownerUId, GambingRecordVec& records)
{
	auto iter = m_GambingRecords.find(ownerUId);
	if (iter != m_GambingRecords.end())
	{
		records = iter->second;
	}
}

void GambingMgr::FindGambingParticipants(UInt32 gambingItemId, UInt16 groupId, GambingParticipantVec& participants)
{
	auto itr = m_ParticipantsMap.find(GambingItemGroupKey(gambingItemId, groupId));
	if (itr != m_ParticipantsMap.end())
	{
		participants = itr->second;
	}
}

GambingLotteryRecord* GambingMgr::FindGambingLotteryRecord(UInt32 gambingItemId, UInt16 groupId)
{
	for (auto lotteryData : m_GambingLotteryRecord)
	{
		if (!lotteryData) continue;

		if (lotteryData->GetGambingItemId() == gambingItemId &&
			lotteryData->GetGroupId() == groupId)
		{
			return lotteryData;
		}
	}

	return NULL;
}

void GambingMgr::OnTick(const Avalon::Time &now)
{
	if (GetGambingActStatus() == AS_END) return;

	if (m_SecondPerTimer.IsTimeout(now))
	{
		for (auto elem : m_GambingItemGroups)
		{
			GambingItemGroupInfo* groupInfo = elem.second;
			if (!groupInfo) continue;

			if (groupInfo->groupStatus == GIS_PREPARE)
			{
				if ((UInt32)now.Sec() >= groupInfo->sellBeginTime)
				{
					groupInfo->groupStatus = GIS_SELLING;
				}
			}
		}
	}
}

void GambingMgr::OnActivityPrepare(const OpActivityInfo* opAct)
{
	if (!opAct) return;

	InfoLogStream << "Activity(" << opAct->id << ") prepare..." << LogStream::eos;

	if (!OnInit(opAct))
	{
		ErrorLogStream << "Init failed!" << LogStream::eos;
		return;
	}

	SetGambingActStatus(AS_PREPARE);
}

void GambingMgr::OnActivityStart(const OpActivityInfo* opAct)
{
	if (!opAct) return;

	InfoLogStream << "Activity(" << opAct->id << ") start..." << LogStream::eos;

	if (!OnInit(opAct))
	{
		ErrorLogStream << "Load table failed!" << LogStream::eos;
		return;
	}

	if (!LoadDB(opAct))
	{
		ErrorLogStream << "Load database failed!" << LogStream::eos;
		return;
	}

	if (opAct->endTime == 0)
	{
		ErrorLogStream << "Gambing activity end time(" << opAct->endTime << ") is zero!" << LogStream::eos;
		return;
	}

	UInt32 startAnnounceInterval = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GAMBING_BEGIN_ANNOUNCE_INTERVAL);
	if (startAnnounceInterval == 0)
	{
		ErrorLogStream << "Can't find system value(" << (UInt32)SVT_GAMBING_BEGIN_ANNOUNCE_INTERVAL << ")!" << LogStream::eos;
		return;
	}

	UInt32 announceInterval = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GAMBING_ANNOUNCE_INTERVAL);
	if (announceInterval == 0)
	{
		ErrorLogStream << "Can't find system value(" << (UInt32)SVT_GAMBING_ANNOUNCE_INTERVAL << ")!" << LogStream::eos;
		return;
	}

	UInt32 announceId = 69;
	AnnouncementDataEntry* dataEntry = AnnouncementDataEntryMgr::Instance()->FindEntry(announceId);
	if (dataEntry == NULL) return;

	if (opAct->endTime < startAnnounceInterval * MIN_TO_SEC)
	{
		ErrorLogStream << "Gambing start time over flow!" << LogStream::eos;
		return;
	}

	// 结束前的公告
	UInt32 startTime = opAct->endTime - startAnnounceInterval * MIN_TO_SEC;
	UInt32 endTime = opAct->endTime;
	while (startTime < endTime)
	{
		UInt32 notifyTime = (endTime - startTime) / MIN_TO_SEC;

		char buffer[CL_MAX_CHAT_LEN + 1];
		if (!SysNotifyMgr::Instance()->MakeNotify(dataEntry->content.c_str(), buffer, CL_MAX_CHAT_LEN, notifyTime))
		{
			ErrorLogStream << "Make notify failed, announcement data entry(65), by param(" << notifyTime << ")!" << LogStream::eos;
			continue;
		}

		AnnouncementData* data = new AnnouncementData();
		data->id = announceId;
		data->begintime = startTime;
		data->interval = dataEntry->interval;
		data->times = dataEntry->times;
		data->content = buffer;
		data->isSave = 1;
		data->gmId = 0;

		GASAnnouncementMgr::Instance()->AddAnnouncement(data);
		startTime += announceInterval * MIN_TO_SEC;
	}
}

void GambingMgr::OnActivityEnd(const OpActivityInfo* opAct)
{
	if (opAct != NULL)
	{
		InfoLogStream << "Activity(" << opAct->id << ") end..." << LogStream::eos;
	}

	SetGambingActStatus(AS_END);

	for (auto itemPool : m_GambingItemPool)
	{
		if (!itemPool) continue;

		// 活动结束后将当前正在展示的没有开过奖的全部开掉
		UInt32 showGroupId = itemPool->totalGroups - itemPool->restGroups;

		auto showGroup = FindGambingGroupInfo(itemPool->gambingItemId, showGroupId);
		if (!showGroup)
		{
			ErrorLogStream << "Can not find group(" << showGroupId << ") in pool " << itemPool->LogFormatStr() << LogStream::eos;
			continue;
		}

		// 已经开奖了 || 没有人买过的
		if (showGroup->groupStatus == GIS_LOTTERY || showGroup->restCopies == itemPool->copiesOfEachGroup)
		{
			continue;
		}

		// 没有买完的，需要系统购完
		if (showGroup->restCopies != 0)
		{
			GambingRecord* gambingRecord = GambingMgr::Instance()->FindGambingRecord(SYSTEM_PLAYER, showGroup->gambingItemId, showGroup->groupId);
			if (gambingRecord != NULL)
			{
				// 一般没有这种情况
				ErrorLogStream << "Find system record(" << showGroup->gambingItemId << ", groupid=" << showGroup->groupId << ")!" << LogStream::eos;
				continue;
			}

			gambingRecord = new GambingRecord();

			gambingRecord->SetOwnerUId(SYSTEM_PLAYER);
			gambingRecord->SetGambingItemId(showGroup->gambingItemId);
			gambingRecord->SetGambingItemNum(itemPool->gambingItemNum);
			gambingRecord->SetItemDataId(itemPool->itemDataId);
			gambingRecord->SetGambingGroupId(showGroup->groupId);
			gambingRecord->SetInvestCurrencyId(itemPool->costCurrencyId);
			gambingRecord->SetCreateTimeSec((UInt32)CURRENT_TIME.Sec());

			// 添加player的组记录
			if (!GambingMgr::Instance()->AddGambingRecord(gambingRecord))
			{
				ErrorLogStream << SYSTEM_PLAYER << "Add gambing record(" << gambingRecord->GetGambingItemId() << ", " << gambingRecord->GetGambingGroupId() << ") faild!" << LogStream::eos;
				delete gambingRecord;
				continue;
			}

			gambingRecord->AddInvestCurrencyNum(showGroup->restCopies * itemPool->unitPrice);
			gambingRecord->AddInvestCopies(showGroup->restCopies);
			gambingRecord->SetGambingWeight(gambingRecord->GetInvestCopies());

			CalculateGambingWeight(itemPool, gambingRecord);

			UpdateParticipantInfo(gambingRecord);

			gambingRecord->InsertToDB();

			InfoLogStream << SYSTEM_PLAYER << "Pay for " << showGroup->LogFormatStr() << " success, buy " << gambingRecord->GetInvestCopies() << " copies." << LogStream::eos;
		}

		OnGambingItemGroupLottery(showGroup);
	}

	_Destroy();
}

void GambingMgr::OnGambingItemGroupSoldOut(PayingGambleContext& payingGambleContext, GASPlayer *player)
{
	auto soldOutGambingGroup = payingGambleContext.groupInfo;

	if (!soldOutGambingGroup || !player) return;

	InfoLogStream << soldOutGambingGroup->LogFormatStr() << " sold out." << LogStream::eos;

	if (soldOutGambingGroup->groupStatus == GIS_SOLD_OUE)
	{
		return;
	}

	soldOutGambingGroup->groupStatus = GIS_SOLD_OUE;

	GambingItemPool* itemPool = FindGambingItemPool(soldOutGambingGroup->gambingItemId);
	if (!itemPool)
	{
		ErrorLogStream << "Can not find gambing item pool(" << soldOutGambingGroup->gambingItemId << ")!" << LogStream::eos;
		return;
	}

	if (itemPool->itemPoolStatus == GIS_OFF_SALE)
	{
		ErrorLogStream << itemPool->LogFormatStr() << " is off sale!" << LogStream::eos;
		return;
	}

	if (itemPool->restGroups == 0)
	{
		DebugLogStream << itemPool->LogFormatStr() << " groups are empty!" << LogStream::eos;
		return;
	}

	GambingItemGroupInfo* group = _CreateGambingItemGroup(itemPool);
	if (!group)
	{
		ErrorLogStream << "Create gambing item group failed!" << LogStream::eos;
		return;
	}

	itemPool->restGroups -= 1;

	GetGambingItemInfo(itemPool, group, player->GetUId(), payingGambleContext.payingResProtocol.itemInfo);

	InfoLogStream << "Add the new group " << group->LogFormatStr() << LogStream::eos;
}

void GambingMgr::UpdateParticipantInfo(GambingRecord* gambingRecord)
{
	if (!gambingRecord) return;

	GambingItemGroupKey key(gambingRecord->GetGambingItemId(), gambingRecord->GetGambingGroupId());

	auto itr = m_ParticipantsMap.find(key);
	if (itr != m_ParticipantsMap.end())
	{
		if (std::find(itr->second.begin(), itr->second.end(), gambingRecord->GetOwnerUId()) == itr->second.end())
		{
			m_ParticipantsMap[key].push_back(gambingRecord->GetOwnerUId());
			InfoLogStream << "Add participant(" << gambingRecord->GetOwnerUId() << ") in gambing(" << gambingRecord->GetGambingItemId()
				<< ", " << gambingRecord->GetGambingGroupId() << ")." << LogStream::eos;
		}
	}
	else
	{
		m_ParticipantsMap[key].push_back(gambingRecord->GetOwnerUId());
		InfoLogStream << "Add participant(" << gambingRecord->GetOwnerUId() << ") in gambing(" << gambingRecord->GetGambingItemId()
			<< ", " << gambingRecord->GetGambingGroupId() << ")." << LogStream::eos;
	}
}

void GambingMgr::CalculateGambingWeight(GambingItemPool* itemPool, GambingRecord* gambingRecord)
{
	if (!itemPool || !gambingRecord)
	{
		ErrorLogStream << "GambingItemPool ptr or GambingRecord ptr is null!" << LogStream::eos;
		return;
	}

	UInt32 ultimateWeight = gambingRecord->GetInvestCopies();
	// 加权
	if (itemPool->weightingStepSize > 0 && ultimateWeight >= itemPool->weightingStepSize)
	{
		UInt32 addCnt = ultimateWeight / itemPool->weightingStepSize;

		if (itemPool->weightingValue == 0)
		{
			ErrorLogStream << itemPool->LogFormatStr() << " weighting value is zero!" << LogStream::eos;
		}
		ultimateWeight += addCnt * itemPool->weightingValue;
	}

	gambingRecord->SetGambingWeight(ultimateWeight);
}

//void GambingMgr::OnParticipantNameChanged(ObjID_t roleId, const std::string& name)
//{
//	InfoLogStream << "Player(" << roleId << ") change name(" << name << ")." << LogStream::eos;
//
//	GambingRecordVec records;
//	FindGambingRecords(roleId, records);
//
//	for (auto record : records)
//	{
//		if (!record) continue;
//
//		record->SetOwnerName(name);
//		record->UpdateToDB();
//	}
//}

void GambingMgr::QueryGambingItems(GASPlayer* player)
{
	CLProtocol::GambingItemQueryRes res;

	if (GetGambingActStatus() == AS_END)
	{
		player->SendProtocol(res);
		return;
	}

	if (m_GambingItemGroups.empty())
	{
		player->SendProtocol(res);
		return;
	}

	for (auto itemPool : m_GambingItemPool)
	{
		if (!itemPool) continue;

		if (itemPool->itemPoolStatus == GIS_OFF_SALE)
		{
			continue;
		}

		if (itemPool->totalGroups < itemPool->restGroups)
		{
			continue;
		}

		UInt16 showGroupId = itemPool->totalGroups - itemPool->restGroups;
		GambingItemGroupInfo* groupInfo = FindGambingGroupInfo(itemPool->gambingItemId, showGroupId);
		if (!groupInfo)
		{
			continue;
		}

		CLProtocol::GambingItemInfo itemInfo;
		GetGambingItemInfo(itemPool, groupInfo, player->GetUId(), itemInfo);

		res.gambingItems.push_back(itemInfo);	
	}

	player->SendProtocol(res);
}

void GambingMgr::GetGambingItemInfo(const GambingItemPool* itemPool, const GambingItemGroupInfo* groupInfo, const std::string& playerUId, CLProtocol::GambingItemInfo& itemInfo)
{
	if (!itemPool || !groupInfo) return;

	itemInfo.gambingItemId = itemPool->gambingItemId;
	itemInfo.gambingItemNum = itemPool->gambingItemNum;
	itemInfo.sortId = itemPool->sortId;
	itemInfo.itemDataId = itemPool->itemDataId;
	itemInfo.costCurrencyId = itemPool->costCurrencyId;
	itemInfo.unitPrice = itemPool->unitPrice;
	itemInfo.restGroups = itemPool->restGroups;
	itemInfo.totalGroups = itemPool->totalGroups;
	itemInfo.rewardsPerCopy = itemPool->rewardsPerCopy;

	itemInfo.curGroupId = groupInfo->groupId;
	itemInfo.statusOfCurGroup = groupInfo->groupStatus;

	if (itemPool->copiesOfEachGroup < groupInfo->restCopies)
	{
		itemInfo.soldCopiesInCurGroup = itemPool->copiesOfEachGroup;
	}
	else
	{
		itemInfo.soldCopiesInCurGroup = itemPool->copiesOfEachGroup - groupInfo->restCopies;
	}

	itemInfo.totalCopiesOfCurGroup = itemPool->copiesOfEachGroup;
	itemInfo.sellBeginTime = groupInfo->sellBeginTime;

	GambingRecord* record = FindGambingRecord(playerUId, groupInfo->gambingItemId, groupInfo->groupId);
	if (record != NULL)
	{
		itemInfo.mineGambingInfo.investCopies = record->GetInvestCopies();
	}
}

void GambingMgr::QueryMineGambingItems(GASPlayer* player)
{
	CLProtocol::GambingMineQueryRes res;

	if (GetGambingActStatus() == AS_END)
	{
		ErrorLogStream << PLAYERINFO(player) << "Gambing activity was end!" << LogStream::eos;
		player->SendProtocol(res);
		return;
	}

	GambingRecordVec gambingRecords;
	FindGambingRecords(player->GetUId(), gambingRecords);

	if (gambingRecords.empty())
	{
		player->SendProtocol(res);
		return;
	}

	for (auto mineRecord : gambingRecords)
	{
		if (!mineRecord) continue;

		// 找到夺宝池子
		GambingItemPool* itemPool = FindGambingItemPool(mineRecord->GetGambingItemId());
		if (!itemPool)
		{
			ErrorLogStream << "Can not find gambing item pool(" << mineRecord->GetGambingItemId() << ")!" << LogStream::eos;
			continue;
		}

		CLProtocol::GambingMineInfo mineInfo;

		mineInfo.gambingItemId = mineRecord->GetGambingItemId();
		mineInfo.gambingItemNum = mineRecord->GetGambingItemNum();
		mineInfo.itemDataId = mineRecord->GetItemDataId();
		mineInfo.groupId = mineRecord->GetGambingGroupId();

		GambingItemGroupInfo* groupInfo = FindGambingGroupInfo(mineRecord->GetGambingItemId(), mineRecord->GetGambingGroupId());
		if (!groupInfo)
		{
			ErrorLogStream << PLAYERINFO(player) << "Can not find gambing group infomation by use item(" << mineRecord->GetGambingItemId()
				<< ") and group(" << mineRecord->GetGambingGroupId() << ")!" << LogStream::eos;
		}
		else
		{
			mineInfo.sortId = itemPool->sortId;

			if (itemPool->copiesOfEachGroup < groupInfo->restCopies)
			{
				ErrorLogStream << "Gambing item pool(" << itemPool->gambingItemId << ")'s copies of each group(" << itemPool->copiesOfEachGroup
					<< ") are less than group(" << groupInfo->groupId << ")'s rest copies(" << groupInfo->restCopies << ")!" << LogStream::eos;
				mineInfo.soldCopies = itemPool->copiesOfEachGroup;
			}
			else
			{
				mineInfo.soldCopies = itemPool->copiesOfEachGroup - groupInfo->restCopies;
			}

			mineInfo.totalCopies = itemPool->copiesOfEachGroup;
		}

		mineInfo.mineGambingInfo.participantId = player->GetID();
		mineInfo.mineGambingInfo.participantName = mineRecord->GetOwnerName();
		mineInfo.mineGambingInfo.gambingItemId = mineRecord->GetGambingItemId();
		mineInfo.mineGambingInfo.groupId = mineRecord->GetGambingGroupId();
		mineInfo.mineGambingInfo.investCopies = mineRecord->GetInvestCopies();
		mineInfo.mineGambingInfo.investCurrencyId = mineRecord->GetInvestCurrencyId();
		mineInfo.mineGambingInfo.investCurrencyNum = mineRecord->GetInvestCurrencyNum();
		mineInfo.mineGambingInfo.gambingRate = mineRecord->CalculateGambingRate(itemPool->copiesOfEachGroup);

		GambingLotteryRecord* lotteryRecord = FindGambingLotteryRecord(mineRecord->GetGambingItemId(), mineRecord->GetGambingGroupId());
		if (!lotteryRecord)
		{
			mineInfo.mineGambingInfo.status = GMS_WAIT;
			res.mineGambingInfo.push_back(mineInfo);
			continue;
		}

		if (player->GetUId() == lotteryRecord->GetGainerUId())
		{
			mineInfo.mineGambingInfo.status = GMS_SUCCESS;
		}
		else
		{
			mineInfo.mineGambingInfo.status = GMS_FAILE;
		}

		// 赢家数据
		GambingRecord* gainerRecord = FindGambingRecord(lotteryRecord->GetGainerUId(), mineRecord->GetGambingItemId(), mineRecord->GetGambingGroupId());
		if (!gainerRecord)
		{
			ErrorLogStream << PLAYERINFO(player) << "Can not find gainer record by use gainerId(" << lotteryRecord->GetGainerUId() << "), gambingItemId("
				<< mineRecord->GetGambingItemId() << ") and groupId(" << mineRecord->GetGambingGroupId() << ")!" << LogStream::eos;
		}
		else
		{
			//mineInfo.gainersGambingInfo.participantId = gainerRecord->GetOwnerUId();
			mineInfo.gainersGambingInfo.participantPlatform = ChannelNameSwitchDataEntryMgr::Instance()->SwitchToChinese(DecodePlatformFromGASPlayerUId(gainerRecord->GetOwnerUId()));
			mineInfo.gainersGambingInfo.participantServerName = gainerRecord->GetOwnerServerName();
			mineInfo.gainersGambingInfo.participantName = gainerRecord->GetOwnerName();
			mineInfo.gainersGambingInfo.gambingItemId = gainerRecord->GetGambingItemId();
			mineInfo.gainersGambingInfo.groupId = gainerRecord->GetGambingGroupId();
			mineInfo.gainersGambingInfo.investCopies = gainerRecord->GetInvestCopies();
			mineInfo.gainersGambingInfo.investCurrencyId = gainerRecord->GetInvestCurrencyId();
			mineInfo.gainersGambingInfo.investCurrencyNum = gainerRecord->GetInvestCurrencyNum();
			mineInfo.gainersGambingInfo.gambingRate = gainerRecord->CalculateGambingRate(itemPool->copiesOfEachGroup);
			mineInfo.gainersGambingInfo.status = GMS_SUCCESS;
		}

		res.mineGambingInfo.push_back(mineInfo);
	}

	player->SendProtocol(res);
}

void GambingMgr::PayingGamble(GASPlayer* player, const CLProtocol::PayingGambleReq& req)
{
	CLProtocol::PayingGambleRes res;
	res.retCode = ErrorCode::SUCCESS;
	res.gambingItemId = req.gambingItemId;
	res.groupId = req.groupId;

	if (req.investCopies == 0)
	{
		ErrorLogStream << PLAYERINFO(player) << "Invest copies are zero!" << LogStream::eos;
		res.retCode = ErrorCode::ITEM_NO_REASON;
		player->SendProtocol(res);
		return;
	}

	if (GetGambingActStatus() != AS_IN)
	{
		ErrorLogStream << PLAYERINFO(player) << "Gambing activity was end!" << LogStream::eos;
		res.retCode = ErrorCode::ITEM_NO_REASON;
		player->SendProtocol(res);
		return;
	}

	GambingItemGroupInfo* groupInfo = FindGambingGroupInfo(res.gambingItemId, res.groupId);
	if (!groupInfo)
	{
		ErrorLogStream << PLAYERINFO(player) << "Can not find gambingItem(id=" << res.gambingItemId << ", groupid=" << res.groupId << ")." << LogStream::eos;
		res.retCode = ErrorCode::ITEM_SOLD_OUT;
		player->SendProtocol(res);
		return;
	}

	GambingItemPool* itemPool = FindGambingItemPool(groupInfo->gambingItemId);
	if (!itemPool)
	{
		ErrorLogStream << PLAYERINFO(player) << "Can not find gambing item pool(" << groupInfo->gambingItemId << ")!" << LogStream::eos;
		res.retCode = ErrorCode::ITEM_OFF_SALE;
		player->SendProtocol(res);
		return;
	}

	res.costCurrencyId = itemPool->costCurrencyId;
	res.costCurrencyNum = itemPool->unitPrice * req.investCopies;

	GetGambingItemInfo(itemPool, groupInfo, player->GetUId(), res.itemInfo);

	if (itemPool->itemPoolStatus == GIS_OFF_SALE)
	{
		ErrorLogStream << PLAYERINFO(player) << itemPool->LogFormatStr() << " is off sale!" << LogStream::eos;
		res.retCode = ErrorCode::ITEM_OFF_SALE;
		player->SendProtocol(res);
		return;
	}

	// 商品没有份数了
	if (groupInfo->restCopies == 0)
	{
		if (itemPool->restGroups == 0)
		{
			res.retCode = ErrorCode::ITEM_GAMBLE_ALL_SOLD_OUT;
		}
		else
		{
			res.retCode = ErrorCode::ITEM_SOLD_OUT;
		}

		ErrorLogStream << PLAYERINFO(player) << groupInfo->LogFormatStr() << "'s rest copies are empty!" << LogStream::eos;
		player->SendProtocol(res);
		return;
	}

	// 商品不可以出售
	if (groupInfo->groupStatus != GIS_SELLING)
	{
		ErrorLogStream << PLAYERINFO(player) << groupInfo->LogFormatStr() << "'s status is error!" << LogStream::eos;
		res.retCode = ErrorCode::ITEM_NOT_SELL;
		player->SendProtocol(res);
		return;
	}

	// 份额检查
	if (groupInfo->restCopies < req.investCopies)
	{
		// 普通购买超出额度返回
		if (!req.bBuyAll)
		{
			ErrorLogStream << PLAYERINFO(player) << groupInfo->LogFormatStr() << " invest copies over flow!" << LogStream::eos;
			res.retCode = ErrorCode::ITEM_COPIES_NOT_ENOUGH;
			player->SendProtocol(res);
			return;
		}

		res.investCopies = groupInfo->restCopies;
	}
	else
	{
		res.investCopies = req.investCopies;
	}

	// 购买上下文
	PayingGambleContext payingContext;
	payingContext.itemPool = itemPool;
	payingContext.groupInfo = groupInfo;
	payingContext.payingReqProtocol = req;
	payingContext.payingResProtocol = res;

	// 扣除买的时候需要消耗的道具
	auto trans = new GambingPayingRmItemTransaction(player, payingContext);
	trans->SetItem(itemPool->costCurrencyId, itemPool->unitPrice * res.investCopies);
	if (!player->BeginItemTransaction(trans))
	{
		ErrorLogStream << PLAYERINFO(player) << "Remove item failed, begin transaction failed." << LogStream::eos;
		res.retCode = ErrorCode::ITEM_NO_REASON;
		player->SendProtocol(res);
	}
}

void GambingMgr::QueryGambingRecordData(GASPlayer* player)
{
	CLProtocol::GambingRecordQueryRes res;

	if (GetGambingActStatus() == AS_END)
	{
		ErrorLogStream << PLAYERINFO(player) << "Gambing activity was end!" << LogStream::eos;
		player->SendProtocol(res);
		return;
	}

	UInt32 todayBeginTime = (UInt32)(GetDayBeginTime(Avalon::Time::CurrentTime()) / SEC_TO_MSEC);
	UInt32 todayEndTime = todayBeginTime + DAY_TO_SEC;

	std::vector<GambingLotteryRecord*> showRecords;

	// 筛选出需要展示的记录
	for (auto lotteryRecord : m_GambingLotteryRecord)
	{
		if (!lotteryRecord) continue;

		if (lotteryRecord->GetLotteryTimeSec() >= todayBeginTime &&
			lotteryRecord->GetLotteryTimeSec() <= todayEndTime)
		{
			showRecords.push_back(lotteryRecord);
		}
	}

	if (showRecords.empty())
	{
		DebugLogStream << PLAYERINFO(player) << "Show records are empty." << LogStream::eos;
	}

	for (auto itemPool : m_GambingItemPool)
	{
		if (!itemPool) continue;

		// 没有出售就过滤掉
		if (itemPool->totalGroups == itemPool->restGroups) continue;

		CLProtocol::GambingItemRecordData itemRecord;
		itemRecord.gambingItemId = itemPool->gambingItemId;
		itemRecord.gambingItemNum = itemPool->gambingItemNum;
		itemRecord.sortId = itemPool->sortId;
		itemRecord.itemDataId = itemPool->itemDataId;

		// 最后一组id
		UInt16 lastGroupId = 0;

		// 售罄
		if (itemPool->restGroups == 0)
		{
			lastGroupId = itemPool->totalGroups;
		}

		for (auto lotteryRecord : showRecords)
		{
			if (!lotteryRecord) continue;

			if (lotteryRecord->GetGambingItemId() != itemPool->gambingItemId)
			{
				continue;
			}

			// 如果是最后一组，设置售罄时间
			if (lotteryRecord->GetGroupId() == lastGroupId)
			{
				itemRecord.soldOutTimestamp = lotteryRecord->GetLotteryTimeSec();
			}

			// 赢家数据
			GambingRecord* gainerRecord = FindGambingRecord(lotteryRecord->GetGainerUId(), lotteryRecord->GetGambingItemId(), lotteryRecord->GetGroupId());
			if (!gainerRecord)
			{
				ErrorLogStream << PLAYERINFO(player) << "Can not find gainer record by use gainerId(" << lotteryRecord->GetGainerUId() << "), gambingItemId("
					<< lotteryRecord->GetGambingItemId() << ") and groupId(" << lotteryRecord->GetGroupId() << ")!" << LogStream::eos;
				continue;
			}

			std::string gainerPf = DecodePlatformFromGASPlayerUId(gainerRecord->GetOwnerUId());

			CLProtocol::GambingGroupRecordData groupRecord;
			groupRecord.groupId = gainerRecord->GetGambingGroupId();
			groupRecord.gainerENPlatform = gainerPf;
			groupRecord.gainerPlatform = ChannelNameSwitchDataEntryMgr::Instance()->SwitchToChinese(gainerPf);
			groupRecord.gainerServerName = gainerRecord->GetOwnerServerName();
			groupRecord.gainerId = DecodeRoleIdFromGASPlayerUId(gainerRecord->GetOwnerUId());			
			groupRecord.gainerName = gainerRecord->GetOwnerName();
			groupRecord.investCurrencyId = itemPool->costCurrencyId;
			groupRecord.investCurrencyNum = gainerRecord->GetInvestCurrencyNum();

			itemRecord.groupRecordData.push_back(groupRecord);
		}

		res.gambingRecordDatas.push_back(itemRecord);
	}

	player->SendProtocol(res);
}

void GambingMgr::_Destroy()
{
	InfoLogStream << "Begin to destroy..." << LogStream::eos;

	for (auto elem : m_GambingItemPool)
	{
		delete elem;
	}
	m_GambingItemPool.clear();

	for (auto elem : m_GambingItemGroups)
	{
		delete elem.second;
	}
	m_GambingItemGroups.clear();

	for (auto elem : m_GambingRecords)
	{
		for (auto record : elem.second)
		{
			delete record;
		}
		elem.second.clear();
	}
	m_GambingRecords.clear();

	m_ParticipantsMap.clear();

	for (auto elem : m_GambingLotteryRecord)
	{
		delete elem;
	}
	m_GambingLotteryRecord.clear();
}

GambingItemGroupInfo* GambingMgr::_CreateGambingItemGroup(GambingItemPool* itemPool)
{
	if (!itemPool) return NULL;

	GambingItemGroupInfo* groupInfo = new GambingItemGroupInfo();

	groupInfo->gambingItemId = itemPool->gambingItemId;

	if (itemPool->totalGroups < itemPool->restGroups)
	{
		ErrorLogStream << itemPool->LogFormatStr() << "'s total groups(" << itemPool->totalGroups << ") and rest groups(" 
			<< itemPool->restGroups << ") are not fit!" << LogStream::eos;
		delete groupInfo;
		return NULL;
	}
	groupInfo->groupId = itemPool->totalGroups - itemPool->restGroups + 1;

	groupInfo->restCopies = itemPool->copiesOfEachGroup;
	groupInfo->groupStatus = GIS_PREPARE;
	groupInfo->sellBeginTime = itemPool->coolDownIntervalBetweenGroups + (UInt32)Avalon::Time::CurrentTime().Sec();

	if (!AddGambingItemGroupInfo(groupInfo))
	{
		ErrorLogStream << "Add gambing item group(" << groupInfo->gambingItemId << ", " << groupInfo->groupId << ") faild!" << LogStream::eos;
		delete groupInfo;
		return NULL;
	}

	return groupInfo;
}

bool GambingMgr::_OnInitDataFromDB(GambingRecord* gambingRecord)
{
	if (!gambingRecord) return false;

	// 找到夺宝池子
	GambingItemPool* itemPool = FindGambingItemPool(gambingRecord->GetGambingItemId());
	if (!itemPool)
	{
		ErrorLogStream << "Can not find gambing item pool(" << gambingRecord->GetGambingItemId() << ")!" << LogStream::eos;
		return false;
	}

	GambingItemGroupInfo* groupInfo = FindGambingGroupInfo(gambingRecord->GetGambingItemId(), gambingRecord->GetGambingGroupId());
	if (!groupInfo)
	{
		groupInfo = new GambingItemGroupInfo();

		groupInfo->gambingItemId = itemPool->gambingItemId;

		if (itemPool->totalGroups < itemPool->restGroups)
		{
			ErrorLogStream << itemPool->LogFormatStr() << "'s total groups(" << itemPool->totalGroups << ") and rest groups("
				<< itemPool->restGroups << ") are not fit!" << LogStream::eos;
			delete groupInfo;
			return false;
		}
		groupInfo->groupId = gambingRecord->GetGambingGroupId();

		groupInfo->restCopies = itemPool->copiesOfEachGroup;
		groupInfo->groupStatus = GIS_PREPARE;
		groupInfo->sellBeginTime = itemPool->coolDownIntervalBetweenGroups + (UInt32)Avalon::Time::CurrentTime().Sec();

		if (!AddGambingItemGroupInfo(groupInfo))
		{
			ErrorLogStream << "Add gambing item group(" << groupInfo->gambingItemId << ", " << groupInfo->groupId << ") faild!" << LogStream::eos;
			delete groupInfo;
			return false;
		}

		if (itemPool->restGroups == 0)
		{
			DebugLogStream << itemPool->LogFormatStr() << " rest groups are zero!" << LogStream::eos;
		}
		else
		{
			itemPool->restGroups -= 1;
		}
	}

	Int32 restCopies = (Int32)groupInfo->restCopies - (Int32)gambingRecord->GetInvestCopies();

	// 非法份额
	if (restCopies < 0)
	{
		ErrorLogStream << groupInfo->LogFormatStr() << "'s rest copies(" << restCopies << ") while decrese record(owner=" << gambingRecord->GetOwnerUId() << ", investCopies=" << gambingRecord->GetInvestCopies() << ")!" << LogStream::eos;
		return false;
	}

	groupInfo->restCopies = (UInt32)restCopies;

	UpdateParticipantInfo(gambingRecord);

	if (groupInfo->groupId > 1)
	{
		_CalculateGroupSellBeginTimeSec(itemPool, groupInfo);
	}
	else
	{
		// 加载数据是在活动开启的时候，所以第一组一般都是在售
		groupInfo->groupStatus = GIS_SELLING;
	}

	if (groupInfo->restCopies == 0)
	{
		// 份额数为0，查看是否已开奖，现在的机制一般都开了
		GambingLotteryRecord* lotteryRecord = FindGambingLotteryRecord(groupInfo->gambingItemId, groupInfo->groupId);
		if (!lotteryRecord)
		{
			//OnGambingItemGroupLottery(groupInfo);
			ErrorLogStream << groupInfo->LogFormatStr() << " was sold out but not lottery!" << LogStream::eos;
		}
		else
		{
			groupInfo->soldOutTimestamp = lotteryRecord->GetSoldOutTimeSec();
		}
		groupInfo->groupStatus = GIS_LOTTERY;
	}

	return true;
}

void GambingMgr::_OnInitGambingFirstGroup(const OpActivityInfo* opAct, GambingItemPool* itemPool)
{
	if (!opAct || !itemPool) return;

	if (itemPool->itemPoolStatus == GIS_OFF_SALE)
	{
		ErrorLogStream << itemPool->LogFormatStr() << " off sale!" << LogStream::eos;
		return;
	}

	if (itemPool->restGroups == 0)
	{
		DebugLogStream << itemPool->LogFormatStr() << " rest groups are zero!" << LogStream::eos;
		return;
	}

	if (itemPool->totalGroups != itemPool->restGroups)
	{
		return;
	}

	GambingItemGroupInfo* firstGroup = FindGambingGroupInfo(itemPool->gambingItemId, 1);
	if (firstGroup != NULL)
	{
		ErrorLogStream << itemPool->LogFormatStr() << " first group had been create!" << LogStream::eos;
		return;
	}

	firstGroup = _CreateGambingItemGroup(itemPool);
	if (!firstGroup)
	{
		ErrorLogStream << "Create gambing item group failed!" << LogStream::eos;
		return;
	}

	firstGroup->sellBeginTime = opAct->startTime;
	if (firstGroup->sellBeginTime <= (UInt32)Avalon::Time::CurrentTime().Sec())
	{
		firstGroup->groupStatus = GIS_SELLING;
	}

	itemPool->restGroups -= 1;

	DebugLogStream << "Add the new group " << firstGroup->LogFormatStr() << LogStream::eos;
}

void GambingMgr::_CalculateGroupSellBeginTimeSec(GambingItemPool* itemPool, GambingItemGroupInfo* groupInfo)
{
	if (!groupInfo || !groupInfo) return;

	UInt32 curTimestamp = (UInt32)Avalon::Time::CurrentTime().Sec();

	if (groupInfo->groupId == 0)
	{
		ErrorLogStream << groupInfo->LogFormatStr() << "'s group id was error!" << LogStream::eos;
		return;
	}

	// 上一组开奖情况
	UInt16 lastGroupId = groupInfo->groupId - 1;

	GambingLotteryRecord* lotteryRecord = FindGambingLotteryRecord(groupInfo->gambingItemId, lastGroupId);
	if (!lotteryRecord)
	{
		groupInfo->sellBeginTime = itemPool->coolDownIntervalBetweenGroups + (UInt32)Avalon::Time::CurrentTime().Sec();
		ErrorLogStream << "Faild to find lottery record(" << groupInfo->gambingItemId << ", " << lastGroupId << ")!" << LogStream::eos;
	}
	else
	{
		if (lotteryRecord->GetSoldOutTimeSec() == 0)
		{
			groupInfo->sellBeginTime = itemPool->coolDownIntervalBetweenGroups + curTimestamp;
			ErrorLogStream << "Lottery record(" << groupInfo->gambingItemId << ", " << lastGroupId << ")'s solt out timestamp is zero!" << LogStream::eos;
		}
		else
		{
			//上一组开奖情况有售罄时间，计算出下一组的出售时间
			groupInfo->sellBeginTime = itemPool->coolDownIntervalBetweenGroups + lotteryRecord->GetSoldOutTimeSec();
			if (groupInfo->sellBeginTime <= curTimestamp)
			{
				groupInfo->groupStatus = GIS_SELLING;
			}
		}
	}
}

void GambingMgr::OnGambingItemGroupLottery(GambingItemGroupInfo* groupInfo)
{
	if (!groupInfo) return;

	if (groupInfo->groupStatus == GIS_LOTTERY)
	{
		ErrorLogStream << groupInfo->LogFormatStr() << " status was error!" << LogStream::eos;
		return;
	}

	// 找到夺宝池子
	GambingItemPool* itemPool = FindGambingItemPool(groupInfo->gambingItemId);
	if (!itemPool)
	{
		ErrorLogStream << "Can not find gambing item pool(" << groupInfo->gambingItemId << ")!" << LogStream::eos;
		return;
	}

	InfoLogStream << "Begin lottery for group(" << groupInfo->gambingItemId << ", " << groupInfo->groupId << ")." << LogStream::eos;

	// 该组参与者
	GambingParticipantVec participants;
	FindGambingParticipants(groupInfo->gambingItemId, groupInfo->groupId, participants);

	if (participants.empty())
	{
		ErrorLogStream << groupInfo->LogFormatStr() << "'s participants are empty!" << LogStream::eos;
		return;
	}

	// 加权参与者记录
	GambingRecordVec weightPartipantRecord;
	// 全部参与者记录
	GambingRecordVec totalPartipantRecord;

	for (auto participant : participants)
	{
		GambingRecord* gambingRecord = FindGambingRecord(participant, groupInfo->gambingItemId, groupInfo->groupId);
		if (!gambingRecord)
		{
			ErrorLogStream << "Can not find player(" << participant << ")'s gambing record(" << groupInfo->gambingItemId
				<< ", " << groupInfo->groupId << ")!" << LogStream::eos;
			continue;
		}

		// 一般情况下权重=投入份数，加权后的权重!=投入份数
		if (gambingRecord->GetGambingWeight() != gambingRecord->GetInvestCopies())
		{
			weightPartipantRecord.push_back(gambingRecord);
		}

		totalPartipantRecord.push_back(gambingRecord);
	}

	// 权重集
	std::vector<UInt32> wts;
	// 随机阈值
	Int32 randThreshold = 0;

	// 如果有加权的参与者，只从加权的参与者中随机
	if (!weightPartipantRecord.empty())
	{
		for (auto record : weightPartipantRecord)
		{
			wts.push_back(record->GetGambingWeight());
		}
		randThreshold = (Int32)weightPartipantRecord.size();
	}
	else
	{
		for (auto record : totalPartipantRecord)
		{
			wts.push_back(record->GetGambingWeight());
		}
		randThreshold = (Int32)totalPartipantRecord.size();
	}

	if (wts.empty())
	{
		ErrorLogStream << "Weights are empty!" << LogStream::eos;
		return;
	}

	Int32 hitIdx = WeightRander::RandWeight(wts);
	if (hitIdx >= randThreshold || hitIdx < 0)
	{
		ErrorLogStream << "Rand weight failed, hitIdx(" << hitIdx << ") is invalid!" << LogStream::eos;
		return;
	}

	// 中奖者记录
	GambingRecord* gainerRecord = NULL;
	if (!weightPartipantRecord.empty())
	{
		gainerRecord = weightPartipantRecord[hitIdx];
	}
	else
	{
		gainerRecord = totalPartipantRecord[hitIdx];
	}

	if (gainerRecord == NULL)
	{
		ErrorLogStream << "Gainer record ptr is null!" << LogStream::eos;
		return;
	}

	auto lotteryItemDataEntry = ItemDataEntryMgr::Instance()->FindEntry(gainerRecord->GetItemDataId());
	if (!lotteryItemDataEntry)
	{
		ErrorLogStream << "Can not find item data entry(" << gainerRecord->GetItemDataId() << ")!" << LogStream::eos;
		return;
	}

	// 记录开奖
	if (!AddGambingLotteryRecord(itemPool, groupInfo, gainerRecord->GetOwnerUId()))
	{
		ErrorLogStream << "Add " << groupInfo->LogFormatStr() << " to record failed, gainer(" << gainerRecord->GetOwnerUId() << ")!" << LogStream::eos;
		return;
	}

	if (gainerRecord->GetOwnerUId() != SYSTEM_PLAYER)
	{
		std::string gainerPf = DecodePlatformFromGASPlayerUId(gainerRecord->GetOwnerUId());

		// 发送中奖通知
		CLProtocol::GambingLotterySync protocol;
		protocol.gainersGambingInfo.participantId = DecodeRoleIdFromGASPlayerUId(gainerRecord->GetOwnerUId());
		protocol.gainersGambingInfo.participantENPlatform = gainerPf;
		protocol.gainersGambingInfo.participantPlatform = ChannelNameSwitchDataEntryMgr::Instance()->SwitchToChinese(gainerPf);
		protocol.gainersGambingInfo.participantServerName = gainerRecord->GetOwnerServerName();
		protocol.gainersGambingInfo.participantName = gainerRecord->GetOwnerName();
		protocol.gainersGambingInfo.gambingItemId = itemPool->gambingItemId;
		protocol.gainersGambingInfo.gambingRate = gainerRecord->CalculateGambingRate(itemPool->copiesOfEachGroup);

		struct CustomLessParticipantGambingRate
		{
			bool operator()(GambingRecord* lhs, GambingRecord* rhs)
			{
				return lhs->GetGambingWeight() > rhs->GetGambingWeight();
			}
		};

		std::sort(totalPartipantRecord.begin(), totalPartipantRecord.end(), CustomLessParticipantGambingRate());

		// 列表容量
		UInt32 participantCapacity = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GAMBING_PARTICIPANT_LIST_CAPACITY);
		if (participantCapacity == 0)
		{
			participantCapacity = 5;
		}

		for (auto participantRecord : totalPartipantRecord)
		{
			if (!participantRecord || participantRecord->GetOwnerUId() == SYSTEM_PLAYER) continue;

			if (protocol.participantsGambingInfo.size() == participantCapacity)
			{
				break;
			}

			CLProtocol::GambingParticipantInfo info;
			info.gambingItemId = itemPool->gambingItemId;
			info.participantPlatform = ChannelNameSwitchDataEntryMgr::Instance()->SwitchToChinese(DecodePlatformFromGASPlayerUId(participantRecord->GetOwnerUId()));
			info.participantServerName = participantRecord->GetOwnerServerName();
			info.participantName = participantRecord->GetOwnerName();
			info.gambingRate = participantRecord->CalculateGambingRate(itemPool->copiesOfEachGroup);

			protocol.participantsGambingInfo.push_back(info);
		}

		// 给所有参与者发送通知
		for (auto participantRecord : totalPartipantRecord)
		{
			if (!participantRecord || participantRecord->GetOwnerUId() == SYSTEM_PLAYER) continue;

			GASGameZone* zone = GASGameZoneMgr::Instance()->FindGASGameZone(participantRecord->GetOwnerSourceCenterNode());
			if (!zone)
			{
				ErrorLogStream << "Can not find game zone(" << participantRecord->GetOwnerSourceCenterNode() << ") while send to player(" << participantRecord->GetOwnerUId()
					<< ", " << participantRecord->GetOwnerServerName() << ")!" << LogStream::eos;
				continue;
			}

			zone->TransmitPlayerMsg(participantRecord->GetOwnerUId(), participantRecord->GetOwnerSourceAdminNode(), protocol);
		}
	}

	// 发送公告
	{
		if (gainerRecord->GetOwnerUId() != SYSTEM_PLAYER)
		{
			auto gambingItemData = GambingItemDataEntryMgr::Instance()->FindEntry(groupInfo->gambingItemId);
			if (gambingItemData && gambingItemData->isAnnounce)
			{
				std::string itemLinkStr;
				std::string systemLinkStr;

				MsgPackage::GetItemMsgTag(itemLinkStr, 0, lotteryItemDataEntry->id, 0);
				MsgPackage::GetSystemSuffixesMsgTag(systemLinkStr, SystemSuffixesType::SYSTEM_SUFFIXES_TYPE_GAMBING_LOTTERY, 0);

				std::string gainerPf = ChannelNameSwitchDataEntryMgr::Instance()->SwitchToChinese(DecodePlatformFromGASPlayerUId(gainerRecord->GetOwnerUId()));

				GASAnnouncementMgr::Instance()->PublicAnnouncement(false, 70, UInt32(CURRENT_TIME.Sec()), gainerPf, gainerRecord->GetOwnerServerName(), gainerRecord->GetOwnerName(), itemLinkStr.c_str(), systemLinkStr);

			}
		}
	}

	// 发奖

	{
		if (gainerRecord->GetOwnerUId() != SYSTEM_PLAYER)
		{
			// 中奖者的奖励
			ItemReward reward;
			reward.id = gainerRecord->GetItemDataId();
			reward.num = gainerRecord->GetGambingItemNum();
			std::vector<ItemReward> rewards = { reward };

			std::string sender = SysNotifyMgr::Instance()->MakeString(9067);
			std::string title = SysNotifyMgr::Instance()->MakeString(9068);

			std::string content = SysNotifyMgr::Instance()->MakeString(9069, lotteryItemDataEntry->name, gainerRecord->GetGambingGroupId(), gainerRecord->GetInvestCurrencyNum());
			std::string reason = GetReason(SOURCE_TYPE_GAMBING_LOTTERY, DecodeRoleIdFromGASPlayerUId(gainerRecord->GetOwnerUId()), gainerRecord->GetItemDataId());

			SysUtility::SendMail(gainerRecord->GetOwnerSourceCenterNode(), gainerRecord->GetOwnerSourceAdminNode(), gainerRecord->GetOwnerUId(), sender, title, content, reason, rewards);

			InfoLogStream << groupInfo->LogFormatStr() << " lottery, gainer(" << gainerRecord->GetOwnerUId() << ")." << LogStream::eos;
		}
	}

	if ( !itemPool->rewardsPerCopy.empty())
	{
		// 其他参与者的奖励
		for (auto participantRecord : totalPartipantRecord)
		{
			if (participantRecord->GetOwnerUId() == gainerRecord->GetOwnerUId())
			{
				continue;
			}

			if (participantRecord->GetOwnerUId() == SYSTEM_PLAYER)
			{
				continue;
			}

			std::vector<ItemReward> rewards;
			for (auto elem : itemPool->rewardsPerCopy)
			{
				ItemReward reward = elem;
				reward.num *= participantRecord->GetInvestCopies();

				rewards.push_back(reward);
			}

			std::string sender = SysNotifyMgr::Instance()->MakeString(9070);
			std::string title = SysNotifyMgr::Instance()->MakeString(9071);
			std::string content = SysNotifyMgr::Instance()->MakeString(9072, lotteryItemDataEntry->name, participantRecord->GetGambingGroupId());
			std::string reason = GetReason(SOURCE_TYPE_GAMBING_LOTTERY, DecodeRoleIdFromGASPlayerUId(participantRecord->GetOwnerUId()));

			SysUtility::SendMail(participantRecord->GetOwnerSourceCenterNode(), participantRecord->GetOwnerSourceAdminNode(), participantRecord->GetOwnerUId(), sender, title, content, reason, rewards);

			InfoLogStream << groupInfo->LogFormatStr() << " lottery, participant(" << participantRecord->GetOwnerUId() << ")." << LogStream::eos;
		}
	}

	groupInfo->groupStatus = GIS_LOTTERY;

	InfoLogStream << groupInfo->LogFormatStr() << " lottery success." << LogStream::eos;
}
