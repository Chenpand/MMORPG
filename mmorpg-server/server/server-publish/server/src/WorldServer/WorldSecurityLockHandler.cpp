#include "WorldSecurityLockHandler.h"
#include "WSPlayer.h"
#include "AccountMgr.h"
#include "WSNetwork.h"
#include "WSServiceSwitchMgr.h"

#include <CLSecurityLockDefine.h>
#include <CLRecordClient.h>

void HandleProtocol(WSPlayer* player, CLProtocol::WorldSecurityLockDataReq& protocol)
{
	if (NULL == player) return;
	player->HandlerSecurityLockDataReq(protocol.deviceID);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldSecurityLockOpReq& protocol)
{
	if (NULL == player) return;
	player->HandlerSecurityLockOpReq(protocol.lockOpType, protocol.passwd);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldChangeSecurityPasswdReq& protocol)
{
	if (NULL == player) return;
	player->ChangeSecurityPasswdReq(protocol.oldPasswd, protocol.newPasswd);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldBindDeviceReq& protocol)
{
	if (NULL == player) return;
	player->HandlerCommonDeviceReq(protocol.bindType, protocol.deviceID);
}

void HandleProtocol(CLProtocol::GateSecurityLockCheckReq& protocol)
{
	if (!WSServiceSwitchMgr::Instance()->IsOpen(SERVICE_SECURITY_LOCK))
	{
		CLProtocol::GateSecurityLockCheckRes res;
		res.accId = protocol.accId;
		res.roleId = protocol.roleId;
		res.errorNum = 0;
		res.ret = ErrorCode::SUCCESS;
		protocol.SendBack(res);
		return; 
	}

	auto info = AccountMgr::Instance()->FindAccount(protocol.accId);
	if (NULL == info)
	{
		// 去数据库加载
		class SelectAccountCheckSecurityLock : public AccountCallback
		{
		public:
			SelectAccountCheckSecurityLock(UInt32 accid, UInt32 _gateId, ObjID_t _roleId, const std::string& _devId)
				: AccountCallback(accid), gateConnId(_gateId), roleId(_roleId), deviceId(_devId) {}

			void OnFinish(AccountInfo* info)
			{
				UInt32 ret = ErrorCode::SUCCESS;

				do
				{
					if ( !deviceId.empty() && info->IsCommonDev(deviceId))
					{
						ret = ErrorCode::SUCCESS;
						break;
					}

					if (info->GetSecurityState() != SECURITY_STATE_UNLOCK)
					{
						ret = ErrorCode::SECURITY_LOCK_DEL_ROLE;
						break;
					}

				} while (0);

				// 检查是否跨天
				info->CheckSecurityLockCrossDay();
				SendToGate(ret, info->GetSecurityPasswdErrorNum());
			}

			void OnFailure()
			{
				SendToGate(ErrorCode::SECURITY_LOCK_FAILED, 0);
			}

			void SendToGate(UInt32 _ret, UInt32 _errNum)
			{
				auto conn = WSNetwork::Instance()->FindConnection(gateConnId);
				if (conn == NULL) return;

				CLProtocol::GateSecurityLockCheckRes res;
				res.accId = m_AccId;
				res.roleId = roleId;
				res.errorNum = _errNum;
				res.ret = _ret;

				Avalon::Packet* packet = Avalon::Packet::Create();
				if (res.Encode(packet))
				{
					conn->SendPacket(packet);
				}
				Avalon::Packet::Destroy(packet);
			}

		private:
			// 网关连接id
			UInt32 gateConnId;
			// 角色Id
			ObjID_t roleId;
			// 设备Id
			std::string deviceId;
		};

		AccountMgr::Instance()->LoadAccountInfo(protocol.accId, new SelectAccountCheckSecurityLock(protocol.accId, protocol.GetConnID(), protocol.roleId, protocol.deviceId));
	}
	else
	{
		// 检查是否跨天
		info->CheckSecurityLockCrossDay();

		CLProtocol::GateSecurityLockCheckRes res;
		res.accId = protocol.accId;
		res.roleId = protocol.roleId;
		res.errorNum = info->GetSecurityPasswdErrorNum();

		do
		{
			if (info->IsCommonDev(protocol.deviceId))
			{
				res.ret = ErrorCode::SUCCESS;
				break;
			}

			if (info->GetSecurityState() != SECURITY_STATE_UNLOCK)
				res.ret = ErrorCode::SECURITY_LOCK_DEL_ROLE;
			else
				res.ret = ErrorCode::SUCCESS;

		} while (0);

		protocol.SendBack(res);
	}
}

void HandleProtocol(CLProtocol::GateToWorldSecurityLockRemoveReq& protocol)
{
	AccountInfo* accInfo = AccountMgr::Instance()->FindAccount(protocol.accId);
	if (NULL == accInfo)
	{
		class SelectAccountRemoveSecurityLock : public AccountCallback
		{
		public:
			SelectAccountRemoveSecurityLock(UInt32 accid, UInt32 _gateId, const std::string& _passwd)
				: AccountCallback(accid), gateConnId(_gateId),  passwd(_passwd) {}

			void OnFinish(AccountInfo* info)
			{
				UInt32 ret = ErrorCode::SUCCESS;

				if (passwd.empty() || passwd != info->GetSecurityPasswd())
				{
					ret = ErrorCode::SECURITY_LOCK_PASSWD_ERROR;
					SendToGate(ret, info->GetSecurityPasswdErrorNum());
					return;
				}

				// 检查是否跨天
				info->CheckSecurityLockCrossDay();
				info->SetSecurityState(SECURITY_STATE_UNLOCK);
				SendToGate(ret, info->GetSecurityPasswdErrorNum());
			}

			void OnFailure()
			{
				SendToGate(ErrorCode::SECURITY_LOCK_FAILED, 0);
			}

			void SendToGate(UInt32 _ret, UInt32 _errNum)
			{
				auto conn = WSNetwork::Instance()->FindConnection(gateConnId);
				if (conn == NULL) return;

				CLProtocol::GateToWorldSecurityLockRemoveRes res;
				res.accId = m_AccId;
				res.ret = _ret;
				res.errorNum = _errNum;

				Avalon::Packet* packet = Avalon::Packet::Create();
				if (res.Encode(packet))
				{
					conn->SendPacket(packet);
				}
				Avalon::Packet::Destroy(packet);
			}

		private:
			//网关id
			UInt32 gateConnId;
			//密码
			std::string passwd;
		};

		AccountMgr::Instance()->LoadAccountInfo(protocol.accId, new SelectAccountRemoveSecurityLock(protocol.accId, protocol.GetConnID(), protocol.passwd));
	}
	else
	{
		CLProtocol::GateToWorldSecurityLockRemoveRes res;
		res.accId = protocol.accId;

		// 检查是否跨天
		accInfo->CheckSecurityLockCrossDay();

		if (protocol.passwd.empty() || protocol.passwd != accInfo->GetSecurityPasswd())
		{
			accInfo->AddSecurityPasswdErrorNum();
			accInfo->SaveToDB();

			res.errorNum = accInfo->GetSecurityPasswdErrorNum();
			res.ret = ErrorCode::SECURITY_LOCK_PASSWD_ERROR;
			protocol.SendBack(res);
			return;
		}

		accInfo->SetSecurityState(SECURITY_STATE_UNLOCK);

		res.errorNum = accInfo->GetSecurityPasswdErrorNum();
		res.ret = ErrorCode::SUCCESS;
		protocol.SendBack(res);
	}
}