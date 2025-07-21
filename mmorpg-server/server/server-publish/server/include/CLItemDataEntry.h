#ifndef _CL_ITEM_DATAENTRY_H_
#define _CL_ITEM_DATAENTRY_H_

#include <functional>
#include "CLItemDefine.h"
#include "CLGameDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <bitset>


/**
 *@brief Item数据项
 */
class ItemDataEntry : public Avalon::DataEntry
{
public:
	ItemDataEntry();
	~ItemDataEntry();

public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);
	//UInt64 GetFabaoGridBits() const{ return (UInt64(effectParam[1]) << 32) + effectParam[0]; }

	bool IsMeetOccuRequirement(Occupation playerOccu) const;

public:
	//道具id
	UInt32		id;
	//道具名称
	std::string	name;
	//类型
	UInt8		type;
	//子类型
	UInt32		subType;
	//三级类型
	UInt32		thirdType;
	//职业
	std::bitset<Occupation::OCCU_MAX>		occu;
	//职业筛选
	std::set<Int8>		occuFilter;
	//UInt8		occu;
	//品质颜色
	UInt8	color;
	//品质颜色2
	UInt8   color2;
	//需要等级
	UInt16		needLevel;
	//最大等级
	UInt16		maxLevel;
	//基础攻速(千分比)
	Int32		baseAtkSpeed;
	//能否使用
	UInt8		canUse;
	//能否交易
	UInt8		canTrade;
	//归属 1.非绑定 2.角色绑定 3.账号绑定
	UInt8		owner;
	//是否封装
	UInt8		seal;
	//封装上限
	UInt32		sealMax;
	//是否可分解
	UInt8		canDecompose;
	//出售成的物品ID
	UInt32		sellItemId;
	//售价
	UInt32		price;
	//CD组
	UInt8		cdGroup;
	//CD时间
	UInt32		cdTime;
	//时效性
	UInt32		time;
	//叠加数
	UInt16		maxNum;
	//使用附加Buff
	UInt32		onUseBuff;
	//获得附加buff
	UInt32		onGetBuff;
	//能否单局使用
	UInt32		canDungeonUse;
	//能否pk使用
	UInt8		canPkUse;
    //推荐价格
    UInt32      recommendPrice;
	//套装ID
	UInt32		suitId;
	//属性ID
	UInt32		attrId;
	//互斥buffID
	std::vector<UInt32>		mutexBuffIDs;
	//是否公告
	UInt8		canAnnounce;
	//使用限制类别
	UInt32		useLimiteType;
	//使用限制值
	UInt32		useLimiteValue;
	//是否废弃
	UInt8		isAbandon;
	//礼包表关联ID
	UInt32		giftId;
	//是否禁止拍卖行复制
	bool		isForbidAuctionCopy;
	//道具续费信息
	std::vector<ItemRenewInfo>	itemRenewInfos;
	// 拍卖行价格下限
	UInt32		auctionMinPriceRate;
	// 拍卖行价格上限
	UInt32		auctionMaxPriceRate;
	// 能否师徒赠送
	UInt8		canMasterGive;
	// 获得限制数量
	UInt32		getLimitNum;
	// 开启消耗道具的魔罐规则礼包的道具id
	UInt32 jarGiftConsumeItemId;
	// 开启消耗道具的魔罐规则礼包的道具数量
	UInt32 jarGiftConsumeItemNum;
	// 是否透明时装
	UInt8		isTransparentFashion;
	// 宝珠镶嵌孔1生成权重
	std::vector<ProWeightItem> PreciousBeadHole1;
	// 宝珠镶嵌孔2生成权重
	std::vector<ProWeightItem> PreciousBeadHole2;
	// 强化券数值表索引
	UInt32 strenTicketDataIndex;
	// 宝珠等级
	UInt8 precBeadLev;
	// 宝珠类型
	UInt8 precBeadType;
	// 是否珍品
	UInt8 isTreasure;
	// 拍卖行交易冷却时间-买量
	UInt32 auctionTransCoolTimeMl;
	// 拍卖行交易冷却时间-其他
	UInt32 auctionTransCoolTimeOther;
	// 强化券装备等级限制最小值
	UInt32 strTicketLvLimitMin;	
	// 强化券装备等级限制最大值
	UInt32 strTicketLvLimitMax;
	// 是否记录日志
	bool isRecordLog;
	// 折扣礼包折扣券折扣比
	UInt32 discountGiftCouponPro;
	// 头衔id
	UInt32 newTitleId;
	// 到期时间,时间戳
	UInt32 expireTime;
	// 拍卖行可交易次数
	UInt32 auctionTransNum;
	// 不可执行操作
	std::bitset<32> prohibitOps;
	// 使用次数上限
	UInt32	useLinmit;
	// 是否特殊附魔卡
	UInt8   isSpeicMagic;
	// 好友赠送道具赠送道具
	std::vector<ItemReward> presentedItems;
	// 附加参数
	std::string params;
	// 赠送道具被赠送上限
	UInt32	sendedLimit;
	// 赠送道具赠送上限
	UInt32	sendLimit;
	// 赠送道具被赠送总上线
	UInt32  sendedTotleLimit;
};

typedef Avalon::DataEntryVisitor<ItemDataEntry>	ItemDataEntryVisitor;
typedef std::vector<ItemDataEntry*> ItemDataEntryVec;

class ItemDataEntryMgr : public Avalon::DataEntryMgrBase<ItemDataEntry>
	, public Avalon::Singleton<ItemDataEntryMgr >
{
	typedef std::map<UInt16, UInt32> LevelEquipMap;

public:
	ItemDataEntryMgr();
	~ItemDataEntryMgr();

public:
	bool AddEntry(ItemDataEntry* dataEntry);

	const std::vector<UInt32>& GetDayUseItemCount()	const { return m_useCountItem; };

	const std::vector<UInt32>& GetWeek6UseItemCount()	const { return m_useCountItemWeek6; };
	
	const std::vector<UInt32>& GetTeamCopyItem()	const { return m_TeamCopyItem; };

	bool CheckCanAddGuildStorage(ItemDataEntry* dataEntry, bool isSealState);

	// 根据强化券索引获取道具data
	bool GetItemDataByStrenTicketIndex(UInt32 index, std::vector<ItemDataEntry*>& itemDatas);

	void GetSendUdpItems(ItemDataEntryVec& itemDatas);

// 	UInt32 GetNextEquip(UInt8 equipType, UInt8 occu, UInt16 level);
// 
// 	UInt32 GetSuitableEquip(UInt8 equipType, UInt8 occu, UInt16 level);
// 
// 	UInt32 SelectRandEquip(UInt8 occu, UInt16 level);
// 
// 	UInt32 GetPetEggId(UInt32 petId);

	const std::vector<UInt32>& GetFlyHellGiftItems()	const { return m_flyHellGiftItem; };

	const std::map<UInt8, std::vector<UInt32>>& GetMagicIds() {
		return m_magicIdsByQuality;
	};

private:
	//装备
	//LevelEquipMap			m_Equips[EQUIP_TYPE_WEAPON_MAX][5];
	//std::map<UInt16, std::vector<UInt32> >	m_mapEquips[5];

	//宠物-蛋索引
	//std::map<UInt32,UInt32> m_PetEggMap;

	//记录使用次数的道具
	std::vector<UInt32> m_useCountItem;

	//团本道具
	std::vector<UInt32> m_TeamCopyItem;

	//强化券索引
	std::map<UInt32, ItemDataEntryVec> m_StrenTicketMap;

	//SendUdpVec
	ItemDataEntryVec m_SendUdpItems;

	//飞升礼盒道具id集合
	std::vector<UInt32> m_flyHellGiftItem;

	//附魔卡集合,按品质
	std::map<UInt8, std::vector<UInt32>> m_magicIdsByQuality;

	//记录周使用次数(周六刷新)的道具
	std::vector<UInt32> m_useCountItemWeek6;
};

