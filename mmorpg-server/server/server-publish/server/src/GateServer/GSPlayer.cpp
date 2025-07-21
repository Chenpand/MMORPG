#include "GSPlayer.h"

#include <CLEnterGameProtocol.h>
#include <CLGateProtocol.h>
#include <CLChatProtocol.h>
#include <CLRecordClient.h>
#include <CLChangeZoneProtocol.h>
#include <CLLeaveGameProtocol.h>
#include <CLLoginProtocol.h>
#include <CLSystemValueDataEntry.h>
#include <CLSysNotify.h>
#include <AvalonNetworkAPI.h>
#include <CLCenterProtocol.h>
#include <CLSyncObjectProtocol.h>
#include <CLJobDataEntry.h>
#include <CLSysProtocol.h>
#include <CLGambingProtocol.h>
#include <CLActiveEventDataEntry.h>
#include <TimeUtil.h>
#include <ActivityUtil.h>
#include <CLAdventureTeamProtocol.h>
#include <PlayerHelper.h>
#include <CLPlayerDataEntry.h>

#include "GSRouter.h"
#include "GSExternalNetwork.h"
#include "GSScene.h"
#include "GSSceneMgr.h"
#include "GSPlayerMgr.h"
#include "GSApplication.h"
#include "CLXtea.h"
#include "GSActivityMgr.h"
#include "GSServiceSwitchMgr.h"
#include "GSAccountMgr.h"
#include "GSUnionProtocolMgr.h"

class InsertAccountCallback : public CLRecordCallback
{
public:
	InsertAccountCallback(UInt32 accid)
		:m_accid(accid){}

	void OnFinished()
	{
		GSPlayer* player = GSPlayerMgr::Instance()->FindPlayerByAccID(m_accid);
		if (player == NULL || player->GetStatus() != GPS_WAITROLEINFO)
		{
			ErrorLogStream << "account(" << m_accid << ") can't find player or invalid status." << LogStream::eos;
			return;
		}
		player->StartToQueryRoleInfo();
	}

	void OnFailed(UInt32 errorcode)
	{
		ErrorLogStream << "insert account(" << m_accid << ") failed, error:" << errorcode << LogStream::eos;

		GSPlayer* player = GSPlayerMgr::Instance()->FindPlayerByAccID(m_accid);
		if (player == NULL || player->GetStatus() != GPS_WAITROLEINFO)
		{
			ErrorLogStream << "account(" << m_accid << ") can't find player or invalid status." << LogStream::eos;
			return;
		}

		CLProtocol::GateClientLoginRet ret;
		ret.result = ErrorCode::LOGIN_SERVER_BUSY;
		player->SendProtocol(ret);
		player->Disconnect();
		return;
	}

private:
	UInt32							m_accid;
};

class QueryAccountCallback : public CLRecordCallback
{
public:
	QueryAccountCallback(UInt32 accid, UInt8 enterGameMode)
		:m_accid(accid), m_EnterGameMode(enterGameMode){}

	void OnFinished()
	{
		if (!NextRow())
		{
			// 没有账号信息，去创建账号表
			CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_account", CLRecordClient::Instance()->GenGuid());
			cmd->PutData("accid", m_accid);
			cmd->PutData("point", 0);
			cmd->PutData("reg_time", (UInt32)CURRENT_TIME.Sec());
			cmd->PutData("storage_size", (UInt8)GSPlayerMgr::Instance()->GetPlayerInitAttr()->GetProperty("PlayerInitInfo", "storagesize"));

			DebugLogStream << "start to create account(" << m_accid << ")." << LogStream::eos;

			CLRecordClient::Instance()->SendCommand(cmd, new InsertAccountCallback(m_accid));
			return;
		}

		// 说明已经有账号信息了
		GSPlayer* player = GSPlayerMgr::Instance()->FindPlayerByAccID(m_accid);
		if (player == NULL || player->GetStatus() != GPS_WAITROLEINFO)
		{
			ErrorLogStream << "account(" << m_accid << ") can't find player or invalid status." << LogStream::eos;
			return;
		}

		player->SetAccountGuid(GetKey());
		player->SetGmAuthority(GetData("gmauthority"));
		player->SetRoleDeleteTime(GetData("role_delete_time"));
		player->SetRoleRecoverTime(GetData("role_recover_time"));
		player->SetAdventureTeamInfo(GetData("adventure_team_name").GetString(), GetData("adventure_team_level"), GetData("adventure_team_exp"));
		player->SetUnlockedExtensibleRoleField(GetData("unlock_extensible_role_num"));
		player->SetAccountReturnTime(GetData("veteran_return_time"));

		player->StartToQueryRoleInfo(m_EnterGameMode);
	}

	void OnFailed(UInt32 errorcode)
	{
		ErrorLogStream << "insert account(" << m_accid << ") failed, error:" << errorcode << LogStream::eos;

		GSPlayer* player = GSPlayerMgr::Instance()->FindPlayerByAccID(m_accid);
		if (player == NULL || player->GetStatus() != GPS_WAITROLEINFO)
		{
			ErrorLogStream << "account(" << m_accid << ") can't find player or invalid status." << LogStream::eos;
			return;
		}

		CLProtocol::GateClientLoginRet ret;
		ret.result = ErrorCode::LOGIN_SERVER_BUSY;
		player->SendProtocol(ret);
		player->Disconnect();

		return;
	}
private:
	UInt32 m_accid;
	//进入游戏方式
	UInt8 m_EnterGameMode;
};

/**
*@brief 查询角色信息回调
*/
class SelectRolesCallback : public CLRecordCallback
{
public:
	SelectRolesCallback(UInt32 accid, UInt8 enterGameMode) :m_Accid(accid), m_EnterGameMode(enterGameMode){}
	~SelectRolesCallback(){}

public:
	void OnFinished()
	{
		DebugLogStream << "player(" << m_Accid << ") query role list finished." << LogStream::eos;

		GSPlayer* player = GSPlayerMgr::Instance()->FindPlayerByAccID(m_Accid);
		if (player == NULL || player->GetStatus() != GPS_WAITROLEINFO) return;

		bool isOldReturn = false;
		UInt32 return_days = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_VETERANT_RETURN_DAYS);
		UInt32 continue_day_no_login = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_RETURN_CONTINUE_LOGIN_DAY);
		UInt32 return_need_level = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_RETURN_NEED_ROLE_LEVEL);
		UInt32 return_cool_cd = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_VETERANT_DEFINE_RETURN_COOLDOWN);
		UInt32 day_start_time = UInt32(GetDayBeginTime(CURRENT_TIME) / 1000);
		UInt32 num_1 = 0;
		UInt32 num_2 = 0;
		bool isOpenReturn = GSServiceSwitchMgr::Instance()->IsOpen(SERVICE_NEW_RETURN);

		while (NextRow())
		{
			GSRoleInfo info;
			info.id = GetKey();
			info.name = GetData("name").GetString();
			info.level = GetData("level");
			info.sex = GetData("sex");
			info.occu = GetData("occu");
			info.sceneid = GetData("sceneid");
			info.mapid = GetData("mapid");
			info.pos.FromString(GetData("pos").GetString());
			info.tourZoneId = GetData("tourzone");
			info.createtime = GetData("createtime");
			UInt64 offlineTime = GetData("savetime");
			info.offlineTime = UInt32(offlineTime / 1000);
			info.deleteTime = GetData("deletetime");
			info.newBoot = GetData("newboot");
			info.bornZoneId = GetData("born_zoneid");
			info.preOccu = GetData("pre_occu");
			info.isAppointmentOccu = GetData("is_appointment_occu");
			info.returnTime = GetData("return_time");
			info.returnLevel = GetData("return_level");
			info.playerLabelInfo.noviceGuideChooseFlag = GetData("novice_guide_choose_flag");
			info.addPreferencesTime = GetData("add_preferences_time");
			info.delPreferencesTime = GetData("del_preferences_time");
			std::string avatarStr = GetData("avatar").GetString();
			std::vector<std::string> avatarInfoes;
			Avalon::StringUtil::Split(avatarStr, avatarInfoes, ",");
			if (avatarInfoes.size() >= 1)
			{
				std::vector<std::string> avatarEquips;
				Avalon::StringUtil::Split(avatarInfoes[0], avatarEquips, "|");
				for (auto& str : avatarEquips)
				{
					info.avatar.equipItemIDs.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(str));
				}
			}

			if (avatarInfoes.size() >= 2)
			{
				info.avatar.weaponStrengthen = Avalon::StringUtil::ConvertToValue<UInt8>(avatarInfoes[1]);
			}
			
			if (avatarInfoes.size() >= 3)
			{
				info.avatar.isShoWeapon = Avalon::StringUtil::ConvertToValue<UInt8>(avatarInfoes[2]);
			}

			//仓库大小跟着账号
