#ifndef __CL_DUNGEON_PROTOCOL_H__
#define __CL_DUNGEON_PROTOCOL_H__

#include <AvalonSocket.h>
#include <AvalonProtocol.h>
#include "CLProtocolDefine.h"
#include "CLDropItemDefine.h"
#include "CLDungeonDefine.h"
#include "CLMatchDefine.h"
#include "CLRelayServerProtocol.h"
#include "CLObjectDefine.h"

namespace CLProtocol
{

    /**
    *@brief server->client ��������ͻ��˷��͵��³ǳ�ʼ��Ϣ
    */
    class SceneDungeonInit : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_INIT)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & allInfo;
        }

        std::vector<SceneDungeonInfo> allInfo;
    };

    /**
    *@brief server->client ��������ͻ��˸��µ��³���Ϣ
    */
    class SceneDungeonUpdate : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_UPDATE)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & info;
        }

        SceneDungeonInfo info;
    };

    /**
    *@brief server->client ��������ͻ��˷��͵��³ǳ�ʼ�Ѷ���Ϣ
    */
    class SceneDungeonHardInit : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_HARD_INIT)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & allInfo;
        }

        std::vector<SceneDungeonHardInfo> allInfo;
    };

    /**
    *@brief server->client ��������ͻ��˸��µ��³��Ѷ���Ϣ
    */
    class SceneDungeonHardUpdate : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_HARD_UPDATE)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & info;
        }

        SceneDungeonHardInfo info;
    };

	/**
	*@brief world->scene ���µ��³��Ѷ���Ϣ
	*/
	class WorldDungeonHardUpdate : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DUNGEON_HARD_UPDATE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accid & info;
		}

		UInt32					accid;
		SceneDungeonHardInfo	info;
	};

	/**
	*@brief client->scene ��������³Ǵ���
	*/
	class SceneDungeonBuyTimesReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_BUY_TIMES_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & subType;
		}

		// ���³�������
		UInt8		subType;
	};

	/**
	*@brief scene->client ������³Ǵ�������
	*/
	class SceneDungeonBuyTimesRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_BUY_TIMES_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		// ���
		UInt32		result;
	};

    /**
    *@brief server->client ��������ͻ���ͬ���¿��ŵĵ��³��б�
    */
    class SceneDungeonSyncNewOpenedList : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_SYNC_NEW_OPENED_LIST)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & newOpenedList & newClosedList;
        }

        std::vector<DungeonOpenInfo>    newOpenedList;
        std::vector<UInt32>             newClosedList;
    };

    /**
    *@brief client->server ����ʼ�ؿ������������������һ�����ڹؿ�ѡ����濪ʼ�ؿ���һ�����ڹؿ���ѡ�����¿�ʼ������һ�ؿ�
    */
    class SceneDungeonStartReq : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_START_REQ)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
			stream & dungeonId & buffDrugs & isRestart & cityMonsterId;
        }

        UInt32				dungeonId;
		std::vector<UInt32> buffDrugs;
		UInt8				isRestart;
		ObjID_t				cityMonsterId;
    };

    struct SceneDungeonMonster
    {
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & id & pointId & typeId & dropItems & prefixes & summonerId;
        }

        UInt32                              id;
        UInt32                              pointId;
        UInt32                              typeId;
        VSceneDungeonDropItem				dropItems;
        std::vector<UInt8>                  prefixes;
        UInt32                              summonerId;
    };

    struct SceneDungeonArea
    {
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & id & monsters & destructs;
        }

        UInt32 id;
        std::vector<SceneDungeonMonster> monsters;
        // ���ƻ���
        std::vector<SceneDungeonMonster> destructs;
    };

    // ��Ԩ������Ϣ
    struct DungeonHellWaveInfo
    {
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & wave & monsters;
        }
        // ����
        UInt8                               wave;
        // ����
        std::vector<SceneDungeonMonster>    monsters;
    };

    // ��Ԩ��Ϣ
    struct DungeonHellInfo
    {
        DungeonHellInfo()
        {
            mode = 0;
            areaId = 0;
        }

        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & mode & areaId & waveInfoes & dropItems;
        }

        // ģʽ����Ӧö�٣�DungeonHellMode��
        UInt8                               mode;
        // ��������
        UInt32                              areaId;
        // ������Ϣ
        std::vector<DungeonHellWaveInfo>    waveInfoes;
        // ��Ԩ����
        std::vector<SceneDungeonDropItem>   dropItems;
    };

	struct DungeonBuff
	{
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & buffId & buffLvl;
		}
		// buffid
		UInt32 buffId;
		// buff�ȼ�
		UInt32 buffLvl;

		DungeonBuff() : buffId(0), buffLvl(0) {}
	};

	struct GuildDungeonInfo
	{
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & bossOddBlood & bossTotalBlood & buffVec;
		}

		// bossʣ��Ѫ��
		UInt64							bossOddBlood;
		// boss��Ѫ��
		UInt64							bossTotalBlood;
		// �ӳ�buff
		std::vector<DungeonBuff>		buffVec;

		GuildDungeonInfo() : bossOddBlood(0), bossTotalBlood(0) {}

		std::string SerializeBuffVec()
		{
			std::stringstream ss;
			for (auto& iter: buffVec)
			{
				ss << iter.buffId << "," << iter.buffLvl << "|";
			}
		
			return ss.str();
		}
	};

	// �ռ��������³�BUFF
	struct ZjslDungeonBuff
	{
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & buffId & buffLvl & buffTarget;
		}
		// buffid
		UInt32 buffId;
		// buff�ȼ�
		UInt32 buffLvl;
		// buff����1����ң�2������
		UInt32 buffTarget;

		ZjslDungeonBuff() : buffId(0), buffLvl(0), buffTarget(0) {}
	};

	// �ռ��������³���Ϣ
	struct ZjslDungeonInfo
	{
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & boss1ID & boss2ID & boss1RemainHp & boss2RemainHp & buffVec;
		}

		// BOSS1��ID
		UInt32							boss1ID;
		// BOSS2��ID
		UInt32							boss2ID;
		// BOSS1��ʣ��Ѫ��
		UInt32							boss1RemainHp;
		// BOSS2��ʣ��Ѫ��
		UInt32							boss2RemainHp;
		// �ӳ�buff
		std::vector<ZjslDungeonBuff>	buffVec;

		ZjslDungeonInfo() : boss1ID(0), boss2ID(0), boss1RemainHp(0), boss2RemainHp(0) {}
	};

    /**
    *@brief server->client ����ʼ�ؿ�����
    */
    class SceneDungeonStartRes : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_START_RES)
    public:
        SceneDungeonStartRes()
        {
            dungeonId = 0;
            startAreaId = 0;
            result = 0;
            isCointnue = 0;
            hp = 0;
            mp = 0;
			openAutoBattle = 0;
			isRecordLog = isRecordReplay = 0;
			hellAutoClose = 0;
			battleFlag = 0;
			memset(md5, 0, sizeof(md5));
        }

        AVALON_DEFINE_NETSERIALIZATION()
        {
			stream & isCointnue & hp & result & key1 & areas & hellInfo & mp & key2 & session & addr & players & key3 & openAutoBattle & dungeonId & startAreaId & key4 & bossDropItems;
			stream & isRecordLog & isRecordReplay & dropOverMonsters & guildDungeonInfo & hellAutoClose & battleFlag & zjslDungeonInfo & clearedDungeonIds;
			stream.SeriaArray(md5, sizeof(md5));
        }

		// �������һ��״̬
		UInt8                           isCointnue;
		// Ѫ��
		UInt32                          hp;
		// ���
		UInt32                          result;
		// ��Կ1
		UInt32							key1;
		// ��������Ϣ
		std::vector<SceneDungeonArea>   areas;
		// ��Ԩ�����Ϣ
		DungeonHellInfo                 hellInfo;
		// ����
		UInt32                          mp;
		// ��Կ2
		UInt32							key2;
		// ��¼RelayServer��session
		UInt64							session;
		// RelayServer��ַ
		Avalon::SockAddr				addr;
		// ���������Ϣ
		std::vector<RacePlayerInfo>		players;
		// ��Կ3
		UInt32							key3;
		// �Ƿ񿪷��Զ�ս��
		UInt8							openAutoBattle;
		// ���³�ID
        UInt32                          dungeonId;
		// ��ʼ����ID
        UInt32                          startAreaId;
		// ��Կ4
		UInt32							key4;
		// boss����
		VSceneDungeonDropItem			bossDropItems;
		// �Ƿ��¼��־
		UInt8							isRecordLog;
		// �Ƿ��¼¼��
		UInt8							isRecordReplay;
		// ������������˵Ĺ���
		std::vector<UInt32>				dropOverMonsters;
		// ������³���Ϣ(ֻ�ڹ�����³�ʹ��)
		GuildDungeonInfo				guildDungeonInfo;
		// ��Ԩ�Ƿ��Զ�����(�������Ӿ��˳�)
		UInt8							hellAutoClose;
		// ���³�ս�����
		UInt64							battleFlag;
		// �ռ�������Ϣ(ֻ���ռ��������³�ʹ��)
		ZjslDungeonInfo					zjslDungeonInfo;
		// �Ѿ�ͨ�ص��³�id
		std::vector<UInt32>				clearedDungeonIds;
		// md5
		UInt8							md5[16];
    };

    /**
    *@brief server->client ������֪ͨ�ͻ��������������
    */
    class SceneDungeonAddMonsterDropItem : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_ADD_MONSTER_DROP_ITEM)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & monsterId & dropItems;
        }

        UInt32                              monsterId;
        std::vector<SceneDungeonDropItem>   dropItems;
    };
    
    /**
    *@brief client->server �ͻ���֪ͨ����˽�����һ����
    */
    class SceneDungeonEnterNextAreaReq : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_ENTER_NEXT_AREA_REQ)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
			stream & areaId & lastFrame & staticVal;
        }

        UInt32 areaId;
		UInt32 lastFrame;
		DungeonStaticValue staticVal;
    };

    /**
    *@brief server->client ����˷��ؽ�����һ������
    */
    class SceneDungeonEnterNextAreaRes : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_ENTER_NEXT_AREA_RES)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & areaId & result;
        }

        UInt32 areaId;
        UInt32 result;
    };

    /**
    *@brief client->server �ͻ��������������ȡ����
    */
    class SceneDungeonRewardReq : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_REWARD_REQ)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
			stream & lastFrame & dropItemIds;
			stream.SeriaArray(md5, sizeof(md5));
        }

		UInt32					lastFrame;
        std::vector<UInt32>     dropItemIds;    // ʰȡ�ĵ�����ƷID
		UInt8					md5[16];		// ������md5ֵ����������Ǵ����˵�
    };

    /**
    *@brief server->client ����˷��ػ�ȡ�������
    */
    class SceneDungeonRewardRes : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_REWARD_RES)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & pickedItems;
        }

        std::vector<UInt32>     pickedItems;    // ʰȡ�ĵ�����ƷID
    };

    /**
    *@brief client->server ����
    */
    class SceneDungeonRaceEndReq : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_RACE_END_REQ)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & score;
			stream.SeriaArray(md5, sizeof(md5));
			stream & usedTime & beHitCount & maxDamage & skillId & param & totalDamage & lastFrame & bossDamage 
					& playerRemainHp & playerRemainMp & boss1ID & boss2ID & boss1RemainHp & boss2RemainHp & lastChecksum;
        }

        UInt8   score;
		UInt8	md5[16];
        UInt32  usedTime;
		UInt16  beHitCount;
		UInt32	maxDamage; 
		UInt32	skillId;
		UInt32  param;
		UInt64  totalDamage;
		UInt32	lastFrame;

		// ��boss���˺�
		UInt64 bossDamage;

		// ��������������
		// ���ʣ��Ѫ��
		UInt32 playerRemainHp;
		// ���ʣ������
		UInt32 playerRemainMp;
		// BOSS1��ID
		UInt32 boss1ID;
		// BOSS2��ID
		UInt32 boss2ID;
		// BOSS1��ʣ��Ѫ��
		UInt32 boss1RemainHp;
		// BOSS2��ʣ��Ѫ��
		UInt32 boss2RemainHp;
		// ���һ֡�������
		UInt32 lastChecksum;
    };

    /**
    *@brief server->client ���㷵��
    */
    class SceneDungeonRaceEndRes : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_RACE_END_RES)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
			stream & result & score & usedTime & killMonsterTotalExp & raceEndExp & hasRaceEndDrop & 
				raceEndDropBaseMulti & addition & teamReward & hasRaceEndChest & monthcartGoldReward & 
				goldTitleGoldReward & fatigueBurnType & chestDoubleFlag & veteranReturnReward 
				& rollReward & rollSingleReward;
        }

        UInt32				result;
		UInt8				score;
        UInt32				usedTime;
        UInt32				killMonsterTotalExp;
        UInt32				raceEndExp;
		UInt8				hasRaceEndDrop;
		UInt8				raceEndDropBaseMulti;
		DungeonAdditionInfo addition;
		ItemReward			teamReward;
		// ��û�н��㷭��
		UInt8				hasRaceEndChest;
		// �¿������ҽ���
		UInt32				monthcartGoldReward;
		// ��ҳƺŽ�ҽ���
		UInt32				goldTitleGoldReward;
		// ƣ��ȼ������
		UInt8				fatigueBurnType;
		// ���Ʒ�����־
		UInt8				chestDoubleFlag;
		// �ع���佱��
		ItemReward			veteranReturnReward;
		// roll����
		std::vector<RollItemInfo>  rollReward;
		// roll�������ľ�������µ���
		std::vector<ItemReward>  rollSingleReward;
    };

    /**
    *@brief server->client ������֪ͨ�ͻ��˿�ʼ���㱦������
    */
    class SceneDungeonChestNotify : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_CHEST_NOTIFY)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
			stream & payChestCostItemId & payChestCost;
        }

        // ������Ϣ
		UInt32		payChestCostItemId;
        UInt32      payChestCost;
    };

    /**
    *@brief client->server �ͻ�������������򿪱���
    */
    class SceneDungeonOpenChestReq : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_OPEN_CHEST_REQ)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & pos;
        }

        // λ��
        UInt8   pos;
    };

    /**
    *@brief server->client �������ͻ��˷��ر�������
    */
    class SceneDungeonOpenChestRes : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_OPEN_CHEST_RES)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & roleId & pos & chest;
        }

		// ��ɫID
		ObjID_t			roleId;
		// λ��
        UInt8           pos;
        // ����
        DungeonChest    chest;
    };

	/**
	*@brief client->server �ͻ�������������������
	*/
	class SceneDungeonEndDropReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_RACE_END_DROP_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & multi;
		}

		// ����
		UInt8   multi;
	};

	/**
	*@brief server->client �������ͻ��˷��ؽ������
	*/
	class SceneDungeonEndDropRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_RACE_END_DROP_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & multi & items;
		}
		
		// �ܱ���
		UInt8   multi;
		// ������Ʒ
		std::vector<SceneDungeonDropItem> items;
	};

    /**
    *@brief client->scene �ͻ������󸴻�
    */
    class SceneDungeonReviveReq : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_REVIVE_REQ)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
			stream & targetId & reviveId & reviveCoinNum & reviveItem & reviveItemNum;
        }

		// Ҫ�����Ŀ��
		ObjID_t		targetId;
		// ÿһ�θ����һ��ID
		UInt32		reviveId;
		// ���ĵĸ��������
		UInt32		reviveCoinNum;
		// �������
		UInt32		reviveItem;
		// ��������
		UInt32		reviveItemNum;
    };

    /**
    *@brief scene->client ������֪ͨ�ͻ��˸����
    */
    class SceneDungeonReviveRes : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_REVIVE_RES)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & result;
        }

        UInt32      result;
    };

    /**
    *@brief scene->world ����ϡ�п�����Ʒ
    */
    class SceneRareItemReq : public Avalon::Protocol
    {
		AVALON_DEFINE_PROTOCOLID(SCENE_RARE_ITEM_REQ)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
			stream & roleId & items;
        }

		ObjID_t	roleId;
		std::vector<ItemReward>	items;
    };

    /**
    *@brief world->scene ����ϡ�п�����Ʒ�Ľ��
    */
    class SceneRareItemRes : public Avalon::Protocol
    {
		AVALON_DEFINE_PROTOCOLID(SCENE_RARE_ITEM_RES)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
			stream & roleId & items;
        }

		ObjID_t	roleId;
		std::vector<ItemReward>	items;
    };

    
    /**
     *@brief scene->world ���ϡ�п�����Ʒ
     */
    class SceneGotRaceItem : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_GOT_RARE_ITEM)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & itemId & num;
        }

		ObjID_t roleId;
        UInt32  itemId;
        UInt32  num;
    };

    /**
    *@brief scene->world ������㱦��
    */
    class SceneDungeonChestReq : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_CHEST_REQ)
    public:
        SceneDungeonChestReq()
        {
            memset(dropSetIds, 0, sizeof(dropSetIds));
        }

        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & dungeonRaceId & playerId & occu;
            stream.SeriaArray(dropSetIds, DCT_NUM);
        }

        UInt64                  dungeonRaceId;
        UInt64                  playerId;
        UInt8                   occu;                   // ְҵID
        UInt32                  dropSetIds[DCT_NUM];    // �����ID��0����˵������Ҫ��ѯ
    };

    /**
    *@brief world->scene ���㱦�䷵��
    */
    class SceneDungeonChestRes : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_CHEST_RES)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & dungeonRaceId & playerId & chests;
        }

        UInt64                      dungeonRaceId;
        UInt64                      playerId;
        std::vector<DungeonChest>   chests;
    };

    /**
    *@brief world->scene ���ɱ������֪ͨ
    */
    class SceneDungeonKillMonsterReq : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_KILL_MONSTER_REQ)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & monsterIds;
        }

        std::vector<UInt32>     monsterIds;
    };
    
    /**
    *@brief world->scene ���ɱ�����ﷵ��
    */
    class SceneDungeonKillMonsterRes : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_KILL_MONSTER_RES)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & monsterIds;
        }

        std::vector<UInt32>     monsterIds;
    };

    /**
    *@brief world->scene ���ɱ�����ﷵ��
    */
    class SceneDungeonClearAreaMonsters : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_CLEAR_AREA_MONSTERS)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & usedTime & remainMp;
			stream.SeriaArray(md5, sizeof(md5));
			stream & remainHp & lastFrame;
        }

        UInt32      usedTime;
		UInt32      remainMp;
		UInt8		md5[16];
        UInt32      remainHp;
		UInt32		lastFrame;
    };

	/**
	*@brief scene->world ������³�ս��
	*/
	class WorldDungeonStartRaceReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DUNGEON_START_RACE_REQ)
	public:
		WorldDungeonStartRaceReq()
		{
			teamId = 0;
			dungeonId = 0;
			isHell = 0;
			isHellGuide = 0;
			isWeekHell = 0;
			passedAreaIndex = 0;
			session = 0;
			relayId = 0;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & cityMonster & teamId & dungeonId & isHell & isHellGuide & isWeekHell & passedAreaIndex & playerInfoes & relayId & session & relayAddr & zjslDungeonInfo;
		}

		SceneNpc								cityMonster;
		UInt32									teamId;
		UInt32									dungeonId;
		UInt8									isHell;
		UInt8									isHellGuide;
		UInt8									isWeekHell;
		UInt32									passedAreaIndex;

		std::vector<DungeonRacePlayerInfo>		playerInfoes;

		// relayserver���
		UInt32									relayId;
		UInt64									session;
		Avalon::SockAddr						relayAddr;

		// �ռ������������
		ZjslDungeonInfo							zjslDungeonInfo;
	};

	/**
	*@brief world->scene ���³�ս������
	*/
	class WorldDungeonStartRaceRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DUNGEON_START_RACE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & dungeonId & isHell & isContinue & isAssist & result & isHellGuide & isWeekHell;
		}

		ObjID_t		roleId;
		UInt32		dungeonId;
		UInt8		isHell;
		UInt8		isContinue;
		UInt8		isAssist;
		UInt32		result;
		UInt8		isHellGuide;
		UInt8		isWeekHell;
	};

	/**
	*@brief world->scene ���³�ս������
	*/
	class WorldDungeonLeaveRace : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DUNGEON_LEAVE_RACE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & isDisconnect;
		}

		ObjID_t		roleId;
		UInt8		isDisconnect;
	};

	/**
	*@brief world->scene ֪ͨ���³ǽ���
	*/
	class WorldDungeonNotifyRaceEnd : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DUNGEON_NOTIFY_RACE_END)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & raceId & cityMonster & roleId & dungeonId & raceEndExp & endAreaId & score & beHitCount & enteredAreaCount & usedTime & reviveCount 
				& teamReward & isAssist & monthcardGoldReward & goldTitleGoldReward & mysticalMerchantId & mysticalMerchantId & teammateIds
				& raceEndRewards & playerRemainHp & playerRemainMp & boss1ID & boss2ID & boss1RemainHp & boss2RemainHp;
		}

		ObjID_t		raceId;
		SceneNpc	cityMonster;
		ObjID_t		roleId;
		UInt32		dungeonId;
		UInt32		raceEndExp;

		UInt32		endAreaId;
		UInt8		score;
		UInt16		beHitCount;
		UInt16		enteredAreaCount;
		UInt32		usedTime;
		UInt16		reviveCount;
		ItemReward	teamReward;
		UInt8		isAssist;
		UInt32		monthcardGoldReward;
		UInt32		goldTitleGoldReward;
		// ��������id
		UInt32		mysticalMerchantId;
		std::vector<ObjID_t> teammateIds;
		// ���㽱��
		ItemRewardVec raceEndRewards;
		// ��������������
		// ���ʣ��Ѫ��
		UInt32 playerRemainHp;
		// ���ʣ������
		UInt32 playerRemainMp;
		// BOSS1��ID
		UInt32 boss1ID;
		// BOSS2��ID
		UInt32 boss2ID;
		// BOSS1��ʣ��Ѫ��
		UInt32 boss1RemainHp;
		// BOSS2��ʣ��Ѫ��
		UInt32 boss2RemainHp;
	};

	/**
	*@brief scene->world ֪ͨ���³Ƿ���Index����
	*/
	class WorldDungeonNotifyResetAreaIndex : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DUNGEON_NOTIFY_RACE_END)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & dungeonId;
		}

		ObjID_t		roleId;
		UInt32		dungeonId;
	};

	/**
	*@brief client->world ������³Ƿ�������
	*/
	class WorldDungeonGetAreaIndexReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DUNGEON_GET_AREA_INDEX_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & dungeonId;
		}

		UInt32		dungeonId;
	};

	/**
	*@brief world->client ���³Ƿ�����������
	*/
	class WorldDungeonGetAreaIndexRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DUNGEON_GET_AREA_INDEX_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & dungeonId & areaIndex;
		}

		UInt32		dungeonId;
		UInt32		areaIndex;
	};

	/**
	*@brief client->scene �������õ��³Ƿ�������
	*/
	class SceneDungeonResetAreaIndexReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_RESET_AREA_INDEX_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & dungeonId;
		}

		UInt32		dungeonId;
	};

	/**
	*@brief scene->client ���õ��³Ƿ�����������
	*/
	class SceneDungeonResetAreaIndexRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_RESET_AREA_INDEX_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & dungeonId & areaIndex;
		}

		UInt32		dungeonId;
		UInt32		areaIndex;
	};
	
	/**
	*@brief world->scene ɱ����ǰ�������й�
	*/
	class WorldDungeonNotifyClearAreaMonster: public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DUNGEON_NOTIFY_CLEAR_AREA_MONSTER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & dungeonId & areaId & usedTime & remainHp & remainMp;
		}

		ObjID_t		roleId;
		UInt32		dungeonId;
		UInt32		areaId;
		UInt32      usedTime;
		UInt32      remainHp;
		UInt32      remainMp;
	};

	/**
	*@brief world->scene ��������
	*/
	class WorldDungeonNotifyConsumeFatigue : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DUNGEON_NOTIFY_CONSUME_FATIGUE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & value & dungeonDataId;
		}

		ObjID_t		roleId;
		UInt16		value;
		UInt32		dungeonDataId;
	};

	/**
	*@brief world->scene ɱ���¼�
	*/
	class WorldDungeonNotifyKillMonster : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DUNGEON_NOTIFY_KILL_MONSTER)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & dungeonId & monsterId & exp & isAssit;
		}

		ObjID_t		roleId;
		UInt32		dungeonId;
		UInt32		monsterId;
		UInt32		exp;
		UInt8		isAssit;
	};

	/**
	*@brief world->scene �����Ʒ
	*/
	class WorldDungeonNotifyGotItems : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DUNGEON_GOT_ITEMS)
	public:
		WorldDungeonNotifyGotItems() : roleId(0), dungeonId(0), gold(0) {}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & dungeonId & gold & items & reason;
		}

		ObjID_t			roleId;
		UInt32			dungeonId;
		UInt32			gold;
		ItemRewardVec	items;
		std::string		reason;
	};

	/**
	*@brief client->world ���֪ͨ���������������
	*/
	class WorldDungeonEnterRaceReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DUNGEON_ENTER_RACE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->client ������������ҽ������
	*/
	class WorldDungeonEnterRaceRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DUNGEON_ENTER_RACE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		UInt32	result;
	};

	/**
	*@brief client->world ����ϱ�֡����
	*/
	class WorldDungeonReportFrameReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DUNGEON_REPORT_FRAME_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & frames & checksums;
		}

		// ����֡����
		std::vector<Frame>				frames;
		// У��֡
		std::vector<FrameChecksum>		checksums;
	};

	/**
	*@brief world->client ��������ϱ�֡���
	*/
	class WorldDungeonReportFrameRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DUNGEON_REPORT_FRAME_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & lastFrame;
		}

		// �յ������һ֡
		UInt32			lastFrame;
	};

	/**
	*@brief scene->world ����������
	*/
	class WorldDungeonCalDropReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DUNGEON_CAL_DROP)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & dungeonId & times & playerInfo;
		}

		ObjID_t						roleId;
		UInt32						dungeonId;
		UInt32						times;
		DungeonRacePlayerInfo		playerInfo;
	};

	/**
	*@brief client->scene ����ɨ������֮��
	*/
	class SceneTowerWipeoutReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TOWER_WIPEOUT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief scene->client ɨ������֮������
	*/
	class SceneTowerWipeoutRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TOWER_WIPEOUT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		UInt32 result;
	};

	/**
	*@brief client->scene ��������֮��ɨ������
	*/
	class SceneTowerWipeoutResultReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TOWER_WIPEOUT_RESULT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief scene->client ����֮��ɨ����������
	*/
	class SceneTowerWipeoutResultRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TOWER_WIPEOUT_RESULT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & items;
		}

		UInt32								result;
		std::vector<SceneDungeonDropItem>	items;
	};

	/**
	*@brief client->scene ��������֮��ɨ������
	*/
	class SceneTowerWipeoutQueryResultReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TOWER_WIPEOUT_QUERY_RESULT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & beginFloor & endFloor;
		}

		UInt32			beginFloor;
		UInt32			endFloor;
	};

	/**
	*@brief scene->client ����֮��ɨ����������
	*/
	class SceneTowerWipeoutQueryResultRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TOWER_WIPEOUT_QUERY_RESULT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & items;
		}

		UInt32								result;
		std::vector<SceneDungeonDropItem>	items;
	};

	/**
	*@brief client->scene ��������������֮��ɨ��
	*/
	class SceneTowerWipeoutQuickFinishReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TOWER_WIPEOUT_QUICK_FINISH_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief scene->client �����������֮������
	*/
	class SceneTowerWipeoutQuickFinishRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TOWER_WIPEOUT_QUICK_FINISH_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		UInt32								result;
	};

	/**
	*@brief client->scene ������������֮��
	*/
	class SceneTowerResetReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TOWER_RESET_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief scene->client ��������֮������
	*/
	class SceneTowerResetRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TOWER_RESET_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		UInt32								result;
	};

	/**
	*@brief client->scene ��������֮����ͨ����
	*/
	class SceneTowerFloorAwardReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TOWER_FLOOR_AWARD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & floor;
		}

		UInt32 floor;
	};

	/**
	*@brief scene->client ��ȡ����֮����ͨ��������
	*/
	class SceneTowerFloorAwardRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TOWER_FLOOR_AWARD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & items;
		}

		UInt32								result;
		std::vector<SceneDungeonDropItem>	items;
	};

	/**
	*@brief client->scene �ռ��������³�ˢ��BUFF����
	*/
	class SceneDungeonZjslRefreshBuffReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_ZJSL_REFRESH_BUFF_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & dungeonId & useItem;
		}

		UInt32 dungeonId;
		UInt32 useItem;
	};

	/**
	*@brief scene->client �ռ��������³�ˢ��BUFF����
	*/
	class SceneDungeonZjslRefreshBuffRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_ZJSL_REFRESH_BUFF_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		UInt32	result;
	};

	/**
	*@brief client->scene �ռ��������³ǻ�ȡȫͨ�ؽ�������
	*/
	class SceneDungeonZjslClearGetAwardReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_ZJSL_CLEAR_GET_AWARD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief scene->client �ռ��������³ǻ�ȡȫͨ�ؽ�������
	*/
	class SceneDungeonZjslClearGetAwardRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_ZJSL_CLEAR_GET_AWARD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		UInt32	result;
	};

	/**
	*@brief relay->server ���³�boss�׶α仯
	*/
	class RelayDungeonBossPhaseChange : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(RELAY_DUNGEON_BOSS_PHASE_CHANGE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & raceSessionId & bossphase;
		}

		ObjID_t			raceSessionId;
		UInt32			bossphase;
	};

	/**
	*@brief TeamCopy->dungen  �ݵ㱻����
	*/
	class TeamCopyNotifyRaceDestroyDungeon : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(TEAMCOPY_NOTIFY_RACE_DES_DUNGEON)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & destroyDungeonId & notifySessionId;
		}
		UInt32			destroyDungeonId;
		ObjID_t			notifySessionId;
	};

	/**
	*@brief TeamCopy->dungen  ��������
	*/
	class  DungeonSvrEndRace : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(DUNGEON_SVR_END_RACE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & raceSessionId & resaon;
		}
		ObjID_t		raceSessionId;
		UInt32		resaon; //CrossDungeonEndReason
	};

	/**
	*@brief  service->dungeon ���󸴻�
	*/
	class DungeonSvrReviveReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(DUNGEON_SVR_REVIVE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & raceSessionId & playerId & targetId & reviveId & reviveCoinNum 
				& reviveItem & reviveItemNum & reviveHp & reviveMp;
		}

		DungeonSvrReviveReq()
		{
			raceSessionId = 0;
			playerId = 0;
			targetId = 0;
			reviveId = 0;
			reviveCoinNum = 0;
			reviveItem = 0;
			reviveItemNum = 0;
			reviveHp = HP_MP_FULL_VALUE;
			reviveMp = HP_MP_FULL_VALUE;
		}

		// ����SessionId
		ObjID_t		raceSessionId;
		// ���id
		ObjID_t     playerId;
		// Ҫ�����Ŀ��
		ObjID_t		targetId;
		// ÿһ�θ����һ��ID
		UInt32		reviveId;
		// ���ĵĸ��������
		UInt32		reviveCoinNum;
		// ���ĵĸ������id
		UInt32      reviveItem;
		// ���ĵĸ����������
		UInt32      reviveItemNum;
		// �����Ѫ��
		UInt32      reviveHp;
		// ���������
		UInt32		reviveMp;
	};

	/**
	*@brief scene -> dungeon ���ٹ�����������
	*/
	class SceneToDungeonQuickBuyTransactionReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TO_DUNGEON_QUICKBUY_TRANSACTION_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sourceSceneId & roleId & transactionId & type & param1 & param2;
		}

		//Դsceneid
		UInt32		sourceSceneId;
		// ��ɫID
		ObjID_t		roleId;
		// ����ID
		UInt32		transactionId;
		// ����(��Ӧö��QuickBuyTargetType)
		UInt8		type;
		// ����
		ObjID_t		param1;
		ObjID_t		param2;
	};

	/**
	*@brief client->world	��������roll����
	*/
	class WorldDungeonRollItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DUNGEON_ROLL_ITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & dropIndex & opType;
		}

		UInt8  dropIndex; //��������

		UInt8  opType; //��������
	};
	

	/**
	*@brief	world->client  ��������roll���󷵻�
	*/
	class WorldDungeonRollItemRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DUNGEON_ROLL_ITEM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & dropIndex & opType & point;
		}

		UInt32	result; //������

		UInt8  dropIndex; //��������

		UInt8  opType; //��������

		UInt32 point;	//roll����
	};

	/**
	*@brief world->client ��������roll������Ϣ
	*/
	class WorldDungeonRollItemResult : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DUNGEON_ROLL_ITEM_RESULT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & items;
		}

	public:
		std::vector<RollDropResultItem>	items;
	};

	/**
	*@brief TeamCopy->dungen  �ű�֪ͨ����Ī˹֮�������������
	*/
	class TeamCopyNotifyBimsEnergyAccuProgress : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(TEAMCOPY_NOTIFY_BXMS_ENER_ACC_PROG)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & raceSessionId & dungeonId & progress;
		}
		ObjID_t		raceSessionId;
		UInt32		dungeonId;
		UInt32		progress;
	};

	/**
	*@brief TeamCopy->dungen  �ű����ӵ��³ǵĸ������
	*/
	class TeamCopyAddRaceReviveCnt : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(TEAMCOPY_ADD_RACE_REVIVE_CNT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & raceSessionId & reviveCnt;
		}

		ObjID_t		raceSessionId;
		UInt32		reviveCnt;
	};

	/**
	*@brief world -> client ֪ͨ���³ǻ�ó�ɫװ��
	*/
	class WorldDungeonNotifyGetYellowEquip : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DUNGEON_NOTIFY_GET_YELLOW_EQUIP)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & itemId;
		}

		// ��ɫID
		ObjID_t		roleId;
		// ����ID
		UInt32		itemId;
	};
}

#endif