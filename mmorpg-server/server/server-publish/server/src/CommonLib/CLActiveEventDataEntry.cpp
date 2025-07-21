#include "CLActiveEventDataEntry.h"

#include "CLActivityDefine.h"
#include "CLBuffDataEntry.h"
#include "CLItemDataEntry.h"
#include <TimeUtil.h>

ActiveTaskDataEntry::ActiveTaskDataEntry()
{
	accountTotalSubmitLimit = 0;
}

ActiveTaskDataEntry::~ActiveTaskDataEntry()
{

}

bool ActiveTaskDataEntry::Read(Avalon::DataRow& row)
{
	if (GetColNum() != row.GetColNum())
	{
		return false;
	}

	id = row.ReadUInt32();
	preTaskID = row.ReadUInt32();
	nextTaskID = row.ReadUInt32();


	std::string split1 = ",";
	std::string split2 = "_";
	std::string strRewards = row.ReadString();
	std::vector<std::string> stdRewardVec;
	Avalon::StringUtil::Split(strRewards, stdRewardVec, split1);
	for (UInt32 i = 0; i < stdRewardVec.size(); ++i)
	{
		std::vector<std::string> strTmp;
		Avalon::StringUtil::Split(stdRewardVec[i], strTmp, split2);

		ItemReward reward;
		if (strTmp.size() == 2)
		{
			reward.id = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[0]);
			reward.num = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[1]);
		}
		else if (strTmp.size() == 4)
		{
			reward.id = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[0]);
			reward.num = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[1]);
			reward.equipType = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[2]);
			reward.strenth = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[3]);
		}
		else
		{
			continue;
		}
		rewards.push_back(reward);
	}

	vipLv = row.ReadUInt32();
	templateID = row.ReadUInt32();
	sortPriority = row.ReadUInt32();
	takeCost = row.ReadUInt32();
	param1 = row.ReadUInt32();
	dungeonId = row.ReadUInt32();
	isAbandon = row.ReadUInt32();

	char split = 0;
	std::istringstream stream2(row.ReadString());
	while (stream2)
	{
		ItemReward item;
		item.DecodeString(stream2);
		if (item.id == 0) break;
		consumeItems.push_back(item);
		stream2 >> split;
	}

	taskCycleCount = row.ReadInt64();
	taskCycleKey = row.ReadString();
	overTaskNumKey = row.ReadString();
	failedTaskNumKey = row.ReadString();
	applyChannel = row.ReadString();
	taskBelong = row.ReadUInt8();

	{
		std::string str = row.ReadString();
		if (str != "-")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "|");
			for (auto paramStr : strs)
			{
				UInt32 param = Avalon::StringUtil::ConvertToValue<UInt32>(paramStr);
				if (param > 0)
				{
					parm2.push_back(param);
				}
			}
		}
	}
	accountTotalSubmitLimit = row.ReadUInt32();
	return true;
}

ActiveTaskDataEntryMgr::ActiveTaskDataEntryMgr()
{

}

ActiveTaskDataEntryMgr::~ActiveTaskDataEntryMgr()
{

}

bool ActiveTaskDataEntryMgr::AddEntry(ActiveTaskDataEntry* dataEntry)
{
	if (!Avalon::DataEntryMgrBase<ActiveTaskDataEntry>::AddEntry(dataEntry)) return false;

	std::vector<ActiveTaskDataEntry*>& vecActives = m_actives[dataEntry->templateID];
	vecActives.push_back(dataEntry);

	return true;
}

void ActiveTaskDataEntryMgr::GetActives(UInt32 tempId, std::vector<ActiveTaskDataEntry*>& vecActives)
{
	vecActives = m_actives[tempId];
}

FatigueMakeUpData::FatigueMakeUpData()
{

}

FatigueMakeUpData::~FatigueMakeUpData()
{

}

