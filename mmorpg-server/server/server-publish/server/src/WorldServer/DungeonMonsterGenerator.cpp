#include "DungeonMonsterGenerator.h"
#include <CLDungeonAreaDataEntry.h>
#include <CLMonsterDataEntry.h>
#include <CLMonsterPrefixDataEntry.h>
#include <CLMonsterGroupDataEntry.h>
#include <CLDungeonDestructTable.h>
#include "SysRecordMgr.h"
#include "WSApplication.h"

DungeonMonsterGenerator::DungeonMonsterGenerator()
{

}

DungeonMonsterGenerator::~DungeonMonsterGenerator()
{

}

bool DungeonMonsterGenerator::Init()
{
    class DungeonAreaInitVisitor : public DungeonAreaDataEntryVisitor
    {
    public:
        bool operator()(DungeonAreaDataEntry *dataEntry)
        {
            if (dataEntry == NULL)
            {
                return false;
            }

            datas.push_back(dataEntry);
            return true;
        }

        std::vector<DungeonAreaDataEntry*> datas;
    };

    DungeonAreaInitVisitor visitor;
    DungeonAreaDataEntryMgr::Instance()->Visit(visitor);

    for (auto data : visitor.datas)
    {
        if (!data)
        {
            continue;
        }

        DungeonAreaInfo* area = FindDungeonAreaInfo(data->areaId);
        if (!area)
        {
            area = new DungeonAreaInfo();
            area->id = data->areaId;

            m_areas[area->id] = area;
        }

        DungeonAreaPoint point;
        point.id = data->id;
		point.type = data->type;
        point.dataId = data->dataId;
        point.summonMonsterGroupId = data->summonMonsterGroupId;
        point.level = data->level;

        bool foundGroup = false;
        for (auto& group : area->groups)
        {
            if (group.id == data->groupId)
            {
                foundGroup = true;
                group.points.push_back(point);
            }
        }

        if (!foundGroup)
        {
            DungeonAreaPointGroup group;
            group.id = data->groupId;
            group.points.push_back(point);

            area->groups.push_back(group);
        }
    }

    if (!_InitMonsterSet())
    {
        return false;
    }

    if (!_InitMonsterPrefixSet())
    {
        return false;
    }

	// 检查深渊配置表是否正确
	class HellDungeonVisitor : public Avalon::DataEntryVisitor<DungeonDataEntry>
	{
	public:
		bool operator()(DungeonDataEntry *dataEntry)
		{
			if (dataEntry && dataEntry->IsHellDungeon())
			{
				hellLevels.insert(dataEntry->level);
			}

			return true;
		}
		std::set<UInt16> hellLevels;
	};
	HellDungeonVisitor vistor;
	bool error = false;
	DungeonDataEntryMgr::Instance()->Visit(vistor);
	for (auto hellLv : vistor.hellLevels)
	{
		DungeonMonsterGenerateContext ctx;
		ctx.dungeonLevel = hellLv;
		if (!_GenHellModeData(ctx))
		{
			ErrorLogStream << "gen hell mode with dungeon level(" << hellLv << ") failed." << LogStream::eos;
			error = true;
		}
	}

	if (error)
	{
		return false;
	}

    return true;
}

bool DungeonMonsterGenerator::_InitMonsterSet()
{
    class MonsterSetInitVisitor : public Avalon::DataEntryVisitor<MonsterGroupDataEntry>
    {
    public:
        bool operator()(MonsterGroupDataEntry *dataEntry)
        {
            datas.push_back(dataEntry);
            return true;
        }

        std::vector<MonsterGroupDataEntry*> datas;
    };

    MonsterSetInitVisitor vistor;
    MonsterGroupDataEntryMgr::Instance()->Visit(vistor);

	bool error = false;
    for (auto data : vistor.datas)
    {
        if (!data)
        {
            continue;
        }

		if (data->groupProb == 0)
		{
			ErrorLogStream << "MonsterGroup(" << data->id << ") groupProb == 0." << LogStream::eos;
			error = true;
			continue;
		}

        if (m_monsterSet.find(data->projID) == m_monsterSet.end())
        {
            MonsterSet monsterSet;
            monsterSet.id = data->projID;
            monsterSet.lossyType = data->lossyType;
            m_monsterSet[data->projID] = monsterSet;
        }

        MonsterGroupElem elem;
        elem.prob = data->prob;
        elem.lossyNum = data->lossyNum;
        elem.monsterList = data->monsterList;

        bool addGroup = true;
        for (auto& group : m_monsterSet[data->projID].groups)
        {
            if (group.id == data->groupID)
            {
                group.elems.push_back(elem);

				if (group.prob != data->groupProb)
				{
					ErrorLogStream << "MonsterGroup(" << data->id << ") groupProb not equal." << LogStream::eos;
					error = true;
				}

                addGroup = false;
            }
        }

        if (addGroup)
        {
            MonsterGroup group;
            group.id = data->groupID;
			group.prob = data->groupProb;
            group.elems.push_back(elem);
            m_monsterSet[data->projID].groups.push_back(group);
        }
    }

	if (error)
	{
		return false;
	}

    return true;
}

