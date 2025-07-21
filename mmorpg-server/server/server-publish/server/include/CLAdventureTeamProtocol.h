#ifndef _CL_ADVENTURE_TEAM_PROTOCOL_H_
#define _CL_ADVENTURE_TEAM_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLGameDefine.h"

namespace CLProtocol
{
	/**
	*@brief 冒险队扩展信息
	*/
	struct AdventureTeamExtraInfo : public AdventureTeamInfo
	{
		AdventureTeamExtraInfo() :adventureTeamRanking(0), ownRoleFileds(0), roleTotalScore(0)
		{}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & adventureTeamName & adventureTeamLevel & adventureTeamExp & adventureTeamGrade 
				& adventureTeamRanking & ownRoleFileds & roleTotalScore;
		}

		// 冒险队评级
		std::string	adventureTeamGrade;
		// 冒险队排名
		UInt32 adventureTeamRanking;
		// 拥有角色栏位数
		UInt32 ownRoleFileds;
		// 冒险团角色总评分
		UInt32 roleTotalScore;
	};

	/**
	*@brief 远征队员信息
	*/
	struct ExpeditionMemberInfo
	{
		ExpeditionMemberInfo()
		: roleid(0), level(0), occu(0), expeditionMapId(0)
		{}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleid & name & level & occu & avatar & expeditionMapId;
		}

		// 角色id
		ObjID_t roleid;
		// 角色名
		std::string name;
		// 角色等级
		UInt16 level;
		// 角色职业
		UInt8 occu;
		// 角色外观
		PlayerAvatar avatar;
		// 远征地图id
		UInt8 expeditionMapId;
	};

	/**
	*@brief 远征地图基本信息
	*/
	struct ExpeditionMapBaseInfo
	{
		ExpeditionMapBaseInfo()
		: mapId(0), expeditionStatus(0), memberNum(0)
		{}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mapId & expeditionStatus & memberNum;
		}

		// 地图id
		UInt8 mapId;
		// 远征状态(对应枚举ExpeditionStatus)
		UInt8 expeditionStatus;
		// 远征队员数
		UInt16 memberNum;
	};

	/**
	 *@brief server->client 同步冒险队信息
	 */
	class AdventureTeamInfoSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_SYNC_ADVENTURE_TEAM_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

	public:
		// 冒险队信息
		AdventureTeamExtraInfo info;
	};

	/**
	*@brief client->world 冒险队队名修改请求
	*/
	class WorldAdventureTeamRenameReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ADVENTURE_TEAM_RENAME_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & newName & costItemUId & costItemDataId;
		}

	public:
		// 要修改的新名字
		std::string newName;
		// 需要消耗的道具
		ObjID_t costItemUId;
		UInt32 costItemDataId;
	};

	/**
	*@brief world->client 冒险队队名修改返回
	*/
	class WorldAdventureTeamRenameRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ADVENTURE_TEAM_RENAME_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & resCode & newName;
		}

	public:
		// 错误码
		UInt32 resCode;
		// 新名字
		std::string newName;
	};

	/**
	*@brief client->world 冒险队扩展信息请求
	*/
	class WorldAdventureTeamExtraInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ADVENTURE_TEAM_EXTRA_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->client 冒险队扩展信息返回
	*/
	class WorldAdventureTeamExtraInfoRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ADVENTURE_TEAM_EXTRA_INFO_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & resCode & extraInfo;
		}

	public:
		// 错误码
		UInt32 resCode;
		AdventureTeamExtraInfo extraInfo;
	};

	/**
	*@brief client->world 赐福水晶信息请求
	*/
	class WorldBlessCrystalInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BLESS_CRYSTAL_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->client 赐福水晶信息返回
	*/
	class WorldBlessCrystalInfoRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BLESS_CRYSTAL_INFO_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & resCode & ownBlessCrystalNum & ownBlessCrystalExp;
		}

		// 错误码
		UInt32 resCode;
		// 拥有的赐福水晶数量
		UInt32 ownBlessCrystalNum;
		// 拥有的赐福水晶经验
		UInt64 ownBlessCrystalExp;
	};

	/**
	*@brief client->world 传承祝福信息请求
	*/
	class WorldInheritBlessInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_INHERIT_BLESS_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->client 传承祝福信息返回
	*/
	class WorldInheritBlessInfoRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_INHERIT_BLESS_INFO_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & resCode & ownInheritBlessNum & ownInheritBlessExp;
		}

		// 错误码
		UInt32 resCode;
		// 拥有的传承祝福数量
		UInt32 ownInheritBlessNum;
		// 拥有的传承祝福经验
		UInt64 ownInheritBlessExp;
	};

	/**
	*@brief client->world 传承经验请求
	*/
	class WorldInheritExpReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_INHERIT_EXP_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->client 传承经验返回
	*/
	class WorldInheritExpRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_INHERIT_EXP_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & resCode & ownInheritBlessNum & ownInheritBlessExp;
		}

		// 错误码
		UInt32 resCode;
		// 拥有的传承祝福数量
		UInt32 ownInheritBlessNum;
		// 拥有的传承祝福经验
		UInt64 ownInheritBlessExp;
	};

	/**
	*@brief client->world 远征地图查询请求
	*/
	class WorldQueryExpeditionMapReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_EXPEDITION_MAP_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mapId;
		}

	public:
		// 地图id
		UInt8 mapId;
	};

	/**
	*@brief world->client 远征地图查询返回
	*/
	class WorldQueryExpeditionMapRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_EXPEDITION_MAP_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & resCode & mapId & expeditionStatus & durationOfExpedition 
				& endTimeOfExpedition & members;
		}

	public:
		// 错误码
		UInt32 resCode;
		// 地图id
		UInt8 mapId;
		// 远征状态(对应枚举ExpeditionStatus)
		UInt8 expeditionStatus;

		// 远征持续时间(小时)
		UInt32 durationOfExpedition;
		// 远征结束时间
		UInt32 endTimeOfExpedition;
		// 远征队员信息
		std::vector<ExpeditionMemberInfo> members;
	};

	/**
	*@brief client->world 可选远征角色查询请求
	*/
	class WorldQueryOptionalExpeditionRolesReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_OPTIONAL_EXPEDITION_ROLES_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->client 可选远征角色查询返回
	*/
	class WorldQueryOptionalExpeditionRolesRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_OPTIONAL_EXPEDITION_ROLES_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & resCode & roles;
		}

	public:
		// 错误码
		UInt32 resCode;
		// 可选远征角色列表
		std::vector<ExpeditionMemberInfo> roles;
	};

	/**
	*@brief client->world 派遣远征队请求
	*/
	class WorldDispatchExpeditionTeamReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DISPATCH_EXPEDITION_TEAM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mapId & members & housOfduration;
		}

	public:
		// 地图id
		UInt8 mapId;
		// 远征成员(角色id列表)
		std::vector<ObjID_t> members;
		// 远征时间(持续时间 小时)
		UInt32 housOfduration;
	};

	/**
	*@brief world->client 派遣远征队返回
	*/
	class WorldDispatchExpeditionTeamRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DISPATCH_EXPEDITION_TEAM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & resCode & mapId & expeditionStatus & durationOfExpedition
				& endTimeOfExpedition & members;
		}

		// 错误码
		UInt32 resCode;
		// 地图id
		UInt8 mapId;
		// 远征状态(对应枚举ExpeditionStatus)
		UInt8 expeditionStatus;

		// 远征持续时间(小时)
		UInt32 durationOfExpedition;
		// 远征结束时间
		UInt32 endTimeOfExpedition;
		// 远征队员信息
		std::vector<ExpeditionMemberInfo> members;
	};

	/**
	*@brief client->world 取消远征请求
	*/
	class WorldCancelExpeditionReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CANCEL_EXPEDITION_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mapId;
		}

	public:
		// 地图id
		UInt8 mapId;
	};

	/**
	*@brief world->client 取消远征返回
	*/
	class WorldCancelExpeditionRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CANCEL_EXPEDITION_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & resCode & mapId & expeditionStatus;
		}

		// 错误码
		UInt32 resCode;
		// 地图id
		UInt8 mapId;
		// 远征状态
		UInt8 expeditionStatus;
	};

	/**
	*@brief client->world 远征奖励领取请求
	*/
	class WorldGetExpeditionRewardsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GET_EXPEDITION_REWARDS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mapId;
		}

	public:
		// 地图id
		UInt8 mapId;
	};

	/**
	*@brief world->client 远征奖励领取返回
	*/
	class WorldGetExpeditionRewardsRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GET_EXPEDITION_REWARDS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & resCode & mapId & expeditionStatus;
		}

		// 错误码
		UInt32 resCode;
		// 地图id
		UInt8 mapId;
		// 远征状态
		UInt8 expeditionStatus;
	};

	/**
	*@brief client->world 查询全部远征地图请求
	*/
	class WorldQueryAllExpeditionMapsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_ALL_EXPEDITION_MAPS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mapIds;
		}

	public:
		// 地图id集
		std::vector<UInt8> mapIds;
	};

	/**
	*@brief world->client 查询全部远征地图返回
	*/
	class WorldQueryAllExpeditionMapsRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_ALL_EXPEDITION_MAPS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & resCode & mapBaseInfos;
		}

	public:
		// 错误码
		UInt32 resCode;
		std::vector<ExpeditionMapBaseInfo> mapBaseInfos;
	};

	/**
	*@brief client->world 请求查询拥有的职业
	*/
	class WorldQueryOwnOccupationsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_OWN_OCCUPATIONS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & baseOccus;
		}

	public:
		// 基础职业
		std::vector<UInt8> baseOccus;
	};

	/**
	*@brief world->client 查询拥有的职业返回
	*/
	class WorldQueryOwnOccupationsRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_OWN_OCCUPATIONS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & resCode & occus;
		}

	public:
		// 错误码
		UInt32 resCode;
		// 拥有的职业
		std::vector<UInt8> occus;
	};

	/**
	*@brief world->client 同步拥有的新职业
	*/
	class WorldQueryOwnNewOccupationsSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_OWN_NEW_OCCUPATIONS_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & ownNewOccus;
		}

	public:
		// 拥有的新职业
		std::vector<UInt8> ownNewOccus;
	};

	/**
	*@brief client->world 请求清除解锁的新职业
	*/
	class WorldRemoveUnlockedNewOccupationsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_REMOVE_UNLOCKED_NEW_OCCUPATIONS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & newOccus;
		}

	public:
		// 职业
		std::vector<UInt8> newOccus;;
	};

	/**
	*@brief world->server 同步冒险队信息
	*/
	class WorldAdventureTeamInfoSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ADVENTURE_TEAM_INFO_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & info & isCreate;
		}

	public:
		ObjID_t roleId;
		// 冒险队信息
		AdventureTeamInfo info;
		// 是否创建佣兵团
		UInt8 isCreate;
	};

	/**
	*@brief world->client 同步全部远征地图信息
	*/
	class WorldAllExpeditionMapsSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ALL_EXPEDITION_MAPS_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mapBaseInfos;
		}

	public:
		std::vector<ExpeditionMapBaseInfo> mapBaseInfos;
	};

	/**
	*@brief gate->world 通知重新计算角色总评分
	*/
	class GateNotifyReCalculateTotalRoleValueScore : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_NOTIFY_RECALCULATE_TOTAL_ROLE_VALUE_SCORE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & reason & accid;
		}

	public:
		std::string reason;
		UInt32 accid;
	};

};

#endif
