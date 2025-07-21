#include "QuickBar.h"
#include "Player.h"
#include "SkillMgr.h"


SkillBar::SkillBar()
{
	m_MaxBarSize = SKILLBAR_NUM;
	m_MaxGridSize = PlayerDataEntryMgr::Instance()->GetMaxSkillGridSize();
	m_SkillBars.resize(m_MaxBarSize);
	for (UInt32 i = 0; i < m_MaxBarSize; ++i)
	{
		m_SkillBars[i].index = i + 1;
		m_SkillBars[i].grids.resize(m_MaxGridSize);
	}
	m_BarIndex = 0;
	m_SkillConfigType = SkillConfigType::SKILL_CONFIG_PVE;
}

SkillBar::~SkillBar() { }

bool SkillBar::IsExist(UInt16 id)
{
	for (UInt32 i = 0; i < m_MaxGridSize; ++i)
	{
		if (id != 0 && m_SkillBars[m_BarIndex].grids[i].id == id)
		{
			return true;
		}
	}
	return false;
}

UInt8 SkillBar::GetSlot(UInt16 id)
{
	for (UInt32 i = 0; i < m_MaxGridSize; ++i)
	{
		if (id != 0 && m_SkillBars[m_BarIndex].grids[i].id == id)
		{
			return (UInt8)m_SkillBars[m_BarIndex].grids[i].slot;
		}
	}
	return (UInt8)0;
}

void SkillBar::SetGrid(UInt8 slot, UInt16 id)
{
	if (slot > m_MaxGridSize) return;
	for (UInt32 i = 0; i < m_MaxGridSize; i++)
	{
		if (i != (UInt32)(slot) && m_SkillBars[m_BarIndex].grids[i].id == id)
		{
			return;
		}
	}

	m_SkillBars[m_BarIndex].grids[slot - 1].id = id;
	SetDirty();
}

void SkillBar::SetBarIndex(UInt32 barIndex)
{
	if (barIndex < m_MaxBarSize)
	{
		m_BarIndex = barIndex;
	}
}

UInt32 SkillBar::PushGrid(UInt8 barIndex, SkillIdsVec ids)
{
	if (barIndex > m_MaxBarSize || barIndex < SETTING_MIX) return ErrorCode::SETTING_INDEX_ERROR;
	SkillBarInfo skillBar = m_SkillBars[barIndex - 1];

	UInt32 size = ids.size();
	UInt32 index = 0;
	for (UInt32 i = 0; i < m_MaxGridSize; ++i)
	{
		if (index >= size) break;
		if (skillBar.grids[i].id == 0)
		{
			skillBar.grids[i].id = ids[index];
			skillBar.grids[i].slot = i + 1;
			index++;
		}
	}
	if (index == 0) return ErrorCode::SUCCESS;
	UInt32 nRes = CheckBar(skillBar);
	if (nRes == ErrorCode::SUCCESS)
	{
		m_SkillBars[barIndex - 1] = skillBar;
		SetDirty();
	}
	else
	{
		ErrorLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName()
			<< ") checkbar is invalid errorcode " << nRes << LogStream::eos;
	}
	return nRes;
}

UInt32 SkillBar::PushGrid(SkillIdsVec ids)
{
	UInt32 nRes = ErrorCode::SUCCESS;
	for (UInt32 i = 0; i < m_MaxBarSize; ++i)
	{
		nRes = PushGrid(i + 1, ids);
		if (nRes != ErrorCode::SUCCESS) return nRes;
	}
	return nRes;
}

UInt32 SkillBar::PushFixGrid(UInt8 barIndex, SkillGridVec skillGirdVec)
{
	if (skillGirdVec.size() == 0)
		return ErrorCode::SUCCESS;

	if (barIndex > m_MaxBarSize || barIndex < SETTING_MIX) return ErrorCode::SETTING_INDEX_ERROR;
	SkillBarInfo skillBar = m_SkillBars[barIndex - 1];
	
	if (skillGirdVec.size() > skillBar.grids.size())
	{
		return ErrorCode::SETTING_ERROR;
	}

	for (UInt32 i = 0; i < skillGirdVec.size();++i)
	{
		UInt8 slot = skillGirdVec[i].slot;
		if (slot == 0 || slot > skillBar.grids.size())
			continue;
		
		// 如果原来槽位有技能则先不卸下
		if(skillBar.grids[slot - 1].id != 0)
			continue;

		skillBar.grids[slot - 1].id = skillGirdVec[i].id;
		skillBar.grids[slot - 1].slot = skillGirdVec[i].slot;
	}

	UInt32 nRes = CheckBar(skillBar);
	if (nRes == ErrorCode::SUCCESS)
	{
		m_SkillBars[barIndex - 1] = skillBar;
		SetDirty();
	}
	else
	{
		ErrorLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName()
			<< ")  PushFixGrid error." << nRes << LogStream::eos;
	}

	return nRes;
}

