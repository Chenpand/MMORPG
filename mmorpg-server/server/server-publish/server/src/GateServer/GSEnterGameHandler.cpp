#include "GSEnterGameHandler.h"

#include <CLErrorCode.h>
#include <AvalonLogger.h>
#include <CLRecordClient.h>
#include <CLForbitWord.h>
#include <CLLoginProtocol.h>
#include <CLGameDefine.h>
#include <CLSecurityLockProtocol.h>

#include <kingnet_analyzer.h>

#include "GSPlayerMgr.h"
#include "GSSceneMgr.h"
#include "GSExternalNetwork.h"
#include "GSRouter.h"
#include "GSApplication.h"
#include "GSActivityMgr.h"
#include "GSServiceSwitchMgr.h"

/**
 *@brief 角色添加到数据库返回
 */
class InsertRoleCallback : public CLRecordCallback
{
public:
	InsertRoleCallback(UInt32 accid,const GSRoleInfo& role, UInt8 preOccu)
		:m_Accid(accid),m_Role(role), m_PreOccu(preOccu){}
	~InsertRoleCallback(){}

	void OnFinished()
	{
        InfoLogStream << "gate player(" << m_Accid << ") created role success, rold(" << m_Role.id << ")." << LogStream::eos;

		GSPlayer* player =  GSPlayerMgr::Instance()->FindPlayerByAccID(m_Accid);
		if(player == NULL || player->GetStatus() != GPS_CREATEROLE) return;

		player->AddRole(m_Role);
		player->SetStatus(GPS_SELECTROLE);

		player->SendRoleInfo();

		CLProtocol::GateCreateRoleRet ret;
		ret.result = ErrorCode::SUCCESS;
		player->SendProtocol(ret);

		player->SendCreateUdpLog(m_Role.name, m_Role.occu, m_Role.id, m_PreOccu, m_Role.isAppointmentOccu);

		InfoLogStream << "gate player(" << m_Accid << ") created role(" << m_Role.name << "," << int(m_Role.occu) << ")." << LogStream::eos;

	}

	void OnFailed(UInt32 errorcode)
	{
        InfoLogStream << "gate player(" << m_Accid << ") created role failed, error code(" << errorcode << ")." << LogStream::eos;

		GSPlayer* player =  GSPlayerMgr::Instance()->FindPlayerByAccID(m_Accid);
		if(player == NULL || player->GetStatus() != GPS_CREATEROLE) return;

		player->SetStatus(GPS_SELECTROLE);

		CLProtocol::GateCreateRoleRet ret;
		if(errorcode == ErrorCode::RECORD_DUPLICATE_NAME)
		{
			ret.result = ErrorCode::ENTERGAME_DUPLICATE_NAME;
		}
		else
		{
			ret.result = ErrorCode::ENTERGAME_SERVER_BUSY;
		}
		player->SendProtocol(ret);
	}

private:
	//账号id
	UInt32		m_Accid;
	//角色信息
	GSRoleInfo	m_Role;
	//预转职职业
	UInt8		m_PreOccu;
};

