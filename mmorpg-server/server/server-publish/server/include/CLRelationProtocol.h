#ifndef _CL_RELATION_PROTOCOL_H_
#define _CL_RELATION_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLRelationDefine.h"
#include "CLItemDefine.h"
#include "CLRetinueDefine.h"
#include "CLMatchDefine.h"

namespace CLProtocol
{
	/**
	 *@brief client->world server 查询一个玩家
	 */
	class WorldQueryPlayerReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_PLAYER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & type & zoneId & name;
		}
		WorldQueryPlayerReq() : roleId(0), type(0), zoneId(0) {}
	public:
		// 角色ID
		ObjID_t		roleId;
		// 查询类型
		UInt32		type;
		// 区ID
		UInt32		zoneId;
		// 名字
		std::string name;
	};

	/**
	*@brief world server->scene server 查询一个玩家
	*/
	class SceneQueryPlayerReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RELATION_QUERY_PLAYER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & srcRoleId & tarAccId & targetRoleId;
		}

	public:
		// 查询者ID
		ObjID_t		srcRoleId;
		// 目标AccID
		UInt32		tarAccId;
		// 目标ID
		ObjID_t		targetRoleId;
	};

	/**
	*@brief scene server->world server 返回玩家信息
	*/
	class SceneQueryPlayerRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RELATION_QUERY_PLAYER_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roldId & success & info;
		}

	public:
		ObjID_t			roldId;
		UInt8			success;
		PlayerWatchInfo info;
	};

	/**
	 *@brief world server->client 返回玩家信息
	 */
	class WorldQueryPlayerRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_PLAYER_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & zoneId & info;
		}
		WorldQueryPlayerRet() : type(0), zoneId(0) {}
	public:
		// 查询类型
		UInt32		type;
		// 区ID
		UInt32		zoneId;
		PlayerWatchInfo info;
	};

	/**
	*@brief client->world server 查询一个玩家详细信息
	*/
	class WorldQueryPlayerDetailsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_PLAYER_DETAILS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & type & zoneId & name;
		}
		WorldQueryPlayerDetailsReq() : roleId(0), type(0), zoneId(0) {}

	public:
		// 角色ID
		ObjID_t		roleId;
		// 查询类型
		UInt32		type;
		// 区ID
		UInt32		zoneId;
		// 名字
		std::string name;
	};


	/**
	*@brief world server->client 返回玩家详细信息
	*/
	class WorldQueryPlayerDetailsRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_PLAYER_DETAILS_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

	public:
		RacePlayerInfo info;
	};


	/**
	*@brief world server->scene server 查询一个玩家详细信息
	*/
	class SceneQueryPlayerDetailsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RELATION_QUERY_PLAYER_DETAILS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & srcRoleId & targetRoleId;
		}

	public:
		// 查询者ID
		ObjID_t		srcRoleId;
		// 目标ID
		ObjID_t		targetRoleId;
	};


	/**
	*@brief scene server->world server 返回玩家详细信息
	*/
	class SceneQueryPlayerDetailsRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RELATION_QUERY_PLAYER_DETAILS_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roldId & success & info;
		}

	public:
		ObjID_t			roldId;
		UInt8			success;
		RacePlayerInfo	info;
	};


	/**
	 *@brief client->server 添加到黑名单
	 */
	class WorldAddToBlackList : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ADDTO_BLACKLIST)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

	public:
		//玩家id
		ObjID_t	id;
	};

	/**
	 *@brief client->server 移除关系
	 */
	class WorldRemoveRelation : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_REMOVE_RELATION)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & id;
		}

	public:
		//关系类型
		UInt8	type;
		//对方id
		ObjID_t id;
	};

	/**
	 *@brief server->client 通知新关系
	 *	格式: type(UInt8) + id(ObjID_t) + isOnline(UInt8) + data
	 *  data: 属性id + 属性 + ... + UInt8(0)
	 */
	class WorldNotifyNewRelation : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_NEWRELATION)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}

	public:
	};

	/**
	 *@brief server->client 通知删除关系
	 */
	class WorldNotifyDelRelation : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_DELRELATION)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & id;
		}

	public:
		//关系类型
		UInt8	type;
		//id
		ObjID_t	id;
	};

	/**
	 *@brief server->client 同步关系数据 
	 * 格式 type(UInt8) + id(ObjID_t) + data
	 * data格式同WorldNotifyNewRelation
	 */
	class WorldSyncRelationData : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_RELATIONDATA)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	};

	/**
	 *@brief server->client 上线同步关系列表
	 *	datalist格式: ObjID_t + isOnline(UInt8) + data + .. + 0(ObjID_t)
	 *	data格式同WorldNotifyNewRelation
	 */
	class WorldSyncRelationList : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_RELATION_LIST)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}

	public:
		//关系类型
		UInt8	type;
		//datalist
	};

	/**
	 *@brief client->server 追杀玩家
	 */
	class WorldChaseKill : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CHASE_KILL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

	public:
		//对方id
		ObjID_t	id;
	};

	/**
	 *@brief server->client 同步上下线状态
	 */
	class WorldSyncOnOffline : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_ONOFFLINE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & isOnline;
		}

	public:
		//id
		ObjID_t	id;
		//1为上线 0为下线
		UInt8	isOnline;
	};

	/**
	 *@brief client->server 设置上线通知
	 */
	class WorldSetOnlineNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SET_ONLINE_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & id & notify;
		}

	public:
		//类型
		UInt8	type;
		//id
		ObjID_t	id;
		//0为不通知 1为通知
		UInt8	notify;
	};

	/**
	 *@brief scene->world 增加亲密度
	 */
	class WorldRelationAddIntimacy : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_RELATION_ADD_INTIMACY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & friendId & intimacy;
		}

	public:
		//玩家id
		ObjID_t id;
		//好友id
		ObjID_t friendId;
		//亲密度
		UInt16	intimacy;
	};

	/**
	 *@brief world->gate 向网关同步黑名单
	 */
	class GateSyncBlackList : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_SYNC_BLACKLIST)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & blacklist;
		}

	public:
		//玩家id
		ObjID_t	id;
		//黑名单
		std::vector<ObjID_t>	blacklist;
	};

	/**
	 *@brief world->gate 通知新增黑名单
	 */
	class GateNotifyNewBlackList : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_NOTIFY_NEWBLACKLIST)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & target;
		}

	public:
		//玩家id
		ObjID_t	id;
		//对方id
		ObjID_t	target;
	};

	/**
	 *@brief world->gate 通知删除黑名单
	 */
	class GateNotifyDelBlackList : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_NOTIFY_DELBLACKLIST)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & target;
		}

	public:
		//玩家id
		ObjID_t	id;
		//对方id
		ObjID_t	target;
	};

	/**
	 *@brief world->scene 同步关系列表到场景
	 */
	class SceneRelationSyncList : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RELATION_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & relationList;
		}

	public:
		//玩家id
		ObjID_t id;
		//关系列表
		std::vector<std::map<ObjID_t, UInt16>>	relationList;
	};

	/**
	 *@brief world->scene 通知新增关系
	 */
	class SceneRelationNotifyNew : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RELATION_NOTIFY_NEW)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & relationType & tarId;
		}

	public:
		//玩家id
		ObjID_t id;
		//关系类型
		UInt8	relationType;
		//关系对象id
		ObjID_t tarId;
	};

	/**
	 *@brief world->scene 通知删除关系
	 */
	class SceneRelationNotifyDel : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RELATION_NOTIFY_DEL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & relationType & tarId;
		}

	public:
		//玩家id
		ObjID_t id;
		//关系类型
		UInt8	relationType;
		//关系对象id
		ObjID_t tarId;
	};

	/**
	 *@brief world->scene 同步亲密度
	 */
	class SceneRelationSyncIntimacy : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RELATION_SYNC_INTIMACY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & friendId & intimacy & incIntimacy & isMaster;
		}

	public:
		//玩家id
		ObjID_t	id;
		//好友id
		ObjID_t friendId;
		//亲密度
		UInt16	intimacy;
		//增量
		UInt16  incIntimacy;
		//是否师傅
		UInt8   isMaster;
	};

	/**
	 *@brief world->scene 追杀时查询目标玩家所在场景
	 */
	class SceneRelationFindPlayerReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RELATION_FIND_PLAYER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & killer & target;
		}

	public:
		//杀人者
		ObjID_t	killer;
		//目标玩家
		ObjID_t	target;
	};

	/**
	 *@brief scene->world 查询目标玩家返回
	 */
	class SceneRelationFindPlayerRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RELATION_FIND_PLAYER_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & killer & target & sceneId & pos;
		}

	public:
		//杀人者
		ObjID_t killer;
		//目标玩家
		ObjID_t	target;
		//所在场景
		UInt32	sceneId;
		//位置
		CLPosition	pos;
	};

	/**
	 *@brief client->server 查找玩家列表
	 */
	class WorldRelationFindPlayersReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_RELATION_FIND_PLAYERS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type;
		}

		UInt8 type;		//RelationFindType
	};

	/**
	 *@brief server->client 返回玩家列表
	 */
	class WorldRelationFindPlayersRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_RELATION_FIND_PLAYERS_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & players;
		}

		UInt8 type;			//RelationFindType
		//列表
		std::vector<QuickFriendInfo>	players;
	};

	/**
	*@brief client->server 好友赠送
	*/
	class WorldRelationPresentGiveReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_RELATION_PRESENT_GIVE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & friendID;
		}

		ObjID_t  friendID;
	};

	/**
	*@brief world->client 好友赠送返回
	*/
	class WorldRelationPresentGiveRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_RELATION_PRESENT_GIVE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & friendID;
		}
		UInt32   code;
		ObjID_t  friendID;
	};

	/**
	*@brief client->server 更新关系
	*/
	class WorldUpdateRelation : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_UPDATE_RELATION)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			
		}

	};

	struct OnlineState
	{
	public:
		OnlineState() :uid(0){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & uid & bOnline;
		}

		ObjID_t uid;
		UInt8	bOnline;
	};
	/**
	*@brief client->server 请求玩家关系
	*/
	class WorldUpdatePlayerOnlineReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_UPDATE_PLAYER_ONLINE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & uids;
		}

		std::vector<ObjID_t> uids;
	};

	/**
	*@brief server->client 请求玩家关系返回
	*/
	class WorldUpdatePlayerOnlineRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_UPDATE_PLAYER_ONLINE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & onlineStates;
		}

		std::vector<OnlineState> onlineStates;
	};
	
	/**
	*@brief client->server 设置师傅公告
	*/
	class WorldSetMasterNoteReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SET_MASTER_NOTE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & note;
		}

		std::string note;
	};
	
	/**
	*@brief server->client 设置师傅公告返回
	*/
	class WorldSetMasterNoteRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SET_MASTER_NOTE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & newNote;
		}

		UInt32 code;
		std::string newNote;
	};

	/**
	*@brief client->server 设置师傅是否收徒
	*/
	class WorldSetMasterIsRecvDiscipleReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SET_RECV_DISCIPLE_STATE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isRecv;
		}

		UInt8 isRecv;
	};

	/**
	*@brief server->client 设置师傅是否收徒返回
	*/
	class WorldSetMasterIsRecvDiscipleRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SET_RECV_DISCIPLE_STATE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & isRecv;
		}

		UInt32 code;
		UInt8 isRecv;
	};

	/**
	*@brief client->server 请求师傅设置信息
	*/
	class WorldQueryMasterSettingReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_MASTER_SETTING_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			
		}

	};

	/**
	*@brief server->client 请求师傅设置信息返回
	*/
	class WorldQueryMasterSettingRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_MASTER_SETTING_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & masterNote & isRecv;
		}
		std::string masterNote;
		UInt8	isRecv;
	};

	/**
	 *@brief client->server 一键征友
	 */
	class WorldRelationQuickMakeFriends : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_RELATION_QUICK_MAKE_FRIENDS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & players;
		}

		//列表
		std::vector<ObjID_t>	players;
	};

	/**
	 *@brief client->world 设置为密友                                                                     
	 */
	class WorldRelationSetClostFriend : public Avalon::Protocol
	{	
		AVALON_DEFINE_PROTOCOLID(WORLD_RELATION_SET_CLOSEFRIEND)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & flag;
		}

		//好友id
		ObjID_t playerId;
		//1表示设为密友 0表示取消设置
		UInt8	flag;
	};

	/**
	*@brief client->world 汇报自己作弊了
	*/
	class WorldRelationReportCheat : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_RELATION_REPORT_CHEAT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	 *@brief scene->client 同步祝福次数                                                                     
	 */
	class SceneRelationSyncBlessTimes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RELATION_SYNC_BLESSTIMES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & times;
		}

		UInt32 times;
	};

	/**
	 *@brief client->scene 请求好友互赠信息                                                                     
	 */
	class SceneRelationPresentInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RELATION_PRESENT_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	};

	/**
	 *@brief scene->client 返回好友互赠信息                                                                     
	 */
	class SceneRelationPresentInfoRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RELATION_PRESENT_INFO_RET)
	public:
		SceneRelationPresentInfoRet():continuousDays(0), giveState1(0), drawState1(0), giveState2(0), drawState2(0){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & continuousDays & giveState1 & drawState1 & giveState2 & drawState2;
		}

	public:
		//创角后连续登陆天数 为0表示非连续登录
		UInt32	continuousDays;
		//今日是否已赠送经验 0表示未领取 1表示已领取
		UInt8	giveState1;
		//今日是否已领取经验 0表示未领取 1表示已领取
		UInt8	drawState1;
		//15天是否已赠送
		UInt8	giveState2;
		//15填是否已领取
		UInt8	drawState2;
	};

	/**
	 *@brief client->scene 领取                                                                     
	 */
	class SceneRelationPresentDraw : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RELATION_PRESENT_DRAW)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type;
		}

		//类型 1为经验 2为15日
		UInt8	type;
	};

	/**
	 *@brief client->scene 赠送                                                                     
	 */
	class SceneRelationPresentGive : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RELATION_PRESENT_GIVE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & friendId & type;
		}

		//好友
		ObjID_t	friendId;
		UInt8 type;
	};

	/**
	 *@brief scene->world->scene 给好友请求                                                                     
	 */
	class SceneRelationPresentToFriendReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RELATION_PRESENT_TOFRIEND_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & playerName & friendId & type & itemId & gamestartDay;
		}

		//玩家id
		ObjID_t	playerId;
		//玩家名
		std::string	playerName;
		//好友id
		ObjID_t	friendId;
		//类型
		UInt8	type;
		//赠送的道具
		UInt32	itemId;
		//对应天
		UInt32  gamestartDay;
	};

	/**
	 *@brief scene->world->scene 给好友返回                                                                     
	 */
	class SceneRelationPresentToFriendRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RELATION_PRESENT_TOFRIEND_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & type & gamestartDay & result;
		}

		ObjID_t	playerId;
		UInt8	type;
		UInt32	gamestartDay;
		//赠送结果 0成功，1好友不在线，2好友次数满
		UInt8	result;
	};

	/**
	*@brief client->scene 师傅赠送装备
	*/
	class SceneMasterGiveEquip : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_MASTER_GIVE_EQUIP)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemuids & disciple;
		}

		std::vector<UInt64>	itemuids;
		UInt64	disciple;
	};


	/**
	*@brief client->scene 请求亲密付(代付)
	*/
	class SceneAddonPayReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ADDONPAYREQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & goodId & tarId & words;
		}

		UInt32	goodId;
		UInt64	tarId;
		std::string words;
	};


	/**
	*@brief scene->world 请求亲密付(代付)
	*/
	class WorldAddonPayReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ADDONPAYREQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & goodId & tarId & tarName & tarOccu & tarLevel & words;
		}

		UInt32	goodId;
		UInt64	tarId;
		std::string tarName;
		UInt8	tarOccu;
		UInt32	tarLevel;
		std::string words;
	};
		
	/**
	*@brief world->client 同步新增一条亲密付信息(代付)
	*/
	class WorldSyncAddonPayData : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_ADDONPAY_DATA)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & data;
		}

		AddonPayData data;
	};

	/**
	*@brief world->client 同步所有亲密付信息(代付)
	*/
	class WorldSyncAllAddonPayDatas : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_ALL_ADDONPAY_DATA)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & datas;
		}

		std::vector<AddonPayData> datas;
	};
	
	/**
	*@brief world->client 回复亲密付(代付)
	*/
	class WorldAddonPayReply : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ADDONPAYREPLY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & orderId & agree;
		}

		UInt64	orderId;
		UInt8	agree;
	};
		
	/**
	*@brief scene->world 检查师傅赠送徒弟次数
	*/
	class WorldCheckMasterGive : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CHECK_MASTER_GIVE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & masterId & disciple & items;
		}

		ObjID_t	masterId;
		ObjID_t disciple;
		std::vector<ObjID_t>	items;
	};

	class WorldCheckMasterGiveRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_CHECK_MASTER_GIVE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & masterId & disciple & items;
		}

		ObjID_t	masterId;
		ObjID_t disciple;
		std::vector<ObjID_t> items;
	};
	
	/*
	*@brief client->world 设置徒弟拜师问卷调查
	*/
	class WorldSetDiscipleQuestionnaireReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_RELATION_DISCIPLE_QUESTIONNAIRE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & activeTimeType & masterType & regionId & declaration;
		}

		UInt8			activeTimeType; //活跃时间类型
		UInt8			masterType;		//希望类型师傅
		UInt8			regionId;		//所在地区id
		std::string		declaration;	//拜师宣言
	};

	/*
	*@brief world->client 设置徒弟拜师问卷调查返回
	*/
	class WorldSetDiscipleQuestionnaireRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_RELATION_DISCIPLE_QUESTIONNAIRE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}
		UInt32 code;
	};

	/*
	*@brief client->world 设置师傅收徒问卷调查
	*/
	class WorldSetMasterQuestionnaireReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_RELATION_MASTER_QUESTIONNAIRE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & activeTimeType & masterType & regionId & declaration;
		}

		UInt8			activeTimeType; //活跃时间类型
		UInt8			masterType;		//我是什么类型师傅
		UInt8			regionId;		//所在地区id
		std::string		declaration;	//收徒宣言
	};

	/*
	*@brief world->client 设置师傅收徒问卷调查返回
	*/
	class WorldSetMasterQuestionnaireRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_RELATION_MASTER_QUESTIONNAIRE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}
		UInt32 code;
	};

	/**
	*@brief world->client 通知新同门关系
	*	格式: id(ObjID_t) + isOnline(UInt8) + data
	*  data: 属性id + 属性 + ... + UInt8(0)
	*/
	class WorldNotifyNewMasterSectRelation : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_NEW_MASTERSECT_RELATION)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}

	public:
	};

	/**
	*@brief world->client 同步同门关系数据
	* 格式  id(ObjID_t) + data
	* data格式同WorldNotifyNewMasterSectRelation
	*/
	class WorldSyncMasterSectRelationData : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_MASTERSECT_RELATIONDATA)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	};

	/**
	*@brief world->client 上线同步同门关系列表
	*	datalist格式: ObjID_t + isOnline(UInt8) + data + .. + 0(ObjID_t)
	*	data格式同WorldNotifyNewMasterSectRelation
	*/
	class WorldSyncMasterSectRelationList : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_MASTERSECT_RELATION_LIST)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief client->world 更新同门关系
	*/
	class WorldUpdateMasterSectRelationReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_UPDATE_MASTERSECT_RELATION_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}

	};

	/**
	*@brief world->client 删除同门关系同步
	*/
	class WorldNotifyDelMasterSectRelation : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_DEL_MASTERSECT_RELATION)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & id;
		}

		//关系类型
		UInt8	type;
		//id
		ObjID_t	id;

	};

	/*
	*@brief world->client 同步问卷调查
	*/
	class WorldSyncRelationQuestionnaire : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_RELATION_QUESTIONNAIRE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & activeTimeType & masterType & regionId & declaration;
		}

		UInt8			activeTimeType; //活跃时间类型
		UInt8			masterType;		//我是什么类型师傅
		UInt8			regionId;		//所在地区id
		std::string		declaration;	//收徒宣言
	};

	/*
	*@brief client->world 请求徒弟出师
	*/
	class WorldDiscipleFinishSchoolReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DISCIPLE_FINISH_SCHOOL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & discipleId;
		}

		ObjID_t		discipleId;		//徒弟id
	};

	/*
	*@brief world->client 请求徒弟出师返回
	*/
	class WorldDiscipleFinishSchoolRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DISCIPLE_FINISH_SCHOOL_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}

		UInt32		code;		
	};

	/*
	*@brief world->client 同步师徒拜师收徒惩罚时间戳
	*/
	class WorldSyncMasterDisciplePunishTime : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_MASTERDISCIPLE_PUNISH_TIME)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & apprenticMasterPunishTime & recruitDisciplePunishTime;
		}

		UInt64		apprenticMasterPunishTime;
		UInt64      recruitDisciplePunishTime;
	};

	/*
	*@brief world->client 出师成功奖励展示
	*/
	class WorldNotifyFinshSchoolReward : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_FINSCHOOL_REWARD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & giftId & masterId & discipleId & masterName & discipleName & discipleGrowth;
		}
		UInt32			giftId;			//出师奖励礼包id
		UInt64			masterId;		//师傅id
		UInt64			discipleId;		//徒弟id
		std::string		masterName;		//师傅名字
		std::string		discipleName;	//徒弟名字
		UInt32			discipleGrowth;	//徒弟成长值
	};

	/**
	*@brief client->world 自动出师
	*/
	class WorldAutomaticFinishSchoolReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AUTOMATIC_FINSCHOOL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & targetId;
		}

		UInt64		targetId;	//目标角色id
	};

	/**
	*@brief world->client 自动出师返回
	*/
	class WorldAutomaticFinishSchoolRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_AUTOMATIC_FINSCHOOL_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}
		UInt32		code;
	};

	/**
	*@brief client->world  设置玩家备注请求
	*/
	class WorldSetPlayerRemarkReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SET_PLAYER_REMARK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & remark;
		}

		ObjID_t		 roleId;
		std::string  remark;
	};

	/**
	*@brief world->client  设置玩家备注返回
	*/
	class WorldSetPlayerRemarkRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SET_PLAYER_REMARK_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}
		UInt32		code;
	};

	/**
	*@brief client->world 师徒世界频道公告
	*/
	class WorldRelationAnnounceReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_RELATION_MASTERDISCIPLE_ANNOUNCE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type;
		}
		UInt32		type;
	};

	/*
	*@brief  world->client 通知出师事件
	*/
	class WorldNotifyFinSchEvent : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_FINSCH_EVENT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}

	};

	class WorldQueryHireInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_HIRE_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	};

	class WorldQueryHireInfoRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_HIRE_INFO_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & identity & code & isBind & isOtherBindMe;
		}

		UInt8 identity = 0;
		std::string code;
		UInt8 isBind = 0;
		UInt8 isOtherBindMe = 0;
	};

	class WorldUseHireCodeReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_USE_HIRE_CODE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}

		std::string code;
	};

	class WorldUseHireCodeRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_USE_HIRE_CODE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & errorCode;
		}

		UInt32 errorCode = 0;
	};

	class WorldQueryTaskStatusReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_HIRE_TASK_STATUS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	};

	class WorldQueryTaskStatusRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_HIRE_TASK_STATUS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & hireTaskInfoList;
		}

		std::vector<HireInfoData> hireTaskInfoList;
	};

	class WorldQueryHireTaskAccidListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_HIRE_TASK_ACCID_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & taskId;
		}

		UInt32 taskId;
	};

	class WorldQueryHireTaskAccidListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_HIRE_TASK_ACCID_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nameList;
		}

		std::vector<std::string> nameList;
	};

	class WorldQueryHireListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_HIRE_LIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
		
	};

	class WorldQueryHireListRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_HIRE_LIST_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & hireList;
		}

		std::vector<HirePlayerData> hireList;
	};

	class WorldSubmitHireTaskReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SUBMIT_HIRE_TASK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & taskId;
		}

		UInt32 taskId;
	};

	class WorldSubmitHireTaskRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SUBMIT_HIRE_TASK_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & taskId & errorCode;
		}

		UInt32 taskId;
		UInt32 errorCode = 0;
	};

	class WorldOnSceneTrigeTask : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ON_SCENE_TRIGE_TASK)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accid & type & value & teammateIds;
		}

		UInt32 accid;
		UInt32 type;
		UInt32 value;
		std::vector<UInt32> teammateIds;
	};

	
	class WorldQueryHireCoinReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_HIRE_COIN_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}

	};

	class WorldQueryHirePushReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_HIRE_PUSH_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type;
		}
		UInt8 type; //0 是查询 1是设置推送
	};

	class SceneQueryHireRedPointReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_QUERY_HIRE_RED_POINT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}

	};


	class WorldQueryHireAlreadyBindReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_HIRE_ALREADY_BIND_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accid & platform & zoneId;
		}
		//平台
		std::string platform;
		UInt32 accid;
		//zoneid
		UInt32 zoneId;
	};

	class WorldQueryHireAlreadyBindRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_QUERY_HIRE_ALREADY_BIND_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & errorCode & accid & zoneId;
		}
		UInt32 errorCode;
		UInt32 accid;
		//zoneid
		UInt32 zoneId;
	};

	

	/**
	*@brief world->globalactivity 检查帐号是否已绑定 如果未绑定则绑定
	*/
	class GASCheckHireBindReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_CHECK_HIRE_BIND_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & platform & zoneId & accid & code;
		}
		//平台
		std::string platform;
		//zoneid
		UInt32 zoneId;
		//帐号
		UInt32 accid;
		//招募码
		std::string code;
	};

	/**
	*@brief globalactivity->world 检查帐号是否已绑定返回
	*/
	class GASCheckHireBindRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_CHECK_HIRE_BIND_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & zoneId & accid & errorCode & code;
		}
		//zoneid
		UInt32 zoneId;
		//帐号
		UInt32 accid;
		//结果
		UInt32 errorCode;
		//招募码
		std::string code;
	};
	
	/**
	*@brief 
	*/
	class SceneSyncMasterDailyTaskComp : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_MASTER_DAILY_TASK_COMP)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId;
		}

		ObjID_t		 roleId;
	};
}

#endif
