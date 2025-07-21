#include "WSRelationMgr.h"

#include <CLRecordClient.h>
#include <CLRelationProtocol.h>
#include <CLChatProtocol.h>
#include <CLSystemValueDataEntry.h>
#include <CLFunctionDataEntry.h>
#include <CLChatDefine.h>

#include "WSPushNotify.h"
#include "WSPlayerMgr.h"
#include "MallMgr.h"
#include "CLSysNotify.h"
#include "WSPlayerShareDataMgr.h"
#include "CLMsgParser.h"

//各种关系数量上限(走表,表里没有用该默认值)
//1.好友,2.黑名单 3.陌生人 4.师傅 5.徒弟
const static UInt32 MAX_RELATION_NUM[] = {0, 50, 50, 100, 1, 5};

/**
 *@brief 查询关系返回
 */
class SelectRelationsCallback : public SelectRoleInfoCallback
{
public:
	SelectRelationsCallback(WSPlayer* player):SelectRoleInfoCallback(player){}
	~SelectRelationsCallback(){}

	void OnFinished(WSPlayer* player)
	{
		player->GetRelationMgr().OnLoadDataRet(this);
	}

	void OnFailed(WSPlayer* player)
	{

	}
};

/**
*@brief 查询个人师傅设置返回
*/
class SelectMasterInfoCallback : public SelectRoleInfoCallback
{
public:
	SelectMasterInfoCallback(WSPlayer* player) :SelectRoleInfoCallback(player){}
	~SelectMasterInfoCallback(){}

	void OnFinished(WSPlayer* player)
	{
		if (NextRow())
		{
			UInt8 isRecvDisciple = GetData("is_recv_disciple");
			std::string masterNote = GetData("master_note").GetString();
			player->GetRelationMgr().SetMasterNote(masterNote);
			player->GetRelationMgr().SetIsRecvDisciple(isRecvDisciple > 0 ? true : false);
		}
	}

	void OnFailed(WSPlayer* player)
	{

	}
};

WSRelationMgr::WSRelationMgr()
{
	m_pOwner = NULL;
	m_SyncDBTimer.SetInterval(10 * 60 * 1000);
	m_MasterNote = "";
	m_IsRecvDisciple = true;
}

WSRelationMgr::~WSRelationMgr()
{
	for(int i = 0; i < RELATION_MAX; ++i)
	{
		for(RelationMap::iterator iter = m_Relations[i].begin();
			iter != m_Relations[i].end(); ++iter)
		{
			CL_SAFE_DELETE(iter->second);
		}
	}
}

bool WSRelationMgr::IsFull(UInt8 type) const
{
	if(type >= RELATION_MAX) return true;

	if (type == RelationType::RELATION_FRIEND)
	{
		return m_Relations[type].size() >= SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_TAP_SYSTEM_FRIEND_MAX_COUNT);
	}
	else if (type == RelationType::RELATION_MASTER)
	{
		return m_Relations[type].size() >= SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_TAP_SYSTEM_MASTER_MAX_COUNT);
	}
	else if (type == RelationType::RELATION_DISCIPLE)
	{
		return m_Relations[type].size() >= SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_TAP_SYSTEM_PUPIL_MAX_COUNT);
	}
	else
	{
		return false;
	}

	return false;
}

WSRelation* WSRelationMgr::FindRelation(UInt8 type, ObjID_t id)
{
	if(type >= RELATION_MAX) return NULL;

	RelationMap::iterator iter = m_Relations[type].find(id);
	if(iter != m_Relations[type].end()) return iter->second;
	return NULL;
}

WSRelation* WSRelationMgr::FindRelation(UInt8 type, const std::string& name)
{
	if(type >= RELATION_MAX) return NULL;

	for(RelationMap::iterator iter = m_Relations[type].begin();
		iter != m_Relations[type].end(); ++iter)
	{
		if(iter->second->GetName() == name) return iter->second;
	}
	return NULL;
}

void WSRelationMgr::RemoveRelation(WSRelation* relation)
{
	if(relation == NULL || relation->GetType() >= RELATION_MAX) return;

	RelationMap::iterator iter = m_Relations[relation->GetType()].find(relation->GetID());
	if(iter == m_Relations[relation->GetType()].end()) return;
	
	m_Relations[relation->GetType()].erase(relation->GetID());

	//通知玩家删除关系
	SendPlayerDelRelation((RelationType)relation->GetType(), relation->GetID());

	//通知网关移除黑名单
	if(relation->GetType() == RELATION_BLACKLIST)
	{
		SendNotifyDelBlackList(GetOwner()->GetID(), relation->GetID());
	}
	//通知场景移除好友
	SendNotifySceneDel(GetOwner()->GetID(), (RelationType)relation->GetType(), relation->GetID());
	
	DebugLogStream << "WorldNotifyDelRelation relationId(" << relation->GetGuid() << ") owner(" << GetOwner()->GetID() << ") targetId(" << relation->GetID() << LogStream::eos;

	relation->DelMe();
	CL_SAFE_DELETE(relation);
}

bool WSRelationMgr::RemoveRelation(UInt8 type, ObjID_t id)
{
	if (type < RelationType::RELATION_FRIEND || type >= RelationType::RELATION_MAX)
	{
		return false;
	}

	RelationMap& relations = m_Relations[type];
	RelationMap::iterator iter = relations.find(id);
	if (iter == relations.end())
	{
		return false;
	}
	
	WSRelation* relation = iter->second;
	if (relation == NULL)
	{
		return false;
	}

	//通知玩家删除关系
	SendPlayerDelRelation((RelationType)relation->GetType(), relation->GetID());

	//通知网关移除黑名单
	if (relation->GetType() == RELATION_BLACKLIST)
	{
		SendNotifyDelBlackList(GetOwner()->GetID(), relation->GetID());
	}
	//通知场景移除好友
	SendNotifySceneDel(GetOwner()->GetID(), (RelationType)relation->GetType(), relation->GetID());
	GetOwner()->SendNotify(617, relation->GetName().c_str());
	

	relations.erase(iter);
	relation->DelMe();	
	CL_SAFE_DELETE(relation);
	
	return true;
}

WSRelation* WSRelationMgr::GetRelation(UInt8 type)
{
	if(type >= RELATION_MAX) return NULL;

	if(m_Relations[type].empty()) return NULL;

	return m_Relations[type].begin()->second;
}

WSRelation* WSRelationMgr::GetRelation(ObjID_t id)
{
	for (int i = 1; i < RELATION_MAX; ++i)
	{
		auto itr = m_Relations[i].find(id);
		if (itr != m_Relations[i].end())
		{
			return itr->second;
		}
	}
	return NULL;
}

bool WSRelationMgr::ChangeToStranger(UInt8 type, ObjID_t id)
{
	if (type == RELATION_STRANGER || type >= RELATION_MAX) return false;

	RelationMap& relations = m_Relations[type];
	if (relations.empty()) return false;

	auto itr = relations.find(id);
	if (itr == relations.end())	return false;

	WSRelation* relation = itr->second;
	if (!relation)	return false;
	relations.erase(itr);

	//通知网关移除黑名单
	if (relation->GetType() == RELATION_BLACKLIST)
	{
		SendNotifyDelBlackList(GetOwner()->GetID(), relation->GetID());
	}

	//更新自己
	RelationMap& strangers = m_Relations[RELATION_STRANGER];
	strangers.insert(std::make_pair(id, relation));
	relation->SetType(RELATION_STRANGER);
	relation->SyncToDB();

	//更新到场景
	SendNotifyNewRelation((RelationType)RELATION_STRANGER, id);
	UpdateRelationDebugLog debugLog("WSRelationMgr::ChangeToStranger");
	UpdateRelation();

	return true;
}

//根据时间排序
struct RelationTimeSorter
{
public:
	bool operator()(const WSRelation* relation1, const WSRelation* relation2) const
	{
		return relation1->GetAddTime() < relation2->GetAddTime();
	}
};



bool WSRelationMgr::AddRelation(WSRelation* relation)
{
	if(relation == NULL || relation->GetType() >= RELATION_MAX) return false;

	RelationMap::iterator iter = m_Relations[relation->GetType()].find(relation->GetID());
	if(iter != m_Relations[relation->GetType()].end()) return false;

	if(IsFull(relation->GetType())) return false;

	if(!m_Relations[relation->GetType()].insert(std::make_pair(relation->GetID(), relation)).second) return false;
	relation->SetGuid(CLRecordClient::Instance()->GenGuid());
	relation->InsertToDB(GetOwner()->GetID());

	//通知玩家新关系
	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::WORLD_NOTIFY_NEWRELATION);
	Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
	//stream & relation->GetType() & relation->GetID() & UInt8(relation->IsOnline()?1:0);
	stream & relation->GetID() & UInt8(relation->IsOnline() ? 1 : 0);
	relation->Encode(stream, UInt32(1) << relation->GetType(), false);
	packet->SetSize(stream.GetPos());
	GetOwner()->SendPacket(packet);
	Avalon::Packet::Destroy(packet);

	//通知网关黑名单
	if(relation->GetType() == RELATION_BLACKLIST)
	{
		CLProtocol::GateNotifyNewBlackList notify;
		notify.id = GetOwner()->GetID();
		notify.target = relation->GetID();
		GetOwner()->SendToGate(notify);

		GetOwner()->SendNotify(616,	relation->GetName().c_str());
	}

	//通知场景新关系
	SendNotifyNewRelation((RelationType)relation->GetType(), relation->GetID());
	
	DebugLogStream << "WorldNotifyNewRelation relationId(" << relation->GetGuid() << ") owner(" << GetOwner()->GetID() << ") targetId(" << relation->GetID() << ")" << LogStream::eos;
	return true;
}

