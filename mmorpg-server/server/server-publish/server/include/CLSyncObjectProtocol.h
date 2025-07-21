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
		// ���ӵľ���(��������ð�նӾ���)
		UInt64 incExp;
	};

	/**
	 *@brief scene->client ͬ����������
	 *		��ʽ��[dataid+data][dataid+data][dataid+data]...[0]
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
	 *@brief scene->client ͬ������obj
	 *		��ʽ [isnew(UInt8)] + [ObjID_t + type + data][ObjID_t + type + data][ObjID_t + type + data]...[0]
	 *		data��ʽ  [dataid+data][dataid+data][dataid+data]...[0]
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
	 *@brief scene->client ͬ���������ݵ���Χ���
	 *		��ʽ  ObjID_t + [dataid+data][dataid+data][dataid+data]...[0]
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
	 *@brief scene->client ɾ������object
	 *		��ʽ  ObjID_t + ObjID_t + ObjID_t + 0
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
	 *@brief world->scene �򳡾�ͬ������ϵ����
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
		//���id
		ObjID_t	playerId;
		//����id
		UInt32	teamId;
	};


	/**
	 *@brief scene->world ͬ����һ�����Ϣ
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
		//���id
		ObjID_t	playerId;
		//�ϵĵȼ�
		UInt16	oldLevel;
		//�ȼ�
		UInt16	level;
		//vip�ȼ�
		UInt8	vip;

		//vip����
		UInt32	vipExp;

		//ս��
		UInt32	power;
		//�ㄻ
		UInt32  point;
		//���õㄻ
		UInt32  creditPoint;
		//ְҵ
		UInt8	occu;
		//����
		UInt8	awaken;
		//��Ծ��
		UInt32	activeDegree;
		//�ܳ�ֵ���
		UInt32	totalChargeNum;
		//�¿�����ʱ��
		UInt32	monthCardExpireTime;
		//������λ
		UInt32	seasonLv;
		//������ˢ��ʱ��
		UInt32	auctionRefreshTime;
		//���⹺�����������λ
		UInt32	auctionAdditionBooth;
		//��ɫ�ܳ�ֵ���
		UInt32  totalPlayerChargeNum;
		//��Ƽƻ�״̬
		UInt8   moneyManageStatus;
		//����
		ExpSyncInfo expInfo;
		// ͷ���
		UInt32		headFrame;
		// �ع�ƺ�
		UInt32      returnYearTitle;
		//��װ�������
		bool haveSuit;
	};

	/**
	 *@brief scene->world ֪ͨɱ�������
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
		//ɱ����
		ObjID_t killer;
		//��ɱ��
		ObjID_t	dieder;
	};

	/**
	 *@brief world->scene ֪ͨ���Ӿ���
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
		//���
		ObjID_t	playerId;
		//����
		UInt32	exp;
	};

	/**
	 *@brief client->world �۲���� ��������
	 */
	class WorldWatchPlayerReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_WATCH_PLAYER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & name;
		}

		//���id
		ObjID_t		playerId;
		//�����
		std::string	name;
	};

	/**
	 *@brief world->scene ת�����󵽳�����������
	 */
	class SceneWatchPlayerReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_WATCH_PLAYER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & watcher & target & tribe;
		}

		//�۲���
		ObjID_t	watcher;
		//Ŀ��
		ObjID_t	target;
		//Ŀ�겿��
		std::string	tribe;
	};

	/**
	 *@brief scene->client ͬ�����۲���
	 *		��ʽ��playerId(ObjID_t) + tribeName(string) + [dataid+data][dataid+data][dataid+data]...[0]
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
	 *@brief world->scene �򳡾�ͬ���ͷ�
	 */
	class SceneSyncPunishment : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_PUNISHMENT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & punishflag & forbitTalkDueTime;
		}
		//���id
		ObjID_t	id;
		//�ͷ���־λ
		UInt32	punishflag;
		//���Ե���ʱ��
		UInt32	forbitTalkDueTime;
	};

	/**
	 *@brief client->scene ����ĳ���������������
	 */
	class SceneObjectDataReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_OBJECTDATA_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

		//����id
		ObjID_t	id;
	};

	/**
	 *@brief client->server ������ҵ�ս��������
	 */
	class WorldPlayerPowerReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_PLAYER_POWER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & name;
		}

		//�����
		std::string	name;
	};

	/**
	 *@brief world->scene worldת����scene����ҵ�ս������������
	 */
	class ScenePlayerPowerReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_PLAYER_POWER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & watcher & target & tribe;
		}

		//�۲���
		ObjID_t	watcher;
		//Ŀ��
		ObjID_t	target;
		//Ŀ�겿��
		std::string	tribe;
	};

	/**
	 *@brief server->client ͬ����ҵ�ս��������
	 *		��ʽ��playerId(ObjID_t) + name(string) + �ȼ�(UInt16) + ְҵ(UInt8) + �Ա�(UInt8) + tribename(string) + ��ս����(UInt32) + [ս����id(UInt8) + ս����(UInt32)] + [ս����id(UInt8) + ս����(UInt32)] + ... UInt8(0)
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
	*@brief world<->scene scene��world֮���¼�ͬ��  added by huchenhui
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

		//���ID
		ObjID_t playerId;

		//�¼�����
		UInt32	type;

		// ����
		ObjID_t	param1;

		// ����
		ObjID_t param2;

		// ����
		ObjID_t param3;
	};

	/**
	*@brief scene->world �����¼�ͬ��  added by zhengfeng
	*/
	class SceneSyncOfflineEvent : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_OFFLINE_EVENT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & type & param1 & param2;
		}

		//���ID
		ObjID_t playerId;
		//�¼�����
		UInt32	type;
		// ����
		ObjID_t	param1;
		// ����
		ObjID_t param2;
	};

	/**
	*@brief client->scene ���������
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
	*@brief scene->world ͬ��������
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
	*@brief scene->world ͬ������˺���Ϣ
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
		// �ֶ�����(��Ӧö��AccountInfoType)
		UInt8		type;
		UInt32		value;
	};

	/**
	*@brief client->scene �����û��Զ����ֶ�
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
	*@brief scene->client npc�б�
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
	*@brief scene->client ����npc
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
	*@brief scene->client ɾ��npc
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
	*@brief scene->client �޸�npc״̬
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
	*@brief world-> scene/client �������ͬ��
	*/
	class WorldFigureStatueSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_FIGURE_STATUE_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & figureStatue;
		}

		// Ҫչʾ���������
		std::vector<FigureStatueInfo> figureStatue;
	};


	/**
	*@brief scene->client item�б�
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
	*@brief scene->client ����item
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
	*@brief scene->client ɾ��item
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
	*@brief scene->client ����item
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
	*@brief client->scene ͬ��item�б�
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

		// ��������
		UInt32 type;
		// ����
		UInt64 num;
	};

	/**
	*@brief world->client �˺ż���
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
	*@brief world->scene �˺ż���
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
	*@brief scene->world ͬ����ɫ����
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
		// ��ɫ����
		UInt32 roleValueScore;
		// �Ƿ񴴽�Ӷ����
		UInt8 isCreateAdvent;
	};

	/**
	*@brief world->scene �˺�����ͬ��
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
	*@brief world<->scene ͬ����Ϸ����
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
	*@brief clinet->scene �ͻ������
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
		// �¼���
		std::string			name;
		// ����1
		std::string			param1;
		// ����2
		std::string			param2;
		// ����3
		std::string			param3;
	};

	/**
	*@brief clinet->scene ѡ���ɫ����ȡ��Ϣ
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
	*@brief scene->client ֪ͨ�ͻ��˻�þ���
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
		// ��ȡ������Դ(��Ӧö��PlayerIncExpReason)
		UInt8			reason;
		// ��õ�ֵ(��������ֵ�������ǰٷֱȣ�������Դ����)
		UInt32			value;
		// ��õľ���ֵ
		UInt32			incExp;
	};
};
#endif
