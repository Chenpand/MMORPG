#include "RetinueMgr.h"
#include <CLRetinueProtocol.h>
#include "Player.h"
#include "SceneSysNotify.h"
#include <CLSystemValueDataEntry.h>
#include "SSServiceSwitchMgr.h"

OffRetinueList::OffRetinueList() 
{
	m_OffRetinueIds.resize(OFF_RETINUE_COUNT); 
}
OffRetinueList::~OffRetinueList() { };

void OffRetinueList::SetOffRetinueId(UInt32 index, UInt64 value) 
{
	if (index < OFF_RETINUE_COUNT && index >= 0)
	{
		m_OffRetinueIds[index] = value;
		SetDirty();
	}
}

UInt64 OffRetinueList::GetOffRetinueId(UInt32 index)
{
	if (index < OFF_RETINUE_COUNT && index >= 0)
	{
		return m_OffRetinueIds[index];
	}
	return 0;
}

void OffRetinueList::RemoveOffRetinueId(UInt64 id)
{
	for (UInt32 i = 0; i < m_OffRetinueIds.size(); ++i)
	{
		if (id == m_OffRetinueIds[i])
		{
			m_OffRetinueIds[i] = 0;
			SetDirty();
		}
	}
}

bool OffRetinueList::IsUsing(UInt64 id, UInt32& index)
{
	for (UInt32 i = 0; i < m_OffRetinueIds.size(); ++i)
	{
		if (id == m_OffRetinueIds[i])
		{
			index = i;
			return true;
		}
	}
	return false;
}

void OffRetinueList::DecodeString(std::istringstream& is)
{
	char split = 0;
	UInt64 value = 0;

	for (UInt32 i = 0; i < OFF_RETINUE_COUNT; ++i)
	{
		is >> value >> split;
		m_OffRetinueIds[i] = value;
	}
}

void OffRetinueList::EncodeString(std::ostringstream& os)
{
	for (UInt32 i = 0; i < m_OffRetinueIds.size(); ++i)
	{
		os << m_OffRetinueIds[i] << ";";
	}
}


RetinueMgr::RetinueMgr()
{
	//m_MaxCount = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_RETINUE_SKILL);
	m_PointRatio = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_RETINUE_SOULS);
}

RetinueMgr::~RetinueMgr()
{
	for (RetinueVec::iterator iter = m_RetinueVec.begin(); iter != m_RetinueVec.end(); ++iter)
	{
		CL_SAFE_DELETE(*iter);
	}
	m_RetinueMap.clear();
	m_RetinueVec.clear();
}

bool RetinueMgr::Init()
{
	if (!RetinueSkillDataEntryMgr::Instance()->CheckData()) return false;
	return true;
}

void RetinueMgr::VisitorRetinue(RetinueVisitor& visitor)
{
	RetinueVec::iterator iter = m_RetinueVec.begin();
	while (iter != m_RetinueVec.end())
	{
		Retinue* retinue = *iter;
		++iter;
		if (!visitor(retinue)) return;
	}
}

Retinue* RetinueMgr::FindRetinue(UInt32 dataId)
{
	class FindRetinueVisitor : public RetinueVisitor
	{
	public:
		FindRetinueVisitor(UInt32 id) : m_DataId(id), m_Retinue(NULL) {}

		bool operator()(Retinue* retinue)
		{
			if(retinue->GetDataId() == m_DataId)
			{
				m_Retinue = retinue;
				return false;
			}
			return true;
		}

		Retinue* GetRetinue() { return m_Retinue; }

	private:
		UInt32	m_DataId;
		Retinue* m_Retinue;
	};

	FindRetinueVisitor visitor(dataId);
	VisitorRetinue(visitor);
	return visitor.GetRetinue();
}

UInt32 RetinueMgr::GetRetinueSize()
{
	return m_RetinueVec.size();
}

Retinue* RetinueMgr::FindRetinue(ObjID_t id)
{
	if (!SSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_RETINUE))
	{
		// 功能未开放
		return NULL;
	}
	RetinueMap::const_iterator iter = m_RetinueMap.find(id);
	if (iter != m_RetinueMap.end()) return iter->second;
	return NULL;
}