bool WSRelationMgr::AddFriend(UInt8 type, ObjID_t id)
{
	if (type == RELATION_FRIEND) return false;
	if (IsFull(RELATION_FRIEND)) return false;

	RelationMap& relations = m_Relations[type];
	if (relations.empty()) return false;

	auto itr = relations.find(id);
	if (itr == relations.end())	return false;

	WSRelation* relation = itr->second;
	if (!relation)	return false;
	relations.erase(itr);

	//更新自己
	RelationMap& strangers = m_Relations[RELATION_FRIEND];
	strangers.insert(std::make_pair(id, relation));
	relation->SetType(RELATION_FRIEND);
	relation->SyncToDB();

	return true;
}

bool WSRelationMgr::HasRelation(ObjID_t id)
{
	for(int i = 1; i < RELATION_MAX; ++i)
	{
		if(m_Relations[i].find(id) != m_Relations[i].end()) return true;
	}
	return false;
}

bool WSRelationMgr::HasRelation(UInt8 type)
{
	if (type >= RELATION_MAX) return false;
	
	return m_Relations[type].size() > 0;
}

void WSRelationMgr::VisitRelations(UInt8 type, RelationVisitor& visitor)
{
	if(type >= RELATION_MAX) return;

	for(RelationMap::iterator iter = m_Relations[type].begin();
		iter != m_Relations[type].end(); ++iter)
	{
		if(!visitor(iter->second)) return;
	}
}

void WSRelationMgr::VisitRelations(RelationVisitor& visitor)
{
	for(int i = 0; i < RELATION_MAX; ++i)
	{
		for(RelationMap::iterator iter = m_Relations[i].begin();
			iter != m_Relations[i].end(); ++iter)
		{
			if(!visitor(iter->second)) return;
		}
	}
}

void  WSRelationMgr::VisitRelationsNew(UInt8 type, const std::function<bool(WSRelation*)>& ff)
{
	if (type >= RELATION_MAX) return;

	for (RelationMap::iterator iter = m_Relations[type].begin();
		iter != m_Relations[type].end(); ++iter)
	{
		if (!ff(iter->second)) return;
	}
}

void WSRelationMgr::LoadRelations()
{
	if(GetOwner()->GetSrcZoneId() != 0) return;

	CLSelectCommand* selectCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_relation");
	selectCmd->PutCond("owner", GetOwner()->GetID());
	CLRecordClient::Instance()->SendCommand(selectCmd, new SelectRelationsCallback(GetOwner()));

	CLSelectCommand* selectMasterInfoCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
	selectMasterInfoCmd->PutColumn("is_recv_disciple");
	selectMasterInfoCmd->PutColumn("master_note");
	selectMasterInfoCmd->PutCond("guid", GetOwner()->GetID());
	CLRecordClient::Instance()->SendCommand(selectMasterInfoCmd, new SelectMasterInfoCallback(GetOwner()));
}

void WSRelationMgr::SyncRelationListToScene()
{
	CLProtocol::SceneRelationSyncList sync;
	sync.id = GetOwner()->GetID();
	
	for (int i = 0; i < RELATION_MAX; ++i)
	{
		std::map<ObjID_t, UInt16> list;

		for (auto itr = m_Relations[i].begin(); itr != m_Relations[i].end(); ++itr)
		{
			list.insert(std::make_pair(itr->first, 0));
		}
		
		sync.relationList.push_back(list);
	}

	GetOwner()->SendToScene(sync);
}

void WSRelationMgr::OnLoadDataRet(CLRecordCallback* callback)
{
	if(callback == NULL) return;

	while(callback->NextRow())
	{
		UInt8	type = callback->GetData("type");
		if(type >= RELATION_MAX) continue;
		DBKey_t key = callback->GetKey();
		if (0 == key)	continue;
		UInt64	tarUid = callback->GetData("id");
		if (0 == tarUid) continue;

		WSRelation* relation = new WSRelation(type);
		relation->SetGuid(key);
		relation->LoadDBData(callback);
		if(!m_Relations[type].insert(std::make_pair(relation->GetID(), relation)).second)
		{
			CL_SAFE_DELETE(relation);
			continue;
		}

		relation->ClearDirty();
	}
}

void WSRelationMgr::OnOnline(bool dayChanged, bool isReconnect)
{
	if (dayChanged && !isReconnect)
	{
		InfoLogStream << "WSRelationMgr::OnOnline player( " << this->GetOwner()->GetID() << ") dayChanged(" << dayChanged << ")" << LogStream::eos;
		this->UpdateMasterDailyTaskStateOnLogicDayChange();
	}

	class RelationOnlineVisitor : public WSPlayerVisitor
	{
	public:
		RelationOnlineVisitor(WSPlayer* owner):m_pOwner(owner)
		{
			m_bUpdateLevel = m_bUpdateVipLv = m_bUpdateSeasonLv = m_bUpdateOccu = m_bUpdateHeadFrame = m_bGuildId = m_bReturnYearTitle = false;
		}

		bool operator()(WSPlayer* player)
		{
			if(m_pOwner == player) return true;

			for(UInt8 i = RELATION_FRIEND; i < RELATION_MAX; ++i)
			{
				WSRelation* relation = player->GetRelationMgr().FindRelation(i, m_pOwner->GetID());
				if(relation != NULL)
				{
					if(IsRelationShowLevel(relation->GetType()) && relation->GetLevel() != m_pOwner->GetLevel())
					{
						relation->SetLevel(m_pOwner->GetLevel());
						m_bUpdateLevel = true;
					}
					if (IsRelationShowVip(relation->GetType()) && relation->GetVipLv() != m_pOwner->GetVipLvl())
					{
						relation->SetVipLv(m_pOwner->GetVipLvl());
						m_bUpdateVipLv = true;
					}
					if (IsRelationShowSeasonLv(relation->GetType()) && relation->GetSeasonLv() != m_pOwner->GetSeasonLv())
					{
						relation->SetSeasonLv(m_pOwner->GetSeasonLv());
						m_bUpdateSeasonLv = true;
					}
					if (IsRelationShowOccu(relation->GetType()) && relation->GetOccu() != m_pOwner->GetOccu())
					{
						relation->SetOccu(m_pOwner->GetOccu());
						m_bUpdateOccu = true;
					}
					if (relation->GetHeadFrame() != m_pOwner->GetHeadFrame())
					{
						relation->SetHeadFrame(m_pOwner->GetHeadFrame());
						m_bUpdateHeadFrame = true;
					}
					if (relation->GetGuildId() != m_pOwner->GetGuildID())
					{
						relation->SetGuildId(m_pOwner->GetGuildID());
						m_bGuildId = true;
					}
					if (relation->GetReturnYearTitle() != m_pOwner->GetReturnYearTitle())
					{
						relation->SetReturnYearTitle(m_pOwner->GetReturnYearTitle());
						m_bReturnYearTitle = true;
					}

					relation->SetStatus(m_pOwner->GetMatchStatus());
					//回归老兵标示
					relation->SetRegressionMark(m_pOwner->IsVeteranReturn());
					//relation->SetOfflineTime(0);
				}
			}

			if(player->GetRelationMgr().HasRelation(m_pOwner->GetID()))
			{
				CLProtocol::WorldSyncOnOffline sync;
				sync.id = m_pOwner->GetID();
				sync.isOnline = 1;
				player->SendProtocol(sync);

				WSRelation* relation = m_pOwner->GetRelationMgr().GetRelation(player->GetID());
				//我是别人的徒弟
				if (relation && relation->GetType() == RelationType::RELATION_MASTER)
				{
					std::string title = SysNotifyMgr::Instance()->MakeString(8926);
					std::string content = SysNotifyMgr::Instance()->MakeString(8925, SysNotifyMgr::Instance()->MakeString(9003).c_str(), m_pOwner->GetName().c_str());
					pushNotifyToCenter(player->GetAccId(), title.c_str(), content.c_str());
				}

				//我是别人的师傅
				if (relation && relation->GetType() == RelationType::RELATION_DISCIPLE)
				{
					std::string title = SysNotifyMgr::Instance()->MakeString(8926);
					std::string content = SysNotifyMgr::Instance()->MakeString(8925, SysNotifyMgr::Instance()->MakeString(9002).c_str(), m_pOwner->GetName().c_str());
					pushNotifyToCenter(player->GetAccId(), title.c_str(), content.c_str());
				}
				
			}
			return true;
		}

		void SyncToDB()
		{
			if(m_bUpdateLevel && m_pOwner->GetLevel() > 20) WSRelationMgr::SyncLevelToDB(m_pOwner->GetID(), m_pOwner->GetLevel());
			if (m_bUpdateSeasonLv) WSRelationMgr::SyncSeasonLvToDB(m_pOwner->GetID(), m_pOwner->GetSeasonLv());
			if (m_bUpdateVipLv) WSRelationMgr::SyncVipLevelToDB(m_pOwner->GetID(), m_pOwner->GetVipLvl());
			if (m_bUpdateOccu) WSRelationMgr::SyncOccuToDB(m_pOwner->GetID(), m_pOwner->GetOccu());
			if (m_bUpdateHeadFrame) WSRelationMgr::SyncHeadFrameToDB(m_pOwner->GetID(), m_pOwner->GetHeadFrame());
			if (m_bGuildId) WSRelationMgr::SyncGuildIdToDB(m_pOwner->GetID(), m_pOwner->GetGuildID());
			if (m_bReturnYearTitle) WSRelationMgr::SyncReturnYearTitleToDB(m_pOwner->GetID(), m_pOwner->GetReturnYearTitle());
		}
		 
	private:
		WSPlayer* m_pOwner;
		bool	m_bUpdateLevel;
		bool	m_bUpdateVipLv;
		bool	m_bUpdateSeasonLv;
		bool	m_bUpdateOccu;
		bool	m_bUpdateHeadFrame;
		bool	m_bGuildId;
		bool	m_bReturnYearTitle;
	};
	RelationOnlineVisitor visitor(GetOwner());
	WSPlayerMgr::Instance()->VisitPlayers(visitor);
	visitor.SyncToDB();
	
	//std::string uids;

	std::ostringstream logss;
	bool printLog = WSApplication::Instance()->GetPrintRelationsUid();
	
	if (printLog)
	{
		logss << "syn relations of player(" << GetOwner()->GetID() << ") : ";
	}

	for(int i = RELATION_FRIEND; i < RELATION_MAX; ++i)
	{
		Avalon::Packet* packet = Avalon::Packet::Create();
		packet->SetID(CLProtocol::WORLD_SYNC_RELATION_LIST/*|CL_PACKET_COMPRESSED_FLAG*/);
		Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
		try
		{
			//stream & UInt8(i);
			for(RelationMap::iterator iter = m_Relations[i].begin();
				iter != m_Relations[i].end(); ++iter)
			{
				WSRelation* relation = iter->second;
				if (!relation)	continue;
				
				relation->UpdateGivePresentTime();
				relation->UpdateMasterGiveTime();
				
				auto player = WSPlayerMgr::Instance()->FindNormalPlayer(relation->GetID());
				if (player)
				{
					relation->SetStatus(player->GetMatchStatus());
					//回归老兵标示
					//relation->SetRegressionMark(player->IsVeteranReturn());
					// 头像框
					relation->SetHeadFrame(player->GetHeadFrame());
					// 公会ID
					relation->SetGuildId(player->GetGuildID());
					// 周年回归称号
					relation->SetReturnYearTitle(player->GetReturnYearTitle());
				}

				stream & relation->GetID() & UInt8(relation->IsOnline()?1:0);
				relation->Encode(stream, UInt32(1) << i, false);

				//uids += std::to_string(relation->GetID());
				//uids += ",";
				if (printLog && (relation->GetType() == RELATION_MASTER || relation->GetType() == RELATION_DISCIPLE))
				{
					logss << " [" << relation->GetID() << ", " << (UInt32)relation->GetType() << ", " << (UInt32)relation->GetDailyMasterTaskState() << "] ";
				}

			}
			stream & ObjID_t(0);
		}
		catch(Avalon::NetException&)
		{
			ErrorLogStream << "encode relation list error! guid=" << GetOwner()->GetID() << LogStream::eos;
			Avalon::Packet::Destroy(packet);
			return;
		}
		packet->SetSize(stream.GetPos());
		GetOwner()->SendPacket(packet);
		Avalon::Packet::Destroy(packet);
	}
	 
	//DebugLogStream << "WorldSyncRelationList player(" << GetOwner()->GetID() << ") sync uids=" << uids.c_str() << LogStream::eos;

	if (printLog)
	{
		InfoLogStream << logss.str() << LogStream::eos;
	}

	if(!m_Relations[RELATION_BLACKLIST].empty())
	{
		CLProtocol::GateSyncBlackList sync;
		sync.id = GetOwner()->GetID();
		for(RelationMap::iterator iter = m_Relations[RELATION_BLACKLIST].begin();
			iter != m_Relations[RELATION_BLACKLIST].end(); ++iter)
		{
			sync.blacklist.push_back(iter->second->GetID());
		}
		GetOwner()->SendToGate(sync);
	}

	SyncRelationListToScene();

	// 查找亲密付订单同步给客户端
	std::vector<AddonPayOrderData*> orders;
	AddonPayOrderMgr::Instance()->GetReqOrders(GetOwner()->GetID(), orders);
	AddonPayOrderMgr::Instance()->GetRepOrders(GetOwner()->GetID(), orders);

	SyncAllAddonPayDatas(orders);
}

