#include "WSEnterGameHandler.h"

#include <AvalonLogger.h>
#include <CLErrorCode.h>
#include <CLSceneDataEntry.h>
#include <CLLeaveGameProtocol.h>

#include "WSPlayerMgr.h"
#include "WSSceneMgr.h"
#include "AsyncMsgCallbackMgr.h"
#include "AccountMgr.h"
#include "LoginPushMgr.h"

void HandleProtocol(CLProtocol::WorldVerifyUniqueReq& protocol)
{
    DebugLogStream << "player(" << protocol.accid << ") verify unique req." << LogStream::eos;

	CLProtocol::WorldVerifyUniqueRet ret;
	ret.accid = protocol.accid;
	ret.needWait = 0;
	WSPlayer* player = WSPlayerMgr::Instance()->FindPlayerByAccid(protocol.accid);
	if(player != NULL)
	{
		if(player->GetGateID() != protocol.GetConnID())
		{
			player->KickOff(ErrorCode::LOGIN_REPEAT);
		}
	}
	else
	{
		// 检查下看看需不需要排队
		if (protocol.checkWaitQueue && WSPlayerMgr::Instance()->IsNeedWaitOhters())
		{
			ret.needWait = 1;
			ret.waitPlayerNum = WSPlayerMgr::Instance()->AddPlayerToWaitList(protocol.accid);
		}
	}

	static UInt32 sequence = 0;
	ret.SetSequence(++sequence);
	ret.serverStartTime = GAMESTART_TIME;
	LoginPushMgr::Instance()->GetLoginPushInfos(ret.infos);

	DebugLogStream << "player(" << protocol.accid << ") send verify unique response(" << ret.GetSequence() << ")." << LogStream::eos;
	protocol.SendBack(ret);
}