ObjUInt32& RetinueMgr::GetFightingDataId()
{ 
	return m_FightingDataId; 
}


ObjSceneRetinue& RetinueMgr::GetSceneRetinueInfo()
{
	return m_SceneRetinue;
}

void RetinueMgr::SetRetinueFighting(UInt64 id, UInt32 dataId, UInt8 level)
{
	m_FightingDataId = dataId;
	m_FightingRetinue = id;
	m_SceneRetinue.SetRetinue(dataId, level);
}

void RetinueMgr::GetRaceRetinues(std::vector<RaceRetinue>& raceRetinues)
{
	if (!SSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_RETINUE))
	{
		// 功能未开放
		return;
	}

	Retinue* retinue = FindRetinue(m_FightingRetinue);
	if (retinue != NULL)
	{
		RaceRetinue fightingRetinue;
		fightingRetinue.m_DataId = retinue->GetDataId();
		fightingRetinue.m_Level = retinue->GetLevel();
		fightingRetinue.m_Star = retinue->GetStarLevel();
		fightingRetinue.m_IsMain = true;
		retinue->GetBuffIds(fightingRetinue.m_BuffIds);
		raceRetinues.push_back(fightingRetinue);
	}

	std::vector<UInt64> offIds = m_OffRetinueList.GetRetinueIds();
	for (UInt32 i = 0; i < offIds.size(); ++i)
	{
		Retinue* entry = FindRetinue(offIds[i]);
		if (entry == NULL) continue;
		RaceRetinue offRetinue;
		offRetinue.m_DataId = entry->GetDataId();
		offRetinue.m_Level = entry->GetLevel();
		offRetinue.m_Star = entry->GetStarLevel();
		offRetinue.m_IsMain = false;
		entry->GetBuffIds(offRetinue.m_BuffIds);
		raceRetinues.push_back(offRetinue);
	}
}

UInt32 RetinueMgr::GetChangeSkillTotal() 
{
	return GetOwner()->GetCounter(COUNTER_RETINUE_SKILL_TOTAL); 
}

UInt32 RetinueMgr::GetSkillCount(UInt32 id, UInt8 level)
{
	class GetSkillCountVisitor : public RetinueVisitor
	{
	public:
		GetSkillCountVisitor(UInt32 id, UInt8 level) : m_Id(id), m_Level(level), m_Count(0){}

		bool operator()(Retinue* retinue)
		{
			m_Count += retinue->GetSkillCount(m_Id, m_Level);
			return true;
		}

		UInt32 GetCount() { return m_Count; }

	private:
		UInt32	m_Id;
		UInt8	m_Level;
		UInt32	m_Count;
	};

	GetSkillCountVisitor visitor(id, level);
	VisitorRetinue(visitor);
	return visitor.GetCount();
}


UInt32 RetinueMgr::GetMaxRetinueLevel()
{
	class GetMaxRetinueLevelVisitor : public RetinueVisitor
	{
	public:
		GetMaxRetinueLevelVisitor() :m_MaxLevel(0){}

		bool operator()(Retinue* retinue)
		{
			if (retinue->GetLevel() > m_MaxLevel)
				m_MaxLevel = retinue->GetLevel();
			return true;
		}

		UInt8 GetMaxLevel() { return m_MaxLevel; };

	private:
		UInt8 m_MaxLevel;
	};

	GetMaxRetinueLevelVisitor visitor;
	VisitorRetinue(visitor);
	return visitor.GetMaxLevel();
}


UInt32 RetinueMgr::GetMaxRetinueStar()
{
	class GetMaxRetinueStarVisitor : public RetinueVisitor
	{
	public:
		GetMaxRetinueStarVisitor() :m_MaxStar(0){}

		bool operator()(Retinue* retinue)
		{
			if (retinue->GetStarLevel() > m_MaxStar)
				m_MaxStar = retinue->GetStarLevel();
			return true;
		}

		UInt8 GetMaxStar() { return m_MaxStar; };

	private:
		UInt8 m_MaxStar;
	};

	GetMaxRetinueStarVisitor visitor;
	VisitorRetinue(visitor);
	return visitor.GetMaxStar();
}

