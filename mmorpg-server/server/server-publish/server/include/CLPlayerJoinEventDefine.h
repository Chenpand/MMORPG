#ifndef __CL_PLAYER_JOIN_EVENT_DEFINE_H__
#define __CL_PLAYER_JOIN_EVENT_DEFINE_H__

// ����
enum PlayerJoinEventType
{
	PJET_TYPE_GUILD_NONE = 0,
	// ���빫��ս��Ա
	PJET_THR_TYPE_GUILD_BATTLE = 2,
	// ����������ս��Ա
	PJET_THR_TYPE_GUILD_CROSS_BATTLE = 3,
	// ���빫����³ǳ�Ա
	PJET_THR_TYPE_GUILD_DUNGEON = 4,

	PJET_THR_TYPE_GUIL_MAX,
};

//���һ�ι���սΨһid
#define LAST_GUILD_BATTLE_GUID_KEY "last_guild_battle_guid"
//���һ�ο������սΨһid
#define LAST_GUILD_CROSS_BATTLE_STATUS_KEY "last_guild_cross_battle_guid"
//���һ�ι�����³�Ψһid
#define LAST_GUILD_BATTLE_STATUS_KEY "last_guild_dungeon_guid"

#endif