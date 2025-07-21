#include "ActiveRecordMgr.h"

#include "CLRecordClient.h"
#include "CLRecordCallback.h"

OpActivityRecordMgr::OpActivityRecordMgr()
{

}

OpActivityRecordMgr::~OpActivityRecordMgr()
{
	for (auto itr : m_Records)
	{
		delete itr.second;
	}
}

class SelectOpActivityRecordCallback : public CLRecordCallback
{
public:
	void OnFinished()
	{
		while (NextRow())
		{
			OpActivityRecord* record = new OpActivityRecord();
			record->guid = GetKey();
			record->behavior = (OpActivityBehavior)(UInt32)GetData("behavior");
			record->ownerType = (OpActivityBehaviorOwnerType)(UInt32)GetData("owner_type");
			record->owner = GetData("owner");
			record->param1 = GetData("param1");
			record->num = GetData("num");
			record->recordTime = GetData("record_time");

			OpActivityRecordMgr::Instance()->LoadRecord(record);
		}
	}

	void OnFailed(UInt32 errorcode)
	{
		
	}
};

bool OpActivityRecordMgr::Init()
{
	LoadRecordFromDB(0, 0);
	return true;
}

void OpActivityRecordMgr::OnTick(const Avalon::Time& now)
{

}

void OpActivityRecordMgr::LoadRecordFromDB(UInt32 accid, ObjID_t roleid)
{
	{
		CLSelectCommand	*cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_activity_op_record");
		cmd->PutCond("owner_type", (UInt32)OP_ACTIVITY_BEHAVIOR_OWNER_ROLE);
		cmd->PutCond("owner", roleid);
		CLRecordClient::Instance()->SendCommand(cmd, new SelectOpActivityRecordCallback());
	}

	{
		CLSelectCommand	*cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_activity_op_record");
		cmd->PutCond("owner_type", (UInt32)OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT);
		cmd->PutCond("owner", accid);
		CLRecordClient::Instance()->SendCommand(cmd, new SelectOpActivityRecordCallback());
	}
}

void  OpActivityRecordMgr::LoadRoleRecordFromDB(ObjID_t roleid)
{
	CLSelectCommand	*cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_activity_op_record");
	cmd->PutCond("owner_type", (UInt32)OP_ACTIVITY_BEHAVIOR_OWNER_ROLE);
	cmd->PutCond("owner", roleid);
	CLRecordClient::Instance()->SendCommand(cmd, new SelectOpActivityRecordCallback());
}

void  OpActivityRecordMgr::LoadAcctRecordFromDB(UInt32 accid)
{
	CLSelectCommand	*cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_activity_op_record");
	cmd->PutCond("owner_type", (UInt32)OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT);
	cmd->PutCond("owner", accid);
	CLRecordClient::Instance()->SendCommand(cmd, new SelectOpActivityRecordCallback());
}

static std::set<UInt32> MergrMaxRecord = { OP_ACTIVITY_BEHAVIOR_CREDIT_POINT_TRANSFER_TIME };

bool OpActivityRecordMgr::LoadRecord(OpActivityRecord* record)
{
	if (!record)
	{
		return false;
	}

	auto iter = m_Records.find(record->GetKey());
	if (iter == m_Records.end() || !iter->second)
	{
		_AddRecord(record);
		return true;
	}

	OpActivityRecord* tmpRecord = iter->second;
	// 合并数据
	if (tmpRecord->guid != record->guid)
	{
		if (tmpRecord->num > 0 && _IsTimeOut(tmpRecord->behavior, tmpRecord->recordTime))
		{
			tmpRecord->num = 0;
			tmpRecord->recordTime = (UInt32)CURRENT_TIME.Sec();
		}

		if (record->num > 0 && _IsTimeOut(record->behavior, record->recordTime))
		{
			record->num = 0;
			record->recordTime = (UInt32)CURRENT_TIME.Sec();
		}

		// 取较大值
		if (MergrMaxRecord.count(record->behavior))
		{
			if (tmpRecord->num < record->num)
			{
				tmpRecord->num = record->num;
			}
		}
		else
		{	// 累加
			tmpRecord->num += record->num;
		}

		if (tmpRecord->recordTime < record->recordTime)
		{
			tmpRecord->recordTime = record->recordTime;
		}

		// 一个更新，另一个删除 
		_RecordToDB(tmpRecord);
		_DeleteFromDB(record);
	}
	
	delete record;
	return true;
}

