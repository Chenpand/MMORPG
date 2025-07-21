#ifndef _CL_SORTLIST_PROTOCOL_H_
#define _CL_SORTLIST_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLSortListDefine.h"

namespace CLProtocol
{
	/**
	 *@brief client->server 请求排行榜
	 */
	class WorldSortListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SORTLIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & occu & start & num;  //modified by aprilliu, 把occu职业信息恢复
			//stream & type & start & num;
		}

		//排行榜类型
		UInt8	type;
		//职业
		UInt8	occu;
		//开始位置 0开始
		UInt16	start;
		//数量
		UInt16	num;
	};

	/**
	 *@brief client->server 请求自身名次附近排名信息                                                                     
	 */
	class WorldSortListNearReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SORTLIST_NEAR_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & occu & num;
		}

		//排行榜类型
		UInt8	type;
		//职业
		UInt8	occu;
		//周围条数
		UInt32	num;
	};

	/**
	 *@brief server->client 返回排行榜
	 */
	class WorldSortListRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SORTLIST_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & ranking & occu & start & total;
		}

		//排行榜类型
		UInt8	type;
		//我的排名
		UInt16	ranking;
		//职业
		UInt8	occu;
		//开始位置
		UInt16	start;
		//总数
		UInt16	total;
		//数据项列表
		//数量(UInt16) + 项1(SortListEntry) + 项2(SortListEntry) ...
	};

	/**
	 *@brief client->server 查看排行榜数据信息 
	 */
	class WorldSortListWatchDataReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SORTLIST_WATCH_DATA_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sortType & id;
		}

		//排行榜类型
		UInt8	sortType;
		//武器或宠物id
		ObjID_t	id;
	};


	/**
	 *@brief world->scene 观察数据
	 */
	class SceneSortListWatchDataReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SORTLIST_WATCH_DATA_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sortType & watcher & id & dataId & ownerId & ownerName & accId;
		}

		//排行榜
		UInt8	sortType;
		//观察者
		ObjID_t	watcher;
		//id
		ObjID_t id;
		//数据id
		UInt32	dataId;
		//主人
		ObjID_t ownerId;
		//主人名
		std::string ownerName;
		//主人账号
		UInt32	accId;
	};

	/**
	 *@brief scene->world 返回观察数据
	 */
	class SceneSortListWatchDataRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SORTLIST_WATCH_DATA_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & watcher & id;
		}

		//类型 1表示装备 2表示宠物
		UInt8	type;
		//观察者
		ObjID_t	watcher;
		//id
		ObjID_t	id;
		//数据
	};

	/**
	 *@brief scene->world 请求更新排行榜
	 */
	class WorldSortListUpdateReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SORTLIST_UPDATE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & entry;
		}
		
		//排行榜数据项
		SortListEntry entry;
	};

	class WorldSortListOwnerChangedReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SORTLIST_OWNER_CHANGED_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & entryId & ownerId & ownerName;
		}


		UInt8 type;
		//排行榜项目ID
		ObjID_t entryId;
		//拥有者ID
		ObjID_t ownerId;
		//拥有者名称
		std::string ownerName;

	};

	

	/**
	 *@brief scene->world 请求删除排名项
	 */
	class WorldSortListDeleteEntryReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SORTLIST_DELETEENTRY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sortType & id;
		}

		//类型
		UInt8 sortType;
		//id
		ObjID_t	id;
	};

	/**
	 *@brief world->scene 向场景同步排行榜
	 *	格式 : 类型 + 最小排序值(UInt64) + 排行榜id列表
	 */
	class SceneSyncSortLists: public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_SORTLISTS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	 *@brief world->scene 同步排行榜值
	 */
	class SceneSyncSortListMinValue : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_SORTLIST_MINVALUE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sortType & value;
		}

		//排行榜类型
		UInt8	sortType;
		//值
		UInt64	value;
	};

	/**
	 *@brief world->scene 同步排行榜项
	 */
	class SceneSyncSortListEntry : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_SORTLIST_ENTRY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sortType & flag & id;
		}

		//类型
		UInt8	sortType;
		//标志  1为进榜 0为出榜
		UInt8	flag;
		//id
		ObjID_t	id;
	};

	/**
	*@brief client->server 查询自身的排行榜信息  using
	*/
	class WorldSortListSelfInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SORTLIST_SELFINFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type;
		}

		UInt8 type;
	};

	/**
	*@brief server->client 自身的排行榜信息返回
	*/
	class WorldSortListSelfInfoRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SORTLIST_SELFINFO_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & ranking;
		}

	public:
		// 0 表示未上榜
		UInt32	ranking;
	};

	/**
	 *@brief world->sortlist  请求某个榜单                                                          
	 */
	class WorldSortListVisitReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SORTLIST_VISIT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sortType & function & num;
		}

	public:
		UInt8 sortType;
		std::string	function;
		UInt32 num;
	};

	struct SortListVisitEntry
	{
		SortListVisitEntry()
			:id(0),value1(0),value2(0),value3(0){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & value1 & value2 & value3;
		}

		ObjID_t id;
		UInt32  value1;
		UInt32	value2;
		UInt32  value3;
	};

	/**
	 *@brief sortlist->world 返回某个榜单
	 */
	class WorldSortListVisitRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SORTLIST_VISIT_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sortType & function & sortlist;
		}

	public:
		UInt8				sortType;
		std::string			function;
		std::vector<SortListVisitEntry> sortlist;	
	};

	/**
	*@brief world->scene 获取排行榜每日奖励
	*/
	class SceneSortListDailyReward : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SORTLIST_DAILY_REWARD)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId;
		}

	public:
		UInt64 playerId;
	};

	/**
	*@brief world->scene 重置赛季积分
	*/
	class SceneSortListResetSeason : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SORTLIST_RESET_SEASON)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId;
		}

	public:
		UInt64 playerId;
	};

};

#endif
