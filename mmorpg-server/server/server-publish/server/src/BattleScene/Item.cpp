#include "Item.h"
#include <CLRecordClient.h>
#include <CLEquipBaseScoreDataEntry.h>
#include <CLEquipStrenLvScoreCoeDataEntry.h>
#include <CLEquipBaseScoreModDataEntry.h>

#include "Player.h"
#include "ScriptMgr.h"
#include "FinalEquipScript.h"
#include "EquipCalcScript.h"
#include "ItemMaker.h"
#include "CLSystemValueDataEntry.h"
#include "SSServiceSwitchMgr.h"

Item::Item()
{
	m_pDataEntry = NULL;
	m_pAttrData = NULL;
	//m_Power = 0;
	m_timeState = ICT_VALID;
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
	m_DayUseNum = 0;
	m_ValuedScore = 0;
	m_SetQlNum = 0;

	m_bNotifyTimeState = true;

	memset(m_StrPropEx, 0, sizeof(m_StrPropEx));
	memset(m_DefPropEx, 0, sizeof(m_DefPropEx));
	memset(m_AbnormalResists, 0, sizeof(m_AbnormalResists));
	m_AbnormalResistsTotal = 0;
	m_AddPreciousBeadTimeStamp = 0;
	m_lockType = 0;
	m_BeadExtripeCnt = 0;
	m_BeadReplaceCnt = 0;
	m_ItemScore = 0;
}

void Item::CopyFrom(Item& other)
{
	m_pDataEntry = NULL;
	m_pAttrData = NULL;
	//m_Power = 0;
	m_timeState = ICT_VALID;
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
	m_DayUseNum = 0;
	m_ValuedScore = 0;
	m_SetQlNum = 0;

	m_bNotifyTimeState = true;

	memset(m_StrPropEx, 0, sizeof(m_StrPropEx));
	memset(m_DefPropEx, 0, sizeof(m_DefPropEx));
	memset(m_AbnormalResists, 0, sizeof(m_AbnormalResists));
	m_AbnormalResistsTotal = 0;

	UInt8 buf[64 * 1024];
	Avalon::NetOutputStream ostream(buf, sizeof(buf));
	other.Encode(ostream, SST_SELF);

	Avalon::NetInputStream istream(buf, ostream.GetPos());
	Decode(istream);

	SetID(other.GetID());
	m_DataId = other.m_DataId;
	m_pDataEntry = ItemDataEntryMgr::Instance()->FindEntry(m_DataId);
}

Item::Item(Item& other)
{
	CopyFrom(other);
}

Item& Item::operator=(Item& other)
{
	CopyFrom(other);
	return *this;
}

Item::~Item()
{
}



void Item::CheckTimeOut(UInt32 deleteIntervalTime)
{
	if (m_timeState == ICT_NEED_DELETE ||
		m_uDeadLine == 0)
		return;

	//不可续费没有续费时限
	if (GetDataEntry()->itemRenewInfos.size() == 0)
	{
		deleteIntervalTime = 0;
	}

	if (m_uDeadLine + deleteIntervalTime <= UInt32(CURRENT_TIME.Sec()))
	{
		m_timeState = ICT_NEED_DELETE;
		return;
	}

	if (m_uDeadLine <= UInt32(CURRENT_TIME.Sec()))
	{
		m_timeState = ICT_EXPIRE;
		return;
	}

	UInt32 adTime = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_NOTIFY_ITEM_ALREAY_DUE);
	if (adTime == 0)
	{
		adTime = 2 * 24 * 3600;
	}

	if (m_uDeadLine - adTime <= UInt32(CURRENT_TIME.Sec()))
	{
		m_timeState = ICT_ABOUT_TO_EXPIRE;
		return;
	}

	return;
}

bool Item::CheckInvalid()
{
	if (m_uDeadLine > 0 && m_uDeadLine <= UInt32(CURRENT_TIME.Sec()))
	{
		return true;
	}

	return false;
}

UInt32 Item::GetEquipGrid()
{
	ItemDataEntry* dataEntry = GetDataEntry();
	if (!dataEntry)	return INVALID_ITEMGRID;

	if (dataEntry->type == ITEM_EQUIP || dataEntry->type == ITEM_TITLE)
	{
		return dataEntry->subType - 1;
	}
	else if (dataEntry->type == ITEM_FASHION)
	{
		if (dataEntry->subType == ItemSubType::ST_FASHION_WEAPON)
		{
			return 6;
		}
		else if (dataEntry->subType == ItemSubType::ST_FASHION_GUANGHUAN)
		{
			return 7;
		}
		return dataEntry->subType - 11;
	}
	
	return INVALID_ITEMGRID;
}

UInt16 Item::CheckMerging(UInt16 num, UInt32 auctionCoolTime)
{
	if(GetType() == ITEM_EQUIP || ITEM_FASHION == GetType()) return 0; //added by aprilliu

	if(m_Num >= m_pDataEntry->maxNum) return 0;

	if (auctionCoolTime > 0 || m_AuctionCoolTimeStamp > 0)
	{
		UInt32 curtime = CURRENT_TIME.Sec();
		if ((auctionCoolTime > curtime || m_AuctionCoolTimeStamp > curtime)
			&& m_AuctionCoolTimeStamp != auctionCoolTime)
		{
			return 0;
		}
	}

	if(m_Num + num > m_pDataEntry->maxNum)
	{
		return m_pDataEntry->maxNum - m_Num;
	}
	else
	{
		return num;
	}
}

UInt16 Item::Merge(const char* reason, Player* player, UInt16 num, UInt32 auctionCoolTimeStamp)
{
	if(GetType() == ITEM_EQUIP) return 0;
	UInt16 oldNum = m_Num;
	if(m_Num >= m_pDataEntry->maxNum) return 0;
	UInt32 currtime = CURRENT_TIME.Sec();
	if ((auctionCoolTimeStamp > currtime || this->m_AuctionCoolTimeStamp > currtime)
		&& auctionCoolTimeStamp != this->m_AuctionCoolTimeStamp)
	{
		return 0;
	}
	UInt16 mergenum = num;
	if(m_Num + mergenum > m_pDataEntry->maxNum)
	{
		mergenum = m_pDataEntry->maxNum - m_Num;
	}
	m_Num = m_Num + mergenum;

	player->OnMerge(reason, this, mergenum, oldNum);

	return mergenum;
}

void Item::GetRaceEquip(RaceEquip& equip, bool pvp)
{
	equip.uid = GetID();
    equip.id = GetDataID();
    equip.pos = m_Grid;
    equip.phyAtk = GetPhyAtk();
    equip.magAtk = GetMagAtk();
    equip.phyDef = GetPhyDef();
	equip.magDef = GetMagDef();
    equip.strenth = GetStrenth();
    equip.stamina = GetStamina();
    equip.intellect = GetIntellect();
    equip.spirit = GetSpirit();
	equip.isTimeLimit = m_uDeadLine != 0 ? 1 : 0;

	if (pvp)
	{
		equip.disPhyAtk = GetPVPDisPhyAtk();
		equip.disMagAtk = GetPVPDisMagAtk();
		equip.disPhyDef = GetPVPDisPhyDef();
		equip.disMagDef = GetPVPDisMagDef();
	}
	else
	{
		equip.disPhyAtk = GetDisPhyAtk();
		equip.disMagAtk = GetDisMagAtk();
		equip.disPhyDef = GetDisPhyDef();
		equip.disMagDef = GetDisMagDef();
	}
	

	equip.strengthen = GetStrengthen();

    for (UInt32 i = 0; i < m_RandProperties.GetPropertyNum(); i++)
    {
        const ItemRandProperty<MAX_EQUIPATTR_NUM>::Property* itemProperty = m_RandProperties.GetProperty(i);
        if (!itemProperty)
        {
            continue;
        }

        RaceItemRandProperty randP;
        randP.type = (UInt8)itemProperty->randPropType;
        randP.value = itemProperty->value;
        equip.properties.push_back(randP);
    }

	equip.cardId = m_AddMagic.GetMagicCard();
	equip.fashionAttrId = GetFashionAttrId();

	if (pvp)
	{
		equip.phyDefPercent = m_PvpPhyDefPercent;
		equip.magDefPercent = m_PvpMagDefPercent;
	}
	else
	{
		equip.phyDefPercent = m_PhyDefPercent;
		equip.magDefPercent = m_MagDefPercent;
	}

	equip.preciousBeadId = m_AddPreciousBead.GetPreciousBeadId();

	equip.atkPropEx.clear();
	equip.strPropEx.clear();
	equip.defPropEx.clear();

	for (int i = 0; i < EEP_MAX; ++i)
	{
		equip.atkPropEx.push_back(GetAttrDataEntry()->AtkPropEx[i]);
		equip.strPropEx.push_back(m_StrPropEx[i]);
		equip.defPropEx.push_back(m_DefPropEx[i]);
	}

	equip.abnormalResistsTotal = m_AbnormalResistsTotal;

	equip.abnormalResists.clear();
	for (int i = 0; i < EAR_MAX; ++i)
	{
		equip.abnormalResists.push_back(m_AbnormalResists[i]);
	}
	m_PreciousBeadHoles.GetRaceInfo(equip.mountPrecBeads);
}

