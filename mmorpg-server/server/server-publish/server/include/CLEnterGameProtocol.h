#ifndef _CL_ENTERGAME_PROTOCOL_H_
#define _CL_ENTERGAME_PROTOCOL_H_

#include <CLProtocolDefine.h>
#include <CLErrorCode.h>
#include <CLGameDefine.h>
#include <CLDungeonDefine.h>
#include <CLAntiAddictDefine.h>

namespace CLProtocol
{
	struct RoleInfo  //noted by aprilliu 复杂结构体要实现自己的序列化  用于 NetOutputStream& operator&(T& value) 以及 NetInputStream& operator&(T& value)的调用
	{
		RoleInfo() :roleId(0), sex(0), occu(0), level(0), offlineTime(0), deleteTime(0), isAppointmentOccu(0), isVeteranReturn(0),addPreferencesTime(0),delPreferencesTime(0){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & strRoleId & name & sex & occu & level & offlineTime & deleteTime & avatar & newBoot & preOccu & isAppointmentOccu & isVeteranReturn & playerLabelInfo & addPreferencesTime & delPreferencesTime;
		}

		//角色id
		ObjID_t	roleId;
		//字符串角色id
		std::string	strRoleId;
		//角色名
		std::string name;
		//性别
		UInt8 sex;
		//职业
		UInt8 occu;
		//等级
		UInt16 level;
		//离线时间
		UInt32 offlineTime;
		//删号剩余时间
		UInt32 deleteTime; 
		//外观
		PlayerAvatar avatar;
		//新手引导
		UInt32 newBoot;
		//预转职
		UInt8 preOccu;
		//是否预约角色
		UInt8 isAppointmentOccu;
		//是否老兵回归
		UInt8 isVeteranReturn;
		//玩家标签信息
		PlayerLabelInfo playerLabelInfo;
		//设置偏好时间
		UInt32 addPreferencesTime;
		//取消偏好时间
		UInt32 delPreferencesTime;
	};

