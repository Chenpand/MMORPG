#include "SysRecordMgr.h"
#include <CLRareItemDataEntry.h>
#include <CLSyncObjectProtocol.h>
#include <CLDropItemVipLimitDataEntry.h>

#include "CLRecordClient.h"
#include "WSSelectInitDataCallback.h"
#include "WSPlayerMgr.h"

SysRecordMgr::SysRecordMgr()
{

}

SysRecordMgr::~SysRecordMgr()
{
	for (auto itr : m_sysRecords)
	{
		delete itr.second;
	}
}

class SelectSysRecordCallback : public CLRecordCallback
{
public:
	void OnFinished()
	{
		while (NextRow())
		{
			SysRecord* record = new SysRecord();
			record->guid = GetKey();
			record->behavior = (SysBehavior)(UInt32)GetData("behavior");
			record->ownerType = (SysBehaviorOwnerType)(UInt32)GetData("owner_type");
			record->owner = GetData("owner");
			record->param1 = GetData("param1");
			record->num = GetData("num");
			record->recordTime = GetData("record_time");

			SysRecordMgr::Instance()->LoadSysRecord(record);
		}
	}

	void OnFailed(UInt32 errorcode)
	{
		ErrorLogStream << "load sys record failed:" << errorcode << LogStream::eos;
	}
};

bool SysRecordMgr::Init()
{
	LoadRecordFromDB(0, 0);
	_CountRareItemMaxNumByTime();
	m_dailyTimer.SetTime(6);
	return true;
}

void SysRecordMgr::OnTick(const Avalon::Time& now)
{
	if (m_dailyTimer.IsTimeout(now))
	{
		_CountRareItemMaxNumByTime();
	}
}

void SysRecordMgr::LoadRecordFromDB(UInt32 accid, ObjID_t roleid)
{
	{
		CLSelectCommand	*cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_sys_record");
		cmd->PutCond("owner_type", (UInt32)SYS_BEHAVIOR_OWNER_ROLE);
		cmd->PutCond("owner", roleid);
		CLRecordClient::Instance()->SendCommand(cmd, new SelectSysRecordCallback());
	}

	{
		CLSelectCommand	*cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_sys_record");
		cmd->PutCond("owner_type", (UInt32)SYS_BEHAVIOR_OWNER_ACCOUNT);
		cmd->PutCond("owner", accid);
		CLRecordClient::Instance()->SendCommand(cmd, new SelectSysRecordCallback());
	}
}

bool SysRecordMgr::LoadSysRecord(SysRecord* record)
{
	if (!record)
	{
		return false;
	}

	// 如果内存中已经有了，以内存中为主
	if (m_sysRecords.find(record->GetKey()) != m_sysRecords.end())
	{
		delete record;
		return true;
	}

	_AddRecord(record);
	return true;
}

SysRecord* SysRecordMgr::GetRecord(SysBehavior behavior, SysBehaviorOwnerType ownerType, ObjID_t owner, UInt32 param1)
{
	SysRecordKey key(behavior, ownerType, owner, param1);

	auto itr = m_sysRecords.find(key);
	if (itr == m_sysRecords.end() || !itr->second)
	{
		return NULL;
	}

	SysRecord* record = itr->second;

	// 每次取数据时判断数据有没有过时，如果已经过时就刷新数据，再更新到数据库
	if (record->num > 0 && _IsTimeOut(record->behavior, record->recordTime))
	{
		record->num = 0;
		record->recordTime = (UInt32)CURRENT_TIME.Sec();

		_RecordToDB(record);
	}

	return record;
}

UInt32 SysRecordMgr::GetRecordValue(SysBehavior behavior, SysBehaviorOwnerType ownerType, ObjID_t owner, UInt32 param1)
{
	auto record = GetRecord(behavior, ownerType, owner, param1);
	return record == NULL ? 0 : record->num;
}

void SysRecordMgr::UpdateRecord(SysBehavior behavior, SysBehaviorOwnerType ownerType, ObjID_t owner, UInt32 param1, int addNum)
{
	auto record = GetRecord(behavior, ownerType, owner, param1);
	if (!record)
	{
		if (addNum <= 0)
		{
			return;
		}

		record = _CreateRecord(behavior, ownerType, owner, param1, addNum);
		_RecordToDB(record, true);
	}
	else
	{
		if (addNum < 0 && record->num < (UInt32)-addNum)
		{
			record->num = 0;
		}
		else
		{
			record->num += addNum;
		}
		record->recordTime = (UInt32)CURRENT_TIME.Sec();
		_RecordToDB(record);
	}
}

