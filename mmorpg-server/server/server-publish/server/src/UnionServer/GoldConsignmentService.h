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
 *@brief ��Ҽ��۹���
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
	 *@brief ���󶩵�ҳ��
	 */
	void HandlerOrderPageReq(RoleIDConnID& info);

	/**
	 *@brief �ϼܶ���
	 */
	void HandlerSumbitOrderReq(USPlayer* player, UInt32 tradeType, UInt32 orderType, UInt32 unitPrice, UInt32 tradeNum, UInt32 param, UInt32 submitId);

	/**
	 *@brief ����ɽ���¼
	 */
	void HandlerDealRecordReq(USPlayer* player);

	/**
	 *@brief �ҵĶ���
	 */
	void HandlerOwnerOrderReq(USPlayer* player);

	/**
	 *@brief �¼ܶ���
	 */
	void HandlerCancelOrderReq(USPlayer* player, UInt32 orderType, UInt64 orderId);

	/**
	 *@brief ���������
	 */
	void ZoneActivityEnd(UInt32 zoneId, UInt32 endTime);

	/**
	 *@brief ��������
	 */
	bool LoadOrderData(CLRecordCallback* callback);

	/**
	 *@brief ���׼�¼����
	 */
	bool LoadTradeRecord(CLRecordCallback* callback);

	/**
	 *@brief ���׼�Ǯ
	 */
	bool LoadTradePrice(CLRecordCallback* callback);

	/**
	 *@brief δ���������
	 */
	bool LoadUntreatProfit(CLRecordCallback* callback);

	/**
	 *@brief δ��������õ�ȯ
	 */
	bool LoadUntreatCreditPoint(CLRecordCallback* callback);

	/**
	 *@brief ��ʼ������
	 */
	void InitUnitPrice();

	/**
	 *@brief �޸ĵ���
	 */
	void ModifyUnitPrice();

	/**
	 *@brief �������񷵻�
	 */
	bool SellGoldTransResult(USPlayer* player, GoldTradeThing* trans);

	/**
	 *@brief ������񷵻�
	 */
	bool BuyGoldTransResult(USPlayer* player, GoldTradeThing* trans);

public: // ��̨GM����

	/**
	 *@brief GM�����ύ����
	 */
	std::string GMSumbitOrder(UInt32 tradeType, UInt32 orderType, UInt32 unitPrice, UInt32 tradeNum);

