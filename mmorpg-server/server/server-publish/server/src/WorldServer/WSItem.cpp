#include "WSItem.h"

WSItem::WSItem()
{
	m_pDataEntry = NULL;
	m_pAttrData = NULL;
	//m_Power = 0;
	m_bTimeOut = false;
	m_RandAttrStar = 0;
	m_SealCount = 0;
	m_OwnerId = 0;
	m_AccId = 0;
	m_DisPhyAtk = 0;
	m_DisMagAtk = 0;
	m_DisPhyDef = 0;
	m_DisMagDef = 0;

	m_PvpDisPhyAtk = 0;
	m_PvpDisMagAtk = 0;
	m_PvpDisPhyDef = 0;
	m_PvpDisMagDef = 0;
	m_StrenthNum = 0;
	m_EnhanceCount = 0;
	m_DayUseNum = 0;
	m_ValuedScore = 0;
	m_SetQlNum = 0;
	memset(m_StrPropEx, 0, sizeof(m_StrPropEx));
	memset(m_DefPropEx, 0, sizeof(m_DefPropEx));
	memset(m_AbnormalResists, 0, sizeof(m_AbnormalResists));
	m_AbnormalResistsTotal = 0;
	m_BeadExtripeCnt = 0;
	m_BeadReplaceCnt = 0;
	m_IndependAtkStreng = 0;
}

WSItem::~WSItem()
{
}

WSItem::WSItem(WSItem& other)
{
	UInt8 buf[64 * 1024];
	Avalon::NetOutputStream ostream(buf, sizeof(buf));
	other.Encode(ostream, SST_SELF);

	Avalon::NetInputStream istream(buf, ostream.GetPos());
	Decode(istream);

	m_DataId = other.m_DataId;
	m_pDataEntry = ItemDataEntryMgr::Instance()->FindEntry(m_DataId);
}

UInt16 WSItem::CheckMerging(UInt16 uNum, UInt8 strenthen)
{
	if(m_Num >= m_pDataEntry->maxNum) return 0;

	if (m_Strengthen != strenthen)
	{
		return 0;
	}

	if(uNum + m_Num > m_pDataEntry->maxNum)
	{
		return m_pDataEntry->maxNum - m_Num;
	}
	else
	{
		return uNum;
	}
}

UInt16 WSItem::Merge(UInt16 uNum, UInt8 strenthen)
{
	if(m_pDataEntry->type == ITEM_EQUIP) return 0;

	if(m_Num >= m_pDataEntry->maxNum) return 0;

	if (m_Strengthen != strenthen)
	{
		return 0;
	}

	UInt16 mergenum = uNum;
	if(m_Num + mergenum > m_pDataEntry->maxNum)
	{
		mergenum = m_pDataEntry->maxNum - m_Num;
	}
	m_Num = m_Num + mergenum;

	return mergenum;
}

EquipPartType WSItem::GetPartType()
{
	if (IsWeapon(GetSubType()))
	{
		return EPT_WEAPON;
	}
	else if (GetThirdType() == TT_ITEM_CLOTH)
	{
		return EPT_CLOTH;
	}
	else if (GetThirdType() == TT_ITEM_SKIN)
	{
		return EPT_LEATHER;
	}
	else if (GetThirdType() == TT_ITEM_LIGHT)
	{
		return EPT_LIGHT;
	}
	else if (GetThirdType() == TT_ITEM_HEAVY)
	{
		return EPT_HEAVY;
	}
	else if (GetThirdType() == TT_ITEM_PLATE)
	{
		return EPT_PLATE;
	}
	else if (IsOrnaments(GetSubType()))
	{
		return EPT_JEWELRY;
	}
	else
	{
		return EPT_NONE;
	}
}

