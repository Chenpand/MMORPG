#ifndef  _AUCTION_TRANS_RECORD_H_
#define  _AUCTION_TRANS_RECORD_H_

#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include "CLAuctionDefine.h"
#include "NewAuctionMgr.h"

//��������״̬
enum TransHandleState
{
	THS_NONE = 0,  //δ����
	THS_HANDLING = 1, //������
	THS_DAOZHUANG = 2,  //����(���쳣)
	THS_WEIGUI_SUCC = 3,  //�쳣Υ�洦��ɹ�
	THS_WEIGUI_HANDLE_TIMEOUT = 4, //�쳣Υ�洦��ʱ
	THS_OVERTIME_LAST = 5, //�����һ���������׳�ʱ
	THS_MAX,
};

//��������
enum TransPunishType
{
	TPT_NONE = 0, 
	TPT_A = 1, //A
	TPT_B = 2, //B
	TPT_C = 3, //C
	TPT_MAX,
};

//����id
enum TransPunishId
{
	TPI_NONE = 0,	//���ͷ�
	TPI_ONE = 1,	//���׳ɹ������׶��������Ҷ��ᣬ�ɸ��ѽ��Ҳ��ͨ��ÿ�ջ�Ծ���
	TPI_TWO = 2,	//���׳ɹ�������ȫ���Ҷ��ᣬ�ɸ��ѽ��Ҳ��ͨ��ÿ�ջ�Ծ���
	TPI_THRE = 3,	//����ȡ�������ߺͽ�Ҹ��Է����������ѷ�����
	TPI_FOUR = 4,	//���׳ɹ������׶��������Ҷ��ᣬֻ�ɸ��ѽ��������ֱ��ɾ���Ĵ���
	TPI_FIRE = 5,	//����ȡ�������߷��������ȫ�����ᣬ�ɸ��ѽ��Ҳ��ͨ��ÿ�ջ�Ծ���
	TPI_SIX = 6,	//���׳ɹ�������ȫ���Ҷ��ᣬֻ�ɸ��ѽ��������ֱ��ɾ���Ĵ���
	TPI_SVEN = 7,	//����ȡ�������߷��������ȫ�����ᣬֻ�ɸ��ѽ��������ֱ��ɾ���Ĵ���
	TPI_EIGHT = 8,	//����ȡ�������߷�������ҿ۳�
	TPI_MAX,
};

#define PUNISHC_DEFAUT_ID 100

class SelectAuctionTransCallback;
class WSPlayer;

class AuctionTransRecord : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(AuctionTransRecord, Avalon::NullMutex)
public:
	AuctionTransRecord();
	~AuctionTransRecord();

	//��˴���ʱֵ
	static const UInt32 punish_timeout = 60;
public:
	void ToClient(ObjID_t playerId, AuctionTransaction& auctionTrans);
	void Update();
	void Insert();
	void Delete();
	bool IsShouldHanlde(UInt32 curr_time);
	bool IsNoAbnormal();
	void SetState(UInt8 st);
	UInt8 GetState() { return m_state; };
	void SetReturnedItemMark(UInt8 mark);
	void SetPunishedId(UInt32 punishedId);
	void SetSceneCheckItemReqId(UInt32 reqId);
	UInt32 GetSceneCheckItemReqId();
	void SetPunishingMark(bool mark);
	bool PunishingMark();
	bool IsTimeOut(UInt32 curr_time);
	void LoadPushishDataFromDb(CLRecordCallback *pCallback);
	void PriteState();
	void SendUdpLog();
