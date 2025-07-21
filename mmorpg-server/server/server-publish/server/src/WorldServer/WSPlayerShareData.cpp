#include "WSPlayerShareData.h"

#include <CLPlayerDefine.h>
#include <CLTaskProtocol.h>
#include <CLRecordCallback.h>
#include <CLRecordClient.h>
#include <CLTaskDataEntry.h>

#include "WSPlayerShareDataMgr.h"
#include "WSPlayerMgr.h"
#include "WSShareDataLoadCallback.h"

void SelectRoleShareDataCallback::OnFinished()
{
	WSPlayerShareData* shareData = WSPlayerShareDataMgr::Instance()->FindPlayer(m_PlayerId);
	//bool isInLoading = shareData->IsInLoading();
	//UInt32 loadReqId1 = shareData->GetLoadReqId();
	//UInt32 loadReqId2 = this->GetLoadDataReqId();
	//bool check = shareData->CheckLoadDataSerial(m_LoadSerial);
	if (shareData == NULL 
		|| !shareData->IsInLoading() 
		|| shareData->GetLoadReqId() != this->GetLoadDataReqId() 
		|| !shareData->CheckLoadDataSerial(m_LoadSerial)) return;

	OnFinished(shareData);
	shareData->SetLoadDataFinished(m_LoadSerial);
}

void SelectRoleShareDataCallback::OnLoadFailed()
{
	WSPlayerShareData* shareData = WSPlayerShareDataMgr::Instance()->FindPlayer(m_PlayerId);
	if (shareData == NULL 
		|| !shareData->IsInLoading()
		|| shareData->GetLoadReqId() != this->GetLoadDataReqId()
		|| !shareData->CheckLoadDataSerial(m_LoadSerial)) return;

	OnFailed(shareData);

	shareData->LoadingFailed();
}


/**
*@brief 查询离线玩家全局信息回调
*/
class SelectShareDataGlobalInfoCallback : public SelectRoleShareDataCallback
{
public:
	SelectShareDataGlobalInfoCallback(WSPlayerShareData* shareData, UInt32 loadReqId) : SelectRoleShareDataCallback(shareData, loadReqId){}
	~SelectShareDataGlobalInfoCallback(){}

	void OnFinished(WSPlayerShareData* shareData)
	{
		if (!this->NextRow())
		{
			OnFailed(shareData);
			return;
		}
		shareData->OnLoadGlobalInfo(this);
	}

	void OnFailed(WSPlayerShareData* shareData)
	{
		GameErrorLogStream("shareData") << "player(" << shareData->GetAccID() << "," << shareData->GetID()
			<< ") select global info failed." << LogStream::eos;
	}
};

/**
*@brief 查询离线玩家任务信息回调
*/
class SelectShareDataTaskInfoCallback : public SelectRoleShareDataCallback
{
public:
	SelectShareDataTaskInfoCallback(WSPlayerShareData* shareData, UInt32 loadReqId) : SelectRoleShareDataCallback(shareData, loadReqId){}
	~SelectShareDataTaskInfoCallback(){}

	void OnFinished(WSPlayerShareData* shareData)
	{
		shareData->OnLoadTaskInfo(this);
	}

	void OnFailed(WSPlayerShareData* shareData)
	{
		GameErrorLogStream("shareData") << "player(" << shareData->GetAccID() << "," << shareData->GetID()
			<< ") select task info failed." << LogStream::eos;
	}
};

WSPlayerShareData::WSPlayerShareData()
{
	m_Guid = 0;
	m_OfflineTime = 0;
	m_level = 0;
	m_FinSchDiscipleSum = 0;
	m_LoadDataCount = 0;
	m_LoadDataSerial = 0;
	m_LoadPlayerType = 0;
	m_RequestTarget = 0;
	m_Status = PSDS_INIT;
	m_DataLoadedStatus = 0;
	m_LoadDataReqId = 0;
}

WSPlayerShareData::~WSPlayerShareData()
{
}

