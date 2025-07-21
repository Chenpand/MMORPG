#ifndef _ACTIVATE_COND_CHECK_EVENT_H_
#define _ACTIVATE_COND_CHECK_EVENT_H_

#include <CLMallDefine.h>
#include <CLMallPersonalTailorDataEntry.h>


class Player;
class Item;

/**
*@brief 商城私人订制触发条件
*/
enum MallPersonalTailorActiavateCond
{
	// 非法
	MPTAC_INVALID,
	// 等级达到50级，携带紫色武器或除泰拉武器外的粉色武器，有充值金额，没购买过破空石，弹出破空石购买
	MPTAC_ONE,
	// 等级达到50级，携带紫色戒指或除泰拉戒指外的粉色戒指，有充值金额，没购买过破空石，弹出破空石购买
	MPTAC_TWO,
	// 等级达到50级，携带紫色项链或除泰拉项链外的粉色项链，有充值金额，没购买过破空石，弹出破空石购买
	MPTAC_THREE,
	// 等级达到50级，携带紫色手镯或除泰拉手镯外的粉色手镯，有充值金额，没购买过破空石，弹出破空石购买
	MPTAC_FOUR,
	// 累计充值达到30元~200元，未拥有魔罐称号以及商场称号，弹出拉个什神的意志称号购买
	MPTAC_FIVE,
	// 全身强化≥8级，但无色晶体不足100个，弹出无色晶体购买
	MPTAC_SIX,
	// 全身强化≥10级，但有色晶体不足50个，弹出有色晶体购买
	MPTAC_SEVEN,
	// 全身强化≥8级，但绑定金币不足1万，弹出绑金购买
	MPTAC_EIGHT,
	// 等级达到30级，没有充值记录，弹出金币购买
	MPTAC_NINE,
	// 等级达到35级，一件时装都没有，弹出时装购买
	MPTAC_TEN,
};

/**
*@brief 数量限制
*/
enum NumLimit
{
	// 数量不限制
	NUM_LIMIT_NONE = 0,
	// 数量限制50个
	NUM_LIMIT_FIFTY = 50,
	// 数量限制100个
	NUM_LIMIT_ONE_HUNDRED = 100,
	// 数量限制1万个
	NUM_LIMIT_TEN_THOUNSAND = 10000,
};

/**
*@brief 玩家等级限制
*/
enum PlayerLevelLimiy
{
	// 不限制等级
	PLL_NONE = 0,
	// 限制50级
	PLL_FIFTY = 50,
};

/**
*@brief 强化等级限制
*/
enum StrengthLevelLimit
{
	// 不限制强化等级
	SLL_NONE = 0,
	// 强化等级限制限制8级
	SLL_EIGHT = 8,
	// 强化等级限制限制10级
	SLL_TEN = 10,
};

/**
*@brief 条件触发子类型
*/
enum CondActivateSubType
{
	// 全部条件判断一次
	CAST_ALL = 0,
	// 根据玩家等级判断
	CAST_PLAYER_LEVEL,
	// 根据道具强化等级判断
	CAST_ITEM_STRENGEN_LEVEL,
	// 根据充值金额判断
	CAST_CHARGE_NUM,
};

/**
*@brief 充值区间限制
*/
#define CHARGE_LEFT_BOUNDARY_VALUE 30
#define CHARGE_RIGHT_BOUNDARY_VALUE 200


/**
*@brief 触发条件检查
*/
class ActivateCondCheckEvent : public Avalon::Singleton<ActivateCondCheckEvent>
{
public:
	ActivateCondCheckEvent();
	~ActivateCondCheckEvent();

public:
	/**
	*@brief 初始化
	*/
	void Init();

	/**
	*@brief 触发一个条件
	*/
	void OnActivate(Player* player, ConditionActivateType condType, CondActivateSubType subType);

	/**
	*@brief 请求触发
	*/
	void RequestActivate(UInt64 roleId, UInt8 cond, UInt8 type);

	/**
	*@brief 获取符合条件的装备数量
	*/
	UInt16 GetEquipNumQualified(Player* player, ItemSubType itemSubType);

private:
	/**
	*@brief 触发私人订制
	*/
	void _OnActivatePersonalTailor(Player* player, CondActivateSubType subType);

	/**
	*@brief 获得符合条件的称号数量
	*/
	UInt16 _GetTitleNumQualified(Player* player);

	/**
	*@brief 获得符合条件的时装数量
	*/
	UInt16 _GetFashonNumQualified(Player* player);

	/**
	*@brief 获得符合条件的已强化的道具数量
	*/
	UInt16 _GetStrengenEquipNumQualified(Player* player, UInt8 strengthLevel);

	/**
	*@brief 获得包裹中的道具
	*/
	void _GetItemFormPackage(Player* player, PackType packType, std::vector<Item*>& items);


private:
	// 限制装备
	std::vector<UInt32>		m_LimitEquips;

	// 限制称号
	std::vector<UInt32>		m_LimitTitles;

	// 私人订制表数据
	std::vector<MallPersonalTailorDataEntry*> m_PersonalTailorTableDatas;
};

#endif