// 			if (0 == player->GetStorageSize())
// 				player->SetStorageSize(GetData("storagesize"));

			player->AddRole(info);

			// 先检查一遍，是否存在老的回归
			if (isOpenReturn && CURRENT_TIME.Sec() - player->GetAccountReturnTime() > return_cool_cd * DAY_TO_SEC)
			{
				if (player->GetAccountReturnTime() == 0 && info.returnTime != 0 && CURRENT_TIME.Sec() - info.returnTime < return_days * DAY_TO_SEC)
				{
					isOldReturn = true;
				}

				// 20级以上玩家的数量
				if (info.level >= return_need_level)
				{
					num_1++;
					// 连续超过14天未登录玩家数量
					if (CURRENT_TIME.Sec() - info.offlineTime > continue_day_no_login * DAY_TO_SEC)
						num_2++;
				}
			}
		}

		bool isNewReturn = num_1 == num_2 && num_1 != 0;
		if (isNewReturn && player->GetAccountReturnTime() != 0)
			player->SetAgainReturn(true);

		if (isOldReturn || isNewReturn)
		{
			InfoLogStream << "New Return Accid:" << m_Accid << LogStream::eos;

			player->SetAccountReturnTime(day_start_time);
			player->UpdateAccountReturnTime();

			for (auto& role : player->GetRoleList())
			{
				if (role.level >= return_need_level)
					player->UpdateRoleReturnInfo(role.id, day_start_time);
			}
		}

		player->SetStatus(GPS_SELECTROLE);

		DebugLogStream << "player(" << m_Accid << ") login success. svrStartTime:" << GSApplication::Instance()->GetServerStartTime() << LogStream::eos;

		//发送角色信息
		player->SendRoleInfo();

		switch ((EnterGameMode)m_EnterGameMode)
		{
		case EGM_NORMAL:
			{
				CLProtocol::GateClientLoginRet ret;
				ret.result = ErrorCode::SUCCESS;
				ret.hasrole = (player->GetRoleNum() > 0) ? 1 : 0;
				ret.serverStartTime = GSApplication::Instance()->GetServerStartTime();
				ret.notifyVeteranReturn = isOldReturn | isNewReturn;
				player->SendProtocol(ret);
				break;
			}
		case EGM_ROLE_SWITCH:
			{
				CLProtocol::RoleSwitchRes res;
				res.result = ErrorCode::SUCCESS;
				res.hasrole = (player->GetRoleNum() > 0) ? 1 : 0;
				res.serverStartTime = GSApplication::Instance()->GetServerStartTime();
				player->SendProtocol(res);
				break;
			}
		}

		player->SendAccountLoginUdpLog();
	}

	void OnFailed(UInt32 errorcode)
	{
		ErrorLogStream << "gate player(" << m_Accid << ") load roles failed!" << LogStream::eos;

		GSPlayer* player = GSPlayerMgr::Instance()->FindPlayerByAccID(m_Accid);
		if (player == NULL || player->GetStatus() != GPS_WAITROLEINFO) return;

		CLProtocol::GateClientLoginRet ret;
		ret.result = ErrorCode::LOGIN_SERVER_BUSY;
		player->SendProtocol(ret);
		player->Disconnect();

		return;
	}

private:
	//账号id
	UInt32 m_Accid;
	//进入游戏方式
	UInt8 m_EnterGameMode;
};

GSPlayer::GSPlayer()
{
	m_AccountGuid = 0;
	m_Accid = 0;
	m_GmAuthority = 0;
	m_Level = 1;

	m_Status = GPS_VERIFY;
	m_ClientOption = 0;
	m_LastMapID = 0;
	m_LastSceneID = 0;
	m_TryTimes = 20;
	m_Inviter = 0;

	m_SrcZoneId = 0;
	m_TourZoneId = 0;
	m_BornZoneId = 0;

	m_pScene = NULL;

	m_SecTimer.SetInterval(1000);
	m_TenSecTimer.SetLastTime(CURRENT_TIME.MSec());
	m_TenSecTimer.SetInterval(10 * 1000);
	m_MinSecTimer.SetLastTime(CURRENT_TIME.MSec());
	m_MinSecTimer.SetInterval(60 * 1000);
	m_StorageSize = 0;
	m_packetSequenceSeed = 0;
	m_Occu = Occupation::OCCU_NONE;
	m_LastRecvSequence = 0;
	m_IsMonitored = false;

	m_NetSimpleFile = NULL;
	m_NetDetailFile = NULL;
	m_PacketCountFile = NULL;

	m_RoleDeleteTime = 0;
	m_RoleRecoverTime = 0; 
	m_PhoneBindRoleId = 0;
	m_CanEnterGame = false;
	m_UnlockeExtensibleRoleField = 0;
	m_AccountReturnTime = 0;
	m_RoleReturnTime = 0;
	m_RoleReturnLevel = 0;
	m_AgainReturn = 0;

}

GSPlayer::~GSPlayer()
{
	m_Roles.clear();
	CloseMonitorFile();
}

bool GSPlayer::Init(UInt32 accid, UInt8 gmauthority, const std::string& openid,
	const QQVipInfo& info, const std::string& source1, const std::string& source2, const std::string& pf, const std::string& did,
	const std::string& model, const std::string& deviceVersion)
{
	m_Conn = GSExternalNetwork::Instance()->FindConnection(accid);
	if(m_Conn == NULL) return false;

	m_Accid = accid;
	m_Ip = m_Conn->GetPeerAddr().GetIP();
	m_GmAuthority = gmauthority;
	m_OpenId = openid;
	m_qqVipInfo = info;
	m_Source1 = source1;
	m_Source2 = source2;
	m_Pf = pf;
	m_DeviceId = did;
	m_Model = model;
	m_DeviceVersion = deviceVersion;
	SetID(0);
	m_packetBuffer.SetAccID(accid);
	return true;
}

void GSPlayer::SetStatus(GatePlayerStatus status)
{
	if(m_Status != status)
	{
		m_Status = status;
	}
}

void GSPlayer::SetLastRecvSequence(UInt32 sequence)
{
#ifdef _RECORD_LAST_SEQUENCE_
	DebugLogStream << "player(" << GetAccID() << ", " << GetID() << ") set last sequence(" << sequence << ")." << LogStream::eos;
#endif
	m_LastRecvSequence = sequence;
}

void GSPlayer::SetMonitored(bool monitored)
{
	m_IsMonitored = monitored;
	if (!m_IsMonitored)
	{
		CloseMonitorFile();
	}
}

void GSPlayer::SetAdventureTeamInfo(const std::string& name, UInt16 level, UInt64 exp)
{
	m_AdventureTeamInfo.adventureTeamName = name;
	m_AdventureTeamInfo.adventureTeamLevel = level;
	m_AdventureTeamInfo.adventureTeamExp = exp;

	SyncAdventureTeamInfo();
}

void GSPlayer::CloseMonitorFile()
{
	if (m_NetDetailFile)
	{
		fclose(m_NetDetailFile);
		m_NetDetailFile = NULL;
	}

	if (m_NetSimpleFile)
	{
		fclose(m_NetSimpleFile);
		m_NetSimpleFile = NULL;
	}

	if (m_PacketCountFile)
	{
		fclose(m_PacketCountFile);
		m_PacketCountFile = NULL;
	}
}

