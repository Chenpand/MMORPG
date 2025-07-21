#include "DailyTodo.h"

#include <CLRecordClient.h>
#include <CLDailyTodoDataEntry.h>
#include "CLDailyTodoDefine.h"
#include "WSPlayer.h"

class LoadDailyTidoCallback : public SelectRoleInfoCallback
{
public:
	LoadDailyTidoCallback(WSPlayer* player) :SelectRoleInfoCallback(player){}
	~LoadDailyTidoCallback(){}

	void OnFinished(WSPlayer* player)
	{
		player->GetDailyTodoMgr().OnLoadDataRet(this);
	}

	void OnFailed(WSPlayer* player)
	{

	}
};

bool DailyTodo::LoadFromDb(CLRecordCallback *pCallback, DailyTodoItemDataEntry* dataEntry)
{
	if (!pCallback || !dataEntry)
	{
		return false;
	}
	this->SetDBData(pCallback);
	this->SetID(pCallback->GetKey());
	this->m_subType = dataEntry->subType;
	this->m_DayProgMax = dataEntry->dayRecommendNum;
	this->m_WeekProgMax = dataEntry->weekRecommendDayNum;
	this->ClearDirty();
	return true;
}

DailyTodoInfo DailyTodo::ToClient()
{
	CheckDayCompeleted();
	CheckWeekCompeleted();
	DailyTodoInfo info;
	info.id = this->GetID();
	info.dataId = m_DataId;
	info.dayProgress = m_DayProgress;
	info.dayProgTime = m_DayProgTime;
	info.dayProgMax = m_DayProgMax;
	info.weekProgress = m_WeekProgress;
	info.weekProgTime = m_WeekProgTime;
	info.weekProgMax = m_WeekProgMax;
	return info;
}

DailyTodo::DailyTodo()
{
	m_subType = 0;
	m_DayProgMax = 0;
	m_WeekProgMax = 0;
}

DailyTodo::~DailyTodo()
{

}

bool DailyTodo::Init(WSPlayer* player, DailyTodoItemDataEntry* dataEntry)
{
	if (!player || !dataEntry)
	{
		return false;
	}

	m_OwnerId = player->GetID();
	m_DataId = dataEntry->GetKey();
	m_DayProgress = 0;
	m_DayProgTime = (UInt32)CURRENT_TIME.Sec();
	m_WeekProgress = 0;
	m_WeekProgTime = (UInt32)CURRENT_TIME.Sec();
	m_subType = dataEntry->subType;
	m_DayProgMax = dataEntry->dayRecommendNum;
	m_WeekProgMax = dataEntry->weekRecommendDayNum;
	return true;
}

