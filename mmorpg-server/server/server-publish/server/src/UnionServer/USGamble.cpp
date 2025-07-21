#include "USGamble.h"

#include <CLRecordClient.h>
#include <CLChampionDefine.h>
#include <CLErrorCode.h>
#include <CLSystemValueDataEntry.h>

#include "USApplication.h"
#include "ChampionService.h"
#include "UnionServiceMgr.h"
#include "SysUtility.h"
#include "ChampionPlayer.h"


class SelectGambleRecordCallBack : public CLRecordCallback
{
public:
	SelectGambleRecordCallBack(USGambleMgr& mgr) : m_Mgr(mgr) {}
	virtual void OnFinished()
	{
		m_Mgr.OnLoadData(this);
	}
	virtual void OnFailed(UInt32 errorcode)
	{
		ErrorLogStream << "Load GambleRecord Faild,ErrorCode is" << errorcode << LogStream::eos;
		USApplication::Instance()->Exit();
	}

private:
	USGambleMgr& m_Mgr;
};

class SelectGambleProjectCallBack : public CLRecordCallback
{
public:
	SelectGambleProjectCallBack(USGambleMgr& mgr) : m_Mgr(mgr) {}
	virtual void OnFinished()
	{
		m_Mgr.OnLoadProjectData(this);
	}
	virtual void OnFailed(UInt32 errorcode)
	{
		ErrorLogStream << "Load GambleProject Faild,ErrorCode is" << errorcode << LogStream::eos;
		USApplication::Instance()->Exit();
	}

private:
	USGambleMgr& m_Mgr;
};


UInt32 USGambleMgr::GetNewProjectID()
{
	return ++m_GamebleProjectID;
}

const bool USGambleMgr::Init()
{
	m_SaveTimer.SetInterval(1000);
	m_SyncTimer.SetInterval(1000);

	SelectGambleProjectCallBack* callback = new SelectGambleProjectCallBack(*this);
	CLSelectCommand* selectCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_champion_gamble");
	CLRecordClient::Instance()->SendCommand(selectCmd, callback);

	return true;
}

void USGambleMgr::Clear()
{
	for (auto& projectIter : m_GambleProjectMap)
	{
		CL_SAFE_DELETE(projectIter.second);
	}
	for (auto& roleMapIter : m_GambleRecordMap)
	{
		for (auto& projectMapIter : roleMapIter.second)
		{
			for (auto& optionMapIter : projectMapIter.second)
			{
				CL_SAFE_DELETE(optionMapIter.second);
			}
		}
	}
	m_GambleProjectMap.clear();
	m_GambleRecordMap.clear();
}

void USGambleMgr::OnTick(const Avalon::Time & now)
{
	if (m_SaveTimer.IsTimeout(now))
	{
		for (auto& roleMapIter : m_GambleRecordMap)
		{
			for (auto& projectMapIter : roleMapIter.second)
			{
				for (auto& optionMapIter : projectMapIter.second)
				{
					optionMapIter.second->SaveToDB();
				}
			}
		}
		for (auto& projectIter : m_GambleProjectMap)
		{
			projectIter.second->SaveToDB();
		}
	}
	if (m_SyncTimer.IsTimeout(now))
	{
		SyncGamebleInfo();
	}
}

void USGambleMgr::OnLoadData(CLRecordCallback * cb)
{
	while (cb->NextRow())
	{
		UInt64 roleID = cb->GetData("role_id");
		UInt32 projectID = cb->GetData("id");
		UInt64 option = cb->GetData("option");
		
		auto& record = m_GambleRecordMap[roleID][projectID][option];
		if (record == NULL)
		{
			record = new GambleRecord();
			m_GambleRecordMap[roleID][projectID][option] = record;
		}

		m_GambleRecordMap[roleID][projectID][option]->LoadDBData(cb);
		if (record == NULL)
		{
			record = new GambleRecord();
			m_GambleRecordMap[roleID][projectID][option] = record;
		}

		record->LoadDBData(cb);
		
		if (m_GambleProjectMap.count(projectID) == 0)
		{
			ErrorLogStream << "project not exists" << projectID << LogStream::eos;
			continue;
		}

		m_GambleProjectMap[record->GetGmableID()]->Push(roleID ,record->GetOption(), record->GetNum());
	}
}

