#ifndef  _AUCTION_TRANS_RECORD_H_
#define  _AUCTION_TRANS_RECORD_H_

#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include "CLAuctionDefine.h"
#include "NewAuctionMgr.h"

//订单处理状态
enum TransHandleState
{
	THS_NONE = 0,  //未处理
	THS_HANDLING = 1, //处理中
	THS_DAOZHUANG = 2,  //到账(无异常)
	THS_WEIGUI_SUCC = 3,  //异常违规处理成功
	THS_WEIGUI_HANDLE_TIMEOUT = 4, //异常违规处理超时
	THS_OVERTIME_LAST = 5, //距最后一次正常交易超时
	THS_MAX,
};

//处罚类型
enum TransPunishType
{
	TPT_NONE = 0, 
	TPT_A = 1, //A
	TPT_B = 2, //B
	TPT_C = 3, //C
	TPT_MAX,
};

//处罚id
enum TransPunishId
{
	TPI_NONE = 0,	//不惩罚
	TPI_ONE = 1,	//交易成功，交易额外获利金币冻结，可付费解除也可通过每日活跃解除
	TPI_TWO = 2,	//交易成功，交易全额金币冻结，可付费解除也可通过每日活跃解除
	TPI_THRE = 3,	//交易取消，道具和金币各自返还（手续费返还）
	TPI_FOUR = 4,	//交易成功，交易额外获利金币冻结，只可付费解除（代替直接删除的处理）
	TPI_FIRE = 5,	//交易取消，道具返还，金币全部冻结，可付费解除也可通过每日活跃解除
	TPI_SIX = 6,	//交易成功，交易全额金币冻结，只可付费解除（代替直接删除的处理）
	TPI_SVEN = 7,	//交易取消，道具返还，金币全部冻结，只可付费解除（代替直接删除的处理）
	TPI_EIGHT = 8,	//交易取消，道具返还，金币扣除
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

	//审核处理超时值
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
	//编解码
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
	//拍卖行id
	ObjUInt64	m_auction_id;
	//是否是珍品
	ObjUInt8	m_is_treas;
	//卖家
	ObjUInt64	m_seller;
	//买家
	ObjUInt64	m_buyer;
	//交易时间
	ObjUInt32	m_transaction_time;
	//交易物品id
	ObjUInt64	m_item_id;
	//交易物品类型id
	ObjUInt32	m_item_data_id;
	//交易物品总数量
	ObjUInt32	m_item_num;
	//品级
	ObjUInt32   m_qualitylv;
	//强化等级
	ObjUInt32   m_strengthen;
	//交易宝珠的时候记录附加buffid
	ObjUInt32   m_beadbuffId;
	//装备类型(EquipType):普通0/带气息1/红字2
	ObjUInt8	m_equipType;
	//红字装备增幅类型
	ObjUInt8	m_enhanceType;
	//增幅装备属性增加值
	ObjUInt32	m_enhanceNum;
	//交易装备时镶嵌的宝珠id
	ObjUInt32   m_mountBeadId;
	//交易装备时镶嵌的宝珠的附加buffid
	ObjUInt32   m_mountBeadBuffId;
	//交易装备时附加宝珠平均交易价格
	ObjUInt32	m_beadAverPrice;
	//交易装备时候镶嵌的附魔卡
	ObjUInt32	m_mountMagicCardId;
	//交易装备时候镶嵌的附魔卡等级
	ObjUInt8	m_mountMagicCardLv;
	//交易装备时附魔卡平均交易价格
	ObjUInt32	m_magicCardAverPrice;
	//交易物品名字
	ObjString	m_item_name;
	//购买数量
	ObjUInt32   m_buy_num;
	//交易物品评分
	ObjUInt32	m_item_score;
	//交易货币类型
	ObjUInt32	m_money_type;
	//交易金额
	ObjUInt32	m_transaction_amount;
	//手续费
	ObjUInt32	m_counter_fee;
	//上架押金
	ObjUInt32	m_deposit;
	//交易额外获利金额
	ObjUInt32	m_transaction_extra_profit;
	//交易物品单价
	ObjUInt32	m_unit_price;
	//买家领取的邮件id
	ObjUInt64	m_mail_id;
	//审核结束时间(到账时间)
	ObjUInt32	m_verify_end_time;
	//程序审核结果
	ObjUInt8   m_program_audit_result;
	//A惩罚id
	ObjUInt8	m_punishA_id;
	//B惩罚id
	ObjUInt8	m_punishB_id;
	//人工惩罚id
	ObjUInt8	m_punishC_id;
	//最终惩罚id
	ObjUInt8	m_punished_id;
	//是否归还道具给卖家
	ObjUInt8    m_item_returned;
	//状态 0未处理1到账2违规处理成功
	ObjUInt8	m_state;
	//在售最低价格
	ObjUInt32	m_minPriceOnSale;
	//是否被系统扫货
	ObjUInt8	m_isSysRecove;
	//是否不计入平均交易价格
	ObjUInt8	m_isNotRecAveprice;
	//珍品交易价格M值
	ObjUInt32	m_MPrice;
public:
	//scene check item request id
	UInt32 m_check_item_req_id;

