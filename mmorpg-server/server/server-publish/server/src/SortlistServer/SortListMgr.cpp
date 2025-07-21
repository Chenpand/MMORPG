#include "SortListMgr.h"

#include "Application.h"
#include "GameZoneMgr.h"

#include <sstream>
#include <AvalonPacket.h>
#include <CLSortListProtocol.h>

SortListMgr::SortListMgr()
{
	m_SaveTimer.SetInterval(10 * 60 * 1000);
}

SortListMgr::~SortListMgr()
{
}

bool SortListMgr::Init()
{
	Avalon::RetryDBConnection* conn = Application::Instance()->GetDBConnection();
	Avalon::ResultSet* resultSet = conn->Query("SELECT * FROM `t_world_sortlist`");
	if(resultSet == NULL) return false;

	while(resultSet->NextRow())
	{
		SortListEntry entry;
		entry.key =		resultSet->GetField("guid").GetUInt64();
		entry.id =		resultSet->GetField("id").GetUInt64();
		entry.sortType = resultSet->GetField("sorttype").GetUInt8();
		entry.name =	resultSet->GetField("name").GetString();
		entry.zoneId =	resultSet->GetField("zoneId").GetUInt32();
		entry.vip =		resultSet->GetField("vip").GetUInt8();
		entry.value1 =	resultSet->GetField("value1").GetUInt32();

		AddToList(entry);
	}

	conn->FreeResultSet(resultSet);

	//删除多余的
	for(int i = 1; i < SORTLIST_MAX; ++i)
	{
		if(!IsSortDataNeedSave(i)) continue;

		UInt32 maxSize = GetSortListSize(i);
		if(m_SortEntrys[i].size() <= maxSize) continue;

		SortEntryList::iterator delIter = m_SortEntrys[i].begin();
		std::advance(delIter, maxSize);

		for(SortEntryList::iterator iter = delIter; iter != m_SortEntrys[i].end(); ++iter)
		{
			std::ostringstream oss;
			oss << "DELETE FROM `t_world_sortlist` WHERE `guid`=" << iter->key;
			conn->Update(oss.str());
		}
		m_SortEntrys[i].erase(delIter, m_SortEntrys[i].end());
	}

	return true;
}

void SortListMgr::Final()
{
	Save();
}

void SortListMgr::ListSortEntries(GameZone* zone, ObjID_t playerId, UInt8 sortType, UInt16 start, UInt16 count)
{
	if(sortType >= SORTLIST_MAX) return;

	SortEntryList& list = m_SortEntrys[sortType];
	if(count > 30) count = 30;	//最大30条
	if(start > list.size()) start = list.size();

	if(size_t(start + count) > list.size())
	{
		count = UInt16(list.size()) - start;
	}

	UInt16 totalNum = 0;
	UInt16 num = 0;

	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::WORLD_SORTLIST_RET);
	Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
	try
	{
		UInt16 ranking = 0;
		stream & sortType;
		UInt16* rankingPos = (UInt16*)(packet->GetBuffer() + stream.GetPos());
		stream & ranking & UInt8(0) & start;
		UInt8* pos1 = packet->GetBuffer() + stream.GetPos();
		stream & totalNum;
		UInt8* pos2 = packet->GetBuffer() + stream.GetPos();
		stream & num;

		UInt16 index = 0;
		for(SortEntryList::iterator iter = list.begin(); iter != list.end(); ++iter)
		{
			if(IsPlayerSortList(sortType) && ranking == 0)
			{
				++index;
				if(iter->id == playerId) ranking = index;
			}

			++totalNum;

			if(totalNum > start && totalNum <= start + count)
			{
				iter->SerializeForClient(stream);
				++num;
			}
		}
		*rankingPos = ranking;
		*((UInt16*)pos1) = totalNum;
		*((UInt16*)pos2) = num;
	}
	catch(Avalon::NetException&)
	{
		Avalon::Packet::Destroy(packet);
		return;
	}
	packet->SetSize(stream.GetPos());

	zone->TransmitToPlayer(playerId, packet);
	Avalon::Packet::Destroy(packet);
}

