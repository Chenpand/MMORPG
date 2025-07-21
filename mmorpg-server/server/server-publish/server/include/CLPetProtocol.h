#ifndef _CL_PET_PROTOCOL_H_
#define _CL_PET_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLPetDefine.h"

namespace CLProtocol
{
	/**
	*@brief server->client 下发宠物列表
	*/
	class SceneSyncPetList : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_PET_LIST)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & followPetId & battlePets & petList;
		}

	public:
		UInt64 followPetId;
		std::vector<UInt64> battlePets;
		std::vector<PetInfo> petList;
	};

	/**
	*@brief server->client 下发宠物
	*/
	class SceneSyncPet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_PET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & petInfo;
		}
	public:
		PetInfo petInfo;
	};

	/**
	*@brief server->client 删除宠物
	*/
	class SceneDeletePet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_DELELE_PET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}
	public:
		ObjID_t id;
	};

	/*
	*@brief server->client 同步宠物属性
	*		格式: Guid1 + Data1 + Guid2 + Data2 + ... + Guid[0]
	*		Data: [ItemAttr(UInt8) + value][ItemAttr(UInt8) + value]...[0];
	*/
	class SceneSyncPetProperty : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_PET_PROPERTY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief client->server 设置宠物槽位
	*/
	class SceneSetPetSoltReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SET_PET_SOLT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & petType & petId;
		}
	public:
		UInt8 petType;
		UInt64 petId;
	};

	/**
	*@brief server->client 设置宠物槽位
	*/
	class SceneSetPetSoltRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SET_PET_SOLT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & battlePets & followPetId;
		}
	public:
		UInt32 result;
		std::vector<UInt64> battlePets;
		UInt64 followPetId;
	};

	/**
	*@brief client->server 喂食宠物
	*/
	class SceneFeedPetReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_FEED_PET_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & feedType;
		}
	public:
		ObjID_t id;
		UInt8 feedType;
	};

	/**
	*@brief server->client 喂食宠物
	*/
	class SceneFeedPetRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_FEED_PET_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & feedType & isCritical & value;
		}
	public:
		UInt32 result;
		//喂食类型
		UInt8 feedType;
		//是否暴击
		UInt8 isCritical;
		//数值
		UInt32 value;
	};

	/**
	*@brief client->server 出售宠物
	*/
	class SceneSellPetReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SELL_PET_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}
	public:
		ObjID_t id;
	};

	/**
	*@brief server->client 出售宠物
	*/
	class SceneSellPetRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SELL_PET_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}
	public:
		UInt32 result;
	};

	/**
	*@brief client->server 变更宠物技能
	*/
	class SceneChangePetSkillReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHANGE_PET_SKILL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & skillIndex;
		}
	public:
		ObjID_t id;
		UInt8 skillIndex;
	};

	/**
	*@brief server->client 变更宠物技能
	*/
	class SceneChangePetSkillRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHANGE_PET_SKILL_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & petId & skillIndex;
		}
	public:
		UInt32 result;
		ObjID_t petId;
		UInt8 skillIndex;
	};

	/**
	*@brief client->server 宠物跟随
	*/
	class SceneSetPetFollowReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SET_PET_FOLLOW_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}
	public:
		ObjID_t id;
	};

	/**
	*@brief server->client 宠物跟随
	*/
	class SceneSetPetFollowRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SET_PET_FOLLOW_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & petId;
		}
	public:
		UInt32 result;
		UInt64 petId;

	};

	/**
	*@brief client->server 吞食宠物
	*/
	class SceneDevourPetReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_DEVOUR_PET_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & petIds;
		}
	public:
		ObjID_t id;
		//被吞食的宠物
		std::vector<ObjID_t> petIds;
	};

	/**
	*@brief server->client 吞食宠物
	*/
	class SceneDevourPetRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_DEVOUR_PET_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & exp;
		}
	public:
		UInt32 result;
		//获得经验
		UInt32 exp;
	};

}

#endif