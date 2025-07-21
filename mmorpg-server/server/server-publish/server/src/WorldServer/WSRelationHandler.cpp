#include "WSRelationHandler.h"
#include <CLRecordClient.h>
#include <CLChatProtocol.h>
#include <CLFunctionDataEntry.h>
#include <CLSystemValueDataEntry.h>
#include <CLForbitWord.h>
#include <CLTaskProtocol.h>
#include <CLMasterSysGiftDataEntry.h>
#include <CLUtf8Define.h>
#include <CLLostDungeonProtocol.h>
#include <CLTransferProtocol.h>

#include "WSPlayerMgr.h"
#include "WSSceneMgr.h"
#include "MailBox.h"
#include "GuildMgr.h"
#include "WSTeam.h"
#include "MallMgr.h"
#include "PunishmentMgr.h"
#include "WSMasterSystem.h"
#include "WSPlayerShareDataMgr.h"
#include "WSShareDataLoadCallback.h"
#include "AnnouncementMgr.h"
#include "CLMsgParser.h"
#include "WSServiceSwitchMgr.h"
#include "WSSortListMgr.h"
#include "AccountMgr.h"

/**
*@brief 查询离线徒弟师门任务回调
*/
class ShareDataLoadingForGetDiscipleMasterTasks : public WSShareDataLoadCallback
{
public:
	ShareDataLoadingForGetDiscipleMasterTasks(ObjID_t requesterId,const CLProtocol::WorldGetDiscipleMasterTasksReq &req)
		: m_RequesterId(requesterId), m_Req(req) {}

	/**
	*@breif 加载成功
	*/
	virtual void OnFinish(WSPlayerShareData* shareData)
	{
		InfoLogStream << "sharedata(" << shareData->GetID() << ") loading for query disciple master tasks, start to send tasks." << LogStream::eos;
		WSPlayer* requester = WSPlayerMgr::Instance()->FindNormalPlayer(this->m_RequesterId);
		if (requester)
		{
			CLProtocol::WorldGetDiscipleMasterTasksRes res;
			res.code = ErrorCode::SUCCESS;
			res.discipleId = shareData->GetID();
			shareData->GetMasterTaskShareData(res.masterTaskShareData);
			requester->SendProtocol(res);
		}
	}

	/**
	*@brief 加载失败
	*/
	virtual void OnFailure(WSPlayerShareData* shareData)
	{
		ErrorLogStream << "sharedata(" << m_Req.discipleId << ") loading failed, requestId(" << m_RequesterId << ")." << LogStream::eos;
		
		WSPlayer* requester = WSPlayerMgr::Instance()->FindNormalPlayer(this->m_RequesterId);
		if (requester)
		{
			CLProtocol::WorldGetDiscipleMasterTasksRes res;
			res.code = ErrorCode::GETDISCIPLE_MASTERTASKS_FAIL;
			res.discipleId = m_Req.discipleId;
			requester->SendProtocol(res);
		}

		ErrorLogStream << "shareData(" << shareData->GetID() << "loading failed ..." << LogStream::eos;
	}

	/**
	*@brief 加载超时
	*/
	virtual void OnTimeout(WSPlayerShareData* shareData)
	{
		OnFailure(shareData);
	}

	ObjID_t		m_RequesterId;
	CLProtocol::WorldGetDiscipleMasterTasksReq		m_Req;
};

/**
*@brief 查询玩家回调
*/
class QueryPlayerCb : public CLRecordCallback
{
public:
	QueryPlayerCb(ObjID_t srcUid) :m_srcUid(srcUid){}
	~QueryPlayerCb() {}


	void OnFinished()
	{
		if (!NextRow())
		{
			OnFailed(0);
			return;
		}

		UInt32 tarAccid = GetData("accid");
		WSPlayer* srcPlayer = WSPlayerMgr::Instance()->FindNormalPlayer(m_srcUid);
		if (!srcPlayer)
		{
			return;
		}

		CLProtocol::SceneQueryPlayerReq protocol;
		protocol.srcRoleId = srcPlayer->GetID();
		protocol.tarAccId = tarAccid;
		protocol.targetRoleId = GetKey();
		srcPlayer->SendToScene(protocol);
	}

	void OnFailed(UInt32 errorcode)
	{
		WSPlayer* srcPlayer = WSPlayerMgr::Instance()->FindNormalPlayer(m_srcUid);
		if (!srcPlayer)
		{
			return;
		}

		srcPlayer->SendNotify(1500001);
	}
private:

	ObjID_t m_srcUid;
};


/**
*@brief 查询玩家详细信息回调
*/
class QueryPlayerDetailsCb : public CLRecordCallback
{
public:
	QueryPlayerDetailsCb(ObjID_t srcUid) :m_srcUid(srcUid){}
	~QueryPlayerDetailsCb() {}


	void OnFinished()
	{
		if (!NextRow())
		{
			OnFailed(0);
			return;
		}

		WSPlayer* srcPlayer = WSPlayerMgr::Instance()->FindNormalPlayer(m_srcUid);
		if (!srcPlayer)
		{
			return;
		}

		CLProtocol::SceneQueryPlayerDetailsReq protocol;
		protocol.srcRoleId = srcPlayer->GetID();
		protocol.targetRoleId = GetKey();
		srcPlayer->SendToScene(protocol);
	}

	void OnFailed(UInt32 errorcode)
	{
		WSPlayer* srcPlayer = WSPlayerMgr::Instance()->FindNormalPlayer(m_srcUid);
		if (!srcPlayer)
		{
			return;
		}

		srcPlayer->SendNotify(1500001);
	}
private:

	ObjID_t m_srcUid;
};

void HandleProtocol(WSPlayer* player, CLProtocol::WorldQueryPlayerReq& req)
{
	if (!player)
	{
		return;
	}

	InfoLogStream << "player(" << player->GetID() << ") request to query player info, targetUid(" << req.roleId << ")." << "targetName(" << req.name << ")." << LogStream::eos;

	if (req.zoneId != 0 && req.zoneId != ZONE_ID)
	{
		if (req.roleId != 0)
		{
			TransferHeader head(req.zoneId, ST_SCENE, req.roleId, player->GetID());
			CLProtocol::TransferQueryPlayerOne query;
			Router::SendToTransfer(head, query);
		}
		return;
	}

	WSPlayer* target = NULL;
	if (req.roleId > 0)
	{
		target = WSPlayerMgr::Instance()->FindNormalPlayer(req.roleId);
	}
	else
	{
		target = WSPlayerMgr::Instance()->FindNormalPlayerByName(req.name);
	}
	 
	if(target == NULL)
	{
		// 查对应玩家accid,roleid
		CLSelectCommand* selectcmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
		selectcmd->PutColumn("accid");

		if (req.roleId > 0)
		{
			// 通过roleId查找
			selectcmd->PutCond("guid", req.roleId);
		}
		else
		{
			// 通过名字查找
			selectcmd->PutCond("name", req.name);
		}

		// 被删除的玩家不能查询
		selectcmd->PutCond("deletetime", 0);

		CLRecordClient::Instance()->SendCommand(selectcmd, new QueryPlayerCb(player->GetID()));

	}
	else
	{
		CLProtocol::SceneQueryPlayerReq protocol;
		protocol.srcRoleId = player->GetID();
		protocol.targetRoleId = target->GetID();
		protocol.tarAccId = target->GetAccId();
		target->SendToScene(protocol);
	}
}

