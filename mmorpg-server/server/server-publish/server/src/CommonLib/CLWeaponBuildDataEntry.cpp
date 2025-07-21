/**********************************************************************************

   注意:
           1. _CUSTOM_*_BEGIN和_CUSTOM_*_END之间的代码是可以手动修改的。
           2. 其他地方的代码都不要改动!!!!

*********************************************************************************/
#include "CLWeaponBuildDataEntry.h"



WeaponBuildCostDataEntry::WeaponBuildCostDataEntry()
{

}

WeaponBuildCostDataEntry::~WeaponBuildCostDataEntry()
{

}

UInt32 WeaponBuildCostDataEntry::GetKey() const
{
	return level;
}

bool WeaponBuildCostDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	level = row.ReadUInt32();

	{
		auto str = row.ReadString();
		if (str != "-")
		{
			std::string split1 = "|";
			std::string split2 = "_";
			std::vector<std::string> stdRewardVec;
			Avalon::StringUtil::Split(str, stdRewardVec, split1);
			for (UInt32 i = 0; i < stdRewardVec.size(); ++i)
			{
				std::vector<std::string> strTmp;
				Avalon::StringUtil::Split(stdRewardVec[i], strTmp, split2);
				ItemReward reward;
				if (strTmp.size() == 2)
				{
					reward.id = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[0]);
					reward.num = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[1]);
				}
				else
				{
					continue;
				}
				mustItem.push_back(reward);
			}
		}
	}

	{
		auto str = row.ReadString();
		if (str != "-")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "_");
			if (strs.size() == 2)
			{
				unnecessaryItem_1.id = Avalon::StringUtil::ConvertToValue<UInt32>(strs[0]);
				unnecessaryItem_1.num = Avalon::StringUtil::ConvertToValue<UInt32>(strs[1]);
			}
		}
	}

	{
		auto str = row.ReadString();
		if (str != "-")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "_");
			if (strs.size() == 2)
			{
				unnecessaryItem_2.id = Avalon::StringUtil::ConvertToValue<UInt32>(strs[0]);
				unnecessaryItem_2.num = Avalon::StringUtil::ConvertToValue<UInt32>(strs[1]);
			}
		}
	}


	return true;
}


bool WeaponBuildCostDataEntryMgr::AddEntry(WeaponBuildCostDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<WeaponBuildCostDataEntry>::AddEntry(entry))
	{
		return false;
	}

	return true;
}


WeaponBuildPropertyPoolDataEntry::WeaponBuildPropertyPoolDataEntry()
{

}

WeaponBuildPropertyPoolDataEntry::~WeaponBuildPropertyPoolDataEntry()
{

}

UInt32 WeaponBuildPropertyPoolDataEntry::GetKey() const
{
	return id;
}

bool WeaponBuildPropertyPoolDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	mainPropertyType = static_cast<MainPropertyType>(row.ReadUInt32());
	propType = static_cast<ItemKeyValuesKeyType>(row.ReadUInt32());

	{
		auto str = row.ReadString();
		if (str != "-")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "|");
			if (strs.size() == 1)
			{
				propertyNum.push_back(Avalon::StringUtil::ConvertToValue<Int32>(strs[0]));
			}
			else if (str.size() == 2)
			{
				propertyNum.push_back(Avalon::StringUtil::ConvertToValue<Int32>(strs[0]));
				propertyNum.push_back(Avalon::StringUtil::ConvertToValue<Int32>(strs[1]));
			}
			else
			{
				return false;
			}
		}
	}

	special = row.ReadUInt32();
	special_detail = row.ReadUInt32();
	buffinfo_PVE = row.ReadUInt32();
	buffinfo_PVP = row.ReadUInt32();
	mechanism_PVE = row.ReadUInt32();
	mechanism_PVP = row.ReadUInt32();
	num = row.ReadUInt32();

	return true;
}