void USGambleMgr::OnLoadProjectData(CLRecordCallback * cb)
{
	while (cb->NextRow())
	{
		UInt32 projectID = cb->GetData("project_id");
		UInt32 type = cb->GetData("type");

		auto project = GenProject(projectID, static_cast<GambleType>(type));
		if (project == nullptr)
		{
			ErrorLogStream << "project(" << projectID << "," << type << ") gen faild" << LogStream::eos;
			USApplication::Instance()->Exit();
		}
		m_GambleProjectMap[projectID] = project;
		project->SetID(cb->GetKey());
		project->SetDBData(cb);
		project->Init();
	}

	SelectGambleRecordCallBack* callback = new SelectGambleRecordCallBack(*this);
	CLSelectCommand* selectCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_champion_gamble_record");
	CLRecordClient::Instance()->SendCommand(selectCmd, callback);
}

GambleRecord* USGambleMgr::AddRecord(UInt32 zoneID, UInt64 roleID, UInt32 id, UInt64 option, UInt64 num)
{
	auto project = GetProject(id);
	if (!project->HasOption(option))
	{
		InfoLogStream << "project dont have this option" << LogStream::eos;
		return nullptr;
	}

	auto record = FindRecord(roleID, id, option);
	if (record == nullptr)
	{
		if (num > SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_CHAMPION_RELIVE_BETLIMIT_NUM))
		{
			ErrorLogStream << "project: " << id << " option: " << option << " roleid is " << roleID << " reach limit: " << num << LogStream::eos;
			return nullptr;
		}
		InfoLogStream << "new record("<< zoneID <<","<< roleID <<","<< id <<","<< option <<","<< num <<")" << LogStream::eos;
		record = new GambleRecord;
		record->SetGmableID(id);
		record->SetType(project->GetType());
		record->SetTime(CURRENT_TIME.Sec());
		record->SetRoleID(roleID);
		record->SetNum(num);
		record->SetOption(option);
		record->SetReward(0);
		record->SetZoneID(zoneID);
		record->InsterToDB();
		m_GambleRecordMap[roleID][id][option] = record;
	}
	else
	{
		if (record->GetNum() + num > SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_CHAMPION_RELIVE_BETLIMIT_NUM))
		{
			ErrorLogStream << "project: " << id << " option: " << option << " roleid is " << roleID << " reach limit,current num is " << record->GetNum() << " bet num is "<< num << LogStream::eos;
			return nullptr;
		}
		record->SetNum(record->GetNum() + num);
	}
	
	project->Push(roleID, option, num);
	return record;
}

GambleRecord * USGambleMgr::FindRecord(UInt64 roleID, UInt32 id, UInt64 option)
{
	auto roleMapIter = m_GambleRecordMap.find(roleID);
	if (roleMapIter == m_GambleRecordMap.end())
	{
		return nullptr;
	}
	auto projectMapIter = roleMapIter->second.find(id);
	if (projectMapIter == roleMapIter->second.end())
	{
		return nullptr;
	}
	auto optionIter = projectMapIter->second.find(option);
	if (optionIter == projectMapIter->second.end())
	{
		return nullptr;
	}
	return optionIter->second;
}

GambleProjectBase* USGambleMgr::GenProject(UInt32 id, GambleType type)
{
	GambleProjectBase* project = nullptr;
	switch (type)
	{
	case GT_1V1:
		project = new GambleProject1V1(id);
		break;
	case GT_CHAMPION:
		project = new GambleProjectChampion(id);
		break;
	case GT_BATTLE_COUNT:
		project = new GambleProjectBattleCount(id);
		break;
	case GT_CHAMPION_BATTLE_SCORE:
		project = new GambleProjectChampionScore(id);
		break;
	default:
		break;
	}
	if (project != nullptr)
	{
		m_GambleProjectMap[id] = project;
		project->Init();
	}
	
	return project;
}

void USGambleMgr::OnEvent(UInt32 id, UInt64 Option)
{
	auto project = GetProject(id);
	if (project == nullptr)
	{
		return;
	}

}

void USGambleMgr::SyncGamebleInfo()
{
	auto service = UnionServiceMgr::Instance()->GetUnionService<ChampionService>();
	if (service == nullptr)
	{
		return;
	}

	auto msg = GetSyncMsg();
	service->BroadcastToZone(msg, ST_SCENE);
}

