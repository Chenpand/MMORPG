#include "VerifyTask.h"

#include <AvalonPacket.h>
#include <CLVerifyProtocol.h>
#include <CLErrorCode.h>

#include "LogicFactory.h"
#include "ILogic.h"
#include "VerifyMgr.h"
#include "VSInternalNetwork.h"
#include "VSApplication.h"

VerifyTask::VerifyTask(RaceType type, ObjID_t raceId, UInt32 connId)
{
	m_Type = type;
	m_Logic = NULL;
	m_RaceId = raceId;
	m_ConnId = connId;
	m_Status = VERIFY_TASK_STATUS_INIT;
	m_StartTime = CURRENT_TIME.MSec();
	m_LastFrame = 0;
	m_LogicRaceEnd = 0;
	m_PlayerReportRaceEndTime = 0;
}

VerifyTask::~VerifyTask()
{
	if (m_Logic)
	{
		delete m_Logic;
	}

	for (auto itr : m_PlayerRaceEnd)
	{
		auto packet = itr.second;
		if (packet)
		{
			Avalon::Packet::Destroy(packet);
		}
	}

	if (m_LogicRaceEnd)
	{
		Avalon::Packet::Destroy(m_LogicRaceEnd);
	}

	VerifyMgr::Instance()->LC_GiveUpVerify(GetID());
}

bool VerifyTask::OnInit(Avalon::Packet* raceInitPacket)
{
	m_Logic = LogicFactory::Instance()->CreateLogic(m_Type);
	if (!m_Logic)
	{
		ErrorLogStream << "race(" << m_RaceId << ") create logic(" << (UInt8)m_Type << ") failed." << LogStream::eos;
		return false;
	}

	if (!m_Logic->OnInit(this, raceInitPacket))
	{
		ErrorLogStream << "race(" << m_RaceId << ") init logic(" << (UInt8)m_Type << ") failed." << LogStream::eos;
		return false;
	}

	SetStatus(VERIFY_TASK_STATUS_DOING);
	VerifyMgr::Instance()->GetStatistic().start++;

	return true;
}

void VerifyTask::OnTick(const Avalon::Time& now)
{
	if (m_Logic)
	{
		m_Logic->OnTick(now);
	}

	_OnFrameChecksum();
	_OnCheckRaceEnd();

	if (GetStatus() == VERIFY_TASK_WAIT_LOGIC_FINISH && m_Logic && m_Logic->IsRunToLastFrame())
	{
		InfoLogStream << "race(" << GetID() << ") verify finish..." << LogStream::eos;

		if (!m_LogicRaceEnd)
		{
			// 逻辑运行结束，没有产生结算，但是玩家上报了结算
			if (!m_PlayerRaceEnd.empty())
			{
				DebugLogStream << "[_OnCheckRaceEnd] race(" << GetID() << ") logic miss report race end." << LogStream::eos;
				VerifyMgr::Instance()->GetStatistic().missLogicRaceEnd++;
			}
		}

		// 验证结束
// 		if (m_LogicRaceEnd)
// 		{
// 			// 逻辑运行结束，没有产生结算，但是玩家上报了结算，说明玩家作弊了
// 			for (auto itr : m_PlayerRaceEnd)
// 			{
// 				auto roleid = itr.first;
// 				_OnPlayerCheat(roleid, VERIFY_CHEAT_NO_RACE_END);
// 				Avalon::Packet::Destroy(itr.second);
// 			}
// 			m_PlayerRaceEnd.clear();
// 		}
		_OnVerifyFinish();
	}

	// 检查下是否超过最大时间
	if (now.CurrentTime().MSec() > m_StartTime + _GetRaceMaxTime())
	{
		ErrorLogStream << "race(" << GetID() << ") verify timeout..." << LogStream::eos;
		_OnVerifyTimeout();
	}
}

void VerifyTask::AddGamer(ObjID_t roleId, UInt8 pos)
{
	if (FindGamerByRoleID(roleId) || FindGamerByPos(pos))
	{
		ErrorLogStream << "race(" << m_RaceId << ") repeat add gamer(" << roleId << ") pos(" << pos << ")." << LogStream::eos;
		return;
	}

	VerifyGamer gamer;
	gamer.roleId = roleId;
	gamer.pos = pos;
	m_Gamers.push_back(gamer);
}