void SortListMgr::ListSortEntriesNear(GameZone* zone,  ObjID_t playerId, UInt8 sortType, UInt32 count)
{
	if(sortType >= SORTLIST_MAX) return;

	SortEntryList& list = m_SortEntrys[sortType];
	if(count > 10) count = 10;	//最大30条

	UInt16 ranking = GetRanking(sortType, playerId);

	UInt16 totalNum = 0;
	UInt16 num = 0;

	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::WORLD_SORTLIST_RET);
	Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
	try
	{
		stream & sortType;
		stream & ranking & UInt8(0) & UInt16(ranking > count + 1 ? ranking - count - 1 : 0);
		UInt8* pos1 = packet->GetBuffer() + stream.GetPos();
		stream & totalNum;
		UInt8* pos2 = packet->GetBuffer() + stream.GetPos();
		stream & num;

		for(SortEntryList::iterator iter = list.begin(); iter != list.end(); ++iter)
		{
			++totalNum;

			if(ranking != 0 && totalNum + count >= ranking && totalNum <= ranking + count)
			{
				iter->SerializeForClient(stream);
				++num;
			}
		}

		*((UInt16*)pos1) = num;
		*((UInt16*)pos2) = num;
	}
	catch(Avalon::NetException&)
	{
		Avalon::Packet::Destroy(packet);
		return;
	}
	packet->SetSize(stream.GetPos());

	zone->TransmitToPlayer(playerId, packet);
	Avalon::Packet::Destroy(packet);
}

void SortListMgr::ListSortEntries(GameZone* zone, UInt8 sortType, const std::string& function, UInt32 num)
{
	if(sortType >= SORTLIST_MAX) return;

	SortEntryList& list = m_SortEntrys[sortType];
	if(num > list.size()) num = list.size();	//最大30条

	CLProtocol::WorldSortListVisitRet ret;
	ret.sortType = sortType;
	ret.function = function;
	SortEntryList::iterator iter = list.begin();
	for(UInt32 i = 0; i < num; ++i)
	{
		CLProtocol::SortListVisitEntry entry;
		entry.id = iter->id;
		entry.value1 = iter->value1;
		entry.value2 = iter->value2;
		entry.value3 = iter->value3;
		ret.sortlist.push_back(entry);
		++iter;
	}
	zone->TransmitToWorld(ret);
}

UInt32 SortListMgr::GetRanking(UInt8 sortType, ObjID_t id) const
{
	if(sortType >= SORTLIST_MAX) return 0;

	UInt32 ranking = 0;
	for(SortEntryList::const_iterator iter = m_SortEntrys[sortType].begin();
		iter != m_SortEntrys[sortType].end(); ++iter)
	{
		++ranking;
		if(iter->id == id) return ranking;
	}
	return 0;
}