CLProtocol::SceneChampionAllGambleInfoRes USGambleMgr::GetSyncMsg()
{
	CLProtocol::SceneChampionAllGambleInfoRes sync;

	for (auto& projectIter : m_GambleProjectMap)
	{
		auto& project = projectIter.second;
		ChampionGambleInfo info;
		info.id = projectIter.first;
		info.type = project->GetType();
		info.startTime = project->GetDataEntry().startTime;
		info.endTime = project->GetDataEntry().endTime;
		for (auto& optionIter : project->GetOptions())
		{
			info.optionVec.push_back(optionIter.second.GetData());
		}
		if (project->GetType() == GT_1V1)
		{
			info.param = project->GetGroupID();
		}
		sync.infos.push_back(info);
	}

	return sync;
}

UInt32 USGambleMgr::OnPlayerBet(UInt32 zoneID, UInt64 roldID, UInt32 accid, UInt32 id, UInt64 option, UInt32 coin)
{
	if (coin > SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_CHAMPION_RELIVE_BETLIMIT_NUM))
	{
		InfoLogStream << "project: " << id << " option: " << option << " roleid is " << roldID  << " reach limit: " << coin << LogStream::eos;
		return ErrorCode::CHAMPION_BET_LIMIT;
	}

	auto project = GetProject(id);
	if (project == nullptr)
	{
		InfoLogStream << "project is null" << LogStream::eos;
		return ErrorCode::CHAMPION_NO_BET;
	}

	if (!project->HasOption(option) || !project->IsOpen())
	{
		InfoLogStream << "project: " << id << " not open or dont have this option: "  << option << " roleid is " << roldID << LogStream::eos;
		return ErrorCode::CHAMPION_NO_BET;
	}

	if (!project->CanAccountBet(accid))
	{
		InfoLogStream << "project: " << id << "cant bet with this account,accid is " << accid << " roleid is " << roldID << LogStream::eos;
		return ErrorCode::CHAMPION_BET_ACCOUNT;
	}


	auto record = AddRecord(zoneID, roldID, id, option, coin);

	if (record == nullptr)
	{
		ErrorLogStream << "add record failed" << LogStream::eos;
		return ErrorCode::CHAMPION_NO_INFO;
	}

	return ErrorCode::SUCCESS;
}

void USGambleMgr::OnGroupBattleEnd(UInt32 groupID, UInt64 option)
{
	InfoLogStream << "group: " << groupID << "battle end, win option is " << option << LogStream::eos;
	for (auto& projectIter : m_GambleProjectMap)
	{
		if (groupID == projectIter.second->GetGroupID())
		{
			projectIter.second->OnResult(option);
		}
	}
}

std::vector<GambleRecord*> USGambleMgr::GetRecord(UInt64 roleID)
{
	std::vector<GambleRecord*> vec;
	if (m_GambleRecordMap.count(roleID) == 0)
	{
		return vec;
	}
	for (auto& projectMapIter : m_GambleRecordMap[roleID])
	{
		for (auto& optionMapIter : projectMapIter.second)
		{
			vec.push_back(optionMapIter.second);
		}
	}
	return vec;
}

GambleProjectBase* USGambleMgr::GetProject(UInt32 id) const
{
	auto projectIter = m_GambleProjectMap.find(id);
	if (projectIter == m_GambleProjectMap.end())
	{
		return nullptr;
	}
	return projectIter->second;
}

std::vector<GambleRecord*> USGambleMgr::GetAllRecord()
{
	std::vector<GambleRecord*> vec;
	for (auto& roleMapIter : m_GambleRecordMap)
	{
		for (auto& projectMapIter : roleMapIter.second)
		{
			for (auto& optionMapIter : projectMapIter.second)
			{
				vec.push_back(optionMapIter.second);
			}
		}
	}
	return vec;
}

GambleRecord::GambleRecord()
{
	m_Time = 0;
	m_RoleID = 0;
	m_Type = 0;
	m_Option = 0;
	m_Num = 0;
}

