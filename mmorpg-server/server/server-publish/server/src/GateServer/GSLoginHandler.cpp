#include "GSLoginHandler.h"

#include <AvalonLogger.h>
#include <CLErrorCode.h>
#include <CLAntiAddictMgr.h>
#include <CLSysNotify.h>

#include "GSApplication.h"
#include "GSAccountMgr.h"
#include "GSRouter.h"
#include "GSPlayerMgr.h"

void HandleProtocol(CLProtocol::GateSendAccountInfo& protocol)
{
	CLProtocol::GateNetAddressRet ret;
	ret.pf = protocol.pf;
	ret.accid = protocol.accid;
	ret.openid = protocol.openid;
	ret.phoneBindRoleId = protocol.phoneBindRoleId;
	ret.encryptProtocol = GSApplication::Instance()->IsEncryptProtocol() ? 1 : 0;
	ret.result = 0;

	if(!GSApplication::Instance()->GetNetAddress()->FindExternalAddress(NODE_ID,ret.addr))
	{
		ret.result = ErrorCode::LOGIN_SERVER_UNREADY;
		Router::SendToAdmin(ret);
		return;
	}

	// ·À³ÁÃÔ¼ì²é
	if (GSApplication::Instance()->IsCheckAntiAddict())
	{
		CLAntiAddictMgr antiAddictCheck;
		UInt32 tourLoginInterval = GSApplication::Instance()->GetTouLoginInterval();
		UInt32 tourOnlineTime = GSApplication::Instance()->GetTourOnlineTime();
		antiAddictCheck.OnInit(protocol.antiAddictInfo, tourLoginInterval, tourOnlineTime);
		UInt32 antiRet = antiAddictCheck.OnLoginCheck();
		if (antiRet != ErrorCode::SUCCESS)
		{
			InfoLogStream << "account(" << protocol.accid << ") anti addict failed:"<< antiRet << LogStream::eos;
			ret.result = antiRet;
			ret.errMsg = SysNotifyMgr::Instance()->MakeString(antiRet);
			Router::SendToAdmin(ret);
			return;
		}
	}

	AccountInfo info;
	info.accid = protocol.accid;
	info.gmauthority = protocol.gmauthority;
	memmove(info.key, protocol.hash, 20);
	info.openid = protocol.openid;
	info.qqVipInfo = protocol.qqVipInfo;
	info.source1 = protocol.source1;
	info.source2 = protocol.source2;
	info.pf = protocol.pf;
	info.did = protocol.did;
	info.model = protocol.model;
	info.deviceVersion = protocol.deviceVersion;
	info.phoneBindRoleId = protocol.phoneBindRoleId;
	info.antiAddictInfo = protocol.antiAddictInfo;
	GSAccountMgr::Instance()->AddAccount(info);
	Router::SendToAdmin(ret);
	return;
}



