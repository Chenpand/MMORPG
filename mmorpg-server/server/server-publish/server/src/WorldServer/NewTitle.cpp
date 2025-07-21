#include "NewTitle.h"
#include "CLNewTitleDataEntry.h"
#include "WSPlayerMgr.h"
#include "GuildMgr.h"
#include "AccountLoadDataCallback.h"

#include <CLNewTitleProtocol.h>
#include <CLRecordClient.h>
#include <CLErrorCode.h>
#include <format.h>
#include <CLSceneDataEntry.h>

/**
*@breif 加载角色头衔回调
*/
class LoadTitleRoleCallback : public SelectRoleInfoCallback
{
public:
	LoadTitleRoleCallback(WSPlayer* player) :SelectRoleInfoCallback(player){}
	~LoadTitleRoleCallback(){}

	void OnFinished(WSPlayer* player)
	{
		player->GetTitleMgr().OnLoadDataRet(this);
	}

	void OnFailed(WSPlayer* player)
	{

	}
};

/**
*@brief 加载账号头衔回调
*/
class LoadTitleAccoutCallback : public LoadAccountDataCallback
{
	AVALON_DEFINE_OBJECTPOOL(LoadTitleAccoutCallback, Avalon::NullMutex)
public:
	LoadTitleAccoutCallback(AccountInfo* info) : LoadAccountDataCallback(info) {}

	virtual void OnFinished(AccountInfo* account)
	{
		account->GetTitleMgr().OnLoadDataRet(this);
	}

	virtual void OnFailed(AccountInfo* account)
	{
		account->OnLoadFailure();
	}
};



PlayerTitle::PlayerTitle()
{
	m_owner = 0;
	m_create_time = 0;
	m_title_id = 0;
	m_nameType = 0;
	m_duetime = 0;
	m_name = "";
	m_style = 0;
}

PlayerTitle::~PlayerTitle()
{}

