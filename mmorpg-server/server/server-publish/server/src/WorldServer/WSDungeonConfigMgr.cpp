#include "WSDungeonConfigMgr.h"
#include <AvalonStringUtil.h>
#include <CLSysProtocol.h>
#include "WSRouter.h"
#include "WSNetwork.h"

WSDungeonConfigMgr::WSDungeonConfigMgr()
{
	m_tickTimer.SetInterval(1000);
}

WSDungeonConfigMgr::~WSDungeonConfigMgr()
{

}

bool WSDungeonConfigMgr::Initialize()
{
	{
		VDungeonOpenTime openTimes;
		GetDungeonOpenTimeByStr("16:00-18:00", openTimes);
		SetDailyConfig(2002000, openTimes, 1);
	}

	{
		VDungeonOpenTime openTimes;
		GetDungeonOpenTimeByStr("00:00-24:00", openTimes);
		std::vector<UInt8> dayOfWeeks = { 1, 3, 5 };
		SetWeeklyConfig(2, dayOfWeeks, openTimes, 1);
	}

	{
		VDungeonOpenTime openTimes;
		GetDungeonOpenTimeByStr("00:00-24:00", openTimes);
		std::vector<UInt8> dayOfWeeks = { 2, 4, 6 };
		SetWeeklyConfig(3, dayOfWeeks, openTimes, 1);
	}

	{
		{
			VDungeonOpenTime openTimes;
			GetDungeonOpenTimeByStr("00:00-24:00", openTimes);
			AddConfig(4, 1, openTimes, 1);
		}

		{
			VDungeonOpenTime openTimes;
			GetDungeonOpenTimeByStr("10:00-24:00", openTimes);
			AddConfig(4, 2, openTimes, 1);
		}
	}

	{
		{
			VDungeonOpenTime openTimes;
			GetDungeonOpenTimeByStr("00:00-24:00", openTimes);
			AddConfig(5, 2, openTimes, 1);
		}
	}

	return true;
}

void WSDungeonConfigMgr::OnTick(const Avalon::Time& now)
{
	if(!m_tickTimer.IsTimeout(now))
	{
		return;
	}

	Avalon::Date date(now);

	UInt8 dayOfWeek = date.WDay();

	//for (auto itr = m_dungeonConfigs.begin(); itr != m_dungeonConfigs.end(); ++itr)
	for (auto& itr : m_dungeonConfigs)
	{
		auto dungeonId = itr.first;
		DungeonWeeklyConfig& weeklyConfig = itr.second;
		auto& dailyConfig = weeklyConfig.dailyConfigs[dayOfWeek - 1];

		bool isOpened = IsDungeonOpened(dungeonId);
		bool newOpen = _CheckDungeonOpenByTime(dailyConfig, date);

		if (isOpened == newOpen && !weeklyConfig.newConfig)
		{
			continue;
		}
		
		if (weeklyConfig.newConfig)
		{
			CLProtocol::SysDungeonConfigUpdate configUpdate;
			configUpdate.dungeonId = dungeonId;
			configUpdate.config = weeklyConfig;
			Router::BroadcastToScene(configUpdate);
		}

		weeklyConfig.newConfig = false;
		if (newOpen)
		{
			this->SetDungeonOpen(dungeonId, dailyConfig.dayOfWeek);
		}
		else
		{
			this->SetDungeonClose(dungeonId);
		}
	}
}



void WSDungeonConfigMgr::OnSceneConnected(UInt32 id)
{
	auto conn = WSNetwork::Instance()->FindConnection(id);
	if (conn == NULL)
	{
		return;
	}

	Avalon::Date date(CURRENT_TIME);
	UInt8 dayOfWeek = date.WDay();
	Avalon::Packet* packet = Avalon::Packet::Create();
	for (auto itr : m_dungeonConfigs)
	{
		auto dungeonId = itr.first;

		auto& weeklyConfig = itr.second;
		DungeonDailyConfig& config = weeklyConfig.dailyConfigs[dayOfWeek - 1];
		if (weeklyConfig.newConfig)
		{
			continue;
		}
		
		CLProtocol::SysDungeonConfigUpdate configUpdate;
		configUpdate.dungeonId = dungeonId;
		configUpdate.config = weeklyConfig;
		if (configUpdate.Encode(packet))
		{
			conn->SendPacket(packet);
		}

		if (IsDungeonOpened(dungeonId))
		{
			CLProtocol::SysDungeonOpenStatusChange protocol;
			protocol.dungeonId = dungeonId;
			protocol.open = 1;
			protocol.maxTimes = config.maxTimes;
			if (protocol.Encode(packet))
			{
				conn->SendPacket(packet);
			}
		}
	}

	Avalon::Packet::Destroy(packet);
}

