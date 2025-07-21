#ifndef _ITEM_MAKER_H_
#define _ITEM_MAKER_H_

#include <AvalonSingleton.h>
#include <CLItemDefine.h>
#include <vector>
#include "Item.h"

/**
 *@brief ���ߴ�����
 */
class ItemMaker : public Avalon::Singleton<ItemMaker>
{
public:
	ItemMaker();
	~ItemMaker();

public:
	/**
	 *@brief ����һ������
	 *@param itemid	���߱�id
	 *@param num ����
	 *@param qualityLv 0��� ��0ָ��Ʒ��(1-100)
	 *@param strenth ǿ���ȼ�
	 */
	Item* MakeItem(Player* player, UInt32 itemid, UInt16 num, UInt8 qualityLv = 0, UInt8 strenth = 0, UInt32 auctionCoolTimeStamp = 0,
		UInt8 equipType = EQUIP_NORMAL, UInt8 enhanceType = ENHANCE_INVALID, bool isBuildWeapon = false, UInt32 auctionTransTimes = 0);

	Item* MakeItemWithCallback(Player* player, UInt32 itemid, UInt16 num, const std::function<bool(Item&)>& initCallback, UInt8 qualityLv = 0, UInt8 strenth = 0, UInt32 auctionCoolTimeStamp = 0);
	
	//�������ɺͽ�ɫ�޹صĵ���
	Item* MakeItem(UInt32 itemid, UInt16 num, UInt8 qualityLv = 0, UInt8 strenth = 0, UInt32 auctionCoolTimeStamp = 0);

	//�ɵ�Ʒ�������ӿ�
	bool	GetRandQLValue(Item* item);

	bool SetQLValue(Item* item, EquipQualityLv ql);

	/**
	*@brief ����װ���������
	*/
	bool	GenEquipAttrs(Item* item);

private:

	//����data��������,���ڳƺź�ʱװ�Ȳ��ü���������Ե�װ��
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