bool GSPlayer::AddRole(const GSRoleInfo &info)
{
	m_Roles.push_back(info);
	return true;
}

bool GSPlayer::IsLimitRoleUpdate(UInt32 updateType, UInt32 intervalTime, UInt32 roleUpdateLimitTime)
{
	if (intervalTime < roleUpdateLimitTime)
	{
		std::string remainingTimeStr = TimeToStr(roleUpdateLimitTime - intervalTime);
		UInt32 roleUpdateLimitDays = roleUpdateLimitTime / DAY_TO_SEC;
		m_RoleUpdateLimitInfo = SysNotifyMgr::Instance()->MakeString(updateType, roleUpdateLimitDays, remainingTimeStr);
		return true;
	}
	else
	{
		return false;
	}
}

std::string GSPlayer::TimeToStr(UInt32 secs)
{
	const SysNotify* day = SysNotifyMgr::Instance()->FindNotify(SysNotifyMgr::Instance()->DAY_STR_ID);
	const SysNotify* hour = SysNotifyMgr::Instance()->FindNotify(SysNotifyMgr::Instance()->HOUR_STR_ID);
	const SysNotify* min = SysNotifyMgr::Instance()->FindNotify(SysNotifyMgr::Instance()->MIN_STR_ID);

	if (day == NULL || hour == NULL || min == NULL) return "";

	UInt32 days = secs / DAY_TO_SEC;
	secs = secs % DAY_TO_SEC;
	UInt32 hours = secs / HOUR_TO_SEC;
	secs = secs % HOUR_TO_SEC;
	UInt32 mins = secs / MIN_TO_SEC;

	std::ostringstream oss;
	if (days != 0)	oss << days << day->format;
	if (hours != 0)	oss << hours << hour->format;
	if (days == 0)
	{
		oss << mins << min->format;
	}

	return oss.str();
}

void GSPlayer::RecordPacketCountToFile()
{
	if (m_packetCountMap.empty())
	{
		return;
	}

	if (!m_PacketCountFile)
	{
		std::string filename = std::string("Packet/Count_") + Avalon::StringUtil::ConvertToString(GetID()) + ".txt";
		m_PacketCountFile = fopen(filename.c_str(), "a");
	}

	if (m_PacketCountFile)
	{
		std::string curTimeStr = TimeUtil::TimestampToStandardFormatCustomer((UInt32)CURRENT_TIME.Sec());
		fprintf(m_PacketCountFile, "***************[%s]***************\n", curTimeStr.c_str());

		for (auto& iter : m_packetCountMap)
		{
			fprintf(m_PacketCountFile, "packetId:[%u] count:[%u] totalSize:[%u]. \n", iter.first, iter.second.num, iter.second.totalSize);
		}

		fprintf(m_PacketCountFile, "\n\n");

		m_packetCountMap.clear();
		fflush(m_PacketCountFile);
	}
}

void GSPlayer::PacketCount(UInt32 packetId, UInt32 packetSize)
{
	// 统计玩家断线的包
	if ( !GSApplication::Instance()->IsCountPacket(GetID()))
	{
		return;
	}

	m_packetCountMap[packetId].num++;
	m_packetCountMap[packetId].totalSize += packetSize;
}

UInt32 GSPlayer::DeleteRole(ObjID_t roleId)
{
	for(auto role=m_Roles.begin(); role!=m_Roles.end();++role)
	{


		if (role->id != roleId)
		{
			continue;
		}

		if (role->deleteTime != 0)
		{
			return ErrorCode::ENTERGAME_DELETE_ROLE_DELETED;
		}

		if (role->level >= DIRECT_DELETE_PLAYER_LEVEL && GetDeletedRoleNum() >= MAX_DELETING_PLAYER_NUM)
		{
			return ErrorCode::ENTERGAME_DELETE_ROLE_MAX_NUM;
		}

		UInt32 currentTime = (UInt32)CURRENT_TIME.Sec();
		if (m_RoleDeleteTime > 0)
		{
			UInt32 intervalTime = currentTime - m_RoleDeleteTime;
			UInt32 roleUpdateLimitTime = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_ROLE_DELETE_LIMIT_TIME) * DAY_TO_SEC;

			if (IsLimitRoleUpdate(8505, intervalTime, roleUpdateLimitTime)) //8505通用提示表中的ID
			{
				return ErrorCode::ENTERGAME_DELETE_ROLE_LIMIT;
			}
		}
		
		InfoLogStream << "account(" << GetAccID() << ") delete player(" << roleId << ")." << LogStream::eos;

		CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_player_info", roleId, true);
		updateCmd->PutData("deletetime", currentTime);
		updateCmd->PutData("add_preferences_time", 0);
		updateCmd->PutData("del_preferences_time", 0);
		CLRecordClient::Instance()->SendCommand(updateCmd);

		CLUpdateCommand* updateAccCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_account", m_AccountGuid, true);
		updateAccCmd->PutData("role_delete_time", currentTime);
		CLRecordClient::Instance()->SendCommand(updateAccCmd);

		SetRoleDeleteTime(currentTime);

		std::string reason = GetReason(SOURCE_TYPE_DELETE_ROLE, roleId, GetAccID());
		NotifyRecalculateTotalRoleValueScore(reason);
		NotifyLeaveGame(roleId);

		role->deleteTime = UInt32(CURRENT_TIME.Sec());
		//置空角色偏好
		role->addPreferencesTime = 0;
		role->delPreferencesTime = 0;
		SendDeleteUdpLog(role->deleteTime);
		return ErrorCode::SUCCESS;
	}

	return ErrorCode::ENTERGAME_DELETE_ROLE_UNEXIST;
}

UInt32 GSPlayer::RecoverRole(ObjID_t roleId)
{
	if (GSServiceSwitchMgr::Instance()->IsOpen(SERVICE_ROLE_FILED))
	{
		if (RoleFieldReachUpperLimit())
		{
			return ErrorCode::ENTERGAME_ROLE_FIELD_REACN_UPPER_LIMIT;
		}
	}

	for(auto& role : m_Roles)
	{
		if (role.id != roleId)
		{
			continue;
		}

		if (role.deleteTime == 0)
		{
			return ErrorCode::ENTERGAME_RECOVER_ROLE_NOT_DELETE;
		}
		
		if (role.IsForeverDeleted())
		{
			return ErrorCode::ENTERGAME_RECOVER_ROLE_DELETED;
		}

		UInt32 currentTime = (UInt32)CURRENT_TIME.Sec();
		if (m_RoleRecoverTime > 0)
		{
			UInt32 intervalTime = currentTime - m_RoleRecoverTime;
			UInt32 roleUpdateLimitTime = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_ROLE_RECOVER_LIMIT_TIME) * DAY_TO_SEC;

			if (IsLimitRoleUpdate(8506, intervalTime, roleUpdateLimitTime))
			{
				return ErrorCode::ENTERGAME_RECOVER_ROLE_LIMIT;
			}
		}

		CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_player_info", roleId, true);
		updateCmd->PutData("deletetime", 0);
		CLRecordClient::Instance()->SendCommand(updateCmd);

		CLUpdateCommand* updateAccCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_account", m_AccountGuid, true);
		updateAccCmd->PutData("role_recover_time", currentTime);
		CLRecordClient::Instance()->SendCommand(updateAccCmd);

		SetRoleRecoverTime(currentTime);
		role.deleteTime = 0;

		std::string reason = GetReason(SOURCE_TYPE_RECOVER_ROLE, roleId, GetAccID());
		NotifyRecalculateTotalRoleValueScore(reason);

		return ErrorCode::SUCCESS;
	}

	return ErrorCode::ENTERGAME_RECOVER_ROLE_UNEXIST;
}

