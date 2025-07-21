#ifndef __SS_DUNGEON_CONFIG_MGR_H__
#define __SS_DUNGEON_CONFIG_MGR_H__

#include <CLDungeonConfigMgr.h>
#include <AvalonSingleton.h>
#include <CLVipDefine.h>
#include <CLDungeonDataEntry.h>

struct ActivityDungeonConfig
{
	ActivityDungeonConfig()
	{
		memset(this, 0, sizeof(*this));
	}

	DungeonSubType		type;
	UInt32				activityId;
	UInt32				maxTimes;
	// Vip特权，如果有特权代表不同的VIP会增加次数
	VipPrivilegeType	vipPrivilege;
};
typedef std::map<DungeonSubType, ActivityDungeonConfig> MapActivityDungeonConfig;

class Player;
// 活动关卡管理
class SSActivityDungeonMgr : public Avalon::Singleton<SSActivityDungeonMgr>
{
public:
	SSActivityDungeonMgr();
	~SSActivityDungeonMgr();

	bool Init();

	void RegActivityDungeon(DungeonSubType type, UInt32 maxTimes, UInt32 activityId, VipPrivilegeType vipPrivilege = VIP_PRIVILEGE_INVALID);
	void DelActivityDungeon(DungeonSubType type);

	bool IsDungeonOpened(UInt32 dungeonId);
	bool IsDungeonTimesLimit(UInt32 dungeonId);
	UInt32 GetDungeonMaxTimes(Player* player, DungeonSubType type);
	UInt32 GetDungeonBuyTimes(Player* player, DungeonSubType type);
	UInt32 GetDungeonWeekMaxTimes(Player* player, DungeonSubType type);
	bool CheckWeekTimes(Player* player, DungeonSubType type);
private:
	MapActivityDungeonConfig		m_dungeonConfigs;
};

#endif