void HandleProtocol(CLProtocol::SceneEnterGameReq& protocol)
{
    DebugLogStream << "account(" << protocol.accid << ") player(" << protocol.id << ") recv enter game request." << LogStream::eos;

	WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(protocol.id);
    if (player && player->GetGameStatus() == PS_LEAVEGAME)
    {
        InfoLogStream << "account(" << protocol.accid << ") player(" << protocol.id
            << ") start quick reconnect to scene(" << player->GetSceneNode() << ")." << LogStream::eos;

        CLProtocol::SceneGameReconnectReq req;
        req.accid = protocol.accid;
        req.roleId = protocol.id;
        req.gatenode = protocol.gatenode;

        player->SendToScene(req);
        return;
    }

    if (player != NULL && player->GetGameStatus() != PS_FINAL)
    {
        DebugLogStream << "player(" << protocol.id << ") state(" << player->GetGameStatus() << ")." << LogStream::eos;
        return;
    }

    auto accPlayer = WSPlayerMgr::Instance()->FindPlayerByAccid(protocol.accid);
    if (accPlayer != NULL)
    {
        // 同一账号下已经有角色在线了
        if (accPlayer->GetGameStatus() == PS_INIT || accPlayer->GetGameStatus() == PS_NORMAL)
        {
            DebugLogStream << "player(" << protocol.accid << ") is online." << LogStream::eos;
            return;
        }
        
        // 上一个角色已经离线，但是未摧毁
        // 直接走离线流程
        if (accPlayer->GetID() != protocol.id)
        {
			DebugLogStream << "player(" << protocol.accid << ") want to login other role(" << protocol.id 
							<< ") last role(" << accPlayer->GetID() << ") offline." << LogStream::eos;

            accPlayer->OnOffline();

            CLProtocol::SceneLeaveGameReq leaveNotify;
            leaveNotify.id = accPlayer->GetID();
			leaveNotify.forceLeave = 1;
            accPlayer->SendToScene(leaveNotify);

            WSPlayerMgr::Instance()->RemovePlayer(accPlayer);
            CL_SAFE_DELETE(accPlayer);
        }
    }

	if(player != NULL)
	{
		WSScene* scene = WSSceneMgr::Instance()->FindScene(player->GetOfflineScene());
		if(scene != NULL 
			&& (IsStaticMap(scene->GetType()) || scene->GetType() == SCENE_TYPE_PUBLICCOPY))
		{
			protocol.sceneid = player->GetOfflineScene();
			protocol.pos = player->GetOfflinePos();
			scene->SendToScene(protocol);

			DebugLogStream << "player(" << protocol.id << ") login to offline scene(" << protocol.sceneid << ")." << LogStream::eos;

			return;
		}
	}

	if(protocol.sceneid == 0) protocol.sceneid = protocol.mapid;
	WSScene* scene = WSSceneMgr::Instance()->FindScene(protocol.sceneid);
	if(scene != NULL && IsStaticMap(scene->GetType()))
	{
		DebugLogStream << "player(" << protocol.id << ") login to scene(" << protocol.sceneid << ")." << LogStream::eos;
		scene->SendToScene(protocol);
		return;
	}

	SceneDataEntry* dataEntry = SceneDataEntryMgr::Instance()->FindEntry(protocol.mapid);
	if(dataEntry == NULL)
	{
		UInt32 initMap = 0;
		std::string initPos;
		initMap = WSPlayerMgr::Instance()->GetPlayerAttr()->GetProperty("PlayerInitInfo","scene");
		initPos = WSPlayerMgr::Instance()->GetPlayerAttr()->GetProperty("PlayerInitInfo","pos").GetString();

		WSScene* scene = WSSceneMgr::Instance()->FindScene(initMap);
		if(scene != NULL)
		{
			protocol.sceneid = scene->GetID();
			protocol.mapid = scene->GetMapID();
			protocol.pos.FromString(initPos);		

			DebugLogStream << "player(" << protocol.id << ") login to scene(" << protocol.sceneid << ")." << LogStream::eos;

			scene->SendToScene(protocol);
		}
		else
		{
			ErrorLogStream << "player(" << protocol.id << ") login failed for not find scene!" << LogStream::eos;
		}
		return;
	}

	//回到主城
	UInt32 majorCityId = dataEntry->birthCity;
	dataEntry = SceneDataEntryMgr::Instance()->FindEntry(majorCityId);
	if (dataEntry == NULL)
	{
		ErrorLogStream << "player(" << protocol.accid << ", " << protocol.id << ") can't find scene(" 
						<< protocol.mapid << ")'s birth city(" << majorCityId << ")." << LogStream::eos;
		return;
	}

	WSScene* majorCity = WSSceneMgr::Instance()->FindScene(majorCityId);
	if(dataEntry == NULL || majorCity == NULL)
	{
		GameErrorLogStream("Player") << "player(," << protocol.id << ",) login failed for not find scene!" << LogStream::eos;
		return;
	}

	protocol.sceneid = UInt32(majorCity->GetID());
	protocol.mapid = majorCity->GetMapID();

	DebugLogStream << "player(" << protocol.id << ") login to scene(" << protocol.sceneid << ")." << LogStream::eos;

	majorCity->SendToScene(protocol);
}

void HandleProtocol(CLProtocol::WorldPlayerLeaveLoginQueue& protocol)
{
	WSPlayerMgr::Instance()->DelFromWaitList(protocol.accid);
}

