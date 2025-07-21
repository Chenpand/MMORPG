#include "WSMasterSystem.h"

#include <CLRecordClient.h>
#include <CLRelationProtocol.h>
#include <CLMasterSysGiftDataEntry.h>
#include <CLTaskProtocol.h>
#include <CLSyncObjectProtocol.h>
#include <CLNewTitleDataEntry.h>

#include "WSPlayer.h"
#include "WSPlayerMgr.h"
#include "MasterSectRelation.h"
#include "WSPlayerShareDataMgr.h"
#include "WSShareDataLoadCallback.h"
#include "WorldSysNotify.h"
#include "AnnouncementMgr.h"
#include "WSTeamMgr.h"
#include "WSTeam.h"
/*
*@brief 查询同门关系回调类
*/
class SelectMSRelationsCallback : public SelectRoleInfoCallback
{
public:
	SelectMSRelationsCallback(WSPlayer* player) :SelectRoleInfoCallback(player){}
	~SelectMSRelationsCallback(){}
	
	void OnFinished(WSPlayer* player)
	{
		player->GetMasterSystem().OnLoadDataRet(this);
	}

	void OnFailed(WSPlayer* player)
	{

	}
};

/*
*@brief 查询玩家师徒问卷
*/
class SelectPlayerQuestionnaireCallback : public SelectRoleInfoCallback
{
public:
	SelectPlayerQuestionnaireCallback(WSPlayer* player) :SelectRoleInfoCallback(player){}
	~SelectPlayerQuestionnaireCallback(){}

	void OnFinished(WSPlayer* player)
	{
		if (player)
		{
			player->GetMasterSystem().GetQuestionnaire().LoadFromDB(this);
		}
	}

	void OnFailed(WSPlayer* player)
	{

	}
};

/**
*@brief  主动解除师徒关系获取玩家离线时间判断是否惩罚
*/
class ShareDataLoadingForRelieveMasterDiscipleDeterminePunish : public WSShareDataLoadCallback
{
public:
	enum RelieveType
	{
		RELIEVE_DISCIPLE = 1, //解除徒弟
		RELIEVE_MASTER = 2, //解除师傅
	};
public:
	ShareDataLoadingForRelieveMasterDiscipleDeterminePunish(ObjID_t m_RelieverId, UInt64 timestamp, UInt8 type)
		: m_RelieverId(m_RelieverId), m_Timestamp(timestamp), m_type(type){}
	~ShareDataLoadingForRelieveMasterDiscipleDeterminePunish() {}

	/**
	*@breif 加载成功
	*/
	virtual void OnFinish(WSPlayerShareData* shareData)
	{
		InfoLogStream << "sharedata(" << shareData->GetID() << ") loading for query disciple master tasks, start to send tasks." << LogStream::eos;
		WSPlayer* reliever = WSPlayerMgr::Instance()->FindPlayer(this->m_RelieverId);

		if (reliever)
		{
			if (m_type == RELIEVE_DISCIPLE)
			{
				reliever->GetMasterSystem().SetRecruitDisciplePunishTimeOnRemoveDisciple(m_Timestamp, shareData->GetOfflineTimeSec());
				
			}
			else if (m_type == RELIEVE_MASTER)
			{
				reliever->GetMasterSystem().SetApprenticMasterPunishTimeOnRemoveMaster(m_Timestamp, shareData->GetOfflineTimeSec());
			}
			//reliever->GetMasterSystem().SyncPunishTimeToClient();
		}
		else
		{
			UInt64 punishTimeSpan = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_RELATION_NOPUNISH_OFFLINETIME) * 3600;
			UInt64 apprenticMasterPunishTime = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_RELATION_REQMASTER_PUNISHTIME) * 3600;
			UInt64 recruitDisciplePunishTime = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_RELATION_REQDISCIP_PUNISHTIME) * 3600;
			if (m_Timestamp - shareData->GetOfflineTimeSec() > punishTimeSpan)
			{
				return;
			}

			if (m_type == RELIEVE_DISCIPLE)
			{
				CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_player_info", m_RelieverId, false);

				cmd->PutData("recruitdisciple_timestamp", m_Timestamp + recruitDisciplePunishTime);
				CLRecordClient::Instance()->SendCommand(cmd);
			}
			else if (m_type == RELIEVE_MASTER)
			{
				CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_player_info", m_RelieverId, false);

				cmd->PutData("apprenticmaster_timestamp", m_Timestamp + apprenticMasterPunishTime);
				CLRecordClient::Instance()->SendCommand(cmd);
			}
		}
	}

	/**
	*@brief 加载失败
	*/
	virtual void OnFailure(WSPlayerShareData* shareData)
	{
		ErrorLogStream << "sharedata(" << shareData->GetID() << ") loading failed for RelieveMasterDiscipleDeterminePunish " << LogStream::eos;
	}

	/**
	*@brief 加载超时
	*/
	virtual void OnTimeout(WSPlayerShareData* shareData)
	{
		OnFailure(shareData);
	}

	ObjID_t		m_RelieverId; //主动解除师徒关系玩家id
	UInt64		m_Timestamp;  //解除时间戳
	UInt8       m_type;		  //解除类型 1:解除徒弟 2:解除师傅
};

/**
*@brief  自动出师获取玩家离线时间判断是否惩罚
*/
class ShareDataLoadingForAutoFinshSchool : public WSShareDataLoadCallback
{
public:
	ShareDataLoadingForAutoFinshSchool(ObjID_t masterId, ObjID_t discipleId, UInt8 type) 
		: m_masterId(masterId), m_discipleId(discipleId), m_type(type)
	{}
	~ShareDataLoadingForAutoFinshSchool()
	{}

	/**
	*@breif 加载成功
	*/
	virtual void OnFinish(WSPlayerShareData* shareData)
	{
		if (m_type == MasterSysAutoFinSchType::MSAFST_MASTER) //师傅发起
		{
			WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(m_masterId);
			if (player)
			{
				player->GetMasterSystem().OnMasterAutomaticFinishSchool(m_discipleId, shareData);
			}
		}
		else if (m_type == MasterSysAutoFinSchType::MSAFST_DISCIPLE) //徒弟发起
		{
			WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(m_discipleId);
			if (player)
			{
				player->GetMasterSystem().OnDiscipleAutomaticFinishSchool(m_masterId, shareData);
			}
		}
	}

	/**
	*@brief 加载失败
	*/
	virtual void OnFailure(WSPlayerShareData* shareData)
	{
		ErrorLogStream << "sharedata(" << shareData->GetID() << ") loading failed for AutoFinshSchool" << LogStream::eos;
	}

	/**
	*@brief 加载超时
	*/
	virtual void OnTimeout(WSPlayerShareData* shareData)
	{
		OnFailure(shareData);
	}

	ObjID_t		m_masterId;
	ObjID_t		m_discipleId;
	UInt8       m_type;		  //解除类型 1:师傅发起 2:徒弟发起
};

bool MsaterDiscipleQuestionnaire::SetDataFromClient(CLProtocol::WorldSetDiscipleQuestionnaireReq& req)
{
	bool change = false;
	if (activeTimeType != req.activeTimeType)
	{
		activeTimeType = req.activeTimeType;
		change = true;
	}
	if (masterType != req.masterType)
	{
		masterType = req.masterType;
		change = true;
	}
	if (regionId != req.regionId)
	{
		regionId = req.regionId;
		change = true;
	}
	if (declaration != req.declaration)
	{
		declaration = req.declaration;
		change = true;
	}
	return change;
}

bool MsaterDiscipleQuestionnaire::SetDataFromClient(CLProtocol::WorldSetMasterQuestionnaireReq& req)
{
	bool change = false;
	if (activeTimeType != req.activeTimeType)
	{
		activeTimeType = req.activeTimeType;
		change = true;
	}
	if (masterType != req.masterType)
	{
		masterType = req.masterType;
		change = true;
	}
	if (regionId != req.regionId)
	{
		regionId = req.regionId;
		change = true;
	}
	if (declaration != req.declaration)
	{
		declaration = req.declaration;
		change = true;
	}
	return change;
}

void MsaterDiscipleQuestionnaire::LoadFromDB(CLRecordCallback* callback)
{
	if (callback == NULL) return;
	if (callback->NextRow())
	{
		this->guid = callback->GetKey();
		this->owner = callback->GetData("owner");
		this->activeTimeType = callback->GetData("activeTimeType");
		this->masterType = callback->GetData("masterType");
		this->regionId = callback->GetData("regionId");
		this->declaration = callback->GetData("declaration").GetString();
	}
}

void MsaterDiscipleQuestionnaire::SynToClient(WSPlayer* player)
{
	CLProtocol::WorldSyncRelationQuestionnaire sync;
	sync.activeTimeType = activeTimeType;
	sync.masterType = masterType;
	sync.regionId = regionId;
	sync.declaration = declaration;
	player->SendProtocol(sync);
}

