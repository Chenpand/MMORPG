#ifndef _GOLD_CONSIGMMENT_SERVICE_H_
#define _GOLD_CONSIGMMENT_SERVICE_H_

#include <CLGoldConsignmentDefine.h>
#include <AvalonSimpleTimer.h>
#include "UnionService.h"

struct RoleIDConnID;
class USPlayer;
class GoldTradeThing;
class CLRecordCallback;

typedef std::shared_ptr<GoldConsignmentOrder> GoldConsignmentOrderPtr;
typedef std::map<UInt64, GoldConsignmentOrderPtr> GoldConsignmentOrderMap;
typedef std::list<GoldConsignmentOrderPtr> GoldConsignmentOrderList;
typedef std::map<UInt32, OrderGradeData> SellOrderGradeMap;
typedef std::map<UInt32, OrderGradeData, std::greater<UInt32>> BuyOrderGradeMap;

/**
 *@brief 金币寄售管理
 */
class GoldConsignmentService : public UnionService
{
public:
	GoldConsignmentService();
	~GoldConsignmentService();

public:

	virtual bool Init();
	virtual void OnTick(const Avalon::Time& now);
	virtual void OnDayChanged();

	virtual USPlayer* CreatePlayer();

	static UnionServerFunctionType GetFunctionType();

	/**
	 *@brief 请求订单页面
	 */
	void HandlerOrderPageReq(RoleIDConnID& info);

	/**
	 *@brief 上架订单
	 */
	void HandlerSumbitOrderReq(USPlayer* player, UInt32 tradeType, UInt32 orderType, UInt32 unitPrice, UInt32 tradeNum, UInt32 param, UInt32 submitId);

	/**
	 *@brief 请求成交记录
	 */
	void HandlerDealRecordReq(USPlayer* player);

	/**
	 *@brief 我的订单
	 */
	void HandlerOwnerOrderReq(USPlayer* player);

	/**
	 *@brief 下架订单
	 */
	void HandlerCancelOrderReq(USPlayer* player, UInt32 orderType, UInt64 orderId);

	/**
	 *@brief 区服活动结束
	 */
	void ZoneActivityEnd(UInt32 zoneId, UInt32 endTime);

	/**
	 *@brief 订单加载
	 */
	bool LoadOrderData(CLRecordCallback* callback);

	/**
	 *@brief 交易记录加载
	 */
	bool LoadTradeRecord(CLRecordCallback* callback);

	/**
	 *@brief 交易价钱
	 */
	bool LoadTradePrice(CLRecordCallback* callback);

	/**
	 *@brief 未处理的收益
	 */
	bool LoadUntreatProfit(CLRecordCallback* callback);

	/**
	 *@brief 未处理的信用点券
	 */
	bool LoadUntreatCreditPoint(CLRecordCallback* callback);

	/**
	 *@brief 初始化单价
	 */
	void InitUnitPrice();

	/**
	 *@brief 修改单价
	 */
	void ModifyUnitPrice();

	/**
	 *@brief 卖金事务返回
	 */
	bool SellGoldTransResult(USPlayer* player, GoldTradeThing* trans);

	/**
	 *@brief 买金事务返回
	 */
	bool BuyGoldTransResult(USPlayer* player, GoldTradeThing* trans);

public: // 后台GM工具

	/**
	 *@brief GM工具提交订单
	 */
	std::string GMSumbitOrder(UInt32 tradeType, UInt32 orderType, UInt32 unitPrice, UInt32 tradeNum);

private:

	/**
	 *@brief 检查涨幅
	 */
	bool _CheckWavePrice(UInt32 unitPrice);

	/**
	 *@brief 是否休市
	 */
	bool _IsClose();

	/**
	 *@brief 检查交易数据脏
	 */
	void _CheckDealData();

	/**
	 *@brief 检查休市
	 */
	void _CheckClose(const Avalon::Time& now);

	/**
	 *@brief 该区服是否正在下架
	 */
	bool _ZoneIsCloseOrder(UInt32 zoneId);

	/**
	 *@brief 检查需要下架的区服
	 */
	void _CheckOrderCloseZone();

	/**
	 *@brief 检查需要下架的角色
	 */
	void _CheckOrderCloseRole();

	/**
	 *@brief 检查需要下架的订单
	 */
	void _CheckOrderClose();

	/**
	 *@brief 检查超时订单
	 */
	void _CheckTimeOutOrder(const Avalon::Time& now);

	/**
	 *@brief 下架订单返还
	 */
	void _CancelOrderReturn(GoldConsignmentOrderPtr orderPtr, const std::string& reason, const std::string& title, const std::string& content, UInt32 profitType);

	/**
	 *@brief 检查卖金订单
	 */
	UInt32 _CheckSellOrder(USPlayer* player, UInt32 tradeType, UInt32 unitPrice, UInt32 tradeNum, UInt32 param, UInt32 submitId);