void HandleProtocol(CLProtocol::WorldRegisterPlayerReq& protocol)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(protocol.id);
	if(player != NULL && player->GetGameStatus() != PS_FINAL)
	{
		GameErrorLogStream("Player") << "player(," << protocol.id << ",) repeat login!" << LogStream::eos;
		
		CLProtocol::WorldRegisterPlayerRet ret;
		ret.id = protocol.id;
		ret.result = ErrorCode::ENTERGAME_REPEAT;
		protocol.SendBack(ret);
		return;
	}

	if(player != NULL)
	{
		if(!player->Init(protocol.id, protocol.accid, protocol.name, protocol.GetConnID(), protocol.gateid))
		{
			GameErrorLogStream("Player") << "player(" << protocol.accid << "," << protocol.id << "," << protocol.name << ") init failed!" << LogStream::eos;
		
			CLProtocol::WorldRegisterPlayerRet ret;
			ret.id = protocol.id;
			ret.result = ErrorCode::ENTERGAME_INIT_FAILED;
			protocol.SendBack(ret);
			return;
		}

		if(!WSPlayerMgr::Instance()->AddPlayerByAccId(player))
		{
			GameErrorLogStream("Player") << "player(" << protocol.accid << "," << protocol.id << "," << protocol.name << ") login failed for repeat accid!" << LogStream::eos;
			return;
		}

		player->SetZoneId(protocol.zoneid);
		player->SetLevel(protocol.level);
		player->SetOccu(protocol.occu);
		player->SetSex(protocol.sex);
		player->SetHeadFrame(protocol.headFrame);
		player->SetQQVipInfo(protocol.qqVipInfo);
		player->SetCreateTime(protocol.createtime);
		player->SetOfflineTime(protocol.offlinetime);
		player->SetLocalOfflineTime(protocol.localOfflineTime);
		player->SetOpenId(protocol.openid);
		player->SetCity(protocol.city);
		player->SetPower(protocol.power);
		player->SetIp(protocol.ip);
		player->SetPf(protocol.pf);
		player->SetDeviceId(protocol.did);
		player->SetModel(protocol.model);
		player->SetDeviceVersion(protocol.deviceVersion);
		player->SetSrcZoneId(protocol.srcZoneId);
		player->SetAuctionAdditionBooth(protocol.auctionAdditionBooth);
		player->SetAuctionRefreshTime(protocol.auctionRefreshTime);
		player->SetMonthCardExpireTime(protocol.monthCardExpireTime);
		player->SetBornZoneId(protocol.bornZoneId);
		player->SetRoleReturnTime(protocol.roleReturnTime);
		player->OnCheckVeteranReturn();

		player->OnLoadDataFinished();
	}
	else
	{
		WSPlayer* player = new WSPlayer();

		if(!player->Init(protocol.id, protocol.accid, protocol.name, protocol.GetConnID(), protocol.gateid))
		{
			GameErrorLogStream("Player") << "player(" << protocol.accid << "," << protocol.id << "," << protocol.name << ") init failed!" << LogStream::eos;
		
			CLProtocol::WorldRegisterPlayerRet ret;
			ret.id = protocol.id;
			ret.result = ErrorCode::ENTERGAME_INIT_FAILED;
			protocol.SendBack(ret);
			CL_SAFE_DELETE(player);
			return;
		}

		DebugLogStream << "player(" << protocol.id << ") start to load data." << LogStream::eos;

		player->SetZoneId(protocol.zoneid);
		player->SetLevel(protocol.level);
		player->SetOccu(protocol.occu);
		player->SetSex(protocol.sex);
		player->SetHeadFrame(protocol.headFrame);
		player->SetQQVipInfo(protocol.qqVipInfo);
		player->SetCreateTime(protocol.createtime);
		player->SetOfflineTime(protocol.offlinetime);
		player->SetLocalOfflineTime(protocol.localOfflineTime);
		player->SetOpenId(protocol.openid);
		player->SetCity(protocol.city);
		player->SetPower(protocol.power);
		player->SetIp(protocol.ip);
		player->SetPf(protocol.pf);
		player->SetDeviceId(protocol.did);
		player->SetModel(protocol.model);
		player->SetDeviceVersion(protocol.deviceVersion);
		player->SetSrcZoneId(protocol.srcZoneId);
		player->SetAuctionAdditionBooth(protocol.auctionAdditionBooth);
		player->SetAuctionRefreshTime(protocol.auctionRefreshTime);
		player->SetMonthCardExpireTime(protocol.monthCardExpireTime);
		player->SetBornZoneId(protocol.bornZoneId);
		player->SetRoleReturnTime(protocol.roleReturnTime);
		player->OnCheckVeteranReturn();

		WSPlayerMgr::Instance()->AddPlayer(player);

		player->LoadData();
	}
}

void HandleProtocol(CLProtocol::SceneGameReconnectRet& protocol)
{
    DebugLogStream << "recv scene player(" << protocol.roleId << ") request reconnect game ret." << LogStream::eos;

    WSPlayer* player = WSPlayerMgr::Instance()->FindPlayer(protocol.roleId);
    if (!player || player->GetGameStatus() != PS_LEAVEGAME)
    {
        ErrorLogStream << "player(" << protocol.roleId << " can't reconnect!" << LogStream::eos;

        protocol.result = ErrorCode::ENTERGAME_REPEAT;
        protocol.SendBack(protocol);
        return;
    }

    player->SetLeaveTimeOut(0);
    player->SetGameStatus(PS_NORMAL);
    

	if (WSApplication::Instance()->IsAdjustOnOnline())
	{
		player->OnLoadDataFinished(true);
	}

	protocol.result = 0;
    protocol.SendBack(protocol);

	if (!WSApplication::Instance()->IsAdjustOnOnline())
	{
		player->OnLoadDataFinished(true);
	}
}