void MsaterDiscipleQuestionnaire::SynToDB(WSPlayer* player)
{
	if (guid == 0)
	{
		guid = CLRecordClient::Instance()->GenGuid();
		CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_questionnaire", guid);
		cmd->PutData("owner", player->GetID());
		cmd->PutData("activeTimeType", this->activeTimeType);
		cmd->PutData("masterType", this->masterType);
		cmd->PutData("regionId", this->regionId);
		cmd->PutData("declaration", this->declaration);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
	else
	{
		CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_questionnaire", guid, true);
		cmd->PutData("owner", player->GetID());
		cmd->PutData("activeTimeType", this->activeTimeType);
		cmd->PutData("masterType", this->masterType);
		cmd->PutData("regionId", this->regionId);
		cmd->PutData("declaration", this->declaration);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
}

WSMasterSystem::WSMasterSystem()
{
	m_pOwner = NULL;
	m_SyncDBTimer.SetInterval(10 * 60 * 1000);
	m_RecommanCoolTime = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_RELATION_RECOMMAN_COOLTIEM);
	m_ApprenticMasterPunishTime = 0;
	m_RecruitDisciplePunishTime = 0;
	m_GoodTeacherValue = 0;
	m_AcademicTotalGrowth = 0;
	m_MasterDailyTaskGrowth = 0;
	m_MasterAcademicTaskGrowth = 0;
	m_MasterGiveEquipGrowth = 0;
	m_MasterUplevelGrowth = 0;
	m_MasterGiveGiftGrowth = 0;
	m_FinSchDiscipleSum = 0;
	m_AnnounceProhibitTimeStamp = 0;
	m_IsRecvingMasterDailyTaskReward = false;
	m_RecvingMasterDailyTaskRewardTime = 0;
	m_IsRecvingDiscipleDailyTaskReward = false;
	m_RecvingDiscipleDailyTaskRewardTime = 0;
}

WSMasterSystem::~WSMasterSystem()
{
	for (int i = 0; i < MSRELATION_MAX; ++i)
	{
		for (MasterSectRelationMap::iterator iter = m_Relations[i].begin();
			iter != m_Relations[i].end(); ++iter)
		{
			CL_SAFE_DELETE(iter->second);
		}
	}
}

/*
*@brief 检查刷同门关系
*/
void WSMasterSystem::CheckAndMakeRelations()
{
	//检查是否要刷同门关系
	bool bmake = false;
	WSRelation*	master_relation = GetOwner()->GetRelationMgr().GetRelation((UInt8)RELATION_MASTER);
	//有师傅检查同门师傅关系是否已经存在
	if (master_relation)
	{
		MasterSectRelation* relation = this->GetRelation(MSRELATION_MASTER, master_relation->GetID());
		//不存在，刷同门关系
		if (relation == NULL)
		{
			bmake = true;
		}
	}
	if (!bmake)
	{
		//检查徒弟
		WSRelation* disciple_relation = GetOwner()->GetRelationMgr().GetRelation((UInt8)RELATION_DISCIPLE);
		if (disciple_relation)
		{
			MasterSectRelation* relation = this->GetRelation(RELATION_DISCIPLE, disciple_relation->GetID());
			if (relation == NULL)
			{
				bmake = true;
			}
		}
	}
	//刷同门关系
	if (bmake)
	{
		WSRelation*	master_relation = GetOwner()->GetRelationMgr().GetRelation((UInt8)RELATION_MASTER);
		//有师傅
		if (master_relation)
		{
			WSPlayer* master = WSPlayerMgr::Instance()->FindPlayer(master_relation->GetID());
			if (master)
			{
				WSMasterSystem::MakeMasterRelation(master, GetOwner());
				WSMasterSystem::MakeBrotherRelationByMasterMaked(master, GetOwner());
			}
			else
			{
				WSPlayer* player = new WSPlayer();
				player->SetOfflineReplyCtx(GetOwner(), LoadType::LT_MAKE_MASTERSECT_MASTER);
				player->SetID(master_relation->GetID());
				WSPlayerMgr::Instance()->AddOfflinePlayerCache(player);
				player->LoadData();
			}
		}
		
		WSRelation* disciple_relation = GetOwner()->GetRelationMgr().GetRelation((UInt8)RELATION_DISCIPLE);
		//有徒弟
		if (disciple_relation)
		{
			WSMasterSystem::MakeAllDiscipleRelations(GetOwner());
		}
	}
}

/**
*@brief 加载所有同门
*/
void WSMasterSystem::LoadRelations()
{
	if (GetOwner()->GetSrcZoneId() != 0) return;

	//加载同门关系
	{
		CLSelectCommand* selectCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_mastersect_relation");
		selectCmd->PutCond("owner", GetOwner()->GetID());
		CLRecordClient::Instance()->SendCommand(selectCmd, new SelectMSRelationsCallback(GetOwner()));
	}
	
	//加载师徒问卷调查
	{
		CLSelectCommand* selectCmd = CLRecordClient::Instance()->PrepareSelectCommand("t_questionnaire");
		selectCmd->PutCond("owner", GetOwner()->GetID());
		CLRecordClient::Instance()->SendCommand(selectCmd, new SelectPlayerQuestionnaireCallback(GetOwner()));
	}
	
}

/**
*@brief 加载数据返回
*/
void WSMasterSystem::OnLoadDataRet(CLRecordCallback* callback)
{
	if (callback == NULL) return;
	while (callback->NextRow())
	{
		UInt8	type = callback->GetData("type");
		if (type >= MSRELATION_MAX) continue;
		DBKey_t key = callback->GetKey();
		if (0 == key)	continue;
		UInt64	tarUid = callback->GetData("id");
		if (0 == tarUid) continue;

		MasterSectRelation* relation = new MasterSectRelation(type);
		relation->SetGuid(key);
		relation->LoadDBData(callback);
		if (!m_Relations[type].insert(std::make_pair(relation->GetID(), relation)).second)
		{
			CL_SAFE_DELETE(relation);
			continue;
		}

		relation->ClearDirty();
	}
}

/*
*#brief 加载玩家数据
*/
void WSMasterSystem::OnLoadPlayerDataRet(CLRecordCallback* callback)
{
	this->SetAcademicTotalGrowth(callback->GetData("academic_total_growth"));
	this->SetMasterDailyTaskGrowth(callback->GetData("master_dailytask_growth"));
	this->SetMasterAcademicTaskGrowth(callback->GetData("master_academictask_growth"));
	this->SetMasterGiveEquipGrowth(callback->GetData("master_giveequip_growth"));
	this->SetMasterUplevelGrowth(callback->GetData("master_uplevel_growth"));
	this->SetMasterGiveGiftGrowth(callback->GetData("master_givegift_growth"));
	this->SetGoodTeacherValue(callback->GetData("good_teacher_value"), false);
	this->SetApprenticMasterPunishTime(callback->GetData("apprenticmaster_timestamp"), false);
	this->SetRecruitDisciplePunishTime(callback->GetData("recruitdisciple_timestamp"), false);
	this->SetFinSchDiscipleSum(callback->GetData("fin_sch_disciple_sum"));
}

/**
*@brief 玩家上线
*/
void WSMasterSystem::OnOnline()
{
	//先刷就的师徒关系产生的同门关系
	CheckAndMakeRelations();

	class RelationOnlineVisitor : public WSPlayerVisitor
	{
	public:
		RelationOnlineVisitor(WSPlayer* owner) :m_pOwner(owner)
		{
			m_bUpdateLevel = m_bUpdateVipLv  = m_bUpdateOccu = false;
		}

		bool operator()(WSPlayer* player)
		{
			if (m_pOwner == player) return true;

			MasterSectRelation* relation = player->GetMasterSystem().GetRelation(m_pOwner->GetID());
			if (relation != NULL)
			{
				if (relation->GetLevel() != m_pOwner->GetLevel())
				{
					relation->SetLevel(m_pOwner->GetLevel());
					m_bUpdateLevel = true;
				}
				if (relation->GetVipLv() != m_pOwner->GetVipLvl())
				{
					relation->SetVipLv(m_pOwner->GetVipLvl());
					m_bUpdateVipLv = true;
				}
				if (relation->GetOccu() != m_pOwner->GetOccu())
				{
					relation->SetOccu(m_pOwner->GetOccu());
					m_bUpdateOccu = true;
				}

				relation->SetStatus(m_pOwner->GetMatchStatus());
			}

			if (player->GetMasterSystem().HasRelation(m_pOwner->GetID()))
			{
				WSRelation* relation = player->GetRelationMgr().GetRelation(m_pOwner->GetID());
				if (!relation || (relation->GetType() != RelationType::RELATION_MASTER
					&& relation->GetType() != RelationType::RELATION_DISCIPLE
					&& relation->GetType() != RelationType::RELATION_FRIEND))
				{
					CLProtocol::WorldSyncOnOffline sync;
					sync.id = m_pOwner->GetID();
					sync.isOnline = 1;
					player->SendProtocol(sync);
				}			
			}
			return true;
		}

		void SyncToDB()
		{
			if (m_bUpdateLevel && m_pOwner->GetLevel() > 20) WSMasterSystem::SyncLevelToDB(m_pOwner->GetID(), m_pOwner->GetLevel());
			if (m_bUpdateVipLv) WSMasterSystem::SyncVipLevelToDB(m_pOwner->GetID(), m_pOwner->GetVipLvl());
			if (m_bUpdateOccu) WSMasterSystem::SyncOccuToDB(m_pOwner->GetID(), m_pOwner->GetOccu());
		}
	private:
		WSPlayer* m_pOwner;
		bool	m_bUpdateLevel;
		bool	m_bUpdateVipLv;
		bool	m_bUpdateOccu;
	};

	RelationOnlineVisitor visitor(GetOwner());
	WSPlayerMgr::Instance()->VisitPlayers(visitor);
	visitor.SyncToDB();

	//同步玩家同门玩家数据
	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::WORLD_SYNC_MASTERSECT_RELATION_LIST);
	Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
	try
	{
		for (int i = MSRELATION_MASTER; i < MSRELATION_MAX; ++i)
		{
			for (MasterSectRelationMap::iterator iter = m_Relations[i].begin();
				iter != m_Relations[i].end(); ++iter)
			{
				MasterSectRelation* relation = iter->second;
				if (!relation)	continue;

				auto player = WSPlayerMgr::Instance()->FindNormalPlayer(relation->GetID());

				if (player)
				{
					relation->SetStatus(player->GetMatchStatus());
				}

				stream & relation->GetID() & UInt8(relation->IsOnline() ? 1 : 0);
				relation->Encode(stream, UInt32(1) << i, false);
			}
		}
		stream & ObjID_t(0);
	}
	catch (Avalon::NetException&)
	{
		ErrorLogStream << "encode master relation list error! guid=" << GetOwner()->GetID() << LogStream::eos;
		Avalon::Packet::Destroy(packet);
		return;
	}
	packet->SetSize(stream.GetPos());
	GetOwner()->SendPacket(packet);
	Avalon::Packet::Destroy(packet);

	//同步师徒问卷调查
	m_Questionnaire.SynToClient(GetOwner());
	//同步惩罚时间
	SyncPunishTimeToClient();
}

/**
*@brief 逻辑日更
*/
void WSMasterSystem::OnLogicDayChange()
{

}

/**
*@brief 玩家下线
*/
void WSMasterSystem::OnOffline()
{
	class RelationOfflineVisitor : public WSPlayerVisitor
	{
	public:
		RelationOfflineVisitor(WSPlayer* owner) :m_pOwner(owner)
		{
			m_SyncOnOffline.id = m_pOwner->GetID();
			m_SyncOnOffline.isOnline = 0;
		}

		bool operator()(WSPlayer* player)
		{
			if (m_pOwner == player) return true;

			if (player->GetMasterSystem().HasRelation(m_pOwner->GetID()))
			{
				WSRelation* relation = player->GetRelationMgr().GetRelation(m_pOwner->GetID());
				if (!relation || (relation->GetType() != RelationType::RELATION_MASTER
					&& relation->GetType() != RelationType::RELATION_DISCIPLE
					&& relation->GetType() != RelationType::RELATION_FRIEND))
				{
					player->SendProtocol(m_SyncOnOffline);
				}
			}		
			return true;
		}

	private:
		WSPlayer* m_pOwner;
		CLProtocol::WorldSyncOnOffline m_SyncOnOffline;
	};
	RelationOfflineVisitor visitor(GetOwner());
	WSPlayerMgr::Instance()->VisitNormalPlayers(visitor);

	SyncToDB();
}

/**
*@brief 等级改变
*/
void WSMasterSystem::OnLevelChanged()
{
	//等级改变通知同门
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
		if (m_PlayerId == player->GetID()) return true;
		
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
	
	SyncLevelToDB(GetOwner()->GetID(), GetOwner()->GetLevel());
}

/**
*@brief vip等级改变
*/
void WSMasterSystem::OnVipLvlChanged()
{
	//vip等级改变通知同门
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
}

/**
*@brief 职业改变
*/
void WSMasterSystem::OnOccuChanged()
{
	//职业改变通知同门
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
}

/**
*@brief 状态改变
*/
void WSMasterSystem::OnStatusChanged(UInt8 status)
{
	//状态改变通知同门
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

			MasterSectRelation* relation = player->GetMasterSystem().GetRelation(m_PlayerId);
			if (relation != NULL)
			{
				relation->SetStatus(m_Status);
			}
			UpdateRelationDebugLog debugLog("WSMasterSystem::OnStatusChanged");
			player->GetMasterSystem().UpdateRelation();
			return true;
		}

	private:
		ObjID_t	m_PlayerId;
		UInt8	m_Status;
	};

	StatusChangedVisitor visitor(GetOwner()->GetID(), status);
	WSPlayerMgr::Instance()->VisitPlayers(visitor);
}

