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
*@brief 黑市竞拍项类
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
	//编解码
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
	//运营活动id
	ObjUInt32	m_op_activity_id;
	//收购的物品id	
	ObjUInt32	m_back_buy_item_id;
	//收购类型
	ObjUInt8	m_back_buy_type;
	//收购子类型
	ObjUInt8	m_back_buy_subtype;
	//收购数量
	ObjUInt32	m_back_buy_num;
	//固定收购价格
	ObjUInt32	m_fixed_price;
	//推荐价格
	ObjUInt32	m_recommend_price;
	//竞购开始时间
	ObjUInt32	m_begin_time;
	//竞购结束时间
	ObjUInt32	m_end_time;
	//是否下架(合服收到设置)
	ObjUInt8    m_off_sale;
	//是否已经结算处理
	ObjUInt8	m_is_settled;
	//是否系统自动取消的
	ObjUInt8	m_is_sys_cancel;

public:
	//竞拍价格下限
	UInt32		m_auction_price_lower;
	//竞拍价格上限
	UInt32		m_auction_price_upper;
};

/**
*@brief 黑市交易订单类
*/
class BlackMarketTransaction : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(BlackMarketTransaction, Avalon::NullMutex)
public:
	friend class BlackMarketMgr;
	
	BlackMarketTransaction();
	~BlackMarketTransaction();

public:
	//编解码
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
	//玩家申请交易的装备
	WSItem*		m_item;
	//创建时间
	ObjUInt32	m_create_time;
	//所属主活动id
	ObjUInt32	m_opact_main_id;
	//所属活动id
	ObjUInt32	m_opact_id;
	//竞拍项guid
	ObjUInt64	m_auction_guid;
	//竞拍的物品id
	ObjUInt32	m_item_id;
	//竞拍玩家guid
	ObjUInt64	m_auctioner_guid;
	//竞拍玩家名字
	ObjString	m_auctioner_name;
	//竞拍玩家账号id
	ObjUInt32	m_auctioner_accid;
	//竞拍玩家vip等级
	ObjUInt8	m_auctioner_viplv;
	//竞拍类型
	ObjUInt8	m_auction_type;
	//竞拍货币类型
	ObjUInt32   m_auction_money_type;
	//竞拍价格
	ObjUInt32	m_auction_price;
	//是否下架(合服收到设置)
	ObjUInt8    m_off_sale;
	//竞拍结果, 0:失败,1:成功
	ObjUInt8	m_auction_result;
	//订单处理状态 0:未处理, 1:已经处理
	ObjUInt8	m_state;
	//发放金币标志 0:未发,1:已发放
	ObjUInt8	m_money_sended;
	//道具退还标志 0:未退还,1:已退还
	ObjUInt8	m_item_returned;
	//申请竞拍道具源guid
	ObjUInt64	m_item_src_guid;
	//道具名称
	ObjString	m_item_name;
	//删除时间
	ObjUInt32	m_delete_time;
};

//黑市商人竞价类型收购的item的平均价格
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
	//竞拍项集合
	typedef std::map<UInt64, BlackMarketAuction*> AuctionMap;
	//竞拍交易集合
	typedef std::map<UInt64, BlackMarketTransaction*> TransactionMap;
	//竞价平均交易价格集合
	typedef std::map<UInt32, BlackMarketItemAuctionPrice*> AuctionTransAverPriceMap;
public:
	BlackMarketMgr();
	~BlackMarketMgr();

	/**
	*@brief初始化
	*/
	void Init();

	/**
	*@brief初始化活动信息
	*/
	void InitOpActInfo();

	void InitActInfo();

	/**
	*@brief 加载数据库记录
	*/
	void LoadRecords();

	/**
	*@brief 处理加载竞拍项记录
	*/
	bool OnLoadAuctionRet(CLRecordCallback *pCallback);

	/**
	*@brief 处理加载竞拍订单
	*/
	bool OnLoadTranscationRet(CLRecordCallback *pCallback);

	/**
	*@brief 加载竞价平均交易价格
	*/
	bool OnLoadAuctionAverPriceRet(CLRecordCallback *pCallback);

	/**
	*@brief tick函数
	*/
	void OnTick(const Avalon::Time &now);

	/**
	*@brief 处理玩家上线
	*/
	void OnPlayerOnline(WSPlayer* player);

	/**
	*@brief 处理黑市商人活动打开 
	*/
	void OnOpActOpen(const OpActivityRegInfo& opActInfo);
	void OnActOpen(const ActivityInfo& actInfo);

	/**
	*@brief 处理黑市商人活动结束
	*/
	void OnOpActClose(const OpActivityRegInfo& opActInfo);
	void OnActClose(const ActivityInfo& actInfo);

	/**
	*@brief 处理玩家请求收购列表
	*/
	void OnPlayerQueryAuctionList(WSPlayer* player);

	/**
	*@brief 处理玩家请求竞拍收购物品
	*/
	void OnPlayerAuctionReq(WSPlayer* player, UInt64 auction_guid, UInt64 item_guid, UInt32 auction_price);

	/**
	*@brief 处理玩家取消竞拍
	*/
	void OnPlayerCancelAuctionReq(WSPlayer* player, UInt64 auction_guid);

