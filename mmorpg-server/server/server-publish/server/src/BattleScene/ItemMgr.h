#ifndef _ITEM_MGR_H_
#define _ITEM_MGR_H_

#include "Package.h"
#include "CLObjectIndex.h"
#include <AvalonSimpleTimer.h>

class Player;

/**
 *@brief 道具管理器
 */
class ItemMgr
{
public:
	ItemMgr();
	~ItemMgr();

public:
	void Clear();

	/**
	 *@brief 设置获取拥有者
	 */
	void SetOwner(Player* owner) { m_pOwner = owner; }
	Player* GetOwner() const { return m_pOwner; }

	/** 
	 *@brief 获取包裹
	 */
	Package* GetPackage(UInt8 pack) const;

	/**
	*@brief 检测包裹容量是否足够
	*/
	bool CheckPackageCapacityEnough(UInt8 pack, UInt32 num) const;

	/**
	*@brief 清空包裹
	*/
	bool ClearPackage(UInt8 pack, UInt32 reasonId);

	/**
	 *@brief 移动道具
	 */
	UInt16 MoveItem(Item* item, UInt16 num, UInt8 destPackType);
	/**
	 *@brief 使用道具
	 */
	UInt32 UseItem(ObjID_t id, UInt16 num, UInt32 param1, UInt32 param2);
	UInt32 UseItem(UInt32 id);
	//for test
	void EquipItem(UInt32 id);

	/**
	 *@brief 查找道具
	 */
	Item* FindCarryOnItem(ObjID_t id);
	Item* FindItem(ObjID_t id);

	/**
	 *@brief 加载一个道具
	 */
	bool LoadItem(Item* item, bool newPos);

	//加载仓库道具
	bool LoadStorageItem(Item* item, bool newPos);

	/**
	 *@brief 添加和移除道具
	 */
	bool RemoveItem(Item* item);
	bool RemoveItemNoSync(Item* item);	//不存db和同步客户端

	// 检查能否添加道具
	bool CheckAddItem(UInt32 itemid, UInt16 num, ItemType itemType, UInt32 auctionCoolTime);

	// 得到给定数量的道具占用的格子数
	Int16 GetGridUsed(UInt32 itemid, UInt16 num, UInt32 auctionCoolTime);

	// 添加道具by itemid
	UInt16 AddItem(const char* reason, UInt32 itemid, UInt16 num, 
		UInt8 qualityLv, UInt8 strenth, ItemType itemType, UInt32 auctionCoolTimeStamp, bool notifyNewItem = true);
	UInt16 AddItemWithCallBack(const char* reason, UInt32 itemid, UInt16 num,
		UInt8 qualityLv, UInt8 strenth, ItemType itemType,
		const std::function<bool(Item&)>& initCallback,
		const std::function<void(Item&)>& makedCallback,
		UInt32 auctionCoolTime = 0);

	// 添加道具实例
	UInt16 AddItem(const char* reason, Item* item, UInt8 pack);
	
	UInt32 GetItemNum(UInt32 itemid, UInt8 pack, UInt8 bindType = 0);
	UInt32 GetItemNumByPackType(UInt32 itemid, PackType pack);
	UInt32 GetNumByPartAndColor(UInt32 subtype, UInt8 color, bool permanent, PackType pack);
	UInt16 RemoveItem(const char* reason, UInt32 itemid, UInt16 num, UInt8 pack);

	// 优先移除评分较低的
	UInt32 RemoveLessScoreItem(const char* reason, UInt32 itemid, UInt32 num, UInt8 pack);

	//added by aprilliu,2017.04.22 当装备积分变化或者获得装备时通知world server进行排行榜更新（当然如果可以引发排行榜变化时）
	void OnItemChangedSortList(Item* pItem);

	//删除所有指定ID的道具 返回值是删除的个数，-1为删除失败
	Int32 RemoveAllItemById(UInt32 itemid);

	//检查能否存仓库
	bool CheckAddStorage(Item* item, UInt16 num, UInt8 storageType = PACK_STORAGE);
	//检查能否一键存仓库(只有仓库里有的才存)
	bool CheckOneKeyAddStorage(UInt32 itemId, UInt32 auctionCoolTimeStamp);
	/**
	 *@brief 整理道具
	 */
	void TrimItems(UInt8 pack);
	
	//分解装备
	UInt32 DecomposeEquip(Item* item, bool sendMail);
	//强化装备
	UInt32 StrengthenEquip(Item* equip, bool useUnbreak, ObjID_t strTicket, UInt32& probability);
	float GetStrenthRate(Item* equip, EquipStrRateData* sucRateData);

	/**
	*@brief 取出仓库的全部道具
	*/
	void TakeOutAllFromStorage(UInt8 pack);

