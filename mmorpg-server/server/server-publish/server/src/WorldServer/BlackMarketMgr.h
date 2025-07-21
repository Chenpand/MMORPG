#ifndef _BLACK_MARKET_MGR_H_
#define _BLACK_MARKET_MGR_H_

#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include "CLBlackMarketDefine.h"
#include "CLItemTypeDefine.h"
#include "AvalonSimpleTimer.h"
#include <functional>

class WSItem;
struct OpActivityRegInfo;
class BlackMarketItemDataEntry;
class WSPlayer;
class ActivityInfo;

/**
*@brief ���о�������
*/
class BlackMarketAuction : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(BlackMarketAuction, Avalon::NullMutex)
public:
	friend class BlackMarketMgr;
	
	BlackMarketAuction();
	BlackMarketAuction(UInt32 opActId, UInt32 backBuyItemId, UInt8 backBuyType, UInt8 backBuySubType,
		UInt8 backBuyNum,UInt32 fixedPrice, UInt32 recomPrice, UInt32 beginTm, UInt32 endTm);
	~BlackMarketAuction();

public:
	//�����
	CL_DBPROPERTY_BEGIN(CLSyncObject)
	CL_DBPROPERTY("op_activity_id", m_op_activity_id)
	CL_DBPROPERTY("back_buy_item_id", m_back_buy_item_id)
	CL_DBPROPERTY("back_buy_type", m_back_buy_type)
	CL_DBPROPERTY("back_buy_subtype", m_back_buy_subtype)	
	CL_DBPROPERTY("back_buy_num", m_back_buy_num)
	CL_DBPROPERTY("fixed_price", m_fixed_price)
	CL_DBPROPERTY("recommend_price", m_recommend_price)
	CL_DBPROPERTY("begin_time", m_begin_time)
	CL_DBPROPERTY("end_time", m_end_time)
	CL_DBPROPERTY("off_sale", m_off_sale)
	CL_DBPROPERTY("is_settled", m_is_settled)
	CL_DBPROPERTY("is_sys_cancel", m_is_sys_cancel)
	
	CL_DBPROPERTY_END()

public:
	void Update();
	void Insert();
	void Delete();

	void LoadFromDb(CLRecordCallback *pCallback);
public:
	//��Ӫ�id
	ObjUInt32	m_op_activity_id;
	//�չ�����Ʒid	
	ObjUInt32	m_back_buy_item_id;
	//�չ�����
	ObjUInt8	m_back_buy_type;
	//�չ�������
	ObjUInt8	m_back_buy_subtype;
	//�չ�����
	ObjUInt32	m_back_buy_num;
	//�̶��չ��۸�
	ObjUInt32	m_fixed_price;
	//�Ƽ��۸�
	ObjUInt32	m_recommend_price;
	//������ʼʱ��
	ObjUInt32	m_begin_time;
	//��������ʱ��
	ObjUInt32	m_end_time;
	//�Ƿ��¼�(�Ϸ��յ�����)
	ObjUInt8    m_off_sale;
	//�Ƿ��Ѿ����㴦��
	ObjUInt8	m_is_settled;
	//�Ƿ�ϵͳ�Զ�ȡ����
	ObjUInt8	m_is_sys_cancel;

public:
	//���ļ۸�����
	UInt32		m_auction_price_lower;
	//���ļ۸�����
	UInt32		m_auction_price_upper;
};

/**
*@brief ���н��׶�����
*/
class BlackMarketTransaction : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(BlackMarketTransaction, Avalon::NullMutex)
public:
	friend class BlackMarketMgr;
	
	BlackMarketTransaction();
	~BlackMarketTransaction();

public:
	//�����
	CL_DBPROPERTY_BEGIN(CLSyncObject)
	CL_DBPROPERTY("create_time", m_create_time)
	CL_DBPROPERTY("opact_main_id", m_opact_main_id)
	CL_DBPROPERTY("opact_id", m_opact_id)
	CL_DBPROPERTY("auction_guid", m_auction_guid)
	CL_DBPROPERTY("item_id", m_item_id)
	CL_DBPROPERTY("auctioner_guid", m_auctioner_guid)
	CL_DBPROPERTY("auctioner_name", m_auctioner_name)
	CL_DBPROPERTY("auctioner_accid", m_auctioner_accid)
	CL_DBPROPERTY("auctioner_viplv", m_auctioner_viplv)
	CL_DBPROPERTY("auction_type", m_auction_type)
	CL_DBPROPERTY("auction_money_type", m_auction_money_type)
	CL_DBPROPERTY("auction_price", m_auction_price)
	CL_DBPROPERTY("off_sale", m_off_sale)
	CL_DBPROPERTY("auction_result", m_auction_result)
	CL_DBPROPERTY("state", m_state)
	CL_DBPROPERTY("money_sended", m_money_sended)
	CL_DBPROPERTY("item_returned", m_item_returned)
	CL_DBPROPERTY("item_src_guid", m_item_src_guid)
	CL_DBPROPERTY("item_name", m_item_name)
	CL_DBPROPERTY("delete_time", m_delete_time)
	CL_DBPROPERTY_END()
