#ifndef _GAMBING_MGR_
#define _GAMBING_MGR_


#include <AvalonObjectPool.h>
#include <AvalonSimpleTimer.h>
#include <CLGambingProtocol.h>
#include <CLActivityDefine.h>

#include "GASPlayer.h"
#include "GambingRecord.h"

struct OpActivityInfo;

/**
*@brief �ᱦ��Ʒ״̬
*/
enum GambingItemStatus
{
	// ��ʼֵ
	GIS_INIT = 0,
	// ׼��
	GIS_PREPARE,
	// ����
	GIS_SELLING,
	// ����
	GIS_SOLD_OUE,
	// ����
	GIS_LOTTERY,
	// �¼�
	GIS_OFF_SALE,
};

/**
*@brief �ҵĶᱦ״̬
*/
enum GambingMineStatus
{
	// ��ʼֵ
	GMS_INIT = 0,
	// �ᱦ�ɹ�
	GMS_SUCCESS,
	// �ᱦʧ��
	GMS_FAILE,
	// �ȴ�����
	GMS_WAIT,
};

/**
*@brief �ᱦ�н����
*/
enum GambingLotteryFlag
{
	// û���н�
	GLF_NONE = 0,
	// �н�
	GLF_GET_IT,
};

/**
*@brief �ᱦ��Ʒ����
*/
struct GambingItemPool
{
	AVALON_DEFINE_OBJECTPOOL(GambingItemPool, Avalon::NullMutex)

	GambingItemPool()
	{
		gambingItemId = 0;
		gambingItemNum = 0;
		sortId = 0;
		itemDataId = 0;
		totalGroups = 0;
		copiesOfEachGroup = 0;
		costCurrencyId = 0;
		unitPrice = 0;
		coolDownIntervalBetweenGroups = 0;
		weightingStepSize = 0;
		weightingValue = 0;
		restGroups = 0;
		itemPoolStatus = GIS_INIT;
	}

	std::string LogFormatStr() const
	{
		std::ostringstream oss;
		oss << "GambingItemPool(id=" << gambingItemId << ", restGroups=" << restGroups 
			<< ", itemPoolStatus=" << (UInt32)itemPoolStatus << ")";
		return oss.str();
	}

	// ��Ʒid
	UInt32 gambingItemId;
	// �ᱦ��Ʒ����
	UInt32 gambingItemNum;
	// ����
	UInt16 sortId;
	// ���߱�id
	UInt32 itemDataId;
	// ������
	UInt16 totalGroups;
	// ÿ�����
	UInt32 copiesOfEachGroup;
	// ���Ļ���id
	UInt32 costCurrencyId;
	// ���ۣ�һ�ݣ�
	UInt32 unitPrice;
	// ���cd
	UInt32 coolDownIntervalBetweenGroups;
	// ÿ�ݽ���
	std::vector<ItemReward> rewardsPerCopy;
	// ��Ȩ����
	UInt32 weightingStepSize;
	// ��Ȩֵ
	UInt32 weightingValue;

	// ʣ������
	UInt16 restGroups;
	// ״̬
	GambingItemStatus itemPoolStatus;
};

/**
*@brief �ᱦ��Ʒkey
*/
struct GambingItemGroupKey
{
	GambingItemGroupKey()
	{
		gambingItemId = 0;
		groupId = 0;
	}

	GambingItemGroupKey(UInt32 itemId, UInt16 groupId) : gambingItemId(itemId), groupId(groupId) {}

	bool operator<(const GambingItemGroupKey& other) const
	{
		if (gambingItemId < other.gambingItemId)
		{
			return true;
		}
		if (other.gambingItemId < gambingItemId)
		{
			return false;
		}

		if (groupId < other.groupId)
		{
			return true;
		}
		if (other.groupId < groupId)
		{
			return false;
		}

		return false;
	}

	UInt32 gambingItemId;
	UInt16 groupId;
};

/**
*@brief �ᱦ��Ʒ����Ϣ
*/
class GambingItemGroupInfo
{
	AVALON_DEFINE_OBJECTPOOL(GambingItemGroupInfo, Avalon::NullMutex)