bool FatigueMakeUpData::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	lowExp = row.ReadUInt64();
	lowNeedMoneyID = row.ReadUInt32();
	hiExp = row.ReadUInt64();
	hiNeedMoneyID = row.ReadUInt32();
	fatigueMax = row.ReadUInt32();
	
	std::string strline = row.ReadString();
	std::vector<std::string> strs;
	Avalon::StringUtil::Split(strline, strs, "|");
	for (std::vector<std::string>::iterator iter = strs.begin();
		iter != strs.end(); ++iter)
	{
		UInt32 vipmax = Avalon::StringUtil::ConvertToValue<UInt32>(*iter);
		vipMaxs.push_back(vipmax);
	}

	return true;
}

FatigueMakeUpDataMgr::FatigueMakeUpDataMgr()
{

}

FatigueMakeUpDataMgr::~FatigueMakeUpDataMgr()
{

}

DungeonMakeUpData::DungeonMakeUpData()
{

}

DungeonMakeUpData::~DungeonMakeUpData()
{

}

bool DungeonMakeUpData::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	level = row.ReadUInt32();
	dungeonID = row.ReadUInt32();
	ReadSplitStr(row, normalItems);
	ReadSplitStr(row, normalNum);
	ReadSplitStr(row, perfectItems);
	ReadSplitStr(row, costNum);
	goldCost = row.ReadUInt32();
	pointCost = row.ReadUInt32();
	countMax = row.ReadUInt32();
	vipCountMax = row.ReadUInt32();
	vipLevel = row.ReadUInt32();

	return true;
}

bool DungeonMakeUpData::ReadSplitStr(Avalon::DataRow& row, std::vector<UInt64>& vec)
{
	std::string str = row.ReadString();
	if (str == "")
		return false;

	std::vector<std::string> strs;
	Avalon::StringUtil::Split(str, strs, "|");
	if (strs.size() == 0)
		return false;

	for (size_t i = 0; i < strs.size(); ++i)
	{
		vec.push_back(Avalon::StringUtil::ConvertToValue<UInt64>(strs[i].c_str()));
	}

	return true;
}

bool DungeonMakeUpData::ReadSplitStr(Avalon::DataRow& row, std::vector<UInt32>& vec)
{
	std::string str = row.ReadString();
	if (str == "")
		return false;

	std::vector<std::string> strs;
	Avalon::StringUtil::Split(str, strs, "|");
	if (strs.size() == 0)
		return false;

	for (size_t i = 0; i < strs.size(); ++i)
	{
		vec.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(strs[i].c_str()));
	}

	return true;
}

DungeonMakeUpDataMgr::DungeonMakeUpDataMgr()
{

}

DungeonMakeUpDataMgr::~DungeonMakeUpDataMgr()
{

}

bool DungeonMakeUpDataMgr::AddEntry(DungeonMakeUpData* dataEntry)
{
	if (!Avalon::DataEntryMgrBase<DungeonMakeUpData>::AddEntry(dataEntry)) return false;

	std::vector<DungeonMakeUpData*>& vec = m_dungeonIdToDatas[dataEntry->dungeonID];
	vec.push_back(dataEntry);

	return true;
}

void DungeonMakeUpDataMgr::GetDatas(UInt32 dungeonID, std::vector<DungeonMakeUpData*>& datas)
{
	auto itr = m_dungeonIdToDatas.find(dungeonID);
	if (itr != m_dungeonIdToDatas.end())
	{
		datas = itr->second;
	}
}

DungeonMakeUpData* DungeonMakeUpDataMgr::GetData(UInt32 dungeonID, UInt32 level)
{
	std::vector<DungeonMakeUpData*> datas;
	GetDatas(dungeonID, datas);
	
	for (auto data : datas)
	{
		if (!data)
		{
			continue;
		}

		if (data->level == level)
		{
			return data;
		}
	}

	return NULL;
}

bool GiftPhaseData::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	eventType = row.ReadUInt8();
	value = row.ReadUInt32();
	nextValue = row.ReadUInt32();

	std::istringstream stream(row.ReadString());
	char split = 0;
	while (stream)
	{
		ItemReward item;
		item.DecodeString(stream);
		if (item.id == 0) return false;
		if (item.num == 0) return false;
		rewards.push_back(item);
		stream >> split;
	}

	return true;
}