void HandleProtocol(GSPlayer* player,CLProtocol::GateCreateRoleReq& protocol)
{
	UInt8 mainOccu = (UInt8)((protocol.occu / 10) * 10);
	if (!IsValidSex(protocol.sex) || !IsValidOccu(mainOccu, protocol.occu, protocol.newer)) {
		InfoLogStream << "player(" << player->GetAccID() << ") sex(" << protocol.sex << "), occu(" << mainOccu << "," << protocol.occu << ")." << LogStream::eos;
		CLProtocol::GateCreateRoleRet ret;
		ret.result = ErrorCode::ENTERGAME_UNVALID_ROLEINFO;
		player->SendProtocol(ret);
        return;
    }

	char namebuf[NAME_LENGTH+1];
	avalon_strncpy(namebuf, protocol.name.c_str(), NAME_LENGTH);

	if(!CheckNameValid(protocol.name.c_str()))
	{
		CLProtocol::GateCreateRoleRet ret;
		ret.result = ErrorCode::ENTERGAME_UNVALID_NAME;
		player->SendProtocol(ret);
		return;
	}

	if(ForbitWordParser::Instance()->Parse(namebuf) != FORBIT_WORD_UNCHANGED)
	{
		CLProtocol::GateCreateRoleRet ret;
		ret.result = ErrorCode::ENTERGAME_UNVALID_NAME;
		player->SendProtocol(ret);
		return;
	}
	/*
	if (player->GetTodayCreatedRoleNum() > 2)
	{
		CLProtocol::GateCreateRoleRet ret;
		ret.result = ErrorCode::ENTERGAME_TODAY_TOOMANY_ROLE;
		player->SendProtocol(ret);
		return;
	}*/

	if (!GSServiceSwitchMgr::Instance()->IsOpen(SERVICE_ROLE_FILED))
	{
		UInt32 createNewRoleRet = player->CanCreateNewRole();
		if (createNewRoleRet != ErrorCode::SUCCESS)
		{
			CLProtocol::GateCreateRoleRet ret;
			ret.result = createNewRoleRet;
			player->SendProtocol(ret);
			return;
		}
	}
	else
	{
		if (player->RoleFieldReachUpperLimit())
		{
			CLProtocol::GateCreateRoleRet ret;
			ret.result = ErrorCode::ENTERGAME_ROLE_FIELD_REACN_UPPER_LIMIT;
			player->SendProtocol(ret);
			return;
		}
	}

	Avalon::PropertySet* initattrs = GSPlayerMgr::Instance()->GetPlayerInitAttr();
	if(initattrs->GetProperty("PlayerInitInfo","no_createrole"))
	{
		CLProtocol::GateCreateRoleRet ret;
		ret.result = ErrorCode::ENTERGAME_NO_CREATEROLE;
		player->SendProtocol(ret);
		return;
	}

	GSRoleInfo role;
	role.id = CLRecordClient::Instance()->GenGuid(); //noted by aprilliu, 角色id 是一个全局唯一的guid
	role.name = protocol.name;
	role.sex = protocol.sex;
	role.preOccu = protocol.occu;
	role.occu = mainOccu;

	if (GSActivityMgr::Instance()->IsAppointmentOccu(mainOccu) && 
		player->GetAppointmentRoleNum() < GSActivityMgr::Instance()->GetMaxAppointmentRoleNum())
	{
		role.isAppointmentOccu = 1;
		InfoLogStream << "account(" << player->GetAccID() << ") create appointment role(" << role.id << ") occu(" << role.occu << ")." << LogStream::eos;
	}
	else
	{
		role.isAppointmentOccu = 0;
	}

	role.sceneid = initattrs->GetProperty("PlayerInitInfo","scene");
	std::string pos = initattrs->GetProperty("PlayerInitInfo","pos").GetString();
	role.pos.FromString(pos);

	role.mapid = role.sceneid;
	role.level = initattrs->GetProperty("PlayerInitInfo","level");
	role.createtime = UInt32(CURRENT_TIME.Sec());
	role.bornZoneId = ZONE_ID;

	CLInsertCommand* insertcmd = CLRecordClient::Instance()->PrepareInsertCommand("t_player_info", role.id);
	insertcmd->PutData("accid",	player->GetAccID());
	insertcmd->PutData("name",	role.name);
	insertcmd->PutData("zoneid",	ZONE_ID);
	insertcmd->PutData("born_zoneid", ZONE_ID);
	insertcmd->PutData("sceneid",	role.sceneid);
	insertcmd->PutData("mapid",	role.mapid);
	insertcmd->PutData("pos",	pos);
	insertcmd->PutData("level",	role.level);
	insertcmd->PutData("sex",	role.sex);
	insertcmd->PutData("occu",	role.occu);
	insertcmd->PutData("pre_occu", protocol.occu);
	insertcmd->PutData("createtime",role.createtime);
	insertcmd->PutData("platform", player->GetPf());
	insertcmd->PutData("openid", player->GetOpenId());
	insertcmd->PutData("is_appointment_occu", role.isAppointmentOccu);
	if (player->NeedPopupNoviceGuideChoose())
	{
		insertcmd->PutData("novice_guide_choose_flag", (UInt8)NGCF_POPUP);
		role.playerLabelInfo.noviceGuideChooseFlag = NGCF_POPUP;
	}
	if(1 == protocol.newer)
		insertcmd->PutData("actionflag", 1 << ACTION_FLAG_NEWER);
	else if (2 == protocol.newer)
		insertcmd->PutData("actionflag", 1 << ACTION_FLAG_ROBOT);
		
	UInt32 uint = initattrs->GetProperty("PlayerInitInfo","exp");
	insertcmd->PutData("exp",uint);

	uint = initattrs->GetProperty("PlayerInitInfo","hp"); 
	insertcmd->PutData("hp", uint);

	uint = initattrs->GetProperty("PlayerInitInfo", "gold");
	insertcmd->PutData("gold", uint);

//	UInt8 packsize = initattrs->GetProperty("PlayerInitInfo","packsize");
// 	insertcmd->PutData("packsize", packsize);
// 
// 	// 统一账号的仓库大小
// 	if (0 == player->GetStorageSize())
// 	{
// 		UInt8 storagesize = initattrs->GetProperty("PlayerInitInfo", "storagesize");
// 		insertcmd->PutData("storagesize", storagesize);
// 	}
// 	else
// 		insertcmd->PutData("storagesize", player->GetStorageSize());

    InfoLogStream << "player(" << player->GetAccID() << "start to create role(" << role.id << ")" << LogStream::eos;

	CLRecordClient::Instance()->SendCommand(insertcmd, new InsertRoleCallback(player->GetAccID(), role, protocol.occu));//noted by aprilliu, 生成callback，记录请求的上下文信息
	
	player->SetStatus(GPS_CREATEROLE);	//设置为创建角色状态
}