void WSRelationMgr::OnLogicDayChange()
{
	DebugLogStream << "WSRelationMgr::OnLogicDayChange player( " << this->GetOwner()->GetID() << ")" << LogStream::eos;
	UpdateMasterDailyTaskStateOnLogicDayChange();
	UpdateRelationDebugLog debugLog("WSRelationMgr::OnLogicDayChange");
	UpdateRelation();
}

void WSRelationMgr::OnOffline()
{
	class RelationOfflineVisitor : public WSPlayerVisitor
	{
	public:
		RelationOfflineVisitor(WSPlayer* owner, UInt32 offlineTime) : m_pOwner(owner), m_offlineTime(offlineTime)
		{
			m_SyncOnOffline.id = m_pOwner->GetID();
			m_SyncOnOffline.isOnline = 0;
		}

		bool operator()(WSPlayer* player)
		{
			if(m_pOwner == player) return true;

			WSRelation* relation = player->GetRelationMgr().GetRelation(m_pOwner->GetID());
			if (relation)
			{
				player->SendProtocol(m_SyncOnOffline);
				relation->SetOfflineTime(m_offlineTime);
			}
				
			return true;
		}

	private:
		WSPlayer* m_pOwner;
		UInt32    m_offlineTime;
		CLProtocol::WorldSyncOnOffline m_SyncOnOffline;
	};

	UInt32 offlineTime = (UInt32)Avalon::Time::CurrentTime().Sec();

	RelationOfflineVisitor visitor(GetOwner(), offlineTime);
	WSPlayerMgr::Instance()->VisitNormalPlayers(visitor);

	SyncToDB();

	SyncOfflineTimeToDB(GetOwner()->GetID(), offlineTime);
}

void WSRelationMgr::OnKilled(WSPlayer* killer)
{
	if(killer == GetOwner()) return;
	if(GetOwner()->GetSrcZoneId() != 0 || killer->GetSrcZoneId() != 0) return;

	/*
	WSRelation* enemyRelation = FindRelation(RELATION_ENEMY, killer->GetID());
	if(enemyRelation == NULL)
	{
		WSRelation* relation = new WSRelation(RELATION_ENEMY, killer);
		relation->SetBeKilledTimes(1);
		if(!AddRelation(relation)) CL_SAFE_DELETE(relation);
	}
	else
	{
		if(enemyRelation->GetBeKilledTimes() < 999)
		{
			enemyRelation->SetBeKilledTimes(enemyRelation->GetBeKilledTimes() + 1);
		}
	}

	WSRelation* beEnemyRelation = killer->GetRelationMgr().FindRelation(RELATION_ENEMY, GetOwner()->GetID());
	if(beEnemyRelation != NULL)
	{
		if(beEnemyRelation->GetKillTimes() < 999)
		{
			beEnemyRelation->SetKillTimes(beEnemyRelation->GetKillTimes() + 1);
		}
	}
	*/
}

void WSRelationMgr::OnLevelChanged()
{
	//遍历关系是更好的处理方法
// 	for (UInt8 type = RELATION_FRIEND; type < RELATION_MAX; ++type)
// 	{
// 		if (!IsRelationShowLevel(type)) continue;
// 		RelationMap& relationMap = m_Relations[type];
// 		auto itr = relationMap.begin();
// 		for (; itr != relationMap.end(); ++itr)
// 		{
// 			WSRelation* relation = itr->second;
// 			if (!relation)
// 			{
// 				continue;
// 			}
// 
// 			relation->SetLevel(GetOwner()->GetLevel());
// 		}
// 	}
// 
// 	if (GetOwner()->GetLevel() >= 20)
// 	{
// 		SyncLevelToDB(GetOwner()->GetID(), GetOwner()->GetLevel());
// 	}

	//等级改变通知相关人
	class LevelChangedVisitor : public WSPlayerVisitor
	{
	public:
		LevelChangedVisitor(WSPlayer* player)
		{
			m_PlayerId = player->GetID();
			m_Level = player->GetLevel();
		}

		bool operator()(WSPlayer* player)
		{
			if(m_PlayerId == player->GetID()) return true;

			for(UInt8 type = RELATION_FRIEND; type < RELATION_MAX; ++type)
			{
				if(!IsRelationShowLevel(type)) continue;

				WSRelation* relation = player->GetRelationMgr().FindRelation(type, m_PlayerId);
				if(relation != NULL)
				{
					relation->SetLevel(m_Level);
				}
			}

			MasterSectRelation* relation = player->GetMasterSystem().GetRelation(m_PlayerId);
			if (relation != NULL)
			{
				relation->SetLevel(m_Level);
			}

			return true;
		}

	private:
		ObjID_t	m_PlayerId;
		UInt16	m_Level;
	};
	LevelChangedVisitor visitor(GetOwner());
	WSPlayerMgr::Instance()->VisitPlayers(visitor);

	if(GetOwner()->GetLevel() >= 20)
	{
		SyncLevelToDB(GetOwner()->GetID(), GetOwner()->GetLevel());
		WSMasterSystem::SyncLevelToDB(GetOwner()->GetID(), GetOwner()->GetLevel());
	}

	//发出祝福邀请
// 	if(GetOwner()->GetLevel() >= MIN_BLESSING_FRIEND_LEVEL && !IsBlessLevelLimit(GetOwner()->GetLevel()))
// 	{
// 		class RequestBlessingVisitor : public RelationVisitor
// 		{
// 		public:
// 			RequestBlessingVisitor(WSPlayer* player):m_pPlayer(player)
// 			{
// 				std::ostringstream oss;
// 				oss << int(player->GetSex()) << ',' << int(player->GetLevel());
// 				m_RequestParam = oss.str();
// 			}
// 			~RequestBlessingVisitor(){}
// 
// 			bool operator()(WSRelation* relation)
// 			{
// 				WSPlayer* pFriend = WSPlayerMgr::Instance()->FindNormalPlayer(relation->GetID());
// 				if(pFriend == NULL) return true;
// 				if(pFriend->GetLevel() < MIN_BLESSING_FRIEND_LEVEL) return true;
// 
// 				m_pPlayer->GetRequestMgr().AddRequest(REQUEST_BLESSLEVELUP, pFriend->GetID(), 60);
// 				pFriend->SyncRequest(REQUEST_BLESSLEVELUP, m_pPlayer, m_RequestParam);
// 				return true;
// 			}
// 
// 		private:
// 			//玩家
// 			WSPlayer* m_pPlayer;
// 			//协议
// 			std::string m_RequestParam;
// 		};
// 		RequestBlessingVisitor blessingVisitor(GetOwner());
// 		VisitRelations(RELATION_FRIEND, blessingVisitor);
// 	}
}

