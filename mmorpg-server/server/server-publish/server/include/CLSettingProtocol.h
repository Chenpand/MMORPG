#ifndef _CL_QUICKBAR_PROTOCOL_H_
#define _CL_QUICKBAR_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLSettingDefine.h"

namespace CLProtocol
{

	/**
	 *@brief client->server 请求设置交换移除技能栏
	 */
	class SceneExchangeSkillBarReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EXCHANGE_SKILLBAR_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & configType & skillBars;
		}

		UInt8 configType;

		SkillBarInfos skillBars;
	};

	/**
	*@brief server->client 响应设置交换移除技能栏
	*/
	class SceneExchangeSkillBarRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EXCHANGE_SKILLBAR_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		UInt32 result;
	};

	/**
	*@brief client->server 请求设置交换移除物品栏
	*/
	class SceneExchangeItemBarReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EXCHANGE_ITEMBAR_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemBar;
		}

		std::vector<ItemBarGrid> itemBar;
	};

	/**
	*@brief server->client 响应设置交换移除物品栏
	*/
	class SceneExchangeItemBarRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EXCHANGE_ITEMBAR_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		UInt32	result;
	};

	/**
	 *@brief client->server 保存选项
	 */
	class SceneSaveOptionsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SAVEOPTIONS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & options;
		}

		//选项(SaveOptionMask)
		UInt32	options;
	};

	/**
	 *@brief client->server 改名
	 */
	class SceneChangeNameReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHANGENAME_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemUid & newName;
		}

	public:
		UInt64	itemUid;					//改名卡uid
		std::string	newName;				//新名字
	};

	/**
	*@brief scene->client 玩家改名返回
	*/
	class SceneChangeNameRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHANGENAME_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}

	public:
		UInt32 code;

	};

	/**
	*@brief client->scene 玩家检查改名请求
	*/
	class SceneCheckChangeNameReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHECK_CHANGENAME_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & newName;
		}

	public:
		std::string	newName;				//新名字

	};

	/**
	*@brief scene->client 玩家检查改名返回
	*/
	class SceneCheckChangeNameRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHECK_CHANGENAME_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}

	public:
		UInt32 code;
	};


	


	/**
	 *@brief scene->world 通知world改名
	 */
	class SceneNotifyChangeNameToWorld : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_CHANGENAME_TOWORLD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & newName;
		}

		//玩家
		ObjID_t	playerId;
		//玩家名
		std::string	newName;
	};

	/**
	 *@brief client->server 请求改性别                                                                     
	 */
	class SceneChangeSexReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHANGESEX_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sex;
		}

	public:
		//性别 
		UInt8	sex;
	};

	/**
	 *@brief scene->world 通知world改性别                                                                     
	 */
	class SceneNotifyChangeSexToWorld : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_CHANGESEX_TOWORLD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & sex;
		}

	public:
		//玩家id
		ObjID_t	playerId;
		//性别
		UInt8	sex;
	};

	/**
	 *@brief client->server  通知服务器图标设置到qq面板了
	 */
	class SceneNotifyAppOnPanel : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_APPONPANEL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	};

	/**
	*@brief client->server  通知转职 by huchenhui
	*/
	class SceneChangeOccu : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHANGE_OCCU)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & occu;
		}
		
		UInt8 occu;
	};

	/**
	*@brief server->client  通知功能解锁 by huchenhui
	*/
	class SceneSyncFuncUnlock : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_FUNC_UNLOCK)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & funcId;
		}

		UInt8 funcId;
	};

	/**
	*@brief scene->client  同步系统开关
	*/
	class SceneSyncServiceSwitch : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_SERVICE_SWITCH)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & closedServices;
		}

		std::vector<UInt16>		closedServices;
	};
	
	/**
	*@brief scene->client  同步系统开关
	*/
	class SceneUpdateServiceSwitch : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_UPDATE_SERVICE_SWITCH)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & isOpen;
		}

		UInt16		type;
		UInt8		isOpen;
	};

	/**
	*@brief client->server 设置武器栏请求
	*/
	class SceneSetWeaponBarReq :public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SET_WEAPON_BAR_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & index & weaponId;
		}

		UInt8 index;
		UInt64 weaponId;

	};

	/**
	*@brief server->client 设置武器栏返回
	*/
	class SceneSetWeaponBarRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SET_WEAPON_BAR_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		UInt32 result;
	};
	

	/**
	*@brief client->server 设置pvp技能配置请求
	*/
	class SceneSetPvpSkillConfigReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SET_PVP_SKILL_CONFIG_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isUsePvpConfig;
		}

		UInt8 isUsePvpConfig;
		
	};

	/**
	*@brief server->client 设置pvp技能配置响应
	*/
	class SceneSetPvpSkillConfigRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SET_PVP_SKILL_CONFIG_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		UInt32 result;

	};


	/**
	*@brief client->server 设置公平竞技技能配置请求 0表示查询设置情况 1表示第一次设置
	*/
	class SceneSetEqualPvpSkillConfigReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SET_EQUAL_PVP_SKILL_CONFIG_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isSetedEqualPvPConfig;
		}

		UInt8 isSetedEqualPvPConfig;

	};

	/**
	*@brief server->client 设置公平竞技技能配置响应 0表示未设置 1表示已设置
	*/
	class SceneSetEqualPvpSkillConfigRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SET_EQUAL_PVP_SKILL_CONFIG_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		UInt32 result;

	};


	/**
	*@brief client->server 设置游戏设置请求
	*/
	class SceneGameSetReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SET_GAME_SET_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & setType & setValue;
		}

		// 设置类型
		UInt32 setType;
		// 设置值
		std::string setValue;
	};

	/**
	*@brief server->client 设置游戏设置返回
	*/
	class SceneGameSetRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SET_GAME_SET_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode;
		}

		UInt32 retCode;
	};

	/**
	*@brief client->scene 通知服务器客户端进行了举报
	*/
	class SceneReportNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_REPORT_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief scene->world 游戏设置同步到world
	*/
	class SceneGameSetSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SET_GAME_SET_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & gameSet;
		}

		UInt64 playerId;
		std::string gameSet;
	};

	/**
	*@brief world->client  通知功能解锁
	*/
	class WorldSyncFuncUnlock : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_FUNC_UNLOCK)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & funcId;
		}

		UInt8 funcId;
	};

	struct ShortcutKeyInfo
	{
		ShortcutKeyInfo() :setType(0) {}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & setType & setValue;
		}

		// 设置类型
		UInt32 setType;
		// 设置值（客户端自定义的长字符串格式，最大1000个字节）
		std::string setValue;
	};

	/**
	*@brief client->server 游戏快捷键设置请求
	*/
	class SceneShortcutKeySetReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SET_SHORTCUT_KEY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & info;
		}

		ShortcutKeyInfo info;
	};

	/**
	*@brief server->client 游戏快捷键设置返回
	*/
	class SceneShortcutKeySetRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SET_SHORTCUT_KEY_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & info;
		}

		UInt32 retCode;
		ShortcutKeyInfo info;
	};

	/**
	*@brief scene->client 游戏快捷键设置同步
	*/
	class SceneShortcutKeySetSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SET_SHORTCUT_KEY_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & infos;
		}

		std::vector<ShortcutKeyInfo> infos;
	};

}

#endif
