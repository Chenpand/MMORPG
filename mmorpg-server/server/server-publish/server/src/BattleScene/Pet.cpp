#include "Pet.h"
#include "Player.h"
#include <CLRecordClient.h>
#include <CLSystemValueDataEntry.h>
#include "SSServiceSwitchMgr.h"

Pet::Pet(Player* player, UInt32 _dataId, ObjID_t _id, UInt16 _level, UInt32 _exp, UInt16 _hunger, UInt8 _skillIndex, UInt8 _goldFeedCount, UInt8 _pointFeedCount, UInt8 _selectSkillCount)
	:m_pOwner(player)
{
	SetID(_id);
	m_DataId.SetDBValue(_dataId);
	m_Level.SetDBValue(_level);
	m_Exp.SetDBValue(_exp);
	m_Hunger.SetDBValue(_hunger);
	m_SkillIndex.SetDBValue(_skillIndex);
	m_GoldFeedCount.SetDBValue(_goldFeedCount);
	m_PointFeedCount.SetDBValue(_pointFeedCount);
	m_SelectSkillCount.SetDBValue(_selectSkillCount);
	m_pDataEntry = NULL;
	m_pLevelDataEntry = NULL;
	m_PetScore = 0;
}

Pet::Pet(Player* player, UInt32 _dataId, UInt16 _hungry) : Pet(player, _dataId, 0, 1, 0, _hungry, 0, 0, 0, 0)
{
}

Pet::~Pet()
{
}

UInt32 Pet::Init()
{
	if (m_pOwner == NULL) return ErrorCode::PET_NOT_PLAYER;
	m_pDataEntry = PetDataEntryMgr::Instance()->FindEntry(m_DataId);
	if (m_pDataEntry == NULL) return ErrorCode::PET_DATA_NOT_EXIST;
	m_pLevelDataEntry = PetLevelDataEntryMgr::Instance()->FindEntry(CL_MAKE_PET_LEVEL_KEY(m_Level, m_pDataEntry->quality));
	if (m_pLevelDataEntry == NULL) return ErrorCode::PET_DATA_NOT_EXIST;
	return ErrorCode::SUCCESS;
}

void Pet::GetPetInfo(PetInfo& info)
{
	info.id = GetID();
	info.dataId = GetDataId();
	info.level = GetLevel();
	info.exp = GetExp();
	info.hunger = GetHunger();
	info.skillIndex = GetSkillIndex();
	info.pointFeedCount = GetPointFeedCount();
	info.goldFeedCount = GetGoldFeedCount();
	info.selectSkillCount = GetSelectSkillCount();
}

UInt32 Pet::GetDevourExp()
{
	UInt32 sumExp = PetLevelDataEntryMgr::Instance()->GetSumExp(GetLevel(), GetQuality());
	sumExp += GetExp();
	sumExp = (UInt32)((sumExp * 1.00) * ((SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_PET_DEVOUR_EXP_RATIO) * 1.00) / 100.00));
	sumExp += m_pDataEntry->devourExp;
	return sumExp;
}

void Pet::OnRemoveFatigue(UInt16 fatigue)
{
	UInt32 oldExp = GetExp();
	UInt16 oldLevel = GetLevel();

	UInt32 exp = m_pLevelDataEntry->fatigueExp * (UInt32)fatigue;
	//饥饿状态下经验减半
	if (GetHunger() <= 0 && exp > 1)
	{
		exp = exp / 2;
	}
	IncExp(exp);

	if (GetLevel() != oldLevel || GetExp() != oldExp)
	{
		SyncProperty();
	}
}

UInt32 Pet::Feed()
{
	return ErrorCode::SUCCESS;
}

UInt32 Pet::SetSkillIndex(UInt8 index)
{
	/*
	UInt32 countMax = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_PET_SELECT_SKILL_COUNT_MAX);
	if (m_SelectSkillCount >= countMax)
	{
		return ErrorCode::PET_SKILL_CHANGE_NUM_ERROR;
	}
	*/

	if (index == m_SkillIndex)
	{
		return ErrorCode::PET_SKILL_CHANGE_DUPLICATION_ERROR;
	}

	UInt32 itemId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_PET_SELECT_SKILL_ITEM);
	UInt32 itemNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_PET_SELECT_SKILL_ITEM_NUM);

	if (m_SelectSkillCount > 0)
	{
		if (GetOwner()->GetItemNum(itemId) < itemNum)
		{
			return ErrorCode::PET_SKILL_CHANGE_ITEM_NUM;
		}

		GetOwner()->RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_PET_SELECT_SKILL, GetID(), m_DataId).c_str(), itemId, itemNum);
	}

	m_SkillIndex = index;
	m_SelectSkillCount = m_SelectSkillCount + 1;
	return ErrorCode::SUCCESS;
}