GiftPhaseData::GiftPhaseData()
{

}

GiftPhaseData::~GiftPhaseData()
{

}

GiftPhaseDataMgr::GiftPhaseDataMgr()
{

}

GiftPhaseDataMgr::~GiftPhaseDataMgr()
{

}

bool GiftPhaseSortFunc(GiftPhaseData* a, GiftPhaseData* b) { return (a->id < b->id); }
bool GiftPhaseDataMgr::LoadData(const std::string& filename)
{
	if (!Avalon::DataEntryMgrBase<GiftPhaseData>::LoadData(filename)) return false;

	//对数据进行排序
	auto itr = m_giftsTypeMap.begin();
	for (; itr != m_giftsTypeMap.end(); ++itr)
	{
		std::vector<GiftPhaseData*>& datas = itr->second;
		std::sort(datas.begin(), datas.end(), GiftPhaseSortFunc);
	}
	return true;
}

bool GiftPhaseDataMgr::AddEntry(GiftPhaseData* dataEntry)
{
	if (!Avalon::DataEntryMgrBase<GiftPhaseData>::AddEntry(dataEntry)) return false;

	std::vector<GiftPhaseData*> datas = m_giftsTypeMap[dataEntry->eventType];
	datas.push_back(dataEntry);
	return true;
}

GiftPhaseData* GiftPhaseDataMgr::GetNextGift(UInt8 type, UInt32 id)
{
	auto itr = m_giftsTypeMap.find(type);
	if (itr == m_giftsTypeMap.end())
	{
		return NULL;
	}

	std::vector<GiftPhaseData*> datas = itr->second;

	if (datas.size() == 0)
	{
		return NULL;
	}

	if (id == 0)
	{
		return datas[0];
	}

	for (size_t i = 0; i < datas.size(); ++i)
	{
		if (datas[i]->id == id)
		{
			if (i+1 < datas.size())
			{
				return datas[i + 1];
			}
			else
			{
				return NULL;
			}
		}
	}

	return NULL;
}

FatigueMakeUpPrice::FatigueMakeUpPrice()
{

}

FatigueMakeUpPrice::~FatigueMakeUpPrice()
{

}

bool FatigueMakeUpPrice::Read(Avalon::DataRow& row)
{
	id = row.ReadUInt32();
	
	std::string str = row.ReadString();
	if (str == "")
		return false;

	std::vector<std::string> strs;
	Avalon::StringUtil::Split(str, strs, "|");
	if (strs.size() != 2)
		return false;

	for (size_t i = 0; i < strs.size(); ++i)
	{
		fatigueSection.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(strs[i].c_str()));
	}

	lowPrice = row.ReadUInt32();
	hiPrice = row.ReadUInt32();
	return true;
}

FatigueMakeUpPriceMgr::FatigueMakeUpPriceMgr()
{

}

FatigueMakeUpPriceMgr::~FatigueMakeUpPriceMgr()
{

}

UInt32 FatigueMakeUpPriceMgr::GetPrice(UInt32 fatigue, bool bLow)
{
	class Visitor : public Avalon::DataEntryVisitor<FatigueMakeUpPrice>
	{
	public:
		Visitor(UInt32 fatigue, bool bLow) : m_fatigue(fatigue), m_bLow(bLow), m_price(0){}

		bool operator()(FatigueMakeUpPrice* dataEntry)
		{
			if (m_fatigue >= dataEntry->fatigueSection[0] &&
				m_fatigue <= dataEntry->fatigueSection[1])
			{
				if (m_bLow)
				{
					m_price = dataEntry->lowPrice;
				}
				else
				{
					m_price = dataEntry->hiPrice;
				}

				return false;
			}

			return true;
		}

		UInt32 GetPrice()
		{
			return m_price;
		}

	private:
		UInt32 m_fatigue;
		bool m_bLow;
		UInt32 m_price;
	};

	Visitor vistor(fatigue, bLow);
	Visit(vistor);
	
	return vistor.GetPrice();
}

