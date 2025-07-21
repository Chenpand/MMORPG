#include "SceneDataCenterHandler.h"
#include "Player.h"

void HandleProtocol(Player *pPlayer, CLProtocol::SceneDataCenterUdpLog &protocol)
{
	/*
	pPlayer->SendUdpLog(protocol.str1.c_str(), protocol.str2.c_str(), protocol.str3.c_str(),
		protocol.str4.c_str(), protocol.str5.c_str(), protocol.str6.c_str(), protocol.strType.c_str());
	*/
}

void HandleProtocol(Player *pPlayer, CLProtocol::SceneCustomLogReport &protocol)
{
	if (!pPlayer) return;
	pPlayer->ReportCustomLog((CustomLogReportType)protocol.type, protocol.param);
}

