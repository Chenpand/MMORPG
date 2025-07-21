#ifndef _ITEM_MGR_H_
#define _ITEM_MGR_H_

#include "Package.h"
#include "CLObjectIndex.h"
#include <AvalonSimpleTimer.h>

class Player;

/**
 *@brief ���߹�����
 */
class ItemMgr
{
public:
	ItemMgr();
	~ItemMgr();

public:
	void Clear();

	/**
	 *@brief ���û�ȡӵ����
	 */
	void SetOwner(Player* owner) { m_pOwner = owner; }
	Player* GetOwner() const { return m_pOwner; }

	/** 
	 *@brief ��ȡ����
	 */
	Package* GetPackage(UInt8 pack) const;

	/**
	*@brief �����������Ƿ��㹻
	*/
	bool CheckPackageCapacityEnough(UInt8 pack, UInt32 num) const;

	/**
	*@brief ��հ���
	*/
	bool ClearPackage(UInt8 pack, UInt32 reasonId);

	/**
	 *@brief �ƶ�����
	 */
	UInt16 MoveItem(Item* item, UInt16 num, UInt8 destPackType);
	/**
	 *@brief ʹ�õ���
	 */
	UInt32 UseItem(ObjID_t id, UInt16 num, UInt32 param1, UInt32 param2);
	UInt32 UseItem(UInt32 id);
	//for test
	void EquipItem(UInt32 id);

	/**
	 *@brief ���ҵ���
	 */
	Item* FindCarryOnItem(ObjID_t id);
	Item* FindItem(ObjID_t id);

	/**
	 *@brief ����һ������
	 */
	bool LoadItem(Item* item, bool newPos);

	//���زֿ����
	bool LoadStorageItem(Item* item, bool newPos);

	/**
	 *@brief ��Ӻ��Ƴ�����
	 */
	bool RemoveItem(Item* item);
	bool RemoveItemNoSync(Item* item);	//����db��ͬ���ͻ���

	// ����ܷ���ӵ���
	bool CheckAddItem(UInt32 itemid, UInt16 num, ItemType itemType, UInt32 auctionCoolTime);

	// �õ����������ĵ���ռ�õĸ�����
	Int16 GetGridUsed(UInt32 itemid, UInt16 num, UInt32 auctionCoolTime);

	// ��ӵ���by itemid
	UInt16 AddItem(const char* reason, UInt32 itemid, UInt16 num, 
		UInt8 qualityLv, UInt8 strenth, ItemType itemType, UInt32 auctionCoolTimeStamp, bool notifyNewItem = true);
	UInt16 AddItemWithCallBack(const char* reason, UInt32 itemid, UInt16 num,
		UInt8 qualityLv, UInt8 strenth, ItemType itemType,
		const std::function<bool(Item&)>& initCallback,
		const std::function<void(Item&)>& makedCallback,
		UInt32 auctionCoolTime = 0);

	// ��ӵ���ʵ��
	UInt16 AddItem(const char* reason, Item* item, UInt8 pack);
	
	UInt32 GetItemNum(UInt32 itemid, UInt8 pack, UInt8 bindType = 0);
	UInt32 GetItemNumByPackType(UInt32 itemid, PackType pack);
	UInt32 GetNumByPartAndColor(UInt32 subtype, UInt8 color, bool permanent, PackType pack);
	UInt16 RemoveItem(const char* reason, UInt32 itemid, UInt16 num, UInt8 pack);

	// �����Ƴ����ֽϵ͵�
	UInt32 RemoveLessScoreItem(const char* reason, UInt32 itemid, UInt32 num, UInt8 pack);

	//added by aprilliu,2017.04.22 ��װ�����ֱ仯���߻��װ��ʱ֪ͨworld server�������а���£���Ȼ��������������а�仯ʱ��
	void OnItemChangedSortList(Item* pItem);

	//ɾ������ָ��ID�ĵ��� ����ֵ��ɾ���ĸ�����-1Ϊɾ��ʧ��
	Int32 RemoveAllItemById(UInt32 itemid);