bool SkillBar::CheckRecommendGrid(UInt8 barIndex, SkillGridVec skillGirdVec)
{
	if (skillGirdVec.size() == 0)
		return true;

	if (barIndex > m_MaxBarSize || barIndex < SETTING_MIX) return false;
	SkillBarInfo skillBar = m_SkillBars[barIndex - 1];

	for (UInt32 i = 0; i < skillBar.grids.size(); ++i)
	{
		for (UInt32 j = 0; j < skillGirdVec.size(); ++j)
		{
			if (skillBar.grids[i].slot == skillGirdVec[j].slot)
			{
				if (skillBar.grids[i].id != skillGirdVec[j].id)
					return false;

				break;
			}
		}
	}

	return true;
}

void SkillBar::ClearEntry(UInt16 id)
{
	if (id == 0) return;
	for (UInt32 i = 0; i < m_MaxBarSize; ++i)
	{
		for (UInt32 j = 0; j < m_MaxGridSize;j++)
		{
			if (m_SkillBars[i].grids[j].id == id)
			{
				m_SkillBars[i].grids[j].id = 0;
			}
		}
	}
}

void SkillBar::ClearAllEntry()
{
	bool isChange = false;

	for (UInt32 i = 0; i < m_MaxBarSize; ++i)
	{
		for (UInt32 j = 0; j < m_MaxGridSize; ++j)
		{
			if (m_SkillBars[i].grids[j].id != 0)
			{
				m_SkillBars[i].grids[j].id = 0;
				isChange = true;
			}
		}
	}
	if (isChange)
	{
		SetDirty();
	}
}

SkillMgr& SkillBar::GetSkillMgr()
{
	if (m_SkillConfigType == SkillConfigType::SKILL_CONFIG_PVP)
	{
		return GetOwner()->GetPvpSkillMgr();
	}
	else
	{
		return GetOwner()->GetSkillMgr();
	}
}


UInt32 SkillBar::SetBars(UInt8 index, std::vector<SkillBarInfo> tbars, bool isContinue)
{
	if(index < SETTING_MIX || index > m_MaxBarSize) return ErrorCode::SETTING_INDEX_ERROR;

	bool isChange = false;

	std::vector<UInt16> preSkillIds = GetOwner()->GetPreOccuSkillId();

	SkillBarVec tmpBars = m_SkillBars;
	for (UInt32 i = 0; i < tbars.size(); ++i)
	{
		//检查tbar.index
		UInt32 barIndex = (UInt32)tbars[i].index;
		if (barIndex > m_MaxBarSize || barIndex < SETTING_MIX) return ErrorCode::SETTING_INDEX_ERROR;
		barIndex = barIndex - 1;
		UInt32 barSize = tbars[i].grids.size();
		for (UInt32 grid = 0; grid < barSize; ++grid)
		{
			UInt32 slot = (UInt32)tbars[i].grids[grid].slot;

			if (slot > m_MaxGridSize || slot == 0)
			{
				ErrorLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName()
					<< ") skillid(" << tbars[i].grids[grid].id << ") slot(" << slot << ") or grid(" << barIndex << ") exceed max size" << LogStream::eos;

				if (!isContinue)
				{
					return ErrorCode::SETTING_SLOT_ERROR;
				}
			}
			else
			{
				//检查是否有预转职技能
				for (UInt32 j = 0; j < preSkillIds.size(); ++j)
				{
					if (preSkillIds[j] == tbars[i].grids[grid].id || preSkillIds[j] == tmpBars[barIndex].grids[slot - 1].id)
						return ErrorCode::SETTING_SKILL_PRE_OCCU_ERROR;
				}

				tmpBars[barIndex].grids[slot - 1].id = tbars[i].grids[grid].id;
				tmpBars[barIndex].grids[slot - 1].slot = tbars[i].grids[grid].slot;
				isChange = true;
			}
		}
	}

	UInt32 nRes = ErrorCode::SUCCESS;
	if (isChange)
	{
		nRes = CheckBars(tmpBars);
	}

	if (nRes == ErrorCode::SUCCESS)
	{
		m_SkillBars = tmpBars;
		m_BarIndex = index - 1;
		SetDirty();

		if(!isChange)
		{
			GetOwner()->SendNotify(1083, index);
		}

		DebugLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName()
			<< ") SetBars success! " << LogStream::eos;

	}
	else
	{
		ErrorLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName()
			<< ") checkbar is invalid errorcode " << nRes << LogStream::eos;
		
	}
	return nRes;
}