/**
*@brief 名字改变
*/
void WSMasterSystem::OnNameChanged(ObjID_t playerId, const std::string& name)
{
	//姓名改变通知同门
	class NameChangedVisitor : public WSPlayerVisitor
	{
	public:
		NameChangedVisitor(ObjID_t id, const std::string& name)
			:m_PlayerId(id), m_Name(name)
		{
		}

		bool operator()(WSPlayer* player)
		{
			if (player->GetID() == m_PlayerId) return true;

			MasterSectRelation* relation = player->GetMasterSystem().GetRelation(m_PlayerId);
			if (relation != NULL)
			{
				relation->SetName(m_Name);
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

/**
*@brief 心跳
*/
void WSMasterSystem::OnHeartbeat(const Avalon::Time& now)
{
	
	CheckRecvRewardState(now);

	bool bSyncDB = m_SyncDBTimer.IsTimeout(now);

	class OnTickVisitor : public MasterSectRelationVisitor
	{
	public:
		OnTickVisitor(WSPlayer* owner, bool bSyncDB) :m_pOwner(owner), m_bSyncDB(bSyncDB)
		{
		}

		~OnTickVisitor()
		{
		}

		bool operator()(MasterSectRelation* relation)
		{
			if (m_bSyncDB) relation->SyncToDB();

			return true;
		}
	private:
		WSPlayer* m_pOwner;
		bool	m_bSyncDB;
	};
	OnTickVisitor visitor(GetOwner(), bSyncDB);
	VisitRelations(visitor);
}

void WSMasterSystem::CheckRecvRewardState(const Avalon::Time& now)
{
	UInt64 cursec = now.Sec();
	UInt32 timespan = 60;
	if (m_IsRecvingMasterDailyTaskReward && cursec > m_RecvingMasterDailyTaskRewardTime + timespan)
	{
		this->ResetRecvingMasterDailyTaskRewardState();
	}

	if (m_IsRecvingDiscipleDailyTaskReward && cursec > m_RecvingDiscipleDailyTaskRewardTime + timespan)
	{
		this->ResetRecvingDiscipleDailyTaskRewardState();
	}
}

/*
*@brief 更新关系
*/
void WSMasterSystem::UpdateRelation()
{
	class OnUpdateVisitor : public MasterSectRelationVisitor
	{
	public:
		OnUpdateVisitor(WSPlayer* owner) :m_pOwner(owner)
		{
			m_pPacket = Avalon::Packet::Create();
			m_pPacket->SetID(CLProtocol::WORLD_SYNC_MASTERSECT_RELATIONDATA);
		}

		~OnUpdateVisitor()
		{
			Avalon::Packet::Destroy(m_pPacket);
		}

		bool operator()(MasterSectRelation* relation)
		{
			if (relation == NULL) return true;

			//同步关系
			Avalon::NetOutputStream stream(m_pPacket->GetBuffer(), m_pPacket->GetMaxSize());
			stream & relation->GetID();
			if (relation->Encode(stream, UInt32(1) << relation->GetType(), true))
			{
				m_pPacket->SetSize(UInt16(stream.GetPos()));
				m_pOwner->SendPacket(m_pPacket);
				m_pPacket->SetSize(0);
			}
			return true;
		}
	private:
		WSPlayer* m_pOwner;
		Avalon::Packet* m_pPacket;
	};
	OnUpdateVisitor visitor(GetOwner());
	VisitRelations(visitor);
}

bool WSMasterSystem::AddRelation(MasterSectRelation* relation)
{
	if (relation == NULL || relation->GetType() >= MSRELATION_MAX || relation->GetType() < MSRELATION_MASTER)
	{
		return false;
	}

	//检查是否已经存在该同门关系
	MasterSectRelationMap::iterator iter = m_Relations[relation->GetType()].find(relation->GetID());
	if (iter != m_Relations[relation->GetType()].end()) return false;

	if (!m_Relations[relation->GetType()].insert(std::make_pair(relation->GetID(), relation)).second) return false;
	relation->SetGuid(CLRecordClient::Instance()->GenGuid());
	relation->InsertToDB(GetOwner()->GetID());

	//通知玩家新同门关系
	Avalon::Packet* packet = Avalon::Packet::Create();
	packet->SetID(CLProtocol::WORLD_NOTIFY_NEW_MASTERSECT_RELATION);
	Avalon::NetOutputStream stream(packet->GetBuffer(), packet->GetMaxSize());
	stream & relation->GetID() & UInt8(relation->IsOnline() ? 1 : 0);
	relation->Encode(stream, UInt32(1) << relation->GetType(), false);
	packet->SetSize(stream.GetPos());
	GetOwner()->SendPacket(packet);
	Avalon::Packet::Destroy(packet);

	return true;
}

/*
*@brief 查找同门关系
*/
MasterSectRelation* WSMasterSystem::GetRelation(ObjID_t id)
{
	for (int i = 1; i < MSRELATION_MAX; ++i)
	{
		auto itr = m_Relations[i].find(id);
		if (itr != m_Relations[i].end())
		{
			return itr->second;
		}
	}
	return NULL;
}

MasterSectRelation* WSMasterSystem::GetRelation(UInt8 type, ObjID_t id)
{
	if (type >= MSRELATION_MAX || type < MSRELATION_MASTER) return NULL;

	MasterSectRelationMap::iterator iter = m_Relations[type].find(id);
	if (iter != m_Relations[type].end()) return iter->second;
	return NULL;
}

bool WSMasterSystem::HasRelation(UInt8 type)
{
	return m_Relations[type].size() > 0;
}

bool  WSMasterSystem::HasRelation(ObjID_t id)
{
	for (int i = 1; i < MSRELATION_MAX; ++i)
	{
		if (m_Relations[i].find(id) != m_Relations[i].end()) return true;
	}
	return false;
}

bool WSMasterSystem::RemoveRelation(UInt8 type, ObjID_t id)
{
	if (type < MSRELATION_MASTER || type >= MSRELATION_MAX)
	{
		return false;
	}

	MasterSectRelationMap& relations = m_Relations[type];
	MasterSectRelationMap::iterator iter = relations.find(id);
	if (iter == relations.end())
	{
		return false;
	}

	MasterSectRelation* relation = iter->second;
	if (relation == NULL)
	{
		return false;
	}
	//通知玩家删除关系
	SendPlayerDelRelation((MasterSectRelationType)relation->GetType(), relation->GetID());

	relations.erase(iter);
	relation->DeleteFromDB();
	CL_SAFE_DELETE(relation);
	return true;
}

void WSMasterSystem::ClearRelations()
{
	for (int i = MSRELATION_MASTER; i < MSRELATION_MAX; ++i)
	{
		this->ClearRelations(i);
	}
}

void WSMasterSystem::ClearRelations(UInt8 type)
{
	MasterSectRelationMap& relationMap = m_Relations[type];
	for (MasterSectRelationMap::iterator itr = relationMap.begin();
		itr != relationMap.end(); ++itr)
	{
		MasterSectRelation* relation = itr->second;
		if (relation)
		{
			UInt8 tp = type;
			if (type == MSRELATION_MASTER)
			{
				tp = MSRELATION_DISCIPLE;
			}
			else if (type == MSRELATION_DISCIPLE)
			{
				tp = MSRELATION_MASTER;
			}

			//删除对方的关系
			WSPlayer* target = WSPlayerMgr::Instance()->FindPlayer(relation->GetID());
			if (target != NULL)
			{
				target->GetMasterSystem().RemoveRelation(tp, GetOwner()->GetID());
			}
			else
			{
				std::ostringstream oss;
				oss << "`id`=" << GetOwner()->GetID() << " AND `owner`=" << relation->GetID() << " AND `type`=" << (Int32)tp;
				std::string ss = oss.str();
				CLDeleteCommand* delCommand = CLRecordClient::Instance()->PrepareDeleteCommand("t_mastersect_relation", oss.str());
				CLRecordClient::Instance()->SendCommand(delCommand);
			}

			//删除自己的关系
			relation->DeleteFromDB();
			//通知玩家删除关系
			SendPlayerDelRelation((MasterSectRelationType)relation->GetType(), relation->GetID());
			CL_SAFE_DELETE(relation);
		}
	}

	relationMap.clear();
}

void WSMasterSystem::VisitRelations(MasterSectRelationVisitor& visitor)
{
	for (int i = 0; i < MSRELATION_MAX; ++i)
	{
		for (MasterSectRelationMap::iterator iter = m_Relations[i].begin();
			iter != m_Relations[i].end(); ++iter)
		{
			if (!visitor(iter->second)) return;
		}
	}
}

void WSMasterSystem::VisitRelations(UInt8 type, MasterSectRelationVisitor& visitor)
{
	if (type < MSRELATION_MASTER || type >= MSRELATION_MAX) return;

	for (MasterSectRelationMap::iterator iter = m_Relations[type].begin();
		iter != m_Relations[type].end(); ++iter)
	{
		if (!visitor(iter->second)) return;
	}
}

UInt64 WSMasterSystem::GetRecommanMasterTimeStamp(ObjID_t masterId)
{
	TimeStampMap::iterator iter = m_MasterRecommanTimestamp.find(masterId);
	if (iter != m_MasterRecommanTimestamp.end())
	{
		return iter->second;
	}
	return 0;
}

UInt64 WSMasterSystem::GetRecommanDiscipleTimeStamp(ObjID_t discipleId)
{
	TimeStampMap::iterator iter = m_DisipleRecommanTimestamp.find(discipleId);
	if (iter != m_DisipleRecommanTimestamp.end())
	{
		return iter->second;
	}
	return 0;
}

void WSMasterSystem::SetRecommanMasterTimeStamp(ObjID_t masterId, UInt64 timestamp)
{
	m_MasterRecommanTimestamp[masterId] = timestamp;
}

void WSMasterSystem::SetRecommanDiscipleTimeStamp(ObjID_t discipleId, UInt64 timestamp)
{
	m_DisipleRecommanTimestamp[discipleId] = timestamp;
}

bool WSMasterSystem::CheckRecommanMasterTimeStamp(ObjID_t masterId, UInt64 curr_timestamp)
{
	UInt64 timestamp = GetRecommanMasterTimeStamp(masterId);
	if (timestamp > 0 && (timestamp + m_RecommanCoolTime) > curr_timestamp)
	{
		return false;
	}
	return true;
}

bool WSMasterSystem::CheckRecommanDiscipleTimeStamp(ObjID_t discipleId, UInt64 curr_timestamp)
{
	UInt64 timestamp = GetRecommanDiscipleTimeStamp(discipleId);
	if (timestamp > 0 && (timestamp + m_RecommanCoolTime) > curr_timestamp)
	{
		return false;
	}
	return true;
}

UInt8 WSMasterSystem::CalcQuestionnaireMatchNum(WSPlayer* master, WSPlayer* disciple)
{
	if (!master || !disciple)
	{
		return 0;
	}
	UInt8 num = 0;
	MsaterDiscipleQuestionnaire& masterQuestion = master->GetMasterSystem().GetQuestionnaire();
	MsaterDiscipleQuestionnaire& discipleQuestion = disciple->GetMasterSystem().GetQuestionnaire();
	if (masterQuestion.activeTimeType != 0 && masterQuestion.activeTimeType == discipleQuestion.activeTimeType)
	{
		num++;
	}
	if (masterQuestion.masterType != 0 && masterQuestion.masterType == discipleQuestion.masterType)
	{
		num++;
	}
	if (masterQuestion.regionId != 0 && masterQuestion.regionId == discipleQuestion.regionId)
	{
		num++;
	}
	return num;
}

/*
*@brief 处理出师
*/
void  WSMasterSystem::HandleDiscipleFinishSchool(WSPlayer* master, WSPlayer* disciple)
{
	if (!master || !disciple)
	{
		return;
	}
	UInt32 disciple_growth = disciple->GetMasterSystem().GetAcademicTotalGrowth();
	UInt32 apprentTimeSec = 0;
	UInt32 finishTimeSec = UInt32(CURRENT_TIME.Sec());
	//出师后师徒关系变成好友关系
	WSRelation* master_oldRelation = master->GetRelationMgr().GetRelation(disciple->GetID());
	if (master_oldRelation)
	{
		apprentTimeSec = master_oldRelation->GetAddTime();
		UpdateRelationDebugLog debugLog("WSMasterSystem::HandleDiscipleFinishSchool 1");
		master->GetRelationMgr().UpdateRelation(disciple->GetID(), master_oldRelation, RELATION_FRIEND);
	}

	WSRelation* disciple_oldRelation = disciple->GetRelationMgr().GetRelation(master->GetID());
	if (disciple_oldRelation)
	{
		UpdateRelationDebugLog debugLog("WSMasterSystem::HandleDiscipleFinishSchool 2");
		disciple->GetRelationMgr().UpdateRelation(master->GetID(), disciple_oldRelation, RELATION_FRIEND);
	}

	//清空离线数据中师门任务数据
	WSPlayerShareDataMgr::Instance()->HandleRelieveMaster(disciple->GetID());
	
	//发放师傅出师礼包
	const MasterSysGiftDataEntry* masterGiftDataEntry = MasterSysGiftMapDataEntryMgr::Instance()->GetDataEntrySpecic(MasterSysGiftType::MSGT_FISNISHCL_MASTER, disciple->GetMasterSystem().GetAcademicTotalGrowth());

	if (masterGiftDataEntry)
	{
		CLProtocol::WorldNotifyGiveMasterSysGiftReqToScene notify;
		notify.masterId = master->GetID();
		notify.discipleId = disciple->GetID();
		notify.type = MasterSysGiftType::MSGT_FISNISHCL_MASTER;
		notify.giftDataId = masterGiftDataEntry->GetKey();
		master->SendToScene(notify);
		
		//成长值转良师值
		//UInt32 addGoodTeachValue = disciple->GetMasterSystem().GetAcademicTotalGrowth() * masterGiftDataEntry->rate / 100;
		//master->SyncEventToScene(SET_ADD_GOODTEACH_VALUE, addGoodTeachValue, disciple->GetID());
		
	}
	else
	{
		InfoLogStream << "HandleDiscipleFinishSchool get master gift fail " << "masterId=" << master->GetID() << "discipleId=" << disciple->GetID() << " type =" << MasterSysGiftType::MSGT_FISNISHCL_MASTER << " disciple academic growth=" << disciple->GetMasterSystem().GetAcademicTotalGrowth() << LogStream::eos;
	}
	//发放徒弟出师礼包
	const MasterSysGiftDataEntry* discipleGiftDataEntry = MasterSysGiftMapDataEntryMgr::Instance()->GetDataEntrySpecic(MasterSysGiftType::MSGT_FISNISHCL_DISCIPLE, disciple->GetMasterSystem().GetAcademicTotalGrowth());
	if (discipleGiftDataEntry)
	{
		CLProtocol::WorldNotifyGiveMasterSysGiftReqToScene notify;
		notify.discipleId = disciple->GetID();
		notify.masterId = master->GetID();
		notify.type = MasterSysGiftType::MSGT_FISNISHCL_DISCIPLE;
		notify.giftDataId = discipleGiftDataEntry->GetKey();
		disciple->SendToScene(notify);
	}
	else
	{
		InfoLogStream << "HandleDiscipleFinishSchool get disciple gift fail" << "masterId=" << master->GetID() << "discipleId=" << disciple->GetID() << " type =" << MasterSysGiftType::MSGT_FISNISHCL_DISCIPLE << " disciple academic growth=" << disciple->GetMasterSystem().GetAcademicTotalGrowth() << LogStream::eos;
	}
	//增加出师徒弟数量
	master->GetMasterSystem().AddFinSchDiscipleSum(1);
	//事件通知
	disciple->SyncEventToScene(SET_RELATION_DISCIPLE_FINISHSCHOOL, master->GetID(), master->GetMasterSystem().GetFinSchDiscipleSum());
	//修改同门数据
	disciple->GetMasterSystem().UpdateMasterSectRelationOnSelfFinSch(master->GetID());
	//出师公告
	WSMasterSystem::SendFinishSchoolAnnounce(master->GetName(), disciple->GetName());
	//解散队伍
	WSTeam* team = master->GetTeam();
	if (team)
	{
		if (team->Dismiss(master))
		{
			WSTeamMgr::Instance()->RemoveTeam(team);
			CL_SAFE_DELETE(team);
		}
	}
	//广播出师事件
	WSMasterSystem::BroadcastFinschEvent(master->GetName(), disciple->GetName());

	//移除头衔
	disciple->GetTitleMgr().OnRemoveMaster();

	master->SendFinishSchoolUdpLog(MasterSysAutoFinSchType::MSAFST_NORMAL, disciple->GetID(), master->GetID(), 
		disciple->GetLevel(), master->GetLevel(), disciple_growth, apprentTimeSec, finishTimeSec);
}

void WSMasterSystem::HandlePublishMasterTaskSuccess(ObjID_t masterId, ObjID_t discipleId)
{
	//更新关系中的师门日常任务状态
	WSPlayer* disciple = WSPlayerMgr::Instance()->FindPlayer(discipleId);
	if (disciple)
	{
		WSRelation* relation = disciple->GetRelationMgr().FindRelation(RELATION_MASTER, masterId);
		if (relation)
		{
			relation->SetDailyMasterTaskState(MDTST_UNCOMPLETE);
			relation->SyncToDB();
			UpdateRelationDebugLog debugLog("WSMasterSystem::HandlePublishMasterTaskSuccess 1");
			disciple->GetRelationMgr().UpdateRelation();
			InfoLogStream << "HandlePublishMasterTaskSuccess masterId(" << masterId << "), " << "discipleId(" << discipleId << ") disciple MasterTaskState=" << relation->GetDailyMasterTaskState() << LogStream::eos;
		}
		else
		{
			WSMasterSystem::SyncDiscipleDailyTaskStateToDB(masterId, discipleId, MDTST_UNCOMPLETE);
			InfoLogStream << "HandlePublishMasterTaskSuccess11 masterId(" << masterId << "), " << "discipleId(" << discipleId << ") modify db" << LogStream::eos;
		}
	}
	else
	{
		WSMasterSystem::SyncDiscipleDailyTaskStateToDB(masterId, discipleId, MDTST_UNCOMPLETE);
		InfoLogStream << "HandlePublishMasterTaskSuccess masterId(" << masterId << "), " << "discipleId(" << discipleId << ") modify db" << LogStream::eos;
	}

	WSPlayer* master = WSPlayerMgr::Instance()->FindPlayer(masterId);
	if (master)
	{
		WSRelation* relation = master->GetRelationMgr().FindRelation(RELATION_DISCIPLE, discipleId);
		if (relation)
		{
			relation->SetDailyMasterTaskState(MDTST_UNCOMPLETE);
			relation->SyncToDB();
			UpdateRelationDebugLog debugLog("WSMasterSystem::HandlePublishMasterTaskSuccess 2");
			master->GetRelationMgr().UpdateRelation();
		}
		//udp log
		master->SendMasterPublishDailyTaskLog(discipleId, masterId, UInt32(CURRENT_TIME.Sec()));
		
		CLProtocol::SceneSyncChat protocol;
		protocol.channel = CHAT_CHANNEL_PRIVATE;
		protocol.objid = master->GetID();
		protocol.occu = master->GetOccu();
		protocol.sex = master->GetSex();
		protocol.level = master->GetLevel();
		protocol.viplvl = master->GetVipLvl();
		protocol.objname = master->GetName();
		protocol.bLink = false;
		protocol.isGm = 0;
		protocol.voiceKey = "";
		protocol.voiceDuration = 0;
		protocol.headFrame = master->GetHeadFrame();
		protocol.word = SysNotifyMgr::Instance()->MakeString(10071);
		protocol.receiverId = discipleId;
		protocol.mask = ChatMask::CHAT_MASK_SYS;

		WSPlayer* disciple_ = WSPlayerMgr::Instance()->FindNormalPlayer(discipleId);
		if (disciple_)
		{
			disciple_->SendProtocol(protocol);
		}
		else
		{
			ChatDataInfo* chatData = new ChatDataInfo();
			chatData->channel = CHAT_CHANNEL_PRIVATE;
			chatData->objid = master->GetID();
			chatData->sex = master->GetSex();
			chatData->occu = master->GetOccu();
			chatData->level = master->GetLevel();
			chatData->viplvl = master->GetVipLvl();
			chatData->objname = master->GetName();
			chatData->word = protocol.word;
			chatData->bLink = false;
			chatData->isGm = 0;
			chatData->voiceKey = "";
			chatData->voiceDuration = 0;
			chatData->receiverId = discipleId;
			master->PutOfflineEvent(discipleId, new SendPrivateChatEvent(chatData));
		}
	}
	else
	{
		WSMasterSystem::SyncMasterDailyTaskStateToDB(masterId, discipleId, MDTST_UNCOMPLETE);
	}
}

void WSMasterSystem::SetApprenticMasterPunishTime(UInt64 timestamp, bool saveDb)
{
	m_ApprenticMasterPunishTime = timestamp;

	if (saveDb)
	{
		CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_player_info", GetOwner()->GetID(), false);
		cmd->PutData("apprenticmaster_timestamp", m_ApprenticMasterPunishTime);
		CLRecordClient::Instance()->SendCommand(cmd);
	}	
}

bool WSMasterSystem::CheckApprenticMasterPunishTime()
{
	if (m_ApprenticMasterPunishTime != 0 && m_ApprenticMasterPunishTime > Avalon::Time::CurrentTime().Sec())
	{
		return false;
	}
	return true;
}

void WSMasterSystem::SetRecruitDisciplePunishTime(UInt64 timestamp, bool saveDb)
{
	m_RecruitDisciplePunishTime = timestamp;
	
	if (saveDb)
	{
		CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_player_info", GetOwner()->GetID(), false);
		cmd->PutData("recruitdisciple_timestamp", m_RecruitDisciplePunishTime);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
}

bool WSMasterSystem::CheckRecruitDisciplePunishTime()
{
	if (m_RecruitDisciplePunishTime != 0 && m_RecruitDisciplePunishTime > Avalon::Time::CurrentTime().Sec())
	{
		return false;
	}
	return true;
}

void WSMasterSystem::SetGoodTeacherValue(UInt32 value, bool saveDb)
{
	m_GoodTeacherValue = value;
	
	if (saveDb)
	{
		CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_player_info", GetOwner()->GetID(), false);
		cmd->PutData("good_teacher_value", m_GoodTeacherValue);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
}

void WSMasterSystem::SetApprenticMasterPunishTimeOnRemoveMaster(UInt64 currTime, UInt64 masterOfflineTime)
{
	UInt64 apprenticMasterPunishTime = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_RELATION_REQMASTER_PUNISHTIME) * 3600;
	bool sync = false;
	if (masterOfflineTime == 0) //在线
	{
		this->SetApprenticMasterPunishTime(currTime + apprenticMasterPunishTime);
		sync = true;
	}
	else //离线
	{
		UInt64 punishTimeSpan = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_RELATION_NOPUNISH_OFFLINETIME) * 3600;
		if (currTime - masterOfflineTime < punishTimeSpan)
		{
			this->SetApprenticMasterPunishTime(currTime + apprenticMasterPunishTime);
			sync = true;
		}
	}
	if (sync)
	{
		this->SyncPunishTimeToClient();
	}
}

void WSMasterSystem::SetRecruitDisciplePunishTimeOnRemoveDisciple(UInt64 currTime, UInt64 discipleOfflineTime)
{
	UInt64 recruitDisciplePunishTime = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_RELATION_REQDISCIP_PUNISHTIME) * 3600;
	bool sync = false;
	if (discipleOfflineTime == 0) //在线
	{
		this->SetRecruitDisciplePunishTime(currTime + recruitDisciplePunishTime);
		sync = true;
	}
	else //离线
	{
		UInt64 punishTimeSpan = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_RELATION_NOPUNISH_OFFLINETIME) * 3600;
		if (currTime - discipleOfflineTime < punishTimeSpan)
		{
			this->SetRecruitDisciplePunishTime(currTime + recruitDisciplePunishTime);
			sync = true;
		}
	}
	if (sync)
	{
		this->SyncPunishTimeToClient();
	}
}

void WSMasterSystem::AddFinSchDiscipleSum(UInt32 addValue)
{
	m_FinSchDiscipleSum += addValue;

	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_player_info", GetOwner()->GetID(), false);
	cmd->PutData("fin_sch_disciple_sum", m_FinSchDiscipleSum);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void WSMasterSystem::SyncPunishTimeToClient()
{
	CLProtocol::WorldSyncMasterDisciplePunishTime protocol;
	protocol.apprenticMasterPunishTime = m_ApprenticMasterPunishTime;
	protocol.recruitDisciplePunishTime = m_RecruitDisciplePunishTime;
	GetOwner()->SendProtocol(protocol);
}

/*
*@brief 处理新的师徒关系成立
*/
void WSMasterSystem::OnMakeMasterDisciple(WSPlayer* master, WSPlayer* disciple)
{
	if (!disciple || !master)
	{
		return;
	}
	//师傅添加新的同门关系
	MasterSectRelation* masterOldRelation = master->GetMasterSystem().GetRelation(MSRELATION_DISCIPLE, disciple->GetID());
	if (masterOldRelation == NULL)
	{
		//创建关系
		MasterSectRelation* newRelation = new MasterSectRelation(MSRELATION_DISCIPLE, disciple);
		master->GetMasterSystem().AddRelation(newRelation);
	}

	//徒弟添加新的同门关系
	MasterSectRelation* discipleOldRelation = disciple->GetMasterSystem().GetRelation(MSRELATION_MASTER, master->GetID());
	if (discipleOldRelation == NULL)
	{
		//创建关系
		MasterSectRelation* newRelation = new MasterSectRelation(MSRELATION_MASTER, master);
		disciple->GetMasterSystem().AddRelation(newRelation);
	}

	WSMasterSystem::MakeBrotherRelationByMasterMaked(master, disciple);

	//添加师徒头衔
	disciple->GetTitleMgr().OnMakeMaster();
	return;
}

/*
*@brief 处理移除徒弟
*/
void WSMasterSystem::OnRemoveDisciple(WSPlayer* master, ObjID_t discipleId)
{
	if (!master)
	{
		return;
	}
	WSPlayer* disciple = WSPlayerMgr::Instance()->FindPlayer(discipleId);
	if (disciple)
	{
		WSMasterSystem::ClearRelationsOnRemoveDisciple(disciple);
	}
	else
	{
		WSPlayer* player = new WSPlayer();
		player->SetOfflineReplyCtx(master, LoadType::LT_REMOVE_MASTERSECT_DISCIPLE);
		player->SetID(discipleId);
		WSPlayerMgr::Instance()->AddOfflinePlayerCache(player);
		player->LoadData();
	}

	//更新收徒惩罚时间
	UInt64 currTime = Avalon::Time::CurrentTime().Sec();
	if (disciple && disciple->GetGameStatus() == PS_NORMAL) //在线
	{
		master->GetMasterSystem().SetRecruitDisciplePunishTimeOnRemoveDisciple(currTime, 0);
	}
	else //离线
	{
		WSPlayerShareData* shareData = WSPlayerShareDataMgr::Instance()->FindPlayer(discipleId);
		if (shareData == NULL)
		{
			WSPlayerShareDataMgr::Instance()->LoadOfflinePlayerCb(discipleId, 0, 
				new ShareDataLoadingForRelieveMasterDiscipleDeterminePunish(master->GetID(), currTime, ShareDataLoadingForRelieveMasterDiscipleDeterminePunish::RelieveType::RELIEVE_DISCIPLE));
		}
		else if (shareData && shareData->IsWorldDataLoaded())
		{
			master->GetMasterSystem().SetRecruitDisciplePunishTimeOnRemoveDisciple(currTime, shareData->GetOfflineTimeSec());
		}
		else if (shareData->IsInLoading())
		{
			shareData->AddLoadingCallback(new ShareDataLoadingForRelieveMasterDiscipleDeterminePunish(master->GetID(), currTime, ShareDataLoadingForRelieveMasterDiscipleDeterminePunish::RelieveType::RELIEVE_DISCIPLE));
		}
		else if (shareData->GetStatus() == PSDS_INIT)
		{
			shareData->StartLoadData(new ShareDataLoadingForRelieveMasterDiscipleDeterminePunish(master->GetID(), currTime, ShareDataLoadingForRelieveMasterDiscipleDeterminePunish::RelieveType::RELIEVE_DISCIPLE));
		}
	}
}

/*
*@brief 处理移除师傅
*/
void WSMasterSystem::OnRemoveMaster(WSPlayer* disciple, ObjID_t masterId)
{
	if (!disciple)
	{
		return;
	}
	
	WSMasterSystem::ClearRelationsOnRemoveDisciple(disciple);

	//更新拜师惩罚时间
	UInt64 currTime = Avalon::Time::CurrentTime().Sec();
	WSPlayer* master = WSPlayerMgr::Instance()->FindNormalPlayer(masterId);
	if (master) //在线
	{
		disciple->GetMasterSystem().SetApprenticMasterPunishTimeOnRemoveMaster(currTime, 0);
	}
	else //离线
	{
		
		WSPlayerShareData* shareData = WSPlayerShareDataMgr::Instance()->FindPlayer(masterId);
		if (shareData == NULL)
		{
			WSPlayerShareDataMgr::Instance()->LoadOfflinePlayerCb(masterId, 0, 
				new ShareDataLoadingForRelieveMasterDiscipleDeterminePunish(disciple->GetID(), currTime, ShareDataLoadingForRelieveMasterDiscipleDeterminePunish::RelieveType::RELIEVE_MASTER));
		}
		else if (shareData && shareData->IsWorldDataLoaded())
		{
			disciple->GetMasterSystem().SetApprenticMasterPunishTimeOnRemoveMaster(currTime, shareData->GetOfflineTimeSec());
		}
		else if (shareData->IsInLoading())
		{
			shareData->AddLoadingCallback(new ShareDataLoadingForRelieveMasterDiscipleDeterminePunish(disciple->GetID(), currTime, ShareDataLoadingForRelieveMasterDiscipleDeterminePunish::RelieveType::RELIEVE_MASTER));
		}
		else if (shareData->GetStatus() == PSDS_INIT)
		{
			shareData->StartLoadData(new ShareDataLoadingForRelieveMasterDiscipleDeterminePunish(disciple->GetID(), currTime, ShareDataLoadingForRelieveMasterDiscipleDeterminePunish::RelieveType::RELIEVE_MASTER));
		}
	}

	//移除师徒头衔
	disciple->GetTitleMgr().OnRemoveMaster();
}

/*
*@brief 徒弟移除后的解除关系
*/
void WSMasterSystem::ClearRelationsOnRemoveDisciple(WSPlayer* disciple)
{
	if (!disciple)
	{
		return;
	}
	disciple->GetMasterSystem().ClearRelations(MSRELATION_MASTER);
	disciple->GetMasterSystem().ClearRelations(MSRELATION_BROTHER);

	//移除师徒头衔
	disciple->GetTitleMgr().OnRemoveMaster();
}

/*
*@brief 处理拜师成功后添加新的师兄同门关系
*/
void WSMasterSystem::MakeBrotherRelationByMasterMaked(WSPlayer* master, WSPlayer* disciple)
{
	if (!master || !disciple)
	{
		return;
	}

	//师傅有同门徒弟数据了
	if (master->GetMasterSystem().HasRelation((UInt8)MSRELATION_DISCIPLE))
	{
		//添加新的师兄弟同门关系
		class DiscipleVisitor : public MasterSectRelationVisitor
		{
		public:
			DiscipleVisitor(WSPlayer* player) : m_newDisciple(player)
			{}

			virtual bool operator()(MasterSectRelation* relation)
			{
				if (relation == NULL || relation->GetID() == m_newDisciple->GetID()) return true;

				WSPlayer* disciple = WSPlayerMgr::Instance()->FindPlayer(relation->GetID());
				if (disciple)
				{
					WSMasterSystem::MakeBrotherRelation(disciple, m_newDisciple);
				}
				else
				{
					WSPlayer* player = new WSPlayer();
					player->SetOfflineReplyCtx(m_newDisciple, LoadType::LT_MAKE_MASTERSECT_BROTHER);
					player->SetID(relation->GetID());
					WSPlayerMgr::Instance()->AddOfflinePlayerCache(player);
					player->LoadData();
				}
				return true;
			}
		private:
			WSPlayer* m_newDisciple; //新的徒弟
		};
		//遍历师傅的徒弟
		DiscipleVisitor visitor(disciple);
		master->GetMasterSystem().VisitRelations(MSRELATION_DISCIPLE, visitor);
	}
	else if (master->GetRelationMgr().HasRelation((UInt8)RELATION_DISCIPLE))
	{
		//师傅数据可能是离线加载的，可能存在旧的师徒关系，同门数据还没生成
		//添加新的师兄弟同门关系
		class DiscipleVisitor : public RelationVisitor
		{
		public:
			DiscipleVisitor(WSPlayer* player) : m_newDisciple(player)
			{}

			virtual bool operator()(WSRelation* relation)
			{
				if (relation == NULL || relation->GetID() == m_newDisciple->GetID()) return true;

				WSPlayer* disciple = WSPlayerMgr::Instance()->FindPlayer(relation->GetID());
				if (disciple)
				{
					WSMasterSystem::MakeBrotherRelation(disciple, m_newDisciple);
				}
				else
				{
					WSPlayer* player = new WSPlayer();
					player->SetOfflineReplyCtx(m_newDisciple, LoadType::LT_MAKE_MASTERSECT_BROTHER);
					player->SetID(relation->GetID());
					WSPlayerMgr::Instance()->AddOfflinePlayerCache(player);
					player->LoadData();
				}
				return true;
			}
		private:
			WSPlayer* m_newDisciple; //新的徒弟
		};
		//遍历师傅的徒弟
		DiscipleVisitor visitor(disciple);
		master->GetRelationMgr().VisitRelations(RELATION_DISCIPLE, visitor);
	}
	
}

/*
*@brief 师傅数据离线加载后添加师傅,师兄关系
*/
void WSMasterSystem::MakeMasterAndBrotherRelationByMasterDataLoaded(WSPlayer* master, WSPlayer* disciple)
{
	if (!master || !disciple)
	{
		return;
	}
	WSMasterSystem::MakeMasterRelation(master, disciple);
	WSMasterSystem::MakeBrotherRelationByMasterMaked(master, disciple);
	return;
}

/*
*@brief 建立所有徒弟的同门关系
*/
void  WSMasterSystem::MakeAllDiscipleRelations(WSPlayer* master)
{
	class DiscipleVisitor : public RelationVisitor
	{
	public:
		DiscipleVisitor(WSPlayer* player) : m_master(player)
		{}

		virtual bool operator()(WSRelation* relation)
		{
			if (relation == NULL) return true;

			WSPlayer* disciple = WSPlayerMgr::Instance()->FindPlayer(relation->GetID());
			if (disciple)
			{
				WSMasterSystem::MakeDiscipleRelation(m_master, disciple);
			}
			else
			{
				WSPlayer* player = new WSPlayer();
				player->SetOfflineReplyCtx(m_master, LoadType::LT_MAKE_MASTERSECT_DISCIPLE);
				player->SetID(relation->GetID());
				WSPlayerMgr::Instance()->AddOfflinePlayerCache(player);
				player->LoadData();
			}
			return true;
		}
	private:
		WSPlayer* m_master; //师傅
	};
	//遍历师傅的徒弟
	DiscipleVisitor visitor(master);
	master->GetRelationMgr().VisitRelations(RELATION_DISCIPLE, visitor);
}

/*
*@brief 建立师兄关系
*/
void WSMasterSystem::MakeBrotherRelation(WSPlayer* older, WSPlayer* younger)
{
	//创建关系
	if (older->GetMasterSystem().GetRelation(MSRELATION_BROTHER, younger->GetID()) == NULL)
	{
		MasterSectRelation* newRelation1 = new MasterSectRelation(MSRELATION_BROTHER, younger);
		//检查older是否已经出师
		if (!older->GetRelationMgr().HasRelation((UInt8)RelationType::RELATION_MASTER))
		{
			newRelation1->SetFinSchMark(true);
		}
		older->GetMasterSystem().AddRelation(newRelation1);
	}

	if (younger->GetMasterSystem().GetRelation(MSRELATION_BROTHER, older->GetID()) == NULL)
	{
		MasterSectRelation* newRelation2 = new MasterSectRelation(MSRELATION_BROTHER, older);
		//检查older是否已经出师
		if (!older->GetRelationMgr().HasRelation((UInt8)RelationType::RELATION_MASTER))
		{
			newRelation2->SetFinSchMark(true);
		}
		younger->GetMasterSystem().AddRelation(newRelation2);
	}
}

/*
*@brief 建立师傅同门关系
*/
void WSMasterSystem::MakeMasterRelation(WSPlayer* master, WSPlayer* disciple)
{
	if (disciple->GetMasterSystem().GetRelation(MSRELATION_MASTER, master->GetID()) == NULL)
	{
		MasterSectRelation* newRelation = new MasterSectRelation(MSRELATION_MASTER, master);
		disciple->GetMasterSystem().AddRelation(newRelation);
	}
}

/*
*@brief 建立徒弟同门关系
*/
void WSMasterSystem::MakeDiscipleRelation(WSPlayer* master, WSPlayer* disciple)
{
	if (master->GetMasterSystem().GetRelation(MSRELATION_DISCIPLE, disciple->GetID()) == NULL)
	{
		MasterSectRelation* newRelation = new MasterSectRelation(MSRELATION_DISCIPLE, disciple);
		master->GetMasterSystem().AddRelation(newRelation);
	}
}

/**
*@brief 处理领取师门日常任务奖励
*/
UInt32 WSMasterSystem::OnReceiveMasterDailyTaskReward(UInt8 type, ObjID_t discipeleId)
{
	InfoLogStream << PLAYERINFO(this->GetOwner()) << "type : " << type << ", discipeleId : " << discipeleId << LogStream::eos;
	if (type == MSRDTR_MASTER) //师傅领取
	{
		if (this->m_IsRecvingMasterDailyTaskReward)
		{
			InfoLogStream << PLAYERINFO(this->GetOwner()) << "return, m_IsRecvingMasterDailyTaskReward : " << m_IsRecvingMasterDailyTaskReward << LogStream::eos;
			return ErrorCode::MASTERSYS_RECEIEVEGIFT_FAIL;
		}

		WSRelation* relation = GetOwner()->GetRelationMgr().FindRelation(RELATION_DISCIPLE, discipeleId);
		if (relation == NULL) return ErrorCode::MASTERSYS_NOTFIND_DISICIPLE;
		if (relation->GetDailyMasterTaskState() != MasterDailyTaskState::MDTST_UNRECEIVED
			&& relation->GetDailyMasterTaskState() != MasterDailyTaskState::MDTST_UNPUB_UNRECE)
		{
			InfoLogStream << PLAYERINFO(this->GetOwner()) << ", state is err : " << relation->GetDailyMasterTaskState() << LogStream::eos;
			return ErrorCode::RECEIEVE_MASTER_DAILYTASK_REWARD_STATEFIAL;
		}
		MasterSysGiftDataEntry* entryData = MasterSysGiftMapDataEntryMgr::Instance()->GetMasterDailyTaskGift();
		if (entryData == NULL)
		{
			InfoLogStream << PLAYERINFO(this->GetOwner()) << "MasterSysGiftMapDataEntryMgr GetMasterDailyTaskGift is null" << LogStream::eos;
			return ErrorCode::ITEM_DATA_INVALID;
		}

		this->SetRecvingMasterDailyTaskRewardState(Avalon::Time::CurrentTime().Sec());

		CLProtocol::WorldNotifyGiveMasterSysGiftReqToScene notify;
		notify.masterId = GetOwner()->GetID();
		notify.type = MSGT_MASTER_DAILYTASK;
		notify.giftDataId = entryData->GetKey();
		notify.discipleId = discipeleId;
		GetOwner()->SendToScene(notify);
	}
	else if (type == MARDTR_DISCIPLE) //徒弟领取
	{
		if (this->m_IsRecvingDiscipleDailyTaskReward)
		{
			InfoLogStream << PLAYERINFO(this->GetOwner()) << "return, m_IsRecvingMasterDailyTaskReward : " << m_IsRecvingMasterDailyTaskReward << LogStream::eos;
			return ErrorCode::MASTERSYS_RECEIEVEGIFT_FAIL;
		}

		WSRelation* relation = GetOwner()->GetRelationMgr().GetRelation((UInt8)RELATION_MASTER);
		if (relation == NULL) return ErrorCode::MASTERSYS_NOTFIND_MASTER;
		if (relation->GetDailyMasterTaskState() != MDTST_UNRECEIVED)
		{
			InfoLogStream << PLAYERINFO(this->GetOwner()) << ", state is err : " << relation->GetDailyMasterTaskState() << LogStream::eos;
			return ErrorCode::RECEIEVE_MASTER_DAILYTASK_REWARD_STATEFIAL;
		}
		MasterSysGiftDataEntry* entryData = MasterSysGiftMapDataEntryMgr::Instance()->GetDiscipleDailyTaskGift();
		if (entryData == NULL)
		{
			InfoLogStream << PLAYERINFO(this->GetOwner()) << "MasterSysGiftMapDataEntryMgr GetDiscipleDailyTaskGift is null" << LogStream::eos;
			return ErrorCode::ITEM_DATA_INVALID;
		}

		this->SetRecvingDiscipleDailyTaskRewardState(Avalon::Time::CurrentTime().Sec());

		CLProtocol::WorldNotifyGiveMasterSysGiftReqToScene notify;
		notify.discipleId = GetOwner()->GetID();
		notify.type = MSGT_DISCIPLE_DAILYTASK;
		notify.giftDataId = entryData->GetKey();
		GetOwner()->SendToScene(notify);
	}
	return ErrorCode::SUCCESS;
}

UInt32 WSMasterSystem::OnReceiveMasterAcademicReward(UInt32 giftDataId)
{
	if (!this->GetOwner()->GetRelationMgr().GetRelation((UInt8)RELATION_MASTER))
	{
		return ErrorCode::MASTERSYS_RECEIEVEGIFT_FAIL;
	}

	MasterSysGiftDataEntry* entryData = MasterSysGiftMapDataEntryMgr::Instance()->FindEntry(giftDataId);
	if (entryData == NULL) return ErrorCode::ITEM_DATA_INVALID;

	CLProtocol::WorldNotifyGiveMasterSysGiftReqToScene notify;
	notify.discipleId = GetOwner()->GetID();
	notify.type = MSGT_DISCIPLE_ACADEMIC;
	notify.giftDataId = entryData->GetKey();
	GetOwner()->SendToScene(notify);
	return ErrorCode::SUCCESS;
}

void WSMasterSystem::OnAutomaticFinishSchool(ObjID_t targetId)
{
	CLProtocol::WorldAutomaticFinishSchoolRes res;

	WSRelation* relation = this->GetOwner()->GetRelationMgr().GetRelation(targetId);
	if (!relation || (relation->GetType() != RELATION_MASTER && relation->GetType() != RELATION_DISCIPLE))
	{
		res.code = ErrorCode::MASTERSYS_FINSCHFAIL_DISSATISFY_CONDITION;
		GetOwner()->SendProtocol(res);
		return;
	}
	
	WSPlayer* target = WSPlayerMgr::Instance()->FindNormalPlayer(targetId);
	if (target)
	{
		res.code = ErrorCode::MASTERSYS_AUTOFINSCHFAIL_TATGETONLINE;
		GetOwner()->SendProtocol(res);
		return;
	}
	
	ObjID_t masterId = 0;
	ObjID_t discipleId = 0;

	UInt8 autoFinshType = 0;
	if (relation->GetType() == RELATION_MASTER)
	{
		if (GetOwner()->GetLevel() < SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_RELATION_FINISHLEVEL))
		{
			res.code = ErrorCode::FISNISH_SCHOOL_DISCIPLE_LEVELINSUFFIC;
			GetOwner()->SendProtocol(res);
			return;
		}
		autoFinshType = MasterSysAutoFinSchType::MSAFST_DISCIPLE;
		masterId = targetId;
		discipleId = GetOwner()->GetID();
	}
	else if (relation->GetType() == RELATION_DISCIPLE)
	{
		autoFinshType = MasterSysAutoFinSchType::MSAFST_MASTER;
		masterId = GetOwner()->GetID();
		discipleId = targetId;
	}

	WSPlayerShareData* shareData = WSPlayerShareDataMgr::Instance()->FindPlayer(targetId);

	if (shareData == NULL)
	{
		WSPlayerShareDataMgr::Instance()->LoadOfflinePlayerCb(targetId, 0,
			new ShareDataLoadingForAutoFinshSchool(masterId, discipleId, autoFinshType));
	}
	else if (shareData && shareData->IsWorldDataLoaded())
	{
		if (autoFinshType == MasterSysAutoFinSchType::MSAFST_DISCIPLE)
		{
		    this->OnDiscipleAutomaticFinishSchool(targetId, shareData);
		}
		else if (autoFinshType == MasterSysAutoFinSchType::MSAFST_MASTER)
		{
			this->OnMasterAutomaticFinishSchool(targetId, shareData);
		}
	}
	else if (shareData->IsInLoading())
	{
		shareData->AddLoadingCallback(new ShareDataLoadingForAutoFinshSchool(masterId, discipleId, autoFinshType));
	}
	else if (shareData->GetStatus() == PSDS_INIT)
	{
		shareData->StartLoadData(new ShareDataLoadingForAutoFinshSchool(masterId, discipleId, autoFinshType));
	}
}

/**
*@brief 师傅自动出师
*/
void WSMasterSystem::OnMasterAutomaticFinishSchool(ObjID_t discipleId, WSPlayerShareData* discipleShareData)
{

	CLProtocol::WorldAutomaticFinishSchoolRes res;

	if (discipleShareData == NULL)
	{
		res.code = ErrorCode::ITEM_DATA_INVALID;
		GetOwner()->SendProtocol(res);
		return;
	}

	WSPlayer* disciple = WSPlayerMgr::Instance()->FindNormalPlayer(discipleId);
	if (disciple)
	{
		res.code = ErrorCode::MASTERSYS_AUTOFINSCHFAIL_TATGETONLINE;
		GetOwner()->SendProtocol(res);
		return;
	}

	if (discipleShareData->GetLevel() < SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_RELATION_FINISHLEVEL))
	{
		res.code = ErrorCode::FISNISH_SCHOOL_DISCIPLE_LEVELINSUFFIC;
		GetOwner()->SendProtocol(res);
		return;
	}

	UInt64 needOfflineTime = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_MASTERSYS_AUTOFINISHSCH_OFFLINETIEM) * 60;
	if (needOfflineTime == 0) needOfflineTime = 1 * 24 * 3600;

	disciple = WSPlayerMgr::Instance()->FindPlayer(discipleId);
	if (disciple)
	{
		UInt64 offlineTime = disciple->GetOfflineTimeSec();
		UInt64 currTime = Avalon::Time::CurrentTime().Sec();
		if (currTime - offlineTime < needOfflineTime)
		{
			res.code = ErrorCode::MASTERSYS_AUTOFINSCHFAIL_TATGETOFFLINETIME;
			GetOwner()->SendProtocol(res);
			return;
		}
	}

	
	UInt64 offlineTime = discipleShareData->GetOfflineTimeSec();
		
	UInt32 academicTotalGrowth = discipleShareData->GetDiscipleGrowthValue();

	UInt64 currTime = Avalon::Time::CurrentTime().Sec();
	if (currTime - offlineTime < needOfflineTime)
	{
		res.code = ErrorCode::MASTERSYS_AUTOFINSCHFAIL_TATGETOFFLINETIME;
		GetOwner()->SendProtocol(res);
		return;
	}

	UInt32 discipleLevel = 0;
	UInt32 apprentTimeSec = 0;
	//修改关系
	//出师后师徒关系变成好友关系
	WSRelation* master_oldRelation = this->GetOwner()->GetRelationMgr().GetRelation(discipleId);
	if (master_oldRelation)
	{
		apprentTimeSec = master_oldRelation->GetAddTime();
		discipleLevel = master_oldRelation->GetLevel();
		UpdateRelationDebugLog debugLog("WSMasterSystem::OnMasterAutomaticFinishSchool 1");
		this->GetOwner()->GetRelationMgr().UpdateRelation(discipleId, master_oldRelation, RELATION_FRIEND);
	}

	if (disciple)
	{
		WSRelation* disciple_oldRelation = disciple->GetRelationMgr().GetRelation(GetOwner()->GetID());
		if (disciple_oldRelation)
		{
			UpdateRelationDebugLog debugLog("WSMasterSystem::OnMasterAutomaticFinishSchool 2");
			disciple->GetRelationMgr().UpdateRelation(GetOwner()->GetID(), disciple_oldRelation, RELATION_FRIEND);
		}
	}
	else
	{
		WSRelationMgr::UpdateRelationTypeToDb(discipleId, RELATION_MASTER, GetOwner()->GetID(), RELATION_FRIEND);
	}
	


	//发放师傅出师奖励
	const MasterSysGiftDataEntry* masterGiftDataEntry = MasterSysGiftMapDataEntryMgr::Instance()->GetDataEntrySpecic(MasterSysGiftType::MSGT_FISNISHCL_MASTER, academicTotalGrowth);

	if (masterGiftDataEntry)
	{
		CLProtocol::WorldNotifyGiveMasterSysGiftReqToScene notify;
		notify.masterId = GetOwner()->GetID();
		notify.discipleId = discipleId;
		notify.type = MasterSysGiftType::MSGT_FISNISHCL_MASTER;
		notify.giftDataId = masterGiftDataEntry->GetKey();
		GetOwner()->SendToScene(notify);

		//成长值转良师值
		//UInt32 addGoodTeachValue = academicTotalGrowth * masterGiftDataEntry->rate / 100;
		//GetOwner()->SyncEventToScene(SET_ADD_GOODTEACH_VALUE, addGoodTeachValue, discipleId);
	}
	else
	{
		InfoLogStream << PLAYERINFO(this->GetOwner()) << "WSMasterSystem::OnMasterAutomaticFinishSchool  masterGiftDataEntry = null, type =" << MasterSysGiftType::MSGT_FISNISHCL_MASTER << "academicTotalGrowth = " << academicTotalGrowth << " discipleId=" << discipleId << LogStream::eos;
	}

	//邮件发送徒弟奖励
	const MasterSysGiftDataEntry* discipleGiftDataEntry = MasterSysGiftMapDataEntryMgr::Instance()->GetDataEntrySpecic(MasterSysGiftType::MSGT_FISNISHCL_DISCIPLE, academicTotalGrowth);
	if (discipleGiftDataEntry)
	{
		if (discipleGiftDataEntry->giftIds.size() > 0)
		{
			std::vector<ItemReward> reward;
			for (UInt32 i = 0; i < discipleGiftDataEntry->giftIds.size(); i++)
			{
				reward.push_back(ItemReward(discipleGiftDataEntry->giftIds[i], 1, 0, 0));
			}
			std::string reason = GetReason(MASTERSYS_RECV_FINSCH_RWD);
			std::string title = SysNotifyMgr::Instance()->MakeString(9896);
			std::string sender = SysNotifyMgr::Instance()->MakeString(9800);
			std::string content = SysNotifyMgr::Instance()->MakeString(9895);
			MailBox::SendSysMail(discipleId, sender, title, content, reason, reward);
		}
		else
		{
			InfoLogStream << PLAYERINFO(this->GetOwner()) << "WSMasterSystem::OnMasterAutomaticFinishSchool giftIds is empty of id=" << discipleGiftDataEntry->GetKey() << " discipleId=" << discipleId << LogStream::eos;
		}
	}
	else
	{
		InfoLogStream << PLAYERINFO(this->GetOwner()) << "WSMasterSystem::OnMasterAutomaticFinishSchool  discipleGiftDataEntry = null, type =" << MasterSysGiftType::MSGT_FISNISHCL_DISCIPLE << "academicTotalGrowth = " << academicTotalGrowth << " discipleId=" << discipleId << LogStream::eos;
	}

	//增加出师徒弟数量
	this->AddFinSchDiscipleSum(1);

	//事件通知
	CLProtocol::SceneSyncEvent protocol;
	protocol.playerId = discipleId;
	protocol.type = SET_RELATION_DISCIPLE_FINISHSCHOOL;
	protocol.param1 = GetOwner()->GetID();
	protocol.param2 = this->GetFinSchDiscipleSum();

	GetOwner()->SendToScene(protocol);

	res.code = ErrorCode::SUCCESS;
	GetOwner()->SendProtocol(res);

	std::string discipleName = "";
	if (master_oldRelation)
	{
		discipleName = master_oldRelation->GetName();
	}
	
	//修改同门关系
	this->UpdateMatserSectRelationOnAutoFinSchOneDiscple(discipleId);
	WSMasterSystem::SendFinishSchoolAnnounce(this->GetOwner()->GetName(), discipleName);

	//广播出师事件
	WSMasterSystem::BroadcastFinschEvent(this->GetOwner()->GetName(), discipleName);

	this->GetOwner()->SendFinishSchoolUdpLog(MasterSysAutoFinSchType::MSAFST_MASTER, discipleId, this->GetOwner()->GetID(),
		GetOwner()->GetLevel(), discipleLevel, academicTotalGrowth, apprentTimeSec, (UInt32)currTime);
	return;
}