/**
*@brief 装备属性表
*/
class EquipAttrDataEntry : public Avalon::DataEntry
{
public:
	EquipAttrDataEntry();
	~EquipAttrDataEntry();

public:
	UInt32 GetKey() const  { return id; }
	bool Read(Avalon::DataRow& row);

public:
	//道具id
	UInt32 id;			
	//物理攻击
	Int32		atk;
	//法术攻击
	Int32		magicAtk;
	//独立攻击力
	Int32		independAtk;
	//物理防御
	Int32		def;
	//法术防御
	Int32		magicDef;
	//力量
	Int32		strenth;
	//智力
	Int32		intellect;
	//精神
	Int32		spirit;
	//体力
	Int32		stamina;
	//物理技能耗蓝变化百分比
	Int8		phySkillMp;
	//物理技能cd变化
	Int32		phySkillCd;
	//魔法技能耗蓝变化百分比
	Int8		magSkillMp;
	//魔法技能cd变化
	Int32		magSkillCd;
	//HP最大值
	Int32		HPMax;
	//MP最大值
	Int32		MPMax;
	//HP恢复
	Int32		HPRecover;
	//MP恢复
	Int32		MPRecover;
	//攻速(千分比)
	Int32		AtkSpeedRate;
	//释放速度(千分比)
	Int32		FireSpeedRate;
	//副本移动速度(千分比)
	Int32		BattleSpeedRate;
	//城镇移动速度(千分比)
	Int16		TownSpeedRate;
	//命中率(千分比)
	Int32		HitRate;
	//回避率(千分比)
	Int32		AvoidRate;
	//物理暴击(千分比)
	Int32		PhysicCrit;
	//魔法暴击(千分比)
	Int32		MagicCrit;
	//硬直
	Int32		Spasticity;
	//攻击附带属性
	Int32		AtkPropEx[EEP_MAX];
	//强化附带属性
	Int32		StrPropEx[EEP_MAX];
	//抗性附带属性
	Int32		DefPropEx[EEP_MAX];
	//异常抗性总
	Int32		AbnormalResistsTotal;
	//异常抗性
	Int32		AbnormalResists[EAR_MAX];
	//技能分
	Int32		skillScore;
	//附加buff info id
	std::vector<UInt32> attchBuffInfoIds;
	//深渊史诗掉落概率加成
	Int32       DungeonEpicDropAddition;
};
class EquipAttrDataMgr : public Avalon::DataEntryMgrBase<EquipAttrDataEntry>
	, public Avalon::Singleton<EquipAttrDataMgr >
{
	typedef std::map<UInt16, UInt32> LevelEquipMap;

public:
	EquipAttrDataMgr();
	~EquipAttrDataMgr();

public:
	bool AddEntry(EquipAttrDataEntry* dataEntry);

};

/**
*@brief 品级调整表
*/
class EquipQLRandData : public Avalon::DataEntry
{
public:
	EquipQLRandData();
	~EquipQLRandData();

public:
	UInt32 GetKey() const { return key; }

	bool Read(Avalon::DataRow& row);

public:
	UInt32					key;
	UInt8					color;			// 品质
	UInt32					count;			// 调整次数
	UInt32					randRangeMin;	// 调整范围min(百分比)
	UInt32					randRangeMax;	// 调整范围max(百分比)
	UInt32					surpriseRate;	// 惊喜概率(千分比)
	UInt32					surpriseRangeMin;	//惊喜范围min(百分比)
	UInt32					surpriseRangeMax;	//惊喜范围max(百分比)
};

class EquipQLRandDataMgr : public Avalon::DataEntryMgrBase<EquipQLRandData>
	, public Avalon::Singleton<EquipQLRandDataMgr >
{

public:
	EquipQLRandDataMgr();
	~EquipQLRandDataMgr();

public:
	bool LoadData(const std::string& filename);
	bool AddEntry(EquipQLRandData* dataEntry);
	void GetDatas(UInt8 color, std::vector<EquipQLRandData*>& datas);
	
	EquipQLRandData* GetData(UInt8 color, UInt32 randNum);
	UInt32	GetAdaptNum(UInt8 color, UInt8 ql);
private:
	std::map<UInt8, std::vector<EquipQLRandData*>> m_color2Datas;
};

/**
*@brief 品级数值表
*/
class EquipQLValueData : public Avalon::DataEntry
{
public:
	EquipQLValueData();
	~EquipQLValueData();

public:
	UInt32 GetKey() const { return part; }

	bool Read(Avalon::DataRow& row);

public:
	UInt32					part;				// 部位
	float					atkDef;				// 攻防系数
	float					fourDimensional;	// 四维系数
	float					perfectAtkDef;		// 完美攻防系数(百分比)
	float					perfectfourDimensional;	// 惊喜概率(千分比)
	float					strProp;			// 属性强化系数
	float					perfectStrProp;		// 完美属性强化系数
	float					defProp;			// 属性抗性系数
	float					perfectDefProp;		// 完美属性抗性系数
	float					adnormalResists;	// 异常抗性系数
	float					perfectAbnormalResists;	// 完美异常抗性系数
	float					independAtkProp; ;	// 独立攻击力系数
	float					perfectIndpendAtkProp; // 完美独立攻击力系数
};

class EquipQLValueDataMgr : public Avalon::DataEntryMgrBase<EquipQLValueData>
	, public Avalon::Singleton<EquipQLValueDataMgr>
{

public:
	EquipQLValueDataMgr();
	~EquipQLValueDataMgr();

public:
	bool AddEntry(EquipQLValueData* dataEntry);
};


/**
*@brief 随机基础属性数值表
*/
class RandBaseDataEntry : public Avalon::DataEntry
{
public:
	RandBaseDataEntry();
	~RandBaseDataEntry();

public:
	UInt32 GetKey() const { return baseRatio; }

	bool Read(Avalon::DataRow& row);

public:
	
	UInt8					baseRatio;		// 基础系数
	std::vector<UInt32>		QLPs;			// 品级概率
	std::vector<UInt32>		QLOMin;			// 品级修正最小值
	std::vector<UInt32>		QLOMax;			// 品级修正最大值
};
typedef Avalon::DataEntryVisitor<RandBaseDataEntry> RandBaseDataEntryVisitor;

class RandBaseDataEntryMgr : public Avalon::DataEntryMgrBase<RandBaseDataEntry>
	, public Avalon::Singleton<RandBaseDataEntryMgr >
{

public:
	RandBaseDataEntryMgr();
	~RandBaseDataEntryMgr();

public:
	bool AddEntry(RandBaseDataEntry* dataEntry);
	RandBaseDataEntry* GetData() { return m_randBaseData; }
private:
	RandBaseDataEntry* m_randBaseData;
};


/**
*@brief 随机属性表
*/
class RandAttrDataEntry : public Avalon::DataEntry
{
public:
	RandAttrDataEntry();
	~RandAttrDataEntry();

public:
	UInt32 GetKey() const { return type; }

	bool Read(Avalon::DataRow& row);

public:
	//随机属性类别
	ItemRandProp	type;
	UInt32			weight;
};
typedef Avalon::DataEntryVisitor<RandAttrDataEntry> RandAttrDataEntryVisitor;

class RandAttrDataEntryMgr : public Avalon::DataEntryMgrBase<RandAttrDataEntry>
	, public Avalon::Singleton<RandAttrDataEntryMgr >
{
	
public:
	RandAttrDataEntryMgr();
	~RandAttrDataEntryMgr();

public:
	bool AddEntry(RandAttrDataEntry* dataEntry);
	void GetTypes(std::vector<UInt32>& types) { types = m_types; }
	void GetWeights(std::vector<UInt32>& wts) { wts = m_wts; }
private:
	std::vector<UInt32>		m_types;
	std::vector<UInt32>		m_wts;
	
};

/**
*@brief 随机属性数目表
*/
class RandAttrNumDataEntry : public Avalon::DataEntry
{
public:
	RandAttrNumDataEntry();
	~RandAttrNumDataEntry();

public:
	UInt32 GetKey() const { return color; }

	bool Read(Avalon::DataRow& row);

public:
	//随机属性类别
	ItemQuality				color;
	std::vector<UInt32>		nums;
	std::vector<UInt32>		weights;
};
typedef Avalon::DataEntryVisitor<RandAttrNumDataEntry> RandAttrNumDataEntryVisitor;
typedef Avalon::DataEntryMgr<RandAttrNumDataEntry> RandAttrNumDataEntryMgr;

/**
*@brief 随机属性数值表
*/
class RandAttrValueDataEntry : public Avalon::DataEntry
{
public:
	RandAttrValueDataEntry();
	~RandAttrValueDataEntry();

public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	//随机属性类别
	UInt32					id;
	UInt32					type;
	ItemQuality				color;
	UInt32					lvMin;
	UInt32					lvMax;
	std::vector<UInt32>		values;
	std::vector<UInt32>		weights;
};
typedef Avalon::DataEntryVisitor<RandAttrValueDataEntry> RandAttrValueDataEntryVisitor;

class RandAttrValueDataEntryMgr : public Avalon::DataEntryMgrBase<RandAttrValueDataEntry>
	, public Avalon::Singleton<RandAttrValueDataEntryMgr >
{
	typedef std::map<UInt32, std::vector<RandAttrValueDataEntry*>> RandAttrValMap;		// 以属性类别为Key

public:
	RandAttrValueDataEntryMgr();
	~RandAttrValueDataEntryMgr();

public:
	bool AddEntry(RandAttrValueDataEntry* dataEntry);
	void GetDatas(UInt32 type, std::vector<RandAttrValueDataEntry*>& datas);
private:
	RandAttrValMap			m_randAttrs;
};



