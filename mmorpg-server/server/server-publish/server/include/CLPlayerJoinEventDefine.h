#ifndef __CL_PLAYER_JOIN_EVENT_DEFINE_H__
#define __CL_PLAYER_JOIN_EVENT_DEFINE_H__

// 类型
enum PlayerJoinEventType
{
	PJET_TYPE_GUILD_NONE = 0,
	// 参与公会战成员
	PJET_THR_TYPE_GUILD_BATTLE = 2,
	// 参与跨服公会战成员
	PJET_THR_TYPE_GUILD_CROSS_BATTLE = 3,
	// 参与公会地下城成员
	PJET_THR_TYPE_GUILD_DUNGEON = 4,

	PJET_THR_TYPE_GUIL_MAX,
};

//最近一次公会战唯一id
#define LAST_GUILD_BATTLE_GUID_KEY "last_guild_battle_guid"
//最近一次跨服公会战唯一id
#define LAST_GUILD_CROSS_BATTLE_STATUS_KEY "last_guild_cross_battle_guid"
//最近一次公会地下城唯一id
#define LAST_GUILD_BATTLE_STATUS_KEY "last_guild_dungeon_guid"

#endif