/**
*@brief 徒弟自动出师
*/
void WSMasterSystem::OnDiscipleAutomaticFinishSchool(ObjID_t masterId, WSPlayerShareData* masterShareData)
{
	CLProtocol::WorldAutomaticFinishSchoolRes res;

	if (GetOwner()->GetLevel() < SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_RELATION_FINISHLEVEL))
	{
		res.code = ErrorCode::FISNISH_SCHOOL_DISCIPLE_LEVELINSUFFIC;
		GetOwner()->SendProtocol(res);
		return;
	}

	if (masterShareData == NULL)
	{
		res.code = ErrorCode::ITEM_DATA_INVALID;
		GetOwner()->SendProtocol(res);
		return;
	}

	WSPlayer* master = WSPlayerMgr::Instance()->FindNormalPlayer(masterId);
	if (master)
	{
		res.code = ErrorCode::MASTERSYS_AUTOFINSCHFAIL_TATGETONLINE;
		GetOwner()->SendProtocol(res);
		return;
	}

	UInt64 needOfflineTime = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_MASTERSYS_AUTOFINISHSCH_OFFLINETIEM) * 60;
	if (needOfflineTime == 0) needOfflineTime = 1 * 24 * 3600;

	master = WSPlayerMgr::Instance()->FindPlayer(masterId);
	if (master)
	{
		UInt64 offlineTime = master->GetOfflineTimeSec();
		UInt64 currTime = Avalon::Time::CurrentTime().Sec();
		if (currTime - offlineTime < needOfflineTime)
		{
			res.code = ErrorCode::MASTERSYS_AUTOFINSCHFAIL_TATGETOFFLINETIME;
			GetOwner()->SendProtocol(res);
			return;
		}
	}

	UInt32 offlineTime = masterShareData->GetOfflineTimeSec();
		
	//UInt32 masterGoodTeachValue = masterShareData->GetMasterSysGoodTeacherValue();

	UInt64 currTime = Avalon::Time::CurrentTime().Sec();
	if (currTime - offlineTime < needOfflineTime)
	{
		res.code = ErrorCode::MASTERSYS_AUTOFINSCHFAIL_TATGETOFFLINETIME;
		GetOwner()->SendProtocol(res);
		return;
	}

	UInt32 masterLevel = 0;
	UInt32 apprentTimeSec = 0;

	//修改关系
	//出师后师徒关系变成好友关系
	WSRelation* disciple_oldRelation = this->GetOwner()->GetRelationMgr().GetRelation(masterId);
	if (disciple_oldRelation)
	{
		masterLevel = disciple_oldRelation->GetLevel();
		apprentTimeSec = disciple_oldRelation->GetAddTime();
		UpdateRelationDebugLog debugLog("WSMasterSystem::OnDiscipleAutomaticFinishSchool 1");
		this->GetOwner()->GetRelationMgr().UpdateRelation(masterId, disciple_oldRelation, RELATION_FRIEND);
	}

	if (master)
	{
		WSRelation* master_oldRelation = master->GetRelationMgr().GetRelation(GetOwner()->GetID());
		if (master_oldRelation)
		{
			UpdateRelationDebugLog debugLog("WSMasterSystem::OnDiscipleAutomaticFinishSchool 2");
			master->GetRelationMgr().UpdateRelation(GetOwner()->GetID(), master_oldRelation, RELATION_FRIEND);
		}
	}
	else
	{
		WSRelationMgr::UpdateRelationTypeToDb(masterId, RELATION_DISCIPLE, GetOwner()->GetID(), RELATION_FRIEND);
	}

	//发放徒弟奖励
	const MasterSysGiftDataEntry* discipleGiftDataEntry = MasterSysGiftMapDataEntryMgr::Instance()->GetDataEntrySpecic(MasterSysGiftType::MSGT_FISNISHCL_DISCIPLE, this->GetAcademicTotalGrowth());
	if (discipleGiftDataEntry)
	{
		CLProtocol::WorldNotifyGiveMasterSysGiftReqToScene notify;
		notify.discipleId = GetOwner()->GetID();
		notify.masterId = masterId;
		notify.type = MasterSysGiftType::MSGT_FISNISHCL_DISCIPLE;
		notify.giftDataId = discipleGiftDataEntry->GetKey();
		GetOwner()->SendToScene(notify);
	}
	else
	{
		InfoLogStream << PLAYERINFO(this->GetOwner()) << "WSMasterSystem::OnDiscipleAutomaticFinishSchool not find gift" << "masterId="<< masterId << " type=" << MasterSysGiftType::MSGT_FISNISHCL_DISCIPLE << " disciple academic growth=" << this->GetAcademicTotalGrowth() << LogStream::eos;
	}
	//师傅奖励(降低一个档次) 邮件发送
	const MasterSysGiftDataEntry* masterGiftDataEntry = MasterSysGiftMapDataEntryMgr::Instance()->GetDataEntrySpecic(MasterSysGiftType::MSGT_FISNISHCL_MASTER, this->GetAcademicTotalGrowth());
	if (masterGiftDataEntry)
	{
		masterGiftDataEntry = MasterSysGiftMapDataEntryMgr::Instance()->GetDataEntrySpecic(MasterSysGiftType::MSGT_FISNISHCL_MASTER, masterGiftDataEntry->param - 1);
		if (masterGiftDataEntry)
		{
			if (masterGiftDataEntry->giftIds.size() > 0)
			{
				std::vector<ItemReward> reward;
				for (UInt32 i = 0; i < masterGiftDataEntry->giftIds.size(); i++)
				{
					reward.push_back(ItemReward(masterGiftDataEntry->giftIds[i], 1, 0, 0));
				}
				std::string reason = GetReason(MASTERSYS_RECV_FINSCH_RWD);
				std::string title = SysNotifyMgr::Instance()->MakeString(9896);
				std::string sender = SysNotifyMgr::Instance()->MakeString(9800);
				std::string content = SysNotifyMgr::Instance()->MakeString(9894);
				MailBox::SendSysMail(masterId, sender, title, content, reason, reward);
			}
			else
			{
				InfoLogStream << PLAYERINFO(this->GetOwner()) << "WSMasterSystem::OnDiscipleAutomaticFinishSchool giftIds is empty of id=" << masterGiftDataEntry->GetKey() << LogStream::eos;
			}
		}
		else
		{
			InfoLogStream << PLAYERINFO(this->GetOwner()) << "WSMasterSystem::OnDiscipleAutomaticFinishSchool  masterGiftDataEntry = null, type =" << MasterSysGiftType::MSGT_FISNISHCL_MASTER << " academicTotalGrowth = " << this->GetAcademicTotalGrowth() << LogStream::eos;
		}
	}

	//增加师傅出师徒弟数量
	masterShareData->AddFinSchDiscipleSum(1);
	if (master)
	{
		master->GetMasterSystem().SetFinSchDiscipleSum(masterShareData->GetFinSchDiscipleSum());
	}

	//事件通知
	CLProtocol::SceneSyncEvent protocol;
	protocol.playerId = GetOwner()->GetID();
	protocol.type = SET_RELATION_DISCIPLE_FINISHSCHOOL;
	protocol.param1 = masterId;
	protocol.param2 = masterShareData->GetFinSchDiscipleSum();

	GetOwner()->SendToScene(protocol);

	res.code = ErrorCode::SUCCESS;
	GetOwner()->SendProtocol(res);

	std::string masterName = "";
	if (disciple_oldRelation)
	{
		masterName = disciple_oldRelation->GetName();
	}

	//修改同门关系
	this->UpdateMasterSectRelationOnSelfFinSch(masterId);

	WSMasterSystem::SendFinishSchoolAnnounce(masterName, this->GetOwner()->GetName());
	//广播出师事件
	WSMasterSystem::BroadcastFinschEvent(masterName, this->GetOwner()->GetName());

	//移除师徒头衔
	GetOwner()->GetTitleMgr().OnRemoveMaster();

	this->GetOwner()->SendFinishSchoolUdpLog(MasterSysAutoFinSchType::MSAFST_DISCIPLE, GetOwner()->GetID(), masterId,
		GetOwner()->GetLevel(), masterLevel, this->GetAcademicTotalGrowth(), apprentTimeSec, (UInt32)currTime);
	
	return;
}