bool DungeonMonsterGenerator::_InitMonsterPrefixSet()
{
    class MonsterPrefixSetInitVisitor : public Avalon::DataEntryVisitor<MonsterPrefixDataEntry>
    {
    public:
        bool operator()(MonsterPrefixDataEntry *dataEntry)
        {
            datas.push_back(dataEntry);
            return true;
        }

        std::vector<MonsterPrefixDataEntry*> datas;
    };

    MonsterPrefixSetInitVisitor vistor;
    MonsterPrefixDataEntryMgr::Instance()->Visit(vistor);

    for (auto data : vistor.datas)
    {
        if (!data)
        {
            continue;
        }

        m_monsterPrefixSet[(MonsterPrefixType)data->type].push_back(data->id);
    }

    return true;
}

void DungeonMonsterGenerator::GenMonstersByArea(DungeonMonsterGenerateContext& context, UInt32 areaId, UInt16 adapterLevel, VDungeonMonster& monsters)
{
    context.areaLossyMonsterGroup.clear();
    context.adapterLevel = adapterLevel;

    DungeonAreaInfo* area = FindDungeonAreaInfo(areaId);
    if (!area)
    {
		ErrorLogStream << "can't find dungeon area(" << areaId << ")'s info." << LogStream::eos;
        return;
    }

    if (area->groups.empty())
    {
        ErrorLogStream << "generate monster for area(" << areaId << ") but monster group is empty." << LogStream::eos;
        return;
    }

    const auto& choosedGroup = area->groups[rand() % area->groups.size()];
    for (const auto& point : choosedGroup.points)
    {
        DungeonMonster monster;
        if (point.type == DAPT_MONSTER)
        {
            if (!_GenMonsterData(context, monster, areaId, 0, point.dataId, point.id, point.level))
            {
                continue;
            }

            if (point.summonMonsterGroupId)
            {
                monster.willSummonMonster = GenMonstersFromSet(monster.id, context, point.id, point.level, point.summonMonsterGroupId, monsters);
            }

            monsters.push_back(monster);
        }
        else if (point.type == DAPT_DESTRUCT)
        {
            auto data = DungeonDestructDataEntryMgr::Instance()->FindEntry(point.dataId);
            if (!data)
            {
                ErrorLogStream << "generate destruct for area(" << areaId << ") but can't find destruct(" << point.dataId << ")'s data." << LogStream::eos;
                continue;
            }

            monster.id = context.GenMonsterID();
            monster.pointId = point.id;
            monster.level = 0;
            monster.type = MT_BOX;
            monster.typeId = point.dataId;
            monster.dropSet = data->dropSetIds;

            if (point.summonMonsterGroupId)
            {
                monster.willSummonMonster = GenMonstersFromSet(monster.id, context, point.id, point.level, point.summonMonsterGroupId, monsters);
            }

            monsters.push_back(monster);
        }
		else if (point.type == DAPT_MONSTER_GROUP)
        {
            if (!GenMonstersFromSet(0, context, point.id, point.level, point.dataId, monsters))
            {
                ErrorLogStream << "generate monster for area(" << areaId << ") by monster set(" << point.dataId << ") failed." << LogStream::eos;
                continue;
            }
        }
		else if (point.type < DAPT_NUM)
		{
			// 活动怪
			auto& group = context.activityMonsterGroups[point.type];
			UInt32 groupId = group.groupId;
			UInt32 curNum = SysRecordMgr::Instance()->GetRecordValue(SYS_BEHAVIOR_DAILY_ACTIVITY_MONSTER, SYS_BEHAVIOR_OWNER_ROLE, 0, groupId);
			if (groupId && Avalon::Random::SelectInHundred(group.prob) && (curNum < group.remainNum))
			{
				VDungeonMonster activityMonsters;
				if (!GenMonstersFromSet(0, context, point.id, context.dungeonLevel, groupId, activityMonsters))
				{
					ErrorLogStream << "generate monster for area(" << areaId << ") by monster set(" << point.dataId << ") failed." << LogStream::eos;
				}
				else
				{
					SysRecordMgr::Instance()->UpdateRecord(SYS_BEHAVIOR_DAILY_ACTIVITY_MONSTER, SYS_BEHAVIOR_OWNER_ROLE, 0, groupId, context.playerNum);
				}

				for (auto& monster : activityMonsters)
				{
					monster.activityMonsterId = group.activityId;
					monsters.push_back(monster);
				}

				continue;
			}
		}
    }
}