	//����ܷ��ֿ�
	bool CheckAddStorage(Item* item, UInt16 num, UInt8 storageType = PACK_STORAGE);
	//����ܷ�һ����ֿ�(ֻ�вֿ����еĲŴ�)
	bool CheckOneKeyAddStorage(UInt32 itemId, UInt32 auctionCoolTimeStamp);
	/**
	 *@brief �������
	 */
	void TrimItems(UInt8 pack);
	
	//�ֽ�װ��
	UInt32 DecomposeEquip(Item* item, bool sendMail);
	//ǿ��װ��
	UInt32 StrengthenEquip(Item* equip, bool useUnbreak, ObjID_t strTicket, UInt32& probability);
	float GetStrenthRate(Item* equip, EquipStrRateData* sucRateData);

	/**
	*@brief ȡ���ֿ��ȫ������
	*/
	void TakeOutAllFromStorage(UInt8 pack);

	// ����/����װ��, ������ʱ�� param 0:������, 1:������
	UInt32 ChangeEquip(Item* item, UInt32 param1 = 0);
	// ����/����ʱװ
	UInt32 ChangeFashion(Item* item);

	// ���ݵȼ���ǿ���ȼ���ȡװ������  by huchenhui 2016.09.13
	UInt32 GetEquipSize(UInt16 level, UInt8 quality, ItemSubType subType);

	// ���ݵȼ���ȡ����װ������		by huchenhui 2016.09.13
	UInt32 GetGreenEquipSize(UInt16 level);

	// ��ȡʱװ����		by huchenhui 2018.03.20
	UInt32 GetFashionSize(UInt8 quality, ItemSubType subType);

	// ��ȡװ��Ʒ���ܺ�
	UInt32 GetEquipColorSum();

	// ����typeת����type
	PackType GetPackType(ItemType itemType);

	//��ȡ�ֿ�,item�������ں�����,�п����ڽӿ���ɾ��
	UInt16 PullStorage(Item* item, UInt16 num);
	UInt16 PushStorage(Item* item, UInt16 num, UInt8 storageType = PACK_STORAGE);

	// װ����װ
	UInt32 CheckSealEquip(Item* item, UInt16& needNum, UInt32& needId);
	UInt32 SealEquip(Item* item);

	// װ������Ʒ�ʵȼ�
	UInt32 ReMakeEquipQlv(Item* item, UInt8 autoUsePoint);
	// װ������Ʒ�ʵȼ� ʹ������ϴ����
	UInt32 ReMakeEquipQlvUsePerfect(Item* item); 
	
	// װ��ת��
	UInt32 TransferEquip(Item* item, Item* transferStone);

	//װ������
	//����װ�������ύ����
	UInt32 handleEqRecSubcmt(std::vector<UInt64>& eqUids, std::vector<EqRecScoreItem>& scoreItems, UInt32& score);
	//װ���������
	UInt32 handleEqRecRedeem(Item* item, UInt32& consScore);
	//װ�����ջ�����������
	UInt32 handleEqRecUpscore(Item* item, UInt32& upscore);
	//�����ֹ����Ƿ���Դ�
	UInt32 checkEqRecOpenJar(UInt32 jarId);
	//������ֽ������Ӵ�
	void handleEqRecOpenJar(UInt32 jarId);
	//����װ������ˢ��ʱ���
	UInt64 getEqRecUpdateTimeStamp();
	//����װ������
	void resetEqRec(bool force = false);
	//ˢ�¹���״̬
	void updateEqRecJarState();

	//��ħ
	UInt32 AddMagic(Item* card, Item* item);
	UInt32 ComposeMagicCard(Item* card_1, Item* card_2, UInt32& cardId);