void WSRelationMgr::OnVipLvlChanged()
{
	//vip等级改变通知相关人
	class VipLevelChangedVisitor : public WSPlayerVisitor
	{
	public:
		VipLevelChangedVisitor(WSPlayer* player)
		{
			m_PlayerId = player->GetID();
			m_VipLevel = player->GetVipLvl();
		}

		bool operator()(WSPlayer* player)
		{
			if (m_PlayerId == player->GetID()) return true;

			for (UInt8 type = RELATION_FRIEND; type < RELATION_MAX; ++type)
			{
				if (!IsRelationShowVip(type)) continue;

				WSRelation* relation = player->GetRelationMgr().FindRelation(type, m_PlayerId);
				if (relation != NULL)
				{
					relation->SetVipLv((UInt8)m_VipLevel);
				}
			}

			MasterSectRelation* relation = player->GetMasterSystem().GetRelation(m_PlayerId);
			if (relation != NULL)
			{
				relation->SetVipLv((UInt8)m_VipLevel);
			}

			return true;
		}

	private:
		ObjID_t	m_PlayerId;
		UInt16	m_VipLevel;
	};
	VipLevelChangedVisitor visitor(GetOwner());
	WSPlayerMgr::Instance()->VisitPlayers(visitor);

	SyncLevelToDB(GetOwner()->GetID(), GetOwner()->GetVipLvl());	
	WSMasterSystem::SyncLevelToDB(GetOwner()->GetID(), GetOwner()->GetVipLvl());
}

void WSRelationMgr::OnOccuChanged()
{
	//职业改变通知相关人
	class OccuChangedVisitor : public WSPlayerVisitor
	{
	public:
		OccuChangedVisitor(WSPlayer* player)
		{
			m_PlayerId = player->GetID();
			m_Occu = player->GetOccu();
		}

		bool operator()(WSPlayer* player)
		{
			if (m_PlayerId == player->GetID()) return true;

			for (UInt8 type = RELATION_FRIEND; type < RELATION_MAX; ++type)
			{
				
				WSRelation* relation = player->GetRelationMgr().FindRelation(type, m_PlayerId);
				if (relation != NULL)
				{
					relation->SetOccu((UInt8)m_Occu);
				}
			}

			MasterSectRelation* relation = player->GetMasterSystem().GetRelation(m_PlayerId);
			if (relation != NULL)
			{
				relation->SetOccu((UInt8)m_Occu);
			}

			return true;
		}

	private:
		ObjID_t	m_PlayerId;
		UInt16	m_Occu;
	};
	OccuChangedVisitor visitor(GetOwner());
	WSPlayerMgr::Instance()->VisitPlayers(visitor);

	SyncOccuToDB(GetOwner()->GetID(), GetOwner()->GetOccu());
	WSMasterSystem::SyncOccuToDB(GetOwner()->GetID(), GetOwner()->GetOccu());
}

void WSRelationMgr::OnSeasonLvChanged()
{
	//段位等级改变通知相关人
	class SeasonLevelChangedVisitor : public WSPlayerVisitor
	{
	public:
		SeasonLevelChangedVisitor(WSPlayer* player)
		{
			m_PlayerId = player->GetID();
			m_SeasonLv = player->GetSeasonLv();
		}

		bool operator()(WSPlayer* player)
		{
			if (m_PlayerId == player->GetID()) return true;

			for (UInt8 type = RELATION_FRIEND; type < RELATION_MAX; ++type)
			{
				if (!IsRelationShowSeasonLv(type)) continue;

				WSRelation* relation = player->GetRelationMgr().FindRelation(type, m_PlayerId);
				if (relation != NULL)
				{
					relation->SetSeasonLv(m_SeasonLv);
				}
			}
			return true;
		}

	private:
		ObjID_t	m_PlayerId;
		UInt32	m_SeasonLv;
	};
	SeasonLevelChangedVisitor visitor(GetOwner());
	WSPlayerMgr::Instance()->VisitPlayers(visitor);

	SyncSeasonLvToDB(GetOwner()->GetID(), GetOwner()->GetSeasonLv());
}

void WSRelationMgr::OnServerChanged()
{
	SyncRelationListToScene();
}

//每秒心跳只处理db保存,数据同步改为用户请求时同步
void WSRelationMgr::OnHeartbeat(const Avalon::Time& now)
{
	bool bSyncDB = m_SyncDBTimer.IsTimeout(now);

	class OnTickVisitor : public RelationVisitor
	{
	public:
		OnTickVisitor(WSPlayer* owner, bool bSyncDB):m_pOwner(owner),m_bSyncDB(bSyncDB)
		{
// 			m_pPacket = Avalon::Packet::Create();
// 			m_pPacket->SetID(CLProtocol::WORLD_SYNC_RELATIONDATA);
		}

		~OnTickVisitor()
		{
			//Avalon::Packet::Destroy(m_pPacket);
		}

		bool operator()(WSRelation* relation)
		{
			if(m_bSyncDB) relation->SyncToDB();

// 			Avalon::NetOutputStream stream(m_pPacket->GetBuffer(), m_pPacket->GetMaxSize());
// 			stream & relation->GetType() & relation->GetID();
// 			if(relation->Encode(stream, UInt32(1) << relation->GetType(), true))
// 			{
// 				m_pPacket->SetSize(UInt16(stream.GetPos()));
// 				m_pOwner->SendPacket(m_pPacket);
// 				m_pPacket->SetSize(0);
// 			}
			return true;
		}
	private:
		WSPlayer* m_pOwner;
		bool	m_bSyncDB;
		//Avalon::Packet* m_pPacket;
	};
	OnTickVisitor visitor(GetOwner(), bSyncDB);
	VisitRelations(visitor);
}

void WSRelationMgr::FiveMinHb()
{
	for (int i = RelationType::RELATION_FRIEND; i < RelationType::RELATION_MAX; ++i)
	{
		ClearRequestPlayer((RelationType)i);
	}
	
}

void WSRelationMgr::OnNameChanged(ObjID_t playerId, const std::string& name)
{
	//姓名改变通知相关人
	class NameChangedVisitor : public WSPlayerVisitor
	{
	public:
		NameChangedVisitor(ObjID_t id, const std::string& name)
			:m_PlayerId(id), m_Name(name)
		{
		}

		bool operator()(WSPlayer* player)
		{
			if(player->GetID() == m_PlayerId) return true;

			for(UInt8 type = RELATION_FRIEND; type < RELATION_MAX; ++type)
			{
				WSRelation* relation = player->GetRelationMgr().FindRelation(type, m_PlayerId);
				if(relation != NULL)
				{
					relation->SetName(m_Name);
					if (type == RELATION_MASTER)
					{
						player->GetTitleMgr().OnMasterNameChange();
					}
				}
			}
			return true;
		}

	private:
		ObjID_t	m_PlayerId;
		std::string	m_Name;
	};
	NameChangedVisitor visitor(playerId, name);
	WSPlayerMgr::Instance()->VisitPlayers(visitor);

	SyncNameToDB(playerId, name);
}

void WSRelationMgr::OnSexChanged(ObjID_t playerId, UInt8 sex)
{
	//性别改变通知相关人
	class SexChangedVisitor : public WSPlayerVisitor
	{
	public:
		SexChangedVisitor(ObjID_t id, UInt8 sex)
			:m_PlayerId(id), m_Sex(sex)
		{
		}

		bool operator()(WSPlayer* player)
		{
			if(player->GetID() == m_PlayerId) return true;

			for(UInt8 type = RELATION_FRIEND; type < RELATION_MAX; ++type)
			{
				WSRelation* relation = player->GetRelationMgr().FindRelation(type, m_PlayerId);
				if(relation != NULL)
				{
					relation->SetSex(m_Sex);
				}
			}
			return true;
		}

	private:
		ObjID_t	m_PlayerId;
		UInt8	m_Sex;
	};
	SexChangedVisitor visitor(playerId, sex);
	WSPlayerMgr::Instance()->VisitPlayers(visitor);

	SyncSexToDB(playerId, sex);
}

void WSRelationMgr::OnRelationTypeChanged()
{

}