void HandleProtocol(CLProtocol::SceneQueryPlayerRet& ret)
{
	auto player = WSPlayerMgr::Instance()->FindNormalPlayer(ret.roldId);
	if (!player)
	{
		return;
	}

	if (ret.success == 0)
	{
		player->SendNotify(1085);
		DebugLogStream << "player(" << player->GetID() << ") request to query player info, target offline." << LogStream::eos;
		return;
	}

	CLProtocol::WorldQueryPlayerRet protocol;
	protocol.info = ret.info;
	protocol.zoneId = ZONE_ID;

	// 获取公会信息
	auto guildMember = GuildMgr::Instance()->FindGuildMember(ret.info.id);
	if (guildMember)
	{
		auto guild = GuildMgr::Instance()->FindGuildByID(guildMember->GetGuildID());
		if (guild && guild->FindMember(ret.info.id))
		{
			protocol.info.guildTitle.guildName = guild->GetName();
			protocol.info.guildTitle.guildPost = guildMember->GetPost();
		}

		protocol.info.emblemLevel = guildMember->GetEmblemLevel();
	}

	auto targetPlayer = WSPlayerMgr::Instance()->FindPlayer(ret.info.id);
	if (targetPlayer)
	{
		protocol.info.activeTimeType = targetPlayer->GetMasterSystem().GetQuestionnaire().activeTimeType;
		protocol.info.masterType = targetPlayer->GetMasterSystem().GetQuestionnaire().masterType;
		protocol.info.regionId = targetPlayer->GetMasterSystem().GetQuestionnaire().regionId;
		protocol.info.declaration = targetPlayer->GetMasterSystem().GetQuestionnaire().declaration;
	}

	UInt32 targetAccId = AccountMgr::Instance()->GetAccIDByRoleID(ret.info.id);
	auto targetAccount = AccountMgr::Instance()->FindAccount(targetAccId);
	if (targetAccount)
	{
		protocol.info.adventureTeamName = targetAccount->GetAdventureTeamName();
		protocol.info.adventureTeamGrade = targetAccount->GetAdventureTeamGradeStr();
		protocol.info.adventureTeamRanking = WSSortListMgr::Instance()->GetRanking(SORTLIST_ADVENTURE_TEAM_GRADE, targetAccount->GetAccID());
	}
	
	player->SendProtocol(protocol);
	
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldQueryPlayerDetailsReq& req)
{
	if (!player)
	{
		return;
	}

	DebugLogStream << "player(" << player->GetID() << ") request to query player details info, targetUid(" << req.roleId << ")." << "targetName(" << req.name << ")." << LogStream::eos;

	if (req.type == QPT_BATTLE_LOST)
	{
		if (req.roleId == 0)
			return;

		CLProtocol::BattleQuerylayerDetailOne query;
		query.srcRoleId = player->GetID();
		query.targetRoleId = req.roleId;
		query.queryType = req.type;
		Router::SendToBattle(query);
		return;
	}

	// 跨服查询玩家信息
	if (req.zoneId != 0 && req.zoneId != ZONE_ID)
	{
		if (req.roleId != 0)
		{
			TransferHeader head(req.zoneId, ST_SCENE, req.roleId, player->GetID());
			CLProtocol::TransferQueryPlayerDetailOne query;
			Router::SendToTransfer(head, query);
		}
		return;
	}

	WSPlayer* target = NULL;
	if (req.roleId > 0)
	{
		target = WSPlayerMgr::Instance()->FindNormalPlayer(req.roleId);
	}
	else
	{
		target = WSPlayerMgr::Instance()->FindNormalPlayerByName(req.name);
	}

	CLProtocol::SceneQueryPlayerDetailsReq protocol;
	if (target == NULL)
	{
		protocol.srcRoleId = player->GetID();
		if (req.roleId > 0)
		{
			protocol.targetRoleId = req.roleId;
			player->SendToScene(protocol);
		}
		else
		{
			// 查对应玩家id
			CLSelectCommand* selectcmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
			selectcmd->PutColumn("accid");
			selectcmd->PutCond("name", req.name);
			CLRecordClient::Instance()->SendCommand(selectcmd, new QueryPlayerDetailsCb(player->GetID()));
		}
	}
	else
	{
		protocol.srcRoleId = player->GetID();
		protocol.targetRoleId = target->GetID();
		target->SendToScene(protocol);
	}
}