void WSDungeonConfigMgr::SetDailyConfig(UInt32 dungeonId, const VDungeonOpenTime& openTimes, UInt8 maxTimes)
{
	ClearConfig(dungeonId);

	for (UInt8 dayOfWeek = 1; dayOfWeek <= 7; dayOfWeek++)
	{
		AddConfig(dungeonId, dayOfWeek, openTimes, maxTimes);
	}
}

void WSDungeonConfigMgr::SetWeeklyConfig(UInt32 dungeonId, std::vector<UInt8> dayOfWeeks, const VDungeonOpenTime& openTimes, UInt8 maxTimes)
{
	ClearConfig(dungeonId);

	for (auto dayOfWeek : dayOfWeeks)
	{
		AddConfig(dungeonId, dayOfWeek, openTimes, maxTimes);
	}
}

bool WSDungeonConfigMgr::GetDungeonOpenTimeByStr(const std::string openTimeStr, VDungeonOpenTime& openTimes)
{
	/*
		字符串格式：
			10:00-12:00|14:00-16:00
	*/
	std::vector<std::string> openTimeRangeStrs;
	Avalon::StringUtil::Split(openTimeStr, openTimeRangeStrs, "|");
	for (auto& openTimeRangeStr : openTimeRangeStrs)
	{
		DungeonOpenTime openTime;
		std::vector<std::string> timeStrs;
		Avalon::StringUtil::Split(openTimeRangeStr, timeStrs, "-");
		if (timeStrs.size() != 2)
		{
			return false;
		}

		std::vector<std::string> beginTimeStrs;
		std::vector<std::string> endTimeStrs;
		Avalon::StringUtil::Split(timeStrs[0], beginTimeStrs, ":");
		Avalon::StringUtil::Split(timeStrs[1], endTimeStrs, ":");

		if (beginTimeStrs.size() != 2 || endTimeStrs.size() != 2)
		{
			return false;
		}

		openTime.beginHour = Avalon::StringUtil::ConvertToValue<UInt8>(beginTimeStrs[0]);
		openTime.beginMinute = Avalon::StringUtil::ConvertToValue<UInt8>(beginTimeStrs[1]);
		openTime.endHour = Avalon::StringUtil::ConvertToValue<UInt8>(endTimeStrs[0]);
		openTime.endMinute = Avalon::StringUtil::ConvertToValue<UInt8>(endTimeStrs[1]);

		openTimes.push_back(openTime);
	}

	return true;
}

void WSDungeonConfigMgr::OnDungeonOpened(UInt32 dungeonId, UInt8 maxTimes)
{
	DebugLogStream << "dungeon(" << dungeonId << ") opened, max times(" << maxTimes << ")." << LogStream::eos;

	CLProtocol::SysDungeonOpenStatusChange protocol;
	protocol.dungeonId = dungeonId;
	protocol.open = 1;
	protocol.maxTimes = maxTimes;
	Router::BroadcastToScene(protocol);
}

void WSDungeonConfigMgr::OnDungeonClosed(UInt32 dungeonId)
{
	DebugLogStream << "dungeon(" << dungeonId << ") closed." << LogStream::eos;

	CLProtocol::SysDungeonOpenStatusChange protocol;
	protocol.dungeonId = dungeonId;
	protocol.open = 0;
	Router::BroadcastToScene(protocol);
}

bool WSDungeonConfigMgr::_CheckDungeonOpenByTime(const DungeonDailyConfig& config, const Avalon::Date& date)
{
	UInt32 sec = date.Hour() * 60 + date.Min();
	for (auto& openTime : config.openTimes)
	{
		// 计算当前时间是不是落在开放时间内
		UInt32 beginSec = openTime.beginHour * 60 + openTime.beginMinute;
		UInt32 endSec = openTime.endHour * 60 + openTime.endMinute;
		if (sec >= beginSec && sec < endSec)
		{
			return true;
		}
	}

	return false;
}