public:
	void Update();
	void Insert();
	void Delete();

	void LoadFromDb(CLRecordCallback *pCallback);
public:
	//������뽻�׵�װ��
	WSItem*		m_item;
	//����ʱ��
	ObjUInt32	m_create_time;
	//�������id
	ObjUInt32	m_opact_main_id;
	//�����id
	ObjUInt32	m_opact_id;
	//������guid
	ObjUInt64	m_auction_guid;
	//���ĵ���Ʒid
	ObjUInt32	m_item_id;
	//�������guid
	ObjUInt64	m_auctioner_guid;
	//�����������
	ObjString	m_auctioner_name;
	//��������˺�id
	ObjUInt32	m_auctioner_accid;
	//�������vip�ȼ�
	ObjUInt8	m_auctioner_viplv;
	//��������
	ObjUInt8	m_auction_type;
	//���Ļ�������
	ObjUInt32   m_auction_money_type;
	//���ļ۸�
	ObjUInt32	m_auction_price;
	//�Ƿ��¼�(�Ϸ��յ�����)
	ObjUInt8    m_off_sale;
	//���Ľ��, 0:ʧ��,1:�ɹ�
	ObjUInt8	m_auction_result;
	//��������״̬ 0:δ����, 1:�Ѿ�����
	ObjUInt8	m_state;
	//���Ž�ұ�־ 0:δ��,1:�ѷ���
	ObjUInt8	m_money_sended;
	//�����˻���־ 0:δ�˻�,1:���˻�
	ObjUInt8	m_item_returned;
	//���뾺�ĵ���Դguid
	ObjUInt64	m_item_src_guid;
	//��������
	ObjString	m_item_name;
	//ɾ��ʱ��
	ObjUInt32	m_delete_time;
};

//�������˾��������չ���item��ƽ���۸�
class BlackMarketItemAuctionPrice 
{
public:
	AVALON_DEFINE_OBJECTPOOL(BlackMarketItemAuctionPrice, Avalon::NullMutex)
		BlackMarketItemAuctionPrice() : guid(0), item_type_id(0), price(0), auction_guid(0){}


public:
	ObjID_t					guid;
	UInt32					item_type_id;
	UInt32					price;
	ObjID_t					auction_guid;
};

class BlackMarketMgr : public Avalon::Singleton<BlackMarketMgr>
{
public:
	//�������
	typedef std::map<UInt64, BlackMarketAuction*> AuctionMap;
	//���Ľ��׼���
	typedef std::map<UInt64, BlackMarketTransaction*> TransactionMap;
	//����ƽ�����׼۸񼯺�
	typedef std::map<UInt32, BlackMarketItemAuctionPrice*> AuctionTransAverPriceMap;
public:
	BlackMarketMgr();
	~BlackMarketMgr();

	/**
	*@brief��ʼ��
	*/
	void Init();

	/**
	*@brief��ʼ�����Ϣ
	*/
	void InitOpActInfo();

	void InitActInfo();

	/**
	*@brief �������ݿ��¼
	*/
	void LoadRecords();

	/**
	*@brief ������ؾ������¼
	*/
	bool OnLoadAuctionRet(CLRecordCallback *pCallback);

	/**
	*@brief ������ؾ��Ķ���
	*/
	bool OnLoadTranscationRet(CLRecordCallback *pCallback);

	/**
	*@brief ���ؾ���ƽ�����׼۸�
	*/
	bool OnLoadAuctionAverPriceRet(CLRecordCallback *pCallback);

	/**
	*@brief tick����
	*/
	void OnTick(const Avalon::Time &now);

	/**
	*@brief �����������
	*/
	void OnPlayerOnline(WSPlayer* player);

	/**
	*@brief ����������˻�� 
	*/
	void OnOpActOpen(const OpActivityRegInfo& opActInfo);
	void OnActOpen(const ActivityInfo& actInfo);

	/**
	*@brief ����������˻����
	*/
	void OnOpActClose(const OpActivityRegInfo& opActInfo);
	void OnActClose(const ActivityInfo& actInfo);

	/**
	*@brief ������������չ��б�
	*/
	void OnPlayerQueryAuctionList(WSPlayer* player);

	/**
	*@brief ��������������չ���Ʒ
	*/
	void OnPlayerAuctionReq(WSPlayer* player, UInt64 auction_guid, UInt64 item_guid, UInt32 auction_price);

	/**
	*@brief �������ȡ������
	*/
	void OnPlayerCancelAuctionReq(WSPlayer* player, UInt64 auction_guid);

protected:
	//����Ƿ���Կ���
	bool CheckCanOpenAct();

	//��Ƿ���
	bool IsActOpen() { return m_isOpen; }

	//֪ͨ������ȥ
	void NotifyRefresh(WSPlayer& Player);

	//��ȡ��ǰ�����еĺ������˻id
	UInt32 GetCurrOpenBlackMarketOpAct();

	//������ڻ�չ�����
	void RandomAuctionType();

	//�����չ�����
	void GenerateBuyLimit();

