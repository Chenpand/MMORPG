#ifndef _ITEM_H_
#define _ITEM_H_

#include <CLItemDefine.h>
#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <CLItemDataEntry.h>
#include <CLSceneObject.h>
#include <CLItemProperty.h>
#include <CLMailDefine.h>

class Player;

#define _GETSCORE(type)	EquipValueSceorDataMgr::Instance()->FindEntry(type)		

#define SET_BASE_PROP(value, baseRatio, perRatio, func) \
	Int32 base = (Int32)(value * baseRatio + value * (1 - baseRatio) * fQlRate); \
	Int32 perfect = (Int32)(bPerfect * value * perRatio); \
	if (bPerfect) { perfect = (perfect == 0) ? 1 : perfect; }	\
	if (perfect > 0 && value < 0) { perfect *= -1; } \
	func(base + perfect);

#define SET_FOURD_PROP(value, baseRatio, perRatio, func) \
	UInt32 base = (UInt32)(value * baseRatio + value * (1 - baseRatio) * fQlRate); \
	UInt32 perfect = (UInt32)(bPerfect * value * perRatio); \
	if (bPerfect) { perfect = (perfect < 1000) ? 1000 : perfect; } \
	UInt32 assitEqAdd = GetFourDimenAddition();\
	func(base + perfect + assitEqAdd);

#define SET_EX_PROP(value, baseRatio, perRatio, func, type) \
	Int32 base = (Int32)(value * baseRatio + value * (1 - baseRatio) * fQlRate); \
	Int32 perfect = (Int32)(bPerfect * value * perRatio); \
	if (bPerfect) { perfect = (perfect == 0) ? 1 : perfect; }	\
	if (perfect > 0 && value < 0) { perfect *= -1; } \
	func(type, base + perfect);

/**
 *@brief 道具
 */
class Item : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(Item, Avalon::NullMutex)

public:
	Item();
	~Item();
	void CopyFrom(Item& other);
	Item(Item& other);
	Item& operator=(Item& other);