void HandleProtocol(GSPlayer* player, CLProtocol::GateDeleteRoleReq& protocol)
{
	// 删除角色时，向world请求检查安全锁
	CLProtocol::GateSecurityLockCheckReq checkReq;
	checkReq.accId = player->GetAccID();
	checkReq.roleId = protocol.roleId;
	checkReq.deviceId = protocol.deviceId;
	Router::SendToWorld(checkReq);
}

void HandleProtocol(GSPlayer* player, CLProtocol::GateRecoverRoleReq& protocol)
{
	CLProtocol::GateRecoverRoleRes res;
	res.roleId = protocol.roleId;
	res.result = player->RecoverRole(protocol.roleId);
	if (res.result == ErrorCode::ENTERGAME_RECOVER_ROLE_LIMIT)
	{
		res.roleUpdateLimit = player->GetRoleUpdateLimitInfo();
	}
	player->SendProtocol(res);
}

void HandleProtocol(GSPlayer* player, CLProtocol::GateEnterGameReq& protocol)
{
    if(!player->BeginEnterGame(protocol.roleId, protocol.option, protocol.city, protocol.inviter)) return;
	player->SetLastRecvSequence(protocol.GetSequence());
}

void HandleProtocol(GSPlayer *player, CLProtocol::GateFinishNewbeeGuide& protocol)
{
	player->OnFinishNewbeeGuide(protocol.roleId, protocol.id);
}

void HandleProtocol(GSPlayer* player, CLProtocol::GateLeaveLoginQueue& protocol)
{
	// 通知到world
	CLProtocol::WorldPlayerLeaveLoginQueue notify;
	notify.accid = player->GetAccID();
	Router::SendToWorld(notify);

	GSPlayerMgr::Instance()->DelFromWaitList(player->GetAccID());

	// 断开连接
	player->Disconnect();
}

