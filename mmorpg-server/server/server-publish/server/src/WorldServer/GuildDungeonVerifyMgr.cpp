#include "GuildDungeonVerifyMgr.h"
#include "GuildDungeonVerify.h"
#include "GuildDungeonMgr.h"
#include "WSPlayerMgr.h"

#include <CLSystemValueDataEntry.h>
#include <CLGuildDungeonLvlDataEntry.h>

UInt64 GetMaxDamage(UInt64 time)
{
	UInt32 secMaxDamage = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_DUNGEON_SEC_MAX_DAMAGE);
	return secMaxDamage * time / 1000;
}

GuildDungeonVerifyMgr::GuildDungeonVerifyMgr()
{
	m_verifyTime = 0;
}

GuildDungeonVerifyMgr::~GuildDungeonVerifyMgr()
{
	for (auto& iter : m_VerifyMap)
	{
		for (auto& it : iter.second)
		{
			delete it;
			it = NULL;
		}

		iter.second.clear();
	}

	m_VerifyMap.clear();
}

void GuildDungeonVerifyMgr::Init()
{
	// 初始化验证超时的时间
	m_verifyTime = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_GUILD_DUNGEON_VERIFY_TIME);
}

void GuildDungeonVerifyMgr::OnTick(const Avalon::Time &now)
{
	GuildDungeonVerifyList timeOutVec;
	for (auto& iter : m_VerifyMap)
	{
		for (auto& it : iter.second)
		{
			GuildDungeonVerify* verifyPtr = it;
			if (NULL == verifyPtr)
				continue;

			// 超时，去除验证伤害
			if (now.Sec() >= verifyPtr->GetStartVerifyTime() + m_verifyTime)
			{
				ErrorLogStream << "Verify Damage TimeOut " << verifyPtr->SerializeVerifyInfo() << LogStream::eos;

				UInt64 damage = verifyPtr->GetDamageVal();

				// 每秒最大伤害限制
				UInt64 maxDamage = GetMaxDamage(UInt64(verifyPtr->GetFightTime()));
				if (damage > maxDamage)
				{
					InfoLogStream << "Guild Dungeon Cheat Suspicion RealDamage:"<< damage <<" MaxDamege:"<< maxDamage
						<< " VerifyInfo" << verifyPtr->SerializeVerifyInfo() << LogStream::eos;
					damage = maxDamage;
				}

				// 去除公会地下城的验证伤害
				GuildDungeonMgr::Instance()->RecordDungeonDamage(verifyPtr->GetGuildId(), verifyPtr->GetDungeonId(),
					verifyPtr->GetPlayerId(), verifyPtr->GetPlayerOccu(), damage, verifyPtr->GetID(),
					verifyPtr->GetPlayerName(), verifyPtr->GetFightTime());

				// 加入验证超时列表
				AddVerifyTimeOut(iter.first, verifyPtr->GetPlayerId());

				// 删除超时的
				timeOutVec.push_back(verifyPtr);
			}
		}
	}

	for (auto& tt : timeOutVec)
	{
		DelVerifyInfo(tt->GetID(), tt->GetPlayerId());
	}
}

const GuildDungeonVerify* GuildDungeonVerifyMgr::GetGuildDungeonVerify(UInt64 id, UInt64 playerId)
{
	auto iter = m_VerifyMap.find(id);
	if (iter == m_VerifyMap.end())
		return NULL;

	for (auto& it : iter->second)
	{
		if (playerId == it->GetPlayerId())
		{
			return it;
		}
	}

	return NULL;
}

void GuildDungeonVerifyMgr::AddVerifyInfo(UInt64 id, UInt64 playerId, UInt64 damage, UInt64 guildId, UInt32 dungeonId, UInt32 useTime, const std::string& name, UInt32 occu)
{
	// 已经验证过了，验证服务器的消息先于客户端的消息到了，所以直接返回不处理
	if (IsVerifyFinish(id, playerId))
	{
		ErrorLogStream << "Verify Server Data Advance Finish RaceId:"<<id<<" playerId:"<<playerId<<
			" damage:"<<damage<<" dungeonId:"<<dungeonId << LogStream::eos;
		return;
	}

	GuildDungeonVerify* verifyInfo = new GuildDungeonVerify();
	verifyInfo->SetID(id);
	verifyInfo->SetPlayerId(playerId);
	verifyInfo->SetPlayerName(name);
	verifyInfo->SetGuildId(guildId);
	verifyInfo->SetDungeonId(dungeonId);
	verifyInfo->SetDamageVal(damage);
	verifyInfo->SetStartVerifyTime(CURRENT_TIME.Sec());
	verifyInfo->SetFightTime(useTime);
	verifyInfo->SetPlayerOccu(occu);

	InfoLogStream << "AddVerifyInfo :"<<verifyInfo->SerializeVerifyInfo()<< LogStream::eos;

	auto iter = m_VerifyMap.find(id);
	if (iter != m_VerifyMap.end())
	{
		iter->second.push_back(verifyInfo);
	}
	else
	{
		GuildDungeonVerifyList verifyVec;
		verifyVec.push_back(verifyInfo);
		m_VerifyMap[id] = verifyVec;
	}
}