void WSPlayerShareData::SyncDataFromScene(const PlayerSenceShareData& shareData)
{
	this->m_SceneshareData = shareData;
	this->SetDataLoadedStatus(PSDLS_SCENEDATA_LOADED);
}

void WSPlayerShareData::SyncDataFromWorld(WSPlayer* player)
{
	this->m_OfflineTime = player->GetOfflineTime();
	this->SetLevel(player->GetLevel());
	this->m_AccId = player->GetAccId();
	this->SetFinSchDiscipleSum(player->GetMasterSystem().GetFinSchDiscipleSum());
	this->SetDataLoadedStatus(PSDLS_WORLDDATA_LOADED);
}

void WSPlayerShareData::SyncMasterTaskDataFromScene(const MasterTaskShareData& masterTaskShareData)
{
	this->m_SceneshareData.m_MasterTaskShareData = masterTaskShareData;
}

void WSPlayerShareData::GetMasterTaskShareData(MasterTaskShareData& masterTaskShareData)
{
	masterTaskShareData = this->m_SceneshareData.m_MasterTaskShareData;
}

void WSPlayerShareData::StartLoadData(WSShareDataLoadCallback* cb)
{
	this->SetStatus(PSDS_LOADING);
	UInt32 loadReqId = this->IncLoadReqId();
	LoadGlobalInfo(loadReqId);
	LoadTaskInfo(loadReqId);
	AddLoadingCallback(cb);
}

UInt32 WSPlayerShareData::IncLoadReqId()
{
	m_LoadDataReqId++;
	return m_LoadDataReqId;
}

void WSPlayerShareData::AddLoadingCallback(WSShareDataLoadCallback* cb)
{
	if (!cb)
	{
		return;
	}

	m_LoadingCallback.push_back(cb);
}

void WSPlayerShareData::LoadGlobalInfo(UInt32 loadReqId)
{
	CLSelectCommand* selectcmd = CLRecordClient::Instance()->PrepareSelectCommand(0, "t_player_info");
	selectcmd->PutCond("guid", GetID());
	selectcmd->PutColumn("savetime");
	selectcmd->PutColumn("level");
	selectcmd->PutColumn("academic_total_growth");
	selectcmd->PutColumn("master_dailytask_growth");
	selectcmd->PutColumn("master_academictask_growth");
	selectcmd->PutColumn("master_uplevel_growth");
	selectcmd->PutColumn("master_giveequip_growth");
	selectcmd->PutColumn("master_givegift_growth");
	selectcmd->PutColumn("master_teamcleardungeon_growth");
	selectcmd->PutColumn("good_teacher_value");
	selectcmd->PutColumn("fin_sch_disciple_sum");
	CLRecordClient::Instance()->SendCommand(selectcmd, new SelectShareDataGlobalInfoCallback(this, loadReqId));
}

void WSPlayerShareData::LoadTaskInfo(UInt32 loadReqId)
{
	CLSelectCommand* cmd = CLRecordClient::Instance()->PrepareSelectCommand(0, "t_task");
	cmd->PutCond("owner", GetID());
	CLRecordClient::Instance()->SendCommand(cmd, new SelectShareDataTaskInfoCallback(this, loadReqId), GetID());
}

void WSPlayerShareData::OnLoadGlobalInfo(CLRecordCallback *callback)
{
	if (!callback) return;

	SetOfflineTime(callback->GetData("savetime"));
	SetLevel(callback->GetData("level"));
	SetDiscipleGrowthValue(callback->GetData("academic_total_growth"));
	SetMasterDailyTaskGrowthValue(callback->GetData("master_dailytask_growth"));
	SetMasterAcademicTaskGrowthValue(callback->GetData("master_academictask_growth"));
	SetMasterGiveEquipGrowthValue(callback->GetData("master_giveequip_growth"));
	SetMasterUplevelGrowthValue(callback->GetData("master_uplevel_growth"));
	SetMasterGiveGiftGrowthValue(callback->GetData("master_givegift_growth"));
	SetMasterSysGoodTeacherValue(callback->GetData("good_teacher_value"));
	SetMasterTeamClearDungeonGrowth(callback->GetData("master_teamcleardungeon_growth"));
	SetFinSchDiscipleSum(callback->GetData("fin_sch_disciple_sum"));
}

