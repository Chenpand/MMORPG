#include "WSShopItem.h"

#include <CLRecordClient.h>
#include <CLAccountShopItemDataEntry.h>
#include "WSPlayerMgr.h"


WSShopItem::WSShopItem()
{
	m_ShopItemId = 0;
	m_Owner = 0;
	m_CreateTime = 0;
	m_UpdateTime = 0;
}

WSShopItem::~WSShopItem()
{

}

bool WSShopItem::InitByShopItemData(WSPlayer* player, const AccountShopItemDataEntry* data)
{
	if (!player || !data) return false;

	SetShopItemId(data->id);
	SetOwner(player->GetAccId());

	return true;
}

const std::string WSShopItem::LogStr()
{
	std::ostringstream oss;
	oss << " account(" << GetOwner() << ") shop item(" << GetID() << "," << GetShopItemId() << ")";
	return oss.str();
}

void WSShopItem::SaveToDB()
{
	if (GetID() == 0)
	{
		SetID(CLRecordClient::Instance()->GenGuid());
		SetCreateTime(CURRENT_TIME.MSec());
		auto cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_account_shop_item", GetID());
		GetDBData(cmd, false);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
	else
	{
		auto cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_account_shop_item", GetID(), true);
		GetDBData(cmd, true);
		if (cmd->GetDataNum() > 0)
		{
			CLRecordClient::Instance()->SendCommand(cmd);
		}
		else
		{
			CL_SAFE_DELETE(cmd);
		}
	}
}