UInt32 GSPlayer::GetRoleNum() const
{
	UInt32 roleNum = 0;
	for(RoleList::const_iterator iter = m_Roles.begin();
		iter != m_Roles.end(); ++iter)
	{
		if(iter->deleteTime == 0) ++roleNum;
	}
	return roleNum;
}

UInt32 GSPlayer::GetDeletedRoleNum()
{
	UInt32 roleNum = 0;
	for (auto role : m_Roles)
	{
		if (role.deleteTime != 0 && !role.IsForeverDeleted())
		{
			roleNum++;
		}
	}

	return roleNum;
}

UInt32 GSPlayer::GetTodayCreatedRoleNum()
{
	Avalon::Time nowTime = Avalon::Time::CurrentTime();
	Avalon::Date nowDate = Avalon::Date(nowTime);
	
	Avalon::Date beginDate(nowDate);
	beginDate.Hour(6);
	beginDate.Min(0);
	beginDate.Sec(0);
	Avalon::Time beginTime = beginDate.ToTime();
	if (beginTime.MSec() > nowTime.MSec())
	{
		beginTime -= Avalon::Time::SECS_OF_DAY * Avalon::Time::MSECS_OF_SEC;
	}

	Avalon::Time endTime = beginTime + Avalon::Time::SECS_OF_DAY * Avalon::Time::MSECS_OF_SEC;

	UInt32 roleNum = 0;
	for (auto& role : m_Roles)
	{
		if (role.createtime >= beginTime.MSec() / Avalon::Time::MSECS_OF_SEC &&
			role.createtime <= endTime.MSec() / Avalon::Time::MSECS_OF_SEC)
		{
			roleNum++;
		}
	}

	return roleNum;
}

UInt32 GSPlayer::CanCreateNewRole()
{
	UInt32 roleNum = GetRoleNum();
	/*if (roleNum == 1)
	{
		if (GetRoleMaxLevel() < 20)
		{
			return ErrorCode::ENTERGAME_NEED_LEVEL_20;
		}
	}
	else if (roleNum == 2)
	{
		if (GetRoleMaxLevel() < 40)
		{
			return ErrorCode::ENTERGAME_NEED_LEVEL_40;
		}
	}
	else */if (roleNum >= 8)
	{
		return ErrorCode::ENTERGAME_TOOMANY_ROLES;
	}

	return ErrorCode::SUCCESS;
}

UInt32 GSPlayer::SetPreferences(ObjID_t roleId, RolePreferences flag)
{
	if(flag == RolePreferences::RP_SELECT && GetPreferencesRoleNum() >= SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_PREFERENCES_ROLE_NUM))
	{
		ErrorLogStream <<"account(id:"<<m_Accid <<")"<<"Preference role reaches upper limit"<< LogStream::eos;
		return ErrorCode::ENTERGAME_PREFERENCES_ROLE_REACN_UPPER_LIMIT;
	}

	for (auto role = m_Roles.begin();role != m_Roles.end(); ++role)
	{
		if (role->id != roleId)continue;

		if (role->deleteTime != 0)
		{
			ErrorLogStream << "player(roleid: " << roleId << ") has been deleted!" << LogStream::eos;
			return ErrorCode::ENTERGAME_UNVALID_ROLEINFO;
		}

		if (flag == RolePreferences::RP_SELECT)
		{
			role->addPreferencesTime = UInt32(CURRENT_TIME.Sec());
			role->delPreferencesTime = 0;
		}
		else if (flag == RolePreferences::RP_CANCLE)
		{
			role->addPreferencesTime = 0;
			role->delPreferencesTime = UInt32(CURRENT_TIME.Sec());
		}
		else 
		{
			ErrorLogStream << "enumeration with a value of" << flag << "could not be found! (RolePreferences) " << LogStream::eos;
			return ErrorCode::ENTERGAME_UNVALID_ROLEINFO;
		}

		CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_player_info", roleId, false);
		updateCmd->PutData("add_preferences_time", role->addPreferencesTime);
		updateCmd->PutData("del_preferences_time", role->delPreferencesTime);

		CLRecordClient::Instance()->SendCommand(updateCmd);
		return ErrorCode::SUCCESS;
	}

	ErrorLogStream << "Find player(roleid: " << roleId << ") failed!" << LogStream::eos;
	return ErrorCode::ENTERGAME_UNVALID_ROLEINFO;
}

UInt32 GSPlayer::GetPreferencesRoleNum()
{
	UInt32 num = 0;

	for (auto role = m_Roles.begin(); role != m_Roles.end(); ++role)
	{
		if (role->addPreferencesTime > 0)++num;
	}

	return num;
}

void GSPlayer::StartToQueryRoleInfo(UInt8 enterGameMode)
{
	m_Roles.clear();

	CLSelectCommand* cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_player_info");
	cmd->PutColumn("name");
	cmd->PutColumn("level");
	cmd->PutColumn("sex");
	cmd->PutColumn("occu");
	cmd->PutColumn("sceneid");
	cmd->PutColumn("mapid");
	cmd->PutColumn("pos");
	cmd->PutColumn("tourzone");
	cmd->PutColumn("createtime");
	cmd->PutColumn("savetime");
	cmd->PutColumn("deletetime");
	cmd->PutColumn("storagesize");
	cmd->PutColumn("avatar");  
	cmd->PutColumn("newboot");
	cmd->PutColumn("born_zoneid");
	cmd->PutColumn("pre_occu");
	cmd->PutColumn("is_appointment_occu");
	cmd->PutColumn("return_time");
	cmd->PutColumn("return_level");
	cmd->PutColumn("novice_guide_choose_flag");
	cmd->PutColumn("add_preferences_time");
	cmd->PutColumn("del_preferences_time");

	cmd->PutCond("accid", m_Accid);
	std::ostringstream limitoss;
	limitoss << " AND (`deletetime` = 0 OR `deletetime` > " << UInt32(CURRENT_TIME.Sec()) - DELETE_PLAYER_SAVE_TIME << ")";
	cmd->SetLimit(limitoss.str());

	DebugLogStream << "player(" << m_Accid << ") start to query role list." << LogStream::eos;
	CLRecordClient::Instance()->SendCommand(cmd, new SelectRolesCallback(m_Accid, enterGameMode));
}

void GSPlayer::BeginQueryAccountInfo(UInt8 enterGameMode)
{
	GSPlayerMgr::Instance()->DelFromWaitList(GetAccID());
	SetStatus(GPS_WAITROLEINFO);

	DebugLogStream << "start to query account(" << GetAccID() << ") info." << LogStream::eos;
	// 先查询账号信息
	CLSelectCommand* cmd = CLRecordClient::Instance()->PrepareSelectCommand("t_account");
	cmd->PutCond("accid", GetAccID());
	CLRecordClient::Instance()->SendCommand(cmd, new QueryAccountCallback(GetAccID(), enterGameMode));
}