bool Item::SetStrengthen(UInt8 strengthen)
{
	if (strengthen > ITEM_STRENTH_MAX)
	{
		ErrorLogStream << "strengthen(" << strengthen << ") more then 20" << LogStream::eos;
		return false;
	}

	ItemDataEntry* data = GetDataEntry();
	if (!data)
	{
		ErrorLogStream << "item(" << GetID() << ") dataEntry err!" << LogStream::eos;
		return false;
	}

	if (0 == data->attrId)
	{
		ErrorLogStream << "data(" << data->id << ") attrId is 0" << LogStream::eos;
		return false;
	}

	EquipAttrDataEntry* attrData = EquipAttrDataMgr::Instance()->FindEntry(data->attrId);
	if (!attrData)
	{
		ErrorLogStream << "item(" << data->id << ") attr data is null!" << LogStream::eos;
		return false;
	}

	EquipStrModData* PVPModData = EquipStrModDataMgr::Instance()->GetPVPStrMod();
	EquipStrModData* PVEModData = EquipStrModDataMgr::Instance()->GetPVEStrMod();

	if (!PVPModData)
	{
		ErrorLogStream << "EquipStrPVPModData is null" << LogStream::eos;
		return false;
	}

	if (!PVEModData)
	{
		ErrorLogStream << "EquipStrPVEModData is null" << LogStream::eos;
		return false;
	}

	//DebugLogStream << "Equip Str " << m_Strengthen << " => " << strengthen << LogStream::eos;
	m_Strengthen = strengthen;
	if (strengthen > 0)
	{
		// PVE系数计算
		float PVEwpStrMod = PVEModData->GetWpStrMod(m_Strengthen);
		float PVEwpClQaMod = PVEModData->GetWpClQaMod(data->color);
		float PVEwpClQbMod = PVEModData->GetWpClQbMod(data->color);
		float PVEwpPhyMod = PVEModData->GetWpPhyMod(data->thirdType);
		float PVEwpMagMod = PVEModData->GetWpMagMod(data->thirdType);

		float PVEarmStrMod = PVEModData->GetArmStrMod(m_Strengthen);
		//float PVEarmClQaMod = PVEModData->GetArmClQaMod(data->color);
		float PVEarmClQbMod = PVEModData->GetArmClQbMod(data->color);

		float PVEjewStrMod = PVEModData->GetJewStrMod(m_Strengthen);
		//float PVEjewClQaMod = PVEModData->GetJewClQaMod(data->color);
		float PVEjewClQbMod = PVEModData->GetJewClQbMod(data->color);

		// PVP系数计算
		float PVPwpStrMod = PVPModData->GetWpStrMod(m_Strengthen);
		float PVPwpClQaMod = PVPModData->GetWpClQaMod(data->color);
		float PVPwpClQbMod = PVPModData->GetWpClQbMod(data->color);
		float PVPwpPhyMod = PVPModData->GetWpPhyMod(data->thirdType);
		float PVPwpMagMod = PVPModData->GetWpMagMod(data->thirdType);

		float PVParmStrMod = PVPModData->GetArmStrMod(m_Strengthen);
		//float PVParmClQaMod = PVPModData->GetArmClQaMod(data->color);
		float PVParmClQbMod = PVPModData->GetArmClQbMod(data->color);

		float PVPjewStrMod = PVPModData->GetJewStrMod(m_Strengthen);
		//float PVPjewClQaMod = PVPModData->GetJewClQaMod(data->color);
		float PVPjewClQbMod = PVPModData->GetJewClQbMod(data->color);

// 		DebugLogStream << "PVE: wpStrMod=" << PVEwpStrMod << " wpClQa=" << PVEwpClQaMod << " wpClQb=" << PVEwpClQbMod << " wpPhyMod=" << PVEwpPhyMod << " wpMagMod=" << PVEwpMagMod
// 			<< " armStrMod=" << PVEarmStrMod << " armClQaMod=" << PVEarmClQaMod << " armClQaMod=" << PVEarmClQbMod
// 			<< " jewStrMod=" << PVEjewStrMod << " jewClQaMod=" << PVEjewClQaMod << " jewClQaMod=" << PVEjewClQbMod << LogStream::eos;
// 
// 		DebugLogStream << "PVP: wpStrMod=" << PVPwpStrMod << " wpClQa=" << PVPwpClQaMod << " wpClQb=" << PVPwpClQbMod << " wpPhyMod=" << PVPwpPhyMod << " wpMagMod=" << PVPwpMagMod
// 			<< " armStrMod=" << PVParmStrMod << " armClQaMod=" << PVParmClQaMod << " armClQaMod=" << PVParmClQbMod
// 			<< " jewStrMod=" << PVPjewStrMod << " jewClQaMod=" << PVPjewClQaMod << " jewClQaMod=" << PVPjewClQbMod << LogStream::eos;

		//武器
		if (IsWeapon(GetSubType()))
		{
			if (0 != attrData->atk)		{
				m_DisPhyAtk = (UInt32)(((data->needLevel + PVEwpClQaMod) * 0.125 * PVEwpStrMod * PVEwpClQbMod * PVEwpPhyMod * 1.1) + 0.5);
				if (m_DisPhyAtk < 1)	m_DisPhyAtk = 1;

				m_PvpDisPhyAtk = (UInt32)(((data->needLevel + PVPwpClQaMod) * 0.125 * PVPwpStrMod * PVPwpClQbMod * PVPwpPhyMod * 1.1) + 0.5);
				if (m_PvpDisPhyAtk < 1)	m_PvpDisPhyAtk = 1;
			}
			if (0 != attrData->magicAtk)	{
				m_DisMagAtk = (UInt32)(((data->needLevel + PVEwpClQaMod) * 0.125 * PVEwpStrMod * PVEwpClQbMod * PVEwpMagMod * 1.1) + 0.5);
				if (m_DisMagAtk < 1)	m_DisMagAtk = 1;

				m_PvpDisMagAtk = (UInt32)(((data->needLevel + PVPwpClQaMod) * 0.125 * PVPwpStrMod * PVPwpClQbMod * PVPwpMagMod * 1.1) + 0.5);
				if (m_PvpDisMagAtk < 1)	m_PvpDisMagAtk = 1;
			}
		}
		//防具计算物理防御百分比
		else if (IsDefend(GetSubType()))
		{
			if (0 != attrData->def)			{
				m_DisPhyDef = (UInt32)(((data->needLevel + PVEwpClQaMod) * 0.125 * PVEarmStrMod * PVEarmClQbMod) + 0.5);
				if (m_DisPhyDef < 1)	m_DisPhyDef = 1;

				m_PvpDisPhyDef = (UInt32)(((data->needLevel + PVPwpClQaMod) * 0.125 * PVParmStrMod * PVParmClQbMod) + 0.5);
				if (m_PvpDisPhyDef < 1)	m_PvpDisPhyDef = 1;
			}

			EquipStrModData* pveDefPercentMod = EquipStrModDataMgr::Instance()->GetPVEDefPercentMod();
			EquipStrModData* pvpDefPercentMod = EquipStrModDataMgr::Instance()->GetPVPDefPercentMod();
			if (!pveDefPercentMod || !pvpDefPercentMod)
			{
				ErrorLogStream << "Percent DefMod is null" << LogStream::eos;
				return false;
			}

			m_PhyDefPercent = (UInt32)(pveDefPercentMod->GetArmStrMod(m_Strengthen) * pveDefPercentMod->GetArmClQaMod(data->color) * 10000 + 0.5);
			m_PvpPhyDefPercent = (UInt32)(pvpDefPercentMod->GetArmStrMod(m_Strengthen) * pvpDefPercentMod->GetArmClQaMod(data->color) * 10000 + 0.5);
		}
		//首饰计算魔法防御百分比
		else if (IsOrnaments(GetSubType()))
		{
			if (0 != attrData->magicDef)	{
				m_DisMagDef = (UInt32)(((data->needLevel + PVEwpClQaMod) * 0.125 * PVEjewStrMod * PVEjewClQbMod) + 0.5);
				if (m_DisMagDef < 1)	m_DisMagDef = 1;

				m_PvpDisMagDef = (UInt32)(((data->needLevel + PVPwpClQaMod) * 0.125 * PVPjewStrMod * PVPjewClQbMod) + 0.5);
				if (m_PvpDisMagDef < 1)	m_PvpDisMagDef = 1;
			}

			EquipStrModData* pveDefPercentMod = EquipStrModDataMgr::Instance()->GetPVEDefPercentMod();
			EquipStrModData* pvpDefPercentMod = EquipStrModDataMgr::Instance()->GetPVPDefPercentMod();
			if (!pveDefPercentMod || !pvpDefPercentMod)
			{
				ErrorLogStream << "Percent JewMod is null" << LogStream::eos;
				return false;
			}
			m_MagDefPercent = (UInt32)(pveDefPercentMod->GetJewStrMod(m_Strengthen) * pveDefPercentMod->GetJewClQaMod(data->color) * 10000 + 0.5);
			m_PvpMagDefPercent = (UInt32)(pvpDefPercentMod->GetJewStrMod(m_Strengthen) * pvpDefPercentMod->GetJewClQaMod(data->color) * 10000 + 0.5);
		}
	}
	else
	{
		m_DisPhyAtk = 0;	
		m_PvpDisPhyAtk = 0;
		m_DisMagAtk = 0; 
		m_PvpDisMagAtk = 0;
		m_DisPhyDef = 0; 
		m_PvpDisPhyDef = 0;
		m_DisMagDef = 0; 
		m_PvpDisMagDef = 0;
		m_PhyDefPercent = 0;
		m_MagDefPercent = 0;
		m_PvpPhyDefPercent = 0;
		m_PvpMagDefPercent = 0;
	}

// 	DebugLogStream << "PVEDisPhyAtk=" << m_DisPhyAtk << " PVPDisPhyAtk=" << m_PvpDisPhyAtk << " PVEDisMagAtk=" << m_DisMagAtk <<
// 		" PVPDisMagAtk=" << m_PvpDisMagAtk << " PVEDisPhyDef=" << m_DisPhyDef << " PVPDisMagDef=" << m_PvpDisPhyDef <<
// 		" PVEDisMagDef=" << m_DisMagDef << " PVPDisMagDef=" << m_PvpDisMagDef << LogStream::eos;

	return true;
}

bool Item::IsAuctionEquip()
{
	switch (GetType())
	{
	case ITEM_EQUIP:
	case ITEM_FASHION:
	case ITEM_TITLE:
		return true;
	}

	return false;
}

bool Item::SetDeadLine()
{
	if (!GetDataEntry())
	{
		ErrorLogStream << " item data is null! id:" << GetDataID() << LogStream::eos;
		return false;
	}

	if (GetDataEntry()->expireTime > 0)
	{
		m_uDeadLine = GetDataEntry()->expireTime;
	}
	else if (GetDataEntry()->time > 0)
	{
		//m_uDeadLine = UInt32(CURRENT_TIME.Sec()) + GetDataEntry()->time;
		//这个配置项用来作为开宝箱的CD了，在吃鸡拾取战利品的时候会出错，默认为永久
		m_uDeadLine.SetDBValue(0);
	}
	else
	{
		m_uDeadLine.SetDBValue(0);
	}

	return true;
}

void Item::SetDeadLine(UInt32 durationDay)
{
	if (durationDay > 0)
	{
		m_uDeadLine = m_uDeadLine + durationDay * 24 * 3600;
	}
	else
	{
		m_uDeadLine = 0;
	}
}

void Item::SetDeadLineSt(UInt32 timestamp)
{
	if (timestamp > 0)
	{
		m_uDeadLine = timestamp;
	}
}

bool Item::CanAuctionSell(UInt32 num)
{
    if (!m_pDataEntry)
    {
        return false;
    }

	/*if (GetQuality() <= ITEM_QUALITY_BLUE)
	{
		return false;
	}*/

	if (m_pDataEntry->owner != 1)
	{
		if (IsAuctionEquip() && GetQuality() >= ITEM_QUALITY_PURPLE)
		{
			if (!GetSealState())
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

    
    // 数量够不够
    if (GetNum() < num)
    {
        return false;
    }

    return true;
}

void Item::SyncProperty(Player* player, bool bFlush)
{
	//同步到玩家
	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::SCENE_SYNC_ITEM_PROPERTY);

	Avalon::NetOutputStream stream(packet->GetBuffer() + packet->GetSize(), packet->GetMaxSize() - packet->GetSize());
	try
	{
		stream & GetID();
		Encode(stream, SST_SELF, true);
		stream & ObjID_t(0);
	}
	catch(const Avalon::NetException& exception)
	{
		ErrorLogStream << "player(" << player->GetID() << "," << player->GetName() 
			<< ") encode items failed:" << exception.what() << "!" << LogStream::eos;
		Avalon::Packet::Destroy(packet);
		return;
	}
	packet->SetSize(UInt16(stream.GetPos()));
	player->SendPacket(packet);

	Avalon::Packet::Destroy(packet);

	//同步到数据库
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand(player->GetSrcZoneId(), "t_item" ,GetID() ,bFlush);
	GetDBData(cmd, true);
	CLRecordClient::Instance()->SendCommand(cmd);
}

bool Item::SetAllQualityProp(UInt8 ql)
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

	// 计算装备实际基础值 公式: 最终值 = （属性*基础攻防百分比 + 属性*（1-基础攻防百分比）*品级修正）+ 属性*完美系数
	UInt8 bPerfect = 0;
	if (ql >= 100)
	{
		bPerfect = 1;
	}
/*
	DebugLogStream << " baseRatio:" << baseRatio << " fourDRatio:" << fourDRatio \
		<< " perBaseRatio:" << perBaseRatio << " perFourDRatio:" << perFourDRatio << " qlRate:" << ql \
		<< " bPerfect:" << bPerfect << LogStream::eos;

	DebugLogStream << " oldAtk:" << m_PhyAtk << " oldMagAtk:" << m_MagAtk \
		<< " oldDef:" << m_PhyDef << " oldMagDef:" << m_MagDef << " oldStrenth:" << m_Strenth \
		<< " oldStamina:" << m_Stamina << " oldSpirit" << m_Spirit << " oldIntellect:" << m_Intellect << LogStream::eos;
*/
	if (0 != attrData->atk)			{ SET_BASE_PROP(attrData->atk, baseRatio, perBaseRatio, SetPhyAtk) }
	if (0 != attrData->magicAtk)	{ SET_BASE_PROP(attrData->magicAtk, baseRatio, perBaseRatio, SetMagAtk) }  
	if (0 != attrData->def)			{ SET_BASE_PROP(attrData->def, baseRatio, perBaseRatio, SetPhyDef) }
	if (0 != attrData->magicDef)	{ SET_BASE_PROP(attrData->magicDef, baseRatio, perBaseRatio, SetMagDef) }
	if (0 != attrData->strenth)		{ SET_FOURD_PROP(attrData->strenth, fourDRatio, perFourDRatio, SetStrenth) }
	if (0 != attrData->stamina)		{ SET_FOURD_PROP(attrData->stamina, fourDRatio, perFourDRatio, SetStamina) }
	if (0 != attrData->spirit)		{ SET_FOURD_PROP(attrData->spirit, fourDRatio, perFourDRatio, SetSpirit) }
	if (0 != attrData->intellect)	{ SET_FOURD_PROP(attrData->intellect, fourDRatio, perFourDRatio, SetIntellect) }
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

// 	DebugLogStream << " newAtk:" << m_PhyAtk << " newMagAtk:" << m_MagAtk 
// 		<< " newDef:" << m_PhyDef << " newMagDef:" << m_MagDef << " newStrenth:" << m_Strenth 
// 		<< " newStamina:" << m_Stamina << " newSpirit" << m_Spirit << " newIntellect:" << m_Intellect << LogStream::eos;

	return true;
}

