#ifndef _SCENE_CHAMPION_HANDLER_H_
#define _SCENE_CHAMPION_HANDLER_H_

#include <CLNetwork.h>
#include <CLChampionProtocol.h>
#include <CLObserveProtocol.h>

class Player;


void HandleProtocol(CLProtocol::SceneReceiveSyncChampionStatus& sync);
void HandleProtocol(CLProtocol::UnionCheckChampionEnrollRes& req);
void HandleProtocol(Player* player, CLProtocol::SceneChampionEnrollReq& req);
void HandleProtocol(Player* player, CLProtocol::SceneChampionJoinPrepareReq& req);
void HandleProtocol(Player* player, CLProtocol::SceneChampionReliveReq& req);
void HandleProtocol(Player* player, CLProtocol::SceneChampionReliveRes& req);
void HandleProtocol(Player* player, CLProtocol::UnionChampionReliveRes& req);
void HandleProtocol(Player* player, CLProtocol::SceneChampion16TableReq& req);
void HandleProtocol(Player* player, CLProtocol::SceneChampionGambleReq& req);
void HandleProtocol(Player* player, CLProtocol::SceneChampionObserveReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneChampionStatusReq& req);
void HandleProtocol(Player* player, CLProtocol::SceneChampionSelfStatusReq& req);
void HandleProtocol(Player* player, CLProtocol::SceneChampionTotalStatusReq& req);


void HandleProtocol(Player* player, CLProtocol::SceneChampionSelfBattleRecordReq& req);
void HandleProtocol(Player* player, CLProtocol::SceneChampionGroupRankReq& req);
void HandleProtocol(Player* player, CLProtocol::SceneChampionGroupRecordReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneChampionScoreBattleRecordsReq& req);
void HandleProtocol(Player* player, CLProtocol::SceneChampionAllGroupReq& req);
void HandleProtocol(Player* player, CLProtocol::SceneChampionAllGambleInfoReq& req);
void HandleProtocol(Player* player, CLProtocol::SceneChampionGambleRecordReq& req);
void HandleProtocol(Player* player, CLProtocol::SceneChampionGmableAlreadyBetReq& req);





void HandleProtocol(CLProtocol::UnionChampionJoinPrepareRes& req);
void HandleProtocol(CLProtocol::SceneChampion16TableSync& sync);
void HandleProtocol(CLProtocol::SceneChampionGambleInfoSync& sync);
void HandleProtocol(CLProtocol::SceneChampionAllGambleInfoRes& sync);
void HandleProtocol(CLProtocol::SceneChampionGambleUnionRes& req);
void HandleProtocol(CLProtocol::UnionChampionPlayerStatusRes& req);
void HandleProtocol(CLProtocol::SceneChampionAllGroupRes& req);
void HandleProtocol(CLProtocol::SceneChampionGroupStatusSync & sync);



class SceneChampionAllRaceInfoSyncFunction : public CLPacketFunction
{
public:
	void operator()(Avalon::Packet* packet) const;
};





#endif  //_SCENE_CHAMPION_HANDLER_H_