public:
	/**
	 *@brief 设置数据项
	 */
	void SetDataEntry(ItemDataEntry* dataEntry)
	{ 
		m_pDataEntry = dataEntry;
		SetSubType(dataEntry->subType);
	}
	void SetAttrDataEntry(EquipAttrDataEntry* data)	{ m_pAttrData = data; }

	ItemDataEntry* GetDataEntry() const { return m_pDataEntry; }
	EquipAttrDataEntry* GetAttrDataEntry() const { return m_pAttrData; };

	void SetAttrDataEntryInherent(EquipAttrDataEntry* data) { m_pAttrDataInherent = data; }
	EquipAttrDataEntry* GetAttrDataEntryInherent() const { return m_pAttrDataInherent; };

	void SetFashionAttrInherentId();
	inline UInt32 GetFashionAttrInherentId() { return m_AttrDataInherentId; }

	/**
	 *@brief 获取数据表id
	 */
	void SetDataID(UInt32 id){ 
		m_DataId = id; 
		m_DataId.ClearDirty(0xFFFFFFFF);
	}

	void ChangeDataID(UInt32 id){
		m_DataId = id;
	}

	UInt32 GetDataID() const { return m_DataId; }

	void SetSubType(UInt8 subType)
	{
		m_SubType = subType;
	}

	/**
	*@brief 检查更新道具失效
	*/
	void CheckTimeOut(UInt32 deleteIntervalTime);

	//判断道具失效,失效时间到了,没到删除时间
	bool CheckInvalid();

	/**
	 *@brief 获取道具类型
	 */
	UInt8 GetType() const { return m_pDataEntry->type; }
	UInt8 GetSubType() const{ return m_pDataEntry->subType; }
	UInt32 GetThirdType() const { return m_pDataEntry->thirdType; }
	UInt32 GetEquipGrid();

	/** 
	 *@brief 获取装备需要等级
	 */
	UInt16 GetNeedLevel() const{ return m_pDataEntry->needLevel; }

	//added by aprilliu,2017.04.22
	UInt32 GetValuedScore() const { return m_ValuedScore; }

	// 判断是否符合职业
	inline bool FitOccu(UInt8 oc) { return m_pDataEntry->occu.test(oc) || m_pDataEntry->occu.test(oc - oc % 10) ; }
	//bool FitOccu(UInt8 oc);
	
	/**
	 *@brief 设置装备基础数值
	 */
	//void SetupEquipBase();

	/**
	 *@brief 检查合并，返回可合并数量
	 *@param num 道具数量
	 */
	UInt16 CheckMerging(UInt16 num, UInt32 auctionCoolTime, UInt8 strengthen, UInt32 auctionTransTimes);
	UInt16 Merge(const char* reason, Player* player, UInt16 num, UInt32 auctionCoolTimeStamp, UInt8 strengthen, UInt32 auctionTransTimes);

    /**
    *@brief 获取战斗用的装备结构体
    */
    void GetRaceEquip(RaceEquip& equip, bool pvp);
	/**
	 *@brief 设置获取数量
	 */
	UInt16 GetNum() const { return m_Num; }
	void SetNum(UInt16 num){ m_Num = num > m_pDataEntry->maxNum ? m_pDataEntry->maxNum : num; }
	UInt16 GetMaxNum() const { return m_pDataEntry->maxNum; }

	/**
	 *@brief 设置获取绑定属性 1.非绑定 2.角色绑定 3.账号绑定
	 */
	void SetBind(UInt8 bind) { m_Bind = bind; }	// 废弃
	UInt8  GetBind() const { return m_pDataEntry->owner; }

	/**
	 *@brief 设置获取道具位置
	 */
	void SetPos(const ItemPos& pos) { m_Pack = pos.pack; m_Grid = pos.gridIndex; }
	const ItemPos GetPos() const { return ItemPos(m_Pack,m_Grid); }

	/**
	 *@brief 设置获取品质
	 */
	void SetQuality(UInt8 quality){ m_Quality = quality; }	// 废弃
	UInt8 GetQuality() const { return m_pDataEntry->color; }

	UInt8 GetQualityColor2() const { return  m_pDataEntry->color2; }

	/**
	*@brief 获取是否公告
	*/
	UInt8 GetCanAnnounce() const { return m_pDataEntry->canAnnounce; }

	UInt8 GetFashionTransparentFlag() const {
		return m_pDataEntry ? m_pDataEntry->isTransparentFashion : 0;
	}

	/**
	 *@brief 设置获取强化等级
	 */
	bool SetStrengthen(UInt8 strengthen);
	inline void SetStrengthenValue(UInt8 str)	{ (GetType() == ITEM_EQUIP || GetSubType() == ST_MAGICCARD) ? m_Strengthen = str : m_Strengthen = 0; }
	inline UInt8 GetStrengthen() const { return m_Strengthen; }

	/**
	 *@brief 能否降低强化或激化等级
	 */
	bool CanReduceStrengthenOrEnhanceLv();

	/**
	*@brief 设置获取强化失败次数
	*/
	void SetStrFailedTimes(UInt32 uTimes) { m_uStrFailedTimes = uTimes; }
	UInt32 GetStrFailedTimes() const { return m_uStrFailedTimes; }

	/**
	*@breif 获取装备属性id
	*/
	UInt32 GetEqPropId() { return m_pDataEntry->attrId; }

	/**
	*@brief 获取装备类型
	*/
	EquipType GetEquipType(){ return static_cast<EquipType>((UInt8)m_EquipType); }
	/**
	*@brief 改变装备类型
	*/
	void ChgEquipType(EquipType equipType);
	/**
	*@brief 改变红字装备增幅路线
	*/
	void ChgEnhanceType(EnhanceType enhanceType){ m_EnhanceType = enhanceType; }
	/**
	*@brief 获取红字装备增幅路线
	*/
	EnhanceType GetEnhanceType(){ return static_cast<EnhanceType>((UInt8)m_EnhanceType); }

	/**
	*@brief 设置获取增幅等级
	*/
	void SyncEnhance(){ SetEnhance(GetEnhance()); }
	bool SetEnhance(UInt8 enhance){ return SetStrengthen(enhance); }
	inline void SetEnhanceValue(UInt8 enhance)	{ (GetType() == ITEM_EQUIP || GetSubType() == ST_MAGICCARD) ? m_Strengthen = enhance : m_Strengthen = 0; }
	inline UInt8 GetEnhance() const { return m_Strengthen; }

	/**
	*@brief 设置获取增幅失败次数
	*/
	void SetEnhanceFailedTimes(UInt32 uTimes) { m_uEnhanceFailedTimes = uTimes; }
	UInt32 GetEnhanceFailedTimes() const { return m_uEnhanceFailedTimes; }

	/**
	*@brief 附魔相关
	*/
	bool SetMagic(MagicCardData* data);
	bool SetMagicNew(Item* card);
	bool SetMagicIdAndLv(UInt32 cardId, UInt8 lv);
	UInt32 GetMagicCardId();
	UInt8  GetMagicCardLv();
	void ClearMagic();
	/**
	*@brief 宝珠相关
	*/
	bool SetPreciousBead(PreciousBeadDataEntry* data);
	UInt32 GetPreciosBeadId();
	UInt64 GetPreciosBeadAddedTimeStamp() { return m_AddPreciousBeadTimeStamp; }

	/**
	 *@brief 随机属性相关
	 */
	void AddRandAttr(ItemRandProp type, UInt32 value){ m_RandProperties.AddProperty(type,value); }
	ItemRandProperty<MAX_EQUIPATTR_NUM>& GetRandAttrs() { return m_RandProperties; }

	/**
	 *@brief 设置获取灵活字段
	 */
	void SetParam1(UInt32 param1){ m_Param1 = param1; }
	UInt32 GetParam1() const { return m_Param1; }
	void SetParam2(UInt32 param2){ m_Param2 = param2; }
	UInt32 GetParam2() const { return m_Param2; }

	/**
	 *@brief 获取配置参数
	 */
	//UInt32 GetEffectParam(int index);

	/**
	*@brief 设置获得失效时间
	*/
	bool SetDeadLine();
	void SetDeadLine(UInt32 durationDay);
	void SetDeadLineSt(UInt32 timestamp);
	UInt32 GetDeadLine() { return m_uDeadLine; }

	bool IsPermanent() { return m_uDeadLine == 0; }
	/**
	 *@brief 将数值加到身上
	 */
	//void ReloadEquipEffect(Player* owner);

	/**
	 *@brief 获取战力值
	 */
	//UInt32 GetPower() const{ return m_Power; }

	/**
	 *@brief 检查选项
	 */
	//bool CheckOption(ItemOption option);

    /**
    *@brief 能否在拍卖行中出售
    */
    bool CanAuctionSell(UInt32 num);

	/**
	*@brief 是不是拍卖行装备
	*/
	bool IsAuctionEquip();

	/**
	 *@brief 同步属性
	 */
	void SyncProperty(Player* player, bool bFlush = false);
	void ScriptSyncProperty(Player* player){ SyncProperty(player, true); }
	 
	/**
	 *@brief 判断位置和数量是否为脏
	 */
	bool IsGridNumDirty(UInt32 type){ return m_Num.IsDirty(type) || m_Grid.IsDirty(type); }
		

	//计算所有的品级属性
	bool SetAllQualityProp(UInt8 ql);
	//计算单条品级属性
	void SetSingleQualityProp(EquipQLAttrType type, UInt8 ql, EquipAttrDataEntry* attrData, EquipQLValueData* qlValueData);
	inline void SetQualityLv(UInt8 ql)	{ m_QualityLv = ql; }
	inline UInt8 GetQualityLv() { return m_QualityLv; }

	inline void SetPhyAtk(UInt32 phyAtk) { m_PhyAtk = phyAtk; }
	inline UInt32 GetPhyAtk() { return m_PhyAtk; }

	inline void SetMagAtk(UInt32 magAtk) { m_MagAtk = magAtk; }
	inline UInt32 GetMagAtk() { return m_MagAtk; }

	inline void SetIndependAtk(UInt32 independAtk) { m_IndependAtk = independAtk; }
	inline UInt32 GetIndependAtk() { return m_IndependAtk; }
	UInt32	GetPVPIndependAtk();

	inline void SetIndependAtkStreng(UInt32 indpendAtkStreng) {	m_IndependAtkStreng = indpendAtkStreng; }
	inline UInt32 GetIndependAtkStreng() { return m_IndependAtkStreng; }
	UInt32 GetPVPIndependAtkStreng();

	inline void SetMagDef(UInt32 def) { m_MagDef = def; }
	inline UInt32 GetMagDef() { return m_MagDef; }

	inline void SetPhyDef(UInt32 def) { m_PhyDef = def; }
	inline UInt32 GetPhyDef() { return m_PhyDef; }

	inline void SetStrenth(UInt32 strenth) { m_Strenth = strenth; }
	inline UInt32 GetStrenth() { return m_Strenth; }

	inline void SetStamina(UInt32 stamina) { m_Stamina = stamina; }
	inline UInt32 GetStamina() { return m_Stamina; }

	inline void SetSpirit(UInt32 spirt) { m_Spirit = spirt; }
	inline UInt32 GetSpirit() { return m_Spirit; }

	inline void SetIntellect(UInt32 intellect) { m_Intellect = intellect; }
	inline UInt32 GetIntellect() { return m_Intellect; }

	inline void SetSealCount(UInt32 count) { m_SealCount = count; }
	inline UInt32 GetSealCount() { return m_SealCount; }

	inline void SetSealState(UInt8 state) { m_SealState = state; }
	inline UInt8 GetSealState() { return m_SealState; }

	inline void SetIsTreas(UInt8 isTreas) { m_isTreas = isTreas; }
	inline bool IsTreas() { return m_isTreas == 1; }

	inline void SetBeadExtripeCnt(UInt32 cnt) { m_BeadExtripeCnt = cnt; }
	inline UInt32 GetBeadExtripeCnt() {  return m_BeadExtripeCnt; }

	inline void SetBeadReplaceCnt(UInt32 cnt) { m_BeadReplaceCnt = cnt; }
	inline UInt32 GetBeadReplaceCnt() { return m_BeadReplaceCnt; }

	inline void SetLightStrProp(Int32 value) {}

	void SetStrProp(EquipExProp type, Int32 value);
	Int32 GetStrProp(EquipExProp type);

	void SetDefProp(EquipExProp type, Int32 value);
	Int32 GetDefProp(EquipExProp type);

	void SetAbnormalResists(EquipAbnormalResist type, Int32 value);
	Int32 GetAbnormalResists(EquipAbnormalResist type);

	void SetAbnormalResistsTotal(Int32 value);
	inline Int32 GetAbnormalResistsTotal() { return m_AbnormalResistsTotal; }

	void SetAccId(UInt32 accid, bool bDirty = false);
	inline UInt32 GetAccId() { return m_AccId; }

	void SetOwnerId(UInt64 ownerid, bool bDirty = false);
	inline UInt64 GetOwnerId() { return m_OwnerId; }

	inline UInt32 GetDisPhyAtk() { return m_DisPhyAtk; }
	inline UInt32 GetDisMagAtk() { return m_DisMagAtk; }
	inline UInt32 GetDisPhyDef()	{ return m_DisPhyDef; }
	inline UInt32 GetDisMagDef()	{ return m_DisMagDef; }

	inline UInt32 GetPVPDisPhyAtk() { return m_PvpDisPhyAtk; }
	inline UInt32 GetPVPDisMagAtk() { return m_PvpDisMagAtk; }
	inline UInt32 GetPVPDisPhyDef()	{ return m_PvpDisPhyDef; }
	inline UInt32 GetPVPDisMagDef()	{ return m_PvpDisMagDef; }

	UInt32 GetPVPAssistStrength();
	UInt32 GetPVPAssistStamina();
	UInt32 GetPVPAssistIntellect();
	UInt32 GetPVPAssistSpirit();

	inline void SetStrNum(UInt32 num)	{ m_StrenthNum = num; }
	inline UInt32 GetStrNum()	{ return m_StrenthNum; }

	inline void SetEnhanceNum(UInt32 num)	{ m_EnhanceCount = num; }
	inline UInt32 GetEnhanceNum()	{ return m_EnhanceCount; }

	inline void SetQlNum(UInt32 num)	{ m_SetQlNum = num; }
	inline UInt32 GetQlNum()			{ return m_SetQlNum; }

	inline void SetFashionAttrId(UInt32 id)	{ m_FashionAttrId = id; AddFashionAttrChangeNum(); }
	inline UInt32 GetFashionAttrId()	{ return m_FashionAttrId.GetDBValue(); }

	bool SetFashionDefaultAttr();
	void AddFashionAttrChangeNum();
	inline UInt32 GetFashionAttrChangeNum()	{ return m_FashionAttrNum.GetDBValue(); }

	inline ItemRandProperty<MAX_EQUIPATTR_NUM>& GetRandProps()	{ return m_RandProperties; }
	inline UInt32	GetDayUseNum()	{ return m_DayUseNum; }

	inline ItemCheckType GetTimeState() { return m_timeState; }

	inline bool GetNotifyTimeState()	{ return m_bNotifyTimeState; }
	inline void SetNotifyTimeState(bool b)	{ m_bNotifyTimeState = b; }

	Int16 GetTownSpeedRate();

	//计算装备评分
	UInt32	CalculateValuedScore(Player* player);
	//获取基础属性评分
	float	GetBasePropScore();
	//获取随机属性评分
	float	GetRandPropScore();
	//获取强化属性评分
	float	GetStrPropScore();
	//获取附魔属性评分
	float	GetMagicPropScore();
	//获取宝珠属性评分
	float   GetPreciousBeadPropScore();
	//属性评分
	float	GetScoreByPropType(ItemScoreProp type);
	//部位系数
	float	GetPartRate();
	//技能分
	float	GetSkillScore();
	//获取附带属性评分
	float	GetExPropScore();
	//获取铭文评分
	float	GetInscriptionScore();
	//获取强化独立攻击力评分
	float	GetIndpentAtkStrScore();

	//转换附带属性到评分属性
	ItemScoreProp TransExProp(EquipExType type, UInt32 prop);
	//转换随机属性到评分属性
	ItemScoreProp TransRandPropType(ItemRandProp rp);
	//for test
	std::string GetTypeTransName(ItemScoreProp type);

	//获取部位类别
	EquipPartType GetPartType();

	//设置属性
	bool SetAttribute(bool isDynamic = false);

	//生成动态虚拟装备属性配置
	EquipAttrDataEntry* GenVirtualAttributeData();

	//清理属性
	void ClearAttribute();

	bool GenDataAttrs();

	//转移石
	bool SetTransferStone(ItemDataEntry* data);
	UInt32 GetTransferStoneid();
	bool ResetTransferStone();

	//装备回收
	UInt32 GetEqRecoScore() { return m_recoScore; }
	void SetEqRecoScore(UInt32 score) { m_recoScore = score; }

	// 道具锁
	bool IsItemLock(UInt32 _lockType);
	void SetItemLock(UInt32 _lockType);
	void ClearItemLock(UInt32 _lockType);

	// 生成宝珠镶嵌孔
	void GenPreciousBeadHoles();
	void GenSecondPreciousBeadHoles();
	PreciousBeadHoleProperty<2>& GetPreciousBeadHoleProperty() { return m_PreciousBeadHoles; }
	void GenPreciousBeadHoleLoaded();
	bool IsMountedPrecBead();
	UInt32 GetFirstPreciosBeadId();
	UInt32 GetSecondPreciosBeadId();
	void ClearPrecBead();

	//宝珠附加属性生成
	bool GenPreciousBeadAddtionAttri();

	//拍卖行交易冷却时间戳
	void SetAuctionCoolTimeStamp(UInt32 timeStamp);
	UInt32 GetAuctionCoolTimeStamp();
	bool IsInAuctionCoolTimeStamp();
	//拍卖行检查过期时间
	bool CheckDueTimeForAuction(Player* player);
	bool CheckAuctionTransNum();
	void SetAuctionTransNum(UInt32 auctionTransNum);
	UInt32 GetAuctionTransNum();
	//租赁物品
	//bool IsLease() { return m_IsLease == 1; }
	//void SetLease(UInt8 isLease) { m_IsLease = isLease; }

	//获取辅助装备强化四维属性加成
	UInt32 GetFourDimenAddition();

	UInt32 GetFourDimenAdditionPvp();

	bool CheckProhibitOp(UInt8 op);

	// 铭文
	InscriptionHoleProperty<2>& GetInscriptionHoleProperty() { return m_InscriptionHoles; }
	void   EquipInscriptionModifyByEqChage();