bool WSItem::SetAttribute()
{
	if (!GetDataEntry())
	{
		return false;
	}

	if ((GetType() == ITEM_EQUIP))
	{
		//设置属性data
		EquipAttrDataEntry* attrData = EquipAttrDataMgr::Instance()->FindEntry(GetDataEntry()->attrId);
		if (!attrData)
		{
			ErrorLogStream << "equip attrData is null! id:" << GetDataEntry()->attrId << LogStream::eos;
			return false;
		}
		SetAttrDataEntry(attrData);

		//计算基础属性
		if (!SetAllQualityProp(GetQualityLv()))
		{
			ErrorLogStream << "equip GenEquipBaseAttrs failed! id:" << GetDataID() << LogStream::eos;
			return false;
		}

		CalcStrengthAttrs();
		return true;
	}
	else if (GetType() == ITEM_TITLE)
	{
		//设置属性data
		EquipAttrDataEntry* attrData = EquipAttrDataMgr::Instance()->FindEntry(GetDataEntry()->attrId);
		if (!attrData)
		{
			ErrorLogStream << "TitleEquip attrData is null! id:" << GetDataEntry()->attrId << LogStream::eos;
			return false;
		}
		SetAttrDataEntry(attrData);

		//计算称号基础属性
		if (!GenDataAttrs())
		{
			ErrorLogStream << "TitleEquip GenBaseAttrs failed! id:" << GetDataID() << LogStream::eos;
			return false;
		}

		return true;
	}
	else
	{
		return true;
	}
}

