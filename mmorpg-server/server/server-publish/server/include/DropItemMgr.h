#ifndef __DROP_ITEM_MGR_H__
#define __DROP_ITEM_MGR_H__

#include <random>
#include <algorithm>
#include <AvalonSingleton.h>
#include <CLDefine.h>
#include <AvalonObjectPool.h>
#include <CLItemTypeDefine.h>
#include <CLDropItemDefine.h>
#include <CLDropItemDataEntry.h>
#include <CLRaceEndDropDataEntry.h>
#include <set>

struct DropChooseSet
{
    DropChooseSet()
    {
        memset(this, 0, sizeof(*this));
    }

    UInt32 num;
    UInt32 prob;
};
typedef std::vector<DropChooseSet> VDropChooseSet;

struct DropItemElem
{
	DropItemElem() : id(0), type(DDT_ITEM), minNum(0), maxNum(0), prob(0), isRareControl(false),
					taskId(0), occuAdditionProb(0), itemType(DIT_COMMON), notice(0),
					minVipLv(0), maxVipLv(0), vipDropLimitId(0), extra(0),
					itemMainType(0), itemLevel(0), itemColor(0), itemColor2(0)   
    {
        // memset(this, 0, sizeof(*this));
    }

	// �����ȡװ������
	UInt8 RandEquipType(UInt32 dungeonId, UInt32 dungeonSubType, UInt32 dungeonHard) const;

	bool CheckMark(DropItemMarkType dropItemMarkType) const;

    UInt32                  id;
    DropDataType            type;
    UInt32                  minNum;
    UInt32                  maxNum;
    UInt32                  prob;
    bool                    isRareControl;
    UInt32                  taskId;
    std::vector<UInt32>     occuAddition;
    Int32                   occuAdditionProb;
    DropItemType            itemType;
	UInt32					notice;
	std::set<UInt32>		activityIds;
	std::bitset<32>         markFlag;
	UInt8					minVipLv;
	UInt8					maxVipLv;
	UInt32					vipDropLimitId;
	UInt32                  extra;

	// ������Ϣ
	UInt8					itemMainType;
	UInt8					itemSubType;
	UInt16					itemLevel;
	UInt8					itemColor;
	UInt8					itemColor2;
};
typedef std::vector<DropItemElem> VDropItemElem;

// ������
struct DropItemSet
{
    AVALON_DEFINE_OBJECTPOOL(DropItemSet, Avalon::NullMutex)
    DropItemSet()
    {
        id = 0;
        hasRareControlItem = false;
    }

    UInt32              id;
    bool                hasRareControlItem;
    VDropItemElem       items;
    VDropChooseSet      chooseSet;
	std::bitset<32>     markFlag;
};
typedef HashMap<UInt32, DropItemSet*> MapDropItemSet;

struct DropItemEx : DropItem
{
    DropItemEx()
    {
        isRareControl = false;
		notice = 0;
		dropSolutionId = 0;
		vipDropLimitId = 0;
    }

    DropItemEx& operator=(const DropItem& item)
    {
        typeId = item.typeId;
        num = item.num;
		strenth = item.strenth;
		dropSolutionId = 0;
		vipDropLimitId = 0;
        return *this;
    }

    DropItem GetDropItem()
    {
        DropItem item;
        item.typeId = typeId;
        item.num = num;
		item.strenth = strenth;
        return item;
    }

	bool CheckMark(DropItemMarkType dropItemMarkType) const
	{
		std::string markStr = markFlag.to_string();
		return markFlag.test(dropItemMarkType);
	}

    UInt32          taskId;
    bool            isRareControl;
	UInt32			dropSolutionId;
	UInt32			notice;
	std::bitset<32> markFlag;
	UInt32			vipDropLimitId;
};
typedef std::vector<DropItemEx> VDropItemEx;

struct GenDropItemResult
{
    VDropItemEx dropItems;
};

class DungeonDataEntry;
struct DropItemContext
{
	DropItemContext(const std::set<UInt32>&	ac):activityList(ac)
	{
		roleId = 0;
		occu = 0;
		pinkDropDec = 0;
		isChestDrop = false;
		dungeonData = NULL;
		viplv = 0;
		
	}
	static void MergeMarkFlag(std::bitset<32>& flag1, std::bitset<32>& flag2)
	{
		if (flag2.none())
		{
			return;
		}
		for (int i = 0; i < 32; ++i)
		{
			if (flag2.test(i))
			{
				flag1.set(i);
			}
		}
	}

	ObjID_t				roleId;
    UInt8               occu;
    DungeonAddition     addition;
	// ��������˥��
	UInt16				pinkDropDec;
    std::vector<UInt32> taskList;
	// ���ڿ��ŵĻ�б�
	const std::set<UInt32>&	activityList;
    VDropItemEx         items;
	// �ǲ��ǽ��㱦�����
	bool				isChestDrop;
	// ���³���Ϣ
	DungeonDataEntry*	dungeonData;
	// ���vip�ȼ�
	UInt8				viplv;
	// �Ƿ�������
	std::bitset<32>     extra;
	// ���
	std::bitset<32>		markFlag;
};

// ���㱦��ķ�װ����˥��
#define RACE_END_CHEST_PINK_DROP_DEC 50