public:// 评分

	/**
	*@brief 设置获取评分
	*/
	void SetItemScore(UInt32 score) { m_ItemScore = score; }
	UInt32 GetItemScore() { return m_ItemScore; }

	/**
	*@brief 计算评分
	*/
	void CalcItemScore(Player* owner);

public://装备方案
	//是否可以放入装备方案
	bool IsCanPushScheme();

public: //item key-value
	bool SetValue(UInt32 key, Int32 value);
	Int32	GetValue(UInt32 key);
public: //道具创建相关
	/**
	 *@brief 创建销毁道具对象
	 */
	static Item* CreateItem(UInt32 itemid);
	static void DestroyItem(Item*& item);

public://编解码相关
	CL_SYNC_PROPERTY_BEGIN(CLSyncObject)

	CL_SYNC_PROPERTY(m_Num,			IA_NUM,			SST_SELF|SST_SCENE|SST_WATCH|SST_WORLD)
	CL_SYNC_PROPERTY(m_Bind,		IA_BIND,		SST_SELF|SST_SCENE|SST_WATCH|SST_WORLD)
	CL_SYNC_PROPERTY(m_Pack,		IA_PACK,		SST_SELF|SST_SCENE)
	CL_SYNC_PROPERTY(m_Grid,		IA_GRID,		SST_SELF|SST_WATCH|SST_SCENE)
	CL_SYNC_PROPERTY(m_PhyAtk,		IA_PHY_ATK,		SST_SELF|SST_WATCH|SST_WORLD)
	CL_SYNC_PROPERTY(m_MagAtk,		IA_MAG_ATK,		SST_SELF | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_PhyDef,		IA_PHY_DEF, SST_SELF | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_MagDef,		IA_MAG_DEF, SST_SELF | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_Strenth,		IA_STR,			SST_SELF | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_Stamina,		IA_STAMINA,		SST_SELF|SST_WATCH|SST_WORLD)
	CL_SYNC_PROPERTY(m_Intellect,	IA_INTELLECT,	SST_SELF | SST_WATCH| SST_WORLD)
	CL_SYNC_PROPERTY(m_Spirit,		IA_SPIRIT,		SST_SELF | SST_WATCH| SST_WORLD)
	CL_SYNC_PROPERTY(m_QualityLv,	IA_QUALITYLV,		SST_SELF | SST_WATCH| SST_WORLD)
	CL_SYNC_PROPERTY(m_Quality,		IA_QUALITY,		SST_SELF|SST_SCENE|SST_WATCH|SST_WORLD)
	CL_SYNC_PROPERTY(m_Strengthen,	IA_STRENGTHEN,		SST_SELF|SST_SCENE|SST_WATCH|SST_WORLD)
	CL_SYNC_PROPERTY(m_RandProperties,	IA_RANDATTR,	SST_SELF|SST_SCENE|SST_WATCH|SST_WORLD)
	CL_SYNC_PROPERTY(m_DayUseNum,			IA_DAYUSENUM,		SST_SELF|SST_SCENE|SST_WATCH|SST_WORLD)
	//CL_SYNC_PROPERTY(m_Gemstones,		IA_GEMSTONE,	SST_SELF|SST_SCENE|SST_WATCH|SST_WORLD)
	CL_SYNC_PROPERTY(m_Param1,		IA_PARAM1,		SST_SELF|SST_SCENE|SST_WATCH|SST_WORLD)
	CL_SYNC_PROPERTY(m_Param2,		IA_PARAM2,		SST_SELF|SST_SCENE|SST_WATCH|SST_WORLD)
	//CL_SYNC_PROPERTY(m_Power,		IA_POWER,		SST_SELF)
	CL_SYNC_PROPERTY(m_uDeadLine,	IA_DEADLINE,	SST_SELF|SST_SCENE|SST_WATCH|SST_WORLD)
	CL_SYNC_PROPERTY(m_uStrFailedTimes,		IA_STRFAILED,	SST_SELF|SST_SCENE|SST_WORLD)
	CL_SYNC_PROPERTY(m_AddMagic, IA_ADDMAGIC, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_SealState, IA_SEAL_STATE, SST_SELF | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_SealCount, IA_SEAL_COUNT, SST_SELF | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_DisPhyAtk, IA_DIS_PHYATK, SST_SELF | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_DisMagAtk, IA_DIS_MAGATK, SST_SELF | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_DisPhyDef, IA_DIS_PHYDEF, SST_SELF | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_DisMagDef, IA_DIS_MAGDEF, SST_SELF | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_PvpDisPhyAtk, IA_PVP_DIS_PHYATK, SST_WORLD)
	CL_SYNC_PROPERTY(m_PvpDisMagAtk, IA_PVP_DIS_MAGATK, SST_WORLD)
	CL_SYNC_PROPERTY(m_PvpDisPhyDef, IA_PVP_DIS_PHYDEF, SST_WORLD)
	CL_SYNC_PROPERTY(m_PvpDisMagDef, IA_PVP_DIS_MAGDEF, SST_WORLD)
	CL_SYNC_PROPERTY(m_ValuedScore, IA_VALUE_SCORE, SST_SELF | SST_WORLD | SST_WATCH)
	CL_SYNC_PROPERTY(m_SetQlNum, IA_SET_QL_NUM, SST_WORLD)
	CL_SYNC_PROPERTY(m_FashionAttrId, IA_FASHION_ATTRID, SST_SELF | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_FashionAttrNum, IA_FASHION_ATTR_SELNUM, SST_SELF | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_PhyDefPercent, IA_PHYDEF_PERCENT, SST_SELF | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_MagDefPercent, IA_MAGDEF_PERCENT, SST_SELF | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_AddPreciousBead, IA_ADD_PRECIOUSBEAD, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_StrPropEx[EEP_LIGHT], IA_STRPROP_LIGHT, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_StrPropEx[EEP_FIRE], IA_STRPROP_FIRE, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_StrPropEx[EEP_ICE], IA_STRPROP_ICE, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_StrPropEx[EEP_DARK], IA_STRPROP_DARK, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_DefPropEx[EEP_LIGHT], IA_DEFPROP_LIGHT, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_DefPropEx[EEP_FIRE], IA_DEFPROP_FIRE, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_DefPropEx[EEP_ICE], IA_DEFPROP_ICE, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_DefPropEx[EEP_DARK], IA_DEFPROP_DARK, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_AbnormalResistsTotal, IA_ABNORMAL_RESISTS_TOTAL, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_AbnormalResists[EAR_FLASH], IA_EAR_FLASH, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_AbnormalResists[EAR_BLEEDING], IA_EAR_BLEEDING, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_AbnormalResists[EAR_BURN], IA_EAR_BURN, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_AbnormalResists[EAR_POISON], IA_EAR_POISON, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_AbnormalResists[EAR_BLIND], IA_EAR_BLIND, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_AbnormalResists[EAR_STUN], IA_EAR_STUN, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_AbnormalResists[EAR_STONE], IA_EAR_STONE, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_AbnormalResists[EAR_FROZEN], IA_EAR_FROZEN, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_AbnormalResists[EAR_SLEEP], IA_EAR_SLEEP, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_AbnormalResists[EAR_CONFUNSE], IA_EAR_CONFUNSE, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_AbnormalResists[EAR_STRAIN], IA_EAR_STRAIN, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_AbnormalResists[EAR_SPEED_DOWN], IA_EAR_SPEED_DOWN, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_AbnormalResists[EAR_CURSE], IA_EAR_CURSE, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_TransferStone, IA_TRANSFER_STONE, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_recoScore, IA_RECO_SCORE, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_lockType, IA_IS_LOCK, SST_SELF | SST_SCENE | SST_WATCH)
	CL_SYNC_PROPERTY(m_PreciousBeadHoles, IA_PRECIOUSBEAD_HOLES, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_AuctionCoolTimeStamp, IA_AUCTION_COOL_TIMESTAMP, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_isTreas, IA_IS_TREAS, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_BeadExtripeCnt, IA_BERD_EXTRIPE_CNT, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_BeadReplaceCnt, IA_BERD_REPLACE_CNT, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_DataId, IA_DATA_ID, SST_SELF | SST_SCENE)
	CL_SYNC_PROPERTY(m_EquipType, IA_EQUIP_TYPE, SST_SELF | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_EnhanceType, IA_ENHANCE_TYPE, SST_SELF | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_EnhanceNum, IA_ENHANCE_NUM, SST_SELF | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_uEnhanceFailedTimes, IA_ENHANCE_FAILED, SST_SELF | SST_SCENE | SST_WORLD)
	CL_SYNC_PROPERTY(m_AuctionTransNum, IA_AUCTION_TRANS_NUM, SST_SELF | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_InscriptionHoles, IA_INSCRIPTION_HOLES, SST_SELF | SST_SCENE | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_IndependAtk, IA_INDEPENDATK, SST_SELF | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_IndependAtkStreng, IA_INDEPENDATK_STRENG, SST_SELF | SST_WATCH | SST_WORLD)
	CL_SYNC_PROPERTY(m_SubType, IA_SUBTYPE, SST_SELF | SST_SCENE | SST_WORLD)
	CL_SYNC_PROPERTY(m_KeyValues, IA_KEYVALUES, SST_SELF | SST_SCENE | SST_WORLD | SST_WATCH)
		
	CL_SYNC_PROPERTY_END()

	CL_DBPROPERTY_BEGIN(CLSyncObject)
	CL_DBPROPERTY("itemid",		m_DataId)
	CL_DBPROPERTY("accid",		m_AccId)
	CL_DBPROPERTY("owner",		m_OwnerId)
	CL_DBPROPERTY("num",		m_Num)
	CL_DBPROPERTY("bind",		m_Bind)
	CL_DBPROPERTY("pack",		m_Pack)
	CL_DBPROPERTY("grid",		m_Grid)
	CL_DBPROPERTY("phyatk",		m_PhyAtk)
	CL_DBPROPERTY("magatk",		m_MagAtk)
	CL_DBPROPERTY("magdef",	m_MagDef)
	CL_DBPROPERTY("phydef", m_PhyDef)
	CL_DBPROPERTY("strenth",	m_Strenth)
	CL_DBPROPERTY("stamina",	m_Stamina)
	CL_DBPROPERTY("intellect",	m_Intellect)
	CL_DBPROPERTY("spirit",		m_Spirit)
	CL_DBPROPERTY("qualitylv", m_QualityLv)
	CL_DBPROPERTY("quality", m_Quality)
	CL_DBPROPERTY("strengthen", m_Strengthen)
	CL_DBPROPERTY("randattr",	m_RandProperties)
	CL_DBPROPERTY("dayusenum",	m_DayUseNum)
	CL_DBPROPERTY("strenthnum", m_StrenthNum)
	CL_DBPROPERTY("addmagic",	m_AddMagic)
	CL_DBPROPERTY("param1",		m_Param1)
	CL_DBPROPERTY("param2",		m_Param2)
	CL_DBPROPERTY("deadline",	m_uDeadLine)
	CL_DBPROPERTY("strfailed",	m_uStrFailedTimes)
	CL_DBPROPERTY("sealstate", m_SealState)
	CL_DBPROPERTY("sealcount", m_SealCount)
	CL_DBPROPERTY("disphyatk", m_DisPhyAtk)
	CL_DBPROPERTY("dismagatk", m_DisMagAtk)
	CL_DBPROPERTY("disphydef", m_DisPhyDef)
	CL_DBPROPERTY("dismagdef", m_DisMagDef)
	CL_DBPROPERTY("pvpdisphyatk", m_PvpDisPhyAtk)
	CL_DBPROPERTY("pvpdismagatk", m_PvpDisMagAtk)
	CL_DBPROPERTY("pvpdisphydef", m_PvpDisPhyDef)
	CL_DBPROPERTY("pvpdismagdef", m_PvpDisMagDef)
	CL_DBPROPERTY("valuedscore", m_ValuedScore)
	CL_DBPROPERTY("setqlnum", m_SetQlNum)
	CL_DBPROPERTY("fashionattrid", m_FashionAttrId)
	CL_DBPROPERTY("fashionattrnum", m_FashionAttrNum)
	CL_DBPROPERTY("addpreciousbead", m_PreciousBeadHoles)
	CL_DBPROPERTY("transferstone", m_TransferStone)
	CL_DBPROPERTY("recoScore", m_recoScore)
	CL_DBPROPERTY("isLock", m_lockType)
	CL_DBPROPERTY("auction_cool_timestamp", m_AuctionCoolTimeStamp)
	CL_DBPROPERTY("bead_extripe_cnt", m_BeadExtripeCnt)
	CL_DBPROPERTY("bead_replace_cnt", m_BeadReplaceCnt)
	CL_DBPROPERTY("equiptype", m_EquipType)
	CL_DBPROPERTY("enhancetype", m_EnhanceType)
	CL_DBPROPERTY("enhancefailed", m_uEnhanceFailedTimes)
	CL_DBPROPERTY("enhancecount", m_EnhanceCount)
	CL_DBPROPERTY("auctionTransNum", m_AuctionTransNum)
	CL_DBPROPERTY("inscriptionHoles", m_InscriptionHoles)
	CL_DBPROPERTY("keyValues", m_KeyValues)
		
	//CL_DBPROPERTY("isLease", m_IsLease)

	CL_DBPROPERTY_END()

	void PackWatchData(Avalon::NetOutputStream& stream);

