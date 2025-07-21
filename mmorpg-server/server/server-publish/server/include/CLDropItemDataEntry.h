#ifndef __CL_DROP_ITEM_DATA_ENTRY_H__
#define __CL_DROP_ITEM_DATA_ENTRY_H__

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <set>
#include <bitset>

enum DropDataType
{
    DDT_ITEM,           // ��Ʒ
    DDT_SOLUTION,       // ����
};

enum DropItemType
{
    DIT_COMMON,         // ��ͨ����
    DIT_GOLD,           // ���
    DIT_PINK,           // ��װ
    DIT_TASK,           // ������Ʒ
	DIT_SS,				// ʷʫ
	DIT_SS_MATERIAL,	// ������
	DIT_BOSS_PINK,		// ������
	DIT_YZZY,			// ����֮��
	DIT_CHEST,			// ����
	DIT_TICKET_SY,		// ��ԨƱ
	DIT_TICKET_YG,		// Զ��Ʊ
	DIT_YUANGU_PINK,	// Զ�ŷ�װ
	DIT_BERYL,			// ͸��ˮ����
	DIT_BIND_GOLD,		// ���
	DIT_XWZY,			// ����֮Դ
	DIT_NUM,
};

/**
*@brief ����������
*/
class DropItemDataEntry : public Avalon::DataEntry
{
public:
    DropItemDataEntry();
    ~DropItemDataEntry();

public:
    UInt32 GetKey() const { return id; }

    bool Read(Avalon::DataRow& row);

public:
    // ����id�����Ǹ�������ID���Զ�����
    UInt32                  id;
    // ���䷽��ID
    UInt32                  projID;
    // ��������
    std::vector<UInt32>     chooseNumSet;
    // ����Ȩ��
    std::vector<UInt32>     numProbSet;
    // ��������
    DropDataType            dataType;
    // ��ƷID
    UInt32                  itemID;
    // Ȩ��
    UInt32                  itemProb;
    // ��Ʒ����
    UInt32                  itemMinNum;
    // ��Ʒ����
    UInt32                  itemMaxNum;
    // �Ƿ�ϡ�п���
    bool                    isRareControl;
    // ����ID
    UInt32                  taskId;
    // ְҵ�ӳ�
    std::vector<UInt32>     occuAddition;
    // �ӳ�Ȩ��
    Int32                   occuAdditionProb;
    // ������Ʒ����(DropItemType)
    UInt8                   itemType;
	// ������ʾ
	UInt32					notice;
	// �ID
	std::set<UInt32>		activityIds;
	// ���
	std::bitset<32>         markFlag;
	// vip��С�ȼ�
	UInt8					minVipLv;
	// vip���ȼ�
	UInt8					maxVipLv;
	// vip�������޿���id
	UInt32					vipDropLimitId;
	// ����ӳ�
	UInt32                  extra;
};

typedef Avalon::DataEntryVisitor<DropItemDataEntry> DropItemDataEntryVisitor;
typedef Avalon::DataEntryMgr<DropItemDataEntry> DropItemDataEntryMgr;

#endif