class IRareItemControl
{
public:
	/**
	*@brief ϡ�п�����Ʒʣ������
	*/
	virtual UInt32 RareItemRemainNum(ObjID_t roleId, UInt32 itemId, UInt32 num) = 0;

	/**
	*@brief ϡ�п��Ʒ���ʣ������
	*/
	virtual UInt32 RareDropSolutionRemainNum(ObjID_t roleId, UInt32 solutionId, UInt32 num) = 0;
};

class DropItemMgr : public Avalon::Singleton<DropItemMgr>
{
public:
    DropItemMgr();
    ~DropItemMgr();

	bool Init(IRareItemControl* control = NULL);
    GenDropItemResult GenDropItems(UInt32 dungeonId, ObjID_t roleId,UInt32 id, UInt8 vipLv, UInt8 occu = 0, const std::vector<UInt32>& taskList = std::vector<UInt32>(),
                                    DungeonAddition addition = DungeonAddition(), const std::set<UInt32>& activityList = std::set<UInt32>(), const UInt32 extra = 0);

	GenDropItemResult GenRaceEndChest(UInt32 dungeonId, ObjID_t roleId, UInt32 id, UInt8 occu, DungeonAddition addition = DungeonAddition(), const UInt32 extra = 0);

	// �������
	// ��û�н������
	bool HasRaceEndDrop(UInt32 dungeonId, UInt32 level);

	// ��ȡ�������
	GenDropItemResult GenRaceEndDrop(ObjID_t roleId, UInt32 dungeonId, UInt32 level, const DungeonAddition& addition, const std::set<UInt32>& activityList = std::set<UInt32>(), const UInt32 extra = 0);

    bool HasRareControlItemInDropSet(UInt32 id);

	UInt32 RandFromRange(UInt32 min, UInt32 max);

public:
    DropItemSet* FindDropItemSet(UInt32 id);

private:
	// ��ȡ�����������
	RaceEndDropDataEntry* FindRaceEndData(UInt32 dungeonId, UInt32 level);

    // ���������ݵ���ȷ��
    bool CheckDropItemData();

    // �����㷭�������ݵ���ȷ��
    bool ChestDungeonChestData();

    void _GenDropItems(UInt32 id, DropItemContext& context);
    UInt32 GenDropItemNum(DropItemSet* dropSet);
    bool GenDropItemFromDropSet(DropItemSet* dropSet, DropItemContext& context, bool removeFromSetIfDrop = true);
    UInt32 Rand();
    void _AdjustDrop(DropItemSet& dropSet, DropItemContext& context);

private:
    MapDropItemSet		m_dropItemSets;
	IRareItemControl*	m_rareItemControl;
};


//α�������
class FakeWeightBase
{
public:
	FakeWeightBase();
	~FakeWeightBase();

	inline void SetOwner(void* owner)				{ m_owner = owner; }
	inline void* GetOwner()							{ return m_owner; }
	inline void SetWeightBase(UInt32 value)			{ m_weightBase = value; }
	inline void SetPlusFix(UInt32 value)			{ m_plusFix = value; }
	inline void SetMinusFix(UInt32 value)			{ m_minusFix = value; }
	inline void SetMinusFixNumBase(UInt32 value)	{ m_minusFixNumBase = value; }
	inline void SetWeightUpper(UInt32 value)		{ m_weightUpper = value; }
	inline void SetWeightLower(UInt32 value)		{ m_weightLower = value; }
	inline UInt32 GetPlusFix()						{ return m_plusFix; }
	inline UInt32 GetMinusFix()						{ return m_minusFix; }

	inline UInt32 GetMissNum()						{ return m_missNum; }
	inline UInt32 GetMinusFixNum()					{ return m_minusFixNum; }
	inline void SetMissNum(UInt32 value)			{ m_missNum = value; }
	inline void SetMinusFixNum(UInt32 value)		{ m_minusFixNum = value; }

	void	OnHit();
	void	OnMiss();
	UInt32	GetFakeWeight();

private:
	void	ReduceMinusFixNum();

private:
	void*		m_owner;
	UInt32		m_weightBase;			//����Ȩ��
	UInt32		m_plusFix;				//��������
	UInt32		m_minusFix;				//��������
	UInt32		m_minusFixNumBase;		//������������
	UInt32		m_minusFixNum;			//������������
	UInt32		m_weightUpper;			//Ȩ������
	UInt32		m_weightLower;			//Ȩ������
	UInt32		m_missNum;				//δ���д���
};

//Ȩ�����
class WeightRander
{
public:
	WeightRander();
	~WeightRander();

	static FakeWeightBase* RandWeight(std::vector<FakeWeightBase*>& list, bool cutRepeat = false);
	
	static Int32 RandWeight(std::vector<UInt32>& list);

	// ��������
	template<typename T>
	static std::vector<T> DisorganizeData(const std::vector<T>& datas);
};

template<typename T>
std::vector<T> WeightRander::DisorganizeData(const std::vector<T>& datas)
{
	std::vector<T> outOfOrder = datas;

	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(outOfOrder.begin(), outOfOrder.end(), g);

	return outOfOrder;
}

#endif