void WSPlayerShareData::OnLoadTaskInfo(CLRecordCallback *callback)
{
	if (!callback) return;
	if (this->IsSceneDataLoaded())
	{
		return;
	}
	while (callback->NextRow())
	{
		const TaskDataEntry* dataEntry = NULL;
		dataEntry = TaskDataEntryMgr::Instance()->GetMasterDailyTask(callback->GetData("dataid"));
		if (dataEntry != NULL)
		{
			UInt64 guid = callback->GetKey();
			UInt32 dataId = callback->GetData("dataid");
			TaskStatus status = (TaskStatus)(UInt32)callback->GetData("status");
			std::string parameter = callback->GetData("parameter").GetString();
			UInt32 finishedTime = callback->GetData("finished_time");
			UInt8 submitCount = callback->GetData("submit_count");
			TaskBriefInfo taskinfo;
			AnalysisTaskBriefInfo(taskinfo, dataId, guid, status, parameter, finishedTime, submitCount);
			AddMasterDailyTask(taskinfo);
			continue;
		}
		dataEntry = TaskDataEntryMgr::Instance()->GetMasterAcademicGrowthTask(callback->GetData("dataid"));
		if (dataEntry != NULL)
		{
			UInt64 guid = callback->GetKey();
			UInt32 dataId = callback->GetData("dataid");
			TaskStatus status = (TaskStatus)(UInt32)callback->GetData("status");
			std::string parameter = callback->GetData("parameter").GetString();
			UInt32 finishedTime = callback->GetData("finished_time");
			UInt8 submitCount = callback->GetData("submit_count");
			TaskBriefInfo taskinfo;
		    AnalysisTaskBriefInfo(taskinfo, dataId, guid, status, parameter, finishedTime, submitCount);
			AddMasterAcademicTask(taskinfo);
			continue;
		}
	}
	return;
}

void WSPlayerShareData::AnalysisTaskBriefInfo(TaskBriefInfo& taskinfo, UInt32 id, UInt64 guid, TaskStatus status, std::string questVarStr, UInt32 finishedTime, UInt8 submitCount)
{
	//taskinfo;
	taskinfo.id = id;
	taskinfo.status = status;
	taskinfo.finTime = finishedTime;
	taskinfo.submitCount = submitCount;

	std::vector<std::string> valuePairs;
	Avalon::StringUtil::Split(questVarStr, valuePairs, ",");

	for (std::vector<std::string>::iterator iter = valuePairs.begin(); iter != valuePairs.end(); ++iter)
	{
		std::vector<std::string> pair;
		Avalon::StringUtil::Split(*iter, pair, "=");
		if (pair.size() < 2) continue;

		taskinfo.vars.insert(std::make_pair(pair[0], pair[1]));
	}
}

bool WSPlayerShareData::AddMasterDailyTask(const TaskBriefInfo& taskinfo)
{
	m_SceneshareData.m_MasterTaskShareData.m_MasterDailyTasks.push_back(taskinfo);
	return true;
}

bool WSPlayerShareData::AddMasterAcademicTask(const TaskBriefInfo& taskinfo)
{
	m_SceneshareData.m_MasterTaskShareData.m_MasterAcademicTasks.push_back(taskinfo);
	return true;
}

void WSPlayerShareData::ClearMasterTaskData()
{
	m_SceneshareData.m_MasterTaskShareData.clear();
}

UInt8 WSPlayerShareData::IncLoadDataCount()
{
	++m_LoadDataCount;
	m_LoadDataSerial |= (UInt64)1 << m_LoadDataCount;
	return m_LoadDataCount;
}