UInt32 RetinueMgr::AddRetinue(UInt32 dataId, bool isConsume)
{
	if (!SSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_RETINUE))
	{
		// 功能未开放
		return ErrorCode::SYS_NOT_OPEN;
	}

	UInt32 nRes = ErrorCode::SUCCESS;
	nRes = CheckAddRetinue(dataId);
	if (nRes != ErrorCode::SUCCESS)
	{
		GetOwner()->SendNotify(nRes);
		return nRes;
	}

	Retinue* retinue = new Retinue(m_pOwner ,dataId);
	nRes = retinue->Init();
	if (nRes != ErrorCode::SUCCESS)
	{
		GetOwner()->SendNotify(nRes);
		CL_SAFE_DELETE(retinue);
		ErrorLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ") AddRetinue error " << nRes << LogStream::eos;
		return nRes;
	}

	nRes = retinue->InitSkill();
	if (nRes != ErrorCode::SUCCESS)
	{
		GetOwner()->SendNotify(nRes);
		CL_SAFE_DELETE(retinue);
		ErrorLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ") AddRetinue error " << nRes << LogStream::eos;
		return nRes;
	}

	//消耗物品
	if (isConsume)
	{
		nRes = retinue->UnlockConsumeItem();
		if (nRes != ErrorCode::SUCCESS)
		{
			GetOwner()->SendNotify(nRes);
			CL_SAFE_DELETE(retinue);
			ErrorLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ") AddRetinue error " << nRes << LogStream::eos;
			return nRes;
		}
	}

	retinue->InsertRetinue();

	m_RetinueVec.push_back(retinue);
	m_RetinueMap.insert(std::make_pair(retinue->GetID(), retinue));

	//下发添加到随从
	SyncRetinue(retinue);

	//添加随从事件
	GetOwner()->OnAddRetinue(retinue);

	InfoLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ") AddRetinue success " << nRes << LogStream::eos;

	return nRes;
}

UInt32 RetinueMgr::AddRetinue(Retinue* retinue)
{
	UInt32 nRes = ErrorCode::SUCCESS;
	nRes = CheckAddRetinue(retinue->GetDataId());
	if (nRes != ErrorCode::SUCCESS) return nRes;

	m_RetinueVec.push_back(retinue);
	m_RetinueMap.insert(std::make_pair(retinue->GetID(), retinue));
	
	return nRes;

}

UInt32 RetinueMgr::UpLevelRetinue(ObjID_t id)
{
	if (!SSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_RETINUE))
	{
		// 功能未开放
		return ErrorCode::SYS_NOT_OPEN;
	}

	Retinue* retinue = FindRetinue(id);
	if (retinue == NULL) return ErrorCode::RETINUE_NOT_EXIST;
	UInt32 result = retinue->UpLevel();
	if (result == ErrorCode::SUCCESS)
	{
		SyncRetinue(retinue);

		//如果是佩戴的随从,那么同步一下.
		if (retinue->GetID() == m_FightingRetinue)
		{
			m_SceneRetinue.SetRetinue(retinue->GetDataId(), retinue->GetLevel());
		}
		
		GetOwner()->OnRetinueUpLevel(retinue);
	}
	return result;
}

UInt32 RetinueMgr::UpStarRetinue(ObjID_t id)
{
	Retinue* retinue = FindRetinue(id);
	if (retinue == NULL) return ErrorCode::RETINUE_NOT_EXIST;
	UInt32 result = retinue->UpStar();
	if (result == ErrorCode::SUCCESS)
	{
		SyncRetinue(retinue);
		GetOwner()->OnRetinueUpStar(retinue);
	}
	return result;
}

UInt32 RetinueMgr::ChangeSkill(ObjID_t id, std::vector<UInt32> skillIds)
{
	if (!SSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_RETINUE))
	{
		// 功能未开放
		return ErrorCode::SYS_NOT_OPEN;
	}

	Retinue* retinue = FindRetinue(id);
	if (retinue == NULL) return ErrorCode::RETINUE_NOT_EXIST;

	UInt32 nRes = retinue->ChangeSkill(skillIds);

	if (nRes == ErrorCode::SUCCESS)
	{
		std::string reason = GetReason(SOURCE_TYPE_RETINUE_CHANGE_SKILL, id);

		GetOwner()->IncCounter(COUNTER_RETINUE_SKILL_NUM, 1);
		GetOwner()->IncCounter(COUNTER_RETINUE_SKILL_TOTAL, 1);

		SyncRetinue(retinue);
	}

	return nRes;
}