void Item::SetSingleQualityProp(EquipQLAttrType type, UInt8 ql, EquipAttrDataEntry* attrData, EquipQLValueData* qlValueData)
{
	if (!attrData)
	{
		ErrorLogStream << " playerUid = " << m_OwnerId << " attrData is null!" << LogStream::eos;
		return;
	}

	if (!qlValueData)
	{
		ErrorLogStream << " playerUid = " << m_OwnerId << " qlValueData is null!" << LogStream::eos;
		return;
	}

	float baseRatio = qlValueData->atkDef;								// 基础攻防百分比系数
	float fourDRatio = qlValueData->fourDimensional;					// 四维系数
	float perBaseRatio = qlValueData->perfectAtkDef;					// 完美基础系数
	float perFourDRatio = qlValueData->perfectAtkDef;					// 完美四维系数
	float fQlRate = (float)ql / 100.00f;								// 品级修正系数

	UInt8 bPerfect = 0;
	if (ql >= 100)
	{
		bPerfect = 1;
	}

	switch (type)
	{
	case EQAT_PHYATK:	{ SET_BASE_PROP(attrData->atk, baseRatio, perBaseRatio, SetPhyAtk) }				 break;
	case EQAT_MAGATK:	{ SET_BASE_PROP(attrData->magicAtk, baseRatio, perBaseRatio, SetMagAtk) }		 break;
	case EQAT_PHYDEF:	{ SET_BASE_PROP(attrData->def, baseRatio, perBaseRatio, SetPhyDef) }				 break;
	case EQAT_MAGDEF:	{ SET_BASE_PROP(attrData->magicDef, baseRatio, perBaseRatio, SetMagDef) }		 break;
	case EQAT_STRENGTH:	{ SET_FOURD_PROP(attrData->strenth, fourDRatio, perFourDRatio, SetStrenth) }		 break;
	case EQAT_STAMINA:	{ SET_FOURD_PROP(attrData->stamina, fourDRatio, perFourDRatio, SetStamina) }		 break;
	case EQAT_SPIRIT:	{ SET_FOURD_PROP(attrData->spirit, fourDRatio, perFourDRatio, SetSpirit) }		 break;
	case EQAT_INTELLECT:{ SET_FOURD_PROP(attrData->intellect, fourDRatio, perFourDRatio, SetIntellect) }	 break;
	case EQAT_LIGHT_STRPROP:{ SET_EX_PROP(attrData->StrPropEx[EEP_LIGHT], qlValueData->strProp, qlValueData->perfectStrProp, SetStrProp, EEP_LIGHT) }	 break;
	case EQAT_FIRE_STRPROP:{ SET_EX_PROP(attrData->StrPropEx[EEP_FIRE], qlValueData->strProp, qlValueData->perfectStrProp, SetStrProp, EEP_FIRE) }	 break;
	case EQAT_ICE_STRPROP:{ SET_EX_PROP(attrData->StrPropEx[EEP_ICE], qlValueData->strProp, qlValueData->perfectStrProp, SetStrProp, EEP_ICE) }	 break;
	case EQAT_DARK_STRPROP:{ SET_EX_PROP(attrData->StrPropEx[EEP_DARK], qlValueData->strProp, qlValueData->perfectStrProp, SetStrProp, EEP_DARK) }	 break;
	case EQAT_LIGHT_DEFPROP:{ SET_EX_PROP(attrData->DefPropEx[EEP_LIGHT], qlValueData->defProp, qlValueData->perfectDefProp, SetDefProp, EEP_LIGHT) }	 break;
	case EQAT_FIRE_DEFPROP:{ SET_EX_PROP(attrData->DefPropEx[EEP_FIRE], qlValueData->defProp, qlValueData->perfectDefProp, SetDefProp, EEP_FIRE) }	 break;
	case EQAT_ICE_DEFPROP:{ SET_EX_PROP(attrData->DefPropEx[EEP_ICE], qlValueData->defProp, qlValueData->perfectDefProp, SetDefProp, EEP_ICE) }	 break;
	case EQAT_DARK_DEFPROP:{ SET_EX_PROP(attrData->DefPropEx[EEP_DARK], qlValueData->defProp, qlValueData->perfectDefProp, SetDefProp, EEP_DARK) }	 break;
	case EQAT_GDKX:{ SET_EX_PROP(attrData->AbnormalResists[EAR_FLASH], qlValueData->adnormalResists, qlValueData->perfectAbnormalResists, SetAbnormalResists, EAR_FLASH) }	 break;
	case EQAT_CXKX:{ SET_EX_PROP(attrData->AbnormalResists[EAR_BLEEDING], qlValueData->adnormalResists, qlValueData->perfectAbnormalResists, SetAbnormalResists, EAR_BLEEDING) }	 break;
	case EQAT_ZSKX:{ SET_EX_PROP(attrData->AbnormalResists[EAR_BURN], qlValueData->adnormalResists, qlValueData->perfectAbnormalResists, SetAbnormalResists, EAR_BURN) }	 break;
	case EQAT_ZDKX:{ SET_EX_PROP(attrData->AbnormalResists[EAR_POISON], qlValueData->adnormalResists, qlValueData->perfectAbnormalResists, SetAbnormalResists, EAR_POISON) }	 break;
	case EQAT_SMKX:{ SET_EX_PROP(attrData->AbnormalResists[EAR_BLIND], qlValueData->adnormalResists, qlValueData->perfectAbnormalResists, SetAbnormalResists, EAR_BLIND) }	 break;
	case EQAT_YXKX:{ SET_EX_PROP(attrData->AbnormalResists[EAR_STUN], qlValueData->adnormalResists, qlValueData->perfectAbnormalResists, SetAbnormalResists, EAR_STUN) }	 break;
	case EQAT_SHKX:{ SET_EX_PROP(attrData->AbnormalResists[EAR_STONE], qlValueData->adnormalResists, qlValueData->perfectAbnormalResists, SetAbnormalResists, EAR_STONE) }	 break;
	case EQAT_BDKX:{ SET_EX_PROP(attrData->AbnormalResists[EAR_FROZEN], qlValueData->adnormalResists, qlValueData->perfectAbnormalResists, SetAbnormalResists, EAR_FROZEN) }	 break;
	case EQAT_SLEEPKX:{ SET_EX_PROP(attrData->AbnormalResists[EAR_SLEEP], qlValueData->adnormalResists, qlValueData->perfectAbnormalResists, SetAbnormalResists, EAR_SLEEP) }	 break;
	case EQAT_HLKX:{ SET_EX_PROP(attrData->AbnormalResists[EAR_CONFUNSE], qlValueData->adnormalResists, qlValueData->perfectAbnormalResists, SetAbnormalResists, EAR_CONFUNSE) }	 break;
	case EQAT_SFKX:{ SET_EX_PROP(attrData->AbnormalResists[EAR_STRAIN], qlValueData->adnormalResists, qlValueData->perfectAbnormalResists, SetAbnormalResists, EAR_STRAIN) }	 break;
	case EQAT_JSKX:{ SET_EX_PROP(attrData->AbnormalResists[EAR_SPEED_DOWN], qlValueData->adnormalResists, qlValueData->perfectAbnormalResists, SetAbnormalResists, EAR_SPEED_DOWN) }	 break;
	case EQAT_ZZKX:{ SET_EX_PROP(attrData->AbnormalResists[EAR_CURSE], qlValueData->adnormalResists, qlValueData->perfectAbnormalResists, SetAbnormalResists, EAR_CURSE) }	 break;
	case EQAT_TOTALKX:{ SET_BASE_PROP(attrData->AbnormalResistsTotal, qlValueData->adnormalResists, qlValueData->perfectAbnormalResists, SetAbnormalResistsTotal) }	 break;
	default:	break;
	}
}

void Item::SetStrProp(EquipExProp type, Int32 value)
{
	if (type >= EEP_MAX || type < EEP_LIGHT)
	{
		ErrorLogStream << " playerUid = " << m_OwnerId << " itemid = " << GetDataID() << " SetStrProp type err! type = " << type << LogStream::eos;
		return;
	}
	m_StrPropEx[type] = value;
}

Int32 Item::GetStrProp(EquipExProp type)
{
	if (type >= EEP_MAX || type < EEP_LIGHT)
	{
		ErrorLogStream << " playerUid = " << m_OwnerId << " itemid = " << GetDataID() << " GetStrProp type err! type = " << type << LogStream::eos;
		return 0;
	}

	return m_StrPropEx[type];
}

void Item::SetDefProp(EquipExProp type, Int32 value)
{
	if (type >= EEP_MAX || type < EEP_LIGHT)
	{
		ErrorLogStream << " playerUid = " << m_OwnerId << " itemid = " << GetDataID() << " SetDefProp type err! type = " << type << LogStream::eos;
		return;
	}
	m_DefPropEx[type] = value;
}

Int32 Item::GetDefProp(EquipExProp type)
{
	if (type >= EEP_MAX || type < EEP_LIGHT)
	{
		ErrorLogStream << " playerUid = " << m_OwnerId << " itemid = " << GetDataID() << " GetDefProp type err! type = " << type << LogStream::eos;
		return 0;
	}

	return m_DefPropEx[type];
}

void Item::SetAbnormalResists(EquipAbnormalResist type, Int32 value)
{
	if (type >= EAR_MAX || type < EAR_FLASH)
	{
		ErrorLogStream << " playerUid = " << m_OwnerId << " itemid = " << GetDataID() << " SetAbnormalResists type err! type = " << type << LogStream::eos;
		return;
	}

	m_AbnormalResists[type] = value;
}

Int32 Item::GetAbnormalResists(EquipAbnormalResist type)
{
	if (type >= EAR_MAX || type < EAR_FLASH)
	{
		ErrorLogStream << " playerUid = " << m_OwnerId << " itemid = " << GetDataID() << " GetAbnormalResists type err! type = " << type << LogStream::eos;
		return 0;
	}

	return m_AbnormalResists[type];
}

void Item::SetAbnormalResistsTotal(Int32 value)
{
	m_AbnormalResistsTotal = value;
}

void Item::SetAccId(UInt32 accid, bool bDirty /*= false*/)
{
	if (bDirty)
	{
		m_AccId = accid;
	}
	else
	{
		m_AccId.SetDBValue(accid);
	}
}

void Item::SetOwnerId(UInt64 ownerid, bool bDirty /*= false*/)
{
	if (bDirty)
	{
		m_OwnerId = ownerid;
	}
	else
	{
		m_OwnerId.SetDBValue(ownerid);
	}
}

bool Item::SetFashionDefaultAttr()
{
	if (GetType() != ITEM_FASHION)
	{
		return false;
	}

	FashionAttrData* data = FashionAttrDataMgr::Instance()->FindEntry(GetDataID());
	if (!data)
	{
		ErrorLogStream << "FashionAttrData is null! id:" << GetDataID() << LogStream::eos;
		return false;
	}

	m_FashionAttrId.SetDBValue(data->defaultAttr);
	return true;
}

void Item::AddFashionAttrChangeNum()
{
	if (m_FashionAttrNum == 0xFFFFFFFF)
	{
		return;
	}

	m_FashionAttrNum = m_FashionAttrNum + 1;
}

