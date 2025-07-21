#include "CLDrawPrizeDataEntry.h"

#include "CLItemDataEntry.h"
#include "CLActiveEventDataEntry.h"
#include "CLActivityDefine.h"
#include "DropItemMgr.h"

bool DrawPrizeDataEntry::Read(Avalon::DataRow& row)
{
	if (GetColNum() != row.GetColNum())
	{
		return false;
	}

	dataId = row.ReadUInt32();
	name = row.ReadString();
	activityId = (ActiveID)row.ReadUInt32();
	opActivityType = (OpActivityType)row.ReadUInt32();

	if (activityId > 0 && opActivityType > 0)
	{
		ErrorLogStream << "Activity id and opActivity type is all more than zero!" << LogStream::eos;
		return false;
	}

	drawPrizeType = row.ReadUInt32();
	channelName = row.ReadString();
	isOpen = row.ReadUInt8();
	baseCount = row.ReadUInt32();
	continuousDrawDays = row.ReadUInt32();
	rewardCount = row.ReadUInt32();
	refreshInterval = row.ReadUInt32();
	refreshTime = row.ReadUInt8();
	drawPrizeCountKey = row.ReadString();
	allCountKey = row.ReadString();
	continuousDrawKey = row.ReadString();
	restDrawCountKey = row.ReadString();
	rewardIdKey = row.ReadString();
	raffleTicketType = (ItemSubType)row.ReadUInt32();

	{
		std::string str = row.ReadString();
		if (str != "-")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "|");
			for (auto paramStr : strs)
			{
				UInt32 param = Avalon::StringUtil::ConvertToValue<UInt32>(paramStr);
				parms.push_back(param);
			}
		}
	}

	return true;
}

bool RewardPoolDataEntry::Read(Avalon::DataRow& row)
{
	if (GetColNum() != row.GetColNum())
	{
		return false;
	}

	dataId = row.ReadUInt32();
	drawPrizeTableId = row.ReadUInt32();
	itemId = row.ReadUInt32();
	itemNum = row.ReadUInt32();
	strengthenLevel = row.ReadUInt8();
	itemWeight = row.ReadUInt32();
	drawPrizeCountLimit = row.ReadUInt32();
	chargeCond = row.ReadUInt32();
	announceNum = row.ReadUInt32();
	serverTotalNum = row.ReadUInt32();

	return true;
}

bool DrawPrizeDataEntryMgr::AddEntry(DrawPrizeDataEntry* dataEntry)
{
	if (!Avalon::DataEntryMgrBase<DrawPrizeDataEntry>::AddEntry(dataEntry)) return false;

	m_Datas.push_back(dataEntry);

	return true;
}

DrawPrizeDataEntry* DrawPrizeDataEntryMgr::GetDrawPrizeDataByType(UInt32 type)
{
	for (auto data : m_Datas)
	{
		if (!data) continue;

		if (data->drawPrizeType == type) return data;
	}

	return NULL;
}

bool RewardPoolDataEntryMgr::Init(IRewardPoolRareItemControl* control)
{
	if (!control)
	{
		return false;
	}

	m_rareItemControl = control;
	return true;
}