VerifyGamer* VerifyTask::FindGamerByRoleID(ObjID_t roleId)
{
	for (auto& gamer : m_Gamers)
	{
		if (gamer.roleId == roleId)
		{
			return &gamer;
		}
	}

	return NULL;
}

VerifyGamer* VerifyTask::FindGamerByPos(UInt8 pos)
{
	for (auto& gamer : m_Gamers)
	{
		if (gamer.pos == pos)
		{
			return &gamer;
		}
	}

	return NULL;
}

void VerifyTask::PushFrame(const std::vector<CLProtocol::Frame>& frames)
{
	if (!m_Logic)
	{
		ErrorLogStream << "race(" << m_RaceId << ") type(" << m_Type << ") no logic instance." << LogStream::eos;
		return;
	}

	if (frames.empty())
	{
		return;
	}

#ifdef _VERIFY_LOG_
	UInt32 firstFrame = frames[0].sequence;
	UInt32 lastFrame = frames[frames.size() - 1].sequence;


	InfoLogStream << "race(" << m_RaceId << ") recv frame (" << firstFrame << ") => (" << lastFrame << ") lastFrame(" << m_LastFrame << ")." << LogStream::eos;
#endif

	CLProtocol::Frame timeFrame;
	for (const auto& frame : frames)
	{
		if (frame.sequence <= m_LastFrame)
		{
			ErrorLogStream << "race(" << m_RaceId << ") type(" << m_Type << ") recv error frame(" << frame.sequence << ") lastframe(" << m_LastFrame << ")." << LogStream::eos;
			continue;
		}
		
		if (frame.sequence >= _GetMaxFrameNum())
		{
			ErrorLogStream << "race(" << m_RaceId << ") type(" << m_Type << ") recv invalid frame(" << frame.sequence << ")." << LogStream::eos;
			//_OnPlayerCheat(, VERIFY_CHEAT_INVALID_FRAME, frame.sequence);
			return;
		}

		// 填充时间帧
		while (m_LastFrame++ < frame.sequence - 1)
		{
			timeFrame.sequence = m_LastFrame;
			m_Logic->PushFrame(timeFrame);
		}
		m_Logic->PushFrame(frame);
	}
}

void VerifyTask::SetPlayerRaceEndInfo(ObjID_t roleId, Avalon::Packet* raceEndInfo)
{
	if (!raceEndInfo)
	{
		return;
	}

	if (m_PlayerRaceEnd.find(roleId) != m_PlayerRaceEnd.end())
	{
		ErrorLogStream << "race(" << m_RaceId << ") repeat set player(" << roleId << ")'s race end info." << LogStream::eos;
		Avalon::Packet::Destroy(raceEndInfo);
		return;
	}

	m_PlayerRaceEnd[roleId] = raceEndInfo;

	if (m_PlayerReportRaceEndTime == 0)
	{
		m_PlayerReportRaceEndTime = (UInt64)CURRENT_TIME.MSec();
		DebugLogStream << "race(" << GetID() << ") set player(" << roleId << ")'s race end, current time(" << m_PlayerReportRaceEndTime << ")." << LogStream::eos;
	}
}

void VerifyTask::SetPlayerChecksum(ObjID_t roleId, UInt32 frame, UInt32 checksum)
{
#ifdef _VERIFY_LOG_
	DebugLogStream << "race(" << m_RaceId << ") gamer(" << roleId << ") frame(" << frame << ") checksum(" << checksum << ")." << LogStream::eos;
#endif

	auto gamer = FindGamerByRoleID(roleId);
	if (!gamer)
	{
		return;
	}

	// 上报的校验帧比最后一帧大，肯定作弊了
// 	if ((frame % FRAME_CHECKSUM_INTERVAL) != 0)
// 	{
// 		ErrorLogStream << "race(" << m_RaceId << ") gamer(" << roleId << ") recv invalid frame(" << frame << ") checksum(" << checksum << ") lastframe(" << m_LastFrame << ")." << LogStream::eos;
// 		_OnPlayerCheat(roleId, PLAYER_CHEAT_REASON_INVALID_CHECKSUM, frame);
// 		return;
// 	}

	ChecksumInfo check;
	check.frame = frame;
	check.checksum = checksum;
	gamer->checksums.push_back(check);
}