void HandleProtocol(GSPlayer* player, CLProtocol::GateConvertAccountReq& protocol)
{
	if (!player)
	{
		return;
	}

	CLProtocol::CenterQueryConvertAccountReq req;
	req.fromGate = 1;
	req.accid = player->GetAccID();
	req.platform = player->GetPf();
	req.openid = player->GetOpenId();
	Router::SendToCenter(req);
}

void HandleProtocol(CLProtocol::CenterQueryConvertAccountRes& protocol)
{
	auto player = GSPlayerMgr::Instance()->FindPlayerByAccID(protocol.accid);
	if (!player)
	{
		return;
	}

	CLProtocol::GateConvertAccountRes res;
	res.account = protocol.account;
	res.passwd = protocol.password;
	res.saveFile = GSApplication::Instance()->IsSaveConvertFile();
	res.screenShot = GSApplication::Instance()->IsConvertScreenShot();
	player->SendProtocol(res);
}

void HandleProtocol(CLProtocol::GateReconnectGameReq& protocol)
{
// 	CLProtocol::GateReconnectGameRes res;
// 
// 	auto player = GSPlayerMgr::Instance()->FindPlayerByID(protocol.roleId);
// 	if (!player)
// 	{
// 		res.result = ErrorCode::RECONNECT_PLAYER_DELETED;
// 		protocol.SendBack(res);
// 		return;
// 	}
// 
// 	res.result = player->BeginReconnect(protocol.sequence);
// 	protocol.SendBack(res);
}

void HandleProtocol(CLProtocol::SceneEnterGameRet& protocol)
{
	GSPlayer* player = GSPlayerMgr::Instance()->FindPlayerByID(protocol.id);
	if(player == NULL || player->GetStatus() != GPS_ENTERGAME) return;

	CLProtocol::GateEnterGameRet ret;
	ret.result = protocol.result;

	if(protocol.result != ErrorCode::SUCCESS)
	{
		player->SendProtocol(ret);
		player->Disconnect();
		return;
	}

	player->EndEnterGame();
	player->SendProtocol(ret);

	DebugLogStream << "[login] player(" << player->GetAccID() << ") enter game." << LogStream::eos;

}

void HandleProtocol(CLProtocol::WorldVerifyUniqueRet& protocol)
{
    DebugLogStream << "player(" << protocol.accid << ") recv ret sequence(" << protocol.GetSequence() << ")." << LogStream::eos;

	GSPlayer* player = GSPlayerMgr::Instance()->FindPlayerByAccID(protocol.accid);
    if (player == NULL || player->GetStatus() != GPS_VERIFY)
    {
        DebugLogStream << "can't find player(" << protocol.accid << ")." << LogStream::eos;
        return;
    }

	if (!player->CanEnterGame() && GSPlayerMgr::Instance()->IsNeedWaitOhters())
	{
		// 开始排队
		player->SetStatus(GPS_QUEUE);
		player->SetCanEnterGame(false);

		CLProtocol::GateClientLoginRet ret;
		ret.result = ErrorCode::LOGIN_WAIT;
		ret.serverStartTime = protocol.serverStartTime;
		ret.waitPlayerNum = GSPlayerMgr::Instance()->AddPlayerToWaitList(player->GetAccID());
		player->SendProtocol(ret);

		InfoLogStream << "player(" << player->GetAccID() << ") in queue, need wait player(" << ret.waitPlayerNum << ")." << LogStream::eos;
		return;
	}
	
	player->SetCanEnterGame(true);
	GSApplication::Instance()->SetServerStartTime(protocol.serverStartTime);

	player->BeginQueryAccountInfo();

	CLProtocol::GateSendLoginPushInfo sendPush;
	sendPush.infos = protocol.infos;
	player->SendProtocol(sendPush);
}

