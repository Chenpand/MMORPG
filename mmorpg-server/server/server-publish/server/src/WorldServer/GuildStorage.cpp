#include <CLItemDataEntry.h>
#include <AvalonTime.h>
#include <CLErrorCode.h>
#include <CLRecordClient.h>
#include <CLSystemValueDataEntry.h>

#include "GuildStorage.h"
#include "Guild.h"
#include "CLGuildProtocol.h"
#include "WSPlayerMgr.h"
#include "WSSelectInitDataCallback.h"
#include "GuildMgr.h"


GuildStorage::GuildStorage()
{
	m_guildId = 0;
	m_items.clear();
	m_size = 0;
	m_opRecords.clear();
}

GuildStorage::~GuildStorage()
{
	for (auto &i : m_procTasks)
	{
		CL_SAFE_DELETE(i);
	}
	m_procTasks.clear();

	for (auto &i : m_items)
	{
		CL_SAFE_DELETE(i);
	}
	m_items.clear();
}

/**
*@brief 加载公会仓库回调
*/
class SelectGuildStorageCallback : public SelectInitDataCallback
{
public:
	SelectGuildStorageCallback(GuildStorage* storage){
		m_storage = storage;
	}

	bool OnLoadFinished()
	{
		if (m_storage == NULL) return false;

		m_storage->LoadItems(this);

		return true;
	}

	GuildStorage* m_storage;
};

class SelectGuildStorageOpRecordCallback : public SelectInitDataCallback
{
public:
	SelectGuildStorageOpRecordCallback(GuildStorage* storage){
		m_storage = storage;
	}

	bool OnLoadFinished()
	{
		if (m_storage == NULL) return false;

		m_storage->LoadOpRecords(this);

		return true;
	}

	GuildStorage* m_storage;
};

bool GuildStorage::Init(Guild* guild)
{
	if (!guild)
	{
		return false;
	}

	m_guildId = guild->GetID();
	m_items.clear();
	m_size = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_GUILD_STORAGE_INIT_SIZE);
	if (m_size == 0)
	{
		m_size = INIT_GUILD_STORAGE_SZ;
	}
	m_opRecords.clear();

	return true;
}


void GuildStorage::LoadData()
{
	// 加载数据
	CLSelectCommand* cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_guild_storage");
	cmd->PutCond("guildid", m_guildId);
	CLRecordClient::Instance()->SendCommand(cmd, new SelectGuildStorageCallback(this));

	// 加载操作记录
	CLSelectCommand* opcmd = CLRecordClient::Instance()->PrepareSelectCommand("t_guildstorage_oprecord");
	opcmd->PutCond("guildid", m_guildId);
	CLRecordClient::Instance()->SendCommand(opcmd, new SelectGuildStorageOpRecordCallback(this));
}




void GuildStorage::LoadItems(CLRecordCallback* cb)
{
	if (!cb)
	{
		return;
	}

	while (cb->NextRow())
	{
		ObjID_t guildId = cb->GetData("guildid");
		if (m_guildId != guildId)
		{
			continue;
		}

		WSItem* item = new WSItem();
		item->SetID(cb->GetKey());
		item->m_DataId = cb->GetData("itemid");
		item->m_pDataEntry = ItemDataEntryMgr::Instance()->FindEntry(item->m_DataId);
		item->SetDBData(cb);
		if (item->m_Num == 0 || NULL == item->m_pDataEntry)
		{
			CL_SAFE_DELETE(item);
		}
		else
		{
			item->GenPreciousBeadHoleLoaded();
			item->SetAttribute();
			m_items.push_back(item);
		}
	}
}

void GuildStorage::LoadOpRecords(CLRecordCallback* cb)
{
	if (!cb)
	{
		return;
	}

	UInt32 maxnum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_GUILD_STORAGE_OP_RECORD_MAXNUM);
	if (maxnum == 0)
	{
		maxnum = INIT_GUILD_STORAGE_RECORD_SZ;
	}

	while (cb->NextRow())
	{
		if (m_opRecords.size() >= maxnum)
		{
			break;
		}

		GuildStorageOpRecord record;
		record.guid = cb->GetKey();
		record.name = cb->GetData("name").GetString();
		record.opType = cb->GetData("optype");

		std::string strItemInfo = cb->GetData("iteminfo").GetString();
		if (strItemInfo.length() > 0)
		{
			std::istringstream is(strItemInfo);

			char split = 0;
			while (split != ';')
			{
				GuildStorageItemInfo info;
				info.DecodeString(is);
				is >> split;

				record.items.push_back(info);
			}
		}
		
		record.time = cb->GetData("time");

		m_opRecords.push_back(record);
	}
}

