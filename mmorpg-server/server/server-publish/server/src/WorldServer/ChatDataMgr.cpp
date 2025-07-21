#include "ChatDataMgr.h"
#include "WSPlayer.h"
#include "WSRouter.h"

ChatDataMgr::ChatDataMgr()
{
}

ChatDataMgr::~ChatDataMgr()
{
	Final();
}

void ChatDataMgr::Init()
{
}

void ChatDataMgr::Final()
{
	for(ChatDataMap::iterator iter = m_ChatDatas.begin();
		iter != m_ChatDatas.end(); ++iter)
	{
		free(iter->second.data);
	}
	m_ChatDatas.clear();
}

void ChatDataMgr::OnTick(const Avalon::Time& now)
{
	for(ChatDataMap::iterator iter = m_ChatDatas.begin();
		iter != m_ChatDatas.end();)
	{
		if(now.Sec() > iter->second.createtime + CHATDATA_TIMEOUT)
		{
			free(iter->second.data);
			m_ChatDatas.erase(iter++);
		}
		else
		{
			++iter;
		}
	}
}

void ChatDataMgr::AddData(ObjID_t id, const char* data, size_t len)
{
	RemoveData(id);
	
	ChatData chatData;
	chatData.createtime = UInt32(CURRENT_TIME.Sec());
	chatData.data = (char*)malloc(len);
	avalon_memmove(chatData.data, data, len);
	chatData.len = UInt16(len);
	m_ChatDatas.insert(std::make_pair(id, chatData));
}

void ChatDataMgr::SendLinkData(WSPlayer* player, UInt8 type, ObjID_t id)
{
	ChatDataMap::iterator iter = m_ChatDatas.find(id);
	if(iter == m_ChatDatas.end()) return;

	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::WORLD_CHAT_LINKDATA_RET);
	Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
	try
	{
		stream & type;
		stream.SeriaArray(iter->second.data, iter->second.len);
	}
	catch(Avalon::NetException&)
	{
		Avalon::Packet::Destroy(packet);
		return;
	}
	packet->SetSize(stream.GetPos());
	player->SendPacket(packet);
	Avalon::Packet::Destroy(packet);
}

void ChatDataMgr::RemoveData(ObjID_t id)
{
	ChatDataMap::iterator iter = m_ChatDatas.find(id);
	if(iter != m_ChatDatas.end())
	{
		free(iter->second.data);
		m_ChatDatas.erase(iter);
	}
}

bool ChatDataMgr::IsExistData(ObjID_t id) const
{
	return m_ChatDatas.find(id) != m_ChatDatas.end();
}

bool ChatDataMgr::OtherZoneQueryTips(UInt8 type, UInt64 id, Avalon::Packet* packet)
{
	if (NULL == packet) return false;

	ChatDataMap::iterator iter = m_ChatDatas.find(id);
	if (iter == m_ChatDatas.end()) return false;

	packet->SetID(CLProtocol::WORLD_CHAT_LINKDATA_RET);
	Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
	try
	{
		stream & type;
		stream.SeriaArray(iter->second.data, iter->second.len);
	}
	catch (Avalon::NetException&)
	{
		Avalon::Packet::Destroy(packet);
		return false;
	}

	packet->SetSize(stream.GetPos());
	return true;
}