void HandleProtocol(CLProtocol::SceneQueryPlayerDetailsRet& ret)
{
	auto player = WSPlayerMgr::Instance()->FindNormalPlayer(ret.roldId);
	if (!player)
	{
		return;
	}

	if (ret.success == 0)
	{
		player->SendNotify(1085);
		DebugLogStream << "player(" << player->GetID() << ") request to query player details info, target offline." << LogStream::eos;
		return;
	}

	CLProtocol::WorldQueryPlayerDetailsRet protocol;
	protocol.info = ret.info;

	// 获取公会信息
	//auto guildMember = GuildMgr::Instance()->FindGuildMember(ret.info.id);

	player->SendProtocol(protocol);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAddToBlackList& req)
{
	//判断开放等级
	auto functionData = FuncNotifyDataEntryMgr::Instance()->FindEntry(FUNC_FRIEND);
	if (functionData)
	{
		if (player->GetLevel() < functionData->endLevel)
		{
			player->SendNotify(1500005);
			return;
		}
	}

	//已有黑名单
	if (player->GetRelationMgr().FindRelation(RELATION_BLACKLIST, req.id) != NULL)
	{
		player->SendNotify(3118);
		return;
	}

	//黑名单满
	if (player->GetRelationMgr().IsFull(RELATION_BLACKLIST))
	{
		player->SendNotify(3119);
		return;
	}

	//不能加自己
	if (player->GetID() == req.id)
	{
		player->SendNotify(3049);
		return;
	}

	WSRelation* masterRelation = player->GetRelationMgr().FindRelation(RELATION_MASTER, req.id);
	WSRelation* discipleRelation = player->GetRelationMgr().FindRelation(RELATION_DISCIPLE, req.id);
	if (masterRelation || discipleRelation)
	{
		player->SendNotify(9850);
		return;
	}

	WSPlayer* target = WSPlayerMgr::Instance()->FindPlayer(req.id);
	if (target == NULL)
	{
		WSPlayer* target_temp = new WSPlayer();
		target_temp->SetOfflineReplyCtx(player, LT_ADDTO_BLACKLIST);
		target_temp->SetID(req.id);
		WSPlayerMgr::Instance()->AddOfflinePlayerCache(target_temp);
		target_temp->LoadData();

		return;
	}

	if (player->AddToBlackList(target))
	{
		player->SendNotify(3121);
	}

}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldRemoveRelation& req)
{
	//判断开放等级
	if (!player->GetRelationMgr().CheckRelationOpenLv((RelationType)req.type))
	{
		player->SendNotify(1500005);
		return;
	}

	WSRelation *relation = player->GetRelationMgr().FindRelation(req.type, req.id);
	if(relation == NULL) return;

	if (req.type == RelationType::RELATION_FRIEND)	//好友关系移除双方
	{
		player->RemoveFriend(req.id);
	}
	else if (req.type == RelationType::RELATION_BLACKLIST)	//黑名单
	{
		//player->GetRelationMgr().RemoveRelation(RELATION_BLACKLIST, req.id);
		player->GetRelationMgr().ChangeToStranger(RELATION_BLACKLIST, req.id);
	}
	else if (req.type == RelationType::RELATION_DISCIPLE) // 徒弟
	{
		player->RemoveDisciple(req.id);
	}
	else if (req.type == RelationType::RELATION_MASTER) // 师傅
	{
		player->RemoveMaster(req.id);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldChaseKill& req)
{
	return;
	/*
	WSPlayer* target = WSPlayerMgr::Instance()->FindNormalPlayer(req.id);
	if(target == NULL || player == target) return;

	if(player->GetRelationMgr().FindRelation(RELATION_ENEMY, target->GetID()) == NULL) return;

	CLProtocol::SceneRelationFindPlayerReq findReq;
	findReq.killer = player->GetID();
	findReq.target = req.id;
	target->SendToScene(findReq);
	*/
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldSetOnlineNotify& req)
{
	if(req.type != RELATION_FRIEND /*&& req.type != RELATION_ENEMY*/) return;

	WSRelation* relation = player->GetRelationMgr().FindRelation(req.type, req.id);
	if(relation == NULL) return;

	relation->SetOnlineNotify(req.notify);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldRelationFindPlayersReq& req)
{
	struct MasterCompare
	{
		bool operator() (const WSPlayer* player1, const WSPlayer* player2) const
		{
			if (player1->GetLevel() == player2->GetLevel())
			{
				if (player1->GetVipLvl() == player2->GetVipLvl())
				{
					return player1->GetID() > player2->GetID();
				}
				else
				{
					return player1->GetVipLvl() > player2->GetVipLvl();
				}
			}
			else
			{
				return player1->GetLevel() > player2->GetLevel();
			}
		}
	};

	struct DiscipleCompare
	{
		bool operator() (const WSPlayer* player1, const WSPlayer* player2) const
		{
			return player1->GetLevel() >= player2->GetLevel();
		}
	};

	class GetRecommandVisitor : public WSPlayerVisitor
	{
	public:
		typedef std::vector<WSPlayer*> PlayerVector;

	public:
		GetRecommandVisitor(WSPlayer* player, UInt8 type) :m_pPlayer(player), m_relationType(type)
		{
			m_TeamMinLevel = 0;
			if (m_pPlayer)
			{
				auto team = m_pPlayer->GetTeam();
				m_TeamMinLevel = team ? team->GetMinLevelLimit() : 0;
			}
			
			auto teamFunctionData = FuncNotifyDataEntryMgr::Instance()->FindEntry(FUNC_TEAM);
			if (teamFunctionData)
			{
				m_TeamFuncLevel = teamFunctionData->endLevel;
			}
			else
			{
				m_TeamFuncLevel = 0;
			}

			auto frientFunctionData = FuncNotifyDataEntryMgr::Instance()->FindEntry(FUNC_FRIEND);
			if (frientFunctionData)
			{
				m_FriendFuncLevel = frientFunctionData->endLevel;
			}
			else
			{
				m_FriendFuncLevel = 0;
			}

			FuncNotifyDataEntry* roomFunctionData = FuncNotifyDataEntryMgr::Instance()->FindEntry(FUNC_PK);
			if (roomFunctionData)
			{
				m_RoomFuncLevel = roomFunctionData->endLevel;
			}
			else
			{
				m_RoomFuncLevel = 0;
			}

			m_MasterFuncLevel = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_TAP_SYSTEM_OPEN_LEVEL);

			m_curr_timestamp = Avalon::Time::CurrentTime().Sec();
		}

		~GetRecommandVisitor(){}

		bool operator()(WSPlayer* player)
		{
			if (!player)	return true;
			
			if(m_pPlayer == player) return true;

			if (m_relationType == RelationFindType::RFT_TEAM)
			{
				DoRecommandTeamPlayer(player);
			}
			else if (m_relationType == RelationFindType::RFT_MASTER)
			{
				DoRecommanMaster(player);
			}
			else if (m_relationType == RelationFindType::RFT_DISCIPLE)
			{
				DoRecommanDisciple(player);
			}
			else if (m_relationType == RelationFindType::RFT_FRIEND)
			{
				DoRecommanFriend(player);
			}
			else if (m_relationType == RelationFindType::RFT_ROOM)
			{
				DoRecommanRoomPlayer(player);
			}
			else
			{
				return false;	// 关系查找类型不正确直接break
			}

			return true;
		}

		void DoRecommanRoomPlayer(WSPlayer* player)
		{
			if (m_pPlayer == NULL && m_pPlayer->GetRoomId() == 0) return;

			if (player->GetTeam() == NULL && player->GetRoomId() == 0 && player->GetLevel() >= m_RoomFuncLevel && player->GetSeasonLv() >= m_pPlayer->GetRoomLimitSeasonLevel() &&
				(player->IsInTown() || player->GetSceneType() == SCENE_TYPE_PK_PREPARE) && player->GetLevel() >= m_pPlayer->GetRoomLimitLevel())
			{
				m_NotRoomPlayers.push_back(player);
			}
		}

		// 推荐组队玩家
		void DoRecommandTeamPlayer(WSPlayer* player)
		{
			if (player->GetTeam() == NULL && player->IsInTown() && player->GetLevel() >= m_TeamMinLevel && player->GetLevel() >= m_TeamFuncLevel &&
				!m_pPlayer->GetRequestMgr().HasRequest(REQUEST_INVITETEAM, player->GetID()) && player->GetRoomId() == 0)
			{
				m_NotTeamPlayers.push_back(player);
			}
		}

		// 推荐徒弟
		void DoRecommanDisciple(WSPlayer* player)
		{
			if (m_pPlayer->GetID() == player->GetID())
			{
				return;
			}
			// 该玩家短时间被请求收徒过
			if (!m_pPlayer->GetRelationMgr().FindRequestPlayer(RelationType::RELATION_DISCIPLE, player->GetID()))
			{
				return;
			}

			if (!m_pPlayer->CanReqDisciple(player->GetID(), false))
			{
				return;
			}

			if (!m_pPlayer->GetMasterSystem().CheckRecommanDiscipleTimeStamp(player->GetID(), m_curr_timestamp))
			{
				return;
			}

			UInt8 matchNum = WSMasterSystem::CalcQuestionnaireMatchNum(m_pPlayer, player);
			m_RecommanDisciples[matchNum].push_back(player);
			//m_pPlayer->GetMasterSystem().SetRecommanDiscipleTimeStamp(player->GetID(), m_curr_timestamp);
		}

		// 推荐师傅
		void DoRecommanMaster(WSPlayer* player)
		{
			// 该玩家短时间被请求拜师过
			if (!m_pPlayer->GetRelationMgr().FindRequestPlayer(RelationType::RELATION_MASTER, player->GetID()))
			{
				return;
			}

			if (!m_pPlayer->CanReqMaster(player->GetID(), false))
			{
				return;
			}

			if (!m_pPlayer->GetMasterSystem().CheckRecommanMasterTimeStamp(player->GetID(), m_curr_timestamp))
			{
				return;
			}

			UInt8 matchNum = WSMasterSystem::CalcQuestionnaireMatchNum(player, m_pPlayer);
			m_RecommanMasters[matchNum].push_back(player);
			//m_pPlayer->GetMasterSystem().SetRecommanMasterTimeStamp(player->GetID(), m_curr_timestamp);
		}

		// 推荐好友
		void DoRecommanFriend(WSPlayer* player)
		{
			if (m_pPlayer->GetRelationMgr().FindRelation(RelationType::RELATION_MASTER, player->GetID()) != NULL) return;
			if (player->GetRelationMgr().IsFull(RelationType::RELATION_FRIEND)) return;
			if (m_pPlayer->GetRelationMgr().FindRelation(RelationType::RELATION_FRIEND, player->GetID()) != NULL) return;

			if (player->GetLevel() >= m_FriendFuncLevel)
			{
				if (player->GetLevel() + 5 >= m_pPlayer->GetLevel() && player->GetLevel() <= m_pPlayer->GetLevel() + 5)
				{
					m_LevelPlayers.push_back(player);
				}
				else
				{
					m_OtherPlayers.push_back(player);
				}
			}
		}

		void GetRoomPlayers(std::vector<WSPlayer*>& players)
		{
			if (players.size() < QUICK_MAKE_FRIEND_NUM)
			{
				if (m_NotRoomPlayers.size() > QUICK_MAKE_FRIEND_NUM - players.size())
				{
					players.insert(players.end(), m_NotRoomPlayers.begin(), m_NotRoomPlayers.begin() + QUICK_MAKE_FRIEND_NUM - players.size());
				}
				else
				{
					players.insert(players.end(), m_NotRoomPlayers.begin(), m_NotRoomPlayers.end());
				}
			}
			std::random_shuffle(players.begin(), players.end());
			if (players.size() > QUICK_MAKE_FRIEND_NUM)
			{
				players.erase(players.begin() + QUICK_MAKE_FRIEND_NUM, players.end());
			}
		}

		// 获得组队推荐玩家
		void GetTeamPlayers(std::vector<WSPlayer*>& players)
		{
			if(players.size() < QUICK_MAKE_FRIEND_NUM)
			{
				if (m_NotTeamPlayers.size() > QUICK_MAKE_FRIEND_NUM - players.size())
				{
					players.insert(players.end(), m_NotTeamPlayers.begin(), m_NotTeamPlayers.begin() + QUICK_MAKE_FRIEND_NUM - players.size());
				}
				else
				{
					players.insert(players.end(), m_NotTeamPlayers.begin(), m_NotTeamPlayers.end());
				}
			}

			std::random_shuffle(players.begin(), players.end());
			if(players.size() > QUICK_MAKE_FRIEND_NUM)
			{
				players.erase(players.begin() + QUICK_MAKE_FRIEND_NUM, players.end());
			}
		}

		// 获得推荐好友玩家
		void GetLevelPlayers(std::vector<WSPlayer*>& players)
		{
			players.insert(players.end(), m_LevelPlayers.begin(), m_LevelPlayers.end());

			if (players.size() < QUICK_MAKE_FRIEND_NUM)
			{
				if (m_OtherPlayers.size() > QUICK_MAKE_FRIEND_NUM - players.size())
				{
					players.insert(players.end(), m_OtherPlayers.begin(), m_OtherPlayers.begin() + QUICK_MAKE_FRIEND_NUM - players.size());
				}
				else
				{
					players.insert(players.end(), m_OtherPlayers.begin(), m_OtherPlayers.end());
				}
			}

			std::random_shuffle(players.begin(), players.end());
			if (players.size() > QUICK_MAKE_FRIEND_NUM)
			{
				players.erase(players.begin() + QUICK_MAKE_FRIEND_NUM, players.end());
			}
		}

		// 获得推荐师傅玩家
		void GetMasterPlayers(std::vector<WSPlayer*>& players)
		{
			/*auto masterItr = m_Master.begin();
			for (; masterItr != m_Master.end(); ++masterItr)
			{
				if (players.size() < QUICK_MAKE_FRIEND_NUM)
				{
					players.push_back(*masterItr);
				}
				else
				{
					break;
				}
			}

			std::random_shuffle(players.begin(), players.end());*/
			UInt8 recommanNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_RELATION_RECOMMAN_PLAYERNUM);
			GetRecommans(m_RecommanMasters, players, recommanNum);
			UInt64 curr_timestamp = Avalon::Time::CurrentTime().Sec();
			for (UInt32 i = 0; i < players.size(); i++)
			{
				if (players[i])
				{
					m_pPlayer->GetMasterSystem().SetRecommanMasterTimeStamp(players[i]->GetID(), curr_timestamp);
				}
			}
		}

		// 获得推荐徒弟玩家
		void GetDiscilpePlayers(std::vector<WSPlayer*>& players)
		{
			/*auto discipleItr = m_Disciple.begin();
			for (; discipleItr != m_Disciple.end(); ++discipleItr)
			{
				if (players.size() < QUICK_MAKE_FRIEND_NUM)
				{
					players.push_back(*discipleItr);
				}
				else
				{
					break;
				}
			}
			std::random_shuffle(players.begin(), players.end());*/
			
			UInt8 recommanNum = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_RELATION_RECOMMAN_PLAYERNUM);
			GetRecommans(m_RecommanDisciples, players, recommanNum);
			UInt64 curr_timestamp = Avalon::Time::CurrentTime().Sec();
			for (UInt32 i = 0; i < players.size(); i++)
			{
				if (players[i])
				{
					m_pPlayer->GetMasterSystem().SetRecommanDiscipleTimeStamp(players[i]->GetID(), curr_timestamp);
				}
			}
		}

		void GetRecommans(std::map<UInt8, PlayerVector, std::greater<UInt8>>& playerMap, std::vector<WSPlayer*>& players, UInt8 max)
		{
			//std::map<UInt8, UInt32> occNum;
			UInt8 num = 0;
			std::map<UInt8, PlayerVector, std::greater<UInt8>>::iterator iter;
			//std::vector<WSPlayer*> others;
			for (iter = playerMap.begin(); iter != playerMap.end(); iter++)
			{
				//UInt8 neednum = max - num;
				PlayerVector& player_vec = iter->second;
				/*if (player_vec.size() <= neednum)
				{
					players.insert(players.end(), player_vec.begin(), player_vec.end());
					num = num + player_vec.size();
				}
				else
				{
					std::random_shuffle(player_vec.begin(), player_vec.end());
					players.insert(players.end(), player_vec.begin(), player_vec.begin() + neednum);
					break;
				}
				if (num >= max)
				{
					break;
				}*/

				std::random_shuffle(player_vec.begin(), player_vec.end());
				for (UInt32 i = 0; i < player_vec.size(); i++)
				{
					WSPlayer* player = player_vec[i];
					UInt8 occ = player->GetOccu();
					if (occ == 33)
					{
						//others.push_back(player);
						continue;
					}
					players.push_back(player);
					++num;
					//occNum[occ] = occNum[occ] + 1;
					if (num >= max)
					{
						break;
					}
				}
				if (num >= max)
				{
					break;
				}
			}
			/*if (players.size() < max && others.size() > 0)
			{
				UInt32 sum = others.size() >= (max - players.size()) ? max - players.size() : others.size();
				players.insert(players.end(), others.begin(), others.begin() + sum);
			}*/
		}

	private:
		//玩家
		WSPlayer*	m_pPlayer;
		//队伍最小等级
		UInt16		m_TeamMinLevel;
		//组队功能开放等级
		UInt16		m_TeamFuncLevel;
		//好友功能开放等级
		UInt16		m_FriendFuncLevel;
		//师徒开放等级
		UInt16		m_MasterFuncLevel;
		//房间开放等级
		UInt16		m_RoomFuncLevel;

		//同城列表
		//std::vector<WSPlayer*>	m_SameCityPlayers;
		//同ip列表
		//std::vector<WSPlayer*>	m_SameIpPlayers1;
		//同ip列表
		//std::vector<WSPlayer*>	m_SameIpPlayers2;

		//同等级端
		std::vector<WSPlayer*>	m_LevelPlayers;
		//其他
		std::vector<WSPlayer*>	m_OtherPlayers;
		//非同队伍
		std::vector<WSPlayer*>	m_NotTeamPlayers;
		//没有房间的玩家
		std::vector<WSPlayer*>	m_NotRoomPlayers;
		//推荐师傅
		//std::set<WSPlayer*, MasterCompare>	m_Master;
		std::map<UInt8, PlayerVector, std::greater<UInt8>> m_RecommanMasters;
		//推荐徒弟
		//std::set<WSPlayer*, DiscipleCompare>	m_Disciple;
		std::map<UInt8, PlayerVector, std::greater<UInt8>> m_RecommanDisciples;

		//关系类型
		UInt8 m_relationType;

		//当前时间戳
		UInt64 m_curr_timestamp;
	};

	GetRecommandVisitor visitor(player, req.type);
	WSPlayerMgr::Instance()->VisitNormalPlayers(visitor);

	CLProtocol::WorldRelationFindPlayersRet ret;
	std::vector<WSPlayer*>	players;
	ret.type = req.type;
	if (RelationFindType::RFT_FRIEND == req.type)
	{
		visitor.GetLevelPlayers(players);
	}
	else if (RelationFindType::RFT_TEAM == req.type)
	{
		visitor.GetTeamPlayers(players);
	}
	else if (RelationFindType::RFT_MASTER == req.type)
	{
		visitor.GetMasterPlayers(players);
	}
	else if (RelationFindType::RFT_DISCIPLE == req.type)
	{
		visitor.GetDiscilpePlayers(players);
	}
	else if (RelationFindType::RFT_ROOM == req.type)
	{
		visitor.GetRoomPlayers(players);
	}
	else
	{
		ErrorLogStream << "req.type is error:" << req.type << LogStream::eos;
		return;
	}
	

	for(std::vector<WSPlayer*>::iterator iter = players.begin();
		iter != players.end(); ++iter)
	{
		WSPlayer* player = *iter;
		QuickFriendInfo info;
		info.playerId = player->GetID();
		info.name = player->GetName();
		info.occu = player->GetOccu();
		info.seasonLv = player->GetSeasonLv();
		info.level = player->GetLevel();
		info.vipLv = player->GetVipLvl();
		info.masterNote = player->GetRelationMgr().GetMasterNote();
		info.avatar = player->GetAvatar();
		info.activeTimeType = player->GetMasterSystem().GetQuestionnaire().activeTimeType;
		info.masterType = player->GetMasterSystem().GetQuestionnaire().masterType;
		info.regionId = player->GetMasterSystem().GetQuestionnaire().regionId;
		info.declaration = player->GetMasterSystem().GetQuestionnaire().declaration;
		player->GetPlayerLabelInfo(info.playerLabelInfo);
		ret.players.push_back(info);
	}
	player->SendProtocol(ret);
}