void GuildStorage::AddItem(WSPlayer* player, std::vector<GuildStorageItemInfo>& inItems, AddGuildStorageItemCb* cb)
{
	if (!player)
	{
		ErrorLogStream << " player is null:" << LogStream::eos;
		CL_SAFE_DELETE(cb);
		return;
	}

	if (player->GetGuildID() != m_guildId)
	{
		ErrorLogStream << " player guildid not belong to this guild player guildid:" << player->GetGuildID() << " storage belong to:" << m_guildId << LogStream::eos;
		CL_SAFE_DELETE(cb);
		return;
	}

	AddItemTask* addTask = new AddItemTask;
	addTask->playerUid = player->GetID();
	addTask->items = inItems;
	addTask->time = (UInt32)Avalon::Time::CurrentTime().Sec();
	addTask->state = PALS_UNPROC;
	addTask->cb = cb;

	m_procTasks.push_back(addTask);

	DebugLogStream << " player:" << player->GetID() << " start add guild storage items." << LogStream::eos;
}

bool GuildStorage::DelItem(UInt32 index, UInt16 num)
{
	if (index >= m_items.size())
	{
		ErrorLogStream << " index error:" << index << LogStream::eos;
		return false;
	}

	WSItem* delItem = m_items[index];
	if (!delItem)
	{
		ErrorLogStream << " delItem is null!" << LogStream::eos;
		return false;
	}

	if (num >= delItem->m_Num)
	{
		_DeleteItem(delItem);
		m_items.erase(m_items.begin() + index);
	}
	else
	{
		delItem->m_Num = delItem->m_Num - num;
		_UpdateItem(delItem);
	}

	return true;
}

bool GuildStorage::DelItem(UInt64 uid, UInt16 num, GuildStorageItemInfo& info)
{
	for (size_t i = 0; i < m_items.size(); ++i)
	{
		if (!m_items[i])
		{
			continue;
		}

		if (m_items[i]->GetID() == uid)
		{
			info.dataId = m_items[i]->m_DataId;
			info.str = m_items[i]->m_Strengthen;
			info.uid = m_items[i]->GetID();

			if (num >= m_items[i]->m_Num)
			{
				_DeleteItem(m_items[i]);
				m_items.erase(m_items.begin() + i);
				info.num = 0;
			}
			else
			{
				m_items[i]->m_Num = m_items[i]->m_Num - num;
				_UpdateItem(m_items[i]);
				info.num = m_items[i]->m_Num;
			}

			return true;
		}
	}

	return false;
}

void GuildStorage::GetItem(WSPlayer* player, UInt32 index, UInt16 num, GetGuildStorageItemCb* cb)
{
	if (!player)
	{
		ErrorLogStream << " player is null:" << LogStream::eos;
		CL_SAFE_DELETE(cb);
		return;
	}

	if (player->GetGuildID() != m_guildId)
	{
		ErrorLogStream << " player guildid not belong to this guild player guildid:" << player->GetGuildID() << " storage belong to:" << m_guildId << LogStream::eos;
		CL_SAFE_DELETE(cb);
		return;
	}

	GetItemTask* getTask = new GetItemTask;
	getTask->playerUid = player->GetID();
	getTask->index = index;
	getTask->num = num;
	getTask->time = (UInt32)Avalon::Time::CurrentTime().Sec();
	getTask->state = PALS_UNPROC;
	getTask->cb = cb;

	m_procTasks.push_back(getTask);

	DebugLogStream << " player:" << player->GetID() << " start get guild storage item." << LogStream::eos;


}

std::vector<GuildStorageItemInfo> GuildStorage::QueryItems()
{
	std::vector<GuildStorageItemInfo> itemInfos;
	for (size_t i = 0; i < m_items.size(); ++i)
	{
		itemInfos.push_back(_TransItemInfo(m_items[i]));
	}
	return itemInfos;
}

UInt32 GuildStorage::GetTotalSize()
{
	return m_size;
}

UInt32 GuildStorage::GetSize()
{
	return m_items.size();
}