void WSMasterSystem::OnRceiveMasterDailyTaskRewardSucc(UInt8 type, ObjID_t discipeleId)
{
	InfoLogStream << PLAYERINFO(this->GetOwner()) << ",  type : " << type << ", discipeleId : " << discipeleId << LogStream::eos;
	WSRelation* relation = NULL;
	if (type == MSGT_MASTER_DAILYTASK) //师傅领取
	{
		relation = GetOwner()->GetRelationMgr().FindRelation(RELATION_DISCIPLE, discipeleId);
		if (relation)
		{
			InfoLogStream << PLAYERINFO(this->GetOwner()) << ", relation id : " << relation->GetGuid() << "daily_mastertask_state : " << relation->GetDailyMasterTaskState() << LogStream::eos;
			if (relation->GetDailyMasterTaskState() == MasterDailyTaskState::MDTST_UNRECEIVED)
			{
				relation->SetDailyMasterTaskState(MasterDailyTaskState::MDTST_RECEIVEED);
				InfoLogStream << PLAYERINFO(this->GetOwner()) << ", set  daily_mastertask_state : " << MDTST_RECEIVEED << LogStream::eos;
			}
			else if (relation->GetDailyMasterTaskState() == MasterDailyTaskState::MDTST_UNPUB_UNRECE)
			{
				relation->SetDailyMasterTaskState(MasterDailyTaskState::MDTST_UNPUBLISHED);
				InfoLogStream << PLAYERINFO(this->GetOwner()) << ", set  daily_mastertask_state : " << MDTST_UNPUBLISHED << LogStream::eos;
			}
			UpdateRelationDebugLog debugLog("WSRelationMgr::OnRceiveMasterDailyTaskRewardSucc 1");
			GetOwner()->GetRelationMgr().UpdateRelation();
		}
		GetOwner()->SendReceiveDailyTaskGiftLog(1, GetOwner()->GetID(), UInt32(CURRENT_TIME.Sec()));
	}
	else if (type == MSGT_DISCIPLE_DAILYTASK) //徒弟领取
	{
		relation = GetOwner()->GetRelationMgr().GetRelation((UInt8)RELATION_MASTER);
		if (relation)
		{
			InfoLogStream << PLAYERINFO(this->GetOwner()) << ", relation id : " << relation->GetGuid() << "daily_mastertask_state : " << relation->GetDailyMasterTaskState() << LogStream::eos;
			relation->SetDailyMasterTaskState(MDTST_RECEIVEED);
			InfoLogStream << PLAYERINFO(this->GetOwner()) << ", set  daily_mastertask_state : " << MDTST_RECEIVEED << LogStream::eos;
		}
		UpdateRelationDebugLog debugLog("WSRelationMgr::OnRceiveMasterDailyTaskRewardSucc 2");
		GetOwner()->GetRelationMgr().UpdateRelation();

		GetOwner()->SendReceiveDailyTaskGiftLog(2, GetOwner()->GetID(), UInt32(CURRENT_TIME.Sec()));
	}
	
}

