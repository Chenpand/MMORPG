#ifndef _WS_SHOP_MGR_H_
#define _WS_SHOP_MGR_H_

#include <AvalonSingleton.h>
#include <CLShopProtocol.h>
#include "WSShop.h"
#include "WSShopItemBuyRecord.h"

class CLRecordCallback;
class ShopDataEntry;
class AccountShopItemDataEntry;
class WSPlayer;


class WSShopMgr : public Avalon::Singleton<WSShopMgr>
{
public:
	WSShopMgr();
	~WSShopMgr();

	bool Init();

public:
	/*
	*@brief 从数据库加载商品
	*/
	void LoadShopItemFromDB(CLRecordCallback* callback);

	/*
	*@brief 初始化玩家账号商店
	*/
	void InitPlayerAccountShopByTableData(WSPlayer* player);
	bool InitPlayerAccountShopByTableData(WSPlayer* player, UInt32 shopId);

	/*
	*@brief 查找商店
	*/
	WSShop* FindShop(UInt32 accid, UInt32 shopid);

	/*
	*@brief 添加商店
	*/
	void AddShop(UInt32 accid, WSShop* shop);

public:
	/*
	*@brief 从数据库加载账号购买记录
	*/
	void LoadShopItemAccountBuyRecordFromDB(CLRecordCallback* callback);

	/*
	*@brief 查找商品账号购买记录
	*/
	WSShopItemAccountBuyRecord* FindShopItemAccountBuyRecord(UInt32 accid, UInt32 shopItemId);
	WSShopItemAccountBuyRecord* FindShopItemAccountBuyRecord(const WSShopItemAccountBuyIndex& index);

	/*
	*@brief 添加商品账号购买记录
	*/
	void AddShopItemAccountBuyRecord(const WSShopItemAccountBuyIndex& index, WSShopItemAccountBuyRecord* record);
	void AddShopItemAccountBuyRecord(UInt32 accid, UInt32 shopItemId, UInt32 buyNum);

public:
	/*
	*@brief 从数据库加载角色购买记录
	*/
	void LoadShopItemRoleBuyRecordFromDB(CLRecordCallback* callback);

	/*
	*@brief 查找商品角色购买记录
	*/
	WSShopItemRoleBuyRecord* FindShopItemRoleBuyRecord(ObjID_t roleid, UInt32 shopItemId);
	WSShopItemRoleBuyRecord* FindShopItemRoleBuyRecord(const WSShopItemRoleBuyIndex& index);

	/*
	*@brief 添加商品角色购买记录
	*/
	void AddShopItemRoleBuyRecord(const WSShopItemRoleBuyIndex& index, WSShopItemRoleBuyRecord* record);
	void AddShopItemRoleBuyRecord(ObjID_t roleid, UInt32 shopItemId, UInt32 buyNum);

public:
	/*
	*@brief 玩家上线
	*/
	void OnPlayerOnline(WSPlayer* player);

public:
	/*
	*@brief 玩家查询商品
	*/
	void OnPlayerQueryShopItem(WSPlayer* player, const CLProtocol::WorldAccountShopItemQueryReq& req);

	/*
	*@brief 玩家购买商品
	*/
	void OnPlayerBuyShopItem(WSPlayer* player, const CLProtocol::WorldAccountShopItemBuyReq& req);

public:
	/*
	*@brief 检查剩余购买数量足够
	*/
	bool IsRestBuyNumEnough(WSPlayer* player, WSShopItem* shopItem, const AccountShopItemDataEntry* shopItemData, UInt32 buyNum);

	/*
	*@brief 发送玩家购买商品返回
	*/
	void SendPlayerBuyShopItemResponse(WSPlayer* player, const CLProtocol::WorldAccountShopItemBuyReq& req, UInt32 errorCode);

	/*
	*@brief 玩家购买商品成功
	*/
	void OnPlayerBuyShopItemSuccess(WSPlayer* player, ShopType	shopType, UInt32 shopId, UInt32 shopItemId, UInt32 buyNum);

	/*
	*@brief 玩家购买商品成功
	*/
	bool OnCheckShopBuyRemoveCostFromSceneCallback(WSPlayer* player, const CLProtocol::WorldAccountShopItemBuyReq& req);
	
public:
	/*
	*@brief 转换为协议信息
	*/
	void TransToProtocolInfo(WSPlayer* player, const AccountShopItemDataEntry* shopItemData, CLProtocol::AccountShopItemInfo& shopItemInfo);

private:
	/*
	*@brief 玩家购买运营活动商店商品
	*/
	UInt32 _OnPlayerBuyOpActivityShopItem(WSPlayer* player, WSShop* shop, const ShopDataEntry* shopData, const AccountShopItemDataEntry* shopItemData, UInt32 buyNum);

	/*
	*@brief 检查运营活动商店条件
	*/
	UInt32 _CheckOpActivityShopCond(WSPlayer* player, UInt32 opActType);

	/*
	*@brief 获取账号下剩余可购买数量
	*/
	UInt32 _GetAccountRestBuyNum(UInt32 accid, const AccountShopItemDataEntry* shopItemData);

	/*
	*@brief 获取账号购买记录下次刷新时间
	*/
	UInt32 _GetAccountBuyRecordNextRefrshTime(UInt32 accid, const AccountShopItemDataEntry* shopItemData);

	/*
	*@brief 获取角色下剩余可购买数量
	*/
	UInt32 _GetRoleRestBuyNum(ObjID_t roleid, const AccountShopItemDataEntry* shopItemData);

	/*
	*@brief 获取角色购买记录下次刷新时间
	*/
	UInt32 _GetRoleBuyRecordNextRefreshTime(ObjID_t roleid, const AccountShopItemDataEntry* shopItemData);

	/*
	*@brief 购买检查
	*/
	UInt32 _CheckBuy(WSPlayer* player, const CLProtocol::WorldAccountShopItemBuyReq& req, const ShopDataEntry* shopData, AccountShopItemDataEntry* shopItemData, const WSShop* shop, WSShopItem* shopItem);

private:

	HashMap<UInt32, std::vector<WSShop*> > m_ShopMap;

	// 账号下商品购买记录
	std::map<WSShopItemAccountBuyIndex, WSShopItemAccountBuyRecord*> m_AccountShopItemBuyRecords;
	// 角色下商品购买记录
	std::map<WSShopItemRoleBuyIndex, WSShopItemRoleBuyRecord*> m_RoleShopItemBuyRecords;
};


#endif