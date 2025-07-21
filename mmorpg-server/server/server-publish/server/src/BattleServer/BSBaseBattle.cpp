#include "BSBaseBattle.h"
#include "BSPlayer.h"
#include "BSBaseRace.h"
#include "BSPlayerMgr.h"
#include "BSNetwork.h"

BSBaseBattle::BSBaseBattle()
{
	m_ID = 0;
	m_nodeID = 0;
	m_sceneId = 0;
	m_beginTime = 0;
	m_state = 0;
	m_destoryTime = 0;
}
BSBaseBattle::~BSBaseBattle()
{}

UInt64 BSBaseBattle::GenRaceID()
{
	// 生成唯一的比赛ID,括号内为相应字段的位数
	// NodeID(48-63) +  时间(s)(32-67) + 随机数(0-32)
	Avalon::Time now = Avalon::Time::CurrentTime();
	UInt64 randNum = (UInt64)rand();
	UInt64 raceID = ((UInt64(ZONE_ID) << 47) + (randNum << 31) + now.Sec());
	return raceID;
}

BSPlayer*  BSBaseBattle::FindPlayer(UInt64 playerId)
{
	return BSPlayerMgr::Instance()->FindPlayer(playerId);
}

void BSBaseBattle::SendToBattleScene(Avalon::Protocol& protocol)
{
	BSNetwork::Instance()->SendToBScene(GetSceneNodeID(), protocol);
}