void WSMasterSystem::OnRceiveMasterAcademicRewardSucc(UInt32 masterSysGiftId)
{

}

/**
*@brief 处理师傅领取出师礼包完成
*/
void WSMasterSystem::OnReceiveMasterFinishSchoolRewardSucc(UInt32 giftId, ObjID_t discipleId)
{
	CLProtocol::WorldNotifyFinshSchoolReward notify;
	notify.masterId = GetOwner()->GetID();
	notify.discipleId = discipleId;
	notify.giftId = giftId;
	
	WSPlayer* disciple = WSPlayerMgr::Instance()->FindPlayer(discipleId);
	if (disciple)
	{
		notify.discipleName = disciple->GetName();
		notify.discipleGrowth = disciple->GetMasterSystem().GetAcademicTotalGrowth();
	}
	
	GetOwner()->SendProtocol(notify);
}

/**
*@brief 处理徒弟领取出师礼包完成
*/
void WSMasterSystem::OnReceiveDiscipleFinishSchoolRewardSucc(UInt32 giftId, ObjID_t masterId)
{
	CLProtocol::WorldNotifyFinshSchoolReward notify;
	notify.masterId = masterId;
	notify.discipleId = GetOwner()->GetID();
	notify.giftId = giftId;
	notify.discipleGrowth = GetOwner()->GetMasterSystem().GetAcademicTotalGrowth();

	WSPlayer* master = WSPlayerMgr::Instance()->FindPlayer(masterId);
	if (master)
	{
		notify.masterName = master->GetName();
		
	}

	GetOwner()->SendProtocol(notify);
}

