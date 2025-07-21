#include "CLActivitySuitAdditionDataEntry.h"

bool ActivitySuitAdditionDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	type = (ActivityMonsterAdditionType)row.ReadUInt32();

	// 装备列表
	{
		std::string str = row.ReadString();
		std::vector<std::string> params;
		Avalon::StringUtil::Split(str, params, "|");
		for (auto& param : params)
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(param, strs, ",");
			std::vector<UInt32> suit;
			for (auto& suitStr : strs)
			{
				UInt32 equipId = Avalon::StringUtil::ConvertToValue<UInt32>(suitStr);
				if (equipId)
				{
					suit.push_back(equipId);
				}
				else
				{
					return false;
				}
			}
			if (!suit.empty())
			{
				equipList.push_back(suit);
			}
		}
	}

	// 活动怪物列表
	{
		std::string str = row.ReadString();
		std::vector<std::string> params;
		Avalon::StringUtil::Split(str, params, "|");
		for (auto& param : params)
		{
			UInt32 monsterId = Avalon::StringUtil::ConvertToValue<UInt32>(param);
			if (monsterId)
			{
				activityMonsterList.push_back(monsterId);
			}
			else
			{
				return false;
			}
		}
	}

	// 翻倍掉落物
	{
		std::string str = row.ReadString();
		std::vector<std::string> params;
		Avalon::StringUtil::Split(str, params, "|");
		for (auto& param : params)
		{
			UInt32 dropItem = Avalon::StringUtil::ConvertToValue<UInt32>(param);
			if (dropItem)
			{
				doubleDropItems.insert(dropItem);
			}
			else
			{
				return false;
			}
		}
	}

	doubleDropRate = row.ReadUInt32();

	// 遇怪概率
	for (UInt32 i = 0; i < sizeof(monsterRateAddition) / sizeof(monsterRateAddition[0]); i++)
	{
		monsterRateAddition[i] = row.ReadUInt32();
	}

	return true;
}