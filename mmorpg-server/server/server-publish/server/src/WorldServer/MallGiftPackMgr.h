#ifndef _MALL_GIFT_PACK_MGR_H_
#define _MALL_GIFT_PACK_MGR_H_


#include <CLMallDefine.h>


class MallGiftPack;
class WSPlayer;

//礼包id-->礼包对象
typedef HashMap<UInt32, MallGiftPack*> RoleMallGiftPackMap;

//礼包条件-->计数
typedef HashMap<UInt8, UInt16>	CondCounterMap;

//道具集
//typedef std::vector<MallItemInfo> MallItemInfosVec;


//已激活限时礼包管理类
class MallGiftPackMgr
{
public:
	MallGiftPackMgr();
	~MallGiftPackMgr();

public:
	//设置所有者
	void SetOwner(WSPlayer* player) { m_Owner = player; }
	WSPlayer* GetOwner() const { return m_Owner; }
	
	//激活商城限时礼包
	void ActivateLimitGiftPack(UInt8 activateCond);

	//设置获取本角色下的限时礼包
	bool SetRoleMallGiftPack(MallGiftPack* giftPack);
	MallGiftPack* GetMallGiftPack(UInt32 mallGoodsId);

	//设置本角色下 条件-->计数
	void SetRoleCondCounter(UInt8 cond, UInt16 counter);

	//增加本账号下 条件-->计数
	void AddAccountCondCounter(UInt8 cond, UInt16 counter);

private:
	//创建礼包
	UInt32 _CreateGiftPack(MallItemInfo* itemInfo, MallItemInfosVec& itemInfos, UInt8 activateCond);

	//本角色下该条件是否可以激活礼包
	bool _IsRoleLimitActivateGiftPack(UInt8 activateCond, UInt16 limitTimes);

	//本账号下该条件是否可以激活礼包
	bool _IsAccLimitActivateGiftPack(UInt8 activateCond, UInt16 limitTimes);

	//激活多选一礼包
	UInt32 _ActivateChooseOneGiftPack(UInt8 activateCond, MallItemInfosVec& itemInfos);

	//发送至客户端
	void _SendToClient(MallItemInfosVec items, UInt32 errorCode);

	//回收限时礼包
	void _DestroyGiftPacks();

private:
	//玩家(一个角色)
	WSPlayer*			m_Owner;

	//本角色下的已激活限时礼包
	RoleMallGiftPackMap m_RoleGiftPacks;

	//本角色下已触发条件计数
	CondCounterMap m_RoleCondCounter;

	//本账号下已触发条件计数
	CondCounterMap m_AccCondCounter;
};

#endif