void SysRecordMgr::SetRecord(SysBehavior behavior, SysBehaviorOwnerType ownerType, ObjID_t owner, UInt32 param1, int num)
{
	auto record = GetRecord(behavior, ownerType, owner, param1);
	if (!record)
	{
		record = _CreateRecord(behavior, ownerType, owner, param1, num);
		_RecordToDB(record, true);
	}
	else
	{
		record->num = num;
		record->recordTime = (UInt32)CURRENT_TIME.Sec();
		_RecordToDB(record);
	}
}

UInt32 SysRecordMgr::RareItemRemainNum(ObjID_t roleId, UInt32 itemId, UInt32 num)
{
	auto data = RareItemDataEntryMgr::Instance()->FindEntryByType(RARE_CTRL_ITEM, itemId);
	if (!data)
	{
		return num;
	}

	// 是否在时间限定内
	if (!data->IsInOpenTime(CURRENT_DATE))
	{
		return 0;
	}

	// 付费限制
	if (data->minPayCost > 0)
	{
		auto player = WSPlayerMgr::Instance()->FindPlayer(roleId);
		if (!player)
		{
			return 0;
		}

		if (player->GetTotalChargeNum() < data->minPayCost)
		{
			return 0;
		}
	}

	// 个人每日产出控制
	num = _GetRareItemRemainNum(SYS_BEHAVIOR_DAILY_RARE_ITEM, roleId, itemId, num, data->persionalDailyNum);
	// 个人每周产出控制
	num = _GetRareItemRemainNum(SYS_BEHAVIOR_WEEKLY_RARE_ITEM, roleId, itemId, num, data->persionalWeeklyNum);
	// 个人每月产出控制
	num = _GetRareItemRemainNum(SYS_BEHAVIOR_MONTHLY_RARE_ITEM, roleId, itemId, num, data->persionalMonthlyNum);
	// 个人总产出控制
	num = _GetRareItemRemainNum(SYS_BEHAVIOR_TOTAL_RARE_ITEM, roleId, itemId, num, data->persionalTotalNum);

	// 服务器每日产出控制
	UInt32 dailyNum = _GetRareItemMaxNumByTime(itemId);
	if (dailyNum == (UInt32)-1)
	{
		dailyNum = data->serverDailyNum;
	} 
	else if (dailyNum == 0)
	{
		num = 0;
	}	
	num = _GetRareItemRemainNum(SYS_BEHAVIOR_DAILY_RARE_ITEM, 0, itemId, num, dailyNum);

	// 服务器每周产出控制
	num = _GetRareItemRemainNum(SYS_BEHAVIOR_WEEKLY_RARE_ITEM, 0, itemId, num, data->serverWeeklyNum);
	// 服务器每月产出控制
	num = _GetRareItemRemainNum(SYS_BEHAVIOR_MONTHLY_RARE_ITEM, 0, itemId, num, data->serverMonthlyNum);
	// 服务器总产出控制
	num = _GetRareItemRemainNum(SYS_BEHAVIOR_TOTAL_RARE_ITEM, 0, itemId, num, data->serverTotalNum);

	return num;
}

void SysRecordMgr::RecordGotRareItem(ObjID_t roleId, UInt32 itemId, int num)
{
	auto data = RareItemDataEntryMgr::Instance()->FindEntryByType(RARE_CTRL_ITEM, itemId);
	if (!data)
	{
		return;
	}

	// 个人每日产出控制
	if (data->persionalDailyNum > 0)
	{
		UpdateRecord(SYS_BEHAVIOR_DAILY_RARE_ITEM, SYS_BEHAVIOR_OWNER_ROLE, roleId, itemId, num);
	}
	// 个人每周产出控制
	if (data->persionalWeeklyNum > 0)
	{
		UpdateRecord(SYS_BEHAVIOR_WEEKLY_RARE_ITEM, SYS_BEHAVIOR_OWNER_ROLE, roleId, itemId, num);
	}
	// 个人每月产出控制
	if (data->persionalMonthlyNum > 0)
	{
		UpdateRecord(SYS_BEHAVIOR_MONTHLY_RARE_ITEM, SYS_BEHAVIOR_OWNER_ROLE, roleId, itemId, num);
	}
	// 个人总产出控制
	if (data->persionalTotalNum > 0)
	{
		UpdateRecord(SYS_BEHAVIOR_TOTAL_RARE_ITEM, SYS_BEHAVIOR_OWNER_ROLE, roleId, itemId, num);
	}

	// 服务器每日产出控制
	if (data->serverDailyNum > 0)
	{
		UpdateRecord(SYS_BEHAVIOR_DAILY_RARE_ITEM, SYS_BEHAVIOR_OWNER_ROLE, 0, itemId, num);
	}
	// 服务器每周产出控制
	if (data->serverWeeklyNum > 0)
	{
		UpdateRecord(SYS_BEHAVIOR_WEEKLY_RARE_ITEM, SYS_BEHAVIOR_OWNER_ROLE, 0, itemId, num);
	}
	// 服务器每月产出控制
	if (data->serverMonthlyNum > 0)
	{
		UpdateRecord(SYS_BEHAVIOR_MONTHLY_RARE_ITEM, SYS_BEHAVIOR_OWNER_ROLE, 0, itemId, num);
	}
	// 服务器总产出控制
	if (data->serverTotalNum > 0)
	{
		UpdateRecord(SYS_BEHAVIOR_TOTAL_RARE_ITEM, SYS_BEHAVIOR_OWNER_ROLE, 0, itemId, num);
	}
}

