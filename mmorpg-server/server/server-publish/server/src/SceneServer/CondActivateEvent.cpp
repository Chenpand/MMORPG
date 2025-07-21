#include "CondActivateEvent.h"

#include <CLMallProtocol.h>

#include "SSRouter.h"
#include "Player.h"
#include "PlayerMgr.h"


/**
*@brief 查询条件中商城商品回调
*/
class SelectCondMallGoodsCallback : public CLRecordCallback
{
	AVALON_DEFINE_OBJECTPOOL(SelectCondMallGoodsCallback, Avalon::Mutex)
public:
	SelectCondMallGoodsCallback(UInt64 roleId, UInt32 goodsId) : m_RoleId(roleId), m_MallGoodsId(goodsId) {}
	~SelectCondMallGoodsCallback() {}

public:
	void OnFinished()
	{
		Player* player = PlayerMgr::Instance()->FindNormalPlayer(m_RoleId);
		if (!player)
		{
			ErrorLogStream << "Failed to find normal player(roleId: " << m_RoleId << ")!" << LogStream::eos;
			return;
		}

		UInt32 itemNum = 0;

		do{
			std::string counterName = GetData("name").GetString();
			UInt32 mallGoodsId = Avalon::StringUtil::ConvertToValue<UInt32>(counterName);

			if (mallGoodsId == m_MallGoodsId)
			{
				itemNum++;
			}

		} while (NextRow());

		if (itemNum != 0)
		{
			return;
		}

		UInt16 weaponNum = ActivateCondCheckEvent::Instance()->GetEquipNumQualified(player, ST_EQUIP_WEAPON);
		if (weaponNum > 0)
		{
			ActivateCondCheckEvent::Instance()->RequestActivate(m_RoleId, (UInt8)MPTAC_ONE, (UInt8)CAT_MALL_PERSONAL_TAILOR);
			return;
		}

		//UInt16 ringNum = ActivateCondCheckEvent::Instance()->GetEquipNumQualified(player, ST_EQUIP_RING);
		//if (ringNum > 0)
		//{
		//	ActivateCondCheckEvent::Instance()->RequestActivate(m_RoleId, (UInt8)MPTAC_TWO, (UInt8)CAT_MALL_PERSONAL_TAILOR);
		//	return;
		//}

		//UInt16 necklaseNum = ActivateCondCheckEvent::Instance()->GetEquipNumQualified(player, ST_EQUIP_NECKLASE);
		//if (necklaseNum > 0)
		//{
		//	ActivateCondCheckEvent::Instance()->RequestActivate(m_RoleId, (UInt8)MPTAC_THREE, (UInt8)CAT_MALL_PERSONAL_TAILOR);
		//	return;
		//}

		//UInt16 braceletNum = ActivateCondCheckEvent::Instance()->GetEquipNumQualified(player, ST_EQUIP_BRACELET);
		//if (braceletNum > 0)
		//{
		//	ActivateCondCheckEvent::Instance()->RequestActivate(m_RoleId, (UInt8)MPTAC_FOUR, (UInt8)CAT_MALL_PERSONAL_TAILOR);
		//	return;
		//}
	}

	void OnFailed(UInt32 errorcode)
	{
		ErrorLogStream << "player(" << m_RoleId << ") select t_counter failed." << LogStream::eos;
	}

private:
	UInt64 m_RoleId;
	// 商城商品id
	UInt32 m_MallGoodsId;
};


ActivateCondCheckEvent::ActivateCondCheckEvent()
{
	m_LimitEquips = { 155410003, 155411001, 155612003, 155415001, 155417003, 155417003, 155420001, 155421001, 155292001, 155291001, 155290001 };
	m_LimitTitles = { 130193025, 130193053 };
}

ActivateCondCheckEvent::~ActivateCondCheckEvent()
{

}

