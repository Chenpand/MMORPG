#include "WSItemMgr.h"
#include "WSPlayer.h"

#include <CLRecordClient.h>

WSItemMgr::WSItemMgr()
{
	m_pOwner = NULL;
	m_uLock = 0;
	m_timer.SetInterval(1000);

	memset(m_pPackages,0,sizeof(m_pPackages));

#define CREATE_PACKAGE(Type, PackT) \
	m_pPackages[Type] = new WSPackage(Type);

	CREATE_PACKAGE(TRIBE_PACK_STORAGE,	Package)
	CREATE_PACKAGE(TRIBE_PACK_SHOP,		Package)
}

WSItemMgr::~WSItemMgr()
{
#define DESTROY_PACKAGE(Type) \
	CL_SAFE_DELETE(m_pPackages[Type]);

	DESTROY_PACKAGE(TRIBE_PACK_STORAGE)
	DESTROY_PACKAGE(TRIBE_PACK_SHOP)

	//删除所有道具
	CLDeleteVisitor<WSItem> delVisitor;
	m_Items.VisitObjects(delVisitor);
	m_Items.Clear();
}

WSPackage* WSItemMgr::GetPackage(UInt8 pack) const
{
	if(pack == TRIBE_PACK_INVALID || pack >= TRIBE_PACK_MAX) return NULL;

	return m_pPackages[pack];
}

WSItem* WSItemMgr::FindItem(ObjID_t id)
{
	return m_Items.FindObject(id);
}

bool WSItemMgr::LoadItem(WSItem* item)
{
	if(item == NULL)
		return false;

	item->m_pDataEntry = ItemDataEntryMgr::Instance()->FindEntry(item->m_DataId);
	if(NULL == item->m_pDataEntry)
		return false;

	UInt8 pack = item->GetPos().pack;
	WSPackage* pPackage = GetPackage(pack);
	if(pPackage == NULL)
		return false;

	if(!pPackage->AddItem(item))
	{
		return false;
	}

	if(!m_Items.AddObject(item))
	{
		pPackage->RemoveItem(item);
		return false;
	}

	item->ClearDirty();

	return true;
}

bool WSItemMgr::AddItem(WSPlayer *pPlayer, WSItem* item, const ItemPos pos, UInt32 uPrice)
{
	if(item == NULL)
		return false;
	WSPackage* pPackage = GetPackage(pos.pack);
	if(pPackage == NULL) 
		return false;

	if(!pPackage->AddItem(item))
	{
		return false;
	}
	
	//item->id = CLRecordClient::Instance()->GenGuid();
	item->SetID(CLRecordClient::Instance()->GenGuid());

	if(!m_Items.AddObject(item))
	{
		pPackage->RemoveItem(item);
		return false;
	}

// 	item->m_uPrice = uPrice;
// 
// 	//brief 存入数据库
// 	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_tribe_item",item->id);
// 	item->GetDBData(cmd,false);
// 	cmd->PutData("itemid",item->dataId);
// 	cmd->PutData("owner",GetOwner()->GetID());
// 	cmd->PutData("pack", item->m_Pack);
// 	cmd->PutData("price", item->m_uPrice);
// 	CLRecordClient::Instance()->SendCommand(cmd);
	
	SyncToPlayer(pPlayer, item);
	return true;
}

UInt16 WSItemMgr::RemoveItem(WSPlayer *pPlayer, WSItem* item, UInt16 uNum)
{
	if(item == NULL) 
		return 0;

	if(item->m_Num > uNum)
	{
		item->m_Num = item->m_Num - uNum;
		SaveItem(item);
		SyncToPlayer(pPlayer, item);
		return uNum;
	}
	else
	{
		WSPackage* pack = GetPackage(item->m_Pack);
		if(pack != NULL) 
			pack->RemoveItem(item);

		if(m_Items.FindObject(item->GetID()) == NULL)
			return 0;

		m_Items.RemoveObject(item);

		//从数据库删除
		CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_tribe_item", item->GetID());
		CLRecordClient::Instance()->SendCommand(cmd);

		return item->m_Num;
	}
}

void WSItemMgr::RemoveItemDirect(WSItem *item)
{
	if(item == NULL) 
		return;

	WSPackage* pack = GetPackage(item->m_Pack);
	if(pack != NULL) 
		pack->RemoveItem(item);

	if(m_Items.FindObject(item->GetID()) == NULL)
		return;

	m_Items.RemoveObject(item);

	//从数据库删除
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_tribe_item", item->GetID());
	CLRecordClient::Instance()->SendCommand(cmd);
}