bool WeaponBuildPropertyPoolDataEntryMgr::AddEntry(WeaponBuildPropertyPoolDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<WeaponBuildPropertyPoolDataEntry>::AddEntry(entry))
	{
		return false;
	}

	return true;
}


WeaponBuildRandomPoolDataEntry::WeaponBuildRandomPoolDataEntry()
{

}

WeaponBuildRandomPoolDataEntry::~WeaponBuildRandomPoolDataEntry()
{

}

UInt32 WeaponBuildRandomPoolDataEntry::GetKey() const
{
	return id;
}

bool WeaponBuildRandomPoolDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	GroupID = row.ReadUInt32();


	//{
	//	auto str = row.ReadString();
	//	if (str != "-")
	//	{
	//		std::vector<std::string> strs;
	//		Avalon::StringUtil::Split(str, strs, "|");
	//		for (auto& param : strs)
	//		{
	//			WeaponBuildData data;
	//			data.id = Avalon::StringUtil::ConvertToValue<UInt32>(param);
	//			chooseNumSet.push_back(data);
	//		}
	//	}
	//}

	chooseNum = row.ReadUInt32();

 	//{
 	//	auto str = row.ReadString();
 	//	if (str != "-")
 	//	{
 	//		std::vector<std::string> strs;
 	//		Avalon::StringUtil::Split(str, strs, "|");
 	//		for (UInt32 i = 0; i < strs.size(); ++i)
 	//		{
 	//			chooseNumSet[i].num = (Avalon::StringUtil::ConvertToValue<UInt32>(strs[i]));
 	//			totalWeight += chooseNumSet[i].num;
 	//		}
 	//	}
 	//}

// 	{
// 		auto str = row.ReadString();
// 		if (str != "-")
// 		{
// 			std::string split1 = "|";
// 			std::string split2 = "_";
// 			std::vector<std::string> vec;
// 			Avalon::StringUtil::Split(str, vec, split1);
// 			for (UInt32 i = 0; i < vec.size(); ++i)
// 			{
// 				std::vector<std::string> strTmp;
// 				Avalon::StringUtil::Split(vec[i], strTmp, split2);
// 				WeaponBuildData data;
// 				if (strTmp.size() == 2)
// 				{
// 					data.id = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[0]);
// 					data.num = Avalon::StringUtil::ConvertToValue<UInt32>(strTmp[1]);
// 					totalWeight += data.num;
// 				}
// 				else
// 				{
// 					continue;
// 				}
// 				chooseNumSet.push_back(data);
// 			}
// 		}
// 	}

	dataType = row.ReadUInt32();
	dataID = row.ReadUInt32();
	dataProb = row.ReadUInt32();
	dataNum = row.ReadUInt32();
	randDataType = row.ReadUInt32();

	{
		auto str = row.ReadString();
		if (str != "-")
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(str, strs, "|");
			for (UInt32 i = 0; i < strs.size(); ++i)
			{
				weaponTypeSet.insert(Avalon::StringUtil::ConvertToValue<UInt32>(strs[i]));
			}
		}
	}

	return true;
}


bool WeaponBuildRandomPoolDataEntryMgr::AddEntry(WeaponBuildRandomPoolDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<WeaponBuildRandomPoolDataEntry>::AddEntry(entry))
	{
		return false;
	}
	
	m_DataMap[entry->GroupID].push_back(*entry);

	return true;
}

void WeaponBuildRandomPoolDataEntryMgr::Init()
{
	class WeaponBuildRandomPoolVisiter : public Avalon::DataEntryVisitor<WeaponBuildRandomPoolDataEntry>
	{
	public:
		bool operator()(WeaponBuildRandomPoolDataEntry *dataEntry)
		{
			if (dataEntry == NULL)
			{
				return false;
			}

			datas.push_back(dataEntry);
			return true;
		}

		std::vector<WeaponBuildRandomPoolDataEntry*> datas;
	};

	WeaponBuildRandomPoolVisiter vister;
	WeaponBuildRandomPoolDataEntryMgr::Instance()->Visit(vister);

	for (auto data : vister.datas)
	{
		//m_DataMap[data->GroupID].push_back(data);
	}
}

