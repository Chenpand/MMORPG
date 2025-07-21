#ifndef _CL_SYNC_OBJECT_PROTOCOL_H_
#define _CL_SYNC_OBJECT_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLGameDefine.h"
#include "CLFigureStatueDefine.h"
#include "CLObjectDefine.h"

namespace CLProtocol
{
	struct ExpSyncInfo
	{
		ExpSyncInfo() :exp(0), incExp(0) {}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & reason & exp & incExp;
		}

		std::string reason;
		UInt64 exp;
		// 增加的经验(用来计算冒险队经验)
		UInt64 incExp;
	};

	/**
	 *@brief scene->client 同步自身数据
	 *		格式：[dataid+data][dataid+data][dataid+data]...[0]
	 */
	class SceneSyncSelf : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_SELF)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	 *@brief scene->client 同步场景obj
	 *		格式 [isnew(UInt8)] + [ObjID_t + type + data][ObjID_t + type + data][ObjID_t + type + data]...[0]
	 *		data格式  [dataid+data][dataid+data][dataid+data]...[0]
	 */
	class SceneSyncSceneObject : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_SCENEOBJECT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	 *@brief scene->client 同步自身数据到周围玩家
	 *		格式  ObjID_t + [dataid+data][dataid+data][dataid+data]...[0]
	 */
	class SceneSyncObjectProperty : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_OBJECTPROPERTY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	 *@brief scene->client 删除场景object
	 *		格式  ObjID_t + ObjID_t + ObjID_t + 0
	 */
	class SceneDeleteSceneObject : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_DELETE_SCENEOBJECT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	 *@brief world->scene 向场景同步社会关系数据
	 */
	class SceneSyncSocialData : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_SOCIAL_DATA)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & teamId;
		}

	public:
		//玩家id
		ObjID_t	playerId;
		//队伍id
		UInt32	teamId;
	};


	/**
	 *@brief scene->world 同步玩家基本信息
	 */
	class WorldSyncPlayerBaseInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_PLAYER_BASEINFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & oldLevel & level & vip & vipExp & power & point & creditPoint &  occu & awaken & activeDegree & totalChargeNum &
				monthCardExpireTime & seasonLv & auctionRefreshTime & auctionAdditionBooth & totalPlayerChargeNum & moneyManageStatus & 
				expInfo & headFrame & returnYearTitle;
		}

	public:
		//玩家id
		ObjID_t	playerId;
		//老的等级
		UInt16	oldLevel;
		//等级
		UInt16	level;
		//vip等级
		UInt8	vip;

		//vip经验
		UInt32	vipExp;

		//战力
		UInt32	power;
		//点劵
		UInt32  point;
		//信用点劵
		UInt32  creditPoint;
		//职业
		UInt8	occu;
		//觉醒
		UInt8	awaken;
		//活跃度
		UInt32	activeDegree;
		//总充值金额
		UInt32	totalChargeNum;
		//月卡到期时间
		UInt32	monthCardExpireTime;
		//赛季段位
		UInt32	seasonLv;
		//拍卖行刷新时间
		UInt32	auctionRefreshTime;
		//额外购买的拍卖行栏位
		UInt32	auctionAdditionBooth;
		//角色总充值金额
		UInt32  totalPlayerChargeNum;
		//理财计划状态
		UInt8   moneyManageStatus;
		//经验
		ExpSyncInfo expInfo;
		// 头像框
		UInt32		headFrame;
		// 回归称号
		UInt32      returnYearTitle;
		//套装解锁情况
		bool haveSuit;
	};

	/**
	 *@brief scene->world 通知杀死了玩家
	 */
	class WorldNotifyKillPlayer : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_NOTIFY_KILL_PLAYER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & killer & dieder;
		}

	public:
		//杀人者
		ObjID_t killer;
		//被杀者
		ObjID_t	dieder;
	};

	/**
	 *@brief world->scene 通知增加经验
	 */
	class WorldAddExpReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ADDEXP_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & exp;
		}

	public:
		//玩家
		ObjID_t	playerId;
		//经验
		UInt32	exp;
	};

	/**
	 *@brief client->world 观察玩家 （废弃）
	 */
	class WorldWatchPlayerReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_WATCH_PLAYER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & name;
		}

		//玩家id
		ObjID_t		playerId;
		//玩家名
		std::string	name;
	};

	/**
	 *@brief world->scene 转发请求到场景（废弃）
	 */
	class SceneWatchPlayerReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_WATCH_PLAYER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & watcher & target & tribe;
		}

		//观察者
		ObjID_t	watcher;
		//目标
		ObjID_t	target;
		//目标部落
		std::string	tribe;
	};

	/**
	 *@brief scene->client 同步被观察者
	 *		格式：playerId(ObjID_t) + tribeName(string) + [dataid+data][dataid+data][dataid+data]...[0]
	 */
	class SceneSyncWatchInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_WATCH_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	 *@brief world->scene 向场景同步惩罚
	 */
	class SceneSyncPunishment : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_PUNISHMENT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & punishflag & forbitTalkDueTime;
		}
		//玩家id
		ObjID_t	id;
		//惩罚标志位
		UInt32	punishflag;
		//禁言到期时间
		UInt32	forbitTalkDueTime;
	};

	/**
	 *@brief client->scene 请求某个场景对象的数据
	 */
	class SceneObjectDataReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_OBJECTDATA_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

		//对象id
		ObjID_t	id;
	};

	/**
	 *@brief client->server 请求玩家的战斗力数据
	 */
	class WorldPlayerPowerReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PLAYER_POWER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & name;
		}

		//玩家名
		std::string	name;
	};

	/**
	 *@brief world->scene world转发到scene的玩家的战斗力数据请求
	 */
	class ScenePlayerPowerReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_PLAYER_POWER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & watcher & target & tribe;
		}

		//观察者
		ObjID_t	watcher;
		//目标
		ObjID_t	target;
		//目标部落
		std::string	tribe;
	};

	/**
	 *@brief server->client 同步玩家的战斗力数据
	 *		格式：playerId(ObjID_t) + name(string) + 等级(UInt16) + 职业(UInt8) + 性别(UInt8) + tribename(string) + 总战斗力(UInt32) + [战斗力id(UInt8) + 战斗力(UInt32)] + [战斗力id(UInt8) + 战斗力(UInt32)] + ... UInt8(0)
	 */
	class SceneSyncPlayerPower : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_PLAYER_POWER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world<->scene scene和world之间事件同步  added by huchenhui
	*/
	class SceneSyncEvent : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_EVENT)
	public:
		SceneSyncEvent() : playerId(0), type(0), param1(0), param2(0), param3(0) {}
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & type & param1 & param2 & param3;
		}

		//玩家ID
		ObjID_t playerId;

		//事件类型
		UInt32	type;

		// 参数
		ObjID_t	param1;

		// 参数
		ObjID_t param2;

		// 参数
		ObjID_t param3;
	};

	/**
	*@brief scene->world 离线事件同步  added by zhengfeng
	*/
	class SceneSyncOfflineEvent : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_OFFLINE_EVENT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & type & param1 & param2;
		}

		//玩家ID
		ObjID_t playerId;
		//事件类型
		UInt32	type;
		// 参数
		ObjID_t	param1;
		// 参数
		ObjID_t param2;
	};

	/**
	*@brief client->scene 清除公会红点
	*/
	class SceneClearRedPoint : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CLEAR_REDPOINT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & flag;
		}

		UInt32		flag;
	};

	/**
	*@brief scene->world 同步玩家外观
	*/
	class SceneSyncPlayerAvatar : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_PLAYER_AVATAR)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & avatar;
		}

		ObjID_t			id;
		PlayerAvatar	avatar;
	};

	/**
	*@brief scene->world 同步玩家账号信息
	*/
	class SceneSyncAccountInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_ACCOUNT_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accid & type & value;
		}

		UInt32		accid;
		// 字段类型(对应枚举AccountInfoType)
		UInt8		type;
		UInt32		value;
	};

	/**
	*@brief client->scene 设置用户自定义字段
	*/
	class SceneSetCustomData : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SET_CUSTOM_DATA)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & data;
		}

		UInt32		data;
	};

	/**
	*@brief scene->client npc列表
	*/
	class SceneNpcList : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NPC_LIST)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & infoes;
		}

		std::vector<SceneNpcInfo>		infoes;
	};

	/**
	*@brief scene->client 新增npc
	*/
	class SceneNpcAdd : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NPC_ADD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & data;
		}

		void AddNpc(UInt32 sceneId, const SceneNpc& npc)
		{
			for (auto& info : data)
			{
				if (info.sceneId == sceneId)
				{
					info.npcs.push_back(npc);
					return;
				}
			}

			SceneNpcInfo info;
			info.sceneId = sceneId;
			info.npcs.push_back(npc);
			data.push_back(info);
		}

		std::vector<SceneNpcInfo>		data;
	};

	/**
	*@brief scene->client 删除npc
	*/
	class SceneNpcDel : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NPC_DEL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guids;
		}

		std::vector<ObjID_t>		guids;
	};

	/**
	*@brief scene->client 修改npc状态
	*/
	class SceneNpcUpdate : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NPC_UPDATE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & data;
		}

		SceneNpcInfo		data;
	};

	/**
	*@brief world-> scene/client 人物雕像同步
	*/
	class WorldFigureStatueSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_FIGURE_STATUE_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & figureStatue;
		}

		// 要展示的人物雕像
		std::vector<FigureStatueInfo> figureStatue;
	};


	/**
	*@brief scene->client item列表
	*/
	class SceneItemList : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ITEM_LIST)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID  & infoes;
		}

		UInt32 battleID;
		std::vector<SceneItemInfo>		infoes;
	};

	/**
	*@brief scene->client 新增item
	*/
	class SceneItemAdd : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ITEM_ADD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID  & data;
		}

		void AddItem(UInt32 sceneId, const SceneItem& item)
		{
			for (auto& info : data)
			{
				if (info.sceneId == sceneId)
				{
					info.items.push_back(item);
					return;
				}
			}

			SceneItemInfo info;
			info.sceneId = sceneId;
			info.items.push_back(item);
			data.push_back(info);
		}

		UInt32 battleID;
		std::vector<SceneItemInfo>		data;
	};

	/**
	*@brief scene->client 删除item
	*/
	class SceneItemDel : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ITEM_DEL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID  & guids;
		}

		UInt32 battleID;
		std::vector<ObjID_t>		guids;
	};

	/**
	*@brief scene->client 更新item
	*/
	class SceneItemUpdate : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ITEM_UPDATE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID & data;
		}

		UInt32 battleID;
		SceneItemInfo		data;
	};

	/**
	*@brief client->scene 同步item列表
	*/
	class SceneItemSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ITEM_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & battleID;
		}

	public:
		UInt32 battleID;
	};

	struct AccountCounter
	{
		AccountCounter() : type(0), num(0) {}
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & num;
		}

		// 计数类型
		UInt32 type;
		// 数量
		UInt64 num;
	};

	/**
	*@brief world->client 账号计数
	*/
	class WorldAccountCounterNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ACCOUNT_COUNTER_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accCounterVec;
		}

	public:
		std::vector<AccountCounter> accCounterVec;
	};

	/**
	*@brief world->scene 账号计数
	*/
	class WorldAccountCounterSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ACCOUNT_COUNTER_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & accCounter;
		}

	public:
		UInt64 roleId;
		AccountCounter accCounter;
	};


	/**
	*@brief scene->world 同步角色评分
	*/
	class SceneSyncRoleValueScore : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_ROLE_VALUE_SCORE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleid & roleValueScore & isCreateAdvent;
		}

	public:
		ObjID_t roleid;
		// 角色评分
		UInt32 roleValueScore;
		// 是否创建佣兵团
		UInt8 isCreateAdvent;
	};

	/**
	*@brief world->scene 账号数据同步
	*/
	class WorldAccountDataSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ACCOUNT_DATA_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & type & val;
		}

	public:

		UInt64 roleId;
		UInt32 type;
		std::string val;
	};

	/**
	*@brief world<->scene 同步游戏参数
	*/
	class SceneGameParamSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_GAME_PARAM_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & gameParamMap;
		}

	public:

		std::map<std::string, UInt32> gameParamMap;
	};

	/**
	*@brief clinet->scene 客户端埋点
	*/
	class SceneClientLog : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CLIENT_LOG)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & name & param1 & param2 & param3;
		}

	public:
		// 事件名
		std::string			name;
		// 参数1
		std::string			param1;
		// 参数2
		std::string			param2;
		// 参数3
		std::string			param3;
	};

	/**
	*@brief clinet->scene 选择角色，拉取信息
	*/
	class SceneSelectObject : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SELECT_OBJECT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guid;
		}

	public:
		UInt64 guid;
	};

	/**
	*@brief scene->client 通知客户端获得经验
	*/
	class SceneNotifyIncExp : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_INC_EXP)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & reason & value & incExp;
		}

	public:
		// 获取经验来源(对应枚举PlayerIncExpReason)
		UInt8			reason;
		// 获得的值(可能是数值，可能是百分比，根据来源决定)
		UInt32			value;
		// 获得的经验值
		UInt32			incExp;
	};
};
#endif