bool DungeonMonsterGenerator::GenHellMonsters(DungeonMonsterGenerateContext& context, VDungeonMonster& monsters)
{
    auto data = _GenHellModeData(context);
    if (!data)
    {
        return false;
    }

    context.hellMode = data->mode;
    for (UInt32 wave = 0; wave < data->monsterGroups.size(); ++wave)
    {
        auto id = data->monsterGroups[wave];
        VDungeonMonster tmpMonsters;
        if (id > 0 && !GenMonstersFromSet(0, context, 0, context.hellMonsterLevel, id, tmpMonsters))
        {
            ErrorLogStream << "generate hell monster by set(" << id << ") failed." << LogStream::eos;
            continue;
        }

        for (auto& monster : tmpMonsters)
        {
            monster.isHell = true;
            monster.hellWave = (UInt8)wave;
            monsters.push_back(monster);
        }
    }
    
    return true;
}

std::vector<UInt8> DungeonMonsterGenerator::GenMonsterPrefix(UInt32 lowPrefixNum, UInt32 seniorPrefixNum)
{
    std::vector<UInt8> prefixes;

    {
        std::vector<UInt8> tmpPrefixes = _GenMonsterPrefix(MPT_LOW, lowPrefixNum);
        for (auto prefix : tmpPrefixes)
        {
            prefixes.push_back(prefix);
        }
    }

    {
        std::vector<UInt8> tmpPrefixes = _GenMonsterPrefix(MPT_SENIOR, seniorPrefixNum);
        for (auto prefix : tmpPrefixes)
        {
            prefixes.push_back(prefix);
        }
    }

    return prefixes;
}

DungeonAreaInfo* DungeonMonsterGenerator::FindDungeonAreaInfo(UInt32 areaId)
{
    auto itr = m_areas.find(areaId);
    return itr == m_areas.end() ? NULL : itr->second;
}

bool DungeonMonsterGenerator::GenMonstersFromSet(UInt32 summonerId, DungeonMonsterGenerateContext& context, UInt32 pointId, UInt16 pointLevel, UInt32 id, VDungeonMonster& monsters)
{
    // 从刷怪表中进行刷怪
    auto itr = m_monsterSet.find(id);
    if (itr == m_monsterSet.end())
    {
        ErrorLogStream << "try to generate monster from dungeon set(" << id << ") failed, it is empty." << LogStream::eos;
        return false;
    }

    auto& monsterSet = itr->second;

    // 先看看怪物组是不是已经随机出来了
    MonsterGroup* group = context.FindMonsterGroup(id);
    if (!group)
    {
        // 从刷怪组中随机刷怪组
        
        if (monsterSet.groups.empty())
        {
            ErrorLogStream << "try to generate monster from dungeon set(" << id << ") failed, group is empty." << LogStream::eos;
            return false;
        }

		UInt32 totalProb = 0;
		MonsterGroup* pGroup = NULL;
		for (auto group : monsterSet.groups)
		{
			totalProb += group.prob;
		}

		if (totalProb != 0)
		{
			UInt32 randProb = Avalon::Random::RandBetween(1, totalProb);
			for (UInt32 i = 0; i < monsterSet.groups.size(); i++)
			{
				auto& group = monsterSet.groups[i];
				if (randProb <= group.prob)
				{
					pGroup = &group;
					break;
				}

				randProb -= group.prob;
			}
		}
		
		if (pGroup == NULL)
		{
			pGroup = &(monsterSet.groups[_Rand() % monsterSet.groups.size()]);
		}

		if (pGroup == NULL)
		{
			ErrorLogStream << "try to generate monster from dungeon set(" << id << ") failed, gen failed." << LogStream::eos;
			return false;
		}

        group = context.AddMonsterGroup(monsterSet.lossyType, id, *pGroup);
        if (!group)
        {
            group = pGroup;
        }
    }

    UInt32 totalProb = 0;
    for (auto elem : group->elems)
    {
        totalProb += elem.prob;  
    }

    if (totalProb == 0)
    {
        ErrorLogStream << "try to generate monster from dungeon set(" << id << ") group(" << group->id << ") failed, total probablity is 0." << LogStream::eos;
        return false;
    }

    bool ret = false;
    UInt32 randProb = _Rand() % totalProb;
    for (UInt32 i = 0; i < group->elems.size(); i++)
    {
        auto& elem = group->elems[i];
       
        if (randProb > elem.prob && i != group->elems.size() - 1)
        {
            randProb -= elem.prob;
            continue;
        }

        for (auto monsterId : elem.monsterList)
        {
            DungeonMonster monster;
            if (_GenMonsterData(context, monster, id, summonerId, monsterId, pointId, pointLevel))
            {
				monster.typeId = monster.typeId / 10 * 10 + context.hardType + 1;
				monster.monsterId = monsterId;
                monsters.push_back(monster);
                ret = true;
            }
        }

        if (monsterSet.lossyType != MGLT_NULL)
        {
            elem.lossyNum--;
            if (elem.lossyNum == 0)
            {
                group->elems.erase(group->elems.begin() + i);
            }
        }
        break;
    }

    return ret;
}

