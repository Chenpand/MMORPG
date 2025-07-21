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
    *@brief server->client 服务器向客户端发送地下城初始信息
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
    *@brief server->client 服务器向客户端更新地下城信息
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
    *@brief server->client 服务器向客户端发送地下城初始难度信息
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
    *@brief server->client 服务器向客户端更新地下城难度信息
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
	*@brief world->scene 更新地下城难度信息
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
	*@brief client->scene 请求购买地下城次数
	*/
	class SceneDungeonBuyTimesReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_BUY_TIMES_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & subType;
		}

		// 地下城子类型
		UInt8		subType;
	};

	/**
	*@brief scene->client 购买地下城次数返回
	*/
	class SceneDungeonBuyTimesRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_BUY_TIMES_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		// 结果
		UInt32		result;
	};

    /**
    *@brief server->client 服务器向客户端同步新开放的地下城列表
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
    *@brief client->server 请求开始关卡，这里有两种情况，一种是在关卡选择界面开始关卡，一种是在关卡中选择重新开始或者另一关卡
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
        // 可破坏物
        std::vector<SceneDungeonMonster> destructs;
    };

    // 深渊波次信息
    struct DungeonHellWaveInfo
    {
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & wave & monsters;
        }
        // 波次
        UInt8                               wave;
        // 怪物
        std::vector<SceneDungeonMonster>    monsters;
    };

    // 深渊信息
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

        // 模式，对应枚举（DungeonHellMode）
        UInt8                               mode;
        // 所在区域
        UInt32                              areaId;
        // 波次信息
        std::vector<DungeonHellWaveInfo>    waveInfoes;
        // 深渊掉落
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
		// buff等级
		UInt32 buffLvl;

		DungeonBuff() : buffId(0), buffLvl(0) {}
	};

	struct GuildDungeonInfo
	{
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & bossOddBlood & bossTotalBlood & buffVec;
		}

		// boss剩余血量
		UInt64							bossOddBlood;
		// boss总血量
		UInt64							bossTotalBlood;
		// 加成buff
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

	// 终极试炼地下城BUFF
	struct ZjslDungeonBuff
	{
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & buffId & buffLvl & buffTarget;
		}
		// buffid
		UInt32 buffId;
		// buff等级
		UInt32 buffLvl;
		// buff对象，1：玩家；2：怪物
		UInt32 buffTarget;

		ZjslDungeonBuff() : buffId(0), buffLvl(0), buffTarget(0) {}
	};

	// 终极试炼地下城信息
	struct ZjslDungeonInfo
	{
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & boss1ID & boss2ID & boss1RemainHp & boss2RemainHp & buffVec;
		}

		// BOSS1的ID
		UInt32							boss1ID;
		// BOSS2的ID
		UInt32							boss2ID;
		// BOSS1的剩余血量
		UInt32							boss1RemainHp;
		// BOSS2的剩余血量
		UInt32							boss2RemainHp;
		// 加成buff
		std::vector<ZjslDungeonBuff>	buffVec;

		ZjslDungeonInfo() : boss1ID(0), boss2ID(0), boss1RemainHp(0), boss2RemainHp(0) {}
	};

    /**
    *@brief server->client 请求开始关卡返回
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

		// 保存的上一次状态
		UInt8                           isCointnue;
		// 血量
		UInt32                          hp;
		// 结果
		UInt32                          result;
		// 密钥1
		UInt32							key1;
		// 各区域信息
		std::vector<SceneDungeonArea>   areas;
		// 深渊相关信息
		DungeonHellInfo                 hellInfo;
		// 蓝量
		UInt32                          mp;
		// 密钥2
		UInt32							key2;
		// 登录RelayServer的session
		UInt64							session;
		// RelayServer地址
		Avalon::SockAddr				addr;
		// 所有玩家信息
		std::vector<RacePlayerInfo>		players;
		// 密钥3
		UInt32							key3;
		// 是否开放自动战斗
		UInt8							openAutoBattle;
		// 地下城ID
        UInt32                          dungeonId;
		// 开始房间ID
        UInt32                          startAreaId;
		// 密钥4
		UInt32							key4;
		// boss掉落
		VSceneDungeonDropItem			bossDropItems;
		// 是否记录日志
		UInt8							isRecordLog;
		// 是否记录录像
		UInt8							isRecordReplay;
		// 掉落次数用完了的怪物
		std::vector<UInt32>				dropOverMonsters;
		// 公会地下城信息(只在公会地下城使用)
		GuildDungeonInfo				guildDungeonInfo;
		// 深渊是否自动结束(打完柱子就退出)
		UInt8							hellAutoClose;
		// 地下城战斗标记
		UInt64							battleFlag;
		// 终极试炼信息(只在终极试炼地下城使用)
		ZjslDungeonInfo					zjslDungeonInfo;
		// 已经通关地下城id
		std::vector<UInt32>				clearedDungeonIds;
		// md5
		UInt8							md5[16];
    };

    /**
    *@brief server->client 服务器通知客户端新增怪物掉落
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
    *@brief client->server 客户端通知服务端进入下一区域
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
    *@brief server->client 服务端返回进入下一区域结果
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
    *@brief client->server 客户端向服务端请求获取奖励
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
        std::vector<UInt32>     dropItemIds;    // 拾取的掉落物品ID
		UInt8					md5[16];		// 计算后的md5值，但是这个是打乱了的
    };

    /**
    *@brief server->client 服务端返回获取奖励结果
    */
    class SceneDungeonRewardRes : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_REWARD_RES)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & pickedItems;
        }

        std::vector<UInt32>     pickedItems;    // 拾取的掉落物品ID
    };

    /**
    *@brief client->server 结算
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

		// 对boss的伤害
		UInt64 bossDamage;

		// 极限试炼的数据
		// 玩家剩余血量
		UInt32 playerRemainHp;
		// 玩家剩余蓝量
		UInt32 playerRemainMp;
		// BOSS1的ID
		UInt32 boss1ID;
		// BOSS2的ID
		UInt32 boss2ID;
		// BOSS1的剩余血量
		UInt32 boss1RemainHp;
		// BOSS2的剩余血量
		UInt32 boss2RemainHp;
		// 最后一帧的随机数
		UInt32 lastChecksum;
    };

    /**
    *@brief server->client 结算返回
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
		// 有没有结算翻牌
		UInt8				hasRaceEndChest;
		// 月卡结算金币奖励
		UInt32				monthcartGoldReward;
		// 金币称号金币奖励
		UInt32				goldTitleGoldReward;
		// 疲劳燃烧类型
		UInt8				fatigueBurnType;
		// 翻牌翻倍标志
		UInt8				chestDoubleFlag;
		// 回归掉落奖励
		ItemReward			veteranReturnReward;
		// roll掉落
		std::vector<RollItemInfo>  rollReward;
		// roll单人消耗精力情况下掉落
		std::vector<ItemReward>  rollSingleReward;
    };

    /**
    *@brief server->client 服务器通知客户端开始结算宝箱流程
    */
    class SceneDungeonChestNotify : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_CHEST_NOTIFY)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
			stream & payChestCostItemId & payChestCost;
        }

        // 宝箱信息
		UInt32		payChestCostItemId;
        UInt32      payChestCost;
    };

    /**
    *@brief client->server 客户端向服务端请求打开宝箱
    */
    class SceneDungeonOpenChestReq : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_OPEN_CHEST_REQ)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & pos;
        }

        // 位置
        UInt8   pos;
    };

    /**
    *@brief server->client 服务端向客户端返回宝箱内容
    */
    class SceneDungeonOpenChestRes : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_OPEN_CHEST_RES)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
            stream & roleId & pos & chest;
        }

		// 角色ID
		ObjID_t			roleId;
		// 位置
        UInt8           pos;
        // 宝箱
        DungeonChest    chest;
    };

	/**
	*@brief client->server 客户端向服务端请求结算掉落
	*/
	class SceneDungeonEndDropReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_RACE_END_DROP_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & multi;
		}

		// 倍率
		UInt8   multi;
	};

	/**
	*@brief server->client 服务端向客户端返回结算掉落
	*/
	class SceneDungeonEndDropRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_RACE_END_DROP_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & multi & items;
		}
		
		// 总倍率
		UInt8   multi;
		// 掉落物品
		std::vector<SceneDungeonDropItem> items;
	};

    /**
    *@brief client->scene 客户端请求复活
    */
    class SceneDungeonReviveReq : public Avalon::Protocol
    {
        AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_REVIVE_REQ)
    public:
        AVALON_DEFINE_NETSERIALIZATION()
        {
			stream & targetId & reviveId & reviveCoinNum & reviveItem & reviveItemNum;
        }

		// 要复活的目标
		ObjID_t		targetId;
		// 每一次复活都有一个ID
		UInt32		reviveId;
		// 消耗的复活币数量
		UInt32		reviveCoinNum;
		// 复活道具
		UInt32		reviveItem;
		// 道具数量
		UInt32		reviveItemNum;
    };

    /**
    *@brief scene->client 服务器通知客户端复活返回
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
    *@brief scene->world 请求稀有控制物品
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
    *@brief world->scene 请求稀有控制物品的结果
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
     *@brief scene->world 获得稀有控制物品
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
    *@brief scene->world 请求结算宝箱
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
        UInt8                   occu;                   // 职业ID
        UInt32                  dropSetIds[DCT_NUM];    // 如果是ID是0，就说明不需要查询
    };

    /**
    *@brief world->scene 结算宝箱返回
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
    *@brief world->scene 玩家杀死怪物通知
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
    *@brief world->scene 玩家杀死怪物返回
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
    *@brief world->scene 玩家杀死怪物返回
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
	*@brief scene->world 请求地下城战斗
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

		// relayserver相关
		UInt32									relayId;
		UInt64									session;
		Avalon::SockAddr						relayAddr;

		// 终极试炼爬塔相关
		ZjslDungeonInfo							zjslDungeonInfo;
	};

	/**
	*@brief world->scene 地下城战斗返回
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
	*@brief world->scene 地下城战斗返回
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
	*@brief world->scene 通知地下城结算
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
		// 神秘商人id
		UInt32		mysticalMerchantId;
		std::vector<ObjID_t> teammateIds;
		// 结算奖励
		ItemRewardVec raceEndRewards;
		// 极限试炼的数据
		// 玩家剩余血量
		UInt32 playerRemainHp;
		// 玩家剩余蓝量
		UInt32 playerRemainMp;
		// BOSS1的ID
		UInt32 boss1ID;
		// BOSS2的ID
		UInt32 boss2ID;
		// BOSS1的剩余血量
		UInt32 boss1RemainHp;
		// BOSS2的剩余血量
		UInt32 boss2RemainHp;
	};

	/**
	*@brief scene->world 通知地下城房间Index重置
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
	*@brief client->world 请求地下城房间索引
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
	*@brief world->client 地下城房间索引返回
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
	*@brief client->scene 请求重置地下城房间索引
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
	*@brief scene->client 重置地下城房间索引返回
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
	*@brief world->scene 杀死当前房间所有怪
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
	*@brief world->scene 消耗体力
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
	*@brief world->scene 杀怪事件
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
	*@brief world->scene 获得物品
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
	*@brief client->world 玩家通知服务器进入比赛了
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
	*@brief world->client 服务器返回玩家进入比赛
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
	*@brief client->world 玩家上报帧数据
	*/
	class WorldDungeonReportFrameReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DUNGEON_REPORT_FRAME_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & frames & checksums;
		}

		// 所有帧数据
		std::vector<Frame>				frames;
		// 校验帧
		std::vector<FrameChecksum>		checksums;
	};

	/**
	*@brief world->client 返回玩家上报帧结果
	*/
	class WorldDungeonReportFrameRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DUNGEON_REPORT_FRAME_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & lastFrame;
		}

		// 收到的最后一帧
		UInt32			lastFrame;
	};

	/**
	*@brief scene->world 计算掉落概率
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
	*@brief client->scene 请求扫荡死亡之塔
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
	*@brief scene->client 扫荡死亡之塔返回
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
	*@brief client->scene 请求死亡之塔扫荡奖励
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
	*@brief scene->client 死亡之塔扫荡奖励返回
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
	*@brief client->scene 请求死亡之塔扫荡奖励
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
	*@brief scene->client 死亡之塔扫荡奖励返回
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
	*@brief client->scene 请求快速完成死亡之塔扫荡
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
	*@brief scene->client 快速完成死亡之塔返回
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
	*@brief client->scene 请求重置死亡之塔
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
	*@brief scene->client 重置死亡之塔返回
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
	*@brief client->scene 请求死亡之塔首通奖励
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
	*@brief scene->client 领取死亡之塔首通奖励返回
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
	*@brief client->scene 终极试炼地下城刷新BUFF请求
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
	*@brief scene->client 终极试炼地下城刷新BUFF返回
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
	*@brief client->scene 终极试炼地下城获取全通关奖励请求
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
	*@brief scene->client 终极试炼地下城获取全通关奖励返回
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
	*@brief relay->server 地下城boss阶段变化
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
	*@brief TeamCopy->dungen  据点被消灭
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
	*@brief TeamCopy->dungen  结束比赛
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
	*@brief  service->dungeon 请求复活
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

		// 比赛SessionId
		ObjID_t		raceSessionId;
		// 玩家id
		ObjID_t     playerId;
		// 要复活的目标
		ObjID_t		targetId;
		// 每一次复活都有一个ID
		UInt32		reviveId;
		// 消耗的复活币数量
		UInt32		reviveCoinNum;
		// 消耗的复活道具id
		UInt32      reviveItem;
		// 消耗的复活道具数量
		UInt32      reviveItemNum;
		// 复活后血量
		UInt32      reviveHp;
		// 复活后蓝量
		UInt32		reviveMp;
	};

	/**
	*@brief scene -> dungeon 快速购买事务请求
	*/
	class SceneToDungeonQuickBuyTransactionReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TO_DUNGEON_QUICKBUY_TRANSACTION_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sourceSceneId & roleId & transactionId & type & param1 & param2;
		}

		//源sceneid
		UInt32		sourceSceneId;
		// 角色ID
		ObjID_t		roleId;
		// 事务ID
		UInt32		transactionId;
		// 类型(对应枚举QuickBuyTargetType)
		UInt8		type;
		// 参数
		ObjID_t		param1;
		ObjID_t		param2;
	};

	/**
	*@brief client->world	公共掉落roll请求
	*/
	class WorldDungeonRollItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DUNGEON_ROLL_ITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & dropIndex & opType;
		}

		UInt8  dropIndex; //掉落索引

		UInt8  opType; //请求类型
	};
	

	/**
	*@brief	world->client  公共掉落roll请求返回
	*/
	class WorldDungeonRollItemRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DUNGEON_ROLL_ITEM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & dropIndex & opType & point;
		}

		UInt32	result; //错误码

		UInt8  dropIndex; //掉落索引

		UInt8  opType; //请求类型

		UInt32 point;	//roll点数
	};

	/**
	*@brief world->client 公共掉落roll结算信息
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
	*@brief TeamCopy->dungen  团本通知贝西莫斯之心能量蓄积进度
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
	*@brief TeamCopy->dungen  团本增加地下城的复活次数
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
	*@brief world -> client 通知地下城获得橙色装备
	*/
	class WorldDungeonNotifyGetYellowEquip : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_DUNGEON_NOTIFY_GET_YELLOW_EQUIP)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & itemId;
		}

		// 角色ID
		ObjID_t		roleId;
		// 道具ID
		UInt32		itemId;
	};
}

#endif