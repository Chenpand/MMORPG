#ifndef _CL_ACTIVEEVENT_DATAENTRY_H_
#define _CL_ACTIVEEVENT_DATAENTRY_H_

#include "CLDefine.h"
#include "CLItemDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>


//�data
class ActiveTaskDataEntry : public Avalon::DataEntry
{
public:
	ActiveTaskDataEntry();
	~ActiveTaskDataEntry();

public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 20; };

public:
	//�����id
	UInt32	id;
	//����
	std::vector<ItemReward>	rewards;
	//˫��VIP�ȼ�
	UInt32	vipLv;
	//�ģ��id
	UInt32  templateID;
	//ǰ������id
	UInt32  preTaskID;
	//��������id
	UInt32  nextTaskID;
	//�����
	UInt32 sortPriority;
	//��ȡ����
	UInt32 takeCost;
	//��չ����
	UInt32 param1;
	//��Ӧ����id
	UInt32 dungeonId;
	//�Ƿ����
	UInt32 isAbandon;
	//���ĵĵ���
	std::vector<ItemReward> consumeItems;
	//����ѭ������
	Int64 taskCycleCount;
	//����ѭ������key
	std::string taskCycleKey;
	//�����������key
	std::string overTaskNumKey;
	//δ�����������key
	std::string failedTaskNumKey;
	//�Ӧ������
	std::string applyChannel;
	//���������1���˺ţ�
	UInt8 taskBelong;
	// ��չ����2
	std::vector<UInt32> parm2;
	//�˺����ƴ���
	UInt32 accountTotalSubmitLimit;
};

typedef std::map<UInt32, std::vector<ActiveTaskDataEntry*>> TempToActives;

class ActiveTaskDataEntryMgr : public Avalon::DataEntryMgrBase<ActiveTaskDataEntry>
	, public Avalon::Singleton<ActiveTaskDataEntryMgr>
{
public:
	ActiveTaskDataEntryMgr();
	~ActiveTaskDataEntryMgr();

public:
	bool AddEntry(ActiveTaskDataEntry* dataEntry);
	void GetActives(UInt32 tempId, std::vector<ActiveTaskDataEntry*>& vecActives);
private:
	TempToActives			m_actives;
};

//ƣ�Ͳ���data
class FatigueMakeUpData : public Avalon::DataEntry
{
public:
	FatigueMakeUpData();
	~FatigueMakeUpData();

public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);

public:
	//keyid �ȼ�
	UInt32	id;
	//�͵��һؾ���
	UInt64	lowExp;
	//�͵��һ���Ҫ���Ļ���ID
	UInt32	lowNeedMoneyID;
	//�ߵ��һؾ���
	UInt64	hiExp;
	//�ߵ��һ���Ҫ���Ļ���ID
	UInt32	hiNeedMoneyID;
	//ƣ�Ͳ�������
	UInt32	fatigueMax;
	//vipƣ�Ͳ�������
	std::vector<UInt32>  vipMaxs;
};

class FatigueMakeUpDataMgr : public Avalon::DataEntryMgrBase<FatigueMakeUpData>
	, public Avalon::Singleton<FatigueMakeUpDataMgr>
{

public:
	FatigueMakeUpDataMgr();
	~FatigueMakeUpDataMgr();
};

//�����һؼ۸��
class FatigueMakeUpPrice : public Avalon::DataEntry
{
public:

	FatigueMakeUpPrice();
	~FatigueMakeUpPrice();

	UInt32 GetKey() const{ return id; }
	bool Read(Avalon::DataRow& row);

	//key
	UInt32 id;
	//ƣ�ͱ�����
	std::vector<UInt32>	fatigueSection;
	//�͵��һص���
	UInt32	lowPrice;
	//�ߵ��һص���
	UInt32	hiPrice;
};

class FatigueMakeUpPriceMgr : public Avalon::DataEntryMgrBase<FatigueMakeUpPrice>
	, public Avalon::Singleton<FatigueMakeUpPriceMgr>
{

public:
	FatigueMakeUpPriceMgr();
	~FatigueMakeUpPriceMgr();

	UInt32	GetPrice(UInt32 fatigue, bool bLow);
};

//������������data
class DungeonMakeUpData : public Avalon::DataEntry
{
public:
	DungeonMakeUpData();
	~DungeonMakeUpData();

public:
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow& row);
	bool ReadSplitStr(Avalon::DataRow& row, std::vector<UInt64>& vec);
	bool ReadSplitStr(Avalon::DataRow& row, std::vector<UInt32>& vec);