void HandleProtocol(WSPlayer* player, CLProtocol::WorldRelationQuickMakeFriends& req)
{
	if(req.players.size() > QUICK_MAKE_FRIEND_NUM) return;
	if(req.players.empty()) return;

	if(player->GetRelationMgr().IsFull(RELATION_FRIEND))
	{
		player->SendNotify(604);
		return;
	}

	if(player->GetSrcZoneId() != 0)
	{
		player->SendNotify(631);
		return;
	}

	for(std::vector<ObjID_t>::iterator iter = req.players.begin();
		iter != req.players.end(); ++iter)
	{
		WSPlayer* target = WSPlayerMgr::Instance()->FindNormalPlayer(*iter);
		if(target == NULL) continue;
		if(player->GetRelationMgr().FindRelation(RELATION_FRIEND, target->GetID()) != NULL) continue;
		//if(target->GetRelationMgr().IsFull(RELATION_FRIEND)) continue;

		player->GetRequestMgr().AddRequest(REQUEST_FRIEND, target->GetID());

		//向对方同步请求
		target->SyncRequest(REQUEST_QUICKFRIEND, player);
	}

	player->SendNotify(613);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldRelationSetClostFriend& req)
{
	WSRelation* relation = player->GetRelationMgr().FindRelation(RELATION_FRIEND, req.playerId);
	if(relation == NULL) return;

	if(req.flag == 1)
	{
		relation->SetCloseFriend(1);
	}
	else
	{
		relation->SetCloseFriend(0);
	}
}

void HandleProtocol(CLProtocol::WorldRelationAddIntimacy& req)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(req.id);
	if(player == NULL) return;

	player->AddIntimacy(req.friendId, req.intimacy);
}

void HandleProtocol(CLProtocol::SceneRelationFindPlayerRet& ret)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(ret.killer);
	if(player == NULL) return;

	WSScene* scene = WSSceneMgr::Instance()->FindScene(ret.sceneId);
	if(scene == NULL) return;
}

void HandleProtocol(CLProtocol::SceneRelationPresentToFriendReq& req)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(req.friendId);
	if(player == NULL)
	{
		CLProtocol::SceneRelationPresentToFriendRet ret;
		ret.playerId = req.playerId;
		ret.type = req.type;
		ret.gamestartDay = req.gamestartDay;
		ret.result = 1;
		req.SendBack(ret);
		return;
	}

	player->SendToScene(req);
}