UInt32 Item::CalculateValuedScore(Player* player)
{
	//	if ((GetSubType() >= ST_EQUIP_WEAPON &&
	//		GetSubType() <= ST_EQUIP_BRACELET)
	//		|| ST_ASSIST_EQUIP == GetSubType())
	//	{
	//#ifdef _DEBUG
	//		DebugLogStream << "---Calculate Item:" << GetDataEntry()->name << " Start---" << LogStream::eos;
	//#endif
	//		
	//		float propScore = GetBasePropScore() + GetRandPropScore() + GetMagicPropScore() + GetStrPropScore() + GetExPropScore() + GetPreciousBeadPropScore();
	//
	//#ifdef _DEBUG
	//		DebugLogStream << "TotlePropScore = " << propScore << LogStream::eos;
	//#endif
	//		UInt32 score = (UInt32)(propScore * GetPartRate() + GetSkillScore() + GetInscriptionScore());
	//		
	//#ifdef _DEBUG
	//		DebugLogStream << "FinalScore = " << score << LogStream::eos;
	//#endif
	//		if ((UInt32)m_ValuedScore != score)
	//		{
	//			m_ValuedScore = score;
	//			//同步到数据库
	//			CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_item", GetID(), false);
	//			GetDBData(cmd, true);
	//			CLRecordClient::Instance()->SendCommand(cmd);
	//		}
	//
	//		//DebugLogStream << "---Calculate End---" << LogStream::eos;
	//		return score;
	//	}
	//
	//	return 0;

	if ((GetSubType() >= ST_EQUIP_WEAPON &&
		GetSubType() <= ST_EQUIP_BRACELET)
		|| ST_ASSIST_EQUIP == GetSubType()
		|| ST_EQUIP_TITLE == GetSubType()
		|| ITEM_FASHION == GetType())
	{
		UInt32 score = 0;
		if (GetSubType() == ST_EQUIP_WEAPON)
		{
#ifdef _DEBUG
			DebugLogStream << "---Calculate Item:" << GetDataEntry()->name << " Start---" << LogStream::eos;
#endif

			float propScore = GetBasePropScore() + GetRandPropScore() + GetMagicPropScore() + GetStrPropScore() + GetExPropScore() + GetPreciousBeadPropScore();

#ifdef _DEBUG
			DebugLogStream << "TotlePropScore = " << propScore << LogStream::eos;
#endif
			score = (UInt32)(propScore * GetPartRate() + GetSkillScore());

#ifdef _DEBUG
			DebugLogStream << "FinalScore = " << score << LogStream::eos;
#endif
		}
		else
		{
			CalcItemScore(player);
			score = GetItemScore();
#ifdef _DEBUG
			DebugLogStream << "calc item score by CalcItemScore, FinalScore = " << score << ", id = " << GetID() << ", typeid = " << GetDataID()
				<< "subtype = " << (UInt32)GetSubType() << LogStream::eos;
#endif
		}

		if ((UInt32)m_ValuedScore != score)
		{
			m_ValuedScore = score;
			//同步到数据库
			CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_item", GetID(), false);
			GetDBData(cmd, true);
			CLRecordClient::Instance()->SendCommand(cmd);
		}

		return score;
	}

	return 0;
}


float Item::GetBasePropScore()
{
#ifdef _DEBUG
	DebugLogStream << "---BaseProp Start---" << LogStream::eos;
#endif
	float score = \
		(GetScoreByPropType(ISP_STR) + \
		GetScoreByPropType(ISP_INT) + \
		GetScoreByPropType(ISP_STAM) + \
		GetScoreByPropType(ISP_SPR) + \
		GetScoreByPropType(ISP_HP) + \
		GetScoreByPropType(ISP_MP) + \
		GetScoreByPropType(ISP_HPRECVR) + \
		GetScoreByPropType(ISP_MPRECVR) + \
		GetScoreByPropType(ISP_PHYATK) + \
		GetScoreByPropType(ISP_MAGATK) + \
		GetScoreByPropType(ISP_PHYDEF) + \
		GetScoreByPropType(ISP_MAGDEF) + \
		GetScoreByPropType(ISP_ATKSPD) + \
		GetScoreByPropType(ISP_MAGSPD) + \
		GetScoreByPropType(ISP_MVSPD) + \
		GetScoreByPropType(ISP_PHYCRT) + \
		GetScoreByPropType(ISP_MAGCRT) + \
		GetScoreByPropType(ISP_HIT) + \
		GetScoreByPropType(ISP_MISS) + \
		GetScoreByPropType(ISP_YZ));

#ifdef _DEBUG
 	DebugLogStream << "BasePropTotleScore = " << score << LogStream::eos;
 	DebugLogStream << "---BaseProp End---" << LogStream::eos;
#endif
	return score;
}

float Item::GetRandPropScore()
{
#ifdef _DEBUG
	DebugLogStream << "---RandProp Start---" << LogStream::eos;
#endif
	float totleScore = 0;
	for (int i = 0; i < MAX_EQUIPATTR_NUM; ++i)
	{
		const ItemRandProperty<MAX_EQUIPATTR_NUM>::Property* prop = m_RandProperties.GetProperty(i);
		if (!prop)	continue;
		
		ItemScoreProp sp = TransRandPropType(prop->randPropType);

		EquipValueSceorData* scoreData = EquipValueSceorDataMgr::Instance()->FindEntry((UInt32)sp);
		if (!scoreData)		return 0;

		UInt32 fix = 1;
		if (sp >= ISP_STR && sp <= ISP_SPR)
		{
			fix = 1000;
		}
		else if (sp >= ISP_ATKSPD && sp <= ISP_MISS)
		{
			fix = 10;
		}

		float score = 0;
		float value = (float)prop->value / fix;
		float rate = (float)scoreData->value / 1000;
		if (fix == 10)
		{
			UInt16 lv = GetNeedLevel() < 5 ? 5 : GetNeedLevel();
			score = value * rate * lv / 10;
#ifdef _DEBUG
			DebugLogStream << "Extra RandPropType:" << GetTypeTransName(sp) << ",value:" << value << ",rate:" << rate << "lv:" << lv << LogStream::eos;
#endif
		}
		else
		{
			score = value * rate;
#ifdef _DEBUG
			DebugLogStream << "RandPropType:" << GetTypeTransName(sp) << ",value:" << value << ",rate:" << rate << LogStream::eos;
#endif
		}

#ifdef _DEBUG
		DebugLogStream << "RandPropType:" << GetTypeTransName(sp) << ",Score:" << score << LogStream::eos;
#endif
		
		totleScore += score;
	}

#ifdef _DEBUG
 	DebugLogStream << "RandPropTotleScore = " << totleScore << LogStream::eos;
 	DebugLogStream << "---RandProp End---" << LogStream::eos;
#endif
	return totleScore;
}


ItemScoreProp Item::TransRandPropType(ItemRandProp rp)
{
	switch (rp)
	{
	case IRP_STR:				return ISP_STR;
	case IRP_STA:				return ISP_STAM;
	case IRP_INT:				return ISP_INT;
	case IRP_SPR:				return ISP_SPR;
	case IRP_HPMAX:				return ISP_HP;
	case IRP_MPMAX:				return ISP_MP;
	case IRP_HPREC:				return ISP_HPRECVR;
	case IRP_MPREC:				return ISP_MPRECVR;
	case IRP_HIT:				return ISP_HIT;
	case IRP_DEX:				return ISP_MISS;
	case IRP_PHYCRT:			return ISP_PHYCRT;
	case IRP_MGCCRT:			return ISP_MAGCRT;
	case IRP_ATKSPD:			return ISP_ATKSPD;
	case IRP_RDYSPD:			return ISP_MAGSPD;
	case IRP_MOVSPD:			return ISP_MVSPD;
	case IRP_HITREC:			return ISP_YZ;
	case IRP_PHYATK:			return ISP_PHYATK;
	case IRP_MAGATK:			return ISP_MAGATK;
	case IRP_PHYDEF:			return ISP_PHYDEF;
	case IRP_MAGDEF:			return ISP_MAGDEF;
	case IRP_LIGHT:				return ISP_LIGHT;
	case IRP_FIRE:				return ISP_FIRE;
	case IRP_ICE:				return ISP_ICE;
	case IRP_DARK:				return ISP_DARK;
	case IRP_LIGHT_ATK:			return ISP_LIGHT_ATK;
	case IRP_FIRE_ATK:			return ISP_FIRE_ATK;
	case IRP_ICE_ATK:			return ISP_ICE_ATK;
	case IRP_DARK_ATK:			return ISP_DARK_ATK;
	case IRP_LIGHT_DEF:			return ISP_LIGHT_DEF;
	case IRP_FIRE_DEF:			return ISP_FIRE_DEF;
	case IRP_ICE_DEF:			return ISP_ICE_DEF;
	case IRP_DARK_DEF:			return ISP_DARK_DEF;
	case IRP_GDKX:				return ISP_GDKX;
	case IRP_CXKX:				return ISP_CXKX;
	case IRP_ZSKX:				return ISP_ZSKX;
	case IRP_ZDKX:				return ISP_ZDKX;
	case IRP_SMKX:				return ISP_SMKX;
	case IRP_XYKX:				return ISP_XYKX;
	case IRP_SHKX:				return ISP_SHKX;
	case IRP_BDKX:				return ISP_BDKX;
	case IRP_SLKX:				return ISP_SLKX;
	case IRP_HLKX:				return ISP_HLKX;
	case IRP_SFKX:				return ISP_SFKX;
	case IRP_JSKX:				return ISP_JSKX;
	case IRP_ZZKX:				return ISP_ZZKX;
	default:					break;
	}

	return ISP_NONE;
}

std::string Item::GetTypeTransName(ItemScoreProp type)
{
	switch (type)
	{
	case ISP_NONE:	return std::string("none");
		break;
	case ISP_STR:	return std::string("ISP_STR");
		break;
	case ISP_INT:	return std::string("ISP_INT");
		break;
	case ISP_STAM:	return std::string("ISP_STAM");
		break;
	case ISP_SPR:	return std::string("ISP_SPR");
		break;
	case ISP_HP:	return std::string("ISP_HP");
		break;
	case ISP_MP:	return std::string("ISP_MP");
		break;
	case ISP_HPRECVR:	return std::string("ISP_HPRECVR");
		break;
	case ISP_MPRECVR:	return std::string("ISP_MPRECVR");
		break;
	case ISP_PHYATK:	return std::string("ISP_PHYATK");
		break;
	case ISP_MAGATK:	return std::string("ISP_MAGATK");
		break;
	case ISP_PHYDEF:	return std::string("ISP_PHYDEF");
		break;
	case ISP_MAGDEF:	return std::string("ISP_MAGDEF");
		break;
	case ISP_DISPHYATK:	return std::string("ISP_DISPHYATK");
		break;
	case ISP_DISMAGATK:	return std::string("ISP_DISMAGATK");
		break;
	case ISP_PHYDMGRDC:	return std::string("ISP_PHYDMGRDC");
		break;	
	case ISP_MAGDMGRDC:	return std::string("ISP_MAGDMGRDC");
		break;
	case ISP_ATKSPD:	return std::string("ISP_ATKSPD");
		break;
	case ISP_MAGSPD:	return std::string("ISP_MAGSPD");
		break;
	case ISP_MVSPD:		return std::string("ISP_MVSPD");
		break;
	case ISP_PHYCRT:	return std::string("ISP_PHYCRT");
		break;
	case ISP_MAGCRT:	return std::string("ISP_MAGCRT");
		break;
	case ISP_CRTDMG:	return std::string("ISP_CRTDMG");
		break;
	case ISP_HIT:	return std::string("ISP_HIT");
		break;
	case ISP_MISS:	return std::string("ISP_MISS");
		break;
	case ISP_JZ:	return std::string("ISP_JZ");
		break;
	case ISP_YZ:	return std::string("ISP_YZ");
		break;
	case ISP_HSWORD:	return std::string("ISP_HSWORD");
		break;
	case ISP_TD:	return std::string("ISP_TD");
		break;
	case ISP_ZL:	return std::string("ISP_ZL");
		break;
	case ISP_SP:	return std::string("ISP_SP");
		break;
	case ISP_FZ:	return std::string("ISP_FZ");
		break;
	case ISP_MZ:	return std::string("ISP_MZ");
		break;
	case ISP_BJ:	return std::string("ISP_BJ");
		break;
	case ISP_PJ:	return std::string("ISP_PJ");
		break;
	case ISP_MAX:	return std::string("ISP_MAX");
		break;
	default:
		break;
	}

	return std::string("");
}

EquipPartType Item::GetPartType()
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

