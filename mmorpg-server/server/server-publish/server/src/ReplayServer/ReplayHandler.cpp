#include "ReplayHandler.h"
#include "ReplayMgr.h"
#include <CLRecordClient.h>
#include <AvalonCompress.h>

#include "RSApplication.h"

void HandleProtocol(CLProtocol::ReplayReportPlayerOnline& protocol)
{
	DebugLogStream << "player(" << protocol.roleId << ") online." << LogStream::eos;

	ReplayListType loadLists[] = { REPLAY_LIST_TYPE_SELF };
	for (auto type : loadLists)
	{
		auto replayList = ReplayMgr::Instance()->FindReplayList(type, protocol.roleId);
		if (replayList)
		{
			// 已经加载好了
			replayList->SetRemoveTime(0);
			continue;
		}

		ReplayMgr::Instance()->LoadReplayList(type, protocol.roleId, NULL);
	}
}

void HandleProtocol(CLProtocol::ReplayReportPlayerOffline& protocol)
{
	DebugLogStream << "player(" << protocol.roleId << ") offline." << LogStream::eos;

	ReplayListType loadLists[] = { REPLAY_LIST_TYPE_SELF };
	for (auto type : loadLists)
	{
		ReplayMgr::Instance()->DelayDelReplayList(type, protocol.roleId);
	}
}

void HandleProtocol(CLProtocol::ReplayReportRace& protocol)
{
	/*
		暂时先这样，正常流程应该是如果已经有自己的列表信息了，那就添加到内存，如果没有列表信息，要先去数据库加载，加载完成后再做处理
		ToDo...
	*/

	InfoLogStream << "Report replay race(raceid=" << protocol.info.raceId << ", type=" << protocol.info.type << ", score=" << protocol.info.score << ")." << LogStream::eos;

	// 先添加录像信息
	if (ReplayMgr::Instance()->FindReplayInfo(protocol.info.raceId))
	{
		// 说明已经有了
		FatalLogStream << "repeat report replay(" << protocol.info.raceId << ")." << LogStream::eos;
		return;
	}

	auto replayInfo = ReplayInfoPtr(new ReplayInfo());
	*replayInfo = protocol.info;
	if (!ReplayMgr::Instance()->AddReplayInfo(replayInfo))
	{
		FatalLogStream << "add replay(" << protocol.info.raceId << ") failed." << LogStream::eos;
		return;
	}

	ReplayMgr::Instance()->InsertReplayInfoToDB(replayInfo);

	// 高手对决,或者赏金联赛淘汰赛
	if (ReplayMgr::Instance()->CanJoinMasterWaitList(replayInfo->score) || replayInfo->type == PK_PREMIUM_LEAGUE_BATTLE || replayInfo->type == PK_CHAMPION_BATTLE)
	{
		CLProtocol::ReplayFileReq req;
		req.raceId = protocol.info.raceId;
		protocol.SendBack(req);

		InfoLogStream << "Replay race(" << protocol.info.raceId << ") request file." << LogStream::eos;
	}

	for (auto& fighter : protocol.info.fighters)
	{
		// roleid是0说明是机器人
		if (fighter.roleId == 0)
		{
			continue;
		}

		auto replayList = ReplayMgr::Instance()->FindReplayList(REPLAY_LIST_TYPE_SELF, fighter.roleId);
		if (!replayList)
		{
			continue;
		}

		ReplayListEntry* entry = new ReplayListEntry(REPLAY_LIST_TYPE_SELF);
		entry->guid = CLRecordClient::Instance()->GenGuid();
		entry->ownerid = fighter.roleId;
		entry->raceId = replayInfo->raceId;

		entry->SaveToDB();

		replayInfo->dbRefNum++;

		replayList->AddEntry(entry);
		replayList->SetReplayInfo(replayInfo);
	}

	ReplayMgr::Instance()->UpdateReplayInfoToDB(replayInfo);
}