public:
	UInt32	id;
	//�ȼ�
	UInt32	level;
	//��Ӧ���³�ID
	UInt32	dungeonID;
	//��ͨ�һص���ID
	std::vector<UInt32>	normalItems;
	//������ͨ�һز�������
	std::vector<UInt64>	normalNum;
	//�����һص���ID
	std::vector<UInt32>	perfectItems;
	//���θ����һز�������
	std::vector<UInt64>	costNum;
	//��ͨ���Ľ��
	UInt32	goldCost;
	//�������ĵ�ȯ
	UInt32	pointCost;
	//��������
	UInt32	countMax;
	//vip��������
	UInt32	vipCountMax;
	//vip����ȼ�
	UInt32	vipLevel;
};

class DungeonMakeUpDataMgr : public Avalon::DataEntryMgrBase<DungeonMakeUpData>
	, public Avalon::Singleton<DungeonMakeUpDataMgr>
{

public:
	DungeonMakeUpDataMgr();
	~DungeonMakeUpDataMgr();

public:
	bool AddEntry(DungeonMakeUpData* dataEntry);
	DungeonMakeUpData* GetData(UInt32 dungeonID, UInt32 level);

private:
	void GetDatas(UInt32 dungeonID, std::vector<DungeonMakeUpData*>& datas);

	std::map<UInt32, std::vector<DungeonMakeUpData*>>	m_dungeonIdToDatas;
};

//�׶������
class GiftPhaseData : public Avalon::DataEntry
{
public:
	GiftPhaseData();
	~GiftPhaseData();

public:
	UInt32 GetKey()	const { return id; }
	bool Read(Avalon::DataRow& row);

	UInt32	id;
	//�¼����
	UInt8	eventType;
	//����ֵ
	UInt32	value;
	//��һ������ֵ
	UInt32	nextValue;
	//����
	std::vector<ItemReward>	rewards;
};

//�׶��������
class GiftPhaseDataMgr : public Avalon::DataEntryMgrBase<GiftPhaseData>
	, public Avalon::Singleton<GiftPhaseDataMgr>
{
public:
	GiftPhaseDataMgr();
	~GiftPhaseDataMgr();

public:
	bool LoadData(const std::string& filename);
	bool AddEntry(GiftPhaseData* dataEntry);

	//��ȡtype id����һ�����
	GiftPhaseData* GetNextGift(UInt8 type, UInt32 id);

private:
	std::map<UInt8, std::vector<GiftPhaseData*>>	m_giftsTypeMap;

};

/**
*@brief ��Ӫ�������
*/
class OpActivityDataEntry : public Avalon::DataEntry
{
public:
	OpActivityDataEntry() {}
	~OpActivityDataEntry() {}

	UInt32 GetKey() const { return id; }
	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 31; };

public:
	// ��Ӫ�id
	UInt32 id;
	// ��Ӫģ�����
	UInt32 tmpType;
	// ���
	std::string name;
	// ���ǩ---����
	UInt8 tag;
	// ��ǩ����ʱ��---����
	UInt32 tagEngTime;
	// ׼��ʱ��
	UInt32 prepareTime;
	// ��ʼʱ��
	UInt32 startTime;
	// ����ʱ��
	UInt32 endTime;
	// �����ȼ�����(��ҵȼ�)
	UInt16 playerLevelLimit;
	// ��С�ȼ�
	UInt32	minLevel;
	// ���ȼ�
	UInt32	maxLevel;
	// Ҫ������Ŀ���ʱ��
	UInt32  needStartServiceTime;
	// �·����Ϸ����·����ڼ��쿪ʼ
	std::string startTimeFromService;
	// �·����Ϸ����·����ڼ������
	std::string endTimeFromService;
	// �·����Ϸ������·����ڼ��쿪ʼ
	std::string startTimeFromFirstService;
	// �·����Ϸ������·����ڼ������
	std::string endTimeFromFirstService;
	// �������
	std::string	openInterval;
	// �رռ��
	std::string	closeInterval;
	// ����
	std::string desc;
	// ��������
	std::string ruleDesc;
	// ѭ������---����
	UInt8 circleType;
	// ��չ����
	UInt32 parm;
	// ��չ����2
	std::vector<UInt32> parm2;
	// ����
	std::vector<UInt32> taskIds;
	// logo
	std::string logoDesc;
	// �����·�����
	UInt8 superNewServiceActFlag;
	// count����
	std::string countParam;
	//��չ����3
	std::vector<UInt32> parm3;
	//�Ԥ����·��
	std::string  prefabPath;
	//����ͼ·��
	std::string  logoPath;
	//�ַ�������
	std::vector<std::string> strParams;
};