void HandleProtocol(CLProtocol::SceneRelationPresentToFriendRet& req)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(req.playerId);
	if(player != NULL)
	{
		player->SendToScene(req);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldRelationPresentGiveReq& req)
{
	if (!player)
		return;
	
	DebugLogStream << "Present Give player id : " << player->GetID()
		<< ", friendId : " << req.friendID << LogStream::eos;

	CLProtocol::WorldRelationPresentGiveRes res;
	res.friendID = req.friendID;
	res.code = ErrorCode::SUCCESS;

	//判断开放等级
	auto functionData = FuncNotifyDataEntryMgr::Instance()->FindEntry(FUNC_FRIEND);
	if (functionData)
	{
		if (player->GetLevel() < functionData->endLevel)
		{
			res.code = ErrorCode::RETINUE_PLAYER_LEVEL;
			player->SendProtocol(res);
			DebugLogStream << "Present Give 1 player id : " << player->GetID()
				<< ", friendId : " << req.friendID << LogStream::eos;
			return;
		}
	}

	WSRelation* relation = NULL;
	WSRelation* masterRelation = player->GetRelationMgr().FindRelation(RELATION_MASTER, req.friendID);
	WSRelation* discipleRelation = player->GetRelationMgr().FindRelation(RELATION_DISCIPLE, req.friendID);
	if (masterRelation)
	{
		relation = masterRelation;
	}
	else if (discipleRelation)
	{
		relation = discipleRelation;
	}
	else
	{
		relation = player->GetRelationMgr().FindRelation(RELATION_FRIEND, req.friendID);
	}
	
	if (!relation)
	{
		//提示不是好友
		res.code = ErrorCode::RELATION_ERR_NOTFRIEND;
		player->SendProtocol(res);
		DebugLogStream << "Present Give 2 player id : " << player->GetID()
			<< ", friendId : " << req.friendID << LogStream::eos;
		return;
	}

	if (relation->GetDayGiftNum() == 0)
	{
		//提示每日限额到了
		res.code = ErrorCode::RELATION_PRESENTGIVE_ALREADYSEND;
		player->SendProtocol(res);
		DebugLogStream << "Present Give 3 player id : " << player->GetID()
			<< ", friendId : " << req.friendID << LogStream::eos;
		return;
	}

	WSPlayer* target = WSPlayerMgr::Instance()->FindPlayer(req.friendID);
	if (!target)
	{
		DebugLogStream << "Present Give target is not find player id : " << player->GetID() 
			<< ", friendId : " << req.friendID << LogStream::eos;
		if (WSApplication::Instance()->IsCloseOffPresentGive())
		{
			res.code = ErrorCode::RELATION_PRESENTGIVE_OFFLINE;
			player->SendProtocol(res);
			DebugLogStream << "Present Give 4 player id : " << player->GetID()
				<< ", friendId : " << req.friendID << LogStream::eos;
			return;
		}

		ObjID_t friendId = req.friendID;
		ObjID_t playerId = player->GetID();
		PlayerOfflineLoadCallBack playerOfflineLoadCb = [=](WSPlayer* friendPlayer)
		{
			DebugLogStream << "Present Give offline callback player id : " << playerId
				<< ", friendId : " << friendId << LogStream::eos;
			WSPlayer* wPlayer = WSPlayerMgr::Instance()->FindPlayer(playerId);
			if (!wPlayer)
			{
				DebugLogStream << "Present Give offline callback not find player player id : " << playerId
					<< ", friendId : " << friendId << LogStream::eos;
				return;
			}

			WSRelation* relation_ = NULL;
			WSRelation* masterRelation_ = wPlayer->GetRelationMgr().FindRelation(RELATION_MASTER, req.friendID);
			WSRelation* discipleRelation_ = wPlayer->GetRelationMgr().FindRelation(RELATION_DISCIPLE, req.friendID);
			if (masterRelation_)
			{
				relation_ = masterRelation_;
			}
			else if (discipleRelation_)
			{
				relation_ = discipleRelation_;
			}
			else
			{
				relation_ = wPlayer->GetRelationMgr().FindRelation(RELATION_FRIEND, req.friendID);
			}

			CLProtocol::WorldRelationPresentGiveRes res;
			res.friendID = friendId;
			res.code = ErrorCode::SUCCESS;

			if (!relation_)
			{
				//提示不是好友
				res.code = ErrorCode::RELATION_ERR_NOTFRIEND;
				wPlayer->SendProtocol(res);
				DebugLogStream << "Present Give offline callback 1 player id : " << playerId
					<< ", friendId : " << friendId << LogStream::eos;
				return;
			}

			wPlayer->HandlerFriendGive(friendPlayer, relation_);
			wPlayer->SendProtocol(res);
		};

		target = new WSPlayer();
		target->SetID(req.friendID);
		target->SetOfflineReplyCtx(player, LoadType::LT_FRIEND_PRSENT, playerOfflineLoadCb);
		WSPlayerMgr::Instance()->AddOfflinePlayerCache(target);
		target->LoadData();
		return;
	}

	player->HandlerFriendGive(target, relation);
	player->SendProtocol(res);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldUpdateRelation& req)
{
	if (!player)
	{
		return;
	}
	UpdateRelationDebugLog debugLog("HandleProtocol  WorldUpdateRelation");
	player->GetRelationMgr().UpdateRelation();
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldUpdatePlayerOnlineReq& req)
{
	if (!player)
	{
		return;
	}

	std::vector<CLProtocol::OnlineState> states;
	for (size_t i = 0; i < req.uids.size(); ++i)
	{
		CLProtocol::OnlineState state;
		state.uid = req.uids[i];
		WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(req.uids[i]);
		
		state.bOnline = player ? 1 : 0;
		
		states.push_back(state);
	}

	CLProtocol::WorldUpdatePlayerOnlineRes res;
	res.onlineStates = states;
	player->SendProtocol(res);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldSetMasterNoteReq& req)
{
	if (!player)
	{
		return;
	}

	CLProtocol::WorldSetMasterNoteRes respons;
	respons.code = ErrorCode::SUCCESS;

	char buff[MASTER_NOTE_MAX_BYTE];
	memset(buff, 0, sizeof(buff));
	avalon_strncpy(buff, req.note.c_str(), MASTER_NOTE_MAX_BYTE);

	if (ForbitWordParser::Instance()->Parse(buff) != FORBIT_WORD_UNCHANGED)
	{
		respons.code = 8927;
		player->SendProtocol(respons);
		return;
	}

	if (req.note.size() >= MASTER_NOTE_MAX_BYTE)
	{
		respons.code = ErrorCode::SET_MASTER_NOTE;
	}
	else
	{
		player->GetRelationMgr().SetMasterNote(req.note, true);
		respons.newNote = req.note;
	}

	player->SendProtocol(respons);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldSetMasterIsRecvDiscipleReq& req)
{
	if (!player)
	{
		return;
	}

	CLProtocol::WorldSetMasterIsRecvDiscipleRes respons;
	player->GetRelationMgr().SetIsRecvDisciple(req.isRecv, true);
	respons.code = ErrorCode::SUCCESS;
	respons.isRecv = req.isRecv;
	player->SendProtocol(respons);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldQueryMasterSettingReq& req)
{
	if (!player)
	{
		return;
	}

	CLProtocol::WorldQueryMasterSettingRes respons;
	respons.isRecv = player->GetRelationMgr().GetIsRecvDisciple();
	respons.masterNote = player->GetRelationMgr().GetMasterNote();

	player->SendProtocol(respons);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAddonPayReq& req)
{
	if (!player)
	{
		return;
	}

	// 检查是否请求师傅代付
	WSRelation* relation = player->GetRelationMgr().FindRelation(RelationType::RELATION_MASTER, req.tarId);
	if (!relation)
	{
		return;
	}

	// 检查商品有效性
	MallItemInfo* mallItem = MallMgr::Instance()->GetMallItemInfo(req.goodId);
	if (!mallItem)
	{
		return;
	}

	// 生成代付订单
	AddonPayOrderData* order = new AddonPayOrderData;
	order->uid = CLRecordClient::Instance()->GenGuid();
	order->relationType = RelationType::RELATION_MASTER;
	order->reqId = player->GetID();
	order->reqName = player->GetName();
	order->reqOccu = player->GetOccu();
	order->reqLevel = player->GetLevel();
	order->tarId = req.tarId;
	order->tarName = req.tarName;
	order->tarOccu = req.tarOccu;
	order->tarLv = req.tarLevel;
	order->dueTime = Avalon::Time::CurrentTime().Sec() + SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_TAP_SYSTEM_ADDONPAY_OVERDUE_TIME);
	order->state = AddonState::AS_UNDO;
	order->goodId = req.goodId;
	order->buyNum = 1;

	AddonPayOrderMgr::Instance()->AddOrder(order);
	
	// 同步给自己
	player->GetRelationMgr().SyncAddonPayData(order);
	
	// 同步给目标玩家
	WSPlayer* tarPlayer = WSPlayerMgr::Instance()->FindNormalPlayer(req.tarId);
	if (tarPlayer)
	{
		tarPlayer->GetRelationMgr().SyncAddonPayData(order);
	}

}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAddonPayReply& req)
{
	if (!player)
	{
		return;
	}

	AddonPayOrderData* order = AddonPayOrderMgr::Instance()->GetOrder(req.orderId);
	if (!order)
	{
		return;
	}


}