void WSMasterSystem::OnMasterDailyTaskComplete()
{
	auto relation = GetOwner()->GetRelationMgr().GetRelation((UInt8)RELATION_MASTER);
	if (relation)
	{
		relation->SetDailyMasterTaskState(MDTST_UNRECEIVED);
		relation->SyncToDB();
		UpdateRelationDebugLog debugLog("WSMasterSystem::OnMasterDailyTaskComplete 1");
		GetOwner()->GetRelationMgr().UpdateRelation();
		InfoLogStream << PLAYERINFO(this->GetOwner()) << ", master daily task is complete , relation id : " << relation->GetGuid()
			<< ", master id : " << relation->GetID() << ", set  daily_mastertask_state : " << relation->GetDailyMasterTaskState() << LogStream::eos;
	
		auto master = WSPlayerMgr::Instance()->FindPlayer(relation->GetID());
		if (master)
		{
			auto disciple_relation = master->GetRelationMgr().FindRelation(RELATION_DISCIPLE, GetOwner()->GetID());
			if (disciple_relation)
			{
				disciple_relation->SetDailyMasterTaskState(MDTST_UNRECEIVED);
				disciple_relation->SyncToDB();
				UpdateRelationDebugLog debugLog("WSMasterSystem::OnMasterDailyTaskComplete 2");
				master->GetRelationMgr().UpdateRelation();
				InfoLogStream << PLAYERINFO(master) << ",  relation id : " << disciple_relation->GetGuid() << ", disciple id : "
					<< relation->GetID() << ", set  daily_mastertask_state : " << relation->GetDailyMasterTaskState() << LogStream::eos;
			}
		}
		else
		{
			WSMasterSystem::SyncMasterDailyTaskStateToDB(relation->GetID(), GetOwner()->GetID(), MDTST_UNRECEIVED);
		}
	}
}