void HandleProtocol(CLProtocol::ReplayListReq& req)
{
	/*
		暂时先这样，正常流程应该是如果已经有自己的列表信息了，那就添加到内存，如果没有列表信息，要先去数据库加载，加载完成后再做处理
		ToDo...
	*/

	DebugLogStream << "player(" << req.roleid << ") request list(" << req.type << ")..." << LogStream::eos;

	CLProtocol::ReplayListRes res;
	res.roleid = req.roleid;
	res.type = req.type;

	if (req.type == REPLAY_LIST_TYPE_MASTER && !RSApplication::Instance()->CanViewMasterReplay())
	{
		req.SendBack(res);
		return;
	}
	
	auto replayList = ReplayMgr::Instance()->FindReplayList((ReplayListType)req.type, req.type == REPLAY_LIST_TYPE_MASTER ? 0 : req.roleid);
	if (!replayList)
	{
		// 没有列表的情况下要去数据库加载
		// ToDo...
		req.SendBack(res);
		ErrorLogStream << "player(" << req.roleid << ") request list(" << req.type << ") failed!" << LogStream::eos;
		return;
	}

	for (auto& entry : replayList->GetEntrys())
	{
		if (!entry)
		{
			ErrorLogStream << "player(" << req.roleid << ") replay entry(" << req.type << ") was null!" << LogStream::eos;
			continue;
		}

		if (!entry->info)
		{
			FatalLogStream << "player(" << req.roleid << ") race(" << entry->raceId << ") don't have info..." << LogStream::eos;
			continue;
		}

		res.replays.push_back(*entry->info);
	}

	req.SendBack(res);
}

void HandleProtocol(CLProtocol::ReplayReportView& protocol)
{
	DebugLogStream << "player(" << protocol.roleid << ") view replay(" << protocol.raceid << ")." << LogStream::eos;

	auto replayInfo = ReplayMgr::Instance()->FindReplayInfo(protocol.raceid);
	if (!replayInfo)
	{
		ErrorLogStream << "replay(" << protocol.raceid << ") missed..." << LogStream::eos;
		return;
	}

	replayInfo->viewNum++;
	ReplayMgr::Instance()->UpdateReplayInfoToDB(replayInfo);
}

void HandleProtocol(CLProtocol::ReplayFileRes& res)
{
	/*InfoLogStream << "recv race(" << res.raceId << ") replay file data, error(" << res.error << ")." << LogStream::eos;

	if (res.error)
	{
		return;
	}

	auto replayInfo = ReplayMgr::Instance()->FindReplayInfo(res.raceId);
	if (!replayInfo)
	{
		FatalLogStream << "can't find race(" << res.raceId << ")'s replay info" << LogStream::eos;
		return;
	}

	// 有可能收到数据的时候这一局已经不需要记录了
	if (!ReplayMgr::Instance()->CanJoinMasterWaitList(replayInfo->score))
	{
		InfoLogStream << "recv race(" << res.raceId << ") replay file data, can't join master wait list, score(" << replayInfo->score << ")." << LogStream::eos;
		return;
	}

	ReplayFile* file = new ReplayFile();
	*file = res.data;
	ReplayMgr::Instance()->AddToMasterWaitList(replayInfo, file);*/
}

