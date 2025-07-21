#ifndef _WS_ITEM_H_
#define _WS_ITEM_H_

#include <CLItemDefine.h>
#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <CLItemProperty.h>
#include <CLSceneObject.h>

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
 *@brief WorldServerʹ�õĵ��߽ṹ�����ڴ洢�����ݴ���
 */
class WSItem : public CLSyncObject
{
public:
	/**
	*@brief �������ٵ��߶���
	*/
	static WSItem* CreateItem(UInt32 itemid);
	static void DestroyItem(WSItem*& item);

	WSItem();
	~WSItem();
	WSItem(WSItem& other);

	/**
	 *@brief ���û�ȡ����λ��
	 */
	void SetPos(const ItemPos& pos) { m_Pack = pos.pack; }
	const ItemPos GetPos() const { return ItemPos(m_Pack, 0); }

	void SetQuality(UInt8 quality) { m_Quality = quality; }

	/**
	 *@brief ���ϲ������ؿɺϲ�����
	 *@param num ��������
	 *@param bind 0�ǰ󶨣�1��
	 *@param bForce ǿ�Ƶ���
	 */
	UInt16 CheckMerging(UInt16 num, UInt8 strenthen);
	UInt16 Merge(UInt16 num, UInt8 strenthen);

	UInt16 GetNeedLevel() const { return m_pDataEntry->needLevel; }

	//��������
	bool SetAttribute();

	bool GenDataAttrs();

	//����ǿ���������
	void CalcStrengthAttrs();
	/**
	*@brief ����������
	*/
	void SetDataEntry(ItemDataEntry* dataEntry){ m_pDataEntry = dataEntry; }
	void SetAttrDataEntry(EquipAttrDataEntry* data)	{ m_pAttrData = data; }

	ItemDataEntry* GetDataEntry() const { return m_pDataEntry; }
	EquipAttrDataEntry* GetAttrDataEntry() const { return m_pAttrData; };


	/**
	*@brief ��ȡ���ݱ�id
	*/
	void SetDataID(UInt32 id){
		m_DataId = id;
		m_DataId.ClearDirty(0xFFFFFFFF);
	}

	UInt32 GetDataID() const { return m_DataId; }

	/**
	*@brief ��ȡ��������
	*/
	UInt8 GetType() const { return m_pDataEntry->type; }
	UInt8 GetSubType() const{ return m_pDataEntry->subType; }
	UInt32 GetThirdType() const { return m_pDataEntry->thirdType; }
	
	void SetNum(UInt32 num) { m_Num = num; }

	void SetStrProp(EquipExProp type, Int32 value);
	Int32 GetStrProp(EquipExProp type);

	void SetDefProp(EquipExProp type, Int32 value);
	Int32 GetDefProp(EquipExProp type);

	void SetAbnormalResists(EquipAbnormalResist type, Int32 value);
	Int32 GetAbnormalResists(EquipAbnormalResist type);

	void SetAbnormalResistsTotal(Int32 value);
	inline Int32 GetAbnormalResistsTotal() { return m_AbnormalResistsTotal; }

	inline void SetIndependAtk(UInt32 independAtk) { m_IndependAtk = independAtk; }
	inline UInt32 GetIndependAtk() { return m_IndependAtk; }

	//��ȡ��λ���
	EquipPartType GetPartType();

	inline UInt8 GetQualityLv() { return m_QualityLv; }

	//�������е�Ʒ������
	bool SetAllQualityProp(UInt8 ql);

	//���������н�����ȴʱ��
	void SetAuctionCoolTimeStamp(UInt32 timestamp) { m_AuctionCoolTimeStamp = timestamp; }


	//���ñ���ժȡ����
	void SetBeadExtripeCnt(UInt32 cnt) { m_BeadExtripeCnt = cnt; }

	//���ñ����滻����
	void SetBeadReplaceCnt(UInt32 cnt) { m_BeadReplaceCnt = cnt; }

	void SetParam2(UInt32 param) { m_Param2 = param; }
	
	void GenPreciousBeadHoleLoaded();

	void GenPreciousBeadHoles();

	UInt8 GetStrengthen() { return  m_Strengthen; }

	UInt8 GetEquipType(){ return m_EquipType; }