	// ��ӱ���
	UInt32 AddPreciousBead(Item* preciousBead, Item* item); //added by wenhao 2017.12.1
	// ������Ƕ
	UInt32 MountPreciousBead(UInt64 preciousBeadUid, UInt64 itemUid, UInt8  holeIndex, UInt32& preciousBeadId);
	// ����ժ��
	UInt32 ExtirpePreciousBead(UInt64 itemUid, UInt8 holeIndex, UInt32 pestleid);
	// ��������
	UInt32 UpgradePreciousBead(UInt8 mountedType, ObjID_t precBeadGuid, ObjID_t eqGuid,
		UInt8 eqPrecHoleIndex, UInt32 precId, UInt32 consumePrecId,
		UInt32& upgradePrecId, UInt32& addBuffId, ObjID_t& newPrebeadUid);
	// �����û�
	UInt32 ReplacePreciousBead(UInt64 preciousBeadUid, UInt64 itemUid, UInt8 holeIndex, UInt32& preciousBeadId);

	//ʱװ�ϳ�
	UInt32 FashionCompose(Item* fashionA, Item* fashionB);
	FashionComposeData* FashionRand(FashionComposeMgr::FashionVec& datas);
	FashionComposeSkyData* FashionSkyCompose(UInt8 occu, UInt8 part);
	bool FashionSkyRand(FashionComposeSkyData* data, UInt32& hitWt);
	UInt32 FashionAttrSel(Item* fashion, UInt32 attrid);

	//ʱװ�ϳ���
	UInt32 FashionComposeNew(UInt32 suitId, UInt32 selComFashionId, UInt64 fashionAId, UInt64 fashionBId, UInt64 composerId);

	//ʱװ��ʱװ�ϳ�
	UInt32 FashionComposeChangeActive(UInt64 fashionId, UInt64 ticketId, UInt32 choiceComFashionId);

	//װ������
	UInt32 EuqipMake(UInt32 equipId);

	//�ϲ��ɵ��ӵĵ���
	void MergeItems(const ItemRewardVec& inItems, ItemRewardVec& outItems);

	//������ʱ����
	UInt32 RenewTimeItem(UInt64 uid, UInt32 durationDay);

	//�Զ�����ҩˮ
	UInt32 AutoSetPotion();

	//ʹ�ø�����
	UInt32 UseChangeNameCard(UInt64 uid);

	//��հ�������
	void ClearPackageChache(PackType packType);

	// ʦ������
	void MasterGiveEquip(std::vector<ObjID_t>& itemUids, Player* disciple);

	//����װ����
	void CalculateValuedScore(Item* item);

	//װ������
	UInt32 EuqieUpdate(ObjID_t equipUid);

private:
	bool CheckMasterGiveEquip(std::vector<ObjID_t>& itemUids, Player* disciple, std::vector<Item*>& outItems);
	void SendMasterGiveMail(std::vector<Item*>& items, ObjID_t recvId);

private:	//ʱװ�ϳ�
	UInt32 _CheckFashionCmpsParam(Item* fashionA, Item* fashionB, Item* composer, UInt32 selFashionId);
	UInt32 _CreateFahsionCmpsRate(Item* fashionA, Item* fashionB, Item* composer, UInt32& rate, UInt32& prob);
	UInt32 _GetFashionCmpsItem(UInt32 skyProb, UInt32 suitId, UInt32 selComFashionId, UInt8 part, UInt32& getSkyFashionId, bool isL);
	void _FilterAlreadyGetSky(FashionComposeSkyMgr::FashionSkyVec& inSkys, FashionComposeSkyMgr::FashionSkyVec& outRealSkys);
	UInt32 _SetSkySuitNum(UInt32 skyFashionId);
	bool _CanGetSuitWing(UInt32 suitId, UInt8 type);
	UInt32 _CheckFashionCmpsCapacity();
	void _SendFashionAnnounce(UInt32 suitId, UInt32 param);
	void _FixWeight(std::vector<UInt32>& wt, FashionComposeSkyData* data);

	bool _IsFashionCmpsOpen();
public://�¼�
	/**
	*@brief ����
	*/
	void OnHeartbeat(const Avalon::Time &now, int tickType);

	/**
	 *@brief ����
	 */
	void OnOffline();

	/**
	 *@brief ����
	 */
	void OnDayChanged();
	void OnLogicDayChanged();

public://ͬ�����
	/**
	 *@brief ͬ�����ͻ���
	 */
	void OnOnline(bool bDayChanged);

