#include "WSWorldChallengeHandler.h"

#include <CLGiftBagDataEntry.h>
#include <CLSysNotify.h>
#include "WSPlayerMgr.h"
#include "WSRouter.h"
#include "MailBox.h"

/*
void HandleProtocol(WSPlayer* player, CLProtocol::WorldWorldChallengeSigninReq& protocol)
{
	CLProtocol::SceneWorldChallengeSigninReq req;
	req.id = player->GetID();
	player->SendToScene(req);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldWorldChallengeSignout& protocol)
{
	CLProtocol::ChallengeWorldChallengeSignout req;
	req.id = player->GetID();
	Router::SendToChallenge(req);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldWorldChallengeInfoReq& protocol)
{
	CLProtocol::ChallengeWorldChallengeInfoReq req;
	req.playerId = player->GetID();
	req.level = player->GetLevel();
	Router::SendToChallenge(req);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldWorldChallengeSortListReq& protocol)
{
	CLProtocol::ChallengeWorldChallengeSortListReq req;
	req.playerId = player->GetID();
	req.group = protocol.group;
	req.start = protocol.start;
	req.num = protocol.num;
	Router::SendToChallenge(req);
}

void HandleProtocol(WSPlayer* player, CLProtocol::WorldWorldChallengeGetAwardReq& protocol)
{
	
}

void HandleProtocol(CLProtocol::ChallengeWorldChallengeGetAwardRet& protocol)
{
	
}

void HandleProtocol(CLProtocol::ChallengeWorldChallengeSigninRet& protocol)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.id);
	if(player == NULL) return;

	CLProtocol::WorldWorldChallengeSigninRet ret;
	ret.result = protocol.result;
	player->SendProtocol(ret);
}

void HandleProtocol(CLProtocol::ChallengeWorldChallengeInfoRet& protocol)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if(player == NULL) return;

	CLProtocol::WorldWorldChallengeInfoRet ret;
	ret.group = protocol.group;
	ret.ranking = protocol.ranking;
	ret.score = protocol.score;
	ret.seasonTime = protocol.seasonTime;
	ret.beginTime = protocol.beginTime;
	ret.awardFlag = protocol.awardFlag;
	player->SendProtocol(ret);
}

void HandleProtocol(CLProtocol::ChallengeWorldChallengeSyncResult& protocol)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if(player == NULL) return;
	
	player->SendToScene(protocol);

	CLProtocol::WorldWorldChallengeSyncResult sync;
	sync.result = protocol.result;
	player->SendProtocol(sync);
}

void HandleProtocol(CLProtocol::ChallengeWorldChallengeSortListRet& protocol)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if(player == NULL) return;

	CLProtocol::WorldWorldChallengeSortListRet ret;
	ret.group = protocol.group;
	ret.start = protocol.start;
	ret.total = protocol.total;
	ret.entries = protocol.entries;
	player->SendProtocol(ret);
}

void HandleProtocol(CLProtocol::ChallengeWorldChallengeNotifyBattle& protocol)
{
	WSPlayer* player = WSPlayerMgr::Instance()->FindNormalPlayer(protocol.playerId);
	if(player == NULL) return;

	player->SendToScene(protocol);
}
*/