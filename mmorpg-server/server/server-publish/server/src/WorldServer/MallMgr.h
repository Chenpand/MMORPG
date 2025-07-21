#ifndef _WS_MALL_MGR_H_
#define _WS_MALL_MGR_H_

#include <CLDefine.h>
#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>
#include "CLMallProtocol.h"

#include "MallScript.h"
#include "MallItemBuyRecord.h"

class WSPlayer;
struct LoginPushInfo;
class MallItem;
class CLRecordCallback;

// �̳�����״̬
enum MallGiftPackActivityState
{
	MGPAS_INVALID = 0,
	MGPAS_OPEN,			// ����
	MGPAS_CLOSED,		// �ر�
};

// �̳���������
enum MallPushCond
{
	// ������
	MALL_PUSH_COND_NONO = 0,
	// �����������
	MALL_PUSH_COND_PLAYER_LEVEL_UP,
};

// ��Ʒ���ϼ�
#define MALL_GOODS_WAIT_ON_SALE	1


//#define ITEM_NUM_PER_PAGE	999

struct ItemKey
{
	ItemKey(UInt8 type, UInt8 subType, UInt8 job) : m_type(type), m_subType(subType), m_job(job){}

	UInt8 m_type;
	UInt8 m_subType;
	UInt8 m_job;

	bool operator < (const ItemKey& other) const
	{
		if (m_type < other.m_type)
		{
			return true;
		}
		if (other.m_type < m_type)
		{
			return false;
		}
		
		if (m_subType < other.m_subType)
		{
			return true;
		}
		if (other.m_subType < m_subType)
		{
			return false;
		}

		if (m_job < other.m_job)
		{
			return true;
		}
		if (other.m_job < m_job)
		{
			return false;
		}

		return false;
	}
};


typedef std::vector<MallItemInfo*>			VecItem;

/**
*@brief �̳ǹ�����
*/
class MallMgr : public Avalon::Singleton<MallMgr>
{

public:

	const static UInt16	INTIMACY_INC_BY_SEND_GIFT = 5;
	const static UInt32 BASE_LIMITED_ITEM_ID = 10000;
	const static UInt32 NORMAL_LIMITED_ITEM_ID = 11000;
	const static UInt32 FESTIVAL_LIMITED_ITEM_ID = 20000;
	const static UInt32 GOLDSTONE_ITEM_ID = 30000;
	const static UInt32 SILVERSTONE_ITEM_ID = 40000;
	const static UInt32 GM_GEN_UID = 1000000000;

public:
	MallMgr();
	~MallMgr();

	/**
	*@brief ��ʼ��
	*/
	bool Init();

	/**
	*@brief ���ر�����
	*/
	void ReloadDataTable();

	/**
	*@brief ������Դ
	*/
	void Destroy();

	/**
	*@brief ��ѯ�̵���Ʒ
	*/
	void OnQueryItem(WSPlayer* player, UInt32 mallItemId);
	UInt32 OnQueryItems(WSPlayer* player, UInt8 tagType, UInt8 type, UInt8 subType, UInt8 job, UInt8 moneyType, UInt8 isPersonalTailor);
	void GetQueryItems(WSPlayer* player, const CLProtocol::SWRefreshLimiteBuyNum& protocol, std::vector<MallItemInfo>& outGoodsInfos);
	void QueryAllItems(VecItem& items);

	std::string EncodRewardItems(std::vector<ItemReward>& items);
	bool DecodeRewardItems(std::vector<ItemReward>& items, std::string strItems);

	/**
	*@brief GM�ӿ�
	*/

	//ת��gm��Ʒuid���ڲ������uid,����0�Ǵ���
	UInt32 TransUidGMToLocal(UInt32 gmUid);
	UInt32 TransUidLocalToGm(UInt32 uid);

	//GM������Ʒ,������ӻ����
	void GmSetGoods(MallItemInfo* good);
	bool GmDelGoods(UInt32 goodsId);

	//���»�ɾ��һ����Ʒ
	void UpdateItem(VecItem& tmpItems, MallItemInfo* good);
	void DeleteItem(VecItem& tmpItems, UInt32 goodsId);

	/**
	*@brief ����
	*/
	UInt32 Buy(WSPlayer* player, UInt32 itemId, UInt16 num);

	/**
	*@brief ��������
	*/
	UInt32 BatchBuy(WSPlayer* player, std::vector<ItemReward>& items);

	/**
	*@brief ��ѯ��Ʒ�����ϸ
	*/
	UInt32 OnQueryItemDetail(UInt32 mallItemId, std::vector<MallGiftDetail>& detail);

	/**
	*@brief ���ָ������Id�ļ۸�
	*/
	void OnGetItemPrice(UInt32 itemId, UInt32& outPrice, UInt8& outMoneyT);

	/**
	*@brief ��鹺�������ķ��ش���
	*/
	void OnCheckBuyRet(ObjID_t uPlayerId, UInt32 retCode, UInt32 uId, const std::vector<UInt64>& itemUids, UInt16 uNum, ObjID_t uReceiver, Int32 restLimitNum);