	// 穿上/脱下装备, 武器的时候 param 0:主武器, 1:副武器
	UInt32 ChangeEquip(Item* item, UInt32 param1 = 0);
	// 穿上/脱下时装
	UInt32 ChangeFashion(Item* item);

	// 根据等级和强化等级获取装备数量  by huchenhui 2016.09.13
	UInt32 GetEquipSize(UInt16 level, UInt8 quality, ItemSubType subType);

	// 根据等级获取传承装备数量		by huchenhui 2016.09.13
	UInt32 GetGreenEquipSize(UInt16 level);

	// 获取时装数量		by huchenhui 2018.03.20
	UInt32 GetFashionSize(UInt8 quality, ItemSubType subType);

	// 获取装备品质总和
	UInt32 GetEquipColorSum();

	// 道具type转背包type
	PackType GetPackType(ItemType itemType);

	//存取仓库,item不可以在后续用,有可能在接口里删除
	UInt16 PullStorage(Item* item, UInt16 num);
	UInt16 PushStorage(Item* item, UInt16 num, UInt8 storageType = PACK_STORAGE);

	// 装备封装
	UInt32 CheckSealEquip(Item* item, UInt16& needNum, UInt32& needId);
	UInt32 SealEquip(Item* item);

	// 装备重置品质等级
	UInt32 ReMakeEquipQlv(Item* item, UInt8 autoUsePoint);
	// 装备重置品质等级 使用完美洗练卷
	UInt32 ReMakeEquipQlvUsePerfect(Item* item); 
	
	// 装备转移
	UInt32 TransferEquip(Item* item, Item* transferStone);

	//装备回收
	//处理装备回收提交请求
	UInt32 handleEqRecSubcmt(std::vector<UInt64>& eqUids, std::vector<EqRecScoreItem>& scoreItems, UInt32& score);
	//装备赎回请求
	UInt32 handleEqRecRedeem(Item* item, UInt32& consScore);
	//装备回收积分提升请求
	UInt32 handleEqRecUpscore(Item* item, UInt32& upscore);
	//检查积分罐子是否可以打开
	UInt32 checkEqRecOpenJar(UInt32 jarId);
	//处理积分奖励罐子打开
	void handleEqRecOpenJar(UInt32 jarId);
	//计算装备回收刷新时间戳
	UInt64 getEqRecUpdateTimeStamp();
	//重置装备回收
	void resetEqRec(bool force = false);
	//刷新罐子状态
	void updateEqRecJarState();

	//附魔
	UInt32 AddMagic(Item* card, Item* item);
	UInt32 ComposeMagicCard(Item* card_1, Item* card_2, UInt32& cardId);

	// 添加宝珠
	UInt32 AddPreciousBead(Item* preciousBead, Item* item); //added by wenhao 2017.12.1
	// 宝珠镶嵌
	UInt32 MountPreciousBead(UInt64 preciousBeadUid, UInt64 itemUid, UInt8  holeIndex, UInt32& preciousBeadId);
	// 宝珠摘除
	UInt32 ExtirpePreciousBead(UInt64 itemUid, UInt8 holeIndex, UInt32 pestleid);
	// 宝珠升级
	UInt32 UpgradePreciousBead(UInt8 mountedType, ObjID_t precBeadGuid, ObjID_t eqGuid,
		UInt8 eqPrecHoleIndex, UInt32 precId, UInt32 consumePrecId,
		UInt32& upgradePrecId, UInt32& addBuffId, ObjID_t& newPrebeadUid);
	// 宝珠置换
	UInt32 ReplacePreciousBead(UInt64 preciousBeadUid, UInt64 itemUid, UInt8 holeIndex, UInt32& preciousBeadId);

	//时装合成
	UInt32 FashionCompose(Item* fashionA, Item* fashionB);
	FashionComposeData* FashionRand(FashionComposeMgr::FashionVec& datas);
	FashionComposeSkyData* FashionSkyCompose(UInt8 occu, UInt8 part);
	bool FashionSkyRand(FashionComposeSkyData* data, UInt32& hitWt);
	UInt32 FashionAttrSel(Item* fashion, UInt32 attrid);

	//时装合成新
	UInt32 FashionComposeNew(UInt32 suitId, UInt32 selComFashionId, UInt64 fashionAId, UInt64 fashionBId, UInt64 composerId);

	//时装节时装合成
	UInt32 FashionComposeChangeActive(UInt64 fashionId, UInt64 ticketId, UInt32 choiceComFashionId);

	//装备制作
	UInt32 EuqipMake(UInt32 equipId);

	//合并可叠加的道具
	void MergeItems(const ItemRewardVec& inItems, ItemRewardVec& outItems);

	//续费限时道具
	UInt32 RenewTimeItem(UInt64 uid, UInt32 durationDay);

	//自动配置药水
	UInt32 AutoSetPotion();

	//使用改名卡
	UInt32 UseChangeNameCard(UInt64 uid);

	//清空包裹缓存
	void ClearPackageChache(PackType packType);