UInt32 GuildStorage::GetRestSize()
{
	if (m_size < m_items.size())
	{
		ErrorLogStream << " total size less than used size!" << LogStream::eos;
		return 0;
	}

	return m_size - m_items.size();
}

UInt64 GuildStorage::GetGuildId()
{
	return m_guildId;
}

std::vector<GuildStorageOpRecord>& GuildStorage::QueryOpRecord()
{
	return m_opRecords;
}

void GuildStorage::OnTick()
{
	_DelOverTimePreAddTask();
	_ProcTask();
}

void GuildStorage::RealAddItem(UInt64 playerUid, UInt32 code, std::vector<WSItem*>& items)
{
	auto itr = m_procTasks.begin();
	if (itr == m_procTasks.end())
	{
		return;
	}

	GuildStorageTaskInfo* task = *itr;
	task->code = code;

	if (task->state != PALS_WAITING)
	{
		ErrorLogStream << " guild real add item state error! playerUid:" << task->playerUid << " state:" << task->state << LogStream::eos;
		return;
	}

	if (task->playerUid != playerUid)
	{
		ErrorLogStream << " guild real add item player uid error! task playerUid:" << task->playerUid << " playerUid:" << playerUid << LogStream::eos;
		return;
	}

	if (!task->cb)
	{
		ErrorLogStream << " guild real add item cb is null! playerUid:" << task->playerUid << LogStream::eos;
		return;
	}

	std::vector<GuildStorageItemInfo> recordItems;
	std::vector<GuildStorageItemInfo> itemInfos;
	for (size_t i = 0; i < items.size(); ++i)
	{
		if (!items[i])
		{
			ErrorLogStream << " guild real add item is null! playerUid:" << task->playerUid << LogStream::eos;
			task->code = ErrorCode::GUILD_STORAGE_DATA_INVALID;
			continue;
		}

		recordItems.push_back(_TransItemInfo(items[i]));

		ItemDataEntry* itemData = ItemDataEntryMgr::Instance()->FindEntry(items[i]->m_DataId);
		if (!itemData)
		{
			ErrorLogStream << " add guild storage item is invalid! palyerUid:" << task->playerUid << " item id:" << items[i]->m_DataId << LogStream::eos;
			task->code = ErrorCode::GUILD_STORAGE_DATA_INVALID;
			continue;
		}
		
		// 不可叠加
		if (1 == itemData->maxNum)
		{
			_AddItem(items[i]);
			itemInfos.push_back(_TransItemInfo(items[i]));
			continue;
		}
		
		// 查找叠加
		UInt16 restNum = items[i]->m_Num;
		for (size_t m = 0; m < m_items.size(); ++m)
		{
			if (!m_items[m])
			{
				ErrorLogStream << " guild storage item is null! player uid:" << task->playerUid << LogStream::eos;
				task->code = ErrorCode::GUILD_STORAGE_DATA_INVALID;
				continue;
			}

			if (itemData->id == m_items[m]->m_DataId && items[i]->GetStrengthen() == m_items[m]->GetStrengthen())
			{
				restNum = restNum - m_items[m]->Merge(restNum, m_items[m]->GetStrengthen());
				_UpdateItem(m_items[m]);
				itemInfos.push_back(_TransItemInfo(m_items[m]));
				if (0 == restNum)
				{
					CL_SAFE_DELETE(items[i]);
					break;
				}
			}
		}

		// 剩余的
		if (restNum > 0)
		{
			items[i]->m_Num = restNum;
			_AddItem(items[i]);
			itemInfos.push_back(_TransItemInfo(items[i]));
		}
	}
	
	if (task->code == ErrorCode::SUCCESS)
	{
		((AddGuildStorageItemCb*)task->cb)->OnSuccess(itemInfos, recordItems);
	}
	else
	{
		((AddGuildStorageItemCb*)task->cb)->OnFailed(task->code);
	}
	task->state = PALS_DONE;

	DebugLogStream << " player:" << task->playerUid << " real add item to storage. code:" << task->code << LogStream::eos;
}

