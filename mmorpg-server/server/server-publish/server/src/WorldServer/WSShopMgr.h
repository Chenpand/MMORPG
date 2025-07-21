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
	*@brief �����ݿ������Ʒ
	*/
	void LoadShopItemFromDB(CLRecordCallback* callback);

	/*
	*@brief ��ʼ������˺��̵�
	*/
	void InitPlayerAccountShopByTableData(WSPlayer* player);
	bool InitPlayerAccountShopByTableData(WSPlayer* player, UInt32 shopId);

	/*
	*@brief �����̵�
	*/
	WSShop* FindShop(UInt32 accid, UInt32 shopid);

	/*
	*@brief ����̵�
	*/
	void AddShop(UInt32 accid, WSShop* shop);

public:
	/*
	*@brief �����ݿ�����˺Ź����¼
	*/
	void LoadShopItemAccountBuyRecordFromDB(CLRecordCallback* callback);

	/*
	*@brief ������Ʒ�˺Ź����¼
	*/
	WSShopItemAccountBuyRecord* FindShopItemAccountBuyRecord(UInt32 accid, UInt32 shopItemId);
	WSShopItemAccountBuyRecord* FindShopItemAccountBuyRecord(const WSShopItemAccountBuyIndex& index);

	/*
	*@brief �����Ʒ�˺Ź����¼
	*/
	void AddShopItemAccountBuyRecord(const WSShopItemAccountBuyIndex& index, WSShopItemAccountBuyRecord* record);
	void AddShopItemAccountBuyRecord(UInt32 accid, UInt32 shopItemId, UInt32 buyNum);

public:
	/*
	*@brief �����ݿ���ؽ�ɫ�����¼
	*/
	void LoadShopItemRoleBuyRecordFromDB(CLRecordCallback* callback);

	/*
	*@brief ������Ʒ��ɫ�����¼
	*/
	WSShopItemRoleBuyRecord* FindShopItemRoleBuyRecord(ObjID_t roleid, UInt32 shopItemId);
	WSShopItemRoleBuyRecord* FindShopItemRoleBuyRecord(const WSShopItemRoleBuyIndex& index);

	/*
	*@brief �����Ʒ��ɫ�����¼
	*/
	void AddShopItemRoleBuyRecord(const WSShopItemRoleBuyIndex& index, WSShopItemRoleBuyRecord* record);
	void AddShopItemRoleBuyRecord(ObjID_t roleid, UInt32 shopItemId, UInt32 buyNum);

public:
	/*
	*@brief �������
	*/
	void OnPlayerOnline(WSPlayer* player);

public:
	/*
	*@brief ��Ҳ�ѯ��Ʒ
	*/
	void OnPlayerQueryShopItem(WSPlayer* player, const CLProtocol::WorldAccountShopItemQueryReq& req);

	/*
	*@brief ��ҹ�����Ʒ
	*/
	void OnPlayerBuyShopItem(WSPlayer* player, const CLProtocol::WorldAccountShopItemBuyReq& req);

public:
	/*
	*@brief ���ʣ�๺�������㹻
	*/
	bool IsRestBuyNumEnough(WSPlayer* player, WSShopItem* shopItem, const AccountShopItemDataEntry* shopItemData, UInt32 buyNum);

	/*
	*@brief ������ҹ�����Ʒ����
	*/
	void SendPlayerBuyShopItemResponse(WSPlayer* player, const CLProtocol::WorldAccountShopItemBuyReq& req, UInt32 errorCode);

	/*
	*@brief ��ҹ�����Ʒ�ɹ�
	*/
	void OnPlayerBuyShopItemSuccess(WSPlayer* player, ShopType	shopType, UInt32 shopId, UInt32 shopItemId, UInt32 buyNum);

	/*
	*@brief ��ҹ�����Ʒ�ɹ�
	*/
	bool OnCheckShopBuyRemoveCostFromSceneCallback(WSPlayer* player, const CLProtocol::WorldAccountShopItemBuyReq& req);
	
public:
	/*
	*@brief ת��ΪЭ����Ϣ
	*/
	void TransToProtocolInfo(WSPlayer* player, const AccountShopItemDataEntry* shopItemData, CLProtocol::AccountShopItemInfo& shopItemInfo);

private:
	/*
	*@brief ��ҹ�����Ӫ��̵���Ʒ
	*/
	UInt32 _OnPlayerBuyOpActivityShopItem(WSPlayer* player, WSShop* shop, const ShopDataEntry* shopData, const AccountShopItemDataEntry* shopItemData, UInt32 buyNum);

	/*
	*@brief �����Ӫ��̵�����
	*/
	UInt32 _CheckOpActivityShopCond(WSPlayer* player, UInt32 opActType);

	/*
	*@brief ��ȡ�˺���ʣ��ɹ�������
	*/
	UInt32 _GetAccountRestBuyNum(UInt32 accid, const AccountShopItemDataEntry* shopItemData);

	/*
	*@brief ��ȡ�˺Ź����¼�´�ˢ��ʱ��
	*/
	UInt32 _GetAccountBuyRecordNextRefrshTime(UInt32 accid, const AccountShopItemDataEntry* shopItemData);

	/*
	*@brief ��ȡ��ɫ��ʣ��ɹ�������
	*/
	UInt32 _GetRoleRestBuyNum(ObjID_t roleid, const AccountShopItemDataEntry* shopItemData);

	/*
	*@brief ��ȡ��ɫ�����¼�´�ˢ��ʱ��
	*/
	UInt32 _GetRoleBuyRecordNextRefreshTime(ObjID_t roleid, const AccountShopItemDataEntry* shopItemData);

	/*
	*@brief ������
	*/
	UInt32 _CheckBuy(WSPlayer* player, const CLProtocol::WorldAccountShopItemBuyReq& req, const ShopDataEntry* shopData, AccountShopItemDataEntry* shopItemData, const WSShop* shop, WSShopItem* shopItem);

private:

	HashMap<UInt32, std::vector<WSShop*> > m_ShopMap;

	// �˺�����Ʒ�����¼
	std::map<WSShopItemAccountBuyIndex, WSShopItemAccountBuyRecord*> m_AccountShopItemBuyRecords;
	// ��ɫ����Ʒ�����¼
	std::map<WSShopItemRoleBuyIndex, WSShopItemRoleBuyRecord*> m_RoleShopItemBuyRecords;
};


#endif