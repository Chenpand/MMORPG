/**********************************************************************************

   ע��:
           1. _CUSTOM_*_BEGIN��_CUSTOM_*_END֮��Ĵ����ǿ����ֶ��޸ĵġ�
           2. �����ط��Ĵ��붼��Ҫ�Ķ�!!!!

*********************************************************************************/
#ifndef __CL_GOLDCONSIGNMENTVALUE_DATA_ENTRY_H__
#define __CL_GOLDCONSIGNMENTVALUE_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

//�Զ���ͷ�ļ�	
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// �Զ���ṹ��ö��	
// __CUSTOM_STRUCT_BEGIN__


enum GoldConsignmentValueType
{
	GC_VALUE_AVERAGE_PRICE = 1,			// ƽ�����׼�
	GC_VALUE_CLOSE_START_TIME = 2,		// ���п�ʼʱ��
	GC_VALUE_CLOSE_END_TIME = 3,		// ���н���ʱ��
	GC_VALUE_ACC_ORDER_NUM = 4,			// �˺��ϼܶ�����
	GC_VALUE_GRADE_PAGE_NUM = 5,		// ������ʾ��λ��
	GC_VALUE_UNIT_PRICE_BASE = 6,		// ������N����Ϊ��λ
	GC_VALUE_NEW_SERVER_DAY = 7,		// �·���������
	GC_VALUE_HANG_UP_TIME = 8,		// �ҵ�ʱ�䣨Сʱ��
	GC_VALUE_MIN_GOLD_NUM = 9,		// ��С���۽������
	GC_VALUE_MAX_GOLD_NUM = 10,		// �����۽������
	GC_VALUE_COMMISSIOM = 11,		// ��Ҽ��������ѱ�����ǧ�ֱȣ�
	GC_VALUE_TRANSFER_POINT_RATE = 12,		// ���õ�ȯ�һ���ȯ����
	GC_VALUE_TRANSFER_POINT_DATA = 13,		// ���õ�ȯת������
	GC_VALUE_MIN_PROTECT_PRICE = 14,		// ��ҽ��׵�����ͱ�����
	GC_VALUE_MAX_PROTECT_PRICE = 15,		// ��ҽ��׵�����߱�����
	GC_VALUE_PRICE_WAVE_RATE = 16,		// ��Ҽ��۽�ҵ����ǵ���������ǧ�ֱȣ�
	GC_VALUE_RECENT_AVERAGE_PRICE_TIME = 18,// ���ƽ���ɽ���ÿ�ζԱ�ʱ�䣨��Ϊ��λ��
	GC_VALUE_CREDIT_POINT_SHOW_DAY = 20,// ���õ�ȯ��¼��ʾ����
	GC_VALUE_RECENT_PRICE_NODE = 21,// ���ƽ���ɽ��ۼ���ڵ���
	GC_VALUE_TRADE_UNIT_BASE = 22,// ���׵��ۻ���
	GC_VALUE_TRADE_RECORD_SHOW_TIME = 23,// ���׼�¼��ʾʱ��
};

// __CUSTOM_STRUCT_END__

class GoldConsignmentValueDataEntry : public Avalon::DataEntry
{
public:
	GoldConsignmentValueDataEntry();
	virtual ~GoldConsignmentValueDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// �Զ���ӿ�	
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	//ID
	UInt32                                        id;
	//����
	std::string                                   name;
	//��ֵ
	UInt32                                        value;

// �Զ����ֶ�
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class GoldConsignmentValueDataEntryMgr : public Avalon::DataEntryMgrBase<GoldConsignmentValueDataEntry>
	, public Avalon::Singleton<GoldConsignmentValueDataEntryMgr>
{
public:
	virtual bool AddEntry(GoldConsignmentValueDataEntry* entry);

// �Զ���ӿڡ��ֶ�
// __CUSTOM_MGR_BEGIN__

	void OnReload();

	UInt32 GetValue(UInt32 type);

	bool IsCheckZeroValue(UInt32 id);

private:

	std::map<UInt32, UInt32> m_ValueMap;

// __CUSTOM_MGR_END__
};

#endif