public:
	//�����
	CL_DBPROPERTY_BEGIN(CLSyncObject)
	CL_DBPROPERTY("auction_id", m_auction_id)
	CL_DBPROPERTY("is_treas", m_is_treas)
	CL_DBPROPERTY("seller", m_seller)
	CL_DBPROPERTY("buyer", m_buyer)
	CL_DBPROPERTY("transaction_time", m_transaction_time)
	CL_DBPROPERTY("item_id", m_item_id)
	CL_DBPROPERTY("item_data_id", m_item_data_id)
	CL_DBPROPERTY("item_num", m_item_num)
	CL_DBPROPERTY("item_qualitylv", m_qualitylv)
	CL_DBPROPERTY("item_strengthen", m_strengthen)
	CL_DBPROPERTY("item_bead_buffId", m_beadbuffId)
	CL_DBPROPERTY("item_equip_type", m_equipType)	
	CL_DBPROPERTY("item_enhance_type", m_enhanceType)
	CL_DBPROPERTY("item_enhance_num", m_enhanceNum)
	CL_DBPROPERTY("item_mount_beadId", m_mountBeadId)
	CL_DBPROPERTY("item_mount_beadBuffId", m_mountBeadBuffId)
	CL_DBPROPERTY("item_bead_averPrice", m_beadAverPrice)
	CL_DBPROPERTY("item_mount_magicId", m_mountMagicCardId)
	CL_DBPROPERTY("item_mount_magicLv", m_mountMagicCardLv)
	CL_DBPROPERTY("item_magic_averPrice", m_magicCardAverPrice)
	CL_DBPROPERTY("item_name", m_item_name)
	CL_DBPROPERTY("buy_num", m_buy_num)
	CL_DBPROPERTY("item_score", m_item_score)
	CL_DBPROPERTY("money_type", m_money_type)
	CL_DBPROPERTY("transaction_amount", m_transaction_amount)
	CL_DBPROPERTY("counter_fee", m_counter_fee)
	CL_DBPROPERTY("transaction_extra_profit", m_transaction_extra_profit)
	CL_DBPROPERTY("deposit", m_deposit)
	CL_DBPROPERTY("unit_price", m_unit_price)
	CL_DBPROPERTY("mail_id", m_mail_id)
	CL_DBPROPERTY("verify_end_time", m_verify_end_time)
	CL_DBPROPERTY("program_audit_result", m_program_audit_result)
	CL_DBPROPERTY("punishA_id", m_punishA_id)
	CL_DBPROPERTY("punishB_id", m_punishB_id)
	CL_DBPROPERTY("punishC_id", m_punishC_id)
	CL_DBPROPERTY("punished_id", m_punished_id)
	CL_DBPROPERTY("item_returned", m_item_returned)
	CL_DBPROPERTY("state", m_state)
	CL_DBPROPERTY("min_price_onsale", m_minPriceOnSale)
	CL_DBPROPERTY("isSysRecove", m_isSysRecove)
	CL_DBPROPERTY("isNotRecAveprice", m_isNotRecAveprice)
	CL_DBPROPERTY("mprice", m_MPrice)
	CL_DBPROPERTY_END()
public:
	//������id
	ObjUInt64	m_auction_id;
	//�Ƿ�����Ʒ
	ObjUInt8	m_is_treas;
	//����
	ObjUInt64	m_seller;
	//���
	ObjUInt64	m_buyer;
	//����ʱ��
	ObjUInt32	m_transaction_time;
	//������Ʒid
	ObjUInt64	m_item_id;
	//������Ʒ����id
	ObjUInt32	m_item_data_id;
	//������Ʒ������
	ObjUInt32	m_item_num;
	//Ʒ��
	ObjUInt32   m_qualitylv;
	//ǿ���ȼ�
	ObjUInt32   m_strengthen;
	//���ױ����ʱ���¼����buffid
	ObjUInt32   m_beadbuffId;
	//װ������(EquipType):��ͨ0/����Ϣ1/����2
	ObjUInt8	m_equipType;
	//����װ����������
	ObjUInt8	m_enhanceType;
	//����װ����������ֵ
	ObjUInt32	m_enhanceNum;
	//����װ��ʱ��Ƕ�ı���id
	ObjUInt32   m_mountBeadId;
	//����װ��ʱ��Ƕ�ı���ĸ���buffid
	ObjUInt32   m_mountBeadBuffId;
	//����װ��ʱ���ӱ���ƽ�����׼۸�
	ObjUInt32	m_beadAverPrice;
	//����װ��ʱ����Ƕ�ĸ�ħ��
	ObjUInt32	m_mountMagicCardId;
	//����װ��ʱ����Ƕ�ĸ�ħ���ȼ�
	ObjUInt8	m_mountMagicCardLv;
	//����װ��ʱ��ħ��ƽ�����׼۸�
	ObjUInt32	m_magicCardAverPrice;
	//������Ʒ����
	ObjString	m_item_name;
	//��������
	ObjUInt32   m_buy_num;
	//������Ʒ����
	ObjUInt32	m_item_score;
	//���׻�������
	ObjUInt32	m_money_type;
	//���׽��
	ObjUInt32	m_transaction_amount;
	//������
	ObjUInt32	m_counter_fee;
	//�ϼ�Ѻ��
	ObjUInt32	m_deposit;
	//���׶���������
	ObjUInt32	m_transaction_extra_profit;
	//������Ʒ����
	ObjUInt32	m_unit_price;
	//�����ȡ���ʼ�id
	ObjUInt64	m_mail_id;
	//��˽���ʱ��(����ʱ��)
	ObjUInt32	m_verify_end_time;
	//������˽��
	ObjUInt8   m_program_audit_result;
	//A�ͷ�id
	ObjUInt8	m_punishA_id;
	//B�ͷ�id
	ObjUInt8	m_punishB_id;
	//�˹��ͷ�id
	ObjUInt8	m_punishC_id;
	//���ճͷ�id
	ObjUInt8	m_punished_id;
	//�Ƿ�黹���߸�����
	ObjUInt8    m_item_returned;
	//״̬ 0δ����1����2Υ�洦��ɹ�
	ObjUInt8	m_state;
	//������ͼ۸�
	ObjUInt32	m_minPriceOnSale;
	//�Ƿ�ϵͳɨ��
	ObjUInt8	m_isSysRecove;
	//�Ƿ񲻼���ƽ�����׼۸�
	ObjUInt8	m_isNotRecAveprice;
	//��Ʒ���׼۸�Mֵ
	ObjUInt32	m_MPrice;