void VerifyTask::SetLogicRaceEndInfo(Avalon::Packet* raceEndInfo)
{
	m_LogicRaceEnd = raceEndInfo;

	// 返回结算信息给服务器
	if (m_Logic && m_Logic->IsNeedReportRaceEndToServer())
	{
		CLProtocol::VerifyReportTaskRaceEnd protocol(raceEndInfo);
		protocol.raceType = GetType();
		protocol.raceId = GetID();
		_SendbackProtocol(protocol);
		protocol.Detach();
	}
}

void VerifyTask::SetLogicChecksum(UInt32 frame, UInt32 checksum)
{
	m_logicChecksums[frame] = checksum;
}

void VerifyTask::GiveupVerify()
{
	VerifyMgr::Instance()->LC_GiveUpVerify(GetID());

	_OnVerifyFinish();
}

void VerifyTask::_OnFrameChecksum()
{
	for (auto& gamer : m_Gamers)
	{
		for (UInt32 i = 0; i < gamer.checksums.size(); i++)
		{
			auto& info = gamer.checksums[i];
			auto itr = m_logicChecksums.find(info.frame);
			if (itr == m_logicChecksums.end())
			{
				continue;
			}

			if (info.checksum != itr->second)
			{
				if (!gamer.cheat)
				{
					InfoLogStream << "[fuck_unsync] race(" << GetID() << ") gamer(" << gamer.roleId << ") invalid checksum, frame(" << info.frame << ") player(" << info.checksum << ") logic(" << itr->second << ")." << LogStream::eos;
				}
				_OnPlayerCheat(gamer.roleId, PLAYER_CHEAT_REASON_INVALID_CHECKSUM, info.frame);
			}

			gamer.checksums.erase(gamer.checksums.begin() + i);
			i--;
		}
	}
}

void VerifyTask::_OnCheckRaceEnd()
{
	if (!m_LogicRaceEnd)
	{
		return;
	}

	for (auto itr = m_PlayerRaceEnd.begin(); itr != m_PlayerRaceEnd.end(); itr++)
	{
		auto roleId = itr->first;
		auto playerEnd = itr->second;
		if (playerEnd->GetSize() != m_LogicRaceEnd->GetSize())
		{
			InfoLogStream << "[RaceEndCheck] race(" << GetID() << ") player(" << roleId << ") check race end error, invalid size." << LogStream::eos;
			
			if (VSApplication::Instance()->IsReportCheatWhenRaceEndUnsync())
			{
				_OnPlayerCheat(roleId, PLAYER_CHEAT_REASON_CHECK_RACE_END);
			}
			
			VerifyMgr::Instance()->GetStatistic().raceEndUnsync++;

			// 结算不一致时打印验证服务器和玩家的结算信息，帮助排查
			ShowDungeonRaceEnd(roleId, playerEnd);
			ShowDungeonRaceEnd(0, m_LogicRaceEnd);

			ShowPkRaceEnd(roleId, playerEnd);
			ShowPkRaceEnd(0, m_LogicRaceEnd);

			Avalon::Packet::Destroy(playerEnd);
			continue;
		}

		if (memcmp(playerEnd->GetBuffer(), m_LogicRaceEnd->GetBuffer(), m_LogicRaceEnd->GetSize()) != 0)
		{
			InfoLogStream << "[RaceEndCheck] race(" << GetID() << ") player(" << roleId << ") check race end error, invalid data." << LogStream::eos;
			
			if (VSApplication::Instance()->IsReportCheatWhenRaceEndUnsync())
			{
				_OnPlayerCheat(roleId, PLAYER_CHEAT_REASON_CHECK_RACE_END);
			}
			VerifyMgr::Instance()->GetStatistic().raceEndUnsync++;

			// 结算不一致时打印验证服务器和玩家的结算信息，帮助排查

			ShowDungeonRaceEnd(roleId, playerEnd);
			ShowDungeonRaceEnd(0, m_LogicRaceEnd);

			ShowPkRaceEnd(roleId, playerEnd);
			ShowPkRaceEnd(0, m_LogicRaceEnd);

			Avalon::Packet::Destroy(playerEnd);

			continue;
		}

		InfoLogStream << "race(" << GetID() << ") player(" << roleId << ") race end check ok." << LogStream::eos;

		Avalon::Packet::Destroy(playerEnd);
	}

	m_PlayerRaceEnd.clear();
}

