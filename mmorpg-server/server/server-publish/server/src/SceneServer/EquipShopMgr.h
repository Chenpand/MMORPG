#ifndef _SCENE_EQUIP_SHOP_H_
#define _SCENE_EQUIP_SHOP_H_

#include <CLDefine.h>
#include <CLObjectProperty.h>
#include <CLItemProtocol.h>

class Player;

/**
*@brief 随机装备商店
*/
class EquipShopMgr : public CLComplexProperty
{
	const static UInt32 NORMAL_SHOP_ITEM_NUM = 10;

public:
	EquipShopMgr();
	~EquipShopMgr();

	/**
	*@brief 设置、获得所有者
	*/
	void SetOwner(Player *pPlayer) { m_pOwner = pPlayer; }
	Player* GetOwner() const { return m_pOwner; }

	/**
	*@brief 购买装备
	*/
	void Buy(UInt8 uType, UInt8 uIndex);

public:	// 事件
	/**
	*@brief 上线
	*/
	void OnOnline();

	/**
	*@brief 心跳
	*/
	void OnTick(const Avalon::Time &now, int tickType);

	/**
	*@brief 升级
	*/
	void OnLevelUp();

public: //编解码
	void Input(Avalon::NetInputStream& stream);
	void Output(Avalon::NetOutputStream& stream);
	void DecodeString(std::istringstream& is);
	void EncodeString(std::ostringstream& os);

private:
	/**
	*@brief 刷新装备列表
	*/
	void RefreshEquipList();

	/**
	*@brief 同步装备列表
	*/
	void SyncEquipList();

private:
	// 所有者
	Player		*m_pOwner;
	// 上次刷新时间
	UInt32		m_uLastRefreshTime;
	// 普通区域商品
	std::list<CLProtocol::RandEquipEntry>	m_listNormal;
	// vip区域商品
	std::list<CLProtocol::RandEquipEntry>	m_listVip;
};

#endif