UInt16 WSItemMgr::GetGridUsed(UInt32 itemid, UInt16 num, UInt8 quality, UInt8 pack, UInt8 strengthen)
{
	WSPackage* pPackage = GetPackage(pack);
	if(pPackage == NULL) return 0;

	// 商店不能叠加
	if(TRIBE_PACK_SHOP == pack)
		return num;

	//查找数据项
	ItemDataEntry* dataEntry = ItemDataEntryMgr::Instance()->FindEntry(itemid);
	if(dataEntry == NULL || dataEntry->maxNum == 0) return 0;

	//不能叠加
	if(dataEntry->maxNum == 1) return num;
	
	//检查合并访问器
	class CheckAddItemVisitor : public WSItemVisitor
	{
	public:
		CheckAddItemVisitor(UInt32 id, UInt16 num, UInt8 quality, UInt8 strengthen)
			:m_ItemID(id), m_Num(num), m_Quality(quality), m_strengthen(strengthen){}
		~CheckAddItemVisitor(){}

		bool operator()(WSItem* item)
		{
			if(item->m_DataId == m_ItemID)
			{
				m_Num = m_Num - item->CheckMerging(m_Num, m_strengthen);
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
		//品质
		UInt8	m_Quality;
		//强化等级
		UInt8	m_strengthen;
	};
	CheckAddItemVisitor visitor(itemid, num, quality, strengthen);
	pPackage->VisitItems(visitor);
	num = visitor.GetNum();

	return UInt16(( num + dataEntry->maxNum - 1 ) / dataEntry->maxNum);
}

bool WSItemMgr::CheckAddItem(UInt32 itemid, UInt16 num, UInt8 quality, UInt8 pack, UInt8 strengthen)
{
	WSPackage* pPackage = GetPackage(pack);
	if(pPackage == NULL) return false;

	return pPackage->GetCapacity() >= GetGridUsed(itemid, num, quality, pack, strengthen);
}

//先合并
class AddItemVisitor : public WSItemVisitor
{
public:
	AddItemVisitor(UInt32 id, UInt16 num, UInt8 quality, UInt8 strengthen, WSPlayer* player, WSItemMgr *pItemMgr)
		:m_ItemID(id), m_Num(num), m_Quality(quality), m_strengthen(strengthen), m_pPlayer(player), m_pItemMgr(pItemMgr){}
	~AddItemVisitor(){}

	bool operator()(WSItem* item)
	{
		if(item->m_DataId == m_ItemID)
		{
			UInt16 mergenum = item->Merge(m_Num, m_strengthen);
			if(mergenum != 0) 
			{
				m_pItemMgr->SyncToPlayer(m_pPlayer, item);
				m_pItemMgr->SaveItem(item);
			}
			
			m_Num = m_Num - mergenum;
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
	//品质
	UInt8	m_Quality;
	//强化等级
	UInt8	  m_strengthen;
	//玩家
	WSPlayer*	m_pPlayer;
	//道具管理器
	WSItemMgr	*m_pItemMgr;
};

UInt16 WSItemMgr::AddItem(WSPlayer *pPlayer, WSItem* item, UInt8 pack, UInt32 uPrice)
{
	if(item == NULL)
		return 0;
	
	item->m_pDataEntry = ItemDataEntryMgr::Instance()->FindEntry(item->m_DataId);
	if(NULL == item->m_pDataEntry)
	{
		delete item;
		return 0;
	}

	WSPackage* pPackage = GetPackage(pack);
	if(pPackage == NULL)
	{
		delete item;
		return 0;
	}

	if(TRIBE_PACK_SHOP == pack)
	{
		if(!AddItem(pPlayer, item, ItemPos(pack, 0), uPrice))
		{
			delete item;
			return 0;
		}
		return item->m_Num;
	}

	UInt16 num = item->m_Num;
	AddItemVisitor visitor(item->m_DataId, num, item->m_Quality, item->GetStrengthen(), pPlayer, this);
	pPackage->VisitItems(visitor);

	UInt16 leftnum = visitor.GetNum();
	if(leftnum == 0)
	{
		delete item;
		return num;
	}

	item->m_Num = leftnum;
	if(!AddItem(pPlayer, item, ItemPos(pack, 0)))
	{
		delete item;

		return num - leftnum;
	}

	return num;
}

/**
 *@brief 同步道具访问器
 */
class SyncItemVisitor : public WSItemVisitor
{
public:
	SyncItemVisitor(WSPlayer* player, UInt8 uPack):m_pPlayer(player), m_uPack(uPack)
	{
		m_pPacket = Avalon::Packet::Create();

		/*
		if(TRIBE_PACK_STORAGE == m_uPack)
			m_pPacket->SetID(CLProtocol::WORLD_TRIBE_SYNC_STORE);
		else
			m_pPacket->SetID(CLProtocol::WORLD_TRIBE_SYNC_SHOP);
		*/
	}
	~SyncItemVisitor()
	{
		Avalon::Packet::Destroy(m_pPacket);
	}

	bool operator()(WSItem* item)
	{
		Avalon::NetOutputStream stream(m_pPacket->GetBuffer() + m_pPacket->GetSize(), m_pPacket->GetMaxSize() - m_pPacket->GetSize());
		try
		{
			stream & item->GetID() & item->m_DataId;
			item->Encode(stream, SST_SELF, false);
		}
		catch(const Avalon::NetException& exception)
		{
			ErrorLogStream << "player(" << m_pPlayer->GetAccId() << "," << m_pPlayer->GetID() << "," << m_pPlayer->GetName()
				<< ") encode items failed:" << exception.what() << "!" << LogStream::eos;
			return false;
		}
		m_pPacket->SetSize(m_pPacket->GetSize() + UInt16(stream.GetPos()));
		return true;
	}

	void Flush()
	{
		Avalon::NetOutputStream stream(m_pPacket->GetBuffer() + m_pPacket->GetSize(), m_pPacket->GetMaxSize() - m_pPacket->GetSize());
		try
		{
			stream & ObjID_t(0);
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
	WSPlayer*	m_pPlayer;
	//包裹类型
	UInt8		m_uPack;
};

void WSItemMgr::SyncToPlayer(WSPlayer *pPlayer, UInt8 pack)
{
	SyncItemVisitor syncItem(pPlayer, pack);
	VisitItems(pack, syncItem);
	syncItem.Flush();
}

void WSItemMgr::SyncToPlayer(WSPlayer *pPlayer, WSItem* item)
{
	Avalon::Packet* packet = Avalon::Packet::Create();

	/*
	if(TRIBE_PACK_STORAGE == item->m_Pack)
		packet->SetID(CLProtocol::WORLD_TRIBE_SYNC_STORE_ITEM);
	else
		packet->SetID(CLProtocol::WORLD_TRIBE_SYNC_SHOP_ITEM);
	*/

	Avalon::NetOutputStream stream(packet->GetBuffer() + packet->GetSize(), packet->GetMaxSize() - packet->GetSize());
	try
	{
		stream & item->GetID() & item->m_DataId;
		item->Encode(stream, SST_SELF, false);
		stream & ObjID_t(0);
	}
	catch(const Avalon::NetException& exception)
	{
		ErrorLogStream << "player(" << pPlayer->GetAccId() << "," << pPlayer->GetID() << "," << pPlayer->GetName() 
			<< ") encode items failed:" << exception.what() << "!" << LogStream::eos;
		Avalon::Packet::Destroy(packet);
		return;
	}
	packet->SetSize(UInt16(stream.GetPos()));

	pPlayer->SendPacket(packet);
	Avalon::Packet::Destroy(packet);
}

void WSItemMgr::VisitItems(WSItemVisitor& visitor)
{
	m_Items.VisitObjects(visitor);
}

void WSItemMgr::VisitItems(UInt8 pack, WSItemVisitor& visitor)
{
	WSPackage* package = GetPackage(pack);
	if(package != NULL)
	{ 
		package->VisitItems(visitor);
	}
}

void WSItemMgr::SaveItem(WSItem *pItem)
{
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_tribe_item", pItem->GetID(), true);
	pItem->GetDBData(cmd,true);
	if(cmd->GetDataNum() > 0) CLRecordClient::Instance()->SendCommand(cmd);
	else CL_SAFE_DELETE(cmd);
}

bool WSItemMgr::IsFull(UInt8 pack)
{
	WSPackage* package = GetPackage(pack);
	if(package != NULL)
	{ 
		return package->GetCapacity() == 0;
	}

	return true;
}

void WSItemMgr::OnTick(const Avalon::Time &now)
{
	if (m_timer.IsTimeout(now))
	{
		if (m_uLock > 0)
			m_uLock--;
	}
}