	/**
	 *@brief �������
	 */
	void Buy(WSPlayer* pPlayer, UInt32 uId, UInt16 uNum, ObjID_t uReceiver);

	/**
	 *@brief �𱴹������
	 */
	void Buy(WSPlayer* pPlayer, UInt32 uId, UInt16 uNum, ObjID_t uReceiver, UInt32 price, UInt32 billingId, const std::string& orderId, const std::string& coins,
		const std::string& moneyType, const std::string& taocanId);

	/**
	*@brief ͬ����������
	*/
	void SyncLimitedItem(WSPlayer *pPlayer);

	/**
	*@brief ���ݵ���id��ȡ�̳ǵ�������
	*/
	void GetMallItemByItemId(WSPlayer *pPlayer, UInt32 itemId);

	/**
	*@brief �����̳���Ʒ
	*/
	void PushMallItems(WSPlayer *player, const MallItemInfosVec& mallItems);

public:
	/**
	*@brief ���������Ʒ
	*/
	void ClearLimitedItem();

	/**
	*@brief ���һ��������Ʒ
	*/
	void AddNormalLimitedItem(const MallItemInfo &item);

	/**
	*@brief ��ӿ���������Ʒ
	*/
	void AddGSLimitedItem(const MallItemInfo &item);

	/**
	*@brief ��ӽ���������Ʒ
	*/
	void AddFestivalLimitedItem(const MallItemInfo &item);
	
	/**
	*@brief ��ӽ�ʯ�̳���Ʒ
	*/
	void AddGoldStoneItem(const MallItemInfo &item);

	/**
	*@brief �����ʯ�̳���Ʒ
	*/
	void AddSilverStoneItem(const MallItemInfo &item);

	/**
	*@brief ���ù㲥��Ϣ
	*/
	void SetNotifyInfo(const std::string &strNotify);

	/**
	*@brief �����Ʒ
	*/
	void AddMallItem(UInt8 type, UInt8 subType, UInt8 job, UInt8 moneyType, const char *szItem);
	bool AddMallItem(MallItemInfo* info);
	bool AddMallWaitGoods(MallItemInfo* info);

public://�̳���Ұ�������
	/**
	*@brief ������Ұ���ʱ�������
	*/
	void SetMallGiftPackNum(WSPlayer* pPlayer, UInt32 giftPackId, Int32 restLimitNum);

	/**
	*@brief ��ȡmallItemInfo
	*/
	MallItemInfo* GetMallItemInfo(UInt32 uid);

	/**
	*@brief ��ȡ������Ʒ
	*/
	void GetIdxItems(ItemKey& key, VecItem& items);

	void GetRechargePushItems(WSPlayer* pPlayer);

	void BuyRechargePushItem(WSPlayer* pPlayer, UInt32 pushId);

	void OnSceneBuyRechargePushItemRes(WSPlayer* pPlayer, UInt32 retCode, UInt32 pushId);

	void OnCheckRechargePushRes(WSPlayer* pPlayer, UInt32 retCode, UInt32 pushId);

public://��Ʒ�ϼ��¼����
	//�ϼ���Ʒ
	bool OnSaleMallWaitGoods(UInt32 goodsId);

	//�¼���Ʒ
	bool OffSaleMallWaitGoods(UInt32 goodsId);

public://�̳���ʱ�������
	/**
	*@brief ͬ������״̬
	*/
	void SyncMallGiftPackActivityState(WSPlayer *pPlayer);

	/**
	*@brief ��ȡʱ�����0���ʱ��
	*/
	UInt32 GetTimeOnZero(Avalon::Time& time);

public://��Ʒ�������
	/**
	*@brief ��ȡ��Ӫ�������
	*/
	//UInt32 GetOpActivityDiscountRate();

	/**
	*@brief ����һЩ�����ı������
	*/
	UInt32 GenDiscountRate(MallItemInfo* goods, UInt32 curTime);

	/**
	*@brief �������մ�����
	*/
	float GenDiscountRate(UInt32 rate);

public://��¼�������
	/**
	*@brief �������¼���Ͱ󶨵���Ʒ��Ϣ
	*/
	void SetLoginPushBindGoodsInfo(LoginPushInfo* pushInfo);

public://�˺��޹����
	/*
	*@brief �����ݿ�����˺Ź����¼
	*/
	void LoadMallItemAccountBuyRecordFromDB(CLRecordCallback* callback);

	/*
	*@brief ������Ʒ�˺Ź����¼
	*/
	MallItemAccountBuyRecord* FindMallItemAccountBuyRecord(UInt32 accid, UInt32 mallItemId);
	MallItemAccountBuyRecord* FindMallItemAccountBuyRecord(const MallItemAccountBuyIndex& index);

	/*
	*@brief �����Ʒ�˺Ź����¼
	*/
	void AddMallItemAccountBuyRecord(const MallItemAccountBuyIndex& index, MallItemAccountBuyRecord* record, bool saveDB = true);
	void AddMallItemAccountBuyRecord(UInt32 accid, UInt32 mallItemId, UInt32 buyNum);

	/*
	*@brief ��ȡ�˺���ʣ��ɹ�������
	*/
	UInt32 GetAccountRestBuyNum(UInt32 accid, const MallItemInfo* mallItem);