// 分解材料表
class DeComposeDataEntry : public Avalon::DataEntry
{
public:
	using ItemRateNumPair = std::pair<UInt32, UInt32>;
	DeComposeDataEntry();
	~DeComposeDataEntry();

public:
	UInt32 GetKey() const { return quality; }
	UInt8  GetColor2() const{ return color2; }

	bool Read(Avalon::DataRow& row);
	void ReadDecMat(Avalon::DataRow& row, UInt8 matType);		// matType:1.有色 2.无色 3.宇宙之眼
	void ReadRateMat(Avalon::DataRow& row, UInt8 matType);      // matType:1.异界 2.特殊 3.气息
public:
	//随机属性类别
	UInt8							quality;
	UInt8							color2;
	UInt16							minLv;
	UInt16							maxLv;
	std::vector<UInt32>				colorMats;
	std::vector<UInt32>				colorLessMats;
	std::vector<UInt32>				dogEyeNums;
	UInt32							colorMatId;
	UInt32							colorLessMatId;
	UInt32							dogEyeId;
	std::vector<ItemRateNumPair>	resistMagicMatNums;
	UInt32							resistMagicMatId;
	UInt32                          resistMagicTotalRate;
	std::vector<ItemRateNumPair>	pinkItemNum;
	UInt32                          pinkItemId;
	UInt32                          pinkItemTotalRate;
	std::vector<ItemRateNumPair>	redItemNum;
	UInt32                          redItemId;
	UInt32                          redItemTotalRate;
};
// 分解data管理
class DeComposeDataEntryMgr : public Avalon::DataEntryMgrBase<DeComposeDataEntry>
	, public Avalon::Singleton<DeComposeDataEntryMgr>
{
	class QualityKey
	{
	public:
		QualityKey() {}
		QualityKey(UInt8 c1, UInt8 c2) : color1(c1), color2(c2){}
		UInt8 color1;
		UInt8 color2;

		bool operator() (const QualityKey& key1, const QualityKey& key2) const
		{
			if (key1.color1 == key2.color1)
			{
				return key1.color2 < key2.color2;
			}
			else
			{
				return key1.color1 < key2.color1;
			}
		}

		bool operator != (const QualityKey& key) const
		{
			return color1 != key.color1 || color2 != key.color2;
		}
	};
	typedef std::map<QualityKey, std::vector<DeComposeDataEntry*>, QualityKey> DeComposeDataMap;		// 以品质为Key

	//typedef std::map<UInt8, std::vector<DeComposeDataEntry*>> DeComposeDataMap;		// 以品质为Key

public:
	DeComposeDataEntryMgr();
	~DeComposeDataEntryMgr();

public:
	bool AddEntry(DeComposeDataEntry* dataEntry);
	void GetDatas(UInt8 quality, UInt8 color2, std::vector<DeComposeDataEntry*>& datas);
private:
	DeComposeDataMap			m_decomposeDatas;
};

//装备强化概率表
class EquipStrRateData : public Avalon::DataEntry
{
public:
	EquipStrRateData();
	~EquipStrRateData();

public:
	UInt32	GetKey() const { return strengthen; }
	bool Read(Avalon::DataRow& row);
	bool ReadCondition(Avalon::DataRow& row);

public:
	UInt8   type;
	UInt8	strengthen;
	UInt32	sucRate;
	UInt8	failPunishment;
	UInt32	fix1;
	UInt32	fix1Max;
	UInt32	fix2;
	UInt32	fix2Max;
	UInt32	fix3Condition[ITEM_QUALITY_MAX-1];
	UInt32	fix3;
	UInt32	tickAddNum;
};
// 装备强化增幅概率表管理
class EquipStrRateDataMgr : public Avalon::DataEntryMgrBase<EquipStrRateData>
	, public Avalon::Singleton<EquipStrRateDataMgr>
{
	typedef std::map<UInt8, EquipStrRateData*> EquipStrRateMap;		// 以强化等级为Key

public:
	EquipStrRateDataMgr();
	~EquipStrRateDataMgr();

public:
	bool AddEntry(EquipStrRateData* dataEntry);
	EquipStrRateData* GetDatas(UInt8 strengthen);
	EquipStrRateData* GetEnhanceDatas(UInt8 enhance);
private:
	EquipStrRateMap			m_equipStrRateMap;
	EquipStrRateMap         m_equipEnhanceRateMap;
};

// 装备强化等级品质系数表
class EquipStrColorData : public Avalon::DataEntry
{
public:
	EquipStrColorData();
	~EquipStrColorData();

public:
	UInt32	GetKey() const { return level; }
	bool Read(Avalon::DataRow& row);

public:
	UInt32	level;
	UInt32	colorRatio[ITEM_QUALITY_MAX];
};

// 装备强化概率表管理
class EquipStrColorDataMgr : public Avalon::DataEntryMgrBase<EquipStrColorData>
	, public Avalon::Singleton<EquipStrColorDataMgr>
{

public:
	EquipStrColorDataMgr();
	~EquipStrColorDataMgr();

};

// 强化消耗
struct StrenthCost
{
	StrenthCost() {
		goldCost = 0;
		colorCost = 0;
		colorLessCost = 0;
	}

	UInt32	goldCost;
	UInt32	colorCost;
	UInt32	colorLessCost;
};



//强化返还
struct StrenthRet
{
	using ItemNumPair = std::pair<UInt32, UInt32>;

	StrenthRet() {
		goldRet = 0;
		colorRet = 0;
		colorLessRet = 0;
		strToTenDropRate = 0;
		strToTenNum = 0;
	}
	
	UInt32  goldRet;
	UInt32	colorRet;
	UInt32	colorLessRet;
	UInt32	strToTenDropRate;
	UInt32	strToTenNum;
	std::vector<ItemNumPair> ItemNumVec;
	UInt32 totalRate;
};

//装备强化表
class EquipStrengthenData : public Avalon::DataEntry
{
public:
	EquipStrengthenData();
	~EquipStrengthenData();

public:
	UInt32	GetKey() const { return strengthen; }
	bool Read(Avalon::DataRow& row);

	bool ReadMatNum(Avalon::DataRow& row, UInt32& colorMat, UInt32& colorLessMat);
public:
	UInt8		strengthen;
	UInt16		minLv;
	UInt16		maxLv;
	StrenthCost	cost[ITEM_QUALITY_MAX];
	StrenthRet ret[ITEM_QUALITY_MAX];
};
// 装备强化表管理
class EquipStrengthenDataMgr : public Avalon::DataEntryMgrBase<EquipStrengthenData>
	, public Avalon::Singleton<EquipStrengthenDataMgr>
{
	typedef std::map<UInt8, std::vector<EquipStrengthenData*>> EquipStrMap;		// 以强化等级为Key

public:
	EquipStrengthenDataMgr();
	~EquipStrengthenDataMgr();

public:
	bool AddEntry(EquipStrengthenData* dataEntry);
	void GetDatas(UInt8 strengthen, std::vector<EquipStrengthenData*>& datas);
private:
	EquipStrMap			m_equipStrMap;
};

//装备部位系数
struct EquipPtMod
{
	UInt32 phyMod;	//物理系数
	UInt32 magMod;	//法术系数
};

//装备强化系数表
class EquipStrModData : public Avalon::DataEntry
{
public:
	EquipStrModData();
	~EquipStrModData();

public:
	UInt32	GetKey() const { return m_id; }
	bool Read(Avalon::DataRow& row);

	bool ReadSplitStr(Avalon::DataRow& row, std::vector<UInt32>& vec);
	bool ReadEquipPtMods(Avalon::DataRow& row, std::vector<EquipPtMod>& mods);

	float GetWpPhyMod(UInt8 subType);		//得到武器类别物攻系数
	float GetWpMagMod(UInt8 subType);		//得到武器类别法攻系数
	float GetWpClQaMod(UInt8 color);		//得到武器品质QA系数
	float GetWpClQbMod(UInt8 color);		//得到武器品质QB系数
	float GetWpStrMod(UInt8 strLv);			//得到武器强化系数
	float GetArmStrMod(UInt8 strLv);		//得到防具强化系数
	float GetArmClQaMod(UInt8 color);		//得到防具品质QA系数
	float GetArmClQbMod(UInt8 color);		//得到防具品质QB系数
	float GetJewStrMod(UInt8 strLv);		//得到首饰强化系数
	float GetJewClQaMod(UInt8 color);		//得到首饰品质QA系数
	float GetJewClQbMod(UInt8 color);		//得到首饰品质QB系数
public:

