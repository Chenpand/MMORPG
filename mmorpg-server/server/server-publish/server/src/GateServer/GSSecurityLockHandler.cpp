#include "GSSecurityLockHandler.h"
#include "GSPlayerMgr.h"
#include <CLEnterGameProtocol.h>
#include "GSRouter.h"

void HandleProtocol(GSPlayer* player, CLProtocol::GateSecurityLockRemoveReq& protocol)
{
	CLProtocol::GateToWorldSecurityLockRemoveReq req;
	req.accId = player->GetAccID();
	req.passwd = protocol.passwd;
	Router::SendToWorld(req);
}

void HandleProtocol(CLProtocol::GateSecurityLockCheckRes& protocol)
{
	GSPlayer* player = GSPlayerMgr::Instance()->FindPlayerByAccID(protocol.accId);
	if (NULL == player) return;

	CLProtocol::GateDeleteRoleRes res;
	res.roleId = protocol.roleId;

	do
	{
		if (protocol.ret != ErrorCode::SUCCESS)
		{
			res.result = protocol.ret;

			if (protocol.ret != ErrorCode::SECURITY_LOCK_FAILED)
			{
				CLProtocol::WorldSecurityLockPasswdErrorNum passwdErrorNum;
				passwdErrorNum.error_num = protocol.errorNum;
				player->SendProtocol(passwdErrorNum);
			}

			break;
		}

		res.result = player->DeleteRole(protocol.roleId);
		if (res.result == ErrorCode::SUCCESS)
		{
			player->SendRoleInfo();
		}
		if (res.result == ErrorCode::ENTERGAME_DELETE_ROLE_LIMIT)
		{
			res.roleUpdateLimit = player->GetRoleUpdateLimitInfo();
		}

	} while (0);

	player->SendProtocol(res);
}

void HandleProtocol(CLProtocol::GateToWorldSecurityLockRemoveRes& protocol)
{
	GSPlayer* player = GSPlayerMgr::Instance()->FindPlayerByAccID(protocol.accId);
	if (NULL == player) return;

	CLProtocol::GateSecurityLockRemoveRes res;
	res.ret = protocol.ret;
	player->SendProtocol(res);

	// 同步密码错误次数
	if (res.ret == ErrorCode::SUCCESS || res.ret == ErrorCode::SECURITY_LOCK_FAILED)
	{
		return;
	}

	CLProtocol::WorldSecurityLockPasswdErrorNum passwdErrorNum;
	passwdErrorNum.error_num = protocol.errorNum;
	player->SendProtocol(passwdErrorNum);
}