bool OpActivityDataEntry::Read(Avalon::DataRow& row)
{
	if (GetColNum() != row.GetColNum())
	{
		return false;
	}

	id = row.ReadUInt32();
	tmpType = row.ReadUInt32();
	name = row.ReadString();
	tag = row.ReadUInt8();

	{
		std::string str = row.ReadString();
		if (!TimeUtil::TimeStrConvertToTimestamp(prepareTime, str))
		{
			ErrorLogStream << "Error prepare time format(" << str << ")!" << LogStream::eos;
			return false;
		}
	}

	tagEngTime = row.ReadUInt32();

	{
		std::string str = row.ReadString();
		if (!TimeUtil::TimeStrConvertToTimestamp(startTime, str))
		{
			ErrorLogStream << "Error start time format(" << str << ")!" << LogStream::eos;
			return false;
		}
	}

	{
		std::string str = row.ReadString();
		if (!TimeUtil::TimeStrConvertToTimestamp(endTime, str))
		{
			ErrorLogStream << "Error end time format(" << str << ")!" << LogStream::eos;
			return false;
		}
	}

	playerLevelLimit = row.ReadUInt16();
	minLevel = row.ReadUInt16();
	maxLevel = row.ReadUInt16();
	needStartServiceTime = row.ReadUInt32();
	startTimeFromService = row.ReadString();
	endTimeFromService = row.ReadString();
	startTimeFromFirstService = row.ReadString();
	endTimeFromFirstService = row.ReadString();

	if (!startTimeFromService.empty() && !startTimeFromFirstService.empty())
	{
		ErrorLogStream << "OpActivity(" << id << ") error format: startTimeFromService and startTimeFromFirstService are all not empty!" << LogStream::eos;
		return false;
	}

	if (!startTimeFromService.empty() && endTimeFromService.empty())
	{
		ErrorLogStream << "OpActivity(" << id << ") error format: startTimeFromService is not empty but endTimeFromService is empty!" << LogStream::eos;
		return false;
	}

	if (!startTimeFromFirstService.empty() && endTimeFromFirstService.empty())
	{
		ErrorLogStream << "OpActivity(" << id << ") error format: startTimeFromFirstService is not empty but endTimeFromFirstService is empty!" << LogStream::eos;
		return false;
	}

	openInterval = row.ReadString();
	closeInterval = row.ReadString();

	desc = row.ReadString();
	ruleDesc = row.ReadString();
	circleType = row.ReadUInt8();
	parm = row.ReadUInt32();

	{
		std::string str = row.ReadString();
		if (str != "-")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "|");
			for (auto paramStr : strs) 
			{
				UInt32 param = Avalon::StringUtil::ConvertToValue<UInt32>(paramStr);
				if (param > 0)
				{
					parm2.push_back(param);
				}
			}
		}
	}

	{
		std::string str = row.ReadString();
		if (str != "-")
		{
			std::vector<std::string> strVec;
			Avalon::StringUtil::Split(str, strVec, "|");
			for (auto elem : strVec)
			{
				taskIds.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(elem));
			}
		}
	}

	logoDesc = row.ReadString();
	superNewServiceActFlag = row.ReadUInt8();

	countParam = row.ReadString();

	{
		std::string str = row.ReadString();
		if (!str.empty())
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "|");
			for (const auto& elem : strs)
			{
				parm3.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(elem));
			}
		}
	}

	prefabPath = row.ReadString();

	logoPath = row.ReadString();

	{
		std::string str = row.ReadString();
		if (!str.empty())
		{
			Avalon::StringUtil::Split(str, strParams, "|");
		}
	}
	return true;
}