	GambingItemGroupInfo()
	{
		gambingItemId = 0;
		groupId = 0;
		restCopies = 0;
		groupStatus = GIS_INIT;
		sellBeginTime = 0;
		soldOutTimestamp = 0;
	}

	std::string LogFormatStr() const
	{
		std::ostringstream oss;
		oss << "GambingItemGroupInfo(id=" << gambingItemId << ", gId=" << groupId << ", rCs=" << restCopies << ", status=" 
			<< (UInt32)groupStatus << ", bT=" << sellBeginTime << ")";
		return oss.str();
	}

	// ��Ʒid
	UInt32 gambingItemId;
	// ��id
	UInt16 groupId;
	// ʣ�����
	UInt32 restCopies;
	// ״̬(��Ӧö�� GambingItemStatus)
	GambingItemStatus groupStatus;
	// ��ʼ����ʱ��
	UInt32 sellBeginTime;
	// ����ʱ��
	UInt32 soldOutTimestamp;
};

/**
*@brief �ᱦ����������
*/
struct PayingGambleContext
{
	PayingGambleContext()
	{
		itemPool = NULL;
		groupInfo = NULL;
	}

	// �ᱦ��Ʒ����
	GambingItemPool* itemPool;
	// �ᱦ��Ʒ����Ϣ
	GambingItemGroupInfo* groupInfo;
	// ����Э��
	CLProtocol::PayingGambleReq payingReqProtocol;
	// ����Э��
	CLProtocol::PayingGambleRes payingResProtocol;
};

typedef std::vector<GambingRecord*> GambingRecordVec;

/**
*@brief �ᱦ������
*/
class GambingMgr : public Avalon::Singleton<GambingMgr>
{
	typedef std::vector<std::string> GambingParticipantVec;

public:
	GambingMgr();
	~GambingMgr();

public:
	bool OnInit(const OpActivityInfo* opAct);
	void Final();

	/**
	*@brief ���ر��
	*/
	bool LoadTable(const OpActivityInfo* opAct, bool bReload = false);

	/**
	*@brief ���ر��
	*/
	bool ReloadTable();

	/**
	*@brief �������ݿ�
	*/
	bool LoadDB(const OpActivityInfo* opAct);

	/**
	*@brief ���ضᱦ��¼
	*/
	bool OnLoadGambingRecord(CLRecordCallback* callback);

	/**
	*@brief ���ضᱦ������¼
	*/
	bool OnLoadGambingLotteryRecord(CLRecordCallback* callback);

public:
	/**
	*@brief ���û�ȡ�ᱦ�״̬
	*/
	void SetGambingActStatus(ActivityState status) { m_GambingActivityStatus = status; }
	ActivityState GetGambingActStatus() const { return m_GambingActivityStatus; }

public:
	/**
	*@brief ��Ӷᱦ��¼
	*/
	bool AddGambingRecord(GambingRecord* record);

	/**
	*@brief ��Ӷᱦ������¼
	*/
	bool AddGambingLotteryRecord(const GambingItemPool* itemPool, const GambingItemGroupInfo* groupInfo, const std::string& gainer);
	void AddGambingLotteryRecord(GambingLotteryRecord* record);

	/**
	*@brief ��Ӷᱦ����Ϣ
	*/
	bool AddGambingItemGroupInfo(GambingItemGroupInfo* group);

public:
	/**
	*@brief ����һ��ᱦ��Ʒ����
	*/
	GambingItemPool* FindGambingItemPool(UInt32 gambingItemId);

	/**
	*@brief ����һ��ᱦ��Ʒ
	*/
	GambingItemGroupInfo* FindGambingGroupInfo(UInt32 gambingItemId, UInt16 groupId);

	/**
	*@brief ���Ҷᱦ��¼
	*/
	GambingRecord* FindGambingRecord(const std::string& ownerUId, UInt32 gambingItemId, UInt16 groupId);
	void FindGambingRecords(const std::string& ownerUId, GambingRecordVec& records);

