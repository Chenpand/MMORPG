#ifndef _ITEM_MAKER_H_
#define _ITEM_MAKER_H_

#include <AvalonSingleton.h>
#include <CLItemDefine.h>
#include <vector>
#include "Item.h"

/**
 *@brief 道具创建器
 */
class ItemMaker : public Avalon::Singleton<ItemMaker>
{
public:
	ItemMaker();
	~ItemMaker();

public:
	/**
	 *@brief 生成一个道具
	 *@param itemid	道具表id
	 *@param num 数量
	 *@param qualityLv 0随机 非0指定品级(1-100)
	 *@param strenth 强化等级
	 */
	Item* MakeItem(Player* player, UInt32 itemid, UInt16 num, UInt8 qualityLv = 0, UInt8 strenth = 0, UInt32 auctionCoolTimeStamp = 0,
		UInt8 equipType = EQUIP_NORMAL, UInt8 enhanceType = ENHANCE_INVALID, bool isBuildWeapon = false, UInt32 auctionTransTimes = 0);

	Item* MakeItemWithCallback(Player* player, UInt32 itemid, UInt16 num, const std::function<bool(Item&)>& initCallback, UInt8 qualityLv = 0, UInt8 strenth = 0, UInt32 auctionCoolTimeStamp = 0);
	
	//用于生成和角色无关的道具
	Item* MakeItem(UInt32 itemid, UInt16 num, UInt8 qualityLv = 0, UInt8 strenth = 0, UInt32 auctionCoolTimeStamp = 0);

	//旧的品级调整接口
	bool	GetRandQLValue(Item* item);

	bool SetQLValue(Item* item, EquipQualityLv ql);

	/**
	*@brief 生成装备随机属性
	*/
	bool	GenEquipAttrs(Item* item);

private:

	//生成data复制属性,用于称号和时装等不用计算基础属性的装备
	bool	GenDataAttrs(Item* item, EquipAttrDataEntry* attrData);
	
	UInt32	GetRandAttrNum(ItemQuality color);
	void	GetRandAttr(std::vector<UInt32>& attrs, Int32 num);
	void	GetRandValue(std::vector<UInt32>& attrs, ItemQuality color, UInt16 lv, std::vector<UInt32>& vals);

public:
	UInt32	randByWeight(std::vector<UInt32>& vals, std::vector<UInt32>& wts);
	Int32	randByWeight(std::vector<UInt32>& wts);
	void	randByWeightCplx(std::vector<UInt32>& vals, std::vector<UInt32>& wts, std::vector<UInt32>& out, Int32 num = 1);
private:
};


#endif