	// 师傅赠送
	void MasterGiveEquip(std::vector<ObjID_t>& itemUids, Player* disciple);

	//计算装备分
	void CalculateValuedScore(Item* item);

	//装备升级
	UInt32 EuqieUpdate(ObjID_t equipUid);

private:
	bool CheckMasterGiveEquip(std::vector<ObjID_t>& itemUids, Player* disciple, std::vector<Item*>& outItems);
	void SendMasterGiveMail(std::vector<Item*>& items, ObjID_t recvId);

private:	//时装合成
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
public://事件
	/**
	*@brief 心跳
	*/
	void OnHeartbeat(const Avalon::Time &now, int tickType);

	/**
	 *@brief 下线
	 */
	void OnOffline();

	/**
	 *@brief 改天
	 */
	void OnDayChanged();
	void OnLogicDayChanged();

public://同步相关
	/**
	 *@brief 同步到客户端
	 */
	void OnOnline(bool bDayChanged);

	/**
	 *@brief 同步单个道具到客户端
	 */
	void NotifyDelItem(Item* item);
	void NotifyNewItem(Item* item);

	/**
	 *@brief 同步装备到观察者
	 */
	void SyncEquipsToWatcher(ObjID_t watcher);

	//批量同步道具更新
	void SyncItemBatch(std::vector<Item*> items);

public:
	/**
	 *@brief 访问所有道具
	 */
	void VisitItems(ItemVisitor& visitor);

	/**
	 *@brief 访问指定包裹道具
	 */
	void VisitItems(UInt8 pack, ItemVisitor& visitor);

	//适配品级调整老数据
	void AdaptQLData(Item* item);

public:
	/*
	*@breif 道具检查
	*/
	static void CheckItem(UInt32 serialId, UInt8 type, ObjID_t owerId, ObjID_t itemId, UInt64 param);

	static void CheckItem(UInt32 serialId, UInt8 type, Player* player, ObjID_t itemId, UInt64 param);

	static void OnCheckItemFail(UInt32 errCode, UInt32 serialId);

public:
	/*
	*@breif 吃鸡
	*/
	UInt32 ChoiceChiJiEquip(UInt32 equipId);

private:
	/**
	*@brief 发送物品相关系统邮件
	*/
	void SendItemSysMail(ObjID_t recvId, UInt32 itemId, UInt32 notifyId, std::string title);

	/**
	 *@brief 加道具到指定位置
	 */
	bool AddItem(Item* item, const ItemPos pos, bool notifyNewItem = true);

	// 检查能否装备
	UInt32 CheckWearEquip(Item* item);

	//使用强化券
	UInt32 UseStrTicket(Item* equip, Item* ticket, UInt32& probability);


	//随机装备品级,方案1,全随均分每条属性
	bool RandEquipQLA(Item* item);

	//随机装备品级,方案2,单随每条属性
	bool RandEquipQLB(Item* item);
	//随机品级
	UInt8 RandQL(EquipQLRandData* randData);
	//适配时装属性老数据
	bool AdaptFashionAttr(Item* item);
	//设置属性表
	void SetAttrData(Item* item);

	//检查道具日使用数量
	UInt32 CheckItemDayUseNum(Item* item, bool& isDayUseLimite, std::string& counterName);

	//道具使用后事件
	void OnUseItemSuc(ItemDataEntry* data, bool isDayUseLimite, std::string& counterName);

	//使用礼包
	UInt32 UseGiftPack(Item* item, UInt32 sel, RewardGroup& rewardGroup);

	//检查礼包是否能使用
	UInt32 CheckGiftCanUse(Item* item);

	//自动找合适的药水
	UInt32 AutoFindPotion(UInt32 slot1, UInt32 slot2, UInt32 slot3, UInt32 fitType);
	//下一个优先级的药水类型
	UInt32 GetNextPotionType(UInt32 type);

	void CheckTimeItem(bool bDayChange, bool bOnline);
	
	void SendItemLoginUdpLog();

public://编解码
		
	void Input(Avalon::NetInputStream& stream);
	void Output(Avalon::NetOutputStream& stream);

#ifdef _DEBUG
	public: 
		inline  void SetEquipQl(bool b) { m_debugEquipQl = b; }
#endif

	void ClearMailGroup();

private:

	//拥有者
	Player*		m_pOwner;
	//包裹
	Package*	m_pPackages[PACK_MAX];
	//道具列表
	CLIDObjIndex<Item>	m_Items;
	//时限道具删除时间(s)
	UInt32		m_timeItemDeleteIntervalTime;

	//超量需要发邮件的仓库道具
	std::vector<Item*> m_MailGroup;

	//吃鸡装备选择列表
	std::vector<UInt32> m_ChiJiEquipVec;

#ifdef _DEBUG
	bool		m_debugEquipQl;

#endif 

};

#endif