void HandleProtocol(CLProtocol::WorldAllowPlayerLogin& protocol)
{
	DebugLogStream << "allow player(" << protocol.accid << ") login." << LogStream::eos;

	GSPlayer* player = GSPlayerMgr::Instance()->FindPlayerByAccID(protocol.accid);
	if (player == NULL)
	{
		ErrorLogStream << "can't find player(" << protocol.accid << ")." << LogStream::eos;
		return;
	}

	if (player->GetStatus() != GPS_QUEUE)
	{
		ErrorLogStream << "player(" << protocol.accid << ") not in wait queue." << LogStream::eos;
		return;
	}

	CLProtocol::GateNotifyAllowLogin notify;
	player->SendProtocol(notify);

	player->BeginQueryAccountInfo();
}

void HandleProtocol(CLProtocol::WorldSyncPlayerLoginWaitInfo& protocol)
{
	GSPlayer* player = GSPlayerMgr::Instance()->FindPlayerByAccID(protocol.accid);
	if (player == NULL)
	{
		ErrorLogStream << "can't find player(" << protocol.accid << ")." << LogStream::eos;
		return;
	}

	if (player->GetStatus() != GPS_QUEUE)
	{
		ErrorLogStream << "player(" << protocol.accid << ") not in wait queue." << LogStream::eos;
		return;
	}

	CLProtocol::GateNotifyLoginWaitInfo notify;
	notify.waitPlayerNum = protocol.waitPlayerNum;
	player->SendProtocol(notify);
}

void HandleProtocol(GSPlayer* player, CLProtocol::RoleSwitchReq& protocol)
{
	if (player == NULL)
	{
		ErrorLogStream << "GSPlayer ptr is null when switching role." << LogStream::eos;
		return;
	}

	InfoLogStream << "player(" << player->GetAccID() << "," << player->GetID() << ") require switch role." << LogStream::eos;

	CLProtocol::WorldGetLoginPushInfoReq req;
	req.roleid = player->GetID();
	Router::SendToWorld(req);
}

void HandleProtocol(CLProtocol::WorldGetLoginPushInfoRet& protocol)
{
	InfoLogStream << "Player(roleid: " << protocol.roleid << ") get login push info success." << LogStream::eos;

	GSPlayer* player = GSPlayerMgr::Instance()->FindPlayerByID(protocol.roleid);
	if (!player)
	{
		ErrorLogStream << "Find player(roleid: " << protocol.roleid << ") failed!" << LogStream::eos;
		return;
	}

	player->NotifyLeaveGame(player->GetID());
	player->SetStatus(GPS_WAITROLEINFO);
	player->BeginQueryAccountInfo((UInt8)EGM_ROLE_SWITCH);

	CLProtocol::GateSendLoginPushInfo sendPush;
	sendPush.infos = protocol.infos;
	player->SendProtocol(sendPush);
}

void HandleProtocol(GSPlayer *player, CLProtocol::GateNoviceGuideChooseReq& req)
{
	if (!player) return;

	if (req.chooseFlag > (UInt8)NGCF_POPUP && req.chooseFlag < (UInt8)NGCF_MAX)
	{
		player->SetNoviceGuideChooseFlag(req.roleid, req.chooseFlag);
	}
}
void HandleProtocol(GSPlayer *player, CLProtocol::GateSetRolePreferencesReq& req)
{
	if (!player) return;

	InfoLogStream << "player(Roleid:" << req.roleId << ") begin set RolePreferences" << LogStream::eos;

	CLProtocol::GateSetRolePreferencesRet protocol;
	protocol.result = player->SetPreferences(req.roleId, (RolePreferences)req.preferencesFlag);
	protocol.roleId = req.roleId;

	if (protocol.result == ErrorCode::SUCCESS)
	{
		GSPlayer::RoleList roleList = player->GetRoleList();

		for (auto role : roleList)
		{
			if (role.id != protocol.roleId)continue;
			protocol.addPreferencesTime = role.addPreferencesTime;
			protocol.delPreferencesTime = role.delPreferencesTime;
			break;
		}
	}

	player->SendProtocol(protocol);

	InfoLogStream << "Role(id:"<<req.roleId<<") set RolePreferences end" << LogStream::eos;

	return;
}