	UInt8 GetEnhancetype() { return  m_EnhanceType; }

	UInt32 GetFourDimenAddition();

	UInt8 GetQuality() const { return m_pDataEntry->color; }

	UInt8 GetQualityColor2() const { return  m_pDataEntry->color2; }

	public: //item key-value
	bool SetValue(UInt32 key, UInt32 value);
	UInt32	GetValue(UInt32 key);
public://��������
	CL_SYNC_PROPERTY_BEGIN(CLSyncObject)

		CL_SYNC_PROPERTY(m_Num, IA_NUM, SST_SELF | SST_SCENE | SST_WATCH)
		CL_SYNC_PROPERTY(m_Bind, IA_BIND, SST_SELF | SST_SCENE | SST_WATCH)
		CL_SYNC_PROPERTY(m_PhyAtk, IA_PHY_ATK, SST_SELF | SST_WATCH | SST_SCENE)
		CL_SYNC_PROPERTY(m_MagAtk, IA_MAG_ATK, SST_SELF | SST_WATCH | SST_SCENE)
		CL_SYNC_PROPERTY(m_PhyDef, IA_PHY_DEF, SST_SELF | SST_WATCH | SST_SCENE)
		CL_SYNC_PROPERTY(m_MagDef, IA_MAG_DEF, SST_SELF | SST_WATCH | SST_SCENE)
		CL_SYNC_PROPERTY(m_Strenth, IA_STR, SST_SELF | SST_WATCH | SST_SCENE)
		CL_SYNC_PROPERTY(m_Stamina, IA_STAMINA, SST_SELF | SST_WATCH | SST_SCENE)
		CL_SYNC_PROPERTY(m_Intellect, IA_INTELLECT, SST_SELF | SST_WATCH | SST_SCENE)
		CL_SYNC_PROPERTY(m_Spirit, IA_SPIRIT, SST_SELF | SST_WATCH | SST_SCENE)
		CL_SYNC_PROPERTY(m_QualityLv, IA_QUALITYLV, SST_SELF | SST_WATCH | SST_SCENE)
		CL_SYNC_PROPERTY(m_Quality, IA_QUALITY, SST_SELF | SST_SCENE | SST_WATCH | SST_SCENE)
		CL_SYNC_PROPERTY(m_Strengthen, IA_STRENGTHEN, SST_SELF | SST_SCENE | SST_WATCH | SST_SCENE)
		CL_SYNC_PROPERTY(m_RandProperties, IA_RANDATTR, SST_SELF | SST_SCENE | SST_WATCH | SST_SCENE)
		CL_SYNC_PROPERTY(m_DayUseNum, IA_DAYUSENUM, SST_SELF | SST_SCENE | SST_WATCH | SST_SCENE)
		//CL_SYNC_PROPERTY(m_Gemstones,		IA_GEMSTONE,	SST_SELF|SST_SCENE|SST_WATCH|SST_WORLD)
		CL_SYNC_PROPERTY(m_Param1, IA_PARAM1, SST_SELF | SST_SCENE | SST_WATCH | SST_SCENE)
		CL_SYNC_PROPERTY(m_Param2, IA_PARAM2, SST_SELF | SST_SCENE | SST_WATCH | SST_SCENE)
		//CL_SYNC_PROPERTY(m_Power,		IA_POWER,		SST_SELF)
		CL_SYNC_PROPERTY(m_uDeadLine, IA_DEADLINE, SST_SELF | SST_SCENE | SST_WATCH | SST_SCENE)
		CL_SYNC_PROPERTY(m_uStrFailedTimes, IA_STRFAILED, SST_SELF | SST_SCENE | SST_SCENE)
		CL_SYNC_PROPERTY(m_AddMagic, IA_ADDMAGIC, SST_SELF | SST_SCENE | SST_WATCH | SST_SCENE)
		CL_SYNC_PROPERTY(m_SealState, IA_SEAL_STATE, SST_SELF | SST_WATCH | SST_SCENE)
		CL_SYNC_PROPERTY(m_SealCount, IA_SEAL_COUNT, SST_SELF | SST_WATCH | SST_SCENE)
		CL_SYNC_PROPERTY(m_DisPhyAtk, IA_DIS_PHYATK, SST_SELF | SST_WATCH | SST_SCENE)
		CL_SYNC_PROPERTY(m_DisMagAtk, IA_DIS_MAGATK, SST_SELF | SST_WATCH | SST_SCENE)
		CL_SYNC_PROPERTY(m_DisPhyDef, IA_DIS_PHYDEF, SST_SELF | SST_WATCH | SST_SCENE)
		CL_SYNC_PROPERTY(m_DisMagDef, IA_DIS_MAGDEF, SST_SELF | SST_WATCH | SST_SCENE)
		CL_SYNC_PROPERTY(m_PvpDisPhyAtk, IA_PVP_DIS_PHYATK, SST_SCENE)
		CL_SYNC_PROPERTY(m_PvpDisMagAtk, IA_PVP_DIS_MAGATK, SST_SCENE)
		CL_SYNC_PROPERTY(m_PvpDisPhyDef, IA_PVP_DIS_PHYDEF, SST_SCENE)
		CL_SYNC_PROPERTY(m_PvpDisMagDef, IA_PVP_DIS_MAGDEF, SST_SCENE)
		CL_SYNC_PROPERTY(m_ValuedScore, IA_VALUE_SCORE, SST_SELF | SST_SCENE)
		CL_SYNC_PROPERTY(m_SetQlNum, IA_SET_QL_NUM, SST_SCENE)
		CL_SYNC_PROPERTY(m_FashionAttrId, IA_FASHION_ATTRID, SST_SELF | SST_WATCH | SST_SCENE)
		CL_SYNC_PROPERTY(m_FashionAttrNum, IA_FASHION_ATTR_SELNUM, SST_SELF | SST_WATCH | SST_SCENE)
		CL_SYNC_PROPERTY(m_PhyDefPercent, IA_PHYDEF_PERCENT, SST_SELF | SST_WATCH | SST_SCENE)
		CL_SYNC_PROPERTY(m_MagDefPercent, IA_MAGDEF_PERCENT, SST_SELF | SST_WATCH | SST_SCENE)
		CL_SYNC_PROPERTY(m_AddPreciousBead, IA_ADD_PRECIOUSBEAD, SST_SELF | SST_SCENE | SST_WATCH)
		CL_SYNC_PROPERTY(m_StrPropEx[EEP_LIGHT], IA_STRPROP_LIGHT, SST_SELF | SST_SCENE | SST_WATCH)
		CL_SYNC_PROPERTY(m_StrPropEx[EEP_FIRE], IA_STRPROP_FIRE, SST_SELF | SST_SCENE | SST_WATCH)
		CL_SYNC_PROPERTY(m_StrPropEx[EEP_ICE], IA_STRPROP_ICE, SST_SELF | SST_SCENE | SST_WATCH)
		CL_SYNC_PROPERTY(m_StrPropEx[EEP_DARK], IA_STRPROP_DARK, SST_SELF | SST_SCENE | SST_WATCH)
		CL_SYNC_PROPERTY(m_DefPropEx[EEP_LIGHT], IA_DEFPROP_LIGHT, SST_SELF | SST_SCENE | SST_WATCH)
		CL_SYNC_PROPERTY(m_DefPropEx[EEP_FIRE], IA_DEFPROP_FIRE, SST_SELF | SST_SCENE | SST_WATCH)
		CL_SYNC_PROPERTY(m_DefPropEx[EEP_ICE], IA_DEFPROP_ICE, SST_SELF | SST_SCENE | SST_WATCH)
		CL_SYNC_PROPERTY(m_DefPropEx[EEP_DARK], IA_DEFPROP_DARK, SST_SELF | SST_SCENE | SST_WATCH)
		CL_SYNC_PROPERTY(m_AbnormalResistsTotal, IA_ABNORMAL_RESISTS_TOTAL, SST_SELF | SST_SCENE | SST_WATCH)
		CL_SYNC_PROPERTY(m_AbnormalResists[EAR_FLASH], IA_EAR_FLASH, SST_SELF | SST_SCENE | SST_WATCH)
		CL_SYNC_PROPERTY(m_AbnormalResists[EAR_BLEEDING], IA_EAR_BLEEDING, SST_SELF | SST_SCENE | SST_WATCH)
		CL_SYNC_PROPERTY(m_AbnormalResists[EAR_BURN], IA_EAR_BURN, SST_SELF | SST_SCENE | SST_WATCH)
		CL_SYNC_PROPERTY(m_AbnormalResists[EAR_POISON], IA_EAR_POISON, SST_SELF | SST_SCENE | SST_WATCH)
		CL_SYNC_PROPERTY(m_AbnormalResists[EAR_BLIND], IA_EAR_BLIND, SST_SELF | SST_SCENE | SST_WATCH)
		CL_SYNC_PROPERTY(m_AbnormalResists[EAR_STUN], IA_EAR_STUN, SST_SELF | SST_SCENE | SST_WATCH)
		CL_SYNC_PROPERTY(m_AbnormalResists[EAR_STONE], IA_EAR_STONE, SST_SELF | SST_SCENE | SST_WATCH)
		CL_SYNC_PROPERTY(m_AbnormalResists[EAR_FROZEN], IA_EAR_FROZEN, SST_SELF | SST_SCENE | SST_WATCH)
		CL_SYNC_PROPERTY(m_AbnormalResists[EAR_SLEEP], IA_EAR_SLEEP, SST_SELF | SST_SCENE | SST_WATCH)
		CL_SYNC_PROPERTY(m_AbnormalResists[EAR_CONFUNSE], IA_EAR_CONFUNSE, SST_SELF | SST_SCENE | SST_WATCH)
		CL_SYNC_PROPERTY(m_AbnormalResists[EAR_STRAIN], IA_EAR_STRAIN, SST_SELF | SST_SCENE | SST_WATCH)
		CL_SYNC_PROPERTY(m_AbnormalResists[EAR_SPEED_DOWN], IA_EAR_SPEED_DOWN, SST_SELF | SST_SCENE | SST_WATCH)
		CL_SYNC_PROPERTY(m_AbnormalResists[EAR_CURSE], IA_EAR_CURSE, SST_SELF | SST_SCENE | SST_WATCH)
		CL_SYNC_PROPERTY(m_TransferStone, IA_TRANSFER_STONE, SST_SELF | SST_SCENE | SST_WATCH)
		CL_SYNC_PROPERTY(m_recoScore, IA_RECO_SCORE, SST_SELF | SST_SCENE | SST_WATCH)
		CL_SYNC_PROPERTY(m_PreciousBeadHoles, IA_PRECIOUSBEAD_HOLES, SST_SELF | SST_SCENE | SST_WATCH)
		CL_SYNC_PROPERTY(m_AuctionCoolTimeStamp, IA_AUCTION_COOL_TIMESTAMP, SST_SELF | SST_SCENE | SST_WATCH)
		CL_SYNC_PROPERTY(m_isTreas, IA_IS_TREAS, SST_SELF | SST_WATCH)
		CL_SYNC_PROPERTY(m_BeadExtripeCnt, IA_BERD_EXTRIPE_CNT, SST_SELF | SST_SCENE | SST_WATCH)
		CL_SYNC_PROPERTY(m_BeadReplaceCnt, IA_BERD_REPLACE_CNT, SST_SELF | SST_SCENE | SST_WATCH)
		CL_SYNC_PROPERTY(m_EquipType, IA_EQUIP_TYPE, SST_SELF | SST_WATCH | SST_SCENE)
		CL_SYNC_PROPERTY(m_EnhanceType, IA_ENHANCE_TYPE, SST_SELF | SST_WATCH | SST_SCENE)
		CL_SYNC_PROPERTY(m_EnhanceNum, IA_ENHANCE_NUM, SST_SELF | SST_WATCH | SST_SCENE)
		CL_SYNC_PROPERTY(m_uEnhanceFailedTimes, IA_ENHANCE_FAILED, SST_SELF | SST_SCENE)
		CL_SYNC_PROPERTY(m_AuctionTransNum, IA_AUCTION_TRANS_NUM, SST_SELF | SST_WATCH | SST_SCENE)
		CL_SYNC_PROPERTY(m_InscriptionHoles, IA_INSCRIPTION_HOLES, SST_SELF | SST_SCENE | SST_WATCH)
		CL_SYNC_PROPERTY(m_IndependAtk, IA_INDEPENDATK, SST_SELF | SST_WATCH | SST_SCENE)
		CL_SYNC_PROPERTY(m_IndependAtkStreng, IA_INDEPENDATK_STRENG, SST_SELF | SST_WATCH | SST_SCENE)
		CL_SYNC_PROPERTY(m_SubType, IA_SUBTYPE, SST_SELF | SST_SCENE)
		CL_SYNC_PROPERTY(m_KeyValues, IA_KEYVALUES, SST_SELF | SST_SCENE)

