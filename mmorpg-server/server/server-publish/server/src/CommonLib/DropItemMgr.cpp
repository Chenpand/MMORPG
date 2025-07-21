#include "DropItemMgr.h"

#include <set>
#include <queue>

#include "CLDropItemDataEntry.h"
#include "CLDungeonChestDataEntry.h"
#include "CLItemDataEntry.h"
#include "CLDropSmellControlDataEntry.h"
#include "CLDungeonDataEntry.h"

UInt8 DropItemElem::RandEquipType(UInt32 dungeonId, UInt32 dungeonSubType, UInt32 dungeonHard) const
{
	if (itemMainType != ITEM_EQUIP || itemSubType == ST_ASSIST_EQUIP
		|| itemSubType == ST_MAGICSTONEE
		|| itemSubType == ST_EARRINGS)
	{
		return EQUIP_NORMAL;
	}

	return DropSmellControlDataEntryMgr::Instance()->RandEquipType(itemLevel, itemColor, itemColor2, dungeonSubType, dungeonHard, dungeonId);
}

bool  DropItemElem::CheckMark(DropItemMarkType dropItemMarkType) const
{
	std::string markStr = markFlag.to_string();
	DebugLogStream << "check drop item mark id = " << id << ", markFlag str = " << markStr << ", dropItemMarkType = " << (UInt32)dropItemMarkType << LogStream::eos;
	return markFlag.test(dropItemMarkType);
}

DropItemMgr::DropItemMgr()
{
	m_rareItemControl = NULL;
}

DropItemMgr::~DropItemMgr()
{
	for (auto iter : m_dropItemSets)
	{
		CL_SAFE_DELETE(iter.second);
	}

	m_dropItemSets.clear();
}

bool DropItemMgr::Init(IRareItemControl* control)
{
	m_rareItemControl = control;

    class DropItemInitVisiter : public DropItemDataEntryVisitor
    {
    public:
        bool operator()(DropItemDataEntry *dataEntry)
        {
            if (dataEntry == NULL)
            {
                return false;
            }

            datas.push_back(dataEntry);
            return true;
        }

        std::vector<DropItemDataEntry*> datas;
    };

    DropItemInitVisiter vister;
    DropItemDataEntryMgr::Instance()->Visit(vister);

    for (auto data : vister.datas)
    {
        DropItemSet* dropSet = FindDropItemSet(data->projID);
        if (dropSet == NULL)
        {
            dropSet = new DropItemSet();
            dropSet->id = data->projID;
			dropSet->markFlag = data->markFlag;

            m_dropItemSets[dropSet->id] = dropSet;

            if (data->chooseNumSet.size() != data->numProbSet.size())
            {
                ErrorLogStream << "init dropitem(" << data->projID << ") failed, chooseNumSet is not match to numProbSet." << LogStream::eos;
                return false;
            }

            for (UInt32 i = 0; i < data->chooseNumSet.size(); i++)
            {
                DropChooseSet chooseSet;
                chooseSet.num = data->chooseNumSet[i];
                chooseSet.prob = data->numProbSet[i];
                dropSet->chooseSet.push_back(chooseSet);
            }
        }

		DropItemType itemType = (DropItemType)data->itemType;
		ItemDataEntry* itemData = NULL;
		if (data->dataType == DDT_ITEM)
		{
			itemData = ItemDataEntryMgr::Instance()->FindEntry(data->itemID);
			if (!itemData)
			{
				ErrorLogStream << "init dropset(" << data->projID << ") failed, item(" << data->itemID << ") not exist." << LogStream::eos;
				continue;
			}

			if (itemData->subType == ST_ITEM_GOLD || (itemData->subType == ST_BIND_GOLD && itemType != DIT_BIND_GOLD))
			{
				itemType = DIT_GOLD;
			}
		}

        DropItemElem item;
        item.type = data->dataType;
        item.id = data->itemID;
        item.prob = data->itemProb;
        item.minNum = data->itemMinNum;
        item.maxNum = data->itemMaxNum;
        item.isRareControl = data->isRareControl;
        item.taskId = data->taskId;
        item.occuAddition = data->occuAddition;
        item.occuAdditionProb = data->occuAdditionProb;
		item.itemType = itemType;
		item.notice = data->notice;
		item.activityIds = data->activityIds;
		item.markFlag = data->markFlag;
		item.minVipLv = data->minVipLv;
		item.maxVipLv = data->maxVipLv;
		item.vipDropLimitId = data->vipDropLimitId;
		item.extra = data->extra;

		// 道具信息
		if (itemData)
		{
			item.itemMainType = itemData->type;
			item.itemSubType = itemData->subType;
			item.itemLevel = itemData->needLevel;
			item.itemColor = itemData->color;
			item.itemColor2 = itemData->color2;
		}

        if (item.isRareControl)
        {
            dropSet->hasRareControlItem = true;
        }
        
        dropSet->items.push_back(item);
    }

    if (!CheckDropItemData())
    {
        ErrorLogStream << "item drop data is not correct, please check it." << LogStream::eos;
        return false;
    }

    if (!ChestDungeonChestData())
    {
        ErrorLogStream << "dungeon chest data is not correct, please check it." << LogStream::eos;
        return false;
    }

    return true;
}

