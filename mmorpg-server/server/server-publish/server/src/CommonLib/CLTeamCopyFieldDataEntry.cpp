/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLTeamCopyFieldDataEntry.h"

// 自定义头文件
// __CUSTOM_HEADER_BEGIN__
#include <CLTeamCopyDefine.h>
#include <CLTeamCopyTargetDataEntry.h>
// __CUSTOM_HEADER_END__

TeamCopyFieldDataEntry::TeamCopyFieldDataEntry()
{
// __CUSTOM_ENTRYCONSTRUCTOR_BEGIN__
// __CUSTOM_ENTRYCONSTRUCTOR_END__
}

TeamCopyFieldDataEntry::~TeamCopyFieldDataEntry()
{
// __CUSTOM_ENTRYDESTRUCTOR_BEGIN__
// __CUSTOM_ENTRYDESTRUCTOR_END__
}

UInt32 TeamCopyFieldDataEntry::GetKey() const
{
// __CUSTOM_GETKEY_BEGIN__
	return id;
// __CUSTOM_GETKEY_END__
}

bool TeamCopyFieldDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	teamGrade = row.ReadUInt32();
	dungeonId = row.ReadUInt32();
	presentedType = row.ReadUInt32();
	defeatCond = row.ReadUInt32();
	rebornTime = row.ReadUInt32();
	{
		auto str = row.ReadString();
		if (str != "-")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "|");
			for (auto& param : strs)
			{
				auto value = Avalon::StringUtil::ConvertToValue<UInt32>(param);
				if (value > 0) unlockedCond.push_back(value);
			}
		}
	}
	appearStatus = row.ReadUInt32();
	{
		auto str = row.ReadString();
		if (str != "-")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "|");
			for (auto& param : strs)
			{
				updateFieldStatus.push_back(param);
			}
		}
	}

// __CUSTOM_READ_BEGIN__

	for (auto& it : updateFieldStatus)
	{
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(it, strs, "_");

		if(strs.size() != 2)
			continue;

		FieldStatusUpdateCfg cfg;
		cfg.fieldId = Avalon::StringUtil::ConvertToValue<UInt32>(strs[0]);
		cfg.status = Avalon::StringUtil::ConvertToValue<UInt32>(strs[1]);

		updateFieldStatusVec.push_back(cfg);
	}

// __CUSTOM_READ_END__
	return true;
}

// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

bool TeamCopyFieldDataEntryMgr::AddEntry(TeamCopyFieldDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<TeamCopyFieldDataEntry>::AddEntry(entry))
	{
		return false;
	}
// __CUSTOM_ADDENTRY_BEGIN__

	if (entry->presentedType == TEAM_COPY_APPEAR_BOSS)
	{
		m_BossFieldMap[entry->teamGrade] = (entry->id);
	}

// __CUSTOM_ADDENTRY_END__
	return true;
}

// __CUSTOM_MGRFUNCTIONDEFINE_BEGIN__

void TeamCopyFieldDataEntryMgr::GetFieldList(UInt32 teamGrade, UInt32 appearType, std::vector<TeamCopyFieldDataEntry*>& fieldVec)
{
	class FieldEntryVisitor : public Avalon::DataEntryVisitor<TeamCopyFieldDataEntry>
	{
	public:
		FieldEntryVisitor(UInt32 _teamGrade, UInt32 _appear, std::vector<TeamCopyFieldDataEntry*>& _fieldVec) 
			: m_TeamGrade(_teamGrade), m_AppearType(_appear), m_FieldVec(_fieldVec) {}

		bool operator()(TeamCopyFieldDataEntry* dataEntry)
		{
			if (NULL == dataEntry) return false;

			if ( !TeamCopyTargetDataEntryMgr::Instance()->IsHasFieldByStage(m_TeamGrade, dataEntry->id))
			{
				return true;
			}

			if (m_AppearType == dataEntry->presentedType)
			{
				m_FieldVec.push_back(dataEntry);
			}

			return true;
		}

	private:
		UInt32 m_TeamGrade;
		UInt32 m_AppearType;
		std::vector<TeamCopyFieldDataEntry*>& m_FieldVec;
	};

	FieldEntryVisitor vistor(teamGrade, appearType, fieldVec);
	Visit(vistor);
}

void TeamCopyFieldDataEntryMgr::GetUnlockFieldList(UInt32 fieldId, std::vector<TeamCopyFieldDataEntry*>& fieldVec)
{
	class FieldEntryVisitor : public Avalon::DataEntryVisitor<TeamCopyFieldDataEntry>
	{
	public:
		FieldEntryVisitor(UInt32 _fieldId, std::vector<TeamCopyFieldDataEntry*>& _fieldVec)
			: m_FieldId(_fieldId), m_FieldVec(_fieldVec) {}

		bool operator()(TeamCopyFieldDataEntry* dataEntry)
		{
			if (NULL == dataEntry) return false;

			for (auto& iter : dataEntry->unlockedCond)
			{
				if (iter == m_FieldId)
				{
					m_FieldVec.push_back(dataEntry);
					return true;
				}
			}

			return true;
		}

	private:
		UInt32 m_FieldId;
		std::vector<TeamCopyFieldDataEntry*>& m_FieldVec;
	};

	FieldEntryVisitor vistor(fieldId, fieldVec);
	Visit(vistor);
}

UInt32 TeamCopyFieldDataEntryMgr::GetBossFieldId(UInt32 teamGrade)
{
	auto iter = m_BossFieldMap.find(teamGrade);
	return iter != m_BossFieldMap.end() ? iter->second : 0;
}

// __CUSTOM_MGRFUNCTIONDEFINE_END__