RewardPoolDataEntry* RewardPoolDataEntryMgr::RandReward(UInt32 drawPrizeId, UInt32 usedDrawCnt, UInt32 totalChargeOnRole)
{
	if (!m_rareItemControl)
	{
		ErrorLogStream << "m_rareItemControl ptr is null!" << LogStream::eos;
		return NULL;
	}

	class RewardPoolVisitor : public Avalon::DataEntryVisitor<RewardPoolDataEntry>
	{
	public:
		RewardPoolVisitor(UInt32 drawPrizeId, UInt32 usedDrawCnt, UInt32 totalChargeOnRole, IRewardPoolRareItemControl* control)
			: m_DrawPrizeId(drawPrizeId), m_UsedDrawCount(usedDrawCnt), m_TotalChargeOnRole(totalChargeOnRole), m_RareItemControl(control) {}

	public:
		bool operator()(RewardPoolDataEntry* dataEntry)
		{
			if (!dataEntry)
			{
				ErrorLogStream << "RewardPoolDataEntry ptr is null!" << LogStream::eos;
				return true;
			}

			if (m_DrawPrizeId == dataEntry->drawPrizeTableId)
			{
				// 稀有控制
				if (dataEntry->serverTotalNum != 0 && m_RareItemControl && m_RareItemControl->RareRewardRemainNum(dataEntry->dataId, dataEntry->serverTotalNum) == 0)
				{
					return true;
				}

				if (dataEntry->drawPrizeCountLimit > 0 && m_UsedDrawCount < dataEntry->drawPrizeCountLimit)
				{
					return true;
				}

				if (dataEntry->chargeCond > 0 && m_TotalChargeOnRole <= dataEntry->chargeCond)
				{
					return true;
				}

				rewardPool.push_back(dataEntry);
			}

			return true;
		}

		std::vector<RewardPoolDataEntry*> rewardPool;

	private:
		// 抽奖表id
		UInt32	m_DrawPrizeId;
		// 已抽奖次数
		UInt32  m_UsedDrawCount;
		// 充值数
		UInt32  m_TotalChargeOnRole;
		IRewardPoolRareItemControl* m_RareItemControl;
	};

	RewardPoolVisitor visitor(drawPrizeId, usedDrawCnt, totalChargeOnRole, m_rareItemControl);
	RewardPoolDataEntryMgr::Instance()->Visit(visitor);

	if (visitor.rewardPool.empty())
	{
		ErrorLogStream << "Reward pool was empty!" << LogStream::eos;
		return NULL;
	}

	//生成了新奖池，根据权重抽奖
	std::vector<UInt32> wts;

	for (auto rewardData : visitor.rewardPool)
	{
		if (!rewardData) continue;
		wts.push_back(rewardData->itemWeight);
	}

	Int32 hitIdx = WeightRander::RandWeight(wts);
	if (hitIdx >= (Int32)visitor.rewardPool.size() || hitIdx < 0)
	{
		ErrorLogStream << "Rand reward failed, hitIdx(" << hitIdx << ") is invalid!" << LogStream::eos;
		return NULL;
	}

	auto reward = visitor.rewardPool[hitIdx];
	if (!reward)
	{
		ErrorLogStream << "Rand reward failed, reward data entry ptr is null!" << LogStream::eos;
		return NULL;
	}

	return reward;
}

bool GambingItemDataEntry::Read(Avalon::DataRow& row)
{
	if (GetColNum() != row.GetColNum())
	{
		return false;
	}

	gambingItemId = row.ReadUInt32();
	actId = row.ReadUInt32();

	//auto actTmpData = ActivityTemplateDataEntryMgr::Instance()->FindEntry(actId);
	//if (!actTmpData)
	//{
	//	ErrorLogStream << "Second column format is error!" << LogStream::eos;
	//	return false;
	//}

	//if (actTmpData->type != ATT_GAMBING)
	//{
	//	ErrorLogStream << "Second column format is error!" << LogStream::eos;
	//	return false;
	//}

	gambingItemNum = row.ReadUInt32();
	sortId = row.ReadUInt16();
	itemDataId = row.ReadUInt32();

	if (ItemDataEntryMgr::Instance()->FindEntry(itemDataId) == NULL)
	{
		ErrorLogStream << "Fifth column format is error!" << LogStream::eos;
		return false;
	}

	totalGroups = row.ReadUInt16();
	if (totalGroups == 0)
	{
		ErrorLogStream << "Sixth column format is error!" << LogStream::eos;
		return false;
	}

	copiesOfEachGroup = row.ReadInt32();
	if (copiesOfEachGroup == 0)
	{
		ErrorLogStream << "Seventh column format is error!" << LogStream::eos;
		return false;
	}

	costCurrencyId = row.ReadUInt32();
	if (ItemDataEntryMgr::Instance()->FindEntry(costCurrencyId) == NULL)
	{
		ErrorLogStream << "Eighth column format is error!" << LogStream::eos;
		return false;
	}

	unitPrice = row.ReadInt32();
	if (unitPrice == 0)
	{
		ErrorLogStream << "Ninth column format is error!" << LogStream::eos;
		return false;
	}

	coolDownIntervalBetweenGroups = row.ReadUInt32();

	{
		std::string str = row.ReadString();
		std::vector<std::string> strVec;
		Avalon::StringUtil::Split(str, strVec, "|");

		for (const auto& elem : strVec)
		{
			std::vector<std::string> strVec2;
			Avalon::StringUtil::Split(elem, strVec2, ":");
			if (strVec2.size() != 2)
			{
				ErrorLogStream << "Eleven column format is error!" << LogStream::eos;
				return false;
			}

			ItemReward reward;
			reward.id = Avalon::StringUtil::ConvertToValue<UInt32>(strVec2[0]);
			reward.num = Avalon::StringUtil::ConvertToValue<UInt32>(strVec2[1]);

			rewardsPerCopy.push_back(reward);
		}
	}

	weightingStepSize = row.ReadUInt32();
	weightingValue = row.ReadUInt32();
	isAnnounce = row.ReadUInt8();

	return true;
}