void WSRelationMgr::OnPowerChanged()
{
// 	战斗力改变通知相关人
// 			class PowerChangedVisitor : public WSPlayerVisitor
// 			{
// 			public:
// 				PowerChangedVisitor(WSPlayer* player)
// 				{
// 					m_PlayerId = player->GetID();
// 					m_Power = player->GetPower();
// 					m_bUpdatePower = false;
// 				}
// 		
// 				bool operator()(WSPlayer* player)
// 				{
// 					if(m_PlayerId == player->GetID()) return true;
// 		
// 					for(UInt8 type = RELATION_FRIEND; type <= RELATION_BLACKLIST; ++type)
// 					{
// 						if(!IsRelationShowPower(type)) continue;
// 		
// 						WSRelation* relation = player->GetRelationMgr().FindRelation(type, m_PlayerId);
// 						if(relation != NULL)
// 						{
// 							if(relation->GetGiveResetTime() < m_Power)
// 							{
// 								m_bUpdatePower = m_Power > relation->GetGiveResetTime() + 100;
// 								relation->SetGiveResetTime(m_Power);
// 							}
// 						}
// 					}
// 		
// 					return true;
// 				}
// 		
// 				bool NeedUpdatePower() { return m_bUpdatePower; }
// 		
// 			private:
// 				ObjID_t	m_PlayerId;
// 				UInt32	m_Power;
// 				bool m_bUpdatePower;
// 			};
// 			PowerChangedVisitor visitor(GetOwner());
// 			WSPlayerMgr::Instance()->VisitPlayers(visitor);
// 		
// 			if(visitor.NeedUpdatePower() && GetOwner()->GetLevel() >= 30)
// 				SyncPowerToDB(GetOwner()->GetID(), GetOwner()->GetPower());
}

void WSRelationMgr::OnStatusChanged(UInt8 status)
{
	//状态改变通知相关人
	class StatusChangedVisitor : public WSPlayerVisitor
	{
	public:
		StatusChangedVisitor(ObjID_t id, UInt8 status)
			:m_PlayerId(id), m_Status(status)
		{
		}

		bool operator()(WSPlayer* player)
		{
			if (player->GetID() == m_PlayerId) return true;

			for (UInt8 type = RELATION_FRIEND; type < RELATION_MAX; ++type)
			{
				WSRelation* relation = player->GetRelationMgr().FindRelation(type, m_PlayerId);
				if (relation != NULL)
				{
					relation->SetStatus(m_Status);
				}
			}
			
			if (!WSApplication::Instance()->IsOptimUpdateRelation())
			{
				UpdateRelationDebugLog debugLog("WSRelationMgr::OnStatusChanged");
				player->GetRelationMgr().UpdateRelation();
			}

			MasterSectRelation* relation = player->GetMasterSystem().GetRelation(m_PlayerId);
			if (relation != NULL)
			{
				relation->SetStatus(m_Status);
			}

			return true;
		}

	private:
		ObjID_t	m_PlayerId;
		UInt8	m_Status;
	};

	StatusChangedVisitor visitor(GetOwner()->GetID(), status);
	WSPlayerMgr::Instance()->VisitPlayers(visitor);
}

void WSRelationMgr::OnHeadFrameChanged()
{
	//头像框改变通知相关人
	class HeadFrameChangedVisitor : public WSPlayerVisitor
	{
	public:
		HeadFrameChangedVisitor(WSPlayer* player)
		{
			m_PlayerId = player->GetID();
			m_HeadFrame = player->GetHeadFrame();
		}

		bool operator()(WSPlayer* player)
		{
			if (m_PlayerId == player->GetID()) return true;

			for (UInt8 type = RELATION_FRIEND; type < RELATION_MAX; ++type)
			{
				WSRelation* relation = player->GetRelationMgr().FindRelation(type, m_PlayerId);
				if (relation != NULL)
				{
					relation->SetHeadFrame(m_HeadFrame);
				}
			}
			return true;
		}

	private:
		ObjID_t	m_PlayerId;
		UInt32	m_HeadFrame;
	};

	HeadFrameChangedVisitor visitor(GetOwner());
	WSPlayerMgr::Instance()->VisitPlayers(visitor);

	SyncHeadFrameToDB(GetOwner()->GetID(), GetOwner()->GetHeadFrame());
}

void WSRelationMgr::OnGuildChanged()
{
	//公会改变通知相关人
	class GuildChangedVisitor : public WSPlayerVisitor
	{
	public:
		GuildChangedVisitor(WSPlayer* player)
		{
			m_PlayerId = player->GetID();
			m_GuildId = player->GetGuildID();
		}

		bool operator()(WSPlayer* player)
		{
			if (m_PlayerId == player->GetID()) return true;

			for (UInt8 type = RELATION_FRIEND; type < RELATION_MAX; ++type)
			{
				WSRelation* relation = player->GetRelationMgr().FindRelation(type, m_PlayerId);
				if (relation != NULL)
				{
					relation->SetGuildId(m_GuildId);
				}
			}
			return true;
		}

	private:
		ObjID_t	m_PlayerId;
		UInt64	m_GuildId;
	};

	GuildChangedVisitor visitor(GetOwner());
	WSPlayerMgr::Instance()->VisitPlayers(visitor);

	SyncGuildIdToDB(GetOwner()->GetID(), GetOwner()->GetGuildID());
}

void WSRelationMgr::OnReturnYearTitle()
{
	//回归周年称号通知相关人
	class ReturnYearTitleChangedVisitor : public WSPlayerVisitor
	{
	public:
		ReturnYearTitleChangedVisitor(WSPlayer* player)
		{
			m_PlayerId = player->GetID();
			m_ReturnYearTitle = player->GetReturnYearTitle();
		}

		bool operator()(WSPlayer* player)
		{
			if (m_PlayerId == player->GetID()) return true;

			for (UInt8 type = RELATION_FRIEND; type < RELATION_MAX; ++type)
			{
				WSRelation* relation = player->GetRelationMgr().FindRelation(type, m_PlayerId);
				if (relation != NULL)
				{
					relation->SetReturnYearTitle(m_ReturnYearTitle);
				}
			}
			return true;
		}

	private:
		ObjID_t	m_PlayerId;
		UInt32	m_ReturnYearTitle;
	};

	ReturnYearTitleChangedVisitor visitor(GetOwner());
	WSPlayerMgr::Instance()->VisitPlayers(visitor);

	SyncReturnYearTitleToDB(GetOwner()->GetID(), GetOwner()->GetReturnYearTitle());
}

bool WSRelationMgr::CheckRelationTypePriority(RelationType typeNew, RelationType typeOld)
{
	if (typeOld == RelationType::RELATION_BLACKLIST || typeOld == RelationType::RELATION_STRANGER)
	{
		return true;
	}

	if (typeOld == RelationType::RELATION_FRIEND &&
		(typeNew == RelationType::RELATION_MASTER || typeNew == RelationType::RELATION_DISCIPLE))
	{
		return true;
	}

	return false;
}

bool WSRelationMgr::CheckRelationOpenLv(RelationType type)
{
	if (type == RelationType::RELATION_FRIEND)
	{
		auto functionData = FuncNotifyDataEntryMgr::Instance()->FindEntry(FUNC_FRIEND);
		if (functionData)
		{
			if (GetOwner()->GetLevel() < functionData->endLevel)
			{
				return false;
			}
		}
	}
	else if (type == RelationType::RELATION_DISCIPLE || type == RelationType::RELATION_MASTER)
	{
		if (GetOwner()->GetLevel() < SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_TAP_SYSTEM_OPEN_LEVEL))
		{
			return false;
		}
	}

	
	return true;
}

void WSRelationMgr::OnRequestPlayer(RelationType type, ObjID_t tarId)
{
	if (type < RelationType::RELATION_FRIEND || type >= RelationType::RELATION_MAX)
	{
		return;
	}

	std::set<ObjID_t>& playerIDs = m_RequstedPlayerID[type];
	if (playerIDs.find(tarId) == playerIDs.end())
	{
		playerIDs.insert(tarId);
	}

}

void WSRelationMgr::ClearRequestPlayer(RelationType type)
{
	if (type < RelationType::RELATION_FRIEND || type >= RelationType::RELATION_MAX)
	{
		return;
	}

	std::set<ObjID_t>& playerIDs = m_RequstedPlayerID[type];
	playerIDs.clear();
}

bool WSRelationMgr::FindRequestPlayer(RelationType type, ObjID_t tarId)
{
	if (type < RelationType::RELATION_FRIEND || type >= RelationType::RELATION_MAX)
	{
		return false;
	}

	std::set<ObjID_t>& playerIDs = m_RequstedPlayerID[type];
	if (playerIDs.size() == 0)
	{
		return true;
	}

	return (playerIDs.find(tarId) == playerIDs.end());
}