		CL_SYNC_PROPERTY_END()

		CL_DBPROPERTY_BEGIN(CLSyncObject)
		CL_DBPROPERTY("itemid", m_DataId)
		CL_DBPROPERTY("num", m_Num)
		CL_DBPROPERTY("bind", m_Bind)
		// 	CL_DBPROPERTY("pack", m_Pack)
		// 	CL_DBPROPERTY("grid", m_Grid)
		CL_DBPROPERTY("phyatk", m_PhyAtk)
		CL_DBPROPERTY("magatk", m_MagAtk)
		CL_DBPROPERTY("magdef", m_MagDef)
		CL_DBPROPERTY("phydef", m_PhyDef)
		CL_DBPROPERTY("strenth", m_Strenth)
		CL_DBPROPERTY("stamina", m_Stamina)
		CL_DBPROPERTY("intellect", m_Intellect)
		CL_DBPROPERTY("spirit", m_Spirit)
		CL_DBPROPERTY("qualitylv", m_QualityLv)
		CL_DBPROPERTY("quality", m_Quality)
		CL_DBPROPERTY("strengthen", m_Strengthen)
		CL_DBPROPERTY("randattr", m_RandProperties)
		CL_DBPROPERTY("dayusenum", m_DayUseNum)
		CL_DBPROPERTY("strenthnum", m_StrenthNum)
		CL_DBPROPERTY("addmagic", m_AddMagic)
		CL_DBPROPERTY("param1", m_Param1)
		CL_DBPROPERTY("param2", m_Param2)
		CL_DBPROPERTY("deadline", m_uDeadLine)
		CL_DBPROPERTY("strfailed", m_uStrFailedTimes)
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
		CL_DBPROPERTY("gemstone", m_Gemstones)
		CL_DBPROPERTY("addpreciousbead", m_PreciousBeadHoles)
		CL_DBPROPERTY("transferstone", m_TransferStone)
		CL_DBPROPERTY("recoScore", m_recoScore)
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
		