private:

	/**
	 *@brief ����Ƿ�
	 */
	bool _CheckWavePrice(UInt32 unitPrice);

	/**
	 *@brief �Ƿ�����
	 */
	bool _IsClose();

	/**
	 *@brief ��齻��������
	 */
	void _CheckDealData();

	/**
	 *@brief �������
	 */
	void _CheckClose(const Avalon::Time& now);

	/**
	 *@brief �������Ƿ������¼�
	 */
	bool _ZoneIsCloseOrder(UInt32 zoneId);

	/**
	 *@brief �����Ҫ�¼ܵ�����
	 */
	void _CheckOrderCloseZone();

	/**
	 *@brief �����Ҫ�¼ܵĽ�ɫ
	 */
	void _CheckOrderCloseRole();

	/**
	 *@brief �����Ҫ�¼ܵĶ���
	 */
	void _CheckOrderClose();

	/**
	 *@brief ��鳬ʱ����
	 */
	void _CheckTimeOutOrder(const Avalon::Time& now);

	/**
	 *@brief �¼ܶ�������
	 */
	void _CancelOrderReturn(GoldConsignmentOrderPtr orderPtr, const std::string& reason, const std::string& title, const std::string& content, UInt32 profitType);

	/**
	 *@brief ������𶩵�
	 */
	UInt32 _CheckSellOrder(USPlayer* player, UInt32 tradeType, UInt32 unitPrice, UInt32 tradeNum, UInt32 param, UInt32 submitId);

	/**
	 *@brief �����𶩵�
	 */
	UInt32 _CheckBuyOrder(USPlayer* player, UInt32 tradeType, UInt32 unitPrice, UInt32 tradeNum, UInt32 submitId);

	/**
	 *@brief ���Ӷ���
	 */
	void _AddOrder(GoldConsignmentOrderPtr orderPtr);

	/**
	 *@brief ���ӵ�λ����
	 */
	void _AddGradeOrder(GoldConsignmentOrderPtr orderPtr);

	/**
	 *@brief �����˺Ŷ���
	 */
	void _AddAccOrder(GoldConsignmentOrderPtr orderPtr);

	/**
	 *@brief ���ӽ��׼�¼
	 */
	void _AddTradeRecord(GoldConsignmentTradeRecord& record);

	/**
	 *@brief �������
	 */
	void _DBInsertOrder(GoldConsignmentOrderPtr orderPtr);

	/**
	 *@brief ����ɾ��
	 */
	void _DBDelOrder(UInt64 orderId);

	/**
	 *@brief ��������������
	 */
	void _DBUpdateOrder(GoldConsignmentOrderPtr orderPtr);

	/**
	 *@brief ���׼�¼���
	 */
	void _DBInsertTradeRecord(GoldConsignmentTradeRecord& record);

	/**
	 *@brief �����Ǯ
	 */	
	void _DBInsertPrice();

	/**
	 *@brief ���¼�Ǯ
	 */
	void _DBUpdatePrice(UInt32 flag);

	/**
	 *@brief ƥ����𶩵�
	 */
	void _MatchBuyOrder(USPlayer* player, GoldConsignmentOrderPtr orderPtr, UInt32 submitId);

	/**
	 *@brief ƥ�����𶩵�
	 */
	void _MatchSellOrder(USPlayer* player, GoldConsignmentOrderPtr orderPtr, UInt32 tradeType, UInt32 submitId);

	/**
	 *@brief ƥ�����λ
	 */
	UInt32 _MatchBuyGrade(USPlayer* player, UInt32 tradeNum, UInt64 serialId, UInt32 sellUnitPrice, OrderGradeData& orderGrade, float& sellProfit);

	/**
	 *@brief ƥ������λ
	 */
	UInt32 _MatchSellGrade(USPlayer* player, UInt32 tradeNum, UInt64 serialId, UInt32 buyUnitPrice, OrderGradeData& orderGrade, UInt32& costPoint, UInt32& returnPoint);

	/**
	 *@brief ���ҵ�λ
	 */
	OrderGradeData* _FindOrderGradeData(UInt32 unitPrice, UInt32 orderType);

	/**
	 *@brief ɾ����λ
	 */
	void _RemoveOrderGrade(UInt32 unitPrice, UInt32 orderType);

	/**
	 *@brief ��λ�¼ܶ���
	 */
	void _GradeCancelOrder(GoldConsignmentOrderPtr orderPtr);

	/**
	 *@brief �˺��Ƿ����ĳ����
	 */
	GoldConsignmentOrderPtr _CreateOrder(USPlayer* player, UInt32 orderType, UInt32 unitPrice, UInt32 tradeNum, UInt32 totalPoint = 0);

	/**
	 *@brief �ҵ�
	 */
	void _SumbitOrder(GoldConsignmentOrderPtr orderPtr);

	/**
	 *@brief �����ϵ��۵���𶩵�
	 */
	UInt32 _CheckMatchUnitPriceBuyOrder(UInt32 accId, UInt32 unitPrice, UInt32 tradeNum);

	/**
	 *@brief �����ϵ��۵����𶩵�
	 */
	UInt32 _CheckMatchUnitPriceSellOrder(UInt32 accId, UInt32 unitPrice, UInt32 tradeNum);

	/**
	 *@brief ���Ҷ���
	 */
	GoldConsignmentOrderPtr _FindOrder(UInt32 orderType, UInt64 orderId);

	/**
	 *@brief ɾ������
	 */
	void _RemoveOrder(UInt32 orderType, UInt64 orderId);

	/**
	 *@brief �����˺Ŷ�����
	 */
	UInt32 _FindAccOrderNum(UInt32 accId, UInt32 zoneId, UInt32 orderType);

	/**
	 *@brief ������һ�����վ���
	 */
	void _SetLastAveragePrice(UInt32 price);

	/**
	 *@brief ���ý��վ���
	 */
	void _SetCurAveragePrice(UInt32 price);

	/**
	 *@brief �����������
	 */
	void _SetRecentAveragePrice(UInt32 price);

	/**
	 *@brief �˺������õ�ȯԤ������
	 */
	UInt32 _EstimateAccCreditPointProfit(UInt32 zoneId, UInt32 accId);

	/**
	 *@brief ֪ͨ�ɽ�
	 */
	void _NotifyOrderDeal(UInt32 zoneId, UInt64 roleId);

	/**
	 *@brief �˺��Ƿ����ĳ����
	 */
	bool _RoleIsHasOrder(USPlayer* player, UInt32 orderType, UInt64 orderId);

	/**
	 *@brief ���ӳɽ���
	 */
	void _AddDealInfo(UInt32 costPoint, UInt32 dealNum);

	/**
	 *@brief �������õ�ȯ
	 */
	bool _SendCreditPointIncome(UInt32 zoneId, UInt32 accId, UInt64 roleId, UInt64 orderId, UInt32 income, const std::string& reason);

	/**
	 *@brief ����δ���������
	 */
	void _SendUntreatProfit();

	/**
	 *@brief ����δ���������
	 */
	void _AddUntreatProfit(UInt64 profitGuid, GoldConsignmentOrderPtr orderPtr, const ItemRewardVec& rewardVec, const std::string& content, UInt64 serialId,
		UInt32 profitType, UInt32 isHandle);

	/**
	 *@brief δ�����������
	 */
	void _DBInsertUntreatProfit(GoldConsignmentProfit& profit);

	/**
	 *@brief δ������������
	 */
	void _DBUpdateUntreatProfit(UInt64 guid);

	/**
	 *@brief ���췵���ʼ�
	 */
	void _CreateUntreatMail(UInt32 profitType, UInt64 orderId, UInt32 orderType, std::string& reason, std::string& sender, std::string& title);

	/**
	 *@brief �����ַ���
	 */
	void CreateProfitStr(const ItemRewardVec& rewardVec, std::string& str);
	void ParseProfitStr(UInt64 guid, ItemRewardVec& rewardVec, const std::string& str);

	/**
	 *@brief ����δ��������õ�ȯ
	 */
	void _SendUntreatCreditPoint();

	/**
	 *@brief ����δ��������õ�ȯ
	 */
	void _AddUntreatCreditPoint(GoldConsignmentOrderPtr orderPtr, UInt32 creditPoint, UInt64 serialId, UInt32 isHandle);

	/**
	 *@brief δ��������õ�ȯ���
	 */
	void _DBInsertUntreatCreditPoint(GoldConsignmentCreditPoint& creditPoint);

	/**
	 *@brief δ��������õ�ȯ����
	 */
	void _DBUpdateUntreatCreditPoint(UInt64 guid);

	/**
	 *@brief �Ƿ��в����ϵ��۷�Χ�Ķ���
	 */
	bool _IsHasAbnormalOrder(UInt32 zoneId, UInt32 accId, UInt64 roleId);

	/**
	 *@brief ɾ�������Ͻ��յ��۷�Χ�ĵ�λ
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

	// ���п�ʼʱ��
	UInt32 m_StartTradeStamp;
	// ��һ�������վ���
	UInt32 m_LastAveragePrice;
	// ��ǰ���ۣ�ÿ�γɽ���Ҫ����
	UInt32 m_CurAveragePrice;
	// ����ɽ��ۣ������Ӽ���һ�εľ���
	UInt32 m_RecentAveragePrice;

	// ����۳��ĵ�ȯ
	ObjUInt64 m_TodayCostPoint;
	// ���ճɽ�����
	ObjUInt64 m_TodayDealNum;

	// ��ʱ�����������
	Avalon::SimpleTimer	m_RecentAverageTimer;

	// 3�붨ʱ��
	Avalon::SimpleTimer	m_Timer3s;

	// ���𶩵�(key->����id)
	GoldConsignmentOrderMap m_SellOrderMap;
	// ��𶩵�(key->����id)
	GoldConsignmentOrderMap m_BuyOrderMap;

	// ����λ(key->����)
	SellOrderGradeMap		m_SellGradeMap;
	// ���λ(key->����)
	BuyOrderGradeMap		m_BuyGradeMap;

	// �˺Ŷ���(key->�˺ţ���id����������)
	std::map<GCAccOrderKey, GoldConsignmentOrderList> m_AccOrderMap;

	// ���׼�¼(key->��ɫid)
	std::map<UInt64, std::vector<GoldConsignmentTradeRecord>> m_RoleRecordMap;

	// δ���������(ֻ����δ���������)
	std::map<UInt64, GoldConsignmentProfit> m_UntreatProfitMap;

	// δ��������õ�ȯ����
	std::map<UInt64, GoldConsignmentCreditPoint> m_UntreatCreditPointMap;
};

#endif // !_GOLD_CONSIGMMENT_SERVICE_H_

