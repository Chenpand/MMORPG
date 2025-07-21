#include "ShopItem.h"
#include <CLRecordClient.h>
#include "Player.h"
#include "ScriptMgr.h"
#include "FinalEquipScript.h"

ShopItem::ShopItem()
{
	m_pDataEntry = NULL;
	m_shopId = 0;
	m_owner = 0;
	m_grid = 0;
	m_restNum = 0;
	m_vipLv = 0;
	m_vipDiscount = 0;
	m_DiscountRate = 0;
}

ShopItem::~ShopItem()
{
}

ShopItem* ShopItem::Create(UInt32 id)
{
	ShopItemDataEntry* dataEntry = ShopItemDataEntryMgr::Instance()->FindEntry(id);
	if(dataEntry == NULL) return NULL;

	ShopItem* item = new ShopItem();
	item->SetDataEntry(dataEntry);
	return item;
}

void ShopItem::Destroy(ShopItem*& item)
{
	CL_SAFE_DELETE(item);
}

void ShopItem::SyncProperty(Player* player, bool bFlush /*= false*/)
{
	//同步到玩家
// 	Avalon::Packet* packet = Avalon::Packet::Create();
// 	packet->SetID(CLProtocol::SCENE_SHOPITEM_SYNC);
// 
// 	Avalon::NetOutputStream stream(packet->GetBuffer() + packet->GetSize(), packet->GetMaxSize() - packet->GetSize());
// 	try
// 	{
// 		stream & GetID();
// 		Encode(stream, SST_SELF, true);
// 		stream & ObjID_t(0);
// 	}
// 	catch (const Avalon::NetException& exception)
// 	{
// 		ErrorLogStream << "player(" << player->GetID() << "," << player->GetName()
// 			<< ") encode ShopItems failed:" << exception.what() << "!" << LogStream::eos;
// 		Avalon::Packet::Destroy(packet);
// 		return;
// 	}
// 	packet->SetSize(UInt16(stream.GetPos()));
// 	player->SendPacket(packet);
// 
// 	Avalon::Packet::Destroy(packet);

	//同步到数据库
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand(player->GetSrcZoneId(), "t_shopitem", GetID(), bFlush);
	GetDBData(cmd, true);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void ShopItem::ReduceRestNum(UInt32 num)
{
	if (m_restNum > 0)
	{
		m_restNum = m_restNum - num;
	}
}

void ShopItem::CalculateDiscountRate()
{
	if (!m_pDataEntry) return;

	if (m_pDataEntry->discountRate.empty()) return;

	// 配置一个就不用随机了
	if (m_pDataEntry->discountRate.size() == 1)
	{
		m_DiscountRate = m_pDataEntry->discountRate.front();
		return;
	}

	if (m_pDataEntry->discountRate.size() != m_pDataEntry->discountRateWeight.size())
	{
		return;
	}

	// 权重集
	std::vector<UInt32> wts;
	// 随机阈值
	Int32 randThreshold = m_pDataEntry->discountRateWeight.size();

	for (auto wt : m_pDataEntry->discountRateWeight)
	{
		wts.push_back(wt);
	}

	if (wts.empty())
	{
		ErrorLogStream << "Weights are empty!" << LogStream::eos;
		return;
	}

	Int32 hitIdx = WeightRander::RandWeight(wts);
	if (hitIdx >= randThreshold || hitIdx < 0)
	{
		ErrorLogStream << "Rand weight failed, hitIdx(" << hitIdx << ") is invalid!" << LogStream::eos;
		return;
	}

	m_DiscountRate = m_pDataEntry->discountRate[hitIdx];
}