void WSRelationMgr::OnAddFriend(WSRelation* relation)
{
	if (relation == NULL) return;
	CLProtocol::SceneSyncChat protocol;
	protocol.channel = CHAT_CHANNEL_PRIVATE;
	protocol.objid = relation->GetID();

	protocol.receiverId = GetOwner()->GetID();
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	protocol.bLink = true;
	protocol.isGm = 0;
	protocol.voiceKey = "";
	protocol.voiceDuration = 0;
	SysNotifyMgr::Instance()->MakeNotify(9897, buffer, sizeof(buffer), relation->GetName().c_str());
	protocol.word = buffer;
	protocol.mask = ChatMask::CHAT_MASK_ADD_FRIEND | ChatMask::CHAT_MASK_NOT_HEADPOINT;
	protocol.headFrame = relation->GetHeadFrame();
	GetOwner()->SendProtocol(protocol);

	WSPlayer* target = WSPlayerMgr::Instance()->FindNormalPlayer(relation->GetID()); 
	if (target)
	{
		CLProtocol::SceneSyncChat protocol;
		protocol.channel = CHAT_CHANNEL_PRIVATE;
		protocol.objid = GetOwner()->GetID();
		protocol.objname = GetOwner()->GetName();
		protocol.receiverId = target->GetID();
		protocol.level = GetOwner()->GetLevel();
		protocol.occu = GetOwner()->GetOccu();
		protocol.sex = GetOwner()->GetSex();
		protocol.viplvl = GetOwner()->GetVipLvl();

		protocol.bLink = true;
		protocol.isGm = 0;
		protocol.voiceKey = "";
		protocol.voiceDuration = 0;
		protocol.headFrame = GetOwner()->GetHeadFrame();
		if (target->GetRelationMgr().FindRelation(RelationType::RELATION_FRIEND, GetOwner()->GetID()))
		{
			char buffer[1024];
			memset(buffer, 0, sizeof(buffer));
			SysNotifyMgr::Instance()->MakeNotify(9979, buffer, sizeof(buffer), GetOwner()->GetName().c_str());
			protocol.word = buffer;
		}
		else{
			std::string systemLinkStr = "";
			MsgPackage::GetSystemSuffixesMsgTag64(systemLinkStr, SystemSuffixesType::SYSTEM_SUFFIXES_TYPE_ADD_FRIEND, GetOwner()->GetID());
			//SysNotifyMgr::Instance()->MakeNotify(9978, buffer, sizeof(buffer), GetOwner()->GetName().c_str(), GetOwner()->GetID());
			std::string content = SysNotifyMgr::Instance()->MakeString(9978, GetOwner()->GetName().c_str(), systemLinkStr);
			protocol.word = content;
		}
		protocol.mask = ChatMask::CHAT_MASK_ADD_FRIEND;
		target->SendProtocol(protocol);
	}
}

void WSRelationMgr::OnChangeRegression(bool bRegress)
{
	class RelationOnlineVisitor : public WSPlayerVisitor
	{
	public:
		RelationOnlineVisitor(WSPlayer* owner, bool isRegress)
			:m_pOwner(owner), m_isRegress(isRegress) {}
		bool operator()(WSPlayer* player)
		{
			if (m_pOwner == player) return true;
			for (UInt8 i = RELATION_FRIEND; i < RELATION_MAX; ++i)
			{
				WSRelation* relation = player->GetRelationMgr().FindRelation(i, m_pOwner->GetID());
				if (relation != NULL)
				{
					relation->SetRegressionMark(m_isRegress);
				}
			}
			return true;
		}
	private:
		WSPlayer*	m_pOwner;
		bool		m_isRegress;
	};
	
	RelationOnlineVisitor visitor(GetOwner(), bRegress);
	WSPlayerMgr::Instance()->VisitPlayers(visitor);
	
	WSRelationMgr::SyncRegressionMarkToDB(GetOwner()->GetID(), bRegress);
}

void WSRelationMgr::SendNotifyDelBlackList(ObjID_t myId, ObjID_t tarId)
{
	CLProtocol::GateNotifyDelBlackList notify;
	notify.id = myId;
	notify.target = tarId;
	GetOwner()->SendToGate(notify);
}

void WSRelationMgr::SendNotifySceneDel(ObjID_t myId, RelationType type, ObjID_t tarId)
{
	CLProtocol::SceneRelationNotifyDel notify;
	notify.id = myId;
	notify.relationType = type;
	notify.tarId = tarId;
	GetOwner()->SendToScene(notify);
}

void WSRelationMgr::RequestAddonPayData(UInt32 goodId, UInt64 tarId)
{

}

void WSRelationMgr::SyncAddonPayData(AddonPayOrderData* order)
{
	CLProtocol::WorldSyncAddonPayData proto;

	if (!TransOrderToNetData(order, proto.data))
	{
		return;
	}

	GetOwner()->SendProtocol(proto);
}

void WSRelationMgr::SyncAllAddonPayDatas(std::vector<AddonPayOrderData*>& orders)
{
	CLProtocol::WorldSyncAllAddonPayDatas proto;

	for (size_t i = 0; i < orders.size(); ++i)
	{
		AddonPayData netData;
		if (!TransOrderToNetData(orders[i], netData))
		{
			continue;
		}

		proto.datas.push_back(netData);
	}

	GetOwner()->SendProtocol(proto);
}

void WSRelationMgr::SendPlayerDelRelation(RelationType type, ObjID_t tarId)
{
	CLProtocol::WorldNotifyDelRelation notify;
	notify.type = (UInt8)type;
	notify.id = tarId;
	GetOwner()->SendProtocol(notify);
}

void WSRelationMgr::SendNotifyNewRelation(RelationType type, ObjID_t tarId)
{
	CLProtocol::SceneRelationNotifyNew notify;
	notify.id = GetOwner()->GetID();
	notify.relationType = type;
	notify.tarId = tarId;
	GetOwner()->SendToScene(notify);
}

bool WSRelationMgr::TransOrderToNetData(AddonPayOrderData* order, AddonPayData& netData)
{
	if (!order)
	{
		return false;
	}

	MallItemInfo* mallItem = MallMgr::Instance()->GetMallItemInfo(order->goodId);
	if (!mallItem)
	{
		return false;
	}

	AddonPayType type;
	std::string name;
	UInt8 occu;
	UInt16 lv;

	// 判断是请求者还是接收者
	if (order->reqId == GetOwner()->GetID())
	{
		type = AddonPayType::APT_SELF;
		name = order->tarName;
		occu = order->tarOccu;
		lv = order->tarLv;
	}
	else if (order->tarId == GetOwner()->GetID())
	{
		type = AddonPayType::APT_OTHER;
		name = order->reqName;
		occu = order->reqOccu;
		lv = order->reqLevel;
	}
	else
	{
		return false;
	}

	netData.uid = order->uid;
	netData.type = type;
	netData.tarName = name;
	netData.tarOccu = occu;
	netData.tarLv = lv;
	netData.dueTime = order->dueTime;
	netData.state = order->state;
	netData.payMoneyType = mallItem->moneytype;
	netData.payMoneyNum = mallItem->price * order->buyNum;

	return true;
}

void WSRelationMgr::SyncToDB()
{
	class SyncToDBVisitor : public RelationVisitor
	{
	public:
		SyncToDBVisitor(WSPlayer* owner):m_pOwner(owner){}

		bool operator()(WSRelation* relation)
		{
			relation->SyncToDB();
			return true;
		}
	private:
		WSPlayer* m_pOwner;
	};
	SyncToDBVisitor visitor(GetOwner());
	VisitRelations(visitor);
}

void WSRelationMgr::SyncRelationTypeToDB(ObjID_t owner, ObjID_t tar, UInt8 type)
{
	std::ostringstream condstream;
	condstream << "owner =" << owner << " and `id`=" << tar;
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_relation", condstream.str());
	updateCmd->PutData("type", type);
	CLRecordClient::Instance()->SendCommand(updateCmd);
}

void WSRelationMgr::SyncLevelToDB(ObjID_t playerId, UInt16 level)
{
	std::ostringstream condstream;
	condstream << "`id`=" << playerId;
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_relation", condstream.str());
	updateCmd->PutData("level", level);
	CLRecordClient::Instance()->SendCommand(updateCmd);
}

void WSRelationMgr::SyncHeadFrameToDB(ObjID_t playerId, UInt32 id)
{
	std::ostringstream condstream;
	condstream << "`id`=" << playerId;
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_relation", condstream.str());
	updateCmd->PutData("head_frame", id);
	CLRecordClient::Instance()->SendCommand(updateCmd);
}

void WSRelationMgr::SyncGuildIdToDB(ObjID_t playerId, UInt64 id)
{
	std::ostringstream condstream;
	condstream << "`id`=" << playerId;
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_relation", condstream.str());
	updateCmd->PutData("guild_id", id);
	CLRecordClient::Instance()->SendCommand(updateCmd);
}

void WSRelationMgr::SyncReturnYearTitleToDB(ObjID_t playerId, UInt32 title)
{
	std::ostringstream condstream;
	condstream << "`id`=" << playerId;
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_relation", condstream.str());
	updateCmd->PutData("return_year_title", title);
	CLRecordClient::Instance()->SendCommand(updateCmd);
}

void WSRelationMgr::SyncVipLevelToDB(ObjID_t playerId, UInt16 vipLvl)
{
	std::ostringstream condstream;
	condstream << "`id`=" << playerId;
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_relation", condstream.str());
	updateCmd->PutData("viplv", vipLvl);
	CLRecordClient::Instance()->SendCommand(updateCmd);
}

void WSRelationMgr::SyncOccuToDB(ObjID_t playerId, UInt8 occu)
{
	std::ostringstream condstream;
	condstream << "`id`=" << playerId;
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_relation", condstream.str());
	updateCmd->PutData("occu", occu);
	CLRecordClient::Instance()->SendCommand(updateCmd);
}

void WSRelationMgr::SyncSeasonLvToDB(ObjID_t playerId, UInt32 seasonLv)
{
	std::ostringstream condstream;
	condstream << "`id`=" << playerId;
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_relation", condstream.str());
	updateCmd->PutData("seasonlv", seasonLv);
	CLRecordClient::Instance()->SendCommand(updateCmd);
}

void WSRelationMgr::SyncNameToDB(ObjID_t playerId, const std::string& name)
{
	std::ostringstream condstream;
	condstream << "`id`=" << playerId;
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_relation", condstream.str());
	updateCmd->PutData("name", name);
	CLRecordClient::Instance()->SendCommand(updateCmd);
}

