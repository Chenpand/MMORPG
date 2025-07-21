#ifndef _MALL_CHECK_BUY_MGR_
#define _MALL_CHECK_BUY_MGR_


#include <CLMallProtocol.h>
#include <CLCounterNameDefine.h>

class Player;


class MallCheckBuyInfo
{
public:
	MallCheckBuyInfo();
	~MallCheckBuyInfo();

public:
	void Init(const CLProtocol::WorldMallCheckBuy &req);

	void SetOwner(Player* owner){ m_Owner = owner; }
	Player* GetOwner() const { return m_Owner; }

	/**
	*@brief 购买商城商品
	*/
	void OnBuy();

private:
	/**
	*@brief 是否是玩家绑定礼包
	*/
	bool _IsPlayerBindGiftPack();

	/**
	*@brief 是否限购
	*/
	bool _IsLimitBuy(const std::string& mallItemIdStr);

	/**
	*@brief 获得道具对应的数量
	*/
	UInt32 _GetItemNum(UInt32 itemDataId);

	/**
	*@brief 将道具发送到玩家
	*/
	void _AddItemsToPlayer(const std::string& itemReason, std::vector<UInt64>& itemUids);

	/**
	*@brief 将道具发送到邮件
	*/
	UInt32 _AddItemsToMail(const std::string& reason, const std::vector<UInt32>& mailItems);

	/**
	*@brief 将道具发送给代付玩家(邮件方式)
	*/
	void _SendMailToAddonPayPlayer(UInt64 tarId, UInt32 mailItem);

	//获取计数name
	std::string _GetCounterName();

	/**
	*@brief 将道具id拼接字符串
	*/
	std::string _GetItemStr(const std::vector<UInt32>& mailItems);

	/**
	*@brief 是否时装
	*/
	bool _IsFashion(UInt8 itemSubType);

	/**
	*@brief 获取商城最终的打折率
	*/
	UInt32 _GenMallUtilDiscountRate(Player* player, UInt32 baseRate);

private:
	//拥有者
	Player*					m_Owner;
	// 玩家id
	ObjID_t					m_PlayerId;
	// 账号id
	UInt32					m_AccId;
	// 购买费用
	UInt32					m_Cost;
	// 货币类型
	UInt8					m_MoneyType;
	// 商城商品id
	UInt32					m_MallItemId;
	// 组数
	UInt16					m_MallItemNum;
	// 道具id集
	std::vector<UInt32>		m_ItemDataId;
	// 道具id-->数量
	std::map<UInt32, UInt32> m_ItemData;
	// 获得道具的玩家id
	ObjID_t					m_Receiver;
	// 限购刷新类型
	MallLimitBuyType		m_MallLimitBuyType;
	// 限购次数
	UInt32					m_LimitNum;
	// 总限购次数
	UInt32					m_TotalLimitNum;
	// 商品类型
	UInt8					m_GoodsType;
	// 购买后获得的vip积分
	UInt16					m_VipScore;
	// 打折率
	UInt32					m_DiscountRate;
	// 折扣券id
	UInt32					m_DiscountCouponId;
};

#endif