bool OpActivityTaskDataEntry::Read(Avalon::DataRow& row)
{
	if (GetColNum() != row.GetColNum())
	{
		return false;
	}

	id = row.ReadUInt32();
	opAvtivityId = row.ReadUInt32();
	opActivityType = row.ReadUInt32();
	preTaskId = row.ReadUInt32();
	nextTaskId = row.ReadUInt32();

	auto actData = OpActivityDataEntryMgr::Instance()->FindEntry(opAvtivityId);
	if (!actData)
	{
		ErrorLogStream << "Can not find opAct(" << opAvtivityId << ") by task(" << id << ")!" << LogStream::eos;
		return false;
	}

	if (actData->tmpType != opActivityType)
	{
		ErrorLogStream << "Task(" << id << ")'s opActType was error!" << LogStream::eos;
		return false;
	}

	completeCount = row.ReadUInt32();

	if (id / 1000 != opAvtivityId)
	{
		ErrorLogStream << "OpActivity(" << opAvtivityId << ") error format: task id(" << id << ")!" << LogStream::eos;
		return false;
	}
	


	std::string split1 = "|";
	std::string split2 = "_";
	std::string strRewards = row.ReadString();
	std::vector<std::string> stdRewardVec;
	Avalon::StringUtil::Split(strRewards, stdRewardVec, split1);
	for (UInt32 i = 0; i < stdRewardVec.size(); ++i)
	{
		std::vector<std::string> strTmp;
		Avalon::StringUtil::Split(stdRewardVec[i], strTmp, split2);

		ItemReward reward;
		if (strTmp.size() == 2)
		{
			reward.id = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[0]);
			reward.num = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[1]);
		}
		else if (strTmp.size() == 4)
		{
			reward.id = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[0]);
			reward.num = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[1]);
			reward.equipType = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[2]);
			reward.strenth = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[3]);
		}
		else
		{
			continue;
		}
		rewards.push_back(reward);
	}

	taskDesc = row.ReadString();

	std::vector<std::string> strs;
	Avalon::StringUtil::Split(row.ReadString(), strs, "|");
	for (auto elem : strs)
	{
		variables.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(elem));
	}

	strs.clear();
	Avalon::StringUtil::Split(row.ReadString(), strs, "|");
	for (auto elem : strs)
	{
		variables2.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(elem));
	}

	std::vector<std::string> counterStrs;
	Avalon::StringUtil::Split(row.ReadString(), counterStrs, ",");
	for (auto counter : counterStrs)
	{
		std::vector<std::string> ss;
		Avalon::StringUtil::Split(counter, ss, "|");
		if (ss.size() == 3)
		{
			CounterItem counterItem;
			counterItem.currencyId = Avalon::StringUtil::ConvertToValue<UInt32>(ss[0]);
			counterItem.name = ss[1];
			counterItem.value = Avalon::StringUtil::ConvertToValue<UInt32>(ss[2]);
			counters.push_back(counterItem);
		}
	}

	taskName = row.ReadString();

	strs.clear();
	Avalon::StringUtil::Split(row.ReadString(), strs, "|");
	for (auto elem : strs)
	{
		InfoLogStream << "id=" << id << "varProgressName=" << elem << LogStream::eos;
		varProgressName.push_back(elem);
	}

	playerLevelLimit = row.ReadUInt16();
	accountDailySubmitLimit = row.ReadUInt32();
	accountTotalSubmitLimit = row.ReadUInt32();
	restType = row.ReadUInt32();
	acceptType = row.ReadUInt32();
	eventType = row.ReadUInt32();
	subType = row.ReadUInt32();
	accountWeeklySubmitLimit = row.ReadUInt32();
	accountTask = row.ReadUInt32();

	bool bErrorVar = false;
	switch ((OpActivityType)opActivityType)
	{
	case OAT_FATIGUE_FOR_BUFF:
		if (variables.size() != 1) 
		{ 
			ErrorLogStream << "OpActivityTask(" << id << ")'s variables size=" << (UInt32)variables.size() << "!" << LogStream::eos;
			bErrorVar = true;
			break; 
		}
		break;

	case OAT_FATIGUE_FOR_TOKEN_COIN:
		if (variables.size() != 1) 
		{ 
			ErrorLogStream << "OpActivityTask(" << id << ")'s variables size=" << (UInt32)variables.size() << "!" << LogStream::eos;
			bErrorVar = true;
			break; 
		}
		if (variables.front() == 0) 
		{ 
			ErrorLogStream << "OpActivityTask(" << id << ") variables first value is zero!" << LogStream::eos;
			bErrorVar = true;
			break; 
		}
		break;

	case OAT_FATIGUE_BURNING:
		if (variables.size() != OPACT_FATIGUE_BURNING_VARIABLE_SIZE)
		{ 
			ErrorLogStream << "OpActivityTask(" << id << ")'s variables size=" << (UInt32)variables.size() << "!" << LogStream::eos;
			bErrorVar = true; 
			break;
		}
		if (!ItemDataEntryMgr::Instance()->FindEntry(variables[0]))
		{
			ErrorLogStream << "OpActivityTask(" << id << ") can't find item data entry(" << variables[0] << "!" << LogStream::eos;
			bErrorVar = true; 
			break;
		}
		if (variables[1] == 0) 
		{ 
			ErrorLogStream << "OpActivityTask(" << id << ") variables second value is zero!" << LogStream::eos;
			bErrorVar = true; 
			break;
		}
		break;
	
	case OAT_CHANGE_FASHION_EXCHANGE:
		if (variables.size() != 2)
		{
			ErrorLogStream << "OpActivityTask(" << id << ")'s variables size=" << (UInt32)variables.size() << "!" << LogStream::eos;
			bErrorVar = true;
			break;
		}
		if (variables2.size() != 3)
		{
			ErrorLogStream << "OpActivityTask(" << id << ")'s variables2 size=" << (UInt32)variables2.size() << "!" << LogStream::eos;
			bErrorVar = true;
			break;
		}
		break;
	case OAT_CHANGE_FASHION_WELFARE:
		if (variables.size() == 0 && counters.size() == 0)
		{
			ErrorLogStream << "OpActivityTask(" << id << ")'s variables size=0, counters size=0" << "!" << LogStream::eos;
			bErrorVar = true;
			break;
		}
		if (variables.size() > 0 && variables.size() != 2)
		{
			ErrorLogStream << "OpActivityTask(" << id << ")'s variables size=" << (UInt32)variables.size() << "!" << LogStream::eos;
			bErrorVar = true;
			break;
		}
		if (counters.size() > 0 && counters.size() != 1)
		{
			ErrorLogStream << "OpActivityTask(" << id << ")'s counters size=" << (UInt32)counters.size() << "!" << LogStream::eos;
			bErrorVar = true;
			break;
		}
		break;
	case OAT_DAILY_BUFF:
	{
		if (variables.size() != 1)
		{
			ErrorLogStream << "OpActivityTask(" << id << ")'s variables size is not one" << "!" << LogStream::eos;
			bErrorVar = true;
			break;
		}
		PrayActivityBuffType type = (PrayActivityBuffType)variables.front();
		if (type == PABT_SYNTH_STREN_TICKET_REDUCE_MAT)
		{
			if (rewards.empty())
			{
				ErrorLogStream << "OpActivityTask(" << id << ")'s rewards size is empty!" << LogStream::eos;
				bErrorVar = true;
				break;
			}
		}
		else if (type == PABT_DUNGEON_DORP_ADD)
		{
			if (rewards.empty() && variables2.empty())
			{
				ErrorLogStream << "OpActivityTask(" << id << ")'s rewards size and variables2 size are all empty!" << LogStream::eos;
				bErrorVar = true;
				break;
			}
			if (rewards.size() != variables2.size())
			{
				ErrorLogStream << "OpActivityTask(" << id << ")'s rewards size is not match variables2 size!" << LogStream::eos;
				bErrorVar = true;
				break;
			}
		}
		break;
	}

	case OAT_BUY_PRRSENT:
	{
		if (rewards.empty())
		{
			ErrorLogStream << "OpActivityTask(" << id << ")'s rewards empty!" << LogStream::eos;
			bErrorVar = true;
			break;
		}
		if (variables.empty() || variables2.empty() || varProgressName.empty())
		{
			ErrorLogStream << "OpActivityTask(" << id << ")'s variables or variables2 or varProgressName empty!" << LogStream::eos;
			bErrorVar = true;
			break;
		}
		if (variables.size() != variables2.size() 
			|| variables.size() != varProgressName.size()
			|| variables2.size() != varProgressName.size())
		{
			ErrorLogStream << "OpActivityTask(" << id << ")'s variables or variables2 or varProgressName size err!" << LogStream::eos;
			bErrorVar = true;
			break;
		}
		break;
	}

	case OAT_CUMULATEPASS_DUNGEON_REWARD:
	{
		if (rewards.empty())
		{
			ErrorLogStream << "OpActivityTask(" << id << ")'s rewards empty!" << LogStream::eos;
			bErrorVar = true;
			break;
		}
		if (variables.empty())
		{
			ErrorLogStream << "OpActivityTask(" << id << ")'s variables empty!" << LogStream::eos;
			bErrorVar = true;
			break;
		}
		if (variables2.size() != 1)
		{
			ErrorLogStream << "OpActivityTask(" << id << ")'s variables2 size err!" << LogStream::eos;
			bErrorVar = true;
			break;
		}
		break;
	}

	default: break;
	}

	if (bErrorVar)
	{
		ErrorLogStream << "OpActivityTask(" << id << ")'s var format was error!" << LogStream::eos;
		ErrorLogStream << "OpActivityTask(" << id << ") opActId=" << opAvtivityId << ", opActType=" << opActivityType << ", completeCount="  << completeCount 
			<< ", rewards size=" << (UInt32)rewards.size() << " taskDesc=" << taskDesc << ", variables size=" << (UInt32)variables.size() << LogStream::eos;

		for (auto varb : variables)
		{
			ErrorLogStream << "OpActivityTask(" << id << ")'s varb=" << varb << LogStream::eos;
		}
		return false;
	}

	return true;
}