GenDropItemResult DropItemMgr::GenDropItems(UInt32 dungeonId, ObjID_t roleId, UInt32 id, UInt8 vipLv, UInt8 occu, const std::vector<UInt32>& taskList, DungeonAddition addition, const std::set<UInt32>& activityList, const UInt32 extra)
{
    GenDropItemResult ret;

    DropItemContext context(activityList);
	context.roleId = roleId;
    context.occu = occu;
    context.taskList = taskList;
    context.addition = addition;
	//context.activityList = activityList;
	context.dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
	context.viplv = vipLv;
	context.extra = extra;

    _GenDropItems(id, context);
    ret.dropItems = context.items;
    return ret;
}

GenDropItemResult DropItemMgr::GenRaceEndChest(UInt32 dungeonId, ObjID_t roleId, UInt32 id, UInt8 occu, DungeonAddition addition, const UInt32 extra)
{
	GenDropItemResult ret;
	static const std::set<UInt32> emptySet;
	DropItemContext context(emptySet);
	context.roleId = roleId;
	context.occu = occu;
	context.pinkDropDec = RACE_END_CHEST_PINK_DROP_DEC;
	context.isChestDrop = true;
	context.addition = addition;
	context.dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
	context.extra = extra;

	_GenDropItems(id, context);
	ret.dropItems = context.items;
	return ret;
}

bool DropItemMgr::HasRaceEndDrop(UInt32 dungeonId, UInt32 level)
{
	auto data = FindRaceEndData(dungeonId, level);
	return data && !data->dropSets.empty();
}

GenDropItemResult DropItemMgr::GenRaceEndDrop(ObjID_t roleId, UInt32 dungeonId, UInt32 level, const DungeonAddition& addition, const std::set<UInt32>& activityList, const UInt32 extra)
{
	GenDropItemResult ret;

	auto raceEndData = FindRaceEndData(dungeonId, level);
	if (!raceEndData)
	{
		return ret;
	}

	DropItemContext context(activityList);
	context.roleId = roleId;
	context.addition = addition;
	context.dungeonData = DungeonDataEntryMgr::Instance()->FindEntry(dungeonId);
	//context.activityList = activityList;
	context.extra = extra;
	

	for (auto& dropSet : raceEndData->dropSets)
	{
		for (UInt32 i = 0; i < dropSet.num; i++)
		{
			_GenDropItems(dropSet.dropSetId, context);
		}
	}
	
	ret.dropItems = context.items;
	return ret;
}

bool DropItemMgr::HasRareControlItemInDropSet(UInt32 id)
{
    auto dropSet = FindDropItemSet(id);
    if (!dropSet)
    {
        return false;
    }

    return dropSet->hasRareControlItem;
}

DropItemSet* DropItemMgr::FindDropItemSet(UInt32 id)
{
    auto itr = m_dropItemSets.find(id);
    return itr == m_dropItemSets.end() ? NULL : itr->second;
}

