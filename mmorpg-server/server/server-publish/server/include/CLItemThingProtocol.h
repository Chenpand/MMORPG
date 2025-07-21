#ifndef _CL_ITEM_THING_PROTOCOL_H_
#define _CL_ITEM_THING_PROTOCOL_H_

#include <CLProtocolDefine.h>
#include <CLItemThing.h>

namespace CLProtocol
{
	/**
	*@brief node->scene ��ʼ��������
	*/
	class SceneItemThingStartReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ITEM_THING_START_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & serialId & servType & param & reason & itemCost;
		}

		// ��ɫ
		UInt64 roleId;
		// �������
		UInt64 serialId;
		// �������
		UInt32 servType;
		// ����
		UInt64 param;
		// reason
		std::string reason;
		// ����
		ItemThingCost itemCost;
	};

	/**
	*@brief scene->node ��ʼ���񷵻�
	*/
	class SceneItemThingStartRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ITEM_THING_START_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & serialId & itemThingGuid;
		}

		SceneItemThingStartRes()
		{
			retCode = 0;
			serialId = 0;
			itemThingGuid = 0;
		}

		// ������
		UInt32 retCode;
		// �������
		UInt64 serialId;
		// ����id
		UInt64 itemThingGuid;
	};

	/**
	*@brief node->scene ��ʼ����ȡ��
	*/
	class SceneItemThingStartCancel : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ITEM_THING_START_CANCEL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & serialId & servType & param;
		}

		// ��ɫ
		UInt64 roleId;
		// �������
		UInt64 serialId;
		// �������
		UInt32 servType;
		// ����
		UInt64 param;
	};

	/**
	*@brief node->scene �����ύ����
	*/
	class SceneItemThingCommitReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ITEM_THING_COMMIT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & itemThingGuid & servType & param & isCommit;
		}

		// ��ɫ
		UInt64 roleId;
		// ����id
		UInt64 itemThingGuid;
		// �������
		UInt32 servType;
		// ����
		UInt64 param;
		// �Ƿ��ύ(�ύ��1�� �ع���0)
		UInt32 isCommit;
	};

	/**
	*@brief scene->node �����ύ����
	*/
	class SceneItemThingCommitRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ITEM_THING_COMMIT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & itemThingGuid;
		}

		SceneItemThingCommitRes()
		{
			retCode = 0;
			itemThingGuid = 0;
		}

		// ������
		UInt32 retCode;
		// ����id
		UInt64 itemThingGuid;
	};
};

#endif // !_CL_ITEM_THING_PROTOCOL_H_

