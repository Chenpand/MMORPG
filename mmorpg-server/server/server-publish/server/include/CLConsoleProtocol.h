#ifndef _CL_CONSOLE_PROTOCOL_H_
#define _CL_CONSOLE_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include <CLItemDefine.h>

namespace CLProtocol
{
	/**
	 *@brief admin->server ������������ת���ͷ���̨����
	 */
	class AdminConsoleRequest : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ADMIN_CONSOLE_REQUSET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeid & cmd & subcmd & timestamp & body;
		}

	public:
		//����id
		UInt32		nodeid;
		//ָ��
		std::string	cmd;
		//������
		std::string	subcmd;
		//ʱ���
		UInt32		timestamp;
		//json
		std::string	body;
	};

	/**
	 *@brief server->admin ����������Ҫ��adminת���ظ����ͷ���̨
	 */
	class AdminConsoleResponse : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ADMIN_CONSOLE_RESPONSE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeid & cmd & subcmd & timestamp & body;
		}

	public:
		//����id
		UInt32		nodeid;
		//ָ��
		std::string	cmd;
		//������
		std::string	subcmd;
		//ʱ���
		UInt32		timestamp;
		//json
		std::string	body;
	};

	/**
	 *@brief world->scene �ͷ���̨�����ѯ�����Ϣ
	 */
	class SceneConsoleQueryPlayer : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CONSOLE_QUERY_PLAYER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeid & subcmd & keyType & accId & playerId & name & param & param2;
		}

	public:
		//����id
		UInt32		nodeid;
		//������
		std::string subcmd;
		//key����	1��ʾ�˺�Ϊkey ����ΪnameΪkey
		UInt8		keyType;
		//�˺�id
		UInt32		accId;
		//���id
		ObjID_t		playerId;
		//�����
		std::string	name;
		//����
		UInt32		param;
		//����2
		UInt32		param2;
	};

	/**
	 *@brief world->scene ���¼��ؽű�
	 */
	class SceneConsoleReloadScripts : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CONSOLE_RELOAD_SCRIPTS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & scripts;
		}

		std::vector<UInt32>		scripts;
	};


	/**
	*@brief world->scene �ͷ���̨����������
	*/
	class SceneConsoleControlPlayer : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CONSOLE_PLAYER_CONTROL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeid & subcmd & keyType & accId & playerId & name & param1 & param2 & items;
		}

	public:
		//����id
		UInt32		nodeid;
		//������
		std::string subcmd;
		//key����	1��ʾ�˺�Ϊkey 2.palyerUid key 3.nameΪkey
		UInt8		keyType;
		//�˺�
		UInt32		accId;
		//���id
		ObjID_t		playerId;
		//�����
		std::string	name;
		//��������1
		UInt64		param1;
		//��������2
		UInt32		param2;
		//ͨ�ò�����������
		std::vector<ItemReward>	items;
	};
	
	/**
	*@brief world->scene �ֻ���
	*/
	class SceneConsolePhoneBind : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CONSOLE_PHONE_BIND)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeid & roleid;
		}

	public:
		//����id
		UInt32		nodeid;
		//��ɫid
		ObjID_t		roleid;
	};

	/**
	*@brief world->scene ͬ�����gmȨ��
	*/
	class WorldSyncPlayerGMAuthority : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_PLAYER_GM_AUTHORITY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleid & gmAuth;
		}

	public:
		//��ɫid
		ObjID_t	roleid;
		// gmȨ��
		UInt8 gmAuth;
	};

	/**
	*@brief world->scene �����ʲ�����
	*/
	class SceneConsoleCurrencyFrozen : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CONSOLE_CURRENCY_FROZEN)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeId & roleId & orderId & opType;
		}

	public:
		UInt32 nodeId;
		//��ɫid
		ObjID_t	roleId;
		// ������
		UInt64 orderId;
		// ����
		UInt32 opType;
	};

	/**
	*@brief world->scene �Ƴ����õ�ȯ
	*/
	class SceneConsoleRemoveCreditPoint : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CONSOLE_REMOVE_CREDIT_POINT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeId & onlineId & creditPoint & reason;
		}

	public:

		UInt32 nodeId;
		UInt64 onlineId;
		UInt32 creditPoint;
		std::string reason;
	};
}

#endif