OpActivityRecord* OpActivityRecordMgr::GetRecord(OpActivityBehavior behavior, OpActivityBehaviorOwnerType ownerType, ObjID_t owner, UInt32 param1)
{
	OpActivityRecordKey key(behavior, ownerType, owner, param1);

	auto itr = m_Records.find(key);
	if (itr == m_Records.end() || !itr->second)
	{
		return NULL;
	}

	OpActivityRecord* record = itr->second;

	// 每次取数据时判断数据有没有过时，如果已经过时就刷新数据，再更新到数据库
	if (record->num > 0 && _IsTimeOut(record->behavior, record->recordTime))
	{
		record->num = 0;
		record->recordTime = (UInt32)CURRENT_TIME.Sec();

		_RecordToDB(record);
	}

	return record;
}

UInt32 OpActivityRecordMgr::GetRecordValue(OpActivityBehavior behavior, OpActivityBehaviorOwnerType ownerType, ObjID_t owner, UInt32 param1)
{
	auto record = GetRecord(behavior, ownerType, owner, param1);
	return record == NULL ? 0 : record->num;
}

void OpActivityRecordMgr::UpdateRecord(OpActivityBehavior behavior, OpActivityBehaviorOwnerType ownerType, ObjID_t owner, UInt32 param1, int addNum)
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

void OpActivityRecordMgr::SetRecord(OpActivityBehavior behavior, OpActivityBehaviorOwnerType ownerType, ObjID_t owner, UInt32 param1, int num)
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

void OpActivityRecordMgr::_AddRecord(OpActivityRecord* record)
{
	if (record)
	{
		m_Records[record->GetKey()] = record;
	}
}

OpActivityRecord* OpActivityRecordMgr::_CreateRecord(OpActivityBehavior behavior, OpActivityBehaviorOwnerType ownerType, ObjID_t owner, UInt32 param1, UInt32 num)
{
	OpActivityRecord* record = new OpActivityRecord();
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

bool OpActivityRecordMgr::_IsTimeOut(OpActivityBehavior behavior, UInt32 recordTime)
{
	if (behavior > OP_ACTIVITY_BEHAVIOR_DAILY_BEGIN && behavior < OP_ACTIVITY_BEHAVIOR_DAILY_END)
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

		if (timeoutTime <= (UInt32)CURRENT_TIME.Sec())
		{
			return true;
		}
	}
	else if (behavior > OP_ACTIVITY_BEHAVIOR_WEEKLY_BEGIN && behavior < OP_ACTIVITY_BEHAVIOR_WEEKLY_END)
	{
		// 每周更新
		Avalon::Date recordDate(Avalon::Time((UInt64)recordTime * Avalon::Time::MSECS_OF_SEC));
		Avalon::Date timeoutDate(recordDate);
		timeoutDate.Hour(6);
		timeoutDate.Min(0);
		timeoutDate.Sec(0);

		UInt32 timeoutTime = (UInt32)timeoutDate.ToTime().Sec();
		if (recordDate.WDay() != 1 || recordDate.Hour() >= 6)
		{
			timeoutTime += (8 - recordDate.WDay()) * Avalon::Time::SECS_OF_DAY;
		}

		if (timeoutTime <= (UInt32)CURRENT_TIME.Sec())
		{
			return true;
		}
	}
	else if (behavior > OP_ACTIVITY_BEHAVIOR_MONTHLY_BEGIN && behavior < OP_ACTIVITY_BEHAVIOR_MONTHLY_END)
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

		if (timeoutTime <= (UInt32)CURRENT_TIME.Sec())
		{
			return true;
		}
	}

	return false;
}

void OpActivityRecordMgr::_RecordToDB(OpActivityRecord* record, bool isNew)
{
	if (isNew)
	{
		CLInsertCommand *cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_activity_op_record", record->guid);
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
		CLUpdateCommand *cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_activity_op_record", record->guid, false);
		cmd->PutData("num", record->num);
		cmd->PutData("record_time", record->recordTime);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
}

void OpActivityRecordMgr::_DeleteFromDB(OpActivityRecord* record)
{
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_activity_op_record", record->guid);
	CLRecordClient::Instance()->SendCommand(cmd);
}

UInt32 OpActivityRecordMgr::RareRewardRemainNum(UInt32 rewardId, UInt32 num)
{
	UInt32 totalNum = GetRecordValue(OP_ACTIVITY_BEHAVIOR_TOTAL_SERVER_ITEM_NUM, OP_ACTIVITY_BEHAVIOR_OWNER_ROLE, 0, rewardId);
	if (totalNum >= num)
	{
		return 0;
	}

	return (num - totalNum);
}