bool Item::SetAttribute()
{
	if (!GetDataEntry())
	{
		return false;
	}

	if (GetType() == ITEM_EQUIP)
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

		//计算随机属性值
		if (!ItemMaker::Instance()->GenEquipAttrs(this))
		{
			ErrorLogStream << "equip GenEquipRandAttrs failed! id:" << GetDataID() << LogStream::eos;
			return false;
		}

		//设置强化等级
		if (!SetStrengthen(GetStrengthen()))
		{
			ErrorLogStream << " equip SetStrengthen failed!" << LogStream::eos;
			return false;
		}

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
	else if (GetType() == ITEM_FASHION)
	{
		//设置默认时装属性
		if (GetFashionAttrId() == 0)
		{
			if (!SetFashionDefaultAttr())
			{
				ErrorLogStream << "FashionEquip SetFashionDefaultAttr failed! id:" << GetDataID() << LogStream::eos;
				return false;
			}
		}

		//设置属性data
		EquipAttrDataEntry* attrData = EquipAttrDataMgr::Instance()->FindEntry(GetFashionAttrId());
		if (!attrData)
		{
			ErrorLogStream << "FashionEquip attrData is null! id:" << GetDataID() << " attrid:" << GetFashionAttrId() << LogStream::eos;
			return false;
		}
		SetAttrDataEntry(attrData);

		//计算时装基础属性
		if (!GenDataAttrs())
		{
			ErrorLogStream << "FashionEquip GenBaseAttrs failed! id:" << GetDataID() << LogStream::eos;
			return false;
		}
		return true;
	}
	else
	{
		return true;
	}
}

