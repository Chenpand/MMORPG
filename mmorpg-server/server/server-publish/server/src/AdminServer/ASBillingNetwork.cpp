#include "ASBillingNetwork.h"

#include <AvalonMutex.h>
#include <sstream>
#include <CLNetAddress.h>
#include <CLBillingProtocol.h>
#include <CLSysProtocol.h>
#include <AvalonMd5.h>

#include "ASRouter.h"
#include "ASApplication.h"

BillingConnection::BillingConnection(CLNetwork* network)
	:CLNetConnection(network)
{
}

BillingConnection::~BillingConnection()
{
}

void BillingConnection::OnDataReached()
{
	do
	{
		Avalon::Packet* packet = RecvPacket();
		if(packet == NULL) break;

		GetNetwork()->DispatchPacket(packet);
	}
	while(true);
}

void BillingConnection::OnTick(const Avalon::Time& now)
{
	CLNetConnection::OnTick(now);
}

bool BillingConnection::PreHandlePacket(Avalon::Packet* packet)
{
	if(CLNetConnection::PreHandlePacket(packet)) return true;

	if(!IsVerified())
	{
		UInt32 id = GenID();
		if(!GetNetwork()->AddVerifiedConnection(id, this))
		{
			GameErrorLogStream("Billing") << "repeat billing connection id:" << id << "!" << LogStream::eos;
			Close();
			return true;
		}

		packet->SetConnID(GetID());
	}

	return false;
}

void BillingConnection::EncodeHeader(UInt8* header, size_t len, UInt16 size, UInt32 id, UInt32 sequence)
{
	*(UInt32*)header = UInt32(size) + 6;
	if(id == CLProtocol::ADMIN_BILLING_GIVEITEM_RET)
	{
		header[4] = 1;
		header[5] = 1;
	}
	else if(id == CLProtocol::ADMIN_BILLING_ROLEINFO_RET)
	{
		header[4] = 2;
		header[4] = 2;
	}
}
		
void BillingConnection::DecodeHeader(UInt8* header, size_t len, UInt16& size, UInt32& id, UInt32& sequence)
{
	size = *(UInt32*)header - 6;
	if(header[4] == 1)
	{
		id = CLProtocol::ADMIN_BILLING_GIVEITEM_REQ;
	}
	else if(header[4] == 2)
	{
		id = CLProtocol::ADMIN_BILLING_ROLEINFO_REQ;
	}
}
	
UInt32 g_BillingIdSeed = 0;
Avalon::Mutex g_BillingMutex;

UInt32 BillingConnection::GenID()
{
	UInt32 ret = 0;
	g_BillingMutex.Acquire();
	if(++g_BillingIdSeed == 0) g_BillingIdSeed = 1;
	ret = g_BillingIdSeed;
	g_BillingMutex.Release();
	return ret;
}


BillingNetwork::BillingNetwork()
{
}

BillingNetwork::~BillingNetwork()
{
}

Avalon::NetConnection* BillingNetwork::CreateConnection()
{
	return new BillingConnection(this);
}

bool BillingNetwork::Startup(Avalon::PropertySet* properties, CLNetAddress* addresses)
{
	if(!CLNetwork::Startup(properties))
	{
		ErrorLogStream << "startup network failed!" << LogStream::eos;
		return false;
	}

	Avalon::SockAddr addr;
	if(!addresses->FindBillingAddress(NODE_ID, addr))
	{
		ErrorLogStream << "lose billing address!" << LogStream::eos;
		return false;
	}

	if(!Bind(addr))
	{
		CLNetwork::Shutdown(0);
		return false;
	}

	return true;
}

void BillingNetwork::Shutdown(UInt32 inputTimeout)
{
	CLNetwork::Shutdown(inputTimeout);
}


bool BillingNetwork::ExecutePacket(Avalon::Packet* packet)
{
	if(packet->GetID() == CLProtocol::SYS_NOTIFY_CONNECTED)
	{
		CLProtocol::SysNotifyConnected protocol;
		protocol.Decode(packet);
		OnConnected(protocol.id);
		return true;
	}
	else if(packet->GetID() == CLProtocol::SYS_NOTIFY_DISCONNECTED)
	{
		CLProtocol::SysNotifyDisconnected protocol;
		protocol.Decode(packet);
		OnDisconnected(protocol.id);
		return true;
	}
	else if(packet->GetID() == CLProtocol::ADMIN_BILLING_GIVEITEM_REQ)
	{
		CLProtocol::AdminBillingGiveItemReq billingReq;
		if(!billingReq.Decode(packet))
		{
			GameErrorLogStream("Billing") << "decode billing request failed!" << LogStream::eos;
			return false;
		}

		char tokenBuff[33];
		std::string tokenSrc = billingReq.orderId + ASApplication::Instance()->GetBillingKey();
		Avalon::md5_encode32(tokenBuff, (void*)tokenSrc.c_str(), tokenSrc.length());
		tokenBuff[32] = 0;
		if(billingReq.token.compare(tokenBuff) != 0)
		{
			GameErrorLogStream("Billing") << "player(" << billingReq.accId << ",,) token wrong!" << LogStream::eos;
			CLProtocol::AdminBillingGiveItemRet ret;
			ret.result = 4;
			billingReq.SendBack(ret);
			return false;
		}

		CLProtocol::WorldBillingGiveItemReq req;
		req.nodeId = packet->GetConnID();
		req.accId = billingReq.accId;
		req.itemId = billingReq.itemId;
		req.num = billingReq.num;
		req.friendId = Avalon::StringUtil::ConvertToValue<UInt64>(billingReq.friendId);
		req.price = billingReq.price;
		req.orderId = billingReq.orderId;
		req.token = billingReq.token;
		req.amt = billingReq.amt;
		req.payamt_coins = billingReq.payamt_coins;
		req.moneyType = billingReq.moneyType;
		req.taocanId = billingReq.taocanId;

		Router::SendToWorld(req);

		GameInfoLogStream("Billing") << "player(" << req.accId << ",,) recved billing request:itemId=" << req.itemId << ",num=" << req.num
			<< ",orderId=" << req.orderId << ",token=" << req.token << ",amt=" << req.amt << ",moneyType=" << req.moneyType << ",taocanId=" << req.taocanId << LogStream::eos;
	}
	else if(packet->GetID() == CLProtocol::ADMIN_BILLING_ROLEINFO_REQ)
	{
		CLProtocol::AdminBillingRoleInfoReq roleInfoReq;
		if(!roleInfoReq.Decode(packet))
		{
			GameErrorLogStream("Billing") << "decode billing roleinfo request failed!" << LogStream::eos;
			return false;
		}

		CLProtocol::WorldBillingRoleInfoReq worldReq;
		worldReq.zoneId = roleInfoReq.zoneId;
		worldReq.accId = roleInfoReq.accId;
		Router::SendToWorld(worldReq);

		GameInfoLogStream("Billing") << "player(" << roleInfoReq.accId << ",,) recved billing roleinfo request:zoneId=" << roleInfoReq.zoneId << LogStream::eos;
	}

	return true;
}

void BillingNetwork::OnConnected(UInt32 id)
{
	GameInfoLogStream("Billing") << "billing " << id << " connected!" << LogStream::eos;
}

void BillingNetwork::OnDisconnected(UInt32 id)
{
	GameInfoLogStream("Billing") << "billing " << id << " disconnected!" << LogStream::eos;
}
