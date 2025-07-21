#ifndef _CL_ITEM_DATAENTRY_H_
#define _CL_ITEM_DATAENTRY_H_

#include <functional>
#include "CLItemDefine.h"
#include "CLGameDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>
#include <bitset>


/**
 *@brief Item������
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
	//����id
	UInt32		id;
	//��������
	std::string	name;
	//����
	UInt8		type;
	//������
	UInt32		subType;
	//��������
	UInt32		thirdType;
	//ְҵ
	std::bitset<Occupation::OCCU_MAX>		occu;
	//ְҵɸѡ
	std::set<Int8>		occuFilter;
	//UInt8		occu;
	//Ʒ����ɫ
	UInt8	color;
	//Ʒ����ɫ2
	UInt8   color2;
	//��Ҫ�ȼ�
	UInt16		needLevel;
	//���ȼ�
	UInt16		maxLevel;
	//��������(ǧ�ֱ�)
	Int32		baseAtkSpeed;
	//�ܷ�ʹ��
	UInt8		canUse;
	//�ܷ���
	UInt8		canTrade;
	//���� 1.�ǰ� 2.��ɫ�� 3.�˺Ű�
	UInt8		owner;
	//�Ƿ��װ
	UInt8		seal;
	//��װ����
	UInt32		sealMax;
	//�Ƿ�ɷֽ�
	UInt8		canDecompose;
	//���۳ɵ���ƷID
	UInt32		sellItemId;
	//�ۼ�
	UInt32		price;
	//CD��
	UInt8		cdGroup;
	//CDʱ��
	UInt32		cdTime;
	//ʱЧ��
	UInt32		time;
	//������
	UInt16		maxNum;
	//ʹ�ø���Buff
	UInt32		onUseBuff;
	//��ø���buff
	UInt32		onGetBuff;
	//�ܷ񵥾�ʹ��
	UInt32		canDungeonUse;
	//�ܷ�pkʹ��
	UInt8		canPkUse;
    //�Ƽ��۸�
    UInt32      recommendPrice;
	//��װID
	UInt32		suitId;
	//����ID
	UInt32		attrId;
	//����buffID
	std::vector<UInt32>		mutexBuffIDs;
	//�Ƿ񹫸�
	UInt8		canAnnounce;
	//ʹ���������
	UInt32		useLimiteType;
	//ʹ������ֵ
	UInt32		useLimiteValue;
	//�Ƿ����
	UInt8		isAbandon;
	//��������ID
	UInt32		giftId;
	//�Ƿ��ֹ�����и���
	bool		isForbidAuctionCopy;
	//����������Ϣ
	std::vector<ItemRenewInfo>	itemRenewInfos;
	// �����м۸�����
	UInt32		auctionMinPriceRate;
	// �����м۸�����
	UInt32		auctionMaxPriceRate;
	// �ܷ�ʦͽ����
	UInt8		canMasterGive;
	// �����������
	UInt32		getLimitNum;
	// �������ĵ��ߵ�ħ�޹�������ĵ���id
	UInt32 jarGiftConsumeItemId;
	// �������ĵ��ߵ�ħ�޹�������ĵ�������
	UInt32 jarGiftConsumeItemNum;
	// �Ƿ�͸��ʱװ
	UInt8		isTransparentFashion;
	// ������Ƕ��1����Ȩ��
	std::vector<ProWeightItem> PreciousBeadHole1;
	// ������Ƕ��2����Ȩ��
	std::vector<ProWeightItem> PreciousBeadHole2;
	// ǿ��ȯ��ֵ������
	UInt32 strenTicketDataIndex;
	// ����ȼ�
	UInt8 precBeadLev;
	// ��������
	UInt8 precBeadType;
	// �Ƿ���Ʒ
	UInt8 isTreasure;
	// �����н�����ȴʱ��-����
	UInt32 auctionTransCoolTimeMl;
	// �����н�����ȴʱ��-����
	UInt32 auctionTransCoolTimeOther;
	// ǿ��ȯװ���ȼ�������Сֵ
	UInt32 strTicketLvLimitMin;	
	// ǿ��ȯװ���ȼ��������ֵ
	UInt32 strTicketLvLimitMax;
	// �Ƿ��¼��־
	bool isRecordLog;
	// �ۿ�����ۿ�ȯ�ۿ۱�
	UInt32 discountGiftCouponPro;
	// ͷ��id
	UInt32 newTitleId;
	// ����ʱ��,ʱ���
	UInt32 expireTime;
	// �����пɽ��״���
	UInt32 auctionTransNum;
	// ����ִ�в���
	std::bitset<32> prohibitOps;
	// ʹ�ô�������
	UInt32	useLinmit;
	// �Ƿ����⸽ħ��
	UInt8   isSpeicMagic;
	// �������͵������͵���
	std::vector<ItemReward> presentedItems;
	// ���Ӳ���
	std::string params;
	// ���͵��߱���������
	UInt32	sendedLimit;
	// ���͵�����������
	UInt32	sendLimit;
	// ���͵��߱�����������
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

	// ����ǿ��ȯ������ȡ����data
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
	//װ��
	//LevelEquipMap			m_Equips[EQUIP_TYPE_WEAPON_MAX][5];
	//std::map<UInt16, std::vector<UInt32> >	m_mapEquips[5];

	//����-������
	//std::map<UInt32,UInt32> m_PetEggMap;

	//��¼ʹ�ô����ĵ���
	std::vector<UInt32> m_useCountItem;

	//�ű�����
	std::vector<UInt32> m_TeamCopyItem;

	//ǿ��ȯ����
	std::map<UInt32, ItemDataEntryVec> m_StrenTicketMap;

	//SendUdpVec
	ItemDataEntryVec m_SendUdpItems;

	//������е���id����
	std::vector<UInt32> m_flyHellGiftItem;

	//��ħ������,��Ʒ��
	std::map<UInt8, std::vector<UInt32>> m_magicIdsByQuality;

	//��¼��ʹ�ô���(����ˢ��)�ĵ���
	std::vector<UInt32> m_useCountItemWeek6;
};

/**
*@brief װ�����Ա�
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
	//����id
	UInt32 id;			
	//������
	Int32		atk;
	//��������
	Int32		magicAtk;
	//����������
	Int32		independAtk;
	//�������
	Int32		def;
	//��������
	Int32		magicDef;
	//����
	Int32		strenth;
	//����
	Int32		intellect;
	//����
	Int32		spirit;
	//����
	Int32		stamina;
	//�����ܺ����仯�ٷֱ�
	Int8		phySkillMp;
	//������cd�仯
	Int32		phySkillCd;
	//ħ�����ܺ����仯�ٷֱ�
	Int8		magSkillMp;
	//ħ������cd�仯
	Int32		magSkillCd;
	//HP���ֵ
	Int32		HPMax;
	//MP���ֵ
	Int32		MPMax;
	//HP�ָ�
	Int32		HPRecover;
	//MP�ָ�
	Int32		MPRecover;
	//����(ǧ�ֱ�)
	Int32		AtkSpeedRate;
	//�ͷ��ٶ�(ǧ�ֱ�)
	Int32		FireSpeedRate;
	//�����ƶ��ٶ�(ǧ�ֱ�)
	Int32		BattleSpeedRate;
	//�����ƶ��ٶ�(ǧ�ֱ�)
	Int16		TownSpeedRate;
	//������(ǧ�ֱ�)
	Int32		HitRate;
	//�ر���(ǧ�ֱ�)
	Int32		AvoidRate;
	//������(ǧ�ֱ�)
	Int32		PhysicCrit;
	//ħ������(ǧ�ֱ�)
	Int32		MagicCrit;
	//Ӳֱ
	Int32		Spasticity;
	//������������
	Int32		AtkPropEx[EEP_MAX];
	//ǿ����������
	Int32		StrPropEx[EEP_MAX];
	//���Ը�������
	Int32		DefPropEx[EEP_MAX];
	//�쳣������
	Int32		AbnormalResistsTotal;
	//�쳣����
	Int32		AbnormalResists[EAR_MAX];
	//���ܷ�
	Int32		skillScore;
	//����buff info id
	std::vector<UInt32> attchBuffInfoIds;
	//��Ԩʷʫ������ʼӳ�
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
*@brief Ʒ��������
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
	UInt8					color;			// Ʒ��
	UInt32					count;			// ��������
	UInt32					randRangeMin;	// ������Χmin(�ٷֱ�)
	UInt32					randRangeMax;	// ������Χmax(�ٷֱ�)
	UInt32					surpriseRate;	// ��ϲ����(ǧ�ֱ�)
	UInt32					surpriseRangeMin;	//��ϲ��Χmin(�ٷֱ�)
	UInt32					surpriseRangeMax;	//��ϲ��Χmax(�ٷֱ�)
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
*@brief Ʒ����ֵ��
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
	UInt32					part;				// ��λ
	float					atkDef;				// ����ϵ��
	float					fourDimensional;	// ��άϵ��
	float					perfectAtkDef;		// ��������ϵ��(�ٷֱ�)
	float					perfectfourDimensional;	// ��ϲ����(ǧ�ֱ�)
	float					strProp;			// ����ǿ��ϵ��
	float					perfectStrProp;		// ��������ǿ��ϵ��
	float					defProp;			// ���Կ���ϵ��
	float					perfectDefProp;		// �������Կ���ϵ��
	float					adnormalResists;	// �쳣����ϵ��
	float					perfectAbnormalResists;	// �����쳣����ϵ��
	float					independAtkProp; ;	// ����������ϵ��
	float					perfectIndpendAtkProp; // ��������������ϵ��
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
*@brief �������������ֵ��
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
	
	UInt8					baseRatio;		// ����ϵ��
	std::vector<UInt32>		QLPs;			// Ʒ������
	std::vector<UInt32>		QLOMin;			// Ʒ��������Сֵ
	std::vector<UInt32>		QLOMax;			// Ʒ���������ֵ
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
*@brief ������Ա�
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
	//����������
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
*@brief ���������Ŀ��
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
	//����������
	ItemQuality				color;
	std::vector<UInt32>		nums;
	std::vector<UInt32>		weights;
};
typedef Avalon::DataEntryVisitor<RandAttrNumDataEntry> RandAttrNumDataEntryVisitor;
typedef Avalon::DataEntryMgr<RandAttrNumDataEntry> RandAttrNumDataEntryMgr;

/**
*@brief ���������ֵ��
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
	//����������
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
	typedef std::map<UInt32, std::vector<RandAttrValueDataEntry*>> RandAttrValMap;		// ���������ΪKey

public:
	RandAttrValueDataEntryMgr();
	~RandAttrValueDataEntryMgr();

public:
	bool AddEntry(RandAttrValueDataEntry* dataEntry);
	void GetDatas(UInt32 type, std::vector<RandAttrValueDataEntry*>& datas);
private:
	RandAttrValMap			m_randAttrs;
};



// �ֽ���ϱ�
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
	void ReadDecMat(Avalon::DataRow& row, UInt8 matType);		// matType:1.��ɫ 2.��ɫ 3.����֮��
	void ReadRateMat(Avalon::DataRow& row, UInt8 matType);      // matType:1.��� 2.���� 3.��Ϣ
public:
	//����������
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
// �ֽ�data����
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
	typedef std::map<QualityKey, std::vector<DeComposeDataEntry*>, QualityKey> DeComposeDataMap;		// ��Ʒ��ΪKey

	//typedef std::map<UInt8, std::vector<DeComposeDataEntry*>> DeComposeDataMap;		// ��Ʒ��ΪKey

public:
	DeComposeDataEntryMgr();
	~DeComposeDataEntryMgr();

public:
	bool AddEntry(DeComposeDataEntry* dataEntry);
	void GetDatas(UInt8 quality, UInt8 color2, std::vector<DeComposeDataEntry*>& datas);
private:
	DeComposeDataMap			m_decomposeDatas;
};

//װ��ǿ�����ʱ�
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
// װ��ǿ���������ʱ����
class EquipStrRateDataMgr : public Avalon::DataEntryMgrBase<EquipStrRateData>
	, public Avalon::Singleton<EquipStrRateDataMgr>
{
	typedef std::map<UInt8, EquipStrRateData*> EquipStrRateMap;		// ��ǿ���ȼ�ΪKey

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

// װ��ǿ���ȼ�Ʒ��ϵ����
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

// װ��ǿ�����ʱ����
class EquipStrColorDataMgr : public Avalon::DataEntryMgrBase<EquipStrColorData>
	, public Avalon::Singleton<EquipStrColorDataMgr>
{

public:
	EquipStrColorDataMgr();
	~EquipStrColorDataMgr();

};

// ǿ������
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



//ǿ������
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

//װ��ǿ����
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
// װ��ǿ�������
class EquipStrengthenDataMgr : public Avalon::DataEntryMgrBase<EquipStrengthenData>
	, public Avalon::Singleton<EquipStrengthenDataMgr>
{
	typedef std::map<UInt8, std::vector<EquipStrengthenData*>> EquipStrMap;		// ��ǿ���ȼ�ΪKey

public:
	EquipStrengthenDataMgr();
	~EquipStrengthenDataMgr();

public:
	bool AddEntry(EquipStrengthenData* dataEntry);
	void GetDatas(UInt8 strengthen, std::vector<EquipStrengthenData*>& datas);
private:
	EquipStrMap			m_equipStrMap;
};

//װ����λϵ��
struct EquipPtMod
{
	UInt32 phyMod;	//����ϵ��
	UInt32 magMod;	//����ϵ��
};

//װ��ǿ��ϵ����
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

	float GetWpPhyMod(UInt8 subType);		//�õ���������﹥ϵ��
	float GetWpMagMod(UInt8 subType);		//�õ�������𷨹�ϵ��
	float GetWpClQaMod(UInt8 color);		//�õ�����Ʒ��QAϵ��
	float GetWpClQbMod(UInt8 color);		//�õ�����Ʒ��QBϵ��
	float GetWpStrMod(UInt8 strLv);			//�õ�����ǿ��ϵ��
	float GetArmStrMod(UInt8 strLv);		//�õ�����ǿ��ϵ��
	float GetArmClQaMod(UInt8 color);		//�õ�����Ʒ��QAϵ��
	float GetArmClQbMod(UInt8 color);		//�õ�����Ʒ��QBϵ��
	float GetJewStrMod(UInt8 strLv);		//�õ�����ǿ��ϵ��
	float GetJewClQaMod(UInt8 color);		//�õ�����Ʒ��QAϵ��
	float GetJewClQbMod(UInt8 color);		//�õ�����Ʒ��QBϵ��
public:

	UInt32					m_id;
	std::vector<UInt32>		m_wpStrMods;		//����ǿ��ϵ��
	std::vector<UInt32>		m_wpColorQaMods;	//����Ʒ��ϵ��Qa
	std::vector<UInt32>		m_wpColorQbMods;	//����Ʒ��ϵ��Qb
	std::vector<UInt32>		m_armStrMods;		//����ǿ��ϵ��
	std::vector<UInt32>		m_armColorQaMods;	//����Ʒ��ϵ��Qa
	std::vector<UInt32>		m_armColorQbMods;	//����Ʒ��ϵ��Qb
	std::vector<UInt32>		m_jewStrMods;		//����ǿ��ϵ��
	std::vector<UInt32>		m_jewColorQaMods;	//����Ʒ��ϵ��Qa
	std::vector<UInt32>		m_jewColorQbMods;	//����Ʒ��ϵ��Qb
	std::vector<EquipPtMod>	m_equipPtMods;	//װ����λϵ��
};

// װ��ǿ��ϵ�������
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
	// Ʒ��
	UInt8                                          quality;
	// �����ȼ�
	UInt8                                          enhanceLevel;
	// װ���ȼ�
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
	// �������
	UInt32                                          needGold;
	// ��ҷ���
	UInt32                                          returnGold;
	// ����ì�ܽᾧ��
	UInt32                                          itemID;
	// ����ì�ܽᾧ������
	UInt32                                          itemNum;
	// Ũ���Ĵ���֮������������ID��
	UInt32                                          returnItemID;
	// Ũ���Ĵ���֮������
	std::string                                     returnItemRule;
	// Ũ���Ĵ���֮������
	std::vector<EnhanceReturn>                      retrunItem;
	//������Ȩ��
	UInt32                                          returnTotalRate;
	// Ũ���Ĵ���֮������������ID��
	UInt32                                          returnItemID2;
	// ǿ��֮�ۼ�����
	std::string                                     returnItemRule2;
	// ǿ��֮�ۼ�����
	std::vector<EnhanceReturn>                      retrunItem2;
	//������Ȩ��
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
	// �������ӵ�����ֵ
	UInt32                                          enhanceAttribute;
	// �������ӵ�PVP����ֵ
	UInt32                                          enhanceAttributePVP;
	// �����ȼ�����
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
	// �ϳɵĵ���ID
	UInt32                                          id;
	// �ϳ���Ҫ�Ĳ���
	std::string                                     composites;
	//����
	std::vector<MaterialIDAndNum>                   Materials;
};

class MaterialsSynthesisDataEntryMgr : public Avalon::DataEntryMgrBase<MaterialsSynthesisDataEntry>
	, public Avalon::Singleton<MaterialsSynthesisDataEntryMgr>
{
public:
	virtual bool AddEntry(MaterialsSynthesisDataEntry* entry);

};

//��ħ����
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

	// ����
	UInt32						score;
	UInt32						upgradeAddScore;
};
// ��ħ������
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

//��ħ����������
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

	//�������ø���
	std::map<UInt8, std::vector<MagicUpgradeMaterial>>  upgradeMaterials;
	//ָ��Ʒ�ʻ����ɹ���
	std::map<UInt8, std::map<UInt8, UInt32>> upgradeMaterialsRates;

	std::map<UInt8, UInt32>		levelAddRate;
	std::vector<SameCardIds>	sameCardIds;
	//UInt32						sameCardAddRate;
	std::map<UInt8, UInt32>		sameCardRates;
	ItemReward					upgradeConsume;
};

typedef Avalon::DataEntryVisitor<MagicCardUpdateData> MagicCardUpdateDataVisitor;

// ��ħ�����������
class MagicCardUpdateDataMgr : public Avalon::DataEntryMgrBase<MagicCardUpdateData>
	, public Avalon::Singleton<MagicCardUpdateDataMgr>
{

public:
	MagicCardUpdateDataMgr();
	~MagicCardUpdateDataMgr();

public:
	MagicCardUpdateData* GetDataEntry(UInt32 cardId, UInt8 cardLv);
};

// ��������
struct PreciousBeadProp
{
	UInt8	propType;
	UInt32	propValue;
};

//����
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
	// ����id
	UInt32 itemId;
	// ��ɫ
	UInt8 color;
	// ���ò�λ
	std::vector<UInt8>			parts;
	// ����
	std::vector<PreciousBeadProp>	props;
	// buff��id
	std::vector<UInt32>			buffsPve;
	// buff��id
	std::vector<UInt32>			buffsPvp;
	// �ȼ�
	UInt8		level;
	// �¼�����id
	UInt32		nextLevPrecbeadID;
	// ������Ҫ����
	std::vector<ItemReward> consumeItem;
	// ���ָ������Եļ���
	UInt32		addtionBuffPro;
	// ���������ID
	UInt32		buffRandomGroupId;
	// �����û��ı���ID
	std::vector<UInt32> replaceVec;
	// ����
	UInt32		score;
	//ͬ������id
	std::vector<UInt32> sameBeadIDVec;
};

// �������
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

	UInt32 itemId;						//����id
	UInt32 num;							//��������
	UInt32 wt;							//����Ȩ��
	UInt32 fix1;						//Ȩ������1
	UInt32 fix2;						//Ȩ������2
	UInt32 fix2Num;						//����2����
	UInt32 fixMax;						//�������ֵ
	UInt32 fixMin;						//������Сֵ
	UInt32 openCond;					//�����������
	UInt32 chargeCond;					//��ֵ������
	UInt32 getNumLim;					//��øõ��ߵ���������,0Ϊ����

	UInt32 minNum;
	UInt32 maxNum;
};

//ְҵkey
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

//�������&ְҵkey
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

//��������&ְҵ&����id ���key
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

//�����ӱ�
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
	std::string					name;						//��������
	UInt32						type;						//���
	std::vector<JarCountBonus>	countBonus;					//�ۼƱص�Ʒ��
	std::vector<JarBonusInfo>	getItems;					//����õ��ĵ���
	UInt32						moneyType;					//����������
	UInt32						moneyValue;					//�������ֵ
	UInt32						dayBuyLimite;				//ÿ�չ����������
	std::string					countDayLimite;				//ÿ�չ���������Ƽ���key
	UInt8						needExItem;					//�Ƿ����ƾ֤
	UInt32						exItemId;					//ƾ֤id
	UInt32						exItemNum;					//ƾ֤����
	UInt32						comboBuyNum;				//�������
	UInt32						singleBuyDiscount;				//�����ۿ�
	UInt32						comboBuyDiscount;			//�����ۿ�
	std::string					counterKey;					//�����Ӽ���key
	UInt32						getPointId;					//���޻�õĻ���id
	UInt32						getPointNum;				//���޻�õĻ�������
	std::vector<UInt32>			getPointCritRate;			//���޻�û��ֱ�������
	std::vector<UInt32>			getPointCritTime;			//���޻�û��ֱ�������
	UInt32						maxFreeNum;					//�����Ѵ���
	UInt32						freeCd;						//���cd
	std::string					freeNumCounterKey;			//������һ�����ʹ�ô���key
	std::string					nextFreeTimeCounterKey;		//������һ��ʹ�����ʱ��key
	UInt8						needRecord;					//�Ƿ�Ҫ��¼
	UInt32						discountMaxNum;				//������ۿ�������
	UInt8						discountReset;				//������ۿ��Ƿ�����
	UInt32						actifactJarRewardNum;		//���������ɽ�����
	UInt32						jarGiftMustOutNum;			//��������س���Ʒ����
	std::string					curCycleIsOutTreasKey;		//����ѭ���������Ƿ������ϧ��Ʒkey
	UInt32						jarGiftOutJarId;			//��������ۼƱس�����
	UInt32                      smellType;                  //������Ϣװ���ĸ�������
};

// �����ӱ����
class JarBonusDataMgr : public Avalon::DataEntryMgrBase<JarBonusDataEntry>
	, public Avalon::Singleton<JarBonusDataMgr>
{

public:
	JarBonusDataMgr();
	~JarBonusDataMgr();

public:
	bool AddEntry(JarBonusDataEntry* dataEntry);
};

//���ӵ���ǿ�����Ȩ��
struct JarItemStrWt
{
	UInt8	strength;		//ǿ���ȼ�
	UInt32	weight;			//Ȩ��
};

//���ӽ��ر�
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
	UInt32			type;						//�������
	std::string		strOccus;
	std::vector<UInt8>		occus;						//ְҵ
	UInt32			itemId;						//����id
	UInt32			num;						//��������
	UInt32			wt;							//����Ȩ��
	UInt32			fix1;						//Ȩ������1
	UInt32			fix2;						//Ȩ������2
	UInt32			fix2Num;					//����2����
	UInt32			fixMax;						//�������ֵ
	UInt32			fixMin;						//������Сֵ
	UInt32			openCond;					//�����������
	UInt32			chargeCond;					//��ֵ������
	UInt32			getNumLim;					//��øõ��ߵ���������,0Ϊ����
	UInt32			needAnouce;					//�Ƿ񹫸�&��¼
	std::vector<JarItemStrWt>	strWts;			//ǿ��Ȩ��
	UInt8           isTreasItem;				//�Ƿ���ϵ����  
	UInt32			isJarGiftNeedAnount;		//��������Ƿ񹫸�
	UInt32			checkLoad;					//�Ƿ����ʱ����
};

// ���ӽ��ر����
class JarItemPoolMgr : public Avalon::DataEntryMgrBase<JarItemPoolData>
	, public Avalon::Singleton<JarItemPoolMgr>
{

public:
	JarItemPoolMgr();
	~JarItemPoolMgr();

	typedef std::vector<JarItemPoolData*>		BonusVec;
	typedef std::map<JarBonusTrpKey, JarItemPoolData*>	PoolDataMap;			//���key����
	typedef std::map<UInt8, BonusVec>				PoolOccuMap;			//ְҵkey����

public:
	bool AddEntry(JarItemPoolData* dataEntry);
	JarItemPoolData* GetPoolData(JarBonusTrpKey key);
	bool GetOccuToDatas(UInt8 key, BonusVec& datas);
	void GetTypeAndOccuToDatas(UInt32 type, UInt8 occu, BonusVec& datas);

private:
	PoolDataMap			poolDatas;				//�������&ְҵ&itemid����data
	PoolOccuMap			occuToDatas;			//ְҵ������data
	std::vector<JarItemPoolData*> datas;		//����datas
};

//buffҩʹ�ñ�
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

// buffҩ����
class BuffDrugDataMgr : public Avalon::DataEntryMgrBase<BuffDrugData>
	, public Avalon::Singleton<BuffDrugDataMgr>
{

public:
	BuffDrugDataMgr();
	~BuffDrugDataMgr();


};

/**
*@brief ǿ��ȯ��
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
	UInt32	rate;			// ǿ���ɹ�����
	UInt32	effectLv;		// Ч���ȼ�
	bool	canFuse;		// �����ں�
	UInt32  fuseValue;		// �ںϼ�ֵ
};

// ǿ��ȯ��������data��������
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

	// ���ݵȼ��͸��ʻ�ȡ����
	StrTicketData* GetFuseStrengthenTicketData(UInt32 level, UInt32 rate);

	// ���ݼ�ֵ����
	void SortFuseStrengthenTicketsByFuseValue();

	// ���ʿ����ںϵ�ǿ��ȯ
	void VisitFuseStrengthenTickets(const std::function<bool(const StrTicketData*)>& F);

	// ��ȡ�����ںϵ�ǿ��Ⱥ
	const std::vector<StrTicketData*>& GetFuseStrTicketDatas() { return m_FuseStrTicketVec; };

private:
	// �����ںϵ�ǿ��ȯ
	std::map<StrenKey, StrTicketData*> m_FuseStrTicketMap;
	std::vector<StrTicketData*> m_FuseStrTicketVec;
};


/**
*@brief ǿ��ȯ��
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
	UInt8	lvLimitBegin;	// ����ʹ�ÿ�ʼǿ���ȼ�
	UInt8	lvLimitEnd;		// ����ʹ�ý���ǿ���ȼ�
	UInt32	rate;			// ǿ���ɹ�����
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

	// ���ݵȼ��͸��ʻ�ȡ����
	StrTicketData* GetFuseStrengthenTicketData(UInt32 level, UInt32 rate);

	// ���ݼ�ֵ����
	void SortFuseStrengthenTicketsByFuseValue();

	// ���ʿ����ںϵ�ǿ��ȯ
	void VisitFuseStrengthenTickets(const std::function<bool(const StrTicketData*)>& F);

	// ��ȡ�����ںϵ�ǿ��Ⱥ
	const std::vector<StrTicketData*>& GetFuseStrTicketDatas() { return m_FuseStrTicketVec; };

private:
	// �����ںϵ�ǿ��ȯ
	std::map<StrenKey, StrTicketData*> m_FuseStrTicketMap;
	std::vector<StrTicketData*> m_FuseStrTicketVec;
};

//ʱװ�ϳ���ͨ����
class FashionComposeData : public Avalon::DataEntry
{
public:
	FashionComposeData();
	~FashionComposeData();

public:
	UInt32 GetKey() const{ return id; }
	bool Read(Avalon::DataRow& row);

public:
	UInt32			id;							//ʱװID
	UInt8			occu;						//����ְҵ
	UInt8			color;						//Ʒ��
	UInt8			part;						//��λ
	UInt8			composeColor;				//�ϳ�����Ʒ��
	UInt32			weight;						//Ȩ��
};

//����ְҵ&Ʒ��&��λ ���key
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

//����ְҵ&��λ ���key
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

// ʱװ�ϳ���ͨ�����
class FashionComposeMgr : public Avalon::DataEntryMgrBase<FashionComposeData>
	, public Avalon::Singleton<FashionComposeMgr>
{

public:
	FashionComposeMgr();
	~FashionComposeMgr();

	typedef std::vector<FashionComposeData*>	FashionVec;
	typedef std::map<FashionComposeKey, FashionVec>	FashionDataMap;			//���key����

public:
	bool AddEntry(FashionComposeData* dataEntry);
	bool GetFahionDatas(FashionComposeKey key, FashionVec& datas);

	void GetFashions(UInt8 part, UInt8 occu, std::vector<UInt32>& fashions);

private:
	FashionDataMap			datasMap;				//����ְҵ&Ʒ��&��λ����data
};

//ʱװ�ϳ���ղ���
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
	UInt32			id;							//���ʱװID
	UInt32			index;						//���0��ʼ
	UInt32			suitId;						//�������װID
	UInt8			occu;						//����ְҵ
	UInt8			part;						//��λ
	UInt8			type;						//���
	UInt32			baseWeight;					//����Ȩ��
	UInt32			fixRandRange[2];			//�����������
	UInt32			hitMin;						//������д���
	UInt32			maxWeight;					//���Ȩ��
	UInt32			hitWeight;					//����Ȩ��
	std::string		suitName;					//��װ����
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

// ʱװ�ϳ�����ױ����
class FashionComposeSkyMgr : public Avalon::DataEntryMgrBase<FashionComposeSkyData>
	, public Avalon::Singleton<FashionComposeSkyMgr>
{

public:
	FashionComposeSkyMgr();
	~FashionComposeSkyMgr();

	typedef std::vector<FashionComposeSkyData*>	FashionSkyVec;
	typedef std::map<FashionComposeSkyKey, FashionSkyVec>	FashionSkyDataMap;			//���key����
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
	FashionSkyDataMap			datasMap;				//����ְҵ&��λ����data
	FashionSkyDataMap			datasMap_Trans;			//͸����װ
	FashionSkyDataMapII			datasMapII;				//ְҵ,��ͨ�ƽ����,��װid ����key
	std::map<SkyFashionSuitKey, std::string>	suitId2Name;
};

//װ������
class EquipMakeData : public Avalon::DataEntry
{
public:
	EquipMakeData();
	~EquipMakeData();

	//������Ϣ
	struct MatInfo
	{
		UInt32 matId;
		UInt16 num;
	};

public:
	UInt32 GetKey() const{ return id; }
	bool Read(Avalon::DataRow& row);

public:
	UInt32					id;							//װ��id
	std::vector<MatInfo>	needMats;					//��Ҫ����
	UInt32					needMoney;					//��Ҫ����id
	UInt32					needMoneyNum;				//��Ҫ��������
};

// װ���������
class EquipMakeDataMgr : public Avalon::DataEntryMgrBase<EquipMakeData>
	, public Avalon::Singleton<EquipMakeDataMgr>
{

public:
	EquipMakeDataMgr();
	~EquipMakeDataMgr();

public:
	bool AddEntry(EquipMakeData* dataEntry);

};

//װ�����ֱ�
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

// װ�����ֱ����
class EquipValueSceorDataMgr : public Avalon::DataEntryMgrBase<EquipValueSceorData>
	, public Avalon::Singleton<EquipValueSceorDataMgr>
{

public:
	EquipValueSceorDataMgr();
	~EquipValueSceorDataMgr();


};

//ʱװѡ���������ñ�
class FashionAttrData : public Avalon::DataEntry
{
public:
	FashionAttrData();
	~FashionAttrData();

	UInt32 GetKey()	const { return id; }
	bool Read(Avalon::DataRow& row);

public:
	UInt32 id;
	UInt32 defaultAttr1; //(�⻷��)
	std::vector<UInt32>	attrs;
	UInt32 defaultAttr;
	UInt32 freeSelNum;
};

//ʱװѡ���������ñ����
class FashionAttrDataMgr : public Avalon::DataEntryMgrBase<FashionAttrData>
	, public Avalon::Singleton<FashionAttrDataMgr>
{
public:
	FashionAttrDataMgr();
	~FashionAttrDataMgr();

	bool AddEntry(FashionAttrData* dataEntry);
};

//�������
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
	// ui����
	UInt8   uiType;
	// �������(����)
	std::string description;
};

//������ù�����
class GiftPackDataMgr : public Avalon::DataEntryMgrBase<GiftPackData>
	, public Avalon::Singleton<GiftPackDataMgr>
{
public:
	GiftPackDataMgr();
	~GiftPackDataMgr();
};

//��Ʒ����
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

//��Ʒ���ù�����
class GiftItemDataMgr : public Avalon::DataEntryMgrBase<GiftItemData>
	, public Avalon::Singleton<GiftItemDataMgr>
{
public:
	GiftItemDataMgr();
	~GiftItemDataMgr();

	void MergeToGiftPack();
};

//ʱװ�ϳ�ϵ��
class FashionComposeRate : public Avalon::DataEntry
{
public:
	FashionComposeRate();
	~FashionComposeRate();

	UInt32 GetKey() const { return type; }
	bool Read(Avalon::DataRow& row);

	UInt32 type;		//ʱװ���(ItemThirdType)
	UInt32	rate;			//ϵ�� * 100
};

class FashionComposeRateMgr : public Avalon::DataEntryMgrBase<FashionComposeRate>
	, public Avalon::Singleton<FashionComposeRateMgr>
{
public:
	FashionComposeRateMgr();
	~FashionComposeRateMgr();
};

//ʱװ��λ�ϳɴ�������
class FashionCompPartRate : public Avalon::DataEntry
{
public:
	FashionCompPartRate();
	~FashionCompPartRate();

	UInt32 GetKey() const { return composeNum; }
	bool Read(Avalon::DataRow& row);

	UInt32 composeNum;		// ͬ��λ�ϳɴ���
	UInt32 noneSkyRate;			// ��һ��û�кϵ�����׸��� * 100
	UInt32 normalSkyRate;		// ��һ�κϵ���ͨ����׸��� * 100
	UInt32 goldSkyRate;			// ��һ�κϵ��ƽ�����׸��� * 100
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

//����ժ����
class PreciousBeadExtirpeDataEntry : public Avalon::DataEntry
{
public:
	PreciousBeadExtirpeDataEntry(){};
	~PreciousBeadExtirpeDataEntry(){};

public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	//���id
	UInt32 id;
	//Ʒ��
	UInt32 color;
	//�ȼ�
	UInt32 level;
	//��Ҫ���Ĳ���id
	UInt32 materialId;
	//��Ҫ���Ĳ�������
	UInt32 materialNum;
	//ժ���ɹ���
	UInt32 rate;
	//��������
	UInt8  type;
	//��ժȡ����
	Int32  pickNum;
};

typedef Avalon::DataEntryVisitor<PreciousBeadExtirpeDataEntry> PreciousBeadExtirpeDataEntryVisitor;

class PreciousBeadExtirpeDataEntryMgr : public Avalon::DataEntryMgrBase<PreciousBeadExtirpeDataEntry>
	, public Avalon::Singleton<PreciousBeadExtirpeDataEntryMgr >
{
public:
	PreciousBeadExtirpeDataEntry* GetDataEntry(UInt32 color, UInt32 level, UInt32 materialId, UInt8 type);
};

//�����û���
class PreciousBeadReplaceDataEntry : public Avalon::DataEntry
{
public:
	PreciousBeadReplaceDataEntry(){};
	~PreciousBeadReplaceDataEntry(){};

public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	//���id
	UInt32 id;
	//Ʒ��
	UInt32 color;
	//�ȼ�
	UInt32 level;
	//��������
	UInt8  type;
	//��Ҫ����itemid
	UInt32 costItemId;
	//��Ҫ����item����
	UInt32 costItemNum;
	//���û�����
	Int32  replaceNum;
};

typedef Avalon::DataEntryVisitor<PreciousBeadReplaceDataEntry> PreciousBeadReplaceDataEntryVisitor;

class PreciousBeadReplaceDataEntryMgr : public Avalon::DataEntryMgrBase<PreciousBeadReplaceDataEntry>
	, public Avalon::Singleton<PreciousBeadReplaceDataEntryMgr >
{
public:
	PreciousBeadReplaceDataEntry* GetDataEntry(UInt32 color, UInt32 leve, UInt8 type);
};

//���������������������
class PrecBeadAddBuffRandomDataEntry : public Avalon::DataEntry
{
public:
	PrecBeadAddBuffRandomDataEntry() {};
	~PrecBeadAddBuffRandomDataEntry(){};
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	//���id
	UInt32	id;
	//buff��id
	UInt32	buffGroupId;
	//Ȩ��
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

//����װ��ǿ����ά���Լӳ���ֵ��

class AssistEqStrengFouerDimAddDataEntry : public Avalon::DataEntry
{
public:
	AssistEqStrengFouerDimAddDataEntry() {};
	~AssistEqStrengFouerDimAddDataEntry(){};
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

public:
	//���id
	UInt32	id;
	//ǿ���ȼ�
	UInt8	strengthen;
	//װ���ȼ�
	UInt16	equipLv;
	//װ��Ʒ��1
	UInt8  equipColor1;
	//װ��Ʒ��2
	UInt8  equipColor2;
	//��ά��������ֵPVE
	UInt32 addValue;
	//��ά��������ֵPVP
	UInt32 addValuePvp;
	//ǿ��װ������
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

	//ǿ���ȼ�
	UInt8	strengthen;
	//װ���ȼ�
	UInt16	equipLv;
	//װ��Ʒ��1
	UInt8  equipColor1;
	//װ��Ʒ��2
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
	//����ID
	UInt32	itemID;
	//�����ȼ�
	UInt32	enhanceLv;
	//Ȩ��
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
*@brief װ���̶�������ǿ��ϵ����
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
	//����ǿ��ϵ��
	std::vector<UInt32>	strengRates;
	//����Ʒ��ϵ��A
	std::vector<UInt32> colorRateA;
	//����Ʒ��ϵ��B
	std::vector<UInt32> colorRateB;
	//��λϵ���̶�����
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