	/**
	*@brief �Ƿ��˺����޹�
	*/
	bool IsLimitBuyOnAccount(UInt32 accid, const MallItemInfo* mallItem, UInt32 buyNum);

public:
	/*
	*@brief �����ݿ�����Զ�����Ʒ
	*/
	void LoadCustomerMallItemFromDB(WSPlayer* player, CLRecordCallback* callback);

	/*
	*@brief �����Զ�����Ʒ
	*/
	MallItem* CreateCustomerMallItem(WSPlayer* player, const MallItemInfo* mallItem);

	/*
	*@brief �����Զ�����Ʒ
	*/
	MallItem* FindCustomerMallItem(ObjID_t roleid, UInt32 mallItemId);

	/*
	*@brief ����Զ�����Ʒ
	*/
	void AddCustomerMallItem(MallItem* mallItem);

public:
	/**
	*@brief ����
	*/
	void OnTick(const Avalon::Time &now);

	/**
	*@brief ����
	*/
	void OnDayChanged();

	/**
	*@brief ��ҵȼ��ı�
	*/
	void OnPlayerLevelChanged(WSPlayer* player, UInt16 oldLv);

private:
	/**
	*@brief ���ر�����
	*/
	bool LoadData(bool isReload = false);

	/**
	*@brief �����ݿ�����
	*/
	void LoadDBConfig();

	/**
	*@brief DB����
	*/
	void InsertDB(MallItemInfo* good);
	void DeleteDB(MallItemInfo* good);
	void UpdateDB(MallItemInfo* newGood, MallItemInfo* oldGood);

	/**
	*@brief ��ȡ������Ʒ
	*/
	void GetHotGoods(UInt8 moneyType, std::vector<CLProtocol::QueryGood>& queryGoods);

	/**
	*@brief ��ȡ˽�˶�����Ʒ
	*/
	void GetPersonalTailorGoods(std::vector<CLProtocol::QueryGood>& queryGoods);

	/**
	*@brief ��ȡ��ͨ��Ʒ
	*/
	UInt32 GetCommonGoods(WSPlayer* player, ItemKey& key, std::vector<CLProtocol::QueryGood>& queryGoods);

	/**
	*@brief ����������Ʒ����
	*/
	void UpdateGiftGood(MallItemInfo& good, WSPlayer* player);

	/**
	*@brief �����������߳�����Ϣ
	*/
	void ResetLimitedItemSelledInfo();
	
	void DecodeItemInfo(MallItemInfo &item, const char *szItem);

	/**
	 *@brief ������������
	 */
	void BuyLimitedItem(WSPlayer* pPlayer, UInt32 uId, UInt16 uNum, ObjID_t uReceiver, UInt32 price, UInt32 billingId, const std::string& orderId, const std::string& coins,
		const std::string& moneyType, const std::string& taocanId);

	/**
	*@brief ͬ��������������
	*/
	void SyncLimitedItemNum(WSPlayer *pPlayer, MallItemInfo *pItem);

	/**
	*@brief ���õ�ǰ��ȯ����
	*/
	void SetCurJQLBActivity();

	/**
	*@brief �Ƿ��ϼ�
	*/
	bool IsOnSale(WSPlayer* player, MallItemInfo* mallItem, UInt32 limitTime);

private:
	// ������Ʒ
	std::map<ItemKey, VecItem>			m_idxItems;

	// ������Ʒ(��Ʒid-->��Ʒ��Ϣ��)
	std::map<UInt32, MallItemInfo*>		m_allItems;
	std::map<UInt32, MallItemInfo*>		m_TempItems;

	// ���ϼ���Ʒ
	std::map<UInt32, MallItemInfo*>		m_waitGoods;

	// �����������������
	std::map<UInt64, MallBatchBuyCtx>	m_PlayersBatchBuyCtx;

	// ��ʱ����״̬
	MallGiftPackActivityState			m_GiftActivityState;

	// ���ʱ��
	Avalon::SimpleTimer					m_secTimer;

	// �Զ�����Ʒ
	HashMap<UInt64, std::vector<MallItem*> > m_MallItemsOnPlayer;

	// �˺�����Ʒ�����¼
	std::map<MallItemAccountBuyIndex, MallItemAccountBuyRecord*> m_AccountMallItemBuyRecords;

	//-----------------��������--------------------------------
	// һ��������Ʒ
	std::map<UInt32, MallItemInfo>	m_mapNormalItem;
	// ����������Ʒ
	std::map<UInt32, MallItemInfo>	m_mapGSItem;
	// ����������Ʒ
	std::map<UInt32, MallItemInfo>	m_mapFestivalItem;
	// ��ʯ�̳���Ʒ
	std::map<UInt32, MallItemInfo>	m_mapGoldStoneItem;
	// ��ʯ�̳���Ʒ
	std::map<UInt32, MallItemInfo>	m_mapSilverStoneItem;
	// ���Ӽ�ʱ��
	Avalon::SimpleTimer						m_minTimer;
	// �㲥��Ϣ����
	std::string							m_strNotify;
};

#endif