void GuildDungeonVerifyMgr::DelVerifyInfo(UInt64 id, UInt64 playerId)
{
	auto iter = m_VerifyMap.find(id);
	if (iter == m_VerifyMap.end())
		return;

	GuildDungeonVerifyList::iterator it = iter->second.begin();
	for (; it != iter->second.end(); it++)
	{
		if (playerId != (*it)->GetPlayerId())
		{
			continue;
		}

		delete *it;
		iter->second.erase(it);
		return;
	}
}

void GuildDungeonVerifyMgr::VerifyServerRes(UInt64 id, UInt64 playerId, UInt32 dungeonId, UInt64 realDamage, UInt32 fightTime)
{
	// 已经超时
	if (IsVerifyTimeOut(id, playerId))
	{
		ErrorLogStream << "Receive Verify Server TimeOut" << id << " playerId:" << playerId << " realDamage:" << realDamage << " fightTime:" << fightTime << LogStream::eos;
		return;
	}

	// 每秒最大伤害限制
	UInt64 maxDamage = GetMaxDamage(fightTime);
	UInt64 damage = realDamage;
	if (realDamage > maxDamage)
	{
		damage = maxDamage;
		InfoLogStream << "Guild Dungeon Cheat Suspicion RaceId:" << id << " PlayerId:" << playerId 
			<< " MaxDamage:" << maxDamage << "realDamage:" << realDamage << LogStream::eos;
	} 

	const GuildDungeonVerify* verifyPtr = GetGuildDungeonVerify(id, playerId);
	if (NULL == verifyPtr)
	{
		// 没有找到验证数据，此时验证服的数据先到，客户端还未上报
		InfoLogStream << "Not Has Client, Verify Server Start RaceId:" << id << " playerId:" << playerId <<
			" realDamage:" << realDamage <<" dungeonId:"<< dungeonId << " damage:" << damage << " fightTime:"
			<< fightTime << LogStream::eos;

		WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(playerId);
		if (NULL == player)
		{
			ErrorLogStream << "Not Find PlayerId"<< LogStream::eos;
			return;
		}

		if (player->GetGuildID() == 0)
		{
			ErrorLogStream << "Player Not Exist Guild" << LogStream::eos;
			return;
		}

		GuildDungeonMgr::Instance()->RecordDungeonDamage(player->GetGuildID(), dungeonId, playerId, player->GetOccu(), damage, 
			id, player->GetName(), fightTime);
	}
	else
	{
		InfoLogStream << "Has Client, Verify Server Start RaceId:" << id << " playerId:" << playerId <<
			" realDamage:" << realDamage << " dungeonId:" << dungeonId << " damage:" << damage << " fightTime:"
			<< fightTime << LogStream::eos;

		// 扣除伤害
		GuildDungeonMgr::Instance()->RecordDungeonDamage(verifyPtr->GetGuildId(), verifyPtr->GetDungeonId(), 
			verifyPtr->GetPlayerId(), verifyPtr->GetPlayerOccu(), damage, verifyPtr->GetID(), verifyPtr->GetPlayerName(), fightTime);
	}
		
	// 加入已经验证
	AddVerify(id, playerId);

	// 删除验证信息
	DelVerifyInfo(id, playerId);
}

UInt64 GuildDungeonVerifyMgr::GetVerifyDamage(UInt64 guildId, UInt32 dungeonId)
{
	UInt64 damage = 0;
	for (auto& iter : m_VerifyMap)
	{
		for (auto& it : iter.second)
		{
			if (guildId == it->GetGuildId() && dungeonId == it->GetDungeonId())
				damage += it->GetDamageVal();
		}
	}

	return damage;
}

bool GuildDungeonVerifyMgr::IsVerifyTimeOut(UInt64 id, UInt64 playerId)
{
	auto iter = m_VerifyTimeOutMap.find(id);
	if (iter == m_VerifyTimeOutMap.end())
	{
		return false;
	}

	auto it = iter->second.find(playerId);
	return it != iter->second.end();
}

void GuildDungeonVerifyMgr::AddVerifyTimeOut(UInt64 id, UInt64 playerId)
{
	auto iter = m_VerifyTimeOutMap.find(id);
	if (iter == m_VerifyTimeOutMap.end())
	{
		std::set<UInt64> tempSet;
		tempSet.insert(playerId);
		m_VerifyTimeOutMap[id] = tempSet;
	}
	else
	{
		iter->second.insert(playerId);
	}
}

bool GuildDungeonVerifyMgr::IsVerifyFinish(UInt64 id, UInt64 playerId)
{
	auto iter = m_VerifyFinishMap.find(id);
	if (iter == m_VerifyFinishMap.end())
	{
		return false;
	}

	auto it = iter->second.find(playerId);
	return it != iter->second.end();
}

void GuildDungeonVerifyMgr::AddVerify(UInt64 id, UInt64 playerId)
{
	auto iter = m_VerifyFinishMap.find(id);
	if (iter == m_VerifyFinishMap.end())
	{
		std::set<UInt64> tempSet;
		tempSet.insert(playerId);
		m_VerifyFinishMap[id] = tempSet;
	}
	else
	{
		iter->second.insert(playerId);
	}
}