		CL_DBPROPERTY_END()
	

public:
	//����id
	ObjUInt32	m_DataId;
	//������
	ObjInt8		m_SubType;
	//��Ӧ���˺�id(�ֿ���)
	ObjUInt32	m_AccId;
	//��Ӧ�Ľ�ɫuid(����ֿ�ǰ�Ľ�ɫ,ȡ���������ɵ�ǰ�Ľ�ɫ)
	ObjUInt64	m_OwnerId;
	//������
	ObjUInt16	m_Num;
	//�Ƿ�� 1δ�� 2��ɫ�� 3�˺Ű�
	ObjUInt8	m_Bind;
	//����
	ObjUInt8	m_Pack;
	//����
	ObjUInt32	m_Grid;

	//������
	ObjUInt32	m_PhyAtk;
	//��������
	ObjUInt32	m_MagAtk;
	//����������
	ObjUInt32	m_IndependAtk;
	//����������ǿ��
	ObjUInt32	m_IndependAtkStreng;
	//�������
	ObjUInt32	m_PhyDef;
	//��������
	ObjUInt32	m_MagDef;
	//����
	ObjUInt32	m_Strenth;
	//����
	ObjUInt32	m_Stamina;
	//����
	ObjUInt32	m_Intellect;
	//����
	ObjUInt32	m_Spirit;
	//Ʒ��
	ObjUInt8	m_QualityLv;