void SysRecordMgr::RecordGotRareItem(RareControlType type, ObjID_t roleId, UInt32 parm1, int num)
{
	auto data = RareItemDataEntryMgr::Instance()->FindEntryByType(type, parm1);
	if (!data)
	{
		ErrorLogStream << "Can't find rare item data by type(" << (UInt32)type << ") and parm1(" << parm1 << ")!" << LogStream::eos;
		return;
	}

	// 个人每日产出控制
	if (data->persionalDailyNum > 0)
	{
		UpdateRecord(_GetSysBehaviorByRareControlType(type, SYS_BEHAVIOR_DAILY_BEGIN), SYS_BEHAVIOR_OWNER_ROLE, roleId, parm1, num);
	}
	// 个人每周产出控制
	if (data->persionalWeeklyNum > 0)
	{
		UpdateRecord(_GetSysBehaviorByRareControlType(type, SYS_BEHAVIOR_WEEKLY_BEGIN), SYS_BEHAVIOR_OWNER_ROLE, roleId, parm1, num);
	}
	// 个人每月产出控制
	if (data->persionalMonthlyNum > 0)
	{
		UpdateRecord(_GetSysBehaviorByRareControlType(type, SYS_BEHAVIOR_MONTHLY_BEGIN), SYS_BEHAVIOR_OWNER_ROLE, roleId, parm1, num);
	}
	// 个人总产出控制
	if (data->persionalTotalNum > 0)
	{
		UpdateRecord(_GetSysBehaviorByRareControlType(type, SYS_BEHAVIOR_TOTAL_BEGIN), SYS_BEHAVIOR_OWNER_ROLE, roleId, parm1, num);
	}

	// 服务器每日产出控制
	if (data->serverDailyNum > 0)
	{
		UpdateRecord(_GetSysBehaviorByRareControlType(type, SYS_BEHAVIOR_DAILY_BEGIN), SYS_BEHAVIOR_OWNER_ROLE, 0, parm1, num);
	}
	// 服务器每周产出控制
	if (data->serverWeeklyNum > 0)
	{
		UpdateRecord(_GetSysBehaviorByRareControlType(type, SYS_BEHAVIOR_WEEKLY_BEGIN), SYS_BEHAVIOR_OWNER_ROLE, 0, parm1, num);
	}
	// 服务器每月产出控制
	if (data->serverMonthlyNum > 0)
	{
		UpdateRecord(_GetSysBehaviorByRareControlType(type, SYS_BEHAVIOR_MONTHLY_BEGIN), SYS_BEHAVIOR_OWNER_ROLE, 0, parm1, num);
	}
	// 服务器总产出控制
	if (data->serverTotalNum > 0)
	{
		UpdateRecord(_GetSysBehaviorByRareControlType(type, SYS_BEHAVIOR_TOTAL_BEGIN), SYS_BEHAVIOR_OWNER_ROLE, 0, parm1, num);
	}
}

UInt32 SysRecordMgr::RareDropSolutionRemainNum(ObjID_t roleId, UInt32 solutionId, UInt32 num)
{
	auto data = RareItemDataEntryMgr::Instance()->FindEntryByType(RARE_CTRL_DROP_SOLUTION, solutionId);
	if (!data)
	{
		return num;
	}

	if (data->persionalTotalNum == 0)
	{
		return num;
	}

	// 目前就限制个人总产出
	num = _GetRareItemRemainNum(SYS_BEHAVIOR_TOTAL_DROP_SOLUTION, roleId, solutionId, num, data->persionalTotalNum);
	return num;
}