void PlayerTitle::Update()
{
	if (GetID() == 0)
	{
		Insert();
	}
	else
	{
		CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand(GetTableName(), GetID(), false);
		this->GetDBData(cmd, true);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
}

void PlayerTitle::Insert()
{
	SetID(CLRecordClient::Instance()->GenGuid());
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand(GetTableName(), GetID());
	this->GetDBData(cmd, false);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void PlayerTitle::Delete()
{
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand(GetTableName(), GetID());
	CLRecordClient::Instance()->SendCommand(cmd);
}

bool PlayerTitle::LoadFromDb(CLRecordCallback *pCallback, NewTitleItemDataEntry* dataEntry)
{
	if (!pCallback || !dataEntry)
	{
		return false;
	}
	this->SetDBData(pCallback);
	this->SetID(pCallback->GetKey());
	this->m_nameType = dataEntry->nameType;
	this->m_style = dataEntry->style;
	this->GenerateTitleName();
	this->ClearDirty();
	return true;
}


void PlayerTitle::InitDataOnCreate(UInt64 owenerId, NewTitleItemDataEntry& entry)
{
	m_owner = owenerId;
	m_title_id = entry.iD;
	m_create_time = (UInt32)CURRENT_TIME.Sec();
	m_nameType = entry.nameType;
	m_style = entry.style;
	GenerateTitleName();
}

bool PlayerTitle::IsValid()
{
	return m_duetime == 0 || m_duetime > (UInt32)CURRENT_TIME.Sec();
}

std::string PlayerTitle::GenerateTitleName()
{
	NewTitleItemDataEntry* dataEntry = NewTitleItemDataEntryMgr::Instance()->FindEntry(m_title_id);
	if (!dataEntry)
	{
		m_name = "";
	}
	if (dataEntry->nameType == TLETP_FIXED)
	{
		m_name = dataEntry->name;
	}
	else if (dataEntry->nameType == TLETP_JOINT)
	{
		m_name = JoinTitleName(*dataEntry);
	}
	else
	{
		m_name = "";
	}
	return m_name;
}

std::string PlayerTitleRole::JoinTitleName(NewTitleItemDataEntry& dataEntry)
{
	std::string name = "";
	WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(m_owner);
	if (!player)
	{
		return name;
	}
	if (dataEntry.category == NWCGRY_GUILD)
	{
		Guild* guild = GuildMgr::Instance()->FindGuildByID(player->GetGuildID());
		if (guild)
		{
			name = MakeName(dataEntry.name.c_str(), guild->GetName());
		}
	}
	else if (dataEntry.category == NWCGRY_MASTER)
	{
		name = MakeName(dataEntry.name.c_str(), player->GetRelationMgr().GetMasterName());
	}
	else if (dataEntry.category == NWCGRY_HONOR)
	{
		JobDataEntry* jobData = JobDataEntryMgr::Instance()->FindEntry(player->GetOccu());
		if (jobData != NULL)
			name = MakeName(dataEntry.name.c_str(), jobData->name);
		else
			name = MakeName(dataEntry.name.c_str(), "");
	}
	else
	{
	}
	return name;
}

std::string PlayerTitleAccout::JoinTitleName(NewTitleItemDataEntry& dataEntry)
{
	std::string name = "";
	AccountInfo* accoutInfo = AccountMgr::Instance()->FindAccount((UInt32)m_owner);
	if (!accoutInfo)
	{
		return name;
	}
	if (dataEntry.category == NWCGRY_YBT)
	{
		WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(accoutInfo->GetRoleID());
		if (player)
		{
			name = MakeName(dataEntry.name.c_str(), accoutInfo->GetAdventureTeamName().c_str());
		}
	}
	else
	{
	}
	return name;
}

PlayerTitleMgr::PlayerTitleMgr()
{
	m_checkDueTitle.SetInterval(1 * 1000);
}

PlayerTitleMgr::~PlayerTitleMgr()
{
	for (auto itr : m_titles)
	{
		if (itr.second)
		{
			CL_SAFE_DELETE(itr.second);
		}
	}
	m_titles.clear();
}

PlayerTitle* PlayerTitleMgr::CreateTitle(UInt64 owenerId, NewTitleItemDataEntry* entry, UInt32 dueTime)
{
	if (!entry)
	{
		return NULL;
	}

	UInt8 ownerType = entry->ownerType;
	if (ownerType < NTOTP_ROLE || ownerType > NTOTP_ACCT)
	{
		ErrorLogStream << "create title fail ownerType err! ownerType = " << ownerType 
			<< ", title id = " << entry->iD << LogStream::eos;
		return NULL;
	}

	PlayerTitle* pTitle = NULL;

	if (ownerType == NTOTP_ROLE)
	{
		pTitle = new PlayerTitleRole();
	}
	else
	{
		pTitle = new PlayerTitleAccout();
	}

	pTitle->InitDataOnCreate(owenerId, *entry);
	pTitle->SetDuTime(dueTime);

	return pTitle;
}

void PlayerTitleMgr::OnTick(const Avalon::Time &now)
{
	if (m_checkDueTitle.IsTimeout(now))
	{
		std::vector<UInt64> titles;
		std::function<bool(PlayerTitle&)> ff = [&titles](PlayerTitle& title){
			if (!title.IsValid())
			{
				titles.push_back(title.GetID());
			}
			return true;
		};

		TraversalTitles(ff);
		if (titles.size() > 0)
		{
			for (auto titleGuid : titles)
			{
				RemoveOneTitleByGuid(titleGuid);
			}
		}
	}
}

void PlayerTitleMgr::OnLoadDataRet(CLRecordCallback* callback)
{
	if (callback == NULL) return;
	std::vector<UInt64> deleteIds;
	while (callback->NextRow())
	{
		UInt32	title_id = callback->GetData("title_id");
		//ObjID_t title_guid = callback->GetKey();
		NewTitleItemDataEntry* dataEntry = NewTitleItemDataEntryMgr::Instance()->FindEntry(title_id);
		if (!dataEntry)
		{
			ErrorLogStream << GetOwnerLogStr() << " load title err, data entry not find guid = " << callback->GetKey()
				<< ", titleId = " << title_id << LogStream::eos;
			continue;
		}

		PlayerTitle* pTitle = PlayerTitleMgr::CreateTitle(GetOwnerId(), dataEntry);
		pTitle->LoadFromDb(callback, dataEntry);
		//检查时限
		if (!pTitle->IsValid())
		{
			pTitle->Delete();
			delete pTitle;
			continue;
		}
		//检查重复
		if (GetOneTitleByDataId(title_id))
		{
			pTitle->Delete();
			delete pTitle;
			continue;
		}
		AddOneTitle(pTitle);
	}
}

bool PlayerTitleMgr::AddOneTitle(PlayerTitle* pTitle)
{
	if (!pTitle)
	{
		return false;
	}
	MapTitle::iterator it = m_titles.find(pTitle->GetID());
	if (it != m_titles.end())
	{
		return false;
	}

	m_titles.insert(std::pair<UInt64, PlayerTitle*>(pTitle->GetID(), pTitle));
	InfoLogStream << "pTitle->GetID() = " << pTitle->GetID() << ", owner = " << pTitle->m_owner << ",  title_id = " << pTitle->m_title_id << LogStream::eos;
	return true;
}

void PlayerTitleMgr::AddOneTitle(NewTitleItemDataEntry& dataEntry)
{	
	PlayerTitle* title = CreateTitle(GetOwnerId(), &dataEntry);
	if (!title)
	{
		return;
	}

	title->Update();
	AddOneTitle(title);
}

PlayerTitle* PlayerTitleMgr::GetOneTitleByGuid(UInt64 titleGuid)
{
	auto title = m_titles.find(titleGuid);
	if (title == m_titles.end() || !title->second)
	{
		return NULL;
	}

	return title->second;
}

PlayerTitle* PlayerTitleMgr::GetOneTitleByDataId(UInt32 titleId)
{
	for (auto itr : m_titles)
	{
		auto& title = itr.second;
		if (title->m_title_id == titleId)
		{
			return title;
		}
	}

	return NULL;
}

void PlayerTitleMgr::RemoveOneTitleByDataId(UInt32 titleId)
{
	PlayerTitle* pTitle = GetOneTitleByDataId(titleId);
	if (!pTitle)
	{
		return;
	}
	
	WSPlayer* player = GetOwnerPlayer();
	if (!player)
	{
		return;
	}

	CLProtocol::WorldNewTitleSyncUpdate protocl;
	protocl.delGuids.push_back(pTitle->GetID());
	player->SendProtocol(protocl);

	if (player->GetCurNewTitleGuid() == pTitle->GetID())
	{
		CLProtocol::WorldNewTitleModifyScene notify;
		notify.roleId = player->GetID();
		notify.ownerType = 0;
		notify.opType = NTOPP_SYNC;
		notify.titleGuid = 0;
		notify.titleName = "";
		notify.titleId = 0;
		notify.style = 0;
		player->SendToScene(notify);
	}

	m_titles.erase(pTitle->GetID());
	pTitle->Delete();
	CL_SAFE_DELETE(pTitle);
}


void PlayerTitleMgr::RemoveOneTitleByGuid(UInt64 titleGuid)
{
	PlayerTitle* pTitle = GetOneTitleByGuid(titleGuid);
	if (!pTitle)
	{
		return;
	}

	WSPlayer* player = GetOwnerPlayer();
	if (!player)
	{
		return;
	}

	CLProtocol::WorldNewTitleSyncUpdate protocl;
	protocl.delGuids.push_back(pTitle->GetID());
	player->SendProtocol(protocl);

	if (player->GetCurNewTitleGuid() == pTitle->GetID())
	{
		CLProtocol::WorldNewTitleModifyScene notify;
		notify.roleId = player->GetID();
		notify.ownerType = 0;
		notify.opType = NTOPP_SYNC;
		notify.titleGuid = 0;
		notify.titleName = "";
		notify.titleId = 0;
		notify.style = 0;

		player->SendToScene(notify);
	}

	m_titles.erase(pTitle->GetID());
	pTitle->Delete();
	CL_SAFE_DELETE(pTitle);
}

void PlayerTitleMgr::TraversalTitles(const std::function<bool(PlayerTitle&)>& ff)
{
	for (auto& elem : m_titles)
	{
		if (!elem.second) continue;
		if (!ff(*elem.second))
		{
			break;
		}
	}
}

void PlayerTitleMgr::GenerateAllTitlesName()
{
	std::function<bool(PlayerTitle&)> ff = [](PlayerTitle& title){
		if (title.IsValid())
		{
			title.GenerateTitleName();
		}
		return true;
	};

	TraversalTitles(ff);
}

PlayerTitle* PlayerTitleMgr::GetOneTitle(WSPlayer* player, UInt64 titleGuid)
{
	if (!player)
	{
		return NULL;
	}
	PlayerTitle* pTitle = player->GetTitleMgr().GetOneTitleByGuid(titleGuid);
	if (pTitle)
	{
		return pTitle;
	}

	return  player->GetAccountInfo()->GetTitleMgr().GetOneTitleByGuid(titleGuid);
}

PlayerTitle* PlayerTitleMgr::GetOneTitle(WSPlayer* player, UInt64 titleGuid, UInt8 titleOwnerType)
{
	PlayerTitle* pTitle = NULL;
	if (titleOwnerType == NTOTP_ROLE)
	{
		pTitle = player->GetTitleMgr().GetOneTitleByGuid(titleGuid);
	}
	else if (titleOwnerType == NTOTP_ACCT)
	{
		if (player->GetAccountInfo())
		{
			pTitle = player->GetAccountInfo()->GetTitleMgr().GetOneTitleByGuid(titleGuid);
		}
	}
	else
	{
	}

	return pTitle;
}

void PlayerTitleMgr::OnPlayerNewTitleTakeUpReq(WSPlayer* player, UInt64 titleGuid, UInt32 titleId)
{
	if (!player || titleGuid == player->GetCurNewTitleGuid())
	{
		return;
	}

	CLProtocol::WorldNewTitleTakeUpRes protocol;
	protocol.titleGuid = titleGuid;

	UInt32 checkRet = CheckTitleTake(player);
	if (checkRet != ErrorCode::SUCCESS)
	{
		ErrorLogStream << "player(" << player->GetID() << ")" << " take title fail" << titleGuid << ", data id = " << titleId 
			<< " reason:" << checkRet << LogStream::eos;
		protocol.res = checkRet;
		player->SendProtocol(protocol);
		return;
	}

	NewTitleItemDataEntry* dataEntry = NewTitleItemDataEntryMgr::Instance()->FindEntry(titleId);
	if (!dataEntry)
	{
		ErrorLogStream << "player(" << player->GetID() << ")" << " take up title fail, can't find title entry, title guid = " << titleGuid << ", data id = " << titleId << LogStream::eos;
		protocol.res = ErrorCode::NEW_TITLE_TAKE_UP_FAIL;
		player->SendProtocol(protocol);
		return;
	}

	PlayerTitle* pTitle = PlayerTitleMgr::GetOneTitle(player, titleGuid);
	
	if (!pTitle)
	{
		ErrorLogStream << "player(" << player->GetID() << ")" << " take up title fail, can't find title, guid = " << titleGuid << ", titleId = " << titleId << ", dataEntry->ownerType = " << 
			dataEntry->ownerType << LogStream::eos;
		protocol.res = ErrorCode::NEW_TITLE_TAKE_UP_FAIL;
		player->SendProtocol(protocol);
		return;
	}

	CLProtocol::WorldNewTitleModifyScene notify;
	notify.roleId = player->GetID();
	notify.ownerType = dataEntry->ownerType;
	notify.opType = NTOPP_UP;
	notify.titleGuid = titleGuid;
	notify.titleName = pTitle->GetName();
	notify.titleId = dataEntry->GetKey();
	notify.style = dataEntry->style;

	player->SendToScene(notify);
	//protocol.res = ErrorCode::SUCCESS;
	//player->SendProtocol(protocol);
}


void PlayerTitleMgr::OnPlayerNewTitleTakeOffReq(WSPlayer* player, UInt64 titleGuid, UInt32 titleId)
{
	if (!player || titleGuid != player->GetCurNewTitleGuid())
	{
		return;
	}

	CLProtocol::WorldNewTitleTakeOffRes protocol;
	protocol.titleGuid = titleGuid;

	UInt32 checkRet = CheckTitleTake(player);
	if (checkRet != ErrorCode::SUCCESS)
	{
		ErrorLogStream << "player(" << player->GetID() << ")" << " take title fail" << titleGuid << ", data id = " << titleId
			<< " reason:" << checkRet << LogStream::eos;
		protocol.res = checkRet;
		player->SendProtocol(protocol);
		return;
	}

	NewTitleItemDataEntry* dataEntry = NewTitleItemDataEntryMgr::Instance()->FindEntry(titleId);
	if (!dataEntry)
	{
		ErrorLogStream << "player(" << player->GetID() << ")" << " take off title fail, can't find title entry, title data id = " << titleId << LogStream::eos;
		protocol.res = ErrorCode::NEW_TITLE_TAKE_OFF_FAIL;
		player->SendProtocol(protocol);
		return;
	}

	PlayerTitle* pTitle = PlayerTitleMgr::GetOneTitle(player, titleGuid, dataEntry->ownerType);

	if (!pTitle)
	{
		ErrorLogStream << "player(" << player->GetID() << ")" << " take off title fail, can't find title, guid = " << titleGuid << LogStream::eos;
		protocol.res = ErrorCode::NEW_TITLE_TAKE_OFF_FAIL;
		player->SendProtocol(protocol);
		return;
	}


	CLProtocol::WorldNewTitleModifyScene notify;
	notify.roleId = player->GetID();
	notify.ownerType = dataEntry->ownerType;
	notify.opType = NTOPP_OFF;
	notify.titleGuid = 0;
	notify.titleName = "";
	notify.titleId = 0;
	notify.style = 0;

	player->SendToScene(notify);
	//protocol.res = ErrorCode::SUCCESS;
	//player->SendProtocol(protocol);
}

void PlayerTitleMgr::OnPlayerAddNewTitle(WSPlayer* player, UInt32 titleId, UInt32 dueTm)
{
	if (!player) return;
	NewTitleItemDataEntry* dataEntry = NewTitleItemDataEntryMgr::Instance()->FindEntry(titleId);
	if (!dataEntry)
	{
		ErrorLogStream << "player(" << player->GetID() << ")" << "add title fail, can't find title entry, title data id = " << titleId << LogStream::eos;
		return;
	}

	if (dataEntry->ownerType == NTOTP_ROLE)
	{
		player->GetTitleMgr().AddOneTitleByTitleId(titleId, dueTm);
	}
	else if (dataEntry->ownerType == NTOTP_ACCT)
	{
		player->GetAccountInfo()->GetTitleMgr().AddOneTitleByTitleId(titleId, dueTm);
	}
	else{
		ErrorLogStream << "player(" << player->GetID() << ")" << "add title fail, dataEntry->ownerType err, title data id = " << titleId 
			<< "ownerType : " << dataEntry->ownerType << LogStream::eos;
		return;
	}
}

UInt32 PlayerTitleMgr::CheckTitleTake(WSPlayer* player)
{
	auto sceneData = SceneDataEntryMgr::Instance()->FindEntry(player->GetSceneID());
	if (sceneData == NULL || sceneData->subType == SCENE_SUB_GUILD_BATTLE || sceneData->subType == SCENE_SUB_CROSS_GUILD_BATTLE)
	{
		return ErrorCode::NEW_TITLE_TAKE_SCENE_FORBID;
	}

	return ErrorCode::SUCCESS;
}

PlayerTitleRoleMgr::PlayerTitleRoleMgr()
{
}

PlayerTitleRoleMgr::~PlayerTitleRoleMgr()
{
}

UInt64 PlayerTitleRoleMgr::GetOwnerId() 
{ 
	return m_pOwner->GetID(); 
}

UInt64 PlayerTitleRoleMgr::GetOwnerPlayerId()
{
	WSPlayer* player = GetOwnerPlayer();
	if (player)
	{
		return player->GetID();
	}
	return 0;
}

void PlayerTitleRoleMgr::LoadTitles()
{
	//加载头衔
	CLSelectCommand* selectCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_new_title");
	selectCmd->PutCond("owner", GetOwner()->GetID());
	CLRecordClient::Instance()->SendCommand(selectCmd, new LoadTitleRoleCallback(GetOwner()));
}

void PlayerTitleRoleMgr::AddOneTitleByTitleId(UInt32 titleId, UInt32 dueTm)
{
	NewTitleItemDataEntry* dataEntry = NewTitleItemDataEntryMgr::Instance()->FindEntry(titleId);
	if (!dataEntry)
	{
		return;
	}

	if (dataEntry->ownerType != NTOTP_ROLE)
	{
		return;
	}

	PlayerTitle* pTitle = GetOneTitleByDataId(titleId); //永久头衔不能叠加
	if (pTitle && dueTm == 0)
	{
		return;
	}

	PlayerTitle* newTitle = CreateTitle(GetOwnerId(), dataEntry, dueTm);
	if (!newTitle)
	{
		return;
	}

	if (pTitle) //去掉旧的
	{
		RemoveOneTitleByGuid(pTitle->GetID());
	}

	if (dataEntry->category == NWCGRY_GUILD) //公会头衔,需要移除现有的其它公会头衔
	{
		std::vector<UInt64> titles;
		std::function<bool(PlayerTitle&)> ff = [&titles](PlayerTitle& title){
			NewTitleItemDataEntry* entry = NewTitleItemDataEntryMgr::Instance()->FindEntry(title.m_title_id);
			if (!entry)
			{
				return true;
			}
			if (entry->category == NWCGRY_GUILD)
			{
				titles.push_back(title.GetID());
			}
			return true;
		};

		TraversalTitles(ff);
		if (titles.size() > 0)
		{
			for (auto titleGuid : titles)
			{
				RemoveOneTitleByGuid(titleGuid);
			}
		}
	}

	newTitle->Update();

	if (PlayerTitleMgr::AddOneTitle(newTitle)) //玩家新获得头衔会自动穿戴新头衔
	{
		if (m_pOwner)
		{
			CLProtocol::WorldNewTitleModifyScene notify;
			notify.roleId = GetOwnerPlayerId();
			notify.ownerType = NTOTP_ROLE;
			notify.opType = NTOPP_SYNC;
			notify.titleGuid = newTitle->GetID();
			notify.titleName = newTitle->GetName();
			notify.titleId = newTitle->DataId();
			notify.style = newTitle->Style();
			
			m_pOwner->SendToScene(notify);

			CLProtocol::WorldNewTitleSyncUpdate protocl;
			protocl.addTitles.push_back(*newTitle);
			m_pOwner->SendProtocol(protocl);
		}
	}
}

void PlayerTitleRoleMgr::OnOnline()
{
	this->CheckNewTitlesOnLoaded();
	GetOwner()->GetAccountInfo()->GetTitleMgr().CheckNewTitlesOnLoaded();
	this->GenerateAllTitlesName();
	GetOwner()->GetAccountInfo()->GetTitleMgr().GenerateAllTitlesName();
	//上线同步头衔数据
	CLProtocol::WorldGetPlayerTitleSyncList protocol;

	std::function<bool(PlayerTitle&)> ff = [&protocol](PlayerTitle& title){
		if (!title.IsValid())
		{
			return true;
		}
		protocol.titles.push_back(title);
		return true;
	};

	TraversalTitles(ff);
	if (GetOwner() && GetOwner()->GetAccountInfo())
	{
		GetOwner()->GetAccountInfo()->GetTitleMgr().TraversalTitles(ff);
	}

	if (GetOwner() && protocol.titles.size() > 0)
	{
		GetOwner()->SendProtocol(protocol);
	}

	if (GetOwner()->GetCurNewTitleGuid() != 0)
	{
		PlayerTitle* pTitle = GetOneTitle(GetOwner(), GetOwner()->GetCurNewTitleGuid());
		if (!pTitle) return;
		CLProtocol::WorldNewTitleModifyScene notify;
		notify.roleId = GetOwner()->GetID();
		notify.ownerType = 0;
		notify.opType = NTOPP_SYNC;
		notify.titleGuid = GetOwner()->GetCurNewTitleGuid();
		notify.titleName = pTitle->GetName();
		notify.titleId = pTitle->DataId();
		notify.style = pTitle->Style();
		
		GetOwner()->SendToScene(notify);
	}
}

void PlayerTitleRoleMgr::CheckNewTitlesOnLoaded()
{
	class Visitor : public NewTitleDataEntryVisitor
	{
	public:
		Visitor(PlayerTitleRoleMgr* titleMgr) : m_titleMgr(titleMgr), m_guildPost(GUILD_POST_INVALID)
		{
			m_player = m_titleMgr->GetOwner();
			Guild* guild = GuildMgr::Instance()->FindGuildByID(m_player->GetGuildID());
			if (guild)
			{
				GuildMember* guildMember = guild->FindMember(m_player->GetID());
				if (guildMember)
				{
					m_guildPost = guildMember->GetPost();
				}
			}
		};
		~Visitor(){};

		virtual bool operator()(NewTitleItemDataEntry* dataEntry)
		{
			if (!dataEntry) return true;
			if (dataEntry->ownerType != NTOTP_ROLE) return true;

			//判断是否应该有这个头衔
			if (NWCGRY_GUILD == dataEntry->category)
			{
				if (dataEntry->categoryParam == m_guildPost)
				{
					m_addTitles.push_back(dataEntry);
				}
				else
				{
					m_delTitles.push_back(dataEntry);
				}
			}
			else if (NWCGRY_MASTER == dataEntry->category)
			{
				if (m_player->GetRelationMgr().GetRelation((UInt8)RELATION_MASTER))
				{
					m_addTitles.push_back(dataEntry);
				}
				else
				{
					m_delTitles.push_back(dataEntry);
				}
			}
			else if (NWCGRY_ACHIEVE == dataEntry->category)
			{
				return true;
			}
			else if (NWCGRY_ABATT == dataEntry->category)
			{
				return true;
			}
			else
			{
				return true;
			}
			return true;
		}
	public:
		PlayerTitleRoleMgr* m_titleMgr;
		WSPlayer* m_player;
		UInt8	m_guildPost; //公会职务
		std::vector<NewTitleItemDataEntry*> m_addTitles;
		std::vector<NewTitleItemDataEntry*> m_delTitles;

	};

	Visitor visitor(this);

	NewTitleItemDataEntryMgr::Instance()->Visit(visitor);

	
	for (auto entry : visitor.m_addTitles)
	{
		if (this->GetOneTitleByDataId(entry->GetKey()))
		{
			continue;
		}
		this->AddOneTitle(*entry);
	}

	for (auto entry : visitor.m_delTitles)
	{
		if (!this->GetOneTitleByDataId(entry->GetKey()))
		{
			continue;
		}
		this->RemoveOneTitleByDataId(entry->GetKey());
	}
	
}

void PlayerTitleRoleMgr::OnPlayerGuildPostChange(UInt8 oldPost, UInt8 newPost)
{
	UInt32 oldTitleId = NewTitleItemDataEntryMgr::Instance()->GetGuidTitleId(oldPost);
	if (oldTitleId > 0)
	{
		this->RemoveOneTitleByDataId(oldTitleId);
	}
	
	UInt32 newTitleId = NewTitleItemDataEntryMgr::Instance()->GetGuidTitleId(newPost);
	if (newTitleId > 0)
	{
		this->AddOneTitleByTitleId(newTitleId);
	}
}

void PlayerTitleRoleMgr::OnMakeMaster()
{
	UInt32 titleId = NewTitleItemDataEntryMgr::Instance()->GetMasterTitleId();
	AddOneTitleByTitleId(titleId);
}

void PlayerTitleRoleMgr::OnRemoveMaster()
{
	UInt32 titleId = NewTitleItemDataEntryMgr::Instance()->GetMasterTitleId();
	RemoveOneTitleByDataId(titleId);
}

void PlayerTitleRoleMgr::OnMasterNameChange()
{
	UInt32 titleId = NewTitleItemDataEntryMgr::Instance()->GetMasterTitleId();
	if (titleId == 0) return;

	PlayerTitle* pTitle = GetOneTitleByDataId(titleId);
	if (!pTitle) return;

	pTitle->GenerateTitleName();
	if (m_pOwner)
	{
		CLProtocol::WorldNewTitleUpdateData protocol;
		protocol.updates.push_back(*pTitle);
		m_pOwner->SendProtocol(protocol);

		if (m_pOwner->GetCurNewTitleGuid() == pTitle->GetID())
		{
			CLProtocol::WorldNewTitleModifyScene notify;
			notify.roleId = m_pOwner->GetID();
			notify.ownerType = NTOTP_ROLE;
			notify.opType = NTOPP_SYNC;
			notify.titleGuid = pTitle->GetID();
			notify.titleName = pTitle->GetName();
			notify.titleId = pTitle->DataId();
			notify.style = pTitle->Style();
			m_pOwner->SendToScene(notify);
		}
	}
}

void PlayerTitleRoleMgr::OnGuidNameChange()
{
	if (!m_pOwner)
	{
		return;
	}
	UInt8 guildPost = 0;
	Guild* guild = GuildMgr::Instance()->FindGuildByID(m_pOwner->GetGuildID());
	if (!guild) return;

	GuildMember* guildMember = guild->FindMember(m_pOwner->GetID());

	if (!guildMember) return;
	
	guildPost = guildMember->GetPost();
	
	UInt32 titleId = NewTitleItemDataEntryMgr::Instance()->GetGuidTitleId(guildPost);
	if (!titleId) return;

	PlayerTitle* pTitle = GetOneTitleByDataId(titleId);
	if (!pTitle) return;

	pTitle->GenerateTitleName();
	
	if (m_pOwner)
	{
		CLProtocol::WorldNewTitleUpdateData protocol;
		protocol.updates.push_back(*pTitle);
		m_pOwner->SendProtocol(protocol);

		if (m_pOwner->GetCurNewTitleGuid() == pTitle->GetID())
		{
			CLProtocol::WorldNewTitleModifyScene notify;
			notify.roleId = m_pOwner->GetID();
			notify.ownerType =  NTOTP_ROLE;
			notify.opType = NTOPP_SYNC;
			notify.titleGuid = pTitle->GetID();
			notify.titleName = pTitle->GetName();
			notify.titleId = pTitle->DataId();
			notify.style = pTitle->Style();
			m_pOwner->SendToScene(notify);
		}
	}
}

void PlayerTitleRoleMgr::OnGuidLeave()
{
	class Visitor : public NewTitleDataEntryVisitor
	{
	public:
		Visitor(UInt8 category) : m_category(category)
		{
		};
		~Visitor(){};

		virtual bool operator()(NewTitleItemDataEntry* dataEntry)
		{
			if (!dataEntry) return true;
			if (dataEntry->ownerType != NTOTP_ROLE) return true;

			//判断是否应该有这个头衔
			if (NWCGRY_GUILD == dataEntry->category)
			{
				m_delTitles.push_back(dataEntry);
				
			}
			else
			{
				return true;
			}
			return true;
		}
	public:
		UInt8 m_category;
		std::vector<NewTitleItemDataEntry*> m_delTitles;

	};

	Visitor visitor(NWCGRY_GUILD);

	NewTitleItemDataEntryMgr::Instance()->Visit(visitor);

	for (auto entry : visitor.m_delTitles)
	{
		if (!this->GetOneTitleByDataId(entry->GetKey()))
		{
			continue;
		}
		this->RemoveOneTitleByDataId(entry->GetKey());
	}
}

void PlayerTitleRoleMgr::OnPlayerNewTitleTakeUpGuildPostReq(WSPlayer* player)
{
	if (NULL == player) return;

	CLProtocol::WorldNewTitleTakeUpGuildPostRes res;
	res.res = ErrorCode::SUCCESS;

	do 
	{
		auto guildMember = GuildMgr::Instance()->FindGuildMember(player->GetID());
		if (!guildMember)
		{
			break;
		}

		UInt32 titleId = NewTitleItemDataEntryMgr::Instance()->GetGuidTitleId(guildMember->GetPost());
		if (titleId == 0)
		{
			ErrorLogStream << PLAYERINFO(player) << "not guild post title id:" << titleId << LogStream::eos;
			break;
		}

		PlayerTitle* pTitle = GetOneTitleByDataId(titleId);
		if (NULL == pTitle)
		{
			ErrorLogStream << PLAYERINFO(player) << "not title id:" << titleId << LogStream::eos;
			break;
		}

		if (player->GetCurNewTitleGuid() == pTitle->GetID())
		{
			break;
		}

		CLProtocol::WorldNewTitleModifyScene notify;
		notify.roleId = player->GetID();
		notify.ownerType = NTOTP_ROLE;
		notify.opType = NTOPP_UP;
		notify.titleGuid = pTitle->GetID();
		notify.titleName = pTitle->GetName();
		notify.titleId = titleId;
		notify.style = pTitle->Style();
		player->SendToScene(notify);

	} while (0);

	player->SendProtocol(res);
}

PlayerTitleAccountMgr::PlayerTitleAccountMgr()
{
}

PlayerTitleAccountMgr::~PlayerTitleAccountMgr()
{
}

UInt64 PlayerTitleAccountMgr::GetOwnerPlayerId()
{
	WSPlayer* player = GetOwnerPlayer();
	if (player)
	{
		return player->GetID();
	}
	return 0;
}

UInt64 PlayerTitleAccountMgr::GetOwnerId()
{ 
	return m_pOwner->GetAccID(); 
}

void PlayerTitleAccountMgr::LoadTitles()
{
	//加载头衔
	CLSelectCommand* selectCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_new_title_accout");
	selectCmd->PutCond("owner", GetOwner()->GetAccID());
	CLRecordClient::Instance()->SendCommand(selectCmd, new LoadTitleAccoutCallback(GetOwner()));
}

void PlayerTitleAccountMgr::AddOneTitleByTitleId(UInt32 titleId, UInt32 dueTm)
{
	NewTitleItemDataEntry* dataEntry = NewTitleItemDataEntryMgr::Instance()->FindEntry(titleId);
	if (!dataEntry)
	{
		return;
	}

	if (dataEntry->ownerType != NTOTP_ACCT)
	{
		return;
	}

	PlayerTitle* pTitle = GetOneTitleByDataId(titleId); //永久头衔不能叠加
	if (pTitle && dueTm == 0)
	{
		return;
	}

	PlayerTitle* newTitle = CreateTitle(GetOwnerId(), dataEntry, dueTm);
	if (!newTitle)
	{
		return;
	}

	if (pTitle) //去掉旧的
	{
		RemoveOneTitleByGuid(pTitle->GetID());
	}
	
	newTitle->Update();

	if (PlayerTitleMgr::AddOneTitle(newTitle)) //玩家新获得头衔会自动穿戴新头衔
	{
		WSPlayer* player = GetOwnerPlayer();
		if (player)
		{
			CLProtocol::WorldNewTitleModifyScene notify;
			notify.roleId = player->GetID();
			notify.ownerType = NTOTP_ACCT;
			notify.opType = NTOPP_SYNC;
			notify.titleGuid = newTitle->GetID();
			notify.titleName = newTitle->GetName();
			notify.titleId = newTitle->DataId();
			notify.style = newTitle->Style();

			player->SendToScene(notify);

			CLProtocol::WorldNewTitleSyncUpdate protocl;
			protocl.addTitles.push_back(*newTitle);
			player->SendProtocol(protocl);
		}
	}
}

WSPlayer* PlayerTitleAccountMgr::GetOwnerPlayer()
{
	return WSPlayerMgr::Instance()->FindPlayer(m_pOwner->GetRoleID());
}

void PlayerTitleAccountMgr::CheckNewTitlesOnLoaded()
{
	class Visitor : public NewTitleDataEntryVisitor
	{
	public:
		Visitor(PlayerTitleAccountMgr* titleMgr) : m_titleMgr(titleMgr)
		{
		};
		~Visitor(){};

		virtual bool operator()(NewTitleItemDataEntry* dataEntry)
		{
			if (!dataEntry) return true;
			if (dataEntry->ownerType != NTOTP_ACCT) return true;

			//判断是否应该有这个头衔
			if (NWCGRY_YBT == dataEntry->category && 0 == dataEntry->categoryParam)
			{
				if (m_titleMgr->GetOwner()->HasAdventureTeam())
				{
					m_addTitles.push_back(dataEntry);
				}
				else
				{
					m_delTitles.push_back(dataEntry);
				}
			}
			else
			{
				return true;
			}
			return true;
		}
	public:
		PlayerTitleAccountMgr* m_titleMgr;
		std::vector<NewTitleItemDataEntry*> m_addTitles;
		std::vector<NewTitleItemDataEntry*> m_delTitles;

	};

	Visitor visitor(this);

	NewTitleItemDataEntryMgr::Instance()->Visit(visitor);

	for (auto entry : visitor.m_addTitles)
	{
		if (this->GetOneTitleByDataId(entry->GetKey()))
		{
			continue;
		}
		this->AddOneTitle(*entry);
	}

	for (auto entry : visitor.m_delTitles)
	{
		if (!this->GetOneTitleByDataId(entry->GetKey()))
		{
			continue;
		}
		this->RemoveOneTitleByDataId(entry->GetKey());
	}
}

void PlayerTitleAccountMgr::OnCreateYbt()
{
	UInt32 titleId = NewTitleItemDataEntryMgr::Instance()->GetYbtTitleId();
	AddOneTitleByTitleId(titleId);
}

void PlayerTitleAccountMgr::OnYbtNameChange()
{
	UInt32 titleId = NewTitleItemDataEntryMgr::Instance()->GetYbtTitleId();
	if (titleId == 0) return;

	PlayerTitle* pTitle = GetOneTitleByDataId(titleId);
	if (!pTitle) return;

	pTitle->GenerateTitleName();

	WSPlayer * player = GetOwnerPlayer();

	if (player)
	{
		CLProtocol::WorldNewTitleUpdateData protocol;
		protocol.updates.push_back(*pTitle);
		player->SendProtocol(protocol);

		if (player->GetCurNewTitleGuid() == pTitle->GetID())
		{
			CLProtocol::WorldNewTitleModifyScene notify;
			notify.roleId = player->GetID();
			notify.ownerType = NTOTP_ACCT;
			notify.opType = NTOPP_SYNC;
			notify.titleGuid = pTitle->GetID();
			notify.titleName = pTitle->GetName();
			notify.titleId = pTitle->DataId();
			notify.style = pTitle->Style();
			player->SendToScene(notify);
		}
	}
}