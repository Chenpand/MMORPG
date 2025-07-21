#ifndef _CL_QUICKBAR_PROTOCOL_H_
#define _CL_QUICKBAR_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLSettingDefine.h"

namespace CLProtocol
{

	/**
	 *@brief client->server �������ý����Ƴ�������
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
	*@brief server->client ��Ӧ���ý����Ƴ�������
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
	*@brief client->server �������ý����Ƴ���Ʒ��
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
	*@brief server->client ��Ӧ���ý����Ƴ���Ʒ��
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
	 *@brief client->server ����ѡ��
	 */
	class SceneSaveOptionsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SAVEOPTIONS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & options;
		}

		//ѡ��(SaveOptionMask)
		UInt32	options;
	};

	/**
	 *@brief client->server ����
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
		UInt64	itemUid;					//������uid
		std::string	newName;				//������
	};

	/**
	*@brief scene->client ��Ҹ�������
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
	*@brief client->scene ��Ҽ���������
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
		std::string	newName;				//������

	};

	/**
	*@brief scene->client ��Ҽ���������
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
	 *@brief scene->world ֪ͨworld����
	 */
	class SceneNotifyChangeNameToWorld : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_CHANGENAME_TOWORLD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & newName;
		}

		//���
		ObjID_t	playerId;
		//�����
		std::string	newName;
	};

	/**
	 *@brief client->server ������Ա�                                                                     
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
		//�Ա� 
		UInt8	sex;
	};

	/**
	 *@brief scene->world ֪ͨworld���Ա�                                                                     
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
		//���id
		ObjID_t	playerId;
		//�Ա�
		UInt8	sex;
	};

	/**
	 *@brief client->server  ֪ͨ������ͼ�����õ�qq�����
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
	*@brief client->server  ֪ͨתְ by huchenhui
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
	*@brief server->client  ֪ͨ���ܽ��� by huchenhui
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
	*@brief scene->client  ͬ��ϵͳ����
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
	*@brief scene->client  ͬ��ϵͳ����
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
	*@brief client->server ��������������
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
	*@brief server->client ��������������
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
	*@brief client->server ����pvp������������
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
	*@brief server->client ����pvp����������Ӧ
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
	*@brief client->server ���ù�ƽ���������������� 0��ʾ��ѯ������� 1��ʾ��һ������
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
	*@brief server->client ���ù�ƽ��������������Ӧ 0��ʾδ���� 1��ʾ������
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
	*@brief client->server ������Ϸ��������
	*/
	class SceneGameSetReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SET_GAME_SET_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & setType & setValue;
		}

		// ��������
		UInt32 setType;
		// ����ֵ
		std::string setValue;
	};

	/**
	*@brief server->client ������Ϸ���÷���
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
	*@brief client->scene ֪ͨ�������ͻ��˽����˾ٱ�
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
	*@brief scene->world ��Ϸ����ͬ����world
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
	*@brief world->client  ֪ͨ���ܽ���
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

		// ��������
		UInt32 setType;
		// ����ֵ���ͻ����Զ���ĳ��ַ�����ʽ�����1000���ֽڣ�
		std::string setValue;
	};

	/**
	*@brief client->server ��Ϸ��ݼ���������
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
	*@brief server->client ��Ϸ��ݼ����÷���
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
	*@brief scene->client ��Ϸ��ݼ�����ͬ��
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
