#ifndef _WS_ITEM_MGR_H_
#define _WS_ITEM_MGR_H_

#include "WSPackage.h"
#include "CLObjectIndex.h"
#include <AvalonSimpleTimer.h>

class Tribe;
class WSPlayer;

/**
 *@brief 道具管理器
 */
class WSItemMgr
{
public:
	WSItemMgr();
	~WSItemMgr();

public:
	/**
	 *@brief 设置获取拥有者
	 */
	void SetOwner(Tribe* owner) { m_pOwner = owner; }
	Tribe* GetOwner() const { return m_pOwner; }

	/** 
	 *@brief 获取包裹
	 */
	WSPackage* GetPackage(UInt8 pack) const;

	/**
	 *@brief 查找道具
	 */
	WSItem* FindItem(ObjID_t id);

	/**
	 *@brief 加载一个道具
	 */
	bool LoadItem(WSItem* item);

	/**
	 *@brief 移除道具
	 */
	UInt16 RemoveItem(WSPlayer *pPlayer, WSItem* item, UInt16 uNum);
	void RemoveItemDirect(WSItem *item);

	/**
	 *@brief 批量添加和删除道具
	 */
	bool CheckAddItem(UInt32 itemid, UInt16 num, UInt8 quality, UInt8 pack, UInt8 strengthen);
	UInt16 GetGridUsed(UInt32 itemid, UInt16 num, UInt8 quality, UInt8 pack, UInt8 strengthen);
	UInt16 AddItem(WSPlayer *pPlayer, WSItem* item, UInt8 pack, UInt32 price = 0);
	bool IsFull(UInt8 pack);

	/**
	*@brief 锁定
	*/
	void Lock() { m_uLock = 3; }
	void UnLock() { m_uLock = 0; }
	bool IsLocked() { return m_uLock != 0; }

public://同步相关
	/**
	 *@brief 同步给玩家
	 */
	void SyncToPlayer(WSPlayer *pPlayer, UInt8 pack);

	/**
	 *@brief 同步单个道具到客户端
	 */
	void SyncToPlayer(WSPlayer *pPlayer, WSItem* item);

	/**
	*@brief 保存单个道具
	*/
	void SaveItem(WSItem *pItem);
	
public:
	/**
	 *@brief 访问所有道具
	 */
	void VisitItems(WSItemVisitor& visitor);

	/**
	 *@brief 访问指定包裹道具
	 */
	void VisitItems(UInt8 pack, WSItemVisitor& visitor);
	
public://事件
	/**
	 *@brief 心跳
	 */
	void OnTick(const Avalon::Time& now);

private:
	/**
	 *@brief 加道具到指定位置
	 */
	bool AddItem(WSPlayer *pPlayer, WSItem* item, const ItemPos pos, UInt32 uPrice = 0);

private:
	//拥有者
	Tribe*		m_pOwner;
	//包裹
	WSPackage*	m_pPackages[TRIBE_PACK_MAX];
	//道具列表
	CLIDObjIndex<WSItem>	m_Items;
	// 锁定标识
	UInt8		m_uLock;
	// 锁定计时器
	Avalon::SimpleTimer		m_timer;

};

#endif

