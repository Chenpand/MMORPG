#include "GuildHonorMgr.h"
#include <CLGuildProtocol.h>
#include "Guild.h"

GuildHonorMgr::GuildHonorMgr()
{
	class honorVistor : public Avalon::DataEntryVisitor<GuildHonorDataEntry>
	{
	public:
		honorVistor(HonorMap& HonorMap, std::vector<GuildHonorData>& HonorVec , MapById& m_HonorById) :honorMap(HonorMap), honorVec(HonorVec), honorById(m_HonorById)
		{
		}

		bool operator()(GuildHonorDataEntry *dataEntry)
		{
			if (!dataEntry)return true;
			GuildHonorData data;
			data.id = dataEntry->id;
			data.groupId = dataEntry->groupId;
			data.conds = dataEntry->conds;
			data.maxCount = dataEntry->count;
			data.type = dataEntry->type;

			honorVec.push_back(data);
			honorById[dataEntry->id] = honorVec.size() - 1;
			honorMap[dataEntry->type][dataEntry->id] = honorVec.size() - 1;

			return true;
		}
		HonorMap& honorMap;
		MapById&  honorById;
		std::vector<GuildHonorData>& honorVec;
	};

	honorVistor vis(m_HonorMap, m_honorVec, m_HonorById);
	GuildHonorDataEntryMgr::Instance()->Visit(vis);
}
void GuildHonorMgr::DecodeString(std::istringstream & is)
{	
	UInt32 id;
	UInt32 groupId;
	UInt32 curCount;
	UInt32 finishTime;
	UInt8 split ;
	while (is >> id)
	{
		is >> split >> groupId >> split >> curCount>> split >> finishTime >> split;
		if (m_HonorById.count(id) == 0)continue;

		GuildHonorData* data = &m_honorVec[m_HonorById[id] ];
		data->curCount = curCount;
		data->finishTime = finishTime;
	}
	SetDirty();
}

void GuildHonorMgr::EncodeString(std::ostringstream & os)
{
	UInt8 split1 = ',';
	UInt8 split2 = '|';
	for (auto it : m_honorVec)
	{
		os << it.id << split1 <<it.groupId << split1 << it.curCount << split1 << it.finishTime << split2;
	}
}

void GuildHonorMgr::AddGuildHonorCnt(GuildHonorChallenge type, std::vector<UInt32>& conds)
{
	if (m_HonorMap.count(type) == 0)
	{
		ErrorLogStream << "guildID("<< m_guild->GetID() <<") not find GuildHonorChallenge type = "<< type << LogStream::eos;
		return;
	}

	if (conds.size() != GuildHonorConditionType::GUILD_COND_MAX)return;

	auto& honors = m_HonorMap[type];
	CLProtocol::WorldGuildHonorInfoRes res;

	res.guildId = m_guild->GetID();
	res.guildName = m_guild->GetName();

	for (auto& it : honors)
	{
		if (it.second >= m_honorVec.size())continue;
		
		GuildHonorData* data = &m_honorVec[it.second];

		//ÒÑÍê³É
		if (data->curCount >= data->maxCount)continue;

		bool flag = false;
		
		for (int i = 0; i < conds.size() && (!flag); i++)
		{
			if (data->conds[i] != 0 && conds[i] == 0)flag = true;

			switch (i)
			{
			case GuildHonorConditionType::GUILD_COND_TYPE:
			{
				if (data->conds[i] != 0 && data->conds[i] != conds[i])flag=true;
				break;
			}
			case GuildHonorConditionType::GUILD_COND_DIFF:
			{
				if (data->conds[i] != 0 && data->conds[i] > conds[i])flag = true;
				break;
			}
			case GuildHonorConditionType::GUILD_COND_TIME:
			{
				if (data->conds[i] != 0 &&data->conds[i] < conds[i])flag = true;
				break;
			}
			case GuildHonorConditionType::GUILD_COND_MEMBER:
			{
				if (data->conds[i] != 0 && data->conds[i] > conds[i])flag = true;
				break;
			}
			case GuildHonorConditionType::GUILD_COND_TEAM:
			{
				if (data->conds[i] != conds[i])flag = true;
				break;
			}
			default:
				break;
			}
		}
		if (!flag)
		{
			++data->curCount;
			if (data->curCount >= data->maxCount)
			{
				data->finishTime = CURRENT_TIME.Sec();
				GuildHonor ghonor;
				ghonor.id = data->id;
				ghonor.groupId = data->groupId;
				ghonor.cnt = data->curCount;
				ghonor.time = data->finishTime;
				res.honors.push_back(ghonor);
			}
			SetDirty();
		}
	}
	if (res.honors.size() != 0)
	{
		m_guild->Broadcast(res);
	}

}

void GuildHonorMgr::GetGuildHonor(CLProtocol::WorldGuildHonorInfoRes & res, std::vector<UInt32>& groupIds)
{
	for (int i = 0; i < m_honorVec.size(); i++)
	{
		bool flag = false;
		if (groupIds.size() == 0)flag = true;
		for (int j = 0; j < groupIds.size(); j++)
		{
			if (m_honorVec[i].groupId == groupIds[i])
			{
				flag = true;
				break;
			}
		}
		if (!flag)continue;
		GuildHonor data;
		data.cnt = m_honorVec[i].curCount;
		data.groupId = m_honorVec[i].groupId;
		data.id = m_honorVec[i].id;
		data.time = m_honorVec[i].finishTime;
		res.honors.push_back(data);
	}
}
