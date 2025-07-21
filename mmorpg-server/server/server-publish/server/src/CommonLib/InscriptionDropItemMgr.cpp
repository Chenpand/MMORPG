#include "InscriptionDropItemMgr.h"
#include <CLItemDataEntry.h>
#include <CLInscriptionDropItemDataEntry.h>

InscriptionDropItemMgr::InscriptionDropItemMgr()
{
	
}

InscriptionDropItemMgr::~InscriptionDropItemMgr()
{
	for (auto iter : m_dropItemSets)
	{
		CL_SAFE_DELETE(iter.second);
	}

	m_dropItemSets.clear();
}

bool InscriptionDropItemMgr::Init()
{
    class InscriptionDropItemInitVisitor : public InscriptionDropItemDataEntryVisitor
    {
    public:
        bool operator()(InscriptionDropItemDataEntry *dataEntry)
        {
            if (dataEntry == NULL)
            {
                return false;
            }

            datas.push_back(dataEntry);
            return true;
        }

        std::vector<InscriptionDropItemDataEntry*> datas;
    };

	InscriptionDropItemInitVisitor vistor;
	InscriptionDropItemDataEntryMgr::Instance()->Visit(vistor);

    for (auto data : vistor.datas)
    {
		InscriptionDropItemSet* dropSet = FindDropItemSet(data->groupID);
        if (dropSet == NULL)
        {
            dropSet = new InscriptionDropItemSet();
            dropSet->id = data->groupID;

            m_dropItemSets[dropSet->id] = dropSet;

            if (data->chooseNumSet.size() != data->numProbSet.size())
            {
                ErrorLogStream << "init inscription drop item(" << data->groupID << ") failed, chooseNumSet is not match to numProbSet." << LogStream::eos;
                return false;
            }

            for (UInt32 i = 0; i < data->chooseNumSet.size(); i++)
            {
				InscriptionDropChooseSet chooseSet;
                chooseSet.num = data->chooseNumSet[i];
                chooseSet.prob = data->numProbSet[i];
                dropSet->chooseSet.push_back(chooseSet);
            }
        }

		ItemDataEntry* itemData = NULL;
		if (data->dataType == 0)
		{
			itemData = ItemDataEntryMgr::Instance()->FindEntry(data->itemID);
			if (!itemData)
			{
				ErrorLogStream << "init inscription drop set(" << data->groupID << ") failed, item(" << data->itemID << ") not exist." << LogStream::eos;
				continue;
			}
		}

		InscriptionDropItemElem item;
        item.id = data->itemID;
		item.type = data->dataType;
        item.prob = data->itemProb;
        item.num = data->itemNum;
		item.occuAddition = data->occuAddition;
		item.occuAdditionProb = data->occuAdditionProb;
        
        dropSet->items.push_back(item);
    }

	if (!CheckDropItemData())
	{
		ErrorLogStream << "item drop data is not correct, please check it." << LogStream::eos;
		return false;
	}

    return true;
}

InscriptionDropItemSet* InscriptionDropItemMgr::FindDropItemSet(UInt32 id)
{
	auto itr = m_dropItemSets.find(id);
	return itr == m_dropItemSets.end() ? NULL : itr->second;
}

bool InscriptionDropItemMgr::CheckDropItemData()
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
					continue;
				}

				for (auto& dropItem : dropSet->items)
				{
					if (dropItem.type != 0)
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

	return true;
}

void InscriptionDropItemMgr::GenDropItems(UInt32 id, UInt32 occu, ItemRewardVec& result)
{
    auto dropSet = FindDropItemSet(id);
    if (dropSet == NULL)
    {
		ErrorLogStream << "InscriptionDropItemMgr::GenDropItems Error id is" << id << LogStream::eos;
        return;
    }

    // 把掉落信息拷贝出来
    auto tmpDropSet = *dropSet;
    UInt32 dropNum = GenDropItemNum(&tmpDropSet);
    if (dropNum == 0)
    {
        return;
    }

	AdjustDropSet(tmpDropSet, occu);

    for (UInt32 i=0; i < dropNum; ++i)
    {
        if (!GenDropItemFromDropSet(&tmpDropSet, occu, result))
        {
            // 失败了，就说明掉落包里面没东西了，再也掉不出东西了
            break;
        }
    }
}

UInt32 InscriptionDropItemMgr::GenDropItemNum(InscriptionDropItemSet* dropSet)
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
        ErrorLogStream << "drop set(" << dropSet->id << ") gen drop item num failed." << LogStream::eos;
        return 0;
    }

    UInt32 prob = Avalon::Random::RandBetween(1, totalProb);
    for (auto& choose : dropSet->chooseSet)
    {
        if (prob <= choose.prob)
        {
            return choose.num;
        }

        prob -= choose.prob;
    }

    return 0;
}

bool InscriptionDropItemMgr::GenDropItemFromDropSet(InscriptionDropItemSet* dropSet, UInt32 occu, ItemRewardVec& result)
{
    if (!dropSet)
    {
        return false;
    }

    UInt32 totalProb = 0;
    for (auto& item : dropSet->items)
    {
        totalProb += item.prob;
    }

    if (totalProb == 0)
    {
        return false;
    }

    UInt32 prob = Avalon::Random::RandBetween(1, totalProb);

    for (size_t i=0; i < dropSet->items.size(); ++i)
    {
        auto& item = dropSet->items[i];
        if (i == (dropSet->items.size() - 1) || prob <= item.prob)
        {
            // 子方案
            if (item.type != 0)
            {
				GenDropItems(item.id, occu, result);
            }
            else
            {
				ItemReward tmpItem(item.id, item.num, 0, 0);
				result.push_back(tmpItem);
            }

            dropSet->items.erase(dropSet->items.begin() + i);
            return true;
        }

        prob -= item.prob;
    }

    return false;
}

void InscriptionDropItemMgr::AdjustDropSet(InscriptionDropItemSet& dropSet, UInt32 occu)
{
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
		if (std::find(item.occuAddition.begin(), item.occuAddition.end(), occu) != item.occuAddition.end())
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
					if ((item.occuAdditionProb + item.prob) <= 0)
					{
						item.prob = 0;
					}
					else
					{
						item.prob += item.occuAdditionProb;
					}
				}
				else
				{
					item.prob += item.occuAdditionProb;
				}
			}
		}
		++itr;
	}
}


