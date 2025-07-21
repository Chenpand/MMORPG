#include "GSActivityHandler.h"
#include "GSActivityMgr.h"

void HandleProtocol(CLProtocol::WSSyncOpActivityData& protocol)
{
	GSActivityMgr::Instance()->OnLoadOpActData(protocol.datas);
}

void HandleProtocol(CLProtocol::WSOpActivityChange& protocol)
{
	GSActivityMgr::Instance()->OnOpActStateChange(protocol.opActRegInfo);
}

void HandleProtocol(CLProtocol::WorldNotifyGameStartTimeToS &protocol)
{
	GAMESTART_TIME = protocol.time;
	GAME_FIRST_START_TIME = protocol.firstTime;

	InfoLogStream << "GateServer receive sync, GAMESTART_TIME=" << GAMESTART_TIME << ", GAME_FIRST_START_TIME=" << GAME_FIRST_START_TIME << LogStream::eos;
}