bool GSPlayer::BeginEnterGame(ObjID_t roleId, UInt8 option, const std::string& city, UInt32 inviter)
{
	if (GetStatus() != GPS_SELECTROLE)
	{
		ErrorLogStream << "player(" << GetAccID() << ", " << roleId << ") is not in select role status(" << (UInt8)GetStatus() << ")." << LogStream::eos;
		return false;
	}
	if (m_Roles.empty())
	{
		ErrorLogStream << "player(" << GetAccID() << ", " << roleId << ") no role to choose." << LogStream::eos;
		return false;
	}

	RoleList::iterator iter = m_Roles.begin();
	 GSRoleInfo mRole ;
	for(;iter != m_Roles.end(); ++iter)
	{
		if(iter->id == roleId)
		{
			mRole = *iter;
			if (iter->deleteTime != 0)
			{
				ErrorLogStream << "player(" << GetAccID() << ", " << roleId << ") this role is deleted." << LogStream::eos;
				return false;
			}
			else
			{
				break;
			}
		}
	}
	if (iter == m_Roles.end())
	{
		ErrorLogStream << "player(" << GetAccID() << ", " << roleId << ") can't find this role." << LogStream::eos;
		return false;
	}

	if (GetID() != 0 && GetID() != roleId)
	{
		// 说明换角色登陆了，要把原来的角色下线
		GSPlayerMgr::Instance()->RemoveGuidPlayer(this);
		m_packetBuffer.Clear();
	}

	// 先发一个离开比赛的消息到world server
	if (GetID() != 0)
	{
		NotifyLeaveGame(GetID());
	}

	SetID(iter->id); //noted by aprilliu, 设置GSPlayer的 Object_ID为 角色id(一个guid，在创建角色的时候生成的)
	SetName(iter->name);
	SetRoleReturnTime(iter->returnTime);
	SetRoleReturnLevel(iter->returnLevel);

	if (!GSPlayerMgr::Instance()->FindPlayerByID(this->GetID()) && !GSPlayerMgr::Instance()->AddGuidPlayer(this))
	{
		ErrorLogStream << "player(" << GetAccID() << ", " << roleId << ") add to playermgr failed." << LogStream::eos;
		return false;
	}

	m_ClientOption = option;
	if(city.size() < 32) m_City = city;
	m_LastMapID = iter->mapid;
	m_LastSceneID = iter->sceneid;
	m_Level = iter->level;
	m_LastPos = iter->pos;
	m_Inviter = inviter;
	m_TourZoneId = iter->tourZoneId;
	m_BornZoneId = iter->bornZoneId;
	m_Occu = Occupation(iter->occu);


	struct cmp
	{
		bool operator()(const GSRoleInfo& a, const  GSRoleInfo &b) const
		{
			if (a.addPreferencesTime != 0 && b.addPreferencesTime != 0)return a.addPreferencesTime < b.addPreferencesTime;
			if (a.addPreferencesTime != 0 || b.addPreferencesTime != 0)return a.addPreferencesTime != 0;
			if (a.delPreferencesTime != 0 || b.delPreferencesTime != 0)return a.delPreferencesTime > b.delPreferencesTime;
			return a.id < b.id;
		}
	};
	UInt32 idx = 0;
	for (auto it : m_Roles)
	{
		if (cmp()(it, mRole))
		{
			++idx;
		}
	}
	SendRolePreferencesUdpLog(mRole.addPreferencesTime > 0 ,  idx+1);

	m_Roles.clear();

	SetStatus(GPS_ENTERGAME);

	CLProtocol::GateSyncServerTime sync;
	sync.time = UInt32(CURRENT_TIME.Sec());
	SendProtocol(sync);

	EnterGame();

	// 清除再次回归的标记
	SetAgainReturn(false);

	GSPlayerMgr::Instance()->DelFromWaitList(GetAccID());

	/*if(m_TourZoneId != 0 && m_TourZoneId != ZONE_ID)
	{
		CheckChangeZone(m_TourZoneId, m_LastMapID);
	}
	else
	{
		SetStatus(GPS_ENTERGAME);

		CLProtocol::GateSyncServerTime sync;
		sync.time = UInt32(CURRENT_TIME.Sec());
		SendProtocol(sync);

		EnterGame();
	}*/
	return true;
}

bool GSPlayer::BeginEnterGameForTour(UInt8 option, const std::string& city, UInt32 inviter)
{
	if(GetStatus() != GPS_SELECTROLE) return false;
	if(GetID() == 0) return false;

	if(!GSPlayerMgr::Instance()->AddGuidPlayer(this)) return false;

	m_ClientOption = option;
	if(city.size() < 32) m_City = city;
	m_Inviter = inviter;

	SetStatus(GPS_ENTERGAME);

	CLProtocol::GateSyncServerTime sync;
	sync.time = UInt32(CURRENT_TIME.Sec());
	SendProtocol(sync);

	EnterGame();
	return true;
}

void GSPlayer::OnFinishNewbeeGuide(ObjID_t roleId, UInt32 id)
{
	DebugLogStream << "player(" << roleId << ") finish newbie guild(" << id << ")." << LogStream::eos;
	if (GetStatus() != GPS_SELECTROLE)
	{
		ErrorLogStream << "player(" << roleId << ") want finish newbie guild(" << id << ") invalid status(" << (UInt8)GetStatus() << ")." << LogStream::eos;
		return;
	}

	for (auto& role : m_Roles)
	{
		if (role.id != roleId)
		{
			continue;
		}

		//新手引导
		if (id > role.newBoot && role.newBoot == 0)
		{
			auto cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_player_info", roleId, true);
			if (cmd)
			{
				DebugLogStream << "player(" << roleId << ") set new boot(" << id << ")." << LogStream::eos;
				cmd->PutData("newboot", id);
				CLRecordClient::Instance()->SendCommand(cmd);
				role.newBoot = id;
			}
		}
	}
}

void GSPlayer::EnterGame()
{
	CLProtocol::SceneEnterGameReq req;
	req.id = GetID();
	req.accid = GetAccID();
	req.name = GetName();
	req.srcZoneId = m_SrcZoneId;
	req.gmauthority = GetGmAuthority();
	req.clientOption = m_ClientOption;
	req.openid = m_OpenId;
	req.city = m_City;
	req.qqVipInfo = m_qqVipInfo;
	req.ip = m_Ip;
	req.source1 = m_Source1;
	req.source2 = m_Source2;
	req.inviter = m_Inviter;
	req.pf = m_Pf;
	req.did = m_DeviceId;
	req.model = m_Model;
	req.deviceVersion = m_DeviceVersion;
	req.mapid = GetLastMap();
	req.sceneid = GetLastScene();
	req.pos = GetLastPos();
	req.gatenode = NODE_ID;
	req.isPhoneBind = (m_PhoneBindRoleId != 0 && m_PhoneBindRoleId == GetID()) ? 1 : 0;
	req.bornZoneId = m_BornZoneId;
	AccountInfo accInfo;
	GSAccountMgr::Instance()->GetAccount(GetAccID(), accInfo);
	req.antiAddictInfo = accInfo.antiAddictInfo;
	req.roleReturnTime = GetRoleReturnTime();
	req.roleReturnLevel = GetRoleReturnLevel();
	req.isAgainReturn = GetAgainReturn();

	Router::SendToWorld(req);

    DebugLogStream << "account(" << GetAccID() << ") player(" << GetID() << ") request enter game." << LogStream::eos;
}

UInt32 GSPlayer::GetRoleMaxLevel()
{
	UInt32 level = 0;
	for (auto& role : m_Roles)
	{
		if (role.deleteTime)
		{
			continue;
		}

		level = role.level > level ? role.level : level;
	}

	return level;
}

void GSPlayer::EndEnterGame()
{
	if(GetStatus() != GPS_ENTERGAME) return;

	SetStatus(GPS_NORMAL);
}