	/**
	 *@brief 检查买金订单
	 */
	UInt32 _CheckBuyOrder(USPlayer* player, UInt32 tradeType, UInt32 unitPrice, UInt32 tradeNum, UInt32 submitId);

	/**
	 *@brief 增加订单
	 */
	void _AddOrder(GoldConsignmentOrderPtr orderPtr);

	/**
	 *@brief 增加挡位订单
	 */
	void _AddGradeOrder(GoldConsignmentOrderPtr orderPtr);

	/**
	 *@brief 增加账号订单
	 */
	void _AddAccOrder(GoldConsignmentOrderPtr orderPtr);

	/**
	 *@brief 增加交易记录
	 */
	void _AddTradeRecord(GoldConsignmentTradeRecord& record);

	/**
	 *@brief 订单存库
	 */
	void _DBInsertOrder(GoldConsignmentOrderPtr orderPtr);

	/**
	 *@brief 订单删库
	 */
	void _DBDelOrder(UInt64 orderId);

	/**
	 *@brief 订单交易量更新
	 */
	void _DBUpdateOrder(GoldConsignmentOrderPtr orderPtr);

	/**
	 *@brief 交易记录存库
	 */
	void _DBInsertTradeRecord(GoldConsignmentTradeRecord& record);

	/**
	 *@brief 插入价钱
	 */	
	void _DBInsertPrice();

	/**
	 *@brief 更新价钱
	 */
	void _DBUpdatePrice(UInt32 flag);

	/**
	 *@brief 匹配买金订单
	 */
	void _MatchBuyOrder(USPlayer* player, GoldConsignmentOrderPtr orderPtr, UInt32 submitId);

	/**
	 *@brief 匹配卖金订单
	 */
	void _MatchSellOrder(USPlayer* player, GoldConsignmentOrderPtr orderPtr, UInt32 tradeType, UInt32 submitId);

	/**
	 *@brief 匹配买金挡位
	 */
	UInt32 _MatchBuyGrade(USPlayer* player, UInt32 tradeNum, UInt64 serialId, UInt32 sellUnitPrice, OrderGradeData& orderGrade, float& sellProfit);

	/**
	 *@brief 匹配卖金挡位
	 */
	UInt32 _MatchSellGrade(USPlayer* player, UInt32 tradeNum, UInt64 serialId, UInt32 buyUnitPrice, OrderGradeData& orderGrade, UInt32& costPoint, UInt32& returnPoint);

	/**
	 *@brief 查找挡位
	 */
	OrderGradeData* _FindOrderGradeData(UInt32 unitPrice, UInt32 orderType);

	/**
	 *@brief 删除挡位
	 */
	void _RemoveOrderGrade(UInt32 unitPrice, UInt32 orderType);

	/**
	 *@brief 挡位下架订单
	 */
	void _GradeCancelOrder(GoldConsignmentOrderPtr orderPtr);

	/**
	 *@brief 账号是否存在某订单
	 */
	GoldConsignmentOrderPtr _CreateOrder(USPlayer* player, UInt32 orderType, UInt32 unitPrice, UInt32 tradeNum, UInt32 totalPoint = 0);

	/**
	 *@brief 挂单
	 */
	void _SumbitOrder(GoldConsignmentOrderPtr orderPtr);

	/**
	 *@brief 检查符合单价的买金订单
	 */
	UInt32 _CheckMatchUnitPriceBuyOrder(UInt32 accId, UInt32 unitPrice, UInt32 tradeNum);

	/**
	 *@brief 检查符合单价的卖金订单
	 */
	UInt32 _CheckMatchUnitPriceSellOrder(UInt32 accId, UInt32 unitPrice, UInt32 tradeNum);

	/**
	 *@brief 查找订单
	 */
	GoldConsignmentOrderPtr _FindOrder(UInt32 orderType, UInt64 orderId);

	/**
	 *@brief 删除订单
	 */
	void _RemoveOrder(UInt32 orderType, UInt64 orderId);

	/**
	 *@brief 查找账号订单数
	 */
	UInt32 _FindAccOrderNum(UInt32 accId, UInt32 zoneId, UInt32 orderType);

	/**
	 *@brief 设置上一交易日均价
	 */
	void _SetLastAveragePrice(UInt32 price);

	/**
	 *@brief 设置今日均价
	 */
	void _SetCurAveragePrice(UInt32 price);

	/**
	 *@brief 设置最近均价
	 */
	void _SetRecentAveragePrice(UInt32 price);

	/**
	 *@brief 账号下信用点券预计收入
	 */
	UInt32 _EstimateAccCreditPointProfit(UInt32 zoneId, UInt32 accId);