void ActivateCondCheckEvent::Init()
{
	class MallPersonalTailorDataEntryVistor : public Avalon::DataEntryVisitor<MallPersonalTailorDataEntry>
	{
	public:
		bool operator()(MallPersonalTailorDataEntry* dataEntry)
		{
			if (!dataEntry)
			{
				ErrorLogStream << "MallPersonalTailorDataEntry ptr is null!" << LogStream::eos;
				return true;
			}

			datas.push_back(dataEntry);
			return true;
		}

	public:
		std::vector<MallPersonalTailorDataEntry*> datas;
	};

	MallPersonalTailorDataEntryVistor vistor;
	MallPersonalTailorDataEntryMgr::Instance()->Visit(vistor);

	for (auto data : vistor.datas)
	{
		m_PersonalTailorTableDatas.push_back(data);
	}
}

void ActivateCondCheckEvent::OnActivate(Player* player, ConditionActivateType condType, CondActivateSubType subType)
{
	if (!player)
	{
		ErrorLogStream << "Player ptr is null!" << LogStream::eos;
		return;
	}

	switch (condType)
	{
	case CAT_MALL_LIMIT_TIME_GIFT_PACK:
		break;

	case CAT_MALL_PERSONAL_TAILOR:
		_OnActivatePersonalTailor(player, subType);
		break;

	default:
		ErrorLogStream << PLAYERINFO(player) << " Error activate cond type(" << condType << ")!" << LogStream::eos;
		return;
	}
}

void ActivateCondCheckEvent::RequestActivate(UInt64 roleId, UInt8 cond, UInt8 type)
{
	CLProtocol::MallActivateCondReq protocol;
	protocol.m_RoleId = roleId;
	protocol.m_ActivateCond = cond;
	protocol.m_ActivateType = type;

	Router::SendToWorld(protocol);
	InfoLogStream << "player(roleid: " << roleId << ") request activate cond(cond: " << cond << ", type: " << type << ")." << LogStream::eos;
}