void WSRelationMgr::SyncSexToDB(ObjID_t playerId, UInt8 sex)
{
	std::ostringstream condstream;
	condstream << "`id`=" << playerId;
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_relation", condstream.str());
	updateCmd->PutData("sex", sex);
	CLRecordClient::Instance()->SendCommand(updateCmd);
}

void SyncSeasonLvToDB(ObjID_t playerId, UInt32	seasonLv)
{
	std::ostringstream condstream;
	condstream << "`id`=" << playerId << " and seasonlv < " << seasonLv;
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_relation", condstream.str());
	updateCmd->PutData("seasonlv", seasonLv);
	CLRecordClient::Instance()->SendCommand(updateCmd);
}

void WSRelationMgr::SyncQQvipToDB(ObjID_t playerId, UInt8 qqVipLvl, UInt8 superBlue)
{
	std::ostringstream condstream;
	condstream << "`id`=" << playerId;
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_relation", condstream.str());
	updateCmd->PutData("qqvip", qqVipLvl);
	updateCmd->PutData("issb", superBlue);
	CLRecordClient::Instance()->SendCommand(updateCmd);
}

void WSRelationMgr::SyncMasterDailyTaskState(ObjID_t owner, ObjID_t tar, UInt8 type, UInt8 state)
{
	std::ostringstream condstream;
	condstream << "owner =" << owner << " and `id`=" << tar << " and `type`=" << (Int32)type;
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_relation", condstream.str());
	updateCmd->PutData("daily_mastertask_state", state);
	CLRecordClient::Instance()->SendCommand(updateCmd);
}

void WSRelationMgr::UpdateMasterDailyTaskStateOnLogicDayChange()
{
	//更新师傅,徒弟关系
	class MasterDiscipleRelationVisitor : public RelationVisitor
	{
	public:
		bool operator()(WSRelation* object)
		{
			object->UpdateDailyMasterTaskStateOnLogicDayChange();
			return true;
		}
	};
	MasterDiscipleRelationVisitor relationVisitor;
	this->VisitRelations(RelationType::RELATION_DISCIPLE, relationVisitor);
	this->VisitRelations(RelationType::RELATION_MASTER, relationVisitor);
}

void WSRelationMgr::UpdateRelationTypeToDb(ObjID_t owner, UInt8 type, ObjID_t id, UInt8 newType)
{
	std::ostringstream condstream;
	condstream << "owner =" << owner << " and `type`=" << (Int32)type << " and `id`=" << id;
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_relation", condstream.str());
	updateCmd->PutData("type", newType);
	CLRecordClient::Instance()->SendCommand(updateCmd);
}

void WSRelationMgr::SyncRegressionMarkToDB(ObjID_t playerId, UInt8 IsRegress)
{
	std::ostringstream condstream;
	condstream << "`id`=" << playerId;
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_relation", condstream.str());
	updateCmd->PutData("isRegress", IsRegress);
	CLRecordClient::Instance()->SendCommand(updateCmd);
}

void WSRelationMgr::SyncOfflineTimeToDB(ObjID_t playerId, UInt32 offlineTime)
{
	std::ostringstream condstream;
	condstream << "`id`=" << playerId;
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_relation", condstream.str());
	updateCmd->PutData("offline_time", offlineTime);
	CLRecordClient::Instance()->SendCommand(updateCmd);
}

bool WSRelationMgr::IsBlessLevelLimit( UInt16 level )
{
	switch(level)
	{
	case 21:
	case 23:
	case 25:
	case 27:
	case 29:
	case 31:
		{
			return true;
		}
		break;
	}

	return false;
}

void WSRelationMgr::UpdateRelation()
{
	class OnUpdateVisitor : public RelationVisitor
	{
	public:
		OnUpdateVisitor(WSPlayer* owner) :m_pOwner(owner)
		{
			m_pPacket = Avalon::Packet::Create();
			m_pPacket->SetID(CLProtocol::WORLD_SYNC_RELATIONDATA);
		}

		~OnUpdateVisitor()
		{
			Avalon::Packet::Destroy(m_pPacket);

			for (size_t i = 0; i < needDelRelations.size(); ++i)
			{
				m_pOwner->GetRelationMgr().RemoveRelation(needDelRelations[i]);
			}
		}

		bool operator()(WSRelation* relation)
		{
			if (relation == NULL) return true;

			// 陌生人赠礼时间到了删除关系(保存赠礼次数和重置时间为了判断同样的人重新加关系后沿用次数)
			if (relation->GetType() == RelationType::RELATION_STRANGER &&
				(UInt32)CURRENT_TIME.Sec() >= relation->GetGiveResetTime() &&
				(UInt32)CURRENT_TIME.Sec() >= relation->GetMasterGiveResetTime())
			{
				needDelRelations.push_back(relation);
			}

			//好友赠送数量刷新
			relation->UpdateGivePresentTime();
			relation->UpdateMasterGiveTime();

			//回归老兵标示
			WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(relation->GetID());
			if (player)
			{
				//relation->SetRegressionMark(player->IsVeteranReturn());
			}

			//同步关系
			Avalon::NetOutputStream stream(m_pPacket->GetBuffer(), m_pPacket->GetMaxSize());
			stream & relation->GetID();
			if (relation->Encode(stream, UInt32(1) << relation->GetType(), true))
			{
				m_pPacket->SetSize(UInt16(stream.GetPos()));
				m_pOwner->SendPacket(m_pPacket);
				m_pPacket->SetSize(0);
				//DebugLogStream << "WorldSyncRelationData relationId(" << relation->GetGuid() << ") owner(" << m_pOwner->GetID() << ") targetId(" << relation->GetID() << LogStream::eos;
			}
			return true;
		}
	private:
		WSPlayer* m_pOwner;
		Avalon::Packet* m_pPacket;
		std::vector<WSRelation*> needDelRelations;
	};
	OnUpdateVisitor visitor(GetOwner());
	VisitRelations(visitor);
}

void WSRelationMgr::UpdateRelation(ObjID_t uid, WSRelation* oldRelation, UInt8 newType)
{
	if (newType >= RELATION_MAX || newType < RELATION_FRIEND)
		return;

	if (!oldRelation)	return;

	RelationMap& oldRelations = m_Relations[oldRelation->GetType()];
	if (oldRelations.empty()) return;

	auto itr = oldRelations.find(uid);
	if (itr == oldRelations.end())	return;
	oldRelations.erase(itr);

	//更新自己
	RelationMap& newRelations = m_Relations[newType];
	newRelations.insert(std::make_pair(uid, oldRelation));
	oldRelation->SetType(newType);
	oldRelation->SyncToDB();

	//更新到场景
	SendNotifyNewRelation((RelationType)newType, uid);

	UpdateRelation();
}