void GuildStorage::RealGetItem(UInt64 playerUid, WSItem* item, UInt64 oldUid)
{
	if (!item)
	{
		return;
	}

	auto itr = m_procTasks.begin();
	if (itr == m_procTasks.end())
	{
		return;
	}

	GuildStorageTaskInfo* task = *itr;
	if (task->state != PALS_WAITING)
	{
		ErrorLogStream << " guild real get item state error! playerUid:" << task->playerUid << " state:" << task->state << LogStream::eos;
		return;
	}

	if (task->playerUid != playerUid)
	{
		ErrorLogStream << " guild real get item player uid error! task playerUid:" << task->playerUid << " playerUid:" << playerUid << LogStream::eos;
		return;
	}

	if (!task->cb)
	{
		ErrorLogStream << " guild real get item cb is null! playerUid:" << task->playerUid << LogStream::eos;
		return;
	}

	
	GuildStorageItemInfo itemInfo;
	itemInfo.uid = oldUid;
	itemInfo.dataId = item->m_DataId;
	itemInfo.str = item->m_Strengthen;
	itemInfo.num = 0;

	
	for (size_t i = 0; i < m_items.size(); ++i)
	{
		if (m_items[i]->GetID() == oldUid)
		{
			itemInfo = _TransItemInfo(m_items[i]);
			break;
		}
	}

	((GetGuildStorageItemCb*)task->cb)->OnSuccess(item, itemInfo);

	task->state = PALS_DONE;

	DebugLogStream << " player:" << task->playerUid << " real add item to storage. code:" << task->code << LogStream::eos;
}

void GuildStorage::_ProcTask()
{
	auto itr = m_procTasks.begin();

	// 没有任务了
	if (itr == m_procTasks.end())
	{
		//DebugLogStream << " guild:" << m_guildId << " has no task->" << LogStream::eos;
		return;
	}
	
	// 任务已完成则删掉,获取下一个
	if ((*itr)->state == PALS_DONE)
	{
		DebugLogStream << " guild:" << m_guildId << " current task is done. delete it and fetch next one." << LogStream::eos;
		CL_SAFE_DELETE(*itr);
		itr = m_procTasks.erase(itr);
		if (itr == m_procTasks.end())
		{
			DebugLogStream << " guild:" << m_guildId << " there is no next task->" << LogStream::eos;
			return;
		}
	}

	GuildStorageTaskInfo* task = *itr;
	// 任务未完成,继续等
	if (task->state != PALS_UNPROC)
	{
		//DebugLogStream << " guild:" << m_guildId << " current task's state:" << task->state << " wait..." << LogStream::eos;
		return;
	}

	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(task->playerUid);
	if (!player)
	{
		ErrorLogStream << " player:" << task->playerUid << " is not online!" << LogStream::eos;
		return;
	}

	if (task->type == PTT_ADD)
	{
		_ProcAddTask(player, (AddItemTask*)task);
	}
	else if (task->type == PTT_GET)
	{
		_ProcGetTask(player, (GetItemTask*)task);
	}
	else
	{
		ErrorLogStream << " unknow task!" << LogStream::eos;
	}
	
}

void GuildStorage::_ProcAddTask(WSPlayer* player, AddItemTask* task)
{
	std::vector<GuildStorageItemInfo>& inItems = task->items;
	std::vector<GuildStorageItemInfo> canAddItems;
	bool canAddAll = _CheckCapacity(inItems, canAddItems);

	// 把能放入仓库的item告诉scene通知删除
	CLProtocol::WSAddGuildStorageItemReq req;
	req.playerUid = task->playerUid;
	req.guildUid = m_guildId;
	req.items = canAddItems;
	player->SendToScene(req);

	task->state = PALS_WAITING;

	DebugLogStream << " proc player:" << task->playerUid << " add guild storage items. canAddAll:" << canAddAll << LogStream::eos;
}