RaceEndDropDataEntry* DropItemMgr::FindRaceEndData(UInt32 dungeonId, UInt32 level)
{
	class RaceEndDropVistor : public Avalon::DataEntryVisitor<RaceEndDropDataEntry>
	{
	public:

		virtual bool operator()(RaceEndDropDataEntry* data)
		{
			if (data)
			{
				datas.push_back(data);
			}

			return true;
		}

		std::vector<RaceEndDropDataEntry*> datas;
	};

	RaceEndDropVistor vistor;
	RaceEndDropDataEntryMgr::Instance()->Visit(vistor);
	
	for (auto data : vistor.datas)
	{
		if (!data)
		{
			continue;
		}

		if (data->dungeonId == dungeonId && data->minLevel <= level && data->maxLevel >= level)
		{
			return data;
		}
	}

	return NULL;
}

bool DropItemMgr::CheckDropItemData()
{
    // 检测表格数据的合法性
    // 拓扑排序算法，检测掉落方案中是否有循环引用a->b->a

    struct DropPoint
    {
        DropPoint() : degree(0) {}
        int degree;
        std::set<UInt32> subDrop;
    };
    for (auto itr : m_dropItemSets)
    {
        std::set<int> flags;
        std::vector<UInt32> solutiones;
        std::map<UInt32, DropPoint> dropMap;

        solutiones.push_back(itr.first);

        // 建图
        do 
        {
            std::vector<UInt32> subSolutiones;
            for (auto dropId : solutiones)
            {
                auto& p = dropMap[dropId];

                auto dropSet = FindDropItemSet(dropId);
                if (!dropSet)
                {
                    //ErrorLogStream << "drop(" << itr.first << ") can't find sub drop set(" << dropId << ")." << LogStream::eos;
                    //return false;
                    continue;
                }

                for (auto& dropItem : dropSet->items)
                {
                    if (dropItem.type == DDT_SOLUTION)
                    {
                        if (p.subDrop.find(dropItem.id) != p.subDrop.end())
                        {
                            continue;
                        }

                        p.subDrop.insert(dropItem.id);

                        if (dropMap.find(dropItem.id) == dropMap.end())
                        {
                            subSolutiones.push_back(dropItem.id);
                        }

                        dropMap[dropItem.id].degree++;
                    }
                }
            }

            solutiones = subSolutiones;
        } while (!solutiones.empty());

        // 拓扑排序算法，检测是否有环
        int pNum = dropMap.size();
        auto dropMapBackUp = dropMap;

        while (true)
        {
            auto itr = dropMap.begin();
            auto next = dropMap.begin();
            int zeroDegreePoint = 0;
            for (; itr != dropMap.end(); itr = next)
            {
                ++next;

                auto& p = itr->second;
                if (p.degree != 0)
                {
                    continue;
                }

                for (auto subDrop : p.subDrop)
                {
                    dropMap[subDrop].degree--;
                }

                dropMap.erase(itr);
                zeroDegreePoint++;
            }

            pNum -= zeroDegreePoint;
            if (zeroDegreePoint == 0)
            {
                break;
            }
        }

        if (pNum != 0)
        {
            ErrorLogStream << "drop(" << itr.first << ") has circular reference." << LogStream::eos;
            return false;
        }
    }

    /*for (auto itr : m_dropItemSets)
    {
        auto dropSet = itr.second;
        if (!dropSet)
        {
            continue;
        }

        for (auto& item : dropSet->items)
        {
            if (item.type == DDT_SOLUTION)
            {
                if (!FindDropItemSet(item.id))
                {
                    ErrorLogStream << "drop set(" << dropSet->id << ") contains non-existent solution(" << item.id << ")." << LogStream::eos;
                }
            }
        }
    }*/

    return true;
}