void WSRelationMgr::SetMasterNote(std::string note, bool syncDB)
{
	if (note.size() >= MASTER_NOTE_MAX_BYTE)
	{
		return;
	}

	if (0 == m_MasterNote.compare(note))
	{
		return;
	}

	m_MasterNote = note;

	if (syncDB)
	{
		CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_player_info", GetOwner()->GetID(), false);
		cmd->PutData("master_note", m_MasterNote);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
	
}

void WSRelationMgr::SetIsRecvDisciple(bool isRecv, bool syncDB)
{
	if (m_IsRecvDisciple == isRecv)
	{
		return;
	}

	m_IsRecvDisciple = isRecv;

	if (syncDB)
	{
		CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_player_info", GetOwner()->GetID(), false);
		cmd->PutData("is_recv_disciple", m_IsRecvDisciple);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
	
}

void WSRelationMgr::RemoveAllRelations()
{
	for (int i = RELATION_FRIEND; i < RELATION_MAX; ++i)
	{
		RelationMap& relationMap = m_Relations[i];
		for (RelationMap::iterator itr = relationMap.begin();
			itr != relationMap.end(); ++itr)
		{
			WSRelation* relation = itr->second;
			if (relation)
			{
				if (i == RELATION_MASTER || i == RELATION_DISCIPLE)
				{
					int targetRelationType = i == RELATION_MASTER ? RELATION_DISCIPLE : RELATION_MASTER;

					//删除对方
					WSPlayer* target = WSPlayerMgr::Instance()->FindNormalPlayer(relation->GetID());
					if (target != NULL)
					{
						target->GetRelationMgr().RemoveRelation(targetRelationType, GetOwner()->GetID());
					}
					else
					{
						std::ostringstream oss;
						oss << "`id`='" << relation->GetID() << "' AND `owner`='" << GetOwner()->GetID() << "' AND `type`='" << (Int32)targetRelationType << "'";
						CLDeleteCommand* delCommand = CLRecordClient::Instance()->PrepareDeleteCommand("t_relation", oss.str());
						CLRecordClient::Instance()->SendCommand(delCommand);
					}

				}

				//删除自己
				//从数据库中删除
				relation->DelMe();
	
				//通知网关移除黑名单
				if (relation->GetType() == RELATION_BLACKLIST)
				{
					SendNotifyDelBlackList(GetOwner()->GetID(), relation->GetID());
				}
				//通知场景移除好友
				SendNotifySceneDel(GetOwner()->GetID(), (RelationType)relation->GetType(), relation->GetID());

				CL_SAFE_DELETE(relation);
			}
		}

		relationMap.clear();
	}
}

void WSRelationMgr::OnDeletePlayer(WSRelation* relation)
{
	if (relation == NULL || relation->GetType() >= RELATION_MAX) return;

	relation->DelMe();

	//通知网关移除黑名单
	if (relation->GetType() == RELATION_BLACKLIST)
	{
		SendNotifyDelBlackList(GetOwner()->GetID(), relation->GetID());
	}
	//通知场景移除好友
	SendNotifySceneDel(GetOwner()->GetID(), (RelationType)relation->GetType(), relation->GetID());
}

/*
void WSRelationMgr::QueryBKOpRecord(UInt16 uStartNo, UInt16 uNum, Avalon::NetOutputStream &stream)
{
	if(uStartNo > m_Relations[RELATION_FRIEND].size())
	{
		stream & UInt64(0);
		return;
	}

	if(uNum > 20)
		uNum = 20;

	std::vector<WSRelation*>	relations;
	for(RelationMap::iterator iter = m_Relations[RELATION_FRIEND].begin(); iter != m_Relations[RELATION_FRIEND].end(); iter++)
	{
		if(iter->second->GetBKOpTime() > 0)
			relations.push_back(iter->second);
	}

	UInt8	uPages = 1;
	if(relations.size() > 0)
	{
		uPages = UInt8((relations.size() - 1) / uNum + 1);
	}
	stream & uPages;

	RelationBkOpTimeSorter	sorter;
	std::sort(relations.begin(), relations.end(), sorter);
	std::vector<WSRelation*>::iterator	itervec = relations.begin() + uStartNo;

	UInt16	uCount = 0;
	for(; itervec != relations.end(); itervec++)
	{
		WSRelation	*pRelation = *itervec;
		stream & pRelation->GetId() & pRelation->GetWateringTimes() & pRelation->GetDeepTimes();

		uCount++;
		if(uCount >= uNum)
			break;
	}

	stream & UInt64(0);
}
*/

bool WSRelationMgr::IsMutualFriend(WSPlayer* player1, WSPlayer* player2)
{
	if (!player1 || !player2)
	{
		return false;
	}
	if (player1->GetRelationMgr().FindRelation(RELATION_FRIEND, player2->GetID())
		&& player2->GetRelationMgr().FindRelation(RELATION_FRIEND, player1->GetID()))
	{
			return true;
	}
	return false;
}

bool WSRelationMgr::IsMasterDisciple(WSPlayer* player1, WSPlayer* player2)
{
	if (!player1 || !player2)
	{
		return false;
	}
	if (player1->GetRelationMgr().FindRelation(RELATION_MASTER, player2->GetID())
		|| player2->GetRelationMgr().FindRelation(RELATION_MASTER, player1->GetID()))
	{
		return true;
	}
	return false;
}

UInt32 WSRelationMgr::SetPlayerRemark(ObjID_t roleId, std::string& remark)
{
	WSRelation* relation = this->GetRelation(roleId);
	if (relation == NULL)
	{
		return ErrorCode::MASTERSYS_NOTFIND_PLAYER;
	}

	if (remark.size() == 0)
	{
		if (relation->GetRemark().size() > 0)
		{
			relation->SetRemark(remark);
			GetOwner()->SendNotify(9982);
			UpdateRelation();
		}
		return ErrorCode::SUCCESS;
	}

	char remarkbuf[NAME_LENGTH + 1];
	avalon_strncpy(remarkbuf, remark.c_str(), NAME_LENGTH);
	
	if (relation->GetRemark().compare(remarkbuf) == 0)
	{
		return ErrorCode::RELATION_SETREMARK_UNCHANGE_ERROR;
	}

	relation->SetRemark(remarkbuf);
	UpdateRelationDebugLog debugLog("WSRelationMgr::SetPlayerRemark");
	UpdateRelation();

	return ErrorCode::SUCCESS;
}

void WSRelationMgr::NotifyRelationReturn()
{
	UInt32 num = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_RETURN_HELL_DROP_DAILY_MAX_NUM);

	CLProtocol::SceneSyncChat protocol;
	protocol.channel = CHAT_CHANNEL_PRIVATE;
	protocol.objid = GetOwner()->GetID();
	protocol.occu = GetOwner()->GetOccu();
	protocol.sex = GetOwner()->GetSex();
	protocol.level = GetOwner()->GetLevel();
	protocol.viplvl = GetOwner()->GetVipLvl();
	protocol.objname = GetOwner()->GetName();
	protocol.bLink = false;
	protocol.isGm = 0;
	protocol.voiceKey = "";
	protocol.voiceDuration = 0; 
	protocol.headFrame = GetOwner()->GetHeadFrame();
	protocol.word = SysNotifyMgr::Instance()->MakeString(9999, num);

	if (HasRelation(UInt8(RELATION_FRIEND)))
		NotifyRelationReturnMsg(RELATION_FRIEND, protocol);

	if (HasRelation(UInt8(RELATION_MASTER)))
		NotifyRelationReturnMsg(RELATION_MASTER, protocol);

	if (HasRelation(UInt8(RELATION_DISCIPLE)))
		NotifyRelationReturnMsg(RELATION_DISCIPLE, protocol);
}

void WSRelationMgr::NotifyRelationReturnMsg(UInt32 type, CLProtocol::SceneSyncChat& chat)
{
	for (auto& iter : m_Relations[type])
	{
		if (type == RELATION_FRIEND && !iter.second->IsMutualFriend())
			continue;

		UInt64 targetId = iter.second->GetID();
		chat.receiverId = targetId;

		WSPlayer* targetPlayer = WSPlayerMgr::Instance()->FindNormalPlayer(targetId);
		if (targetPlayer == NULL)
		{
			ChatDataInfo* chatData = new ChatDataInfo();
			chatData->channel = CHAT_CHANNEL_PRIVATE;
			chatData->objid = GetOwner()->GetID();
			chatData->sex = GetOwner()->GetSex();
			chatData->occu = GetOwner()->GetOccu();
			chatData->level = GetOwner()->GetLevel();
			chatData->viplvl = GetOwner()->GetVipLvl();
			chatData->objname = GetOwner()->GetName();
			chatData->word = chat.word;
			chatData->bLink = false;
			chatData->isGm = 0;
			chatData->voiceKey = "";
			chatData->voiceDuration = 0;
			chatData->receiverId = targetId;
			GetOwner()->PutOfflineEvent(targetId, new SendPrivateChatEvent(chatData));
		}
		else
		{
			targetPlayer->SendProtocol(chat);
		}

		GetOwner()->SendProtocol(chat);
	}
}

std::string WSRelationMgr::GetMasterName()
{
	WSRelation* relation = GetRelation((UInt8)RELATION_MASTER);
	if (!relation)
	{
		return "";
	}
	return relation->GetName();
}

GlobalRelationMgr::GlobalRelationMgr()
{

}

GlobalRelationMgr::~GlobalRelationMgr()
{

}

void GlobalRelationMgr::RemoveAllRelation(ObjID_t roleUid)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(roleUid);
	if (player)
	{
		player->GetRelationMgr().RemoveAllRelations();
	}
	
	class PlayerVisitor : public WSPlayerVisitor
	{
	public:
		PlayerVisitor(ObjID_t roleId) : m_roleId(roleId) {}
		~PlayerVisitor() {}
		bool operator()(WSPlayer* player)
		{
			if (NULL == player || m_roleId == player->GetID()) return true;
			WSRelation* relation = player->GetRelationMgr().GetRelation(m_roleId);
			if (relation)
			{
				player->GetRelationMgr().RemoveRelation(relation);
			}
			return true;
		}

		ObjID_t m_roleId;
	};

	PlayerVisitor visitor(roleUid);
	WSPlayerMgr::Instance()->VisitPlayers(visitor);

	std::ostringstream oss;
	oss << "`id`='" << roleUid << "' OR `owner`='" << roleUid << "'";
	CLDeleteCommand* delCommand = CLRecordClient::Instance()->PrepareDeleteCommand("t_relation", oss.str());
	CLRecordClient::Instance()->SendCommand(delCommand);
	
}

void AddonPayOrderMgr::AddOrder(AddonPayOrderData* data)
{
	if (!data)
	{
		return;
	}
	
	m_AddonPayDatas[data->uid] = data;

	std::vector<AddonPayOrderData*>& requestDatas = m_RequestDatas[data->reqId];
	requestDatas.push_back(data);

	std::vector<AddonPayOrderData*>& replyDatas = m_ReplyDatas[data->tarId];
	replyDatas.push_back(data);
}

void AddonPayOrderMgr::GetReqOrders(UInt64 reqId, std::vector<AddonPayOrderData*>& outDatas)
{
	auto itr = m_RequestDatas.find(reqId);
	if (itr != m_RequestDatas.end())
	{
		for (size_t i = 0; i < itr->second.size(); ++i)
		{
			outDatas.push_back(itr->second[i]);
		}
	}
}

void AddonPayOrderMgr::GetRepOrders(UInt64 repId, std::vector<AddonPayOrderData*>& outDatas)
{
	auto itr = m_ReplyDatas.find(repId);
	if (itr != m_ReplyDatas.end())
	{
		for (size_t i = 0; i < itr->second.size(); ++i)
		{
			outDatas.push_back(itr->second[i]);
		}
	}
}

AddonPayOrderMgr::AddonPayOrderMgr()
{

}

AddonPayOrderMgr::~AddonPayOrderMgr()
{

}

AddonPayOrderData* AddonPayOrderMgr::GetOrder(UInt64 id)
{

	auto itr = m_AddonPayDatas.find(id);
	if (itr != m_AddonPayDatas.end())
	{
		return itr->second;
	}

	return NULL;

}