	UInt32					m_id;
	std::vector<UInt32>		m_wpStrMods;		//武器强化系数
	std::vector<UInt32>		m_wpColorQaMods;	//武器品质系数Qa
	std::vector<UInt32>		m_wpColorQbMods;	//武器品质系数Qb
	std::vector<UInt32>		m_armStrMods;		//防具强化系数
	std::vector<UInt32>		m_armColorQaMods;	//防具品质系数Qa
	std::vector<UInt32>		m_armColorQbMods;	//防具品质系数Qb
	std::vector<UInt32>		m_jewStrMods;		//首饰强化系数
	std::vector<UInt32>		m_jewColorQaMods;	//首饰品质系数Qa
	std::vector<UInt32>		m_jewColorQbMods;	//首饰品质系数Qb
	std::vector<EquipPtMod>	m_equipPtMods;	//装备部位系数
};

// 装备强化系数表管理
class EquipStrModDataMgr : public Avalon::DataEntryMgrBase<EquipStrModData>
	, public Avalon::Singleton<EquipStrModDataMgr>
{
	typedef std::map<UInt32, EquipStrModData*> EquipStrModMap;

public:
	EquipStrModDataMgr();
	~EquipStrModDataMgr();

public:
	bool AddEntry(EquipStrModData* dataEntry);
	EquipStrModData* GetPVPStrMod();
	EquipStrModData* GetPVEStrMod();
	EquipStrModData* GetPVPDefPercentMod();
	EquipStrModData* GetPVEDefPercentMod();
	EquipStrModData* GetDatas(UInt32 id);

private:
	EquipStrModMap			m_equipStrModMap;
};

struct MagicCardProp
{
	UInt8	propType;
	UInt32	propValue;
};

struct MagicUpgradeMaterial
{
	MagicUpgradeMaterial()
	:color(0), quality(0), num(0){}
	MagicUpgradeMaterial(UInt8	color_, UInt8 quality_, UInt8 num_)
		:color(color_), quality(quality_), num(num_){}
	UInt8	color;
	UInt8   quality;
	UInt8	num;
};

struct SameCardIds
{
	UInt32 itemId;
	UInt32 comuseNum;
};

class EquipEnhanceDataKey
{
public:
	// 品质
	UInt8                                          quality;
	// 增幅等级
	UInt8                                          enhanceLevel;
	// 装备等级
	UInt8                                          level;
	bool operator<(const EquipEnhanceDataKey &rsh) const {
		if (quality < rsh.quality)
		{
			return true;
		}
		else if (quality == rsh.quality)
		{
			if (enhanceLevel < rsh.enhanceLevel)
			{
				return true;
			}
			else if (enhanceLevel == rsh.enhanceLevel)
			{
				if (level < rsh.level)
				{
					return true;
				}
			}
		}
		return false;
	}
};

struct EnhanceReturn
{
	UInt32 m_Rate;
	UInt32 m_Num;
};

class EquipEnhanceCostDataEntry : public Avalon::DataEntry
{
public:
	EquipEnhanceCostDataEntry(){};
	virtual ~EquipEnhanceCostDataEntry(){};

	UInt32 GetKey() const{ return 0; }
	bool Read(Avalon::DataRow &row);

public:
	EquipEnhanceDataKey                             key;
	// 金币消耗
	UInt32                                          needGold;
	// 金币返还
	UInt32                                          returnGold;
	// 消耗矛盾结晶体
	UInt32                                          itemID;
	// 消耗矛盾结晶体数量
	UInt32                                          itemNum;
	// 浓缩的纯洁之骇返还（道具ID）
	UInt32                                          returnItemID;
	// 浓缩的纯洁之骇返还
	std::string                                     returnItemRule;
	// 浓缩的纯洁之骇返还
	std::vector<EnhanceReturn>                      retrunItem;
	//返还总权重
	UInt32                                          returnTotalRate;
	// 浓缩的纯洁之骇返还（道具ID）
	UInt32                                          returnItemID2;
	// 强烈之痕迹返还
	std::string                                     returnItemRule2;
	// 强烈之痕迹返还
	std::vector<EnhanceReturn>                      retrunItem2;
	//返还总权重
	UInt32                                          returnTotalRate2;
};


class EquipEnhanceCostDataEntryMgr : public Avalon::DataEntryMgrBase<EquipEnhanceCostDataEntry>
	, public Avalon::Singleton<EquipEnhanceCostDataEntryMgr>
{
public:
	virtual bool AddEntry(EquipEnhanceCostDataEntry* entry) override;
	EquipEnhanceCostDataEntry* FindEntry(UInt8 level, UInt8 enhanceLevel, UInt8 quality);
	void ClearDataEntries();
private:
	std::map<EquipEnhanceDataKey, EquipEnhanceCostDataEntry*> m_EquipEnhanceCostMap;
};



class EquipEnhanceAttributeDataEntry : public Avalon::DataEntry
{
public:
	EquipEnhanceAttributeDataEntry(){};
	virtual ~EquipEnhanceAttributeDataEntry(){};

	UInt32 GetKey() const{ return 0; }
	bool Read(Avalon::DataRow &row);
public:
	EquipEnhanceDataKey                             key;
	// 增幅增加的属性值
	UInt32                                          enhanceAttribute;
	// 增幅增加的PVP属性值
	UInt32                                          enhanceAttributePVP;
	// 增幅等级评分
	UInt32											eqScore;
};

class EquipEnhanceAttributeDataEntryMgr : public Avalon::DataEntryMgrBase<EquipEnhanceAttributeDataEntry>
	, public Avalon::Singleton<EquipEnhanceAttributeDataEntryMgr>
{
public:
	virtual bool AddEntry(EquipEnhanceAttributeDataEntry* entry) override;
	EquipEnhanceAttributeDataEntry* FindEntry(UInt8 level, UInt8 enhanceLevel, UInt8 quality);
	void ClearDataEntries();
private:
	std::map<EquipEnhanceDataKey, EquipEnhanceAttributeDataEntry*> m_EquipEnhanceAttributeMap;
};

class MaterialsSynthesisDataEntry : public Avalon::DataEntry
{
public:
	using MaterialIDAndNum = std::pair<UInt32, UInt32>;

	MaterialsSynthesisDataEntry();
	virtual ~MaterialsSynthesisDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);


public:
	// 合成的道具ID
	UInt32                                          id;
	// 合成需要的材料
	std::string                                     composites;
	//材料
	std::vector<MaterialIDAndNum>                   Materials;
};

class MaterialsSynthesisDataEntryMgr : public Avalon::DataEntryMgrBase<MaterialsSynthesisDataEntry>
	, public Avalon::Singleton<MaterialsSynthesisDataEntryMgr>
{
public:
	virtual bool AddEntry(MaterialsSynthesisDataEntry* entry);

};

//附魔卡表
class MagicCardData : public Avalon::DataEntry
{
public:
	MagicCardData();

public:
	UInt32	GetKey() const { return id; }
	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 15; };

public:
	UInt32						id;
	UInt8						color;
	UInt8						quality;
	std::vector<UInt8>			parts;
	std::vector<MagicCardProp>	props;
	std::vector<UInt32>			buffs;
	UInt8						maxLevel;
	UInt32						upValue;
	UInt32						weight;
	UInt32						rate;
	UInt32						costId;
	UInt32						costNum;

	// 评分
	UInt32						score;
	UInt32						upgradeAddScore;
};
// 附魔卡管理
class MagicCardDataMgr : public Avalon::DataEntryMgrBase<MagicCardData>
	, public Avalon::Singleton<MagicCardDataMgr>
{

public:
	MagicCardDataMgr();
	~MagicCardDataMgr();

public:
	bool AddEntry(MagicCardData* dataEntry);
	bool GetDatasByColor(UInt8 color, std::vector<MagicCardData*>& datas);
	bool GetDatasByColorAndBind(UInt8 color, std::vector<MagicCardData*>& datas, bool isBind);
private:
	std::map<UInt8, std::vector<MagicCardData*>>	m_colorToDatas;
};

//附魔卡表升级表
class MagicCardUpdateData : public Avalon::DataEntry
{
public:
	MagicCardUpdateData();

public:
	UInt32	GetKey() const { return id; }
	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 14; };

public:
	UInt32						id;
	UInt32						dataId;
	UInt8						minLv;
	UInt8						maxLv;

	//升级可用副卡
	std::map<UInt8, std::vector<MagicUpgradeMaterial>>  upgradeMaterials;
	//指定品质基础成功率
	std::map<UInt8, std::map<UInt8, UInt32>> upgradeMaterialsRates;

	std::map<UInt8, UInt32>		levelAddRate;
	std::vector<SameCardIds>	sameCardIds;
	//UInt32						sameCardAddRate;
	std::map<UInt8, UInt32>		sameCardRates;
	ItemReward					upgradeConsume;
};

typedef Avalon::DataEntryVisitor<MagicCardUpdateData> MagicCardUpdateDataVisitor;