private:
	//道具id
	ObjUInt32	m_DataId;
	//子类型
	ObjInt8		m_SubType;
	//对应的账号id(仓库用)
	ObjUInt32	m_AccId;
	//对应的角色uid(存入仓库前的角色,取出来更换成当前的角色)
	ObjUInt64	m_OwnerId;
	//叠加数
	ObjUInt16	m_Num;
	//是否绑定 1未绑定 2角色绑定 3账号绑定
	ObjUInt8	m_Bind;
	//包裹
	ObjUInt8	m_Pack;
	//格子
	ObjUInt32	m_Grid;

	//物理攻击
	ObjUInt32	m_PhyAtk;
	//法术攻击
	ObjUInt32	m_MagAtk;
	//独立攻击力
	ObjUInt32	m_IndependAtk;
	//独立攻击力强化
	ObjUInt32	m_IndependAtkStreng;
	ObjUInt32	m_IndependAtkPvpStreng;
	//物理防御
	ObjUInt32	m_PhyDef;
	//法术防御
	ObjUInt32	m_MagDef;
	//力量
	ObjUInt32	m_Strenth;
	//体力
	ObjUInt32	m_Stamina;
	//智力
	ObjUInt32	m_Intellect;
	//精神
	ObjUInt32	m_Spirit;
	//品级
	ObjUInt8	m_QualityLv;

	//装备品质
	ObjUInt8	m_Quality;
	//强化等级
	ObjUInt8	m_Strengthen;
	//强化连续失败次数
	ObjUInt32	m_uStrFailedTimes;
	//强化次数
	ObjUInt32	m_StrenthNum;
	//装备类型(EquipType):普通0/带气息1/红字2
	ObjUInt8    m_EquipType;
	//红字装备增幅类型:力量0/智力1/体力2/精神3
	ObjUInt8    m_EnhanceType;
	//红字装备增幅数值
	ObjUInt32   m_EnhanceNum;
	//红字装备增幅PVP数值
	UInt32      m_EnhancePVPNum;
	//增幅连续失败次数
	ObjUInt32	m_uEnhanceFailedTimes;
	//增幅次数
	ObjUInt32	m_EnhanceCount;