bool WSItem::SetAllQualityProp(UInt8 ql)
{
	EquipQLValueData* qlValueData = EquipQLValueDataMgr::Instance()->FindEntry(GetPartType());
	if (!qlValueData)
	{
		ErrorLogStream << " EquipQLValueData is null! partType:" << GetPartType() << LogStream::eos;
		return false;
	}

	EquipAttrDataEntry* attrData = GetAttrDataEntry();
	if (!attrData)
	{
		ErrorLogStream << " EquipAttrDataEntry is null! attrId:" << GetDataEntry()->attrId << LogStream::eos;
		return false;
	}

	float baseRatio = qlValueData->atkDef;								// 基础攻防百分比系数
	float fourDRatio = qlValueData->fourDimensional;					// 四维系数
	float perBaseRatio = qlValueData->perfectAtkDef;					// 完美基础系数
	float perFourDRatio = qlValueData->perfectfourDimensional;					// 完美四维系数
	float fQlRate = (float)ql / 100.00f;								// 品级修正系数

	float indpendAtkRatio = qlValueData->independAtkProp;				// 独立攻击力系数
	float perIndpendAtkRatio = qlValueData->perfectIndpendAtkProp;		// 完美独立攻击力系数

	// 计算装备实际基础值 公式: 最终值 = （属性*基础攻防百分比 + 属性*（1-基础攻防百分比）*品级修正）+ 属性*完美系数
	UInt8 bPerfect = 0;
	if (ql >= 100)
	{
		bPerfect = 1;
	}

	DebugLogStream << " baseRatio:" << baseRatio << " fourDRatio:" << fourDRatio \
		<< " perBaseRatio:" << perBaseRatio << " perFourDRatio:" << perFourDRatio << " qlRate:" << ql \
		<< " bPerfect:" << bPerfect << LogStream::eos;

	DebugLogStream << " oldAtk:" << m_PhyAtk << " oldMagAtk:" << m_MagAtk \
		<< " oldDef:" << m_PhyDef << " oldMagDef:" << m_MagDef << " oldStrenth:" << m_Strenth \
		<< " oldStamina:" << m_Stamina << " oldSpirit" << m_Spirit << " oldIntellect:" << m_Intellect << LogStream::eos;

	if (0 != attrData->StrPropEx[EEP_LIGHT])	{ SET_EX_PROP(attrData->StrPropEx[EEP_LIGHT], qlValueData->strProp, qlValueData->perfectStrProp, SetStrProp, EEP_LIGHT) }
	if (0 != attrData->StrPropEx[EEP_FIRE])	{ SET_EX_PROP(attrData->StrPropEx[EEP_FIRE], qlValueData->strProp, qlValueData->perfectStrProp, SetStrProp, EEP_FIRE) }
	if (0 != attrData->StrPropEx[EEP_ICE])	{ SET_EX_PROP(attrData->StrPropEx[EEP_ICE], qlValueData->strProp, qlValueData->perfectStrProp, SetStrProp, EEP_ICE) }
	if (0 != attrData->StrPropEx[EEP_DARK])	{ SET_EX_PROP(attrData->StrPropEx[EEP_DARK], qlValueData->strProp, qlValueData->perfectStrProp, SetStrProp, EEP_DARK) }
	if (0 != attrData->DefPropEx[EEP_LIGHT])	{ SET_EX_PROP(attrData->DefPropEx[EEP_LIGHT], qlValueData->defProp, qlValueData->perfectDefProp, SetDefProp, EEP_LIGHT) }
	if (0 != attrData->DefPropEx[EEP_FIRE])	{ SET_EX_PROP(attrData->DefPropEx[EEP_FIRE], qlValueData->defProp, qlValueData->perfectDefProp, SetDefProp, EEP_FIRE) }
	if (0 != attrData->DefPropEx[EEP_ICE])	{ SET_EX_PROP(attrData->DefPropEx[EEP_ICE], qlValueData->defProp, qlValueData->perfectDefProp, SetDefProp, EEP_ICE) }
	if (0 != attrData->DefPropEx[EEP_DARK])	{ SET_EX_PROP(attrData->DefPropEx[EEP_DARK], qlValueData->defProp, qlValueData->perfectDefProp, SetDefProp, EEP_DARK) }
	if (0 != attrData->AbnormalResists[EAR_FLASH])	{ SET_EX_PROP(attrData->AbnormalResists[EAR_FLASH], qlValueData->adnormalResists, qlValueData->perfectAbnormalResists, SetAbnormalResists, EAR_FLASH) }
	if (0 != attrData->AbnormalResists[EAR_BLEEDING])	{ SET_EX_PROP(attrData->AbnormalResists[EAR_BLEEDING], qlValueData->adnormalResists, qlValueData->perfectAbnormalResists, SetAbnormalResists, EAR_BLEEDING) }
	if (0 != attrData->AbnormalResists[EAR_BURN])	{ SET_EX_PROP(attrData->AbnormalResists[EAR_BURN], qlValueData->adnormalResists, qlValueData->perfectAbnormalResists, SetAbnormalResists, EAR_BURN) }
	if (0 != attrData->AbnormalResists[EAR_POISON])	{ SET_EX_PROP(attrData->AbnormalResists[EAR_POISON], qlValueData->adnormalResists, qlValueData->perfectAbnormalResists, SetAbnormalResists, EAR_POISON) }
	if (0 != attrData->AbnormalResists[EAR_BLIND])	{ SET_EX_PROP(attrData->AbnormalResists[EAR_BLIND], qlValueData->adnormalResists, qlValueData->perfectAbnormalResists, SetAbnormalResists, EAR_BLIND) }
	if (0 != attrData->AbnormalResists[EAR_STUN])	{ SET_EX_PROP(attrData->AbnormalResists[EAR_STUN], qlValueData->adnormalResists, qlValueData->perfectAbnormalResists, SetAbnormalResists, EAR_STUN) }
	if (0 != attrData->AbnormalResists[EAR_STONE])	{ SET_EX_PROP(attrData->AbnormalResists[EAR_STONE], qlValueData->adnormalResists, qlValueData->perfectAbnormalResists, SetAbnormalResists, EAR_STONE) }
	if (0 != attrData->AbnormalResists[EAR_FROZEN])	{ SET_EX_PROP(attrData->AbnormalResists[EAR_FROZEN], qlValueData->adnormalResists, qlValueData->perfectAbnormalResists, SetAbnormalResists, EAR_FROZEN) }
	if (0 != attrData->AbnormalResists[EAR_SLEEP])	{ SET_EX_PROP(attrData->AbnormalResists[EAR_SLEEP], qlValueData->adnormalResists, qlValueData->perfectAbnormalResists, SetAbnormalResists, EAR_SLEEP) }
	if (0 != attrData->AbnormalResists[EAR_CONFUNSE])	{ SET_EX_PROP(attrData->AbnormalResists[EAR_CONFUNSE], qlValueData->adnormalResists, qlValueData->perfectAbnormalResists, SetAbnormalResists, EAR_CONFUNSE) }
	if (0 != attrData->AbnormalResists[EAR_STRAIN])	{ SET_EX_PROP(attrData->AbnormalResists[EAR_STRAIN], qlValueData->adnormalResists, qlValueData->perfectAbnormalResists, SetAbnormalResists, EAR_STRAIN) }
	if (0 != attrData->AbnormalResists[EAR_SPEED_DOWN])	{ SET_EX_PROP(attrData->AbnormalResists[EAR_SPEED_DOWN], qlValueData->adnormalResists, qlValueData->perfectAbnormalResists, SetAbnormalResists, EAR_SPEED_DOWN) }
	if (0 != attrData->AbnormalResists[EAR_CURSE])	{ SET_EX_PROP(attrData->AbnormalResists[EAR_CURSE], qlValueData->adnormalResists, qlValueData->perfectAbnormalResists, SetAbnormalResists, EAR_CURSE) }
	if (0 != attrData->AbnormalResistsTotal)	{ SET_BASE_PROP(attrData->AbnormalResistsTotal, qlValueData->adnormalResists, qlValueData->perfectAbnormalResists, SetAbnormalResistsTotal) }

	if (0 != attrData->independAtk) { SET_FOURD_PROP(attrData->independAtk, indpendAtkRatio, perIndpendAtkRatio, SetIndependAtk) }

	DebugLogStream << " newAtk:" << m_PhyAtk << " newMagAtk:" << m_MagAtk \
		<< " newDef:" << m_PhyDef << " newMagDef:" << m_MagDef << " newStrenth:" << m_Strenth \
		<< " newStamina:" << m_Stamina << " newSpirit" << m_Spirit << " newIntellect:" << m_Intellect << " newIndependAtk:" << m_IndependAtk << LogStream::eos;

	return true;
}