void HandleProtocol(ObjID_t id, CLProtocol::SceneQueryAccountReq& req)
{
	auto info = AccountMgr::Instance()->FindAccount(req.accid);
	if (!info)
	{
		// 去数据库加载
		class QueryAccount : public AccountCallback
		{
		public:
			QueryAccount(CLProtocol::SceneQueryAccountReq& req, UInt32 accid, ObjID_t id) : AccountCallback(accid), m_Req(req), m_AsyncId(id) {}

			void OnFinish(AccountInfo* info) 
			{
				CLProtocol::WorldQueryAccountRet ret;
				ret.result = ErrorCode::SUCCESS;
				ret.point = info->GetPoint();
				ret.creditPoint = info->GetCreditPoint();
				ret.accountGuid = info->GetID();
				ret.accountId = info->GetAccID();
				ret.vipLevel = info->GetVipLevel();
				ret.vipExp = info->GetVipExp();
				ret.totalChargeNum = info->GetTotalChargeNum();
				ret.monthCardExpireTime = info->GetMonthCardExpireTime();
				info->GetAllDungeonHardInfo(ret.dungeonHardInfoes);
				ret.storageSize = info->GetStoragetSize();
				ret.moneyManageStatus = info->GetMoneyManageStatus();
				ret.weaponLeaseTickets = info->GetWeaponLeaseTickets();
				ret.adventureTeamLevel = info->GetAdventureTeamLevel();
				ret.adventureTeamGrade = info->GetAdventureTeamGrade();
				ret.accountAchievementScore = info->GetAccountAchievementScore();
				ret.mallPoint = info->GetMallPoint();
				ret.adventureCoin = info->GetAdventureCoin();
				AsyncMsgCallbackMgr::Instance()->SendBackReturnMsg(m_AsyncId, m_Req, ret);
			}

			void OnFailure()
			{
				CLProtocol::WorldQueryAccountRet ret;
				ret.result = 1;
				AsyncMsgCallbackMgr::Instance()->SendBackReturnMsg(m_AsyncId, m_Req, ret);
				
			}
		private:
			CLProtocol::SceneQueryAccountReq	m_Req;
			ObjID_t							m_AsyncId;
		};

		AccountMgr::Instance()->LoadAccountInfo(req.accid, new QueryAccount(req, req.accid, id));
	}
	else
	{
		CLProtocol::WorldQueryAccountRet ret;
		ret.result = ErrorCode::SUCCESS;
		ret.point = info->GetPoint();
		ret.creditPoint = info->GetCreditPoint();
		ret.accountGuid = info->GetID();
		ret.accountId = info->GetAccID();
		ret.vipLevel = info->GetVipLevel();
		ret.vipExp = info->GetVipExp();
		ret.totalChargeNum = info->GetTotalChargeNum();
		ret.monthCardExpireTime = info->GetMonthCardExpireTime();
		info->GetAllDungeonHardInfo(ret.dungeonHardInfoes);
		ret.storageSize = info->GetStoragetSize();
		ret.moneyManageStatus = info->GetMoneyManageStatus();
		ret.weaponLeaseTickets = info->GetWeaponLeaseTickets();
		ret.adventureTeamLevel = info->GetAdventureTeamLevel();
		ret.adventureTeamGrade = info->GetAdventureTeamGrade();
		ret.accountAchievementScore = info->GetAccountAchievementScore();
		ret.mallPoint = info->GetMallPoint();
		ret.adventureCoin = info->GetAdventureCoin();
		AsyncMsgCallbackMgr::Instance()->SendBackReturnMsg(id, req, ret);
	}
}

void HandleProtocol(CLProtocol::WorldGetLoginPushInfoReq& protocol)
{
	InfoLogStream << "Player(roleid: " << protocol.roleid << ") require get login push info." << LogStream::eos;

	CLProtocol::WorldGetLoginPushInfoRet ret;
	ret.roleid = protocol.roleid;

	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.roleid);
	if (!player)
	{
		ErrorLogStream << "Find normal player(roleid: " << protocol.roleid << ") failed!" << LogStream::eos;
		protocol.SendBack(ret);
		return;
	}

	LoginPushMgr::Instance()->GetLoginPushInfos(ret.infos);
	protocol.SendBack(ret);
}