bool DropItemMgr::ChestDungeonChestData()
{
    class DungeonChestCheckVisiter : public Avalon::DataEntryVisitor<DungeonChestDataEntry>
    {
    public:
        bool operator()(DungeonChestDataEntry *dataEntry)
        {
            if (!dataEntry)
            {
                return false;
            }

            datas.push_back(dataEntry);
            return true;
        }

        std::vector<DungeonChestDataEntry*> datas;
    };

    bool ret = true;
    DungeonChestCheckVisiter vistor;
    DungeonChestDataEntryMgr::Instance()->Visit(vistor);

    for (auto entry : vistor.datas)
    {
        if (!entry)
        {
            continue;
        }

        std::vector<UInt32> chestIds;
        chestIds.push_back(entry->goldDropSetId);
        for (auto chestId : entry->chestDropoSetIds)
        {
            chestIds.push_back(chestId);
        }

        for (auto chestId : chestIds)
        {
            if (!FindDropItemSet(chestId))
            {
                //ErrorLogStream << "can't find dungeon(" << entry->dungeonId << ")'s chest set(" << chestId << ")." << LogStream::eos;
                //ret = false;
            }
        }
    }

    return ret;
}

void DropItemMgr::_GenDropItems(UInt32 id, DropItemContext& context)
{
    auto dropSet = FindDropItemSet(id);
    if (dropSet == NULL)
    {
        return;
    }

	DropItemContext::MergeMarkFlag(context.markFlag, dropSet->markFlag);

    // 把掉落信息拷贝出来
    auto tmpDropSet = *dropSet;
	// 如果是结算宝箱的掉落，只能随出一个东西
    UInt32 dropNum = context.isChestDrop ? 1 : GenDropItemNum(&tmpDropSet);
    if (dropNum == 0)
    {
        return;
    }

    _AdjustDrop(tmpDropSet, context);

    for (UInt32 i = 0; i < dropNum; i++)
    {
        if (!GenDropItemFromDropSet(&tmpDropSet, context))
        {
            // 失败了，就说明掉落包里面没东西了，再也掉不出东西了
            break;
        }
    }
}

UInt32 DropItemMgr::GenDropItemNum(DropItemSet* dropSet)
{
    if (!dropSet)
    {
        return 0;
    }

    UInt32 totalProb = 0;
    for (auto& choose : dropSet->chooseSet)
    {
        totalProb += choose.prob;
    }

    if (totalProb == 0)
    {
        ErrorLogStream << "dropset(" << dropSet->id << ") gen dropitem num failed." << LogStream::eos;
        return 0;
    }

    UInt32 prob = Rand() % totalProb;
    for (auto& choose : dropSet->chooseSet)
    {
        if (prob < choose.prob)
        {
            //DebugLogStream << "dropset(" << dropSet->id << ") drop num(" << choose.num << ")." << LogStream::eos;
            return choose.num;
        }

        prob -= choose.prob;
    }

    return 0;
}