protected:
	//检查活动是否可以开启
	bool CheckCanOpenAct();

	//活动是否开启
	bool IsActOpen() { return m_isOpen; }

	//通知重新拉去
	void NotifyRefresh(WSPlayer& Player);

	//获取当前进行中的黑市商人活动id
	UInt32 GetCurrOpenBlackMarketOpAct();

	//随机本期活动收购类型
	void RandomAuctionType();

	//生成收购上限
	void GenerateBuyLimit();

	//获取本期活动收购类型
	UInt8 GetAuctionType();

	//计算一个竞拍项的固定收购价格
	UInt32 CalcAuctionFixedPrice(BlackMarketItemDataEntry* entry);

	//生成本期活动竞拍项
	void GenerateAuctions(const OpActivityRegInfo& opActInfo);
	void GenerateAuctions(const ActivityInfo& actInfo);

	//查询一个竞拍项
	BlackMarketAuction* GetAuction(UInt64 uid);
	
	//添加一个竞拍项
	bool AddOneAuction(BlackMarketAuction* record);
	
	//移除一个竞拍项
	void RemoveAuction(UInt64 uid);

	//查询一个竞拍交易
	BlackMarketTransaction* GetTransaction(UInt64 uid);
	
	//添加一个竞拍交易
	bool AddOneTransaction(BlackMarketTransaction* record);
	
	//移除一个竞拍项
	void RemoveOneTransaction(UInt64 uid, bool del = false);

	//获取交易货币类型
	MoneyType GetMoneyType();
	
	//获取本期活动收购金币上限
	UInt32 GetBackBuyMoneyUpperLimit();

	//结算当前活动竞拍
	void SettleCurActAuctions();

	//结算一个竞拍
	void SettleOneAuction(BlackMarketAuction& auction);

	//结算一个固定价格竞拍
	void SettleOneFixedPriceAuction(BlackMarketAuction& auction);

	//结算一个竞价价格竞拍
	void SettleOneAuctionPriceAuction(BlackMarketAuction& auction);

	//取消一个竞拍
	void CancelOneAuction(BlackMarketAuction& auction);

	//取消一个交易申请
	void CancelOneTransaction(WSPlayer& player, BlackMarketTransaction& trans);

	//获取指定竞拍的竞卖列表
	void GetTransactionByAuctionId(UInt64 autionId, std::vector<BlackMarketTransaction*>& vipTrans, 
					std::vector<BlackMarketTransaction*>& notVipTrans);

	//获取指定竞拍的竞卖列表
	void GetTransactionByAuctionId(UInt64 autionId, std::vector<BlackMarketTransaction*>& trans);

	//处理一个竞拍订单
	void HandleOneTransaction(BlackMarketTransaction &trans, bool select);

	//获取货币物品ID
	UInt32 GetItemTypeIDByMoneyType(MoneyType type);

	//获取指定期活动的竞拍项
	void GetAuctionsByOpActId(UInt32 opActId, std::vector<BlackMarketAuction*>& auctions);

	void GetAuctionsByOpAct(const ActivityInfo* pActInfo, std::vector<BlackMarketAuction*>& auctions);

	//更新竞价交易平均交易价格
	void UpdateAuctionTransAverPrice(UInt32 itemTypeId, UInt32 averPrice, UInt64 auctionGuid);
	
	//查询竞价交易平均交易价格
	UInt32 GetAuctionTransAverPrice(UInt32 itemTypeId);

	//遍历竞价项
	void TraversalAuction(const std::function<bool(BlackMarketAuction&)>& ff);

	//获取竞拍项信息
	BlackMarketAuctionInfo GetAuctionInfo(WSPlayer& player, const BlackMarketAuction& auction);

	//遍历交易项
	void TraversalTransaction(const std::function<bool(BlackMarketTransaction&)>& ff);

	//获取指定交易
	BlackMarketTransaction* GetPlayerAuctionTransaction(UInt64 playerId, UInt64 auctionId);

	//创建交易
	BlackMarketTransaction* CreateOnAuctionTranscation(UInt64 playerId, UInt64 itemGuid, UInt64 auctionGuid, 
		UInt8 auctionType,UInt32 auctionPrice, UInt32 opActMainId, UInt32 opActId, std::string playerName, 
		UInt32 accid, UInt32 viplv, UInt32 moneyType, WSItem& item);

protected:
	/**
	*@breif 发送黑市商人回收道具日志
	*/
	void SendBlackMarketAuctionUdpLog(UInt64 auctId, UInt8 auctType, UInt32  itemDataId, UInt32 recovePrice, UInt32 recoveNum);

	/**
	*@breif 发送黑市商人最终回收道具日志
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
	//主活动id(第一个阶段的活动id)
	//UInt32						m_mainOpActId;
	Avalon::SimpleTimer			m_checkAuctionTimer;
	Avalon::SimpleTimer			m_checkTransDeleteTimer;

	UInt32						m_curActId;
	std::string					m_curActName;
};

#endif