void HandleProtocol(CLProtocol::WorldCheckMasterGive& req)
{
	CLProtocol::WorldCheckMasterGiveRes response;
	response.masterId = req.masterId;
	response.disciple = req.disciple;
	response.items = req.items;

	WSPlayer* master = WSPlayerMgr::Instance()->FindNormalPlayer(req.masterId);
	if (!master)
	{
		return;
	}

	WSRelation* relation = master->GetRelationMgr().FindRelation(RelationType::RELATION_DISCIPLE, req.disciple);
	if (!relation)
	{
		return;
	}

	if (relation->GetMasterDayGiftNum() < req.items.size())
	{
		master->SendNotify(8908);
		return;
	}

	relation->SetMasterDayGiftNum(relation->GetMasterDayGiftNum() - req.items.size());
	Int32 curH = CURRENT_DATE.Hour();
	Avalon::Date resetDate;
	if (curH < FRIEND_DAY_GIFT)
	{
		resetDate = CURRENT_DATE;
		resetDate.Hour(FRIEND_DAY_GIFT);
		resetDate.Min(0);
		resetDate.Sec(0);
		resetDate.MSec(0);
	}
	else
	{
		Avalon::Time resetTime(CURRENT_TIME.MSec() + DAY_TO_SEC * SEC_TO_MSEC);
		resetDate = Avalon::Date(resetTime);
		resetDate.Hour(FRIEND_DAY_GIFT);
		resetDate.Min(0);
		resetDate.Sec(0);
		resetDate.MSec(0);
	}
	relation->SetMasterGiveResetTime(UInt32(resetDate.ToTime().Sec()));
	UpdateRelationDebugLog debugLog("HandleProtocol WorldCheckMasterGive");
	master->GetRelationMgr().UpdateRelation();
	relation->SyncToDB();

	master->SendToScene(response);
	return;
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldRelationReportCheat& req)
{
	InfoLogStream << "player(" << player->GetID() << ") report self cheat, reason: modify memory." << LogStream::eos;

	// 上报作弊情况
	PunishmentMgr::Instance()->OnReportPlayerCheat(player, PLAYER_CHEAT_REASON_MODIFY_MEMORY);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldSetPlayerRemarkReq& req)
{
	if (!player) return;

	CLProtocol::WorldSetPlayerRemarkRes res;

	if (req.remark.size() == 0)
	{
		res.code = player->GetRelationMgr().SetPlayerRemark(req.roleId, req.remark);
		player->SendProtocol(res);
		return;
	}

	if (GetUtf8ShowWidth(req.remark.c_str()) > PLAYER_REMARK_MAXLEN)
	{
		res.code = ErrorCode::RELATION_SETREMARK_LEN_ERROR;
		player->SendProtocol(res);
		return;
	}

	if (!CheckNameValid(req.remark.c_str(), PLAYER_REMARK_MAXLEN))
	{
		res.code = ErrorCode::RELATION_SETREMARK_UNVALID_NAME;
		player->SendProtocol(res);
		return;
	}

	char remarkbuf[NAME_LENGTH + 1];
	memset(remarkbuf, 0, sizeof(remarkbuf));
	avalon_strncpy(remarkbuf, req.remark.c_str(), NAME_LENGTH);
	if (ForbitWordParser::Instance()->Parse(remarkbuf) != FORBIT_WORD_UNCHANGED)
	{
		res.code = ErrorCode::RELATION_SETREMARK_UNVALID_NAME;
		player->SendProtocol(res);
		return;
	}

	res.code =player->GetRelationMgr().SetPlayerRemark(req.roleId, req.remark);

	player->SendProtocol(res);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldRelationAnnounceReq& req)
{
	if (!player) return;

	UInt32 curTimeStamp = player->GetMasterSystem().GetAnnounceProhibitTimeStamp();
	if (curTimeStamp > (UInt32)CURRENT_TIME.Sec())
	{
		player->SendNotify(9908);
		return;
	}

	std::string playerLinkStr;
	UInt32 announcementId = 0;
	std::string systemLinkStr;
	SystemSuffixesType sysType = SystemSuffixesType::SYSTEM_SUFFIXES_TYPE_INVALID;
	std::string announcement = player->GetMasterSystem().GetQuestionnaire().declaration;
	if (!MsgPackage::GetPlayerMsgTag(playerLinkStr, player->GetID(), player->GetName(), player->GetOccu(), player->GetLevel()))
	{
		playerLinkStr = player->GetName();
	}

	if (req.type == RelationAnnounceType::RAT_MASTER)
	{
		announcementId = 79;
		sysType = SystemSuffixesType::SYSTEM_SUFFIXES_TYPE_MASTER;
	}
	else if (req.type == RelationAnnounceType::RAT_DISCIPLE)
	{
		announcementId = 80;
		sysType = SystemSuffixesType::SYSTEM_SUFFIXES_TYPE_DISCIPLE;
	}
	MsgPackage::GetSystemSuffixesMsgTag64(systemLinkStr, sysType, player->GetID());
	AnnouncementMgr::Instance()->PublicAnnouncement(0,announcementId, (UInt32)CURRENT_TIME.Sec(), playerLinkStr.c_str(), announcement.c_str(), systemLinkStr.c_str());
	UInt32 span = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_MASTERSYS_ANNOUNCE_SPAN);

	UInt32 newTimeStamp = (UInt32)CURRENT_TIME.Sec() + span;
	player->GetMasterSystem().SetAnnounceProhibitTimeStamp(newTimeStamp);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldSetDiscipleQuestionnaireReq& req)
{
	CLProtocol::WorldSetDiscipleQuestionnaireRes res;
	res.code = ErrorCode::SUCCESS;
	if (req.activeTimeType < QAT_ALLDAY || req.activeTimeType > QAT_NIGHT)
	{
		res.code = ErrorCode::ITEM_DATA_INVALID;
	}
	if (req.masterType < QMT_STRENGTH || req.masterType > QMT_CHITCHAT)
	{
		res.code = ErrorCode::ITEM_DATA_INVALID;
	}

	if (res.code == ErrorCode::SUCCESS && GetUtf8ShowWidth(req.declaration.c_str()) > QUSETIONNAIRE_DECLAR_MAXLEN)
	{
		res.code = ErrorCode::MASTERSYS_SET_QUEST_DECL_LEN_ERROR;
	}

	if (res.code == ErrorCode::SUCCESS)
	{
		char declarationlbuf[QUSETIONNAIRE_DECLAR_MAXLEN + 1];
		avalon_strncpy(declarationlbuf, req.declaration.c_str(), QUSETIONNAIRE_DECLAR_MAXLEN);
		if (ForbitWordParser::Instance()->Parse(declarationlbuf) != FORBIT_WORD_UNCHANGED)
		{
			res.code = ErrorCode::MASTERSYS_SET_QUEST_DECL_FORBID_ERROR;
		}
	}

	if (res.code == ErrorCode::SUCCESS)
	{
		MsaterDiscipleQuestionnaire& questionnaire = player->GetMasterSystem().GetQuestionnaire();
		if (questionnaire.SetDataFromClient(req))
		{
			questionnaire.SynToClient(player);
			questionnaire.SynToDB(player);
		}
	}
	player->SendProtocol(res);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldSetMasterQuestionnaireReq& req)
{
	CLProtocol::WorldSetMasterQuestionnaireRes res;
	res.code = ErrorCode::SUCCESS;
	if (req.activeTimeType < QAT_ALLDAY || req.activeTimeType > QAT_NIGHT)
	{
		res.code = ErrorCode::ITEM_DATA_INVALID;
	}
	if (req.masterType < QMT_STRENGTH || req.masterType > QMT_CHITCHAT)
	{
		res.code = ErrorCode::ITEM_DATA_INVALID;
	}

	if (res.code == ErrorCode::SUCCESS && GetUtf8ShowWidth(req.declaration.c_str()) > QUSETIONNAIRE_DECLAR_MAXLEN)
	{
		res.code = ErrorCode::MASTERSYS_SET_QUEST_DECL_LEN_ERROR;
	}

	if (res.code == ErrorCode::SUCCESS)
	{
		char declarationlbuf[QUSETIONNAIRE_DECLAR_MAXLEN + 1];
		avalon_strncpy(declarationlbuf, req.declaration.c_str(), QUSETIONNAIRE_DECLAR_MAXLEN);
		if (ForbitWordParser::Instance()->Parse(declarationlbuf) != FORBIT_WORD_UNCHANGED)
		{
			res.code = ErrorCode::MASTERSYS_SET_QUEST_DECL_FORBID_ERROR;
		}
	}

	if (res.code == ErrorCode::SUCCESS)
	{
		MsaterDiscipleQuestionnaire& questionnaire = player->GetMasterSystem().GetQuestionnaire();
		if (questionnaire.SetDataFromClient(req))
		{
			questionnaire.SynToClient(player);
			questionnaire.SynToDB(player);
		}
	}
	player->SendProtocol(res);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldUpdateMasterSectRelationReq& req)
{
	if (!player)
	{
		return;
	}

	player->GetMasterSystem().UpdateRelation();
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldDiscipleFinishSchoolReq& req)
{
	if (!player)
	{
		return;
	}

	if (!WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_MASTER))
	{
		player->SendNotify(9927);
		return;
	}

	CLProtocol::WorldDiscipleFinishSchoolRes res;
	res.code = ErrorCode::SUCCESS;
	//出师条件检查
	//是否是徒弟
	if (!player->GetRelationMgr().FindRelation(RELATION_DISCIPLE, req.discipleId))
	{
		res.code = ErrorCode::FISNISH_SCHOOL_NOTDISCIPLE;
		player->SendProtocol(res);
		return;
	}
	//徒弟是否在线
	WSPlayer* disciple = WSPlayerMgr::Instance()->FindNormalPlayer(req.discipleId);
	if (disciple == NULL)
	{
		res.code = ErrorCode::RELATION_DISCIPLE_NOTONLINE;
		player->SendProtocol(res);
		return;
	}

	//检查队伍, 是否在一个队伍里面,且师傅是队长
	WSTeam* team = player->GetTeam();
	/*UInt32 num = team->GetSize();
	ObjID_t masterId = team->GetMaster();
	ObjID_t selfId = player->GetID();
	bool findMember = team->FindMember(req.discipleId);*/
	if (!team || team->GetSize() != 2
		||	team->GetMaster() != player->GetID()
		||  !team->FindMember(req.discipleId))
	{
		res.code = ErrorCode::FISNISH_SCHOOL_TEAMERR;
	}

	//检查徒弟等级
	if (res.code == ErrorCode::SUCCESS)
	{
		if (disciple->GetLevel() < SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_RELATION_FINISHLEVEL))
		{
			res.code = ErrorCode::FISNISH_SCHOOL_DISCIPLE_LEVELINSUFFIC;
		}
	}

	if (res.code == ErrorCode::SUCCESS)
	{
		WSMasterSystem::HandleDiscipleFinishSchool(player, disciple);
	}

	player->SendProtocol(res);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldPublishMasterDialyTaskReq& req)
{
	if (!WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_MASTER))
	{
		player->SendNotify(9927);
		return;
	}

	CLProtocol::WorldPublishMasterDialyTaskRes res;
	res.discipleId = req.discipleId;
	res.code = ErrorCode::SUCCESS;

	if (WSApplication::Instance()->IsCloseMasterOffPubTask())
	{
		WSPlayer* disciple = WSPlayerMgr::Instance()->FindNormalPlayer(req.discipleId);
		if (disciple == NULL)
		{
			res.code = ErrorCode::RELATION_DISCIPLE_NOTONLINE;
			player->SendProtocol(res);
			return;
		}
	}
	InfoLogStream << "master publish master task masterId=" << player->GetID() << " discipleId=" << req.discipleId << LogStream::eos;

	if (player->CheckPublishTaskFlag(req.discipleId))
	{
		res.code = ErrorCode::MASTER_PUBlISHTASK_FAIL;
		player->SendProtocol(res);
		return;
	}

	WSRelation* relation = player->GetRelationMgr().FindRelation(RELATION_DISCIPLE, req.discipleId);
	if (!relation)
	{
		res.code = ErrorCode::MASTER_PUBlISHTASK_FAIL;
		player->SendProtocol(res);
		return;
	}

	if (relation->GetDailyMasterTaskState() == MasterDailyTaskState::MDTST_UNCOMPLETE
		|| relation->GetDailyMasterTaskState() == MasterDailyTaskState::MDTST_RECEIVEED)
	{
		res.code = ErrorCode::MASTER_PUBlISHTASK_FAIL_PUBISHED;
		player->SendProtocol(res);
		return;
	}
	if (relation->GetDailyMasterTaskState() == MasterDailyTaskState::MDTST_UNRECEIVED
		|| relation->GetDailyMasterTaskState() == MasterDailyTaskState::MDTST_UNPUB_UNRECE)
	{
		res.code = ErrorCode::MASTER_PUBlISHTASK_FAIL_UNREIEVE;
		player->SendProtocol(res);
		return;
	}

	InfoLogStream << "master publish master check state success masterId=" << player->GetID() << " discipleId=" << req.discipleId << "state=" << relation->GetDailyMasterTaskState() << LogStream::eos;

	//转发给徒弟所在的scene
	CLProtocol::WorldNotifyPublishMasterTaskToScene notify;
	notify.masterId = player->GetID();
	notify.discipleId = req.discipleId;

	Router::BroadcastToScene(notify);

	player->SetPubishTasFlag(req.discipleId);
}

