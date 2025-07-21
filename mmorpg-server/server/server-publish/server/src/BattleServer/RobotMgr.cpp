#include "RobotMgr.h"
#include <CLRobotDataEntry.h>
#include <CLItemDataEntry.h>

RobotMgr::RobotMgr()
{
	m_SpecifyRobotOccu = 0;
	m_SpecifyRobotAI = ROBOT_AI_INVALID;
	m_SpecifyRobotHard = ROBOT_ATTR_INVALID;
}

bool RobotMgr::Init()
{
	class RobotDataVistor : public Avalon::DataEntryVisitor<RobotDataEntry>
	{
	public:
		bool operator()(RobotDataEntry *dataEntry)
		{
			allData.push_back(dataEntry);
			return true;
		}
		std::vector<RobotDataEntry*> allData;
	};

	RobotDataVistor vistor;
	RobotDataEntryMgr::Instance()->Visit(vistor);

	for (auto data : vistor.allData)
	{
		RacePlayerInfo raceInfo;
		raceInfo.name = data->name;
		raceInfo.occu = data->occu;
		raceInfo.level = data->level;

		// 装备
		std::vector<std::string> equipStrs;
		Avalon::StringUtil::Split(data->equips, equipStrs, "|");
		for (const auto& str : equipStrs)
		{
			UInt32 equipId = Avalon::StringUtil::ConvertToValue<UInt32>(str);
			auto equip = _GenEquip(equipId);
			if (equip.id == 0)
			{
				continue;
			}

			raceInfo.equips.push_back(equip);
		}

		// 技能
		std::vector<std::string> skillStrs;
		Avalon::StringUtil::Split(data->skills, skillStrs, "|");
		UInt8 skillSlot = 1;
		for (const auto& skillStr : skillStrs)
		{
			std::vector<std::string> strs;
			Avalon::StringUtil::Split(skillStr, strs, ":");
			if (strs.size() != 2)
			{
				ErrorLogStream << "error robot config:" << data->name << LogStream::eos;
				return false;
			}

			RaceSkillInfo skill;
			skill.id = Avalon::StringUtil::ConvertToValue<UInt16>(strs[0]);
			skill.level = Avalon::StringUtil::ConvertToValue<UInt8>(strs[1]);
			skill.slot = skillSlot++;
			raceInfo.skills.push_back(skill);
		}

		Robot* robot = new Robot();
		robot->hardType = (RobotAttrHardType)data->hard;
		robot->raceInfo = raceInfo;

		// 机器人的查找范围是机器人等级的前5级到当前等级
		for (UInt16 level = data->level - 4; level <= data->level; level++)
		{
			RobotKey key;
			key.level = level;
			key.hardType = robot->hardType;
			m_Robots[key].push_back(robot);
		}
	}

	return true;
}

Robot* RobotMgr::GenRobot(RobotAttrHardType hardType, UInt16 level)
{

#ifdef _DEBUG
	// 指定了机器人的属性
	if (GetSepcifyRobotHard() > ROBOT_ATTR_INVALID)
	{
		hardType = GetSepcifyRobotHard();
	}
#endif

	RobotKey key;
	key.hardType = hardType;
	key.level = level;

	auto itr = m_Robots.find(key);
	if (itr == m_Robots.end())
	{
		return NULL;
	}

	auto& robots = itr->second;
	if (robots.empty())
	{
		return NULL;
	}

#ifdef _DEBUG
	// 指定了机器人职业
	if (GetSepcifyRobotOccu() > 0)
	{
		std::vector<Robot*> specRobots;
		for (auto robot : robots)
		{
			if (robot->raceInfo.occu == GetSepcifyRobotOccu())
			{
				specRobots.push_back(robot);
			}
		}

		if (specRobots.empty())
		{
			return NULL;
		}

		return specRobots[Avalon::Random::RandBetween(0, specRobots.size() - 1)];
	}

#endif

	return robots[Avalon::Random::RandBetween(0, robots.size() - 1)];
}