void SysRecordMgr::RecordGotRareDropSolution(ObjID_t roleId, UInt32 solutionId, int num)
{
	auto data = RareItemDataEntryMgr::Instance()->FindEntryByType(RARE_CTRL_DROP_SOLUTION, solutionId);
	if (!data)
	{
		return;
	}

	// 个人总产出控制
	if (data->persionalTotalNum > 0)
	{
		UpdateRecord(SYS_BEHAVIOR_TOTAL_DROP_SOLUTION, SYS_BEHAVIOR_OWNER_ROLE, roleId, solutionId, num);
		auto remainNum = _GetRareItemRemainNum(SYS_BEHAVIOR_TOTAL_DROP_SOLUTION, roleId, solutionId, num, data->persionalTotalNum);
		InfoLogStream << "player(" << roleId << ") got rare drop(" << solutionId << ") num(" << num << ") remainNum(" << remainNum << ")." << LogStream::eos;
	}
}

void SysRecordMgr::OnSysRecordDataReq(CLProtocol::SceneSysRecordDataReq& req)
{
	CLProtocol::SceneSysRecordDataRes res;
	res.ownerId = req.ownerId;
	res.sysRecordEvent = req.sysRecordEvent;
	res.extraParam = req.extraParam;

	ObjID_t roleId = req.ownerId;

	for (auto itemId : req.params)
	{
		CLProtocol::ProtoSysRecord record;
		record.param = itemId;

		UInt32 totalNum = 0;
		UInt32 remainNum = 0;

		auto data = RareItemDataEntryMgr::Instance()->FindEntryByType((RareControlType)req.rareControlType, itemId);
		if (!data)
		{
			continue;
		}

		// 个人每日产出控制
		if (data->persionalDailyNum > 0)
		{
			totalNum = data->persionalDailyNum;
			remainNum = _GetRareItemRemainNum(_GetSysBehaviorByRareControlType((RareControlType)req.rareControlType, SYS_BEHAVIOR_DAILY_BEGIN), roleId, itemId, data->persionalDailyNum);
		}
		// 个人每周产出控制
		if (data->persionalWeeklyNum > 0)
		{
			totalNum = data->persionalWeeklyNum;
			remainNum = _GetRareItemRemainNum(_GetSysBehaviorByRareControlType((RareControlType)req.rareControlType, SYS_BEHAVIOR_WEEKLY_BEGIN), roleId, itemId, data->persionalWeeklyNum);
		}
		// 个人每月产出控制
		if (data->persionalMonthlyNum > 0)
		{
			totalNum = data->persionalMonthlyNum;
			remainNum = _GetRareItemRemainNum(_GetSysBehaviorByRareControlType((RareControlType)req.rareControlType, SYS_BEHAVIOR_MONTHLY_BEGIN), roleId, itemId, data->persionalMonthlyNum);
		}
		// 个人总产出控制
		if (data->persionalTotalNum > 0)
		{
			totalNum = data->persionalTotalNum;
			remainNum = _GetRareItemRemainNum(_GetSysBehaviorByRareControlType((RareControlType)req.rareControlType, SYS_BEHAVIOR_TOTAL_BEGIN), roleId, itemId, data->persionalTotalNum);
		}

		// 服务器每日产出控制
		if (data->serverDailyNum > 0)
		{
			// 服务器每日产出控制
			UInt32 dailyNum = _GetRareItemMaxNumByTime(itemId);
			if (dailyNum == (UInt32)-1)
			{
				dailyNum = data->serverDailyNum;
			}
			else if (dailyNum == 0)
			{
				remainNum = 0;
			}

			totalNum = dailyNum;
			remainNum = _GetRareItemRemainNum(_GetSysBehaviorByRareControlType((RareControlType)req.rareControlType, SYS_BEHAVIOR_DAILY_BEGIN), 0, itemId, remainNum, dailyNum);
		}
		// 服务器每周产出控制
		if (data->serverWeeklyNum > 0)
		{
			totalNum = data->serverWeeklyNum;
			remainNum = _GetRareItemRemainNum(_GetSysBehaviorByRareControlType((RareControlType)req.rareControlType, SYS_BEHAVIOR_WEEKLY_BEGIN), 0, itemId, data->serverWeeklyNum);
		}
		// 服务器每月产出控制
		if (data->serverMonthlyNum > 0)
		{
			totalNum = data->serverMonthlyNum;
			remainNum = _GetRareItemRemainNum(_GetSysBehaviorByRareControlType((RareControlType)req.rareControlType, SYS_BEHAVIOR_MONTHLY_BEGIN), 0, itemId, data->serverMonthlyNum);
		}
		// 服务器总产出控制
		if (data->serverTotalNum > 0)
		{
			totalNum = data->serverTotalNum;
			remainNum = _GetRareItemRemainNum(_GetSysBehaviorByRareControlType((RareControlType)req.rareControlType, SYS_BEHAVIOR_TOTAL_BEGIN), 0, itemId, data->serverTotalNum);
		}

		record.totalNum = totalNum;
		record.remainNum = remainNum;

		res.records.push_back(record);
	}

	req.SendBack(res);
}