void GSPlayer::SendRoleInfo()
{
	//跟着角色信息下发系统开关
	NotifySysSwitch();
	struct cmp
	{
		bool operator()(CLProtocol::RoleInfo &a, CLProtocol::RoleInfo &b) const
		{
			if (a.addPreferencesTime != 0 && b.addPreferencesTime != 0)return a.addPreferencesTime < b.addPreferencesTime;
			if (a.addPreferencesTime != 0 || b.addPreferencesTime != 0)return a.addPreferencesTime != 0;
			if (a.delPreferencesTime != 0 || b.delPreferencesTime != 0)return a.delPreferencesTime > b.delPreferencesTime;
			return a.roleId < b.roleId;
		}
	};

	CLProtocol::GateSendRoleInfo protocol;
	for (RoleList::iterator iter = m_Roles.begin(); iter != m_Roles.end(); iter++)
	{
		CheckRoleAlreadyInAppointment(*iter);

		if (iter->deleteTime != 0 && iter->IsForeverDeleted())
		{
			continue;
		}

		CLProtocol::RoleInfo info;
		info.roleId = iter->id;
		info.strRoleId = Avalon::StringUtil::ConvertToString<UInt64>(iter->id);
		info.name = iter->name;
		info.sex = iter->sex;
		info.occu = iter->occu;
		info.level = iter->level;
		info.offlineTime = iter->offlineTime;
		
		info.addPreferencesTime = iter->addPreferencesTime;
		info.delPreferencesTime = iter->delPreferencesTime;
		if(iter->deleteTime != 0)
		{
			info.deleteTime = iter->deleteTime;
			//info.deleteTime = UInt32(iter->deleteTime + DAY_TO_SEC - CURRENT_TIME.Sec()) ;
		}
		info.avatar = iter->avatar;
		info.newBoot = iter->newBoot;
		info.preOccu = iter->preOccu;
		info.isAppointmentOccu = iter->isAppointmentOccu;
		info.playerLabelInfo = iter->playerLabelInfo;

		protocol.roles.push_back(info);
	}
	std::sort(protocol.roles.begin(), protocol.roles.end(), cmp());
	protocol.appointmentOccus = GSActivityMgr::Instance()->GetAppointmentOccus();
	protocol.appointmentRoleNum = GetAppointmentRoleNum();

	if (GSServiceSwitchMgr::Instance()->IsOpen(SERVICE_ROLE_FILED))
	{
		protocol.baseRoleField = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_ROLE_BASE_FIELD);
		protocol.extensibleRoleField = GSPlayerMgr::Instance()->GetSystemOpenedExtensibleRoleFields();
		protocol.unlockedExtensibleRoleField = GetUnlockedExtensiblRoleField();
	}

	SendProtocol(protocol);

	// 发送到Center
	SendRoleInfoToCenter();
}

void GSPlayer::SendRoleInfoToCenter()
{
	CLProtocol::CenterReportPlayerRoleList report;
	report.platform = GetPf();
	report.openid = GetOpenId();
	report.zoneid = ZONE_ID;
	for (RoleList::iterator iter = m_Roles.begin(); iter != m_Roles.end(); iter++)
	{
		if (iter->deleteTime != 0)
		{
			continue;
		}

		CLProtocol::RoleInfo info;
		info.roleId = iter->id;
		info.strRoleId = Avalon::StringUtil::ConvertToString<UInt64>(iter->id);
		info.name = iter->name;
		info.sex = iter->sex;
		info.occu = iter->occu;
		info.level = iter->level;
		info.offlineTime = iter->offlineTime;
		info.avatar = iter->avatar;
		info.newBoot = iter->newBoot;
		report.roles.push_back(info);
	}

	Router::SendToCenter(report);
}

void GSPlayer::CheckChangeZone(UInt32 dstZoneId, UInt32 dstMapId)
{
	if(dstZoneId == ZONE_ID) return;

	CLProtocol::GateChangeZoneCheckReq req;
	req.srcZoneId = ZONE_ID;
	req.srcGateId = NODE_ID;
	req.bReturn = dstZoneId == GetSrcZoneId() ? 1 : 0;
	req.accId = GetAccID();
	req.playerId = GetID();
	req.name = GetName();
	req.mapId = dstMapId;
	req.gmAuthority = GetGmAuthority();
	req.openid = GetOpenId();
	req.qqvipinfo = GetQQVip();
	req.source1 = GetSource1();
	req.source2 = GetSource2();
	req.pf = GetPf();
	req.level = GetLevel();
	Router::SendToZone(dstZoneId, req);
}

void GSPlayer::SetOfflineTimer(UInt32 sec)
{
    m_offlineTimer.SetLastTime(CURRENT_TIME.MSec());
    m_offlineTimer.SetInterval(sec * Avalon::Time::MSECS_OF_SEC);
}

bool GSPlayer::IsOfflineTimeout(const Avalon::Time& now)
{
    return m_offlineTimer.IsTimeout(now);
}

void GSPlayer::NotifyLeaveGame(ObjID_t roleid, UInt8 isOfflineTimeout)
{
	CLProtocol::WorldLeaveGameReq sceneReq;
	sceneReq.id = roleid;
	sceneReq.accid = GetAccID();
	sceneReq.isOfflineTimeOut = isOfflineTimeout;
	Router::SendToWorld(sceneReq);
}

void GSPlayer::NotifyDisconnect()
{
	CLProtocol::GateNotifyDisconnect notify;
	notify.id = GetID();
	Router::SendToWorld(notify);
}

void GSPlayer::NotifyReconnectSuccess()
{
	CLProtocol::GateNotifyReconnectSuccess notify;
	notify.id = GetID();
	Router::SendToWorld(notify);
}

void GSPlayer::SyncAdventureTeamInfo()
{
	CLProtocol::AdventureTeamInfoSync protocol;
	protocol.info.adventureTeamName = m_AdventureTeamInfo.adventureTeamName;
	protocol.info.adventureTeamLevel = m_AdventureTeamInfo.adventureTeamLevel;
	SendProtocol(protocol);
}

void GSPlayer::CheckRoleAlreadyInAppointment(GSRoleInfo& info)
{
	if (!info.isAppointmentOccu)
	{
		return;
	}

	auto baseOccu = JobDataEntry::GetBaseOccu(info.occu);
	if (GSActivityMgr::Instance()->IsOccuInAppointmentOccuActivity(baseOccu, info.createtime))
	{
		return;
	}

	info.isAppointmentOccu = 0;

	// 发送sql语句
	/*auto cmd = CLRecordClient::Instance()->PrepareUpdateCommand(0, "t_player_info", info.id, true);
	cmd->PutData("is_appointment_occu", info.isAppointmentOccu);
	CLRecordClient::Instance()->SendCommand(cmd);*/

	InfoLogStream << "player(" << info.id << ") is not in appointment now..." << LogStream::eos;
}

UInt32 GSPlayer::GetAppointmentRoleNum()
{
	UInt32 num = 0;
	for (auto& role : m_Roles)
	{
		CheckRoleAlreadyInAppointment(role);

		if (role.isAppointmentOccu && !role.IsForeverDeleted())
		{
			num++;
		}
	}

	return num;
}

bool GSPlayer::RoleFieldReachUpperLimit()
{
	//当前拥有的角色数量
	UInt32 ownRoles = GetRoleNum();
	//系统开放的基础栏位
	UInt32 systemOpenedBaseRoleFields = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_ROLE_BASE_FIELD);
	//可使用的角色栏位
	UInt32 canUseRoleFields = systemOpenedBaseRoleFields + GetUnlockedExtensiblRoleField();

	if (ownRoles >= canUseRoleFields)
	{
		ErrorLogStream << PLAYERINFO(this) << " role field reach upper limit(" << canUseRoleFields << ")!" << LogStream::eos;
		return true;
	}
	return false;
}

void GSPlayer::NotifySysSwitch()
{
	CLProtocol::GateNotifySysSwitch protocol;
	std::vector<SystemSwitchData*> infos;
	SystemSwitchDataMgr::Instance()->GetCloseSysInfo(infos);
	for (size_t i = 0; i < infos.size(); ++i)
	{
		SysSwitchCfg cfg;
		cfg.sysType = infos[i]->id;
		cfg.switchValue = infos[i]->value;
		protocol.cfgs.push_back(cfg);
	}
	SendProtocol(protocol);
}

void GSPlayer::OnTick(const Avalon::Time& now)
{
	if(m_SecTimer.IsTimeout(now))
	{
		if(GetStatus() == GPS_ENTERGAME)
		{
		
			--m_TryTimes;

			if(m_TryTimes == 0)
			{
				ErrorLogStream << "player(" << GetID() << "," << GetName() << ") enter game timeout!" << LogStream::eos;
				Disconnect();
				return;
			}
			else if(m_TryTimes == 3 || m_TryTimes == 6)
			{
				//EnterGame();
			}
		}
	}

	if(m_TenSecTimer.IsTimeout(now))
	{
		CLProtocol::GateSyncServerTime sync;
		sync.time = UInt32(CURRENT_TIME.Sec());
		SendProtocol(sync);
	}

	if (m_MinSecTimer.IsTimeout(now))
	{
		// 输出消息统计到文件
		RecordPacketCountToFile();
	}
}

