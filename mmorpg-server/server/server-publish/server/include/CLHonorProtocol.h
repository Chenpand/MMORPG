#ifndef _CL_HONOR_PROTOOL_H_
#define _CL_HONOR_PROTOOL_H_

#include <AvalonProtocol.h>
#include "CLProtocolDefine.h"

namespace CLProtocol
{
	/**
	*@brief client->server ��������
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
	*@brief server->client ��������
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

		// �����ȼ�
		UInt32 honorLvl;
		// ��������
		UInt32 honorExp;
		// ��������
		UInt32 lastWeekRank;
		// ��ʷ����
		UInt32 historyRank;
		// ��������ȼ�
		UInt32 highestHonorLvl;
		// �Ƿ�ʹ�ñ��Ͽ�
		UInt32 isUseCard;
		// ��������ʱ��
		UInt32 rankTime;
		// ��ʷ������Ϣ
		std::vector<HistoryHonorInfo> historyHonorInfoVec;
	};

	/**
	*@brief server->client ����С���
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