#include "CLCityMonsterDataEntry.h"

bool CityMonsterGenerateDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	monsterType = (CityMonsterType)row.ReadUInt8();
	{
		auto str = row.ReadString();
		if (str != "-")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "|");
			for (auto& param : strs)
			{
				auto sceneId = Avalon::StringUtil::ConvertToValue<UInt32>(param);
				if (sceneId > 0)
				{
					sceneIds.push_back(sceneId);
				}
			}
		}
	}
	
	posGroupId = row.ReadUInt32();
	minNum = row.ReadUInt32();
	maxNum = row.ReadUInt32();

	{
		auto str = row.ReadString();
		if (str != "-")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "|");
			for (auto& param : strs)
			{
				std::vector<std::string> params;
				Avalon::StringUtil::Split(param, params, ":");
				if (params.size() < 2)
				{
					ErrorLogStream << "CityMonster Table " << id << " monster list invalid." << LogStream::eos;
					return false;
				}

				CityMonsterElem elem;
				elem.sceneId = Avalon::StringUtil::ConvertToValue<UInt32>(params[0]);
				elem.monsterId = Avalon::StringUtil::ConvertToValue<UInt32>(params[1]);
				if (params.size() > 2)
				{
					elem.prob = Avalon::StringUtil::ConvertToValue<UInt32>(params[2]);
				}
				else
				{
					elem.prob = 1;
				}
				
				if (elem.monsterId == 0 || elem.prob == 0)
				{
					ErrorLogStream << "CityMonster Table " << id << " monster list invalid." << LogStream::eos;
					return false;
				}

				monsterList.push_back(elem);
			}
		}
	}
	
	dungeonId = row.ReadUInt32();

	return true;
}

UInt32 CityMonsterGenerateDataEntry::RandMonster(UInt32 sceneId)
{
	if (monsterList.empty())
	{
		return 0;
	}

	UInt32 total = 0;
	for (auto& monster : monsterList)
	{
		if (monster.sceneId == sceneId)
		{
			total += monster.prob;
		}
		
	}

	if (total == 0)
	{
		return 0;
	}

	UInt32 prob = Avalon::Random::RandBetween(0, total);
	for (auto& monster : monsterList)
	{
		if (monster.sceneId != sceneId)
		{
			continue;
		}

		if (prob < monster.prob)
		{
			return monster.monsterId;
		}

		prob -= monster.prob;
	}

	return monsterList[monsterList.size() - 1].monsterId;
}

UInt32 CityMonsterGenerateDataEntry::RandScene()
{
	if (sceneIds.empty())
	{
		return 0;
	}

	UInt32 index = Avalon::Random::RandBetween(0, sceneIds.size() - 1);
	if (index >= sceneIds.size())
	{
		return 0;
	}

	return sceneIds[index];
}

/*CityMonsterGenerateDataEntry* CityMonsterGenerateDataEntryMgr::FindMonsterSolutionByTypeAndScene(CityMonsterType type, UInt32 sceneId)
{
	class CityMonsterVistor : public Avalon::DataEntryVisitor<CityMonsterGenerateDataEntry>
	{
	public:
		CityMonsterVistor(CityMonsterType type, UInt32 sceneId) : type_(type), sceneId_(sceneId){};
		bool operator()(CityMonsterGenerateDataEntry* dataEntry)
		{
			if (dataEntry && dataEntry->monsterType == type_ && dataEntry->sceneId == sceneId_)
			{
				datas.push_back(dataEntry);
				return false;
			}

			return true;
		}

		CityMonsterType type_;
		UInt32 sceneId_;
		std::vector<CityMonsterGenerateDataEntry*> datas;
	};

	CityMonsterVistor vistor(type, sceneId);
	Visit(vistor);
	
	if (vistor.datas.empty())
	{
		return NULL;
	}
	return vistor.datas[0];
}*/

std::vector<CityMonsterGenerateDataEntry*> CityMonsterGenerateDataEntryMgr::FindMonsterSolutionByType(CityMonsterType type)
{
	class CityMonsterVistor : public Avalon::DataEntryVisitor<CityMonsterGenerateDataEntry>
	{
	public:
		CityMonsterVistor(CityMonsterType type) : type_(type){};
		bool operator()(CityMonsterGenerateDataEntry* dataEntry)
		{
			if (dataEntry && dataEntry->monsterType == type_)
			{
				datas.push_back(dataEntry);
				return true;
			}

			return true;
		}

		CityMonsterType type_;
		std::vector<CityMonsterGenerateDataEntry*> datas;
	};

	CityMonsterVistor vistor(type);
	Visit(vistor);

	return vistor.datas;
}

std::vector<CityMonsterGenerateDataEntry*> CityMonsterGenerateDataEntryMgr::FindMonsterSolutionByTypeAndSceneId(CityMonsterType type, UInt32 sceneId)
{
	class CityMonsterVistor : public Avalon::DataEntryVisitor<CityMonsterGenerateDataEntry>
	{
	public:
		CityMonsterVistor(CityMonsterType type, UInt32 sceneId) : type_(type), sceneId_(sceneId){};
		bool operator()(CityMonsterGenerateDataEntry* dataEntry)
		{
			if (!dataEntry || dataEntry->sceneIds.size() != 1)
			{
				return true;
			}
			if (dataEntry->monsterType == type_ && dataEntry->sceneIds[0] == sceneId_)
			{
				datas_.push_back(dataEntry);
				return true;
			}

			return true;
		}

		CityMonsterType					type_;
		UInt32							sceneId_;
		std::vector<CityMonsterGenerateDataEntry*> datas_;
	};

	CityMonsterVistor vistor(type, sceneId);
	Visit(vistor);

	return vistor.datas_;
}

/////////////////////////////////////////////////////////////////////////
//						城镇刷怪位置表								/////
/////////////////////////////////////////////////////////////////////////

bool CityMonsterPositionDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	sceneId = row.ReadUInt32();
	groupId = row.ReadUInt32();

	{
		auto str = row.ReadString();
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(str, strs, ",");
		if (strs.size() != 2)
		{
			ErrorLogStream << "city monster position(" << id << ") invalid pos(" << str << ")." << LogStream::eos;
			return false;
		}

		pos.x = (UInt32)(Avalon::StringUtil::ConvertToValue<float>(strs[0]) * ScenePos::SCALE_TO_FLOAT);
		pos.y = (UInt32)(Avalon::StringUtil::ConvertToValue<float>(strs[1]) * ScenePos::SCALE_TO_FLOAT);
	}

	return true;
}

bool CityMonsterPositionDataEntryMgr::AddEntry(CityMonsterPositionDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<CityMonsterPositionDataEntry>::AddEntry(entry))
	{
		return false;
	}

	auto key = std::make_pair(entry->sceneId, entry->groupId);
	m_MapPos[key].insert(entry->pos);
	return true;
}

std::set<NpcPos> CityMonsterPositionDataEntryMgr::GetAllPos(UInt32 sceneId, UInt32 groupId)
{
	auto key = std::make_pair(sceneId, groupId);
	
	return m_MapPos[key];
}