void GuildStorage::_ProcGetTask(WSPlayer* player, GetItemTask* task)
{
	if (task->index >= m_items.size())
	{
		task->state = PALS_DONE;
		task->code = ErrorCode::GUILD_STORAGE_DELITEM_FAIL;
		((GetGuildStorageItemCb*)task->cb)->OnFailed(task->code);
		return;
	}

	WSItem* delItem = m_items[task->index];
	if (!delItem)
	{
		task->state = PALS_DONE;
		task->code = ErrorCode::GUILD_STORAGE_DELITEM_FAIL;
		((GetGuildStorageItemCb*)task->cb)->OnFailed(task->code);
		return;
	}

	WSItem tmpItem(*delItem);
	tmpItem.SetID(delItem->GetID());
	if (delItem->m_Num > task->num)
	{
		tmpItem.m_Num = task->num;
	}

	if (!DelItem(task->index, task->num))
	{
		task->state = PALS_DONE;
		task->code = ErrorCode::GUILD_STORAGE_DELITEM_FAIL;
		((GetGuildStorageItemCb*)task->cb)->OnFailed(task->code);
		return;
	}

	// 把能取出仓库的item告诉scene通知添加
	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::WS_GUILD_GETSTORAGEITEM_REQ);
	Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
	try
	{
		stream & player->GetID() & m_guildId & tmpItem.GetID() & tmpItem.m_DataId;
		tmpItem.Encode(stream, SST_SELF, false);
	}
	catch (const Avalon::NetException&)
	{
		ErrorLogStream << "encode item:" << tmpItem.GetID() << " failed!" << LogStream::eos;
		Avalon::Packet::Destroy(packet);
		return;
	}
	packet->SetSize(stream.GetPos());

	player->SendToScene(packet);
	Avalon::Packet::Destroy(packet);

	task->state = PALS_WAITING;

	DebugLogStream << " proc player:" << task->playerUid << " get guild storage items." << LogStream::eos;
}

void GuildStorage::_DelOverTimePreAddTask()
{
	auto itr = m_procTasks.begin();
	while (itr != m_procTasks.end())
	{
		if ((UInt32)Avalon::Time::CurrentTime().Sec() - (*itr)->time > PRE_TASK_OT)
		{
			DebugLogStream << " task is ot! delete. player:" << (*itr)->playerUid << LogStream::eos;
			
			if ((*itr)->type == PTT_ADD)
			{
				((AddGuildStorageItemCb*)(*itr)->cb)->OnFailed(ErrorCode::GUILD_STORAGE_DATA_INVALID);
			}
			else if ((*itr)->type == PTT_GET)
			{
				((GetGuildStorageItemCb*)(*itr)->cb)->OnFailed(ErrorCode::GUILD_STORAGE_DATA_INVALID);
			}
			
			CL_SAFE_DELETE(*itr);
			itr = m_procTasks.erase(itr);
		}
		else
		{
			++itr;
		}
	}
}

bool GuildStorage::_CheckCapacity(std::vector<GuildStorageItemInfo>& inItems, std::vector<GuildStorageItemInfo>& outItems)
{
	UInt32 needGridSz = 0;
	UInt32 restCapacity = m_size - (UInt32)m_items.size();

	for (size_t i = 0; i < inItems.size(); ++i)
	{
		// 检查道具有效性
		ItemDataEntry* itemData = ItemDataEntryMgr::Instance()->FindEntry(inItems[i].dataId);
		if (!itemData)
		{
			ErrorLogStream << " add guild storage item is invalid! item id:" << inItems[i].dataId << LogStream::eos;
			continue;
		}

		if (itemData->isAbandon)
		{
			ErrorLogStream << " add guild storage item is abandon! item id:" << inItems[i].dataId << LogStream::eos;
			continue;
		}

		// 不可合并的不用检查合并
		if (1 == itemData->maxNum)
		{
			needGridSz++;
		}
		else
		{
			// 检查合并计算真实需要占用的格子
			UInt16 restNum = inItems[i].num;
			for (size_t m = 0; m < m_items.size(); ++m)
			{
				if (!m_items[m])
				{
					ErrorLogStream << " guild storage item is null! guild uid:" << m_guildId << LogStream::eos;
					continue;
				}

				if (itemData->id == m_items[m]->m_DataId)
				{
					restNum = restNum - m_items[m]->CheckMerging(inItems[i].num, inItems[i].str);
					if (0 == restNum)
					{
						break;
					}
				}
			}

			needGridSz += (restNum / itemData->maxNum) + (restNum % itemData->maxNum > 0 ? 1 : 0);
		}
		
		if (restCapacity < needGridSz)
		{
			return false;
		}
		else
		{
			outItems.push_back(inItems[i]);
		}
	}

	return true;
}

void GuildStorage::_AddItem(WSItem* item)
{
	if (!item)
	{
		return;
	}

	m_items.push_back(item);

	CLInsertCommand* insertCommand = CLRecordClient::Instance()->PrepareInsertCommand("t_guild_storage", item->GetID());
	insertCommand->PutData("guildid", m_guildId);
	item->GetDBData(insertCommand, false);
	CLRecordClient::Instance()->SendCommand(insertCommand);
}