void Pet::IncExp(UInt32 exp)
{
	UInt16 oldLevel = m_Level;
	UInt32 oldExp = m_Exp;

	if (m_pLevelDataEntry->upLevelExp == 0) return;

	m_Exp = m_Exp + exp;

	UpLevel();

	if (oldLevel != m_Level)
	{
		GetOwner()->OnPetUpLevel(GetType(), GetQuality(), m_Level);
	}

	InfoLogStream << PLAYERINFO(GetOwner()) << "Pet(" << GetID() << ") IncExp(" << exp << ") Level(" << oldLevel << "->" << m_Level << ") Exp(" << oldExp << "->" << m_Exp << ")" << LogStream::eos;
}

void Pet::ConsumeHunger(UInt16 value)
{
	UInt16 oldHunger = m_Hunger;
	if (m_Hunger >= value)
		m_Hunger = m_Hunger - value;
	else
		m_Hunger = 0;

	InfoLogStream << PLAYERINFO(GetOwner()) << "Pet(" << GetID() << ") ConsumeHunger(" << value << ") Hunger(" << oldHunger << "->" << m_Hunger << ")" << LogStream::eos;
}

void Pet::AddHunger(UInt16 value)
{
	UInt16 oldHunger = m_Hunger;
	UInt16 maxHunger = (UInt16)SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_PET_HUNGER_MAX_NUM);
	if (m_Hunger + value > maxHunger)
		m_Hunger = maxHunger;
	else
		m_Hunger = m_Hunger + value;

	InfoLogStream << PLAYERINFO(GetOwner()) << "Pet(" << GetID() << ") AddHunger(" << value << ") Hunger(" << oldHunger << "->" << m_Hunger << ")" << LogStream::eos;
}

bool Pet::IsFullHunger()
{
	UInt16 maxHunger = (UInt16)SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_PET_HUNGER_MAX_NUM);
	return m_Hunger >= maxHunger;
}

void Pet::ResetFeedCount()
{
	//m_GoldFeedCount.SetDBValue(0);
	//m_PointFeedCount.SetDBValue(0);
	m_GoldFeedCount = 0;
	m_PointFeedCount = 0;
}

void Pet::UpLevel()
{
	while (m_pLevelDataEntry->upLevelExp != 0 && m_Exp >= m_pLevelDataEntry->upLevelExp)
	{
		PetLevelDataEntry* nextDataEntry = PetLevelDataEntryMgr::Instance()->FindEntry(CL_MAKE_PET_LEVEL_KEY(m_Level + 1, m_pDataEntry->quality));
		if (nextDataEntry == NULL) return;

		m_Exp = m_Exp - m_pLevelDataEntry->upLevelExp;
		m_Level = m_Level + 1;

		m_pLevelDataEntry = nextDataEntry;
		if (m_pLevelDataEntry->upLevelExp == 0) m_Exp = 0;

		//计算评分
		CalcPetScore();
		GetOwner()->CalcRoleValueScore();
	}
}

void Pet::CalcPetScore()
{
	if (!SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_ADVENTURE_TEAM_SORTLIST))
	{
		return;
	}

	UInt32 scoreVar = 0;
	if (GetDataEntry() && GetDataEntry()->maxLevel > 0)
	{
		scoreVar = GetDataEntry()->maxLevelScore * (UInt32)GetLevel() / (UInt32)GetDataEntry()->maxLevel;
	}
	SetPetScore(scoreVar);
}

void Pet::InsertPet()
{
	SetID(CLRecordClient::Instance()->GenGuid());
	CLInsertCommand* insertCommand = CLRecordClient::Instance()->PrepareInsertCommand(GetOwner()->GetSrcZoneId(), "t_pet", GetID());
	insertCommand->PutData("owner", GetOwner()->GetID());
	insertCommand->PutData("create_time", UInt32(CURRENT_TIME.Sec()));
	this->GetDBData(insertCommand, false);
	CLRecordClient::Instance()->SendCommand(insertCommand);
}

void Pet::DeletePet()
{
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand(GetOwner()->GetSrcZoneId(), "t_pet", GetID());
	CLRecordClient::Instance()->SendCommand(cmd);
}

void Pet::WatchData(Avalon::NetOutputStream& stream)
{
	stream & GetID() & GetDataId() & GetLevel() & GetExp() & GetSkillIndex();
}

void Pet::SyncToDB(bool bFlush)
{
	//同步到数据库
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand(GetOwner()->GetSrcZoneId(), "t_pet", GetID(), bFlush);
	GetDBData(cmd, true);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void Pet::SyncProperty(bool bDirty)
{
	//同步到玩家
	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::SCENE_SYNC_PET);

	Avalon::NetOutputStream stream(packet->GetBuffer() + packet->GetSize(), packet->GetMaxSize() - packet->GetSize());
	try
	{
		stream & GetID() & GetDataId();
		Encode(stream, SST_SELF, bDirty);
	}
	catch (const Avalon::NetException& exception)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " Pet(" << GetID() << ") encode pet failed:" << exception.what() << "!" << LogStream::eos;
		Avalon::Packet::Destroy(packet);
		return;
	}
	packet->SetSize(UInt16(stream.GetPos()));
	GetOwner()->SendPacket(packet);

	Avalon::Packet::Destroy(packet);

	SyncToDB(false);
}