// 附魔卡升级表管理
class MagicCardUpdateDataMgr : public Avalon::DataEntryMgrBase<MagicCardUpdateData>
	, public Avalon::Singleton<MagicCardUpdateDataMgr>
{

public:
	MagicCardUpdateDataMgr();
	~MagicCardUpdateDataMgr();

public:
	MagicCardUpdateData* GetDataEntry(UInt32 cardId, UInt8 cardLv);
};

// 宝珠属性
struct PreciousBeadProp
{
	UInt8	propType;
	UInt32	propValue;
};

//宝珠
class PreciousBeadDataEntry : public Avalon::DataEntry
{
public:
	PreciousBeadDataEntry();
	~PreciousBeadDataEntry();

public:
	UInt32 GetKey() const { return itemId; }
	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 15; };

	bool IsCanReplace(UInt32 replaceId);

public:
	// 道具id
	UInt32 itemId;
	// 颜色
	UInt8 color;
	// 作用部位
	std::vector<UInt8>			parts;
	// 属性
	std::vector<PreciousBeadProp>	props;
	// buff表id
	std::vector<UInt32>			buffsPve;
	// buff表id
	std::vector<UInt32>			buffsPvp;
	// 等级
	UInt8		level;
	// 下级宝珠id
	UInt32		nextLevPrecbeadID;
	// 升级需要消耗
	std::vector<ItemReward> consumeItem;
	// 出现附加属性的几率
	UInt32		addtionBuffPro;
	// 随机属性组ID
	UInt32		buffRandomGroupId;
	// 可以置换的宝珠ID
	std::vector<UInt32> replaceVec;
	// 评分
	UInt32		score;
	//同名宝珠id
	std::vector<UInt32> sameBeadIDVec;
};

// 宝珠管理
class PreciousBeadDataEntryMgr : public Avalon::DataEntryMgrBase<PreciousBeadDataEntry>
	, public Avalon::Singleton<PreciousBeadDataEntryMgr>
{

public:
	PreciousBeadDataEntryMgr();
	~PreciousBeadDataEntryMgr();
};

struct JarBonusInfo
{
	void init(Avalon::DataRow& row){
		itemId = row.ReadUInt32();						
		num = row.ReadUInt32();
		wt = row.ReadUInt32();
		fix1 = row.ReadUInt32();
		fix2 = row.ReadUInt32();
		fix2Num = row.ReadUInt32();
		fixMax = row.ReadUInt32();
		fixMin = row.ReadUInt32();
		openCond = row.ReadUInt32();
		chargeCond = row.ReadUInt32();
		getNumLim = row.ReadUInt32();

		minNum = 0;
		maxNum = 0;
	}

	UInt32 itemId;						//道具id
	UInt32 num;							//道具数量
	UInt32 wt;							//道具权重
	UInt32 fix1;						//权重修正1
	UInt32 fix2;						//权重修正2
	UInt32 fix2Num;						//修正2次数
	UInt32 fixMax;						//修正最大值
	UInt32 fixMin;						//修正最小值
	UInt32 openCond;					//开箱次数条件
	UInt32 chargeCond;					//充值量条件
	UInt32 getNumLim;					//获得该道具的数量限制,0为无限

	UInt32 minNum;
	UInt32 maxNum;
};

//职业key
struct JarOccusKey
{
	JarOccusKey(std::string occus) : m_occus(occus){}

	JarOccusKey(UInt8 occu) {
		m_occus = Avalon::StringUtil::ConvertToString<UInt8>(occu);
	}

	std::string m_occus;
	
	bool operator < (const JarOccusKey& other) const
	{
		if (m_occus.find(other.m_occus) != std::string::npos &&
			other.m_occus.find(m_occus) == std::string::npos)
			return false;
		if (m_occus.find(other.m_occus) == std::string::npos &&
			other.m_occus.find(m_occus) != std::string::npos)
			return true;
		
		if (m_occus < other.m_occus)
			return true;
		if (other.m_occus < m_occus)
			return false;

		return false;
	}
};

//开罐类别&职业key
struct JarBonusKey
{
	JarBonusKey(UInt32 type, UInt8 occu) {
		m_type = type;
		m_occu = occu;
	}

	UInt32 m_type;
	UInt8 m_occu;

	bool operator < (const JarBonusKey& other) const
	{
		if (m_type < other.m_type)
			return true;
		if (other.m_type < m_type)
			return false;

		if (m_occu < other.m_occu)
			return true;
		if (other.m_occu < m_occu)
			return false;

		return false;
	}
};

//开罐类型&职业&道具id 组合key
struct JarBonusTrpKey
{
	JarBonusTrpKey(UInt32 type, std::string occus, UInt32 itemId) : m_type(type), m_occus(occus), m_itemId(itemId){}

	UInt32 m_type;
	std::string m_occus;
	UInt32 m_itemId;

	bool operator < (const JarBonusTrpKey& other) const
	{
		if (m_type < other.m_type)
			return true;
		if (other.m_type < m_type)
			return false;

		if (m_occus < other.m_occus)
			return true;
		if (other.m_occus < m_occus)
			return false;

		if (m_itemId < other.m_itemId)
			return true;
		if (other.m_itemId < m_itemId)
			return false;

		return false;
	}
};

struct JarCountBonus
{
	UInt32 count;
	UInt32 jarId;
};

//开罐子表
class JarBonusDataEntry : public Avalon::DataEntry
{
public:
	JarBonusDataEntry();
	~JarBonusDataEntry();
	
public:
	UInt32 GetKey() const{ return id; }
	bool Read(Avalon::DataRow& row);

public:
	UInt32						id;							
	std::string					name;						//罐子名字
	UInt32						type;						//类别
	std::vector<JarCountBonus>	countBonus;					//累计必得品质
	std::vector<JarBonusInfo>	getItems;					//购买得到的道具
	UInt32						moneyType;					//购买货币类别
	UInt32						moneyValue;					//购买货币值
	UInt32						dayBuyLimite;				//每日购买次数限制
	std::string					countDayLimite;				//每日购买次数限制计数key
	UInt8						needExItem;					//是否可用凭证
	UInt32						exItemId;					//凭证id
	UInt32						exItemNum;					//凭证数量
	UInt32						comboBuyNum;				//连抽次数
	UInt32						singleBuyDiscount;				//单抽折扣
	UInt32						comboBuyDiscount;			//连抽折扣
	std::string					counterKey;					//开罐子计数key
	UInt32						getPointId;					//开罐获得的积分id
	UInt32						getPointNum;				//开罐获得的积分数量
	std::vector<UInt32>			getPointCritRate;			//开罐获得积分暴击倍率
	std::vector<UInt32>			getPointCritTime;			//开罐获得积分暴击倍数
	UInt32						maxFreeNum;					//最大免费次数
	UInt32						freeCd;						//免费cd
	std::string					freeNumCounterKey;			//罐子下一次免费使用次数key
	std::string					nextFreeTimeCounterKey;		//罐子下一次使用免费时间key
	UInt8						needRecord;					//是否要记录
	UInt32						discountMaxNum;				//活动罐子折扣最大次数
	UInt8						discountReset;				//活动罐子折扣是否重置
	UInt32						actifactJarRewardNum;		//神器罐子派奖人数
	UInt32						jarGiftMustOutNum;			//罐子礼包必出物品次数
	std::string					curCycleIsOutTreasKey;		//本次循环周期内是否出过珍惜物品key
	UInt32						jarGiftOutJarId;			//罐子礼包累计必出罐子
	UInt32                      smellType;                  //产生气息装备的概率类型
};

// 开罐子表管理
class JarBonusDataMgr : public Avalon::DataEntryMgrBase<JarBonusDataEntry>
	, public Avalon::Singleton<JarBonusDataMgr>
{

public:
	JarBonusDataMgr();
	~JarBonusDataMgr();

public:
	bool AddEntry(JarBonusDataEntry* dataEntry);
};

//罐子道具强化随机权重
struct JarItemStrWt
{
	UInt8	strength;		//强化等级
	UInt32	weight;			//权重
};

//罐子奖池表
class JarItemPoolData : public Avalon::DataEntry
{
public:
	JarItemPoolData();
	~JarItemPoolData();

public:
	UInt32 GetKey() const{ return id; }
	bool Read(Avalon::DataRow& row);
	bool HasOccu(UInt8 occu);

public:
	UInt32			id;
	UInt32			type;						//罐子类别
	std::string		strOccus;
	std::vector<UInt8>		occus;						//职业
	UInt32			itemId;						//道具id
	UInt32			num;						//道具数量
	UInt32			wt;							//道具权重
	UInt32			fix1;						//权重修正1
	UInt32			fix2;						//权重修正2
	UInt32			fix2Num;					//修正2次数
	UInt32			fixMax;						//修正最大值
	UInt32			fixMin;						//修正最小值
	UInt32			openCond;					//开箱次数条件
	UInt32			chargeCond;					//充值量条件
	UInt32			getNumLim;					//获得该道具的数量限制,0为无限
	UInt32			needAnouce;					//是否公告&记录
	std::vector<JarItemStrWt>	strWts;			//强化权重
	UInt8           isTreasItem;				//是否真系道具  
	UInt32			isJarGiftNeedAnount;		//礼包奖励是否公告
	UInt32			checkLoad;					//是否加载时候检查
};