	//是否在惩罚处理中
	bool m_is_in_punishing;
	//惩罚超时时间戳
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

	void LoadUnHandledRecords(); //加载未处理订单

	void LoadShouldHandleRecords(); //加载应该处理订单

	void LoadAlreadyTransedRecords(); //加载已经交易的订单

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
	*@检查购买道具是否在玩家邮件中
	*/
	bool GetItemInMailOfBuyer(AuctionTransRecord& trans, WSPlayer& buyer, WSItem*& item);

	/**
	*@删除邮件中道具
	*/
	void DeleteItemInMailOfBuyer(AuctionTransRecord& trans, WSPlayer& buyer);

	/**
	*@brief B类型惩罚,交易成功 
	*/
	void HandleOnAbNormalByTypeB(AuctionTransRecord& trans, UInt32 punishId);

	/**
	*@brief 判断异常, 但交易成功
	*/
	void HandleOnAbnormalTransactionSucc(AuctionTransRecord& trans, UInt32 punishId);

	/**
	*@brief A类型惩罚，取消交易或交易成功
	*/
	void HandleOnAbNormalByTypeA(AuctionTransRecord& trans, UInt32 punishId);

	/**
	*@brief C类型惩罚，取消交易或交易成功
	*/
	void HandleOnAbNormalByTypeC(AuctionTransRecord& trans, UInt32 punishId);

	/**
	*@brief 尝试取消交易
	*/
	void HandleOnTryCancelTransaction(AuctionTransRecord& trans, UInt32 punishId);

	/**
	*@  取消交易
	*/
	void HandleOnCancelTransaction(AuctionTransRecord& trans, UInt32 punishId, WSItem* item);

	/**
	*@brief 取消交易,返还交易物品给卖家
	*/
	void ReturnItemsToSeller(AuctionTransRecord& trans, std::vector<ItemReward>& depositItems, std::vector<WSItem*> vecitems);

	/**
	*@brief 取消交易,退还金币给买家
	*/
	void ReturnPayMoneyToBuyer(AuctionTransRecord& trans);

	/**
	*@brief 交易成功,发放金币给卖家
	*/
	void SendGetMoneyToSeller(AuctionTransRecord& trans, UInt32 money);

	/**
	*@brief 是否判定取消交易的的处罚id
	*/
	bool IsCancelTransactionPunish(UInt32 punishId);

	/**
	*@brief 添加已完成正常交易记录
	*/
	bool AddOneTransedRecord(const AuctionTransRecord& record);

	/**
	*@brief 获取物品交易记录
	*/
	UInt32 GetItemTransedRecords(UInt32 itemTypeId, UInt32 strengthen, UInt32 beadBuffId, UInt8 enhanceType, std::vector<AuctionTransaction>& itemTrans);
private:
	AuctionTransRecordMap							m_records;

	// 检测拍卖行超时物品定时器
	Avalon::SimpleTimer	                            m_checkTimer;

	// 已经交易的记录
	ItemTransedMap									m_transedRecords;
};
#endif