void WeaponBuildRandomPoolDataEntryMgr::GenProperty(UInt32 id, std::vector<UInt32>& vec, WeaponBuildRandomPoolContext& context)
{
	if (m_DataMap.count(id) == 0)
	{
		return;
	}
	if (m_DataMap[id].empty())
	{
		return;
	}

	auto& data = m_DataMap[id][0];
	UInt32 num = data.chooseNum;
	if (num == 0)
	{
		return;
	}

	auto tempDataVec = m_DataMap[id];

	for (auto i = tempDataVec.begin(); i < tempDataVec.end(); )
	{
		if (i->weaponLevel != context.weaponLevel || i->randDataType != context.type || i->weaponTypeSet.count(context.weaponType) == 0)
		{
			i = tempDataVec.erase(i);
		}
		else
		{
			++i;
		}
	}


	for (UInt32 i = 0; i < num; ++ i)
	{
		UInt32 totalWeight = 0;
		for (auto& tempData : tempDataVec)
		{
			totalWeight += tempData.dataProb;
		}
		UInt32 rnd = Avalon::Random::RandBetween(1, totalWeight);
		WeaponBuildRandomPoolDataEntry temp;
		for (auto j = tempDataVec.begin(); j < tempDataVec.end(); ++j)
		{
			if (rnd > j->dataProb)
			{
				rnd -= j->dataProb;
			}
			else
			{
				temp = *j;
				tempDataVec.erase(j);
				break;
			}
		}
		if (temp.dataID == 0)
		{
			continue;
		}
		while (temp.dataNum--)
		{
			if (temp.dataType == WBRPT_RANDOM_POOL)
			{
				GenProperty(temp.dataID, vec, context);
			}
			else if (temp.dataType == WBRPT_PROPERTY_POOL)
			{
				vec.push_back(temp.dataID);
			}
		}
	}
}


WeaponBuildOccuDataEntry::WeaponBuildOccuDataEntry()
{

}

WeaponBuildOccuDataEntry::~WeaponBuildOccuDataEntry()
{

}

UInt32 WeaponBuildOccuDataEntry::GetKey() const
{
	return id;
}

bool WeaponBuildOccuDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();

	yellowItemID = row.ReadUInt32();
	silverItemID = row.ReadUInt32();

	naturePropertyType = row.ReadUInt32();
	quality = row.ReadUInt32();

	itemThirdType = (ItemThirdType)row.ReadUInt32();
	level = row.ReadUInt32();

	return true;
}



bool WeaponBuildDataEntryMgr::AddEntry(WeaponBuildOccuDataEntry* entry)
{
	if (!Avalon::DataEntryMgrBase<WeaponBuildOccuDataEntry>::AddEntry(entry))
	{
		return false;
	}

	return true;
}

std::pair<UInt32, UInt32> WeaponBuildDataEntryMgr::FindItemID(UInt32 quality, UInt32 thirdType, UInt32 needLevel, UInt32 naturePropertyType)
{
	class Vistor : public Avalon::DataEntryVisitor<WeaponBuildOccuDataEntry>
	{
	private:
		UInt32 m_Quality;
		UInt32 m_ThirdType;
		UInt32 m_NeedLevel;
		UInt32 m_NaturePropertyType;
		std::pair<UInt32, UInt32>& m_Pair;
	public:
		Vistor(Int32 quality, UInt32 thirdType, UInt32 needLevel, UInt32 naturePropertyType, std::pair<UInt32, UInt32>& pair) :
			m_Quality(quality), m_ThirdType(thirdType), m_NeedLevel(needLevel), m_NaturePropertyType(naturePropertyType), m_Pair(pair){};
		virtual bool operator()(WeaponBuildOccuDataEntry* dataEntry)
		{
			if (dataEntry->quality == m_Quality && dataEntry->itemThirdType == m_ThirdType && dataEntry->level == m_NeedLevel && dataEntry->naturePropertyType == m_NaturePropertyType)
			{
				m_Pair.first = dataEntry->yellowItemID;
				m_Pair.second = dataEntry->silverItemID;
			}
			return true;
		}
	};
	std::pair<UInt32, UInt32> pair;
	Vistor vistor(quality, thirdType, needLevel, naturePropertyType, pair);
	WeaponBuildDataEntryMgr::Instance()->Visit(vistor);
	return pair;
}