void HandleProtocol(CLProtocol::SceneNotifyPublishMasterTaskResToWorld& protocol)
{
	CLProtocol::WorldPublishMasterDialyTaskRes res;
	res.code = protocol.code;
	res.discipleId = protocol.discipleId;

	if (protocol.code == ErrorCode::SUCCESS)
	{
		InfoLogStream << "master publish master task success masterId=" << protocol.masterId << " discipleId=" << protocol.discipleId << LogStream::eos;
		WSMasterSystem::HandlePublishMasterTaskSuccess(protocol.masterId, protocol.discipleId);
	}
	else
	{
		InfoLogStream << "master publish master task fail masterId=" << protocol.masterId << " discipleId=" << protocol.discipleId << "fail code=" << protocol.code << LogStream::eos;
	}

	WSPlayer* master = WSPlayerMgr::Instance()->FindPlayer(protocol.masterId);
	if (master)
	{
		master->SendProtocol(res);
		master->ResetPubishTasFlag(protocol.discipleId);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldGetDiscipleMasterTasksReq& req)
{
	if (!player) return;
	
	if (!WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_MASTER))
	{
		return;
	}

	WSPlayer* disciple = WSPlayerMgr::Instance()->FindNormalPlayer(req.discipleId);
	if (disciple) //在线
	{
		CLProtocol::WorldGetDiscipleMasterTasksToSceneReq  notify;
		notify.masterId = player->GetID();
		notify.discipleId = req.discipleId;
		disciple->SendToScene(notify);
	}
	else //不在线,从离线缓存找
	{
		WSPlayerShareData* shareData = WSPlayerShareDataMgr::Instance()->FindPlayer(req.discipleId);
		if (shareData == NULL)
		{
			WSPlayerShareDataMgr::Instance()->LoadOfflinePlayerCb(req.discipleId, 0, new ShareDataLoadingForGetDiscipleMasterTasks(player->GetID(), req));
		}
		else if (shareData->IsSceneDataLoaded())
		{
			CLProtocol::WorldGetDiscipleMasterTasksRes res;
			res.code = ErrorCode::SUCCESS;
			res.discipleId = req.discipleId;
			shareData->GetMasterTaskShareData(res.masterTaskShareData);
			player->SendProtocol(res);
		}
		else if (shareData->IsInLoading())
		{
			shareData->AddLoadingCallback(new ShareDataLoadingForGetDiscipleMasterTasks(player->GetID(), req));
		}
		else if (shareData->GetStatus() == PSDS_INIT)
		{
			shareData->StartLoadData(new ShareDataLoadingForGetDiscipleMasterTasks(player->GetID(), req));
		}
		else
		{
		}
	}
}

void HandleProtocol(CLProtocol::WorldGetDiscipleMasterTasksToSceneRes& protocol)
{
	CLProtocol::WorldGetDiscipleMasterTasksRes  notify;
	
	notify.discipleId = protocol.discipleId;

	WSPlayer* master = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.masterId);
	if (master == NULL) return;

	if (protocol.code == ErrorCode::SUCCESS)
	{
		notify.code = protocol.code;
		notify.masterTaskShareData = protocol.masterTaskShareData;
		master->SendProtocol(notify);
	}
	else if (protocol.code == ErrorCode::GETDISCIPLE_MASTERTASKS_FAIL)
	{
		WSPlayerShareData* shareData = WSPlayerShareDataMgr::Instance()->FindPlayer(protocol.discipleId);
		if (shareData)
		{
			notify.code = ErrorCode::SUCCESS;
			shareData->GetMasterTaskShareData(protocol.masterTaskShareData);
			master->SendProtocol(notify);
		}
		else
		{
			notify.code = protocol.code;
			master->SendProtocol(notify);
		}
	}
}