UInt32 SysRecordMgr::VipLimitItemRemainNum(ObjID_t roleId, UInt32 itemId, UInt32 num, UInt32 vipLimitId, UInt8 vipLv)
{
	auto data = DropItemVipLimitDataEntryMgr::Instance()->FindEntry(vipLimitId);
	if (!data)
	{
		InfoLogStream << "not find vip limit item id : " << vipLimitId << LogStream::eos;
		return 0;
	}

	if (vipLv >= data->vipLimits.size())
	{
		InfoLogStream << "vipLimits size is err id : " << vipLimitId << LogStream::eos;
		return 0;
	}

	UInt32 limit = data->vipLimits[vipLv];

	if (limit == 0)
	{
		return num;
	}

	UInt32 dailyGetNum = GetRecordValue(SYS_BEHAVIOR_DAILY_ITEM_GET_NUM, SYS_BEHAVIOR_OWNER_ROLE, roleId, itemId);
	if (dailyGetNum >= limit)
	{
		return 0;
	}
	return (num <= (limit - dailyGetNum)) ? num : (limit - dailyGetNum);
}

void SysRecordMgr::RecordGotVipLimitItem(ObjID_t roleId, UInt32 itemId, int num)
{

}

void SysRecordMgr::_AddRecord(SysRecord* record)
{
	if (record)
	{
		m_sysRecords[record->GetKey()] = record;
	}
}

SysRecord* SysRecordMgr::_CreateRecord(SysBehavior behavior, SysBehaviorOwnerType ownerType, ObjID_t owner, UInt32 param1, UInt32 num)
{
	SysRecord* record = new SysRecord();
	if (!record)
	{
		return NULL;
	}

	record->guid = CLRecordClient::Instance()->GenGuid();
	record->behavior = behavior;
	record->ownerType = ownerType;
	record->owner = owner;
	record->param1 = param1;
	record->num = num;
	record->recordTime = (UInt32)CURRENT_TIME.Sec();
	_AddRecord(record);
	return record;
}

bool SysRecordMgr::_IsTimeOut(SysBehavior behavior, UInt32 recordTime)
{
	if (behavior > SYS_BEHAVIOR_DAILY_BEGIN && behavior < SYS_BEHAVIOR_DAILY_END)
	{
		// 每天更新
		Avalon::Date recordDate(Avalon::Time((UInt64)recordTime * Avalon::Time::MSECS_OF_SEC));
		Avalon::Date timeoutDate(recordDate);
		timeoutDate.Hour(6);
		timeoutDate.Min(0);
		timeoutDate.Sec(0);

		UInt32 timeoutTime = (UInt32)timeoutDate.ToTime().Sec();
		if (recordDate.Hour() >= 6)
		{
			timeoutTime += Avalon::Time::SECS_OF_DAY;
		}

		if (timeoutTime < (UInt32)CURRENT_TIME.Sec())
		{
			return true;
		}
	}
	else if (behavior > SYS_BEHAVIOR_WEEKLY_BEGIN && behavior < SYS_BEHAVIOR_WEEKLY_END)
	{
		// 每周更新
		Avalon::Date recordDate(Avalon::Time((UInt64)recordTime * Avalon::Time::MSECS_OF_SEC));
		Avalon::Date timeoutDate(recordDate);
		timeoutDate.Hour(6);
		timeoutDate.Min(0);
		timeoutDate.Sec(0);

		UInt32 timeoutTime = (UInt32)timeoutDate.ToTime().Sec();
		if (recordDate.WDay() != 0 || recordDate.Hour() >= 6)
		{
			timeoutTime += (8 - recordDate.WDay()) * Avalon::Time::SECS_OF_DAY;
		}

		if (timeoutTime < (UInt32)CURRENT_TIME.Sec())
		{
			return true;
		}
	}
	else if (behavior > SYS_BEHAVIOR_MONTHLY_BEGIN && behavior < SYS_BEHAVIOR_MONTHLY_END)
	{
		// 每月更新
		Avalon::Date recordDate(Avalon::Time((UInt64)recordTime * Avalon::Time::MSECS_OF_SEC));
		Avalon::Date timeoutDate(recordDate);
		timeoutDate.Hour(6);
		timeoutDate.Min(0);
		timeoutDate.Sec(0);

		UInt32 timeoutTime = (UInt32)timeoutDate.ToTime().Sec();
		if (recordDate.WDay() != 0 || recordDate.Hour() >= 6)
		{
			if (timeoutDate.Month() == 12)
			{
				timeoutDate.Year(timeoutDate.Year() + 1);
				timeoutDate.Month(1);
			}
			else
			{
				timeoutDate.Month(timeoutDate.Month() + 1);
			}
			timeoutDate.Day(1);

			timeoutTime = (UInt32)timeoutDate.ToTime().Sec();
		}

		if (timeoutTime < (UInt32)CURRENT_TIME.Sec())
		{
			return true;
		}
	}

	return false;
}