UInt16 DungeonMonsterGenerator::GenMonsterLevel(DungeonMonsterGenerateContext& context, UInt16 pointLevel)
{
    if (context.adapterLevel > 0)
    {
        return context.adapterLevel;
    }

    return pointLevel;
}

UInt32 DungeonMonsterGenerator::GenMonsterExp(DungeonMonsterGenerateContext& context, DungeonMonster& monster)
{
	monster.attrDataEntry = MonsterAttributeDataEntryMgr::Instance()->FindEntry(MonsterAttributeDataEntry::MakeKey(context.hardType + 1, monster.type, monster.mode, monster.level));
    if (monster.attrDataEntry)
    {
        monster.exp = monster.attrDataEntry->exp;
    }

	// 如果是死亡之塔，经验为0
	if (context.dungeonData && context.dungeonData->subType == DST_SIWANGZHITA)
	{
		monster.exp = 0;
	}

    return monster.exp;
}

UInt32 DungeonMonsterGenerator::_Rand()
{
    return rand();
}

MonsterAttributeDataEntry* DungeonMonsterGenerator::_GetMonsterAttributeDataEntry(DungeonHardType hardType, MonsterType type, UInt16 level)
{
    UInt32 key = ((hardType + 1) << 24) | (type << 16) | level;
    return MonsterAttributeDataEntryMgr::Instance()->FindEntry(key);
}

std::vector<UInt8> DungeonMonsterGenerator::_GenMonsterPrefix(MonsterPrefixType type, UInt32 num)
{
    std::vector<UInt8> prefixes;

    auto itr = m_monsterPrefixSet.find(type);
    if (itr == m_monsterPrefixSet.end())
    {
        return prefixes;
    }

    auto& prefixSet = itr->second;
    while (num > 0 && !prefixSet.empty())
    {
        num--;
        UInt32 randIndex = _Rand() % prefixSet.size();
        prefixes.push_back(prefixSet[randIndex]);
        prefixSet.erase(prefixSet.begin() + randIndex);
    }

    // 再放回去
    for (auto prefix : prefixes)
    {
        prefixSet.push_back(prefix);
    }

    return prefixes;
}

UInt32 DungeonMonsterGenerator::_EncodeMonsterTypeID(UInt32 id, UInt16 level)
{
    return id + level * 100;
}

bool DungeonMonsterGenerator::_GenMonsterData(DungeonMonsterGenerateContext& context, DungeonMonster& monster, UInt32 monsterSetId, UInt32 summonerId, UInt32 monsterId, UInt32 pointId, UInt16 level)
{
    //monsterId = monsterId / 10000 * 10000 + monsterId % 100;
    auto data = MonsterDataEntryMgr::Instance()->FindEntry(monsterId);
    if (!data)
    {
        ErrorLogStream << "generate monster for monster set(" << monsterSetId << ") but can't find monster(" << monsterId << ")'s data." << LogStream::eos;
        return false;
    }

    monster.id = context.GenMonsterID();
    monster.pointId = pointId;
    monster.summonerId = summonerId;
    monster.level = GenMonsterLevel(context, level);
    monster.type = data->type;
	monster.mode = data->mode;
    monster.typeId = _EncodeMonsterTypeID(monsterId, monster.level);
    GenMonsterExp(context, monster);
    monster.dropSet = data->dropSetIds;
	monster.dailyMaxDropNum = data->dailyMaxDropNum;
    return true;
}

