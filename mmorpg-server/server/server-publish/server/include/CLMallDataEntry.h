#ifndef _CL_MALL_DATAENTRY_H_
#define _CL_MALL_DATAENTRY_H_

#include "CLDefine.h"
#include <CLMallDefine.h>

#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief 商城道具数据项
*/
struct MallItemDataEntry : public Avalon::DataEntry
{
public:
	MallItemDataEntry();
	~MallItemDataEntry();
	
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 48; };

public:
	// 商品id
	UInt32					id;
	// 类别
	UInt8					type;
	// 子类别
	UInt8					subType;
	// 职业类别
	UInt8					jobType;
	// 道具id
	UInt32					itemId;
	// 一组数量
	UInt32					groupNum;
	// 原价
	UInt32					price;
	// 折扣价
	UInt32					disPrice;
	// 货币类型
	UInt8					moneyType;
	// 限购类型
	UInt8					limitType;
	// 限购数量
	UInt16					limitNum;
	// 礼包内容
	std::vector<ItemReward>	giftPackItems;
	// icon
	std::string				giftPackIcon;
	// 时间计算类型
	UInt8					timeCalcType;
	// 开始时间
	UInt32					limitStartTime;
	// 结束时间
	UInt32					limitEndTime;
	// 新服（合服算新服）第几天开始
	std::string				startTimeFromService;
	// 新服（合服算新服）第几天结束
	std::string				endTimeFromService;
	// 新服（合服不算新服）第几天开始
	std::string				startTimeFromFirstService;
	// 新服（合服不算新服）第几天结束
	std::string				endTimeFromFirstService;
	// 开启间隔
	std::string				openInterval;
	// 关闭间隔
	std::string				closeInterval;
	// 总限购次数
	UInt16					limitTotalNum;
	// 购买后获得的vip积分
	UInt16					vipScore;
	// 礼包名字
	std::string				giftPackName;
	// 标签,1.热门
	UInt8					tagType;
	// 排序序号
	UInt32					sort;
	// 热门排序序号
	UInt32					hotSort;
	// 商品类型
	UInt8					goodsType;
	// 商品子类别
	UInt16					goodsSubType;
	// 商品状态
	UInt8					goodsState;
	// 是否推荐
	UInt8					isRecommend;
	// 私人推荐条件
	UInt8					personalTailorActivateCond;
	// 打折率(当前为运营配置，表格只需配置大于1)
	UInt32					discountRate;
	// 登录推送表id
	UInt8					loginPushId;
	// 时装推荐图片路径
	std::string				fashionImagePath;
	// 描述
	std::string				desc;
	// 购买获得物信息
	MallBuyGotInfoVec		buyGotInfos;
	// 扩展参数
	std::vector<UInt32>		params;
	// 玩家等级限制
	UInt16					playerLevelLimit;
	// 商城推送条件
	UInt8					mallPushCond;
	// 账号刷新类型
	TimeUtil::RefreshType	accountRefreshType;
	// 账号刷新日期
	TimeUtil::RefreshDate	accountRefreshDate;
	// 账号限购数量
	UInt32					accountLimitBuyNum;
	// 折扣券id
	UInt32					discountCouponId;
	// 积分倍率
	UInt8                   multiple;
	// 抵扣券id
	UInt32					deductionCouponId;
	// 是否可用信用点券
	UInt32					creditPointDeduction;
};

typedef std::vector<MallItemDataEntry*> MallItemsVec;

/**
*@brief 商城道具管理器
*/
class MallItemDataEntryMgr : public Avalon::DataEntryMgrBase<MallItemDataEntry>
	, public Avalon::Singleton<MallItemDataEntryMgr>
{
public:
	MallItemDataEntryMgr();
	~MallItemDataEntryMgr();

	bool AddEntry(MallItemDataEntry* entry);

	bool CheckGiftPack(MallItemDataEntry* entry);

	//获取商城道具
	MallItemsVec& GetMallItems() { return m_MallItems; }

	//获取商城价格信息(废弃）
	UInt32 GetMallItemPrice(UInt32 uItemId);

private:
	// 商城道具的价格信息(废弃)
	std::map<UInt32, UInt32>	m_mapMallItemPrice;

	//商城道具
	MallItemsVec m_MallItems;
};

#endif