#include "ItemMgr.h"
#include <CLItemProtocol.h>
#include <CLErrorCode.h>
#include <CLRecordClient.h>
#include <CLEqualItemDataEntry.h>
#include <CLSystemValueDataEntry.h>
#include <CLMsgParser.h>
#include <CLFunctionDataEntry.h>
#include <CLEquipRecoveryRewardDataEntryBase.h>
#include <CLEquipRecoveryPriceDataEntryBase.h>
#include <CLEquipRecoScUpConsRtiDataEntryBase.h>
#include <CLEquipRecoScUpPrbWeightDataEntryBase.h>
#include <CLEquipTransMapDataEntry.h>
#include <CLFashionMergeChangeActiveDataEntry.h>
#include <CLEquieUpdateDataEntry.h>
#include <CLNewTitleDataEntry.h>
#include <CLMallShopMultiIDataEntry.h>
#include <CLFashionDecomposeDataEntry.h>
#include <CLInscriptionExtractDataEntry.h>
#include <CLRechargePushDataEntry.h>
#include <CLBeadConvertCostDataEntry.h>
#include <CLBeadConvertDropDataEntry.h>
#include <CLEquipChangeDataEntry.h>
#include <CLEquipChgConsDataEntry.h>
#include <CLWeaponBuildDataEntry.h>
#include <CLTeamCopyDefine.h>

#include "ScriptMgr.h"
#include "PlayerScript.h"

#include "Player.h"
#include "PlayerMgr.h"
#include "ItemMaker.h"
#include "Scene.h"
#include "SSRouter.h"
#include "SortListMgr.h"
#include "ItemScript.h"
#include "EquipCalcScript.h"
#include "CondActivateEvent.h"
#include "SceneSysNotify.h"
#include "CLQuickBuyDataEntry.h"
#include "SceneSysNotify.h"
#include "udplog_typedef.h"
#include "SSServiceSwitchMgr.h"
#include "ActivityMgr.h"
#include "PlayerLoadCallback.h"
#include "InscriptionDropItemMgr.h"
#include "ActiveRecordMgr.h"
#include "SSApplication.h"

using namespace ErrorCode;
ItemMgr::ItemMgr()
{
	m_pOwner = NULL;

	memset(m_pPackages,0,sizeof(m_pPackages));

#define CREATE_PACKAGE(Type, PackT) \
	m_pPackages[Type] = new PackT(Type);

	CREATE_PACKAGE(PACK_EQUIP, Package)
	CREATE_PACKAGE(PACK_MATERIAL, Package)
	CREATE_PACKAGE(PACK_EXPENDABLE, Package)
	CREATE_PACKAGE(PACK_TASK, Package)
	CREATE_PACKAGE(PACK_FASHION, Package)
	CREATE_PACKAGE(PACK_WEAR, Package)
	CREATE_PACKAGE(PACK_FASHION_WEAR, Package)
	CREATE_PACKAGE(PACK_STORAGE, Package)

	CREATE_PACKAGE(PACK_ENERGY_STONE, Package)	//added by huchenhui 2016.07.21
	CREATE_PACKAGE(PACK_TITLE, Package)			//added by huchenhui 2016.07.21
	CREATE_PACKAGE(PACK_PET, Package)			//added by huchenhui 2017.08.04
	CREATE_PACKAGE(PACK_EQRCOV, Package)
	CREATE_PACKAGE(PACK_ROLE_STORAGE, Package)

	CREATE_PACKAGE(PACK_INSCRIPTION, Package)

	CREATE_PACKAGE(PACK_LOSTDUNGEON, Package)

#ifdef _DEBUG
	m_debugEquipQl = false;
#endif

	m_timeItemDeleteIntervalTime = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_TIME_ITEM_DELETE_INTERVAL) * 24 * 3600;

}

ItemMgr::~ItemMgr()
{
	for(UInt8 type = PACK_EQUIP; type < PACK_MAX; ++type)
	{
		CL_SAFE_DELETE(m_pPackages[type]);
	}

	//删除所有道具
	CLDeleteVisitor<Item> delVisitor;
	m_Items.VisitObjects(delVisitor);
	m_Items.Clear();

	for (auto item : m_MailGroup)
	{
		CL_SAFE_DELETE(item);
	}
	m_MailGroup.clear();
}

Package* ItemMgr::GetPackage(UInt8 pack) const
{
	if(pack == PACK_INVALID || pack >= PACK_MAX) return NULL;

	return m_pPackages[pack];
}

bool ItemMgr::CheckPackageCapacityEnough(UInt8 pack, UInt32 num) const
{
	Package* pkg = GetPackage(pack);
	if (!pkg)
	{
		return false;
	}

	if (pkg->GetCapacity() < num)
	{
		return false;
	}

	return true;
}

bool ItemMgr::ClearPackage(UInt8 pack, UInt32 reasonId)
{
	if (pack == PACK_INVALID || pack >= PACK_MAX) return false;

	Package* ppack = m_pPackages[pack];
	class Visitor : public ItemVisitor
	{
	public:
		Visitor(ItemMgr* itemMgr_, UInt32 reasonId_) : m_itemMgr(itemMgr_), m_reasonId(reasonId_){}
		virtual bool operator()(Item* object)
		{
			std::string reason = GetReason((LogSourceType)m_reasonId, object->GetDataID(), object->GetNum());
			m_itemMgr->GetOwner()->ReduceItemNum(reason.c_str(), object, object->GetNum());
			return true;
		}
	protected:
	private:
		ItemMgr* m_itemMgr;
		UInt32   m_reasonId;
	};
	Visitor visitor(this, reasonId);
	ppack->VisitItems(visitor);
	return true;
}


UInt32 ItemMgr::ChangeEquip(Item* item, std::vector<Item*>& syncItems, UInt32 param1, bool isChangeScheme)
{
	if (!item || !item->GetDataEntry())
	{
		ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")item or itemdata is null" << LogStream::eos;
		return ITEM_DATA_INVALID;
	}
	
	if (item->GetEquipType() == EQUIP_HAVE_SMELL)
	{
		return ITEM_CAN_NOT_EQUIP;
	}

	EquipAttrDataEntry* attrData = EquipAttrDataMgr::Instance()->FindEntry(item->GetDataEntry()->attrId);
	if (!attrData)
	{
		if (item->GetValue(IKVKT_WBP_BUILD_WEAPON) > 0)
		{
			attrData = item->GetAttrDataEntry();
		}
		if (attrData == nullptr)
		{
			ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")attrData is null key = " << item->GetDataEntry()->attrId << LogStream::eos;

			GetOwner()->SendNotify("attrData is null key = {0}", item->GetDataEntry()->attrId);
			return ITEM_DATA_INVALID;
		}
	}

	Package* wearPack = GetPackage(PACK_WEAR);
	if (!wearPack)
	{
		ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")wearPack is null packtype = " << PACK_WEAR << LogStream::eos;
		GetOwner()->SendNotify("wearPack is null packtype = {0}", PACK_WEAR);
		return ITEM_DATA_INVALID;
	}
		

	ItemType itemType = (ItemType)item->GetDataEntry()->type;
	PackType packType = GetPackType(itemType);
	Package* equipPack = GetPackage(packType);
	//Package* equipPack = GetPackage(PACK_EQUIP);
	if (!equipPack)
	{
		ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() <<
			")equipPack is null itemdataID = " << item->GetDataEntry()->id << ",itemType" << ",packType = " << packType << LogStream::eos;

		GetOwner()->SendNotify("equipPack is null itemdataID = {0}, itemType = {0}, packType = {0}", item->GetDataEntry()->id, itemType, packType);

		return ITEM_DATA_INVALID;
	}
		

	const ItemPos pos = item->GetPos();

	std::vector<Item*> items;
	if(pos.pack == PACK_WEAR) //卸下
	{
		if (0 == equipPack->GetCapacity())
			return ITEM_ADD_PACK_FULL;

		if (!wearPack->RemoveItem(item))
		{
			ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ") removeItem from wearPack err item uid = " << item->GetID() << LogStream::eos;
			GetOwner()->SendNotify("removeItem from wearPack err item uid = {0}", item->GetID());
			return ITEM_DATA_INVALID;
		}
		else
		{
			if (!equipPack->AddItem(item))
			{
				wearPack->AddItem(item);

				ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ") addItem to equipPack err item uid = " << item->GetID() << LogStream::eos;
				GetOwner()->SendNotify("addItem to equipPack err item uid = {0}", item->GetID());
				return ITEM_DATA_INVALID;
			}
		}
		// 卸下逻辑
		GetOwner()->CountMoveSpeed();
		
		items.push_back(item);
		syncItems.push_back(item);
		//item->SyncProperty(GetOwner());

		//added by huchenhui 2016.07.01 设置称号
		if (item->GetSubType() == ST_EQUIP_TITLE)
		{
			GetOwner()->SetActiveTitle(0);
		}

		GetOwner()->GetActiveTaskMgr().OnChangeEquip(item->GetDataID());

		// 回归周年称号
		GetOwner()->UseReturnYearTitle(item->GetDataEntry()->id, false);
		
		//更新装备方案
		if (!isChangeScheme)
		{
			GetOwner()->GetEqSchemeMgr().OnTakeOffEquip(item, true);
		}
	}
	//else if(pos.pack == PACK_EQUIP) //装备
	else if (pos.pack == packType)
	{
		if (item->CheckInvalid())
		{
			return ITEM_DATA_INVALID;
		}

		//穿戴副武器 则检查职业, 武器
		if (param1 == 1)
		{
			//判断是否是剑斗士
			if (GetOwner()->GetOccu() != SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_EQ_DOUBLESWORD_OCC))
			{
				ErrorLogStream << "ItemMgr::ChangeEquip accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << "use vice equip occ not right occ = " << GetOwner()->GetOccu() << LogStream::eos;
				return ITEM_DATA_INVALID;
			}
			//重剑，武士刀，轻剑
			UInt32 thirdtype = item->GetThirdType();
			if (thirdtype != TT_ITEM_HUGESWORD && thirdtype != TT_ITEM_KATANA && thirdtype != TT_ITEM_SHORTSWORD)
			{
				ErrorLogStream << "ItemMgr::ChangeEquip accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << "use vice equip thirdtype not right thirdtype = " << thirdtype << LogStream::eos;
				return ITEM_DATA_INVALID;
			}
		}

		UInt32 code = CheckWearEquip(item);
		if (code != SUCCESS)
			return code;

		UInt32 grid = item->GetEquipGrid();
		if (param1 == 1) //副武器固定在10位置
		{
			grid = ACCESSWEAPON_EQUIPED_PACKAGE_GRID_ID;
		}
		Item* oldEquip = wearPack->FindItemByPos(grid);

		if (!oldEquip)	//不用替换
		{
			if (!equipPack->RemoveItem(item))
			{
				ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ") removeItem from equipPack err item uid = " << item->GetID() << LogStream::eos;
				GetOwner()->SendNotify("removeItem from equipPack err item uid = {0}", item->GetID());
				return ITEM_DATA_INVALID;
			}
			else
			{
				if (!wearPack->AddItem(item, grid))
				{
					equipPack->AddItem(item);
					ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ") addItem to wearPack err item uid = " << item->GetID() << LogStream::eos;
					GetOwner()->SendNotify("addItem to wearPack err item uid = {0}", item->GetID());
					return ITEM_DATA_INVALID;
				}
			}

			GetOwner()->CountMoveSpeed();

		}
		else	// 要替换
		{
			if (!oldEquip->GetDataEntry())
			{
				ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")oldEquip data is null uid = " << oldEquip->GetID() << LogStream::eos;
				GetOwner()->SendNotify("oldEquip data is null uid = {0}", oldEquip->GetID());

				return ITEM_DATA_INVALID;
			}

			EquipAttrDataEntry* oldAttrData = EquipAttrDataMgr::Instance()->FindEntry(oldEquip->GetDataEntry()->attrId);
			if (!oldAttrData)
			{
				ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")oldAttrData is null key = " << oldEquip->GetDataEntry()->attrId << LogStream::eos;
				GetOwner()->SendNotify("oldAttrData is null key = {0}", oldEquip->GetDataEntry()->attrId);

				return ITEM_DATA_INVALID;
			}
				
			if (!wearPack->RemoveItem(oldEquip))
			{
				ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ") removeItem from wearPack err item uid = " << oldEquip->GetID() << LogStream::eos;
				GetOwner()->SendNotify("removeItem from wearPack err item uid = {0}", oldEquip->GetID());
				return ITEM_DATA_INVALID;
			}
				
			if (!equipPack->RemoveItem(item))
			{
				ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ") removeItem from equipPack err item uid = " << item->GetID() << LogStream::eos;
				GetOwner()->SendNotify("removeItem from equipPack err item uid = {0}", item->GetID());
				return ITEM_DATA_INVALID;
			}
				
			
			if (!equipPack->AddItem(oldEquip, pos.gridIndex))
			{
				wearPack->AddItem(oldEquip);

				ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ") addItem to equipPack err item uid = " << oldEquip->GetID() << LogStream::eos;
				GetOwner()->SendNotify("addItem to equipPack err item uid = {0}", oldEquip->GetID());

				return ITEM_DATA_INVALID;
			}
				
			if (!wearPack->AddItem(item, grid))
			{
				equipPack->RemoveItem(oldEquip);
				equipPack->AddItem(item);
				wearPack->AddItem(oldEquip);

				ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ") addItem to wearPack err item uid = " << item->GetID() << LogStream::eos;
				GetOwner()->SendNotify("addItem to wearPack err item uid = {0}", item->GetID());
				return ITEM_DATA_INVALID;
			}
			
			
			GetOwner()->CountMoveSpeed();

			items.push_back(oldEquip);
			syncItems.push_back(oldEquip);
			//oldEquip->SyncProperty(GetOwner());
		}

		//处理装备栏
		if (item->GetDataEntry() && item->GetDataEntry()->subType == ST_EQUIP_WEAPON)
		{
			ObjID_t oldEquipId = 0;
			if (oldEquip != NULL)
			{
				oldEquipId = oldEquip->GetID();
			}
			GetOwner()->SetWeaponBarById(item->GetID(), oldEquipId);
		}

		//封装装备穿上后解封
		if (item->GetSealState() == 1)
			item->SetSealState(0);
	
		items.push_back(item);
		syncItems.push_back(item);
		//item->SyncProperty(GetOwner());

		//added by huchenhui 2016.07.01 设置称号
		if (item->GetSubType() == ST_EQUIP_TITLE)
		{
			GetOwner()->SetActiveTitle(item->GetDataID());
		}

		GetOwner()->OnPutEquip(item->GetNeedLevel(),item->GetSubType(), item->GetQuality(), item->GetStrengthen());

		GetOwner()->GetActiveTaskMgr().OnWearEquip();

		if (item->GetStrengthen() >= (UInt8)SLL_EIGHT)
		{
			ActivateCondCheckEvent::Instance()->OnActivate(GetOwner(), CAT_MALL_PERSONAL_TAILOR, CAST_ITEM_STRENGEN_LEVEL);
		}

		if (GetOwner()->GetLevel() >= (UInt16)PLL_FIFTY)
		{
			ActivateCondCheckEvent::Instance()->OnActivate(GetOwner(), CAT_MALL_PERSONAL_TAILOR, CAST_PLAYER_LEVEL);
		}

		GetOwner()->GetActiveTaskMgr().OnChangeEquip(item->GetDataID());

		item->ResetTransferStone(); //去掉转移石

		//计算评分
		item->CalculateValuedScore(GetOwner());

		// 回归周年称号
		GetOwner()->UseReturnYearTitle(item->GetDataEntry()->id, true);

		//更新装备方案
		if (!isChangeScheme)
		{
			if (oldEquip != NULL)
			{
				GetOwner()->GetEqSchemeMgr().OnTakeOffEquip(oldEquip);
			}
			GetOwner()->GetEqSchemeMgr().OnTakeUpEquip(item);
		}
	}

	GetOwner()->CalcRoleValueScore();

	if (!isChangeScheme)
	{
		SyncItemBatch(items);
	}
	
	if (item->GetDataEntry() && item->GetDataEntry()->subType == ST_EQUIP_WEAPON)
	{
		GetOwner()->CountAvatar();
	}

	GetOwner()->SyncProperty();

	return SUCCESS;
}

UInt32 ItemMgr::ChangeFashion(Item* item)
{
	if (!item || !item->GetDataEntry())
		return ITEM_DATA_INVALID;

	Package* wearPack = GetPackage(PACK_FASHION_WEAR);
	if (!wearPack)
		return ITEM_DATA_INVALID;

	Package* fashionPack = GetPackage(PACK_FASHION);
	if (!fashionPack)
		return ITEM_DATA_INVALID;

	EquipAttrDataEntry* attrData = EquipAttrDataMgr::Instance()->FindEntry(item->GetFashionAttrId());
	if (!attrData)
	{
		ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")attrData is null key = " << item->GetDataEntry()->attrId << LogStream::eos;
		return ITEM_DATA_INVALID;
	}

	const ItemPos pos = item->GetPos();

	if (pos.pack == PACK_FASHION_WEAR) //卸下
	{
		if (0 == fashionPack->GetCapacity())
			return ITEM_ADD_PACK_FULL;

		if (!wearPack->RemoveItem(item))
			return ITEM_DATA_INVALID;
		else
		{
			if (!fashionPack->AddItem(item))
			{
				wearPack->AddItem(item);
				return ITEM_DATA_INVALID;
			}
		}
		// 卸下逻辑
		// 卸下逻辑
		GetOwner()->CountMoveSpeed();

		item->SyncProperty(GetOwner());
	}
	else if (pos.pack == PACK_FASHION) //装备
	{
		if (item->CheckInvalid())
		{
			return ITEM_DATA_INVALID;
		}

		UInt32 code = CheckWearEquip(item);
		if (code != SUCCESS)
			return code;

		UInt32 grid = item->GetEquipGrid();
		Item* oldEquip = wearPack->FindItemByPos(grid);
		if (!oldEquip)	//不用替换
		{
			if (!fashionPack->RemoveItem(item))
				return ITEM_DATA_INVALID;
			else
			{
				if (!wearPack->AddItem(item, grid))
				{
					fashionPack->AddItem(item);
					return ITEM_DATA_INVALID;
				}
			}

			GetOwner()->CountMoveSpeed();
		}
		else	// 要替换
		{
			EquipAttrDataEntry* oldAttrData = EquipAttrDataMgr::Instance()->FindEntry(oldEquip->GetFashionAttrId());
			if (!oldAttrData)
			{
				ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")attrData is null key = " << oldEquip->GetDataEntry()->attrId << LogStream::eos;
				return ITEM_DATA_INVALID;
			}

			if (!wearPack->RemoveItem(oldEquip))
				return ITEM_DATA_INVALID;
			if (!fashionPack->RemoveItem(item))
				return ITEM_DATA_INVALID;

			if (!fashionPack->AddItem(oldEquip, pos.gridIndex))
			{
				wearPack->AddItem(oldEquip);
				return ITEM_DATA_INVALID;
			}

			if (!wearPack->AddItem(item, grid))
			{
				fashionPack->RemoveItem(oldEquip);
				fashionPack->AddItem(item);
				wearPack->AddItem(oldEquip);
				return ITEM_DATA_INVALID;
			}

			GetOwner()->CountMoveSpeed();
			oldEquip->SyncProperty(GetOwner());
		}

		//封装装备穿上后解封
		if (item->GetSealState() == 1)
			item->SetSealState(0);

		//计算评分
		item->CalculateValuedScore(GetOwner());

		item->SyncProperty(GetOwner());

		GetOwner()->GetActiveTaskMgr().OnWearFashion(wearPack->GetItemNum());
		GetOwner()->OnPutFashion((ItemSubType)item->GetSubType(), item->GetQuality(), item->GetFashionTransparentFlag());
	}

	GetOwner()->CountAvatar();
	GetOwner()->SyncProperty();
	GetOwner()->GetActiveTaskMgr().OnOpChangeFashion(item->GetDataID());
	GetOwner()->CalcRoleValueScore();

	return SUCCESS;
}

UInt32 ItemMgr::GetEquipSize(UInt16 level, UInt8 quality, ItemSubType subType)
{
	Package* pPackage = GetPackage(PACK_WEAR);
	if (pPackage == NULL) return 0;

	class GetEquipSizeVisitor : public ItemVisitor
	{
	public:
		GetEquipSizeVisitor(UInt16 level, UInt8 quality, ItemSubType subType)
			:m_Level(level), m_Quality(quality), m_ItemSubType(subType), m_Num(0){}
		~GetEquipSizeVisitor(){}

		bool operator()(Item* item)
		{
			if (item->GetType() != ITEM_TITLE
				&& (m_Quality == ITEM_QUALITY_INVLID || item->GetQuality() == m_Quality)
				&& (m_Level == 0 || item->GetNeedLevel() >= m_Level)
				&& (m_ItemSubType == ItemSubType::ST_ITEM_INVALID || m_ItemSubType == item->GetSubType()))
			{
				m_Num += 1;
			}
			return true;
		}

		UInt32 GetSize() const { return m_Num; }

	private:
		UInt16 m_Level;
		UInt8 m_Quality;
		ItemSubType m_ItemSubType;
		UInt32 m_Num;
	};

	GetEquipSizeVisitor visitor(level, quality, subType);
	pPackage->VisitItems(visitor);
	return visitor.GetSize();
}

UInt32 ItemMgr::GetGreenEquipSize(UInt16 level)
{
	Package* pPackage = GetPackage(PACK_WEAR);
	if (pPackage == NULL) return 0;

	class GetGreenEquipSizeVisitor : public ItemVisitor
	{
	public:
		GetGreenEquipSizeVisitor(UInt16 level) :m_Level(level), m_Num(0){}
		~GetGreenEquipSizeVisitor(){}

		bool operator()(Item* item)
		{
			if (item->GetType() != ITEM_TITLE 
				&& item->GetQuality() == ITEM_QUALITY_GREEN
				&& (m_Level == 0 || item->GetNeedLevel() >= m_Level))
			{
				m_Num += 1;
			}
			return true;
		}

		UInt32 GetSize() const { return m_Num; }

	private:
		UInt16 m_Level;
		UInt32 m_Num;
	};

	GetGreenEquipSizeVisitor visitor(level);
	pPackage->VisitItems(visitor);
	return visitor.GetSize();
}

UInt32 ItemMgr::GetFashionSize(UInt8 quality, ItemSubType subType, UInt8 isTrans)
{
	Package* pPackage = GetPackage(PACK_FASHION_WEAR);
	if (pPackage == NULL) return 0;

	class GetFashionSizeVisitor : public ItemVisitor
	{
	public:
		GetFashionSizeVisitor(UInt8 quality, ItemSubType subType, UInt8 isTrans)
			:m_Quality(quality), m_ItemSubType(subType), m_isTrans(isTrans), m_Num(0){}
		~GetFashionSizeVisitor(){}

		bool operator()(Item* item)
		{
			if ((m_Quality == ITEM_QUALITY_INVLID || item->GetQuality() >= m_Quality)
				&& (m_ItemSubType == ItemSubType::ST_ITEM_INVALID || m_ItemSubType == item->GetSubType())
				&& (m_isTrans == 0 || m_isTrans == item->GetFashionTransparentFlag()))
			{
				m_Num += 1;
			}
			return true;
		}

		UInt32 GetSize() const { return m_Num; }

	private:
		UInt8 m_Quality;
		ItemSubType m_ItemSubType;
		UInt8	m_isTrans;
		UInt32 m_Num;
	};

	GetFashionSizeVisitor visitor(quality, subType, isTrans);
	pPackage->VisitItems(visitor);
	return visitor.GetSize();
}

PackType ItemMgr::GetPackType(ItemType itemType)
{
	switch (itemType)
	{
	case ITEM_EQUIP:		return PACK_EQUIP;
	case ITEM_MATERIAL:		return PACK_MATERIAL;
	case ITEM_EXPENDABLE:	return PACK_EXPENDABLE;
	case ITEM_TASK:			return PACK_TASK;
	case ITEM_FASHION:		return PACK_FASHION;
	case ITEM_ENERGY_STONE:	return PACK_ENERGY_STONE;
	case ITEM_TITLE:		return PACK_TITLE;
	case ITEM_PET:			return PACK_PET;
	case ITEM_INSCRIPTION:	return PACK_INSCRIPTION;
	case ITEM_LOSTDUNGEON:	return PACK_LOSTDUNGEON;
	default:				return PACK_INVALID;
	}
}

UInt32 ItemMgr::UseItem(ObjID_t id, UInt16 num, UInt32 param1, UInt32 param2)
{
	Item* item = FindItem(id);
	ItemDataEntry* data = item->GetDataEntry();
	if (!item || !data)
	{
		return ITEM_DATA_INVALID;
	}

	auto dataid = data->id;
	switch ((ItemSubType)data->subType)
	{
	case ST_GIFT_PACKAGE:
	case ST_JAR_GIFT:
		if (!data->IsMeetOccuRequirement((Occupation)GetOwner()->GetOccu()))
		{
			return ITEM_EQUIP_OCCU;
		}
		break;

	default:
		break;
	}

	if (item->CheckInvalid())
	{
		return ITEM_DATA_INVALID;
	}

	//判断道具日使用计数
	bool isDayUseLimite = false;
	std::string counterName = "";
	auto dailyUseLimitRet = CheckItemDayUseNum(item, isDayUseLimite, counterName);
	if (dailyUseLimitRet != ErrorCode::SUCCESS)
	{
		DebugLogStream << PLAYERINFO(GetOwner()) << " UseItem Fail! day times used over." << LogStream::eos;
		return dailyUseLimitRet;
	}

	
	auto useLimitRet = CheckItemUseLimitByDataId(item);
	if (useLimitRet != ErrorCode::SUCCESS)
	{
		DebugLogStream << PLAYERINFO(GetOwner()) << " UseItem Fail! use times used over." << LogStream::eos;
		return useLimitRet;
	}


	RewardGroup rewardGroup;
	UInt32  sourceType = LogSourceType::SOURCE_TYPE_INVALID;

	for (UInt16 i = 0; i < num; ++i)
	{
		//检查CD
		if (data->cdGroup != 0)
		{
			UInt32 endTime = (UInt32)(GetOwner()->GetItemUseTime(data->cdGroup) / 1000);
			if (CURRENT_TIME.Sec() < endTime)
			{
				DebugLogStream << PLAYERINFO(GetOwner()) << " UseItem Fail! CD endTime = " << endTime << " curTime = " << CURRENT_TIME.Sec() << LogStream::eos;
				return ITEM_USE_CD;
			}
				
		}

		//检查互斥buff
		for (UInt32 i = 0; i < data->mutexBuffIDs.size(); ++i)
		{
			Buff* buff = GetOwner()->GetBuffMgr().FindBuff(data->mutexBuffIDs[i]);
			if (buff)
			{
				DebugLogStream << PLAYERINFO(GetOwner()) << " UseItem Fail! Buff Already Exist id = " << buff->GetId() << LogStream::eos;
				return ITEM_USE_BUFFALREADYEXIST;
			}
		}
		
		UInt32 ret = SUCCESS;
		//是礼包
		if (data->subType == ST_GIFT_PACKAGE)
		{
			if (data->needLevel > GetOwner()->GetLevel() || data->maxLevel < GetOwner()->GetLevel())
			{
				InfoLogStream << PLAYERINFO(GetOwner()) << " level(" << GetOwner()->GetLevel() << ") limit." << LogStream::eos;
				return ITEM_USE_LEVEL_LIMIT;
			}

			sourceType = LogSourceType::SOURCE_USE_GIFT_PACK;
			ret = UseGiftPack(item, param1, rewardGroup);
		}
		else if (data->subType == ST_JAR_GIFT)
		{
			sourceType = LogSourceType::SOURCE_USE_GIFT_PACK;
			if (data->needLevel > GetOwner()->GetLevel() || data->maxLevel < GetOwner()->GetLevel())
			{
				InfoLogStream << PLAYERINFO(GetOwner()) << " level(" << GetOwner()->GetLevel() << ") limit." << LogStream::eos;
				ret = ITEM_USE_LEVEL_LIMIT;
			}

			if (ret == SUCCESS && (GetPackage(PACK_EQUIP)->GetCapacity() == 0 || GetPackage(PACK_MATERIAL)->GetCapacity() == 0 ||
				GetPackage(PACK_EXPENDABLE)->GetCapacity() == 0))
			{
				ret = ErrorCode::ITEM_MUST_HAS_ONE_POS;
			}
			
			if (ret == SUCCESS)
			{
				ret = GetOwner()->GetJarMgr().OpenJarGift(data->giftId, data->name, rewardGroup);
			}
			
			if (ret == SUCCESS)
			{
				OnUsedLimitedItem(item);
				if (CheckLimitedItemDel(item))
				{
				ret = GetOwner()->ReduceItemNum(GetReason(LogSourceType::SOURCE_USE_GIFT_PACK, data->id).c_str(), item, 1);
			}
			}

			/*if (data->needLevel > GetOwner()->GetLevel() || data->maxLevel < GetOwner()->GetLevel())
			{
				InfoLogStream << PLAYERINFO(GetOwner()) << " level(" << GetOwner()->GetLevel() << ") limit." << LogStream::eos;
				return ITEM_USE_LEVEL_LIMIT;
			}

			if (GetPackage(PACK_EQUIP)->GetCapacity() == 0 || GetPackage(PACK_MATERIAL)->GetCapacity() == 0 ||
				GetPackage(PACK_EXPENDABLE)->GetCapacity() == 0)
			{
				return ErrorCode::ITEM_MUST_HAS_ONE_POS;
			}

			ret = GetOwner()->GetJarMgr().OpenJarGift(data->giftId);
			if (SUCCESS != ret)
			{
				return ret;
			}

			ret = GetOwner()->ReduceItemNum(GetReason(LogSourceType::SOURCE_USE_GIFT_PACK).c_str(), item, 1);*/

		}
		else if (data->subType == ST_COLORLESS_CRYSTAL)
		{
			// 放技能用无色晶体
			UInt32 hasNum = GetOwner()->GetItemNum(data->id);
			if (param1 > hasNum)
			{
				DebugLogStream << PLAYERINFO(GetOwner()) << "use colorless crystal num err! num = " << param1 << LogStream::eos;
				return ITEM_NUM_INVALID;
			}

			if (!GetOwner()->RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_DUNGEON, param2).c_str(), data->id, param1))
			{
				DebugLogStream << PLAYERINFO(GetOwner()) << "use colorless crystal delete faied! num = " << param1 << LogStream::eos;
				return ITEM_NUM_INVALID;
			}

			ret = SUCCESS;
			
		}
		else if (data->subType == ST_ADD_CHARGE_POINT)
		{
			UInt32 chargeValue = 1;
			ret = GetOwner()->ReduceItemNum(GetReason(LogSourceType::SOURCE_TYPE_ITEM).c_str(), item, chargeValue);
			if (ret != SUCCESS)
			{
				return ITEM_NUM_INVALID;
			}

			GetOwner()->GetActiveTaskMgr().OnCharge(GetReason(LogSourceType::SOURCE_TYPE_ITEM).c_str(), chargeValue);

			DebugLogStream << PLAYERINFO(GetOwner()) << " Active OnCharge " << chargeValue << "!" << LogStream::eos;
		}
		else if (data->subType == ST_ADD_VIP_POINT)
		{
			UInt32 chargeValue = 1;
			ret = GetOwner()->ReduceItemNum(GetReason(LogSourceType::SOURCE_TYPE_ITEM).c_str(), item, chargeValue);
			if (ret != SUCCESS)
			{
				return ITEM_NUM_INVALID;
			}

			GetOwner()->OnChargeMakeup(GetReason(SOURCE_TYPE_BILLING_GOODS, chargeValue).c_str(), chargeValue, chargeValue);

			DebugLogStream << PLAYERINFO(GetOwner()) << " Vip OnCharge " << chargeValue << "!" << LogStream::eos;
			
		}
		else if (data->subType == ST_CONSUME_JAR_GIFT)
		{
			Item* itemTemp = FindItem(id);
			if (itemTemp == NULL)
			{
				return ITEM_DATA_INVALID;
			}
			//检查需要消耗的道具
			UInt32 consItemId = data->jarGiftConsumeItemId;
			UInt32 consNum = data->jarGiftConsumeItemNum;
			ItemDataEntry* consItemData = ItemDataEntryMgr::Instance()->FindEntry(consItemId);
			if (consItemData == NULL)
			{
				return ITEM_DATA_INVALID;
			}
			if (GetOwner()->GetItemNum(consItemId) < consNum)
			{
				ret = ITEM_CONS_JARGIFT_ITEM_INSUFFIC;
			}

			if (ret == SUCCESS && (data->needLevel > GetOwner()->GetLevel() || data->maxLevel < GetOwner()->GetLevel()))
			{
				InfoLogStream << PLAYERINFO(GetOwner()) << " level(" << GetOwner()->GetLevel() << ") limit." << LogStream::eos;
				ret = ITEM_USE_LEVEL_LIMIT;
			}

			if (ret == SUCCESS && (GetPackage(PACK_EQUIP)->GetCapacity() == 0 || GetPackage(PACK_MATERIAL)->GetCapacity() == 0 ||
				GetPackage(PACK_EXPENDABLE)->GetCapacity() == 0))
			{
				ret = ErrorCode::ITEM_MUST_HAS_ONE_POS;
			}

			if (ret == SUCCESS)
			{
				ret = GetOwner()->GetJarMgr().OpenJarGift(data->giftId, consItemData->name, rewardGroup, false, GetReason(LogSourceType::SOURCE_TYPE_CONSUME_JARGIFT, data->giftId, data->id).c_str());
			}

			if (ret == SUCCESS)
			{
				if (!GetOwner()->RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_CONSUME_JARGIFT).c_str(), consItemId, consNum))
				{
					ret = ITEM_DATA_INVALID;
				}
				if (ret == SUCCESS)
				{
					OnUsedLimitedItem(itemTemp);
					if (CheckLimitedItemDel(itemTemp))
					{
						ret = GetOwner()->ReduceItemNum(GetReason(LogSourceType::SOURCE_USE_GIFT_PACK, data->id).c_str(), itemTemp, 1);
					}
				}
			}
		}
		else if (data->subType == ST_UPLEVEL)
		{
			if (data->needLevel > GetOwner()->GetLevel() || data->maxLevel < GetOwner()->GetLevel())
			{
				InfoLogStream << PLAYERINFO(GetOwner()) << " level(" << GetOwner()->GetLevel() << ") limit." << LogStream::eos;
				return ErrorCode::ITEM_USE_LEVEL_LIMIT;
			}

			if (GetOwner()->GetLevel() == MAX_LEVEL)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << " use uplevel item(" << dataid << ") is alread max level(" << GetOwner()->GetLevel() << ")." << LogStream::eos;
				return ErrorCode::ITEM_UPLEVEL_ALREADY_MAX;
			}

			auto newLevel = GetOwner()->GetLevel() + 1;
			ret = GetOwner()->ReduceItemNum(GetReason(LogSourceType::SOURCE_TYPE_USE_ITEM).c_str(), item, 1);
			if (ret != ErrorCode::SUCCESS)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << " use uplevel item(" << dataid << "), reduce item failed:" << (UInt32)ret << "." << LogStream::eos;
				return ret;
			}
			
			UInt64 needExp = GetOwner()->GetLevelExp(newLevel - 1);
			GetOwner()->IncExp(GetReason(SOURCE_TYPE_USE_ITEM, dataid, newLevel).c_str(), needExp);

			ret = ErrorCode::SUCCESS;
		}
		/*else if (data->subType == ST_NEW_TITLE)
		{
			NewTitleItemDataEntry* dataEntry = NewTitleItemDataEntryMgr::Instance()->FindEntry(data->newTitleId);
			if (!dataEntry)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << " use new title item(" << dataid << "), newTitleId config err:" << data->newTitleId << "." << LogStream::eos;
				return ITEM_DATA_INVALID;
			}

			UInt32 dueTm = item->GetDeadLine();
			
			ret = GetOwner()->ReduceItemNum(GetReason(LogSourceType::SOURCE_TYPE_USE_ITEM).c_str(), item, 1);
			if (ret != ErrorCode::SUCCESS)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << " use new title item(" << dataid << "), reduce item failed:" << (UInt32)ret << "." << LogStream::eos;
				return ret;
			}

			
			GetOwner()->AddNewTitle(data->newTitleId, dueTm);
		}*/
		else if (data->subType == ST_FLYUPITEM)
		{
			//飞升秘药
			if (data->needLevel > GetOwner()->GetLevel() || data->maxLevel < GetOwner()->GetLevel())
			{
				InfoLogStream << PLAYERINFO(GetOwner()) << " level(" << GetOwner()->GetLevel() << ") limit." << LogStream::eos;
				return ITEM_USE_LEVEL_LIMIT;
			}

			if (ErrorCode::SUCCESS == (ret = GetOwner()->ReduceItemNum(GetReason(LogSourceType::SOURCE_TYPE_ITEM).c_str(), item, 1)) )
			{
				CLProtocol::SceneSyncFlyUpStatus msg;
				msg.status = FLY_UP_STATUS_IN;
				GetOwner()->SendProtocol(msg);
				UInt64 needExp = GetOwner()->GetLevelExp(static_cast<UInt8>(data->color2 - 1));
				GetOwner()->SetIsInFlyUP(true);
				//任务只做到前一级，所以这里给的经验少一点避免升级
				GetOwner()->IncExp(GetReason(SOURCE_TYPE_FLAY_UP).c_str(), needExp - 1);
				GetOwner()->SetIsInFlyUP(false);
				GetOwner()->SetObtainExp(false);
				ret = GetOwner()->FinishAllTask(TASK_MAIN);
				GetOwner()->SetObtainExp(true);
				//任务已经完成了，之前避免的一点经验再给上
				GetOwner()->IncExp(GetReason(SOURCE_TYPE_FLAY_UP).c_str(), 1);
				if (SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_FLY_UP))
				{
					GetOwner()->GetActiveTaskMgr().OnLevelup();
				}
				msg.status = FLY_UP_STATUS_OUT;
				GetOwner()->SendProtocol(msg);
			}
		}
		else if (data->subType == ST_ADVENTURE_PASS_ACTIVATE_ITEM)
		{
			if (ErrorCode::SUCCESS == (ret = GetOwner()->ReduceItemNum(GetReason(LogSourceType::SOURCE_TYPE_ITEM).c_str(), item, 1)))
			{
				GetOwner()->SyncAccountInfo(ACCOUNT_ADVENTURE_KING, APOKT_ITEM);
				GetOwner()->SendNotify(13004);
			}
		}
		else if (data->subType == ST_TEAM_COPY_CNT || data->subType == ST_TEAM_COPY_DIFF_CNT)
		{
			UInt32 useRet = UseTeamCopyItem(item, data);
			if (useRet != ErrorCode::SUCCESS)
			{
				return useRet;
			}

			GetOwner()->SendNotify(10067);
		}
		else if (data->subType == ST_HONOR_GUARD_CARD)
		{
			if (GetOwner()->GetRoleHonor()->IsUseGuardCard())
			{
				return ErrorCode::ITEM_WEEK_REPEAT_USE;
			}

			if (ErrorCode::SUCCESS == (ret = GetOwner()->ReduceItemNum(GetReason(LogSourceType::SOURCE_TYPE_ITEM).c_str(), item, 1)))
			{
				GetOwner()->GetRoleHonor()->UseGuildCard();
			}
		}
		else if (data->subType == ST_LOST_DUNGEON_INCREASE_TIMES)
		{
			ret = GetOwner()->ReduceItemNum(GetReason(LogSourceType::SOURCE_LOSTDUNGEON_CONS).c_str(), item, 1);
			if (ret != SUCCESS)
			{
				return ITEM_NUM_INVALID;
			}
			GetOwner()->IncCounter(COUNTER_LOSTDUNG_REMAIN_NUM, data->useLimiteValue);
		}
		else if (ST_LOST_DUNGEON_INTEGRAL == data->subType)
		{
			UInt32 score = GetOwner()->GetLostDungeon().GetLostDungeonScore();
			GetOwner()->GetLostDungeon().SetLostDungeonScore(score + data->useLimiteValue);
		}
		else if (ST_GIFT_RIGHT_CARD == data->subType)
		{
			// 如果是礼遇特权卡
			if (GetOwner()->GetCounter(OPACT_GIFT_RIGHT_CARD_BUY_TIME) > 0)
			{
				InfoLogStream << PLAYERINFO(GetOwner()) << "giftRightCard already have" << LogStream::eos;
			}
			else
			{
				if (ErrorCode::SUCCESS == (ret = GetOwner()->ReduceItemNum(GetReason(LogSourceType::SOURCE_TYPE_ITEM).c_str(), item, 1)))
				{
					GetOwner()->SetCounter(OPACT_GIFT_RIGHT_CARD, 1);
					GetOwner()->SetCounter(OPACT_GIFT_RIGHT_CARD_BUY_TIME, CURRENT_TIME.Sec());

					GetOwner()->SyncGiftRightToWorld();

					GetOwner()->GetActiveTaskMgr().UpdateGiftRightOnlineTask();

					InfoLogStream << PLAYERINFO(GetOwner()) << "have gift right card" << LogStream::eos;
				}
			}
		}
		else if (ST_DUNGEON_TICKET_RIGHT_CARD == data->subType)
		{
			// 如果是深渊特权卡
			if (!GetOwner()->GetActiveTaskMgr().CanBuyDungeonRightCard())
			{
				InfoLogStream << PLAYERINFO(GetOwner()) << "dungeonTicketRightCard can't use" << LogStream::eos;
			}
			else
			{
				if (ErrorCode::SUCCESS == (ret = GetOwner()->ReduceItemNum(GetReason(LogSourceType::SOURCE_TYPE_ITEM).c_str(), item, 1)))
				{
					GetOwner()->GetActiveTaskMgr().SetDungeonRightCard();
					GetOwner()->SyncDungeonRightToWorld();
					InfoLogStream << PLAYERINFO(GetOwner()) << "have dungeon right card" << LogStream::eos;
				}
			}
		}
		else
		{
			if (data->subType == ST_EXP_PILL)
			{
				if (data->needLevel > GetOwner()->GetLevel() || data->maxLevel < GetOwner()->GetLevel())
				{
					InfoLogStream << PLAYERINFO(GetOwner()) << " level(" << GetOwner()->GetLevel() << ") limit." << LogStream::eos;
					return ITEM_USE_LEVEL_LIMIT;
				}
			}
			ItemScript* script = ScriptMgr::Instance()->FindItemScript(item->GetDataID());
			if (!script)
			{
				ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")item script is null key = " << item->GetDataID() << LogStream::eos;
				GetOwner()->SendNotify("item script is null key = {0}", item->GetDataID());
				return ITEM_DATA_INVALID;
			}

			ret = script->OnUseSelf(GetOwner(), item);
		}

		if (SUCCESS == ret)
		{
			//里面不能再使用item对象!!
			OnUseItemSuc(data, isDayUseLimite, counterName);

			//是单局药品用完要更新药品配置信息
			if (data->thirdType == TT_DUNGEON_POTION &&
				0 == GetOwner()->GetItemNum(data->id))
			{
				std::vector<UInt32> pos = GetOwner()->GetPotionPos();
				for (size_t i = 0; i < pos.size(); ++i)
				{
					if (pos[i] == data->id)
					{
						GetOwner()->SetPotionPos(i, 0);
						break;
					}
				}

				GetOwner()->SyncProperty();
			}
		}
		else
		{
			if (rewardGroup.GetRewards().size() > 0)
			{
				const std::vector<ItemReward> rewards = rewardGroup.GetRewards();
				std::vector<ItemReward> mergeRewards;
				//合并奖励
				MergeItems(rewards, mergeRewards);
				CLProtocol::SceneNotifyGetItem notify;
				notify.items = mergeRewards;
				notify.sourceType = sourceType;
				GetOwner()->SendProtocol(notify);
			}
			return ret;
		}
	}
	
	if (rewardGroup.GetRewards().size() > 0)
	{
		const std::vector<ItemReward> rewards = rewardGroup.GetRewards();
		std::vector<ItemReward> mergeRewards;
		//合并奖励
		MergeItems(rewards, mergeRewards);
		CLProtocol::SceneNotifyGetItem notify;
		notify.items = mergeRewards;
		notify.sourceType = sourceType;
		GetOwner()->SendProtocol(notify);
	}

	return SUCCESS;
}

UInt32 ItemMgr::UseItem(UInt32 id)
{
	ItemDataEntry* data = ItemDataEntryMgr::Instance()->FindEntry(id);
	if (!data)
		return ITEM_DATA_INVALID;

	if (data->cdGroup != 0)
	{
		if (CURRENT_TIME.MSec() < GetOwner()->GetItemUseTime(data->cdGroup)) return ITEM_USE_CD;
	}

	for (UInt32 i = 0; i < data->mutexBuffIDs.size(); ++i)
	{
		Buff* buff = GetOwner()->GetBuffMgr().FindBuff(data->mutexBuffIDs[i]);
		if (buff)
		{
			return ITEM_USE_CD;
		}
	}

	//附加buff
	if (data->onUseBuff != 0)
	{
		GetOwner()->GetBuffMgr().AddBuff(data->onUseBuff);
	}

	if (data->cdGroup != 0)
	{
		GetOwner()->SetItemUseTime(data->cdGroup, CURRENT_TIME.MSec() + data->cdTime * 1000, data->cdTime);
		GetOwner()->SyncProperty();
	}

	GetOwner()->OnUseItem(data->id);
	

	return SUCCESS;
}

UInt32 ItemMgr::UseGiftPack(Item* item, UInt32 sel, RewardGroup& rewardGroup)
{
	UInt32 canUse = CheckGiftCanUse(item);
	if (canUse != SUCCESS)
	{
		return canUse;
	}

	GiftPackData* gpData = GiftPackDataMgr::Instance()->FindEntry(item->GetDataEntry()->giftId);
	if (!gpData)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " GiftPackData is null! id = " << item->GetDataEntry()->giftId << LogStream::eos;
		return ITEM_DATA_INVALID;
	}

	RewardGroup* rgp = GetOwner()->GetRewardGroup();
	if (!rgp)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " RewardGroup is null!" << LogStream::eos;
		return ITEM_DATA_INVALID;
	}

	if (GPFT_NONE == gpData->filterType)
	{
		//全部获得
		for (size_t i = 0; i < gpData->giftItems.size(); ++i)
		{
			GiftItemData* giData = GiftItemDataMgr::Instance()->FindEntry(gpData->giftItems[i]);
			if (!giData)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << " GiftItemData is null! id = " << gpData->giftItems[i] << LogStream::eos;
				return ITEM_DATA_INVALID;
			}
			if (giData->levels[0] > GetOwner()->GetLevel() || giData->levels[1] < GetOwner()->GetLevel())
			{
				continue;
			}
			rgp->AddReward(giData->itemId, giData->num, 0, giData->strengthen, 0, (EquipType)giData->equipType);
		}
	}
	else if (GPFT_OCCU == gpData->filterType)
	{
		RewardGroup temp_reward_group;
		//获得职业符合得
		for (size_t i = 0; i < gpData->giftItems.size(); ++i)
		{
			GiftItemData* giData = GiftItemDataMgr::Instance()->FindEntry(gpData->giftItems[i]);
			if (!giData)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << " GiftItemData is null! id = " << gpData->giftItems[i] << LogStream::eos;
				return ITEM_DATA_INVALID;
			}

			bool fitOccu = false;
			for (size_t j = 0; j < giData->occus.size(); ++j)
			{
				if (giData->occus[j] == GetOwner()->GetOccu())
				{
					fitOccu = true;
					break;
				}
			}
			
			if (fitOccu)
			{
				if (giData->levels[0] > GetOwner()->GetLevel() || giData->levels[1] < GetOwner()->GetLevel())
				{
					continue;
				}
				temp_reward_group.AddReward(giData->itemId, giData->num, 0, giData->strengthen, 0, (EquipType)giData->equipType);
			}
		}
		if (temp_reward_group.GetRewards().size() > (size_t)gpData->filterCount)
		{
			std::random_shuffle(temp_reward_group.GetRewardsRef().begin(), temp_reward_group.GetRewardsRef().end());
			const ItemRewardVec& reward_vec = temp_reward_group.GetRewards();
			for (Int32 k = 0; k < gpData->filterCount; ++k)
			{
				rgp->AddReward(reward_vec[k].id, reward_vec[k].num, reward_vec[k].qualityLv, reward_vec[k].strenth, 0, (EquipType)reward_vec[k].equipType);
			}
		}
		else
		{
			rgp->AddRewards(temp_reward_group.GetRewards());
		}
	}
	else if (GPFT_RANDOM == gpData->filterType)
	{
		//随机获得
		if (gpData->filterCount <= 0)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " random gift pack filterCount error! value = " << gpData->filterCount << LogStream::eos;
			return ITEM_DATA_INVALID;
		}

		std::vector<UInt32> wts;
		ItemRewardVec rewards;
		for (size_t i = 0; i < gpData->giftItems.size(); ++i)
		{
			GiftItemData* giData = GiftItemDataMgr::Instance()->FindEntry(gpData->giftItems[i]);
			if (!giData)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << " GiftItemData is null! id = " << gpData->giftItems[i] << LogStream::eos;
				return ITEM_DATA_INVALID;
			}
			if (giData->levels[0] > GetOwner()->GetLevel() || giData->levels[1] < GetOwner()->GetLevel())
			{
				continue;
			}
			wts.push_back(giData->weight);
			rewards.push_back(ItemReward(giData->itemId, giData->num, 1, giData->strengthen, 0, (EquipType)giData->equipType));
		}

		//随机filterCount个
		for (int i = 0; i < gpData->filterCount; ++i)
		{
			Int32 hitIdx = WeightRander::RandWeight(wts);
			if (hitIdx >= (Int32)rewards.size() || hitIdx < 0)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << " hitIdx is invalid! hitidx = " << hitIdx << LogStream::eos;
				return ITEM_DATA_INVALID;
			}
			rgp->AddReward(rewards[hitIdx].id, rewards[hitIdx].num, 0, rewards[hitIdx].strenth, 0, (EquipType)rewards[hitIdx].equipType);
		}
	}
	else if (GPFT_CUSTOM == gpData->filterType)
	{
		//获得选择的
		if (gpData->filterCount <= 0)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " sel gift pack filterCount error! value = " << gpData->filterCount << LogStream::eos;
			return ITEM_DATA_INVALID;
		}

		UInt32 selNum = 0;
		for (int i = 0; i < 32; ++i)
		{
			if ((Int32)selNum >= gpData->filterCount)
			{
				continue;
			}

			UInt32 idx = sel & (1 << i);
			if (idx == 0)
			{
				continue;
			}

			if (i < 0 || i >= (int)gpData->giftItems.size())
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << " sel gift pack index is error! index = " << i << LogStream::eos;
				return ITEM_DATA_INVALID;
			}

			GiftItemData* giData = GiftItemDataMgr::Instance()->FindEntry(gpData->giftItems[i]);
			if (!giData)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << " GiftItemData is null! id = " << gpData->giftItems[i] << LogStream::eos;
				return ITEM_DATA_INVALID;
			}
			if (giData->levels[0] > GetOwner()->GetLevel() || giData->levels[1] < GetOwner()->GetLevel())
			{
				continue;
			}
			rgp->AddReward(giData->itemId, giData->num, 0, giData->strengthen, 0, (EquipType)giData->equipType);
			selNum++;
		}

		if (selNum == 0)
		{
			rgp->ClearRewards();
			ErrorLogStream << PLAYERINFO(GetOwner()) << " use select gift(" << item->GetID() << ", " << item->GetDataID() << ") failed, no select item." << LogStream::eos;
			return ITEM_USE_FAIL;
		}
	}
	else if (GPFT_CUSTOM_OCCU == gpData->filterType)
	{
		//职业符合道具中获得选择的
		if (gpData->filterCount <= 0)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " sel gift pack filterCount error! value = " << gpData->filterCount << LogStream::eos;
			return ITEM_DATA_INVALID;
		}

		//删选出符合职业得道具集合
		std::vector<ItemReward> fitOccuItems;
		for (size_t i = 0; i < gpData->giftItems.size(); ++i)
		{
			GiftItemData* giData = GiftItemDataMgr::Instance()->FindEntry(gpData->giftItems[i]);
			if (!giData)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << " GiftItemData is null! id = " << gpData->giftItems[i] << LogStream::eos;
				return ITEM_DATA_INVALID;
			}

			bool fitOccu = false;
			for (size_t j = 0; j < giData->occus.size(); ++j)
			{
				if (giData->occus[j] == GetOwner()->GetOccu())
				{
					fitOccu = true;
					break;
				}
			}

			if (fitOccu)
			{
				if (giData->levels[0] > GetOwner()->GetLevel() || giData->levels[1] < GetOwner()->GetLevel())
				{
					continue;
				}
				fitOccuItems.push_back(ItemReward(giData->itemId, giData->num, 0, giData->strengthen, 0, (EquipType)giData->equipType));
			}
		}

		UInt32 selNum = 0;
		for (int i = 0; i < 32; ++i)
		{
			if ((Int32)selNum >= gpData->filterCount)
			{
				continue;
			}

			UInt32 idx = sel & (1 << i);
			if (idx == 0)
			{
				continue;
			}

			if (i < 0 || i >= (int)fitOccuItems.size())
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << " sel gift pack index is error! index = " << i << LogStream::eos;
				return ITEM_DATA_INVALID;
			}

			rgp->AddReward(fitOccuItems[i].id, fitOccuItems[i].num, 0, fitOccuItems[i].strenth, 0, (EquipType)fitOccuItems[i].equipType);
			selNum++;
		}
	}
	else
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " GiftPackData filterType is error! type = " << gpData->filterType  << LogStream::eos;
		return ITEM_DATA_INVALID;
	}
	
	UInt32 itemDataId = item->GetDataEntry()->id;

	OnUsedLimitedItem(item);

	if (CheckLimitedItemDel(item))
	{
	UInt32 ret = GetOwner()->ReduceItemNum(GetReason(LogSourceType::SOURCE_USE_GIFT_PACK, itemDataId).c_str(), item, 1);
	if (SUCCESS != ret)
	{
		return ret;
	}
	}
	
	GetOwner()->AddRewards(GetReason(LogSourceType::SOURCE_USE_GIFT_PACK, itemDataId).c_str(), rgp, false);
	
	if (rgp->GetRewards().size() > 0)
	{
		rewardGroup.AddRewards(rgp->GetRewards());
	}
	return SUCCESS;
}

void ItemMgr::UseGiftPackGm(UInt32 itemId, UInt32 num)
{
	ItemDataEntry* itemData = ItemDataEntryMgr::Instance()->FindEntry(itemId);
	if (!itemData)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " itemData is null! id = " << itemId << LogStream::eos;
		return;
	}

	std::map<UInt32, UInt32> itemIds;
	std::map<UInt32, UInt32> itemNums;
	for (UInt32 i = 0; i < num; ++i)
	{
		GiftPackData* gpData = GiftPackDataMgr::Instance()->FindEntry(itemData->giftId);
		if (!gpData)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " GiftPackData is null! id = " << itemData->giftId << LogStream::eos;
			return;
		}

		if (GPFT_RANDOM == gpData->filterType)
		{
			//随机获得
			if (gpData->filterCount <= 0)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << " random gift pack filterCount error! value = " << gpData->filterCount << LogStream::eos;
				return;
			}

			std::vector<UInt32> wts;
			ItemRewardVec rewards;
			for (size_t i = 0; i < gpData->giftItems.size(); ++i)
			{
				GiftItemData* giData = GiftItemDataMgr::Instance()->FindEntry(gpData->giftItems[i]);
				if (!giData)
				{
					ErrorLogStream << PLAYERINFO(GetOwner()) << " GiftItemData is null! id = " << gpData->giftItems[i] << LogStream::eos;
					return;
				}
				if (giData->levels[0] > GetOwner()->GetLevel() || giData->levels[1] < GetOwner()->GetLevel())
				{
					continue;
				}
				wts.push_back(giData->weight);
				rewards.push_back(ItemReward(giData->itemId, giData->num, 1, giData->strengthen, 0, (EquipType)giData->equipType));
			}

			//随机filterCount个
			for (int i = 0; i < gpData->filterCount; ++i)
			{
				Int32 hitIdx = WeightRander::RandWeight(wts);
				if (hitIdx >= (Int32)rewards.size() || hitIdx < 0)
				{
					ErrorLogStream << PLAYERINFO(GetOwner()) << " hitIdx is invalid! hitidx = " << hitIdx << LogStream::eos;
					return;
				}
				//rewards.push_back(ItemReward(rewards[hitIdx].id, rewards[hitIdx].num, 0, rewards[hitIdx].strenth, 0, (EquipType)rewards[hitIdx].equipType));
				itemIds[rewards[hitIdx].id] = itemIds[rewards[hitIdx].id] + 1;
				itemNums[rewards[hitIdx].id] = itemNums[rewards[hitIdx].id] + rewards[hitIdx].num;
			}
		}

	}

	std::string rootpath = SSApplication::Instance()->GetGiftCountPath();
	std::string accidStr = Avalon::StringUtil::ConvertToString(GetOwner()->GetAccID());
	std::string itemIdStr = Avalon::StringUtil::ConvertToString(itemId);
	std::string numStr = Avalon::StringUtil::ConvertToString(num);

	std::string postfix = accidStr + "_" + itemIdStr + "_" + numStr + ".csv";
	std::string filename = rootpath + "OpenGiftCount_" + postfix;

	std::ofstream ofs;
	ofs.open(filename.c_str());
	if (!ofs.good())
	{
		ErrorLogStream << "FILE ptr is null!" << LogStream::eos;
		return;
	}
	std::string header = "itemid,name,times,num\n";
	ofs.write(header.c_str(), header.length());
	for (auto itr : itemIds)
	{
		auto data = ItemDataEntryMgr::Instance()->FindEntry(itr.first);
		if (!data)
		{
			continue;
		}

		std::ostringstream oss;
		oss << itr.first << ", " << data->name << ", " << itr.second << ", " << itemNums[itr.first] <<  "\n";
		std::string line = oss.str();
		ofs.write(line.c_str(), line.size());
	}

	ofs.close();
}

void ItemMgr::EquipItem(UInt32 id)
{
	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_ITEM);
	Item* item = ItemMaker::Instance()->MakeItem(GetOwner(), id, 1, 0);
	if (!item)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " make item error! id = " << id << LogStream::eos;
		return;
	}
	if (!AddItem(item, ItemPos(PACK_WEAR, item->GetEquipGrid())))
	{
		Item::DestroyItem(item);
		return;
	}
}

Item* ItemMgr::FindCarryOnItem(ObjID_t id)
{
	Item* item = FindItem(id);
	if(item != NULL && !IsCarryOnPackage(item->GetPos().pack)) return NULL;
	return item;
}

Item* ItemMgr::FindItem(ObjID_t id)
{
	return m_Items.FindObject(id);
}

bool ItemMgr::LoadItem(Item* item, bool newPos)
{
	if(item == NULL) return false;
	UInt8 pack = item->GetPos().pack;
	if(newPos) pack = GetPackType((ItemType)item->GetType());
	Package* pPackage = GetPackage(pack);
	if(pPackage == NULL) return false;

	//适配品级老数据
	AdaptQLData(item);

	//设置装备和时装的属性
	if (item->GetValue(IKVKT_WBP_BUILD_WEAPON) > 0)
	{
		if (!item->SetAttribute(true))
		{
			return false;
		}
	}
	else
	{
		if (!item->SetAttribute())
		{
			return false;
		}
	}
	


	if(!pPackage->AddItem(item,newPos?INVALID_ITEMGRID:item->GetPos().gridIndex))
	{
		return false;
	}

	if(!m_Items.AddObject(item))
	{
		pPackage->RemoveItem(item);
		return false;
	}

	item->ClearDirty();

	item->GenPreciousBeadHoleLoaded();

	if (item->GetDataEntry() && item->GetDataEntry()->expireTime > 0
		&& item->GetDeadLine() == item->GetDataEntry()->expireTime)
	{
		item->SetDeadLineSt(item->GetDataEntry()->expireTime);
	}
	return true;
}

bool ItemMgr::LoadStorageItem(Item* item, bool newPos)
{
	if (item == NULL)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " item is null!" << LogStream::eos;
		return false;
	}
		

	UInt8 pack = item->GetPos().pack;
	if (pack != PACK_STORAGE)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " storage item pack error! pack = " << pack << LogStream::eos;
		return false;
	}

	if (newPos) pack = PACK_STORAGE;
	Package* pPackage = GetPackage(pack);
	if (pPackage == NULL)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " pPackage is null! pcak = " << pack << LogStream::eos;
		return false;
	}
		

	//适配品级老数据
	AdaptQLData(item);

	//设置装备和时装的属性
	if (!item->SetAttribute())
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " item set attribute error! uid = " << item->GetID() << LogStream::eos;
		return false;
	}

	//仓库有超量的则发邮件
	InfoLogStream << PLAYERINFO(GetOwner()) << " storage capacity:" << pPackage->GetCapacity() << LogStream::eos;
	if (pPackage->GetCapacity() == 0)
	{
		m_MailGroup.push_back(item);

		InfoLogStream << PLAYERINFO(GetOwner()) << " storage over load,push to wait send mail list! storage size = " << pPackage->GetSize()
			<< "item uid:" << item->GetID() << LogStream::eos;
		return true;
	}

	if (!pPackage->AddItem(item, newPos ? INVALID_ITEMGRID : item->GetPos().gridIndex))
	{
		InfoLogStream << PLAYERINFO(GetOwner()) << " storage have load same grid item uid:" << item->GetID() << LogStream::eos;
		return false;
	}

	if (!m_Items.AddObject(item))
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " item add to items error! uid = " << item->GetID() << LogStream::eos;
		pPackage->RemoveItem(item);
		return false;
	}

	//是获取限量道具
// 	if (item->GetDataEntry()->getLimitNum > 0)
// 	{
// 		AddLimitGetItem(item);
// 	}

	if (item->GetDataEntry() && item->GetDataEntry()->expireTime > 0
		&& item->GetDeadLine() == 0)
	{
		item->SetDeadLineSt(item->GetDataEntry()->expireTime);
	}

	item->ClearDirty();

	item->GenPreciousBeadHoleLoaded();
	return true;
}

bool ItemMgr::AddItem(Item* item, const ItemPos pos)
{
	if(item == NULL) return false;
	Package* pPackage = GetPackage(pos.pack);
	if(pPackage == NULL) return false;

	item->SetAccId(GetOwner()->GetAccID());
	item->SetOwnerId(GetOwner()->GetID());
	if(!pPackage->AddItem(item,pos.gridIndex))
	{
		return false;
	}

	if(!m_Items.AddObject(item))
	{
		pPackage->RemoveItem(item);
		return false;
	}

	//计算装备分数
	CalculateValuedScore(item);

	//brief 存入数据库
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand(GetOwner()->GetSrcZoneId(), "t_item",item->GetID());
	item->GetDBData(cmd,false);
	//cmd->PutData("owner",GetOwner()->GetID());
	CLRecordClient::Instance()->SendCommand(cmd);

	NotifyNewItem(item);
	
	item->ClearDirty();
	return true;
}

UInt32  ItemMgr::GetNumByPartAndColor(UInt32 subtype, UInt8 color, bool permanent, PackType pack)
{
	Package* pPackage = GetPackage(pack);
	if (pPackage == NULL) return 0;

	//获取数量访问器
	class GetItemNumVisitor : public ItemVisitor
	{
	public:
		GetItemNumVisitor(UInt32 subtype, UInt8 color, bool permant)
			:m_Subtype(subtype), m_Color(color), m_Permant(permant), m_Num(0){}
		~GetItemNumVisitor(){}

		bool operator()(Item* item)
		{
			if (item->GetSubType() == m_Subtype && item->GetQuality() == m_Color && item->IsPermanent() == m_Permant)
			{
				m_Num = m_Num + item->GetNum();
			}
			return true;
		}

		UInt32  GetNum() const { return m_Num; }
	private:
		//subtype
		UInt32	m_Subtype;
		//颜色
		UInt8	m_Color;
		//是否永久
		bool    m_Permant;
		//数量
		UInt32  m_Num;
	};
	GetItemNumVisitor visitor(subtype, color, permanent);
	pPackage->VisitItems(visitor);

	return visitor.GetNum();
}

bool ItemMgr::RemoveItem(Item* item)
{
	if(item == NULL) return false;
	UInt8 packtype = item->GetPos().pack;
	Package* pack = GetPackage(packtype);
	if(pack != NULL) pack->RemoveItem(item);

	if(m_Items.FindObject(item->GetID()) == NULL) return false;


	m_Items.RemoveObject(item);

	if(item->GetType() == ITEM_EQUIP && IsWeapon(item->GetSubType()))
	{
		SortListMgr::Instance()->OnSortEntryDeleted(SORTLIST_WEAPON, item->GetID());
		//added by aprilliu.2017.04.22 01:43
		SortListMgr::Instance()->OnSortEntryDeleted(GetWeaponSortListTypeByItemThirdType(item->GetThirdType()), item->GetID());
		//end of aprilliu
	}

	//从数据库删除
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand(GetOwner()->GetSrcZoneId(), "t_item", item->GetID());
	CLRecordClient::Instance()->SendCommand(cmd);

	//通知客户端
	CLProtocol::SceneNotifyDeleteItem notify;
	notify.id = item->GetID();
	GetOwner()->SendProtocol(notify);

	return true;
}

//added by aprilliu,2017.04.22 
void ItemMgr::OnItemChangedSortList(Item* pItem)
{
	if (NULL == pItem)
	{
		return;
	}

	//只关心装备-武器类
	if (pItem->GetType() == ITEM_EQUIP && IsWeapon(pItem->GetSubType()))
	{
		//
		SortListMgr::Instance()->OnSortEquipValueChanged(SORTLIST_WEAPON, pItem->GetID(), pItem->GetName(), pItem->GetQuality(), 0,
			GetOwner()->GetID(), pItem->GetNeedLevel(), (UInt32)pItem->GetStrengthen(), pItem->GetValuedScore(), pItem->GetDataID(), pItem->GetEquipType(), pItem->GetEnhanceType());
		SortListType listType = GetWeaponSortListTypeByItemThirdType(pItem->GetThirdType());
		if (SORTLIST_INVALID != listType)
		{
			SortListMgr::Instance()->OnSortEquipValueChanged(listType, pItem->GetID(), pItem->GetName(), pItem->GetQuality(), 0,
				GetOwner()->GetID(), pItem->GetNeedLevel(), (UInt32)pItem->GetStrengthen(), pItem->GetValuedScore(), pItem->GetDataID(), pItem->GetEquipType(), pItem->GetEnhanceType());
		}

	}


	return;
}

bool ItemMgr::RemoveItemNoSync(Item* item)
{
	if (item == NULL) return false;
	UInt8 packtype = item->GetPos().pack;
	Package* pack = GetPackage(packtype);
	if (pack != NULL) pack->RemoveItem(item);

	if (m_Items.FindObject(item->GetID()) == NULL) return false;

	m_Items.RemoveObject(item);

	return true;
}

Int16 ItemMgr::GetGridUsed(UInt32 itemid, UInt16 num, UInt32 auctionCoolTime, UInt8 strengthen, UInt32 auctionTransTimes)
{
	ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(itemid);
	if (dataEntry == NULL) return -1;

	PackType pack = GetPackType((ItemType)dataEntry->type);
	Package* pPackage = GetPackage(pack);
	if (pPackage == NULL) return -1;

	//不能叠加
	if(dataEntry->maxNum == 1) return num;
	//quality = ValidateItemQuality(dataEntry->type, dataEntry->subType, quality); 
	
	//检查合并访问器
	class CheckAddItemVisitor : public ItemVisitor
	{
	public:
		CheckAddItemVisitor(UInt32 id, UInt16 num, UInt32 auctionCoolTime, UInt8 strengthen, UInt32 auctionTransTimes)
			:m_ItemID(id), m_Num(num), m_auctionCoolTime(auctionCoolTime), m_strengthen(strengthen), m_auctionTransTimes(auctionTransTimes){}
		~CheckAddItemVisitor(){}

		bool operator()(Item* item)
		{
			if(item->GetDataID() == m_ItemID)
			{
				m_Num = m_Num - item->CheckMerging(m_Num, m_auctionCoolTime, m_strengthen, m_auctionTransTimes);
				if(m_Num == 0) return false;
			}
			return true;
		}

		UInt16 GetNum() const { return m_Num; }
	private:
		//道具id
		UInt32	m_ItemID;
		//数量
		UInt16	m_Num;
		//拍卖行交易冷却时间
		UInt32  m_auctionCoolTime;
		//strengthen值
		UInt8	m_strengthen;
		//拍卖行交易次数
		UInt32	m_auctionTransTimes;
	};
	CheckAddItemVisitor visitor(itemid, num, auctionCoolTime, strengthen, auctionTransTimes);
	pPackage->VisitItems(visitor);
	num = visitor.GetNum();

	if (0 == dataEntry->maxNum)
		return -1;
	else
		return UInt16(( num + dataEntry->maxNum - 1 ) / dataEntry->maxNum);
}

bool ItemMgr::CheckAddItem(UInt32 itemid, UInt16 num, ItemType itemType, UInt32 auctionCoolTime, UInt8 strengthen, UInt32 auctionTransTimes)
{
	PackType pack = GetPackType(itemType);
	Package* pPackage = GetPackage(pack);
	if(pPackage == NULL) return false;

	if (0 == pPackage->GetCapacity())
		return false;

	Int16 gridUsed = GetGridUsed(itemid, num, auctionCoolTime, strengthen, auctionTransTimes);
	if (gridUsed >= 0)
		return pPackage->GetCapacity() >= (UInt32)gridUsed;
	else
		return false;
}

bool ItemMgr::CheckAddStorage(Item* item, UInt16 num, UInt8 storageType)
{
	if (!item)	return false;
	ItemDataEntry* dataEntry = item->GetDataEntry();
	if (!dataEntry)	return false;
	
	Package* pPackage = GetPackage(storageType);
	if (pPackage == NULL) return false;

	Int16 gridUsed = 0;
	//不能叠加
	if (dataEntry->maxNum == 1)		gridUsed = num;

	//检查合并访问器
	class CheckAddItemVisitor : public ItemVisitor
	{
	public:
		CheckAddItemVisitor(UInt32 id, UInt16 num, UInt32 auctionCoolTime, UInt8 strengthen, UInt32 auctionTransTimes)
			:m_ItemID(id), m_Num(num), m_auctionCoolTime(auctionCoolTime), m_strengthen(strengthen), m_auctionTransTimes(auctionTransTimes){}
		~CheckAddItemVisitor() {}

		bool operator()(Item* item)
		{
			if (item->GetDataID() == m_ItemID)
			{
				m_Num = m_Num - item->CheckMerging(m_Num, m_auctionCoolTime, m_strengthen, m_auctionTransTimes);
				if (m_Num == 0) return false;
			}
			return true;
		}

		UInt16 GetNum() const { return m_Num; }
	private:
		//道具id
		UInt32	m_ItemID;
		//数量
		UInt16	m_Num;
		//拍卖行交易冷却时间
		UInt32  m_auctionCoolTime;
		//强化等级
		UInt8   m_strengthen;
		//拍卖行交易次数
		UInt32  m_auctionTransTimes;
	};
	CheckAddItemVisitor visitor(dataEntry->id, num, item->GetAuctionCoolTimeStamp(), item->GetStrengthen(), item->GetAuctionTransNum());
	pPackage->VisitItems(visitor);

	gridUsed = (visitor.GetNum() + dataEntry->maxNum - 1) / dataEntry->maxNum;

	return pPackage->GetCapacity() >= (UInt32)gridUsed;
}
	
bool ItemMgr::CheckOneKeyAddStorage(UInt32 itemId, UInt32 auctionCoolTimeStamp, UInt8 strengthen, UInt32 auctionTransTimes)
{
	Package* pPackage = GetPackage(PACK_STORAGE);
	if (pPackage == NULL) return false;

	//检查合并访问器
	class CheckOneKeyVisitor : public ItemVisitor
	{
	public:
		CheckOneKeyVisitor(UInt32 id, UInt32 auctionCoolTimeStamp, UInt8 strengthen, UInt32 auctionTransTimes)
			:m_ItemID(id), m_canAdd(false), m_auctionCoolTimeStamp(auctionCoolTimeStamp), m_strengthen(strengthen), m_auctionTransTimes(auctionTransTimes){}
		~CheckOneKeyVisitor() {}

		bool operator()(Item* item)
		{
			if (item->GetDataID() == m_ItemID)
			{
				if (item->GetAuctionCoolTimeStamp() > 0 || m_auctionCoolTimeStamp > 0)
				{
					UInt32 curtime = (UInt32)CURRENT_TIME.Sec();
					if ((m_auctionCoolTimeStamp > curtime || item->GetAuctionCoolTimeStamp() > curtime)
						&& m_auctionCoolTimeStamp != item->GetAuctionCoolTimeStamp())
					{
						return true;
					}
				}

				if (item->GetStrengthen() != m_strengthen)
				{
					return true;
				}

				if (item->GetAuctionTransNum() != m_auctionTransTimes)
				{
					return true;
				}

				m_canAdd = true;
				return false;
			}

			return true;
		}

		bool GetCanAdd() const { return m_canAdd; }
	private:
		UInt32 m_ItemID;
		bool m_canAdd;
		UInt32 m_auctionCoolTimeStamp;
		UInt8  m_strengthen;
		UInt32	m_auctionTransTimes;
	};
	CheckOneKeyVisitor visitor(itemId, auctionCoolTimeStamp, strengthen, auctionTransTimes);
	pPackage->VisitItems(visitor);
	return visitor.GetCanAdd();
}

bool ItemMgr::CheckAddRoleStorage(Item* item, UInt16 num, UInt32 gridIndex)
{
	if (!item)	return false;
	ItemDataEntry* dataEntry = item->GetDataEntry();
	if (!dataEntry)	return false;

	Package* pPackage = GetPackage(PACK_ROLE_STORAGE);
	if (pPackage == NULL) return false;

	UInt32 page = GetRoleStoragePageByGrid(gridIndex);
	if (page > GetOwner()->GetCounter(COUNTER_ROLE_STORAGE_OPEN_NUM))
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " bad role storage page = " << page << " gridIndex = " << gridIndex << LogStream::eos;
		return false;
	}

	Int16 gridUsed = 0;
	//不能叠加
	if (dataEntry->maxNum == 1)		gridUsed = num;

	//检查合并访问器
	class CheckAddItemVisitor : public ItemVisitor
	{
	public:
		CheckAddItemVisitor(UInt32 id, UInt16 num, UInt32 auctionCoolTime, UInt8 strengthen, UInt32 page, UInt32 auctionTransTimes)
			:m_ItemID(id), m_Num(num), m_auctionCoolTime(auctionCoolTime), m_strengthen(strengthen), m_page(page), m_auctionTransTimes(auctionTransTimes)
		{
			m_gridUsedCount = 0;
		}

		~CheckAddItemVisitor() {}

		bool operator()(Item* item)
		{
			//把不是同一页的排除掉
			if (m_page != GetRoleStoragePageByGrid(item->GetPos().gridIndex))
			{
				return true;
			}

			if (item->GetDataID() == m_ItemID)
			{
				m_Num = m_Num - item->CheckMerging(m_Num, m_auctionCoolTime, m_strengthen, m_auctionTransTimes);
				if (m_Num == 0) return false;
			}

			++m_gridUsedCount;
			return true;
		}

		UInt16 GetNum() const { return m_Num; }

		UInt32 GetCapacity() const { return m_gridUsedCount < MAX_GRID_NUM_PER_ROLE_STORAGE_PAGE ? (MAX_GRID_NUM_PER_ROLE_STORAGE_PAGE - m_gridUsedCount) : 0; };
	private:
		//道具id
		UInt32	m_ItemID;
		//数量
		UInt16	m_Num;
		//拍卖行交易冷却时间
		UInt32  m_auctionCoolTime;
		//强化等级
		UInt8   m_strengthen;
		//当前页
		UInt32	m_page;
		//已占格子总数量
		UInt32	m_gridUsedCount;
		//拍卖行交易次数
		UInt32	m_auctionTransTimes;
	};

	CheckAddItemVisitor visitor(dataEntry->id, num, item->GetAuctionCoolTimeStamp(), item->GetStrengthen(), page, item->GetAuctionTransNum());
	pPackage->VisitItems(visitor);

	gridUsed = (visitor.GetNum() + dataEntry->maxNum - 1) / dataEntry->maxNum;

	return visitor.GetCapacity() >= gridUsed;
}

//先合并
class AddItemVisitor : public ItemVisitor
{
public:
	AddItemVisitor(UInt32 id, UInt16 num, Player* player, const char* reason)
		:m_ItemID(id), m_Num(num), m_pPlayer(player), m_sReason(reason), m_auctionCoolTimeStamp(0), m_strengthen(0), m_auctionTransTimes(0){}
	AddItemVisitor(UInt32 id, UInt16 num, Player* player, const char* reason, UInt32 auctionCoolTimeStamp, UInt8 strengthen, UInt32 auctionTransTimes)
		:m_ItemID(id), m_Num(num), m_pPlayer(player), m_sReason(reason), m_auctionCoolTimeStamp(auctionCoolTimeStamp), m_strengthen(strengthen), m_auctionTransTimes(auctionTransTimes){}
	~AddItemVisitor(){}

	bool operator()(Item* item)
	{
		if(item->GetDataID() == m_ItemID)
		{
			UInt16 mergenum = item->Merge(m_sReason, m_pPlayer, m_Num, m_auctionCoolTimeStamp, m_strengthen, m_auctionTransTimes);
			
			item->SetParam1(1);	// 设置是新道具标识
			if (mergenum != 0) 
			{
				item->SyncProperty(m_pPlayer);
				m_pPlayer->OnMakeItem(m_sReason, item, mergenum, false);
				mergedItems.push_back(item->GetID());
			}

			m_Num = m_Num - mergenum;
			if (m_Num == 0)
			{
				//通知更新道具
				UpdateItemParam param;
				param._updateItem = (void*)item;
				CLEventMgr::Instance()->SendPlayerEvent(m_pPlayer->GetID(), CLEventMgr::ET_NOTIFY_UPDATEITEM, &param);
				return false;
			}
				
		}
		return true;
	}

	UInt16 GetNum() const { return m_Num; }

	std::vector<ObjID_t>& GetMergedItems() { return mergedItems; }


private:
	//道具id
	UInt32	m_ItemID;
	//数量
	UInt16	m_Num;
	//玩家
	Player*	m_pPlayer;
	//理由
	const char* m_sReason;
	//拍卖行交易冻结时间
	UInt32 m_auctionCoolTimeStamp;
	//强化等级
	UInt8  m_strengthen;
	//拍卖行交易次数
	UInt32 m_auctionTransTimes;
	//合并到的道具的guid集合
	std::vector<ObjID_t> mergedItems;
};

UInt16 ItemMgr::AddItem(const char* reason, UInt32 itemid, UInt16 num, UInt8 qualityLv, UInt8 strenth,
	ItemType itemType, UInt32 auctionCoolTimeStamp, UInt32 auctionTransTimes, UInt8 equipType, UInt8 enhanceType)
{
	// 先遍历背包合并	
	PackType pack = GetPackType(itemType);
	Package* pPackage = GetPackage(pack);
	if (pPackage == NULL) return 0;

	AddItemVisitor visitor(itemid, num, GetOwner(), reason, auctionCoolTimeStamp, strenth, auctionTransTimes);
	pPackage->VisitItems(visitor);

	UInt16 leftnum = visitor.GetNum();
	if(leftnum == 0) return num;

	// 没合并完的创建新的
	Item* item = ItemMaker::Instance()->MakeItem(GetOwner(), itemid, leftnum, qualityLv, strenth, auctionCoolTimeStamp, equipType, enhanceType, false, auctionTransTimes);
	if (!item)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " make item error! id = " << itemid << LogStream::eos;
		return 0;
	}

	while(item != NULL)
	{
		if(!AddItem(item, ItemPos(pack, INVALID_ITEMGRID)))
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " add item fail id = " << itemid << ", num = " << item->GetNum() << LogStream::eos;
			Item::DestroyItem(item);
			break;
		}
		
		GetOwner()->OnMakeItem(reason, item, leftnum);

		leftnum = leftnum - item->GetNum();
		item = ItemMaker::Instance()->MakeItem(GetOwner(), itemid, leftnum, qualityLv, strenth, auctionCoolTimeStamp, equipType, enhanceType, false, auctionTransTimes);
	}
	
	UInt16 addnum = num - leftnum;
	if(addnum != num)
	{
		//TODO
		InfoLogStream << "add item addnum != num, num : " << num << ", addnum : " << addnum << LogStream::eos;
	}
	return addnum;
}

UInt16 ItemMgr::AddItemGetGuid(const char* reason, UInt32 itemid, UInt16 num,
	UInt8 qualityLv, UInt8 strenth, ItemType itemType, UInt32 auctionCoolTimeStamp,
	UInt32 auctionTransTimes, std::vector<ObjID_t>& guids)
{
	// 先遍历背包合并	
	PackType pack = GetPackType(itemType);
	Package* pPackage = GetPackage(pack);
	if (pPackage == NULL) return 0;

	AddItemVisitor visitor(itemid, num, GetOwner(), reason, auctionCoolTimeStamp, strenth, auctionTransTimes);
	pPackage->VisitItems(visitor);

	UInt16 leftnum = visitor.GetNum();
	guids = visitor.GetMergedItems();
	if (leftnum == 0) return num;

	// 没合并完的创建新的
	Item* item = ItemMaker::Instance()->MakeItem(GetOwner(), itemid, leftnum, qualityLv, strenth, auctionCoolTimeStamp, EQUIP_NORMAL, ENHANCE_INVALID, false, auctionTransTimes);
	if (!item)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " make item error! id = " << itemid << LogStream::eos;
		return 0;
	}

	while (item != NULL)
	{
		if (!AddItem(item, ItemPos(pack, INVALID_ITEMGRID)))
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " add item fail id = " << itemid << ", num = " << item->GetNum() << LogStream::eos;
			Item::DestroyItem(item);
			break;
		}

		GetOwner()->OnMakeItem(reason, item, leftnum);
		guids.push_back(item->GetID());

		leftnum = leftnum - item->GetNum();
		item = ItemMaker::Instance()->MakeItem(GetOwner(), itemid, leftnum, qualityLv, strenth, auctionCoolTimeStamp, EQUIP_NORMAL, ENHANCE_INVALID, false, auctionTransTimes);
	}

	UInt16 addnum = num - leftnum;
	if (addnum != num)
	{
		//TODO
		InfoLogStream << "add item addnum != num, num : " << num << ", addnum : " << addnum << LogStream::eos;
	}
	return addnum;
}

UInt16 ItemMgr::AddItemWithCallBack(const char* reason, UInt32 itemid, UInt16 num,
	UInt8 qualityLv, UInt8 strenth, ItemType itemType,
	const std::function<bool(Item&)>& initCallback,
	const std::function<void(Item&)>& makedCallback,
	UInt32 auctionCoolTime)
{
	// 先遍历背包合并	
	PackType pack = GetPackType(itemType);
	Package* pPackage = GetPackage(pack);
	if (pPackage == NULL) return 0;

	AddItemVisitor visitor(itemid, num, GetOwner(), reason, auctionCoolTime, strenth, 0);
	pPackage->VisitItems(visitor);

	UInt16 leftnum = visitor.GetNum();
	if (leftnum == 0) return num;

	// 没合并完的创建新的
	Item* item = ItemMaker::Instance()->MakeItemWithCallback(GetOwner(), itemid, leftnum, initCallback, qualityLv, strenth, auctionCoolTime);
	if (!item)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " make item error! id = " << itemid << LogStream::eos;
		return 0;
	}

	while (item != NULL)
	{
		if (!AddItem(item, ItemPos(pack, INVALID_ITEMGRID)))
		{
			Item::DestroyItem(item);
			break;
		}

		GetOwner()->OnMakeItem(reason, item, leftnum);
		if (makedCallback != nullptr) makedCallback(*item);

		leftnum = leftnum - item->GetNum();
		item = ItemMaker::Instance()->MakeItemWithCallback(GetOwner(), itemid, leftnum, initCallback, qualityLv, strenth, auctionCoolTime);
	}

	UInt16 addnum = num - leftnum;
	if (addnum != num)
	{
		//TODO
	}
	return addnum;
}

UInt16 ItemMgr::AddItem(const char* reason, Item* item, UInt8 pack)
{
	if(item == NULL) return 0;

	Package* pPackage = GetPackage(pack);
	if(pPackage == NULL) return 0;

	UInt16 num = item->GetNum();
	AddItemVisitor visitor(item->GetDataID(), num, GetOwner(), reason, item->GetAuctionCoolTimeStamp(), item->GetStrengthen(), item->GetAuctionTransNum());
	pPackage->VisitItems(visitor);

	UInt16 leftnum = visitor.GetNum();
	if(leftnum == 0)
	{
		Item::DestroyItem(item);
		return num;
	}

	item->SetNum(leftnum);
	if(!AddItem(item, ItemPos(pack, INVALID_ITEMGRID)))
	{
		if(num != leftnum)
		{
			Item::DestroyItem(item);
		}
		return num - leftnum;
	}

	GetOwner()->SendItemUdpLog(reason, item->GetID(), item->GetDataID(), (ItemType)item->GetType(), (ItemSubType)item->GetSubType(), item->GetName(), (UInt32)item->GetQuality(), 
		leftnum, 0, item->GetStrengthen(), item->GetEquipType(), item->GetEnhanceType());

	return num;
}

UInt32 ItemMgr::GetItemNum(UInt32 itemid, UInt8 pack, UInt8 bindType)
{
	Package* pPackage = GetPackage(GetPackType((ItemType)pack));
	if(pPackage == NULL) return 0;

	//获取数量访问器
	class GetItemNumVisitor : public ItemVisitor
	{
	public:
		GetItemNumVisitor(UInt32 id, UInt8 bindType)
			:m_ItemID(id),m_Num(0),m_BindType(bindType){}
		~GetItemNumVisitor(){}

		bool operator()(Item* item)
		{
			if(item->GetDataID() == m_ItemID)
			{
				m_Num = m_Num + item->GetNum();
			}
			return true;
		}

		UInt32  GetNum() const { return m_Num; }
	private:
		//道具id
		UInt32	m_ItemID;
		//数量
		UInt32	m_Num;
		//绑定类型
		UInt8	m_BindType;
	};
	GetItemNumVisitor visitor(itemid, bindType);
	pPackage->VisitItems(visitor);

	return visitor.GetNum();
}

UInt32 ItemMgr::GetItemNumWithFilter(UInt32 itemid, UInt8 pack, std::function<bool(Item*)>& filterCallback, UInt8 bindType)
{
	Package* pPackage = GetPackage(GetPackType((ItemType)pack));
	if (pPackage == NULL) return 0;

	//获取数量访问器
	class GetItemNumVisitor : public ItemVisitor
	{
	public:
		GetItemNumVisitor(UInt32 id, UInt8 bindType, std::function<bool(Item*)>& filter)
			:m_ItemID(id), m_Num(0), m_BindType(bindType), m_filter(filter){}
		~GetItemNumVisitor() {}

		bool operator()(Item* item)
		{
			if (item->GetDataID() == m_ItemID)
			{
				if (m_filter != nullptr && m_filter(item))
				{
					return true;
				}
				m_Num = m_Num + item->GetNum();
			}
			return true;
		}

		UInt32  GetNum() const { return m_Num; }
	private:
		//道具id
		UInt32	m_ItemID;
		//数量
		UInt32	m_Num;
		//绑定类型
		UInt8	m_BindType;
		//过滤
		std::function<bool(Item*)>& m_filter;
	};
	GetItemNumVisitor visitor(itemid, bindType, filterCallback);
	pPackage->VisitItems(visitor);

	return visitor.GetNum();
}

UInt32 ItemMgr::GetItemNumByPackType(UInt32 itemid, PackType pack)
{
	Package* pPackage = GetPackage(pack);
	if (pPackage == NULL) return 0;

	//获取数量访问器
	class GetItemNumVisitor : public ItemVisitor
	{
	public:
		GetItemNumVisitor(UInt32 id, UInt8 bindType)
			:m_ItemID(id), m_Num(0), m_BindType(bindType){}
		~GetItemNumVisitor(){}

		bool operator()(Item* item)
		{
			if (item->GetDataID() == m_ItemID)
			{
				m_Num = m_Num + item->GetNum();
			}
			return true;
		}

		UInt32  GetNum() const { return m_Num; }
	private:
		//道具id
		UInt32	m_ItemID;
		//数量
		UInt32	m_Num;
		//绑定类型
		UInt8	m_BindType;
	};
	GetItemNumVisitor visitor(itemid, 0);
	pPackage->VisitItems(visitor);

	return visitor.GetNum();
}

UInt16 ItemMgr::RemoveItem(const char* reason, UInt32 itemid, UInt16 num, UInt8 pack)
{
	Package* pPackage = GetPackage(pack);
	if(pPackage == NULL) return 0;

	//移除道具访问器
	class RemoveItemVisitor : public ItemVisitor
	{
	public:
		RemoveItemVisitor(UInt32 itemid, UInt16 num, ItemMgr* mgr, Player* player, const char* reason)
			:m_ItemID(itemid), m_Num(num), m_pItemMgr(mgr), m_pPlayer(player), m_sReason(reason){}
		~RemoveItemVisitor(){}

		bool operator()(Item* item)
		{
			if(item->GetDataID() == m_ItemID)
			{
				UInt16 itemnum = item->GetNum();
				if(itemnum <= m_Num)
				{
					m_Num = m_Num - itemnum;
					m_pItemMgr->RemoveItem(item);
					m_pPlayer->OnRemoveItem(m_sReason, item, itemnum, itemnum);
					Item::DestroyItem(item);
					return m_Num > 0;
				}
				else
				{
					UInt16 itemNum = item->GetNum();
					item->SetNum(itemnum - m_Num);
					
					item->SyncProperty(m_pItemMgr->GetOwner());
					m_pPlayer->OnRemoveItem(m_sReason, item, m_Num, itemNum);
					m_Num = 0;
					return false;
				}
			}
			return true;
		}

		UInt16 GetLeftNum() const{ return m_Num; }
	public:
		//道具id
		UInt32	m_ItemID;
		//道具数量
		UInt16	m_Num;
		//道具管理器
		ItemMgr*	m_pItemMgr;
		//玩家
		Player*		m_pPlayer;
		//理由
		const char* m_sReason;
	};

	UInt32 leftNum = num;
	
	RemoveItemVisitor visitor(itemid, leftNum, this, GetOwner(), reason);
	pPackage->VisitItems(visitor);
	if(visitor.GetLeftNum() == 0) return num;
	leftNum = visitor.GetLeftNum();
	
	return num - leftNum;
}

UInt16 ItemMgr::RemoveItemWithFilter(const char* reason, UInt32 itemid, UInt16 num, UInt8 pack, std::function<bool(Item*)>& filterCallback)
{
	Package* pPackage = GetPackage(pack);
	if (pPackage == NULL) return 0;

	//移除道具访问器
	class RemoveItemVisitor : public ItemVisitor
	{
	public:
		RemoveItemVisitor(UInt32 itemid, UInt16 num, ItemMgr* mgr, Player* player, const char* reason, std::function<bool(Item*)>& filter)
			:m_ItemID(itemid), m_Num(num), m_pItemMgr(mgr), m_pPlayer(player), m_sReason(reason), m_filter(filter){}
		~RemoveItemVisitor() {}

		bool operator()(Item* item)
		{
			if (item->GetDataID() == m_ItemID)
			{
				if (m_filter != nullptr && m_filter(item))
				{
					return true;
				}
				UInt16 itemnum = item->GetNum();
				if (itemnum <= m_Num)
				{
					m_Num = m_Num - itemnum;
					m_pItemMgr->RemoveItem(item);
					m_pPlayer->OnRemoveItem(m_sReason, item, itemnum, itemnum);
					Item::DestroyItem(item);
					return m_Num > 0;
				}
				else
				{
					UInt16 itemNum = item->GetNum();
					item->SetNum(itemnum - m_Num);

					item->SyncProperty(m_pItemMgr->GetOwner());
					m_pPlayer->OnRemoveItem(m_sReason, item, m_Num, itemNum);
					m_Num = 0;
					return false;
				}
			}
			return true;
		}

		UInt16 GetLeftNum() const { return m_Num; }
	public:
		//道具id
		UInt32	m_ItemID;
		//道具数量
		UInt16	m_Num;
		//道具管理器
		ItemMgr*	m_pItemMgr;
		//玩家
		Player*		m_pPlayer;
		//理由
		const char* m_sReason;
		//过滤
		std::function<bool(Item*)>& m_filter;
	};

	UInt32 leftNum = num;

	RemoveItemVisitor visitor(itemid, leftNum, this, GetOwner(), reason, filterCallback);
	pPackage->VisitItems(visitor);
	if (visitor.GetLeftNum() == 0) return num;
	leftNum = visitor.GetLeftNum();

	return num - leftNum;
}

UInt32 ItemMgr::RemoveLessScoreItem(const char* reason, UInt32 itemid, UInt32 num, UInt8 pack)
{
	if (reason == NULL) return 0;
	if (num == 0) return 0;

	Package* pPackage = GetPackage(pack);
	if (pPackage == NULL)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Get Package(" << pack << ") failed!" << LogStream::eos;
		return 0;
	}

	class SelectItemVisitor : public ItemVisitor
	{
	public:
		SelectItemVisitor(UInt32 itemId) : m_ItemDataId(itemId) {}

	public:
		bool operator()(Item *item)
		{
			if (!item)
			{
				ErrorLogStream << "Item ptr is null!" << LogStream::eos;
				return true;
			}

			if (item->GetDataID() == m_ItemDataId)
			{
				m_Items.push_back(item);
			}

			return true;
		}

		std::vector<Item*>& GetItems() { return m_Items; }

	private:
		//道具id
		UInt32	m_ItemDataId;
		std::vector<Item*> m_Items;
	};

	SelectItemVisitor visitor(itemid);
	pPackage->VisitItems(visitor);

	if (visitor.GetItems().empty())
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "No item(" << itemid << ") in package(" << pack << ")!" << LogStream::eos;
		return 0;
	}

	struct CustomItemLessScoreCompare
	{
		bool operator()(const Item* lhs, const Item* rhs)
		{
			return lhs->GetValuedScore() < rhs->GetValuedScore();
		}
	};

	// 装备评分较低的先删除
	std::sort(visitor.GetItems().begin(), visitor.GetItems().end(), CustomItemLessScoreCompare());

	UInt32 restNum = num;

	for (auto item : visitor.GetItems())
	{
		if (restNum == 0) break;

		UInt32 itemnum = (UInt32)item->GetNum();
		if (itemnum <= restNum)
		{
			restNum -= itemnum;
			RemoveItem(item);
			GetOwner()->OnRemoveItem(reason, item, itemnum, itemnum);
			Item::DestroyItem(item);
		}
		else
		{
			UInt32 itemNum = (UInt32)item->GetNum();
			item->SetNum(itemnum - restNum);

			item->SyncProperty(GetOwner());
			GetOwner()->OnRemoveItem(reason, item, restNum, itemNum);
			restNum = 0;
			break;
		}
	}

	return num - restNum;
}

Int32 ItemMgr::RemoveAllItemById(UInt32 itemid)
{
	ItemDataEntry* itemData = ItemDataEntryMgr::Instance()->FindEntry(itemid);
	if (!itemData)	return -1;
	
	Package* pPackage = GetPackage(GetPackType((ItemType)itemData->type));
	if (!pPackage) return -1;

	//移除道具访问器
	class RemoveItemVisitor : public ItemVisitor
	{
	public:
		RemoveItemVisitor(UInt32 itemid, ItemMgr* mgr)
			:m_ItemID(itemid), m_pItemMgr(mgr), m_Num(0) {}
		~RemoveItemVisitor() {}

		bool operator()(Item* item)
		{
			if (item->GetDataID() == m_ItemID)
			{
				m_Num += item->GetNum();
				m_pItemMgr->RemoveItem(item);
				Item::DestroyItem(item);
			}
			return true;
		}

	public:
		UInt32 GetDelNum() { return m_Num; }
		//道具id
		UInt32	m_ItemID;
		//道具管理器
		ItemMgr*	m_pItemMgr;
		UInt32		m_Num;
	};


	RemoveItemVisitor visitor(itemid, this);
	pPackage->VisitItems(visitor);
	
	return visitor.GetDelNum();
}

class ItemSorter
{
public:
	bool operator()(const Item* item1,const Item* item2) const
	{
		if(item1->GetDataID() < item2->GetDataID()) return true;
		else if(item1->GetDataID() > item2->GetDataID()) return false;

		if(item1->GetQuality() > item2->GetQuality()) return true;
		else if(item1->GetQuality() < item2->GetQuality()) return false;

		if(item1->GetSubType() < item2->GetSubType()) return true;
		else if(item1->GetSubType() > item2->GetSubType()) return false;

// 		if(item1->GetDataEntry()->needlevel < item2->GetDataEntry()->needlevel) return true;
// 		else if(item1->GetDataEntry()->needlevel > item2->GetDataEntry()->needlevel) return false;
// 
// 		if(item1->GetDataEntry()->needoccu < item2->GetDataEntry()->needoccu) return true;
// 		else if(item1->GetDataEntry()->needoccu > item2->GetDataEntry()->needoccu) return false;

		if(item1->GetBind() < item2->GetBind()) return true;
		else if(item1->GetBind() > item2->GetBind()) return false;

		if(item1->GetNum() > item2->GetNum()) return true;
		else return false;
	};
};

void ItemMgr::TrimItems(UInt8 pack)
{
	class MergeItemVisitor : public ItemVisitor
	{
	public:
		MergeItemVisitor(ItemMgr* itemMgr, Player* player) :m_pItemMgr(itemMgr), m_pPlayer(player) {}
		~MergeItemVisitor(){}

	public:
		bool operator()(Item* item)
		{
			std::string reason = GetReason(LogSourceType::SOURCE_TYPE_ITEM_TRIM);
			for(std::vector<Item*>::iterator iter = m_Items.begin(); iter != m_Items.end(); ++iter)
			{
				if(item->GetDataID() != (*iter)->GetDataID()) continue;

				UInt16 mergeNum = (*iter)->Merge(reason.c_str(), m_pPlayer, item->GetNum(), item->GetAuctionCoolTimeStamp(), item->GetStrengthen(), item->GetAuctionTransNum());
				UInt16 oldNum = item->GetNum();
				item->SetNum(item->GetNum() - mergeNum);
				m_pPlayer->OnRemoveItem(reason.c_str(), item, mergeNum, oldNum);
				if(item->GetNum() == 0)
				{
					m_pItemMgr->RemoveItem(item);
					Item::DestroyItem(item);
					return true;
				}
			}
			m_Items.push_back(item);
			return true;
		}

		bool SyncItems()
		{
			Avalon::Packet* packet = Avalon::Packet::Create();
			packet->SetID(CLProtocol::SCENE_SYNC_ITEM_PROPERTY);

			Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());

// 目前服务器不用排序
// 			try
// 			{
// 				ItemSorter sorter;
// 				std::set<Int32>	types;
// 				for(size_t i = 0; i < sizeof(ITEMTYPE_SORT_ORDER) / sizeof(Int32); ++i)
// 				{
// 					std::vector<Item*>& items = m_Items[ITEMTYPE_SORT_ORDER[i]];
// 					std::sort(items.begin(), items.end(), sorter);
// 					AddToPackage(stream, items, pack);
// 					types.insert(ITEMTYPE_SORT_ORDER[i]);
// 				}
// 
// 				for(int i = 1; i < ITEM_MAX; ++i)
// 				{
// 					if(types.find(i) != types.end()) continue;
// 
// 					std::sort(m_Items[i].begin(), m_Items[i].end(), sorter);
// 					AddToPackage(stream, m_Items[i], pack);
// 				}
// 				
// 				stream & ObjID_t(0);
// 			}
// 			catch(Avalon::NetException&)
// 			{
// 				Avalon::Packet::Destroy(packet);
// 				return false;
// 			}

			SyncItems(stream, m_Items);
			packet->SetSize(stream.GetPos());
			if(packet->GetSize() > sizeof(ObjID_t))
			{
				m_pItemMgr->GetOwner()->SendPacket(packet);
			}
			Avalon::Packet::Destroy(packet);
			return true;
		}

	private:
		void AddToPackage(Avalon::NetOutputStream& stream, std::vector<Item*>& items, Package* pack)
		{
			for(std::vector<Item*>::iterator iter = items.begin();
				iter != items.end(); ++iter)
			{
				Item* item = *iter;
				pack->AddItem(item);
				if(item->IsGridNumDirty(SST_SELF))
				{
					stream & item->GetID();
					item->Encode(stream, SST_SELF, true);

						//同步到数据库
					CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand(m_pItemMgr->GetOwner()->GetSrcZoneId(), "t_item" ,item->GetID() ,false);
					item->GetDBData(cmd, true);
					CLRecordClient::Instance()->SendCommand(cmd);
				}
			}
			items.clear();
		}

		void SyncItems(Avalon::NetOutputStream& stream, std::vector<Item*>& items)
		{
			for (std::vector<Item*>::iterator iter = items.begin();
			iter != items.end(); ++iter)
			{
				Item* item = *iter;
				stream & item->GetID();
				item->Encode(stream, SST_SELF, true);

				//同步到数据库
				CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand(m_pItemMgr->GetOwner()->GetSrcZoneId(), "t_item", item->GetID(), false);
				item->GetDBData(cmd, true);
				CLRecordClient::Instance()->SendCommand(cmd);
				
			}
			items.clear();
		}

	private:
		std::vector<Item*>		m_Items;
		ItemMgr*	m_pItemMgr;
		Player*		m_pPlayer;
	};

	MergeItemVisitor visitor(this, GetOwner());
	Package* pkg = GetPackage(pack);
	if (!pkg) return;
	
	pkg->VisitItems(visitor);
	visitor.SyncItems();
}
		

UInt32 ItemMgr::DecomposeEquip(Item* item, bool sendMail)
{
	if (!item)
		return ITEM_DATA_INVALID;

	//不可分解
	if (item->GetDataEntry()->canDecompose == 0)
		return ITEM_CNA_NOT_DECOMPOSE;

	//是否有强化的装备
	std::vector<EquipStrengthenData*> strengthDatas;
	if ((item->GetStrengthen() > 0)&&(item->GetEquipType() == EQUIP_NORMAL))
	{
		EquipStrengthenDataMgr::Instance()->GetDatas(item->GetStrengthen(), strengthDatas);
		if (0 == strengthDatas.size())
		{
			ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")strenth data size is 0, strenth = " << item->GetStrengthen() << LogStream::eos;
			GetOwner()->SendNotify("strenth data size is 0, strenth = {0}", item->GetStrengthen());
			return ITEM_DATA_INVALID;
		}
	}

	EquipEnhanceCostDataEntry* costData = nullptr;
	//有增幅的装备
	if ((item->GetEnhance() > 0) && (item->GetEquipType() == EQUIP_RED))
	{
		costData = EquipEnhanceCostDataEntryMgr::Instance()->FindEntry(item->GetNeedLevel(), item->GetEnhance(), item->GetQuality());
		if (!costData)
		{
			ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")enhance data size is 0, strenth = " << item->GetEnhance() << LogStream::eos;
			GetOwner()->SendNotify("enhance data size is null, enhance = {0}", item->GetEnhance());
			return ITEM_DATA_INVALID;
		}
	}
	
	//时装
	if (item->GetType() == ITEM_FASHION)
	{
		if (!item->IsPermanent())
		{
			ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ") is not permanent fashion" << LogStream::eos;
			return ITEM_DATA_INVALID;
		}

		UInt32 key = FashionDecomposeDataEntry::MakeKey(item->GetDataEntry()->subType, item->GetDataEntry()->thirdType, item->GetDataEntry()->color);
		FashionDecomposeDataEntry* decomposeDataEntry = FashionDecomposeDataEntryMgr::Instance()->FindEntry(key);
		if (!decomposeDataEntry)
		{
			ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ") fashion decompose config error" << LogStream::eos;
			return ITEM_DATA_INVALID;
		}

		ItemRewardVec result;
		InscriptionDropItemMgr::Instance()->GenDropItems(decomposeDataEntry->groupID, GetOwner()->GetOccu(), result);
		RewardGroup* rewards = GetOwner()->GetRewardGroupNoClear();
		rewards->AddRewards(result);
		return SUCCESS;
	}

	if (SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_EQUIP_SCHEME) && item->IsCanPushScheme() && GetOwner()->GetEqSchemeMgr().IsEquipInScheme(item->GetID()))
	{
		return	ITEM_IILEG_OPER_IN_SCHEME;
	}

	//非装备调用脚本
	if (item->GetType() != ITEM_EQUIP)
	{
		ItemScript* script = ScriptMgr::Instance()->FindItemScript(item->GetDataID());
		if (!script)
		{
			ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")item script is null key = " << item->GetDataID() << LogStream::eos;
			GetOwner()->SendNotify("item script is null key = {0}", item->GetDataID());
			return ITEM_DATA_INVALID;
		}
			

		UInt32 ret = script->OnDecompose(GetOwner(), item);
		return ret;
	}
		
	std::vector<DeComposeDataEntry*> datas;
	DeComposeDataEntryMgr::Instance()->GetDatas(item->GetQuality(), item->GetQualityColor2(), datas);
	 
	if (0 == datas.size())
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "DeComposeDataEntry size is 0!" << LogStream::eos;
		return ITEM_DECOMPOSE_FAIL;
	}
		

	for (int i = 0; i < (int)datas.size(); ++i)
	{
		//可分解
		if (item->GetNeedLevel() >= datas[i]->minLv && item->GetNeedLevel() <= datas[i]->maxLv)
		{
			RewardGroup* rewards = GetOwner()->GetRewardGroupNoClear();
			
			UInt32 dogEyeSz = datas[i]->dogEyeNums.size();
			UInt32 colorMatSz = datas[i]->colorMats.size();
			UInt32 colorLessMatSz = datas[i]->colorLessMats.size();
			UInt32 resistMagicMatSz = datas[i]->resistMagicMatNums.size();
			UInt32 pinkItemMatSz = datas[i]->pinkItemNum.size();
			UInt32 redItemMatSz = datas[i]->redItemNum.size();

			//宇宙之眼
			UInt32 univerEyeId = datas[i]->dogEyeId;
			//有色晶体
			UInt32 colorMatId = datas[i]->colorMatId;
			//无色晶体
			UInt32 colorLessMatId = datas[i]->colorLessMatId;
			//异界材料
			UInt32 resistMagicMatId = datas[i]->resistMagicMatId;

			//特殊材料如果配了就会分解出来
			if (pinkItemMatSz >= 1)
			{
				UInt32 rnd = Avalon::Random::RandBetween(1, datas[i]->pinkItemTotalRate);
				UInt32 tmp = 0;
				for (auto j = datas[i]->pinkItemNum.cbegin(); j != datas[i]->pinkItemNum.cend(); ++j)
				{
					tmp += j->first;
					if (tmp >= rnd)
					{
						rewards->AddReward(datas[i]->pinkItemId, j->second);
						break;
					}
				}
			}

			//带气息的装备的分解奖励
			if (item->GetEquipType() == EQUIP_HAVE_SMELL)
			{
				if (redItemMatSz >= 1)
				{
					UInt32 rnd = Avalon::Random::RandBetween(1, datas[i]->redItemTotalRate);
					UInt32 tmp = 0;
					for (auto j = datas[i]->redItemNum.cbegin(); j != datas[i]->redItemNum.cend(); ++j)
					{
						tmp += j->first;
						if (tmp >= rnd)
						{
							rewards->AddReward(datas[i]->redItemId, j->second);
							break;
						}
					}
				}
			}

			if (item->GetQuality() == ITEM_QUALITY_PINK && item->GetQualityColor2() == ITEM_QUALITY2_ROSERED)
			{
				if (resistMagicMatSz < 1)
				{
					DebugLogStream << PLAYERINFO(GetOwner()) << "resistMagic size is 0!" << "color : " << datas[i]->GetKey() << "color2 : " << datas[i]->GetColor2() << LogStream::eos;
					return ITEM_DECOMPOSE_FAIL;
				}

				Int32 num = 0;
				if (colorMatSz > 1)
				{
					num = Avalon::Random::RandBetween(datas[i]->colorMats[0], datas[i]->colorMats[colorMatSz - 1]);
					rewards->AddReward(colorMatId, num);
					DebugLogStream << PLAYERINFO(GetOwner()) << "decompose color id:" << colorMatId << ",num:" << num << LogStream::eos;
				}
				
				if (colorLessMatSz > 1)
				{
					num = Avalon::Random::RandBetween(datas[i]->colorLessMats[0], datas[i]->colorLessMats[colorLessMatSz - 1]);
					rewards->AddReward(colorLessMatId, num);
					DebugLogStream << PLAYERINFO(GetOwner()) << "decompose colorless id:" << colorLessMatId << ",num:" << num << LogStream::eos;
				}

				if (resistMagicMatSz >= 1)
				{
					UInt32 rnd = Avalon::Random::RandBetween(1, datas[i]->resistMagicTotalRate);
					UInt32 tmp = 0;
					for (auto j = datas[i]->resistMagicMatNums.cbegin(); j != datas[i]->resistMagicMatNums.cend(); ++j)
					{
						tmp += j->first;
						if (tmp >= rnd)
						{
							rewards->AddReward(datas[i]->resistMagicMatId, j->second);
							DebugLogStream << PLAYERINFO(GetOwner()) << "decompose resistMagicMat id:" << resistMagicMatId << ",num:" << j->second << LogStream::eos;
							break;
						}
					}
				}
			}
			else if (item->GetQuality() == ITEM_QUALITY_YELLOW) //橙装备多一个宇宙眼可掉落
			{
				if (dogEyeSz == 0)
				{
					ErrorLogStream << PLAYERINFO(GetOwner()) << "DogEye size is 0!" << LogStream::eos;
					return ITEM_DECOMPOSE_FAIL;
				}
					

				Int32 num = Avalon::Random::RandBetween(datas[i]->dogEyeNums[0], datas[i]->dogEyeNums[dogEyeSz - 1]);
				rewards->AddReward(univerEyeId, num);
				//DebugLogStream << PLAYERINFO(GetOwner()) << "decompose univerEye id:" << univerEyeId << ",num:" << num << LogStream::eos;
			}
			else if (item->GetQuality() == ITEM_QUALITY_WHITE)	//白装只掉无色
			{
				if (colorLessMatSz == 0)
				{
					ErrorLogStream << PLAYERINFO(GetOwner()) << "colorLessMat size is 0!" << LogStream::eos;
					return ITEM_DECOMPOSE_FAIL;
				}
					

				Int32 num = Avalon::Random::RandBetween(datas[i]->colorLessMats[0], datas[i]->colorLessMats[colorLessMatSz - 1]);
				rewards->AddReward(colorLessMatId, num);
				//DebugLogStream << PLAYERINFO(GetOwner()) << "decompose colorless id:" << colorLessMatId << ",num:" << num << LogStream::eos;
			}
			else // 掉无色和有色
			{
				if (colorMatSz == 0)
				{
					ErrorLogStream << PLAYERINFO(GetOwner()) << "colorMat size is 0!" << LogStream::eos;
					return ITEM_DECOMPOSE_FAIL;
				}
					
				if (colorLessMatSz == 0)
				{
					ErrorLogStream << PLAYERINFO(GetOwner()) << "colorLessMat size is 0!" << LogStream::eos;
					return ITEM_DECOMPOSE_FAIL;
				}
					
				Int32 num = Avalon::Random::RandBetween(datas[i]->colorMats[0], datas[i]->colorMats[colorMatSz - 1]);
				rewards->AddReward(colorMatId, num);
				//DebugLogStream << PLAYERINFO(GetOwner()) << "decompose color id:" << colorMatId << ",num:" << num << LogStream::eos;

				num = Avalon::Random::RandBetween(datas[i]->colorLessMats[0], datas[i]->colorLessMats[colorLessMatSz - 1]);
				rewards->AddReward(colorLessMatId, num);
				//DebugLogStream << PLAYERINFO(GetOwner()) << "decompose colorless id:" << colorLessMatId << ",num:" << num << LogStream::eos;
			}
			

			//有强化等级的装备额外返还
			if ((item->GetStrengthen() > 0) && (item->GetEquipType() == EQUIP_NORMAL))
			{
				for (int i = 0; i < (int)strengthDatas.size(); ++i)
				{
					EquipStrengthenData* data = strengthDatas[i];
					if (item->GetNeedLevel() >= data->minLv && item->GetNeedLevel() <= data->maxLv)
					{
						
						StrenthRet ret = data->ret[item->GetQuality()];
						if (Avalon::Random::SelectInThousand(ret.strToTenDropRate))
						{
							//强10券
							UInt32 strToTenID = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_STRTOTEN_ID);
							rewards->AddReward(strToTenID, ret.strToTenNum);

							//DebugLogStream << PLAYERINFO(GetOwner()) << "decompose item give to10Item: id = " << strToTenID << " num = " << ret.strToTenNum << LogStream::eos;
						}

						//绑金ID
						UInt32 bindGoldID = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_BINDGOLD_ID);
						rewards->AddReward(bindGoldID, ret.goldRet);
						//DebugLogStream << "decompose item give gold: num = " << ret.goldRet << LogStream::eos;
						if (ret.colorRet > 0)
						{
							//有色晶体
							UInt32 colorMatId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_STRCOLOR_ID);
							rewards->AddReward(colorMatId, ret.colorRet);
							//DebugLogStream << PLAYERINFO(GetOwner()) << "decompose item give colorMat: id = " << colorMatId << " num = " << ret.colorRet << LogStream::eos;
						}

						if (ret.colorLessRet > 0)
						{
							//无色晶体
							UInt32 colorLessMatId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_STRCOLORLESS_ID);
							rewards->AddReward(colorLessMatId, ret.colorLessRet);
							//DebugLogStream << PLAYERINFO(GetOwner()) << "decompose item give colorlessMat: id = " << colorLessMatId << " num = " << ret.colorLessRet << LogStream::eos;
						}
					}
				}
			}

			//有增幅等级的装备额外返还
			if ((item->GetEnhance() > 0) && (item->GetEquipType() == EQUIP_RED))
			{
				if (costData)
				{
					UInt32 bindGoldID = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_BINDGOLD_ID);
					rewards->AddReward(bindGoldID, costData->returnGold);
					DebugLogStream << "decompose item give gold: num = " << costData->returnGold << LogStream::eos;
				}
			}


			if (!sendMail)
			{
				UInt32 ckCode = GetOwner()->CheckAddRewards(rewards, item, 1);
				if (ckCode != ErrorCode::SUCCESS)
					return ckCode;
			}

			return SUCCESS;
		}
	}

	return ITEM_DECOMPOSE_FAIL;
}

UInt32 ItemMgr::StrengthenEquip(Item* equip, bool useUnbreak, ObjID_t strTicket, UInt32& probability, bool useDisposable)
{
	if (!equip)
		return ITEM_DATA_INVALID;

	if (equip->CheckInvalid())
	{
		return ITEM_DATA_INVALID;
	}

	if (equip->GetEquipType() != EQUIP_NORMAL)
	{
		return ITEM_DATA_INVALID;
	}

	Item* disposable = FindItem(strTicket);
	if (useDisposable)
	{
		if (!disposable)
		{
			return ITEM_DATA_INVALID;
		}

		if (disposable->GetDataEntry() == nullptr)
		{
			return ITEM_DATA_INVALID;
		}

		if (disposable->GetThirdType() != TT_DISPOSABLE_STRENG_ITEM)
		{
			return ITEM_DATA_INVALID;
		}

		//存在配置时才启用限制
		auto data = OnceStrTicketDataMgr::Instance()->FindEntry(disposable->GetDataID());
		if (data != nullptr)
		{
			if (equip->GetStrengthen() < data->lvLimitBegin || equip->GetStrengthen() > data->lvLimitEnd)
			{
				return ITEM_STRENTH_LV_INVALID;
			}
		}
	}

	if (equip->GetType() != ITEM_EQUIP)
		return ITEM_TYPE_INVALID;
	//称号不能强化	added by huchenhui 2016.06.30
	if (equip->GetSubType() == ST_EQUIP_TITLE)
		return ITEM_STRENTH_FAIL_TITLE;

	if (equip->GetQuality() < ITEM_QUALITY_WHITE ||
		equip->GetQuality() >= ITEM_QUALITY_MAX)
	{
		ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")equip quality err, quality = " << equip->GetQuality() << LogStream::eos;
		GetOwner()->SendNotify("equip quality err, quality = {0}", equip->GetQuality());
		return ITEM_DATA_INVALID;
	}

	if (IsAssistEquip(equip->GetSubType()) && !SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_ASSIST_EQUIP))
	{
		InfoLogStream << "assist equip is close!" << LogStream::eos;
		return ITEM_DATA_INVALID;
	}

	EquipStrColorData* colorData = EquipStrColorDataMgr::Instance()->FindEntry(equip->GetNeedLevel());
	if (!colorData)
	{
		return ITEM_DATA_INVALID;
	}

	//检查有否达到强化条件
	if (equip->GetStrengthen() >= ITEM_STRENTH_MAX)
		return ITEM_STRENTH_LV_INVALID;

	if (equip->CheckProhibitOp(IPOE_STRENG))
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "item(guid: " << equip->GetID() << "). Prohibit Strengthen " << LogStream::eos;
		return ITEM_PROHIBIT_OPERATE;
	}

	//使用强化券
	if (strTicket > 0 && !useDisposable)
	{
		Item* ticket = FindItem(strTicket);
		if (!ticket)
		{
			return ITEM_DATA_INVALID;
		}
		
		return UseStrTicket(equip, ticket, probability);
	}

	std::vector<EquipStrengthenData*> datas;
	EquipStrengthenDataMgr::Instance()->GetDatas(equip->GetStrengthen(), datas);
	if (0 == datas.size())
	{
		ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")strenth data size is 0, strenth = " << equip->GetStrengthen() << LogStream::eos;
		GetOwner()->SendNotify("strenth data size is 0, strenth = {0}", equip->GetStrengthen());
		return ITEM_DATA_INVALID;
	}
		
	//有色晶体
	UInt32 colorMatId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_STRCOLOR_ID);
	//无色晶体
	UInt32 colorLessMatId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_STRCOLORLESS_ID);
	//保护劵
	UInt32 protectID = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_STRPROTECT_ID);
	//强10券
	UInt32 strToTenID = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_STRTOTEN_ID);
	//绑金ID
	UInt32 bindGoldID = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_BINDGOLD_ID);

	if (useUnbreak && GetOwner()->GetItemNumWithEqualItem(protectID) <= 0)
	{
		return ITEM_PROTECT_NOT_EXIST;
	}

	float ratioCost = 0.0f;
	if (IsWeapon(equip->GetSubType()))
		ratioCost = (float)SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_STRENGTH_WP_COST_MOD) / 10;
	else if (IsDefend(equip->GetSubType()))
		ratioCost = (float)SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_STRENGTH_DF_COST_MOD) / 10;
	else if (IsOrnaments(equip->GetSubType()))
		ratioCost = (float)SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_STRENGTH_JW_COST_MOD) / 10;
	else if (IsAssistEquip(equip->GetSubType()))
		ratioCost = (float)SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_STRENGTH_ASSIST_COST_MOD) / 10;
	else
	{
		ErrorLogStream << "not ratioConst of equip subtype subtype = " << equip->GetSubType() << LogStream::eos;
		return ITEM_DATA_INVALID;
	}
	

	for (int i = 0; i < (int)datas.size(); ++i)
	{
		EquipStrengthenData* data = datas[i];
		if (equip->GetNeedLevel() >= data->minLv && equip->GetNeedLevel() <= data->maxLv)
		{

			StrenthCost needCost = data->cost[equip->GetQuality()];
			UInt32 colorNum = m_pOwner->GetItemNum(colorMatId);
			UInt32 colorLessNum = m_pOwner->GetItemNum(colorLessMatId);

			UInt32 cost = (UInt32)((float)needCost.goldCost * ratioCost + 0.5);
			//vip特权折扣
			UInt32 vipVal = 0;
			GetOwner()->GetVipValue(VIP_PRIVILEGE_STRENGTHEN_GOLD, vipVal);
			Int32 priceOff = 1000 - vipVal;
			if (priceOff < 0)
			{
				ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")vip value error! key = " << VIP_PRIVILEGE_STRENGTHEN_GOLD << LogStream::eos;
				GetOwner()->SendNotify("vip value error! key = {0}", VIP_PRIVILEGE_STRENGTHEN_GOLD);
				return ITEM_DATA_INVALID;
			}
			float fcost = cost * ((float)priceOff / 1000.0f);
			cost = (UInt32)(fcost + 0.5);

			UInt32 colorCost = (UInt32)((float)needCost.colorCost * ratioCost + 0.5);
			UInt32 colorLessCost = (UInt32)((float)needCost.colorLessCost * ratioCost + 0.5);

#ifdef _DEBUG
			InfoLogStream << "[equip_strengthen] item(" << equip->GetID() << ", " << equip->GetDataID() << ") gold(" << cost << ") color(" << colorCost << ") colorLess(" << colorLessCost << ")." << LogStream::eos;
#endif

			if (!useDisposable)
			{
				if (GetOwner()->GetGoldBoth() < cost)
					return ITEM_NOT_ENOUGH_GOLD;


				if ((colorNum < colorCost) ||
					(colorLessNum < colorLessCost))
					return ITEM_NOT_ENOUGH_MAT;
			}


			EquipStrRateData* sucRateData = EquipStrRateDataMgr::Instance()->GetDatas(equip->GetStrengthen());
			if (!sucRateData)
			{
				ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")sucRateData is null! key = " << equip->GetStrengthen() << LogStream::eos;
				GetOwner()->SendNotify("sucRateData is null! key = {0}", equip->GetStrengthen());
				return ITEM_DATA_INVALID;
			}
				
			std::string reason = GetReason(LogSourceType::SOURCE_TYPE_EQUIP_STRENGTHEN, equip->GetID());
			if (!useDisposable)
			{
				//扣钱
				GetOwner()->RemoveGoldBoth(reason.c_str(), cost);
				//扣材料
				if (colorCost > 0)
					GetOwner()->RemoveItem(reason.c_str(), colorMatId, colorCost);
				if (colorLessCost > 0)
					GetOwner()->RemoveItem(reason.c_str(), colorLessMatId, colorLessCost);
			}
			else
			{
				GetOwner()->ReduceItemNum(reason.c_str(), disposable);
			}

			GetOwner()->SendFunctionUdpLog(FUNCTION_EQUIP_STRENGTHEN);

			std::ostringstream osfix2;
			osfix2 << STRENGTH_FAIL_FIX2 << (UInt32)equip->GetStrengthen();
			const CounterConfig* counterConfigFix2 = CounterCfg::Instance()->GetCycleConfig(osfix2.str());
			if (!counterConfigFix2)
			{
				CounterCfg::Instance()->RegCounter(osfix2.str().c_str(), 0);
			}

			float rate = GetStrenthRate(equip, sucRateData);
			equip->SetStrNum(equip->GetStrNum() + 1);

			std::ostringstream os;
			os << STRENGTH_FAIL_NUM << (UInt32)equip->GetStrengthen();
			const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(os.str());
			if (!counterConfig)
			{
				CounterCfg::Instance()->RegCounter(os.str().c_str(), 0);
			}

			bool testSuc = false;
#ifdef _DEBUG
			testSuc = GetOwner()->m_testEquipStrengthFlag == 1 ? true : false;
#endif // _DEBUG

			// 运营活动相关
			{
				float oldRate = rate;
				GetOwner()->GetActiveTaskMgr().EnhanceEquipStrengthenRate(equip->GetStrengthen(), 1, rate);

#ifdef _DEBUG
				GetOwner()->SendNotify("StrengthenEquip old rate = {0}, new rate = {1}", oldRate * 1000, rate * 1000);
#endif
			}

			probability = rate * 1000;
			if (Avalon::Random::SelectInThousand((UInt32)(rate * 1000)) ||
				testSuc)
			{
				equip->SetStrFailedTimes(0);

				GetOwner()->SetCounter(os.str().c_str(), 0);

				GetOwner()->SetCounter(osfix2.str().c_str(), 0);

				//成功
				equip->SetStrengthen(equip->GetStrengthen() + 1);
				
				//计算评分
				this->CalculateValuedScore(equip);
				equip->SyncProperty(GetOwner());

				GetOwner()->CalcRoleValueScore();
				return SUCCESS;
			}
			else
			{
				equip->SetStrFailedTimes(equip->GetStrFailedTimes() + 1);
				GetOwner()->GetCounterMgr().IncCounter(os.str().c_str(), 1);

				bool isNotLoseLv = GetOwner()->GetActiveTaskMgr().IsNotLoseStrenLvAfterEquipStrengthenFaile(equip->GetStrengthen(), 1);

//#ifdef _DEBUG
//				{
//					UInt8 oldStrenLevel = equip->GetStrengthen();
//					UInt8 newStrenLevel = oldStrenLevel + 1;
//					GetOwner()->SendNotify("StrengthenEquip failed, old level = {0}, new level = {1}, not lose = {2}", oldStrenLevel, newStrenLevel, isNotLoseLv ? 1 : 0);
//				}
//#endif

				//失败
				switch (sucRateData->failPunishment)
				{
					case SPT_NONE:		//无惩罚
					{
						return ITEM_STRENTH_FAIL;
					}
					case SPT_MINUSONE:		// 强化等级减一
					{
						if (isNotLoseLv)
						{
							return ITEM_STRENTH_FAIL;
						}

						if (equip->CanReduceStrengthenOrEnhanceLv())
						{
							equip->SetStrengthen(equip->GetStrengthen() - 1);
						}
						
						equip->SyncProperty(GetOwner());
						return ITEM_STRENTH_FAIL_MINUS;
					}
					case SPT_ZERO:		// 强化等级归0
					{
						if (isNotLoseLv)
						{
							return ITEM_STRENTH_FAIL;
						}

						if (equip->CanReduceStrengthenOrEnhanceLv())
						{
							equip->SetStrengthen(0);
						}

						equip->SyncProperty(GetOwner());
						return ITEM_STRENTH_FAIL_ZERO;
					}
					case SPT_BROKEN:		// 装备破碎
					{
						// 使用保护劵
						//if (useUnbreak && GetItemNum(protectID, ITEM_MATERIAL) > 0)
						if (useUnbreak && GetOwner()->GetItemNumWithEqualItem(protectID) > 0) //考虑等价物
						{	
							//失败给返还
							RewardGroup* grp = GetOwner()->GetRewardGroup();
							StrenthRet ret = data->ret[equip->GetQuality()];
							if (Avalon::Random::SelectInThousand(ret.strToTenDropRate))
							{
								grp->AddReward(strToTenID, ret.strToTenNum);

								DebugLogStream << PLAYERINFO(GetOwner()) << "strenth fail give to10Item: id = " << strToTenID << " num = " << ret.strToTenNum << LogStream::eos;
							}

							grp->AddReward(bindGoldID, ret.goldRet);
							DebugLogStream << "strenth fail give gold: num = " << ret.goldRet << LogStream::eos;
							if (ret.colorRet > 0)
							{
								grp->AddReward(colorMatId, ret.colorRet);
								DebugLogStream << PLAYERINFO(GetOwner()) << "strenth fail give colorMat: id = " << colorMatId << " num = " << ret.colorRet << LogStream::eos;
							}

							if (ret.colorLessRet > 0)
							{
								grp->AddReward(colorLessMatId, ret.colorLessRet);
								DebugLogStream << PLAYERINFO(GetOwner()) << "strenth fail give colorlessMat: id = " << colorLessMatId << " num = " << ret.colorLessRet << LogStream::eos;
							}
							
							std::string reason = GetReason(LogSourceType::SOURCE_TYPE_EQUIP_STRENGTHEN, equip->GetID());
							GetOwner()->AddRewards(reason.c_str(), GetOwner()->GetRewardGroupNoClear());

							GetOwner()->RemoveItemWithEqualItem(reason.c_str(), protectID, 1);
							if (equip->CanReduceStrengthenOrEnhanceLv())
							{
								equip->SetStrengthen(0);
							}
							equip->SyncProperty(GetOwner());
							return ITEM_STRENTH_FAIL_ZERO;
						}
						else
						{
							StrenthRet strRet = data->ret[equip->GetQuality()];
							RewardGroup* grp = GetOwner()->GetRewardGroup();
							UInt32 rnd = Avalon::Random::RandBetween(1, strRet.totalRate);
							UInt32 tmp = 0;
							for (auto i = strRet.ItemNumVec.cbegin(); i != strRet.ItemNumVec.cend(); ++i)
							{
								tmp += i->first;
								if (tmp >= rnd)
								{
									grp->AddReward(SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_STRONG_TEMP), i->second);
									std::string reason = GetReason(LogSourceType::SOURCE_TYPE_EQUIP_ENHANCE, equip->GetID());
									GetOwner()->AddRewards(reason.c_str(), GetOwner()->GetRewardGroupNoClear());
									break;
								}
							}
							std::vector<ObjID_t> decEquip;
							ItemRewardVec getItems;
							decEquip.push_back(equip->GetID());
							ItemRewardVec ReturnItems = grp->GetRewards();

							UInt32 ret = GetOwner()->DecomposeEquip(GetReason(SOURCE_TYPE_EQUIP_STRENGTHEN).c_str(), decEquip, getItems, true);
							grp->AddRewards(ReturnItems);
							if (SUCCESS != ret)
							{
								InfoLogStream << "player(" << this->GetOwner()->GetID() << "), strengthen equip err equip(" << equip->GetID() << "), equip typeid("
									<< equip->GetDataID() << ") decompose fail ret = " << ret << LogStream::eos;
								if (SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_EQUIP_STRENG_DESC_FAIL_REMOVE))
								{
									GetOwner()->RemoveItemDirect(GetReason(SOURCE_TYPE_EQUIP_STRENGTHEN).c_str(), equip);
								}
							}
							return ITEM_STRENTH_FAIL_BROKE;
						}	
					}
					case SPT_MINUSMORE:		// 强化等级减二到三
					{
						if (isNotLoseLv)
						{
							return ITEM_STRENTH_FAIL;
						}

						if (equip->CanReduceStrengthenOrEnhanceLv())
						{						
							//随机惩罚数
							Int32 punish = Avalon::Random::RandBetween(2, 3);
							equip->SetStrengthen(equip->GetStrengthen() - punish);
						}

						equip->SyncProperty(GetOwner());

						return ITEM_STRENTH_FAIL_MINUS;
					}
					default:
					{
						ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")sucRateData->failPunishment is err! value = " << sucRateData->failPunishment << LogStream::eos;
						GetOwner()->SendNotify("sucRateData->failPunishment is err! value = {0}", sucRateData->failPunishment);
						return ITEM_DATA_INVALID;
					}
				}
			}
			break;
		}
	}
	return ITEM_DATA_INVALID;
}

float ItemMgr::GetStrenthRate(Item* equip, EquipStrRateData* sucRateData)
{
	if (!equip)
	{
		ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")equip is null!" << LogStream::eos;
		return 0.0f;
	}

	if (!sucRateData)
	{
		ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")sucRateData is null! key = " << equip->GetStrengthen() << LogStream::eos;
		GetOwner()->SendNotify("sucRateData is null! key = {0}", equip->GetStrengthen());
		return 0.0f;
	}

	EquipStrColorData* lvColorRatio = EquipStrColorDataMgr::Instance()->FindEntry(equip->GetNeedLevel());
	if (!lvColorRatio)
	{
		ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")EquipStrColorData is null! key = " << equip->GetNeedLevel() << LogStream::eos;
		GetOwner()->SendNotify("sucRateData is null! key = {0}", equip->GetStrengthen());
		return 0.0f;
	}

	if (equip->GetStrengthen() > ITEM_STRENTH_MAX)
	{
		ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")equip strenth=" << equip->GetStrengthen() << " is err!" << LogStream::eos;
		return 0.0f;
	}

	if (equip->GetQuality() < ITEM_QUALITY_WHITE ||
		equip->GetQuality() >= ITEM_QUALITY_MAX)
	{
		ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")equip quality=" << equip->GetQuality() << " is err!" << LogStream::eos;
		return 0.0f;
	}

	float baseRate = (float)sucRateData->sucRate / 1000;
	UInt32 fix1Count = equip->GetStrFailedTimes();

	std::ostringstream os;
	os << STRENGTH_FAIL_NUM << (UInt32)equip->GetStrengthen();
	const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(os.str());
	if (!counterConfig)
	{
		CounterCfg::Instance()->RegCounter(os.str().c_str(), 0);
	}

	UInt32 fix2Count = GetOwner()->GetCounter(os.str().c_str());

	float fix1 = fix1Count * ((float)sucRateData->fix1 / 1000);
	if (fix1 > (float)sucRateData->fix1Max / 1000)
		fix1 = (float)sucRateData->fix1Max / 1000;

	std::ostringstream osfix2;
	osfix2 << STRENGTH_FAIL_FIX2 << (UInt32)equip->GetStrengthen();
	const CounterConfig* counterConfigFix2 = CounterCfg::Instance()->GetCycleConfig(osfix2.str());
	if (!counterConfigFix2)
	{
		CounterCfg::Instance()->RegCounter(osfix2.str().c_str(), 0);
	}

	//更新fix2系数
	UInt32 fix2 = GetOwner()->GetCount(osfix2.str().c_str());
	UInt32 curFix2 = 0;

	UInt32 colorRatio = lvColorRatio->colorRatio[equip->GetQuality()];

	UInt32 totFix2 = 0;

	//上一次是失败
	if (fix2Count > 0)
	{
		curFix2 = (colorRatio * sucRateData->fix2) / 1000;
		totFix2 = fix2 + curFix2;

		if (totFix2 > sucRateData->fix2Max)
			totFix2 = sucRateData->fix2Max;

		GetOwner()->SetCounter(osfix2.str().c_str(), totFix2);
	}
	
	float fix3 = 0.0f;
	if (equip->GetStrNum() < sucRateData->fix3Condition[equip->GetQuality() - 1])
		fix3 = (float)sucRateData->fix3 / 1000;

	float rate = baseRate + fix1 + (float)totFix2 / 1000 - fix3;
	
	if (rate < 0)
		return 0.0f;
	else
		return rate;
}


UInt32 ItemMgr::EnhanceEquip(Item* equip, bool useUnbreak, ObjID_t strTicket, UInt32& probability, bool useDisposable)
{
	if (!equip)
		return ITEM_DATA_INVALID;

	if (equip->CheckInvalid())
	{
		return ITEM_DATA_INVALID;
	}

	if (equip->GetEquipType() != EQUIP_RED)
	{
		return ITEM_DATA_INVALID;
	}

	Item* disposable = FindItem(strTicket);
	if (useDisposable)
	{
		if (!disposable)
		{
			return ITEM_DATA_INVALID;
		}

		if (disposable->GetDataEntry() == nullptr)
		{
			return ITEM_DATA_INVALID;
		}

		if (disposable->GetThirdType() != TT_DISPOSABLE_INCREASE_ITEM)
		{
			return ITEM_DATA_INVALID;
		}

		//存在配置时才启用限制
		auto data = OnceStrTicketDataMgr::Instance()->FindEntry(disposable->GetDataID());
		if (data == nullptr)
		{
			if (equip->GetEnhance() < data->lvLimitBegin || equip->GetEnhance() > data->lvLimitEnd)
			{
				return ITEM_STRENTH_LV_INVALID;
			}
		}
	}

	if (equip->GetType() != ITEM_EQUIP)
		return ITEM_TYPE_INVALID;
	//称号不能强化	added by huchenhui 2016.06.30
	if (equip->GetSubType() == ST_EQUIP_TITLE)
		return ITEM_STRENTH_FAIL_TITLE;

	if (equip->GetQuality() < ITEM_QUALITY_WHITE ||
		equip->GetQuality() >= ITEM_QUALITY_MAX)
	{
		ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")equip quality err, quality = " << equip->GetQuality() << LogStream::eos;
		GetOwner()->SendNotify("equip quality err, quality = {0}", equip->GetQuality());
		return ITEM_DATA_INVALID;
	}

	if (equip->CheckProhibitOp(IPOE_ENHANCE))
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "item(guid: " << equip->GetID() << "). Prohibit enhance" << LogStream::eos;
		return ITEM_PROHIBIT_OPERATE;
	}

	EquipEnhanceCostDataEntry* costData = EquipEnhanceCostDataEntryMgr::Instance()->FindEntry(equip->GetNeedLevel(),equip->GetEnhance(),equip->GetQuality());
	if (!costData)
	{
		return ITEM_DATA_INVALID;
	}

	EquipEnhanceAttributeDataEntry* AttributeData = EquipEnhanceAttributeDataEntryMgr::Instance()->FindEntry(equip->GetNeedLevel(), equip->GetEnhance(), equip->GetQuality());
	if (!AttributeData)
	{
		return ITEM_DATA_INVALID;
	}

	//检查有否达到增幅条件
	if (equip->GetEnhance() >= ITEM_STRENTH_MAX)
		return ITEM_STRENTH_LV_INVALID;

	//使用增幅券
	if (strTicket > 0 && !useDisposable)
	{
		Item* ticket = FindItem(strTicket);
		if (!ticket)
		{
			return ITEM_DATA_INVALID;
		}

		return UseEnhanceTicket(equip, ticket, probability);
	}

	//保护劵
	UInt32 protectID = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_ENHANCE_PROTECT_ID);
	//强10券
	//UInt32 strToTenID = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_STRTOTEN_ID);
	//绑金ID
	UInt32 bindGoldID = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_BINDGOLD_ID);

	if (useUnbreak && GetOwner()->GetItemNumWithEqualItem(protectID) <= 0)
	{
		return ITEM_PROTECT_NOT_EXIST;
	}


	if (!useDisposable)
	{
		if (GetOwner()->GetGoldBoth() < costData->needGold)
		{
			return ITEM_NOT_ENOUGH_GOLD;
		}
		if (GetOwner()->GetItemNum(costData->itemID) < costData->itemNum)
		{
			return ITEM_NOT_ENOUGH_MAT;
		}
	}


	EquipStrRateData* sucRateData = EquipStrRateDataMgr::Instance()->GetEnhanceDatas(equip->GetEnhance());
	if (!sucRateData)
	{
		ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")sucRateData is null! key = " << equip->GetEnhance() << LogStream::eos;
		GetOwner()->SendNotify("sucRateData is null! key = {0}", equip->GetEnhance());
		return ITEM_DATA_INVALID;
	}

	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_EQUIP_ENHANCE, equip->GetID());
	if (!useDisposable)
	{
		
		//扣钱
		GetOwner()->RemoveGoldBoth(reason.c_str(), costData->needGold);
		//扣材料
		if (costData->itemNum > 0)
		{
			GetOwner()->RemoveItem(reason.c_str(), costData->itemID, costData->itemNum);
		}
	}
	else
	{
		GetOwner()->ReduceItemNum(reason.c_str(), disposable);
	}


	GetOwner()->SendFunctionUdpLog(FUNCTION_EQUIP_ENHANCE);

	std::ostringstream osfix2;
	osfix2 << ENHANCE_FAIL_FIX2 << (UInt32)equip->GetEnhance();
	const CounterConfig* counterConfigFix2 = CounterCfg::Instance()->GetCycleConfig(osfix2.str());
	if (!counterConfigFix2)
	{
		CounterCfg::Instance()->RegCounter(osfix2.str().c_str(), 0);
	}

	float rate = GetEnhanceRate(equip, sucRateData);
	equip->SetEnhanceNum(equip->GetEnhanceNum() + 1);

	std::ostringstream os;
	os << ENHANCE_FAIL_NUM << (UInt32)equip->GetEnhance();
	const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(os.str());
	if (!counterConfig)
	{
		CounterCfg::Instance()->RegCounter(os.str().c_str(), 0);
	}

	bool testSuc = false;
#ifdef _DEBUG
	testSuc = GetOwner()->m_testEquipStrengthFlag == 1 ? true : false;
	GetOwner()->SendNotify("EnhanceEquip SelectInThousand, rate = {0}", (UInt32)(rate * 1000));
#endif // _DEBUG


	probability = rate * 1000;
	if (Avalon::Random::SelectInThousand((UInt32)(rate * 1000)) || testSuc)
	{
		equip->SetEnhanceFailedTimes(0);

		GetOwner()->SetCounter(os.str().c_str(), 0);

		GetOwner()->SetCounter(osfix2.str().c_str(), 0);

		//成功
		equip->SetEnhance(equip->GetEnhance() + 1);
		equip->SyncProperty(GetOwner());

		return SUCCESS;
	}
	else
	{
		equip->SetEnhanceFailedTimes(equip->GetEnhanceFailedTimes() + 1);
		GetOwner()->GetCounterMgr().IncCounter(os.str().c_str(), 1);

		bool isNotLoseLv = false;//GetOwner()->GetActiveTaskMgr().IsNotLoseStrenLvAfterEquipStrengthenFaile(equip->GetStrengthen(), 1);

#ifdef _DEBUG
		{
			UInt8 oldStrenLevel = equip->GetStrengthen();
			UInt8 newStrenLevel = oldStrenLevel + 1;
			GetOwner()->SendNotify("EnhanceEquip failed, old level = {0}, new level = {1}, not lose = {2}", oldStrenLevel, newStrenLevel, isNotLoseLv ? 1 : 0);
		}
#endif

		//失败
		switch (sucRateData->failPunishment)
		{
			case SPT_NONE:		//无惩罚
			{
				return ITEM_STRENTH_FAIL;
			}
			case SPT_MINUSONE:		// 增幅等级减一
			{
				if (isNotLoseLv)
				{
					return ITEM_STRENTH_FAIL;
				}

				if (equip->CanReduceStrengthenOrEnhanceLv())
				{
					equip->SetEnhance(equip->GetEnhance() - 1);
				}
				
				equip->SyncProperty(GetOwner());
				return ITEM_STRENTH_FAIL_MINUS;
			}
			case SPT_ZERO:		// 增幅等级归0
			{
				if (isNotLoseLv)
				{
					return ITEM_STRENTH_FAIL;
				}

				if (equip->CanReduceStrengthenOrEnhanceLv())
				{
					equip->SetEnhance(0);
				}
				equip->SyncProperty(GetOwner());
				return ITEM_STRENTH_FAIL_ZERO;
			}
			case SPT_BROKEN:		// 装备破碎
			{
				RewardGroup* grp = GetOwner()->GetRewardGroup();
				std::string reason = GetReason(LogSourceType::SOURCE_TYPE_EQUIP_ENHANCE, equip->GetID());
				// 使用保护劵
				if (useUnbreak && GetOwner()->GetItemNumWithEqualItem(protectID) > 0) //考虑等价物
				{
					GetOwner()->RemoveItemWithEqualItem(reason.c_str(), protectID, 1);
					if (equip->CanReduceStrengthenOrEnhanceLv())
					{
						equip->SetEnhance(0);
					}
					equip->SyncProperty(GetOwner());
					grp->AddReward(bindGoldID, costData->returnGold);
					GetOwner()->AddRewards(reason.c_str(), GetOwner()->GetRewardGroupNoClear());
					return ITEM_STRENTH_FAIL_ZERO;
				}
				else
				{
					std::vector<ObjID_t> decEquip;
					ItemRewardVec getItems;
					decEquip.push_back(equip->GetID());
					UInt32 rnd = Avalon::Random::RandBetween(1, costData->returnTotalRate);
					UInt32 tmp = 0;
					for (auto i = costData->retrunItem.cbegin(); i != costData->retrunItem.cend(); ++i)
					{
						tmp += i->m_Rate;
						if (tmp >= rnd)
						{

							grp->AddReward(costData->returnItemID, i->m_Num);
							break;
						}
					}
					rnd = Avalon::Random::RandBetween(1, costData->returnTotalRate2);
					tmp = 0;
					for (auto i = costData->retrunItem2.cbegin(); i != costData->retrunItem2.cend(); ++i)
					{
						tmp += i->m_Rate;
						if (tmp >= rnd)
						{

							grp->AddReward(costData->returnItemID2, i->m_Num);
							break;
						}
					}
					GetOwner()->AddRewards(reason.c_str(), GetOwner()->GetRewardGroupNoClear());
					ItemRewardVec ReturnItems = grp->GetRewards();
					if (GetOwner()->DecomposeEquip(reason.c_str(), decEquip, getItems, true) != ErrorCode::SUCCESS)
					{
						GetOwner()->ReduceItemNum(reason.c_str(), equip, 1);
					}
					grp->AddRewards(ReturnItems);
					return ITEM_STRENTH_FAIL_BROKE;
				}
			}
			case SPT_MINUSMORE:		// 增幅等级减二到三
			{
				if (isNotLoseLv)
				{
					return ITEM_STRENTH_FAIL;
				}

				if (equip->CanReduceStrengthenOrEnhanceLv())
				{
					//随机惩罚数
					Int32 punish = Avalon::Random::RandBetween(2, 3);
					equip->SetEnhance(equip->GetEnhance() - punish);
				}

				equip->SyncProperty(GetOwner());

				return ITEM_STRENTH_FAIL_MINUS;
			}
			default:
			{
				ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")sucRateData->failPunishment is err! value = " << sucRateData->failPunishment << LogStream::eos;
				GetOwner()->SendNotify("sucRateData->failPunishment is err! value = {0}", sucRateData->failPunishment);
				return ITEM_DATA_INVALID;
			}
		}
	}
	return ITEM_DATA_INVALID;
}

float ItemMgr::GetEnhanceRate(Item* equip, EquipStrRateData* sucRateData)
{
	if (!equip)
	{
		ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")equip is null!" << LogStream::eos;
		return 0.0f;
	}

	if (!sucRateData)
	{
		ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")sucRateData is null! key = " << equip->GetEnhance() << LogStream::eos;
		GetOwner()->SendNotify("sucRateData is null! key = {0}", equip->GetEnhance());
		return 0.0f;
	}

	EquipStrColorData* lvColorRatio = EquipStrColorDataMgr::Instance()->FindEntry(equip->GetNeedLevel());
	if (!lvColorRatio)
	{
		ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")EquipStrColorData is null! key = " << equip->GetNeedLevel() << LogStream::eos;
		GetOwner()->SendNotify("sucRateData is null! key = {0}", equip->GetEnhance());
		return 0.0f;
	}

	if (equip->GetEnhance() > ITEM_STRENTH_MAX)
	{
		ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")equip strenth=" << equip->GetEnhance() << " is err!" << LogStream::eos;
		return 0.0f;
	}

	if (equip->GetQuality() < ITEM_QUALITY_WHITE ||
		equip->GetQuality() >= ITEM_QUALITY_MAX)
	{
		ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")equip quality=" << equip->GetQuality() << " is err!" << LogStream::eos;
		return 0.0f;
	}

	float baseRate = (float)sucRateData->sucRate / 1000;
	UInt32 fix1Count = equip->GetEnhanceFailedTimes();

	std::ostringstream os;
	os << ENHANCE_FAIL_NUM << (UInt32)equip->GetEnhance();
	const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(os.str());
	if (!counterConfig)
	{
		CounterCfg::Instance()->RegCounter(os.str().c_str(), 0);
	}

	UInt32 fix2Count = GetOwner()->GetCounter(os.str().c_str());

	float fix1 = fix1Count * ((float)sucRateData->fix1 / 1000);
	if (fix1 > (float)sucRateData->fix1Max / 1000)
		fix1 = (float)sucRateData->fix1Max / 1000;

	std::ostringstream osfix2;
	osfix2 << ENHANCE_FAIL_FIX2 << (UInt32)equip->GetEnhance();
	const CounterConfig* counterConfigFix2 = CounterCfg::Instance()->GetCycleConfig(osfix2.str());
	if (!counterConfigFix2)
	{
		CounterCfg::Instance()->RegCounter(osfix2.str().c_str(), 0);
	}

	//更新fix2系数
	UInt32 fix2 = GetOwner()->GetCount(osfix2.str().c_str());
	UInt32 curFix2 = 0;

	UInt32 colorRatio = lvColorRatio->colorRatio[equip->GetQuality()];

	UInt32 totFix2 = 0;

	//上一次是失败
	if (fix2Count > 0)
	{
		curFix2 = (colorRatio * sucRateData->fix2) / 1000;
		totFix2 = fix2 + curFix2;

		if (totFix2 > sucRateData->fix2Max)
			totFix2 = sucRateData->fix2Max;

		GetOwner()->SetCounter(osfix2.str().c_str(), totFix2);
	}

	float fix3 = 0.0f;
	if (equip->GetEnhanceNum() < sucRateData->fix3Condition[equip->GetQuality() - 1])
		fix3 = (float)sucRateData->fix3 / 1000;

	float rate = baseRate + fix1 + (float)totFix2 / 1000 - fix3;

	DebugLogStream << PLAYERINFO(GetOwner()) << " itemid=" << equip->GetDataID() << " curStr=" << equip->GetEnhance() << " base=" << baseRate << " fix1=" << fix1 << " fix2=" << (float)totFix2 / 1000 <<
		" fix3=" << fix3 << " strNum=" << equip->GetEnhanceNum() << " conditionNum=" << sucRateData->fix3Condition[equip->GetQuality() - 1] <<
		" rate=" << rate << LogStream::eos;

	if (rate < 0)
		return 0.0f;
	else
		return rate;

}

Item* ItemMgr::BuildWeapon(UInt32 id, bool extraMaterial1, bool extraMaterial2)
{
	auto buildData = WeaponBuildDataEntryMgr::Instance()->FindEntry(id);
	if (buildData == nullptr)
	{
		return nullptr;
	}

	//判断职业是否符合
	auto weaponDataEntry = ItemDataEntryMgr::Instance()->FindEntry(buildData->yellowItemID);
	if (weaponDataEntry == nullptr)
	{
		return nullptr;
	}
	if (!weaponDataEntry->IsMeetOccuRequirement(GetOwner()->GetOccupation()))
	{
		return nullptr;
	}

	auto costData = WeaponBuildCostDataEntryMgr::Instance()->FindEntry(buildData->level);
	if (costData == nullptr)
	{
		return nullptr;
	}

	//检查材料够不够
	for (auto material : costData->mustItem)
	{
		if (GetOwner()->GetItemNum(material.id) < material.num)
		{
			return nullptr;
		}	
	}

	if (extraMaterial1)
	{
		if (GetOwner()->GetItemNum(costData->unnecessaryItem_1.id) < costData->unnecessaryItem_1.num)
		{
			return nullptr;
		}
	}
	if (extraMaterial1)
	{
		if (GetOwner()->GetItemNum(costData->unnecessaryItem_2.id) < costData->unnecessaryItem_2.num)
		{
			return nullptr;
		}
	}

	//材料够了开始扣除材料
	std::string reason = GetReason(LogSourceType::SOURCE_WEAPON_BUILD);
	for (auto material : costData->mustItem)
	{
		if (!GetOwner()->RemoveItem(reason.c_str(), material.id, material.num))
		{
			ErrorLogStream << "weapon build remove item faild," << PLAYERINFO(GetOwner()) << " ,id is " << material.id << " num is " << material.num << LogStream::eos;
			return nullptr;
		}
	}
	if (extraMaterial1)
	{
		if (!GetOwner()->RemoveItem(reason.c_str(), costData->unnecessaryItem_1.id, costData->unnecessaryItem_1.num))
		{
			ErrorLogStream << "weapon build remove item faild," << PLAYERINFO(GetOwner()) << " ,id is " << costData->unnecessaryItem_1.id << " num is " << costData->unnecessaryItem_1.num << LogStream::eos;
			return nullptr;
		}
	}
	if (extraMaterial2)
	{
		if (!GetOwner()->RemoveItem(reason.c_str(), costData->unnecessaryItem_2.id, costData->unnecessaryItem_2.num))
		{
			ErrorLogStream << "weapon build remove item faild," << PLAYERINFO(GetOwner()) << " ,id is " << costData->unnecessaryItem_2.id << " num is " << costData->unnecessaryItem_2.num << LogStream::eos;
			return nullptr;
		}
	}

	//随机档次
	auto pair = WeaponBuildQualityRandomDataEntryMgr::Instance()->GetRandomGradeAndGroupID(buildData->level,extraMaterial1);
	UInt32 grade = pair.first;
	UInt32 groupId = pair.second;

	//获得算出来的属性
	WeaponBuildRandomPoolContext context;
	context.type = buildData->naturePropertyType;
	context.weaponType = buildData->itemThirdType;
	context.weaponLevel = buildData->level;
	std::vector<UInt32> totalPropertyVec;
	WeaponBuildRandomPoolDataEntryMgr::Instance()->GenProperty(groupId, totalPropertyVec, context);


	auto itemPair = WeaponBuildDataEntryMgr::Instance()->FindItemID(grade, buildData->itemThirdType, buildData->level, buildData->naturePropertyType);
	UInt32 yellowItemId = itemPair.first;
	UInt32 silverItemId = itemPair.second;

	UInt32 itemID = 0;
	//先检查是不是神圣品质，因为神圣品质道具id不同
	for (auto propertyID : totalPropertyVec)
	{
		auto property = WeaponBuildPropertyPoolDataEntryMgr::Instance()->FindEntry(propertyID);
		if (property == nullptr)
		{
			continue;
		}
		if (property->special != 0 && property->special == WBS_PINZHI)
		{
			itemID = silverItemId;
			break;
		}
	}

	//生成武器本身
	auto weapon = ItemMaker::Instance()->MakeItem(GetOwner(), itemID, 1, 0, 0, 0, EQUIP_NORMAL, ENHANCE_INVALID, true);
	if (weapon == nullptr)
	{
		return nullptr;
	}

	weapon->SetValue(IKVKT_WBP_BUILD_WEAPON, 1);
	//添加生成的属性
	for (auto propertyID : totalPropertyVec)
	{
		auto property = WeaponBuildPropertyPoolDataEntryMgr::Instance()->FindEntry(propertyID);
		if (property == nullptr)
		{
			continue;
		}

		weapon->SetValue(IKVKT_WBP_ADDITION_SCORE, weapon->GetValue(IKVKT_WBP_ADDITION_SCORE) + property->num);

		Int32 minNum = 0;
		Int32 maxNum = 0;

		//属性池里的属性要分成几类：1.基本属性受材料二影响 2.buff信息表的特殊属性 3.装备特性类型
		if (property->mainPropertyType == MPT_BUFF)
		{
			if (property->buffinfo_PVE != 0)
			{
				for (UInt32 i = IKVKT_WBP_BUFF_BEGIN; i <= IKVKT_WBP_BUFF_END; ++i)
				{
					if (weapon->GetValue(i) == 0)
					{
						weapon->SetValue(i, property->buffinfo_PVE);
						break;
					}
				}
			}
			if (property->buffinfo_PVP != 0)
			{
				for (UInt32 i = IKVKT_WBP_BUFF_BEGIN; i <= IKVKT_WBP_BUFF_END; ++i)
				{
					if (weapon->GetValue(i) == 0)
					{
						weapon->SetValue(i, property->buffinfo_PVP);
						break;
					}
				}
			}
			if (property->mechanism_PVE != 0)
			{
				for (UInt32 i = IKVKT_WBP_BUFF_BEGIN; i <= IKVKT_WBP_BUFF_END; ++i)
				{
					if (weapon->GetValue(i) == 0)
					{
						weapon->SetValue(i, property->mechanism_PVE);
						break;
					}
				}
			}
			if (property->mechanism_PVP != 0)
			{
				for (UInt32 i = IKVKT_WBP_BUFF_BEGIN; i <= IKVKT_WBP_BUFF_END; ++i)
				{
					if (weapon->GetValue(i) == 0)
					{
						weapon->SetValue(i, property->mechanism_PVP);
						break;
					}
				}
			}
		}
		else if(property->mainPropertyType == MPT_BASE)
		{
			UInt32 num = 0;
			if (property->propertyNum.size() == 1)
			{
				num = property->propertyNum[0];
			}
			else if (property->propertyNum.size() == 2)
			{
				if (extraMaterial2)
				{
					num = Avalon::Random::RandBetween(property->propertyNum[0], property->propertyNum[1]);
				}
			}
			else
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << "can't find property num " << property->id << LogStream::eos;
				continue;
			}
			weapon->SetValue(property->propType, num);
		}
		else if (property->mainPropertyType == MPT_SPECIAL)
		{
			if (property->special != 0)
			{
				weapon->SetValue(IKVKT_WBP_SPECIAL, property->special);
				weapon->SetValue(IKVKT_WBP_SPECIAL_DETAIL, property->special_detail);
			}
		}
		else
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "can't find this type " << property->mainPropertyType << LogStream::eos;
			continue;
		}
	}

	if (weapon->GetValue(IKVKT_WBP_SPECIAL) == WBS_DOUBLE)
	{
		weapon->GenSecondPreciousBeadHoles();
	}

	weapon->SetAttribute(true);

	return weapon;
}

UInt32 ItemMgr::UseStrTicket(Item* equip, Item* ticket, UInt32& probability)
{
	if (!equip || !ticket || !ticket->GetDataEntry())
	{
		return ITEM_DATA_INVALID;
	}

	//如果强化券有等级限制，则需要判断装备的等级是否在区间内
	if (ticket->GetDataEntry()->strTicketLvLimitMin > 0)
	{
		if (equip->GetNeedLevel() < ticket->GetDataEntry()->strTicketLvLimitMin || equip->GetNeedLevel() > ticket->GetDataEntry()->strTicketLvLimitMax)
		{
			return ITEM_STR_TICKET_LEVEL_LIMIT;
		}
	}


	EquipStrRateData* rateData = nullptr;
	UInt32 rate = 0;
	UInt32 strengthenLv = 0;
	if (ticket->GetSubType() == ST_STRENTH_LUCK)
	{
		auto data = OnceStrTicketDataMgr::Instance()->FindEntry(ticket->GetDataID());
		if (!data)
		{
			return ITEM_DATA_INVALID;
		}

		if (equip->GetStrengthen() < data->lvLimitBegin || equip->GetStrengthen() > data->lvLimitEnd)
		{
			return ITEM_STR_TICKET_LEVEL_LIMIT;
		}

		rateData = EquipStrRateDataMgr::Instance()->FindEntry(equip->GetStrengthen() + 1);
		rate = data->rate;
		strengthenLv = equip->GetStrengthen() + 1;
	}
	else
	{
		StrTicketData* data = StrTicketDataMgr::Instance()->FindEntry(ticket->GetDataEntry()->strenTicketDataIndex);
		if (!data)
		{
			return ITEM_DATA_INVALID;
		}

		if (data->effectLv == 0)
		{
			return ITEM_DATA_INVALID;
		}

		if (data->effectLv > ITEM_STRENTH_MAX)
		{
			return ITEM_DATA_INVALID;
		}

		if (data->effectLv <= equip->GetStrengthen())
		{
			return ITEM_STRTICKET_LV_ERR;
		}

		rateData = EquipStrRateDataMgr::Instance()->FindEntry(data->effectLv - 1);
		rate = data->rate;
		strengthenLv = data->effectLv;
	}

	if (!rateData || strengthenLv == 0 || rate == 0)
	{
		return ITEM_DATA_INVALID;
	}

	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_EQUIP_STRENGTHEN, equip->GetID());
	if (SUCCESS != GetOwner()->ReduceItemNum(reason.c_str(), ticket))
	{
		return ITEM_STRTICKET_REDUCE_ERR;
	}

	//增加强化次数
	Int32 realAddNum = (Int32)((float)rateData->tickAddNum * (float)rate / 1000);
	equip->SetStrNum(equip->GetStrNum() + realAddNum);
	if (Avalon::Random::SelectInThousand(rate))
	{
		equip->SetStrengthen(strengthenLv);
		equip->SyncProperty(GetOwner());
		return SUCCESS;
	}
	else
	{
		equip->SyncProperty(GetOwner());
		return ITEM_SPECIAL_STRENTH_FAIL;
	}
}

UInt32 ItemMgr::UseEnhanceTicket(Item* equip, Item* ticket, UInt32& probability)
{
	if (!equip || !ticket || !ticket->GetDataEntry())
	{
		return ITEM_DATA_INVALID;
	}

	//如果增幅券有等级限制，则需要判断装备的等级是否在区间内
	if (ticket->GetDataEntry()->strTicketLvLimitMin > 0)
	{
		if (equip->GetNeedLevel() < ticket->GetDataEntry()->strTicketLvLimitMin || equip->GetNeedLevel() > ticket->GetDataEntry()->strTicketLvLimitMax)
		{
			return ITEM_STR_TICKET_LEVEL_LIMIT;
		}
	}

	StrTicketData* data = EnhanceTicketDataMgr::Instance()->FindEntry(ticket->GetDataEntry()->strenTicketDataIndex);
	if (!data)
	{
		return ITEM_DATA_INVALID;
	}

	if (data->effectLv == 0)
	{
		return ITEM_DATA_INVALID;
	}

	EquipStrRateData* rateData = EquipStrRateDataMgr::Instance()->GetEnhanceDatas(data->effectLv - 1);
	if (!rateData)
	{
		return ITEM_DATA_INVALID;
	}

	if (data->effectLv > ITEM_STRENTH_MAX)
	{
		return ITEM_DATA_INVALID;
	}

	if (data->effectLv <= equip->GetEnhance())
	{
		return ITEM_STRTICKET_LV_ERR;
	}

	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_EQUIP_ENHANCE, equip->GetID());

	if (SUCCESS != GetOwner()->ReduceItemNum(reason.c_str(), ticket))
	{
		return ITEM_STRTICKET_REDUCE_ERR;
	}

	//增加增幅次数
	Int32 realAddNum = (Int32)((float)rateData->tickAddNum * (float)data->rate / 1000);
	equip->SetEnhanceNum(equip->GetEnhanceNum() + realAddNum);
	probability = data->rate;
	if (Avalon::Random::SelectInThousand(data->rate))
	{
		equip->SetEnhance(data->effectLv);
		equip->SyncProperty(GetOwner());
		return SUCCESS;
	}
	else
	{
		equip->SyncProperty(GetOwner());
		return ITEM_SPECIAL_STRENTH_FAIL;
	}
}

bool ItemMgr::RandEquipQLA(Item* item)
{
	EquipQLRandData* randData = EquipQLRandDataMgr::Instance()->GetData(item->GetQuality(), item->GetQlNum());
	if (!randData)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " Get EquipQLRandData Error! color:" << item->GetQuality() << " qlnum:" << item->GetQlNum() << LogStream::eos;
		return false;
	}

	UInt8 qlRate = RandQL(randData);
	item->SetQualityLv(qlRate);
	//DebugLogStream << PLAYERINFO(GetOwner()) << " SetQualityLv:" << qlRate << LogStream::eos;
	item->ClearAttribute();
	item->SetAllQualityProp(qlRate);
	
	item->SetQlNum(item->GetQlNum() + 1);
	item->SyncProperty(GetOwner());

	return true;
}

bool ItemMgr::RandEquipQLB(Item* item)
{
	EquipQLRandData* randData = EquipQLRandDataMgr::Instance()->GetData(item->GetQuality(), item->GetQlNum());
	if (!randData)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " Get EquipQLRandData Error! color:" << item->GetQuality() << " qlnum:" << item->GetQlNum() << LogStream::eos;
		return false;
	}

	EquipAttrDataEntry* attrData = item->GetAttrDataEntry();
	if (!attrData)
	{
		ErrorLogStream << " EquipAttrDataEntry is null! attrId:" << item->GetDataEntry()->attrId << LogStream::eos;
		return false;
	}

	EquipQLValueData* qlValueData = EquipQLValueDataMgr::Instance()->FindEntry(item->GetPartType());
	if (!qlValueData)
	{
		ErrorLogStream << " EquipQLValueData is null! partType:" << item->GetPartType() << LogStream::eos;
		return false;
	}

	UInt8 ql[EQAT_MAX];
	memset(ql, 0, sizeof(UInt8)*EQAT_MAX);
	if (0 != attrData->atk)
	{
		ql[EQAT_PHYATK] = RandQL(randData);
		item->SetSingleQualityProp(EQAT_PHYATK, ql[EQAT_PHYATK], attrData, qlValueData);
	}
	if (0 != attrData->magicAtk)
	{
		ql[EQAT_MAGATK] = RandQL(randData);
		item->SetSingleQualityProp(EQAT_MAGATK, ql[EQAT_MAGATK], attrData, qlValueData);
	}
	if (0 != attrData->def)
	{
		ql[EQAT_PHYDEF] = RandQL(randData);
		item->SetSingleQualityProp(EQAT_PHYDEF, ql[EQAT_PHYDEF], attrData, qlValueData);
	}
	if (0 != attrData->magicDef)
	{
		ql[EQAT_MAGDEF] = RandQL(randData);
		item->SetSingleQualityProp(EQAT_MAGDEF, ql[EQAT_MAGDEF], attrData, qlValueData);
	}
	if (0 != attrData->strenth)
	{
		ql[EQAT_STRENGTH] = RandQL(randData);
		item->SetSingleQualityProp(EQAT_STRENGTH, ql[EQAT_STRENGTH], attrData, qlValueData);
	}
	if (0 != attrData->stamina)
	{
		ql[EQAT_STAMINA] = RandQL(randData);
		item->SetSingleQualityProp(EQAT_STAMINA, ql[EQAT_STAMINA], attrData, qlValueData);
	}
	if (0 != attrData->spirit)
	{
		ql[EQAT_SPIRIT] = RandQL(randData);
		item->SetSingleQualityProp(EQAT_SPIRIT, ql[EQAT_SPIRIT], attrData, qlValueData);
	}
	if (0 != attrData->intellect)
	{
		ql[EQAT_INTELLECT] = RandQL(randData);
		item->SetSingleQualityProp(EQAT_INTELLECT, ql[EQAT_INTELLECT], attrData, qlValueData);
	}
	if (0 != attrData->independAtk)
	{
		ql[EQAT_INDEPENDATK] = RandQL(randData);
		item->SetSingleQualityProp(EQAT_INDEPENDATK, ql[EQAT_INDEPENDATK], attrData, qlValueData);
	}

	UInt8 qlRate = 0;
	UInt32 totQlRate = 0;
	UInt8 propNum = 0;
	for (int i = 0; i < EQAT_MAX; ++i)
	{
		if (ql[i] > 0)
		{
			totQlRate += ql[i];
			propNum++;
		}
	}

	if (propNum == 0)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "PropNum is 0!" << LogStream::eos;
		return false;
	}

	qlRate = totQlRate / propNum;
	DebugLogStream << PLAYERINFO(GetOwner()) << " SetQualityLv:" << qlRate << LogStream::eos;
	item->SetQualityLv(qlRate);

	item->SetQlNum(item->GetQlNum() + 1);
	item->SyncProperty(GetOwner());

	return true;
}

UInt8 ItemMgr::RandQL(EquipQLRandData* randData)
{
	Int32 qlRate = 0;
	//随机惊喜范围
	if (Avalon::Random::SelectInTenThousand(randData->surpriseRate))
	{
		qlRate = Avalon::Random::RandBetween((Int32)randData->surpriseRangeMin, (Int32)randData->surpriseRangeMax);
	}
	else
	{
		qlRate = Avalon::Random::RandBetween((Int32)randData->randRangeMin, (Int32)randData->randRangeMax);
	}

	return (UInt8)qlRate;
}

void ItemMgr::AdaptQLData(Item* item)
{
	if (item->GetQualityLv() > 0 && item->GetQlNum() == 0)
	{
		UInt32 newSetQlNum = EquipQLRandDataMgr::Instance()->GetAdaptNum(item->GetQuality(), item->GetQualityLv());
		item->SetQlNum(newSetQlNum);
	}
}

bool ItemMgr::AdaptFashionAttr(Item* item)
{
	if (!item)	return false;
	if (item->GetFashionAttrId() == 0)
	{
		return item->SetFashionDefaultAttr();
	}

	return true;
}

void ItemMgr::SetAttrData(Item* item)
{
	if (!item || !item->GetDataEntry())
	{
		return;
	}

	if (item->GetType() == ITEM_EQUIP)
	{
		EquipAttrDataEntry* attrData = EquipAttrDataMgr::Instance()->FindEntry(item->GetDataID());
		if (!attrData)
		{
			ErrorLogStream << "item attrData is null! id:" << item->GetDataID() << LogStream::eos;
			return;
		}

		item->SetAttrDataEntry(attrData);
	}
	else if (item->GetType() == ITEM_FASHION)
	{
		EquipAttrDataEntry* attrData = EquipAttrDataMgr::Instance()->FindEntry(item->GetFashionAttrId());
		if (!attrData)
		{
			ErrorLogStream << "item attrData is null! id:" << item->GetDataID() << LogStream::eos;
			return;
		}

		item->SetAttrDataEntry(attrData);
	}
}

UInt32 ItemMgr::CheckItemDayUseNum(Item* item, bool& isDayUseLimite, std::string& counterName)
{
	UInt32 dayUseNum = 0;
	UInt32	weekUseNum = 0;
	isDayUseLimite = false;
	if (item->GetDataEntry()->useLimiteType == IULT_DAYLIMITE)
	{
		isDayUseLimite = true;
		counterName = ITEM_DAYUSE_PRE + Avalon::StringUtil::ConvertToString((UInt32)item->GetDataID());
		dayUseNum = GetOwner()->GetCounter(counterName.c_str());

		if (dayUseNum >= item->GetDataEntry()->useLimiteValue)
		{
			return ITEM_DAYUSENUM;
		}
	}
	else if (item->GetDataEntry()->useLimiteType == IULT_VIPLIMITE)
	{
		isDayUseLimite = true;
		counterName = ITEM_DAYUSE_PRE + Avalon::StringUtil::ConvertToString((UInt32)item->GetDataID());
		dayUseNum = GetOwner()->GetCounter(counterName.c_str());

		UInt32 vipVal = GetOwner()->GetVipValueById(item->GetDataEntry()->useLimiteValue);

		if (dayUseNum >= vipVal)
		{
			return ITEM_DAYUSENUM;
		}
	}
	else if (item->GetDataEntry()->useLimiteType == IULT_TEAMCOPY)
	{
		isDayUseLimite = true;
		counterName = COUNTER_TEAM_COPY_ITEM_USE_CNT + Avalon::StringUtil::ConvertToString((UInt32)item->GetDataID());
		UInt32 weekUseNum = GetOwner()->GetCounter(counterName.c_str());

		if (weekUseNum >= item->GetDataEntry()->useLimiteValue)
		{
			return ErrorCode::ITEM_USELIMIT;
		}
	}
	else if (item->GetDataEntry()->useLimiteType == IULT_HONORLV)
	{
		if (GetOwner()->GetHonorLevel() > item->GetDataEntry()->useLimiteValue)
		{
			return ErrorCode::ITEM_HONOR_LVL_HIGH;
		}
	}
	else if (item->GetDataEntry()->useLimiteType == IULT_WEEK6LIMITE)
	{
		isDayUseLimite = true;
		counterName = ITEM_WEEK6USE_PRE + Avalon::StringUtil::ConvertToString((UInt32)item->GetDataID());
		weekUseNum = GetOwner()->GetCounter(counterName.c_str());

		if (weekUseNum >= item->GetDataEntry()->useLimiteValue)
		{
			return ITEM_WEEK6USENUM;
		}
	}
	else
	{
		counterName = "";
		isDayUseLimite = false;
		return SUCCESS;
	}

	return SUCCESS;
}

void ItemMgr::OnUseItemSuc(ItemDataEntry* data, bool isDayUseLimite, std::string& counterName)
{
	//附加buff
	if (data->onUseBuff != 0)
	{
		GetOwner()->GetBuffMgr().AddBuff(data->onUseBuff);
	}

	if (data->cdGroup != 0)
	{
		UInt64 endtime = (CURRENT_TIME.Sec() + data->cdTime) * 1000;
		GetOwner()->SetItemUseTime(data->cdGroup, endtime, data->cdTime);
		GetOwner()->SyncProperty();
	}

	//更新日使用次数
	if (isDayUseLimite)
	{
		GetOwner()->IncCounter(counterName.c_str(), 1);
	}

	GetOwner()->OnUseItem(data->id);

	// 使用药水，发送消息到World
	if (data->thirdType == TT_DUNGEON_POTION)
	{
		if (GetOwner()->GetTeamCopyMgr().IsTeamCopy())
		{
			GetOwner()->SyncEventToTeamCopy(SET_DUNGEON_USE_POTION, data->id);
		}
		else
		{
			GetOwner()->SyncEventToWorld(SET_DUNGEON_USE_POTION, data->id);
		}
	}
}

UInt32 ItemMgr::CheckGiftCanUse(Item* item)
{
	if (item->GetDataEntry()->occu.all())
	{
		return SUCCESS;
	}

	//没转职检查礼包是否需要转职
	bool bFitOccu = true;
	if (!GetOwner()->IsChangeOccu(GetOwner()->GetOccu()))
	{
		// 如果有Occupation::OCCU_MAX，说明只有转职后能使用
		if (item->GetDataEntry()->occuFilter.find(-1) != item->GetDataEntry()->occuFilter.end())
		{
			//专职后才能使用的礼包
			bFitOccu = false;
		}
	}
	
	if (bFitOccu)
	{
		return SUCCESS;
	}
	else
	{
		return ITEM_NOT_CHANGE_OCCU;
	}
}

UInt32 ItemMgr::AutoFindPotion(UInt32 slot1, UInt32 slot2, UInt32 slot3, UInt32 fitType)
{
	class AutoPotionVisitor : public ItemVisitor
	{
	public:
		AutoPotionVisitor(UInt32 slot1, UInt32 slot2, UInt32 slot3, UInt32 lv, UInt32 fitType) :
			m_slot1ItemId(slot1), m_slot2ItemId(slot2), 
			m_slot3ItemId(slot3), m_FitItemId(0), m_MyLevel(lv), m_LastItemLevel(0), m_fitType(fitType){}

		bool operator()(Item* item)
		{
			if (item->GetDataEntry()->thirdType == TT_DUNGEON_POTION &&
				item->GetDataEntry()->id != m_slot1ItemId &&
				item->GetDataEntry()->id != m_slot2ItemId &&
				item->GetDataEntry()->id != m_slot3ItemId &&
				m_MyLevel >= item->GetDataEntry()->needLevel &&
				item->GetDataEntry()->needLevel > m_LastItemLevel &&
				item->GetDataEntry()->subType == m_fitType)
			{
				m_FitItemId = item->GetDataEntry()->id;
				m_LastItemLevel = item->GetDataEntry()->needLevel;
			}

			return true;
		}
	public: 
		UInt32 GetItemId()
		{
			return m_FitItemId;
		}

	private:
		UInt32 m_slot1ItemId;
		UInt32 m_slot2ItemId;
		UInt32 m_slot3ItemId;
		UInt32 m_FitItemId;
		UInt32 m_MyLevel;
		UInt32 m_LastItemLevel;
		UInt32 m_fitType;
	};

	Package* pkg = GetPackage(PACK_EXPENDABLE);
	if (!pkg) return 0;
	AutoPotionVisitor visitor(slot1, slot2, slot3, GetOwner()->GetLevel(), fitType);
	pkg->VisitItems(visitor);

	return visitor.GetItemId();
}

std::vector<UInt32> ItemMgr::GetFitPotionType(UInt32 type)
{
	if (type == POTION_CONFIG_TYPE_MAIN)
	{
		return { ST_HPMP_POTION , ST_HP_POTION , ST_ATTRIBUTE_DRUG };
	}
	else if (type == POTION_CONFIG_TYPE_HP)
	{
		return { ST_HPMP_POTION , ST_HP_POTION , ST_ATTRIBUTE_DRUG };
	}
	else if (type == POTION_CONFIG_TYPE_MP)
	{
		return { ST_HPMP_POTION , ST_MP_POTION, ST_ATTRIBUTE_DRUG };
	}

	return {};
}

void ItemMgr::CheckTimeItem(bool bDayChange, bool bOnline)
{
	class OnHeartbeatVisitor : public ItemVisitor
	{
	public:
		OnHeartbeatVisitor(Player *pPlayer, UInt32 time, bool dayChange, bool online) : m_pPlayer(pPlayer), m_deleteIntervalTime(time), m_dayChange(dayChange), m_online(online){}
		~OnHeartbeatVisitor() {}

		bool operator()(Item *item)
		{
			UInt32 oldState = item->GetTimeState();
			item->CheckTimeOut(m_deleteIntervalTime);
			UInt32 newState = item->GetTimeState();

			if (m_dayChange)
			{
				item->SetNotifyTimeState(true);
				return ProcItem(item);
			}

			if (m_online)
			{
				return ProcItem(item);
			}

			if (oldState == newState)
			{
				return true;
			}
			
			return ProcItem(item);
			
		}

	public:
		bool ProcItem(Item* item)
		{
			
			//时间到了删除道具
			if (item->GetTimeState() == ICT_NEED_DELETE)
			{
				if (item->GetPos().pack == PACK_WEAR && item->GetSubType() == ST_EQUIP_TITLE)
				{
					std::vector<Item*> syncItems;
					m_pPlayer->GetItemMgr().ChangeEquip(item, syncItems);
				}

				m_pPlayer->ReduceItemNum(GetReason(SOURCE_TYPE_ITEM_TIMEOUT).c_str(), item, item->GetNum());
				return true;
			}

			//道具到期可续费状态把装备，时装，称号都卸下
			if (item->GetTimeState() == ICT_EXPIRE)
			{
				if (item->GetPos().pack == PACK_WEAR)
				{
					std::vector<Item*> syncItems;
					m_pPlayer->GetItemMgr().ChangeEquip(item, syncItems);
				}

				if (item->GetPos().pack == PACK_FASHION_WEAR)
				{
					m_pPlayer->GetItemMgr().ChangeFashion(item);
				}

				//通知客户端
				if (item->GetNotifyTimeState())
				{
					m_items.push_back(item);
				}

				return true;
			}

			//即将到期的
			if (item->GetTimeState() == ICT_ABOUT_TO_EXPIRE)
			{
				//通知客户端
				if (item->GetNotifyTimeState())
				{
					m_items.push_back(item);
				}

				return true;
			}

			return true;
		}
		std::vector<Item*>&	GetItems()	{ return m_items; }
	private:
		Player	*m_pPlayer;
		UInt32	 m_deleteIntervalTime;
		std::vector<Item*>	m_items;
		bool	m_dayChange;
		bool	m_online;
	};
	
	OnHeartbeatVisitor	visitor(GetOwner(), m_timeItemDeleteIntervalTime, bDayChange, bOnline);
	VisitItems(PACK_EQUIP, visitor);
	VisitItems(PACK_MATERIAL, visitor);
	VisitItems(PACK_EXPENDABLE, visitor);
	VisitItems(PACK_TASK, visitor);
	VisitItems(PACK_FASHION, visitor);
	VisitItems(PACK_WEAR, visitor);
	VisitItems(PACK_FASHION_WEAR, visitor);
	VisitItems(PACK_STORAGE, visitor);
	VisitItems(PACK_ENERGY_STONE, visitor);
	VisitItems(PACK_TITLE, visitor);
	VisitItems(PACK_ROLE_STORAGE, visitor);
	VisitItems(PACK_INSCRIPTION, visitor);

	GetOwner()->SendNotifyTimeItemState(visitor.GetItems());

}

void ItemMgr::SendItemLoginUdpLog()
{
	class ItemLogVisitor : public ItemVisitor
	{
	public:
		ItemLogVisitor(Player* player, UInt32 itemId = 0)
			:m_Player(player), m_ItemId(itemId){}
		~ItemLogVisitor(){}

		bool operator()(Item* item)
		{
			PackType packageType = (PackType)item->GetPos().pack;

			if (item->GetDataID() != m_ItemId && 
				(packageType != PackType::PACK_WEAR && packageType != PackType::PACK_FASHION_WEAR))
			{
				return true;
			}

			UInt32 preciousBeadId1 = item->GetPreciousBeadHoleProperty().GetPreciousBeadId(1);
			ItemQuality preciousBeadQuality = ItemQuality::ITEM_QUALITY_INVLID;
			if (preciousBeadId1 != 0)
			{
				ItemDataEntry* preciousBeadData = ItemDataEntryMgr::Instance()->FindEntry(preciousBeadId1);
				if (preciousBeadData != NULL)
				{
					preciousBeadQuality = (ItemQuality)preciousBeadData->color;
				}
			}
			UInt32 cardId = item->GetMagicCardId();
			ItemQuality cardQuality = ItemQuality::ITEM_QUALITY_INVLID;
			if (cardId != 0)
			{
				ItemDataEntry* cardData = ItemDataEntryMgr::Instance()->FindEntry(cardId);
				if (cardData != NULL)
				{
					cardQuality = (ItemQuality)cardData->color;
				}
			}

			UInt32 inscriptionId1 = item->GetInscriptionHoleProperty().GetInscriptionId(1);
			ItemQuality inscriptionQuality1 = ItemQuality::ITEM_QUALITY_INVLID;
			if (inscriptionId1 != 0)
			{
				ItemDataEntry* tempDataEntry = ItemDataEntryMgr::Instance()->FindEntry(inscriptionId1);
				if (tempDataEntry)
				{
					inscriptionQuality1 = (ItemQuality)tempDataEntry->color;
				}
			}
			UInt32 inscriptionId2 = item->GetInscriptionHoleProperty().GetInscriptionId(2);
			ItemQuality inscriptionQuality2 = ItemQuality::ITEM_QUALITY_INVLID;
			if (inscriptionId2 != 0)
			{
				ItemDataEntry* tempDataEntry = ItemDataEntryMgr::Instance()->FindEntry(inscriptionId2);
				if (tempDataEntry)
				{
					inscriptionQuality2 = (ItemQuality)tempDataEntry->color;
				}
			}

			UInt32 schemeIds = 0;
			if (item->GetType() == ITEM_EQUIP || item->GetType() == ITEM_TITLE)
			{
				schemeIds = m_Player->GetEqSchemeMgr().GetSchemeOfEquip(item->GetID());
			}

			if (packageType == PackType::PACK_FASHION_WEAR || packageType == PackType::PACK_WEAR)
			{
			m_Player->SendEquipLoginUdpLog(item->GetID(), item->GetDataID(), (ItemType)item->GetType(), (ItemSubType)item->GetSubType(),
					(ItemQuality)item->GetQuality(), item->GetStrengthen(), item->GetQualityLv(), item->GetName().c_str(),
					preciousBeadId1, preciousBeadQuality, cardId, cardQuality, item->GetPos().gridIndex,item->GetEquipType(), item->GetMagicCardId(), item->GetMagicCardLv(),
					inscriptionId1, inscriptionQuality1, inscriptionId2, inscriptionQuality2, item->GetNeedLevel(), schemeIds);
			}
			else
			{
				m_Player->SendItemLoginUdpLog(item->GetID(), item->GetDataID(), item->GetNum(), (ItemType)item->GetType(), (ItemSubType)item->GetSubType(),
					(ItemQuality)item->GetQuality(), item->GetStrengthen(), item->GetQualityLv(), item->GetName().c_str(),
					preciousBeadId1, preciousBeadQuality, cardId, cardQuality, item->GetPos().gridIndex, item->GetEquipType(), schemeIds);
			}
			return true;
		}

	private:
		Player* m_Player;
		UInt32 m_ItemId;
	};

	Package* wearEquipPackage = GetPackage(PackType::PACK_WEAR);
	if (wearEquipPackage != NULL)
	{
		ItemLogVisitor visitor(GetOwner());
	wearEquipPackage->VisitItems(visitor);
	}

	Package* wearFashionPackage = GetPackage(PackType::PACK_FASHION_WEAR);
	if (wearFashionPackage != NULL)
	{
		ItemLogVisitor visitor(GetOwner());
		wearFashionPackage->VisitItems(visitor);
	}

	std::vector<ItemDataEntry*> itemDatas;
	ItemDataEntryMgr::Instance()->GetSendUdpItems(itemDatas);
	for (UInt32 i = 0; i < itemDatas.size(); ++i)
	{
		ItemType itemType = (ItemType)itemDatas[i]->type;
		ItemDataEntry* itemData = itemDatas[i];
		if (itemType == ItemType::ITEM_MATERIAL || itemType == ItemType::ITEM_EXPENDABLE)
		{
			UInt32 itemNum = GetItemNum(itemDatas[i]->id, (UInt8)itemType);
			if (itemNum > 0)
			{
				GetOwner()->SendItemLoginUdpLog(0, itemData->id, itemNum, (ItemType)itemData->type, (ItemSubType)itemData->subType,
					(ItemQuality)itemData->color, 0, 0, itemData->name.c_str(), 0, ITEM_QUALITY_INVLID, 0, ITEM_QUALITY_INVLID, 0, 0, 0);
			}
		}
		else
		{
			PackType packageType = GetPackType(itemType);
			Package* package = GetPackage(packageType);
			if (package == NULL)
			{
				continue;
			}
			ItemLogVisitor visitor(GetOwner(), itemData->id);
			package->VisitItems(visitor);
		}
	}

}

class GetPackageItemsVisitor : public ItemVisitor
{
public:
	GetPackageItemsVisitor(){}
	~GetPackageItemsVisitor(){}

	bool operator()(Item* item)
	{
		m_Items.push_back(item);
		return true;
	}

	void AddToPackage(Player* owner, Package* pack)
	{
		Avalon::Packet* packet = Avalon::Packet::Create();
		packet->SetID(CLProtocol::SCENE_SYNC_ITEM_PROPERTY);

		Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());

		try
		{
			for(std::vector<Item*>::iterator iter = m_Items.begin();
				iter != m_Items.end(); ++iter)
			{
				Item* item = *iter;
				pack->AddItem(item);
			
				if(item->IsGridNumDirty(SST_SELF))
				{
					stream & item->GetID();
					item->Encode(stream, SST_SELF, true);

					//同步到数据库
					CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand(owner->GetSrcZoneId(), "t_item" ,item->GetID() ,false);
					item->GetDBData(cmd, true);
					CLRecordClient::Instance()->SendCommand(cmd);
				}
			}
			stream & ObjID_t(0);
		}
		catch(Avalon::NetException&)
		{
			Avalon::Packet::Destroy(packet);
			return;
		}

		packet->SetSize(stream.GetPos());
		if(packet->GetSize() > sizeof(ObjID_t))
		{
			owner->SendPacket(packet);
		}
		Avalon::Packet::Destroy(packet);
	}

private:
	//道具列表
	std::vector<Item*>	m_Items;
};

UInt16 ItemMgr::MoveItem(Item* item, UInt16 num, UInt8 destPack)
{
	if(num > item->GetNum()) num = item->GetNum();
	if(num == 0) return 0;
	Package* srcpack = GetPackage(item->GetPos().pack);
	Package* destpack = GetPackage(destPack);
	if(srcpack == NULL || destpack == NULL) return 0;
	if(srcpack == destpack) return 0;
	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_ITEM_MOVE, srcpack->GetType(), destPack);
	UInt16 oldNum = item->GetNum();

	AddItemVisitor visitor(item->GetDataID(), num, GetOwner(), reason.c_str(), item->GetAuctionCoolTimeStamp(), item->GetStrengthen(), item->GetAuctionTransNum());
	destpack->VisitItems(visitor);
	UInt16 leftnum = visitor.GetNum();

	UInt16 movenum = num - leftnum;
	item->SetNum(item->GetNum() - movenum);
	GetOwner()->OnRemoveItem(reason.c_str(), item, movenum, oldNum);
	

	if(item->GetNum() == 0)	//全部添加
	{
		RemoveItem(item);
		Item::DestroyItem(item);
		return num;
	}

	if(leftnum > 0 && destpack->GetCapacity() >= 1)
	{
		if(leftnum == item->GetNum())	//剩余整体移动
		{
			srcpack->RemoveItem(item);
			destpack->AddItem(item);
			movenum = num;
		}
		else
		{
			Item* newitem = ItemMaker::Instance()->MakeItem(GetOwner(), item->GetDataID(), leftnum, item->GetBind(), item->GetStrengthen(), item->GetAuctionCoolTimeStamp(), 
				item->GetEquipType(), item->GetEnhanceType(), false, item->GetAuctionTransNum());
			if(newitem == NULL || !AddItem(newitem, ItemPos(destPack, INVALID_ITEMGRID)))
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << " make item error! id = " << item->GetDataID() << LogStream::eos;
				item->SyncProperty(GetOwner(), true);
				Item::DestroyItem(newitem);
				return movenum;
			}
			
			item->SetNum(item->GetNum() - leftnum);
			GetOwner()->OnRemoveItem(reason.c_str(), item, leftnum, oldNum);
			movenum = num;
		}
	}

	item->SyncProperty(GetOwner(), true);
	return movenum;
}

UInt16 ItemMgr::MoveItemToRoleStorage(Item* item, UInt16 num, UInt32 gridIndex)
{
	if (num > item->GetNum()) num = item->GetNum();
	if (num == 0) return 0;
	Package* srcpack = GetPackage(item->GetPos().pack);
	Package* destpack = GetPackage(PACK_ROLE_STORAGE);
	if (srcpack == NULL || destpack == NULL) return 0;
	if (srcpack == destpack) return 0;
	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_ITEM_MOVE, srcpack->GetType(), PACK_ROLE_STORAGE);
	UInt16 oldNum = item->GetNum();

	//先合并
	class TmpItemVisitor : public ItemVisitor
	{
	public:
		TmpItemVisitor(UInt32 id, UInt16 num, Player* player, const char* reason)
			:m_ItemID(id), m_Num(num), m_pPlayer(player), m_sReason(reason), m_auctionCoolTimeStamp(0), m_auctionTransTimes(0), m_strengthen(0) {}
		TmpItemVisitor(UInt32 id, UInt16 num, Player* player, const char* reason, UInt32 auctionCoolTimeStamp, UInt32 auctionTransTimes, UInt8 strengthen, UInt32 gridIndex = INVALID_ITEMGRID)
			:m_ItemID(id), m_Num(num), m_pPlayer(player), m_sReason(reason), m_auctionCoolTimeStamp(auctionCoolTimeStamp), m_auctionTransTimes(auctionTransTimes),  m_strengthen(strengthen)
		{
			m_page = GetRoleStoragePageByGrid(gridIndex);
			if (m_page <= m_pPlayer->GetCounter(COUNTER_ROLE_STORAGE_OPEN_NUM))
			{
				m_pageGridIndex = m_page * MAX_GRID_NUM_PER_ROLE_STORAGE_PAGE;
			}
			else
			{
				m_pageGridIndex = INVALID_ITEMGRID;
			}
		}
		~TmpItemVisitor() {}

		bool operator()(Item* item)
		{
			if (m_pageGridIndex == INVALID_ITEMGRID)
			{
				return true;
			}

			//把不是同一页的排除掉
			if (m_page != GetRoleStoragePageByGrid(item->GetPos().gridIndex))
			{
				return true;
			}

			m_usedGridIndexMap[item->GetPos().gridIndex] = true;

			if (item->GetDataID() == m_ItemID)
			{
				UInt16 mergenum = item->Merge(m_sReason, m_pPlayer, m_Num, m_auctionCoolTimeStamp, m_strengthen, m_auctionTransTimes);

				item->SetParam1(1);	// 设置是新道具标识
				if (mergenum != 0)
				{
					item->SyncProperty(m_pPlayer);
					m_pPlayer->OnMakeItem(m_sReason, item, mergenum, false);
					mergedItems.push_back(item->GetID());
				}

				m_Num = m_Num - mergenum;
				if (m_Num == 0)
				{
					//通知更新道具
					UpdateItemParam param;
					param._updateItem = (void*)item;
					CLEventMgr::Instance()->SendPlayerEvent(m_pPlayer->GetID(), CLEventMgr::ET_NOTIFY_UPDATEITEM, &param);
					return false;
				}
			}

			return true;
		}

		UInt16 GetNum() const { return m_Num; }

		std::vector<ObjID_t>& GetMergedItems() { return mergedItems; }

		UInt32 FindFreeGridIndex()
		{
			bool found = false;
			if (m_pageGridIndex != INVALID_ITEMGRID)
			{
				for (UInt32 i = m_pageGridIndex; i < (m_pageGridIndex + MAX_GRID_NUM_PER_ROLE_STORAGE_PAGE); ++i)
				{
					if (m_usedGridIndexMap.find(i) == m_usedGridIndexMap.end())
					{
						found = true;
						m_pageGridIndex = i;
						break;
					}
				}
			}

			if (!found)
			{
				m_pageGridIndex = INVALID_ITEMGRID;
			}

			return m_pageGridIndex;
		}

	private:
		//道具id
		UInt32	m_ItemID;
		//数量
		UInt16	m_Num;
		//玩家
		Player*	m_pPlayer;
		//理由
		const char* m_sReason;
		//拍卖行交易冻结时间
		UInt32 m_auctionCoolTimeStamp;
		//拍卖行交易次数
		UInt32 m_auctionTransTimes;
		//强化等级
		UInt8  m_strengthen;
		//合并到的道具的guid集合
		std::vector<ObjID_t> mergedItems;
		//角色仓库的页
		UInt32 m_page;
		//角色仓库的页起始位置
		UInt32 m_pageGridIndex;
		//已经使用的位置
		std::map<UInt32, bool> m_usedGridIndexMap;
	};

	TmpItemVisitor visitor(item->GetDataID(), num, GetOwner(), reason.c_str(), item->GetAuctionCoolTimeStamp(), item->GetAuctionTransNum(),item->GetStrengthen(), gridIndex);
	destpack->VisitItems(visitor);
	UInt16 leftnum = visitor.GetNum();
	UInt32 freeGridIndex = visitor.FindFreeGridIndex();

	UInt16 movenum = num - leftnum;
	item->SetNum(item->GetNum() - movenum);
	GetOwner()->OnRemoveItem(reason.c_str(), item, movenum, oldNum);


	if (item->GetNum() == 0)	//全部添加
	{
		RemoveItem(item);
		Item::DestroyItem(item);
		return num;
	}

	if (leftnum > 0 && freeGridIndex < ((GetOwner()->GetCounter(COUNTER_ROLE_STORAGE_OPEN_NUM) + 1) * MAX_GRID_NUM_PER_ROLE_STORAGE_PAGE))
	{
		if (leftnum == item->GetNum())	//剩余整体移动
		{
			srcpack->RemoveItem(item);
			destpack->AddItem(item, freeGridIndex);
			movenum = num;
		}
		else
		{
			Item* newitem = ItemMaker::Instance()->MakeItem(GetOwner(), item->GetDataID(), leftnum, item->GetBind(), item->GetStrengthen(), 
				item->GetAuctionCoolTimeStamp(), item->GetEquipType(), item->GetEnhanceType(), false, item->GetAuctionTransNum());
			if (newitem == NULL || !AddItem(newitem, ItemPos(PACK_ROLE_STORAGE, freeGridIndex)))
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << " make item error! id = " << item->GetDataID() << LogStream::eos;
				item->SyncProperty(GetOwner(), true);
				Item::DestroyItem(newitem);
				return movenum;
			}

			item->SetNum(item->GetNum() - leftnum);
			GetOwner()->OnRemoveItem(reason.c_str(), item, leftnum, oldNum);
			movenum = num;
		}
	}

	item->SyncProperty(GetOwner(), true);
	return movenum;
}

UInt16 ItemMgr::PullStorage(Item* item, UInt16 num)
{
	if (!item)	return 0;
	ItemDataEntry* dataEntry = item->GetDataEntry();
	if (!dataEntry)	return 0;
		
	PackType pack = GetPackType((ItemType)dataEntry->type);
	if(PACK_STORAGE == pack && !GetOwner()->CheckUseStorage()) return 0;
	Package* destPack = GetPackage(pack);
	if (!destPack)	return 0;

	//携带上限判断
	if (item->GetDataEntry()->getLimitNum > 0 && item->GetDataEntry()->getLimitNum < num + GetOwner()->GetItemNum(item->GetDataID()))
	{
		return 0;
	}

	item->SetOwnerId(GetOwner()->GetID(), true);

	return MoveItem(item, num, pack);
}

void ItemMgr::TakeOutAllFromStorage(UInt8 pack)
{
	if(PACK_LOTTERY != pack)
		return;
	
	class GetItemVisitor : public ItemVisitor
	{
	public:
		GetItemVisitor(std::list<Item*>& items)
			: m_Items(items){}
		~GetItemVisitor(){}

		bool operator()(Item* item)
		{
			m_Items.push_back(item);
			return true;
		}

	private:
		std::list<Item*>&	m_Items;
	};

	std::list<Item*> srcitems;
	GetItemVisitor visitor(srcitems);
	Package* pkg = GetPackage(pack);
	if (!pkg)	return;
	
	pkg->VisitItems(visitor);

	for(std::list<Item*>::iterator iter = srcitems.begin();
		iter != srcitems.end(); ++iter)
	{
		Item* item = *iter;
		PackType srcPack = GetPackType((ItemType)item->GetType());
		UInt16 itemNum = item->GetNum();
		if(MoveItem(item, itemNum, srcPack) < itemNum)
		{
			GetOwner()->SendNotify(21);
			break;
		}
	}

	pkg->OnItemChanged(GetOwner());
	//GetPackage(PACK_NORMAL)->OnItemChanged(GetOwner());
}

UInt16 ItemMgr::PushStorage(Item* item, UInt16 num, UInt8 storageType)
{
	// 称号背包的东西也可以放到仓库
	//					modify by zjy 
	//					2018.09.13
	if (item->GetPos().pack > PACK_FASHION && item->GetPos().pack != PACK_TITLE) return 0;
	if(!GetOwner()->CheckUseStorage()) return 0;

	/*if (!item->GetDataEntry()->canTrade)
	{
		// 不可交易的物品不能存仓库
		return 0;
	}*/                            

	//携带上限判断
	if (item->GetDataEntry()->getLimitNum > 0 && item->GetDataEntry()->getLimitNum < num + GetItemNumByPackType(item->GetDataID(), PackType(storageType)))
	{
		return 0;
	}

	return MoveItem(item, num, storageType);
}

UInt16 ItemMgr::PushRoleStorage(Item* item, UInt16 num, UInt32 gridIndex)
{
	if (item->GetPos().pack > PACK_FASHION && item->GetPos().pack != PACK_TITLE) return 0;
	if (!GetOwner()->CheckUseStorage()) return 0;

	//携带上限判断
	if (item->GetDataEntry()->getLimitNum > 0 && item->GetDataEntry()->getLimitNum < num + GetItemNumByPackType(item->GetDataID(), PACK_ROLE_STORAGE))
	{
		return 0;
	}

	return MoveItemToRoleStorage(item, num, gridIndex);
}

UInt32 ItemMgr::CheckWearEquip(Item* item)
{
	if (!item || !item->GetDataEntry() || !GetOwner())
		return ITEM_DATA_INVALID;

	if (!item->FitOccu(GetOwner()->GetOccu())) 
		return ITEM_EQUIP_OCCU;

	UInt32 decLevel = 0;
	if (item->GetValue(IKVKT_WBP_SPECIAL) == WBS_QIYUE)
	{
		//降低15级使用等级
		decLevel = item->GetValue(IKVKT_WBP_SPECIAL_DETAIL);
	}
	if (item->GetNeedLevel() > GetOwner()->GetLevel() + decLevel)
	{
		return ITEM_EQUIP_LV;
	}

	return SUCCESS;
}

void ItemMgr::OnHeartbeat(const Avalon::Time &now, int tickType)
{
	if (tickType == LOGICTICK_MIN)
	{
		CheckTimeItem(false, false);
	}
	
}

void ItemMgr::OnOffline()
{
	class DropItemVisitor : public ItemVisitor
	{
	public:
		DropItemVisitor(Player* owner, ItemMgr* mgr)
			:m_pOwner(owner), m_pItemMgr(mgr){}
		~DropItemVisitor(){}

		bool operator()(Item* item)
		{
// 			if(item->CheckOption(ITEM_OPTION_DROP_ONOFFLINE))
// 			{
// 				m_pItemMgr->RemoveItem(item);
// 				m_pOwner->DropItemDirect(item);
// 			}
			return true;
		}

	private:
		Player* m_pOwner;
		ItemMgr* m_pItemMgr;
	};
// 	DropItemVisitor visitor(GetOwner(), this);
// 	GetPackage(PACK_EQUIP)->VisitItems(visitor);
// 	GetPackage(PACK_NORMAL)->VisitItems(visitor);
}


void ItemMgr::OnDayChanged()
{
// 	class OnDayChangedVisitor : public ItemVisitor
// 	{
// 	public:
// 		OnDayChangedVisitor(Player* player):m_pPlayer(player){}
// 		~OnDayChangedVisitor(){}
// 
// 		bool operator()(Item* item)
// 		{
// // 			if(item->GetType() == ITEM_LOGINLIMIT)
// // 			{
// // 				item->SetParam1(item->GetParam1() + 1);
// // 				item->SyncProperty(m_pPlayer);
// // 
// // 				ScriptMgr::Instance()->GetPlayerScript()->CheckLoginLimitItem(m_pPlayer, item);
// // 			}
// 
// 			if (item->GetDataEntry()->dayUseNumMax > 0)
// 			{
// 				item->SetDayUseNum(0);
// 			}
// 			return true;
// 		}
// 
// 	private:
// 		Player* m_pPlayer;
// 	};
// 	OnDayChangedVisitor visitor(GetOwner());
// 	VisitItems(visitor);

	GetOwner()->SetCounter(COUNTER_OPEN_JAR_DAY, 0);
}

void ItemMgr::OnLogicDayChanged()
{
	resetEqRec();
	CheckTimeItem(true, false);
}

/**
 *@brief 同步道具访问器
 */
class SyncItemVisitor : public ItemVisitor
{
public:
	SyncItemVisitor(Player* player):m_pPlayer(player)
	{
		m_pPacket = Avalon::Packet::Create();
		m_pPacket->SetID(CLProtocol::SCENE_SYNC_ITEM /*| CL_PACKET_COMPRESSED_FLAG*/);
	}
	~SyncItemVisitor()
	{
		Avalon::Packet::Destroy(m_pPacket);
	}

	bool operator()(Item* item)
	{
		//兼容老数据,如果没有计算过分数的话就在load出来的时候计算一下
		if (item->GetValuedScore() == 0)
		{
			m_pPlayer->GetItemMgr().CalculateValuedScore(item);
		}

		Avalon::NetOutputStream stream(m_pPacket->GetBuffer() + m_pPacket->GetSize(), m_pPacket->GetMaxSize() - m_pPacket->GetSize());
		try
		{
			stream & item->GetID() & item->GetDataID();
			item->Encode(stream, SST_SELF, false);
		}
		catch(const Avalon::NetException& exception)
		{
			ErrorLogStream << "player(" << m_pPlayer->GetID() << "," << m_pPlayer->GetName()
				<< ") encode items failed:" << exception.what() << "!" << LogStream::eos;
			return false;
		}
		m_pPacket->SetSize(m_pPacket->GetSize() + UInt16(stream.GetPos()));

		// 快要超过道具最多个数了，先刷新一遍
		if (m_pPacket->GetSize() + UInt16(stream.GetPos()) > m_pPacket->GetMaxSize() - 5 * 1024)
		{
			DebugLogStream << "player(" << m_pPlayer->GetID() << ") item data too big, size(" << m_pPacket->GetSize() << "), flush.." << LogStream::eos;
			Flush();
		}
		return true;
	}

	void Flush()
	{
		Avalon::NetOutputStream stream(m_pPacket->GetBuffer() + m_pPacket->GetSize(), m_pPacket->GetMaxSize() - m_pPacket->GetSize());
		try
		{
			//最后64位0是数据结束符,8位1是表示登录初始化数据同步
			stream & ObjID_t(0) & UInt8(1);
		}
		catch(const Avalon::NetException&)
		{
			return;
		}
		m_pPacket->SetSize(m_pPacket->GetSize() + UInt16(stream.GetPos()));
		m_pPlayer->SendPacket(m_pPacket);
		m_pPacket->SetSize(0);
	}

private:
	//消息包
	Avalon::Packet*	m_pPacket;
	//玩家
	Player*	m_pPlayer;
};

//查询是否发改名卡返回
class SelectChangeNameCardCallback : public CLRecordCallback
{
	AVALON_DEFINE_OBJECTPOOL(SelectChangeNameCardCallback, Avalon::Mutex)
public:
	SelectChangeNameCardCallback(Player* player)
		:m_player(player) {}
	~SelectChangeNameCardCallback() {}

public:
	void OnFinished()
	{
		while (NextRow())
		{
			UInt32 changeNameCardType = GetData("type");
			std::string strSenderName;
			std::string	strTitle;
			std::string	strContent;

			enum ChangeNameCardType
			{
				CHANGE_NAME_CARD_TYPE_ADVENTURE_TEAM_NAME = 3,
			};

			UInt32 accid = GetData("accid");
			if (accid > 0)
			{
				if ((UInt32)CHANGE_NAME_CARD_TYPE_ADVENTURE_TEAM_NAME == changeNameCardType)
				{
					std::string reason = GetReason(SOURCE_TYPE_MERGE_SERVER_ADVENTURETEAMCHANGENAME);
					m_player->GetMailInfoByReason(reason.c_str(), strSenderName, strTitle, strContent);

					UInt32 cardId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_MERGE_SERVER_GIVE_RENAME_CARD);
					RewardGroup* group = m_player->GetRewardGroup();
					if (group)
					{
						group->AddReward(cardId, 1);

						Sys::SendSysMail(m_player->GetID(), strSenderName.c_str(), strTitle.c_str(), strContent.c_str(), reason.c_str(), group, MAIL_VALID_DAY);

						std::ostringstream cond;
						cond << "type = " << (UInt32)CHANGE_NAME_CARD_TYPE_ADVENTURE_TEAM_NAME << " AND accid = " << m_player->GetAccID();
						CLDeleteCommand* delCmd = CLRecordClient::Instance()->PrepareDeleteCommand(m_player->GetSrcZoneId(), "t_merge_give_namecard", cond.str());
						CLRecordClient::Instance()->SendCommand(delCmd);
					}
				}
			}
			else
			{
				//角色改名卡
				if (1 == changeNameCardType)
				{
					std::string reason = GetReason(SOURCE_TYPE_MERGE_SERVER_ROLECHANGENAME);
					m_player->GetMailInfoByReason(reason.c_str(), strSenderName, strTitle, strContent);


					UInt32 cardId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_CHANGE_PLAYERNAME_CARD);
					RewardGroup* group = m_player->GetRewardGroup();
					if (group)
					{
						group->AddReward(cardId, 1);

						Sys::SendSysMail(m_player->GetID(), strSenderName.c_str(), strTitle.c_str(), strContent.c_str(), reason.c_str(), group, MAIL_VALID_DAY);

						std::ostringstream cond;
						cond << "type = " << 1 << " AND guid = " << m_player->GetID();
						CLDeleteCommand* delCmd = CLRecordClient::Instance()->PrepareDeleteCommand(m_player->GetSrcZoneId(), "t_merge_give_namecard", cond.str());
						CLRecordClient::Instance()->SendCommand(delCmd);
					}
				}
				else if (2 == changeNameCardType)
				{
					//公会改名卡
					std::string reason = GetReason(SOURCE_TYPE_MERGE_SERVER_GUILDCHANGENAME);
					m_player->GetMailInfoByReason(reason.c_str(), strSenderName, strTitle, strContent);

					UInt32 cardId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_CHANGE_GUILDNAME_CARD);
					RewardGroup* group = m_player->GetRewardGroup();
					if (group)
					{
						group->AddReward(cardId, 1);
						Sys::SendSysMail(m_player->GetID(), strSenderName.c_str(), strTitle.c_str(), strContent.c_str(), reason.c_str(), group, MAIL_VALID_DAY);

						std::ostringstream cond;
						cond << "type = " << 2 << " AND guid = " << m_player->GetID();
						CLDeleteCommand* delCmd = CLRecordClient::Instance()->PrepareDeleteCommand(m_player->GetSrcZoneId(), "t_merge_give_namecard", cond.str());
						CLRecordClient::Instance()->SendCommand(delCmd);
					}
				}
			}
			
		}
	}

	void OnFailed(UInt32 errorcode)
	{
		
	}
private:
	Player* m_player;
};

void ItemMgr::OnOnline(bool bDayChanged)
{
	resetEqRec();
	SyncItemVisitor syncItem(GetOwner());
	VisitItems(syncItem);
	syncItem.Flush();

	if(bDayChanged)
	{
		OnDayChanged();
	}

	//added by huchenhui 2016.07.01
	Item* item = GetOwner()->GetEquip(ST_EQUIP_TITLE);
	if (item != NULL)
	{
		GetOwner()->SetActiveTitle(item->GetDataID());
	}

	//邮件发送仓库超量的道具
	if (m_MailGroup.size() > 0)
	{
		InfoLogStream << PLAYERINFO(GetOwner()) << " send overload storage item! num = " << (UInt32)m_MailGroup.size() << LogStream::eos;

		std::string strSenderName;
		std::string	strTitle;
		std::string	strContent;

		std::string reason = GetReason(SOURCE_TYPE_MERGE_SERVER_STORAGE);
		GetOwner()->GetMailInfoByReason(reason.c_str(), strSenderName, strTitle, strContent);

		std::vector<Item*> tmpMailGroup;
		for (size_t i = 0; i < m_MailGroup.size(); ++i)
		{
			if (!m_MailGroup[i])
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << " merge server storage send mail item is null!" << LogStream::eos;
				continue;
			}

			tmpMailGroup.push_back(m_MailGroup[i]);

			// 到邮件附件上限发送
			if ((UInt32)tmpMailGroup.size() % MAX_MAILITEM_NUM == 0)
			{
				Sys::SendSysMail(GetOwner()->GetID(), strSenderName.c_str(), strTitle.c_str(), strContent.c_str(), reason.c_str(), MAIL_VALID_DAY, NULL, tmpMailGroup);
				tmpMailGroup.clear();
			}
		}

		// 剩余的发送
		if (tmpMailGroup.size() != 0)
		{
			Sys::SendSysMail(GetOwner()->GetID(), strSenderName.c_str(), strTitle.c_str(), strContent.c_str(), reason.c_str(), MAIL_VALID_DAY, NULL, tmpMailGroup);
			tmpMailGroup.clear();
		}

		// 把多余的对象删除
		for (size_t i = 0; i < m_MailGroup.size(); ++i)
		{
			if (!m_MailGroup[i])
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << " merge server storage send mail item is null!" << LogStream::eos;
				continue;
			}

			//从数据库删除
			CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand(GetOwner()->GetSrcZoneId(), "t_item", m_MailGroup[i]->GetID());
			CLRecordClient::Instance()->SendCommand(cmd);

			//删除对象
			CL_SAFE_DELETE(m_MailGroup[i]);
		}

		m_MailGroup.clear();
	}

	//查看是否要发改名卡
	CLSelectCommand* selectcmd = CLRecordClient::Instance()->PrepareSelectCommand("t_merge_give_namecard");
	selectcmd->PutCond("accid", 0);
	selectcmd->PutCond("guid", GetOwner()->GetID());
	CLRecordClient::Instance()->SendCommand(selectcmd, new SelectChangeNameCardCallback(GetOwner()));

	//查看是否要发改名卡(账号相关)
	CLSelectCommand* selectcmd2 = CLRecordClient::Instance()->PrepareSelectCommand("t_merge_give_namecard");
	selectcmd2->PutCond("accid", GetOwner()->GetAccID());
	selectcmd2->PutCond("guid", 0);
	CLRecordClient::Instance()->SendCommand(selectcmd2, new SelectChangeNameCardCallback(GetOwner()));

	CheckTimeItem(bDayChanged, true);


	//清空不合法的药品配置
	std::vector<UInt32> pos = GetOwner()->GetPotionPos();
	for (size_t i = 0; i < pos.size(); ++i)
	{
		auto data = ItemDataEntryMgr::Instance()->FindEntry(pos[i]);
		if (data == nullptr)
		{
			continue;
		}
		bool invalid = true;
		auto vec = GetFitPotionType(i);
		for (auto &j : vec)
		{
			if (data->thirdType == TT_DUNGEON_POTION && data->subType == j)
			{
				invalid = false;
				break;
			}
		}
		if (invalid)
		{
			GetOwner()->SetPotionPos(i, 0);
			GetOwner()->SyncProperty();
		}
	}

	if (bDayChanged)
	{
		SendItemLoginUdpLog();
	}
}

void ItemMgr::NotifyDelItem(Item* item)
{
	CLProtocol::SceneNotifyDeleteItem notify;
	notify.id = item->GetID();
	GetOwner()->SendProtocol(notify);
}

void ItemMgr::NotifyNewItem(Item* item)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::SCENE_SYNC_ITEM);
	
	Avalon::NetOutputStream stream(packet->GetBuffer() + packet->GetSize(), packet->GetMaxSize() - packet->GetSize());
	try
	{
		stream & item->GetID() & item->GetDataID();
		item->Encode(stream, SST_SELF, false);
		//最后8位0标识非初始化数据同步
		stream & ObjID_t(0) & UInt8(0);
	}
	catch(const Avalon::NetException& exception)
	{
		ErrorLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() 
			<< ") encode items failed:" << exception.what() << "!" << LogStream::eos;
		Avalon::Packet::Destroy(packet);
		return;
	}
	packet->SetSize(UInt16(stream.GetPos()));

	GetOwner()->SendPacket(packet);
	Avalon::Packet::Destroy(packet);

	//通知获得新道具
	GetNewItemParam param;
	param.itemId = item->GetID();
	param._newItem = (void*)item;
	CLEventMgr::Instance()->SendPlayerEvent(GetOwner()->GetID(), CLEventMgr::ET_NOTIFY_NEWITEM, &param);
}

/**
 *@brief 同步装备访问器
 */
class SyncEquipsVisitor : public ItemVisitor
{
public:
	SyncEquipsVisitor(Avalon::NetOutputStream& stream):m_Stream(stream)
	{
	}
	~SyncEquipsVisitor()
	{
	}

	bool operator()(Item* item)
	{
		m_Stream & item->GetID() & item->GetDataID();
		item->Encode(m_Stream, SST_SELF, false);
		return true;
	}
private:
	//流
	Avalon::NetOutputStream& m_Stream;
};

void ItemMgr::SyncEquipsToWatcher(ObjID_t watcher)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::SCENE_SYNC_WATCH_EQUIPS);
	Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
	try
	{
		stream & GetOwner()->GetID();
		SyncEquipsVisitor visitor(stream);
		Package* pkg = GetPackage(PACK_EQUIP);
		if (!pkg)	return;
		
		pkg->VisitItems(visitor);
		stream & ObjID_t(0);
	}
	catch(Avalon::NetException&)
	{
		Avalon::Packet::Destroy(packet);
		return;
	}
	packet->SetSize(stream.GetPos());

	Router::TransmitToPlayer(watcher, packet);
	Avalon::Packet::Destroy(packet);
}

void ItemMgr::VisitItems(ItemVisitor& visitor)
{
	m_Items.VisitObjects(visitor);
}

void ItemMgr::VisitItems(UInt8 pack, ItemVisitor& visitor)
{
	Package* package = GetPackage(pack);
	if(package != NULL)
	{ 
		package->VisitItems(visitor);
	}
}

void ItemMgr::Input(Avalon::NetInputStream& stream)
{
	GetOwner()->InitPackage();

	ObjID_t itemId = 0;
	stream & itemId;
	while(itemId != 0)
	{
		UInt32 dataId = 0;
		stream & dataId;
		Item* item = Item::CreateItem(dataId);
		if(item == NULL)
		{
			throw Avalon::NetException("create item failed!");
			return;
		}
		item->SetID(itemId);
		if(!item->Decode(stream) || !LoadItem(item, false))
		{
			Item::DestroyItem(item);
			throw Avalon::NetException("load item failed!");
			return;
		}

		stream & itemId;
	}
}

void ItemMgr::Output(Avalon::NetOutputStream& stream)
{
	class EncodeItemVisitor : public ItemVisitor
	{
	public:
		EncodeItemVisitor(Avalon::NetOutputStream& stream)
			:m_Stream(stream), m_bSucc(true){}

		bool operator()(Item* item)
		{
			try
			{
				m_Stream & item->GetID() & item->GetDataID();
				item->Encode(m_Stream, SST_SCENE, false);
			}
			catch(const Avalon::NetException&)
			{
				m_bSucc = false;
				return false;
			}
			return true;
		}

		bool IsSucc() const{ return m_bSucc; }
	private:
		Avalon::NetOutputStream& m_Stream;
		bool m_bSucc;
	};
	EncodeItemVisitor visitor(stream);
	VisitItems(visitor);

	if(!visitor.IsSucc())
	{
		throw Avalon::NetException("encode item failed!");
	}
	stream & ObjID_t(0);
	
	return;
}

void ItemMgr::ClearMailGroup()
{
	for (size_t i = 0; i < m_MailGroup.size(); ++i)
	{
		CL_SAFE_DELETE(m_MailGroup[i]);
	}
	m_MailGroup.clear();
}

void ItemMgr::SyncItemBatch(std::vector<Item*> items)
{
	//同步到玩家
	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::SCENE_SYNC_ITEM_PROPERTY);

	Avalon::NetOutputStream stream(packet->GetBuffer() + packet->GetSize(), packet->GetMaxSize() - packet->GetSize());
	
	for (int i = 0; i < (int)items.size(); ++i)
	{
		Item* item = items[i];
		if (!item)
			continue;

		try
		{
			stream & item->GetID();
			item->Encode(stream, SST_SELF, true);
		}
		catch (const Avalon::NetException& exception)
		{
			ErrorLogStream << "player(" << GetOwner()->GetID() << "," << GetOwner()->GetName()
				<< ") encode items failed:" << exception.what() << "!" << LogStream::eos;
			Avalon::Packet::Destroy(packet);
			return;
		}

		//同步到数据库
		CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand(GetOwner()->GetSrcZoneId(), "t_item", item->GetID(), false);
		item->GetDBData(cmd, true);
		CLRecordClient::Instance()->SendCommand(cmd);
	}

	stream & ObjID_t(0);
	packet->SetSize(UInt16(stream.GetPos()));
	GetOwner()->SendPacket(packet);

	Avalon::Packet::Destroy(packet);
}

UInt32 ItemMgr::SealEquip(Item* item, std::vector<UInt32>& inscriptionIds)
{
	// 蜜蜡
	UInt32 matID = 200110001;

	if (!item || !item->GetDataEntry())
		return ITEM_DATA_INVALID;

	if (item->CheckInvalid())
	{
		return ITEM_DATA_INVALID;
	}

	if (item->GetSealCount() >= item->GetDataEntry()->sealMax)
		return ITEM_SEAL_COUNT_MAX;

	if (1 == item->GetSealState())
		return ITEM_ALREADY_SEAL;

	if (item->GetDataEntry()->color < ITEM_QUALITY_PURPLE ||
		item->GetDataEntry()->color > ITEM_QUALITY_PINK)
		return ITEM_SEAL_QUALITY_ERR;

	if (SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_EQUIP_SCHEME) && item->IsCanPushScheme() && GetOwner()->GetEqSchemeMgr().IsEquipInScheme(item->GetID()))
	{
		return ITEM_IILEG_OPER_IN_SCHEME;
	}

	EquipCalcScript * ecs = ScriptMgr::Instance()->GetEquipCalcScript();
	if (!ecs)
	{
		ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")EquipCalcScript is null!" << LogStream::eos;
		GetOwner()->SendNotify("EquipCalcScript is null!");
		return ITEM_DATA_INVALID;
	}
		
	UInt32 needMatNum = ecs->GetNeedSealMat(item->GetDataEntry()->needLevel, item->GetDataEntry()->color, item->GetSealCount());
	if (m_pOwner->GetItemNum(matID) < needMatNum)
		return ITEM_NOT_ENOUGH_MAT;
 
	//扣材料
	GetOwner()->RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_SEAL_EQUIP, item->GetID()).c_str(), matID, needMatNum);

	//摘取铭文
	UInt32 inscriptionId = 0;
	inscriptionId = GetOwner()->EquipInscriptionExtractWhenSeal(item, 1);
	if (inscriptionId != 0)
	{
		inscriptionIds.push_back(inscriptionId);
	}

	inscriptionId = GetOwner()->EquipInscriptionExtractWhenSeal(item, 2);
	if (inscriptionId != 0)
	{
		inscriptionIds.push_back(inscriptionId);
	}

	item->SetSealCount(item->GetSealCount() + 1);
	item->SetSealState(1);
	item->SyncProperty(GetOwner());

	if (item->IsItemLock(ILT_AUCTION_RETURN_LOCK))
	{
		item->ClearItemLock(ILT_AUCTION_RETURN_LOCK);
	}

	return SUCCESS;
}

void ItemMgr::SendItemSysMail(ObjID_t recvId, UInt32 itemId, UInt32 notifyId, std::string title)
{
	char buffer[CL_MAX_CHAT_LEN + 1];
	memset(buffer, 0, sizeof(buffer));
	bool ret = SysNotifyMgr::Instance()->MakeNotify(notifyId, 0, 0, buffer, CL_MAX_CHAT_LEN, itemId);

	if (!ret) return;

	//没有发件人名字   by huchenhui 2016.8.17
	//Sys::SendSysMail(recvId, title.c_str(), buffer, 0);
}

UInt32 ItemMgr::CheckSealEquip(Item* item, UInt16& needNum, UInt32& needId)
{
	if (!item || !item->GetDataEntry())
		return ITEM_DATA_INVALID;

	if (item->GetSealCount() >= item->GetDataEntry()->sealMax)
		return ITEM_SEAL_COUNT_MAX;

	if (1 == item->GetSealState())
		return ITEM_ALREADY_SEAL;

	if (item->GetDataEntry()->color < ITEM_QUALITY_PURPLE ||
		item->GetDataEntry()->color > ITEM_QUALITY_PINK)
		return ITEM_SEAL_QUALITY_ERR;

	EquipCalcScript * ecs = ScriptMgr::Instance()->GetEquipCalcScript();
	if (!ecs)
	{
		ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")EquipCalcScript is null!" << LogStream::eos;
		GetOwner()->SendNotify("EquipCalcScript is null!");
		return ITEM_DATA_INVALID;
	}
		

	needNum = ecs->GetNeedSealMat(item->GetDataEntry()->needLevel, item->GetDataEntry()->color, item->GetSealCount());
	needId = 200110001;	//黄金蜜蜡 

	return SUCCESS;
}

UInt32 ItemMgr::ReMakeEquipQlv(Item* item, UInt8 autoUsePoint)
{
	if (!item || !item->GetDataEntry())
		return ITEM_DATA_INVALID;

	if (item->CheckInvalid())
	{
		return ITEM_DATA_INVALID;
	}

	UInt32 oldQlv = item->GetQualityLv();
	if (oldQlv >= 100)
	{
		return ITEM_EQUIPQL_FULL;
	}

	//检查材料和钱
	UInt32 removeId = 0;
	UInt32 removeNum = 0;
	UInt32 matID = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_EQUIPQL_MATID);
	UInt32 needMatNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_EQUIPQL_MATNUM);
	EquipQLMode mode = (EquipQLMode)SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_EQUIPQL_MODE);

	if (mode >= EQM_MAX)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "EquipQLMode is error! mode:" << mode << LogStream::eos;
		return ITEM_DATA_INVALID;
	}

	UInt32 matNum = m_pOwner->GetItemNum(matID);
	UInt32 pointNum = m_pOwner->GetPoint();

	auto quickData = QuickBuyDataEntryMgr::Instance()->FindEntry(matID);
	if (needMatNum <= matNum)
	{
		removeId = matID;
		removeNum = needMatNum;
	}
	else
	{
		if (autoUsePoint > 0)
		{
			//进行快速购买
			if (!quickData)
			{
				return ITEM_DATA_INVALID;
			}

			//钱不够
			if (quickData->costNum > m_pOwner->GetItemNum(quickData->costItemId))
			{
				return ITEM_NOT_ENOUGH_MONEY;
			}

			removeId = quickData->costItemId;
			removeNum = quickData->costNum;
		}
		else
		{
			return ITEM_NOT_ENOUGH_MAT;
		}
	}

	
		
	//进行品级调整
	if (mode == EQM_ALL)
	{
		if (!RandEquipQLA(item))
		{
			return ITEM_DATA_INVALID;
		}
	}
	else if (mode == EQM_SINGLE)
	{
		if (!RandEquipQLB(item))
		{
			return ITEM_DATA_INVALID;
		}
	}
	
#ifdef _DEBUG
	if (m_debugEquipQl)
	{
		char szContent[256];
		memset(szContent, 0, sizeof(szContent));
		SysNotifyMgr::Instance()->MakeNotify("ItemQlCount = {0}", szContent, 256, item->GetQlNum());
		std::string chatContent(szContent);
		GetOwner()->SyncDebugChat(chatContent);
	}
#endif

	// 安全锁开启，不允许消耗点券，品级调整
	if (removeId == ITEM_POINT && !GetOwner()->SecurityLockCheck())
		return ErrorCode::SECURITY_LOCK_FORBID_OP;


	if (autoUsePoint > 0)
	{
		//进行快速购买
		if (quickData)
		{
			if (quickData->costItemId == ITEM_POINT)
			{
				UInt32 rate = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_TICKET_CONVERT_MALL_INTEGRAL_RATE);
				UInt8 multiple = 0;
				UInt32 endTime = 0;
				MallShopMultiIDataEntryMgr::Instance()->GetMallItemMultiple(quickData->id, multiple, endTime);
				GetOwner()->AddMallPoint(GetReason().c_str(), quickData->costNum * (quickData->multiple + multiple) * rate);
			}
		}
	}
	//扣材料
	GetOwner()->RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_REMAKE_EQUIP, item->GetID()).c_str(), removeId, removeNum);

	//发日志
	UInt32 newMatNum = m_pOwner->GetItemNum(matID);
	UInt32 newPointNum = m_pOwner->GetPoint();
	m_pOwner->SendReMakeEquipQlvUdpLog(item->GetID(), item->GetDataID(), oldQlv, item->GetQualityLv(), matNum, newMatNum, pointNum, newPointNum, 
		autoUsePoint, item->GetQuality(), item->GetSubType());

	m_pOwner->OnRemakeEquip(item->GetDataID(), item->GetQualityLv());

	//抛出事件
	IEventParam param;
	CLEventMgr::Instance()->SendPlayerEvent(GetOwner()->GetID(), CLEventMgr::EventType::ET_EQUIP_REMAKE_QL, &param);

	return SUCCESS;
}

UInt32 ItemMgr::ReMakeEquipQlvUsePerfect(Item* item)
{
	if (!item || !item->GetDataEntry())
		return ITEM_DATA_INVALID;

	if (item->CheckInvalid())
	{
		return ITEM_DATA_INVALID;
	}

	UInt32 oldQlv = item->GetQualityLv();
	if (oldQlv >= 100)
	{
		return ITEM_EQUIPQL_FULL;
	}

	//检查材料和钱
	UInt32 removeId = 0;
	UInt32 removeNum = 0;
	UInt32 matID = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_EQUIPQL_PERF_MATID);
	UInt32 needMatNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_EQUIPQL_PERF_MATNUM);

	UInt32 matNum = m_pOwner->GetItemNum(matID);
	UInt32 pointNum = m_pOwner->GetPoint();

	if (needMatNum <= matNum)
	{
		removeId = matID;
		removeNum = needMatNum;
	}
	else
	{
		return ITEM_NOT_ENOUGH_MAT;
	}

	UInt8 qlRate = 100;
	item->SetQualityLv(qlRate);
	DebugLogStream << PLAYERINFO(GetOwner()) << " SetQualityLv:" << qlRate << LogStream::eos;
	item->ClearAttribute();
	item->SetAllQualityProp(qlRate);

	item->SetQlNum(item->GetQlNum() + 1);
	item->SyncProperty(GetOwner());

	// 安全锁开启，不允许消耗点券，品级调整
	if (removeId == ITEM_POINT && !GetOwner()->SecurityLockCheck())
		return ErrorCode::SECURITY_LOCK_FORBID_OP;

	//扣材料
	GetOwner()->RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_REMAKE_EQUIP, item->GetID()).c_str(), removeId, removeNum);

	//发日志
	UInt32 newMatNum = m_pOwner->GetItemNum(matID);
	UInt32 newPointNum = m_pOwner->GetPoint();
	m_pOwner->SendReMakeEquipQlvUdpLog(item->GetID(), item->GetDataID(), oldQlv, item->GetQualityLv(), matNum, newMatNum, pointNum, newPointNum,
		0, item->GetQuality(), item->GetSubType());

	m_pOwner->OnRemakeEquip(item->GetDataID(), item->GetQualityLv());

	//抛出事件
	IEventParam param;
	CLEventMgr::Instance()->SendPlayerEvent(GetOwner()->GetID(), CLEventMgr::EventType::ET_EQUIP_REMAKE_QL, &param);

	return SUCCESS;

}

UInt32 ItemMgr::TransferEquip(Item* item, Item* transferStone)
{
	if (!item || !item->GetDataEntry() || !transferStone || !transferStone->GetDataEntry())
		return ITEM_DATA_INVALID;

	if (item->CheckInvalid())
	{
		return ITEM_DATA_INVALID;
	}

	if (SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_EQUIP_SCHEME) && item->IsCanPushScheme() && GetOwner()->GetEqSchemeMgr().IsEquipInScheme(item->GetID()))
	{
		return ITEM_IILEG_OPER_IN_SCHEME;
	}

	//检查材料和钱
	UInt32 needcolor = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_EQUIP_TRANSFER_COLOR);
	if (item->GetDataEntry()->color < needcolor)
	{
		return ITEM_EQUIPTSF_COLOR;
	}

	//根据装备找到需要的转移石头  代码后面添加
	UInt32 matID = transferStone->GetDataID();
	UInt32 needMatNum = 1;
	//UInt32 needMatNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_EQUIP_TRANSFER_NUM);

	class GetDataEntryVisitor : public EqTransMapDataEntryVisitor
	{
	public:
		GetDataEntryVisitor(UInt32 itemId_) : itemId(itemId_), dataEntry(NULL){}
		~GetDataEntryVisitor()  {}
		virtual bool operator()(EquipTransMapDataEntryBase* dataEntry)
		{
			if (dataEntry->itemId == itemId)
			{
				this->dataEntry = dataEntry;
				return false;
			}
			return true;
		}
	public:
		UInt32 itemId;
		EquipTransMapDataEntryBase* dataEntry;
	};

	GetDataEntryVisitor visitor(matID);
	EquipTransMapDataEntryMgr::Instance()->Visit(visitor);

	if (visitor.dataEntry == NULL)
	{
		InfoLogStream << PLAYERINFO(GetOwner()) << "TransferEquip visitor.dataEntry == NULL" << LogStream::eos;
		return ITEM_DATA_INVALID;
	}

	UInt32 eqlv = item->GetNeedLevel();
	if (eqlv < visitor.dataEntry->minLev || eqlv > visitor.dataEntry->maxLev)
	{
		return ITEM_DATA_INVALID;
	}

	bool checkpart = false;
	for (UInt32 i = 0; i <= visitor.dataEntry->subTypes.size(); i++)
	{
		if (visitor.dataEntry->subTypes[i] == item->GetSubType())
		{
			checkpart = true;
			break;
		}
	}

	if (!checkpart)
	{
		return ITEM_DATA_INVALID;
	}

	//UInt32 matNum = m_pOwner->GetItemNum(matID);
	if (needMatNum > transferStone->GetNum())
	{
		return ITEM_NOT_ENOUGH_MAT;
	}

	item->SetTransferStone(transferStone->GetDataEntry());
	item->SyncProperty(GetOwner());

	//扣材料
	GetOwner()->RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_TRANSFER, item->GetID()).c_str(), matID, needMatNum);

	DebugLogStream << PLAYERINFO(GetOwner()) << " TransferEquip  equip id:" << item->GetID() << "  tansferstone itemid:" << matID << LogStream::eos;
	//抛出事件

	return SUCCESS;
}

//装备回收
//处理装备回收提交请求
UInt32 ItemMgr::handleEqRecSubcmt(std::vector<UInt64>& eqUids, std::vector<EqRecScoreItem>& scoreItems, UInt32& score)
{
	Package* equipPack = GetPackage(PACK_EQUIP);
	if (!equipPack)
	{
		ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() <<
			")equipPack is null packType = " << PACK_EQUIP << LogStream::eos;

		return ITEM_DATA_INVALID;
	}

	Package* eqrecoPack = GetPackage(PACK_EQRCOV);
	if (!eqrecoPack)
	{
		ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() <<
			")equipPack is null packType = " << PACK_EQRCOV << LogStream::eos;

		return ITEM_DATA_INVALID;
	}
	
	UInt32 tms = GetOwner()->GetCounter(EQUIP_RECO_TIMES);
	if (tms + eqUids.size() > SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_EQUIP_RECO_TIMES))
	{
		return ITEM_EQUIPRECO_BEYOND;
	}

	UInt32 totleScores = 0;
	std::vector<Item*> equips;
	std::vector<UInt32> scores;

	for (size_t i = 0; i < eqUids.size(); ++i)
	{
		Item* equip = FindItem(eqUids[i]);
		if (!equip)
		{
			return ITEM_DATA_INVALID;
		}

		ItemPos pos = equip->GetPos();
		if (pos.pack != PACK_EQUIP)
		{
			return ITEM_DATA_INVALID;
		}


		if (equip->IsItemLock(ILT_ITEM_LOCK))
		{
			return ErrorCode::ITEM_LOCKED;
		}

		if (equip->IsItemLock(ILT_LEASE_LOCK))
		{
			return ITEM_IN_LEASING;
		}

		//算出装备回收积分
		UInt32 score = 0;
		EquipRecoveryPriceDataEntry* entry = EqRecoveryPriceDataEntryMgr::Instance()->FindEntry(equip->GetNeedLevel());
		if (!entry)
		{
			ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() <<
				")EquipRecoveryPriceDataEntry is null id = " << equip->GetDataEntry()->needLevel << LogStream::eos;
			return ITEM_DATA_INVALID;
		}
		ItemQuality color = (ItemQuality)equip->GetDataEntry()->color;
		
		std::vector<UInt32>& prices = entry->prices[color];
		if (prices.size() == 0)
		{
			ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() <<
				") ItemMgr::handleEqRecSubcmt prices.size() == 0 id = " << equip->GetDataEntry()->needLevel << " color = " << color << LogStream::eos;
			return ITEM_DATA_INVALID;
		}
		score = prices[0];
		//获取积分提升权重
		class DataEntryVisitor : public EqRecoScUpPrbWeightDataEntryVisitor
		{
		public:
			DataEntryVisitor(std::vector<UInt32>& prices_) : prices(prices_){}
			~DataEntryVisitor() {}
			virtual bool operator()(EquipRecoScUpPrbWeightDataEntryBase* dataEntry)
			{
				if (dataEntry->iD <= prices.size())
				{
					ScoreItem item(prices[dataEntry->iD - 1], dataEntry->probWeight);
					items.push_back(item);
				}
				return true;
			}
		public:
			struct ScoreItem
			{
				ScoreItem(UInt32 sc, UInt32 wet) : score(sc), weight(wet) {}
				UInt32	score;
				UInt32	weight;
			};

			std::vector<ScoreItem> items;
			std::vector<UInt32> prices; //价格范围
		};

		DataEntryVisitor visitor_(prices);
		EquipRecoScUpPrbWeightDataEntryMgr::Instance()->Visit(visitor_);

		//随机新积分
		UInt32 totalWeight = 0;
		for (UInt32 i = 0; i < visitor_.items.size(); ++i)
		{
			totalWeight += visitor_.items[i].weight;
		}

		UInt32 randWeight = Avalon::Random::RandBetween(1, totalWeight);

		for (UInt32 i = 0; i < visitor_.items.size(); ++i)
		{
			if (visitor_.items[i].weight >= randWeight)
			{
				score = visitor_.items[i].score;
				break;
			}
			else
			{
				randWeight -= visitor_.items[i].weight;
			}
		}
		//score = Avalon::Random::RandBetween(price[0], price[price.size() - 1]);
		EqRecScoreItem sitem(eqUids[i], score);
		scoreItems.push_back(sitem);
		equips.push_back(equip);
		scores.push_back(score);
		totleScores += score;
	}

	//移动装备
	for (size_t i = 0; i < equips.size(); ++i)
	{
		Item* equip = equips[i];
		equip->SetEqRecoScore(scores[i]);
		if (equip->IsItemLock(ILT_AUCTION_RETURN_LOCK))
		{
			equip->ClearItemLock(ILT_AUCTION_RETURN_LOCK);
		}
		MoveItem(equip, 1, PACK_EQRCOV);
	}

	//GetOwner()->IncCounter(EQUIP_RECO_SCORE, totleScores);
	//score = GetOwner()->GetCounter(EQUIP_RECO_SCORE);
	
	auto reason = GetReason(SOURCE_TYPE_EQRECO);
	GetOwner()->AddEqRecScore(reason.c_str(), totleScores);
	score = GetOwner()->GetEqRecScore();

	//修改罐子状态
	updateEqRecJarState();

	GetOwner()->IncCounter(EQUIP_RECO_TIMES, eqUids.size());

	UInt32 stamp = getEqRecUpdateTimeStamp() / 1000;
	if (GetOwner()->GetCounter(EQUIP_RECO_RESET_TM) != stamp)
	{
		GetOwner()->SetCounter(EQUIP_RECO_RESET_TM, stamp);
	}
	
	return SUCCESS;
}

//装备赎回请求
UInt32 ItemMgr::handleEqRecRedeem(Item* item, UInt32& consScore)
{
	if (!item || !item->GetDataEntry())
		return ITEM_DATA_INVALID;

	if (item->CheckInvalid())
	{
		return ITEM_DATA_INVALID;
	}

	if (item->GetPos().pack != PACK_EQRCOV)
	{
		return ITEM_DATA_INVALID;
	}

	Package* equipPack = GetPackage(PACK_EQUIP);
	if (!equipPack)
	{
		ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() <<
			")equipPack is null packType = " << PACK_EQUIP << LogStream::eos;

		return ITEM_DATA_INVALID;
	}

	Package* eqrecoPack = GetPackage(PACK_EQRCOV);
	if (!eqrecoPack)
	{
		ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() <<
			")equipPack is null packType = " << PACK_EQRCOV << LogStream::eos;

		return ITEM_DATA_INVALID;
	}

	if (equipPack->GetCapacity() == 0)
	{
		return ITEM_EQUIPPACK_FULL;
	}

	//计算开罐消耗积分
	class GetDataEntryVisitor : public EqRecoRewardDataEntryVisitor
	{
	public:
		GetDataEntryVisitor(Player* player) : m_player(player), m_score(0){}
		~GetDataEntryVisitor()  {}
		virtual bool operator()(EquipRecoveryRewardDataEntryBase* dataEntry)
		{
			std::ostringstream jarstate;
			jarstate << EQUIP_RECO_REWARD_PER << dataEntry->iD;
			const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(jarstate.str());
			if (!counterConfig)
			{
				CounterCfg::Instance()->RegCounter(jarstate.str().c_str(), COUNTER_CYCLE_NONE);
			}
			UInt32 jarst = m_player->GetCounter(jarstate.str().c_str());
			if (jarst == EQRECO_JARST_OPEND && dataEntry->integral > m_score)
			{
				m_score = dataEntry->integral;
			}
			return true;
		}
	public:
		Player* m_player;
		UInt32 m_score;
	};

	GetDataEntryVisitor visitor(GetOwner());
	EquipRecoveryRewardDataEntryMgr::Instance()->Visit(visitor);

	//UInt32 score = GetOwner()->GetCounter(EQUIP_RECO_SCORE);
	UInt32 score = GetOwner()->GetEqRecScore();
	UInt32 redeemScore = score - visitor.m_score; //可赎回积分
	if (redeemScore < item->GetEqRecoScore())
	{
		return ITEM_EQUIPRECO_REDEEM;
	}

	UInt32 moneyId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_EQUIP_RECO_SHUHUI_HBTYID);
	UInt32 min_consume = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_EQUIP_RECO_SHUHUI_HBNUM);
	UInt32 priceratio = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_EQUIP_RECO_SHUHUI_PRICERATION);
	priceratio = priceratio == 0 ? 5 : priceratio;
	UInt32 consume = item->GetDataEntry()->recommendPrice * priceratio / 100;
	consume = consume < min_consume ? min_consume : consume;

	ItemDataEntry* mdataEntry = ItemDataEntryMgr::Instance()->FindEntry(moneyId);
	if (!mdataEntry)
	{
		InfoLogStream << PLAYERINFO(GetOwner()) << "handleEqRecRedeem mdataEntry == null of moneyId = " << moneyId << LogStream::eos;
		return ITEM_DATA_INVALID;
	}
	ItemSubType moneyType = (ItemSubType)mdataEntry->subType;
	if (!GetOwner()->CheckRmMoney(moneyType, consume))
	{
		return ITEM_NOT_ENOUGH_MONEY;
	}

	//扣消耗
	GetOwner()->RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_EQRECO_REDEEM, item->GetID()).c_str(), moneyId, consume);

	//GetOwner()->SetCounter(EQUIP_RECO_SCORE, score - item->GetEqRecoScore()); //扣积分
	GetOwner()->RemoveEqRecScore(GetReason(LogSourceType::SOURCE_TYPE_EQRECO_REDEEM, item->GetID()).c_str(), item->GetEqRecoScore());
	GetOwner()->SetCounter(EQUIP_RECO_TIMES, GetOwner()->GetCounter(EQUIP_RECO_TIMES) - 1); //返还捐献次数
	consScore = item->GetEqRecoScore();
	item->SetEqRecoScore(0);
	MoveItem(item, 1, PACK_EQUIP);

	//修改罐子状态
	updateEqRecJarState();

	return SUCCESS;
}

//装备回收积分提升请求
UInt32 ItemMgr::handleEqRecUpscore(Item* item, UInt32& upscore)
{
	upscore = 0;
	if (!item || !item->GetDataEntry())
		return ITEM_DATA_INVALID;

	if (item->CheckInvalid())
	{
		return ITEM_DATA_INVALID;
	}

	if (item->GetPos().pack != PACK_EQRCOV)
	{
		return ITEM_DATA_INVALID;
	}

	//从配置表算出提高到的积分和消耗
	UInt32 oldscore = item->GetEqRecoScore();
	UInt32 newscore = 0;
	UInt32 consume = 0;

	//计算消耗
	UInt32 times = GetOwner()->GetCounter(EQUIP_RECO_UPSC_TMS);
	//获取装备最高回收积分
	UInt32 maxscore = EqRecoveryPriceDataEntryMgr::Instance()->getEquipMaxPrice(item->GetNeedLevel(), item->GetDataEntry()->color);
	if (maxscore == 0)
	{
		InfoLogStream << PLAYERINFO(GetOwner()) << "handleEqRecUpscore maxscore == 0, lv = " << item->GetNeedLevel() << " color = " << item->GetDataEntry()->color << LogStream::eos;
		return ITEM_DATA_INVALID;
	}
	if (oldscore == maxscore)
	{
		return ITEM_EQUIPRECO_UPSCMAX;
	}

	//获取系数
	class GetDataEntryVisitor : public EqRecoScUpConsRtiDataEntryVisitor
	{
	public:
		GetDataEntryVisitor(UInt32 times) : m_times(times), m_ratio(0){}
		~GetDataEntryVisitor()  {}
		virtual bool operator()(EquipRecoScUpConsRtiDataEntry* dataEntry)
		{
			if (dataEntry->timesSpans[1] == -1 && (Int32)m_times >= dataEntry->timesSpans[0])
			{
				m_ratio = dataEntry->ratio;
				return false;
			}
			else if ((Int32)m_times >= dataEntry->timesSpans[0] && (Int32)m_times <= dataEntry->timesSpans[1])
			{
				m_ratio = dataEntry->ratio;
				return false;
			}
			return true;
		}
	public:
		UInt32 m_times;
		UInt32 m_ratio;
	};

	GetDataEntryVisitor visitor(times + 1);
	EquipRecoScUpConsRtiDataEntryMgr::Instance()->Visit(visitor);

	if (visitor.m_ratio == 0)
	{
		InfoLogStream << PLAYERINFO(GetOwner()) << "handleEqRecUpscore visitor.m_ratio == 0, times = " << times << LogStream::eos;
		return ITEM_DATA_INVALID;
	}

	consume = maxscore * visitor.m_ratio;
	UInt32 moneyId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_EQUIP_RECO_UPSHB);
	ItemDataEntry* mdataEntry = ItemDataEntryMgr::Instance()->FindEntry(moneyId);
	if (!mdataEntry)
	{
		InfoLogStream << PLAYERINFO(GetOwner()) << "handleEqRecUpscore mdataEntry == null" << LogStream::eos;
		return ITEM_DATA_INVALID;
	}
	ItemSubType moneyType = (ItemSubType)mdataEntry->subType;
	if (!GetOwner()->CheckRmMoney(moneyType, consume))
	{
		return ITEM_EQUIPRECO_UPSCMONY;
	}
	
	//获取装备积分范围
	EquipRecoveryPriceDataEntry* price_entry = EqRecoveryPriceDataEntryMgr::Instance()->FindEntry(item->GetNeedLevel());
	if (!price_entry)
	{
		InfoLogStream << PLAYERINFO(GetOwner()) << "handleEqRecUpscore EqRecoveryPriceDataEntryMgr price_entry == null of id = " << item->GetNeedLevel() << LogStream::eos;
		return ITEM_DATA_INVALID;
	}
	std::map<UInt8, std::vector<UInt32>>::iterator it = price_entry->prices.find(item->GetDataEntry()->color);
	if (it == price_entry->prices.end())
	{
		InfoLogStream << PLAYERINFO(GetOwner()) << "handleEqRecUpscore EqRecoveryPriceDataEntryMgr prices not find of id = " << item->GetNeedLevel() << "color = " << item->GetDataEntry()->color << LogStream::eos;
		return ITEM_DATA_INVALID;
	}
	
	std::vector<UInt32>& prices = it->second; //装备积分范围
	//获取积分提升权重
	class DataEntryVisitor : public EqRecoScUpPrbWeightDataEntryVisitor
	{
	public:
		DataEntryVisitor(std::vector<UInt32>& prices_) : prices(prices_){}
		~DataEntryVisitor() {}
		virtual bool operator()(EquipRecoScUpPrbWeightDataEntryBase* dataEntry)
		{
			if (dataEntry->iD <= prices.size())
			{
				ScoreItem item(prices[dataEntry->iD -1 ], dataEntry->probWeight);
				items.push_back(item);
			}
			return true;
		}
	public:
		struct ScoreItem
		{
			ScoreItem(UInt32 sc, UInt32 wet) : score(sc), weight(wet) {}
			UInt32	score;
			UInt32	weight;
		};

		std::vector<ScoreItem> items; 
		std::vector<UInt32> prices; //价格范围
	};

	DataEntryVisitor visitor_(prices);
	EquipRecoScUpPrbWeightDataEntryMgr::Instance()->Visit(visitor_);

	//随机新积分
	UInt32 totalWeight = 0;
	for (UInt32 i = 0; i < visitor_.items.size(); ++i)
	{
		totalWeight += visitor_.items[i].weight;
	}

	UInt32 randWeight = Avalon::Random::RandBetween(1, totalWeight);

//#ifdef _DEBUG
//	GetOwner()->SendNotify("totalWeight = {0}, randWeight = {1}", totalWeight, randWeight);
//#endif

	for (UInt32 i = 0; i < visitor_.items.size(); ++i)
	{
		if (visitor_.items[i].weight >= randWeight)
		{
			newscore = visitor_.items[i].score;
			break;
		}
		else
		{
			randWeight -= visitor_.items[i].weight;
		}
	}

	newscore = newscore > maxscore ? maxscore : newscore;

	if (newscore > oldscore)
	{
		upscore = newscore - oldscore;
		item->SetEqRecoScore(newscore);
		item->SyncProperty(GetOwner());
		//GetOwner()->SetCounter(EQUIP_RECO_SCORE, GetOwner()->GetCounter(EQUIP_RECO_SCORE) - oldscore + newscore);
		GetOwner()->AddEqRecScore(GetReason(LogSourceType::SOURCE_TYPE_EQRECO_UPSCORE, item->GetID()).c_str(), upscore);
		//修改罐子状态
		updateEqRecJarState();
	}

	GetOwner()->IncCounter(EQUIP_RECO_UPSC_TMS, 1);
	//扣消耗
	GetOwner()->RemoveItem(GetReason(LogSourceType::SOURCE_TYPE_EQRECO_UPSCORE, item->GetID()).c_str(), moneyId, consume);
	return SUCCESS;
}

//检查积分罐子是否可以打开
UInt32 ItemMgr::checkEqRecOpenJar(UInt32 jarId)
{
	class GetDataEntryVisitor : public EqRecoRewardDataEntryVisitor
	{
	public:
		GetDataEntryVisitor(UInt32 jarid) : jar_id(jarid), entry_id(0){}
		~GetDataEntryVisitor()  {}
		virtual bool operator()(EquipRecoveryRewardDataEntryBase* dataEntry)
		{
			if (dataEntry->jarID == jar_id)
			{
				entry_id = dataEntry->iD;
				return false;
			}
			return true;
		}

	public:
		UInt32 jar_id;
		UInt32 entry_id;
	};

	GetDataEntryVisitor visitor(jarId);
	EquipRecoveryRewardDataEntryMgr::Instance()->Visit(visitor);

	if (visitor.entry_id > 0)
	{
		std::ostringstream jarstate;
		jarstate << EQUIP_RECO_REWARD_PER << visitor.entry_id;
		const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(jarstate.str());
		if (!counterConfig)
		{
			CounterCfg::Instance()->RegCounter(jarstate.str().c_str(), COUNTER_CYCLE_NONE);
		}
		UInt32 st = GetOwner()->GetCounter(jarstate.str().c_str());
		if (st == EQRECO_JARST_CANOPEN)
		{
			return SUCCESS;
		}
		else if (st == EQRECO_JARST_CANNOTOPEN)
		{
			return ITEM_EQUIPRECO_JARCANNOTOPEN;
		}
		else if (st == EQRECO_JARST_OPEND)
		{
			return ITEM_EQUIPRECO_JAROPENED;
		}
		return ITEM_DATA_INVALID;
	}
	return ITEM_DATA_INVALID;
}

void ItemMgr::handleEqRecOpenJar(UInt32 jarId)
{
	class GetDataEntryVisitor : public EqRecoRewardDataEntryVisitor
	{
	public:
		GetDataEntryVisitor(UInt32 jarid) : jar_id(jarid), entry_id(0){}
		~GetDataEntryVisitor()  {}
		virtual bool operator()(EquipRecoveryRewardDataEntryBase* dataEntry)
		{
			if (dataEntry->jarID == jar_id)
			{
				entry_id = dataEntry->iD;
				return false;
			}
			return true;
		}

	public:
		UInt32 jar_id;
		UInt32 entry_id;
	};

	GetDataEntryVisitor visitor(jarId);
	EquipRecoveryRewardDataEntryMgr::Instance()->Visit(visitor);
	
	if (visitor.entry_id > 0)
	{
		std::ostringstream jarstate;
		jarstate << EQUIP_RECO_REWARD_PER << visitor.entry_id;
		const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(jarstate.str());
		if (!counterConfig)
		{
			CounterCfg::Instance()->RegCounter(jarstate.str().c_str(), COUNTER_CYCLE_NONE);
		}
		GetOwner()->SetCounter(jarstate.str().c_str(), 2);
	}
}

//计算装备回收刷新时间戳
UInt64 ItemMgr::getEqRecUpdateTimeStamp()
{
	Avalon::Time currtime = Avalon::Time::CurrentTime();
	Avalon::Date currDate(currtime);
	currDate.Hour(6);
	currDate.Min(0);
	currDate.Sec(0);
	currDate.MSec(0);
	Int32 wday = currDate.WDay();
	//周一
	if (wday == 1)
	{
		if (currtime.MSec() < currDate.ToTime().MSec())
		{
			return currDate.ToTime().MSec();
		}
	}

	UInt64 timestamp = (8 - wday) * Avalon::Time::MSECS_OF_DAY + currDate.ToTime().MSec();
	return timestamp;
}

//重置装备回收
void ItemMgr::resetEqRec(bool force)
{
	UInt32  deltm = GetOwner()->GetCounter(EQUIP_RECO_RESET_TM);
	if (CURRENT_TIME.Sec() >= deltm || force)
	{
		ClearPackage(PACK_EQRCOV, SOURCE_TYPE_EQRECO_DESTORY_SELF);
		GetOwner()->SetCounter(EQUIP_RECO_TIMES, 0);
		GetOwner()->SetCounter(EQUIP_RECO_SCORE, 0);
		GetOwner()->SetCounter(EQUIP_RECO_UPSC_TMS, 0);
		GetOwner()->SetCounter(EQUIP_RECO_RESET_TM, getEqRecUpdateTimeStamp() / 1000);
		//修改罐子状态
		class DataEntryVisitor : public EqRecoRewardDataEntryVisitor
		{
		public:
			DataEntryVisitor(Player* player) : m_player(player){}
			~DataEntryVisitor()  {}
			virtual bool operator()(EquipRecoveryRewardDataEntryBase* dataEntry)
			{
				std::ostringstream jarstate;
				jarstate << EQUIP_RECO_REWARD_PER << dataEntry->iD;
				const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(jarstate.str());
				if (!counterConfig)
				{
					CounterCfg::Instance()->RegCounter(jarstate.str().c_str(), COUNTER_CYCLE_NONE);
				}
			
				m_player->SetCounter(jarstate.str().c_str(), EQRECO_JARST_CANNOTOPEN);
				return true;
			}
		public:
			Player* m_player;
		};

		DataEntryVisitor visit(GetOwner());
		EquipRecoveryRewardDataEntryMgr::Instance()->Visit(visit);

		CLProtocol::SceneEquipRecNotifyReset noitfy;
		GetOwner()->SendProtocol(noitfy);

	}
}

void ItemMgr::updateEqRecJarState()
{
	//修改罐子状态
	class DataEntryVisitor : public EqRecoRewardDataEntryVisitor
	{
	public:
		DataEntryVisitor(Player* player, UInt32 score) : m_player(player), m_score(score){}
		~DataEntryVisitor()  {}
		virtual bool operator()(EquipRecoveryRewardDataEntryBase* dataEntry)
		{
			std::ostringstream jarstate;
			jarstate << EQUIP_RECO_REWARD_PER << dataEntry->iD;
			const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(jarstate.str());
			if (!counterConfig)
			{
				CounterCfg::Instance()->RegCounter(jarstate.str().c_str(), COUNTER_CYCLE_NONE);
			}
			UInt32 jarst = m_player->GetCounter(jarstate.str().c_str());
			if (jarst == EQRECO_JARST_CANOPEN && dataEntry->integral > m_score)
			{
				m_player->SetCounter(jarstate.str().c_str(), EQRECO_JARST_CANNOTOPEN);
			}
			else if (jarst == EQRECO_JARST_CANNOTOPEN && dataEntry->integral <= m_score)
			{
				m_player->SetCounter(jarstate.str().c_str(), EQRECO_JARST_CANOPEN);
			}
			return true;
		}
	public:
		Player* m_player;
		UInt32 m_score;
	};

	//DataEntryVisitor visit(GetOwner(), GetOwner()->GetCounter(EQUIP_RECO_SCORE));
	DataEntryVisitor visit(GetOwner(), GetOwner()->GetEqRecScore());
	EquipRecoveryRewardDataEntryMgr::Instance()->Visit(visit);
}

UInt32 ItemMgr::AddMagic(Item* card, Item* item)
{
	if (!card || !item || !card->GetDataEntry() || !item->GetDataEntry())
		return ITEM_DATA_INVALID;

	if (item->IsItemLock(ILT_LEASE_LOCK))
	{
		return ITEM_IN_LEASING;
	}

	if (item->CheckProhibitOp(IPOE_ADDMAGIC))
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "item(guid: " << item->GetID() << "). Prohibit Add Magic " << LogStream::eos;
		return ITEM_PROHIBIT_OPERATE;
	}

	if (card->CheckInvalid()
		|| item->CheckInvalid())
	{
		return ITEM_DATA_INVALID;
	}
	

	MagicCardData* cardData = MagicCardDataMgr::Instance()->FindEntry(card->GetDataID());
	if (!cardData)
	{
		ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")MagicCardData is null! key = " << card->GetDataID() << LogStream::eos;
		GetOwner()->SendNotify("MagicCardData is null! key ={0}", card->GetDataID());
		return ITEM_DATA_INVALID;
	}

	if (item->GetType() != ITEM_EQUIP)
		return ITEM_TYPE_INVALID;

	bool bFitPart = false;
	for (size_t i = 0; i < cardData->parts.size(); ++i)
	{
		if (cardData->parts[i] == item->GetSubType())
		{
			bFitPart = true;
			break;
		}
	}
	if (!bFitPart)
		return ITEM_ADDMAGIC_PART_ERR;
	
	
	if (false == item->SetMagicNew(card))
		return ITEM_DATA_INVALID;

	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_ADD_MAGIC);
	//GetOwner()->RemoveItem(reason.c_str(), card->GetDataEntry()->id, 1);
	GetOwner()->ReduceItemNum(reason.c_str(), card, 1);
	
	if (item->IsItemLock(ILT_AUCTION_RETURN_LOCK))
	{
		item->ClearItemLock(ILT_AUCTION_RETURN_LOCK);
	}

	CalculateValuedScore(item);
	item->SyncProperty(m_pOwner);

	return SUCCESS;
}

UInt32 ItemMgr::ComposeMagicCard(Item* card_1, Item* card_2, UInt32& cardId, UInt32& cousumeBindGold, UInt32& consumeGold, UInt8& cardLev, bool autoUseGold, bool isonekey)
{
	if (!card_1 || !card_2 || !card_1->GetDataEntry() || !card_2->GetDataEntry())
		return ITEM_DATA_INVALID;

	// 同一张卡没有叠加数量,不允许复制
	if (card_1->GetID() == card_2->GetID() &&
		card_1->GetNum() < 2)
	{
		return ITEM_DATA_INVALID;
	}

	if (card_1->CheckInvalid()
		|| card_2->CheckInvalid())
	{
		return ITEM_DATA_INVALID;
	}

	if (card_1->GetQuality() != card_2->GetQuality())
		return ITEM_MAGCARD_COMP_DIF_COLOR;

	MagicCardData* card_1_Data = MagicCardDataMgr ::Instance()->FindEntry(card_1->GetDataID());
	if (!card_1_Data)
	{
		ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")MagicCardData is null! key = " << card_1->GetDataID() << LogStream::eos;
		GetOwner()->SendNotify("MagicCardData is null! key ={0}", card_1->GetDataID());
		return ITEM_DATA_INVALID;
	}

	if (!autoUseGold)
	{
		if (GetOwner()->GetBindGold() < card_1_Data->costNum)
		{
			return ITEM_NOT_ENOUGH_MONEY;
		}
	}
	else
	{
		if (GetOwner()->GetGoldBoth() < card_1_Data->costNum)
		{
			return ITEM_NOT_ENOUGH_MONEY;
		}
	}

	bool isBind = true;
	//if (card_1->GetBind() == card_2->GetBind() && card_1->GetBind() == 1)
	//{
	//	isBind = false;
	//}

	std::vector<MagicCardData*> datas;
	MagicCardDataMgr::Instance()->GetDatasByColorAndBind(card_1->GetQuality(), datas, isBind);
	if (datas.size() == 0)
	{
		ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")MagicCardData size is 0! color = " << card_1->GetQuality() 
			<< ", isBind : " << isBind << ", card_1 : " << card_1->GetDataID() << ", card_2 : " << card_2->GetDataID() << LogStream::eos;
		GetOwner()->SendNotify("MagicCardData size is 0! color = {0}", card_1->GetQuality());
		return ITEM_DATA_INVALID;
	}

	UInt32 compCardId = 0;
	UInt32 sameQualityRate = 0;
	if (card_1->GetQuality() == ITEM_QUALITY_MAX - 1)
		sameQualityRate = 1000;
	else
		sameQualityRate = datas[0]->rate;

	// 随到高一品质的
	if (false == Avalon::Random::SelectInThousand(sameQualityRate))
		MagicCardDataMgr::Instance()->GetDatasByColorAndBind(card_1->GetQuality() + 1, datas, isBind);

	std::vector<UInt32> wts;
	for (auto data : datas)
	{
		wts.push_back(data->weight);
	}
	Int32 idx = ItemMaker::Instance()->randByWeight(wts);
	if (-1 == idx || idx >= (Int32)datas.size())
	{
		ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")wt idx err! idx = " << idx << LogStream::eos;
		GetOwner()->SendNotify("wt idx err! idx = {0}", idx);
		return ITEM_DATA_INVALID;
	}
		
	compCardId = datas[idx]->id;

	if (false == GetOwner()->CheckAddItem(compCardId, 1))
		return ITEM_ADD_PACK_FULL;

	std::string reason = "";
	if (isonekey)
	{
		reason = GetReason(LogSourceType::SOURCE_TYPE_ADD_MAGIC_CARD_ONEKEY);
	}
	else
	{
		reason = GetReason(LogSourceType::SOURCE_TYPE_ADD_MAGIC_CARD);
	}
	
	if (!autoUseGold)
	{
		/*if (!GetOwner()->RemoveItem(reason.c_str(), card_1_Data->costId, card_1_Data->costNum))
		{
			return ITEM_DATA_INVALID;
		}*/
		GetOwner()->RemoveBindGold(reason.c_str(), card_1_Data->costNum);
		cousumeBindGold = card_1_Data->costNum;
	}
	else
	{
		if (!GetOwner()->RemoveGoldBothNew(reason.c_str(), card_1_Data->costNum, cousumeBindGold, consumeGold))
		{
			return ITEM_DATA_INVALID;
		}
	}
	

	if (SUCCESS != GetOwner()->ReduceItemNum(reason.c_str(), card_1, 1))
	{
		return ITEM_DATA_INVALID;
	}

	if (SUCCESS != GetOwner()->ReduceItemNum(reason.c_str(), card_2, 1))
	{
		return ITEM_DATA_INVALID;
	}

	GetOwner()->AddItem(reason.c_str(), compCardId, 1, 0, 0, false);

	cardId = compCardId;
	cardLev = 0;
	return SUCCESS;
}

UInt32 ItemMgr::AddPreciousBead(Item* preciousBead, Item* item)
{
	if (!preciousBead || !item)
	{
		ErrorLogStream << "Item ptr is null!" << LogStream::eos;
		return ITEM_DATA_INVALID;
	}

	if (preciousBead->CheckInvalid() || item->CheckInvalid())
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "PreciousBead(id: " << preciousBead->GetDataID() << ") or item(id: " << item->GetDataID() << ") is invalid." << LogStream::eos;
		return ITEM_DATA_INVALID;
	}

	PreciousBeadDataEntry* dataEntry = PreciousBeadDataEntryMgr::Instance()->FindEntry(preciousBead->GetDataID());
	if (!dataEntry)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "PreciousBeadDataEntry is null! key = " << preciousBead->GetDataID() << LogStream::eos;
		return ITEM_DATA_INVALID;
	}

	if (item->GetType() != ITEM_EQUIP)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "The item(id: " << item->GetDataID() << ") that add jewelry(id: " << preciousBead->GetDataID() << ") to is not equip!" << LogStream::eos;
		return ITEM_TYPE_INVALID;
	}

	bool bFitPart = false;
	for (size_t i = 0; i < dataEntry->parts.size(); ++i)
	{
		if (dataEntry->parts[i] == item->GetSubType())
		{
			bFitPart = true;
			break;
		}
	}
	if (!bFitPart)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "The part(itemSubType: " << item->GetSubType() << " that player want to add jewelry(id: " << preciousBead->GetDataID() << " is error!" << LogStream::eos;
		return ITEM_DATA_INVALID;
	}

	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_JEWELRY, preciousBead->GetDataID(), item->GetDataID());
	if (!GetOwner()->RemoveItem(reason.c_str(), preciousBead->GetDataID(), 1))
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Remove item(id: " << preciousBead->GetDataID() << ") failed." << LogStream::eos;
		return ITEM_DATA_INVALID;
	}

	if (!item->SetPreciousBead(dataEntry))
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Set jewelry failed!" << LogStream::eos;
		return ITEM_DATA_INVALID;
	}

	item->SyncProperty(m_pOwner);
	CalculateValuedScore(item);

	GetOwner()->OnAddPreciousBead();

	return SUCCESS;
}

UInt32 ItemMgr::MountPreciousBead(UInt64 preciousBeadUid, UInt64 itemUid, UInt8  holeIndex, UInt32& preciousBeadId)
{
	Item* item = GetOwner()->FindItem(itemUid);
	if (!item)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find item(guid: " << itemUid << ")." << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	if (item->CheckProhibitOp(IPOE_MOUNTBEAD))
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "item(guid: " << itemUid << "). Prohibit mount bead" << LogStream::eos;
		return ITEM_PROHIBIT_OPERATE;
	}

	//称号镶嵌宝珠,检查称号
	if (item->GetSubType() == ST_EQUIP_TITLE)
	{
		if (1 == item->GetSealState()) //封装状态 
		{
			return ErrorCode::ITEM_MOUNT_TITLE_BEAD_FAIL;
		}
		if (item->GetSealCount() > 0)
		{
			return ErrorCode::ITEM_MOUNT_TITLE_BEAD_FAIL;
		}
	}

	Item* preciousBead = GetOwner()->FindItem(preciousBeadUid);
	if (!preciousBead)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find preciousBead(guid: " << preciousBeadUid << ")." << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	if (item->GetValue(IKVKT_WBP_SPECIAL) == WBS_DOUBLE)
	{
		if (holeIndex == 1)
		{
			auto preciousBeadData = PreciousBeadDataEntryMgr::Instance()->FindEntry(item->GetSecondPreciosBeadId());
			if (preciousBeadData != nullptr)
			{
				for (auto id : preciousBeadData->sameBeadIDVec)
				{
					if (id == preciousBead->GetDataID())
					{
						return ErrorCode::ITEM_PREABEAD_OPEAR_FAIL_MOUNTEDBEAD;
					}
				}
			}
		}
		else if (holeIndex == 2)
		{
			auto preciousBeadData = PreciousBeadDataEntryMgr::Instance()->FindEntry(item->GetFirstPreciosBeadId());
			if (preciousBeadData != nullptr)
			{
				for (auto id : preciousBeadData->sameBeadIDVec)
				{
					if (id == preciousBead->GetDataID())
					{
						return ErrorCode::ITEM_PREABEAD_OPEAR_FAIL_MOUNTEDBEAD;
					}
				}
			}
		}
	}

	preciousBeadId = preciousBead->GetDataID();
	if (holeIndex < 1 || holeIndex > EQ_PREADBEAD_MAXNUM)
	{
		ErrorLogStream << "err holeIndex = " << holeIndex << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}
	PreciousBeadMountHole* pHole = item->GetPreciousBeadHoleProperty().FindHole(holeIndex);
	if (!pHole) return ErrorCode::ITEM_PREABEAD_HOLE_NOTFINND;
	if (pHole->type != PBMHT_COLOUR && preciousBead->GetDataEntry()->precBeadType != pHole->type)
	{
		ErrorLogStream << "type err BeadType: " << preciousBead->GetDataEntry()->precBeadType << "Hole type: " << pHole->type << LogStream::eos;
		return ErrorCode::ITEM_PREABEAD_MOUNTFAIL_TYPEERR;
	}

	if (item->GetPreciousBeadHoleProperty().CheckHavePrecBeadByType(preciousBead->GetThirdType(), holeIndex)) return ErrorCode::ITEM_PREABEAD_EQ_PBTYPE_HAVEED;

	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_PRECBEAD_MOUNT, item->GetID(), (UInt32)holeIndex);
	UInt32 precBeadId = preciousBead->GetDataID();
	UInt32 buffId = preciousBead->GetParam2();
	UInt32 auctionCoolTimeStamp = preciousBead->GetAuctionCoolTimeStamp();
	UInt32 beadExtripeCnt = preciousBead->GetBeadExtripeCnt();
	UInt32 beadReplaceCnt = preciousBead->GetBeadReplaceCnt();
	UInt32 auctionTransTimes = preciousBead->GetAuctionTransNum();

	if (GetOwner()->ReduceItemNum(reason.c_str(), preciousBead, 1) != SUCCESS)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Remove item(id: " << preciousBead->GetDataID() << ") failed." << LogStream::eos;
		return ITEM_DATA_INVALID;
	}
	
	UInt32 oldPreciousBead = pHole->GetPreciousBead();
	item->GetPreciousBeadHoleProperty().SetPreciousBeadId(holeIndex, precBeadId);
	item->GetPreciousBeadHoleProperty().SetBuffId(holeIndex, buffId);
	item->GetPreciousBeadHoleProperty().SetAuctionCoolTimeStamp(holeIndex, auctionCoolTimeStamp);
	item->GetPreciousBeadHoleProperty().SetExtirpeCnt(holeIndex, beadExtripeCnt);
	item->GetPreciousBeadHoleProperty().SetReplaceCnt(holeIndex, beadReplaceCnt);
	item->GetPreciousBeadHoleProperty().SetAuctionTransTimes(holeIndex, auctionTransTimes);

	if (item->IsItemLock(ILT_AUCTION_RETURN_LOCK))
	{
		item->ClearItemLock(ILT_AUCTION_RETURN_LOCK);
	}

	CalculateValuedScore(item);
	item->SyncProperty(GetOwner());

	GetOwner()->OnAddPreciousBead();

	{
		UInt32 precBeadId1 = 0;
		UInt32 precBeadId2 = 0;
		PreciousBeadMountHole* pHole1 = item->GetPreciousBeadHoleProperty().FindHole(1);
		if (pHole1 != NULL)
		{
			precBeadId1 = pHole1->GetPreciousBead();
		}
		PreciousBeadMountHole* pHole2 = item->GetPreciousBeadHoleProperty().FindHole(2);
		if (pHole2 != NULL)
		{
			precBeadId2 = pHole2->GetPreciousBead();
		}

		GetOwner()->SendPreciousBeadUdpLog(PRECIOUS_BEAD_MOUNT, item->GetID(), item->GetDataID(), oldPreciousBead, precBeadId, item->GetQuality(), item->GetSubType(), 0, precBeadId1, precBeadId2);
	}

	return ErrorCode::SUCCESS;
}

UInt32 ItemMgr::ExtirpePreciousBead(UInt64 itemUid, UInt8 holeIndex, UInt32 pestleId)
{
	Item* item = GetOwner()->FindItem(itemUid);
	if (!item)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find item(guid: " << itemUid << ")." << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}
	/*Item* pestle = GetOwner()->FindItem(pestleUid);
	if (!pestle)
	{
	ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find pestle(guid: " << pestleUid << ")." << LogStream::eos;
	return ErrorCode::ITEM_DATA_INVALID;
	}*/

	if (holeIndex < 1 || holeIndex > EQ_PREADBEAD_MAXNUM)
	{
		ErrorLogStream << "err holeIndex = " << holeIndex << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	// 检查时装背包格子
	Package* pkg = GetPackage(PACK_EXPENDABLE);
	if (!pkg)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}
	if (pkg->GetCapacity() < PRECBEAD_UP_NEED_CAP)
	{
		return ErrorCode::ITEM_PACKSIZE_MAX;
	}

	UInt32 preciousBeadId = 0;
	PreciousBeadMountHole* pHole = item->GetPreciousBeadHoleProperty().FindHole(holeIndex);
	if (!pHole) return ErrorCode::ITEM_PREABEAD_HOLE_NOTFINND;

	preciousBeadId = pHole->GetPreciousBead();
	if (preciousBeadId == 0) return ErrorCode::ITEM_PREABEAD_HOLE_NOHAVEED;

	ItemDataEntry* itemDataEntry = ItemDataEntryMgr::Instance()->FindEntry(preciousBeadId);
	if (!itemDataEntry) return ErrorCode::ITEM_DATA_INVALID;

	PreciousBeadDataEntry* precbeadDataEntry = PreciousBeadDataEntryMgr::Instance()->FindEntry(preciousBeadId);
	if (!precbeadDataEntry) return ErrorCode::ITEM_DATA_INVALID;

	PreciousBeadExtirpeDataEntry* dataEntry = PreciousBeadExtirpeDataEntryMgr::Instance()->GetDataEntry(itemDataEntry->color, precbeadDataEntry->level, pestleId, itemDataEntry->precBeadType);
	if (!dataEntry) return ErrorCode::ITEM_DATA_INVALID;

	if (GetOwner()->GetItemNum(pestleId) < dataEntry->materialNum) return ErrorCode::ITEM_PREABEAD_MATERIAL_NOENOUGH;
	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_PRECBEAD_EXTIRPE);

	// 普通摘除
	if (330000211 == pestleId)
	{
		if (dataEntry->pickNum != -1)
		{
			// 扣除宝珠摘除次数
			UInt32 yet_cnt = pHole->GetExtirpeCnt();
			if (yet_cnt >= (UInt32)dataEntry->pickNum)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << "Extirpe PreciousBead Cnt Not Enough:" << pHole->GetPreciousBead() << LogStream::eos;
				return ErrorCode::ITEM_PREABEAD_EXTRIPE_CNT_NOT_ENOUGH;
			}
			else
			{
				item->GetPreciousBeadHoleProperty().SetExtirpeCnt(holeIndex, yet_cnt + 1);
			}
		}

		if (!GetOwner()->RemoveItem(reason.c_str(), pestleId, dataEntry->materialNum))
		{
			item->SyncProperty(GetOwner());
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Remove pestle(id: " << pestleId << ", num: " << dataEntry->materialNum << ") failed." << LogStream::eos;
			return ErrorCode::ITEM_DATA_INVALID;
		}

		bool bExtirpe = Avalon::Random::SelectInTenThousand(dataEntry->rate);
		if (!bExtirpe)
		{
			item->SyncProperty(GetOwner());

			InfoLogStream << PLAYERINFO(GetOwner()) << "Extirpe PreciousBead fail " << LogStream::eos;
			GetOwner()->SendPreciousBeadUdpLog(PRECIOUS_BEAD_EXTRIPE, item->GetID(), item->GetDataID(), preciousBeadId, 0, pestleId, dataEntry->materialNum, 0, 0, 0);

			return ErrorCode::ITEM_PREABEAD_EXTRIPE_FAIL;
		}
	}
	else if (330000212 == pestleId)	// 完美摘除，必定成功
	{
		if (!GetOwner()->RemoveItem(reason.c_str(), pestleId, dataEntry->materialNum))
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Remove pestle(id: " << pestleId << ", num: " << dataEntry->materialNum << ") failed." << LogStream::eos;
			return ErrorCode::ITEM_DATA_INVALID;
		}
	}

	/*RewardGroup* grp = GetOwner()->GetRewardGroup();
	grp->AddReward(preciousBeadId, 1);
	GetOwner()->AddRewards(reason.c_str(), grp, true);*/
	Item* precbeadItem = ItemMaker::Instance()->MakeItem(GetOwner(), preciousBeadId, 1);
	if (precbeadItem == NULL)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " MakeItem item fail! datatId = " << preciousBeadId << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}
	UInt32 buffId = item->GetPreciousBeadHoleProperty().GetBuffId(holeIndex);
	UInt32 auctionCoolTimeStamp = item->GetPreciousBeadHoleProperty().GetAuctionCoolTimeStamp(holeIndex);
	if (buffId > 0)
	{
		precbeadItem->SetParam2(buffId);
	}
	if (auctionCoolTimeStamp > 0)
	{
		precbeadItem->SetAuctionCoolTimeStamp(auctionCoolTimeStamp);
	}

	precbeadItem->SetBeadExtripeCnt(pHole->GetExtirpeCnt());
	precbeadItem->SetBeadReplaceCnt(pHole->GetReplaceCnt());

	UInt32 auctionTransTimes = item->GetPreciousBeadHoleProperty().GetAuctionTransTimes(holeIndex);
	precbeadItem->SetAuctionTransNum(auctionTransTimes);

	GetOwner()->AddItemDirect(reason.c_str(), precbeadItem);

	item->GetPreciousBeadHoleProperty().SetPreciousBeadId(holeIndex, 0);
	item->GetPreciousBeadHoleProperty().SetBuffId(holeIndex, 0);
	item->GetPreciousBeadHoleProperty().SetAuctionCoolTimeStamp(holeIndex, 0);
	item->GetPreciousBeadHoleProperty().SetExtirpeCnt(holeIndex, 0);
	item->GetPreciousBeadHoleProperty().SetReplaceCnt(holeIndex, 0);
	item->GetPreciousBeadHoleProperty().SetAuctionTransTimes(holeIndex, 0);

	CalculateValuedScore(item);
	item->SyncProperty(GetOwner());

	GetOwner()->SendPreciousBeadUdpLog(PRECIOUS_BEAD_EXTRIPE, item->GetID(), item->GetDataID(), preciousBeadId, 0, pestleId, dataEntry->materialNum, 1, 0, 0);

	return ErrorCode::SUCCESS;
}

UInt32 ItemMgr::UpgradePreciousBead(UInt8 mountedType, ObjID_t preciousBeadUid, ObjID_t eqUid,
	UInt8 eqPrecHoleIndex, UInt32 precId, UInt32 consumePrecId,
	UInt32& upgradePrecId, UInt32& addBuffId, ObjID_t& newPrebeadUid)
{
	upgradePrecId = 0;
	addBuffId = 0;
	newPrebeadUid = 0;
	if (mountedType == UP_PRECBEAD_UNMOUNTED)  //升级未镶嵌的宝珠
	{
		// 检查时装背包格子
		Package* pkg = GetPackage(PACK_EXPENDABLE);
		if (!pkg)
		{
			return ErrorCode::ITEM_DATA_INVALID;
		}
		if (pkg->GetCapacity() < PRECBEAD_UP_NEED_CAP)
		{
			return ErrorCode::ITEM_PACKSIZE_MAX;
		}

		Item* preciousBead = GetOwner()->FindItem(preciousBeadUid);
		if (!preciousBead)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find preciousBead(guid: " << preciousBeadUid << ")." << LogStream::eos;
			return ErrorCode::ITEM_DATA_INVALID;
		}
		PreciousBeadDataEntry* dataEntry = PreciousBeadDataEntryMgr::Instance()->FindEntry(preciousBead->GetDataID());
		if (dataEntry == NULL)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "dataEntry of preciousBead is NULL  (dataId:" << preciousBead->GetDataID() << ")." << LogStream::eos;
			return ErrorCode::ITEM_DATA_INVALID;
		}
		if (dataEntry->nextLevPrecbeadID == 0)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "nextLevPrecbeadID of preciousBead dataEntry is NULL  (dataId:" << preciousBead->GetDataID() << ")." << LogStream::eos;
			return ErrorCode::ITEM_DATA_INVALID;
		}
		PreciousBeadDataEntry* nextLevDataEntry = PreciousBeadDataEntryMgr::Instance()->FindEntry(dataEntry->nextLevPrecbeadID);
		if (nextLevDataEntry == NULL)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "dataEntry of preciousBead is NULL  (dataId:" << dataEntry->nextLevPrecbeadID << ")." << LogStream::eos;
			return ErrorCode::ITEM_DATA_INVALID;
		}
		bool findConsume = false;
		ItemReward itemConsume;
		//InfoLogStream << PLAYERINFO(GetOwner()) << "UpgradePreciousBead (dataId:" << preciousBead->GetDataID() << "dataEntry->consumeItem size = " << dataEntry->consumeItem.size() << LogStream::eos;
		for (auto item : dataEntry->consumeItem)
		{
			//InfoLogStream << PLAYERINFO(GetOwner()) << "item.id = " << item.id << "consumePrecId = " << consumePrecId << LogStream::eos;
			if (item.id == consumePrecId)
			{
				itemConsume = item;
				findConsume = true;
				break;
			}
		}
		if (!findConsume)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "can not find upLevel consume item  (dataId:" << preciousBead->GetDataID() << "), (consumePrecId:" << consumePrecId << ")" << LogStream::eos;
			return ErrorCode::ITEM_DATA_INVALID;
		}

		std::string reason = GetReason(LogSourceType::SOURCE_PRECIOUSBEAD_UPLEVEL, mountedType);
		//消耗数量检查
		if (itemConsume.id == preciousBead->GetDataID())
		{
			if (this->GetOwner()->GetItemNum(itemConsume.id)  < itemConsume.num + 1)
			{
				return ErrorCode::ITEM_PREABEAD_UPLEVFAIL_MATER_INSUFF;
			}
		}
		else if (this->GetOwner()->GetItemNum(itemConsume.id) < itemConsume.num)
		{
			return ErrorCode::ITEM_PREABEAD_UPLEVFAIL_MATER_INSUFF;
		}

		//扣物品
		if (SUCCESS != GetOwner()->ReduceItemNum(reason.c_str(), preciousBead, 1) ||
			!GetOwner()->RemoveItem(reason.c_str(), itemConsume.id, itemConsume.num))
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " remove item error!" << LogStream::eos;
			return ErrorCode::ITEM_DATA_INVALID;
		}
		//发放新宝珠
		/*RewardGroup* grp = GetOwner()->GetRewardGroup();
		grp->AddReward(dataEntry->nextLevPrecbeadID, 1);
		GetOwner()->AddRewards(reason.c_str(), grp, true);*/
		UInt32 buffId = 0;
		Item* item = ItemMaker::Instance()->MakeItem(GetOwner(), dataEntry->nextLevPrecbeadID, 1);
		if (item == NULL)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " MakeItem item fail! datatId = " << dataEntry->nextLevPrecbeadID << LogStream::eos;
			return ErrorCode::ITEM_DATA_INVALID;
		}
		if (nextLevDataEntry->addtionBuffPro > 0 && nextLevDataEntry->buffRandomGroupId > 0)
		{
			bool baddtionBuffPro = Avalon::Random::SelectInTenThousand(nextLevDataEntry->addtionBuffPro);
			if (baddtionBuffPro)
			{
				buffId = PrecBeadAddBuffRandomDataEntryMgr::Instance()->RandomBuffIdByBuffGroupId(nextLevDataEntry->buffRandomGroupId);
				item->SetParam2(buffId);
				addBuffId = buffId;
			}
		}
		item->SetAuctionCoolTimeStamp(0);
		GetOwner()->AddItemDirect(reason.c_str(), item, false);
		upgradePrecId = dataEntry->nextLevPrecbeadID;
		newPrebeadUid = item->GetID();

		GetOwner()->SendPreciousBeadUdpLog(PRECIOUS_BEAD_UPGRADE, 0, 0, dataEntry->itemId, dataEntry->nextLevPrecbeadID, itemConsume.id, itemConsume.num, 0, dataEntry->color, buffId );
	}
	else if (mountedType == UP_PRECBEAD_MOUNTED)  //升级已镶嵌的宝珠
	{
		Item* item = GetOwner()->FindItem(eqUid);
		if (!item)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find item(guid: " << eqUid << ")." << LogStream::eos;
			return ErrorCode::ITEM_DATA_INVALID;
		}
		if (eqPrecHoleIndex < 1 || eqPrecHoleIndex > EQ_PREADBEAD_MAXNUM)
		{
			ErrorLogStream << "err eqPrecHoleIndex = " << eqPrecHoleIndex << LogStream::eos;
			return ErrorCode::ITEM_DATA_INVALID;
		}
		PreciousBeadMountHole* pHole = item->GetPreciousBeadHoleProperty().FindHole(eqPrecHoleIndex);
		if (!pHole) return ErrorCode::ITEM_PREABEAD_HOLE_NOTFINND;

		if (pHole->GetPreciousBead() != precId)
		{
			return ErrorCode::ITEM_DATA_INVALID;
		}
		PreciousBeadDataEntry* dataEntry = PreciousBeadDataEntryMgr::Instance()->FindEntry(precId);
		if (dataEntry == NULL)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "dataEntry of preciousBead is NULL  (dataId:" << precId << ")." << LogStream::eos;
			return ErrorCode::ITEM_DATA_INVALID;
		}
		if (dataEntry->nextLevPrecbeadID == 0)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "nextLevPrecbeadID of preciousBead dataEntry is NULL  (dataId:" << precId << ")." << LogStream::eos;
			return ErrorCode::ITEM_DATA_INVALID;
		}
		PreciousBeadDataEntry* nextLevDataEntry = PreciousBeadDataEntryMgr::Instance()->FindEntry(dataEntry->nextLevPrecbeadID);
		if (nextLevDataEntry == NULL)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "dataEntry of preciousBead is NULL  (dataId:" << dataEntry->nextLevPrecbeadID << ")." << LogStream::eos;
			return ErrorCode::ITEM_DATA_INVALID;
		}
		bool findConsume = false;
		ItemReward itemConsume;
		for (auto item : dataEntry->consumeItem)
		{
			if (item.id == consumePrecId)
			{
				itemConsume = item;
				findConsume = true;
				break;
			}
		}
		if (!findConsume)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "can not find upLevel consume item  (dataId:" << precId << "), (consumePrecId:" << consumePrecId << LogStream::eos;
			return ErrorCode::ITEM_DATA_INVALID;
		}
		std::string reason = GetReason(LogSourceType::SOURCE_PRECIOUSBEAD_UPLEVEL, mountedType);
		//消耗数量检查
		if (this->GetOwner()->GetItemNum(itemConsume.id) < itemConsume.num)
		{
			return ErrorCode::ITEM_PREABEAD_UPLEVFAIL_MATER_INSUFF;
		}
		//扣物品
		if (!GetOwner()->RemoveItem(reason.c_str(), itemConsume.id, itemConsume.num))
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " remove item error!" << LogStream::eos;
			return ErrorCode::ITEM_DATA_INVALID;
		}
		//修改镶嵌的宝珠id
		UInt32 buffId = 0;
		item->GetPreciousBeadHoleProperty().SetPreciousBeadId(eqPrecHoleIndex, dataEntry->nextLevPrecbeadID);
		if (nextLevDataEntry->addtionBuffPro > 0 && nextLevDataEntry->buffRandomGroupId > 0)
		{
			bool baddtionBuffPro = Avalon::Random::SelectInTenThousand(nextLevDataEntry->addtionBuffPro);
			if (baddtionBuffPro)
			{
				buffId = PrecBeadAddBuffRandomDataEntryMgr::Instance()->RandomBuffIdByBuffGroupId(nextLevDataEntry->buffRandomGroupId);
				item->GetPreciousBeadHoleProperty().SetBuffId(eqPrecHoleIndex, buffId);
				addBuffId = buffId;
			}
		}

		item->GetPreciousBeadHoleProperty().SetExtirpeCnt(eqPrecHoleIndex, 0);
		item->GetPreciousBeadHoleProperty().SetReplaceCnt(eqPrecHoleIndex, 0);

		CalculateValuedScore(item);
		item->SyncProperty(GetOwner());
		
		GetOwner()->OnAddPreciousBead();
		upgradePrecId = dataEntry->nextLevPrecbeadID;

		GetOwner()->SendPreciousBeadUdpLog(PRECIOUS_BEAD_UPGRADE, 0, 0, dataEntry->itemId, dataEntry->nextLevPrecbeadID, itemConsume.id, itemConsume.num, 0, dataEntry->color, buffId);
	}
	else
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}
	return ErrorCode::SUCCESS;
}

UInt32 ItemMgr::ReplacePreciousBead(UInt64 preciousBeadUid, UInt64 itemUid, UInt8 holeIndex, UInt32& preciousBeadId)
{
	Item* item = GetOwner()->FindItem(itemUid);
	if (NULL == item)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find item(guid: " << itemUid << ")." << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	Item* preciousBead = GetOwner()->FindItem(preciousBeadUid);
	if (NULL == preciousBead)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find preciousBead(guid: " << preciousBeadUid << ")." << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	preciousBeadId = preciousBead->GetDataID();
	if (holeIndex < 1 || holeIndex > EQ_PREADBEAD_MAXNUM)
	{
		ErrorLogStream << "err holeIndex = " << holeIndex << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	PreciousBeadMountHole* pHole = item->GetPreciousBeadHoleProperty().FindHole(holeIndex);
	if (!pHole) return ErrorCode::ITEM_PREABEAD_HOLE_NOTFINND;

	if (pHole->type != PBMHT_COLOUR && preciousBead->GetDataEntry()->precBeadType != pHole->type)
	{
		ErrorLogStream << "type err BeadType: " << preciousBead->GetDataEntry()->precBeadType << "Hole type: " << pHole->type << LogStream::eos;
		return ErrorCode::ITEM_PREABEAD_MOUNTFAIL_TYPEERR;
	}

	ItemDataEntry* oldItemDataEntry = ItemDataEntryMgr::Instance()->FindEntry(pHole->GetPreciousBead());
	if (!oldItemDataEntry) return ErrorCode::ITEM_DATA_INVALID;

	PreciousBeadDataEntry* oldBeadDataEntry = PreciousBeadDataEntryMgr::Instance()->FindEntry(pHole->GetPreciousBead());
	if (!oldBeadDataEntry) return ErrorCode::ITEM_DATA_INVALID;

	if (item->GetPreciousBeadHoleProperty().CheckHavePrecBeadByType(preciousBead->GetThirdType(), holeIndex)) return ErrorCode::ITEM_PREABEAD_EQ_PBTYPE_HAVEED;

	PreciousBeadReplaceDataEntry* beadReplaceEntry = PreciousBeadReplaceDataEntryMgr::Instance()->GetDataEntry(oldItemDataEntry->color, oldBeadDataEntry->level, oldItemDataEntry->precBeadType);
	if (NULL == beadReplaceEntry)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Can not find beadReplaceEntry(guid: " << preciousBeadUid << ")." << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	// 不允许置换
	if (beadReplaceEntry->replaceNum == 0)
		return ErrorCode::ITEM_PREABEAD_REPLACE_CNT_NOT_ENOUGH;

	// 检查钱够不够
	if (GetOwner()->GetItemNum(beadReplaceEntry->costItemId) < beadReplaceEntry->costItemNum)
	{
		return ErrorCode::ITEM_PREABEAD_REPLACE_ITEM_NOT_ENOUGH;
	}

	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_PRECBEAD_REPLACE);
	if (beadReplaceEntry->replaceNum != -1)
	{
		if ( !oldBeadDataEntry->IsCanReplace(preciousBeadId))
		{
			return ErrorCode::ITEM_PREABEAD_REPLACE_FAILED;
		}

		UInt32 yet_cnt = pHole->GetReplaceCnt();
		if (yet_cnt >= (UInt32)beadReplaceEntry->replaceNum)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "bead replace cnt not enough (guid: " << preciousBeadUid << ")." << LogStream::eos;
			return ErrorCode::ITEM_PREABEAD_REPLACE_CNT_NOT_ENOUGH;
		}

		pHole->SetReplaceCnt(yet_cnt + 1);
	}

	if (!GetOwner()->RemoveItem(reason.c_str(), beadReplaceEntry->costItemId, beadReplaceEntry->costItemNum))
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "BeadReplace Cost (id: " << beadReplaceEntry->costItemId << ", num: "
			<< beadReplaceEntry->costItemNum << ") failed." << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	// 摘下旧宝珠
	{
		Item* precbeadItem = ItemMaker::Instance()->MakeItem(GetOwner(), pHole->GetPreciousBead(), 1);
		if (precbeadItem == NULL)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " MakeItem item fail! datatId = " << preciousBeadId << LogStream::eos;
			return ErrorCode::ITEM_DATA_INVALID;
		}

		precbeadItem->SetBeadExtripeCnt(pHole->GetExtirpeCnt());
		precbeadItem->SetBeadReplaceCnt(pHole->GetReplaceCnt());

		if (pHole->GetBuffId() > 0)
			precbeadItem->SetParam2(pHole->GetBuffId());

		if (pHole->GetAuctionCoolTimeStamp() > 0)
			precbeadItem->SetAuctionCoolTimeStamp(pHole->GetAuctionCoolTimeStamp());

		if (pHole->GetAuctionTransTimes() > 0)
			precbeadItem->SetAuctionTransNum(pHole->GetAuctionTransTimes());

		GetOwner()->AddItemDirect(reason.c_str(), precbeadItem);
	}
	
	// 镶嵌上新的宝珠
	{
		UInt32 precBeadId = preciousBead->GetDataID();
		UInt32 buffId = preciousBead->GetParam2();
		UInt32 auctionCoolTimeStamp = preciousBead->GetAuctionCoolTimeStamp();
		UInt32 beadExtripeCnt = preciousBead->GetBeadExtripeCnt();
		UInt32 beadReplaceCnt = preciousBead->GetBeadReplaceCnt();
		UInt32 auctionTransTimes = preciousBead->GetAuctionTransNum();

		if (GetOwner()->ReduceItemNum(reason.c_str(), preciousBead, 1) != SUCCESS)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Remove item(id: " << preciousBead->GetDataID() << ") failed." << LogStream::eos;
			return ITEM_DATA_INVALID;
		}

		item->GetPreciousBeadHoleProperty().SetPreciousBeadId(holeIndex, precBeadId);
		item->GetPreciousBeadHoleProperty().SetBuffId(holeIndex, buffId);
		item->GetPreciousBeadHoleProperty().SetAuctionCoolTimeStamp(holeIndex, auctionCoolTimeStamp);
		item->GetPreciousBeadHoleProperty().SetExtirpeCnt(holeIndex, beadExtripeCnt);
		item->GetPreciousBeadHoleProperty().SetReplaceCnt(holeIndex, beadReplaceCnt);
		item->GetPreciousBeadHoleProperty().SetAuctionTransTimes(holeIndex, auctionTransTimes);

		CalculateValuedScore(item);
		item->SyncProperty(GetOwner());
	}
	
	GetOwner()->SendPreciousBeadUdpLog(PRECIOUS_BEAD_REPLACE, item->GetID(), item->GetDataID(), oldItemDataEntry->id, preciousBeadId, 0, 0, 0, 0, 0);

	GetOwner()->OnAddPreciousBead();

	return ErrorCode::SUCCESS;
}

void ItemMgr::BeadConvert(UInt64 beadGuid, UInt64 materialGuid)
{
	CLProtocol::SceneBeadConvertRes res;

	do 
	{
		// 检查背包格子
		Package* pkg = GetPackage(PACK_EXPENDABLE);
		if (!pkg)
		{
			res.retCode = ErrorCode::ITEM_DATA_INVALID;
			break;
		}
		if (pkg->GetCapacity() < 1)
		{
			res.retCode = ErrorCode::ITEM_PACKSIZE_MAX;
			break;
		}

		Item* item = FindItem(beadGuid);
		if (NULL == item || NULL == item->GetDataEntry() || item->GetSubType() != ST_BEAD)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " beadGuid item error! beadGuid:" << beadGuid << LogStream::eos;
			res.retCode = ErrorCode::ITEM_DATA_INVALID;
			break;
		}

		if (beadGuid == materialGuid && item->GetNum() < 2)
		{
			ErrorLogStream << "bead material guid same error!" << LogStream::eos;
			return;
		}

		UInt32 convertItemId = item->GetDataID();
		UInt32 materialItemId = 0;
		std::string reason = GetReason(LogSourceType::SOURCE_TYPE_PRECBEAD_CONVERT);

		const BeadConvertCostDataEntry* costEntry = BeadConvertCostDataEntryMgr::Instance()->FindEntry(item->GetDataEntry()->color);
		if (NULL == costEntry)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " cost not find error! color:" << item->GetDataEntry()->color << LogStream::eos;
			res.retCode = ErrorCode::ITEM_DATA_INVALID;
			break;
		}

		UInt32 dropId = 0;

		// 消耗金币转换
		if (materialGuid == 0)
		{
			// 检查钱够不够
			if (GetOwner()->GetItemNum(costEntry->moneyCostId) < costEntry->moneyCostNum)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << " checkCoin error! coin:" << costEntry->moneyCostNum << LogStream::eos;
				res.retCode = ErrorCode::ITEM_DATA_INVALID;
				break;
			}

			if ( !GetOwner()->RemoveItem(reason.c_str(), costEntry->moneyCostId, costEntry->moneyCostNum))
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << " costCoin error! coin:" << costEntry->moneyCostNum << LogStream::eos;
				res.retCode = ErrorCode::ITEM_DATA_INVALID;
				break;
			}

			dropId = item->GetBind() == 1 ? costEntry->dropId : costEntry->bindDropId;
		}
		else
		{	// 消耗宝珠转换
			Item* materItem = FindItem(materialGuid);
			if (NULL == materItem || materItem->GetSubType() != ST_BEAD || materItem->GetDataEntry() == NULL)
			{
				res.retCode = ErrorCode::ITEM_DATA_INVALID;
				break;
			}

			if (materItem->GetDataEntry()->color != costEntry->beadCost)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << " costColor error! color:" << costEntry->beadCost << LogStream::eos;
				res.retCode = ErrorCode::ITEM_DATA_INVALID;
				break;
			}

			// 检查材料够不够
			if (materItem->GetNum() < costEntry->beadCostNum)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << " checkBead error! coin:" << costEntry->beadCostNum << LogStream::eos;
				res.retCode = ErrorCode::ITEM_DATA_INVALID;
				break;
			}

			// 扣除材料宝珠
			if (SUCCESS != GetOwner()->ReduceItemNum(reason.c_str(), materItem, costEntry->beadCostNum))
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << " remove material item error! id:" << materItem->GetDataID() << LogStream::eos;
				res.retCode = ErrorCode::ITEM_DATA_INVALID;
				break;
			}

			materialItemId = materItem->GetDataID();

			if (item->GetBind() == 1 && materItem->GetBind() == 1)
			{
				dropId = costEntry->dropId;
			}
			else
			{
				dropId = costEntry->bindDropId;
			}
		}

		// 扣掉原有的宝珠
		if (SUCCESS != GetOwner()->ReduceItemNum(reason.c_str(), item, 1))
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " remove bead item error! id:" << item->GetDataID() << LogStream::eos;
			res.retCode = ErrorCode::ITEM_DATA_INVALID;
			break;
		}
	
		// 发放奖励
		UInt32 rewardId = 0;
		UInt32 rewardNum = 0;
		BeadConvertDropDataEntryMgr::Instance()->GetConvertReward(dropId, rewardId, rewardNum);

		if (rewardId == 0 || rewardNum == 0)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " reward error! id:" << beadGuid << " material:" << materialGuid
				<< " dropId:" << dropId << LogStream::eos;
			res.retCode = ErrorCode::ITEM_DATA_INVALID;
			break;
		}

		InfoLogStream << PLAYERINFO(GetOwner()) <<"bead convert success convertItemId:" << convertItemId << " materialItemId:" << materialItemId 
			<< " get reward itemId:" << rewardId << LogStream::eos;

		if (GetOwner()->AddItem(reason.c_str(), rewardId, rewardNum) == 0)
		{
			InfoLogStream << PLAYERINFO(GetOwner()) << "bead convert failed convertItemId:" << convertItemId << " materialItemId:" << materialItemId
				<< " get reward itemId:" << rewardId << LogStream::eos;
			res.retCode = ErrorCode::ITEM_ADD_PACK_FULL;
			break;
		}

		// 消耗宝珠转换
		if (materialGuid != 0)
		{
			GetOwner()->SendPreciousBeadUdpLog(PRECIOUS_BEAD_CONVERT, 0, 1, convertItemId, rewardId, materialItemId, costEntry->beadCostNum, 1, item->GetDataEntry()->color, item->GetDataEntry()->color);
		}
		else
		{
			GetOwner()->SendPreciousBeadUdpLog(PRECIOUS_BEAD_CONVERT, 0, 0, convertItemId, rewardId, costEntry->moneyCostId, costEntry->moneyCostNum, 1, item->GetDataEntry()->color, item->GetDataEntry()->color);
		}

		res.retCode = ErrorCode::SUCCESS;

	} while (0);

	GetOwner()->SendProtocol(res);
}

UInt32 ItemMgr::FashionCompose(Item* fashionA, Item* fashionB)
{
	if (!fashionA)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "fashionA is null!" << LogStream::eos;
		return ITEM_DATA_INVALID;
	}

	if (!fashionB)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "fashionB is null!" << LogStream::eos;
		return ITEM_DATA_INVALID;
	}

	if (fashionA->CheckInvalid()
		|| fashionB->CheckInvalid())
	{
		return ITEM_DATA_INVALID;
	}

	//判断是不是时装
	if (fashionA->GetType() != ITEM_FASHION)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "fashionA is not FASHION!" << LogStream::eos;
		return ITEM_DATA_INVALID;
	}
	if (fashionB->GetType() != ITEM_FASHION)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "fashionB is not FASHION!" << LogStream::eos;
		return ITEM_DATA_INVALID;
	}

	//判断是不是同一部位
	if (fashionA->GetSubType() != fashionB->GetSubType())
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "fashionA part is not same as fashionB!" << LogStream::eos;
		return ITEM_DATA_INVALID;
	}

	//判断有没有合成器
	UInt32 matId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_FASHION_COMPOSE_MATID);
	if (GetItemNum(matId, ITEM_MATERIAL) == 0)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "not enough COMPOSER!" << LogStream::eos;
		return ITEM_NOT_ENOUGH_MAT;
	}

	//开始合成
	UInt32 srcFashionId = fashionA->GetDataEntry()->color < fashionB->GetDataEntry()->color ? fashionA->GetDataID() : fashionB->GetDataID();
	FashionComposeData* data = FashionComposeMgr::Instance()->FindEntry(srcFashionId);
	if (!data)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "srcFashionId = " << srcFashionId << " can not find match data!" << LogStream::eos;
		return ITEM_DATA_INVALID;
	}

	FashionComposeKey key(data->composeColor, GetOwner()->GetMainOccu(), fashionA->GetSubType());
	FashionComposeMgr::FashionVec datas;
	if (false == FashionComposeMgr::Instance()->GetFahionDatas(key, datas))
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "not find match datas color = " << data->composeColor << " occu = " << GetOwner()->GetMainOccu() << " part = " << fashionA->GetSubType() << LogStream::eos;
		return ITEM_DATA_INVALID;
	}
	FashionComposeData* commonFashion = FashionRand(datas);
	if (!commonFashion)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "FashionRand is no result!" << LogStream::eos;
		return ITEM_DATA_INVALID;
	}

	FashionComposeSkyData* skyFashion = FashionSkyCompose(GetOwner()->GetMainOccu(), fashionA->GetSubType());

	//扣材料加道具
	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_FASHION_COMPOSE);

	GetOwner()->RemoveItem(reason.c_str(), matId, 1);
	GetOwner()->ReduceItemNum(reason.c_str(), fashionA, 1);
	GetOwner()->ReduceItemNum(reason.c_str(), fashionB, 1);

	RewardGroup* grp = GetOwner()->GetRewardGroup();
	grp->AddReward(commonFashion->id, 1);
	if (skyFashion)
	{
		grp->AddReward(skyFashion->id, 1);
	}

	GetOwner()->AddRewards(reason.c_str(), grp, false);
	GetOwner()->SendFashionComposeBack(SUCCESS, !skyFashion ? 1 : 2, commonFashion->id, !skyFashion ? 0 : skyFashion->id, 0);

	GetOwner()->SendFashionComposeUdpLog(fashionA->GetDataID(), fashionB->GetDataID(), commonFashion->id, !skyFashion ? 0 : skyFashion->id);

	return SUCCESS;
}

FashionComposeData* ItemMgr::FashionRand(FashionComposeMgr::FashionVec& datas)
{
	if (datas.size() == 0)
	{
		return NULL;
	}

	Int32 totWt = 0;
	for (size_t i = 0; i < datas.size(); ++i){
		if (!datas[i])	continue;
		totWt += datas[i]->weight;
	}

	Int32 randWt = Avalon::Random::RandBetween(1, totWt);
	Int32 sumWt = 0;
	
	auto itr = datas.begin();
	for (; itr != datas.end(); ++itr)
	{
		FashionComposeData* info = *itr;
		if (!info)
		{
			++itr;
			continue;
		}

		sumWt += info->weight;
		//抽到
		if (randWt <= sumWt)
		{
			return info;
		}
	}

	return NULL;
}

FashionComposeSkyData* ItemMgr::FashionSkyCompose(UInt8 occu, UInt8 part)
{
	FashionComposeSkyKey key(occu, part, 1);
	FashionComposeSkyMgr::FashionSkyVec datas;
	FashionComposeSkyMgr::Instance()->GetFahionSkyDatas(key, datas);

	std::string wtCountName = FASHION_WEIGHT_PRE + Avalon::StringUtil::ConvertToString(part);
	std::string cpCountName = FASHION_COMPOSE_PRE + Avalon::StringUtil::ConvertToString(part);

	UInt32 composeNum = GetOwner()->GetCounter(cpCountName.c_str());
	UInt32 commonWt = GetOwner()->GetCounter(wtCountName.c_str());

	for (size_t i = 0; i < datas.size(); ++i)
	{
		FashionComposeSkyData* data = datas[i];
		if (!data)
		{
			return NULL;
		}

		if (commonWt == 0)
		{
			commonWt += data->baseWeight;
		}

		if (composeNum < data->hitMin - 1)
		{
			GetOwner()->IncCounter(cpCountName.c_str(), 1);
			continue;
		}

		//必出
		if (commonWt >= data->hitWeight)
		{
			GetOwner()->SetCounter(wtCountName.c_str(), 0);
			GetOwner()->SetCounter(cpCountName.c_str(), 0);
			return data;
		}
		else //真实随机
		{
			if (FashionSkyRand(data, commonWt))
			{
				GetOwner()->SetCounter(wtCountName.c_str(), 0);
				GetOwner()->SetCounter(cpCountName.c_str(), 0);
				return data;
			}
			else
			{
				GetOwner()->SetCounter(wtCountName.c_str(), commonWt);
				GetOwner()->IncCounter(cpCountName.c_str(), 1);
				return NULL;
			}
				
		}
	}
	
	return NULL;
}

bool ItemMgr::FashionSkyRand(FashionComposeSkyData* data, UInt32& hitWt)
{
	if (!data)
	{
		return false;
	}

	Int32 randWt = Avalon::Random::RandBetween(1, data->maxWeight);

	//命中
	if (randWt <= (Int32)hitWt)
	{
		return true;
	}
	else
	{
		Int32 fixWt = Avalon::Random::RandBetween(data->fixRandRange[0], data->fixRandRange[1]);
		hitWt += fixWt;
		return false;
	}
}

UInt32 ItemMgr::FashionAttrSel(Item* fashion, UInt32 attrid)
{
	if (!fashion)
	{
		return ITEM_DATA_INVALID;
	}

	if (fashion->CheckInvalid())
	{
		return ITEM_DATA_INVALID;
	}

	FashionAttrData* data = FashionAttrDataMgr::Instance()->FindEntry(fashion->GetEqPropId());
	if (!data)
	{
		return ITEM_DATA_INVALID;
	}

	bool find = false;
	for (size_t i = 0; i < data->attrs.size(); ++i)
	{
		if (data->attrs[i] == attrid)
		{
			find = true;
			break;
		}
	}

	if (!find)
	{
		return ITEM_DATA_INVALID;
	}

	if (data->freeSelNum <= fashion->GetFashionAttrChangeNum())
	{
		//没有免费次数,检查所需道具
		UInt32 matId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_FASHION_ATTRIBUTE_RESET_COST_ITEM_ID);
		if (0 == GetOwner()->GetItemNum(matId))
		{
			return ITEM_NOT_ENOUGH_MAT;
		}

		std::string reason = GetReason(LogSourceType::SOURCE_TYPE_FASHION_SEL_ATTR);
		GetOwner()->RemoveItem(reason.c_str(), matId, 1);
	}

	fashion->SetFashionAttrId(attrid);
	fashion->SyncProperty(GetOwner());

	return SUCCESS;
}

UInt32 ItemMgr::FashionComposeNew(UInt32 suitId, UInt32 selComFashionId, UInt64 fashionAId, UInt64 fashionBId, UInt64 composerId)
{
	if (!_IsFashionCmpsOpen())
	{
		return ErrorCode::FASHION_CMPS_NOT_OPEN;
	}

	// 检查参数
	Item* fashionA = FindItem(fashionAId);
	Item* fashionB = FindItem(fashionBId);
	Item* composer = FindItem(composerId);

	UInt32 ret = _CheckFashionCmpsParam(fashionA, fashionB, composer, selComFashionId);
	if (ErrorCode::SUCCESS != ret)
	{
		return ret;
	}

	// 检查时装背包格子
	ret = _CheckFashionCmpsCapacity();
	if (ErrorCode::SUCCESS != ret)
	{
		return ret;
	}

	bool isL = false;

	std::vector<UInt32> tmpSuffix;
	tmpSuffix.clear();
	tmpSuffix.push_back((UInt32)(fashionA->GetSubType()));
	FashionComposeSkyData::SkyType type = (FashionComposeSkyData::SkyType)(GetOwner()->GetCounterMgr().GetCounterDynamic(GET_SKY_FASHION_TYPE, tmpSuffix));
	if (FashionComposeSkyData::SkyType::ST_NONE == type) // L
	{
		isL = true;
	}
	// 生成天空套概率
	UInt32 rate = 0;
	UInt32 prob = 0;
	ret = _CreateFahsionCmpsRate(fashionA, fashionB, composer, rate, prob);
	if (ErrorCode::SUCCESS != ret)
	{
		return ret;
	}

	// 得到合成结果
	UInt32 getSkyFashionId = 0;
	UInt32 windId = 0;

	DebugLogStream << PLAYERINFO(GetOwner()) << " rate:" << rate << " prob:" << prob << LogStream::eos;


	ret = _GetFashionCmpsItem(rate * prob, suitId, selComFashionId, fashionA->GetSubType(), getSkyFashionId, isL);
	if (ErrorCode::SUCCESS != ret)
	{
		return ret;
	}

	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_FASHION_COMPOSE, fashionA->GetDataID(), fashionB->GetDataID());
	UInt32 fashionASubType = (UInt32)fashionA->GetSubType();
	// 删除合成材料
	if (SUCCESS != GetOwner()->ReduceItemNum(reason.c_str(), composer, 1) ||
		SUCCESS != GetOwner()->ReduceItemNum(reason.c_str(), fashionA, 1) ||
		SUCCESS != GetOwner()->ReduceItemNum(reason.c_str(), fashionB, 1))
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " remove item error!" << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	// 添加新时装
	RewardGroup* grp = GetOwner()->GetRewardGroup();
	grp->AddReward(selComFashionId, 1);

	std::ostringstream os;
	os << FASHION_CMPS_PRE << fashionASubType;

	// 合出天空套计数
	if (getSkyFashionId > 0)
	{
		grp->AddReward(getSkyFashionId, 1);
		_SetSkySuitNum(getSkyFashionId);

		FashionComposeSkyData* skyData = FashionComposeSkyMgr::Instance()->FindEntry(getSkyFashionId);
		if (skyData == NULL)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " skyData null! getSkyFashionId = " << getSkyFashionId << LogStream::eos;
			return ErrorCode::ITEM_DATA_INVALID;
		}

		//发获得天空套公告
		_SendFashionAnnounce(0, getSkyFashionId);

		// 得到一套天空套送翅膀
		if (_CanGetSuitWing(suitId, skyData->type))
		{
			windId = FashionComposeSkyMgr::Instance()->GetWingId(suitId, GetOwner()->GetMainOccu(), skyData->type);
			grp->AddReward(windId, 1);
			_SetSkySuitNum(windId);

			//发集齐套装公告
			_SendFashionAnnounce(suitId, skyData->type);
		}

		GetOwner()->SetCounter(os.str().c_str(), 0);
		GetOwner()->RecordGetSkyFashion(getSkyFashionId);
	}
	else
	{
		GetOwner()->IncCounter(os.str().c_str(), 1);
	}

	//换装活动检查
	if (ActivityMgr::Instance()->IsInOpActivityByTmpType(OpActivityType::OAT_CHANGE_FASHION_MERGE, GetOwner()->GetLevel()))
	{
		UInt32 actScoreNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_CHFASH_ACTSCORE_FASHCOMP);
		OpActivityRegInfo* opAct = ActivityMgr::Instance()->GetOpenActivityRegInfoByTmplateType(GetOwner(), OAT_CHANGE_FASHION_MERGE);
		if (opAct)
		{
			std::string reason = GetReason(LogSourceType::SOURCE_TYPE_FASHION_COMPOSE, fashionA->GetDataID(), fashionB->GetDataID());
			GetOwner()->AddItem(reason.c_str(), opAct->parm, actScoreNum);
		}
	}

	GetOwner()->AddRewards(reason.c_str(), grp, false);

	GetOwner()->SendFashionComposeBack(ErrorCode::SUCCESS, 0, selComFashionId, getSkyFashionId, windId);
	
	//充值推送的天空套合成行为记录 
	const RechargePushDataEntry* dataEntry = RechargePushDataEntryMgr::Instance()->GetDataEntryByPushId(7001);
	if (dataEntry && dataEntry->judgePropIDs.size() > 0)
	{
		if (std::find(dataEntry->judgePropIDs.begin(), dataEntry->judgePropIDs.end(), suitId) != dataEntry->judgePropIDs.end())
		{
			OpActivityRecordMgr::Instance()->UpdateRecord(OP_RECHARGE_PUSH_BEHAVIOR_THIS_WEEK, OP_ACTIVITY_BEHAVIOR_OWNER_ROLE, GetOwner()->GetID(), OP_ACTIVITY_PARAM_FASHION_COMPOSE, 1);
		}
	}
	
	return ErrorCode::SUCCESS;
}

UInt32 ItemMgr::FashionComposeChangeActive(UInt64 fashionId, UInt64 ticketId, UInt32 choiceComFashionId)
{
	//活动检查
	if (!ActivityMgr::Instance()->IsInOpActivityByTmpType(OpActivityType::OAT_CHANGE_FASHION_MERGE, GetOwner()->GetLevel()))
	{
		return ITEM_FASHCOM_CHANGEACTIV_NOTOPEN;
	}
	// 检查参数
	Item* fashion = FindItem(fashionId);
	Item* ticket = FindItem(ticketId);
	if (!fashion || !ticket)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " param err!" << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}
	if (fashion->GetID() == ticket->GetID())
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " param err fashion ticket GetId() equal!" << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	// 检查参数合法性
	if (!IsFashion((ItemSubType)fashion->GetSubType()) ||
		ticket->GetSubType() != ST_CHANGE_FASHION_ACTIVE_TICKET)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " fashion type err!" << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}
	// 不是同部位
	ItemDataEntry* choiceFsData = ItemDataEntryMgr::Instance()->FindEntry(choiceComFashionId);
	if (!choiceFsData)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " choiceFsData is null! id:" << choiceComFashionId << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}
	if (choiceFsData->subType != fashion->GetSubType())
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " fashion part not same! fashion:" << fashion->GetSubType() << " choiceFsData:" << choiceFsData->subType << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}
	if (fashion->GetQuality() != ITEM_QUALITY_PURPLE || choiceFsData->color != ITEM_QUALITY_PURPLE)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " color err fashion color:" << fashion->GetQuality() << " choiceComFashion color:" << choiceFsData->color << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	// 是否时限时装
	if (fashion->GetDeadLine() > 0)
	{
		return ErrorCode::FASHION_CMPS_TIME_FS;
	}
	// 检查时装背包格子
	UInt32 ret = _CheckFashionCmpsCapacity();
	if (ErrorCode::SUCCESS != ret)
	{
		return ret;
	}
	FashionMergeChangeActiveDataEntry* mergeDataEntry = FashionMergeChangeActiveDataEntryMgr::Instance()->FindEntry(choiceComFashionId);
	if (!mergeDataEntry)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " mergeDataEntry is null! id:" << choiceComFashionId << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}
	//检查货币，换装卷
	if (this->GetOwner()->GetGoldBoth() < mergeDataEntry->goldConsume.num)
	{
		return ErrorCode::ITEM_NOT_ENOUGH_GOLD;
	}
	if (this->GetOwner()->GetItemNum(mergeDataEntry->ticketConsume.id) < mergeDataEntry->ticketConsume.num)
	{
		return ErrorCode::ITEM_FASHCOM_CHANGEACTIV_TICKETNO;
	}
	// 删除合成材料
	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_CHANGE_FASHION_COMP, fashion->GetDataID(), ticket->GetDataID());
	if (SUCCESS != GetOwner()->ReduceItemNum(reason.c_str(), fashion, 1) ||
		!GetOwner()->RemoveItem(reason.c_str(), mergeDataEntry->ticketConsume.id, mergeDataEntry->ticketConsume.num) ||
		!GetOwner()->RemoveGoldBoth(reason.c_str(), mergeDataEntry->goldConsume.num))
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " remove item error!" << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	//合成时装
	bool bGetAdvance = Avalon::Random::SelectInThousand(mergeDataEntry->pro);
	RewardGroup* grp = GetOwner()->GetRewardGroup();
	grp->AddReward(choiceComFashionId, 1);
	bool allMerged = false;
	if (bGetAdvance)
	{
		grp->AddReward(mergeDataEntry->advanceId, 1);
		std::string partMergedCounter = CHANGEFASH_ACT_PART_MERGED_PER + Avalon::StringUtil::ConvertToString(fashion->GetSubType());
		if (CounterCfg::Instance()->GetCycleType(partMergedCounter) == COUNTER_CYCLE_INVALID)
		{
			CounterCfg::Instance()->RegCounter(partMergedCounter.c_str(), COUNTER_CYCLE_NONE);
		}
		GetOwner()->SetCounter(partMergedCounter.c_str(), 1);

		allMerged = true;
		for (UInt32 part = ST_FASHION_HEAD; part <= ST_FASHION_EPAULET; part++)
		{
			partMergedCounter = CHANGEFASH_ACT_PART_MERGED_PER + Avalon::StringUtil::ConvertToString(part);
			if (CounterCfg::Instance()->GetCycleType(partMergedCounter) == COUNTER_CYCLE_INVALID)
			{
				CounterCfg::Instance()->RegCounter(partMergedCounter.c_str(), COUNTER_CYCLE_NONE);
			}
			if (GetOwner()->GetCounter(partMergedCounter.c_str()) == 0)
			{
				allMerged = false;
				break;
			}
		}
	}
	GetOwner()->AddRewards(reason.c_str(), grp, false);
	GetOwner()->SendFashionCompBackChangeActive(ErrorCode::SUCCESS, bGetAdvance ? 2 : 1, choiceComFashionId, bGetAdvance ? mergeDataEntry->advanceId : 0, allMerged);

	UInt32 actScoreNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_CHFASH_ACTSCORE_ACTFASHCOMP);
	OpActivityRegInfo* opAct = ActivityMgr::Instance()->GetOpenActivityRegInfoByTmplateType(GetOwner(), OAT_CHANGE_FASHION_MERGE);
	if (opAct)
	{
		std::string reason = GetReason(LogSourceType::SOURCE_TYPE_CHANGE_FASHION_COMP, fashionId, choiceComFashionId);
		GetOwner()->AddItem(reason.c_str(), opAct->parm, actScoreNum);
	}
	return ErrorCode::SUCCESS;
}

UInt32 ItemMgr::EuqipMake(UInt32 equipId)
{
	EquipMakeData* data = EquipMakeDataMgr::Instance()->FindEntry(equipId);
	if (!data)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "EquipMakeData is null! id = " << equipId << LogStream::eos;
		return ITEM_DATA_INVALID;
	}

	std::function<bool(Item*)> cb = [this](Item* item)->bool
	{
		if (!item)	return true;
		if (!item->IsCanPushScheme())
		{
			return false;
		}
		if (SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_EQUIP_SCHEME) && GetOwner()->GetEqSchemeMgr().IsEquipInScheme(item->GetID()))
		{
			return true;
		}
		return false;
	};

	//检查材料
	for (size_t i = 0; i < data->needMats.size(); ++i)
	{
		EquipMakeData::MatInfo& matInfo = data->needMats[i];
		//UInt32 hasNum = GetOwner()->GetItemNumWithEqualItem(matInfo.matId);
		UInt32 hasNum = GetOwner()->GetItemNumWithFilter(matInfo.matId, cb);
		if (hasNum < matInfo.num)
		{
			return ITEM_NOT_ENOUGH_MAT;
		}
	}

	//检查货币
	UInt32 hasMoneyNum = GetOwner()->GetItemNum(data->needMoney);
	if (hasMoneyNum < data->needMoneyNum)
	{
		return ITEM_NOT_ENOUGH_GOLD;
	}

	std::string reason = GetReason(LogSourceType::SOURCE_EQUIP_MAKE);

	//扣钱扣材料加道具
	if (data->needMoneyNum > 0 && !GetOwner()->RemoveItem(reason.c_str(), data->needMoney, data->needMoneyNum))
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Remove item(id: " << data->needMoney << ") failed." << LogStream::eos;
		return ITEM_NOT_ENOUGH_GOLD;
	}
	for (size_t i = 0; i < data->needMats.size(); ++i)
	{
		EquipMakeData::MatInfo& matInfo = data->needMats[i];
		if (matInfo.num > 0 && !GetOwner()->RemoveItemWithFilter(reason.c_str(), matInfo.matId, matInfo.num, cb))
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Remove item(id: " << matInfo.matId << ") failed." << LogStream::eos;
			return ITEM_NOT_ENOUGH_MAT;
		}
	}
	//GetOwner()->RemoveItemWithEqualItem(reason.c_str(), matInfo.matId, matInfo.num);

	//添加新道具到背包或邮件
	RewardGroup* grp = GetOwner()->GetRewardGroup();
	grp->AddReward(data->id, 1);
	GetOwner()->AddRewards(reason.c_str(), grp, true);

	GetOwner()->SendFunctionUdpLog(FunctionLogType::FUNCTION_EQUIP_MAKE, data->id);

	return SUCCESS;
}

void ItemMgr::MergeItems(const ItemRewardVec& inItems, ItemRewardVec& outItems)
{
	for (std::vector<ItemReward>::const_iterator iter = inItems.begin();
		iter != inItems.end(); ++iter)
	{
		bool bFind = false;
		ItemDataEntry* data = ItemDataEntryMgr::Instance()->FindEntry(iter->id);
		if (!data)
			continue;

		if (data->type == ITEM_EQUIP)
		{
			outItems.push_back(*iter);
			continue;
		}

		for (int i = 0; i < (int)outItems.size(); ++i)
		{
			if (iter->id == outItems[i].id)
			{
				bFind = true;

				if (data->type == ITEM_INCOME)
				{
					outItems[i].num += iter->num;
					break;
				}
				if (data->maxNum == outItems[i].num)
				{
					bFind = false;
					continue;
				}

				if (iter->auctionCoolTimeStamp > 0 || outItems[i].auctionCoolTimeStamp > 0)
				{
					UInt32 curtime = CURRENT_TIME.Sec();
					if ((iter->auctionCoolTimeStamp > curtime || outItems[i].auctionCoolTimeStamp > curtime)
						&& iter->auctionCoolTimeStamp != outItems[i].auctionCoolTimeStamp)
					{
						bFind = false;
						continue;
					}
				}

				if (iter->strenth != outItems[i].strenth)
				{
					bFind = false;
					continue;
				}

				if (iter->auctionTransTimes != outItems[i].auctionTransTimes)
				{
					bFind = false;
					continue;
				}

				if (data->maxNum >= iter->num + outItems[i].num)
				{
					outItems[i].num += iter->num;
				}
				else
				{
					UInt32 restNum = (iter->num + outItems[i].num) % data->maxNum;
					UInt8 grid = (iter->num + outItems[i].num) / data->maxNum;
					outItems[i].num = data->maxNum;
					ItemReward newReward = *iter;
					newReward.num = restNum;
					outItems.push_back(newReward);
					while (grid > 1)
					{
						ItemReward newReward = *iter;
						newReward.num = data->maxNum;

						grid -= 1;
						outItems.push_back(newReward);
					}

				}

				break;
			}
		}

		if (!bFind)
			outItems.push_back(*iter);
	}
}

UInt32 ItemMgr::RenewTimeItem(UInt64 uid, UInt32 durationDay)
{
	Item* item = FindItem(uid);
	if (!item)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " item uid error! uid = " << uid << LogStream::eos;
		return ITEM_DATA_INVALID;
	}
	if (item->IsItemLock(ILT_LEASE_LOCK))
	{
		return ITEM_IN_LEASING;
	}
	UInt32 costId = 0;
	UInt32 costNum = 0;
	bool find = false;
	for (size_t i = 0; i < item->GetDataEntry()->itemRenewInfos.size(); ++i)
	{
		ItemRenewInfo& renewInfo = item->GetDataEntry()->itemRenewInfos[i];
		if (durationDay == renewInfo.durationDay)
		{
			costId = renewInfo.moneyId;
			costNum = renewInfo.moneyNum;
			find = true;
			break;
		}
	}
	
	// 安全锁开启，不允许消耗点券，续费时限道具
	if (costId == ITEM_POINT && !GetOwner()->SecurityLockCheck())
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " durationDay error! security lock not allow." << LogStream::eos;
		return ErrorCode::SECURITY_LOCK_FORBID_OP;
	}

	if (!find)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " durationDay error! durationDay = " << durationDay << LogStream::eos;
		return ITEM_DATA_INVALID;
	}

	ItemDataEntry* costItemData = ItemDataEntryMgr::Instance()->FindEntry(costId);
	if (!costItemData)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " cost itemData error! id = " << costId << LogStream::eos;
		return ITEM_DATA_INVALID;
	}

	if (GetOwner()->GetItemNum(costId) < costNum)
	{
		return ITEM_NOT_ENOUGH_MONEY;
	}

	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_RENEW_TIME_ITEM, item->GetDataID());
	GetOwner()->RemoveItem(reason.c_str(), costId, (UInt32)costNum);

	item->SetDeadLine(durationDay);
	item->SyncProperty(GetOwner());

	return SUCCESS;
}

UInt32 ItemMgr::AutoSetPotion()
{
	
	// 检查等级
	UInt32 validLv = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_AUTO_SETPOTION);
	if (GetOwner()->GetLevel() < validLv)
	{
		return ITEM_AUTO_SETPOTION_LV_INVALID;
	}


	std::vector<UInt32> pos = GetOwner()->GetPotionPos();
	for (size_t i = 0; i < pos.size(); ++i)
	{
		pos = GetOwner()->GetPotionPos();
		if (0 == pos[i])
		{
			auto vec = GetFitPotionType(i);
			for (auto &j : vec)
			{
				if (j == ST_ATTRIBUTE_DRUG)
				{
					continue;
				}
				UInt32 findId = AutoFindPotion(pos[0], pos[1], pos[2], j);
				if (findId > 0)
				{
					GetOwner()->SetPotionPos(i, findId);
					break;
				}
			}
		}
	}
	GetOwner()->SyncProperty();
	return SUCCESS;
}

UInt32 ItemMgr::UseChangeNameCard(UInt64 uid)
{
	Item* card = FindItem(uid);
	if (!card)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " item obj is null! uid = " << uid << LogStream::eos;
		return ITEM_DATA_INVALID;
	}


	return SUCCESS;
}

void ItemMgr::ClearPackageChache(PackType packType)
{
	class DelItemVisitor : public ItemVisitor
	{
	public:
		DelItemVisitor(Player *player) : m_player(player) {}
		~DelItemVisitor() {}

		bool operator()(Item *item)
		{
			m_player->GetItemMgr().RemoveItemNoSync(item);
			Item::DestroyItem(item);
			return true;
		}

	private:
		Player*	m_player;
	};

	DelItemVisitor delVisitor(GetOwner());
	GetOwner()->VisitItems(packType, delVisitor);
}



void ItemMgr::MasterGiveEquip(std::vector<ObjID_t>& itemUids, Player* disciple)
{
	if (!disciple)
	{
		return;
	}

	std::vector<Item*> sendMailItems;
	if (!CheckMasterGiveEquip(itemUids, disciple, sendMailItems))
	{
		// 师傅赠礼不符合要求
		GetOwner()->SendNotifyById(8917);
		return;
	}

	SendMasterGiveMail(sendMailItems, disciple->GetID());

	disciple->OnGiveEquipMasterSys(itemUids.size());

	// 师傅赠礼成功
	GetOwner()->SendNotifyById(8916);
}

void ItemMgr::CalculateValuedScore(Item* item)
{
	if (item == NULL) return;

	if (SSApplication::Instance()->OpenPlayerOnOnline())
	{
		if (item->GetType() != ITEM_EQUIP && item->GetType() != ITEM_FASHION && item->GetType() != ITEM_TITLE && item->GetType() != ITEM_PET)
		{
			return;
		}
	}

	item->GetValuedScore();

	item->CalculateValuedScore(GetOwner());

	OnItemChangedSortList(item);

	GetOwner()->OnCalculateValuedScore((ItemSubType)item->GetDataEntry()->subType, (ItemQuality)item->GetQuality(), item->GetValuedScore());

	if (item->GetPos().pack == PACK_WEAR)
	{
		GetOwner()->OnUpdateTotalEquipScore();
	}
}

UInt32 ItemMgr::EuqieUpdate(ObjID_t equipUid)
{
	Item* item = FindItem(equipUid);
	if (!item || item->GetTransferStoneid() != 0)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " item uid error! uid = " << equipUid << LogStream::eos;
		return ITEM_DATA_INVALID;
	}

	if (item->CheckProhibitOp(IPOE_UPGRADE))
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "item(guid: " << item->GetID() << "). Prohibit Update " << LogStream::eos;
		return ITEM_PROHIBIT_OPERATE;
	}

	ItemDataEntry* oldDataEntry = item->GetDataEntry();
	if (!oldDataEntry)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " item data entry error! uid = " << equipUid << LogStream::eos;
		return ITEM_DATA_INVALID;
	}

	UInt32 key1 = item->GetDataID() * 100;
	UInt32 key2 = item->GetDataID() * 100 + GetOwner()->GetOccu();

	EquieUpdateDataEntry* data = EquieUpdateDataEntryMgr::Instance()->FindEntry(key1);
	if (!data)
	{
		data = EquieUpdateDataEntryMgr::Instance()->FindEntry(key2);
		if (!data)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "EquieUpdate is null! id = " << item->GetDataID() << LogStream::eos;
			return ITEM_DATA_INVALID;
		}
	}

	ItemDataEntry* newDataEntry = ItemDataEntryMgr::Instance()->FindEntry(data->nextLvEquieID);
	if (!newDataEntry)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "EquieUpdate is null! new id = " << data->nextLvEquieID << LogStream::eos;
		return ITEM_DATA_INVALID;
	}

	if (SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_EQUIP_SCHEME) && item->IsCanPushScheme() && GetOwner()->GetEqSchemeMgr().IsEquipInScheme(item->GetID()))
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "EquieUpdate is fail, is in scheme! uid  = " << equipUid << LogStream::eos;
		return ITEM_IILEG_OPER_IN_SCHEME;
	}

	std::vector<EquieUpdateDataEntry::MatInfo> needMats;

	if (EQUIP_NORMAL == item->GetEquipType())
	{
		for (auto& infoLimit : data->needMatLimits)
		{
			if (item->GetStrengthen() >= infoLimit.minStrenthLevel && item->GetStrengthen() <= infoLimit.maxStrenthLevel)
			{
				needMats = infoLimit.needMats;
				break;
			}
		}
	}
	else
	{
		for (auto& infoLimit : data->increaseNeedMatLimits)
		{
			if (item->GetStrengthen() >= infoLimit.minStrenthLevel && item->GetStrengthen() <= infoLimit.maxStrenthLevel)
			{
				needMats = infoLimit.needMats;
				break;
			}
		}
	}

	if (needMats.size() <= 0)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "need mats error! new id = " << data->nextLvEquieID << LogStream::eos;
		return ITEM_DATA_INVALID;
	}

	//检查材料
	for (size_t i = 0; i < needMats.size(); ++i)
	{
		EquieUpdateDataEntry::MatInfo& matInfo = needMats[i];
		UInt32 hasNum = GetOwner()->GetItemNum(matInfo.matId);
		if (hasNum < matInfo.num)
		{
			return ITEM_NOT_ENOUGH_MAT;
		}
	}

	std::string reason = GetReason(LogSourceType::SOURCE_EQUIE_UPDATE, data->equieID, data->jobID);

	//扣钱扣材料
	for (size_t i = 0; i < needMats.size(); ++i)
	{
		EquieUpdateDataEntry::MatInfo& matInfo = needMats[i];
		if (matInfo.num > 0 && !GetOwner()->RemoveItem(reason.c_str(), matInfo.matId, matInfo.num))
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Remove item(id: " << matInfo.matId << ") failed." << LogStream::eos;
			return ITEM_NOT_ENOUGH_MAT;
		}
	}

	item->ChangeDataID(data->nextLvEquieID);
	item->SetDataEntry(newDataEntry);
	item->SetName(newDataEntry->name);
	item->SetIsTreas(newDataEntry->isTreasure && SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_TREAS));

	item->SetSealState(newDataEntry->seal);
	item->SetSealCount(0);

	item->ClearAttribute();
	item->SetAttribute();

	//计算装备分数
	CalculateValuedScore(item);

	//同步属性
	item->SyncProperty(GetOwner());

	if (data->announceID == 92 || data->announceID == 93 || data->announceID == 95)
	{
		std::string playerLinkStr;
		if (!MsgPackage::GetPlayerMsgTag(playerLinkStr, GetOwner()->GetID(), GetOwner()->GetName(), GetOwner()->GetOccu(), GetOwner()->GetLevel()))
		{
			playerLinkStr = GetOwner()->GetName();
		}

		std::string systemLinkStr;
		MsgPackage::GetSystemSuffixesMsgTag(systemLinkStr, SystemSuffixesType::SYSTEM_SUFFIXES_TYPE_EQUIE_UPDATE, 0);
		Sys::SendAnnouncement(0, data->announceID, playerLinkStr.c_str(), oldDataEntry->name.c_str(), newDataEntry->name.c_str(), systemLinkStr.c_str());
	}

	return SUCCESS;
}

bool ItemMgr::CheckMasterGiveEquip(std::vector<ObjID_t>& itemUids, Player* disciple, std::vector<Item*>& outItems)
{
	for (size_t i = 0; i < itemUids.size(); ++i)
	{
		Item* item = FindItem(itemUids[i]);
		if (!item)
		{
			return false;
		}

		// 赠送得装备等级超过徒弟等级
		if (item->GetNeedLevel() > disciple->GetLevel())
		{
			return false;
		}

		// 赠送得装备品质大于紫色
// 		if (item->GetQuality() > ItemQuality::ITEM_QUALITY_PURPLE)
// 		{
// 			return false;
// 		}

		// 不能师傅赠送
		if (0 == item->GetDataEntry()->canMasterGive)
		{
			return false;
		}

		if (item->IsItemLock(ILT_ANY_LOCK))
		{
			return false;
		}

		outItems.push_back(item);
	}

	return true;
}

void ItemMgr::SendMasterGiveMail(std::vector<Item*>& items, ObjID_t recvId)
{
	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_MASTER_GIVE, recvId);
	std::string strSenderName = GetOwner()->GetName();
	std::string	strTitle;
	std::string	strContent;
	GetOwner()->GetMailInfoByReason(reason.c_str(), strSenderName, strTitle, strContent);
	
	std::vector<Item*> tmpMailGroup;
	for (size_t i = 0; i < items.size(); ++i)
	{
		if (!items[i])
		{
			continue;
		}

		tmpMailGroup.push_back(items[i]);

		// 到邮件附件上限发送
		if ((UInt32)tmpMailGroup.size() % MAX_MAILITEM_NUM == 0)
		{
			Sys::SendSysMail(recvId, strSenderName.c_str(), strTitle.c_str(), strContent.c_str(), reason.c_str(), MAIL_VALID_DAY, NULL, tmpMailGroup);
			tmpMailGroup.clear();
		}
	}

	// 剩余的发送
	if (tmpMailGroup.size() != 0)
	{
		Sys::SendSysMail(recvId, strSenderName.c_str(), strTitle.c_str(), strContent.c_str(), reason.c_str(), MAIL_VALID_DAY, NULL, tmpMailGroup);
		tmpMailGroup.clear();
	}

	// 删除道具
	for (size_t i = 0; i < items.size(); ++i)
	{
		GetOwner()->ReduceItemNum(reason.c_str(), items[i], items[i]->GetNum());
	}
	
}

UInt32 ItemMgr::_CheckFashionCmpsParam(Item* fashionA, Item* fashionB, Item* composer, UInt32 selFashionId)
{
	if (!fashionA || !fashionB || !composer)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " param err!" << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	if (fashionA->IsItemLock(ILT_FASHION_LOCK) || fashionB->IsItemLock(ILT_FASHION_LOCK))
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " param err fashionA or fashionB is fashion lock " << LogStream::eos;
		return ErrorCode::ITEM_FASH_CMPS_FAIL_ITEMLOCK;
	}

	if (fashionA->GetID() == fashionB->GetID() ||
		fashionA->GetID() == composer->GetID() ||
		fashionB->GetID() == composer->GetID())
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " param err fashionA fashionB composer GetId() equal!" << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	// 检查参数合法性
	if (!IsFashion((ItemSubType)fashionA->GetSubType()) ||
		!IsFashion((ItemSubType)fashionA->GetSubType()) ||
		composer->GetSubType() != ST_FASHION_COMPOSER)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " fashion type err!" << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	// 不是同部位
	ItemDataEntry* selFsData = ItemDataEntryMgr::Instance()->FindEntry(selFashionId);
	if (!selFsData)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " selFsData is null! id:" << selFashionId << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	if (selFsData->subType != fashionA->GetSubType() ||
		fashionA->GetSubType() != fashionB->GetSubType())
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " fashion part not same! A:" << fashionA->GetSubType() << " B:" << fashionB->GetSubType() << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	// 是否时限时装
	if (fashionA->GetDeadLine() > 0 || fashionB->GetDeadLine() > 0)
	{
		return ErrorCode::FASHION_CMPS_TIME_FS;
	}

	return ErrorCode::SUCCESS;
}

UInt32 ItemMgr::_CreateFahsionCmpsRate(Item* fashionA, Item* fashionB, Item* composer, UInt32& rate, UInt32& prob)
{
	// 检查data
	FashionComposeRate* RateAData = FashionComposeRateMgr::Instance()->FindEntry(fashionA->GetThirdType());
	FashionComposeRate* RateBData = FashionComposeRateMgr::Instance()->FindEntry(fashionB->GetThirdType());
	FashionComposeRate* RatecomposerData = FashionComposeRateMgr::Instance()->FindEntry(composer->GetThirdType());
	if (!RateAData || !RateBData || !RatecomposerData)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " datas err! A type:" << fashionA->GetThirdType() << " B type:" << fashionB->GetThirdType() << " C type:" << composer->GetThirdType() << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	// 生成天空套合成系数
	rate = RateAData->rate + RateBData->rate + RatecomposerData->rate;
	if (ActivityMgr::Instance()->IsInOpActivityByTmpType(OAT_FLYUP_GIFT, GetOwner()->GetLevel()))
	{
		OpActivityRegInfo* opActRegInfo = ActivityMgr::Instance()->GetOpenActivityRegInfoByTmplateType(GetOwner(), OAT_FLYUP_GIFT);
		if (opActRegInfo && opActRegInfo->parm3.size() > 0)
		{
			rate += opActRegInfo->parm3[0];
		}
	}
	UInt32 maxNum = FashionCompPartRateMgr::Instance()->GetMaxNum();

	UInt32 partCmpsNum = 0;

	// 初始化合成时装相关得计数
	std::ostringstream os;
	os << FASHION_CMPS_PRE << (UInt32)fashionA->GetSubType();
	const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(os.str());
	if (!counterConfig)
	{
		CounterCfg::Instance()->RegCounter(os.str().c_str(), COUNTER_CYCLE_NONE);
	}
	
	partCmpsNum = GetOwner()->GetCount(os.str().c_str());

	// 生成天空套合成概率
	UInt32 findKey = partCmpsNum + 1;
	if (findKey > maxNum)
	{
		findKey = maxNum;
	}
	FashionCompPartRate* partRateData = FashionCompPartRateMgr::Instance()->FindEntry(findKey);
	if (!partRateData)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " partRateData err!" << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	// 合成概率
	prob = 0;

	std::ostringstream osII;
	osII << GET_SKY_FASHION_TYPE << (UInt32)fashionA->GetSubType();
	counterConfig = CounterCfg::Instance()->GetCycleConfig(osII.str());
	if (!counterConfig)
	{
		CounterCfg::Instance()->RegCounter(osII.str().c_str(), COUNTER_CYCLE_NONE);
	}

	FashionComposeSkyData::SkyType type = (FashionComposeSkyData::SkyType)GetOwner()->GetCount(osII.str().c_str());
	if (FashionComposeSkyData::SkyType::ST_NONE == type)
	{
		prob = partRateData->noneSkyRate;
	}
	else if (FashionComposeSkyData::SkyType::ST_COMMON == type)
	{
		prob = partRateData->normalSkyRate;
	}
	else if (FashionComposeSkyData::SkyType::ST_ADVANCE == type)
	{
		prob = partRateData->goldSkyRate;
	}
	else
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " last get skyfashion type err = " << type << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	return ErrorCode::SUCCESS;
}

UInt32 ItemMgr::_GetFashionCmpsItem(UInt32 skyProb, UInt32 suitId, UInt32 selComFashionId, UInt8 part, UInt32& getSkyFashionId, bool isL)
{
	bool bGetSky = Avalon::Random::SelectInTenThousand(skyProb);
#ifdef _DEBUG
	if (GetOwner()->m_testHappySky)
	{
		bGetSky = true;
	}

	GetOwner()->SendNotify("skyProb = {0}", skyProb);
#endif

	//DebugLogStream << PLAYERINFO(GetOwner()) << " skyprob:" << skyProb << LogStream::eos;

	// 合成天空套
	if (bGetSky)
	{
		bool isTpSky = false;
		std::ostringstream os;
		os << PLAYER_SKY_COMPNUM_IN_PRODUCE_TPSKY << (UInt32)part;
		const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(os.str());
		if (!counterConfig)
		{
			CounterCfg::Instance()->RegCounter(os.str().c_str(), COUNTER_CYCLE_NONE);
		}
		UInt32 couter = GetOwner()->GetCount(os.str().c_str());
		UInt32 num = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_TPSKYFASHION_PART_NOTCREATE_TIMES);

		if (couter > 0 && couter <= num) //同一部位合出透明天空套时,接下来该部位num次不会再出透明天空套
		{
			isTpSky = false;
			if (couter == num)
			{
				GetOwner()->SetCounter(os.str().c_str(), 0);
			}
			else
			{
				GetOwner()->IncCounter(os.str().c_str(), 1);
			}
		}
		else  //判断是否出透明天空套
		{
			if (couter > num)
			{
				GetOwner()->SetCounter(os.str().c_str(), 0);
			}
			Int32 basic_rate = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_TPSKYFASHION_BASICS_RATE);
			UInt32 sky_conti_num = GetOwner()->GetCount(PLAYER_SKY_CONTI_NUM);
			UInt32 tpsky_conti_num = GetOwner()->GetCount(PLATER_TPSKY_CONTI_NUM);
			if (sky_conti_num > 0)
			{
				basic_rate += sky_conti_num * SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_TPSKYFASHION_INCRE_RATE);
			}
			else if (tpsky_conti_num > 0)
			{
				basic_rate -= tpsky_conti_num * SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_TPSKYFASHION_REDUCE_RATE);
			}

			Int32 rate_lowerlimit = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_TPSKYFASHION_RATE_LOWERLIMIT);
			Int32 rate_upperlimit = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_TPSKYFASHION_INCRE_UPPERLIMIT);

			basic_rate = basic_rate < rate_lowerlimit ? rate_lowerlimit : basic_rate;
			basic_rate = basic_rate > rate_upperlimit ? rate_upperlimit : basic_rate;

#ifdef _DEBUG
			GetOwner()->SendNotify("tpSkyProb = {0}", basic_rate);
#endif

			isTpSky = Avalon::Random::SelectInThousand(basic_rate);
		}

		if (isTpSky) //产出透明天空套
		{
			FashionComposeSkyMgr::FashionSkyVec skys;
			FashionComposeSkyKey key(GetOwner()->GetMainOccu(), part, suitId);
			FashionComposeSkyMgr::Instance()->GetTransFashionSkyDatas(key, skys);

			if (skys.size() == 0)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << " not find tp sky Occu:" << GetOwner()->GetMainOccu() << " part: " << part << " suitId: " << suitId << LogStream::eos;
				return ErrorCode::ITEM_DATA_INVALID;
			}
			getSkyFashionId = skys[0]->id;

			GetOwner()->SetCounter(os.str().c_str(), 1);

			GetOwner()->IncCounter(PLATER_TPSKY_CONTI_NUM, 1);
			GetOwner()->SetCounter(PLAYER_SKY_CONTI_NUM, 0);
		}
		else  //产出正常天空套
		{
			GetOwner()->IncCounter(PLAYER_SKY_CONTI_NUM, 1);
			GetOwner()->SetCounter(PLATER_TPSKY_CONTI_NUM, 0);

			FashionComposeSkyMgr::FashionSkyVec skys;
			FashionComposeSkyMgr::FashionSkyVec realSkys;

			FashionComposeSkyKey key(GetOwner()->GetMainOccu(), part, suitId);
			FashionComposeSkyMgr::Instance()->GetFahionSkyDatas(key, skys);

			//去掉已经合出过得
			_FilterAlreadyGetSky(skys, realSkys);

			//如果库里已经没有了就重置
			if (realSkys.size() == 0)
			{
				for (size_t i = 0; i < skys.size(); ++i)
				{
					if (!skys[i])	continue;
					GetOwner()->ResetGetSkyRecord(skys[i]->id);
					realSkys.push_back(skys[i]);
				}
			}

			std::vector<UInt32> realSkyWt;
			for (size_t i = 0; i < realSkys.size(); ++i)
			{
				if (!realSkys[i])	continue;

				if (isL)
				{
					_FixWeight(realSkyWt, realSkys[i]);
				}
				else
				{
					realSkyWt.push_back(realSkys[i]->baseWeight);
				}

			}

			Int32 idx = WeightRander::RandWeight(realSkyWt);
			if (idx < 0 || idx >= (Int32)realSkys.size())
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << " rand skysuit error! idx:" << idx << LogStream::eos;
				return ErrorCode::ITEM_DATA_INVALID;
			}

			getSkyFashionId = realSkys[idx]->id;

			FashionComposeSkyData* skyData = FashionComposeSkyMgr::Instance()->FindEntry(getSkyFashionId);
			if (!skyData)
			{
				ErrorLogStream << PLAYERINFO(GetOwner()) << " skyData is null! fashionId:" << getSkyFashionId << LogStream::eos;
				return ErrorCode::ITEM_DATA_INVALID;
			}

			std::ostringstream os;
			os << GET_SKY_FASHION_TYPE << (UInt32)part;
			const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(os.str());
			if (!counterConfig)
			{
				CounterCfg::Instance()->RegCounter(os.str().c_str(), COUNTER_CYCLE_NONE);
			}

			// 更新伪概率
			if (GetOwner()->GetCount(os.str().c_str()) != FashionComposeSkyData::SkyType::ST_NONE)
			{
				GetOwner()->SetCounter(os.str().c_str(), (UInt32)FashionComposeSkyData::SkyType::ST_NONE);
			}
			else
			{
				if ((FashionComposeSkyData::SkyType)skyData->type > FashionComposeSkyData::ST_ADVANCE)
				{
					ErrorLogStream << PLAYERINFO(GetOwner()) << " skyData type is error! type:" << skyData->type << LogStream::eos;
					return ErrorCode::ITEM_DATA_INVALID;
				}
				GetOwner()->SetCounter(os.str().c_str(), (UInt32)(skyData->type));

				int flag = skyData->type == FashionComposeSkyData::ST_COMMON ? 0 : 1;
				int mask = GetOwner()->GetCount(L_SKY_GET_INFO);
				mask = (mask << 1) | flag;
				GetOwner()->SetCounter(L_SKY_GET_INFO, mask);
				GetOwner()->IncCounter(L_SKY_GET_NUM, 1);
			}
		}

	}

	// 合成普通高级时装
	{
		FashionComposeData* normalData = FashionComposeMgr::Instance()->FindEntry(selComFashionId);
		if (!normalData)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " selComFashionId error! id:" << selComFashionId << LogStream::eos;
			return ErrorCode::ITEM_DATA_INVALID;
		}
	}

	return ErrorCode::SUCCESS;
}

void ItemMgr::_FilterAlreadyGetSky(FashionComposeSkyMgr::FashionSkyVec& inSkys, FashionComposeSkyMgr::FashionSkyVec& outRealSkys)
{
	for (size_t i = 0; i < inSkys.size(); ++i)
	{
		if (!inSkys[i])
		{
			continue;
		}

		if (!GetOwner()->HasGetSkyFashion(inSkys[i]->id))
		{
			outRealSkys.push_back(inSkys[i]);
		}
	}
}

UInt32 ItemMgr::_SetSkySuitNum(UInt32 skyFashionId)
{
	FashionComposeSkyData* skyFashionData = FashionComposeSkyMgr::Instance()->FindEntry(skyFashionId);
	if (!skyFashionData)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " skyFashionData is null!" << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	if (skyFashionData->part < ST_FASHION_HALO || skyFashionData->part > ST_FASHION_EPAULET)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " skyFashionData part is err! id:" << skyFashionData->id << " part:" << skyFashionData->part << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	// 初始化天空套获得件数
	std::ostringstream os;
	os << SKY_FASHOPN_SUIT_NUM << (UInt32)skyFashionData->suitId << (UInt32)skyFashionData->type;
	const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(os.str());
	if (!counterConfig)
	{
		CounterCfg::Instance()->RegCounter(os.str().c_str(), COUNTER_CYCLE_NONE);
	}

	UInt32 skySuitNum = GetOwner()->GetCount(os.str().c_str());
	UInt32 part = (UInt32)skyFashionData->part % 10;
	skySuitNum |= (1 << part);
	
	GetOwner()->SetCounter(os.str().c_str(), skySuitNum);

#ifdef _DEBUG
	//GetOwner()->SendNotify("skySuit get part = {0}", skySuitNum);
#endif

	return ErrorCode::SUCCESS;
}

bool ItemMgr::_CanGetSuitWing(UInt32 suitId, UInt8 type)
{
	std::ostringstream os;
	os << SKY_FASHOPN_SUIT_NUM << suitId << (UInt32)type;
	const CounterConfig* counterConfig = CounterCfg::Instance()->GetCycleConfig(os.str());
	if (!counterConfig)
	{
		CounterCfg::Instance()->RegCounter(os.str().c_str(), COUNTER_CYCLE_NONE);
	}
	UInt32 skySuitNum = GetOwner()->GetCount(os.str().c_str());

	// 该套装已经得到过翅膀了
	if (skySuitNum & (1 << (ST_FASHION_HALO % 10)))
	{
		return false;
	}

	bool all = true;
	FashionComposeSkyKeyII key(GetOwner()->GetMainOccu(), type, suitId);
	FashionComposeSkyMgr::FashionSkyVec datas;
	FashionComposeSkyMgr::Instance()->GetFashionSkyDatasII(key, datas);

	for (size_t i = 0; i < datas.size(); ++i)
	{
		if (!datas[i])	continue;
		if ((ItemSubType)datas[i]->part == ST_FASHION_HALO)	continue;
		
		if (0 == (skySuitNum & (1 << (datas[i]->part % 10))))
		{
			all = false;
			break;
		}
	}

	return all;
}

UInt32 ItemMgr::_CheckFashionCmpsCapacity()
{
	Package* pkg = GetPackage(PACK_FASHION);
	if (!pkg)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}

	if (pkg->GetCapacity() < FASHION_CMPS_NEED_CAP)
	{
		return ErrorCode::ITEM_PACKSIZE_MAX;
	}

	return ErrorCode::SUCCESS;
}

void ItemMgr::_SendFashionAnnounce(UInt32 suitId, UInt32 param)
{
	std::string linkStr;
	std::string playerLinkStr;
	std::string systemLinkStr;

	if (suitId > 0)
	{
		if (!MsgPackage::GetPlayerMsgTag(playerLinkStr, GetOwner()->GetID(), GetOwner()->GetName(), GetOwner()->GetOccu(), GetOwner()->GetLevel()))
		{
			playerLinkStr = GetOwner()->GetName();
		}

		linkStr = FashionComposeSkyMgr::Instance()->GetSuitName(suitId, param);
		MsgPackage::GetSystemSuffixesMsgTag(systemLinkStr, SystemSuffixesType::SYSTEM_SUFFIXES_TYPE_FASHION_COMPOSE, 0);
		Sys::SendAnnouncement(0, 63, playerLinkStr.c_str(), linkStr.c_str(), systemLinkStr.c_str());
	}
	else
	{
		if (!MsgPackage::GetPlayerMsgTag(playerLinkStr, GetOwner()->GetID(), GetOwner()->GetName(), GetOwner()->GetOccu(), GetOwner()->GetLevel()))
		{
			playerLinkStr = GetOwner()->GetName();
		}

		if (MsgPackage::GetItemMsgTag(linkStr, 0, param, 0))
		{
			MsgPackage::GetSystemSuffixesMsgTag(systemLinkStr, SystemSuffixesType::SYSTEM_SUFFIXES_TYPE_FASHION_COMPOSE, 0);
			Sys::SendAnnouncement(0, 62, playerLinkStr.c_str(), linkStr.c_str(), systemLinkStr.c_str());
		}
	}
}


void ItemMgr::_FixWeight(std::vector<UInt32>& wt, FashionComposeSkyData* data)
{
	if (!data)
	{
		return;
	}

	//满足重置条件
	int index = GetOwner()->GetCounter(L_SKY_GET_NUM) % L_SKY_RESET_NUM;
	if (0 == index)
	{
		wt.push_back(data->baseWeight);
		GetOwner()->SetCounter(L_SKY_GET_INFO, 0);
		return;
	}

	// 根据之前获得天空套情况计算权重
	/*
		1）仅对L概率下出天空时装时，库概率产生影响
		2）每5次为1个循环（线上版本玩家的次数要记录）
		3）循环期内，每合出1件高级天空，则下一次高级天空的随机概率减少20%（最低0%），普通天空的随机概率增加20%（最高100%）
		4）循环期内，连续合出2件普通天空时，每再多连续合出1件普通天空，高级天空产出概率增加10%，普通天空产出概率减少10%
		5）L系数下，每产出5次天空，概率重置为普通天空70%，高级天空30%

	*/
	int weight = data->baseWeight;
	FashionComposeSkyData::SkyType skyType = (FashionComposeSkyData::SkyType)data->type;
	int mask = GetOwner()->GetCounter(L_SKY_GET_INFO);
	int commonComboNum = 0;
	for (int i = 0; i < index; i++)
	{
		int flag = (mask & (1 << i));
		bool isAdvance = false;
		if (flag != 0)
		{
			isAdvance = true;
		}
		
		if (isAdvance)
		{
			// 如果是高级天空套，高级天空的随机概率减少20%（最低0%），普通天空的随机概率增加20%（最高100%）
			if (skyType == FashionComposeSkyData::ST_ADVANCE)
			{
				weight -= L_FIX_ADVANCE_WT;
			}
			else
			{
				weight += L_FIX_ADVANCE_WT;
			}
			commonComboNum = 0;
		}
		else
		{
			// 循环期内，连续合出2件普通天空时，每再多连续合出1件普通天空，高级天空产出概率增加10%，普通天空产出概率减少10%
			if (++commonComboNum < L_COMMON_COMBO_NUM)
			{
				continue;
			}

			if (skyType == FashionComposeSkyData::ST_ADVANCE)
			{
				weight += L_FIX_COMMON_WT;
			}
			else
			{
				weight -= L_FIX_COMMON_WT;
			}
		}
	}

	if (weight < 0)
	{
		weight = 0;
	}
	else if (weight > L_FIX_MAX_WT)
	{
		weight = L_FIX_MAX_WT;
	}

	wt.push_back((UInt32)weight);
}
bool ItemMgr::_IsFashionCmpsOpen()
{
	return SSServiceSwitchMgr::Instance()->IsOpen(FASHION_COMPOSE);
}

class PlayerLoadingForCheckItem : public PlayerLoadingCallback
{
public:
	PlayerLoadingForCheckItem(UInt32 serialId, UInt8 type, ObjID_t itemId, UInt64 param)
		: m_serialId(serialId), m_type(type), m_itemId(itemId), m_param(param){}
	virtual void OnFinish(Player* player)
	{
		if (!player)
		{
			ErrorLogStream << "PlayerLoadingForCheckItem player null" << LogStream::eos;
			ItemMgr::OnCheckItemFail(ITEM_CHECKITEM_FAIL, m_serialId);
			return;
		}
		ItemMgr::CheckItem(m_serialId, m_type, player, m_itemId, m_param);
	}
	
	/**
	*@brief 加载失败
	*/
	virtual void OnFailure(Player* player)
	{
		if (player->GetGameStatus() == PS_INIT)
		{
			// 加载失败，设为代摧毁状态
			player->SetGameStatus(PS_DESTORY);
		}
		ItemMgr::OnCheckItemFail(ITEM_CHECKITEM_FAIL, m_serialId);
	}

	/**
	*@brief 加载超时
	*/
	virtual void OnTimeout(Player* player)
	{
		OnFailure(player);
	}

	UInt32		m_serialId;
	UInt8		m_type;
	ObjID_t		m_itemId;
	UInt64		m_param;
};

void ItemMgr::CheckItem(UInt32 serialId, UInt8 type, ObjID_t owerId, ObjID_t itemId, UInt64 param)
{
	InfoLogStream << "check item serialId:" << serialId << ", type:" << (UInt32)type << ", owerId:" << owerId << ", itemId:" << itemId
		<< ", param:" << param << LogStream::eos;
	Player* player = PlayerMgr::Instance()->FindPlayer(owerId);
	if (player)
	{
		InfoLogStream << "finded player" << LogStream::eos;
		CheckItem(serialId, type, player, itemId, param);
	}
	else //离线加载
	{
		InfoLogStream << "not finded player, start off load" << LogStream::eos;
		PlayerMgr::Instance()->LoadOfflinePlayerCb(owerId, 0, new PlayerLoadingForCheckItem(serialId, type, itemId, param));
	}
}

void ItemMgr::CheckItem(UInt32 serialId, UInt8 type, Player* player, ObjID_t itemId, UInt64 param)
{
	InfoLogStream << "ckeckItem serialId:" << serialId << ", type:" << (UInt32)type << ", itemId:" << itemId << ", param:" << param << LogStream::eos;
	if (!player)
	{
		ErrorLogStream << "player null" << LogStream::eos;
		ItemMgr::OnCheckItemFail(ITEM_CHECKITEM_FAIL, serialId);
		return;
	}

	Item* pItem = player->FindItem(itemId);
	if (!pItem)
	{
		ErrorLogStream << "item(" << itemId << ") find failed!" << LogStream::eos;
		ItemMgr::OnCheckItemFail(ITEM_CHECKITEM_FAIL, serialId);
		return;
	}

	bool item_can_returned = false;
	std::string reason = "";
	if (type == CMRT_AUCTION_PUNISH_CANCEL_TRANS)
	{
		if (pItem->IsItemLock(ILT_AUCTION_RETURN_LOCK)) //可以返还
		{
			item_can_returned = true;
			if (pItem->GetType() == ITEM_EQUIP)
			{
				pItem->SetSealState(1);
			}
			reason = GetReason(SOURCE_TYPE_ACTION_ADUIT_CANCEL_TRANS, param);
		}
		else{
			item_can_returned = false;
			InfoLogStream << "item(" << itemId << ") not set lock ILT_AUCTION_RETURN_LOCK" << LogStream::eos;
			OnCheckItemFail(ITEM_CHECKITEM_FAIL, serialId);
		}
	}
	else if (type == CMRT_BLACKMARKET_TRANSACTION_REQ)
	{
		if (pItem->IsItemLock(ILT_ITEM_LOCK) || pItem->IsItemLock(ILT_LEASE_LOCK))
		{
			item_can_returned = false;
			InfoLogStream << "check item fail type : " << type << ", item(" << itemId << ") set lock ILT_ITEM_LOCK or ILT_LEASE_LOCK" << LogStream::eos;
			OnCheckItemFail(ITEM_CHECKITEM_FAIL, serialId);
		}
		else if (pItem->GetType() == ITEM_EQUIP && !pItem->IsPermanent())
		{
			item_can_returned = false;
			InfoLogStream << "check item fail type : " << type << ", item(" << itemId << ") is not permanent" << LogStream::eos;
			OnCheckItemFail(ITEM_CHECKITEM_FAIL, serialId);
		}
		else if (pItem->IsInAuctionCoolTimeStamp())
		{
			item_can_returned = false;
			InfoLogStream << "check item fail type : " << type << ", item(" << itemId << ") is in AuctionCoolTimeStamp" << LogStream::eos;
			OnCheckItemFail(ITEM_CHECKITEM_FAIL, serialId);
		}
		else{
			item_can_returned = true;
			reason = GetReason(SOURCE_TYPE_BLACKMARKET_AUCTION_REQ, param);
		}
	}

	if (item_can_returned)
	{
		Avalon::Packet* pPacket = Avalon::Packet::Create();
		pPacket->SetID(CLProtocol::SCENE_CHECK_ITEM_RET);
		Avalon::NetOutputStream stream(pPacket->GetBuffer(), pPacket->GetMaxSize());
		try
		{
			stream  & serialId & (UInt32)type & param & (UInt32)SUCCESS & pItem->GetID() & pItem->GetDataID();
			pItem->Encode(stream, SST_WORLD, false);
			InfoLogStream << "check item reduce item param : " << param
				<< ", item id : " << itemId << ", item type id : " << pItem->GetDataID() << ", num : " << pItem->GetNum() << LogStream::eos;
			player->ReduceItemNum(reason.c_str(), pItem, pItem->GetNum());
		}
		catch (Avalon::NetException&)
		{
			Avalon::Packet::Destroy(pPacket);
			OnCheckItemFail(ITEM_CHECKITEM_FAIL, serialId);
			return;
		}
		pPacket->SetSize(stream.GetPos());
		Router::SendToWorld(pPacket);
		Avalon::Packet::Destroy(pPacket);
		DebugLogStream << "send SCENE_CHECK_ITEM_RET msg back to world" << LogStream::eos;
	}
}

void ItemMgr::OnCheckItemFail(UInt32 errCode, UInt32 serialId)
{
	Avalon::Packet* pPacket = Avalon::Packet::Create();
	pPacket->SetID(CLProtocol::SCENE_CHECK_ITEM_RET);
	Avalon::NetOutputStream stream(pPacket->GetBuffer(), pPacket->GetMaxSize());
	try
	{
		stream  & serialId & (UInt32)0 & (UInt64)0 & (UInt32)errCode;
	}
	catch (Avalon::NetException&)
	{
		Avalon::Packet::Destroy(pPacket);
		return;
	}
	pPacket->SetSize(stream.GetPos());
	Router::SendToWorld(pPacket);
	Router::SendToWorld(pPacket);
	Avalon::Packet::Destroy(pPacket);
}

//背包附魔卡升级
UInt32 ItemMgr::UpgradeMagicCard(ObjID_t upgradeUid, ObjID_t materialUid, UInt32& cardTypeId, UInt8& cardLev, ObjID_t& cardGuid)
{
	Item* upgradeCard = GetOwner()->FindItem(upgradeUid);
	Item* materialCard = GetOwner()->FindItem(materialUid);

	if (!upgradeCard || !materialCard || !upgradeCard->GetDataEntry() || !materialCard->GetDataEntry())
		return ITEM_DATA_INVALID;

	/*if (upgradeCard->IsItemLock(ILT_ITEM_LOCK) || materialCard->IsItemLock(ILT_ITEM_LOCK))
	{
	return ErrorCode::ITEM_LOCKED;
	}*/

	// 同一张卡没有叠加数量,不允许复制
	if (upgradeCard->GetID() == materialCard->GetID() &&
		upgradeCard->GetNum() < 2)
	{
		return ITEM_DATA_INVALID;
	}

	MagicCardData* upgradeCardData = MagicCardDataMgr::Instance()->FindEntry(upgradeCard->GetDataID());
	MagicCardUpdateData* upgradeCardUpdateData = MagicCardUpdateDataMgr::Instance()->GetDataEntry(upgradeCard->GetDataID(), upgradeCard->GetStrengthen());
	MagicCardData* materialCardData = MagicCardDataMgr::Instance()->FindEntry(materialCard->GetDataID());
	if (!upgradeCardData || !materialCardData || !upgradeCardUpdateData)
	{
		return ITEM_DATA_INVALID;
	}
	if (upgradeCardData->maxLevel == 0)
	{
		return ITEM_MAGIC_CANNOT_UP;
	}
	if (upgradeCardData->maxLevel <= upgradeCard->GetStrengthen())
	{
		return ITEM_MAGIC_UP_FAIL_MAXLEV;
	}

	PackType pack = GetPackType((ItemType)upgradeCard->GetType());
	Package* pPackage = GetPackage(pack);
	if (pPackage == NULL) return ITEM_DATA_INVALID;

	if (pPackage->GetCapacity() < 1)
	{
		return ITEM_ADD_PACK_FULL;
	}

	//消耗数量, 概率
	UInt32	counmeNum = 0;
	UInt32  rate = 0;

	//是否同名卡
	bool isSameCard = false;
	for (size_t i = 0; i < upgradeCardUpdateData->sameCardIds.size(); ++i)
	{
		if (upgradeCardUpdateData->sameCardIds[i].itemId == materialCardData->GetKey())
		{
			isSameCard = true;
			counmeNum = upgradeCardUpdateData->sameCardIds[i].comuseNum;
			rate = upgradeCardUpdateData->levelAddRate[materialCard->GetStrengthen()] + upgradeCardUpdateData->sameCardRates[upgradeCard->GetStrengthen()];
			break;
		}
	}

	bool isCanUseMate = false;
	if (!isSameCard) //非同名卡
	{
		UInt8 mateCardQuality = materialCardData->quality; //副卡品质
		auto iter = upgradeCardUpdateData->upgradeMaterials.find(mateCardQuality);
		if (iter != upgradeCardUpdateData->upgradeMaterials.end())
		{
			std::vector<MagicUpgradeMaterial>& upMaters = iter->second;
			for (size_t i = 0; i < upMaters.size(); ++i)
			{
				if (upMaters[i].color == materialCardData->color)
				{
					isCanUseMate = true;
					counmeNum = upMaters[i].num;
					rate = upgradeCardUpdateData->levelAddRate[materialCard->GetStrengthen()] + upgradeCardUpdateData->upgradeMaterialsRates[mateCardQuality][upgradeCard->GetStrengthen()];
					break;
				}
			}
		}
	}

	if ((!isCanUseMate && !isSameCard) || counmeNum == 0)
	{
		return ITEM_MAGIC_UP_FAIL_MATERCARD_TYPE;
	}

	//GetOwner()->SendNotify("up magic rate = {0}", rate);
	
	if (upgradeCard->GetID() == materialCard->GetID())
	{
		if (materialCard->GetNum() < counmeNum + 1)
		{
			return ITEM_MAGIC_UP_FAIL_MATERCARD_NUM;
		}
	}
	else if (materialCard->GetNum() < counmeNum)
	{
		return ITEM_MAGIC_UP_FAIL_MATERCARD_NUM;
	}

	if (GetOwner()->GetItemNum(upgradeCardUpdateData->upgradeConsume.id) < upgradeCardUpdateData->upgradeConsume.num)
	{
		if (upgradeCardUpdateData->upgradeConsume.id == BIND_GOLD_ID)
		{
			if (GetOwner()->GetGoldBoth() < upgradeCardUpdateData->upgradeConsume.num)
			{
				return ITEM_MAGIC_UP_FAIL_MONEY;
			}
		}
		else{
			return ITEM_MAGIC_UP_FAIL_MONEY;
		}
	}

	cardTypeId = upgradeCard->GetDataID();
	cardLev = upgradeCard->GetStrengthen();
	UInt8 oldColor = upgradeCard->GetQuality();
	UInt8 oldLv = cardLev;
	UInt32 consumeId = materialCardData->GetKey();

	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_MAGIC_UP, upgradeCard->GetDataID(), materialCard->GetDataID());

	if (Avalon::Random::SelectInThousand(rate))
	{
		//升级成功,确定升级后的附魔卡id
		UInt8 newLev = upgradeCard->GetStrengthen() + 1;
		UInt32 cardDataId = upgradeCard->GetDataID();

		//副卡非绑,主卡不是角色绑定
		if (materialCard->GetBind() != 1 && upgradeCard->GetBind() !=2)
		{
			for (size_t i = 0; i < upgradeCardUpdateData->sameCardIds.size(); ++i)
			{
				ItemDataEntry* data = ItemDataEntryMgr::Instance()->FindEntry(upgradeCardUpdateData->sameCardIds[i].itemId);
				if (data && data->owner == 2)
				{
					cardDataId = upgradeCardUpdateData->sameCardIds[i].itemId;
					break;
				}
			}
		}

		cardTypeId = cardDataId;
		cardLev = newLev;

		//检查背包
		if (false == GetOwner()->CheckAddItem(cardDataId, 1, newLev))
			return ITEM_ADD_PACK_FULL;
		
		if (upgradeCardUpdateData->upgradeConsume.id == BIND_GOLD_ID)
		{
			if (!GetOwner()->RemoveGoldBoth(reason.c_str(), upgradeCardUpdateData->upgradeConsume.num))
			{
				return ITEM_DATA_INVALID;
			}
		}
		else{
			if (!GetOwner()->RemoveItem(reason.c_str(), upgradeCardUpdateData->upgradeConsume.id, upgradeCardUpdateData->upgradeConsume.num))
			{
				return ITEM_DATA_INVALID;
			}
		}
		
		if (SUCCESS != GetOwner()->ReduceItemNum(reason.c_str(), materialCard, counmeNum))
		{
			return ITEM_DATA_INVALID;
		}

		if (SUCCESS != GetOwner()->ReduceItemNum(reason.c_str(), upgradeCard, 1))
		{
			return ITEM_DATA_INVALID;
		}
		std::vector<ObjID_t> guids;
		GetOwner()->AddItemGetGuid(reason.c_str(), cardDataId, 1, guids, 0, newLev, false);
		if (guids.size() > 0)
		{
			cardGuid = guids[0];
		}

		GetOwner()->SendUdpLog("up_magic", LOG_ITEM_UP_MAGIC, 1, upgradeCardData->GetKey(), oldLv, cardDataId, newLev, consumeId, counmeNum, oldColor);
	}
	else
	{
		//升级失败
		if (SUCCESS != GetOwner()->ReduceItemNum(reason.c_str(), materialCard, counmeNum))
		{
			return ITEM_DATA_INVALID;
		}

		if (upgradeCardUpdateData->upgradeConsume.id == BIND_GOLD_ID)
		{
			if (!GetOwner()->RemoveGoldBoth(reason.c_str(), upgradeCardUpdateData->upgradeConsume.num))
			{
				return ITEM_DATA_INVALID;
			}
		}
		else{
			if (!GetOwner()->RemoveItem(reason.c_str(), upgradeCardUpdateData->upgradeConsume.id, upgradeCardUpdateData->upgradeConsume.num))
			{
				return ITEM_DATA_INVALID;
			}
		}

		GetOwner()->SendUdpLog("up_magic", LOG_ITEM_UP_MAGIC, 0, upgradeCardUpdateData->GetKey(), oldLv, upgradeCardUpdateData->GetKey(), oldLv, consumeId, counmeNum, oldColor);
		return ITEM_MAGIC_UP_FAIL;
	}
	return ErrorCode::SUCCESS;
}

//装备附魔卡升级
UInt32 ItemMgr::UpgradeMountMagicCard(ObjID_t equipUid, UInt32 cardId, ObjID_t materialUid, UInt32& cardTypeId, UInt8& cardLev)
{
	Item* equip = GetOwner()->FindItem(equipUid);
	Item* materialCard = GetOwner()->FindItem(materialUid);

	if (!equip || !materialCard || !equip->GetDataEntry() || !materialCard->GetDataEntry())
		return ITEM_DATA_INVALID;

	/*if (equip->IsItemLock(ILT_ITEM_LOCK) || materialCard->IsItemLock(ILT_ITEM_LOCK))
	{
	return ErrorCode::ITEM_LOCKED;
	}*/

	UInt32 mountedCardId = equip->GetMagicCardId();
	UInt32 mountedCardLv = equip->GetMagicCardLv();
	cardTypeId = mountedCardId;
	cardLev = mountedCardLv;
	UInt8 oldLv = mountedCardLv;

	ItemDataEntry* mountedCardData = ItemDataEntryMgr::Instance()->FindEntry(mountedCardId);

	if (!mountedCardData)
	{
		return ITEM_DATA_INVALID;
	}

	if (mountedCardId != cardId)
	{
		return ITEM_DATA_INVALID;
	}
	UInt8 oldColor = mountedCardData->color;
	MagicCardData* upgradeCardData = MagicCardDataMgr::Instance()->FindEntry(mountedCardId);
	MagicCardUpdateData* upgradeCardUpdateData = MagicCardUpdateDataMgr::Instance()->GetDataEntry(mountedCardId, mountedCardLv);
	MagicCardData* materialCardData = MagicCardDataMgr::Instance()->FindEntry(materialCard->GetDataID());
	if (!upgradeCardData || !materialCardData || !upgradeCardUpdateData)
	{
		return ITEM_DATA_INVALID;
	}
	if (upgradeCardData->maxLevel == 0)
	{
		return ITEM_MAGIC_CANNOT_UP;
	}
	if (upgradeCardData->maxLevel <= mountedCardLv)
	{
		return ITEM_MAGIC_UP_FAIL_MAXLEV;
	}

	//消耗数量, 概率
	UInt32	counmeNum = 0;
	UInt32  rate = 0;
	UInt32 consumeId = materialCardData->GetKey();

	//是否同名卡
	bool isSameCard = false;
	for (size_t i = 0; i < upgradeCardUpdateData->sameCardIds.size(); ++i)
	{
		if (upgradeCardUpdateData->sameCardIds[i].itemId == materialCardData->GetKey())
		{
			isSameCard = true;
			counmeNum = upgradeCardUpdateData->sameCardIds[i].comuseNum;
			rate = upgradeCardUpdateData->levelAddRate[materialCard->GetStrengthen()] + upgradeCardUpdateData->sameCardRates[mountedCardLv];
			break;
		}
	}

	bool isCanUseMate = false;
	if (!isSameCard) //非同名卡
	{
		UInt8 mateCardQuality = materialCardData->quality; //副卡品质
		auto iter = upgradeCardUpdateData->upgradeMaterials.find(mateCardQuality);
		if (iter != upgradeCardUpdateData->upgradeMaterials.end())
		{
			std::vector<MagicUpgradeMaterial>& upMaters = iter->second;
			for (size_t i = 0; i < upMaters.size(); ++i)
			{
				if (upMaters[i].color == materialCardData->color)
				{
					isCanUseMate = true;
					counmeNum = upMaters[i].num;
					rate = upgradeCardUpdateData->levelAddRate[materialCard->GetStrengthen()] + upgradeCardUpdateData->upgradeMaterialsRates[mateCardQuality][mountedCardLv];
					break;
				}
			}
		}
	}

	if ((!isCanUseMate && !isSameCard) || counmeNum == 0)
	{
		return ITEM_MAGIC_UP_FAIL_MATERCARD_TYPE;
	}

	//GetOwner()->SendNotify("up magic rate = {0}", rate);

	if (materialCard->GetNum() < counmeNum)
	{
		return ITEM_MAGIC_UP_FAIL_MATERCARD_NUM;
	}

	if (GetOwner()->GetItemNum(upgradeCardUpdateData->upgradeConsume.id) < upgradeCardUpdateData->upgradeConsume.num)
	{
		if (upgradeCardUpdateData->upgradeConsume.id == BIND_GOLD_ID)
		{
			if (GetOwner()->GetGoldBoth() < upgradeCardUpdateData->upgradeConsume.num)
			{
				return ITEM_MAGIC_UP_FAIL_MONEY;
			}
		}
		else{
			return ITEM_MAGIC_UP_FAIL_MONEY;
		}
	}

	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_MAGIC_UP, equipUid, materialCard->GetDataID());

	if (Avalon::Random::SelectInThousand(rate))
	{
		//升级成功,确定升级后的附魔卡id
		UInt8 newLev = mountedCardLv + 1;
		UInt32 cardDataId = mountedCardId;

		//副卡非绑,主卡不是角色绑定
		if (materialCard->GetBind() != 1 && mountedCardData->owner != 2)
		{
			for (size_t i = 0; i < upgradeCardUpdateData->sameCardIds.size(); ++i)
			{
				ItemDataEntry* data = ItemDataEntryMgr::Instance()->FindEntry(upgradeCardUpdateData->sameCardIds[i].itemId);
				if (data && data->owner == 2)
				{
					cardDataId = upgradeCardUpdateData->sameCardIds[i].itemId;
					break;
				}
			}
		}

		cardTypeId = cardDataId;
		cardLev = newLev;

		if (upgradeCardUpdateData->upgradeConsume.id == BIND_GOLD_ID)
		{
			if (!GetOwner()->RemoveGoldBoth(reason.c_str(), upgradeCardUpdateData->upgradeConsume.num))
			{
				return ITEM_DATA_INVALID;
			}
		}
		else{
			if (!GetOwner()->RemoveItem(reason.c_str(), upgradeCardUpdateData->upgradeConsume.id, upgradeCardUpdateData->upgradeConsume.num))
			{
				return ITEM_DATA_INVALID;
			}
		}

		if (SUCCESS != GetOwner()->ReduceItemNum(reason.c_str(), materialCard, counmeNum))
		{
			return ITEM_DATA_INVALID;
		}

		equip->SetMagicIdAndLv(cardDataId, newLev);
		//计算装备分数
		CalculateValuedScore(equip);
		equip->SyncProperty(GetOwner());
		GetOwner()->SendUdpLog("up_magic", LOG_ITEM_UP_MAGIC, 1, upgradeCardData->GetKey(), oldLv, cardDataId, newLev, consumeId, counmeNum, oldColor);
	}
	else
	{
		//升级失败
		if (SUCCESS != GetOwner()->ReduceItemNum(reason.c_str(), materialCard, counmeNum))
		{
			return ITEM_DATA_INVALID;
		}

		if (upgradeCardUpdateData->upgradeConsume.id == BIND_GOLD_ID)
		{
			if (!GetOwner()->RemoveGoldBoth(reason.c_str(), upgradeCardUpdateData->upgradeConsume.num))
			{
				return ITEM_DATA_INVALID;
			}
		}
		else {
			if (!GetOwner()->RemoveItem(reason.c_str(), upgradeCardUpdateData->upgradeConsume.id, upgradeCardUpdateData->upgradeConsume.num))
			{
				return ITEM_DATA_INVALID;
			}
		}

		GetOwner()->SendUdpLog("up_magic", LOG_ITEM_UP_MAGIC, 0, upgradeCardData->GetKey(), oldLv, upgradeCardData->GetKey(), oldLv, consumeId, counmeNum, oldColor);
		return ITEM_MAGIC_UP_FAIL;
	}
	return ErrorCode::SUCCESS;
}

UInt32 ItemMgr::ComposeMagicCardOneKey(UInt8 isCompWhite, UInt8 isCompBlue, UInt8 autoUseGold, UInt8 compNotBind,
	UInt8& endReason, UInt32& compTimes, UInt32& consumeBindGolds, UInt32& comsumeGolds, std::vector<ItemReward>& items)
{
	if (isCompWhite != 1 && isCompBlue != 1)
	{
		return ITEM_DATA_INVALID;
	}

	UInt32	compMaxTimes = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_MAGIC_COMP_ONES_MAXTIMES);
	compMaxTimes = (compMaxTimes == 0) ? 50 : compMaxTimes;

	//获取背包中的所有白色或蓝色附魔卡
	std::vector<Item*> cards;
	PackType packType = GetPackType(ITEM_EXPENDABLE);
	Package* packet = GetPackage(packType);
	if (!packet)
	{
		return ITEM_DATA_INVALID;
	}

	if (packet->GetCapacity() == 0)
	{
		return  ITEM_ADD_PACK_FULL;
	}

	class GetItemsVisitor : public ItemVisitor
	{
	public:
		GetItemsVisitor(UInt8 isCompWhite_, UInt8 isCompBlue_, UInt8 compNotBind_, std::vector<Item*>& cards_)
			: isCompWhite(isCompWhite_), isCompBlue(isCompBlue_), compNotBind(compNotBind_), cards(cards_){}
		~GetItemsVisitor(){}

		bool operator()(Item* item)
		{
			if (item->GetSubType() != ST_MAGICCARD)
				return true;
			
			if (item->GetStrengthen() > 0)
			{
				return true;
			}

			if (item->GetBind() == 1 && compNotBind == 0)
			{
				return true;
			}

			if ((isCompWhite && item->GetQuality() == ITEM_QUALITY_WHITE)
				|| (isCompBlue && item->GetQuality() == ITEM_QUALITY_BLUE))
			{
				cards.push_back(item);
				return true;
			}
			return true;
		}
	public:
		UInt8	isCompWhite;
		UInt8	isCompBlue;
		UInt8   compNotBind;
		std::vector<Item*>& cards;
	};

	GetItemsVisitor visitor(isCompWhite, isCompBlue, compNotBind, cards);
	packet->VisitItems(visitor);
	if (cards.size() == 0)
	{
		return ErrorCode::ITEM_MAGIC_COMP_ONEKEY_FAIL_NOITEMS;
	}

	std::sort(cards.begin(), cards.end(), [](const Item* a, const Item* b)
	{
		if (a->GetQuality() != b->GetQuality())
		{
			return a->GetQuality() < b->GetQuality();
		}
		if (a->GetBind() != b->GetBind())
		{
			UInt8 ab = a->GetBind();
			UInt8 bb = b->GetBind();
			ab = (ab == 1) ? 4 : ab;
			bb = (bb == 1) ? 4 : bb;
			return ab < bb;
			//return a->GetBind() < b->GetBind();
		}
		if (a->GetNum() != b->GetNum())
		{
			return a->GetNum() < b->GetNum();
		}
		return a->GetDataID() < b->GetDataID();
	});

	struct compGuid
	{
		ObjID_t cardId1;
		ObjID_t cardId2;
	};

	std::vector<compGuid> compGuidv;
	bool dec = false;
	for (size_t i = 0; i < cards.size(); ++i)
	{
		if (compGuidv.size() >= compMaxTimes)
		{
			break;
		}
		Item* card = cards[i];
		UInt32 pair = 0;
		UInt32 num = card->GetNum();
		if (dec)
		{
			num -= 1;
			dec = false;
		}
		if (num == 0)
		{
			continue;
		}

		pair = num / 2;

		for (UInt32 i = 0; i < pair; ++i)
		{
			compGuid cmp;
			cmp.cardId1 = card->GetID();
			cmp.cardId2 = card->GetID();
			compGuidv.push_back(cmp);
			if (compGuidv.size() >= compMaxTimes)
			{
				break;
			}
		}

		if (compGuidv.size() >= compMaxTimes)
		{
			break;
		}

		UInt8 remain = num % 2;

		if (remain == 1 && i < cards.size() - 1)
		{
			Item* nextCard = cards[i + 1];
			if (nextCard->GetQuality() != card->GetQuality())
			{
				continue;
			}
			compGuid cmp;
			cmp.cardId1 = card->GetID();
			cmp.cardId2 = nextCard->GetID();
			compGuidv.push_back(cmp);
			dec = true;
		}
	}

	//合成
	compTimes = 0;
	consumeBindGolds = 0;
	comsumeGolds = 0;
	std::map<UInt32, UInt32> compCards;
	for (size_t i = 0; i < compGuidv.size(); ++i)
	{
		Item* card1 = FindItem(compGuidv[i].cardId1);
		Item* card2 = FindItem(compGuidv[i].cardId2);
		if (!card1 || !card1)
		{
			continue;
		}
		UInt32 compCardId = 0;
		UInt32 consumeBindGold = 0;
		UInt32 comsumeGold = 0;
		UInt8  cardLev = 0;
		UInt32 res = ComposeMagicCard(card1, card2, compCardId, consumeBindGold, comsumeGold, cardLev, autoUseGold, true);
		if (res == SUCCESS)
		{
			compTimes += 1;
			consumeBindGolds += consumeBindGold;
			comsumeGolds += comsumeGold;
			compCards[compCardId] += 1;
			GetOwner()->OnAddMagicCard();
		}
		else if (res == ITEM_NOT_ENOUGH_MONEY)
		{
			endReason = MCCER_MONEY_INSUFF;
			break;
		}
		else if (res == ITEM_ADD_PACK_FULL)
		{
			endReason = MCCER_ITEMPACK_FULL;
			break;
		}
		else
		{
			endReason = MCCER_TIMES_MAX;
			break;
		}
	}

	if (compTimes == compGuidv.size())
	{
		endReason = MCCER_TIMES_MAX;
	}

	for (auto compCard : compCards)
	{
		items.push_back(ItemReward(compCard.first, compCard.second, 0, 0));
	}

	return ErrorCode::SUCCESS;
}

void ItemMgr::ComposeMgaicGM(UInt32 card1, UInt32 card2, UInt32 num)
{
	std::map<UInt32, UInt32> compCardIds;
	for (UInt32 i = 0; i < num; ++i)
	{
		MagicCardData* card_1_Data = MagicCardDataMgr::Instance()->FindEntry(card1);
		MagicCardData* card_2_Data = MagicCardDataMgr::Instance()->FindEntry(card2);
		if (!card_1_Data)
		{
			ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")MagicCardData is null! key = " << card1 << LogStream::eos;
			return;
		}
		if (!card_2_Data)
		{
			ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")MagicCardData is null! key = " << card2 << LogStream::eos;
			return;
		}

		bool isBind = true;
		std::vector<MagicCardData*> datas;
		MagicCardDataMgr::Instance()->GetDatasByColorAndBind(card_1_Data->color, datas, isBind);
		if (datas.size() == 0)
		{
			ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")MagicCardData size is 0! color = " << card_1_Data->color
				<< ", isBind : " << isBind << ", card_1 : " << card1 << ", card_2 : " << card2 << LogStream::eos;
			GetOwner()->SendNotify("MagicCardData size is 0! color = {0}", card_1_Data->color);
			return;
		}


		UInt32 compCardId = 0;
		UInt32 sameQualityRate = 0;
		if (card_1_Data->color == ITEM_QUALITY_MAX - 1)
			sameQualityRate = 1000;
		else
			sameQualityRate = datas[0]->rate;

		if (false == Avalon::Random::SelectInThousand(sameQualityRate))
			MagicCardDataMgr::Instance()->GetDatasByColorAndBind(card_1_Data->color + 1, datas, isBind);

		std::vector<UInt32> wts;
		for (auto data : datas)
		{
			wts.push_back(data->weight);
		}
		Int32 idx = ItemMaker::Instance()->randByWeight(wts);
		if (-1 == idx || idx >= (Int32)datas.size())
		{
			ErrorLogStream << "accid(" << GetOwner()->GetAccID() << ") playerid(" << GetOwner()->GetID() << ")wt idx err! idx = " << idx << LogStream::eos;
			GetOwner()->SendNotify("wt idx err! idx = {0}", idx);
			return;
		}
		compCardId = datas[idx]->id;

		compCardIds[compCardId] = compCardIds[compCardId] + 1;
	}

	std::string rootpath = SSApplication::Instance()->GetComposeCardPath();
	std::string accidStr = Avalon::StringUtil::ConvertToString(GetOwner()->GetAccID());
	std::string card1Str = Avalon::StringUtil::ConvertToString(card1);
	std::string card2Str = Avalon::StringUtil::ConvertToString(card2);
	std::string numStr = Avalon::StringUtil::ConvertToString(num);

	std::string postfix = accidStr + "_" + card1Str + "_" + card2Str + "_" + numStr + ".csv";
	std::string filename = rootpath + "ComposeCardCount_" + postfix;

	std::ofstream ofs;
	ofs.open(filename.c_str());
	if (!ofs.good())
	{
		ErrorLogStream << "FILE ptr is null!" << LogStream::eos;
		return;
	}
	std::string header = "itemid,name,num\n";
	ofs.write(header.c_str(), header.length());
	for (auto itr : compCardIds)
	{
		auto data = ItemDataEntryMgr::Instance()->FindEntry(itr.first);
		if (!data)
		{
			continue;
		}

		std::ostringstream oss;
		oss << itr.first << ", " << data->name << ", " << itr.second << "\n";
		std::string line = oss.str();
		ofs.write(line.c_str(), line.size());
	}

	ofs.close();
}

//检查道具使用次数上限
UInt32 ItemMgr::CheckItemUseLimitByDataId(Item* item)
{
	if (!item)
	{
		return ITEM_DATA_INVALID;
	}

	if (item->GetDataEntry()->useLinmit == 0)
	{
		return SUCCESS;
	}

	std::string counterName = ITEM_FLY_HELL_GIFT_USE_PRE + Avalon::StringUtil::ConvertToString((UInt32)item->GetDataID());

	UInt32 useNum = GetOwner()->GetCounter(counterName.c_str());

	if (useNum >= item->GetDataEntry()->useLinmit)
	{
		return ITEM_USELIMIT;
	}

	return SUCCESS;
}

void ItemMgr::OnUsedLimitedItem(Item* item)
{
	if (!item || (item->GetDataEntry() && item->GetDataEntry()->useLinmit == 0))
	{
		return;
	}

	std::string counterName = ITEM_FLY_HELL_GIFT_USE_PRE + Avalon::StringUtil::ConvertToString((UInt32)item->GetDataID());
	UInt32 useNum = GetOwner()->GetCounter(counterName.c_str());
	GetOwner()->SetCounter(counterName.c_str(), useNum + 1);
}

bool ItemMgr::CheckLimitedItemDel(Item* item)
{
	if (!item)
	{
		return false;
	}

	if (item->GetDataEntry() && item->GetDataEntry()->useLinmit == 0)
	{
		return true;
	}

	std::string counterName = ITEM_FLY_HELL_GIFT_USE_PRE + Avalon::StringUtil::ConvertToString((UInt32)item->GetDataID());
	UInt32 useNum = GetOwner()->GetCounter(counterName.c_str());

	if (useNum >= item->GetDataEntry()->useLinmit)
	{
		return true;
	}

	return false;
}

UInt32 ItemMgr::UseTeamCopyItem(Item* item, ItemDataEntry* itemData)
{
	if (NULL == item || NULL == itemData)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}

	switch (itemData->thirdType)
	{
	case TC_TYPE_ONE :
	{
		if (!GetOwner()->CheckFuncFlag(FUNC_TEAM_COPY)) 
			return ErrorCode::TEAM_COPY_FUN_LOCK;
	}
	break;
	case TC_TYPE_TWO : 
	{
		if(!GetOwner()->CheckFuncFlag(FUNC_TEAM_COPY_TWO))
			return ErrorCode::TEAM_COPY_FUN_LOCK;
	}
	break;
	default:return ErrorCode::ITEM_TYPE_INVALID;
	}

	if (ErrorCode::SUCCESS != GetOwner()->ReduceItemNum(GetReason(LogSourceType::SOURCE_TYPE_ITEM).c_str(), item, 1))
	{
		return ErrorCode::ITEM_USE_FAIL;
	}

	if (itemData->subType == ST_TEAM_COPY_CNT)
	{
		GetOwner()->GetTeamCopyMgr().UseTeamCopyItem(itemData->thirdType, TEAM_COPY_TEAM_GRADE_COMMON, 1);
	}
	else if (itemData->subType == ST_TEAM_COPY_DIFF_CNT)
	{
		GetOwner()->GetTeamCopyMgr().UseTeamCopyItem(itemData->thirdType, TEAM_COPY_TEAM_GRADE_DIFF, 1);
	}

	return ErrorCode::SUCCESS;
}

UInt32 ItemMgr::HandleEquipConvert(UInt8 type, UInt64 soucreEqGuid, UInt32 targetEqTypeId, UInt64 convertorGuid, UInt64& eqGuid)
{
	if (EQCTP_SAME == type)
	{
		return HandleEquipConvertSomeSuit(type, soucreEqGuid, targetEqTypeId, convertorGuid, eqGuid);
	}
	else if(EQCTP_DIFF == type)
	{
		return HandleEquipConvertDiffSuit(type, soucreEqGuid, targetEqTypeId, convertorGuid, eqGuid);
	}
	return ErrorCode::ITEM_DATA_INVALID;
}

UInt32 ItemMgr::HandleEquipConvertSomeSuit(UInt8 type, UInt64 soucreEqGuid, UInt32 targetEqTypeId, UInt64 convertorGuid, UInt64& eqGuid)
{
	Item* soucreEq = FindItem(soucreEqGuid);
	if (!soucreEq)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}
	Item* convertor = NULL;
	if (convertorGuid > 0)
	{
		convertor = FindItem(convertorGuid);
		if (!convertor)
		{
			return  ErrorCode::ITEM_DATA_INVALID;
		}
	}

	if (SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_EQUIP_SCHEME) && soucreEq->IsCanPushScheme() && GetOwner()->GetEqSchemeMgr().IsEquipInScheme(soucreEqGuid))
	{
		return ITEM_IILEG_OPER_IN_SCHEME;
	}
	if (soucreEq->GetDataID() == targetEqTypeId)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}
	ItemDataEntry* targetData = ItemDataEntryMgr::Instance()->FindEntry(targetEqTypeId);
	if (!targetData)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}
	UInt32 suitid1 = EquipChangeDataEntryMgr::Instance()->GetSuitIdByEqId(soucreEq->GetDataID());
	UInt32 suitid2 = EquipChangeDataEntryMgr::Instance()->GetSuitIdByEqId(targetEqTypeId);
	if (suitid1 == 0 || suitid2 == 0)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}
	if (suitid1 != suitid2)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}
	EquipChangeDataEntry* eqChgeData = EquipChangeDataEntryMgr::Instance()->FindEntry(suitid1);
	if (!eqChgeData)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}
	//职业检查
	if (eqChgeData->jobIds.size() > 0)
	{
		bool find = false;
		for (auto occu : eqChgeData->jobIds)
		{
			if (occu == GetOwner()->GetOccu())
			{
				find = true;
				break;
			}
		}
		if (!find)
		{
			return ErrorCode::ITEM_DATA_INVALID;
		}
	}
	if (targetData->subType == soucreEq->GetSubType())
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}
	if (targetData->thirdType != soucreEq->GetThirdType())
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}

	//检查材料
	EquipChgConsDataEntry* consumeData = EquipChgConsDataEntryMgr::Instance()->GetDataEntry(type,
		targetData->subType, targetData->needLevel, targetData->color, targetData->color2);
	if (!consumeData)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}

	if (convertorGuid > 0)
	{
		if (convertor->GetThirdType() != consumeData->convertor.threType)
		{
			return ErrorCode::ITEM_DATA_INVALID;
		}
		if (convertor->GetDataEntry()->useLimiteType != IULT_SUITLEV)
		{
			return ErrorCode::ITEM_DATA_INVALID;
		}
		if (convertor->GetDataEntry()->useLimiteValue != consumeData->convertor.suitLev)
		{
			return ErrorCode::ITEM_DATA_INVALID;
		}
		if (convertor->GetNum() < consumeData->convertor.coumseNum)
		{
			return ErrorCode::ITEM_NOT_ENOUGH_MAT;
		}
	}
	else
	{
		for (auto item : consumeData->consumeItems)
		{
			if (GetOwner()->GetItemNum(item.id) < item.num)
			{
				return ErrorCode::ITEM_NOT_ENOUGH_MAT;
			}
		}
	}

	std::string reason = GetReason(LogSourceType::SOURCE_EQUIE_COVERNT, soucreEqGuid, targetEqTypeId);

	//扣材料
	if (convertorGuid > 0)
	{
		GetOwner()->RemoveItem(reason.c_str(), convertor->GetDataID(), consumeData->convertor.coumseNum);
	}
	else
	{
		for (auto item : consumeData->consumeItems)
		{
			GetOwner()->RemoveItem(reason.c_str(), item.id, item.num);
		}
	}

	//发日志
	GetOwner()->SendEquipConvertUdpLog(soucreEqGuid, soucreEq->GetDataID(), soucreEq->GetSubType(), soucreEq->GetEquipType(), soucreEq->GetStrengthen(),
		soucreEq->GetPreciosBeadId(), soucreEq->GetMagicCardId(), soucreEq->GetInscriptionHoleProperty().GetInscriptionId(1), targetEqTypeId,
		targetData->subType, 1);

	eqGuid = soucreEqGuid;

	soucreEq->ChangeDataID(targetEqTypeId);
	soucreEq->SetDataEntry(targetData);
	soucreEq->SetName(targetData->name);
	soucreEq->SetIsTreas(targetData->isTreasure && SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_TREAS));

	soucreEq->SetSealState(targetData->seal);
	soucreEq->SetSealCount(0);

	//清除附魔卡，宝珠，铭文
	if (soucreEq->GetMagicCardId() > 0)
	{
		soucreEq->ClearMagic();
	}
	soucreEq->ClearPrecBead();
	soucreEq->GetInscriptionHoleProperty().Clear();
	soucreEq->EquipInscriptionModifyByEqChage();

	soucreEq->ClearAttribute();
	soucreEq->SetAttribute();

	//计算装备分数
	CalculateValuedScore(soucreEq);

	//同步属性
	soucreEq->SyncProperty(GetOwner());
	return ErrorCode::SUCCESS;
}

UInt32 ItemMgr::HandleEquipConvertDiffSuit(UInt8 type, UInt64 soucreEqGuid, UInt32 targetEqTypeId, UInt64 convertorGuid, UInt64& eqGuid)
{
	Item* soucreEq = FindItem(soucreEqGuid);
	if (!soucreEq)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}
	Item* convertor = NULL;
	if (convertorGuid > 0)
	{
		convertor = FindItem(convertorGuid);
		if (!convertor)
		{
			return  ErrorCode::ITEM_DATA_INVALID;
		}
	}

	if (SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_EQUIP_SCHEME) && soucreEq->IsCanPushScheme() && GetOwner()->GetEqSchemeMgr().IsEquipInScheme(soucreEqGuid))
	{
		return ITEM_IILEG_OPER_IN_SCHEME;
	}
	if (soucreEq->GetDataID() == targetEqTypeId)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}
	ItemDataEntry* targetData = ItemDataEntryMgr::Instance()->FindEntry(targetEqTypeId);
	if (!targetData)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}
	UInt32 suitid1 = EquipChangeDataEntryMgr::Instance()->GetSuitIdByEqId(soucreEq->GetDataID());
	UInt32 suitid2 = EquipChangeDataEntryMgr::Instance()->GetSuitIdByEqId(targetEqTypeId);
	if (suitid1 == 0 || suitid2 == 0)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}
	if (suitid1 == suitid2)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}

	//职业检查
	EquipChangeDataEntry* sourceChData = EquipChangeDataEntryMgr::Instance()->FindEntry(suitid1);
	if (!sourceChData)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}
	EquipChangeDataEntry* targetChData = EquipChangeDataEntryMgr::Instance()->FindEntry(suitid2);
	if (!targetChData)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}
	
	for (auto occu : sourceChData->jobIds)
	{
		if (occu == GetOwner()->GetOccu())
		{
			return ErrorCode::ITEM_DATA_INVALID;
		}
	}
	bool find = false;
	for (auto occu : targetChData->jobIds)
	{
		if (occu == GetOwner()->GetOccu())
		{
			find = true;
			break;
		}
	}
	if (!find)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}

	if (targetData->subType != soucreEq->GetSubType())
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}

	//检查材料
	EquipChgConsDataEntry* consumeData = EquipChgConsDataEntryMgr::Instance()->GetDataEntry(type,
		targetData->subType, targetData->needLevel, targetData->color, targetData->color2);
	if (!consumeData)
	{
		return ErrorCode::ITEM_DATA_INVALID;
	}

	if (convertorGuid > 0)
	{
		if (convertor->GetThirdType() != consumeData->convertor.threType)
		{
			return ErrorCode::ITEM_DATA_INVALID;
		}
		if (convertor->GetDataEntry()->useLimiteType != IULT_SUITLEV)
		{
			return ErrorCode::ITEM_DATA_INVALID;
		}
		if (convertor->GetDataEntry()->useLimiteValue != consumeData->convertor.suitLev)
		{
			return ErrorCode::ITEM_DATA_INVALID;
		}
		if (convertor->GetNum() < consumeData->convertor.coumseNum)
		{
			return ErrorCode::ITEM_NOT_ENOUGH_MAT;
		}
	}
	else
	{
		for (auto item : consumeData->consumeItems)
		{
			if (GetOwner()->GetItemNum(item.id) < item.num)
			{
				return ErrorCode::ITEM_NOT_ENOUGH_MAT;
			}
		}
	}

	std::string reason = GetReason(LogSourceType::SOURCE_EQUIE_COVERNT, soucreEqGuid, targetEqTypeId);

	//扣材料
	if (convertorGuid > 0)
	{
		GetOwner()->RemoveItem(reason.c_str(), convertor->GetDataID(), consumeData->convertor.coumseNum);
	}
	else
	{
		for (auto item : consumeData->consumeItems)
		{
			GetOwner()->RemoveItem(reason.c_str(), item.id, item.num);
		}
	}

	//发日志
	GetOwner()->SendEquipConvertUdpLog(soucreEqGuid, soucreEq->GetDataID(), soucreEq->GetSubType(), soucreEq->GetEquipType(), soucreEq->GetStrengthen(),
		soucreEq->GetPreciosBeadId(), soucreEq->GetMagicCardId(), soucreEq->GetInscriptionHoleProperty().GetInscriptionId(1), targetEqTypeId,
		targetData->subType, 2);

	eqGuid = soucreEqGuid;

	soucreEq->ChangeDataID(targetEqTypeId);
	soucreEq->SetDataEntry(targetData);
	soucreEq->SetName(targetData->name);
	soucreEq->SetIsTreas(targetData->isTreasure && SSServiceSwitchMgr::Instance()->IsOpen(SERVICE_AUCTION_TREAS));

	soucreEq->SetSealState(targetData->seal);
	soucreEq->SetSealCount(0);

	//清除附魔卡，宝珠，铭文
	if (soucreEq->GetMagicCardId() > 0)
	{
		soucreEq->ClearMagic();
	}
	soucreEq->ClearPrecBead();
	soucreEq->GetInscriptionHoleProperty().Clear();

	soucreEq->ClearAttribute();
	soucreEq->SetAttribute();

	//计算装备分数
	CalculateValuedScore(soucreEq);

	//同步属性
	soucreEq->SyncProperty(GetOwner());
	return ErrorCode::SUCCESS;
}