// 罐子奖池表管理
class JarItemPoolMgr : public Avalon::DataEntryMgrBase<JarItemPoolData>
	, public Avalon::Singleton<JarItemPoolMgr>
{

public:
	JarItemPoolMgr();
	~JarItemPoolMgr();

	typedef std::vector<JarItemPoolData*>		BonusVec;
	typedef std::map<JarBonusTrpKey, JarItemPoolData*>	PoolDataMap;			//组合key索引
	typedef std::map<UInt8, BonusVec>				PoolOccuMap;			//职业key索引

public:
	bool AddEntry(JarItemPoolData* dataEntry);
	JarItemPoolData* GetPoolData(JarBonusTrpKey key);
	bool GetOccuToDatas(UInt8 key, BonusVec& datas);
	void GetTypeAndOccuToDatas(UInt32 type, UInt8 occu, BonusVec& datas);

private:
	PoolDataMap			poolDatas;				//罐子类别&职业&itemid索引data
	PoolOccuMap			occuToDatas;			//职业索引的data
	std::vector<JarItemPoolData*> datas;		//所有datas
};

//buff药使用表
class BuffDrugData : Avalon::DataEntry
{
public:
	BuffDrugData();
	~BuffDrugData();

public:
	UInt32 GetKey() const { return itemId; }
	bool Read(Avalon::DataRow& row);

public:
	UInt32			itemId;
	std::string		name;
	UInt32			freeLevel;
};

// buff药管理
class BuffDrugDataMgr : public Avalon::DataEntryMgrBase<BuffDrugData>
	, public Avalon::Singleton<BuffDrugDataMgr>
{

public:
	BuffDrugDataMgr();
	~BuffDrugDataMgr();


};

/**
*@brief 强化券表
*/
class StrTicketData : Avalon::DataEntry
{
public:
	StrTicketData();
	~StrTicketData();

	UInt32 GetKey() const{ return id; }
	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 5; };

public:
	UInt32  id;				// id
	UInt32	rate;			// 强化成功概率
	UInt32	effectLv;		// 效果等级
	bool	canFuse;		// 可以融合
	UInt32  fuseValue;		// 融合价值
};

// 强化券索引道具data数量限制
#define STREN_TICKET_INDEX_ITEM_DATA_NUM_LIMIT 2

class StrTicketDataMgr : public Avalon::DataEntryMgrBase<StrTicketData>
	, public Avalon::Singleton<StrTicketDataMgr>
{
	struct StrenKey
	{
		StrenKey():level(0),rate(0) {}

		bool operator<(const StrenKey& rhs) const
		{
			if (level != rhs.level)
				return level < rhs.level;

			if (rate != rhs.rate)
				return rate < rhs.rate;

			return false;
		}

		UInt32 level;
		UInt32 rate;
	};

public:
	bool AddEntry(StrTicketData* dataEntry);

	// 根据等级和概率获取数据
	StrTicketData* GetFuseStrengthenTicketData(UInt32 level, UInt32 rate);

	// 根据价值排序
	void SortFuseStrengthenTicketsByFuseValue();

	// 访问可以融合的强化券
	void VisitFuseStrengthenTickets(const std::function<bool(const StrTicketData*)>& F);

	// 获取可以融合的强化群
	const std::vector<StrTicketData*>& GetFuseStrTicketDatas() { return m_FuseStrTicketVec; };

private:
	// 可以融合的强化券
	std::map<StrenKey, StrTicketData*> m_FuseStrTicketMap;
	std::vector<StrTicketData*> m_FuseStrTicketVec;
};


/**
*@brief 强化券表
*/
class OnceStrTicketData : Avalon::DataEntry
{
public:
	OnceStrTicketData();
	~OnceStrTicketData();

	UInt32 GetKey() const { return id; }
	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 4; };

public:
	UInt32  id;				// id
	UInt8	lvLimitBegin;	// 限制使用开始强化等级
	UInt8	lvLimitEnd;		// 限制使用结束强化等级
	UInt32	rate;			// 强化成功概率
};


class OnceStrTicketDataMgr : public Avalon::DataEntryMgrBase<OnceStrTicketData>
	, public Avalon::Singleton<OnceStrTicketDataMgr>
{

public:
	bool AddEntry(OnceStrTicketData* dataEntry);

};



class EnhanceTicketDataMgr : public Avalon::DataEntryMgrBase<StrTicketData>
	, public Avalon::Singleton<EnhanceTicketDataMgr>
{
	struct StrenKey
	{
		StrenKey() :level(0), rate(0) {}

		bool operator<(const StrenKey& rhs) const
		{
			if (level != rhs.level)
				return level < rhs.level;

			if (rate != rhs.rate)
				return rate < rhs.rate;

			return false;
		}

		UInt32 level;
		UInt32 rate;
	};

public:
	bool AddEntry(StrTicketData* dataEntry);

	// 根据等级和概率获取数据
	StrTicketData* GetFuseStrengthenTicketData(UInt32 level, UInt32 rate);

	// 根据价值排序
	void SortFuseStrengthenTicketsByFuseValue();

	// 访问可以融合的强化券
	void VisitFuseStrengthenTickets(const std::function<bool(const StrTicketData*)>& F);

	// 获取可以融合的强化群
	const std::vector<StrTicketData*>& GetFuseStrTicketDatas() { return m_FuseStrTicketVec; };

private:
	// 可以融合的强化券
	std::map<StrenKey, StrTicketData*> m_FuseStrTicketMap;
	std::vector<StrTicketData*> m_FuseStrTicketVec;
};

//时装合成普通产出
class FashionComposeData : public Avalon::DataEntry
{
public:
	FashionComposeData();
	~FashionComposeData();

public:
	UInt32 GetKey() const{ return id; }
	bool Read(Avalon::DataRow& row);

public:
	UInt32			id;							//时装ID
	UInt8			occu;						//基础职业
	UInt8			color;						//品质
	UInt8			part;						//部位
	UInt8			composeColor;				//合成所得品质
	UInt32			weight;						//权重
};

//基础职业&品质&部位 组合key
struct FashionComposeKey
{
	FashionComposeKey(UInt8 color, UInt8 occu, UInt8 part) : m_color(color), m_occu(occu), m_part(part){}

	UInt8 m_color;
	UInt8 m_occu;
	UInt8 m_part;

	bool operator < (const FashionComposeKey& other) const
	{
		if (m_color < other.m_color)
			return true;
		if (other.m_color < m_color)
			return false;

		if (m_occu < other.m_occu)
			return true;
		if (other.m_occu < m_occu)
		return false;

		if (m_part < other.m_part)
			return true;
		if (other.m_part < m_part)
			return false;

		return false;
	}
};

//基础职业&部位 组合key
struct FashionComposeSkyKey
{
	FashionComposeSkyKey(UInt8 occu, UInt8 part, UInt32 suitId) : m_occu(occu), m_part(part), m_suitId(suitId){}

	UInt8 m_occu;
	UInt8 m_part;
	UInt32 m_suitId;

	bool operator < (const FashionComposeSkyKey& other) const
	{
		if (m_occu < other.m_occu)
			return true;
		if (other.m_occu < m_occu)
			return false;

		if (m_part < other.m_part)
			return true;
		if (other.m_part < m_part)
		return false;

		if (m_suitId < other.m_suitId)
			return true;
		if (other.m_suitId < m_suitId)
			return false;

		return false;
	}
};


struct FashionComposeSkyKeyII
{
	FashionComposeSkyKeyII(UInt8 occu, UInt8 type, UInt32 suitId) : m_occu(occu), m_type(type), m_suitId(suitId){}

	UInt8 m_occu;
	UInt8 m_type;
	UInt32 m_suitId;

	bool operator < (const FashionComposeSkyKeyII& other) const
	{
		if (m_occu < other.m_occu)
			return true;
		if (other.m_occu < m_occu)
			return false;

		if (m_type < other.m_type)
			return true;
		if (other.m_type < m_type)
		return false;

		if (m_suitId < other.m_suitId)
			return true;
		if (other.m_suitId < m_suitId)
			return false;

		return false;
	}
};