//强化
	//无视物理攻击
	ObjUInt32	m_DisPhyAtk;
	//无视法术攻击
	ObjUInt32	m_DisMagAtk;
	//无视防御(除以10000还原)
	ObjUInt32	m_DisPhyDef;
	ObjUInt32	m_DisMagDef;
	//物理防御百分比
	ObjUInt32	m_PhyDefPercent;
	//魔法防御百分比
	ObjUInt32	m_MagDefPercent;

	//PVP强化属性
	ObjUInt32	m_PvpDisPhyAtk;
	ObjUInt32	m_PvpDisMagAtk;
	ObjUInt32	m_PvpDisPhyDef;
	ObjUInt32	m_PvpDisMagDef;
	ObjUInt32	m_PvpPhyDefPercent;
	ObjUInt32	m_PvpMagDefPercent;

	//随机属性
	ItemRandProperty<MAX_EQUIPATTR_NUM>	m_RandProperties;
	
	ItemRandProperty<1>					m_FinalEquipProperty;

	//随机属性最低星级
	UInt8		m_RandAttrStar;

	//日已经使用次数
	ObjUInt32	m_DayUseNum;
	
	//附魔卡
	ItemMagicProperty<1/*MAX_MAGIC_NUM*/>		m_AddMagic;

	//灵活字段1:是否新增道具
	ObjUInt32	m_Param1;
	//灵活字段2:道具使用时间
	ObjUInt32	m_Param2;

	//失效时间
	ObjUInt32	m_uDeadLine;
	//时效状态
	ItemCheckType		m_timeState;
	//封印状态
	ObjUInt8	m_SealState;
	//封印次数
	ObjUInt32	m_SealCount;

	//表格项
	ItemDataEntry*	m_pDataEntry;
	//属性表
	EquipAttrDataEntry*	m_pAttrData;
	//固有属性
	EquipAttrDataEntry*	m_pAttrDataInherent;
	//固有属性id
	UInt32		m_AttrDataInherentId;

	ObjUInt32	m_ValuedScore;
	//品级调整次数
	ObjUInt32	m_SetQlNum;

	//时装属性id
	ObjUInt32	m_FashionAttrId;
	//时装属性更改次数
	ObjUInt32	m_FashionAttrNum;

	//是否发送时限通知
	bool m_bNotifyTimeState;

	//宝珠
	PreciousBeadProperty<1>	m_AddPreciousBead; //added by wenhao 2017.12.1
	//添加宝珠时间戳
	UInt64			m_AddPreciousBeadTimeStamp;

	//宝珠镶嵌孔
	PreciousBeadHoleProperty<2> m_PreciousBeadHoles;

	//强化附带属性
	ObjInt32		m_StrPropEx[EEP_MAX];
	//抗性附带属性
	ObjInt32		m_DefPropEx[EEP_MAX];
	//异常抗性总
	ObjInt32		m_AbnormalResistsTotal;
	//异常抗性
	ObjInt32		m_AbnormalResists[EAR_MAX];

	//传家石 itemid
	ObjUInt32		m_TransferStone;
	//装备回收后所得积分
	ObjUInt32		m_recoScore;
	//锁类型
	ObjUInt32		m_lockType;
	//拍卖行交易冷却时间戳(秒)
	ObjUInt32		m_AuctionCoolTimeStamp;
	//是否租赁道具
	//ObjUInt8		m_IsLease;
	//是否珍品
	ObjInt8			m_isTreas;
	//宝珠的摘除次数
	ObjInt32		m_BeadExtripeCnt;
	//宝珠的替换次数
	ObjInt32		m_BeadReplaceCnt;
	//评分
	UInt32			m_ItemScore;
	//拍卖行交易次数
	ObjUInt32	m_AuctionTransNum;
	//铭文镶嵌孔
	InscriptionHoleProperty<2> m_InscriptionHoles;
	//item key values
	ItemKeyValues<0> m_KeyValues;
};