void ActivateCondCheckEvent::_OnActivatePersonalTailor(Player* player, CondActivateSubType subType)
{
	return;

	if (!player)
	{
		ErrorLogStream << "Player ptr is null!" << LogStream::eos;
		return;
	}

	UInt64 roleId = player->GetID();
	// 总的充值金额（角色）
	UInt32 totalChargeNum = player->GetTotalChargeNumOnRole();
	// 无色晶体数量
	UInt32 colorLessMatId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_STRCOLORLESS_ID);
	UInt32 colorlessCrystalNum = player->GetItemNum(colorLessMatId);
	// 有色晶体数量
	UInt32 colorMatId = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_STRCOLOR_ID);
	UInt32 colorCrystalNum = player->GetItemNum(colorMatId);
	// 绑定金币数量
	UInt32 bindGoldID = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_BINDGOLD_ID);
	UInt32 bindGoldNum = player->GetItemNum(bindGoldID);

	InfoLogStream << PLAYERINFO(player) << " On activate personal tailor(playerLevel: " << player->GetLevel() << ", subType: " << subType << ", colorlessCrystalNum: "
			<< colorlessCrystalNum << ", colorCrystalNum: " << colorCrystalNum << ", bindGoldNum: " << bindGoldNum << ")." << LogStream::eos;

	for (auto dataEntry : m_PersonalTailorTableDatas)
	{
		if (!dataEntry)
		{
			ErrorLogStream << "MallPersonalTailorDataEntry ptr is null!" << LogStream::eos;
			continue;
		}

		if (subType != CAST_ALL && dataEntry->activateSubType != (UInt8)subType)
		{
			continue;
		}

		if (dataEntry->playerLevelLimit != 0 && dataEntry->playerLevelLimit > player->GetLevel())
		{
			continue;
		}

		if (dataEntry->mallGoodsIds.empty())
		{
			ErrorLogStream << "In peronsal tailor table(cond: " << dataEntry->activateCond << "), mall goods id is empty!" << LogStream::eos;
			continue;
		}

		switch ((MallPersonalTailorActiavateCond)dataEntry->activateCond)
		{
		case MPTAC_ONE: break; // 条件1、2、3、4共用触发

		case MPTAC_TWO: break;

		case MPTAC_THREE: break;

		case MPTAC_FOUR:
			if (totalChargeNum > 0)
			{
				// 查找破空石购买记录
				CLSelectCommand* selectcounters = CLRecordClient::Instance()->PrepareSelectCommand(player->GetSrcZoneId(), "t_counter");
				selectcounters->PutCond("owner", roleId);
				CLRecordClient::Instance()->SendCommand(selectcounters, new SelectCondMallGoodsCallback(roleId, dataEntry->mallGoodsIds[0]));
			}
			break;

		case MPTAC_FIVE:
			if (totalChargeNum >= CHARGE_LEFT_BOUNDARY_VALUE && totalChargeNum <= CHARGE_RIGHT_BOUNDARY_VALUE)
			{
				if (_GetTitleNumQualified(player) == 0)
				{
					RequestActivate(roleId, (UInt8)MPTAC_FIVE, (UInt8)CAT_MALL_PERSONAL_TAILOR);
				}
			}
			break;

		case MPTAC_SIX:
			if (colorlessCrystalNum <= (UInt32)NUM_LIMIT_ONE_HUNDRED)
			{
				if (_GetStrengenEquipNumQualified(player, (UInt8)SLL_EIGHT) > 0)
				{
					RequestActivate(roleId, (UInt8)MPTAC_SIX, (UInt8)CAT_MALL_PERSONAL_TAILOR);
				}
			}
			break;

		case MPTAC_SEVEN:
			if (colorCrystalNum <= (UInt32)NUM_LIMIT_FIFTY)
			{
				if (_GetStrengenEquipNumQualified(player, (UInt8)SLL_TEN) > 0)
				{
					RequestActivate(roleId, (UInt8)MPTAC_SEVEN, (UInt8)CAT_MALL_PERSONAL_TAILOR);
				}
			}
			break;

		case MPTAC_EIGHT:
			if (bindGoldNum <= (UInt32)NUM_LIMIT_TEN_THOUNSAND)
			{
				if (_GetStrengenEquipNumQualified(player, (UInt8)SLL_EIGHT) > 0)
				{
					RequestActivate(roleId, (UInt8)MPTAC_EIGHT, (UInt8)CAT_MALL_PERSONAL_TAILOR);
				}
			}
			break;

		case MPTAC_NINE:
			if (totalChargeNum == 0)
			{
				RequestActivate(roleId, (UInt8)MPTAC_NINE, (UInt8)CAT_MALL_PERSONAL_TAILOR);
			}
			break;

		case MPTAC_TEN:
			if (_GetFashonNumQualified(player) == 0)
			{
				RequestActivate(roleId, (UInt8)MPTAC_TEN, (UInt8)CAT_MALL_PERSONAL_TAILOR);
			}
			break;

		default:
			ErrorLogStream << PLAYERINFO(player) << " Error activate cond(" << dataEntry->activateCond << ")!" << LogStream::eos;
			break;
		}
	}
}

UInt16 ActivateCondCheckEvent::GetEquipNumQualified(Player* player, ItemSubType itemSubType)
{
	UInt16 equipNum = 0;

	if (!player)
	{
		ErrorLogStream << "Player ptr is null!" << LogStream::eos;
		return equipNum;
	}

	std::vector<Item*> equipItems;
	_GetItemFormPackage(player, PACK_WEAR, equipItems);

	for (auto equipItem : equipItems)
	{
		if (!equipItem)
		{
			ErrorLogStream << PLAYERINFO(player) << " Item ptr is null!" << LogStream::eos;
			continue;
		}

// 		if (equipItem->GetSubType() != itemSubType)
// 		{
// 			continue;
// 		}

		for (auto limitId : m_LimitEquips) //过滤限制装备
		{
			if (equipItem->GetDataID() == limitId)
			{
				DebugLogStream << PLAYERINFO(player) << " had limit equip(id: " << limitId << ")." << LogStream::eos;
				return 0;
			}
		}
		equipNum++;
	}

	return equipNum;
}