// 时装合成普通表管理
class FashionComposeMgr : public Avalon::DataEntryMgrBase<FashionComposeData>
	, public Avalon::Singleton<FashionComposeMgr>
{

public:
	FashionComposeMgr();
	~FashionComposeMgr();

	typedef std::vector<FashionComposeData*>	FashionVec;
	typedef std::map<FashionComposeKey, FashionVec>	FashionDataMap;			//组合key索引

public:
	bool AddEntry(FashionComposeData* dataEntry);
	bool GetFahionDatas(FashionComposeKey key, FashionVec& datas);

	void GetFashions(UInt8 part, UInt8 occu, std::vector<UInt32>& fashions);

private:
	FashionDataMap			datasMap;				//基础职业&品质&部位索引data
};

//时装合成天空产出
class FashionComposeSkyData : public Avalon::DataEntry
{
public:
	FashionComposeSkyData();
	~FashionComposeSkyData();

	enum SkyType
	{
		ST_NONE = 0,
		ST_COMMON,
		ST_ADVANCE,
	};

public:
	UInt32 GetKey() const{ return id; }
	bool Read(Avalon::DataRow& row);

public:
	UInt32			id;							//天空时装ID
	UInt32			index;						//序号0开始
	UInt32			suitId;						//天空套套装ID
	UInt8			occu;						//基础职业
	UInt8			part;						//部位
	UInt8			type;						//类别
	UInt32			baseWeight;					//基础权重
	UInt32			fixRandRange[2];			//修正随机区间
	UInt32			hitMin;						//最低命中次数
	UInt32			maxWeight;					//最大权重
	UInt32			hitWeight;					//命中权重
	std::string		suitName;					//套装名字
};

struct SkyFashionSuitKey
{
	SkyFashionSuitKey(UInt32 suitId, UInt8 type) : m_suitId(suitId), m_type(type){}

	UInt32 m_suitId;
	UInt8 m_type;

	bool operator < (const SkyFashionSuitKey& other) const
	{
		if (m_type < other.m_type)
			return true;
		if (other.m_type < m_type)
			return false;

		if (m_suitId < other.m_suitId)
			return true;
		if (other.m_suitId < m_suitId)
			return false;

		return false;
	}
};

// 时装合成天空套表管理
class FashionComposeSkyMgr : public Avalon::DataEntryMgrBase<FashionComposeSkyData>
	, public Avalon::Singleton<FashionComposeSkyMgr>
{

public:
	FashionComposeSkyMgr();
	~FashionComposeSkyMgr();

	typedef std::vector<FashionComposeSkyData*>	FashionSkyVec;
	typedef std::map<FashionComposeSkyKey, FashionSkyVec>	FashionSkyDataMap;			//组合key索引
	typedef std::map<FashionComposeSkyKeyII, FashionSkyVec> FashionSkyDataMapII;

public:
	bool AddEntry(FashionComposeSkyData* dataEntry);
	bool GetFahionSkyDatas(FashionComposeSkyKey key, FashionSkyVec& datas);
	bool GetFashionSkyDatasII(FashionComposeSkyKeyII key, FashionSkyVec& datas);
	bool GetTransFashionSkyDatas(FashionComposeSkyKey key, FashionSkyVec& datas);

	void GetFashionSkysWithFilter(UInt32 suitId, UInt8 part, std::vector<UInt32>& filter, std::vector<UInt32>& fashionSkys);
	void GetFashionSkys(UInt32 suitId, UInt8 part, UInt8 occu, std::vector<FashionComposeSkyData*>& fashionSkys);
	void GetFashionIdxBySuitId(UInt32 suitId, std::vector<UInt32>& fashionIdx);
	UInt32 GetWingId(UInt32 suitId, UInt8 occu, UInt32 type);
	std::string GetSuitName(UInt32 suitId, UInt8 type);
private:
	FashionSkyDataMap			datasMap;				//基础职业&部位索引data
	FashionSkyDataMap			datasMap_Trans;			//透明套装
	FashionSkyDataMapII			datasMapII;				//职业,普通黄金类别,套装id 复合key
	std::map<SkyFashionSuitKey, std::string>	suitId2Name;
};

//装备打造
class EquipMakeData : public Avalon::DataEntry
{
public:
	EquipMakeData();
	~EquipMakeData();

	//材料信息
	struct MatInfo
	{
		UInt32 matId;
		UInt16 num;
	};

public:
	UInt32 GetKey() const{ return id; }
	bool Read(Avalon::DataRow& row);

public:
	UInt32					id;							//装备id
	std::vector<MatInfo>	needMats;					//需要材料
	UInt32					needMoney;					//需要货币id
	UInt32					needMoneyNum;				//需要货币数量
};

// 装备打造管理
class EquipMakeDataMgr : public Avalon::DataEntryMgrBase<EquipMakeData>
	, public Avalon::Singleton<EquipMakeDataMgr>
{

public:
	EquipMakeDataMgr();
	~EquipMakeDataMgr();

public:
	bool AddEntry(EquipMakeData* dataEntry);

};

//装备评分表
class EquipValueSceorData : public Avalon::DataEntry
{
public:
	EquipValueSceorData();
	~EquipValueSceorData();

	UInt32 GetKey() const{ return type; }
	bool Read(Avalon::DataRow& row);

public:
	UInt32		type;
	UInt32		value;
};

// 装备评分表管理
class EquipValueSceorDataMgr : public Avalon::DataEntryMgrBase<EquipValueSceorData>
	, public Avalon::Singleton<EquipValueSceorDataMgr>
{

public:
	EquipValueSceorDataMgr();
	~EquipValueSceorDataMgr();


};

//时装选择属性配置表
class FashionAttrData : public Avalon::DataEntry
{
public:
	FashionAttrData();
	~FashionAttrData();

	UInt32 GetKey()	const { return id; }
	bool Read(Avalon::DataRow& row);

public:
	UInt32 id;
	UInt32 defaultAttr1; //(光环用)
	std::vector<UInt32>	attrs;
	UInt32 defaultAttr;
	UInt32 freeSelNum;
};

//时装选择属性配置表管理
class FashionAttrDataMgr : public Avalon::DataEntryMgrBase<FashionAttrData>
	, public Avalon::Singleton<FashionAttrDataMgr>
{
public:
	FashionAttrDataMgr();
	~FashionAttrDataMgr();

	bool AddEntry(FashionAttrData* dataEntry);
};

//礼包配置
class GiftPackData : public Avalon::DataEntry
{
public:
	GiftPackData();
	~GiftPackData();

	UInt32 GetKey() const { return giftPackId; }
	bool Read(Avalon::DataRow& row);

	UInt32	giftPackId;
	UInt32	filterType;
	Int32	filterCount;
	std::vector<UInt32>	giftItems;
	// ui类型
	UInt8   uiType;
	// 礼包描述(废弃)
	std::string description;
};

//礼包配置管理器
class GiftPackDataMgr : public Avalon::DataEntryMgrBase<GiftPackData>
	, public Avalon::Singleton<GiftPackDataMgr>
{
public:
	GiftPackDataMgr();
	~GiftPackDataMgr();
};

//礼品配置
class GiftItemData : public Avalon::DataEntry
{
public:
	GiftItemData();
	~GiftItemData();

	UInt32 GetKey() const { return giftId; }
	bool Read(Avalon::DataRow& row);

	UInt32	giftId;
	UInt32	itemId;
	UInt32  giftPackId;
	UInt32	num;
	std::vector<UInt8>	occus;
	UInt32	weight;
	std::vector<UInt32>  levels;
	UInt8	equipType;
	UInt32	strengthen;
};

//礼品配置管理器
class GiftItemDataMgr : public Avalon::DataEntryMgrBase<GiftItemData>
	, public Avalon::Singleton<GiftItemDataMgr>
{
public:
	GiftItemDataMgr();
	~GiftItemDataMgr();

	void MergeToGiftPack();
};

//时装合成系数
class FashionComposeRate : public Avalon::DataEntry
{
public:
	FashionComposeRate();
	~FashionComposeRate();

	UInt32 GetKey() const { return type; }
	bool Read(Avalon::DataRow& row);

	UInt32 type;		//时装类别(ItemThirdType)
	UInt32	rate;			//系数 * 100
};

class FashionComposeRateMgr : public Avalon::DataEntryMgrBase<FashionComposeRate>
	, public Avalon::Singleton<FashionComposeRateMgr>
{
public:
	FashionComposeRateMgr();
	~FashionComposeRateMgr();
};

//时装部位合成次数概率
class FashionCompPartRate : public Avalon::DataEntry
{
public:
	FashionCompPartRate();
	~FashionCompPartRate();

	UInt32 GetKey() const { return composeNum; }
	bool Read(Avalon::DataRow& row);