	/**
	 *@brief 通知成交
	 */
	void _NotifyOrderDeal(UInt32 zoneId, UInt64 roleId);

	/**
	 *@brief 账号是否存在某订单
	 */
	bool _RoleIsHasOrder(USPlayer* player, UInt32 orderType, UInt64 orderId);

	/**
	 *@brief 增加成交量
	 */
	void _AddDealInfo(UInt32 costPoint, UInt32 dealNum);

	/**
	 *@brief 发放信用点券
	 */
	bool _SendCreditPointIncome(UInt32 zoneId, UInt32 accId, UInt64 roleId, UInt64 orderId, UInt32 income, const std::string& reason);

	/**
	 *@brief 发送未处理的收益
	 */
	void _SendUntreatProfit();

	/**
	 *@brief 增加未处理的收益
	 */
	void _AddUntreatProfit(UInt64 profitGuid, GoldConsignmentOrderPtr orderPtr, const ItemRewardVec& rewardVec, const std::string& content, UInt64 serialId,
		UInt32 profitType, UInt32 isHandle);

	/**
	 *@brief 未处理的收益存库
	 */
	void _DBInsertUntreatProfit(GoldConsignmentProfit& profit);

	/**
	 *@brief 未处理的收益更新
	 */
	void _DBUpdateUntreatProfit(UInt64 guid);

	/**
	 *@brief 构造返还邮件
	 */
	void _CreateUntreatMail(UInt32 profitType, UInt64 orderId, UInt32 orderType, std::string& reason, std::string& sender, std::string& title);

	/**
	 *@brief 收益字符串
	 */
	void CreateProfitStr(const ItemRewardVec& rewardVec, std::string& str);
	void ParseProfitStr(UInt64 guid, ItemRewardVec& rewardVec, const std::string& str);

	/**
	 *@brief 发送未处理的信用点券
	 */
	void _SendUntreatCreditPoint();

	/**
	 *@brief 增加未处理的信用点券
	 */
	void _AddUntreatCreditPoint(GoldConsignmentOrderPtr orderPtr, UInt32 creditPoint, UInt64 serialId, UInt32 isHandle);

	/**
	 *@brief 未处理的信用点券存库
	 */
	void _DBInsertUntreatCreditPoint(GoldConsignmentCreditPoint& creditPoint);

	/**
	 *@brief 未处理的信用点券更新
	 */
	void _DBUpdateUntreatCreditPoint(UInt64 guid);

	/**
	 *@brief 是否有不符合单价范围的订单
	 */
	bool _IsHasAbnormalOrder(UInt32 zoneId, UInt32 accId, UInt64 roleId);

	/**
	 *@brief 删除不符合今日单价范围的挡位
	 */
	void _RemoveAbnormalUnitPrice();

private:

	enum 
	{
		DB_LAST_PRICE = 1 << 0,
		DB_CUR_PRICE = 1 << 1,
		DB_RECENT_PRICE = 1 << 2,
		DB_TODAY_COST_POINT = 1 << 3,
		DB_TODAY_DEAL_NUM = 1 << 4,
	};

	// 休市开始时间
	UInt32 m_StartTradeStamp;
	// 上一个交易日均价
	UInt32 m_LastAveragePrice;
	// 当前均价，每次成交都要计算
	UInt32 m_CurAveragePrice;
	// 最近成交价，几分钟计算一次的均价
	UInt32 m_RecentAveragePrice;

	// 今天扣除的点券
	ObjUInt64 m_TodayCostPoint;
	// 今日成交笔数
	ObjUInt64 m_TodayDealNum;

	// 定时保存最近均价
	Avalon::SimpleTimer	m_RecentAverageTimer;

	// 3秒定时器
	Avalon::SimpleTimer	m_Timer3s;

	// 卖金订单(key->订单id)
	GoldConsignmentOrderMap m_SellOrderMap;
	// 买金订单(key->订单id)
	GoldConsignmentOrderMap m_BuyOrderMap;

	// 卖金挡位(key->单价)
	SellOrderGradeMap		m_SellGradeMap;
	// 买金挡位(key->单价)
	BuyOrderGradeMap		m_BuyGradeMap;

	// 账号订单(key->账号，区id，订单类型)
	std::map<GCAccOrderKey, GoldConsignmentOrderList> m_AccOrderMap;

	// 交易记录(key->角色id)
	std::map<UInt64, std::vector<GoldConsignmentTradeRecord>> m_RoleRecordMap;

	// 未处理的收益(只保存未处理的收益)
	std::map<UInt64, GoldConsignmentProfit> m_UntreatProfitMap;

	// 未处理的信用点券收益
	std::map<UInt64, GoldConsignmentCreditPoint> m_UntreatCreditPointMap;
};

#endif // !_GOLD_CONSIGMMENT_SERVICE_H_

