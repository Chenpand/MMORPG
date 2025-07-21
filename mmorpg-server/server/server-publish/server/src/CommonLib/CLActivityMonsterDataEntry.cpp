#include "CLActivityMonsterDataEntry.h"

bool ActivityMonsterDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	name = row.ReadString();

	// 开始日期
	{
		std::string str = row.ReadString();
		std::vector<std::string> params;
		Avalon::StringUtil::Split(str, params, ".");
		if (params.size() != 3)
		{
			return false;
		}

		Avalon::Date date;
		date.Year(Avalon::StringUtil::ConvertToValue<UInt32>(params[0]));
		date.Month(Avalon::StringUtil::ConvertToValue<UInt32>(params[1]));
		date.Day(Avalon::StringUtil::ConvertToValue<UInt32>(params[2]));
		date.Hour(0);
		date.Min(0);
		date.Sec(0);

		startDate = (UInt32)date.ToTime().Sec();
	}

	// 结束日期
	{
		std::string str = row.ReadString();
		std::vector<std::string> params;
		Avalon::StringUtil::Split(str, params, ".");
		if (params.size() != 3)
		{
			return false;
		}

		Avalon::Date date;
		date.Year(Avalon::StringUtil::ConvertToValue<UInt32>(params[0]));
		date.Month(Avalon::StringUtil::ConvertToValue<UInt32>(params[1]));
		date.Day(Avalon::StringUtil::ConvertToValue<UInt32>(params[2]));
		date.Hour(0);
		date.Min(0);
		date.Sec(0);

		endDate = (UInt32)date.ToTime().Sec();
	}

	// 每日开启时间
	{
		std::string str = row.ReadString();
		std::vector<std::string> strs;
		Avalon::StringUtil::Split(str, strs, "-");
		if (strs.size() != 2)
		{
			return false;
		}

		for (int i = 0; i < 2; i++)
		{
			std::vector<std::string> params;
			Avalon::StringUtil::Split(strs[i], params, ":");
			if (params.size() != 2)
			{
				return false;
			}

			if (i == 0)
			{
				openTime.startHour = Avalon::StringUtil::ConvertToValue<UInt32>(params[0]);
				openTime.startMin = Avalon::StringUtil::ConvertToValue<UInt32>(params[1]);
			}
			else
			{
				openTime.endHour = Avalon::StringUtil::ConvertToValue<UInt32>(params[0]);
				openTime.endMin = Avalon::StringUtil::ConvertToValue<UInt32>(params[1]);
			}
		}
	}

	perRollMonsterNum = row.ReadUInt32();
	perRollDurningSec = row.ReadUInt32() * Avalon::Time::SECS_OF_HOUR;
	pointType = (DungeonAreaPointType)row.ReadUInt32();
	groupId = row.ReadUInt32();
	prob = row.ReadUInt32();

	// 掉落信息
	{
		UInt32 minLevel = 1;
		std::string str = row.ReadString();
		std::vector<std::string> params;
		Avalon::StringUtil::Split(str, params, "|");

		for (auto& param : params)
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(param, strs, ":");
			if (strs.size() != 2)
			{
				return false;
			}

			UInt16 level = Avalon::StringUtil::ConvertToValue<UInt16>(strs[0]);
			UInt32 dropSetId = Avalon::StringUtil::ConvertToValue<UInt32>(strs[1]);

			ActivityMonsterDropInfo info;
			info.minLevel = minLevel;
			info.maxLevel = level;
			info.dropSetId = dropSetId;

			drops.push_back(info);
			minLevel = level + 1;
		}
	}

	// 可能掉落
	{
		std::string str = row.ReadString();
		std::vector<std::string> params;
		Avalon::StringUtil::Split(str, params, "|");
		for (auto& param : params)
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(param, strs, ":");
			if (strs.size() < 1)
			{
				return false;
			}

			UInt32 id = Avalon::StringUtil::ConvertToValue<UInt32>(strs[0]);
			if (id == 0)
			{
				return false;
			}

			UInt32 num = 1;
			if (strs.size() > 1)
			{
				num = Avalon::StringUtil::ConvertToValue<UInt32>(strs[1]);
			}

			DropItem item;
			item.typeId = id;
			item.num = num;
			mayDropItems.push_back(item);
		}
	}

	startNotice = row.ReadUInt32();
	killNotice = row.ReadUInt32();
	clearNotice = row.ReadUInt32();
	overNotice = row.ReadUInt32();
	needDungeonLevel = row.ReadUInt32();

	return true;
}