	/**
	 *@brief ͬ���������ߵ��ͻ���
	 */
	void NotifyDelItem(Item* item);
	void NotifyNewItem(Item* item);

	/**
	 *@brief ͬ��װ�����۲���
	 */
	void SyncEquipsToWatcher(ObjID_t watcher);

	//����ͬ�����߸���
	void SyncItemBatch(std::vector<Item*> items);

public:
	/**
	 *@brief �������е���
	 */
	void VisitItems(ItemVisitor& visitor);

	/**
	 *@brief ����ָ����������
	 */
	void VisitItems(UInt8 pack, ItemVisitor& visitor);

	//����Ʒ������������
	void AdaptQLData(Item* item);

public:
	/*
	*@breif ���߼��
	*/
	static void CheckItem(UInt32 serialId, UInt8 type, ObjID_t owerId, ObjID_t itemId, UInt64 param);

	static void CheckItem(UInt32 serialId, UInt8 type, Player* player, ObjID_t itemId, UInt64 param);

	static void OnCheckItemFail(UInt32 errCode, UInt32 serialId);

public:
	/*
	*@breif �Լ�
	*/
	UInt32 ChoiceChiJiEquip(UInt32 equipId);

private:
	/**
	*@brief ������Ʒ���ϵͳ�ʼ�
	*/
	void SendItemSysMail(ObjID_t recvId, UInt32 itemId, UInt32 notifyId, std::string title);

	/**
	 *@brief �ӵ��ߵ�ָ��λ��
	 */
	bool AddItem(Item* item, const ItemPos pos, bool notifyNewItem = true);

	// ����ܷ�װ��
	UInt32 CheckWearEquip(Item* item);

	//ʹ��ǿ��ȯ
	UInt32 UseStrTicket(Item* equip, Item* ticket, UInt32& probability);


	//���װ��Ʒ��,����1,ȫ�����ÿ������
	bool RandEquipQLA(Item* item);

	//���װ��Ʒ��,����2,����ÿ������
	bool RandEquipQLB(Item* item);
	//���Ʒ��
	UInt8 RandQL(EquipQLRandData* randData);
	//����ʱװ����������
	bool AdaptFashionAttr(Item* item);
	//�������Ա�
	void SetAttrData(Item* item);

	//��������ʹ������
	UInt32 CheckItemDayUseNum(Item* item, bool& isDayUseLimite, std::string& counterName);

	//����ʹ�ú��¼�
	void OnUseItemSuc(ItemDataEntry* data, bool isDayUseLimite, std::string& counterName);

	//ʹ�����
	UInt32 UseGiftPack(Item* item, UInt32 sel, RewardGroup& rewardGroup);

	//�������Ƿ���ʹ��
	UInt32 CheckGiftCanUse(Item* item);

	//�Զ��Һ��ʵ�ҩˮ
	UInt32 AutoFindPotion(UInt32 slot1, UInt32 slot2, UInt32 slot3, UInt32 fitType);
	//��һ�����ȼ���ҩˮ����
	UInt32 GetNextPotionType(UInt32 type);

	void CheckTimeItem(bool bDayChange, bool bOnline);
	
	void SendItemLoginUdpLog();

public://�����
		
	void Input(Avalon::NetInputStream& stream);
	void Output(Avalon::NetOutputStream& stream);

#ifdef _DEBUG
	public: 
		inline  void SetEquipQl(bool b) { m_debugEquipQl = b; }
#endif

	void ClearMailGroup();

private:

	//ӵ����
	Player*		m_pOwner;
	//����
	Package*	m_pPackages[PACK_MAX];
	//�����б�
	CLIDObjIndex<Item>	m_Items;
	//ʱ�޵���ɾ��ʱ��(s)
	UInt32		m_timeItemDeleteIntervalTime;

	//������Ҫ���ʼ��Ĳֿ����
	std::vector<Item*> m_MailGroup;

	//�Լ�װ��ѡ���б�
	std::vector<UInt32> m_ChiJiEquipVec;

#ifdef _DEBUG
	bool		m_debugEquipQl;

#endif 

};

#endif
