#include "TestLogic.h"
#include "VerifyTask.h"
#include <CLDungeonProtocol.h>

bool TestLogic::OnInit(VerifyTask* task, Avalon::Packet* raceInitPacket)
{
	m_Task = task;

	CLProtocol::SceneDungeonStartRes init;
	try
	{
		if (!init.Decode(raceInitPacket))
		{
			return false;
		}
	}
	catch (Avalon::NetSerializeException& e)
	{
		ErrorLogStream << e.what();
	}

	for (auto& fighter : init.players)
	{
		m_Task->AddGamer(fighter.roleId, fighter.pos);
	}

	return true;
}

void TestLogic::PushFrame(const CLProtocol::Frame& frame)
{
	if (frame.sequence % FRAME_CHECKSUM_INTERVAL == 0)
	{
		m_Task->SetLogicChecksum(frame.sequence, frame.sequence / FRAME_CHECKSUM_INTERVAL);
	}

	if (frame.sequence == 1)
	{
		CLProtocol::RelaySvrDungeonRaceEndReq end;
		auto packet = Avalon::Packet::Create();
		end.Encode(packet);
		m_Task->SetLogicRaceEndInfo(packet);
	}
}

bool TestLogic::IsRunToLastFrame()
{
	bool end = false;
	return end;
}