void  WSItem::CalcStrengthAttrs()
{
	ItemDataEntry* data = GetDataEntry();
	if (!data) return;
	
	EquipAttrDataEntry* attrData = GetAttrDataEntry();
	if (!attrData) return;

	if (IsWeapon(GetSubType()))
	{
		if (GetStrengthen() > 0 && attrData->independAtk > 0)
		{
			float	strengRate = EquipIndpendatkStrengModDataEntryMgr::Instance()->GetStrengRate(EquipIndpendatkStrengModDataEntryMgr::PVEID, GetStrengthen());
			float	colorARate = EquipIndpendatkStrengModDataEntryMgr::Instance()->GetColorARate(EquipIndpendatkStrengModDataEntryMgr::PVEID, GetQuality());
			float	colorBRate = EquipIndpendatkStrengModDataEntryMgr::Instance()->GetColorBRate(EquipIndpendatkStrengModDataEntryMgr::PVEID, GetQuality());
			float  partRate = EquipIndpendatkStrengModDataEntryMgr::Instance()->GetPartRate(EquipIndpendatkStrengModDataEntryMgr::PVEID);
			m_IndependAtkStreng = ((GetNeedLevel() + (colorARate / 100)) / 8) * strengRate / 100 * colorBRate / 100 * 1.1 * partRate / 100 * 1000;
		}
		else
		{
			m_IndependAtkStreng = 0;
		}
	}
	else if (IsDefend(GetSubType()))
	{
		EquipStrModData* pveDefPercentMod = EquipStrModDataMgr::Instance()->GetPVEDefPercentMod();
		EquipStrModData* pvpDefPercentMod = EquipStrModDataMgr::Instance()->GetPVPDefPercentMod();
		if (!pveDefPercentMod || !pvpDefPercentMod)
		{
			ErrorLogStream << "Percent DefMod is null, guid : " << GetID() << LogStream::eos;
			return;
		}
		m_PhyDefPercent = (UInt32)(pveDefPercentMod->GetArmStrMod(m_Strengthen) * pveDefPercentMod->GetArmClQaMod(data->color) * 10000 + 0.5);
	}
	else if (IsOrnaments(GetSubType()))
	{
		EquipStrModData* pveDefPercentMod = EquipStrModDataMgr::Instance()->GetPVEDefPercentMod();
		EquipStrModData* pvpDefPercentMod = EquipStrModDataMgr::Instance()->GetPVPDefPercentMod();
		if (!pveDefPercentMod || !pvpDefPercentMod)
		{
			ErrorLogStream << "Percent JewMod is null" << LogStream::eos;
			return;
		}
		m_MagDefPercent = (UInt32)(pveDefPercentMod->GetJewStrMod(m_Strengthen) * pveDefPercentMod->GetJewClQaMod(data->color) * 10000 + 0.5);
	}
}

bool WSItem::GenDataAttrs()
{
	if (!GetAttrDataEntry())
		return false;

	for (int i = EEP_LIGHT; i < EEP_MAX; ++i)
	{
		SetStrProp((EquipExProp)i, GetAttrDataEntry()->StrPropEx[i]);
	}

	for (int i = EEP_LIGHT; i < EEP_MAX; ++i)
	{
		SetDefProp((EquipExProp)i, GetAttrDataEntry()->DefPropEx[i]);
	}

	for (int i = EAR_FLASH; i < EAR_MAX; ++i)
	{
		SetAbnormalResists((EquipAbnormalResist)i, GetAttrDataEntry()->AbnormalResists[i]);
	}

	SetAbnormalResistsTotal(GetAttrDataEntry()->AbnormalResistsTotal);

	return true;
}

