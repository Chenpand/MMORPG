#ifndef _CL_DAILY_TODO_PROTOCOL_H_
#define _CL_DAILY_TODO_PROTOCOL_H_
#include <AvalonProtocol.h>
#include "CLProtocolDefine.h"
#include "CLDailyTodoDefine.h"

namespace CLProtocol
{
	/**
	*@brief client->world ÿ�ձ�������
	*/
	class WorldGetPlayerDailyTodosReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GET_PLAYER_DAILY_TODOS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->client ÿ�ձ�������
	*/
	class WorldGetPlayerDailyTodosRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GET_PLAYER_DAILY_TODOS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & dailyTodos;
		}

		std::vector<DailyTodoInfo>	dailyTodos;
	};

	/**
	*@brief scene->world ÿ�ձ������ݱ仯֪ͨworld
	*/
	class WorldDailyTodoSyncData : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DAILYTODO_SYNC_DATA)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & id & value;
		}

		ObjID_t playerId;
		UInt8	id;
		UInt32	value;
	};
}

#endif