void SkillBar::Init()
{
	if (m_SkillBars[m_BarIndex].grids.size() >= GetOwner()->GetSkillGridSize())
		m_MaxGridSize = GetOwner()->GetSkillGridSize();
}

void SkillBar::OnLevelup()
{
	if (m_SkillBars[m_BarIndex].grids.size() >= GetOwner()->GetSkillGridSize())
		m_MaxGridSize = GetOwner()->GetSkillGridSize();
}

void SkillBar::OnNewSkill(UInt32 skillId, bool isSetting)
{
	if (isSetting == false) return;
	SkillIdsVec ids;
	ids.push_back(skillId);
	PushGrid(m_BarIndex + 1, ids);
}

UInt32 SkillBar::CheckBar(SkillBarInfo skillBar)
{
	UInt32 barIndex = (UInt32)skillBar.index - 1;
	SkillBarInfo oldBar = m_SkillBars[barIndex];
	for (UInt32 i = 0; i < m_MaxGridSize - 1; ++i)
	{
		UInt16 skillId = skillBar.grids[i].id;
		if (skillId == 0) continue;
		for (UInt32 j = i + 1; j < m_MaxGridSize; ++j)
		{
			if (skillId == skillBar.grids[j].id)
			{
				return ErrorCode::SETTING_SKILL_REPEAT;
			}
		}

		Skill* skill = GetSkillMgr().FindSkill(skillId);
		if (skill == NULL || !GetSkillMgr().IsSetting(skill->dataEntry))
		{
			return ErrorCode::SETTING_SKILL_ERROR;
		}
	}
	return ErrorCode::SUCCESS;
}

UInt32 SkillBar::CheckBars(SkillBarVec skillBars)
{
	UInt32 nRes = ErrorCode::SUCCESS;
	for (UInt32 i = 0; i < m_MaxBarSize; ++i)
	{
		nRes = CheckBar(skillBars[i]);
		if (nRes != ErrorCode::SUCCESS)
		{
			return nRes;
		}
	}
	return nRes;
}

SkillBar::SkillBarVec SkillBar::GetExistSkillBar()
{
	SkillBarVec tmpSkillBars;
	for (UInt32 i = 0; i < m_MaxBarSize; ++i)
	{
		SkillBarInfo tmpSkillBar;
		tmpSkillBar.index = i + 1;
		for (UInt32 j = 0; j < m_MaxGridSize; ++j)
		{
			if (m_SkillBars[i].grids[j].id != 0)
			{
				SkillBarGrid grid;
				grid.id = m_SkillBars[i].grids[j].id;
				grid.slot = m_SkillBars[i].grids[j].slot;
				tmpSkillBar.grids.push_back(grid);
			}
		}
		tmpSkillBars.push_back(tmpSkillBar);
	}

	return tmpSkillBars;
}

void SkillBar::SetExistSkillBar(SkillBarVec skillBars)
{
	for (UInt32 i = 0; i < skillBars.size(); ++i)
	{
		UInt8 index = skillBars[i].index == 0 ? 0 : skillBars[i].index - 1;
		for (UInt32 j = 0; j < skillBars[i].grids.size(); ++j)
		{
			UInt8 slot = skillBars[i].grids[j].slot == 0 ? 0 : skillBars[i].grids[j].slot - 1;
			m_SkillBars[index].grids[slot].id = skillBars[i].grids[j].id;
			m_SkillBars[index].grids[slot].slot = skillBars[i].grids[j].slot;
		}
	}
}

UInt32 SkillBar::GetFreeGridSize()
{
	UInt32 result = 0;
	for (UInt32 i = 0; i < m_MaxGridSize; ++i)
	{
		if (m_SkillBars[m_BarIndex].grids[i].id == 0)
			result += 1;
	}
	return result;
}


void SkillBar::ClearNotValidEntry()
{
	bool isChange = false;
	for (UInt32 i = 0; i < m_MaxBarSize; i++)
	{
		for (UInt32 j = 0; j < m_MaxGridSize; j++)
		{
			UInt16 skillId = m_SkillBars[i].grids[j].id;
			//检查技能是否存在
			if (skillId != 0 && GetSkillMgr().FindSkill(skillId) == NULL)
			{
				m_SkillBars[i].grids[j].id = 0;
				isChange = true;
			}
		}
	}
	if (isChange)
	{
		SetDirty();
	}
}