public:
	//scene check item request id
	UInt32 m_check_item_req_id;

	//�Ƿ��ڳͷ�������
	bool m_is_in_punishing;
	//�ͷ���ʱʱ���
	UInt32 m_punish_timeout_timestamp;
};



class AuctionTransRecordMgr : public Avalon::Singleton<AuctionTransRecordMgr>
{
public:
	typedef std::map<UInt64, AuctionTransRecord*> AuctionTransRecordMap;
	typedef std::map<zjy_auction::AuctionPricesKey, std::map<UInt64, AuctionTransRecord>>  ItemTransedMap;

public:
	AuctionTransRecordMgr();
	~AuctionTransRecordMgr();

	void Init();

	void OnTick(const Avalon::Time &now);

	void LoadUnHandledRecords(); //����δ������

	void LoadShouldHandleRecords(); //����Ӧ�ô�����

	void LoadAlreadyTransedRecords(); //�����Ѿ����׵Ķ���

	bool AddOneRecord(AuctionTransRecord* record);
	
	AuctionTransRecord* GetRecord(UInt64 uid);
	
	bool OnLoadUnHandledTransRet(CLRecordCallback *pCallback);

	bool OnLoadShouldHandleTransRet(CLRecordCallback *pCallback);
	
	bool OnLoadAlreadyTransRet(CLRecordCallback *pCallback);

	void GetPlayerTrans(WSPlayer* player, std::vector<AuctionTransaction>& sales, std::vector<AuctionTransaction>& buys);

	UInt64 AddNewTransaction(ObjID_t buyer, zjy_auction::AuctionInfo *auctionInfo, UInt32 num, const std::string& itemName, 
		UInt32 deposit, UInt64 mailId, UInt32 minPriceOnSale, UInt8 isSysRecove, UInt32 MPrice);

	void HandleOneTrans(AuctionTransRecord& trans);

	void HandleOnNoAbnormal(AuctionTransRecord& trans);

	void HandleOnAbNormal(AuctionTransRecord& trans);

	bool JudgeOvertime(AuctionTransRecord& trans);
	/**
	*@��鹺������Ƿ�������ʼ���
	*/
	bool GetItemInMailOfBuyer(AuctionTransRecord& trans, WSPlayer& buyer, WSItem*& item);

	/**
	*@ɾ���ʼ��е���
	*/
	void DeleteItemInMailOfBuyer(AuctionTransRecord& trans, WSPlayer& buyer);

	/**
	*@brief B���ͳͷ�,���׳ɹ� 
	*/
	void HandleOnAbNormalByTypeB(AuctionTransRecord& trans, UInt32 punishId);

	/**
	*@brief �ж��쳣, �����׳ɹ�
	*/
	void HandleOnAbnormalTransactionSucc(AuctionTransRecord& trans, UInt32 punishId);

	/**
	*@brief A���ͳͷ���ȡ�����׻��׳ɹ�
	*/
	void HandleOnAbNormalByTypeA(AuctionTransRecord& trans, UInt32 punishId);

	/**
	*@brief C���ͳͷ���ȡ�����׻��׳ɹ�
	*/
	void HandleOnAbNormalByTypeC(AuctionTransRecord& trans, UInt32 punishId);

	/**
	*@brief ����ȡ������
	*/
	void HandleOnTryCancelTransaction(AuctionTransRecord& trans, UInt32 punishId);

	/**
	*@  ȡ������
	*/
	void HandleOnCancelTransaction(AuctionTransRecord& trans, UInt32 punishId, WSItem* item);

	/**
	*@brief ȡ������,����������Ʒ������
	*/
	void ReturnItemsToSeller(AuctionTransRecord& trans, std::vector<ItemReward>& depositItems, std::vector<WSItem*> vecitems);

	/**
	*@brief ȡ������,�˻���Ҹ����
	*/
	void ReturnPayMoneyToBuyer(AuctionTransRecord& trans);

	/**
	*@brief ���׳ɹ�,���Ž�Ҹ�����
	*/
	void SendGetMoneyToSeller(AuctionTransRecord& trans, UInt32 money);

	/**
	*@brief �Ƿ��ж�ȡ�����׵ĵĴ���id
	*/
	bool IsCancelTransactionPunish(UInt32 punishId);

	/**
	*@brief ���������������׼�¼
	*/
	bool AddOneTransedRecord(const AuctionTransRecord& record);

	/**
	*@brief ��ȡ��Ʒ���׼�¼
	*/
	UInt32 GetItemTransedRecords(UInt32 itemTypeId, UInt32 strengthen, UInt32 beadBuffId, UInt8 enhanceType, std::vector<AuctionTransaction>& itemTrans);
private:
	AuctionTransRecordMap							m_records;

	// ��������г�ʱ��Ʒ��ʱ��
	Avalon::SimpleTimer	                            m_checkTimer;

	// �Ѿ����׵ļ�¼
	ItemTransedMap									m_transedRecords;
};
#endif