void HandleProtocol(CLProtocol::SceneSyncMasterTaskShareDataToWorld& protocol)
{
	WSPlayerShareData* shareData = WSPlayerShareDataMgr::Instance()->FindOrCreatePlayer(protocol.roleId);
	if (shareData)
	{
		shareData->SyncMasterTaskDataFromScene(protocol.masterTaskShareData);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldReportMasterDailyTaskReq& req)
{
	if (NULL == player) return;

	if (!WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_MASTER))
	{
		player->SendNotify(9927);
		return;
	}

	CLProtocol::WorldReportMasterDailyTaskRes res;
	res.code = ErrorCode::SUCCESS;

	WSRelation* relation = player->GetRelationMgr().FindRelation(RELATION_MASTER, req.masterId);
	if (!relation)
	{
		res.code = ErrorCode::MASTER_DAILYTASK_REPORTFAIIL;
		player->SendProtocol(res);
		InfoLogStream << "report master daily task fail, not find master displeId:" << player->GetID() << ",  masterId:" << req.masterId << LogStream::eos;
		return;
	}

	InfoLogStream << "report master daily task, relation Id:" << relation->GetGuid() << ",  player Id:" << player->GetID() << ", master Id : " << req.masterId << LogStream::eos;

	//检查师门日常任务是否完成
	CLProtocol::WorldCheckMasterDailyTaskReqToScene protocol;
	protocol.discipleId = player->GetID();
	player->SendToScene(protocol);
}

void HandleProtocol(CLProtocol::SceneCheckMasterDailyTaskResToWorld& protocol)
{
	WSPlayer* disciple = WSPlayerMgr::Instance()->FindPlayer(protocol.discipleId);
	if (disciple == NULL)
	{
		InfoLogStream << "Not find disciple id : " << protocol.discipleId << LogStream::eos;
		return;
	}

	InfoLogStream << "discipleId : " << protocol.discipleId << ", completed : " << protocol.completed << LogStream::eos;

	CLProtocol::WorldReportMasterDailyTaskRes res;
	res.code = ErrorCode::SUCCESS;
	if (protocol.completed == MDTCS_COMPLTETEED)
	{
		WSRelation* relation = disciple->GetRelationMgr().GetRelation((UInt8)RELATION_MASTER);
		if (relation)
		{
			relation->SetDailyMasterTaskState(MDTST_UNRECEIVED);
			InfoLogStream << "set relation id : " << relation->GetGuid() << ", daily_mastertask_state = " << MDTST_UNRECEIVED << LogStream::eos;
			UpdateRelationDebugLog debugLog("HandleProtocol SceneCheckMasterDailyTaskResToWorld 1");
			disciple->GetRelationMgr().UpdateRelation();
			if (disciple->GetGameStatus() != PS_NORMAL)
			{
				relation->SyncToDB();
			}
			WSPlayer* master = WSPlayerMgr::Instance()->FindPlayer(relation->GetID());
			if (master)
			{
				WSRelation* relation_disciple = master->GetRelationMgr().FindRelation(RELATION_DISCIPLE, disciple->GetID());
				if (relation_disciple)
				{
					relation_disciple->SetDailyMasterTaskState(MDTST_UNRECEIVED);
					InfoLogStream << "set relation id : " << relation_disciple->GetGuid() << ", daily_mastertask_state = " << MDTST_UNRECEIVED << LogStream::eos;
					UpdateRelationDebugLog debugLog("HandleProtocol SceneCheckMasterDailyTaskResToWorld 2");
					master->GetRelationMgr().UpdateRelation();
					if (master->GetGameStatus() != PS_NORMAL)
					{
						relation_disciple->SyncToDB();
					}
				}
			}
			else
			{
				WSRelationMgr::SyncMasterDailyTaskState(relation->GetID(), disciple->GetID(), RELATION_DISCIPLE, MDTST_UNRECEIVED);
				InfoLogStream << "set relation owner : " << relation->GetID() << ", id = " << disciple->GetID() << ", daily_mastertask_state = " << MDTST_UNRECEIVED << LogStream::eos;
			}
		}
	}
	else if (protocol.completed == MDTCS_NOTCOMPLTETEED)
	{
		res.code = ErrorCode::REPORT_DAILYTASK_FAIL_NOTCOMPLETE;
	}
	disciple->SendProtocol(res);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldReceiveMasterDailyTaskRewardReq& req)
{
	if (NULL == player) return;

	CLProtocol::WorldReceiveMasterDailyTaskRewardRes res;
	res.code = player->GetMasterSystem().OnReceiveMasterDailyTaskReward(req.type, req.discipeleId);

	if (res.code != ErrorCode::SUCCESS)
	{
		player->SendProtocol(res);
	}
}

void HandleProtocol(CLProtocol::SceneNotifyGiveMasterSysGiftResToWorld& protocol)
{
	InfoLogStream << "code : " << protocol.code << ", masterId : " << protocol.masterId << ", discipleId : " << protocol.discipleId << ". type : " << protocol.type
		<< ", giftDataId : " << protocol.giftDataId << LogStream::eos;
	WSPlayer* player = NULL;
	if (protocol.type == MasterSysGiftType::MSGT_MASTER_DAILYTASK
		|| protocol.type == MasterSysGiftType::MSGT_FISNISHCL_MASTER)
	{
		player = WSPlayerMgr::Instance()->FindPlayer(protocol.masterId);
	}
	else if (protocol.type == MasterSysGiftType::MSGT_DISCIPLE_DAILYTASK
		|| protocol.type == MasterSysGiftType::MSGT_DISCIPLE_ACADEMIC
		|| protocol.type == MasterSysGiftType::MSGT_FISNISHCL_DISCIPLE)
	{
		player = WSPlayerMgr::Instance()->FindPlayer(protocol.discipleId);
	}

	if (player)
	{
		CLProtocol::WorldReceiveMasterDailyTaskRewardRes res;
		res.code = protocol.code;
		if (protocol.code == ErrorCode::SUCCESS)
		{
			if (protocol.type == MasterSysGiftType::MSGT_DISCIPLE_DAILYTASK || protocol.type == MasterSysGiftType::MSGT_MASTER_DAILYTASK)
			{
				player->GetMasterSystem().OnRceiveMasterDailyTaskRewardSucc(protocol.type, protocol.discipleId);
			}
			else if (protocol.type == MasterSysGiftType::MSGT_DISCIPLE_ACADEMIC)
			{
				player->GetMasterSystem().OnRceiveMasterAcademicRewardSucc(protocol.giftDataId);
			}
			else if (protocol.type == MasterSysGiftType::MSGT_FISNISHCL_MASTER)
			{
				player->GetMasterSystem().OnReceiveMasterFinishSchoolRewardSucc(protocol.giftDataId, protocol.discipleId);
			}
			else if (protocol.type == MasterSysGiftType::MSGT_FISNISHCL_DISCIPLE)
			{
				player->GetMasterSystem().OnReceiveDiscipleFinishSchoolRewardSucc(protocol.giftDataId, protocol.masterId);
			}
		}

		if (protocol.type == MasterSysGiftType::MSGT_MASTER_DAILYTASK)
		{
			player->GetMasterSystem().ResetRecvingMasterDailyTaskRewardState();
		}
		else if (protocol.type == MasterSysGiftType::MSGT_DISCIPLE_DAILYTASK)
		{
			player->GetMasterSystem().ResetRecvingDiscipleDailyTaskRewardState();
		}

		player->SendProtocol(res);
	}
	else
	{
		InfoLogStream << "not find player" << LogStream::eos;
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldReceiveMasterAcademicTaskRewardReq& req)
{
	if (NULL == player) return;

	CLProtocol::WorldReceiveMasterAcademicTaskRewardRes res;

	res.code = player->GetMasterSystem().OnReceiveMasterAcademicReward(req.giftDataId);
	
	if (res.code != ErrorCode::SUCCESS)
	{
		player->SendProtocol(res);
	}
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldAutomaticFinishSchoolReq& req)
{
	if (NULL == player) return;
	if (!WSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_MASTER))
	{
		player->SendNotify(9927);
		return;
	}
    player->GetMasterSystem().OnAutomaticFinishSchool(req.targetId);
	
}

void HandleProtocol(CLProtocol::SceneSyncMasterSysDataToWorld& protocol)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(protocol.m_roleId);
	if (player)
	{
		player->GetMasterSystem().SetAcademicTotalGrowth(protocol.m_AcademicTotalGrowth);
		player->GetMasterSystem().SetGoodTeacherValue(protocol.m_GoodTeacherValue);
	}
	
	WSPlayerShareData* shareData = WSPlayerShareDataMgr::Instance()->FindPlayer(protocol.m_roleId);
	if (shareData)
	{
		shareData->SetMasterGiveEquipGrowthValue(protocol.m_MasterGiveEquipGrowth);
		shareData->SetDiscipleGrowthValue(protocol.m_AcademicTotalGrowth);
		shareData->SetMasterSysGoodTeacherValue(protocol.m_GoodTeacherValue);
	}
}

void HandleProtocol(CLProtocol::SceneSyncMasterDailyTaskComp& protocol)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(protocol.roleId);
	if (player)
	{
		player->GetMasterSystem().OnMasterDailyTaskComplete();
	}
}