void WSPlayerShareData::ResetLoadDataStatus()
{
	m_LoadDataCount = 0;
	m_LoadDataSerial = 0;

}

bool WSPlayerShareData::CheckLoadDataSerial(UInt8 serial)
{
	if ((m_LoadDataSerial & (1 << serial)) != 0)
	{
		return true;
	}

	return false;
}

void WSPlayerShareData::SetLoadDataFinished(UInt8 serial)
{
	if (CheckLoadDataSerial(serial))
	{
		m_LoadDataSerial = m_LoadDataSerial & (~(1 << serial));
		if (m_LoadDataSerial == 0)
		{
			LoadDataFinished();
			m_LoadDataCount = 0;
		}
	}
}

void WSPlayerShareData::LoadDataFinished()
{
	DebugLogStream << "player(" << GetID() << ") load sharedata finished " << LogStream::eos;
	
	this->SetStatus(PSDS_LOADED);

	this->SetDataLoadedStatus(PSDLS_WORLDDATA_LOADED);
	this->SetDataLoadedStatus(PSDLS_SCENEDATA_LOADED);

	this->LoadFinishedCallback();
}

void WSPlayerShareData::LoadingFailed()
{
	for (UInt32 i = 0; i < m_LoadingCallback.size(); i++)
	{
		auto cb = m_LoadingCallback[i];
		if (cb)
		{
			cb->OnFailure(this);
			delete cb;
			m_LoadingCallback[i] = NULL;
		}
	}

	m_LoadingCallback.erase(std::remove(m_LoadingCallback.begin(), m_LoadingCallback.end(), (WSShareDataLoadCallback*)NULL), m_LoadingCallback.end());

	// 加载失败，设置为初始状态
	this->SetStatus(PSDS_INIT);
	this->ResetLoadDataStatus();
}

void WSPlayerShareData::LoadFinishedCallback()
{
	for (UInt32 i = 0; i < m_LoadingCallback.size(); i++)
	{
		auto cb = m_LoadingCallback[i];
		if (cb)
		{
			cb->OnFinish(this);
			delete cb;
			m_LoadingCallback[i] = NULL;
		}
	}
	
	m_LoadingCallback.erase(std::remove(m_LoadingCallback.begin(), m_LoadingCallback.end(), (WSShareDataLoadCallback*)NULL), m_LoadingCallback.end());

}

void WSPlayerShareData::SetDataLoadedStatus(UInt8 status)
{
	m_DataLoadedStatus |= status;
}

bool WSPlayerShareData::CheckDataLoadedStatus(UInt8 status)
{
	return m_DataLoadedStatus & status;
}

bool WSPlayerShareData::IsWorldDataLoaded()
{
	return this->CheckDataLoadedStatus(PSDLS_WORLDDATA_LOADED);
}

bool WSPlayerShareData::IsSceneDataLoaded()
{
	return this->CheckDataLoadedStatus(PSDLS_SCENEDATA_LOADED);
}

void WSPlayerShareData::SetMasterSysGoodTeacherValue(UInt32 value, bool syncDb)
{
	m_SceneshareData.m_MasterTaskShareData.m_GoodTeachValue = value;
	if (syncDb)
	{
		std::ostringstream condstream;
		condstream << "guid =" << this->m_Guid;
		CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_player_info", condstream.str());
		updateCmd->PutData("good_teacher_value", value);
		CLRecordClient::Instance()->SendCommand(updateCmd);
	}
}

void WSPlayerShareData::AddFinSchDiscipleSum(UInt32 addValue)
{
	m_FinSchDiscipleSum += addValue;

	std::ostringstream condstream;
	condstream << "guid =" << this->m_Guid;
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_player_info", condstream.str());
	updateCmd->PutData("fin_sch_disciple_sum", m_FinSchDiscipleSum);
	CLRecordClient::Instance()->SendCommand(updateCmd);
}