	/**
	*@brief ���Ҳ�����
	*/
	void FindGambingParticipants(UInt32 gambingItemId, UInt16 groupId, GambingParticipantVec& participants);

	/**
	*@brief ����һ��ᱦ�н�����
	*/
	GambingLotteryRecord* FindGambingLotteryRecord(UInt32 gambingItemId, UInt16 groupId);

public:
	/**
	*@brief tick
	*/
	void OnTick(const Avalon::Time &now);

	/**
	*@brief �׼��
	*/
	void OnActivityPrepare(const OpActivityInfo* opAct);

	/**
	*@brief �����
	*/
	void OnActivityStart(const OpActivityInfo* opAct);

	/**
	*@brief ��ر�
	*/
	void OnActivityEnd(const OpActivityInfo* opAct);

public:
	/**
	*@brief �ᱦ��Ʒ������(����������һ��)
	*/
	void OnGambingItemGroupSoldOut(PayingGambleContext& payingGambleContext, GASPlayer *player);

	/**
	*@brief �ᱦ��Ʒ����
	*/
	void OnGambingItemGroupLottery(GambingItemGroupInfo* gambingItem);

	/**
	*@brief ���²�������Ϣ
	*/
	void UpdateParticipantInfo(GambingRecord* gambingRecord);

	/**
	*@brief ����ᱦȨ��
	*/
	void CalculateGambingWeight(GambingItemPool* itemPool, GambingRecord* gambingRecord);

	/**
	*@brief ����
	*/
	//void OnParticipantNameChanged(ObjID_t roleId, const std::string& name);

public:
	/**
	*@brief ��ѯ�ᱦ��Ʒ
	*/
	void QueryGambingItems(GASPlayer* player);

	/**
	*@brief ��ȡ������Ʒ��Ϣ
	*/
	void GetGambingItemInfo(const GambingItemPool* itemPool, const GambingItemGroupInfo* groupInfo, const std::string& playerUId, CLProtocol::GambingItemInfo& itemInfo);

	/**
	*@brief ��ѯ�ҵĶᱦ��Ʒ
	*/
	void QueryMineGambingItems(GASPlayer* player);

	/**
	*@brief ����ᱦ��Ʒ
	*/
	void PayingGamble(GASPlayer* player, const CLProtocol::PayingGambleReq& req);

	/**
	*@brief ��ѯ�ᱦ��¼
	*/
	void QueryGambingRecordData(GASPlayer* player);

private:
	/**
	*@brief ����
	*/
	void _Destroy();

	/**
	*@brief �����ᱦ��
	*/
	GambingItemGroupInfo* _CreateGambingItemGroup(GambingItemPool* itemPool);

	/**
	*@brief ��ʼ����������
	*/
	bool _OnInitDataFromDB(GambingRecord* gambingRecord);

	/**
	*@brief ��ʼ����һ��ᱦ
	*/
	void _OnInitGambingFirstGroup(const OpActivityInfo* opAct, GambingItemPool* itemPool);

	/**
	*@brief �������ʱ��
	*/
	void _CalculateGroupSellBeginTimeSec(GambingItemPool* itemPool, GambingItemGroupInfo* groupInfo);

private:
	// �ᱦ����
	std::vector<GambingItemPool*> m_GambingItemPool;
	// ��������ʱ����ʱ����
	std::vector<GambingItemPool*> m_TmpGambingItemPool;

	// �ᱦ��Ʒ����Ϣ
	std::map<GambingItemGroupKey, GambingItemGroupInfo*> m_GambingItemGroups;

	// �ᱦ��¼
	HashMap<std::string, GambingRecordVec> m_GambingRecords;

	// ������
	std::map<GambingItemGroupKey, GambingParticipantVec> m_ParticipantsMap;

	// ������¼
	std::vector<GambingLotteryRecord*> m_GambingLotteryRecord;


	// ÿ�붨ʱ��
	Avalon::SimpleTimer m_SecondPerTimer;
	// �ᱦ�״̬
	ActivityState m_GambingActivityStatus;
};

#endif