void GuildStorage::_DeleteItem(WSItem* item)
{
	if (!item)
	{
		return;
	}

	CLDeleteCommand* deleteCommand = CLRecordClient::Instance()->PrepareDeleteCommand("t_guild_storage", item->GetID());
	CLRecordClient::Instance()->SendCommand(deleteCommand);

	CL_SAFE_DELETE(item);
}

void GuildStorage::_UpdateItem(WSItem* item)
{
	if (!item)
	{
		return;
	}

	CLUpdateCommand* updateCommand = CLRecordClient::Instance()->PrepareUpdateCommand("t_guild_storage", item->GetID(), false);
	item->GetDBData(updateCommand, true);
	CLRecordClient::Instance()->SendCommand(updateCommand);
}

void GuildStorage::InsertOpRecord(std::string name, GuildStorageOpType type, std::vector<GuildStorageItemInfo>& items, std::vector<GuildStorageOpRecord>& records)
{
	records.clear();
	
	std::vector<GuildStorageItemInfo> tmpItems;
	tmpItems.clear();
	for (size_t i = 0; i < items.size(); ++i)
	{
		tmpItems.push_back(items[i]);

		if (0 == (tmpItems.size() % OP_RECORD_BATCH_NUM))
		{
			_InsertOpRecord(name, type, tmpItems, records);
			tmpItems.clear();
		}
		
	}

	if (tmpItems.size() != 0)
	{
		_InsertOpRecord(name, type, tmpItems, records);

		tmpItems.clear();
	}

}

WSItem* GuildStorage::QueryItemInfo(UInt64 uid)
{
	for (size_t i = 0; i < m_items.size(); ++i)
	{
		if (!m_items[i])
		{
			continue;
		}

		if (m_items[i]->GetID() == uid)
		{
			return m_items[i];
		}
	}
	 
	return NULL;
}

bool GuildStorage::ShopBuyAddItem(WSPlayer* player, WSItem* item)
{
	if (!player)
	{
		return false;
	}

	if (!item)
	{
		return false;
	}

	ItemDataEntry* itemData = ItemDataEntryMgr::Instance()->FindEntry(item->m_DataId);
	if (!itemData)
	{
		return false;
	}

	Guild* guild = GuildMgr::Instance()->FindGuildByID(m_guildId);
	if (!guild)
	{
		return false;
	}

	std::vector<GuildStorageItemInfo> opItemLogs;

	// 不可叠加
	if (1 == itemData->maxNum)
	{
		_AddItem(item);
		GuildStorageItemInfo itemInfo = _TransItemInfo(item);

		//同步仓库道具to客户端
		CLProtocol::WorldGuildStorageItemSync sync;
		sync.items.push_back(itemInfo);
		opItemLogs.push_back(itemInfo);
		InsertOpRecord(player->GetName(), GuildStorageOpType::GSOT_PUT, opItemLogs, sync.itemRecords);

		guild->Broadcast(sync);

		CLProtocol::WorldGuildAddStorageRes res;
		res.result = ErrorCode::SUCCESS;
		player->SendProtocol(res);
		return true;
	}

	//同步仓库道具to客户端
	CLProtocol::WorldGuildStorageItemSync sync;

	UInt16 oldNum = item->m_Num;
	// 查找叠加
	UInt16 restNum = item->m_Num;
	for (size_t m = 0; m < m_items.size(); ++m)
	{
		if (!m_items[m])
		{
			continue;
		}

		if (itemData->id == m_items[m]->m_DataId && item->GetStrengthen() == m_items[m]->GetStrengthen())
		{
			restNum = restNum - m_items[m]->Merge(restNum, item->GetStrengthen());
			_UpdateItem(m_items[m]);
			sync.items.push_back(_TransItemInfo(m_items[m]));

			if (0 == restNum)
			{
				GuildStorageItemInfo itemInfo = _TransItemInfo(m_items[m]);
				itemInfo.num = oldNum;
				opItemLogs.push_back(itemInfo);
				InsertOpRecord(player->GetName(), GuildStorageOpType::GSOT_PUT, opItemLogs, sync.itemRecords);
				guild->Broadcast(sync);

				CLProtocol::WorldGuildAddStorageRes res;
				res.result = ErrorCode::SUCCESS;
				player->SendProtocol(res);
				return true;
			}
		}
	}

	// 剩余的
	if (restNum > 0)
	{
		item->m_Num = restNum;
		_AddItem(item);
		
		sync.items.push_back(_TransItemInfo(item));
	}
	
	GuildStorageItemInfo itemInfo = _TransItemInfo(item);
	itemInfo.num = oldNum;
	opItemLogs.push_back(itemInfo);
	InsertOpRecord(player->GetName(), GuildStorageOpType::GSOT_PUT, opItemLogs, sync.itemRecords);
	guild->Broadcast(sync);

	CLProtocol::WorldGuildAddStorageRes res;
	res.result = ErrorCode::SUCCESS;
	player->SendProtocol(res);

	return true;
}