void VerifyTask::_OnVerifyFinish()
{
	SetStatus(VERIFY_TASK_STATUS_FINISH);
	VerifyMgr::Instance()->GetStatistic().success++;
}

void VerifyTask::_OnVerifyTimeout()
{
	SetStatus(VERIFY_TASK_STATUS_FINISH);
	VerifyMgr::Instance()->GetStatistic().timeout++;
}

void VerifyTask::_OnPlayerCheat(ObjID_t roleid, PlayerCheatReason reason, UInt32 param)
{
	auto gamer = FindGamerByRoleID(roleid);
	if (!gamer)
	{
		ErrorLogStream << "race(" << m_RaceId << ") player(" << roleid << ") cheated, no this player..." << LogStream::eos;
		return;
	}
	if (gamer->cheat)
	{
		return;
	}

	gamer->cheat = true;
	InfoLogStream << "race(" << m_RaceId << ") player(" << roleid << ") reason(" << (UInt32)reason << ") param(" << param << ") cheated " << m_Logic->ShowInfo() << "." << LogStream::eos;
	VerifyMgr::Instance()->LC_SaveRecord(GetID());

	// 发送玩家作弊到服务器
	CLProtocol::VerifyReportTaskResult result;
	result.raceId = GetID();
	result.roleId = roleid;
	result.dungeonId = m_Logic ? m_Logic->GetDungeonID() : 0;
	result.result = reason;
	_SendbackProtocol(result);

	VerifyMgr::Instance()->GetStatistic().unsync++;
}

UInt32 VerifyTask::_GetMaxFrameNum() const
{
	// 一局不能超过1个钟头
	// 逻辑是60帧，一个钟头就是60帧*60秒*60分钟
	return 60 * 60 * 60;
}

UInt64 VerifyTask::_GetRaceMaxTime() const
{
	return Avalon::Time::SECS_OF_HOUR * Avalon::Time::MSECS_OF_SEC;
}

void VerifyTask::ShowDungeonRaceEnd(UInt64 roleid, Avalon::Packet* packet)
{
	if (m_Type != RT_DUNGEON)
	{
		return;
	}

	if (!packet)
	{
		return;
	}

	try
	{
		Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());

		CLProtocol::RelaySvrDungeonRaceEndReq protocol;
		if (!protocol.Decode(packet))
		{
			ErrorLogStream << "[RaceEndCheck] decode race(" << GetID() << ") player(" << roleid << ")'s dungeon race end failed." << LogStream::eos;
			return;
		}

		DebugLogStream << "[RaceEndCheck] race(" << GetID() << ") owner:" << roleid << " --- " << protocol.raceEndInfo.ToString() << LogStream::eos;
	}
	catch (Avalon::NetSerializeException& e)
	{
		ErrorLogStream << "[RaceEndCheck] serialize race(" << GetID() << ") player(" << roleid << ")'s dungeon race end failed." << LogStream::eos;
		return;
	}
}

void VerifyTask::ShowPkRaceEnd(ObjID_t roleid, Avalon::Packet* packet)
{
	if (m_Type == RT_DUNGEON)
	{
		return;
	}

	if (!packet)
	{
		return;
	}

	try
	{
		Avalon::NetInputStream stream(packet->GetBuffer(), packet->GetSize());

		CLProtocol::RelaySvrEndGameReq protocol;
		if (!protocol.Decode(packet))
		{
			ErrorLogStream << "[RaceEndCheck] decode race(" << GetID() << ", " << (UInt8)GetType() << ") player(" << roleid << ")'s dungeon race end failed." << LogStream::eos;
			return;
		}

		DebugLogStream << "[RaceEndCheck] race(" << GetID() << ") owner:" << roleid << " --- " << protocol.info.ToString() << LogStream::eos;
	}
	catch (Avalon::NetSerializeException& e)
	{
		ErrorLogStream << "[RaceEndCheck] serialize race(" << GetID() << ") player(" << roleid << ")'s dungeon race end failed." << LogStream::eos;
		return;
	}
}

void VerifyTask::_SendbackProtocol(Avalon::Protocol& protocol)
{
	auto conn = VSInternalNetwork::Instance()->FindConnection(m_ConnId);
	if (conn == NULL)
	{
		return;
	}

	CL_SEND_PROTOCOL(conn, protocol);
}