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
	// ��ʼ����֤��ʱ��ʱ��
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

			// ��ʱ��ȥ����֤�˺�
			if (now.Sec() >= verifyPtr->GetStartVerifyTime() + m_verifyTime)
			{
				ErrorLogStream << "Verify Damage TimeOut " << verifyPtr->SerializeVerifyInfo() << LogStream::eos;

				UInt64 damage = verifyPtr->GetDamageVal();

				// ÿ������˺�����
				UInt64 maxDamage = GetMaxDamage(UInt64(verifyPtr->GetFightTime()));
				if (damage > maxDamage)
				{
					InfoLogStream << "Guild Dungeon Cheat Suspicion RealDamage:"<< damage <<" MaxDamege:"<< maxDamage
						<< " VerifyInfo" << verifyPtr->SerializeVerifyInfo() << LogStream::eos;
					damage = maxDamage;
				}

				// ȥ��������³ǵ���֤�˺�
				GuildDungeonMgr::Instance()->RecordDungeonDamage(verifyPtr->GetGuildId(), verifyPtr->GetDungeonId(),
					verifyPtr->GetPlayerId(), verifyPtr->GetPlayerOccu(), damage, verifyPtr->GetID(),
					verifyPtr->GetPlayerName(), verifyPtr->GetFightTime());

				// ������֤��ʱ�б�
				AddVerifyTimeOut(iter.first, verifyPtr->GetPlayerId());

				// ɾ����ʱ��
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
	// �Ѿ���֤���ˣ���֤����������Ϣ���ڿͻ��˵���Ϣ���ˣ�����ֱ�ӷ��ز�����
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
	// �Ѿ���ʱ
	if (IsVerifyTimeOut(id, playerId))
	{
		ErrorLogStream << "Receive Verify Server TimeOut" << id << " playerId:" << playerId << " realDamage:" << realDamage << " fightTime:" << fightTime << LogStream::eos;
		return;
	}

	// ÿ������˺�����
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
		// û���ҵ���֤���ݣ���ʱ��֤���������ȵ����ͻ��˻�δ�ϱ�
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

		// �۳��˺�
		GuildDungeonMgr::Instance()->RecordDungeonDamage(verifyPtr->GetGuildId(), verifyPtr->GetDungeonId(), 
			verifyPtr->GetPlayerId(), verifyPtr->GetPlayerOccu(), damage, verifyPtr->GetID(), verifyPtr->GetPlayerName(), fightTime);
	}
		
	// �����Ѿ���֤
	AddVerify(id, playerId);

	// ɾ����֤��Ϣ
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