bool DropItemMgr::GenDropItemFromDropSet(DropItemSet* dropSet, DropItemContext& context, bool removeFromSetIfDrop)
{
    if (!dropSet)
    {
        return false;
    }

	DropItemContext::MergeMarkFlag(context.markFlag, dropSet->markFlag);

    UInt32 totalProb = 0;
    for (auto& item : dropSet->items)
    {
        totalProb += item.prob;
    }

    if (totalProb == 0)
    {
        //ErrorLogStream << "dropset(" << dropSet->id << ") gen drop num failed, total prob is 0." << LogStream::eos;
        return false;
    }

    UInt32 prob = Rand() % totalProb;

    //DebugLogStream << "dropset(" << dropSet->id << ") prob(" << prob << ") total prob(" << totalProb << ")";

    for (UInt32 i = 0; i < dropSet->items.size(); i++)
    {
        auto& item = dropSet->items[i];
        if (i == (dropSet->items.size() - 1) || prob < item.prob)
        {
            // 子方案
            if (item.type == DDT_SOLUTION)
            {
				// 如果是结算宝箱的掉落，只能生成一个掉落物
                int num = context.isChestDrop ? 1 : RandFromRange(item.minNum, item.maxNum);
                for (int j = 0; j < num; j++)
                {
                    _GenDropItems(item.id, context);
                }
            }
            else
            {
                UInt32 num = RandFromRange(item.minNum, item.maxNum);
                if (item.itemType == DIT_GOLD && context.addition.additionRate[DART_GOLD_RATE] > 0)
                {
					num = num * (100 + context.addition.additionRate[DART_GOLD_RATE]) / 100;
				}

				if (item.itemType == DIT_BIND_GOLD && context.addition.additionRate[DART_BIND_GOLD_RATE] > 0)
				{
					num = num * (100 + context.addition.additionRate[DART_BIND_GOLD_RATE]) / 100;
				}

				if (item.itemType == DIT_BERYL && context.addition.additionRate[DART_BERYL_RATE] > 0)
				{
					num = num * (100 + context.addition.additionRate[DART_BERYL_RATE]) / 100;
				}

				// 稀有控制
				if (item.isRareControl && m_rareItemControl)
				{
					num = m_rareItemControl->RareItemRemainNum(context.roleId, item.id, num);
				}
				else
				{
					if (num > 0)
					{
						UInt32 multiplyRate = context.addition.dropMultiplyRateAddition[item.itemType];

						for (UInt32 idx = 0; idx < multiplyRate; ++idx)
						{
							DropItemEx dropItem;
							dropItem.typeId = item.id;
							dropItem.num = num;
							dropItem.taskId = item.taskId;
							dropItem.isRareControl = item.isRareControl;
							dropItem.notice = item.notice;
							dropItem.dropSolutionId = dropSet->id;
							dropItem.markFlag = item.markFlag;
							DropItemContext::MergeMarkFlag(dropItem.markFlag, context.markFlag);
							dropItem.vipDropLimitId = item.vipDropLimitId;
							if (context.dungeonData)
							{
								dropItem.equipType = item.RandEquipType(context.dungeonData->id, context.dungeonData->subType, context.dungeonData->hardType);
							}
							context.items.push_back(dropItem);

							//DebugLogStream << "Player(" << context.roleId << ") multiply num, dropset(" << dropSet->id << ") gen drop item(" << item.id << ", " << num << "), drop type=" << item.itemType << LogStream::eos;
						}
					}
				}

				if (num > 0)
				{
					DropItemEx dropItem;
					dropItem.typeId = item.id;
					dropItem.num = num;
					dropItem.taskId = item.taskId;
					dropItem.isRareControl = item.isRareControl;
					dropItem.notice = item.notice;
					dropItem.dropSolutionId = dropSet->id;
					dropItem.markFlag = item.markFlag;
					DropItemContext::MergeMarkFlag(dropItem.markFlag, context.markFlag);
					dropItem.vipDropLimitId = item.vipDropLimitId;
					if (context.dungeonData)
					{
						dropItem.equipType = item.RandEquipType(context.dungeonData->id, context.dungeonData->subType, context.dungeonData->hardType);
					}
					context.items.push_back(dropItem);

					//DebugLogStream << "Player(" << context.roleId << ") dropset(" << dropSet->id << ") gen drop item(" << dropItem.typeId << ", " << dropItem.num << "), drop type=" << item.itemType << LogStream::eos;
				}
            }

            if (removeFromSetIfDrop)
            {
                dropSet->items.erase(dropSet->items.begin() + i);
            }

            return true;
        }

        prob -= item.prob;
    }

    return false;
}

UInt32 DropItemMgr::Rand()
{
    return (UInt32)rand();
}

UInt32 DropItemMgr::RandFromRange(UInt32 min, UInt32 max)
{
	if (min >= max)
	{
		std::swap(min, max);
	}

    UInt32 range = max - min;
    if (range <= 0)
    {
        return min;
    }

    return min + Rand() % range;
}

