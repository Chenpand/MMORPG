#ifndef _ITEM_FROZEN_MGR_H_
#define _ITEM_FROZEN_MGR_H_

#include <set>
#include <CLDefine.h>

struct CurrencyFrozen
{
	CurrencyFrozen() : guid(0), roleId(0), itemId(0), num(0), frozenTime(0) {}

	// guid
	UInt64 guid;
	// 角色id
	UInt64 roleId;
	// 物品id
	UInt32 itemId;
	// 数量
	UInt32 num;
	// 时间
	UInt64 frozenTime;
	// reason
	std::string reason;

	std::string SerializeToString()
	{
		std::stringstream ss;
		ss << " guid:" << guid;
		ss << " roleId:" << roleId;
		ss << " itemId:" << itemId;
		ss << " num:" << num;
		ss << " frozenTime:" << frozenTime;
		ss << " reason:" << reason;

		return ss.str();
	};
};

class CLRecordCallback;

/**
*@brief 货币冻结管理
*/
class CurrencyFrozenMgr : public Avalon::Singleton<CurrencyFrozenMgr>
{
public:
	CurrencyFrozenMgr();
	~CurrencyFrozenMgr();

	/**
	*@brief 初始化
	*/
	void Init();

	/**
	*@brief 申请冻结
	*/
	UInt64 ApplyFrozen(UInt64 roleId, UInt32 itemId, UInt32 num, const std::string& reason);

	/**
	*@brief 解除冻结
	*/
	UInt32 RemoveFrozen(UInt64 guid);

	/**
	*@brief 扣除冻结
	*/
	UInt32 DeductFrozen(UInt64 guid);

	/**
	*@brief 获取玩家订单信息
	*/
	void GetRoleFrozenOrder(UInt64 roleId, std::vector<CurrencyFrozen>& orderVec);

	/**
	*@brief 加载
	*/
	void LoadCurrencyFrozen(CLRecordCallback* callback);

private:

	void _AddRoleOrder(UInt64 roleId, UInt64 orderId);
	void _DelRoleOrder(UInt64 roleId, UInt64 orderId);

	void _InsertFrozen(const CurrencyFrozen& frozen);
	void _DelFrozen(UInt64 guid);

private:

	// 冻结列表(key->订单id)
	HashMap<UInt64, CurrencyFrozen> m_FrozenMap;
	// 玩家订单(key->角色id，val->订单列表)
	HashMap<UInt64, std::set<UInt64>> m_RoleOrderMap;
};

#endif