	//װ��Ʒ��
	ObjUInt8	m_Quality;
	//ǿ���ȼ�
	ObjUInt8	m_Strengthen;
	//ǿ������ʧ�ܴ���
	ObjUInt32	m_uStrFailedTimes;
	//ǿ������
	ObjUInt32	m_StrenthNum;

	//ǿ��
	//����������
	ObjUInt32	m_DisPhyAtk;
	//���ӷ�������
	ObjUInt32	m_DisMagAtk;
	//���ӷ���(����10000��ԭ)
	ObjUInt32	m_DisPhyDef;
	ObjUInt32	m_DisMagDef;
	//��������ٷֱ�
	ObjUInt32	m_PhyDefPercent;
	//ħ�������ٷֱ�
	ObjUInt32	m_MagDefPercent;

	//PVPǿ������
	ObjUInt32	m_PvpDisPhyAtk;
	ObjUInt32	m_PvpDisMagAtk;
	ObjUInt32	m_PvpDisPhyDef;
	ObjUInt32	m_PvpDisMagDef;
	ObjUInt32	m_PvpPhyDefPercent;
	ObjUInt32	m_PvpMagDefPercent;

	//�������
	ItemRandProperty<MAX_EQUIPATTR_NUM>	m_RandProperties;

	ItemRandProperty<1>					m_FinalEquipProperty;

