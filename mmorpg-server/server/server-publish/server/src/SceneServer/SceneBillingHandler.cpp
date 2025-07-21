#include "SceneBillingHandler.h"
#include "PlayerMgr.h"
#include "PlayerLoadCallback.h"
#include "SSRouter.h"

class PlayerLoadingForCharge : public PlayerLoadingCallback
{
public:
	PlayerLoadingForCharge(const CLProtocol::SceneBillingSendGoods &req) : m_Req(req) {}
	virtual void OnFinish(Player* player)
	{
		InfoLogStream << "player(" << player->GetID() << ") loading for charge finish, start to send goods." << LogStream::eos;

		CLProtocol::SceneBillingSendGoodsRes res;
		res.orderId = m_Req.orderId;
		res.result = player->GiveChargeGoods(m_Req);
		Router::SendToWorld(res);

		if (player->GetStatus() == PS_INIT)
		{
			player->SetGameStatus(PS_FINAL);
			player->SetRefTime(CURRENT_TIME.MSec());
		}
	}

	/**
	*@brief 加载失败
	*/
	virtual void OnFailure(Player* player)
	{
		ErrorLogStream << "player(" << m_Req.accid << ", " << m_Req.roleId << ") loading failed, order(" << m_Req.orderId << ")." << LogStream::eos;

		CLProtocol::SceneBillingSendGoodsRes res;
		res.orderId = m_Req.orderId;
		res.result = ErrorCode::BILLING_PLAYER_OFFLINE;
		Router::SendToWorld(res);

		if (player->GetGameStatus() == PS_INIT)
		{
			// 加载失败，设为代摧毁状态
			ErrorLogStream << PLAYERINFO(player) << "loading failed wait to destory..." << LogStream::eos;
			player->SetGameStatus(PS_DESTORY);
		}
	}

	/**
	*@brief 加载超时
	*/
	virtual void OnTimeout(Player* player)
	{
		OnFailure(player);
	}

	CLProtocol::SceneBillingSendGoods		m_Req;
};

void HandleProtocol(CLProtocol::SceneBillingSendGoods &req)
{
	CLProtocol::SceneBillingSendGoodsRes res;
	res.orderId = req.orderId;

	auto player = PlayerMgr::Instance()->FindLoadingFinishPlayer(req.roleId);
	if (!player)
	{
		PlayerMgr::Instance()->LoadOfflinePlayerCb(req.roleId, req.accid, new PlayerLoadingForCharge(req));
	}
	else
	{
		res.result = player->GiveChargeGoods(req);
		req.SendBack(res);
	}
}