void DailyTodo::Update()
{
	if (GetID() == 0)
	{
		Insert();
	}
	else
	{
		CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_daily_todo", GetID(), false);
		this->GetDBData(cmd, true);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
}

void DailyTodo::Insert()
{
	SetID(CLRecordClient::Instance()->GenGuid());
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_daily_todo", GetID());
	this->GetDBData(cmd, false);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void DailyTodo::Delete()
{
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_daily_todo", GetID());
	CLRecordClient::Instance()->SendCommand(cmd);
}

bool DailyTodo::IncDayProgress(UInt32 inc)
{
	if (CheckDayCompeleted())
	{
		return false;
	}

	m_DayProgress = m_DayProgress + inc;
	m_DayProgTime = (UInt32)CURRENT_TIME.Sec();
	Update();
	return true;
}

bool DailyTodo::UpdateDayProgress(UInt32 progress)
{
	if (CheckDayCompeleted())
	{
		return false;
	}

	m_DayProgress = progress;
	m_DayProgTime = (UInt32)CURRENT_TIME.Sec();
	Update();
	return true;
}

bool DailyTodo::IncWeekProgress(UInt32 inc)
{
	if (CheckWeekCompeleted())
	{
		return false;
	}

	m_WeekProgress = m_WeekProgress + inc;
	m_WeekProgTime = (UInt32)CURRENT_TIME.Sec();
	Update();
	return true;
}

bool DailyTodo::UpdateWeekProgress(UInt32 progress)
{
	if (CheckWeekCompeleted())
	{
		return false;
	}

	m_WeekProgress = progress;
	m_WeekProgTime = (UInt32)CURRENT_TIME.Sec();
	Update();
	return true;
}

bool DailyTodo::CheckDayCompeleted()
{
	UInt32 curTm = (UInt32)CURRENT_TIME.Sec();
	if (!CheckInSameLogicDay(curTm, m_DayProgTime))
	{
		m_DayProgress = 0;
		m_DayProgTime = curTm;
		Update();
		return false;
	}

	return m_DayProgress >= m_DayProgMax;
}

bool DailyTodo::CheckWeekCompeleted()
{
	DailyTodoItemDataEntry* cfg = DailyTodoItemDataEntryMgr::Instance()->FindEntry(m_DataId);
	if (NULL == cfg)
	{
		ErrorLogStream << m_OwnerId << " not find daily to do cfg:" << m_DataId << LogStream::eos;
		return false;
	}

	UInt32 refreshWeek = cfg->refreshWeek != 0 ? cfg->refreshWeek : 1;

	UInt64 curTm = CURRENT_TIME.MSec();
	if (!isSameWeek(curTm, (UInt64)m_WeekProgTime * 1000, refreshWeek, 6))
	{
		m_WeekProgress = 0;
		m_WeekProgTime = (UInt32)curTm;
		Update();
		return false;
	}

	return m_WeekProgress >= m_WeekProgMax;
}

DailyTodoMgr::DailyTodoMgr()
{
	
}

DailyTodoMgr::~DailyTodoMgr()
{
	for (auto iter : m_DailyTodos)
	{
		if (iter.second)
		{
			CL_SAFE_DELETE(iter.second);
		}
	}
	m_DailyTodos.clear();
}

WSPlayer* DailyTodoMgr::GetOwner()
{
	return m_pOwner;
}

void DailyTodoMgr::LoadDailyTodos()
{
	//╪стьм╥он
	CLSelectCommand* selectCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_daily_todo");
	selectCmd->PutCond("owner_id", GetOwner()->GetID());
	CLRecordClient::Instance()->SendCommand(selectCmd, new LoadDailyTidoCallback(GetOwner()));
}

void DailyTodoMgr::OnLoadDataRet(CLRecordCallback* callback)
{
	if (callback == NULL) return;
	while (callback->NextRow())
	{
		UInt32	data_id = callback->GetData("data_id");
	
		DailyTodoItemDataEntry* dataEntry = DailyTodoItemDataEntryMgr::Instance()->FindEntry(data_id);
		if (!dataEntry)
		{
			ErrorLogStream << " load dailyTodo err, data entry not find , player id = " << GetOwner()->GetID() << ", guid = " << callback->GetKey()
				<< ", data_id = " << data_id << LogStream::eos;
			continue;
		}
		DailyTodo* pDailyTodo = new DailyTodo;
		if (!pDailyTodo)
		{
			ErrorLogStream << " new dailyTodo err, data entry not find , player id = " << GetOwner()->GetID() << ", guid = " << callback->GetKey()
				<< ", data_id = " << data_id << LogStream::eos;
			continue;
		}

		pDailyTodo->LoadFromDb(callback, dataEntry);
		
		if (!_AddOneDailyTodo(pDailyTodo))
		{
			ErrorLogStream << " add pDailyTodo fail , player id = " << callback->GetKey()
				<< ", data_id = " << data_id << LogStream::eos;
			delete pDailyTodo;
		}
	}
}

bool DailyTodoMgr::_AddOneDailyTodo(DailyTodo* pDailyTodo)
{
	if (!pDailyTodo)
	{
		return false;
	}
	auto it = m_DailyTodos.find(pDailyTodo->SubType());
	if (it != m_DailyTodos.end())
	{
		ErrorLogStream << "add one duplicate dailyTodo id = " << pDailyTodo->GetID() << ", subType = " << pDailyTodo->SubType()
			<< ",  owner = " << GetOwner()->GetID() << LogStream::eos;
		return false;
	}

	m_DailyTodos.insert(std::pair<UInt8, DailyTodo*>(pDailyTodo->SubType(), pDailyTodo));
	DebugLogStream << "add one dailyTodo id = " << pDailyTodo->GetID() << ", owner = " << pDailyTodo->Owner() 
		<< ",  dataId = " << pDailyTodo->DataId() << LogStream::eos;
	return true;
}

void DailyTodoMgr::GetDailyTodoList(std::vector<DailyTodoInfo>& list)
{
	for (auto dailyTodo : m_DailyTodos)
	{
		list.push_back(dailyTodo.second->ToClient());
	}
}

void DailyTodoMgr::OnPlayerClearDungeon(const DungeonDataEntry* dungeonData, LevelScore score, UInt32 usedTime)
{
	if (!dungeonData) return;

	if (score <= LS_C)
	{
		return;
	}

	UInt8 subType = DailyTodoItemDataEntryMgr::Instance()->DungeonTypeToDailyToDoSubType(dungeonData);
	if (subType == DTSTP_NONE)
	{
		return;
	}

	DailyTodo*  pDailyTodo = _GetAndCreateDailyTodo((DailyTodoSubType)subType);
	if (!pDailyTodo)
	{
		return;
	}
	if (pDailyTodo->CheckWeekCompeleted())
	{
		return;
	}

	if (!pDailyTodo->IncDayProgress(1))
	{
		return;
	}

	if (pDailyTodo->CheckDayCompeleted())
	{
		pDailyTodo->IncWeekProgress(1);
	}
	return;
}

void DailyTodoMgr::OnPlayerTaskUpdate(UInt8 subType, UInt32 value, bool isOver)
{
	DailyTodo*  pDailyTodo = _GetAndCreateDailyTodo((DailyTodoSubType)subType);
	if (!pDailyTodo)
	{
		return;
	}

	if (pDailyTodo->CheckWeekCompeleted())
	{
		return;
	}

	if (isOver)
	{
		if (!pDailyTodo->IncDayProgress(value))
			return;
	}
	else
	{
		if (!pDailyTodo->UpdateDayProgress(value))
			return;
	}

	if (pDailyTodo->CheckDayCompeleted())
	{
		pDailyTodo->IncWeekProgress(1);
	}
	return;
}

DailyTodo* DailyTodoMgr::_GetAndCreateDailyTodo(DailyTodoSubType subType)
{
	DailyTodo* pDailyTodo = _FindDailyTodo(subType);
	if (pDailyTodo)
	{
		return pDailyTodo;
	}

	DailyTodoItemDataEntry* entry = DailyTodoItemDataEntryMgr::Instance()->GetEntryBySubType(subType);
	if (!entry)
	{
		ErrorLogStream << "create dailyTodo not find entry subType = " << subType << ", player = " << GetOwner()->GetID() << LogStream::eos;
		return NULL;
	}

	pDailyTodo = new DailyTodo;
	if (!pDailyTodo->Init(GetOwner(), entry))
	{
		delete pDailyTodo;
		return NULL;
	}
	
	if (!_AddOneDailyTodo(pDailyTodo))
	{
		delete pDailyTodo;
		return NULL;
	}
	return pDailyTodo;
}

DailyTodo* DailyTodoMgr::_FindDailyTodo(DailyTodoSubType subType)
{
	auto iter = m_DailyTodos.find(subType);
	if (iter == m_DailyTodos.end())
	{
		return NULL; 
	}

	return iter->second;
}

void DailyTodoMgr::CompleteByGm(UInt8 subType)
{
	DailyTodo*  pDailyTodo = _GetAndCreateDailyTodo((DailyTodoSubType)subType);
	if (!pDailyTodo)
	{
		return;
	}

	if (pDailyTodo->UpdateDayProgress(pDailyTodo->GetDayProgMax())
		&& pDailyTodo->CheckDayCompeleted())
	{
		pDailyTodo->IncWeekProgress(1);
	}
}