	//�����������Ǽ�
	UInt8		m_RandAttrStar;

	//���Ѿ�ʹ�ô���
	ObjUInt32	m_DayUseNum;

	//��ħ��
	ItemMagicProperty<1/*MAX_MAGIC_NUM*/>		m_AddMagic;

	//����ֶ�1:�Ƿ���������
	ObjUInt32	m_Param1;
	//����ֶ�2:����ʹ��ʱ��
	ObjUInt32	m_Param2;

	//ʧЧʱ��
	ObjUInt32	m_uDeadLine;
	//ʧЧ���
	bool		m_bTimeOut;
	//��ӡ״̬
	ObjUInt8	m_SealState;
	//��ӡ����
	ObjUInt32	m_SealCount;

	//�����
	ItemDataEntry*	m_pDataEntry;
	//���Ա�
	EquipAttrDataEntry*	m_pAttrData;

	ObjUInt32	m_ValuedScore;
	//Ʒ����������
	ObjUInt32	m_SetQlNum;

	//ʱװ����id
	ObjUInt32	m_FashionAttrId;
	//ʱװ���Ը��Ĵ���
	ObjUInt32	m_FashionAttrNum;

	ObjString	m_Gemstones;

	//����
	PreciousBeadProperty<1>	m_AddPreciousBead;

	//������Ƕ��
	PreciousBeadHoleProperty<2> m_PreciousBeadHoles;

	//ǿ����������
	ObjInt32		m_StrPropEx[EEP_MAX];
	//���Ը�������
	ObjInt32		m_DefPropEx[EEP_MAX];
	//�쳣������
	ObjInt32		m_AbnormalResistsTotal;
	//�쳣����
	ObjInt32		m_AbnormalResists[EAR_MAX];

	//ת��ʯ
	ObjUInt32	m_TransferStone;
	//װ�����պ����û���
	ObjUInt32	m_recoScore;
	//�����н�����ȴʱ���(��)
	ObjUInt32		m_AuctionCoolTimeStamp;
	//�Ƿ���Ʒ
	ObjInt8			m_isTreas;
	//����ժȡ����
	ObjUInt32		m_BeadExtripeCnt;
	//�����滻����
	ObjUInt32		m_BeadReplaceCnt;
	//װ������(EquipType):��ͨ0/����Ϣ1/����2
	ObjUInt8    m_EquipType;
	//����װ����������:��Ч0/����1/����2/����3/����4
	ObjUInt8    m_EnhanceType;
	//����װ��������ֵ
	ObjUInt32   m_EnhanceNum;
	//��������ʧ�ܴ���
	ObjUInt32	m_uEnhanceFailedTimes;
	//��������
	ObjUInt32	m_EnhanceCount;
	//�����н��״���
	ObjUInt32	m_AuctionTransNum;
	//������Ƕ��
	InscriptionHoleProperty<2> m_InscriptionHoles;
	//item key values
	ItemKeyValues<0> m_KeyValues;
};
/**
 *@brief ���߷�����
 */
typedef CLVisitor<WSItem>	WSItemVisitor;

#endif