bool Item::GenDataAttrs()
{
	if (!GetAttrDataEntry())
		return false;

	SetPhyAtk(GetAttrDataEntry()->atk);
	SetMagAtk(GetAttrDataEntry()->magicAtk);
	SetPhyDef(GetAttrDataEntry()->def);
	SetMagDef(GetAttrDataEntry()->magicDef);
	SetStrenth(GetAttrDataEntry()->strenth);
	SetStamina(GetAttrDataEntry()->stamina);
	SetSpirit(GetAttrDataEntry()->spirit);
	SetIntellect(GetAttrDataEntry()->intellect);
	
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

float Item::GetStrPropScore()
{
#ifdef _DEBUG
	DebugLogStream << "---StrengthProp Start---" << LogStream::eos;
#endif
	float score = 0;
	score = \
		(GetScoreByPropType(ISP_DISPHYATK) + \
		GetScoreByPropType(ISP_DISMAGATK) + \
		GetScoreByPropType(ISP_PHYDMGRDC) + \
		GetScoreByPropType(ISP_MAGDMGRDC));

#ifdef _DEBUG
	DebugLogStream << "disPhy:" << m_DisPhyAtk \
		<< ",disMagAtk:" << m_DisMagAtk \
		<< ",disPhyDef:" << m_DisPhyDef \
		<< ",disMagDef:" << m_DisMagDef \
		<< ",StrPropScore:" << score << LogStream::eos;

	DebugLogStream << "---StrengthProp End---" << LogStream::eos;
#endif
	return score;
}

float Item::GetMagicPropScore()
{
#ifdef _DEBUG
	DebugLogStream << "---MagicProp Start---" << LogStream::eos;
#endif

	UInt32 magicCardId = m_AddMagic.GetMagicCard();
	MagicCardData* data = MagicCardDataMgr::Instance()->FindEntry(magicCardId);
	if (!data) return 0;

	float score = 0;

	for (size_t i = 0; i < data->props.size(); ++i)
	{
		MagicCardProp& mp = data->props[i];
		ItemScoreProp sp = TransRandPropType((ItemRandProp)mp.propType);

		if (sp >= ISP_LIGHT && sp <= ISP_ZZKX)
		{
			continue;
		}

		EquipValueSceorData* scoreData = EquipValueSceorDataMgr::Instance()->FindEntry((UInt32)sp);
		if (!scoreData)		return 0;

		UInt32 fix = 1;
		if (sp >= ISP_STR && sp <= ISP_SPR)
		{
			fix = 1000;
		}
		else if (sp >= ISP_ATKSPD && sp <= ISP_MISS)
		{
			fix = 10;
		}
		
		float value = (float)mp.propValue / fix;
		float rate = (float)scoreData->value / 1000;
		if (fix == 10)
		{
			UInt16 lv = GetNeedLevel() < 5 ? 5 : GetNeedLevel();
			score += value * rate * lv / 10;
#ifdef _DEBUG
			DebugLogStream << "Extra MagicPropType:" << GetTypeTransName(sp) << ",Value:" << value << ",Rate:" << rate << LogStream::eos;
#endif
		}
		else
		{
			score += value * rate;
#ifdef _DEBUG
			DebugLogStream << "MagicPropType:" << GetTypeTransName(sp) << ",Value:" << value << ",Rate:" << rate << LogStream::eos;
#endif
		}
	}

#ifdef _DEBUG
 	DebugLogStream << "MagicPropScore = " << score << LogStream::eos;
 	DebugLogStream << "---MagicProp End---" << LogStream::eos;
#endif
	return score;
}

float Item::GetPreciousBeadPropScore()
{
	float total_score = 0;
	for (UInt32 i = 1; i < PRECBEAD_MAXNUM; i++)
	{
		UInt32 preciousBeadId = m_PreciousBeadHoles.GetPreciousBeadId(i);
		PreciousBeadDataEntry* dataEntry = PreciousBeadDataEntryMgr::Instance()->FindEntry(preciousBeadId);
		if (!dataEntry)
		{
			ErrorLogStream << "Can not find bead data entry(id: " << preciousBeadId << ")." << LogStream::eos;
			return 0;
		}

		float score = 0;

		for (size_t i = 0; i < dataEntry->props.size(); ++i)
		{
			PreciousBeadProp& preciousBeadProp = dataEntry->props[i];
			ItemScoreProp sp = TransRandPropType((ItemRandProp)preciousBeadProp.propType);

			if (sp >= ISP_LIGHT && sp <= ISP_ZZKX)
			{
				continue;
			}

			EquipValueSceorData* scoreData = EquipValueSceorDataMgr::Instance()->FindEntry((UInt32)sp);
			if (!scoreData)
			{
				ErrorLogStream << "EquipValueSceorData(id: " << sp << ") ptr is null!" << LogStream::eos;
				return 0;
			}

			UInt32 fix = 1;

			if (sp >= ISP_STR && sp <= ISP_SPR)
			{
				fix = 1000;
			}
			else if (sp >= ISP_ATKSPD && sp <= ISP_MISS)
			{
				fix = 10;
			}

			float value = (float)preciousBeadProp.propValue / fix;
			float rate = (float)scoreData->value / 1000;

			if (fix == 10)
			{
				UInt16 lv = GetNeedLevel() < 5 ? 5 : GetNeedLevel();
				score += value * rate * lv / 10;

#ifdef _DEBUG
				InfoLogStream << "PreciousBead(id: " << preciousBeadId << ") property(score: " << score << "): ItemScoreProp(" << GetTypeTransName(sp)
					<< "), fix(" << fix << "), rate(" << rate << "), level(" << lv << ")." << LogStream::eos;
#endif			
			}
			else
			{
				score += value * rate;
#ifdef _DEBUG
				InfoLogStream << "PreciousBead(id: " << preciousBeadId << ") property(score: " << score << "): ItemScoreProp(" << GetTypeTransName(sp)
					<< "), fix(" << fix << "), rate(" << rate << ")." << LogStream::eos;
#endif	
			}
		}

#ifdef _DEBUG
		InfoLogStream << "PreciousBead(id: " << preciousBeadId << ") property score = " << score << "." << LogStream::eos;
#endif
		total_score += score;
	}

#ifdef _DEBUG
	InfoLogStream << "PreciousBead PropScore Item(id: " << this->GetID() << ") property total_score = " << total_score << "." << LogStream::eos;
#endif

	return total_score;
}

float Item::GetScoreByPropType(ItemScoreProp type)
{
	float score = 0;

	EquipValueSceorData* scoreData = EquipValueSceorDataMgr::Instance()->FindEntry((UInt32)type);
	if (!scoreData)		return 0;
	
	EquipAttrDataEntry* attrDate = GetAttrDataEntry();
	if (!attrDate)	return 0;

	ItemDataEntry* data = GetDataEntry();
	if (!data)	return 0;
	
	float rate = (float)scoreData->value / 1000;
	float value = 0.0f;

	bool extra = false;

	switch (type)
	{
	case ISP_STR:		value = (float)GetStrenth() / 1000;	break;
	case ISP_INT:		value = (float)GetIntellect() / 1000;	break;
	case ISP_STAM:		value = (float)GetStamina() / 1000;	break;
	case ISP_SPR:		value = (float)GetSpirit() / 1000;	break;
	case ISP_HP:		value = (float)attrDate->HPMax;		break;
	case ISP_MP:		value = (float)attrDate->MPMax;		break;
	case ISP_HPRECVR:	value = (float)attrDate->HPRecover;		break;
	case ISP_MPRECVR:	value = (float)attrDate->MPRecover;		break;
	case ISP_PHYATK:	value = (float)GetPhyAtk();		break;
	case ISP_MAGATK:	value = (float)GetMagAtk();		break;
	case ISP_PHYDEF:	value = (float)GetPhyDef();		break;
	case ISP_MAGDEF:	value = (float)GetMagDef();		break;
	case ISP_DISPHYATK:	value = (float)m_DisPhyAtk;		break;
	case ISP_DISMAGATK:	value = (float)m_DisMagAtk;		break;
	case ISP_PHYDMGRDC:	value = (float)m_DisPhyDef;		break;
	case ISP_MAGDMGRDC:	value = (float)m_DisMagDef;		break;
	case ISP_ATKSPD:	value = (float)(data->baseAtkSpeed + attrDate->AtkSpeedRate) / 10;	extra = true;		break;
	case ISP_MAGSPD:	value = (float)(attrDate->FireSpeedRate) /10;	extra = true;		break;
	case ISP_MVSPD:		value = (float)(attrDate->BattleSpeedRate) /10;	extra = true;		break;
	case ISP_PHYCRT:	value = (float)(attrDate->PhysicCrit) / 10;		extra = true;		break;
	case ISP_MAGCRT:	value = (float)(attrDate->MagicCrit) / 10;		extra = true;		break;
	case ISP_HIT:		value = (float)(attrDate->HitRate) / 10;		extra = true;			break;
	case ISP_MISS:		value = (float)(attrDate->AvoidRate) / 10;	extra = true;			break;
	case ISP_YZ:		value = (float)(attrDate->Spasticity);			break;
	default:		return 0;
		
	}

	if (extra)
	{
		UInt16 lv = GetNeedLevel() < 5 ? 5 : GetNeedLevel();
		score = value * rate * lv / 10;

#ifdef _DEBUG
		if (value > 0)
			DebugLogStream << "Extra BaseType:" << GetTypeTransName(type) << ",value:" << value << ",rate:" << rate << ",lv:" << lv << LogStream::eos;
#endif
	}
	else
	{
		score = value * rate;

#ifdef _DEBUG
		if (value > 0)
			DebugLogStream << "BaseType:" << GetTypeTransName(type) << ",value:" << value << ",rate:" << rate << LogStream::eos;
#endif
	}

#ifdef _DEBUG
	if (score > 0)
		DebugLogStream << "BasePropType:" << GetTypeTransName(type) << ",BasePropScore:" << score << LogStream::eos;
#endif

	return score;
}

float Item::GetPartRate()
{
#ifdef _DEBUG
	DebugLogStream << "---MagicProp Start---" << LogStream::eos;
#endif

	ItemScoreProp type;

	//装备属性表必须有对应数据项,否则不应有对应关系,直接返回1.0   by huchenhui 2018.03.28
	switch (GetThirdType())
	{
	case TT_ITEM_HUGESWORD:		type = ISP_HSWORD;	break;
	case TT_ITEM_KATANA:		type = ISP_TD;	break;
	case TT_ITEM_SHORTSWORD:	type = ISP_DUANJIAN;	break;
	case TT_ITEM_BEAMSWORD:		type = ISP_GUANGJIAN;   break;
	case TT_ITEM_REVOLVER:		type = ISP_ZL;	break;
	case TT_ITEM_CROSSBOW:		type = ISP_NUJIAN;		break;
	case TT_ITEM_HANDCANNON:	type = ISP_SP;	break;
	case TT_ITEM_RIFLE:			type = ISP_BUQIANG;		break;
	case TT_ITEM_STAFF:			type = ISP_FZ;	break;
	case TT_ITEM_WAND:			type = ISP_MZ;	break;
	//case TT_ITEM_CLAW:			type = ISP_CLAW;	break;
	//case TT_ITEM_BIKAI:			type = ISP_BIKAI;	break;

	case TT_ITEM_CLOTH:			type = ISP_BJ;	break;
	case TT_ITEM_SKIN:			type = ISP_PJ;	break;
	case TT_ITEM_LIGHT:			type = ISP_QINJIA;	break;
	case TT_ITEM_HEAVY:			type = ISP_ZHONGJIA;	break;
	case TT_ITEM_PLATE:			type = ISP_BANJIA;	break;
	case TT_ITEM_OFG:			type = ISP_OFG;  break;

	case TT_ITEM_SICKLE:		type = ISP_SICKLE;	break;
	case TT_ITEM_TOTEM:			type = ISP_TOTEM;	break;
	case TT_ITEM_AXE:			type = ISP_AXE;		break;
	case TT_ITEM_BEADS:			type = ISP_BEADS;	break;
	case TT_ITEM_CROSS:			type = ISP_CROSS;	break;
		
	default:	return 1.0;
	}

	EquipValueSceorData* scoreData = EquipValueSceorDataMgr::Instance()->FindEntry((UInt32)type);
	if (!scoreData)		return 0;

#ifdef _DEBUG
	DebugLogStream << "part = " << GetThirdType() << ",partRate = " << (float)scoreData->value / 1000 << LogStream::eos;
#endif
	return (float)scoreData->value / 1000;
}

float Item::GetSkillScore()
{
#ifdef _DEBUG
	DebugLogStream << "---SkillScore Start---" << LogStream::eos;
#endif
	EquipAttrDataEntry* attrDate = GetAttrDataEntry();
	if (!attrDate)	return 0;

#ifdef _DEBUG
	DebugLogStream << "skillScore = " << (float)attrDate->skillScore << LogStream::eos;
	DebugLogStream << "---SkillScore End---" << LogStream::eos;
#endif
	return (float)attrDate->skillScore;
}

float Item::GetExPropScore()
{
	Int32 highestAtk = 0;
	Int32 highestStr = 0;
	EquipExProp hiStrType = EquipExProp::EEP_MAX;
	Int32 highestDef = 0;
	EquipExProp hiDefType = EquipExProp::EEP_MAX;
	Int32 highestAR = 0;
	EquipAbnormalResist hiARType = EquipAbnormalResist::EAR_MAX;

	UInt32 numOfAtk = 0;
	UInt32 numOfStr = 0;
	UInt32 numOfDef = 0;
	UInt32 numOfAR = 0;

	Int32		AtkPropEx[EEP_MAX];
	Int32		StrPropEx[EEP_MAX];
	Int32		DefPropEx[EEP_MAX];
	Int32		AbnormalResists[EAR_MAX];

	memcpy(AtkPropEx, GetAttrDataEntry()->AtkPropEx, sizeof(Int32)*EEP_MAX);
	memcpy(StrPropEx, GetAttrDataEntry()->StrPropEx, sizeof(Int32)*EEP_MAX);
	memcpy(DefPropEx, GetAttrDataEntry()->DefPropEx, sizeof(Int32)*EEP_MAX);
	memcpy(AbnormalResists, GetAttrDataEntry()->AbnormalResists, sizeof(Int32)*EAR_MAX);

	for (int i = 0; i < EEP_MAX; ++i)
	{
		StrPropEx[i] = m_StrPropEx[i];
		DefPropEx[i] = m_DefPropEx[i];
	}
	for (int i = 0; i < EAR_MAX; ++i)
	{
		AbnormalResists[i] = m_AbnormalResists[i];
	}

	UInt32 magicCardId = m_AddMagic.GetMagicCard();
	MagicCardData* data = MagicCardDataMgr::Instance()->FindEntry(magicCardId);
	if (data)
	{
		for (size_t i = 0; i < data->props.size(); ++i)
		{
			MagicCardProp& mp = data->props[i];
			ItemScoreProp sp = TransRandPropType((ItemRandProp)mp.propType);
			if (sp >= ISP_LIGHT && sp <= ISP_DARK)
			{
				size_t idx = sp - ISP_LIGHT;
				AtkPropEx[idx] = 1;
			}
			else if (sp >= ISP_LIGHT_ATK && sp <= ISP_DARK_ATK)
			{
				size_t idx = sp - ISP_LIGHT_ATK;
				//StrPropEx[idx] = mp.propValue > StrPropEx[idx] ? mp.propValue : StrPropEx[idx];
				StrPropEx[idx] = StrPropEx[idx] + mp.propValue;
			}
			else if (sp >= ISP_LIGHT_DEF && sp <= ISP_DARK_DEF)
			{
				size_t idx = sp - ISP_LIGHT_DEF;
				//DefPropEx[idx] = mp.propValue > DefPropEx[idx] ? mp.propValue : DefPropEx[idx];
				DefPropEx[idx] = DefPropEx[idx] + mp.propValue;
			}
			else if (sp >= ISP_GDKX && sp <= ISP_ZZKX)
			{
				size_t idx = sp - ISP_GDKX;
				//AbnormalResists[idx] = mp.propValue > AbnormalResists[idx] ? mp.propValue : AbnormalResists[idx];
				AbnormalResists[idx] = AbnormalResists[idx] + mp.propValue;
			}
			else {
			}
		}
	}

	/*UInt32 preciousBeadId = m_AddPreciousBead.GetPreciousBeadId();
	PreciousBeadDataEntry* dataEntry = PreciousBeadDataEntryMgr::Instance()->FindEntry(preciousBeadId);
	if (dataEntry)
	{
		for (size_t i = 0; i < dataEntry->props.size(); ++i) 
		{
			PreciousBeadProp& preciousBeadProp = dataEntry->props[i];
			ItemScoreProp sp = TransRandPropType((ItemRandProp)preciousBeadProp.propType);
			if (sp >= ISP_LIGHT && sp <= ISP_DARK)
			{
				Int8 idx = sp - ISP_LIGHT;
				AtkPropEx[idx] = 1;
			}
			else if (sp >= ISP_LIGHT_ATK && sp <= ISP_DARK_ATK)
			{
				Int8 idx = sp - ISP_LIGHT_ATK;
				//StrPropEx[idx] = preciousBeadProp.propValue > StrPropEx[idx] ? preciousBeadProp.propValue : StrPropEx[idx];
				StrPropEx[idx] = StrPropEx[idx] + preciousBeadProp.propValue;
			}
			else if (sp >= ISP_LIGHT_DEF && sp <= ISP_DARK_DEF)
			{
				Int8 idx = sp - ISP_LIGHT_DEF;
				//DefPropEx[idx] = preciousBeadProp.propValue > DefPropEx[idx] ? preciousBeadProp.propValue : DefPropEx[idx];
				DefPropEx[idx] = DefPropEx[idx] + preciousBeadProp.propValue;
			}
			else if (sp >= ISP_GDKX && sp <= ISP_ZZKX)
			{
				Int8 idx = sp - ISP_GDKX;
				//AbnormalResists[idx] = preciousBeadProp.propValue > AbnormalResists[idx] ? preciousBeadProp.propValue : AbnormalResists[idx];
				AbnormalResists[idx] = AbnormalResists[idx] + preciousBeadProp.propValue;
			}
			else {
			}
		}
	}*/

	for (UInt32 i = 1; i < PRECBEAD_MAXNUM; i++)
	{
		UInt32 preciousBeadId = m_PreciousBeadHoles.GetPreciousBeadId(i);
		PreciousBeadDataEntry* dataEntry = PreciousBeadDataEntryMgr::Instance()->FindEntry(preciousBeadId);
		if (!dataEntry)
		{
			continue;
		}
		for (size_t i = 0; i < dataEntry->props.size(); ++i)
		{
			PreciousBeadProp& preciousBeadProp = dataEntry->props[i];
			ItemScoreProp sp = TransRandPropType((ItemRandProp)preciousBeadProp.propType);
			if (sp >= ISP_LIGHT && sp <= ISP_DARK)
			{
				size_t idx = sp - ISP_LIGHT;
				AtkPropEx[idx] = 1;
			}
			else if (sp >= ISP_LIGHT_ATK && sp <= ISP_DARK_ATK)
			{
				size_t idx = sp - ISP_LIGHT_ATK;
				//StrPropEx[idx] = preciousBeadProp.propValue > StrPropEx[idx] ? preciousBeadProp.propValue : StrPropEx[idx];
				StrPropEx[idx] = StrPropEx[idx] + preciousBeadProp.propValue;
			}
			else if (sp >= ISP_LIGHT_DEF && sp <= ISP_DARK_DEF)
			{
				size_t idx = sp - ISP_LIGHT_DEF;
				//DefPropEx[idx] = preciousBeadProp.propValue > DefPropEx[idx] ? preciousBeadProp.propValue : DefPropEx[idx];
				DefPropEx[idx] = DefPropEx[idx] + preciousBeadProp.propValue;
			}
			else if (sp >= ISP_GDKX && sp <= ISP_ZZKX)
			{
				size_t idx = sp - ISP_GDKX;
				//AbnormalResists[idx] = preciousBeadProp.propValue > AbnormalResists[idx] ? preciousBeadProp.propValue : AbnormalResists[idx];
				AbnormalResists[idx] = AbnormalResists[idx] + preciousBeadProp.propValue;
			}
			else {
			}
		}
	}

	//第一,二,三类附加属性攻击
	for (int i = 0; i < EEP_MAX; ++i)
	{
		if (AtkPropEx[i] > 0)	numOfAtk++;
		if (StrPropEx[i] > 0)	numOfStr++;
		if (DefPropEx[i] > 0)	numOfDef++;

		if (AtkPropEx[i] > highestAtk)
		{
			highestAtk = AtkPropEx[i];
		}

		if (StrPropEx[i] > highestStr)
		{
			highestStr = StrPropEx[i];
			hiStrType = (EquipExProp)i;
		}

		if (DefPropEx[i] > highestDef)
		{
			highestDef = DefPropEx[i];
			hiDefType = (EquipExProp)i;
		}
	}

	//第四类异常抗性
	for (int i = 0; i < EAR_MAX; ++i)
	{
		Int32 curAR = AbnormalResists[i] + GetAbnormalResistsTotal();
		if (curAR <= 0)	continue;

		numOfAR++;
		if (curAR > highestAR)
		{
			highestAR = curAR;
			hiARType = (EquipAbnormalResist)i;
		}
	}
	//第一类属性评分系数最大值
	UInt32 maxFirstValue = 0;
	for (int i = ISP_LIGHT; i <= ISP_DARK; ++i)
	{
		EquipValueSceorData* scoreData = EquipValueSceorDataMgr::Instance()->FindEntry(i);
		if (!scoreData)
		{
			continue;
		}

		if (scoreData->value > maxFirstValue)
		{
			maxFirstValue = scoreData->value;
		}
	}

	float firstScore = 0.0f;
	float secondScore = 0.0f;
	float thirdScore = 0.0f;
	float fourthScore = 0.0f;

	if (numOfAtk > 0)
	{
		firstScore = (float)highestAtk * (float)maxFirstValue/1000 * (1 + (float)(numOfAtk - 1) * 0.1);
	}
	if (numOfStr > 0)
	{
		EquipValueSceorData* scoreData = EquipValueSceorDataMgr::Instance()->FindEntry(TransExProp(EquipExType::EET_STR, hiStrType));
		if (scoreData)
		{
			secondScore = (float)highestStr * (float)scoreData->value/1000 * (1 + (float)(numOfStr - 1) * 0.1);
		}
	}
	if (numOfDef > 0)
	{
		EquipValueSceorData* scoreData = EquipValueSceorDataMgr::Instance()->FindEntry(TransExProp(EquipExType::EET_DEF, hiDefType));
		if (scoreData)
		{
			thirdScore = (float)highestDef * (float)scoreData->value/1000 * (1 + (float)(numOfDef - 1) * 0.1);
		}
	}
	if (numOfAR > 0)
	{
		EquipValueSceorData* scoreData = EquipValueSceorDataMgr::Instance()->FindEntry(TransExProp(EquipExType::EET_AR, hiARType));
		if (scoreData)
		{
			fourthScore = (float)highestAR * (float)scoreData->value/1000 * (1 + (float)(numOfAR - 1) * 0.1);
		}
	}
	
	auto expProbScore = firstScore + secondScore + thirdScore + fourthScore;
#ifdef _DEBUG
	DebugLogStream << "item(" << GetID() << ") ExpPropScore(" << expProbScore << ")." << LogStream::eos;
#endif
	return expProbScore;
}

ItemScoreProp Item::TransExProp(EquipExType type, UInt32 prop)
{
	if (EquipExType::EET_ATK == type)
	{
		if (EquipExProp::EEP_LIGHT == (EquipExProp)prop)	return ISP_LIGHT;
		else if (EquipExProp::EEP_FIRE == (EquipExProp)prop) return ISP_FIRE;
		else if (EquipExProp::EEP_ICE == (EquipExProp)prop) return ISP_ICE;
		else if (EquipExProp::EEP_DARK == (EquipExProp)prop) return ISP_DARK;
	}
	else if (EquipExType::EET_STR == type)
	{
		if (EquipExProp::EEP_LIGHT == (EquipExProp)prop)	return ISP_LIGHT_ATK;
		else if (EquipExProp::EEP_FIRE == (EquipExProp)prop) return ISP_FIRE_ATK;
		else if (EquipExProp::EEP_ICE == (EquipExProp)prop) return ISP_ICE_ATK;
		else if (EquipExProp::EEP_DARK == (EquipExProp)prop) return ISP_DARK_ATK;
	}
	else if (EquipExType::EET_DEF == type)
	{
		if (EquipExProp::EEP_LIGHT == (EquipExProp)prop)	return ISP_LIGHT_DEF;
		else if (EquipExProp::EEP_FIRE == (EquipExProp)prop) return ISP_FIRE_DEF;
		else if (EquipExProp::EEP_ICE == (EquipExProp)prop) return ISP_ICE_DEF;
		else if (EquipExProp::EEP_DARK == (EquipExProp)prop) return ISP_DARK_DEF;
	}
	else if (EquipExType::EET_AR == type)
	{
		if (EquipAbnormalResist::EAR_STUN == (EquipAbnormalResist)prop)	return ISP_XYKX;
		else if (EquipAbnormalResist::EAR_SLEEP == (EquipAbnormalResist)prop) return ISP_SLKX;
		else if (EquipAbnormalResist::EAR_FROZEN == (EquipAbnormalResist)prop) return ISP_BDKX;
		else if (EquipAbnormalResist::EAR_STONE == (EquipAbnormalResist)prop) return ISP_SHKX;
		else if (EquipAbnormalResist::EAR_STRAIN == (EquipAbnormalResist)prop) return ISP_SFKX;
		else if (EquipAbnormalResist::EAR_POISON == (EquipAbnormalResist)prop) return ISP_ZDKX;
		else if (EquipAbnormalResist::EAR_BURN == (EquipAbnormalResist)prop) return ISP_ZSKX;
		else if (EquipAbnormalResist::EAR_BLEEDING == (EquipAbnormalResist)prop) return ISP_CXKX;
		else if (EquipAbnormalResist::EAR_SPEED_DOWN == (EquipAbnormalResist)prop) return ISP_JSKX;
		else if (EquipAbnormalResist::EAR_BLIND == (EquipAbnormalResist)prop) return ISP_SMKX;
		else if (EquipAbnormalResist::EAR_FLASH == (EquipAbnormalResist)prop) return ISP_GDKX;
		else if (EquipAbnormalResist::EAR_CURSE == (EquipAbnormalResist)prop) return ISP_ZZKX;
		else if (EquipAbnormalResist::EAR_CONFUNSE == (EquipAbnormalResist)prop) return ISP_HLKX;
	}
	
	return ISP_NONE;
}

Item* Item::CreateItem(UInt32 itemid)
{
	ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(itemid);
	if(dataEntry == NULL) return NULL;

	if (1 == dataEntry->isAbandon)	return NULL;
	
	Item* item = new Item();

	item->SetDataID(itemid);
	item->SetDataEntry(dataEntry);
	
	item->SetName(dataEntry->name);
	item->SetSealState(dataEntry->seal);
	item->SetIsTreas(dataEntry->isTreasure && SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_TREAS));
	return item;
}