void ReplayFileResFunction::operator()(Avalon::Packet* packet) const
{
	Avalon::NetInputStream	stream(packet->GetBuffer(), packet->GetSize());
	UInt8 error;
	ObjID_t raceId;
	size_t srcLen;
	UInt32 totalLen;
	UInt32 index;
	static UInt8 replaySrcBuffer[Avalon::Packet::MAX_BODY_SIZE];
	try
	{
		stream & error & raceId & totalLen & index;

		if (error)
		{
			ErrorLogStream << "recv race(" << raceId << ")'s replay file return, error occur..." << LogStream::eos;
			return;
		}

		UInt16 tmpLen;
		stream & tmpLen;
		srcLen = (size_t)tmpLen;
		stream.SeriaArray(replaySrcBuffer, srcLen);

		InfoLogStream << "recv race(" << raceId << ") replay file data, index(" << index << ") srcLen(" << (UInt32)srcLen << ") total(" << totalLen << ")..." << LogStream::eos;
	}
	catch (Avalon::NetException& e)
	{
		FatalLogStream << e.what() << LogStream::eos;
		return;
	}

	auto replayInfo = ReplayMgr::Instance()->FindReplayInfo(raceId);
	if (!replayInfo)
	{
		FatalLogStream << "can't find race(" << raceId << ")'s replay info" << LogStream::eos;
		ReplayMgr::Instance()->DelReplayBuffer(raceId);
		return;
	}

	// 有可能收到数据的时候这一局已经不需要记录了
	if (!ReplayMgr::Instance()->CanJoinMasterWaitList(replayInfo->score) && replayInfo->type != PK_PREMIUM_LEAGUE_BATTLE && replayInfo->type != PK_CHAMPION_BATTLE)
	{
		FatalLogStream << "recv race(" << raceId << ") replay file data, can't join master wait list, score(" << replayInfo->score << ")." << LogStream::eos;
		ReplayMgr::Instance()->DelReplayBuffer(raceId);
		return;
	}

	// 总大小超过最大值
	if (totalLen >= 10 * 1024 * 1024)
	{
		FatalLogStream << "recv race(" << raceId << ") replay file data, len(" << totalLen << ") too large." << LogStream::eos;
		ReplayMgr::Instance()->DelReplayBuffer(raceId);
		return;
	}

	auto buffer = ReplayMgr::Instance()->GetReplayBuffer(raceId);
	if (buffer)
	{
		// 已经有缓存了，需要把新的拼到后面
		if (index + srcLen > totalLen)
		{
			FatalLogStream << "recv race(" << raceId << ") replay file data, invalid len(" << index + (UInt32)srcLen << ") total len(" << totalLen << ")." << LogStream::eos;
			ReplayMgr::Instance()->DelReplayBuffer(raceId);
			return;
		}
		
		avalon_memmove(buffer + index, replaySrcBuffer, srcLen);
	}
	else
	{
		buffer = new UInt8[totalLen];
		memset(buffer, 0, sizeof(totalLen));
		avalon_memmove(buffer, replaySrcBuffer, srcLen);
		ReplayMgr::Instance()->AddToReplayBuffer(raceId, buffer);
	}

	if (!buffer)
	{
		FatalLogStream << "recv race(" << raceId << ") replay file data, system error." << LogStream::eos;
		ReplayMgr::Instance()->DelReplayBuffer(raceId);
		return;
	}

	// 说明已经全部收到了
	if (index + srcLen < totalLen)
	{
		FatalLogStream << "recv race(" << raceId << ") replay file data, wait next packet, current pos(" << index + (UInt32)srcLen << ") total(" << totalLen << ")." << LogStream::eos;
		return;
	}

	static UInt8 replayBuffer[1024 * 1024];
	size_t len = 0;
	if (!Avalon::snappy_uncompressedlen((const char*)buffer, totalLen, &len) || len > sizeof(replayBuffer))
	{
		FatalLogStream << "recv race(" << raceId << ")'s replay file return, uncompress failed." << LogStream::eos;
		ReplayMgr::Instance()->DelReplayBuffer(raceId);
		return;
	}

	if (!Avalon::snappy_uncompress((char*)replayBuffer, (const char*)buffer, totalLen))
	{
		FatalLogStream << "recv race(" << raceId << ")'s replay file return, uncompress failed." << LogStream::eos;
		ReplayMgr::Instance()->DelReplayBuffer(raceId);
		return;
	}

	// 解压完了，把之前缓存的数据删掉
	ReplayMgr::Instance()->DelReplayBuffer(raceId);

	Avalon::NetInputStream	replayStream(replayBuffer, len);
	ReplayFile* file = new ReplayFile();
	try
	{
		replayStream & *file;
	}
	catch (Avalon::NetException& e)
	{
		FatalLogStream << "recv race(" << raceId << ")'s replay file return, decode failed." << LogStream::eos;
		return;
	}

	if (replayInfo->type == PK_PREMIUM_LEAGUE_BATTLE)
	{
		ReplayMgr::Instance()->AddToPremiumLeagueBattleList(replayInfo, file);
	}
	else if (replayInfo->type == PK_CHAMPION_BATTLE)
	{
		ReplayMgr::Instance()->InsertReplayFileToDB(file);
	}
	else
	{
		ReplayMgr::Instance()->AddToMasterWaitList(replayInfo, file);
	}
}