bool GuildStorage::CheckCapacity(WSItem* item)
{
	if (!item)
	{
		return false;
	}

	UInt32 needGridSz = 0;
	UInt32 restCapacity = m_size - (UInt32)m_items.size();

	
	// 检查道具有效性
	ItemDataEntry* itemData = ItemDataEntryMgr::Instance()->FindEntry(item->m_DataId);
	if (!itemData)
	{
		ErrorLogStream << " shop buy add guild storage item is invalid! item id:" << item->m_DataId << LogStream::eos;
		return false;
	}

	if (itemData->isAbandon)
	{
		ErrorLogStream << " shop buy add guild storage item is abandon! item id:" << item->m_DataId << LogStream::eos;
		return false;
	}

	// 不可合并的不用检查合并
	if (1 == itemData->maxNum)
	{
		needGridSz++;
	}
	else
	{
		// 检查合并计算真实需要占用的格子
		UInt16 restNum = item->m_Num;
		for (size_t m = 0; m < m_items.size(); ++m)
		{
			if (!m_items[m])
			{
				ErrorLogStream << " shop buy guild storage item is null! guild uid:" << m_guildId << LogStream::eos;
				continue;
			}

			if (itemData->id == m_items[m]->m_DataId)
			{
				restNum = restNum - m_items[m]->CheckMerging(item->m_Num, item->GetStrengthen());
				if (0 == restNum)
				{
					break;
				}
			}
		}

		needGridSz += (restNum / itemData->maxNum) + (restNum % itemData->maxNum > 0 ? 1 : 0);
	}

	if (restCapacity < needGridSz)
	{
		return false;
	}

	return true;
}

GuildStorageItemInfo GuildStorage::_TransItemInfo(WSItem* item)
{
	GuildStorageItemInfo itemInfo;
	itemInfo.uid = item->GetID();
	itemInfo.dataId = item->m_DataId;
	itemInfo.num = item->m_Num;
	itemInfo.str = item->m_Strengthen;
	itemInfo.equipType = item->GetEquipType();
	return itemInfo;
}

void GuildStorage::_InsertOpRecord(std::string name, GuildStorageOpType type, std::vector<GuildStorageItemInfo>& items, std::vector<GuildStorageOpRecord>& records)
{
	UInt32 maxnum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SystemValueType::SVT_GUILD_STORAGE_OP_RECORD_MAXNUM);
	if (maxnum == 0)
	{
		maxnum = INIT_GUILD_STORAGE_RECORD_SZ;
	}

	GuildStorageOpRecord record;
	record.guid = CLRecordClient::Instance()->GenGuid();
	record.name = name;
	record.opType = (UInt32)type;
	record.items = items;
	record.time = (UInt32)Avalon::Time::CurrentTime().Sec();

	if (m_opRecords.size() >= maxnum)
	{
		CLDeleteCommand* delcmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_guildstorage_oprecord", m_opRecords[0].guid);
		CLRecordClient::Instance()->SendCommand(delcmd);
		m_opRecords.erase(m_opRecords.begin());
	}

	CLInsertCommand* inscmd = CLRecordClient::Instance()->PrepareInsertCommand("t_guildstorage_oprecord", record.guid);
	inscmd->PutData("guildid", m_guildId);
	inscmd->PutData("name", name);
	inscmd->PutData("optype", (UInt32)type);

	std::ostringstream os;
	for (size_t i = 0; i < items.size(); ++i)
	{
		items[i].EncodeString(os);
		if (i < items.size() - 1)
		{
			os << '|';
		}
	}
	os << ';';
	
	inscmd->PutData("iteminfo", os.str());
	inscmd->PutData("time", record.time);
	CLRecordClient::Instance()->SendCommand(inscmd);

	m_opRecords.push_back(record);
	records.push_back(record);
}