void Item::DestroyItem(Item*& item)
{
	CL_SAFE_DELETE(item);
}

void Item::PackWatchData(Avalon::NetOutputStream& stream)
{
	Encode(stream, SST_WATCH, false);
}


// bool Item::FitOccu(UInt8 oc)
// {
// 	if (m_pDataEntry->occu == 0 || m_pDataEntry->occu / 10 == oc / 10)
// 		return true;
// 	else
// 		return false;
// }

bool Item::SetMagic(MagicCardData* data)
{
// 	if (!data)
// 		return false;
// 
// 	if (data->props.size() + data->buffs.size() > MAX_MAGIC_NUM)
// 		return false;
// 
// 	m_AddMagic.ClearMagicProp();
// 
// 	for (size_t i = 0; i < data->props.size(); ++i)
// 	{
// 		m_AddMagic.AddMagicProp(1, data->props[i].propType, data->props[i].propValue);
// 	}
// 	
// 	for (size_t i = 0; i < data->buffs.size(); ++i)
// 	{
// 		m_AddMagic.AddMagicProp(2, data->buffs[i], 0);
// 	}

	if (!data)
		return false;
	m_AddMagic.SetMagciCard(data->id);
	return true;
}

UInt32 Item::GetMagicCardId()
{
	return m_AddMagic.GetMagicCard();
}

UInt8  Item::GetMagicCardLv()
{
	return m_AddMagic.GetMagicCardLev();
}

bool Item::SetPreciousBead(PreciousBeadDataEntry* data)
{
	if (!data)
	{
		ErrorLogStream << "MagicCardData ptr is null!" << LogStream::eos;
		return false;
	}

	m_AddPreciousBead.SetPreciousBeadId(data->itemId);
	m_AddPreciousBeadTimeStamp = Avalon::Time::CurrentTime().Sec();
	return true;
}

UInt32 Item::GetPreciosBeadId()
{
	return m_AddPreciousBead.GetPreciousBeadId();
}

bool Item::SetTransferStone(ItemDataEntry* data)
{
	if (!data)
	{
		ErrorLogStream << "SetTransferStone ItemDataEntry ptr is null!" << LogStream::eos;
		return false;
	}
	m_TransferStone = data->id;
	return true;
}

UInt32 Item::GetTransferStoneid()
{
	return m_TransferStone;
}

bool Item::ResetTransferStone()
{
	if (m_TransferStone != 0)
	{
		m_TransferStone = 0;
		return true;
	}
	return false;
}