void DropItemMgr::_AdjustDrop(DropItemSet& dropSet, DropItemContext& context)
{
    // 任务道具
    for (auto itr = dropSet.items.begin(); itr != dropSet.items.end(); )
    {
        auto& item = *itr;
        if (item.taskId > 0 && std::find(context.taskList.begin(), context.taskList.end(), item.taskId) == context.taskList.end())
        {
            itr = dropSet.items.erase(itr);
        }
        else
        {
            ++itr;
        }
    }

	// 活动掉落
	for (auto itr = dropSet.items.begin(); itr != dropSet.items.end();)
	{
		auto& item = *itr;
		bool filter = item.activityIds.empty() ? false : true;
		for (auto activityId : item.activityIds)
		{
			if (activityId == 0 || context.activityList.find(activityId) != context.activityList.end())
			{
				filter = false;
				//DebugLogStream << "gen drop activity(" << activityId << ")'s dropset(" << item.id << ")." << LogStream::eos;
			}
		}

		if (filter)
		{
			//DebugLogStream << "erase drop activity(" << activityId << ")'s dropset(" << item.id << ")." << LogStream::eos;
			itr = dropSet.items.erase(itr);
		}
		else
		{
			++itr;
		}
	}

    // 职业加成
    for (auto itr = dropSet.items.begin(); itr != dropSet.items.end(); )
    {
        auto& item = *itr;
        if (item.occuAddition.empty())
        {
            ++itr;
            continue;
        }

        bool occuAddition = false;
        if (std::find(item.occuAddition.begin(), item.occuAddition.end(), context.occu) != item.occuAddition.end())
        {
            occuAddition = true;
        }

        // 0代表只有occuAddition中的职业能掉
        if (item.occuAdditionProb == 0)
        {
            if (!occuAddition)
            {
                itr = dropSet.items.erase(itr);
                continue;
            }
        }
        else
        {
            if (occuAddition)
            {
				if (item.occuAdditionProb < 0)
				{
					// 防止item.occuAdditionProb的绝对值大于item.prob，检查一下大小，如果大于item.prob,那就将item.occuAdditionProb改为-item.prob
					if (UInt32(-item.occuAdditionProb) >= item.prob)
					{
						item.occuAdditionProb = Int32(-item.prob);
						ErrorLogStream << "invalid drop set(" << dropSet.id << ") item(" << item.id << " ) prob(" 
										<< item.prob << ") occuAdditionProb(" << item.occuAdditionProb << ")." << LogStream::eos;
					}
				}
                item.prob += item.occuAdditionProb;
            }
        }
        ++itr;
    }

    // 神器掉率加成
    //if (context.addition.additionRate[DART_PINK_DROP_RATE] > 0 || context.pinkDropDec > 0)
    {
        for (auto itr = dropSet.items.begin(); itr != dropSet.items.end(); ++itr)
        {
            auto& item = *itr;

			int addition = 0;
            if (item.itemType == DIT_PINK)
            {
				addition = (int)context.addition.additionRate[DART_PINK_DROP_RATE] - (int)context.pinkDropDec;
				if (addition < -100)
				{
					addition = -100;
				}
            }

			if (context.isChestDrop)
			{
				addition += context.addition.dropAddition[DIT_CHEST];
			}
			else
			{
				if (item.itemType > DIT_COMMON && item.itemType < DIT_NUM && context.addition.dropAddition[item.itemType] > 0)
				{
					addition += context.addition.dropAddition[item.itemType];
				}
			}

			if (addition != 0)
			{
				item.prob = item.prob * (100 + addition) / 100;
            }
			
        }
    }
    
	for (auto itr = dropSet.items.begin(); itr != dropSet.items.end();)
	{
		auto& item = *itr;

		if (item.type == DDT_ITEM && item.isRareControl && m_rareItemControl)
		{
			// 先过滤一遍稀有物品
			if (m_rareItemControl->RareItemRemainNum(context.roleId, item.id, 1) == 0)
			{
				itr = dropSet.items.erase(itr);
				continue;
			}
		}
		else if (item.type == DDT_SOLUTION && item.isRareControl && m_rareItemControl)
		{
			if (m_rareItemControl->RareDropSolutionRemainNum(context.roleId, item.id, 1) == 0)
			{
				itr = dropSet.items.erase(itr);
				continue;
			}
		}

		++itr;
	}

	//vip等级过滤
	for (auto itr = dropSet.items.begin(); itr != dropSet.items.end();)
	{
		auto& item = *itr;
		if (item.minVipLv == item.maxVipLv && item.minVipLv == 0)
		{
			++itr;
			continue;
		}

		if (context.viplv >= item.minVipLv && context.viplv <= item.maxVipLv)
		{
			++itr;
			continue;
		}

		itr = dropSet.items.erase(itr);
	}

	//额外过滤
	for (auto itr = dropSet.items.begin(); itr != dropSet.items.end(); )
	{
		auto& item = *itr;
		if (item.extra > 0)
		{
			if ((context.extra.to_ulong() & item.extra) != 0)
			{
				++itr;
				continue;
			}
			itr = dropSet.items.erase(itr);
		}
		else
		{
			++itr;
			continue;
		}
	}

}