WSItem* WSItem::CreateItem(UInt32 itemid)
{
	ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(itemid);
	if (dataEntry == NULL) return NULL;

	if (1 == dataEntry->isAbandon)	return NULL;

	WSItem* item = new WSItem();

	item->SetDataID(itemid);
	item->SetDataEntry(dataEntry);

	return item;
}

void WSItem::DestroyItem(WSItem*& item)
{
	CL_SAFE_DELETE(item);
}

void WSItem::SetStrProp(EquipExProp type, Int32 value)
{
	if (type >= EEP_MAX || type < EEP_LIGHT)
	{
		ErrorLogStream << " itemid = " << GetDataID() << " SetStrProp type err! type = " << type << LogStream::eos;
		return;
	}
	m_StrPropEx[type] = value;
}

void WSItem::SetDefProp(EquipExProp type, Int32 value)
{
	if (type >= EEP_MAX || type < EEP_LIGHT)
	{
		ErrorLogStream << " itemid = " << GetDataID() << " SetDefProp type err! type = " << type << LogStream::eos;
		return;
	}
	m_DefPropEx[type] = value;
}

void WSItem::SetAbnormalResists(EquipAbnormalResist type, Int32 value)
{
	if (type >= EAR_MAX || type < EAR_FLASH)
	{
		ErrorLogStream << " itemid = " << GetDataID() << " SetAbnormalResists type err! type = " << type << LogStream::eos;
		return;
	}

	m_AbnormalResists[type] = value;
}

void WSItem::SetAbnormalResistsTotal(Int32 value)
{
	m_AbnormalResistsTotal = value;
}

void WSItem::GenPreciousBeadHoleLoaded()
{
	if (this->GetType() == ITEM_EQUIP && this->m_PreciousBeadHoles.HolesNum() == 0
		&& (this->GetDataEntry()->PreciousBeadHole1.size() > 0 || this->GetDataEntry()->PreciousBeadHole2.size() > 0))
	{
		this->GenPreciousBeadHoles();
	}
}

void WSItem::GenPreciousBeadHoles()
{
	ItemDataEntry* entry = GetDataEntry();
	if (!entry || entry->type != ITEM_EQUIP) return;

	//bool genHole = false;
	if (entry->PreciousBeadHole1.size() > 0)
	{
		UInt32 totalWeight = 0;
		for (auto weightItem : entry->PreciousBeadHole1)
		{
			totalWeight += weightItem.weight;
		}
		UInt32 randWeight = Avalon::Random::RandBetween(1, totalWeight);
		UInt8 type = 0;
		for (auto weightItem : entry->PreciousBeadHole1)
		{
			if (weightItem.weight >= randWeight)
			{
				type = weightItem.value;
				break;
			}
			else
			{
				randWeight -= weightItem.weight;
			}
		}
		if (type > PBMHT_NONE && type < PBMHT_MAX)
		{
			this->m_PreciousBeadHoles.AddHole(1, type);
			//genHole = true;
		}
	}

	if (entry->PreciousBeadHole2.size() > 0)
	{
		UInt32 totalWeight = 0;
		for (auto weightItem : entry->PreciousBeadHole2)
		{
			totalWeight += weightItem.weight;
		}
		UInt32 randWeight = Avalon::Random::RandBetween(1, totalWeight);
		UInt8 type = 0;
		for (auto weightItem : entry->PreciousBeadHole2)
		{
			if (weightItem.weight >= randWeight)
			{
				type = weightItem.value;
				break;
			}
			else
			{
				randWeight -= weightItem.weight;
			}
		}
		if (type > PBMHT_NONE && type < PBMHT_MAX)
		{
			this->m_PreciousBeadHoles.AddHole(2, type);
			//genHole = true;
		}
	}
}

UInt32 WSItem::GetFourDimenAddition()
{
	ItemDataEntry* dataEntry = GetDataEntry();
	if (!dataEntry || dataEntry->subType != ST_ASSIST_EQUIP)
	{
		return 0;
	}

	return AssistEqStrengFouerDimAddDataEntryMgr::Instance()->GetAddValue(GetStrengthen(), dataEntry->needLevel, GetQuality(), GetQualityColor2());
}

bool WSItem::SetValue(UInt32 key, UInt32 value)
{
	return m_KeyValues.SetValue(key, value);
}

UInt32	WSItem::GetValue(UInt32 key)
{
	return m_KeyValues.GetValue(key);
}