DungeonHellDataEntry* DungeonMonsterGenerator::_GenHellModeData(DungeonMonsterGenerateContext& context)
{
    class DungeonHellVistor : public Avalon::DataEntryVisitor<DungeonHellDataEntry>
    {
    public:
		DungeonHellVistor(DungeonMonsterGenerateContext& ctx) : context(ctx), totalProb(0) {}
        bool operator()(DungeonHellDataEntry *dataEntry)
        {
			if (dataEntry && context.dungeonData && std::find(dataEntry->dungeonIDs.begin(), dataEntry->dungeonIDs.end(), ((context.dungeonData->id / 10) * 10)) != dataEntry->dungeonIDs.end())
			{
				totalProb = dataEntry->prob;
				datas.clear();
				datas.push_back(dataEntry);
				return false;
			}

            if (dataEntry && dataEntry->dungeonLevel == context.dungeonLevel)
            {
				totalProb += dataEntry->prob;
                datas.push_back(dataEntry);
            }

            return true;
        }

		DungeonMonsterGenerateContext context;
		UInt32 totalProb;
        std::vector<DungeonHellDataEntry*> datas;
    };

    // 先随机
    DungeonHellVistor vistor(context);
    DungeonHellDataEntryMgr::Instance()->Visit(vistor);

    if (vistor.datas.empty() || vistor.totalProb == 0)
    {
		ErrorLogStream << "dungeonlevel(" << context.dungeonLevel << ") gen hell mode failed." << LogStream::eos;
        return NULL;
    }

    UInt32 prob = Avalon::Random::RandBetween(0, vistor.totalProb - 1);
	for (auto data : vistor.datas)
	{
		if (prob < data->prob)
		{
			return data;
		}
		prob -= data->prob;
	}

	return vistor.datas[0];
}

void DungeonMonsterGenerator::GenHellMonstersForGm(UInt32	dungeonId, UInt32 times)
{
	auto dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
	if (!dungeonData)
	{
		ErrorLogStream << "dungeonData is null, id : " << dungeonId << LogStream::eos;
		return;
	}

	struct MonsterIdKey
	{
		MonsterIdKey(UInt32	 monsterId_, UInt32 typeId_)
			: monsterId(monsterId_), typeId(typeId_){}

		bool operator<(const MonsterIdKey& other) const
		{
			if (other.monsterId != monsterId)
			{
				return monsterId < other.monsterId;
			}

			return typeId < other.typeId;
		}

		UInt32							monsterId;
		UInt32                          typeId;


	};

	std::map<MonsterIdKey, UInt32> monsterIdMapNum;

	VDungeonMonster monsters;
	DungeonMonsterGenerateContext context;
	context.hellMonsterLevel = dungeonData->level;
	context.hardType = dungeonData->hardType;
	context.dungeonLevel = dungeonData->level;
	for (UInt32 idx = 0; idx < times; ++idx)
	{
		DungeonMonsterGenerator::Instance()->GenHellMonsters(context, monsters);
		for (auto m:monsters)
		{
			MonsterIdKey key(m.monsterId, m.typeId);
			auto it = monsterIdMapNum.find(key);
			if (it == monsterIdMapNum.end())
			{
				monsterIdMapNum[key] = 1;
			}
			else
			{
				monsterIdMapNum[key] = monsterIdMapNum[key] + 1;
			}
		}
		monsters.clear();
	}

	std::string rootpath = WSApplication::Instance()->GetHellMonsterCountPath();
	std::string dungenIdStr = Avalon::StringUtil::ConvertToString(dungeonId);
	std::string timesStr = Avalon::StringUtil::ConvertToString(times);
	std::string postfix = dungenIdStr + "_" + timesStr + ".csv";
	std::string filename = rootpath + "HellMonsterCount_" + postfix;
	std::ofstream ofs;
	ofs.open(filename.c_str());
	if (!ofs.good())
	{
		ErrorLogStream << "FILE ptr is null!" << LogStream::eos;
		return;
	}

	std::string header = "monsterId,typeId,num\n";
	ofs.write(header.c_str(), header.length());
	for (auto itr :monsterIdMapNum)
	{
		std::ostringstream oss;
		oss << itr.first.monsterId << ", " << itr.first.typeId << ", " << itr.second << ", " << "\n";
		std::string line = oss.str();
		ofs.write(line.c_str(), line.size());
	}
	ofs.close();
}