#ifndef _CL_SORTLIST_PROTOCOL_H_
#define _CL_SORTLIST_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLSortListDefine.h"

namespace CLProtocol
{
	/**
	 *@brief client->server �������а�
	 */
	class WorldSortListReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SORTLIST_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & occu & start & num;  //modified by aprilliu, ��occuְҵ��Ϣ�ָ�
			//stream & type & start & num;
		}

		//���а�����
		UInt8	type;
		//ְҵ
		UInt8	occu;
		//��ʼλ�� 0��ʼ
		UInt16	start;
		//����
		UInt16	num;
	};

	/**
	 *@brief client->server �����������θ���������Ϣ                                                                     
	 */
	class WorldSortListNearReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SORTLIST_NEAR_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & occu & num;
		}

		//���а�����
		UInt8	type;
		//ְҵ
		UInt8	occu;
		//��Χ����
		UInt32	num;
	};

	/**
	 *@brief server->client �������а�
	 */
	class WorldSortListRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SORTLIST_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & ranking & occu & start & total;
		}

		//���а�����
		UInt8	type;
		//�ҵ�����
		UInt16	ranking;
		//ְҵ
		UInt8	occu;
		//��ʼλ��
		UInt16	start;
		//����
		UInt16	total;
		//�������б�
		//����(UInt16) + ��1(SortListEntry) + ��2(SortListEntry) ...
	};

	/**
	 *@brief client->server �鿴���а�������Ϣ 
	 */
	class WorldSortListWatchDataReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SORTLIST_WATCH_DATA_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sortType & id;
		}

		//���а�����
		UInt8	sortType;
		//���������id
		ObjID_t	id;
	};


	/**
	 *@brief world->scene �۲�����
	 */
	class SceneSortListWatchDataReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SORTLIST_WATCH_DATA_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sortType & watcher & id & dataId & ownerId & ownerName & accId;
		}

		//���а�
		UInt8	sortType;
		//�۲���
		ObjID_t	watcher;
		//id
		ObjID_t id;
		//����id
		UInt32	dataId;
		//����
		ObjID_t ownerId;
		//������
		std::string ownerName;
		//�����˺�
		UInt32	accId;
	};

	/**
	 *@brief scene->world ���ع۲�����
	 */
	class SceneSortListWatchDataRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SORTLIST_WATCH_DATA_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & watcher & id;
		}

		//���� 1��ʾװ�� 2��ʾ����
		UInt8	type;
		//�۲���
		ObjID_t	watcher;
		//id
		ObjID_t	id;
		//����
	};

	/**
	 *@brief scene->world ����������а�
	 */
	class WorldSortListUpdateReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SORTLIST_UPDATE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & entry;
		}
		
		//���а�������
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
		//���а���ĿID
		ObjID_t entryId;
		//ӵ����ID
		ObjID_t ownerId;
		//ӵ��������
		std::string ownerName;

	};

	

	/**
	 *@brief scene->world ����ɾ��������
	 */
	class WorldSortListDeleteEntryReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SORTLIST_DELETEENTRY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sortType & id;
		}

		//����
		UInt8 sortType;
		//id
		ObjID_t	id;
	};

	/**
	 *@brief world->scene �򳡾�ͬ�����а�
	 *	��ʽ : ���� + ��С����ֵ(UInt64) + ���а�id�б�
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
	 *@brief world->scene ͬ�����а�ֵ
	 */
	class SceneSyncSortListMinValue : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_SORTLIST_MINVALUE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sortType & value;
		}

		//���а�����
		UInt8	sortType;
		//ֵ
		UInt64	value;
	};

	/**
	 *@brief world->scene ͬ�����а���
	 */
	class SceneSyncSortListEntry : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_SORTLIST_ENTRY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sortType & flag & id;
		}

		//����
		UInt8	sortType;
		//��־  1Ϊ���� 0Ϊ����
		UInt8	flag;
		//id
		ObjID_t	id;
	};

	/**
	*@brief client->server ��ѯ��������а���Ϣ  using
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
	*@brief server->client ��������а���Ϣ����
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
		// 0 ��ʾδ�ϰ�
		UInt32	ranking;
	};

	/**
	 *@brief world->sortlist  ����ĳ����                                                          
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
	 *@brief sortlist->world ����ĳ����
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
	*@brief world->scene ��ȡ���а�ÿ�ս���
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
	*@brief world->scene ������������
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
