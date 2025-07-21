#include "WSPlayerShareDataMgr.h"
#include <CLPlayerDefine.h>
#include <CLRecordClient.h>

#include "WSPlayer.h"

WSPlayerShareDataMgr::WSPlayerShareDataMgr()
{
}

WSPlayerShareDataMgr::~WSPlayerShareDataMgr()
{
}

bool WSPlayerShareDataMgr::AddPlayer(WSPlayerShareData *player)
{
	// 判断缓存的玩家数量，如果超过一定值，就尝试删除部分玩家
	//TryRelasePlayerOfflineData();

	return m_Players.AddObject(player);
}

void WSPlayerShareDataMgr::TryRelasePlayerOfflineData()
{
	if (m_Players.GetNum() < 5000)
	{
		return;
	}

	class LruVisitor : public PlayerShareDataVisitor
	{
	public:
		LruVisitor() : lastPlayer(NULL) {}

		bool operator()(WSPlayerShareData* player)
		{
			if (lastPlayer == NULL || player->GetOfflineTime() > lastPlayer->GetOfflineTime())
			{
				lastPlayer = player;
			}

			return true;
		}

		WSPlayerShareData* lastPlayer;
	};

	LruVisitor vistor;
	m_Players.VisitObjects(vistor);

	if (vistor.lastPlayer)
	{
		InfoLogStream << "memory out of memory, free player(" << vistor.lastPlayer->GetID() << ", " << vistor.lastPlayer->GetAccID() << ")." << LogStream::eos;

		m_Players.RemoveObject(vistor.lastPlayer);
		CL_SAFE_DELETE(vistor.lastPlayer);
	}
}

WSPlayerShareData* WSPlayerShareDataMgr::FindPlayer(ObjID_t id)
{
	return m_Players.FindObject(id);
}

WSPlayerShareData* WSPlayerShareDataMgr::FindOrCreatePlayer(ObjID_t id)
{
	WSPlayerShareData* pShareData = m_Players.FindObject(id);
	if (pShareData)
	{
		return pShareData;
	}

	WSPlayerShareData* newShareData = new WSPlayerShareData();
	newShareData->SetID(id);
	this->AddPlayer(newShareData);
	return newShareData;
}

void WSPlayerShareDataMgr::OnOffline(WSPlayer* player)
{
	WSPlayerShareData* pShareData = this->FindPlayer(player->GetID());
	if (pShareData)
	{
		pShareData->SyncDataFromWorld(player);
	}
	else
	{
		pShareData = new WSPlayerShareData();
		pShareData->SetID(player->GetID());
		pShareData->SyncDataFromWorld(player);
		this->AddPlayer(pShareData);
	}
}

void WSPlayerShareDataMgr::HandleScenePlayerSyncShareData(ObjID_t guid, const PlayerSenceShareData& sharedata)
{
	WSPlayerShareData* pShareData = this->FindPlayer(guid);
	if (pShareData)
	{
		pShareData->SyncDataFromScene(sharedata);
	}
	else
	{
		pShareData = new WSPlayerShareData();
		pShareData->SetID(guid);
		pShareData->SyncDataFromScene(sharedata);
		this->AddPlayer(pShareData);
	}
	//pShareData->SetStatus(PSDS_LOADED);
}

void WSPlayerShareDataMgr::HandleRelieveMaster(ObjID_t discipleId)
{
	WSPlayerShareData* pShareData = this->FindPlayer(discipleId);
	if (pShareData)
	{
		pShareData->ClearMasterTaskData();
	}
}

void WSPlayerShareDataMgr::LoadOfflinePlayerCb(ObjID_t roleid, UInt32 accid, WSShareDataLoadCallback* cb)
{
	WSPlayerShareData* pShareData = new WSPlayerShareData();
	pShareData->SetStatus(PSDS_INIT);
	pShareData->SetID(roleid);
	pShareData->SetAccID(accid);

	this->AddPlayer(pShareData);
	pShareData->StartLoadData(cb);
}