bool ActivityTemplateDataEntry::Read(Avalon::DataRow& row)
{
	if (GetColNum() != row.GetColNum())
	{
		return false;
	}

	id = row.ReadUInt32();
	type = row.ReadUInt8();

	return true;
}

bool ActivityTemplateDataEntryMgr::AddEntry(ActivityTemplateDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<ActivityTemplateDataEntry>::AddEntry(entry)) return false;

	m_ActivityTempToType[entry->id] = entry->type;

	return true;
}

UInt8 ActivityTemplateDataEntryMgr::GetActivityTemplateType(UInt32 activityId)
{
	auto iter = m_ActivityTempToType.find(activityId);
	if (iter != m_ActivityTempToType.end())
	{
		return iter->second;
	}

	return 0;
}

bool ActivitySystemConfigDataEntry::Read(Avalon::DataRow& row)
{
	if (GetColNum() != row.GetColNum())
	{
		return false;
	}

	id = row.ReadUInt32();
	actSytem = row.ReadUInt32();

	{
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(row.ReadString(), strs, "|");
		for (auto elem : strs)
		{
			opActList.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(elem));
		}
	}

	{
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(row.ReadString(), strs, "|");
		for (auto elem : strs)
		{
			mallItemList.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(elem));
		}
	}

	{
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(row.ReadString(), strs, "|");
		for (auto elem : strs)
		{
			actList.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(elem));
		}
	}

	{
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(row.ReadString(), strs, "|");
		for (auto elem : strs)
		{
			pushInfoList.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(elem));
		}
	}

	isClose = row.ReadUInt8() > 0 ? true : false;
	returnActId = row.ReadUInt32();

	return true;
}

bool ActivityTabDataEntry::Read(Avalon::DataRow& row)
{
	if (GetColNum() != row.GetColNum())
	{
		return false;
	}

	id = row.ReadUInt32();
	name = row.ReadString();

	{
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(row.ReadString(), strs, "|");
		for (auto elem : strs)
		{
			actIds.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(elem));
		}
	}

	return true;
}