void RobotMgr::GenRobotEquipStrengthen(Robot* robot)
{
	if (!robot)
	{
		return;
	}

	for (auto& equip : robot->raceInfo.equips)
	{
		auto itemData = ItemDataEntryMgr::Instance()->FindEntry(equip.id);
		if (!itemData)
		{
			continue;
		}

		if (itemData->subType == ST_EQUIP_WEAPON)
		{
			// 武器
			if (robot->hardType == ROBOT_ATTR_EASY)
			{
				equip.strengthen = Avalon::Random::RandBetween(0, 5);
			}
			else if (robot->hardType == ROBOT_ATTR_NORMAL)
			{
				equip.strengthen = Avalon::Random::RandBetween(6, 10);
			}
			else
			{
				equip.strengthen = Avalon::Random::RandBetween(11, 13);
			}
		}
		else if (itemData->subType >= ST_EQUIP_HEAD && itemData->subType <= ST_EQUIP_BRACELET)
		{
			// 防具
			if (robot->hardType == ROBOT_ATTR_EASY)
			{
				equip.strengthen = 0;
			}
			else if (robot->hardType == ROBOT_ATTR_NORMAL)
			{
				equip.strengthen = Avalon::Random::RandBetween(0, 7);
			}
			else
			{
				equip.strengthen = Avalon::Random::RandBetween(8, 10);
			}
		}
	}
}

RaceEquip RobotMgr::_GenEquip(UInt32 itemId)
{
	RaceEquip equip;

	RandBaseDataEntry* randBasedata = RandBaseDataEntryMgr::Instance()->GetData();
	ItemDataEntry* itemData = ItemDataEntryMgr::Instance()->FindEntry(itemId);
	if (!randBasedata || !itemData)
	{
		return equip;
	}

	// 随机得到装备品级
	//Int32 qualityLv = EQL_HIGH;

	// 在品级区间内随机得到装备品级修正率
	//Int32 rate = Avalon::Random::RandBetween(randBasedata->QLOMin[qualityLv], randBasedata->QLOMax[qualityLv]);

	//高级品级装备区间为[61, 80]
	Int32 rate = Avalon::Random::RandBetween(61, 80);
	float baseRatio = (float)randBasedata->baseRatio / 100.00f;		// 基础百分比系数
	float qlRate = (float)rate / 100.00f;							// 品级修正系数

	if (itemData->attrId != 0)
	{
		EquipAttrDataEntry* attrData = EquipAttrDataMgr::Instance()->FindEntry(itemData->attrId);
		if (!attrData)
		{
			ErrorLogStream << "make item(" << itemData->id << ") failed." << "attr id(" << itemData->attrId << ") err!" << LogStream::eos;
			return equip;
		}

		// 计算装备实际基础值 公式: 最终值 = round（基础值*百分比a + 基础值*（1-百分比a）*品级修正）
		if (0 != attrData->atk)			equip.phyAtk = (UInt32)((attrData->atk * baseRatio + attrData->atk * (1 - baseRatio) * qlRate) + 0.5);
		if (0 != attrData->magicAtk)	equip.magAtk = (UInt32)((attrData->magicAtk * baseRatio + attrData->magicAtk * (1 - baseRatio) * qlRate) + 0.5);
		if (0 != attrData->def)			equip.phyDef = (UInt32)((attrData->def * baseRatio + attrData->def * (1 - baseRatio) * qlRate) + 0.5);
		if (0 != attrData->magicDef)	equip.magDef = (UInt32)((attrData->magicDef * baseRatio + attrData->magicDef * (1 - baseRatio) * qlRate) + 0.5);
		if (0 != attrData->strenth)		equip.strenth = (UInt32)((attrData->strenth * baseRatio + attrData->strenth * (1 - baseRatio) * qlRate) + 0.5);
		if (0 != attrData->stamina)		equip.stamina = (UInt32)((attrData->stamina * baseRatio + attrData->stamina * (1 - baseRatio) * qlRate) + 0.5);
		if (0 != attrData->spirit)		equip.spirit = (UInt32)((attrData->spirit * baseRatio + attrData->spirit * (1 - baseRatio) * qlRate) + 0.5);
		if (0 != attrData->intellect)	equip.intellect = (UInt32)((attrData->intellect * baseRatio + attrData->intellect * (1 - baseRatio) * qlRate) + 0.5);
	}

	equip.id = itemId;
	return equip;
}