UInt32 RetinueMgr::TestChangeSkill(ObjID_t id, std::vector<UInt32> skillIds)
{
	Retinue* retinue = FindRetinue(id);
	if (retinue == NULL) return ErrorCode::RETINUE_NOT_EXIST;

	UInt32 nRes = retinue->TestChangeSkill(skillIds);
	
	if (nRes == ErrorCode::SUCCESS)
	{
		SyncRetinue(retinue);
	}

	return nRes;
}

UInt32 RetinueMgr::SetRetinue(ObjID_t id, UInt32 index)
{
	if (!SSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_RETINUE))
	{
		//功能未开放
		return ErrorCode::SYS_NOT_OPEN;
	}

	if (id == 0) return ErrorCode::RETINUE_NOT_DOWN_ERROR;

	if (index > MAIN_RETINUE_INDEX + OFF_RETINUE_COUNT || index <= 0)
		return ErrorCode::RETINUE_RETINUE_INDEX_ERROR;

	if (GetOwner()->GetMaxRetinueNum() < index)
		return ErrorCode::RETINUE_PLAYER_LEVEL;

	Retinue* retinue = FindRetinue(id);
	if (retinue == NULL) return ErrorCode::RETINUE_NOT_EXIST;

	//主随从
	if (index == MAIN_RETINUE_INDEX)
	{
		UInt32 nRes = retinue->IsFollow();
		if (nRes != ErrorCode::SUCCESS) return nRes;

		//检查这个随从是否在副随从里
		UInt32 tmpIndex = 0;
		if (m_OffRetinueList.IsUsing(id, tmpIndex))
		{
			//如果在副随从里,把主随从设置到副随从
			m_OffRetinueList.SetOffRetinueId(tmpIndex, m_FightingRetinue);
		}
		//设置主随从
		SetRetinueFighting(retinue->GetID(), retinue->GetDataId(), retinue->GetLevel());
	}
	else
	{
		if (GetRetinueFighting() == 0) return ErrorCode::RETINUE_NOT_MAIN_ERROR;

		index = index - OFF_RETINUE_INDEX_OFFSET;

		UInt32 tmpIndex = 0;

		//随从已经在主随从位
		if (GetRetinueFighting() == id)
		{
			Retinue* tmpRetinue = FindRetinue(m_OffRetinueList.GetOffRetinueId(index));
			if (tmpRetinue == NULL) return ErrorCode::RETINUE_NOT_DOWN_ERROR;
			SetRetinueFighting(tmpRetinue->GetID(), tmpRetinue->GetDataId(), tmpRetinue->GetLevel());
		}
		else if (m_OffRetinueList.IsUsing(id, tmpIndex))  //已经在副随从位置
		{
			m_OffRetinueList.SetOffRetinueId(tmpIndex, m_OffRetinueList.GetOffRetinueId(index));
		}
		m_OffRetinueList.SetOffRetinueId(index, id);
	}

	return ErrorCode::SUCCESS;
}

void RetinueMgr::OnOnline()
{
	/*
	if (!SSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_RETINUE))
	{
		// 功能未开放
		return;
	}
	*/

	Retinue* retinue = FindRetinue(GetRetinueFighting());
	if (retinue != NULL) SetRetinueFighting(retinue->GetID(), retinue->GetDataId(), retinue->GetLevel());

	//下发随从数据
	SyncRetinueList();
}

void RetinueMgr::OnSelectDataRet(CLRecordCallback* callback)
{
	if (callback == NULL) return;

	while (callback->NextRow())
	{
		ObjID_t id = callback->GetKey();
		UInt32 dataId = callback->GetData("dataid");
		UInt8 level = callback->GetData("level");
		UInt8 starLevel = callback->GetData("starlevel");
		std::string skills = callback->GetData("skills").GetString();
		Retinue* retinue = new Retinue(GetOwner(), dataId, id, level, starLevel, skills);
		UInt32 nRes = retinue->Init();
		if (nRes != ErrorCode::SUCCESS)
		{
			CL_SAFE_DELETE(retinue);
			continue;
		}

		nRes = AddRetinue(retinue);
		if (nRes != ErrorCode::SUCCESS)
		{
			CL_SAFE_DELETE(retinue);
			continue;
		}
	}
}

