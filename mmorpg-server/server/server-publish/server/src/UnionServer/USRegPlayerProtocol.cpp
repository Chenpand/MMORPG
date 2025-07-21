#include "USPlayerMgr.h"

#include "USSceneHandler.h"
#include "USChampionHandler.h"
#include "USGoldConsignmentHandler.h"

template<typename ProtocolT>
class USPlayerPacketFunction : public CLParamPacketFunction<USPlayer*>
{
public:
	void operator()(Avalon::Packet* packet, USPlayer* player) const
	{
		ProtocolT protocol;
		if (protocol.Decode(packet))
		{
			HandleProtocol(protocol, player);
		}
	}
};
#define US_REGISTER_PLAYER_PROTOCOL(ProtocolT) \
	m_Executor.Bind(ProtocolT::__ID, new USPlayerPacketFunction<ProtocolT>);



template<typename ProtocolT>
class USPlayerIDPacketFunction : public CLParamPacketFunction<RoleIDConnID>
{
public:
	void operator()(Avalon::Packet* packet, RoleIDConnID info) const
	{
		ProtocolT protocol;
		if (protocol.Decode(packet))
		{
			HandleProtocol(protocol, info);
		}
	}
};
#define US_REGISTER_PLAYER_ID_PROTOCOL(ProtocolT) \
	m_IDExecutor.Bind(ProtocolT::__ID, new USPlayerIDPacketFunction<ProtocolT>);

void USPlayerMgr::RegisterProtocols()
{
	// add by zhengfeng 2020.07.11
	// ******** ��ע�⣬��ע�⡣�����Ҫֱ�Ӵ�GateServer����Union����Ϣ����Ҫ��GSUnionProtocolMgr��ע����Ϣ����**********

	US_REGISTER_PLAYER_PROTOCOL(CLProtocol::SceneChampionReliveReq);
	US_REGISTER_PLAYER_PROTOCOL(CLProtocol::UnionChampionReliveReq);
	US_REGISTER_PLAYER_PROTOCOL(CLProtocol::UnionChampionPlayerStatusReq);
	US_REGISTER_PLAYER_PROTOCOL(CLProtocol::SceneCheckItemNumRet);
	US_REGISTER_PLAYER_PROTOCOL(CLProtocol::SceneItemThingStartRes);
	US_REGISTER_PLAYER_PROTOCOL(CLProtocol::SceneItemThingCommitRes);

	US_REGISTER_PLAYER_ID_PROTOCOL(CLProtocol::SceneChampionSelfStatusReq);
	US_REGISTER_PLAYER_ID_PROTOCOL(CLProtocol::SceneChampionTotalStatusReq);
	US_REGISTER_PLAYER_ID_PROTOCOL(CLProtocol::SceneChampionGroupRecordReq);
	US_REGISTER_PLAYER_ID_PROTOCOL(CLProtocol::SceneChampionGambleRecordReq);
	US_REGISTER_PLAYER_ID_PROTOCOL(CLProtocol::SceneChampionGmableAlreadyBetReq);

	//��Ҽ���
	US_REGISTER_PLAYER_ID_PROTOCOL(CLProtocol::UnionGoldConsignmentOrderPageReq)
	US_REGISTER_PLAYER_PROTOCOL(CLProtocol::SceneGoldConsignmentSubmitOrderReq)
	US_REGISTER_PLAYER_PROTOCOL(CLProtocol::UnionGoldConsignmentDealRecordReq)
	US_REGISTER_PLAYER_PROTOCOL(CLProtocol::UnionGoldConsignmentOwnerOrderReq)
	US_REGISTER_PLAYER_PROTOCOL(CLProtocol::UnionGoldConsignmentCancelOrderReq)
}