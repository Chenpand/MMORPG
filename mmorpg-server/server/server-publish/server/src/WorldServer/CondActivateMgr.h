#ifndef _COND_ACTIVATE__MGR_
#define _COND_ACTIVATE__MGR_

#include "CondActivateInfo.h"

#include <CLMallDefine.h>
#include <AvalonSimpleTimer.h>
#include <CLMallPersonalTailorDataEntry.h>
#include <CLMallGiftPackDataEntry.h>


// 私人订制触发状态
enum PersonalTailorActivateState
{
	// 初始化
	PTAS_INIT = 0,
	// 已开启
	PTAS_OPEN,
	// 已关闭
	PTAS_CLOSED,
	// 上线
	PTAS_ONLINE,
};

// 触发条件类型
enum ActivateCondType
{
	// 无类型
	COND_TYPE_NONE = 0,
	// 共用触发
	COND_TYPE_SHARE,
};

/**
*@brief 触发计数key
*/
struct ActivateCountKey
{
	ActivateCountKey(UInt8 type, UInt8 cond) : m_ActivateType(type), m_ActivateCond(cond) {}

	// 触发类型
	UInt8 m_ActivateType;
	// 触发条件
	UInt8 m_ActivateCond;

	bool operator < (const ActivateCountKey& other) const
	{
		if (m_ActivateType < other.m_ActivateType)
		{
			return true;
		}
		if (other.m_ActivateType < m_ActivateType)
		{
			return false;
		}

		if (m_ActivateCond < other.m_ActivateCond)
		{
			return true;
		}
		if (other.m_ActivateCond < m_ActivateCond)
		{
			return false;
		}

		return false;
	}
};


/**
*@brief 条件触发访问器
*/
//typedef CLVisitor<CondActivateInfo>	CondActivateInfoVisitor;

/**
*@brief 条件触发管理类
*/
class CondActivateMgr
{

public:
	CondActivateMgr();
	~CondActivateMgr();

public:
	/**
	*@brief 设置获取拥有者
	*/
	void SetOwner(WSPlayer* player) { m_Owner = player; }
	WSPlayer* GetOwner() const { return m_Owner; }

	/**
	*@brief 触发一个条件
	*/
	void ActivateCond(UInt8 activateCond, UInt8 activateType);

	/**
	*@brief 增加条件触发
	*/
	bool AddCondActivateInfo(CondActivateInfo* info);

	/**
	*@brief 上线处理
	*/
	void OnOnline();

	/**
	*@brief 心跳
	*/
	void OnTick(const Avalon::Time& now);

	//void VisitCondActivateInfo(CondActivateInfoVisitor& visitor);

	/**
	*@brief 获取条件触发
	*/
	CondActivateInfo* FindCondActivateInfoByCond(UInt8 activateType, UInt8 activateCond);

	/**
	*@brief 获取已开启的条件触发
	*/
	CondActivateInfo* FindOpenCondActivateInfoByCond(UInt8 activateType, UInt8 activateCond);
	CondActivateInfo* FindOpenCondActivateInfoByGoodsId(UInt8 activateType, UInt32 mallGoodsId);

private:
	/* 私人订制 */
	/**
	*@brief 触发私人订制
	*/
	void _ActivatePersonalTailor(UInt8 activateCond);

	/**
	*@brief 检查是否限制创建私人订制
	*/
	bool _IsLimitCreatePersonalTailor(MallPersonalTailorDataEntry* dataEntry);

	/**
	*@brief 同步客户端私人订制
	*/
	void _SyncMallPersonalTailorState(PersonalTailorActivateState state);

private:
	/* 限时礼包 */
	/**
	*@brief 触发限时礼包
	*/
	void _ActivateLimitTimeGiftPack(UInt8 activateCond);

	/**
	*@brief 检查是否限制创建限时礼包
	*/
	bool _IsLimitCreateLimitTimeGiftPack(MallGiftPackDataEntry* dataEntry);

private:
	/* 通用的接口 */
	/**
	*@brief 创建条件触发
	*/
	void _CreateCondActivate(UInt8 activateCond, UInt32 mallGoodsId, UInt16 limitInterval, ConditionActivateType activateType, UInt8 activateSubCond = 0);

private:
	// 玩家(一个角色)
	WSPlayer*			m_Owner;

	// 本账号下已触发过的条件的计数
	std::map<ActivateCountKey, UInt16>	m_ActivatedCondCountOnAccount;

	// 一个角色下的全部触发条件
	std::vector<CondActivateInfo*>	m_AllCondActivateInfo;

	// 私人订制推送状态
	PersonalTailorActivateState		m_PersonalTailorPushState;
};

#endif