void GSPlayer::NotifyNodeRoleConnect(bool isLeave)
{
	CLProtocol::GateNotifyLeaveConnect req;
	req.roleId = GetID();
	req.leaveTime = isLeave ? CURRENT_TIME.Sec() : 0;
	SendToSceneServer(req);
}

void GSPlayer::SendDeleteUdpLog(UInt32 deleteTime)
{
	SendUdpLog("delete", LOG_DELETE_FORMAT, deleteTime);
}

void GSPlayer::SendCreateUdpLog(std::string name, UInt8 occu, ObjID_t roleId, UInt8 preOccu, UInt8 isAppointMentOccu)
{
	SendUdpLog("create", LOG_CREATE_FORMAT, name.c_str(), occu, roleId, preOccu, isAppointMentOccu);
}

void GSPlayer::SendAccountLoginUdpLog()
{
	//系统开放的基础栏位
	UInt32 systemOpenedBaseRoleFields = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_ROLE_BASE_FIELD);
	//可使用的角色栏位
	UInt32 canUseRoleFields = systemOpenedBaseRoleFields + GetUnlockedExtensiblRoleField();

	SendUdpLog("account_login", LOG_ACCOUNT_LOGIN_FORMAT, canUseRoleFields);
}

void GSPlayer::SendRolePreferencesUdpLog(UInt32 isPreferences, UInt32 roleLocation)
{
	SendUdpLog("role_preferences", LOG_ROLE_PREFERENCES, isPreferences, roleLocation);
}

void GSPlayer::SendProtocol(Avalon::Protocol &protocol)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	if(protocol.Encode(packet))
	{
		SendPacket(packet);
	}
	Avalon::Packet::Destroy(packet); 
}

void GSPlayer::SendPacket(Avalon::Packet *packet)
{
	// 断开连接也需要把包存起来
	if ( !GSApplication::Instance()->IsFilterPacketBuffer(packet->GetID()))
	{
		packet->SetSequence(GenPacketSequence());
	}

	auto srcPacket = packet;

	// 地下城开始返回消息需要加密
	if (GSApplication::Instance()->IsEncryptProtocol() && (packet->GetID() == CLProtocol::SCENE_DUNGEON_START_RES || packet->GetID() == CLProtocol::SCENE_NOTIFY_ENTERSCENE))
	{
		unsigned char key[16];
		memset(key, 0, sizeof(key));
		for (int j = 0; j < 4; j++)
		{
			UInt32 seed = htonl(packet->GetSequence());
			for (int i = 0; i < 4; i++)
			{
				UInt8 value = (UInt8)(seed & 0xFF);
				key[i] = value == 0 ? 255 : value;
				seed = seed >> 8;
			}
		}
		
		size_t size = 0;
		auto outBuffer = xxtea_encrypt(packet->GetBuffer(), packet->GetSize(), key, (size_t*)&size);
		if (!outBuffer || size > Avalon::Packet::MAX_BODY_SIZE)
		{
			ErrorLogStream << "player(" << GetID() << ") create encrypt packet failed, need size(" << (UInt32)size << ")." << LogStream::eos;
			return;
		}

		Avalon::Packet* tmpPacket = Avalon::Packet::Create();
		if (!tmpPacket)
		{
			free(outBuffer);
			ErrorLogStream << "player(" << GetID() << ") create encrypt packet failed, need size(" << (UInt32)size << ")." << LogStream::eos;
			return;
		}

		packet->CopyTo(tmpPacket);
		tmpPacket->SetSequence(packet->GetSequence());
		memcpy(tmpPacket->GetBuffer(), outBuffer, size);
		tmpPacket->SetSize(size);
		packet = tmpPacket;

		free(outBuffer);
	}

	m_packetBuffer.StorePacket(packet);

	PacketCount(packet->GetID(), packet->GetSize());

	if (m_Conn == NULL)
	{
		if (packet != srcPacket)
		{
			Avalon::Packet::Destroy(packet);
		}

		return;
	}

	//黑名单过滤聊天和其他请求
	if(packet->GetID() == CLProtocol::SCENE_SYNC_CHAT)
	{
		Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());
		UInt8 channel = 0;
		ObjID_t fromId = 0;
		try
		{
			stream & channel & fromId;
		}
		catch(Avalon::NetException&)
		{
			return;
		}
		if(IsInBlackList(fromId)) return;
	}
//黑名单不过滤请求
// 	else if(packet->GetID() == CLProtocol::SCENE_SYNC_REQUEST)
// 	{
// 		Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());
// 		UInt8 type = 0;
// 		ObjID_t	requester = 0;
// 		try
// 		{
// 			stream & type & requester;
// 		}
// 		catch(Avalon::NetException&)
// 		{
// 			return;
// 		}
// 		if(IsInBlackList(requester)) return;
// 	}

	RecordRecord(packet, false);

	m_Conn->SendPacket(packet);

	if (packet != srcPacket)
	{
		Avalon::Packet::Destroy(packet);
	}
}

void GSPlayer::SendToSceneServer(Avalon::Protocol& protocol)
{
	if(m_pScene != NULL)
	{
		m_pScene->SendToSceneServer(protocol);
	}
}

void GSPlayer::TransToSceneServer(Avalon::Protocol& protocol)
{
	CLProtocol::GateTransmitPacket trans;
	if(protocol.Encode(trans.packet))
	{
		trans.id = GetID();
		SendToSceneServer(trans);
	}
}

void GSPlayer::TransToWorldServer(Avalon::Protocol& protocol)
{
	CLProtocol::GateTransmitPacket trans;
	if(protocol.Encode(trans.packet))
	{
		trans.id = GetID();
		Router::SendToWorld(trans);
	}
}

void GSPlayer::SendToGlobalActivity(Avalon::Protocol& protocol)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	if (protocol.Encode(packet))
	{
		SendToGlobalActivity(packet);
	}
	Avalon::Packet::Destroy(packet);
}

void GSPlayer::SendToGlobalActivity(Avalon::Packet* packet)
{
	CLProtocol::SysTransmitPlayerFormNodesToGlobal transmit(packet);
	transmit.playerInfo.roleId = GetID();
	transmit.playerInfo.accId = GetAccID();
	transmit.playerInfo.name = GetName();
	transmit.playerInfo.platform = GetPf();
	transmit.playerInfo.serverZoneId = ZONE_ID;
	transmit.playerInfo.serverName = SERVER_NAME;
	transmit.playerInfo.sourceNode[ST_GATE] = NODE_ID;
	transmit.playerInfo.sourceNode[ST_SCENE] = GetScene() == NULL ? 0 : GetScene()->GetNodeID();

	transmit.destNode = GetGlobalActivityNodeID(0);

	Router::SendToAdmin(transmit);
	transmit.Detach();
}

void GSPlayer::SendToBattle(Avalon::Protocol& protocol)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	if (protocol.Encode(packet))
	{
		SendToBattle(packet);
	}
	Avalon::Packet::Destroy(packet);
}

void GSPlayer::SendToBattle(Avalon::Packet* packet)
{
	CLProtocol::SysTransmitInNodes transmit(packet);
	transmit.destnode = CL_MAKE_NODEID(0, ST_BATTLE, 0);

	Router::SendToAdmin(transmit);
	transmit.Detach();
}

void GSPlayer::SendToTeamCopy(Avalon::Protocol& protocol)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	if (protocol.Encode(packet))
	{
		SendToTeamCopy(packet);
	}
	Avalon::Packet::Destroy(packet);
}

void GSPlayer::SendToTeamCopy(Avalon::Packet* packet)
{
	CLProtocol::SysTransmitPlayerToTeamCopy transmit(packet);
	transmit.playerId = GetID();
	Router::SendToAdmin(transmit);
	transmit.Detach();
}


void GSPlayer::SendToUnion(Avalon::Protocol& protocol)
{
	Avalon::Packet* packet = Avalon::Packet::Create();
	if (protocol.Encode(packet))
	{
		SendToUnion(packet);
	}
	Avalon::Packet::Destroy(packet);
}