	UInt32 composeNum;		// 同部位合成次数
	UInt32 noneSkyRate;			// 上一次没有合到天空套概率 * 100
	UInt32 normalSkyRate;		// 上一次合到普通天空套概率 * 100
	UInt32 goldSkyRate;			// 上一次合到黄金天空套概率 * 100
};

class FashionCompPartRateMgr : public Avalon::DataEntryMgrBase<FashionCompPartRate>
	, public Avalon::Singleton<FashionCompPartRateMgr>
{
public:
	FashionCompPartRateMgr();
	~FashionCompPartRateMgr();

	bool AddEntry(FashionCompPartRate* dataEntry);

	UInt32 GetMaxNum();

	UInt32 m_maxNum;
};

//宝珠摘除表
class PreciousBeadExtirpeDataEntry : public Avalon::DataEntry
{
public:
	PreciousBeadExtirpeDataEntry(){};
	~PreciousBeadExtirpeDataEntry(){};

public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	//序号id
	UInt32 id;
	//品质
	UInt32 color;
	//等级
	UInt32 level;
	//需要消耗材料id
	UInt32 materialId;
	//需要消耗材料数量
	UInt32 materialNum;
	//摘除成功率
	UInt32 rate;
	//宝珠类型
	UInt8  type;
	//可摘取次数
	Int32  pickNum;
};

typedef Avalon::DataEntryVisitor<PreciousBeadExtirpeDataEntry> PreciousBeadExtirpeDataEntryVisitor;

class PreciousBeadExtirpeDataEntryMgr : public Avalon::DataEntryMgrBase<PreciousBeadExtirpeDataEntry>
	, public Avalon::Singleton<PreciousBeadExtirpeDataEntryMgr >
{
public:
	PreciousBeadExtirpeDataEntry* GetDataEntry(UInt32 color, UInt32 level, UInt32 materialId, UInt8 type);
};

//宝珠置换表
class PreciousBeadReplaceDataEntry : public Avalon::DataEntry
{
public:
	PreciousBeadReplaceDataEntry(){};
	~PreciousBeadReplaceDataEntry(){};

public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	//序号id
	UInt32 id;
	//品质
	UInt32 color;
	//等级
	UInt32 level;
	//宝珠类型
	UInt8  type;
	//需要消耗itemid
	UInt32 costItemId;
	//需要消耗item数量
	UInt32 costItemNum;
	//可置换次数
	Int32  replaceNum;
};

typedef Avalon::DataEntryVisitor<PreciousBeadReplaceDataEntry> PreciousBeadReplaceDataEntryVisitor;

class PreciousBeadReplaceDataEntryMgr : public Avalon::DataEntryMgrBase<PreciousBeadReplaceDataEntry>
	, public Avalon::Singleton<PreciousBeadReplaceDataEntryMgr >
{
public:
	PreciousBeadReplaceDataEntry* GetDataEntry(UInt32 color, UInt32 leve, UInt8 type);
};

//宝珠升级附加属性随机表
class PrecBeadAddBuffRandomDataEntry : public Avalon::DataEntry
{
public:
	PrecBeadAddBuffRandomDataEntry() {};
	~PrecBeadAddBuffRandomDataEntry(){};
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	//序号id
	UInt32	id;
	//buff组id
	UInt32	buffGroupId;
	//权重
	UInt32	weight;
	//buffinfo id
	UInt32  buffInfoId;
};

typedef Avalon::DataEntryVisitor<PrecBeadAddBuffRandomDataEntry> PrecBeadAddBuffRandomDataEntryVisitor;

class PrecBeadAddBuffRandomDataEntryMgr : public Avalon::DataEntryMgrBase<PrecBeadAddBuffRandomDataEntry>
	, public Avalon::Singleton<PrecBeadAddBuffRandomDataEntryMgr>
{
public:
	typedef std::vector<PrecBeadAddBuffRandomDataEntry*> DataEntryVector;
	typedef std::map<UInt32, DataEntryVector> DataEntryMap;
	bool AddEntry(PrecBeadAddBuffRandomDataEntry* dataEntry);
	UInt32 RandomBuffIdByBuffGroupId(UInt32 buffGroupId);
public:
	DataEntryMap dataEntrysMap;
};

//辅助装备强化四维属性加成数值表

class AssistEqStrengFouerDimAddDataEntry : public Avalon::DataEntry
{
public:
	AssistEqStrengFouerDimAddDataEntry() {};
	~AssistEqStrengFouerDimAddDataEntry(){};
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	//序号id
	UInt32	id;
	//强化等级
	UInt8	strengthen;
	//装备等级
	UInt16	equipLv;
	//装备品质1
	UInt8  equipColor1;
	//装备品质2
	UInt8  equipColor2;
	//四维属性增加值PVE
	UInt32 addValue;
	//四维属性增加值PVP
	UInt32 addValuePvp;
	//强化装备评分
	UInt32	eqScore;
};

struct AssistEqStrengKey
{
	AssistEqStrengKey(UInt8	strengthen_, UInt16	equipLv_, UInt8	equipColor1_, UInt8 equipColor2_)
	: strengthen(strengthen_), equipLv(equipLv_), equipColor1(equipColor1_), equipColor2(equipColor2_)
	{}

	bool operator<(const AssistEqStrengKey& other) const
	{
		if (other.strengthen != strengthen)
		{
			return strengthen < other.strengthen;
		}
		if (other.equipLv != equipLv)
		{
			return equipLv < other.equipLv;
		}
		if (other.equipColor1 != equipColor1)
		{
			return equipColor1 < other.equipColor1;
		}
		return equipColor2 < other.equipColor2;
	}

	//强化等级
	UInt8	strengthen;
	//装备等级
	UInt16	equipLv;
	//装备品质1
	UInt8  equipColor1;
	//装备品质2
	UInt8  equipColor2;
};

class AssistEqStrengFouerDimAddDataEntryMgr : public Avalon::DataEntryMgrBase<AssistEqStrengFouerDimAddDataEntry>
	, public Avalon::Singleton<AssistEqStrengFouerDimAddDataEntryMgr>
{
public:
	typedef std::map<AssistEqStrengKey, AssistEqStrengFouerDimAddDataEntry*> DataEntryMap;
	bool AddEntry(AssistEqStrengFouerDimAddDataEntry* dataEntry);
	UInt32	GetAddValue(UInt8	strengthen, UInt16	equipLv, UInt8	equipColor1, UInt8 equipColor2);
	UInt32	GetPvpAddValue(UInt8	strengthen, UInt16	equipLv, UInt8	equipColor1, UInt8 equipColor2);
	UInt32  GetEqScore(UInt8	strengthen, UInt16	equipLv, UInt8	equipColor1, UInt8 equipColor2);
public:
	DataEntryMap	dataEntrysMap;
};





class EnhanceOverLoadDataEntry : public Avalon::DataEntry
{
public:
	EnhanceOverLoadDataEntry() {};
	~EnhanceOverLoadDataEntry() {};
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	UInt32 id;
	//道具ID
	UInt32	itemID;
	//增幅等级
	UInt32	enhanceLv;
	//权重
	UInt32 raito;
};

class EnhanceOverLoadDataEntryMgr : public Avalon::DataEntryMgrBase<EnhanceOverLoadDataEntry>
	, public Avalon::Singleton<EnhanceOverLoadDataEntryMgr>
{
public:
	bool AddEntry(EnhanceOverLoadDataEntry* dataEntry);
	UInt32	GetEnhanceLevel(UInt32 itemID);
public:
	std::map<UInt32, std::vector<std::pair<UInt32,UInt32>>>	dataMap;
};


/**
*@brief 装备固定攻击力强化系数表
*/
class EquipIndpendatkStrengModDataEntry : public Avalon::DataEntry
{
public:
	EquipIndpendatkStrengModDataEntry() {};
	~EquipIndpendatkStrengModDataEntry() {};
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	//id
	UInt32	id;
	//武器强化系数
	std::vector<UInt32>	strengRates;
	//武器品质系数A
	std::vector<UInt32> colorRateA;
	//武器品质系数B
	std::vector<UInt32> colorRateB;
	//部位系数固定攻击
	UInt32	partRateAtk;
};

class EquipIndpendatkStrengModDataEntryMgr : public Avalon::DataEntryMgrBase<EquipIndpendatkStrengModDataEntry>
	, public Avalon::Singleton<EquipIndpendatkStrengModDataEntryMgr>
{
public:
	static const UInt32  PVEID = 1;
	static const UInt32  PVPID = 2;
public:
	UInt32 GetStrengRate(UInt32 id, UInt32 strengLv);

	UInt32 GetColorARate(UInt32 id, UInt32 colorA);

	UInt32 GetColorBRate(UInt32 id, UInt32 colorB);

	UInt32 GetPartRate(UInt32 id);
};

#endif