	/**
	 *@brief gate->client 发送角色基本信息
	 */
	class GateSendRoleInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_SEND_ROLEINFO)
	public:
		GateSendRoleInfo() : appointmentRoleNum(0), baseRoleField(0), extensibleRoleField(0), unlockedExtensibleRoleField(0) {}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roles & appointmentOccus & appointmentRoleNum & baseRoleField & extensibleRoleField 
				& unlockedExtensibleRoleField;
		}

		//角色列表
		std::vector<RoleInfo>	roles;
		//可预约职业
		std::vector<UInt8>		appointmentOccus;
		//预约角色数量
		UInt32					appointmentRoleNum;
		//角色基础栏位
		UInt32					baseRoleField;
		//可扩展角色栏位
		UInt32					extensibleRoleField;
		//可扩展角色解锁栏位
		UInt32					unlockedExtensibleRoleField;
	};

	/**
	 *@brief client->gate 请求创建角色
	 */
	class GateCreateRoleReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_CREATEROLE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & name & sex & occu  & newer;
		}

		//名字
		std::string name;
		//性别
		UInt8 sex;
		//职业
		UInt8 occu;
		//玩家标识，1 新玩家，0 老玩家, 2 机器人
		UInt8 newer;
	};

	/**
	 *@brief gate->client 返回创建结果
	 */
	class GateCreateRoleRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_CREATEROLE_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		//结果
		UInt32 result;
	};

	/**
	 *@brief client->gate 请求删除角色
	 */
	class GateDeleteRoleReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_DELETEROLE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & deviceId;
		}

		//角色id
		ObjID_t	roleId;
		//设备Id
		std::string deviceId;
	};

	/**
	 *@brief client->gate 请求恢复角色
	 */
	class GateRecoverRoleReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_RECOVERROLE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId;
		}

		//角色id
		ObjID_t	roleId;
	};

	/**
	 *@brief client->gate 请求进入游戏
	 */
	class GateEnterGameReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_ENTERGAME_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & option & city & inviter;
		}

		//角色id
		ObjID_t roleId;
		//选项
		UInt8	option;
		//城市
		std::string	city;
		//邀请者
		UInt32	inviter;
	};

	/**
	 *@brief gate->client 进入游戏结果
	 */
	class GateEnterGameRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_ENTERGAME_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		//结果
		UInt32 result;
	};

	/**
	*@brief client->gate 请求重连
	*/
	class GateReconnectGameReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_RECONNECT_GAME_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accid & roleId & sequence & session;
		}
		
		UInt32 accid;
		//角色id
		ObjID_t roleId;
		//消息序号
		UInt32 sequence;
		//session
		std::vector<UInt8> session;
	};
	
	/**
	*@brief gate->client 重连返回
	*/
	class GateReconnectGameRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_RECONNECT_GAME_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & lastRecvSequence;
		}

		UInt32 result;
		UInt32 lastRecvSequence;
	};

	/**
	*@brief client->gate 完成新手引导
	*/
	class GateFinishNewbeeGuide : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_FINISH_NEWBEE_GUIDE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & id;
		}

		ObjID_t roleId;
		UInt32 id;
	};

	/**
	*@brief gate->client 恢复角色返回
	*/
	class GateRecoverRoleRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_RECOVERROLE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & result & roleUpdateLimit;
		}

		ObjID_t		roleId;
		UInt32		result;
		std::string roleUpdateLimit;
	};

	/**
	*@brief gate->client 删除角色返回
	*/
	class GateDeleteRoleRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_DELETEROLE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & result & roleUpdateLimit;
		}

		ObjID_t		roleId;
		UInt32		result;
		std::string roleUpdateLimit;
	};

	/**
	*@brief gate->client 删除角色返回
	*/
	class GateNotifyLoginWaitInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_NOTIFY_LOGIN_WAIT_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & waitPlayerNum;
		}

		UInt32 waitPlayerNum;
	};

	/**
	*@brief gate->client 通知玩家可以登录了
	*/
	class GateNotifyAllowLogin : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_NOTIFY_ALLOW_LOGIN)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief client->gate 玩家退出排队
	*/
	class GateLeaveLoginQueue : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_LEAVE_LOGIN_QUEUE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief gate->client 同步玩家系统开关
	*/
	class GateNotifySysSwitch : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_NOTIFY_SYSTEM_SWITCH)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & cfgs;
		}

		std::vector<SysSwitchCfg> cfgs;
	};
	/**
	 *@brief Client->gate 设置角色偏好
	 */
	class GateSetRolePreferencesReq :public Avalon::Protocol {
		AVALON_DEFINE_PROTOCOLID(GATE_SET_ROLE_PREFERENCES_REQ)
	public:
		GateSetRolePreferencesReq() :roleId(0), preferencesFlag(0) {}
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & preferencesFlag;
		}
		ObjID_t roleId;
		//枚举RolePreferences
		UInt8 preferencesFlag;
	};
	/**
	 * @brief Client->gate 设置角色偏好返回
	 */
	class GateSetRolePreferencesRet :public Avalon::Protocol {
		AVALON_DEFINE_PROTOCOLID(GATE_SET_ROLE_PREFERENCES_RET)
	public:
		GateSetRolePreferencesRet() :roleId(0),result(0),addPreferencesTime(0),delPreferencesTime(0) {}
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & result & addPreferencesTime & delPreferencesTime;
		}
		ObjID_t roleId;
		UInt32 result;
		UInt32 addPreferencesTime;
		UInt32 delPreferencesTime;

	};
	
	/** 
	 *@brief gate->client 同步服务器时间
	 */
	class GateSyncServerTime : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_SYNC_SERVER_TIME)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & time;
		}

		UInt32	time;
	};

	/**
	 *@brief gate->world->scene 通知场景玩家进入游戏
	 */
	class SceneEnterGameReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ENTERGAME_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & accid & name & srcZoneId & gmauthority & clientOption & openid & city
				& qqVipInfo & ip & source1 & source2 & inviter & pf & did & mapid & sceneid & pos 
				& gatenode & isPhoneBind & bornZoneId & antiAddictInfo & model & deviceVersion
				& roleReturnTime & roleReturnLevel & isAgainReturn;
		}

		//玩家guid
		ObjID_t			id;
		//账号id
		UInt32			accid;
		//玩家名
		std::string		name;
		//源区id
		UInt32			srcZoneId;
		//gm权限
		UInt8			gmauthority;
		//客户端选项
		UInt8			clientOption;
		//openid
		std::string		openid;
		//城市
		std::string		city;
		//qq特权信息
		QQVipInfo		qqVipInfo;
		//ip地址
		std::string		ip;
		//来源1
		std::string		source1;
		//来源2
		std::string		source2;
		//邀请者
		UInt32			inviter;
		//平台
		std::string		pf;
		//设备id
		std::string		did;

		//进入地图id
		UInt32			mapid;
		//进入场景id
		UInt32			sceneid;
		//进入位置
		CLPosition		pos;
		//所在网关
		UInt32			gatenode;
		//是否绑定手机
		UInt8			isPhoneBind;

		//原区ID
		UInt32			bornZoneId;
		//防沉迷信息
		AntiAddictInfo	antiAddictInfo;

		//设备型号
		std::string		model;
		//设备版本
		std::string		deviceVersion;

		//角色回归时间
		UInt32			roleReturnTime;
		//角色回归等级
		UInt32			roleReturnLevel;
		// 是否再次回归
		UInt8			isAgainReturn;
	};

	/**
	 *@brief scene->gate 玩家进入游戏返回
	 */
	class SceneEnterGameRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ENTERGAME_RET)
	public:
		SceneEnterGameRet():id(0),result(0){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & result;
		}

		//玩家guid
		ObjID_t			id;
		//结果
		UInt32			result;
	};

	/**
	 *@brief scene->world 场景通知world注册玩家
	 */
	class WorldRegisterPlayerReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_REGISTER_PLAYER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & accid & name & zoneid & srcZoneId & occu & sex & headFrame & viplvl & qqVipInfo & level & power & point & createtime & offlinetime & localOfflineTime & gateid & openid & city & ip & pf & did & model & deviceVersion & auctionRefreshTime & auctionAdditionBooth & monthCardExpireTime & bornZoneId & roleReturnTime;

		}

		//id
		ObjID_t		id;
		//账号id
		UInt32		accid;
		//名字
		std::string	name;
		//区id
		UInt16		zoneid;
		//原区id
		UInt32		srcZoneId;
		//职业
		UInt8		occu;
		//性别
		UInt8		sex;
		// 头像框
		UInt32		headFrame;
		//vip等级
		UInt8		viplvl;
		//qq特权信息
		QQVipInfo	qqVipInfo;
		//等级
		UInt16		level;
		//战斗力
		UInt32		power;
		//点劵
		UInt32		point;
		//创建时间
		UInt32		createtime;
		//上次离线时间
		UInt64		offlinetime;
		//本区上次离线时间
		UInt64		localOfflineTime;
		//网关id
		UInt32		gateid;
		//openid
		std::string openid;
		//城市
		std::string	city;
		//ip
		std::string	ip;
		//平台
		std::string	pf;

		//设备ID
		std::string did;
		//设备型号
		std::string model;
		//设备版本
		std::string deviceVersion;

		//拍卖行刷新时间
		UInt32	auctionRefreshTime;
		//额外购买的拍卖行栏位
		UInt32	auctionAdditionBooth;
		//月卡到期时间
		UInt32		monthCardExpireTime;
		//出生区ID
		UInt32		bornZoneId;

		//角色回归时间
		UInt32 roleReturnTime;
	};

	/**
	 *@brief world->scene world注册玩家结果
	 */
	class WorldRegisterPlayerRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_REGISTER_PLAYER_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & result;
		}

		//id
		ObjID_t		id;
		//结果
		UInt32		result;
	};

	/**
	*@brief world->gate world通知gate玩家可以登录了
	*/
	class WorldAllowPlayerLogin : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ALLOW_PLAYER_LOGIN)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accid;
		}

		//accid
		UInt32		accid;
	};

	/**
	*@brief world->gate world通知gate玩家可以登录了
	*/
	class WorldSyncPlayerLoginWaitInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_PLAYER_LOGIN_WAIT_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accid & waitPlayerNum;
		}

		//accid
		UInt32		accid;
		//需要等待的玩家数
		UInt32		waitPlayerNum;
	};

	/**
	*@brief gate->world 通知玩家退出排队
	*/
	class WorldPlayerLeaveLoginQueue : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PLAYER_LEAVE_LOGIN_QUEUE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accid;
		}

		//accid
		UInt32		accid;
	};
	

	/**
	 *@brief gate->world 验证唯一性登录
	 */
	class WorldVerifyUniqueReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_VERIFY_UNIQUE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accid & checkWaitQueue;
		}

		//账号id
		UInt32	accid;		
		//是否判断排队
		UInt8	checkWaitQueue;
	};

	/**
	 *@brief world->gate 验证唯一性登录返回
	 */
	class WorldVerifyUniqueRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_VERIFY_UNIQUE_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accid & needWait & waitPlayerNum & serverStartTime & infos;
		}

		//账号id
		UInt32	accid;
		//是否需要等待
		UInt8	needWait;
		//前面的玩家数
		UInt32	waitPlayerNum;
		//开服时间
		UInt32  serverStartTime;
		//登录推送信息
		std::vector<LoginPushInfo> infos;
	};

	/**
	*@brief scene->world 查询账号信息请求
	*/
	class SceneQueryAccountReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_QUERY_ACCOUNT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accid;
		}

		//账号id
		UInt32	accid;
	};
	
	/**
	*@brief world->scene 查询账号信息返回
	*/
	class WorldQueryAccountRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_QUERY_ACCOUNT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & accountGuid & accountId & point & creditPoint & vipLevel & vipExp
				& totalChargeNum & monthCardExpireTime & dungeonHardInfoes 
				& storageSize & moneyManageStatus & weaponLeaseTickets
				& adventureTeamLevel & adventureTeamGrade & accountAchievementScore & mallPoint & adventureCoin;
		}

		//结果
		UInt32		result;
		//账号表guid
		ObjID_t		accountGuid;
		//账号id
		UInt32		accountId;
		//点劵
		UInt32		point;
		//信用点劵
		UInt32		creditPoint;
		//vip等级
		UInt8		vipLevel;
		//vip经验
		UInt32		vipExp;
		//总充值金额
		UInt32		totalChargeNum;
		//月卡到期时间
		UInt32		monthCardExpireTime;
		// 地下城难度信息
		std::vector<SceneDungeonHardInfo> dungeonHardInfoes;
		//仓库格子数
		UInt32		storageSize;

		//理财管理状态
		UInt8		moneyManageStatus;
		// 武器租赁好运符
		UInt32		weaponLeaseTickets;
		// 冒险队等级
		UInt16		adventureTeamLevel;
		// 冒险队评级
		UInt8		adventureTeamGrade;
		// 账号成就点
		UInt32		accountAchievementScore;
		// 商城积分
		UInt32      mallPoint;
		//冒险币
		UInt32      adventureCoin;
	};

	/**
	 *@brief client->server 同步防沉迷收益系数
	 */
	class SceneSyncWallowFactor : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_WALLOW_FACTOR)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & factor;
		}

		//系数
		UInt32	factor;
	};

	/**
	*@brief client->server 记录更新版本号
	*/
	class SceneUpdateBulletinRecord : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_UPDATE_BULLETIN_RECORD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & updateVersion;
		}

	public:
		//更新版本号
		UInt32 updateVersion;
	};

    
    /**
    *@brief world server->scene server 请求重连
    */
    class SceneGameReconnectReq : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_GAME_RECONN_REQ)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & roleId & accid & gatenode;
        }

    public:
        //角色ID
        ObjID_t         roleId;

        //账号ID        
        UInt32          accid;

        //所在网关
        UInt32			gatenode;
    };

    /**
    *@brief scene server->world server 重连返回
    */
    class SceneGameReconnectRet : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_GAME_RECONN_RET)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
			stream & roleId & result;
        }

    public:
        ObjID_t     roleId;
        UInt32      result;
	};

	/**
	*@brief client->gate 请求角色切换
	*/
	class RoleSwitchReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_ROLE_SWITCH_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief gate->client 请求角色切换返回
	*/
	class RoleSwitchRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_ROLE_SWITCH_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & hasrole & waitPlayerNum & serverStartTime;
		}

		//结果
		UInt32	result;
		//有木有角色 1有 0木有
		UInt8	hasrole;
		//需要等待的玩家数
		UInt32	waitPlayerNum;
		//服务器开服时间
		UInt32	serverStartTime;
	};

	/**
	*@brief gate->client 发送登录推送信息
	*/
	class GateSendLoginPushInfo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_SEND_LOGIN_PUSH_INFO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & infos;
		}

		// 登录推送信息
		std::vector<LoginPushInfo> infos;
	};

	/**
	*@brief client->gate 请求转移账号信息
	*/
	class GateConvertAccountReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_CONVERT_ACCOUNT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief gate->client 返回转移账号信息
	*/
	class GateConvertAccountRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_CONVERT_ACCOUNT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & account & passwd & saveFile & screenShot;
		}

		std::string		account;
		std::string		passwd;
		// 是否保存文件
		UInt8			saveFile;
		// 是否截图
		UInt8			screenShot;
	};
	
	/**
	*@brief world->client 同步玩家登陆状态
	*/
	class SyncPlayerLoginStatus : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_PLAYER_LOGIN_STATUS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerLoginStatus;
		}

		// 玩家登录状态
		UInt8 playerLoginStatus;
	};

	/**
	*@brief gate->world 获取登录推送信息请求
	*/
	class WorldGetLoginPushInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GET_LOGIN_PUSH_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleid;
		}

		//角色id
		UInt64	roleid;
	};
	
	/**
	*@brief world->gate 获取登录推送信息返回
	*/
	class WorldGetLoginPushInfoRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GET_LOGIN_PUSH_INFO_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleid & infos;
		}

		//角色id
		UInt64	roleid;
		// 登录推送信息
		std::vector<LoginPushInfo> infos;
	};

	/**
	*@brief client->gate新手引导选择请求
	*/
	class GateNoviceGuideChooseReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GATE_NOVICE_GUIDE_CHOOSE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleid & chooseFlag;
		}

	public:
		// 角色id
		ObjID_t roleid;
		// 选择标志(NoviceGuideChooseFlag)
		UInt8 chooseFlag;
	};
}

#endif