class OpActivityDataEntryMgr : public Avalon::DataEntryMgrBase<OpActivityDataEntry>
	, public Avalon::Singleton<OpActivityDataEntryMgr>
{
public:
	OpActivityDataEntryMgr() {}
	~OpActivityDataEntryMgr() {}
};

/**
*@brief ��Ӫ�����������
*/
class OpActivityTaskDataEntry : public Avalon::DataEntry
{
public:
	OpActivityTaskDataEntry() {}
	~OpActivityTaskDataEntry() {}

	UInt32 GetKey() const { return id; }
	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 22; };

public:
	// ����id
	UInt32 id;
	// ��������
	std::string taskName;
	// ��Ӫ�id
	UInt32 opAvtivityId;
	// ��Ӫ�����
	UInt32 opActivityType;
	// ǰ������id
	UInt32 preTaskId;
	// ��������id
	UInt32 nextTaskId;
	// ������ɼ���
	UInt32 completeCount;
	// ������
	std::vector<ItemReward> rewards;
	// ��������
	std::string taskDesc;
	// �������
	std::vector<UInt32>	variables;
	// �������
	std::vector<UInt32>	variables2;
	// counter
	std::vector<CounterItem> counters;
	// �����������
	std::vector<std::string> varProgressName;
	// �����ȼ�����(��ҵȼ�)
	UInt16 playerLevelLimit;
	// �˺�ÿ���콱����
	UInt32 accountDailySubmitLimit;
	// �˺�ÿ���콱����
	UInt32 accountTotalSubmitLimit;
	// ˢ������
	UInt32 restType;
	// ������
	UInt32 acceptType;
	UInt32 eventType;
	UInt32 subType;
	// �˻�ÿ���콱���ƴ���
	UInt32 accountWeeklySubmitLimit;
	// �˺�����
	UInt32 accountTask;
};

/**
*@brief ��Ӫ����������Զ�������
*/
struct CustomLessOpActTaskData
{
	bool operator()(OpActivityTaskDataEntry* lhs, OpActivityTaskDataEntry* rhs)
	{
		return lhs->id < rhs->id;
	}
};

class OpActivityTaskDataEntryMgr : public Avalon::DataEntryMgrBase<OpActivityTaskDataEntry>
	, public Avalon::Singleton<OpActivityTaskDataEntryMgr>
{
public:
	OpActivityTaskDataEntryMgr() {}
	~OpActivityTaskDataEntryMgr() {}
};

/**
*@brief �ģ���������
*/
class ActivityTemplateDataEntry : public Avalon::DataEntry
{
public:
	ActivityTemplateDataEntry() {}
	~ActivityTemplateDataEntry() {}

	UInt32 GetKey() const { return id; }
	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 2; };

public:
	// �id
	UInt32 id;
	// �����
	UInt8 type;
};

class ActivityTemplateDataEntryMgr : public Avalon::DataEntryMgrBase<ActivityTemplateDataEntry>
	, public Avalon::Singleton<ActivityTemplateDataEntryMgr>
{
public:
	ActivityTemplateDataEntryMgr() {}
	~ActivityTemplateDataEntryMgr() {}

	bool AddEntry(ActivityTemplateDataEntry* entry);
	UInt8 GetActivityTemplateType(UInt32 activityId);

private:
	std::map<UInt32, UInt8> m_ActivityTempToType;
};


#define OPACT_FATIGUE_BURNING_VARIABLE_SIZE 4


/**
*@brief �ϵͳ���ñ�
*/
class ActivitySystemConfigDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return actSytem; }

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 8; };

public:
	// id(������)
	UInt16 id;
	// �ϵͳ
	UInt32 actSytem;
	// ��Ӫ��б�
	std::vector<UInt32> opActList;
	// �̳ǵ����б�
	std::vector<UInt32> mallItemList;
	// ��б�
	std::vector<UInt32> actList;
	// ������Ϣ�б�
	std::vector<UInt32> pushInfoList;
	// �Ƿ�ر�
	bool isClose;
	// �ع�id
	UInt32 			returnActId;
};

typedef Avalon::DataEntryMgr<ActivitySystemConfigDataEntry> ActivitySystemConfigDataEntryMgr;

/**
*@brief �ҳǩ��
*/
class ActivityTabDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 3; };

public:
	// ҳǩid
	UInt32 id;
	// ҳǩ��
	std::string name;
	// ҳǩ�µĻid
	std::vector<UInt32> actIds;
};

typedef Avalon::DataEntryMgr<ActivityTabDataEntry> ActivityTabDataEntryMgr;


#endif
