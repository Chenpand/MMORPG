#ifndef _CL_HONOR_PROTOOL_H_
#define _CL_HONOR_PROTOOL_H_

#include <AvalonProtocol.h>
#include "CLProtocolDefine.h"

namespace CLProtocol
{
	/**
	*@brief client->server 荣誉请求
	*/
	class SceneHonorReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_HONOR_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	};

	struct PvpStatistics
	{
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & pvpType & pvpCnt;
		}

		PvpStatistics() : pvpType(0), pvpCnt(0) {}
		UInt32 pvpType;
		UInt32 pvpCnt;
	};

	struct HistoryHonorInfo
	{
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & dateType & totalHonor & pvpStatisticsVec;
		}

		HistoryHonorInfo() : dateType(0), totalHonor(0) {}
		UInt32 dateType;
		UInt32 totalHonor;
		std::vector<PvpStatistics> pvpStatisticsVec;
	};

	/**
	*@brief server->client 荣誉返回
	*/
	class SceneHonorRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_HONOR_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & honorLvl & honorExp & lastWeekRank & historyRank & highestHonorLvl 
				& isUseCard & rankTime & historyHonorInfoVec;
		}

		SceneHonorRes() : honorLvl(0), honorExp(0), lastWeekRank(0), historyRank(0), 
			highestHonorLvl(0), isUseCard(0), rankTime(0){}

		// 荣誉等级
		UInt32 honorLvl;
		// 荣誉经验
		UInt32 honorExp;
		// 上周排名
		UInt32 lastWeekRank;
		// 历史排名
		UInt32 historyRank;
		// 最高荣誉等级
		UInt32 highestHonorLvl;
		// 是否使用保障卡
		UInt32 isUseCard;
		// 排名结算时间
		UInt32 rankTime;
		// 历史荣誉信息
		std::vector<HistoryHonorInfo> historyHonorInfoVec;
	};

	/**
	*@brief server->client 荣誉小红点
	*/
	class SceneHonorRedPoint : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_HONOR_RED_POINT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};
	
};

#endif