#include "ASBillingHandler.h"
#include "ASBillingNetwork.h"

void HandleProtocol(CLProtocol::WorldBillingGiveItemRet& protocol)
{
	Avalon::NetConnectionPtr conn = BillingNetwork::Instance()->FindConnection(protocol.nodeId);
	if(conn == NULL)
	{
		GameErrorLogStream("Billing") << "player(" << protocol.accId << ",,) can not find connection "
			<< protocol.nodeId << " when send response!" << LogStream::eos;
		return;
	}

	CLProtocol::AdminBillingGiveItemRet ret;
	ret.result = protocol.result;
	ret.level = protocol.level;
	ret.isqqvip = protocol.isqqvip;
	ret.qqviplvl = protocol.qqviplvl;
	CL_SEND_PROTOCOL(conn, ret);
	conn->Close();

	GameInfoLogStream("Billing") << "player(" << protocol.accId << ",,) send items result:" << protocol.result << LogStream::eos;
}

void HandleProtocol(CLProtocol::WorldBillingRoleInfoRet& protocol)
{
	Avalon::NetConnectionPtr conn = BillingNetwork::Instance()->FindConnection(protocol.nodeId);
	if(conn == NULL)
	{
		GameErrorLogStream("Billing") << "player(," << protocol.name << ",) can not find connection "
			<< protocol.nodeId << " when send roleinfo response!" << LogStream::eos;
		return;
	}

	CLProtocol::AdminBillingRoleInfoRet ret;
	ret.name = protocol.name;
	ret.level = protocol.level;
	ret.sex = protocol.sex;
	ret.gold = protocol.gold;
	CL_SEND_PROTOCOL(conn, ret);
	conn->Close();

	GameInfoLogStream("Billing") << "player(," << ret.name << ",) send roleinfo!" << LogStream::eos;
}