void SkillBar::Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty)
{
	std::vector<SkillBarInfo> infos = GetExistSkillBar();
	UInt8 index = (UInt8)(m_BarIndex + 1);
	stream & index & infos;
}

void SkillBar::Decode(Avalon::NetInputStream& stream)
{
	UInt8 index;
	std::vector<SkillBarInfo> skillBars;
	stream & index & skillBars;
	m_BarIndex = index == 0 ? 0 : index - 1;
	SetExistSkillBar(skillBars);
}

void SkillBar::DecodeString(std::istringstream& is)
{
	char split;
	UInt32 id;

	if (is.str().empty()) return;

	is >> m_BarIndex >> split;
	if (split != '|') return;

	for (UInt32 i = 0; i < m_MaxBarSize; ++i)
	{
		m_SkillBars[i].index = i + 1;
		for (UInt32 j = 0; j < m_MaxGridSize; ++j)
		{
			is >> id >> split;
			m_SkillBars[i].grids[j].id = id;
			m_SkillBars[i].grids[j].slot = j + 1;
		}
		is >> split;
	}
}

void SkillBar::EncodeString(std::ostringstream& os)
{
	os << m_BarIndex << '|';

	for (UInt32 i = 0; i < m_SkillBars.size(); ++i)
	{
		for (UInt32 j = 0; j < m_SkillBars[i].grids.size(); ++j)
		{
			os << m_SkillBars[i].grids[j].id << ';';
		}
		os << '|';
	}
}

/*
void ItemBar::Init()
{
	m_MaxGridSize = GetOwner()->GetItemGridSize();
}

void ItemBar::OnLevelup()
{
	m_MaxGridSize = GetOwner()->GetItemGridSize();
}

bool ItemBar::CheckBar(IdsVec tbar)
{
	for (UInt32 i = 0; i < m_MaxSize; i++)
	{
		for (UInt32 j = i + 1; j < m_MaxSize; j++)
		{
			//检查是否有重复的物品.
			if (tbar[i] == tbar[j] && tbar[i] != 0)
			{
				return false;
			}
		}
		//应该检查物品是否存在玩家背包里.
	}
	return true;
}

void ItemBar::ClearNotValidEntry()
{

}
*/

WeaponBar::WeaponBar()
{
	m_MaxGridSize = WEAPON_GRID_NUM;
	m_Weapons.resize(m_MaxGridSize);
}

WeaponBar::~WeaponBar()
{

}

bool WeaponBar::IsInvalid()
{
	for (UInt32 i = 0; i < m_MaxGridSize; ++i)
	{
		if (m_Weapons[i] != 0)
		{
			return false;
		}
	}
	return true;
}

bool WeaponBar::IsExist(ObjID_t id)
{
	for (UInt32 i = 0; i < m_MaxGridSize; ++i)
	{
		if (m_Weapons[i] == id)
		{
			return true;
		}
	}
	return false;
}

UInt32 WeaponBar::SetBarById(ObjID_t oldId, ObjID_t newId)
{
	for (UInt32 i = 0; i < m_MaxGridSize; ++i)
	{
		if (m_Weapons[i] == oldId)
		{
			m_Weapons[i] = newId;
			SetDirty();
			return ErrorCode::SUCCESS;
		}
	}
	return ErrorCode::WEAPON_SETTING_NOT_FOUNT;
}

UInt32 WeaponBar::SetBarByIndex(UInt32 index, ObjID_t newId)
{
	if (index >= m_MaxGridSize) return ErrorCode::WEAPON_SETTING_INDEX_ERROR;

	m_Weapons[index] = newId;
	SetDirty();
	return ErrorCode::SUCCESS;
}

void WeaponBar::Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty)
{
	stream & m_Weapons;
}

void WeaponBar::Decode(Avalon::NetInputStream& stream)
{
	std::vector<ObjID_t> weaponBar;
	stream & weaponBar;
	m_Weapons = weaponBar;
}

void WeaponBar::DecodeString(std::istringstream& is)
{
	char split;
	ObjID_t id = 0;
	UInt32 index = 0;
	if (is.str().empty()) return;

	std::vector<ObjID_t> weaponBar;

	while (index < m_MaxGridSize)
	{
		index++;
		is >> id;
		weaponBar.push_back(id);
		is >> split;
	}

	m_Weapons = weaponBar;
}

void WeaponBar::EncodeString(std::ostringstream& os)
{
	for (UInt32 i = 0; i < m_Weapons.size(); ++i)
	{
		os << m_Weapons[i] << '|';
	}
}


