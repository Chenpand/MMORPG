#ifndef __DIG_MAP_MGR_H__
#define __DIG_MAP_MGR_H__

#include <AvalonSingleton.h>
#include "Dig.h"
#include "DigMap.h"
#include "WSPlayer.h"

class DigMapMgr : public Avalon::Singleton<DigMapMgr>
{
public:
	DigMapMgr() :m_LastRefreshTime(0){};
	~DigMapMgr() {}

	bool Init();

public:
	
	UInt32 OpenMap(WSPlayer* player, UInt32 mapId, std::vector<DigInfo>& infos, UInt32& playerSize);

	UInt32 CloseMap(WSPlayer* player, UInt32 mapId);

	UInt32 WatchDig(UInt32 mapId, UInt32 index, DigDetailInfo& info);

	UInt32 OpenDig(WSPlayer* player, UInt32 mapId, UInt32 digIndex);

	UInt32 GetDigMapInfo(std::vector<DigMapInfo>& infos);

	void Refresh(UInt32 refreshTime, bool isInit = false);

	void Supply(UInt32 supplyTime);

	void SyncToPlayers(Avalon::Protocol& sync, UInt32 mapId);

	void SyncToPlayers(Avalon::Protocol& sync);

	void AddDigRecord(DigRecordInfo info);

	UInt32 GetDigRecords(std::vector<DigRecordInfo>& infos);

	UInt32 GMOpenDig(WSPlayer* player, UInt32 mapId);

	UInt32 GMRandomOpenDig(WSPlayer* player, DigType digType, UInt32& itemId, UInt32& itemNum);

public:
	void OnTick(Avalon::Time now);

	void OnPlayerOnline(WSPlayer* player);

	void OnPlayerOffline(WSPlayer* player);

private:

	UInt32 GetSupplyNum(UInt32 remainDigNum, UInt32 openDigNum);

	DigMap* GetDigMap(UInt32 mapId);

private:
	std::vector<DigMap> m_DigMaps;

	UInt32 m_LastRefreshTime;

	UInt32 m_NextRefreshTime;

	UInt32 m_SupplyTime;

	UInt32 m_StartHour;

	UInt32 m_EndHour;

	UInt32 m_SupplySum;

	std::vector<DigRecordInfo> m_DigRecords;

};


#endif //__DIG_MGR_H__