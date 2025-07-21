#ifndef _WS_MALL_SCRIPT_H_
#define _WS_MALL_SCRIPT_H_

#include <CLDefine.h>
#include <AvalonScriptFile.h>
#include <CLMallDefine.h>


/**
 *@brief 商城脚本
 */
class MallScript : public Avalon::ScriptFile
{
public:
	MallScript(Avalon::ScriptState* state);
	~MallScript();

	void Init();

	/**
	*@brief 清除全部信息
	*/
	void ClearMall();

	/**
	*@brief 添加一般限量道具信息
	*/
	void AddNormalLimitItem(UInt32 uIndex, const char *szItem);

	/**
	*@brief 添加开服限量道具
	*/
	void AddGSLimitItem(const char *szItem);

	/**
	*@brief 添加节日限量道具
	*/
	void AddFestivalLimitItem(const char *szActivity, const char *szItem);

	/**
	*@brief 添加金石商城道具
	*/
	void AddGoldStoneItem(const char *szItem);

	/**
	*@brief 添加银石商城道具
	*/
	void AddSilverStoneItem(const char *szItem);

	/**
	*@brief 设置当前的限量道具
	*/
	void SetCurLimitedItems();
	void SetCurFestivalItems();

	/**
	*@brief 编码玩家可购买的开服道具
	*/
	void GetGSItems(UInt32 uBuyRecord, std::list<UInt32> &listBuy);

	/**
	*@brief 获得开服限量道具的索引
	*/
	UInt32 GetGSItemIndex(UInt32 uId);

	// 添加商城道具
	void AddMallItem(UInt8 type, UInt8 subType, UInt8 job, const char *szItem);

	// 获得礼包详情
	const char* GetGiftDetail(UInt32 giftId);

	//上架商品
	void OnSaleMallWaitGoods(UInt32 goodId);

	//下架商品
	void OffSaleMallWaitGoods(UInt32 goodId);

private:
	void DecodeItemInfo(MallItemInfo &item, const char *szItem);

	void SetGSItems();
	bool SetCurNormalItems(UInt32 uRecords[4]);

private:
	//typedef std::vector<MallItemInfo>			VecItem;

	//std::map<ItemKey, VecItem>	m_allItems;

	// 一般限量道具信息
	std::vector<MallItemInfo>	m_vecLimitedInfo[4];
	// 开服限量道具信息
	std::vector<MallItemInfo>	m_vecGSItems;
	// 节日限量道具
	std::map<std::string, std::vector<MallItemInfo> >	m_mapFestivalItems;
	// 金石商城
	std::vector<MallItemInfo>	m_vecGoldStoneItems;
	// 银石商城
	std::vector<MallItemInfo>	m_vecSilverStoneItems;
};

#endif