/**
 *@brief 道具访问器
 */
typedef CLVisitor<Item>	ItemVisitor;

/**
 *@brief 奖励组
 */
class RewardGroup
{
public:
	RewardGroup(){}
	explicit RewardGroup(const ItemRewardVec& rewards):m_Rewards(rewards){}
	~RewardGroup(){}

public:
	/**
	 *@brief 添加一个奖励,id,数量,品级,强化等级
	 */
	void AddReward(UInt32 id, UInt32 num, UInt8 qualityLv = 0, UInt8 strenth = 0, 
		UInt32 auctionCoolTimeStamp = 0, EquipType equipType = EQUIP_NORMAL, UInt32 auctionTransTimes = 0);

	void AddRewardScript(UInt32 id, UInt32 num, UInt8 qualityLv = 0, UInt8 strenth = 0);

	void AddRewards(const ItemRewardVec& rewards);

	/**
	 *@brief 获取请求奖励列表
	 */
	const ItemRewardVec& GetRewards() const { return m_Rewards; }
	ItemRewardVec& GetRewardsRef()  { return m_Rewards; }
	void ClearRewards(){ m_Rewards.clear(); }

	/**
	 *@brief 获取符合条件的奖励列表
	 */
	static void GetFitRewards(Player* player, const ItemRewardVec& rewards, ItemRewardVec& fitRewards);
	void GetFitRewards(Player* player, const ItemRewardVec& rewards);
	void GetFitRewards(Player* player, const RewardGroup* rewards);

private:
	//奖励列表
	ItemRewardVec	m_Rewards;
};

#endif