UInt16 ActivateCondCheckEvent::_GetTitleNumQualified(Player* player)
{
	UInt16 titleItemNum = 0;
	if (!player)
	{
		ErrorLogStream << "Player ptr is null!" << LogStream::eos;
		return titleItemNum;
	}

	std::vector<Item*> titleItems;
	_GetItemFormPackage(player, PACK_TITLE, titleItems);

	for (auto titleItem : titleItems)
	{
		if (!titleItem)
		{
			ErrorLogStream << PLAYERINFO(player) << " Item ptr is null!" << LogStream::eos;
			continue;
		}

		for (auto limitId : m_LimitTitles)
		{
			if (titleItem->GetDataID() == limitId)
			{
				DebugLogStream << PLAYERINFO(player) << " had titleItem: " << limitId << LogStream::eos;
				titleItemNum++;
			}
		}
	}

	return titleItemNum;
}

UInt16 ActivateCondCheckEvent::_GetFashonNumQualified(Player* player)
{
	UInt16 fashonItemNum = 0;
	if (!player)
	{
		ErrorLogStream << "Player ptr is null!" << LogStream::eos;
		return fashonItemNum;
	}

	std::vector<Item*> fashonItems;
	_GetItemFormPackage(player, PACK_FASHION_WEAR, fashonItems);
	_GetItemFormPackage(player, PACK_FASHION, fashonItems);

	fashonItemNum = fashonItems.size();

	return fashonItemNum;
}

UInt16 ActivateCondCheckEvent::_GetStrengenEquipNumQualified(Player* player, UInt8 strengthLevel)
{
	UInt16 strengenEquipNum = 0;
	if (!player)
	{
		ErrorLogStream << "Player ptr is null!" << LogStream::eos;
		return strengenEquipNum;
	}

	std::vector<Item*> equipItems;
	_GetItemFormPackage(player, PACK_WEAR, equipItems);

	Package* package = player->GetItemMgr().GetPackage((UInt8)PACK_WEAR);
	if (!package)
	{
		ErrorLogStream << PLAYERINFO(player) << " Package ptr is null!" << LogStream::eos;
		return 0;
	}

	for (auto equipItem : equipItems)
	{
		if (!equipItem)
		{
			ErrorLogStream << PLAYERINFO(player) << " Item ptr is null!" << LogStream::eos;
			continue;
		}

		if (equipItem->GetStrengthen() >= strengthLevel)
		{
			strengenEquipNum++;
		}
	}

	// 去掉称号后身上的所有格子数量
	UInt32 num = package->GetSize() - 1;
	UInt32 itemSize = equipItems.size();
	DebugLogStream << PLAYERINFO(player) << " Strengen equip qualified num(" << strengenEquipNum << "), equip on ware num(" << itemSize << "), package size(" << num << "), strengthLimitLevel(" << strengthLevel << ")." << LogStream::eos;
	if (strengenEquipNum == num)
	{
		return strengenEquipNum;
	}

	return 0;
}

void ActivateCondCheckEvent::_GetItemFormPackage(Player* player, PackType packType, std::vector<Item*>& items)
{
	if (!player)
	{
		ErrorLogStream << "Player ptr is null!" << LogStream::eos;
		return;
	}

	class SelectEquipVisitor : public ItemVisitor
	{
	public:
		bool operator()(Item *item)
		{
			if (!item)
			{
				ErrorLogStream << "Item ptr is null!" << LogStream::eos;
				return true;
			}

			m_Items.push_back(item);
			return true;
		}

		std::vector<Item*>& GetItems() { return m_Items; }

	private:
		std::vector<Item*> m_Items;
	};

	SelectEquipVisitor visitor;
	player->GetItemMgr().VisitItems((UInt8)packType, visitor);

	for (auto item : visitor.GetItems())
	{
		items.push_back(item);
	}
}