void GambleRecord::SaveToDB()
{
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_champion_gamble_record", GetID(), false);
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

void GambleRecord::InsterToDB()
{
	SetID(CLRecordClient::Instance()->GenGuid());
	CLInsertCommand* insertCmd = CLRecordClient::Instance()->PrepareInsertCommand("t_champion_gamble_record", GetID());
	GetDBData(insertCmd, false);
	CLRecordClient::Instance()->SendCommand(insertCmd);
	ClearDirty();
}

void GambleRecord::LoadDBData(CLRecordCallback* callback)
{
	SetID(callback->GetKey());
	SetDBData(callback);
}

GambleProjectBase::GambleProjectBase(UInt32 id)
{
	m_ProjectID = id; 
	m_WinOption = 0;
	m_Type = 0;
	m_TotalNum = 0;
	m_GroupID = 0;
}

void GambleProjectBase::Init()
{
	auto data = ChampionGambleDataEntryMgr::Instance()->FindEntry(GetProjectID());
	if (data == nullptr)
	{
		ErrorLogStream << "Gamble Project config not exists id: " << GetProjectID() << LogStream::eos;
		return;
	}
	SetDataEntry(data);
	LoadOptionFromString();
	m_Type = GetType();
}

GambleOptionInfo& GambleProjectBase::AddInfo(const UInt64 option)
{
	return m_GambleInfoMap[option];
}

void GambleProjectBase::SetOption(const std::set<UInt64>& optionSet)
{
	for (auto& option : optionSet)
	{
		m_GambleInfoMap[option].Init(option);
	}
}

const bool GambleProjectBase::HasOption(const UInt64 option) const
{
	if (m_GambleInfoMap.count(option) > 0)
	{
		return true;
	}
	return false;
}

const std::map<UInt64, GambleOptionInfo>& GambleProjectBase::GetOptions() const
{
	return m_GambleInfoMap;
}

void GambleProjectBase::SetDataEntry(ChampionGambleDataEntry * dataEntry)
{
	m_DataEntry = dataEntry;
}

const ChampionGambleDataEntry & GambleProjectBase::GetDataEntry()
{
	return *m_DataEntry;
}

void GambleProjectBase::OnResult(const UInt64 winOption)
{
	InfoLogStream << GetID() << " project: " << GetProjectID() << " win option is  " << winOption << LogStream::eos;
	if (!HasOption(winOption))
	{
		InfoLogStream << GetID() << " project: " << GetProjectID() << " dont have this option " << winOption << LogStream::eos;
		return;
	}

	auto service = UnionServiceMgr::Instance()->GetUnionService<ChampionService>();
	if (service == nullptr)
	{
		InfoLogStream << GetID() << " project: " << GetProjectID() << " service not exists "  << LogStream::eos;
		return;
	}

	m_WinOption = winOption;
	for (auto& optionInfoIter : m_GambleInfoMap)
	{
		auto& optionInfo = optionInfoIter.second;
		if (optionInfo.GetTotalNum() == 0)
		{
			InfoLogStream << GetID() << " project: " << GetProjectID() << "option: " << optionInfoIter.first << "total num = 0" << LogStream::eos;
			continue;
		}

		if (optionInfoIter.first == winOption)
		{
			auto& playerBet = optionInfo.GetPlayerBet();
			for (auto& betIter : playerBet)
			{
				auto record = USGambleMgr::Instance()->FindRecord(betIter.first, GetProjectID(), winOption);
				if (record == nullptr)
				{
					ErrorLogStream << GetID() << " project: " << GetProjectID() << "option: " << optionInfoIter.first 
						<< "can't find player bet record,player bet is(" << betIter.first << "," << betIter.second << ")" << LogStream::eos;
					continue;
				}

				UInt64 rewardNum = m_TotalNum * record->GetNum() / optionInfo.GetTotalNum();
				record->SetReward(rewardNum);
				//·¢ËÍ½±ÀøÓÊ¼þ
				ItemReward reward(CHAMPION_GAMBLE_COIN_ITEM_ID, rewardNum, 0, 0);
				ItemRewardVec vec;
				vec.push_back(reward);
				std::string title;
				std::string sender = SysNotifyMgr::Instance()->MakeString(4900039);
				std::string content;
				if (GetType() == GT_1V1)
				{
					std::string name;
					auto geter = [&](ChampionPlayer* player)->void {
						name = player->GetName();
					};
					service->GetPlayerAttributes(optionInfo.GetData().option, geter);
					title = SysNotifyMgr::Instance()->MakeString(4900041);
					content = SysNotifyMgr::Instance()->MakeString(4900045, name);
				}
				else if (GetType() == GT_CHAMPION)
				{
					std::string name;
					auto geter = [&](ChampionPlayer* player)->void {
						name = player->GetName();
					};
					service->GetPlayerAttributes(optionInfo.GetData().option, geter);
					title = SysNotifyMgr::Instance()->MakeString(4900040);
					content = SysNotifyMgr::Instance()->MakeString(4900043, name);
				}
				else if (GetType() == GT_BATTLE_COUNT || GetType() == GT_CHAMPION_BATTLE_SCORE)
				{
					title = SysNotifyMgr::Instance()->MakeString(4900042);
					if (GetType() == GT_BATTLE_COUNT)
					{
						std::string str = std::to_string(optionInfo.GetData().option / 100) + "-" + std::to_string(optionInfo.GetData().option % 100);
						content = SysNotifyMgr::Instance()->MakeString(4900047, str);
					}
					else
					{
						std::string str = std::to_string(optionInfo.GetData().option / 10) + ":" + std::to_string(optionInfo.GetData().option % 10);
						content = SysNotifyMgr::Instance()->MakeString(4900047, str);
					}
				}
				std::string reason = GetReason(SOURCE_CHAMPION_ENROLL);
				SysUtility::SendMail(record->GetZoneID(), record->GetRoleID(), sender, title, content, reason, vec);
			}
		}
		else
		{
			auto& playerBet = optionInfoIter.second.GetPlayerBet();
			for (auto& betIter : playerBet)
			{
				auto record = USGambleMgr::Instance()->FindRecord(betIter.first, GetProjectID(), optionInfoIter.first);
				if (record == nullptr)
				{
					ErrorLogStream << GetID() << " project: " << GetProjectID() << "option: " << optionInfoIter.first
						<< "can't find player bet record,player bet is(" << betIter.first << "," << betIter.second << ")" << LogStream::eos;
					continue;
				}

				ItemRewardVec vec;
				std::string title;
				std::string sender = SysNotifyMgr::Instance()->MakeString(4900039);
				std::string content;
				if (GetType() == GT_1V1)
				{
					std::string name;
					auto geter = [&](ChampionPlayer* player)->void {
						name = player->GetName();
					};
					service->GetPlayerAttributes(optionInfo.GetData().option, geter);
					title = SysNotifyMgr::Instance()->MakeString(4900041);
					content = SysNotifyMgr::Instance()->MakeString(4900046, name);
				}
				else if (GetType() == GT_CHAMPION)
				{
					std::string name;
					auto geter = [&](ChampionPlayer* player)->void {
						name = player->GetName();
					};
					service->GetPlayerAttributes(optionInfo.GetData().option, geter);
					title = SysNotifyMgr::Instance()->MakeString(4900040);
					content = SysNotifyMgr::Instance()->MakeString(4900044, name);
				}
				else if (GetType() == GT_BATTLE_COUNT || GetType() == GT_CHAMPION_BATTLE_SCORE)
				{
					title = SysNotifyMgr::Instance()->MakeString(4900042);
					if (GetType() == GT_BATTLE_COUNT)
					{
						std::string str = std::to_string(optionInfo.GetData().option / 100) + "-" + std::to_string(optionInfo.GetData().option % 100);
						content = SysNotifyMgr::Instance()->MakeString(4900048, str);
					}
					else
					{
						std::string str = std::to_string(optionInfo.GetData().option / 10) + ":" + std::to_string(optionInfo.GetData().option % 10);
						content = SysNotifyMgr::Instance()->MakeString(4900048, str);
					}
				}
				std::string reason = GetReason(SOURCE_CHAMPION_ENROLL);
				SysUtility::SendMail(record->GetZoneID(), record->GetRoleID(), sender, title, content, reason, vec);
			}
		}
	}
}

void GambleProjectBase::Push(UInt64 roleID, UInt64 option, UInt64 num)
{
	m_TotalNum += num;
	auto& info = AddInfo(option);
	info.Push(roleID, num);
	if (info.GetTotalNum() == 0)
	{
		ErrorLogStream << "totalnum added error (" << roleID << "," << option << "," << num << "," << m_TotalNum << ")" << LogStream::eos;
		return;
	}
	//info.SetOdds(GetTotalNum() * 100 / info.GetTotalNum());
	for (auto& info : m_GambleInfoMap)
	{
		if (info.second.GetTotalNum() == 0)
		{
			continue;
		}
		info.second.SetOdds(GetTotalNum() * 100 / info.second.GetTotalNum());
	}
	InfoLogStream << "totalnum added(" << roleID <<","<< option <<","<< num <<","<< m_TotalNum <<")" << LogStream::eos;
}

const UInt64 GambleProjectBase::GetTotalNum() const
{
	return m_TotalNum;
}

bool GambleProjectBase::IsOpen()
{
	if (CURRENT_TIME.Sec() <= GetDataEntry().endTime && CURRENT_TIME.Sec() > GetDataEntry().startTime && m_WinOption == 0)
	{
		return true;
	}
	return false;
}

const UInt32 GambleProjectBase::GetProjectID() const
{
	return m_ProjectID;
}

const UInt32 GambleProjectBase::GetGroupID() const
{
	return m_GroupID;
}

void GambleProjectBase::SetGroupID(UInt32 id)
{
	m_GroupID = id;
}

UInt64 GambleProjectBase::GetWinOption() const
{
	return m_WinOption;
}

bool GambleProjectBase::CanAccountBet(UInt32 accid)
{
	if (GetType() != GT_1V1)
	{
		return true;
	}
	auto service = UnionServiceMgr::Instance()->GetUnionService<ChampionService>();
	if (service == nullptr)
	{
		InfoLogStream << GetID() << " project: " << GetProjectID() << " service not exists, accid is " << accid << LogStream::eos;
		return false;
	}
	for (auto& optionInfoIter : m_GambleInfoMap)
	{
		UInt32 optionAccid = 0;
		auto geter = [&](ChampionPlayer* player)->void {
			optionAccid = player->GetAccID();
		};
		service->GetPlayerAttributes(optionInfoIter.first, geter);
		if (accid == optionAccid)
		{
			return false;
		}
	}
	return true;
}

void GambleProjectBase::SaveToDB()
{
	SaveOptionToString();
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_champion_gamble", GetID(), false);
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

void GambleProjectBase::InsertToDB()
{
	SaveOptionToString();
	SetID(CLRecordClient::Instance()->GenGuid());
	CLInsertCommand* insertCmd = CLRecordClient::Instance()->PrepareInsertCommand("t_champion_gamble", GetID());
	GetDBData(insertCmd, false);
	CLRecordClient::Instance()->SendCommand(insertCmd);
	ClearDirty();
}

void GambleProjectBase::SaveOptionToString()
{
	std::ostringstream os;
	bool first = true;
	for (auto &i : m_GambleInfoMap)
	{
		if (first)
		{
			first = false;
		}
		else
		{
			os << '|';
		}
		os << i.first;
	}
	if (static_cast<std::string>(m_OptionsStr) != os.str())
	{
		m_OptionsStr = os.str();
	}
}

void GambleProjectBase::LoadOptionFromString()
{
	if (static_cast<std::string>(m_OptionsStr).size() == 0)
	{
		return;
	}
	std::string splitOption = "|";
	std::vector<std::string> optionVec;
	Avalon::StringUtil::Split(m_OptionsStr, optionVec, splitOption);
	std::set<UInt64> tempSet;
	for (UInt32 i = 0; i < optionVec.size(); ++i)
	{
		tempSet.insert(Avalon::StringUtil::ConvertToValue<UInt64>(optionVec[i]));
	}
	SetOption(tempSet);
}

void GambleOptionInfo::Init(UInt64 option)
{
	m_Data.option = option;
	m_Data.num = 0;
	m_Data.odds = 100;
}

void GambleOptionInfo::Push(const UInt64 roleID, const UInt64 num)
{
	m_PlayerBet[roleID] += num;
	m_Data.num += num;

	InfoLogStream << "GambleOptionInfo pushed(" << roleID << "," << m_Data.option << "," << num << "," << m_Data.num << ")" << LogStream::eos;
}

const UInt64  GambleOptionInfo::GetTotalNum() const
{
	return m_Data.num;
}

std::map<UInt64, UInt64>& GambleOptionInfo::GetPlayerBet()
{
	return m_PlayerBet;
}

const ChampionGambleOption & GambleOptionInfo::GetData() const
{
	return m_Data;
}