void SortListMgr::UpdateSortList(SortListEntry& entry)
{
	if(entry.sortType >= SORTLIST_MAX) return;

	SortEntryList& list = m_SortEntrys[entry.sortType];
	SortEntryList::iterator iter = list.begin();
	while(iter != list.end())
	{
		if(iter->id == entry.id) break;
		++iter;
	}

	if(iter != list.end()) //在排行榜中
	{
		if(iter->value1 == entry.value1 && iter->value2 == entry.value2 && iter->value3 == entry.value3)
		{
			return;
		}

		if(IsMaxValueSortList(entry.sortType) && entry.GetSortFactor() < iter->GetSortFactor())
		{
			return;
		}

		iter->name	= entry.name;
		iter->vip	= entry.vip;
		iter->zoneId = entry.zoneId;
		iter->value1 = entry.value1;
		iter->value2 = entry.value2;
		iter->value3 = entry.value3;
		iter->bDirty = true;

		bool bUpdate = false;
		if(iter != list.begin())
		{
			SortEntryList::iterator preIter = iter;
			--preIter;
			if(iter->GetSortFactor() >= preIter->GetSortFactor())
			{
				bUpdate = true;
			}
		}
		if(!bUpdate)
		{
			SortEntryList::iterator nextIter = iter;
			++nextIter;
			if(nextIter != list.end() &&  iter->GetSortFactor() < nextIter->GetSortFactor())
			{
				bUpdate = true;
			}
		}

		if(bUpdate)
		{
			SortListEntry sortEntry = *iter;
			list.erase(iter);
			AddToList(sortEntry);
		}
	}
	else
	{
		if(list.size() >= GetSortListSize(entry.sortType))
		{
			SortEntryList::iterator lastIter = --list.end();
			if(entry.GetSortFactor() < lastIter->GetSortFactor()) return;

			DeleteEntry(entry.sortType, lastIter);
		}

		if(IsSortDataNeedSave(entry.sortType))
		{
			Avalon::RetryDBConnection* conn = Application::Instance()->GetDBConnection();

			std::ostringstream oss;
			oss << "INSERT INTO `t_world_sortlist`(`id`,`sorttype`,`name`,`zoneId`,`vip`,`value1`) VALUES("
				<< entry.id << "," 
				<< int(entry.sortType) << "," 
				<< "'" << entry.name << "',"
				<< entry.zoneId << ","
				<< int(entry.vip) << ","
				<< entry.value1 << ")";
			if(conn->Update(oss.str()) < 0)
			{
				ErrorLogStream << "insert sortentry failed!" << LogStream::eos;
				Application::Instance()->Exit();
				return;
			}
			entry.key = conn->GetInsertID();
		}

		AddToList(entry);
	}
}

void SortListMgr::OnTick(const Avalon::Time& now)
{
	if(m_SaveTimer.IsTimeout(now))
	{
		Save();
	}
}

void SortListMgr::AddToList(SortListEntry& entry)
{
	if(entry.sortType >= SORTLIST_MAX) return;

	SortEntryList& list = m_SortEntrys[entry.sortType];
	UInt64 sortFactor = entry.GetSortFactor();

	SortEntryList::iterator iter = list.begin();
	while(iter != list.end())
	{
		if(sortFactor >= iter->GetSortFactor()) break;
		++iter;
	}

	list.insert(iter, entry);
}

UInt64 SortListMgr::GetMinSortValue(UInt8 sortType) const
{
	if(m_SortEntrys[sortType].empty()) return 0;
	return (--m_SortEntrys[sortType].end())->GetSortFactor();
}

void SortListMgr::DeleteEntry(UInt8 sortType, SortEntryList::iterator iter)
{
	if(IsSortDataNeedSave(sortType))
	{
		Avalon::RetryDBConnection* conn = Application::Instance()->GetDBConnection();
		std::ostringstream oss;
		oss << "DELETE FROM `t_world_sortlist` WHERE `guid`=" << iter->key;
		conn->Update(oss.str());
	}
	m_SortEntrys[sortType].erase(iter);
}

void SortListMgr::Save()
{
	for(int i = 1; i < SORTLIST_MAX; ++i)
	{
		if(!IsSortDataNeedSave(i)) continue;

		for(SortEntryList::iterator iter = m_SortEntrys[i].begin();
			iter != m_SortEntrys[i].end(); ++iter)
		{
			if(iter->bDirty)
			{
				Save(*iter);
			}
		}
	}
}

void SortListMgr::Save(SortListEntry& entry)
{
	Avalon::RetryDBConnection* conn = Application::Instance()->GetDBConnection();
	std::ostringstream oss;
	oss << "UPDATE `t_world_sortlist` SET `name`='" << entry.name << "',`zoneId`=" << entry.zoneId <<  ",`vip`=" << int(entry.vip) << ",`value1`=" << entry.value1 
		<< " WHERE `guid`=" << entry.key;
	if(conn->Update(oss.str()) < 0)
	{
		ErrorLogStream << "update sortentry failed!" << LogStream::eos;
		Application::Instance()->Exit();
		return;
	}
	entry.bDirty = false;
}
