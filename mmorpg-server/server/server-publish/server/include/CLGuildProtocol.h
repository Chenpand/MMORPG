#ifndef _CL_GUILD_PROTOCOL_H_
#define _CL_GUILD_PROTOCOL_H_

#include <AvalonProtocol.h>
#include "CLProtocolDefine.h"
#include "CLGuildDefine.h"
#include "CLSkillDefine.h"
#include <CLMaskProperty.h>
#include <AvalonPacket.h>
#include <CLSyncObjectProtocol.h>

struct ItemReward;

namespace CLProtocol
{
	/**
		*@brief client->world 创建公会
		*/
	class WorldGuildCreateReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_CREATE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & name & declaration;
		}

	public:
		//公会名
		std::string	name;
		//宣言
		std::string declaration;
	};

	/**
		*@brief world->client 创建公会返回
		*/
	class WorldGuildCreateRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_CREATE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		UInt32		result;
	};

	/**
		*@brief client->server 离开公会
		*/
	class WorldGuildLeaveReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_LEAVE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
		*@brief world->client 离开公会返回
		*/
	class WorldGuildLeaveRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_LEAVE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		UInt32 result;
	};

	/**
	*@brief client->world 加入公会
	*/
	class WorldGuildJoinReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_JOIN_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

	public:
		ObjID_t			id;
	};

	/**
	*@brief world->client 加入公会返回
	*/
	class WorldJoinGuildRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_JOIN_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		UInt32		result;
	};

	/**
	*@brief client->server 请求公会列表
	*/
	class WorldGuildListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & start & num;
		}

	public:
		//开始位置 0开始
		UInt16	start;
		//数量
		UInt16	num;
	};

	/**
	*@brief world->client 返回公会列表
	*/
	class WorldGuildListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & start & totalnum & guilds;
		}

	public:
		//开始位置
		UInt16	start;
		//总数
		UInt16	totalnum;
		//部落列表
		std::vector<GuildEntry>	guilds;
	};

	/**
	*@brief client->world 请求申请入公会的列表
	*/
	class WorldGuildRequesterReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_REQUESTERS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}

	};

	/**
	*@brief world->client 返回申请入公会的列表
	*/
	class WorldGuildRequesterRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_REQUESTERS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & requesters;
		}

		//申请人列表
		std::vector<GuildRequesterInfo>	requesters;
	};

	/**
	*@brief world->client 通知新的入部落请求
	*/
	class WorldGuildNewRequester : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_NEW_REQUESTER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief client->world 处理公会加入请求
	*/
	class WorldGuildProcessRequester : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_PROCESS_REQUESTER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & agree;
		}

	public:
		//id(如果是0代表清空列表)
		ObjID_t	id;
		//同意进入(0:不同意，1:同意)
		UInt8	agree;
	};

	/**
	*@brief world->client 处理公会加入请求返回
	*/
	class WorldGuildProcessRequesterRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_PROCESS_REQUESTER_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & entry;
		}

	public:
		UInt32				result;
		GuildMemberEntry	entry;
	};

	/**
	*@brief client->world 任命职位
	*/
	class WorldGuildChangePostReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_CHANGE_POST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & post & replacerId;
		}

	public:
		//id
		ObjID_t id;
		//职位
		UInt8	post;
		//被替换的人
		ObjID_t	replacerId;
	};

	/**
	*@brief world->client 任命职位返回
	*/
	class WorldGuildChangePostRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_CHANGE_POST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		UInt32		result;
	};

	/**
	*@brief client->world 踢人
	*/
	class WorldGuildKickReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_KICK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

	public:
		//id
		ObjID_t id;
	};

	/**
	*@brief world->client 踢人返回
	*/
	class WorldGuildKickRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_KICK_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		UInt32		result;
	};



	/**
	*@brief world->client 上线或新加入公会发送初始数据
	*/
	class WorldGuildSyncInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_SYNC_BASE_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

		// 公会基础信息
		GuildBaseInfo		info;
	};


	/**
	*@brief client->world 请求公会成员列表
	*/
	class WorldGuildMemberListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_MEMBER_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guildID;
		}
		//0为查询本行会成员 别的值为查询别的行会成员，仅供公会兼并使用
		UInt64 guildID;
	};

	/**
	*@brief server->client 返回公会成员列表
	*/
	class WorldGuildMemberListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_MEMBER_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & members;
		}

		//列表
		std::vector<GuildMemberEntry>	members;
	};

	/**
	*@brief client->world 修改公会宣言
	*/
	class WorldGuildModifyDeclaration : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_MODIFY_DECLARATION)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & declaration;
		}

		std::string declaration;
	};

	/**
	*@brief client->world 修改公会名
	*/
	class WorldGuildModifyName : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_MODIFY_NAME)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & name & itemId & itemTypeId;
		}

		std::string name;
		ObjID_t		itemId;
		UInt32		itemTypeId;
	};

	/**
	*@brief world->client 修改公会公告
	*/
	class WorldGuildModifyAnnouncement : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_MODIFY_ANNOUNCEMENT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & content;
		}

		std::string content;
	};

	/**
	*@brief client->world 发送公会邮件
	*/
	class WorldGuildSendMail : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_SEND_MAIL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & content;
		}

		std::string content;
	};

	/**
	*@brief world->client 帮会通用操作返回
	*/
	class WorldGuildOperRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_OPER_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & result & param1 & param2;
		}

		// 操作类型（对应枚举GuildOperation）
		UInt8	type;
		// 结果
		UInt32	result;
		// 参数1
		UInt32	param1;
		// 参数2
		ObjID_t	param2;
	};

	/**
	*@brief client->world 升级建筑
	*/
	class WorldGuildUpgradeBuilding : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_UPGRADE_BUILDING)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type;
		}

		// 建筑类型（对应枚举GuildBuildingType）
		UInt8	type;
	};

	/**
	*@brief client->world 请求捐赠
	*/
	class WorldGuildDonateReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DONATE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & num;
		}

		// 捐赠类型（对应枚举GuildDonateType）
		UInt8	type;
		// 次数
		UInt8	num;
	};

	/**
	*@brief client->world 请求捐赠日志
	*/
	class WorldGuildDonateLogReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DONATE_LOG_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->client 返回捐赠日志
	*/
	class WorldGuildDonateLogRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DONATE_LOG_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & logs;
		}

		std::vector<GuildDonateLog> logs;
	};

	/**
	*@brief client->world 升级技能
	*/
	class WorldGuildUpgradeSkill : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_UPGRADE_SKILL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & skillId;
		}

		// 技能id
		UInt16	skillId;
	};

	/**
	*@brief client->world 解散公会
	*/
	class WorldGuildDismissReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DISMISS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief client->world 解散公会
	*/
	class WorldGuildCancelDismissReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_CANCEL_DISMISS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief client->world 请求会长信息
	*/
	class WorldGuildLeaderInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_LEADER_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->client 返回会长信息
	*/
	class WorldGuildLeaderInfoRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_LEADER_INFO_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

		GuildLeaderInfo info;
	};

	/**
	*@brief client->world 请求膜拜
	*/
	class WorldGuildOrzReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_ORZ_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type;
		}
		// 膜拜类型，对应枚举（GuildOrzType）
		UInt8		type;
	};

	/**
	*@brief client->world 请求膜拜
	*/
	class WorldGuildTableJoinReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_TABLE_JOIN_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & seat & help;
		}

		// 位置
		UInt8	seat;
		// 是不是协助
		UInt8	help;
	};

	/**
	*@brief world->client 通知客户端有新的圆桌会议成员
	*/
	class WorldGuildTableNewMember : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_TABLE_NEW_MEMBER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & member;
		}

		GuildTableMember member;
	};

	/**
	*@brief world->client 通知客户端删除圆桌会议成员
	*/
	class WorldGuildTableDelMember : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_TABLE_DEL_MEMBER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

		ObjID_t		id;
	};

	/**
	*@brief world->client 通知客户端的圆桌会议完成
	*/
	class WorldGuildTableFinish : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_TABLE_FINISH)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief client->world 请求发自费红包
	*/
	class WorldGuildPayRedPacketReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_PAY_REDPACKET_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & packetId & name & num;
		}

		UInt16	packetId;
		std::string name;
		UInt8	num;
	};

	/**
	*@brief client->world 请求公会战报名
	*/
	class WorldGuildBattleReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & terrId;
		}

	public:
		//领地ID
		UInt8	terrId;
	};

	/**
	*@brief world->client 请求公会战报名返回
	*/
	class WorldGuildBattleRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & terrId & enrollSize;
		}

	public:
		UInt32	result;
		UInt8	terrId;
		UInt32	enrollSize;
	};

	/**
	*@brief client->world 请求鼓舞
	*/
	class WorldGuildBattleInspireReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_INSPIRE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{ }
	};

	/**
	*@brief world->client 请求鼓舞返回
	*/
	class WorldGuildBattleInspireRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_INSPIRE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & inspire;
		}

	public:
		UInt32	result;
		//鼓舞次数
		UInt8	inspire;
	};

	/**
	*@brief client->world 请求领取奖励
	*/
	class WorldGuildBattleReceiveReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_RECEIVE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & boxId;
		}

	public:
		UInt8 boxId;
	};

	/**
	*@brief world->client 领取奖励返回
	*/
	class WorldGuildBattleReceiveRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_RECEIVE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & boxId;
		}

	public:
		UInt32	result;
		UInt8	boxId;
	};

	/**
	*@brief client->world 请求领地战斗记录
	*/
	class WorldGuildBattleRecordReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_RECORD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isSelf & startIndex & count;
		}

	public:
		UInt8 isSelf;
		Int32 startIndex;
		UInt32 count;
	};

	/**
	*@brief world->client 领地战斗记录返回
	*/
	class WorldGuildBattleRecordRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_RECORD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & records;
		}

	public:
		UInt32	result;
		std::vector<GuildBattleRecord> records;
	};

	/**
	*@brief world->client 领地战斗记录同步
	*/
	class WorldGuildBattleRecordSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_RECORD_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & record;
		}

	public:
		GuildBattleRecord record;
	};

	/**
	*@brief client->world 请求领地信息
	*/
	class WorldGuildBattleTerritoryReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_TERRITORY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & terrId;
		}
	public:
		UInt8 terrId;
	};

	/**
	*@brief world->client 返回领地信息
	*/
	class WorldGuildBattleTerritoryRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_TERRITORY_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & info;
		}
	public:
		UInt32 result;
		//领地信息
		GuildTerritoryBaseInfo info;
	};

	/**
	*@brief world->scene 同步玩家的公会信息到scene server
	*/
	class WorldGuildSyncToScene : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_SYNC_TO_SCENE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & guildId & guildLvl & name & post & shopId & enterTime & buildings & occupyCrossTerrId &
				guildBattleNumber & guildBattleScore & guildBattleLotteryStatus & guildBattleRewardMask & occupyTerrId;
		}

		// ID
		ObjID_t		id;
		// 公会ID
		ObjID_t		guildId;
		// 公会等级
		UInt32		guildLvl;
		// 公会名
		std::string name;
		// 职位
		UInt8		post;
		//加入时间
		UInt32		enterTime;
		// 商店id
		UInt8		shopId;
		// 公会建筑
		std::vector<GuildBuilding> buildings;
		// 工会战跨服领地
		UInt8		occupyCrossTerrId;
		// 公会战次数
		UInt32		guildBattleNumber;
		// 公会战积分
		UInt32		guildBattleScore;
		// 公会战抽奖状态
		UInt8		guildBattleLotteryStatus;
		//公会奖励情况
		std::string	guildBattleRewardMask;
		// 工会战领地
		UInt8		occupyTerrId;
	};

	/**
	*@brief world与scene之间互相同步公会贡献
	*/
	class WorldGuildSyncContribution : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_UPDATE_CONTRI)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & isAdd & value & reason;
		}

		// ID
		ObjID_t		id;
		// 是否添加
		UInt8		isAdd;
		// 公会贡献
		UInt32		value;
		// 原因
		std::string	reason;
	};

	/**
	*@brief scene->world 同步公会技能
	*/
	class WorldGuildSyncSkill : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_SYNC_SKILL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & skills;
		}

		ObjID_t id;
		std::vector<SkillBase> skills;
	};

	/**
	*@brief world->scene 同步公会技能
	*/
	class WorldGuildUpdateSkill : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_UPDATE_SKILL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & skillType & skill;
		}

		ObjID_t id;
		UInt32 skillType;
		SkillBase skill;
	};

	/**
	*@brief client->scene 公会兑换
	*/
	class SceneGuildExchangeReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_GUILD_EXCHANGE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->scene 通知领取奖励
	*/
	class WorldGuildGiveItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_GIVE_ITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & boxId & itemReward;
		}

		//玩家ID
		UInt64 playerId;
		//奖励ID
		UInt32 boxId;
		//奖励物品
		std::vector<ItemReward> itemReward;
	};

	/**
	*@brief world->scene 领取奖励返回
	*/
	class WorldGuildGiveItemRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_GIVE_ITEM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & playerId & boxId;
		}

		UInt32 result;
		//玩家ID
		UInt64 playerId;
		//奖励ID
		UInt32 boxId;
	};

	/**
	*@brief world->client 一场公会战结束-结算.
	*/
	class WorldGuildBattleRaceEnd : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_RACE_END)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & oldScore & newScore;
		}

		UInt8 result;
		UInt32 oldScore;
		UInt32 newScore;
	};

	/**
	*@brief world->client 公会战结束
	*/
	class WorldGuildBattleEnd : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_END)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

		std::vector<GuildBattleEndInfo> info;
	};

	/**
	*@brief client->world 请求自身公会排行
	*/
	class WorldGuildBattleSelfSortListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_SELF_SORTLIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	};

	/**
	*@brief world->client 请求自身公会排行响应
	*/
	class WorldGuildBattleSelfSortListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_SELF_SORTLIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & memberRanking & guildRanking;
		}

		UInt32 result;
		UInt32 memberRanking;
		UInt32 guildRanking;
	};

	/**
	*@brief world->client 公会邀请通知，有别的玩家邀请加入公会
	*/
	class WorldGuildInviteNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_INVITE_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & inviterId & inviterName & inviterOccu & inviterLevel & inviterVipLevel
				& guildId & guildName & playerLabelInfo;
		}

		ObjID_t		inviterId;
		std::string	inviterName;
		UInt8		inviterOccu;
		UInt16		inviterLevel;
		UInt8		inviterVipLevel;
		ObjID_t		guildId;
		std::string guildName;
		//玩家标签信息
		PlayerLabelInfo playerLabelInfo;
	};

	/**
	*@brief world->client 公会战状态同步
	*/
	class WorldGuildBattleStatusSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_STATUS_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & status & time & endInfo;
		}

		//公会战类型
		UInt8 type;

		//状态
		UInt8 status;

		//状态结束的时间戳
		UInt32 time;

		//公会战结束情况
		std::vector<GuildBattleEndInfo> endInfo;
	};

	/**
	*@brief client->world 请求公会宣战报名
	*/
	class WorldGuildChallengeReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_CHALLENGE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & terrId & itemId & itemNum;
		}

		//领地ID
		UInt8 terrId;
		//消耗物品ID
		UInt32 itemId;
		//消耗物品数量
		UInt32 itemNum;

	};


	/**
	*@brief world->client 返回公会宣战报名
	*/
	class WorldGuildChallengeRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_CHALLENGE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		UInt32	result;
	};


	/**
	*@brief client->world 请求公会宣战信息
	*/
	class WorldGuildChallengeInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_CHALLENGE_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->client 公会宣战信息同步
	*/
	class WorldGuildChallengeInfoSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_CHALLENGE_INFO_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info & enrollGuildId & enrollGuildName & enrollGuildleaderName & enrollGuildLevel & itemId & itemNum;
		}

	public:
		//领地信息
		GuildTerritoryBaseInfo info;

		//宣战公会ID
		UInt64 enrollGuildId;

		//宣战公会名称
		std::string enrollGuildName;

		//宣战公会会长名称
		std::string enrollGuildleaderName;

		//宣战公会等级
		UInt8 enrollGuildLevel;

		//消耗道具id
		UInt32 itemId;

		//消耗道具数量
		UInt32 itemNum;
	};

	/**
	*@brief client->world 请求公会战鼓舞信息
	*/
	class WorldGuildBattleInspireInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_INSPIRE_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			
		}

	};
	
	/**
	*@brief world->client 返回公会战鼓舞信息
	*/
	class WorldGuildBattleInspireInfoRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_INSPIRE_INFO_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result &terrId & inspireInfos;
		}

		UInt32 result;

		//领地ID
		UInt8 terrId;

		//成员鼓舞信息
		std::vector<GuildBattleInspireInfo> inspireInfos;

	};

	/**
	*@brief client->world 请求公会设置
	*/
	class WorldGuildStorageSettingReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_STORAGE_SETTING_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & value;
		}

		// 设置类型
		UInt8 type;

		// 设置数值
		UInt32 value;

	};

	/**
	*@brief world->client 返回公会设置
	*/
	class WorldGuildStorageSettingRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_STORAGE_SETTING_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		//结果
		UInt32 result;

	};


	/**
	*@brief world->scene 放入公会仓库请求
	*/
	class WSAddGuildStorageItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WS_GUILD_ADDSTORAGEITEM_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerUid & guildUid & items;
		}

		UInt64 playerUid;
		UInt64 guildUid;

		//放入的道具
		std::vector<GuildStorageItemInfo> items;
	};

	/**
	*@brief world->scene 商店购买放入公会仓库返回
	*/
	class WSShopBuyAddGuildStorageRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WS_GUILDSTORAGE_ADD_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerUid & code & shopId & shopItemId & num & costItemId & costNum;
		}

		UInt64 playerUid;
		UInt32 code;
		UInt8 shopId;
		UInt32 shopItemId;
		UInt16 num;
		UInt32 costItemId;
		UInt32 costNum;
	};

	/**
	*@brief scene->world 放入公会仓库返回
	*/
	class SWAddGuildStorageItemRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SW_GUILD_ADDSTORAGEITEM_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			
		}

	};

	/**
	*@brief scene->world 商店购买放入公会仓库
	*/
	class SWShopBuyAddGuildStorage : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SW_GUILDSTORAGE_ADD_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}

	};

	
	/**
	*@brief client->world 请求公会战抽奖
	*/
	class WorldGuildBattleLotteryReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_LOTTERY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{}
	};

	/**
	*@brief world->client 返回公会战抽奖
	*/
	class WorldGuildBattleLotteryRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_BATTLE_LOTTERY_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & contribution;
		}

		UInt32 result;
		UInt32 contribution;
	};

	/**
	*@brief client->world 请求公会仓库列表
	*/
	class WorldGuildStorageListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_STORAGE_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{}
	};

	/**
	*@brief world->client 返回公会仓库列表
	*/
	class WorldGuildStorageListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_STORAGE_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & maxSize & items & itemRecords;
		}

		UInt32 result;

		UInt32 maxSize;

		std::vector<GuildStorageItemInfo> items;

		std::vector<GuildStorageOpRecord> itemRecords;
	};

	/**
	*@brief world->client 同步仓库物品数据
	*/
	class WorldGuildStorageItemSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_STORAGE_ITEM_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & items & itemRecords;
		}

		std::vector<GuildStorageItemInfo> items;

		std::vector<GuildStorageOpRecord> itemRecords;
	};

	/**
	*@brief client->world 请求放入公会仓库
	*/
	class WorldGuildAddStorageReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_ADD_STORAGE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & items;
		}

		//放入的道具
		std::vector<GuildStorageItemInfo> items;
	};

	/**
	*@brief world->client 返回放入公会仓库
	*/
	class WorldGuildAddStorageRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_ADD_STORAGE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		UInt32 result;
	};

	/**
	*@brief client->world 请求删除公会仓库物品
	*/
	class WorldGuildDelStorageReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DEL_STORAGE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & items;
		}

		std::vector<GuildStorageDelItemInfo> items;
	};

	/**
	*@brief world->client 返回删除公会仓库物品
	*/
	class WorldGuildDelStorageRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DEL_STORAGE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		UInt32 result;
	};

	/**
	*@brief client->world 查看公会仓库道具详细信息
	*/
	class WorldWatchGuildStorageItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_WATCH_STORAGE_ITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & uid;
		}

		UInt64 uid;
	};

	/**
	*@brief cross->world 同步跨服领地列表信息
	*/
	class WorldCrossGuildBattleTerritoryListInfoSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CROSS_GUILD_BATTLE_TERRITORY_LIST_INFO_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & infos;
		}

		std::vector<GuildTerritoryBaseInfo> infos;
	};

	/**
	*@brief cross->world 同步跨服领地信息
	*/
	class WorldCrossGuildBattleTerritoryInfoSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CROSS_GUILD_BATTLE_TERRITORY_INFO_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

		GuildTerritoryBaseInfo info;
	};

	/**
	*@brief cross->world 同步跨服领地战斗信息
	*/
	class WorldCrossGuildBattleRecordInfoSync :public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CROSS_GUILD_BATTLE_RECORD_INFO_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & terrId & info;
		}

		//领地ID
		UInt8 terrId;
		//战斗记录
		GuildBattleRecord info;
	};

	/**
	*@brief cross->world 同步工会战结束信息
	*/
	class WorldCrossGuildBattleEndInfoSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CROSS_GUILD_BATTLE_END_INFO_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

		std::vector<GuildBattleEndInfo> info;

	};


	/**
	*@brief cross->world 向world请求报名信息
	*/
	class WorldCrossGuildBattleEnrollInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CROSS_GUILD_BATTLE_ENROLL_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}

	};

	/**
	*@brief world->cross 向world请求报名信息返回
	*/
	class WorldCrossGuildBattleEnrollInfoRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CROSS_GUILD_BATTLE_ENROLL_INFO_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & terrEnrollInfo & terrBaseInfo;
		}

		UInt32 result;
		std::map<UInt8, std::vector<CSEnrollInfo>> terrEnrollInfo;
		std::vector<GuildTerritoryBaseInfo> terrBaseInfo;
	};

	/**
	*@brief world->cross 同步排行榜信息
	*/
	class CrossGuildBattleSortListSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_GUILD_BATTLE_SORT_LIST_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & terrId & entry;
		}

		UInt8 terrId;

		TerritorySortListEntry entry;
	};

	/**
	*@brief world->cross 同步GM命令
	*/
	class CrossGuildBattleGMSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_GUILD_BATTLE_GM_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isByTime & targetStatus;
		}

		UInt8 isByTime;
		UInt8 targetStatus;
	};

	/**
	*@brief world->cross 通知Cross报名信息
	*/
	class CrossGuildBattleEnrollReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_GUILD_BATTLE_ENROLL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

		CSEnrollInfo info;
	};

	/**
	*@brief cross->world 通知Cross报名信息返回
	*/
	class CrossGuildBattleEnrollRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_GUILD_BATTLE_ENROLL_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		UInt32 result;
	};

	/**
	*@brief world->cross 请求跨服领地战斗记录
	*/
	class CrossGuildBattleRecordReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_GUILD_BATTLE_RECORD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & terrId & isSelf & startIndex & count;
		}

		UInt64 playerId;
		UInt8 terrId;
		UInt8 isSelf;
		Int32 startIndex;
		UInt32 count;
	};

	/**
	*@brief cross->world 请求跨服领地战斗记录返回
	*/
	class CrossGuildBattleRecordRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_GUILD_BATTLE_RECORD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & result & records;
		}

		UInt64 playerId;
		UInt32 result;
		std::vector<GuildBattleRecord> records;
	};

	/**
	*@brief world->cross 请求跨服排行榜记录
	*/
	class CrossGuildBattleSortListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_GUILD_BATTLE_SORT_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & terrId & playerId & guildId & type & start & num;
		}

		UInt8 terrId;
		//玩家ID
		ObjID_t playerId;
		//工会ID
		ObjID_t guildId;
		//排行榜类型
		UInt8	type;
		//开始位置 0开始
		UInt16	start;
		//数量
		UInt16	num;
	};

	/**
	*@brief cross->world 请求跨服领地排行榜返回
	*/
	class CrossGuildBattleSortListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_GUILD_BATTLE_SORT_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & (*packet);
		}

		CrossGuildBattleSortListRes() { packet = Avalon::Packet::Create(); }
		CrossGuildBattleSortListRes(Avalon::Packet* pack) { packet = pack; }
		~CrossGuildBattleSortListRes() { Avalon::Packet::Destroy(packet); }

		void Detach()
		{
			packet = NULL;
		}

		ObjID_t playerId;
		Avalon::Packet* packet;
	};

	/**
	*@brief world->cross 请求领地自身公会排行榜
	*/
	class CrossGuildBattleSelfSortListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_GUILD_BATTLE_SELF_SORT_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & guildId & terrId;
		}

		//玩家ID
		ObjID_t playerId;
		//工会ID
		ObjID_t guildId;
		// 领地ID
		UInt8 terrId;
	};

	/**
	*@brief cross->world 请求跨服领地排行榜返回
	*/
	class CrossGuildBattleSelfSortListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(CROSS_GUILD_BATTLE_SELF_SORT_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & (*packet);
		}

		CrossGuildBattleSelfSortListRes() { packet = Avalon::Packet::Create(); }
		CrossGuildBattleSelfSortListRes(Avalon::Packet* pack) { packet = pack; }
		~CrossGuildBattleSelfSortListRes() { Avalon::Packet::Destroy(packet); }

		void Detach()
		{
			packet = NULL;
		}

		ObjID_t playerId;
		Avalon::Packet* packet;
	};
	
	/**
	*@brief client->world 设置加入公会等级请求
	*/
	class WorldGuildSetJoinLevelReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_SET_JOIN_LEVEL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & joinLevel;
		}

		UInt32 joinLevel;
	};

	/**
	*@brief world->client 设置加入公会等级返回
	*/
	class WorldGuildSetJoinLevelRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_SET_JOIN_LEVEL_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		UInt32 result;
	};


	/**
	*@brief client->world 公会徽记升级请求
	*/
	class WorldGuildEmblemUpReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_EMBLEM_UP_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->client 公会徽记升级返回
	*/
	class WorldGuildEmblemUpRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_EMBLEM_UP_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & emblemLevel;
		}

		UInt32 result;
		UInt32 emblemLevel;
	};

	/**
	*@brief world->scene 公会徽记同步到
	*/
	class WorldGuildEmblemSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_EMBLEM_SYNC_TO_SCENE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & emblemLevel;
		}

		UInt64 roleId;
		UInt32 emblemLevel;
	};
	
	/**
	*@brief client->world 设置公会副本难度请求
	*/
	class WorldGuildSetDungeonTypeReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_SET_DUNGEON_TYPE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & dungeonType;
		}

		// 公会副本难度
		UInt32 dungeonType;
	};

	/**
	*@brief world->client 设置公会副本难度返回
	*/
	class WorldGuildSetDungeonTypeRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_SET_DUNGEON_TYPE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & dungeonType;
		}

		// 结果
		UInt32 result;
		// 公会副本难度
		UInt32 dungeonType;
	};

	/**
	*@brief client->world 公会领地每日奖励请求
	*/
	class WorldGuildGetTerrDayRewardReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_GET_TERR_DAY_REWARD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	};

	/**
	*@brief world->client 公会领地每日奖励返回
	*/
	class WorldGuildGetTerrDayRewardRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_GET_TERR_DAY_REWARD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		UInt32 result;
	};


	//********************公会地下城**************************

	/**
	*@brief client->world 公会地下城信息请求
	*/
	class WorldGuildDungeonInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DUNGEON_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	struct GuildDungeonBossBlood
	{
		GuildDungeonBossBlood() : dungeonId(0), oddBlood(0), verifyBlood(0) {}
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & dungeonId & oddBlood & verifyBlood;
		}

		// 地下城id
		UInt32 dungeonId;
		// 剩余血量
		UInt64 oddBlood;
		// 待验证血量
		UInt64 verifyBlood;
	};

	struct GuildDungeonClearGateTime
	{
		GuildDungeonClearGateTime() : spendTime(0) {}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guildName & spendTime;
		}
		// 公会名字
		std::string guildName;
		// 通关用时
		UInt64 spendTime;
	};

	/**
	*@brief world->client 公会地下城信息返回
	*/
	class WorldGuildDungeonInfoRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DUNGEON_INFO_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & dungeonStatus & statusEndStamp & isReward & bossBlood & clearGateVec;
		}

		// 返回结果
		UInt32 result;
		// 地下城状态
		UInt32 dungeonStatus;
		// 状态结束的时间戳
		UInt32 statusEndStamp;
		// 是否已经领取奖励宝箱
		UInt32 isReward;
		// boss血量信息
		std::vector<GuildDungeonBossBlood> bossBlood;
		// 通关时间排行榜
		std::vector<GuildDungeonClearGateTime> clearGateVec;
	};
	
	/**
	*@brief client->world 公会地下城伤害排行请求
	*/
	class WorldGuildDungeonDamageRankReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DUNGEON_DAMAGE_RANK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	// 伤害信息
	struct GuildDungeonDamage
	{
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & rank & damageVal & playerId& playerName;
		}

		// 排名
		UInt32 rank;
		// 伤害值
		UInt64 damageVal;
		// 玩家ID
		UInt64 playerId;
		// 玩家名字
		std::string playerName;

		GuildDungeonDamage() : rank(0), damageVal(0), playerId(0) {}
	};

	/**
	*@brief world->client 公会地下城伤害排行返回
	*/
	class WorldGuildDungeonDamageRankRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DUNGEON_DAMAGE_RANK_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & damageVec;
		}

		// 伤害列表
		std::vector<GuildDungeonDamage> damageVec;
	};

	/**
	*@brief client->world 公会地下城副本信息请求
	*/
	class WorldGuildDungeonCopyReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DUNGEON_COPY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->client 公会地下城副本信息返回
	*/
	class WorldGuildDungeonCopyRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DUNGEON_COPY_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & BattleRecordVec;
		}

		// 公会地下城信息
		std::vector<GuildDungeonBattleRecord> BattleRecordVec;
	};

	/**
	*@brief client->world 
	*/
	class WorldGuildDungeonLotteryReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DUNGEON_LOTTERY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->client 公会地下城抽奖返回
	*/
	class WorldGuildDungeonLotteryRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DUNGEON_LOTTERY_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & lotteryItemVec;
		}

		// 结果
		UInt32 result;
		// 奖励
		GuildDungeonRewardVec lotteryItemVec;
	};

	/**
	*@brief world->client 公会地下城状态同步
	*/
	class WorldGuildDungeonStatusSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DUNGEON_STATUS_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & dungeonStatus;
		}

		// 地下城状态
		UInt32 dungeonStatus;
	};

	/**
	*@brief client->world 公会地下城雕像请求
	*/
	class WorldGuildDungeonStatueReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DUNGEON_STATUE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->client 公会地下城雕像返回
	*/
	class WorldGuildDungeonStatueRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DUNGEON_STATUE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & figureStatue;
		}

		// 要展示的人物雕像
		std::vector<FigureStatueInfo> figureStatue;
	};

	/**
	*@brief world->client 公会地下城boss死亡通知
	*/
	class WorldGuildDungeonBossDeadNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DUNGEON_BOSS_DEAD_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & dungeonId;
		}

		// 地下城id
		UInt32 dungeonId;
	};
	
	/**
	*@brief world->client 公会地下城结束通知
	*/
	class WorldGuildDungeonEndNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DUNGEON_END_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & dungeonId;
		}

		UInt32 dungeonId;
	};

	/**
	*@brief world->client 公会地下城boss剩余血量
	*/
	class WorldGuildDungeonBossOddBlood : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_DUNGEON_BOSS_ODD_BLOOD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & bossOddBlood & bossTotalBlood;
		}

		UInt64 bossOddBlood;
		UInt64 bossTotalBlood;
	};

	/**
	*@brief client->world 拍卖行信息请求
	*/
	class WorldGuildAuctionItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_AUCTION_ITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type;
		}

		// 拍卖类型
		UInt32 type;
	};

	/**
	*@brief world->client 拍卖行信息返回
	*/
	class WorldGuildAuctionItemRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_AUCTION_ITEM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & auctionItemVec;
		}

		// 拍卖类型
		UInt32 type;
		// 拍卖物品列表
		std::vector<GuildAuctionItem> auctionItemVec;
	};

	/**
	*@brief client->world 拍卖行出价请求
	*/
	class WorldGuildAuctionBidReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_AUCTION_BID_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guid & bidPrice;
		}

		// guid
		UInt64 guid;
		// 出价
		UInt32 bidPrice;
	};

	/**
	*@brief world->client 拍卖行出价返回
	*/
	class WorldGuildAuctionBidRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_AUCTION_BID_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode;
		}

		// 返回码
		UInt32 retCode;
	};

	/**
	*@brief client->world 拍卖行一口价请求
	*/
	class WorldGuildAuctionFixReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_AUCTION_FIX_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guid;
		}

		// guid
		UInt64 guid;
	};

	/**
	*@brief world->client 拍卖行一口价返回
	*/
	class WorldGuildAuctionFixRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_AUCTION_FIX_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode;
		}

		// 返回码
		UInt32 retCode;
	};

	/**
	*@brief world->client 拍卖行通知
	*/
	class WorldGuildAuctionNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_AUCTION_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & isOpen;
		}

		// 类型
		UInt32 type;
		// 是否打开
		UInt32 isOpen;
	};


	/**
	*@brief client->world 查看可兼并的公会列表请求
	*/
	class WorldGuildWatchCanMergerReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_WATCH_CAN_MERGER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & start & num;
		}
		//开始位置
		UInt16	start;
		//总数
		UInt16	num;
	};

	/**
	*@brief world -> client  查看可兼并的公会列表返回
	*/
	class WorldGuildWatchCanMergerRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_WATCH_CAN_MERGER_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & start & totalNum & guilds;
		}

		//开始位置
		UInt16	start;
		//总数
		UInt16	totalNum;
		//公会列表
		std::vector<GuildEntry>	guilds;
	};

	/**
	*@brief client -> world 兼并操作，包括申请兼并，取消申请兼并请求
	*/
	class WorldGuildMergerRequestOperatorReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_MERGER_REQUEST_OPERATOR_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guildId & opType;
		}
		UInt64 guildId;
		//操作类型 0申请兼并 1取消申请兼并
		UInt8 opType;
	};

	/**
	*@brief world -> client 兼并操作结果
	*/
	class WorldGuildMergerRequestOperatorRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_MERGER_REQUEST_OPERATOR_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & errorCode & opType;
		}

		//操作结果
		UInt32 errorCode;
		//操作类型 0申请兼并 1取消申请兼并
		UInt8 opType;
	};

	/**
	*@brief client ->  world 查询兼并请求
	*/
	class WorldGuildReceiveMergerRequestReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_RECEIVE_MERGER_REQUEST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world -> client  返回兼并请求
	*/
	class WorldGuildReceiveMergerRequestRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_RECEIVE_MERGER_REQUEST_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isHave;
		}
		//是否有兼并请求 0没有 1有
		UInt8 isHave;
	};



	/**
	*@brief client ->  world 查看本公会收到的兼并申请列表
	*/
	class WorldGuildWatchHavedMergerRequestReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_WATCH_HAVED_MERGER_REQUEST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world -> client  返回本公会收到的兼并申请列表
	*/
	class WorldGuildWatchHavedMergerRequestRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_WATCH_HAVED_MERGER_REQUEST_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guilds;
		}
		//申请列表 此处需要一个申请结构
		std::vector<GuildEntry>	guilds;
	};


	/**
	*@brief client ->  world 同意，拒绝公会兼并申请，取消已同意的申请
	*/
	class WorldGuildAcceptOrRefuseOrCancleMergerRequestReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_ACCEPT_OR_REFUSE_OR_CANCLE_MERGER_REQUEST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guildId & opType;
		}
		UInt64 guildId;
		//操作类型 0同意 1拒绝 2取消 3清空请求
		UInt8 opType;
	};

	/**
	*@brief world -> client  返回申请请求操作结果
	*/
	class WorldGuildAcceptOrRefuseOrCancleMergerRequestRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_ACCEPT_OR_REFUSE_OR_CANCLE_MERGER_REQUEST_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & errorCode & opType;
		}
		UInt32 errorCode;
		//操作类型 0同意 1拒绝 2取消 3清空请求
		UInt8 opType;
	};
	//**************************************

	class WorldGuildSyncMergerInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_SYNC_MERGER_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & prosperityStatus & mergerRequsetStatus;
		}
		//公会繁荣状态 1解散 2低繁荣 3中繁荣 4高繁荣
		UInt8 prosperityStatus;
		//请求状态 0无请求 1已申请 2已接受
		UInt8 mergerRequsetStatus;
	};
	
	/**
	 *@brief  client->world 获取公会事件列表
	 */
	class WorldGuildEventListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_EVENT_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & uptime;
		}

	public:
		//上次拉取的时间戳,0为第一次拉取
		UInt32	uptime;
	};

	/**
	 *@brief  world->client 获取公会事件列表返回
	 */
	class WorldGuildEventListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_EVENT_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & uptime & guildEvents;
		}

	public:
		//当前事件戳, 用于下次拉取参考
		UInt32	uptime;
		//公会事件列表
		std::vector<GuildEvent>	guildEvents;
	};


	/**
	*@brief server->world 同步公会日志
	*/
	class SyncGuildEventToWorld : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SYNC_GUILD_EVENT_TO_WORLD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & content;
		}

		// guildID
		ObjID_t		id;
		// 公会日志
		std::string	content;
	};

	/**
	*@brief client->world 请求公会荣誉
	*/
	class WorldGuildHonorInfoReq: public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_HONOR_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guildId & groupIds;
		}
		//公会荣誉
		UInt64			guildId;
		std::vector<UInt32> groupIds;
	};


	class WorldGuildHonorInfoRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GUILD_HONOR_INFO_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guildId & guildName & honors;
		}
		UInt64					guildId;
		std::string				guildName;
		//公会荣誉
		std::vector<GuildHonor> honors;
	};
}



#endif // _CL_GUILD_PROTOCOL_H_