void GSPlayer::SendToUnion(Avalon::Packet* packet)
{
	UInt32 funType = GSUnionProtocolMgr::Instance()->GetProcotolUnionType(packet->GetID());
	if (funType == USFT_NONE)
	{
		ErrorLogStream << PLAYERINFO(this) << "union fun not find error!!! packet id:" << packet->GetID() << LogStream::eos;
		return;
	}

	CLProtocol::SysTransmitPlayerToUnion transmit(packet);
	transmit.funType = funType;
	transmit.accId = GetAccID();
	transmit.playerId = GetID();
	transmit.sourceNode[ST_GATE] = NODE_ID;
	transmit.sourceNode[ST_SCENE] = GetScene() == NULL ? 0 : GetScene()->GetNodeID();
	Router::SendToAdmin(transmit);
	transmit.Detach();
}

void GSPlayer::Disconnect()
{
	SetStatus(GPS_FINAL);
	if(m_Conn != NULL)
	{
		m_Conn->Close();
	}

	NotifyNodeRoleConnect();
}

UInt32 GSPlayer::BeginReconnect(Avalon::NetConnectionPtr conn, std::vector<UInt8> session, UInt32 sequence)
{
	if (conn == NULL)
	{
		return ErrorCode::RECONNECT_NO_CONNECTION;
	}

	/*auto oldConn = GSExternalNetwork::Instance()->FindConnection(GetAccID());
	if (GetStatus() != GPS_OFFLINE && (oldConn != NULL && oldConn->GetStatus() == Avalon::NCS_NORMAL))
	{
		InfoLogStream << "player(" << GetAccID() << ", " << GetID() << ") reconnect failed, status(" << (UInt8)GetStatus() << ")." << LogStream::eos;
		return ErrorCode::RECONNECT_PLAYER_ONLINE;
	}*/

	if (m_Session != session)
	{
		InfoLogStream << "player(" << GetAccID() << ", " << GetID() << ") reconnect failed, invalid session." << LogStream::eos;
		return ErrorCode::RECONNECT_PLAYER_ONLINE;
	}

	if (!m_packetBuffer.IsValidSequence(sequence))
	{
		InfoLogStream << "player(" << GetAccID() << ", " << GetID() << ") reconnect failed, invalid sequence(" << sequence << ") minSequence("
			<< m_packetBuffer.GetMinSequence() << ")." << LogStream::eos;
		return ErrorCode::RECONNECT_INVALID_SEQUENCE;
	}

	DebugLogStream << "player(" << GetAccID() << ", " << GetID() << ") begin reconnect, sequence(" << sequence << ") lastRecvSequence(" << m_LastRecvSequence << ")." << LogStream::eos;

	do
	{
		UInt8 buffer[Avalon::Packet::MAX_BODY_SIZE];
		Avalon::NetOutputStream stream(buffer, sizeof(buffer));
		auto newSequence = m_packetBuffer.FetchPacket(sequence, stream);
		if (newSequence == sequence)
		{
			ErrorLogStream << "player(" << GetAccID() << ", " << GetID() << ") fetch packet failed, start sequence(" << sequence << ")." << LogStream::eos;
			return ErrorCode::RECONNECT_INVALID_SEQUENCE;
		}

		sequence = newSequence;
		if (stream.GetPos() > 0)
		{
			conn->Send(buffer, stream.GetPos());
		}
		
	} while (sequence > 0);
	
	if (GetID() != 0 && !GSPlayerMgr::Instance()->FindPlayerByID(GetID()))
	{
		GSPlayerMgr::Instance()->AddGuidPlayer(this);
	}
	
	SetConn(conn);
	SetStatus(GPS_RECONNECT);
	NotifyNodeRoleConnect(false);

	return ErrorCode::SUCCESS;
}

void GSPlayer::RecordRecord(Avalon::Packet* packet, bool isRecv)
{
	if (!m_IsMonitored || !packet)
	{
		return;
	}

	if (!m_NetSimpleFile)
	{
		std::string filename = std::string("Monitor/Simple_") + Avalon::StringUtil::ConvertToString(GetAccID()) + ".txt";
		m_NetSimpleFile = fopen(filename.c_str(), "w");
	}

	if (m_NetSimpleFile)
	{
		fprintf(m_NetSimpleFile, "[%llu] player(%u,%llu) %s packet(%u) len(%d).\n", CURRENT_TIME.MSec(), GetAccID(), GetID(), isRecv ? "recv" : "send", packet->GetID(), packet->GetPacketSize());
		//fflush(m_NetSimpleFile);
	}

	if (!m_NetDetailFile)
	{
		std::string filename = std::string("Monitor/Detail_") + Avalon::StringUtil::ConvertToString(GetAccID()) + ".txt";
		m_NetDetailFile = fopen(filename.c_str(), "wb");
	}

	if (m_NetDetailFile)
	{
		Avalon::UInt8* buffer = packet->GetBuffer() - Avalon::Packet::HEAD_SIZE;  //得到消息头部的起始位置， 创建Packet时已经给头部预留出HEAD_SIZE（2+4）个字节
		packet->EncodeHeader();
		fwrite(buffer, packet->GetPacketSize(), 1, m_NetDetailFile);
		//fflush(m_NetDetailFile);
	}
}

void GSPlayer::UpdateRoleReturnInfo(UInt64 roleId, UInt32 time)
{
	for (auto& iter : m_Roles)
	{
		if (iter.id != roleId)
			continue;
		
		iter.returnTime = time;
		iter.returnLevel = iter.level;

		CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_player_info", iter.id, true);
		cmd->PutData("return_time", time);
		cmd->PutData("return_level", iter.returnLevel);
		cmd->PutData("return_first_login", 1);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
}

void GSPlayer::UpdateAccountReturnTime()
{
	CLUpdateCommand* updateAccCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_account", GetAccountGuid(), true);
	updateAccCmd->PutData("veteran_return_time", m_AccountReturnTime);
	CLRecordClient::Instance()->SendCommand(updateAccCmd);
}

bool GSPlayer::NeedPopupNoviceGuideChoose()
{
	if (GetAdventureTeamInfo().adventureTeamName.empty())
	{
		return false;
	}

	UInt16 playerMaxLevel = PlayerDataEntryMgr::Instance()->GetMaxLevel();
	for (const auto& roleInfo : m_Roles)
	{
		// 筛掉删了的角色
		if (roleInfo.deleteTime != 0)
		{
			continue;
		}

		if (roleInfo.level >= playerMaxLevel)
		{
			return true;
		}
	}
	return false;
}

void GSPlayer::SetNoviceGuideChooseFlag(ObjID_t roleid, UInt8 chooseFlag)
{
	for (auto& role : m_Roles)
	{
		if (role.id != roleid)
		{
			continue;
		}

		auto cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_player_info", roleid, true);
		if (cmd)
		{
			cmd->PutData("novice_guide_choose_flag", chooseFlag);
			CLRecordClient::Instance()->SendCommand(cmd);
			role.playerLabelInfo.noviceGuideChooseFlag = chooseFlag;
			InfoLogStream << "player(" << roleid << ") set novice guide choose flag(" << (UInt32)chooseFlag << ")." << LogStream::eos;
			//SendRoleInfo();
		}
	}
}

void GSPlayer::NotifyRecalculateTotalRoleValueScore(const std::string& reason)
{
	CLProtocol::GateNotifyReCalculateTotalRoleValueScore protocol;
	protocol.reason = reason;
	protocol.accid = GetAccID();
	Router::SendToWorld(protocol);
}

bool GSPlayer::IsVeteranReturn()
{
	if (!GSServiceSwitchMgr::Instance()->IsOpen(ServiceType::SERVICE_NEW_RETURN))
	{
		return false;
	}

	if (!ActivityUtil::IsInVeteranReturnPeriod(GetRoleReturnTime()))
	{
		return false;
	}

	return true;
}