UInt32 RetinueMgr::CheckAddRetinueByItemId(UInt32 itemId)
{
	RetinueDataEntry* dataEntry = RetinueDataEntryMgr::Instance()->GetRetinueDataEntry(itemId);
	if (dataEntry == NULL)
	{
		return ErrorCode::RETINUE_DATA_NOT_EXIST;
	}

	Retinue* retinue = FindRetinue(dataEntry->id);
	if (retinue != NULL)
	{
		return ErrorCode::RETINUE_IS_EXIST;
	}

	if (m_pOwner->GetLevel() < dataEntry->level)
	{
		return ErrorCode::RETINUE_PLAYER_LEVEL;
	}

	return ErrorCode::SUCCESS;
}

UInt32 RetinueMgr::GetRetinueSizeByQuality(RetinueQuality quality)
{
	class GetRetinueSizeVisitor : public RetinueVisitor
	{
	public:
		GetRetinueSizeVisitor(RetinueQuality quality) : m_Quality(quality),m_Num(0) { }

		bool operator()(Retinue* retinue)
		{
			if (retinue->GetQuality() >= m_Quality)
			{
				m_Num += 1;
			}
			return true;
		}

		UInt32 GetSize() { return m_Num; }

	private:
		RetinueQuality	m_Quality;
		UInt32			m_Num;
	};

	GetRetinueSizeVisitor visitor(quality);
	VisitorRetinue(visitor);
	return visitor.GetSize();
}

void RetinueMgr::ResetChangeSkillCount()
{
	GetOwner()->SetCounter(COUNTER_RETINUE_SKILL_NUM, 0);
}

void RetinueMgr::SaveToDB()
{
	class SaveRetinueVisitor : public RetinueVisitor
	{
	public:
		SaveRetinueVisitor(Player* pPlayer) : m_pPlayer(pPlayer) {}
		~SaveRetinueVisitor(){}

		bool operator()(Retinue *pRetinue)
		{
			pRetinue->SyncToDB(true);
			return true;
		}

	private:
		Player	*m_pPlayer;
	};

	SaveRetinueVisitor visitor(GetOwner());
	VisitorRetinue(visitor);
}

UInt32 RetinueMgr::CheckAddRetinue(UInt32 dataId)
{
	Retinue* retinue = FindRetinue(dataId);
	if (retinue != NULL) return ErrorCode::RETINUE_IS_EXIST;
	return ErrorCode::SUCCESS;
}

void RetinueMgr::RemoveOffRetinue(UInt64 id)
{
	m_OffRetinueList.RemoveOffRetinueId(id);
}

std::vector<RetinueInfo> RetinueMgr::GetRetinueList()
{
	class GetRetinueListVisitor : public RetinueVisitor
	{
	public:
		GetRetinueListVisitor() {}

		bool operator()(Retinue* retinue)
		{
			RetinueInfo info;
			retinue->GetRetinueInfo(info);
			m_RetinueList.push_back(info);
			return true;
		}

		std::vector<RetinueInfo> GetRetinueList() { return m_RetinueList; }

	private:
		std::vector<RetinueInfo> m_RetinueList;
	};

	GetRetinueListVisitor visitor;
	VisitorRetinue(visitor);
	return visitor.GetRetinueList();
}

void RetinueMgr::SyncRetinueList()
{
	CLProtocol::SceneSyncRetinueList protocol;
	protocol.id = m_FightingRetinue;
	protocol.offRetinueIds = m_OffRetinueList.GetRetinueIds();
	protocol.retinueList = GetRetinueList();
	GetOwner()->SendProtocol(protocol);
}

void RetinueMgr::SyncRetinue(Retinue* retinue)
{
	CLProtocol::SceneSyncRetinue protocol;
	retinue->GetRetinueInfo(protocol.info);
	GetOwner()->SendProtocol(protocol);

	retinue->SyncToDB(false);
}
