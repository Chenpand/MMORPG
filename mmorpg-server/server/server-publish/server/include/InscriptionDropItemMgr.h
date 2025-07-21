#ifndef __INSCRIPTION_DROP_ITEM_MGR_H__
#define __INSCRIPTION_DROP_ITEM_MGR_H__

#include <random>
#include <algorithm>
#include <AvalonObjectPool.h>
#include <CLItemTypeDefine.h>
#include <CLItemDefine.h>

struct InscriptionDropChooseSet
{
	InscriptionDropChooseSet()
    {
        memset(this, 0, sizeof(*this));
    }

    UInt32 num;
    UInt32 prob;
};
typedef std::vector<InscriptionDropChooseSet> VInscriptionDropChooseSet;

struct InscriptionDropItemElem
{
	InscriptionDropItemElem() : id(0), type(0), num(0), prob(0), occuAdditionProb(0)
    {
        
    }

    UInt32 id;
	UInt32 type; // 0:物品；1：方案
    UInt32 num;
    UInt32 prob;
	std::vector<UInt32>     occuAddition;
	Int32                   occuAdditionProb;
};
typedef std::vector<InscriptionDropItemElem> VInscriptionDropItemElem;

// 掉落组
struct InscriptionDropItemSet
{
    AVALON_DEFINE_OBJECTPOOL(InscriptionDropItemSet, Avalon::NullMutex)
	InscriptionDropItemSet()
    {
        id = 0;
    }

    UInt32 id;
    VInscriptionDropItemElem items;
    VInscriptionDropChooseSet chooseSet;
};
typedef HashMap<UInt32, InscriptionDropItemSet*> MapInscriptionDropItemSet;

class InscriptionDropItemMgr : public Avalon::Singleton<InscriptionDropItemMgr>
{
public:
	InscriptionDropItemMgr();
    ~InscriptionDropItemMgr();

	bool Init();

	void GenDropItems(UInt32 id, UInt32 occu, ItemRewardVec& result);

private:
	InscriptionDropItemSet* FindDropItemSet(UInt32 id);

	// 检测掉落数据的正确性
	bool CheckDropItemData();

	UInt32 GenDropItemNum(InscriptionDropItemSet* dropSet);

	bool GenDropItemFromDropSet(InscriptionDropItemSet* dropSet, UInt32 occu, ItemRewardVec& result);

	void AdjustDropSet(InscriptionDropItemSet& dropSet, UInt32 occu);

private:
    MapInscriptionDropItemSet m_dropItemSets;
};

#endif