void Item::GenPreciousBeadHoles()
{
	ItemDataEntry* entry = GetDataEntry();
	if (!entry || (entry->type != ITEM_EQUIP && entry->type != ITEM_TITLE)) return;

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

void Item::GenPreciousBeadHoleLoaded()
{
	if ((this->GetType() == ITEM_EQUIP || this->GetType() == ITEM_TITLE) && this->m_PreciousBeadHoles.HolesNum() == 0
		&& (this->GetDataEntry()->PreciousBeadHole1.size()>0 || this->GetDataEntry()->PreciousBeadHole2.size()>0))
	{
		this->GenPreciousBeadHoles();
	}
}

bool Item::GenPreciousBeadAddtionAttri()
{
	PreciousBeadDataEntry* dataEntry = PreciousBeadDataEntryMgr::Instance()->FindEntry(this->GetDataID());
	if (dataEntry && dataEntry->addtionBuffPro > 0 && dataEntry->buffRandomGroupId > 0)
	{
		bool baddtionBuffPro = Avalon::Random::SelectInTenThousand(dataEntry->addtionBuffPro);
		if (baddtionBuffPro)
		{
			m_Param2 = PrecBeadAddBuffRandomDataEntryMgr::Instance()->RandomBuffIdByBuffGroupId(dataEntry->buffRandomGroupId);
			if (m_Param2 > 0) return true;
		}
	}
	return false;
}

bool Item::IsMountedPrecBead()
{
	return this->m_PreciousBeadHoles.GetPrecBeadNum() > 0;
}

UInt32 Item::GetFirstPreciosBeadId()
{
	return  this->m_PreciousBeadHoles.GetPreciousBeadId(1);
}

bool Item::IsItemLock(UInt32 _lockType)
{
	return m_lockType & _lockType;
}

void Item::SetItemLock(UInt32 _lockType) 
{
	m_lockType = m_lockType | _lockType;
}

void Item::ClearItemLock(UInt32 _lockType)
{
	m_lockType = m_lockType & (~_lockType);
}

void Item::SetAuctionCoolTimeStamp(UInt32 timeStamp)
{
	this->m_AuctionCoolTimeStamp = timeStamp;
}

UInt32 Item::GetAuctionCoolTimeStamp()
{
	return m_AuctionCoolTimeStamp;
}

bool Item::IsInAuctionCoolTimeStamp()
{
	return this->m_AuctionCoolTimeStamp > 0 && this->m_AuctionCoolTimeStamp > (UInt32)CURRENT_TIME.Sec();
}

void Item::CalcItemScore(Player* player)
{
	if (!SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_ADVENTURE_TEAM_SORTLIST))
	{
		return;
	}

	if (!GetDataEntry()) return;

	if (!player)
	{
		return;
	}

	UInt32 itemScore = 0;
	UInt32 baseScore = 0;

	switch ((ItemType)GetType())
	{
	case ITEM_EQUIP:
	{
		EquipBaseScoreModDataEntry* baseScoreEntry = EquipBaseScoreModDataEntryMgr::Instance()->GetEntry(GetSubType(), GetQuality());
		if (baseScoreEntry == NULL)
		{
			ErrorLogStream << "calc item score err, EquipBaseScoreModDataEntry not find subtype : " << GetSubType() << ", quality : " << GetQuality() << LogStream::eos;
			return;
		}

		//攻击力评分
		EquipValueSceorData* attackScoreData = EquipValueSceorDataMgr::Instance()->FindEntry((UInt32)ISP_ATTACK);
		if (attackScoreData == NULL)
		{
			ErrorLogStream << "calc item score err, EquipValueSceorData not find id  : " << ISP_ATTACK << LogStream::eos;
			return;
		}
		UInt32	attackScore = (GetNeedLevel() * (EQ_BASESC_PARAM(baseScoreEntry->attrMod2)) + EQ_BASESC_PARAM(baseScoreEntry->attrMod1)) * EQ_BASESC_PARAM(attackScoreData->value);

		//四维评分
		EquipValueSceorData* powerScoreData = EquipValueSceorDataMgr::Instance()->FindEntry((UInt32)ISP_POWER);
		if (powerScoreData == NULL)
		{
			ErrorLogStream << "calc item score err, EquipValueSceorData not find id  : " << ISP_POWER << LogStream::eos;
			return;
		}
		UInt32  powerScore = (EQ_BASESC_PARAM(baseScoreEntry->powerMod1) + ceil((float)GetNeedLevel() / 5) * EQ_BASESC_PARAM(baseScoreEntry->powerMod2) + GetNeedLevel() % 5 * EQ_BASESC_PARAM(baseScoreEntry->powerMod3))
			* EQ_BASESC_PARAM(powerScoreData->value);


		//装备防御评分
		EquipValueSceorData* reduceScoreData = EquipValueSceorDataMgr::Instance()->FindEntry((UInt32)ISP_REDUCE);
		if (reduceScoreData == NULL)
		{
			ErrorLogStream << "calc item score err, EquipValueSceorData not find id  : " << ISP_REDUCE << LogStream::eos;
			return;
		}
		UInt32 eqDefScore = 0;
		eqDefScore = (GetNeedLevel() + EQ_BASESC_PARAM(baseScoreEntry->defMod1)) * EQ_BASESC_PARAM(baseScoreEntry->defMod2) * EQ_BASESC_PARAM(reduceScoreData->value);

		//辅助装备强化评分
		UInt32 assistEqStrengScore = 0;
		if (GetSubType() == ST_ASSIST_EQUIP && GetStrengthen() > 0)
		{
			assistEqStrengScore = AssistEqStrengFouerDimAddDataEntryMgr::Instance()->GetEqScore(GetStrengthen(), GetNeedLevel(), GetQuality(), GetQualityColor2());
		}

		//增幅评分
		UInt32 enhanceEqScore = 0;
		//if (GetEquipType() == EQUIP_RED)
		//{
		//	auto* dataEntry = EquipEnhanceAttributeDataEntryMgr::Instance()->FindEntry(GetNeedLevel(), GetEnhance(), GetQuality());
		//	if (dataEntry == nullptr)
		//	{
		//		ErrorLogStream << "calc item score err, EquipEnhanceAttributeDataEntry not level : " << GetNeedLevel() << ", enhance : " << GetEnhance() << ", quality : " << GetQuality() << LogStream::eos;
		//		return;
		//	}
		//	enhanceEqScore = dataEntry->eqScore;
		//}

		//装备基础评分
		baseScore = attackScore + powerScore + eqDefScore + assistEqStrengScore + enhanceEqScore;

		InfoLogStream << PLAYERINFO(player) << "calc equip baseScore id : " << GetDataID() << ", attackScore : " << attackScore << ", powerScore : " << powerScore
			<< ", eqDefScore : " << eqDefScore << ", assistEqStrengScore : " << assistEqStrengScore << ", enhanceEqScore : " << enhanceEqScore << ", baseScore : " << baseScore << LogStream::eos;

		UInt32 qualityLv = (UInt32)GetQualityLv();
		UInt32 qualityLvCoefficient = 0;
		if (qualityLv == 100)
		{
			qualityLvCoefficient = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_MAX_QUALITY_LEVEL_SCORE_COEFFICIENT);
		}
		else
		{
			qualityLvCoefficient = qualityLv + 1;
		}

		UInt32 calcVar1 = 0;
		if (qualityLv == 100)
		{
			calcVar1 = baseScore * qualityLvCoefficient / 100;
		}
		else
		{
			calcVar1 = baseScore * 20 / 100 * qualityLvCoefficient / 100 + baseScore * 80 / 100;
		}

		//UInt32 calcVar1 = baseScore * 20 / 100 * qualityLvCoefficient / 100 + baseScore * 80 / 100;

		UInt32 eqStrengWsAttackScore = 0;	//装备强化无视攻击评分
		UInt32 eqStrengWsDefScore = 0;		//装备强化无视防御评分
		UInt32 eqStrengDecHurtScore = 0;	//装备强化减伤评分

		//装备强化无视攻击评分, 装备强化减伤评分(当装备为防具类型和首饰时计算)
		if (IsDefend(GetSubType()) || IsOrnaments(GetSubType()))
		{
			//装备强化无视攻击评分
			if (GetStrengthen() >= baseScoreEntry->strengthMod2.size())
			{
				ErrorLogStream << "calc err strengthen >= baseScoreEntry->strengthMod2.size() subtype : " << GetSubType() << ", quality : " << GetQuality() << ", strengthen : " << GetStrengthen()
					<< " baseScoreEntry->strengthMod2.size() : " << (UInt32)baseScoreEntry->strengthMod2.size() << LogStream::eos;
				return;
			}
			EquipValueSceorData* scoreData = EquipValueSceorDataMgr::Instance()->FindEntry((UInt32)ISP_DISPHYREDUCE);
			if (!scoreData)
			{
				ErrorLogStream << "calc item score err, EquipValueSceorData not find id  : " << ISP_DISPHYREDUCE << LogStream::eos;
				return;
			}
			eqStrengWsAttackScore = (GetNeedLevel() + EQ_BASESC_PARAM(baseScoreEntry->strenthQualityMod1)) / 8
				* EQ_BASESC_PARAM(baseScoreEntry->strengthMod2[GetStrengthen()]) * EQ_BASESC_PARAM(baseScoreEntry->strenthQualityMod2)
				* EQ_BASESC_PARAM(scoreData->value);

			//装备强化减伤评分
			if (GetStrengthen() >= baseScoreEntry->strengthMod3.size())
			{
				ErrorLogStream << "calc err strengthen >= baseScoreEntry->strengthMod3.size() subtype : " << GetSubType() << ", quality : " << GetQuality() << ", strengthen : " << GetStrengthen()
					<< " baseScoreEntry->strengthMod3.size() : " << (UInt32)baseScoreEntry->strengthMod3.size() << LogStream::eos;
				return;
			}
			EquipValueSceorData* reduceScoreData = EquipValueSceorDataMgr::Instance()->FindEntry((UInt32)ISP_REDUCE);
			if (!reduceScoreData)
			{
				ErrorLogStream << "calc item score err, EquipValueSceorData not find id  : " << ISP_REDUCE << LogStream::eos;
				return;
			}
			eqStrengDecHurtScore = (200 * GetNeedLevel() * EQ_BASESC_PARAM(baseScoreEntry->strengthMod3[GetStrengthen()]) / (1 - EQ_BASESC_PARAM(baseScoreEntry->strengthMod3[GetStrengthen()]))) * EQ_BASESC_PARAM(reduceScoreData->value);
		}


		//装备强化无视防御评分（当装备为武器类型时计算）
		if (IsWeapon(GetSubType()))
		{
			if (GetStrengthen() >= baseScoreEntry->strengthMod1.size())
			{
				ErrorLogStream << "calc err strengthen >= baseScoreEntry->strengthMod1.size() subtype : " << GetSubType() << ", quality : " << GetQuality() << ", strengthen : " << GetStrengthen()
					<< " baseScoreEntry->strengthMod1.size() : " << (UInt32)baseScoreEntry->strengthMod1.size() << LogStream::eos;
				return;
			}
			EquipValueSceorData* scoreData = EquipValueSceorDataMgr::Instance()->FindEntry((UInt32)ISP_DISPHYATTACK);
			if (!scoreData)
			{
				ErrorLogStream << "calc item score err, EquipValueSceorData not find id  : " << ISP_DISPHYATTACK << LogStream::eos;
				return;
			}
			eqStrengWsDefScore = (GetNeedLevel() + EQ_BASESC_PARAM(baseScoreEntry->strenthQualityMod1)) / 8
				* EQ_BASESC_PARAM(baseScoreEntry->strengthMod1[GetStrengthen()]) * EQ_BASESC_PARAM(baseScoreEntry->strenthQualityMod2)
				* EQ_BASESC_PARAM(scoreData->value);
		}

		//附加属性评分
		UInt32 itemAdditionalScore = 0;
		EquipAttrDataEntry* equipAttrData = EquipAttrDataMgr::Instance()->FindEntry(GetDataID());
		if (!equipAttrData)
		{
			ErrorLogStream << "calc item score err, EquipAttrDataEntry not find id  : " << GetDataID() << LogStream::eos;
			return;
		}
		EquipValueSceorData* fuJiaScoreData = EquipValueSceorDataMgr::Instance()->FindEntry((UInt32)ISP_FUJIA);
		if (!fuJiaScoreData)
		{
			ErrorLogStream << "calc item score err, EquipValueSceorData not find id  : " << ISP_FUJIA << LogStream::eos;
			return;
		}

		itemAdditionalScore = equipAttrData->skillScore * EQ_BASESC_PARAM(fuJiaScoreData->value);


		//附魔卡评分
		UInt32 magicCardScore = 0;
		if (GetMagicCardId() > 0)
		{
			UInt8 magicCardLv = GetMagicCardLv();
			auto magicCardData = MagicCardDataMgr::Instance()->FindEntry(GetMagicCardId());
			if (!magicCardData)
			{
				ErrorLogStream << "calc item score err, MagicCardData not find id  : " << GetMagicCardId() << LogStream::eos;
			}
			else
			{
				magicCardScore = magicCardData->score + magicCardData->upgradeAddScore * magicCardLv;
			}
		}

		//宝珠评分
		UInt32 preciousBeadScore = 0;
		if (IsMountedPrecBead())
		{
			auto preciousBeadData = PreciousBeadDataEntryMgr::Instance()->FindEntry(GetFirstPreciosBeadId());
			if (preciousBeadData)
			{
				preciousBeadScore = preciousBeadData->score;
			}
			else
			{
				ErrorLogStream << "calc item score err, PreciousBeadDataEntry not find id  : " << GetFirstPreciosBeadId() << LogStream::eos;
			}
		}

		itemScore = calcVar1 + eqStrengWsAttackScore + eqStrengWsDefScore + eqStrengDecHurtScore + itemAdditionalScore + magicCardScore + preciousBeadScore;

		InfoLogStream << PLAYERINFO(player) << "calc equip itemScore id : " << GetDataID() << ", calcVar1 : " << calcVar1 << ", eqStrengWsAttackScore : " << eqStrengWsAttackScore
			<< ", eqStrengWsDefScore : " << eqStrengWsDefScore << ", eqStrengDecHurtScore" << eqStrengDecHurtScore << ", itemAdditionalScore : " << itemAdditionalScore
			<< ", magicCardScore : " << magicCardScore << ", preciousBeadScore : " << preciousBeadScore << ", itemScore : " << itemScore << LogStream::eos;
		break;
	}

	case ITEM_TITLE:
	{
		//基础评分
		auto equipBaseScoreData = EquipBaseScoreDataEntryMgr::Instance()->GetDataEntryByItemData(GetDataEntry());
		if (!equipBaseScoreData)
		{
			ErrorLogStream << "Can't find equip base score data by item data(" << GetDataID() << ")!" << LogStream::eos;
			return;
		}
		baseScore = equipBaseScoreData->score;

		//附加属性评分
		UInt32 itemAdditionalScore = 0;
		EquipAttrDataEntry* equipAttrData = EquipAttrDataMgr::Instance()->FindEntry(GetDataEntry()->attrId);
		if (!equipAttrData)
		{
			ErrorLogStream << "calc item score err, EquipAttrDataEntry not find id  : " << GetDataID() << LogStream::eos;
			return;
		}
		itemAdditionalScore = equipAttrData->skillScore;

		//宝珠评分
		UInt32 preciousBeadScore = 0;
		if (IsMountedPrecBead())
		{
			auto preciousBeadData = PreciousBeadDataEntryMgr::Instance()->FindEntry(GetFirstPreciosBeadId());
			if (preciousBeadData)
			{
				preciousBeadScore = preciousBeadData->score;
			}
			else
			{
				ErrorLogStream << "calc item score err, PreciousBeadDataEntry not find id  : " << GetFirstPreciosBeadId() << LogStream::eos;
			}
		}

		itemScore = baseScore + itemAdditionalScore + preciousBeadScore;

		InfoLogStream << PLAYERINFO(player) << "calc title itemScore id : " << GetDataID() << ", baseScore : " << baseScore << ", itemAdditionalScore : " << itemAdditionalScore
			<< ", preciousBeadScore : " << preciousBeadScore << ", itemScore : " << itemScore << LogStream::eos;
		break;
	}
	case ITEM_FASHION:
	{
		//基础评分
		auto equipBaseScoreData = EquipBaseScoreDataEntryMgr::Instance()->GetDataEntryByItemData(GetDataEntry());
		if (!equipBaseScoreData)
		{
			ErrorLogStream << "Can't find equip base score data by item data(" << GetDataID() << ")!" << LogStream::eos;
			return;
		}
		baseScore = equipBaseScoreData->score;

		//套装激活积分
		itemScore = baseScore;
		InfoLogStream << PLAYERINFO(player) << "calc fashion itemScore id : " << GetDataID() << ", baseScore : " << baseScore << ", itemScore : " << itemScore << LogStream::eos;
		break;
	}
	default:
		break;
	}
	//if (player)
	//{
	//	player->SendNotify("CalcItemScore id={0}, score={1}", GetDataID(), itemScore);
	//}

	SetItemScore(itemScore);
}

void RewardGroup::AddReward(UInt32 id, UInt32 num, UInt8 qualityLv, UInt8 strenth, UInt32 auctionCoolTimeStamp)
{
	if (num > 0)
	{
		m_Rewards.push_back(ItemReward(id, num, qualityLv, strenth, auctionCoolTimeStamp));
	}
}

void RewardGroup::AddRewards(const ItemRewardVec& rewards)
{
	m_Rewards.insert(m_Rewards.end(), rewards.begin(), rewards.end());
}

void RewardGroup::GetFitRewards(Player* player, const ItemRewardVec& rewards, ItemRewardVec& fitRewards)
{
	if(player == NULL) return;

	for(ItemRewardVec::const_iterator iter = rewards.begin();
		iter != rewards.end(); ++iter)
	{
		ItemDataEntry* itemEntry = ItemDataEntryMgr::Instance()->FindEntry(iter->id);
		if(itemEntry == NULL) continue;

		//if (itemEntry->occu == 0 || itemEntry->occu / 10 == player->GetOccu() / 10)
		if (itemEntry->occu.test(player->GetOccu() - player->GetOccu() % 10) || itemEntry->occu.test(player->GetOccu()))
		{
			fitRewards.push_back(*iter);
		}
	}
}

void RewardGroup::GetFitRewards(Player* player, const ItemRewardVec& rewards)
{
	GetFitRewards(player, rewards, m_Rewards);
}

void RewardGroup::GetFitRewards(Player* player, const RewardGroup* rewards)
{
	if(rewards == NULL) return;
	GetFitRewards(player, rewards->m_Rewards, m_Rewards);
}