void SysRecordMgr::_RecordToDB(SysRecord* record, bool isNew)
{
	if (isNew)
	{
		CLInsertCommand *cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_sys_record", record->guid);
		cmd->PutData("behavior", (UInt32)record->behavior);
		cmd->PutData("owner_type", (UInt32)record->ownerType);
		cmd->PutData("owner", record->owner);
		cmd->PutData("param1", record->param1);
		cmd->PutData("num", record->num);
		cmd->PutData("record_time", record->recordTime);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
	else
	{
		CLUpdateCommand *cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_sys_record", record->guid, false);
		cmd->PutData("num", record->num);
		cmd->PutData("record_time", record->recordTime);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
}

UInt32 SysRecordMgr::_GetRareItemRemainNum(SysBehavior behavior, ObjID_t roleId, UInt32 itemId, UInt32 num, UInt32 maxNum)
{
	if (num == 0 || maxNum == 0)
	{
		return num;
	}

	auto recordNum = GetRecordValue(behavior, SYS_BEHAVIOR_OWNER_ROLE, roleId, itemId);
	if (recordNum >= maxNum)
	{
		num = 0;
		return num;
	}

	auto remainNum = maxNum - recordNum;
	num = num > remainNum ? remainNum : num;
	return num;
}

UInt32 SysRecordMgr::_GetRareItemRemainNum(SysBehavior behavior, ObjID_t roleId, UInt32 itemId, UInt32 maxNum)
{
	auto recordNum = GetRecordValue(behavior, SYS_BEHAVIOR_OWNER_ROLE, roleId, itemId);
	if (recordNum >= maxNum)
	{
		return 0;
	}

	return maxNum - recordNum;
}

void SysRecordMgr::_CountRareItemMaxNumByTime()
{
	m_rareItemNumLimitByTime.clear();

	class RareItemDataVistor : public Avalon::DataEntryVisitor<RareItemDataEntry>
	{
	public:
		bool operator()(RareItemDataEntry *dataEntry)
		{
			if (dataEntry->type == RARE_CTRL_ITEM && dataEntry->serverDailyNum > 0)
			{
				datas.push_back(dataEntry);
			}
			
			return true;
		}

		std::vector<RareItemDataEntry*> datas;
	};

	// 今天的开始和结束时间
	UInt32 todayStartTime = _CountTodayStartTime();
	//UInt32 todayEndTime = todayStartTime + Avalon::Time::SECS_OF_DAY;

	RareItemDataVistor vistor;
	RareItemDataEntryMgr::Instance()->Visit(vistor);
	for (auto data : vistor.datas)
	{
		// 如果没填具体的时间段，那就是全天控制
		if (data->timeLimit.empty())
		{
			// 全天控制按照 06:00-12:00,12:00-18:00,18:00-24:00,第二天的00:00-06:00四个时段控制
			// 每个时间段的产出比例为 10%, 10%, 30%, 50%
			UInt32 everyDataSectionRate[] = { 10, 10, 30, 50 };
			UInt32 sectionNum = sizeof(everyDataSectionRate) / sizeof(everyDataSectionRate[0]);
			UInt32 everySectionSec = 24 / sectionNum * Avalon::Time::SECS_OF_HOUR;
			UInt32 dailyNum = data->serverDailyNum;
			UInt32 index = 0;

			for (UInt32 i = 0; i < sectionNum; i++)
			{	
				UInt32 startTime = todayStartTime + i * everySectionSec;
				UInt32 endTime = startTime + everySectionSec;
				UInt32 maxNum = data->serverDailyNum * everyDataSectionRate[i] / 100;
				if (i == sectionNum - 1)
				{
					maxNum = dailyNum - dailyNum * (100 - everyDataSectionRate[i]) / 100;
				}

				_CountRareItemMaxNumByTime(data->id, startTime, endTime, index, maxNum);
				index += maxNum;
			}
		}
		else if (data->timeLimitTotalMin > 0)
		{
			UInt32 totalMin = data->timeLimitTotalMin;
			UInt32 dailyNum = data->serverDailyNum;
			UInt32 index = 0;

			for (UInt32 i = 0; i < data->timeLimit.size(); i++)
			{
				const auto& openTime = data->timeLimit[i];

				auto beginDate = CURRENT_DATE;
				beginDate.Hour(openTime.startTime.hour);
				beginDate.Min(openTime.startTime.minute);
				beginDate.Sec(openTime.startTime.second);

				auto endDate = CURRENT_DATE;
				endDate.Hour(openTime.endTime.hour);
				endDate.Min(openTime.endTime.minute);
				endDate.Sec(openTime.endTime.second);

				UInt32 startTime = (UInt32)beginDate.ToTime().Sec();
				UInt32 endTime = (UInt32)endDate.ToTime().Sec();
				UInt32 durationMin = (endTime - startTime) / Avalon::Time::SECS_OF_MIN;

				UInt32 maxNum = dailyNum * durationMin / totalMin;
				if (i == data->timeLimit.size() - 1)
				{
					maxNum = dailyNum - dailyNum * (totalMin - durationMin) / totalMin;
				}

				_CountRareItemMaxNumByTime(data->id, startTime, endTime, index, maxNum);
				index += maxNum;
			}
		}
	}
}

void SysRecordMgr::_CountRareItemMaxNumByTime(UInt32 itemId, UInt32 startTime, UInt32 endTime, UInt32 beginNum, UInt32 maxNum)
{
	DebugLogStream << "count rare item(" << itemId << ") num, start(" << startTime << ") endTime(" << endTime << ") maxNum(" << maxNum << ")." << LogStream::eos;
	
	UInt32 durationMin = (endTime - startTime) / 60;
	if (durationMin == 0 || maxNum == 0)
	{
		return;
	}

	// 间隔时间(min)
	UInt32 intervalMin = durationMin / maxNum == 0 ? 1 : durationMin / maxNum;
	UInt32 intervalSec = intervalMin * Avalon::Time::SECS_OF_MIN;
	// 步长，每个间隔时间内的数量
	UInt32 stepSize = 1;
	if (durationMin < maxNum)
	{
		stepSize = maxNum / durationMin;
	}
	int additionNum = maxNum - durationMin / intervalMin * stepSize;
	additionNum = additionNum < 0 ? 0 : additionNum;

	UInt32 num = 0;
	UInt32 nextStartTime = startTime;
	for (; startTime < endTime && num < maxNum; startTime += intervalSec)
	{
		num += stepSize;
		if (additionNum > 0)
		{
			num++;
			additionNum--;
		}

		if (num > maxNum)
		{
			num = maxNum;
		}

		RareItemLimitByTime limit;
		limit.beginTime = nextStartTime;
		nextStartTime = Avalon::Random::RandBetween(startTime + intervalSec, startTime + 2 * intervalSec);
		if (nextStartTime >= endTime)
		{
			nextStartTime = endTime;
		}
		limit.endTime = nextStartTime;
		limit.itemId = itemId;
		limit.maxNum = beginNum + num;
		m_rareItemNumLimitByTime[limit] = beginNum + num;
	}
}

UInt32 SysRecordMgr::_GetRareItemMaxNumByTime(UInt32 itemId)
{
	UInt32 curSec = (UInt32)CURRENT_TIME.Sec();
	RareItemLimitByTime min;
	min.itemId = itemId;
	min.beginTime = curSec;

	DebugLogStream << "get item(" << itemId << ")'s max num, now(" << _GetTimeStr(curSec) << ")." << LogStream::eos;

	auto itr = m_rareItemNumLimitByTime.lower_bound(min);
	if (itr == m_rareItemNumLimitByTime.end())
	{
		return 0;
	}

	auto& limit = itr->first;
	UInt32 num = itr->second;

	DebugLogStream << "item(" << itemId << ")'s first iterator, begin(" << _GetTimeStr(limit.beginTime) << ") end(" << _GetTimeStr(limit.endTime) << ") num(" << limit.maxNum << ")." << LogStream::eos;

	if (limit.itemId == itemId && limit.beginTime <= curSec && limit.endTime >= curSec)
	{
		DebugLogStream << "get item(" << itemId << ")'s max num, time(" << _GetTimeStr(curSec) << ") max num(" << num << ")." << LogStream::eos;
		return num;
	}
	else if (itr != m_rareItemNumLimitByTime.begin())
	{
		--itr;
		auto& limit = itr->first;
		UInt32 num = itr->second;

		DebugLogStream << "item(" << itemId << ")'s second iterator, begin(" << _GetTimeStr(limit.beginTime) << ") end(" << _GetTimeStr(limit.endTime) << ") num(" << limit.maxNum << ")." << LogStream::eos;

		if (limit.itemId == itemId && limit.beginTime <= curSec && limit.endTime >= curSec)
		{
			DebugLogStream << "get item(" << itemId << ")'s max num, time(" << _GetTimeStr(curSec) << ") max num(" << num << ")." << LogStream::eos;
			return num;
		}
	}

	return 0;
}

UInt32 SysRecordMgr::_CountTodayStartTime()
{
	auto beginDate = CURRENT_DATE;
	if (beginDate.Hour() >= 6)
	{
		beginDate.Hour(6);
		beginDate.Min(0);
		beginDate.Sec(0);
		return (UInt32)beginDate.ToTime().Sec();
	}

	beginDate.Hour(6);
	beginDate.Min(0);
	beginDate.Sec(0);
	return (UInt32)beginDate.ToTime().Sec() - Avalon::Time::SECS_OF_DAY;
}

std::string SysRecordMgr::_GetTimeStr(UInt32 time)
{
	Avalon::Time aTime((UInt64)time * Avalon::Time::MSECS_OF_SEC);
	Avalon::Date date(aTime);

	char str[50];
	sprintf(str, "%02d:%02d:%02d", date.Hour(), date.Min(), date.Sec());
	return str;
}

SysBehavior SysRecordMgr::_GetSysBehaviorByRareControlType(RareControlType rareType, SysBehavior behaviorType)
{
	switch (rareType)
	{
	case RARE_CTRL_ITEM:
		break;
	case RARE_CTRL_DROP_SOLUTION:
		break;
	case RARE_CTRL_DUNGEON_MATERIAL_SHOP:
		if (behaviorType == SYS_BEHAVIOR_DAILY_BEGIN) return SYS_BEHAVIOR_DAILY_DUNGEON_MATERIAL_SHOP_ITEM;
		if (behaviorType == SYS_BEHAVIOR_WEEKLY_BEGIN) return SYS_BEHAVIOR_WEEKLY_DUNGEON_MATERIAL_SHOP_ITEM;
		if (behaviorType == SYS_BEHAVIOR_MONTHLY_BEGIN) return SYS_BEHAVIOR_MONTHLY_DUNGEON_MATERIAL_SHOP_ITEM;
		if (behaviorType == SYS_BEHAVIOR_TOTAL_BEGIN) return SYS_BEHAVIOR_TOTAL_DUNGEON_MATERIAL_SHOP_ITEM;
		break;
	case RARE_CTRL_DUNGEON_EQUIP_SHOP:
		if (behaviorType == SYS_BEHAVIOR_DAILY_BEGIN) return SYS_BEHAVIOR_DAILY_DUNGEON_MATERIAL_EQUIP_ITEM;
		if (behaviorType == SYS_BEHAVIOR_WEEKLY_BEGIN) return SYS_BEHAVIOR_WEEKLY_DUNGEON_EQUIP_SHOP_ITEM;
		if (behaviorType == SYS_BEHAVIOR_MONTHLY_BEGIN) return SYS_BEHAVIOR_MONTHLY_DUNGEON_EQUIP_SHOP_ITEM;
		if (behaviorType == SYS_BEHAVIOR_TOTAL_BEGIN) return SYS_BEHAVIOR_TOTAL_DUNGEON_EQUIP_SHOP_ITEM;
		break;
	case RARE_CTRL_DUNGEON_JEWELRY_SHOP:
		if (behaviorType == SYS_BEHAVIOR_DAILY_BEGIN) return SYS_BEHAVIOR_DAILY_DUNGEON_MATERIAL_JEWELRY_ITEM;
		if (behaviorType == SYS_BEHAVIOR_WEEKLY_BEGIN) return SYS_BEHAVIOR_WEEKLY_DUNGEON_JEWELRY_SHOP_ITEM;
		if (behaviorType == SYS_BEHAVIOR_MONTHLY_BEGIN) return SYS_BEHAVIOR_MONTHLY_DUNGEON_JEWELRY_SHOP_ITEM;
		if (behaviorType == SYS_BEHAVIOR_TOTAL_BEGIN) return SYS_BEHAVIOR_TOTAL_DUNGEON_JEWELRY_SHOP_ITEM;
		break;
	default:
		break;
	}

	ErrorLogStream << "Error rare control type(" << (UInt32)rareType << ") or sys behavior(" << behaviorType << ")!" << LogStream::eos;
	return SYS_BEHAVIOR_NULL;
}