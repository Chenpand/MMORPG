#ifndef _CL_AUCTION_SYSVALUE_DATAENTRY_H_
#define _CL_AUCTION_SYSVALUE_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

enum AuctionSystemValueType
{
	ASVT_INVALID = 0,

	ASVT_CALC_AVERPRICE_MINTIMES = 1,			//��������Ʒ����ƽ�����׼۸���ͽ��״���
	ASVT_NOTTREAN_CALC_AVERPRICE_MAXTIMES = 2,	//�����з���Ʒ��Ʒ����ƽ�����׼۸�����״���
	ASVT_TREAN_CALC_AVERPRICE_MAXTIMES = 3,		//��������Ʒ��Ʒ����ƽ�����׼۸�����״���
	ASVT_BEAD_AVERPRICE_RATE = 4,				//�����ƽ�����׼۸�ϵ�����ٷֱȣ�
	ASVT_MAGICCARD_AVERPRICE_RATE = 5,			//��ɫƷ�����ϸ�ħ��ƽ�����׼۸�ϵ�����ٷֱȣ�
	ASVT_MINIMUM_GUARANTEE_RATIO = 6,			//Ʒ��100���׼�ֵϵ�����ٷֱȣ�
	ASVT_GRADE_TOPLIMIT = 7,					//Ʒ��100���޼�ֵ
	ASVT_VERIFY_INTERVAL_TIAME = 8,				//��Ʒ���ʱ�䳤��(��λСʱ)
	ASVT_AUCTION_CMPBEDS_RATIO = 9,				//��װ�۸����ͱ���ƽ���۸�Ƚ�ϵ��
	ASVT_AUCTION_CMPMAGIC_RATIO = 10,			//��װ�۸����͸�ħ��ƽ���۸�Ƚ�ϵ��
	ASVT_EXPIRE_TIME_AUTO_OFFSALE = 12,			//���ڳ���ʱ���Զ��¼�
	SVT_AUCTION_QUERY_RECORD_PRICE_UPLIMIT = 13, //�����в�ѯ��Ʒ���׼�¼ʵʱƽ�����׼۸�����ϵ��
	SVT_AUCTION_QUERY_RECORD_PRICE_LOWLIMIT = 14, //�����в�ѯ��Ʒ���׼�¼ʵʱƽ�����׼۸�����ϵ��

	SVT_AUCTION_QUERY_RECORD_MAX = 15, //�����в�ѯ��Ʒ���׼�¼��ʾ����
	SVT_AUCTION_TREAS_OVERTIME = 16, //��������Ʒ���׳�ʱʱ��
	SVT_AUCTION_NUM_CACHE_TIME = 17, //������������Ʒ��������ʱ��

	SVT_AUCTION_TREAS_DETER_TIMES = 18, //�����н�����ȴʱ���ж�����
	SVT_AUCTION_TREAS_EXTRA_DAYS = 19,  //�����н�����ȴʱ�������������
	SVT_AUCTION_TREAS_TIMES_CLEAR_DAYS = 20,  //�����н��״�����ռ��(��)
	ASVT_MAX,
};

struct AuctionSystemValueDataEntry : public Avalon::DataEntry
{
public:
	AuctionSystemValueDataEntry();
	~AuctionSystemValueDataEntry();

public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	//ID
	UInt32 id;

	//��������
	AuctionSystemValueType type;

	//��ֵ
	UInt32 value;

};

class AuctionSystemValueDataEntryMgr : public Avalon::DataEntryMgrBase<AuctionSystemValueDataEntry>, public Avalon::Singleton<AuctionSystemValueDataEntryMgr>
{
public:
	typedef std::map<AuctionSystemValueType, AuctionSystemValueDataEntry*> SystemValueMap;
public:
	AuctionSystemValueDataEntryMgr();
	~AuctionSystemValueDataEntryMgr();

public:
	bool AddEntry(AuctionSystemValueDataEntry* entry);

	void OnReload();

	void OnAddEntry(AuctionSystemValueDataEntry* entry);

	UInt32 GetSystemValue(AuctionSystemValueType type);

private:
	SystemValueMap	m_SystemValues;
};



#endif //_CL_SYSTEM_VALUE_DATAENTRY_H_