bool GambingItemDataEntry::ReadCustomer(UInt32 _actId, UInt32 _gambingItemId, UInt32 _sortId, const ItemRewardVec& soldItems, const std::vector<std::string>& strVec)
{
	if (soldItems.size() != 1)
	{
		ErrorLogStream << "sold items size(" << (UInt32)soldItems.size() << ") is error!" << LogStream::eos;
		return false;
	}

	if (strVec.size() != 8)
	{
		ErrorLogStream << "gambing item data size(" << (UInt32)strVec.size() << ") is error!" << LogStream::eos;
		return false;
	}

	gambingItemId = _gambingItemId;
	actId = _actId;
	gambingItemNum = soldItems.front().num;
	sortId = _sortId;
	itemDataId = soldItems.front().id;

	if (gambingItemId == 0)
	{
		ErrorLogStream << "gambing item id is zero!" << LogStream::eos;
		return false;
	}

	if (actId == 0)
	{
		ErrorLogStream << "activity id is zero!" << LogStream::eos;
		return false;
	}

	if (gambingItemNum == 0)
	{
		ErrorLogStream << "gambing item num is zero!" << LogStream::eos;
		return false;
	}

	if (ItemDataEntryMgr::Instance()->FindEntry(itemDataId) == NULL)
	{
		ErrorLogStream << "can't find item data(" << itemDataId << ")!" << LogStream::eos;
		return false;
	}

	{
		std::vector<std::string> strVec2;
		Avalon::StringUtil::Split(strVec[0], strVec2, ":");
		if (strVec2.size() != 2)
		{
			ErrorLogStream << "unit price format(" << strVec[0] << ") is error!" << LogStream::eos;
			return false;
		}
		costCurrencyId = Avalon::StringUtil::ConvertToValue<UInt32>(strVec2[0]);
		unitPrice = Avalon::StringUtil::ConvertToValue<UInt32>(strVec2[1]);

		if (ItemDataEntryMgr::Instance()->FindEntry(costCurrencyId) == NULL)
		{
			ErrorLogStream << "can't find cost currency's item data(" << costCurrencyId << ")!" << LogStream::eos;
			return false;
		}

		if (unitPrice == 0)
		{
			ErrorLogStream << "unit price is zero!" << LogStream::eos;
			return false;
		}
	}

	{
		std::vector<std::string> strVec2;
		Avalon::StringUtil::Split(strVec[1], strVec2, ":");
		if (strVec2.size() != 2)
		{
			ErrorLogStream << "per copy rewards format(" << strVec[1] << ") is error!" << LogStream::eos;
			return false;
		}

		ItemReward reward;
		reward.id = Avalon::StringUtil::ConvertToValue<UInt32>(strVec2[0]);
		reward.num = Avalon::StringUtil::ConvertToValue<UInt32>(strVec2[1]);
		rewardsPerCopy.push_back(reward);

		if (ItemDataEntryMgr::Instance()->FindEntry(reward.id) == NULL)
		{
			ErrorLogStream << "can't find per copy reward's item data(" << reward.id << ")!" << LogStream::eos;
			return false;
		}

		if (reward.num == 0)
		{
			ErrorLogStream << "per copy reward's num is zero!" << LogStream::eos;
			return false;
		}
	}

	totalGroups = Avalon::StringUtil::ConvertToValue<UInt16>(strVec[2]);
	copiesOfEachGroup = Avalon::StringUtil::ConvertToValue<UInt32>(strVec[3]);
	coolDownIntervalBetweenGroups = Avalon::StringUtil::ConvertToValue<UInt32>(strVec[4]);
	weightingStepSize = Avalon::StringUtil::ConvertToValue<UInt32>(strVec[5]);
	weightingValue = Avalon::StringUtil::ConvertToValue<UInt32>(strVec[6]);
	isAnnounce = Avalon::StringUtil::ConvertToValue<UInt8>(strVec[7]);
	
	if (totalGroups == 0)
	{
		ErrorLogStream << "total groups are zero!" << LogStream::eos;
		return false;
	}

	if (copiesOfEachGroup == 0)
	{
		ErrorLogStream << "copies of each group are zero!" << LogStream::eos;
		return false;
	}

	return true;
}

bool GambingItemDataEntryMgr::AddDataEntryCustomer(UInt32 _actId, UInt32 _gambingItemId, UInt32 _sortId, const ItemRewardVec& soldItems, const std::vector<std::string>& strVec)
{
	auto newDataEntry = new GambingItemDataEntry();
	if (!newDataEntry->ReadCustomer(_actId, _gambingItemId, _sortId, soldItems, strVec))
	{
		delete newDataEntry;
		return false;
	}

	auto oldDataEntry = FindEntry(newDataEntry->gambingItemId);
	if (!oldDataEntry)
	{
		AddEntry(newDataEntry);
	}
	else
	{
		*oldDataEntry = *newDataEntry;
		delete newDataEntry;
	}

	return true;
}