FakeWeightBase::FakeWeightBase()
{
	m_owner = NULL;
	m_weightBase = 0;			
	m_plusFix = 0;				
	m_minusFix = 0;				
	m_minusFixNumBase = 0;		
	m_minusFixNum = 0;			
	m_weightUpper = 0;			
	m_weightLower = 0;			
	m_missNum = 0;								
}

FakeWeightBase::~FakeWeightBase()
{

}

void FakeWeightBase::OnHit()
{
	m_minusFixNum = m_minusFixNumBase;
	m_missNum = 0;
}

void FakeWeightBase::OnMiss()
{
	if (m_missNum < 0x00FFFFFF)
		m_missNum++;

	ReduceMinusFixNum();
}

UInt32 FakeWeightBase::GetFakeWeight()
{
	UInt32 minusFix = 0;
	UInt32 fakeWeight = 0;

	if (m_minusFixNum > 0)
	{
		minusFix = m_minusFix;
	}

	fakeWeight = m_weightBase + m_plusFix * m_missNum - minusFix;
	if (fakeWeight > m_weightUpper + m_weightBase)
	{
		fakeWeight = m_weightUpper + m_weightBase;
	}

	if (fakeWeight < m_weightLower)
	{
		fakeWeight = m_weightLower;
	}

	return fakeWeight;
}

void FakeWeightBase::ReduceMinusFixNum()
{
	if (m_minusFixNum > 0)
	{
		m_minusFixNum--;
	}
}

WeightRander::WeightRander()
{

}

WeightRander::~WeightRander()
{

}

FakeWeightBase* WeightRander::RandWeight(std::vector<FakeWeightBase*>& list, bool cutRepeat)
{
	if (list.size() == 0)
	{
		return NULL;
	}

	Int32 totWt = 0;
	for (size_t i = 0; i < list.size(); ++i){
		if (!list[i])	continue;
		totWt += list[i]->GetFakeWeight();
	}

	Int32 randWt = Avalon::Random::RandBetween(1, totWt);
	Int32 sumWt = 0;
	FakeWeightBase* hiter = NULL;

	std::vector<FakeWeightBase*>::iterator itr = list.begin();
	for (; itr != list.end(); )
	{
		FakeWeightBase* info = *itr;
		if (!info)
		{
			++itr;
			continue;
		}

		if (hiter) 
		{
			info->OnMiss();
			++itr;
			continue;
		}
		
		sumWt += info->GetFakeWeight();
		//抽到
		if (randWt <= sumWt)
		{
			info->OnHit();
			hiter = info;

			if (cutRepeat)
			{
				itr = list.erase(itr);
			}
			else
			{
				++itr;
			}
		}
		else
		{
			info->OnMiss();
			++itr;
		}
	}

	return hiter;
}

Int32 WeightRander::RandWeight(std::vector<UInt32>& list)
{
	if (list.size() == 0)
	{
		return -1;
	}

	if (list.size() > MAX_INT32)
	{
		return -1;
	}

	Int32 totWt = 0;
	for (size_t i = 0; i < list.size(); ++i)
	{
		if (list[i] > MAX_INT32)
		{
			return -1;
		}

		totWt += list[i];
	}

	Int32 randWt = Avalon::Random::RandBetween(1, totWt);
	Int32 sumWt = 0;

	for (size_t i = 0; i < list.size(); ++i)
	{
		sumWt += list[i];

		//抽到
		if (randWt <= sumWt)
		{
			return (Int32)i;
		}

		if (sumWt > MAX_INT32 || sumWt < 0)
		{
			return -1;
		}
	}

	return -1;
}