void WSMasterSystem::SyncToDB()
{
	class SyncToDBVisitor : public MasterSectRelationVisitor
	{
	public:
		SyncToDBVisitor(WSPlayer* owner) :m_pOwner(owner){}

		bool operator()(MasterSectRelation* relation)
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

void WSMasterSystem::SyncLevelToDB(ObjID_t playerId, UInt16 level)
{
	std::ostringstream condstream;
	condstream << "`id`=" << playerId;
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_mastersect_relation", condstream.str());
	updateCmd->PutData("level", level);
	CLRecordClient::Instance()->SendCommand(updateCmd);
}

void WSMasterSystem::SyncVipLevelToDB(ObjID_t playerId, UInt16 vipLvl)
{
	std::ostringstream condstream;
	condstream << "`id`=" << playerId;
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_mastersect_relation", condstream.str());
	updateCmd->PutData("viplv", vipLvl);
	CLRecordClient::Instance()->SendCommand(updateCmd);
}

void WSMasterSystem::SyncOccuToDB(ObjID_t playerId, UInt8 occu)
{
	std::ostringstream condstream;
	condstream << "`id`=" << playerId;
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_mastersect_relation", condstream.str());
	updateCmd->PutData("occu", occu);
	CLRecordClient::Instance()->SendCommand(updateCmd);
}

void WSMasterSystem::SyncNameToDB(ObjID_t playerId, const std::string& name)
{
	std::ostringstream condstream;
	condstream << "`id`=" << playerId;
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_mastersect_relation", condstream.str());
	updateCmd->PutData("name", name);
	CLRecordClient::Instance()->SendCommand(updateCmd);
}


void WSMasterSystem::SendPlayerDelRelation(MasterSectRelationType type, ObjID_t tarId)
{
	CLProtocol::WorldNotifyDelMasterSectRelation notify;
	notify.type = (UInt8)type;
	notify.id = tarId;
	GetOwner()->SendProtocol(notify);
}

void WSMasterSystem::SyncMasterDailyTaskStateToDB(ObjID_t masterId, ObjID_t discipleId, UInt8 state)
{
	std::ostringstream condstream;
	condstream << "owner =" << masterId << " and `id`=" << discipleId << " and `type`=" << (Int32)RELATION_DISCIPLE;
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_relation", condstream.str());
	updateCmd->PutData("daily_mastertask_state", state);
	CLRecordClient::Instance()->SendCommand(updateCmd);
}

void WSMasterSystem::SyncDiscipleDailyTaskStateToDB(ObjID_t masterId, ObjID_t discipleId, UInt8 state)
{
	std::ostringstream condstream;
	condstream << "owner =" << discipleId << " and `id`=" << masterId << " and `type`=" << (Int32)RELATION_MASTER;
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_relation", condstream.str());
	updateCmd->PutData("daily_mastertask_state", state);
	CLRecordClient::Instance()->SendCommand(updateCmd);
}

void WSMasterSystem::SendFinishSchoolAnnounce(const std::string& masterName, const std::string& discipleName)
{
	UInt32 announcementId = 81;
	AnnouncementMgr::Instance()->PublicAnnouncement(0, announcementId, (UInt32)CURRENT_TIME.Sec(), discipleName.c_str(), masterName.c_str());
}

void WSMasterSystem::SyncFinSchMarkToDB(ObjID_t playerId)
{
	std::ostringstream condstream;
	condstream << "`type` =" << MasterSectRelationType::MSRELATION_BROTHER << " and `id` =" << playerId;
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_mastersect_relation", condstream.str());
	updateCmd->PutData("isFinSch", 1);
	CLRecordClient::Instance()->SendCommand(updateCmd);
}

void WSMasterSystem::UpdateMasterSectRelationOnSelfFinSch(ObjID_t masterId)
{
	WSPlayer* master = WSPlayerMgr::Instance()->FindPlayer(masterId);
	if (master)
	{
		MasterSectRelation* master_relation = master->GetMasterSystem().GetRelation(MasterSectRelationType::MSRELATION_DISCIPLE, this->GetOwner()->GetID());
		if (master_relation)
		{
			master_relation->SetFinSchMark(true);
		}
	}
	else
	{
		std::ostringstream condstream;
		condstream << "`owner` =" << masterId <<" and `type` =" << MasterSectRelationType::MSRELATION_DISCIPLE << " and `id` =" << this->GetOwner()->GetID();
		CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_mastersect_relation", condstream.str());
		updateCmd->PutData("isFinSch", 1);
		CLRecordClient::Instance()->SendCommand(updateCmd);
	}

	MasterSectRelationMap& relations = m_Relations[MasterSectRelationType::MSRELATION_BROTHER];
	MasterSectRelationMap::iterator iter = relations.begin();
	for (; iter != relations.end(); iter++)
	{
		iter->second->SetFinSchMark(true);
		WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(iter->first);
		if (player)
		{
			MasterSectRelation* relation = player->GetMasterSystem().GetRelation(MasterSectRelationType::MSRELATION_BROTHER, iter->first);
			if (relation)
			{
				relation->SetFinSchMark(true);
			}
		}
	}
	WSMasterSystem::SyncFinSchMarkToDB(this->GetOwner()->GetID());
}

void WSMasterSystem::UpdateMatserSectRelationOnAutoFinSchOneDiscple(ObjID_t discipleId)
{
	MasterSectRelation* disciple_relation = GetRelation(MasterSectRelationType::MSRELATION_DISCIPLE, discipleId);
	if (disciple_relation)
	{
		disciple_relation->SetFinSchMark(true);
	}

	//修改出师徒弟的同门数据
	WSPlayer* disciple = WSPlayerMgr::Instance()->FindPlayer(discipleId);
	if (disciple)
	{
		MasterSectRelationMap& relations = disciple->GetMasterSystem().m_Relations[MasterSectRelationType::MSRELATION_BROTHER];
		MasterSectRelationMap::iterator iter = relations.begin();
		for (; iter != relations.end(); iter++)
		{
			if (iter->second)
			{
				iter->second->SetFinSchMark(true);
			}
		}
	}
	else
	{
		std::ostringstream condstream;
		condstream << "`type` =" << MasterSectRelationType::MSRELATION_BROTHER << " and `owner` =" << discipleId;
		CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_mastersect_relation", condstream.str());
		updateCmd->PutData("isFinSch", 1);
		CLRecordClient::Instance()->SendCommand(updateCmd);
	}

	MasterSectRelationMap& relations = m_Relations[MasterSectRelationType::MSRELATION_DISCIPLE];
	MasterSectRelationMap::iterator iter = relations.begin();
	for (; iter != relations.end(); iter++)
	{
		if (iter->first != discipleId)
		{
			WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(iter->first);
			if (player)
			{
				MasterSectRelation* relation = player->GetMasterSystem().GetRelation(MasterSectRelationType::MSRELATION_BROTHER, discipleId);
				if (relation)
				{
					relation->SetFinSchMark(true);
				}
			}
		}
	}
	WSMasterSystem::SyncFinSchMarkToDB(discipleId);
}

void WSMasterSystem::OnPlayerForeverDeleted(ObjID_t roleId)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(roleId);
	if (player)
	{
		player->GetMasterSystem().ClearRelations();
	}
	else
	{
		class PlayerVisitor : public WSPlayerVisitor
		{
		public:
			PlayerVisitor(ObjID_t roleId) : m_roleId(roleId) {}
			~PlayerVisitor() {}
			bool operator()(WSPlayer* player)
			{
				if (NULL == player || m_roleId == player->GetID()) return true;
				MasterSectRelation* relation = player->GetMasterSystem().GetRelation(m_roleId);
				if (relation)
				{
					player->GetMasterSystem().RemoveRelation(relation->GetType(), m_roleId);
				}
				return true;
			}

			ObjID_t m_roleId;
		};

		PlayerVisitor visitor(roleId);
		WSPlayerMgr::Instance()->VisitPlayers(visitor);

		std::ostringstream oss;
		oss << "`id`='" << roleId << "' OR `owner`='" << roleId << "'";
		CLDeleteCommand* delCommand = CLRecordClient::Instance()->PrepareDeleteCommand("t_mastersect_relation", oss.str());
		CLRecordClient::Instance()->SendCommand(delCommand);
	}
}

void WSMasterSystem::BroadcastFinschEvent(const std::string& masterName, const std::string& discipleName)
{
	class Visitor : public WSPlayerVisitor
	{
	public:
		Visitor(const std::string& masterName, const std::string& discipleName)
			: m_masterName(masterName), m_discipleName(discipleName)
		{
		}

		bool operator()(WSPlayer* player)
		{
			if (player && player->GetSceneType() == SceneType::SCENE_TYPE_NORMAL)
			{
				player->SendProtocol(m_protocol);
			}
			return true;
		}

		CLProtocol::WorldNotifyFinSchEvent m_protocol;
		std::string m_masterName;
		std::string m_discipleName;
	};

	Visitor visitor(masterName, discipleName);
	WSPlayerMgr::Instance()->VisitNormalPlayers(visitor);
}

void WSMasterSystem::SetRecvingMasterDailyTaskRewardState(UInt64 timesec)
{
	this->m_IsRecvingMasterDailyTaskReward = true;
	this->m_RecvingMasterDailyTaskRewardTime = timesec;
}

void WSMasterSystem::ResetRecvingMasterDailyTaskRewardState()
{
	this->m_IsRecvingMasterDailyTaskReward = false;
	this->m_RecvingMasterDailyTaskRewardTime = 0;
}

void WSMasterSystem::SetRecvingDiscipleDailyTaskRewardState(UInt64 timesec)
{
	this->m_IsRecvingDiscipleDailyTaskReward = true;
	this->m_RecvingDiscipleDailyTaskRewardTime = timesec;
}

void WSMasterSystem::ResetRecvingDiscipleDailyTaskRewardState()
{
	this->m_IsRecvingDiscipleDailyTaskReward = false;
	this->m_RecvingDiscipleDailyTaskRewardTime = 0;
}
