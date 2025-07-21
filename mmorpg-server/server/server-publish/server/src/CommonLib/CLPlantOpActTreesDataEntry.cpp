#include "CLPlantOpActTreesDataEntry.h"

bool PlantOpActTreesDataEntry::Read(Avalon::DataRow& row)
{
	id = row.ReadInt32();
	
	{
		std::string str = row.ReadString();
		std::string split1 = "|";
		std::vector<std::string> ss;
		Avalon::StringUtil::Split(str, ss, split1);
		if (ss.size() != 2)
		{
			return false;
		}
		for (UInt32 i = 0; i < ss.size(); ++i)
		{
			proficSect.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(ss[i]));
		}
	}

	{
		std::string str = row.ReadString();
		std::string split1 = "|";
		std::string split2 = ",";
		std::vector<std::string> ss;
		Avalon::StringUtil::Split(str, ss, split1);
		if (ss.size() == 0)
		{
			return false;
		}
		for (auto treeItmstr : ss)
		{
			std::vector<std::string> sss;
			Avalon::StringUtil::Split(treeItmstr, sss, split2);
			if (sss.size() != 3)
			{
				return false;
			}
			TreeItem treeItem;
			treeItem.itemId = Avalon::StringUtil::ConvertToValue<UInt32>(sss[0]);
			treeItem.profic = Avalon::StringUtil::ConvertToValue<UInt32>(sss[1]);
			treeItem.weight = Avalon::StringUtil::ConvertToValue<UInt32>(sss[2]);
			trees.push_back(treeItem);
		}
	}
	return true;
}

TreeItem*  PlantOpActTreesDataEntryMgr::RandomOneTreeItemByProf(UInt32	prof)
{
	class GetDataEntryVisitor : public PlantOpActTreesDataEntryVisitor
	{
	public:
		GetDataEntryVisitor(UInt32 prof) : m_prof(prof), m_data(NULL) {}
		~GetDataEntryVisitor() {}
		virtual bool operator()(PlantOpActTreesDataEntry* dataEntry)
		{
			if (m_prof >= dataEntry->proficSect[0] && m_prof <= dataEntry->proficSect[1])
			{
				m_data = dataEntry;
				return false;
			}
			return true;
		}
	public:
		UInt32 m_prof;
		PlantOpActTreesDataEntry* m_data;
	};

	GetDataEntryVisitor visitor(prof);
	PlantOpActTreesDataEntryMgr::Instance()->Visit(visitor);

	if (visitor.m_data == NULL)
	{
		return NULL;
	}

	if (visitor.m_data->trees.size() == 0)
	{
		return NULL;
	}

	UInt32 totalWeight = 0;
	for (UInt32 i = 0; i < visitor.m_data->trees.size(); ++i)
	{
		totalWeight += visitor.m_data->trees[i].weight;
	}
	UInt32 randWeight = Avalon::Random::RandBetween(1, totalWeight);

	for (UInt32 i = 0; i < visitor.m_data->trees.size(); ++i)
	{
		if (visitor.m_data->trees[i].weight >= randWeight)
		{
			return &(visitor.m_data->trees[i]);
		}
		else
		{
			randWeight -= visitor.m_data->trees[i].weight;
		}
	}
	return NULL;
}