bool IsAttackProp(ItemKeyValuesKeyType prop)
{
	switch (prop)
	{
	case IRP_PHYATK:
	case IRP_MAGATK:
	case IRP_INDEPEND_ATK:
		return true;
		break;
	default:
		break;
	}
	return false;
}

bool IsFourDimensionalProp(ItemKeyValuesKeyType prop)
{
	switch (prop)
	{
	case IRP_STR:		//力量
	case IRP_STA:		//体力
	case IRP_INT:		//智力
	case IRP_SPR:		//精神
		return true;
		break;
	default:
		break;
	}
	return false;
}

bool IsDefProp(ItemKeyValuesKeyType prop)
{
	switch (prop)
	{
	case IKVKT_WBP_PHYDEF:		
	case IKVKT_WBP_MAGDEF:		
		return true;
		break;
	default:
		break;
	}
	return false;
}

WeaponBuildQualityRandomDataEntry::WeaponBuildQualityRandomDataEntry()
{
}

WeaponBuildQualityRandomDataEntry::~WeaponBuildQualityRandomDataEntry()
{
}

UInt32 WeaponBuildQualityRandomDataEntry::GetKey() const
{
	return id;
}

bool WeaponBuildQualityRandomDataEntry::Read(Avalon::DataRow & row)
{
	id = row.ReadUInt32();
	weaponLevel = row.ReadUInt32();
	grade = row.ReadUInt32();
	classWeighted = row.ReadUInt32();
	classAssitanceWeighted = row.ReadUInt32();
	groupID = row.ReadUInt32();
	return true;
}

bool WeaponBuildQualityRandomDataEntryMgr::AddEntry(WeaponBuildQualityRandomDataEntry * entry)
{
	if (!Avalon::DataEntryMgrBase<WeaponBuildQualityRandomDataEntry>::AddEntry(entry))
	{
		return false;
	}

	m_TotalWeightMap[entry->weaponLevel] += entry->classWeighted;
	m_ExtraTotalWeightMap[entry->weaponLevel] += entry->classAssitanceWeighted;
	m_LevelGradeMap[entry->weaponLevel].push_back(entry);

	return true;
}

std::pair<UInt32, UInt32> WeaponBuildQualityRandomDataEntryMgr::GetRandomGradeAndGroupID(UInt32 weaponLevel, bool useExtra)
{
	UInt32 total = 0;
	if (useExtra)
	{
		total = m_ExtraTotalWeightMap[weaponLevel];
	}
	else
	{
		total = m_TotalWeightMap[weaponLevel];
	}
	UInt32 rnd = Avalon::Random::RandBetween(1, total);
	
	for (auto grade : m_LevelGradeMap[weaponLevel])
	{
		if (useExtra)
		{
			if (rnd <= grade->classAssitanceWeighted)
			{
				return { grade->grade, grade->groupID };
			}
			rnd -= grade->classAssitanceWeighted;
		}
		else
		{
			if (rnd <= grade->classWeighted)
			{
				return { grade->grade, grade->groupID };
			}
			rnd -= grade->classWeighted;
		}
	}
	return std::pair<UInt32, UInt32>(0, 0);
}