	//��ȡ���ڻ�չ�����
	UInt8 GetAuctionType();

	//����һ��������Ĺ̶��չ��۸�
	UInt32 CalcAuctionFixedPrice(BlackMarketItemDataEntry* entry);

	//���ɱ��ڻ������
	void GenerateAuctions(const OpActivityRegInfo& opActInfo);
	void GenerateAuctions(const ActivityInfo& actInfo);

	//��ѯһ��������
	BlackMarketAuction* GetAuction(UInt64 uid);
	
	//���һ��������
	bool AddOneAuction(BlackMarketAuction* record);
	
	//�Ƴ�һ��������
	void RemoveAuction(UInt64 uid);

	//��ѯһ�����Ľ���
	BlackMarketTransaction* GetTransaction(UInt64 uid);
	
	//���һ�����Ľ���
	bool AddOneTransaction(BlackMarketTransaction* record);
	
	//�Ƴ�һ��������
	void RemoveOneTransaction(UInt64 uid, bool del = false);

	//��ȡ���׻�������
	MoneyType GetMoneyType();
	
	//��ȡ���ڻ�չ��������
	UInt32 GetBackBuyMoneyUpperLimit();

	//���㵱ǰ�����
	void SettleCurActAuctions();

	//����һ������
	void SettleOneAuction(BlackMarketAuction& auction);

	//����һ���̶��۸���
	void SettleOneFixedPriceAuction(BlackMarketAuction& auction);

	//����һ�����ۼ۸���
	void SettleOneAuctionPriceAuction(BlackMarketAuction& auction);

	//ȡ��һ������
	void CancelOneAuction(BlackMarketAuction& auction);

	//ȡ��һ����������
	void CancelOneTransaction(WSPlayer& player, BlackMarketTransaction& trans);

	//��ȡָ�����ĵľ����б�
	void GetTransactionByAuctionId(UInt64 autionId, std::vector<BlackMarketTransaction*>& vipTrans, 
					std::vector<BlackMarketTransaction*>& notVipTrans);

	//��ȡָ�����ĵľ����б�
	void GetTransactionByAuctionId(UInt64 autionId, std::vector<BlackMarketTransaction*>& trans);

	//����һ�����Ķ���
	void HandleOneTransaction(BlackMarketTransaction &trans, bool select);

	//��ȡ������ƷID
	UInt32 GetItemTypeIDByMoneyType(MoneyType type);

	//��ȡָ���ڻ�ľ�����
	void GetAuctionsByOpActId(UInt32 opActId, std::vector<BlackMarketAuction*>& auctions);

	void GetAuctionsByOpAct(const ActivityInfo* pActInfo, std::vector<BlackMarketAuction*>& auctions);

	//���¾��۽���ƽ�����׼۸�
	void UpdateAuctionTransAverPrice(UInt32 itemTypeId, UInt32 averPrice, UInt64 auctionGuid);
	
	//��ѯ���۽���ƽ�����׼۸�
	UInt32 GetAuctionTransAverPrice(UInt32 itemTypeId);

	//����������
	void TraversalAuction(const std::function<bool(BlackMarketAuction&)>& ff);

	//��ȡ��������Ϣ
	BlackMarketAuctionInfo GetAuctionInfo(WSPlayer& player, const BlackMarketAuction& auction);

	//����������
	void TraversalTransaction(const std::function<bool(BlackMarketTransaction&)>& ff);

	//��ȡָ������
	BlackMarketTransaction* GetPlayerAuctionTransaction(UInt64 playerId, UInt64 auctionId);

	//��������
	BlackMarketTransaction* CreateOnAuctionTranscation(UInt64 playerId, UInt64 itemGuid, UInt64 auctionGuid, 
		UInt8 auctionType,UInt32 auctionPrice, UInt32 opActMainId, UInt32 opActId, std::string playerName, 
		UInt32 accid, UInt32 viplv, UInt32 moneyType, WSItem& item);

protected:
	/**
	*@breif ���ͺ������˻��յ�����־
	*/
	void SendBlackMarketAuctionUdpLog(UInt64 auctId, UInt8 auctType, UInt32  itemDataId, UInt32 recovePrice, UInt32 recoveNum);

	/**
	*@breif ���ͺ����������ջ��յ�����־
	*/
	void SendBlackMarketRecovedUdpLog(UInt64 auctId, UInt8 auctType, UInt32 itemDataId, UInt64 palyerId, UInt64 itemId, UInt32 recovedPrice);
private:
	AuctionMap					m_auctions;
	TransactionMap				m_transactions;
	AuctionTransAverPriceMap	m_transAverPrices;
	bool						m_isOpen;
	//UInt32						m_curOpActId;
	UInt8						m_auction_type;
	UInt32						m_beginTm;
	UInt32						m_endTm;
	//���id(��һ���׶εĻid)
	//UInt32						m_mainOpActId;
	Avalon::SimpleTimer			m_checkAuctionTimer;
	Avalon::SimpleTimer			m_checkTransDeleteTimer;

	UInt32						m_curActId;
	std::string					m_curActName;
};

#endif