#ifndef __DUNGEON_PLAYER_H__
#define __DUNGEON_PLAYER_H__

#include <CLDefine.h>
#include <CLObject.h>
#include <AvalonObjectPool.h>
#include <CLDropItemDefine.h>
#include <CLMatchDefine.h>
#include <CLDungeonDefine.h>
#include <CLDungeonProtocol.h>
#include <kingnet_analyzer.h>
#include <set>

#include "DSPlayer.h"

/*
enum DungeonPlayerStatus
{
    DPS_RACING,
    DPS_DISCONNECT,
    DPS_LEAVE,
    DPS_
};*/

struct ActivityMonsterDropAddition
{
	std::set<UInt32>	dropItems;
	UInt32				doubleRate;
};

class DungeonPlayer : public DSPlayer
{
    AVALON_DEFINE_OBJECTPOOL(DungeonPlayer, Avalon::NullMutex)
public:
    DungeonPlayer();
    ~DungeonPlayer();

	inline void SetRacePlayerInfo(const RacePlayerInfo& info) { m_raceInfo = info; }
	inline RacePlayerInfo GetRacePlayerInfo() const { return m_raceInfo; }
	inline const std::vector<RaceEquip>& GetEquips() const { return m_raceInfo.equips; }

    inline void SetOccu(UInt8 occu) { m_occu = occu; }
    inline UInt8 GetOccu() const { return m_occu; }

	inline void SetAccID(UInt32 id) { m_accId = id; }
	inline UInt32 GetAccID() const { return m_accId; }

	inline void SetZoneID(UInt32 id) { m_zoneId = id; }
	inline UInt32 GetZoneID() const { return m_zoneId; }

    inline void SetLevel(UInt16 level) { m_level = level; }
    inline UInt16 GetLevel() const { return m_level; }

    inline void SetLeave(bool leave) { m_isLeave = leave; }
    inline bool IsLeave() const { return m_isLeave; }

    inline void SetDisconnect(bool disconnect) { m_isDisconnect = disconnect; }
    inline bool IsDisconnect() const { return m_isDisconnect; }

    inline void SetCurAreaID(UInt32 areaId) { m_curAreaId = areaId; }
    inline UInt32 GetCurAreaID() const { return m_curAreaId; }

    inline UInt16 GetEnteredAreaCount() const { return (UInt16)m_enteredAreaIds.size(); };

	inline void SetEnterAreaTime(UInt32 time) { m_enterAreaTime = time; }
	inline UInt32 GetEnterAreaTime() const { return m_enterAreaTime; }

    inline std::vector<UInt32>& GetTaskList() { return m_taskList; }
	inline void SetTaskList(const std::vector<UInt32>& taskList) { m_taskList = taskList; }

    inline void SetDungeonAddition(DungeonAddition addition) { m_addition = addition; }
    inline DungeonAddition GetDungeonAddition() const { return m_addition; }
	UInt32 GetDungenAdditionByType(DungeonAdditionRateType type);
	bool   SetDungenAdditionByType(DungeonAdditionRateType type, UInt32 additionRate);

    inline UInt32 KillMonsterTotalExp() const { return m_monsterExp; }

	inline void AddGoldDrop(UInt32 num) { m_goldDropNum += num; }
	inline UInt32 GetGoldDropNum() const { return m_goldDropNum; }

	inline void AddBindGoldDrop(UInt32 num) { m_bindGoldDropNum += num; }
	inline UInt32 GetBindGoldDropNum() const { return m_bindGoldDropNum; }

    inline UInt32 GetUsedTime() const { return m_usedTime; }
    inline void AddUsedTime(UInt32 usedTime) { m_usedTime += usedTime; }
	inline void SetUsedTime(UInt32 usedTime) { m_usedTime = usedTime; }

    inline UInt32 GetRemainHp() const { return m_remainHp; }
    inline void SetRemainHp(UInt32 hp) { m_remainHp = hp; }

    inline UInt32 GetRemainMp() const { return m_remainMp; }
    inline void SetRemainMp(UInt32 mp) { m_remainMp = mp; }

	inline bool HasGotRaceEndDrop() const { return m_hasGotRaceEndDrop; }
	inline void SetGotRaceEndDrop() { m_hasGotRaceEndDrop = true; }

	inline UInt32 GetDailyCount() const { return m_dailyCount; }
	inline void SetDailyCount(UInt32 dailyCount) { m_dailyCount = dailyCount; }

	inline UInt32 GetTotalCount() const { return m_totalCount; }
	inline void SetTotalCount(UInt32 count) { m_totalCount = count; }

	inline void IncReviveCount() { m_reviveCount++; }
	inline UInt16 GetReviveCount() { return m_reviveCount; }

	inline bool IsFreeOpenGoldChest() { return m_freeOpenGoldChest; }
	inline void SetFreeOpenGoldChest(bool free) { m_freeOpenGoldChest = free; }

	inline LevelScore GetLevelScore() { return m_Score; }

	inline bool IsAssist() const { return m_IsAssist; }
	inline void SetAssist(bool isAssist) { m_IsAssist = isAssist; }

	inline bool IsRacing() const { return m_IsRacing; }
	inline void SetRacing(bool racing) { m_IsRacing = racing; }

	inline bool IsGenGuaranteeDrop() const { return m_IsGenGuaranteeDrop; }
	inline void SetGenGuaranteeDrop() { m_IsGenGuaranteeDrop = true; }

	inline UInt32 GetAddedDropNum() const { return m_addedDropNum; }
	inline void SetAddedDropNum(UInt32 num) { m_addedDropNum = num; }

	inline void SetBossDropItems(const VPlayerDropItem& dropItems) { m_bossDrop = dropItems; }
	PlayerDropItem* FindBossDropItem(UInt32 id);
	const VPlayerDropItem& GetBossDropItems() const { return m_bossDrop; }

	inline void SetCheat() { m_cheat = true; }
	inline bool IsCheat() const { return m_cheat; }

	void AddActivityDoubleDropInfo(UInt32 activityMonsterId, std::set<UInt32>& dropItems, UInt32 doubleRate);
	UInt32 GetActivityDoubleDropRate(UInt32 activityMonsterId, UInt32 dropItem);

	bool HasGoldTitle() const;

	void AddDropOverMonster(UInt32 id) { m_dropOverMonsters.push_back(id); }
	const std::vector<UInt32>& GetDropOverMonsters() const { return m_dropOverMonsters; }

	inline bool IsVeteranReturn() const { return m_isVeteranReturn; }
	inline void SetVeteranReturn() { m_isVeteranReturn = true; }

	void IncBufferItemUsedNum(UInt32 itemid);
	UInt32 GetBufferItemUsedNum(UInt32 itemid);

public:
    void OnEnterArea(UInt32 areaId);
    void OnLeaveArea(UInt32 areaId);

	void SetGotAreaReward(UInt32 areaId);
    bool HasGotAreaReward(UInt32 areaId);
    bool HasEnteredArea(UInt32 areaId);
	bool HasClearedArea(UInt32 areaId);

    bool HasRaceEnd() { return m_hasRaceEnd; }
    void OnRaceEnd(LevelScore score);

    void OnKillMonster(UInt32 monsterId, UInt32 exp);
    bool HasKilledMonster(UInt32 monster);

public:
	void OnSyncSceneEvent(SyncEventType type, ObjID_t param1);

public:	//数据中心日志相关
	template <typename... Args>
	void SendUdpLog(const char* typeInfo, const char* format, const Args & ... args)
	{
		if (typeInfo == NULL || format == NULL) return;
		CUserLogger	*pLogger = _analyzer.GetInstance(UDPLOG_DUNGEON_UID);
		if (NULL == pLogger) return;

		char userLogBuffer[MAX_USERMSG_LENGTH];
		memset(userLogBuffer, 0, MAX_USERMSG_LENGTH);
		UInt32 nTime = (UInt32)Avalon::Time::CurrentTime().Sec();
		UInt16 zoneId = (GetSrcZoneId() != 0 ? GetSrcZoneId() : ZONE_ID);

#ifndef AVALON_OS_WINDOWS
		snprintf(userLogBuffer, sizeof(userLogBuffer), LOG_USERMSG_FORMAT, GAME_VERSION.c_str(), "", zoneId, "", "", "", "", "", 
			GetID(), GetAccID(), (UInt32)GetLevel(), 0, (UInt32)GetOccu(), 0, 0, nTime, 0, 0, 0, 0, 0);
#else
		_snprintf_s(userLogBuffer, sizeof(userLogBuffer), LOG_USERMSG_FORMAT, GAME_VERSION.c_str(), "", zoneId, "", "", "", "", "", 
			GetID(), GetAccID(), (UInt32)GetLevel(), 0, (UInt32)GetOccu(), 0, 0, nTime, 0, 0, 0, 0, 0);
#endif

		pLogger->SetUserIP("");
		pLogger->SetUserMsg(userLogBuffer);
		pLogger->LogMsg(typeInfo, format, args...);
	}

	/**
	*@brief 发送复活日志
	*/
	void SendReviveUdpLog(UInt32 dungeonId, UInt64 targetId, UInt32 areaId);

	/**
	*@brief 发送开始地下城的日志
	*/
	void SendDungeonStartUdpLog(UInt32 dungeonId, DungeonType type, std::string dungeonName, DungeonHellMode hellmode, UInt8 isTeam, UInt8 isHaveFriend, UInt8 hellAutoClose);

	/**
	*@brief 发送清房间日志
	*/
	void SendDungeonClearAreaUdpLog(UInt32 dungeonId, DungeonType type, std::string dungeonName, DungeonHellMode hellmode, UInt32 areaId, UInt32 usedTime);

	/**
	*@brief 发送地下城吃药日志
	*/
	void SendDungeonUsePotionUdpLog(UInt64 raceId, UInt32 dungeonId, UInt32 areaId, UInt32 itemTypeId, UInt8 isTeam);
private:    
	RacePlayerInfo			m_raceInfo;				// 战斗需要的数据
    UInt8                   m_occu;                 // 职业ID
    UInt16                  m_level;                // 等级
	UInt32					m_accId;
	UInt32					m_zoneId;
    bool                    m_isLeave;
    bool                    m_isDisconnect;
    bool                    m_hasRaceEnd;
	bool					m_hasGotRaceEndDrop;
    UInt32                  m_curAreaId;
	UInt32					m_enterAreaTime;
    DungeonAddition         m_addition;
    std::vector<UInt32>     m_gotRewardAreaIds;  // 已经获取了经验的区域ID
    std::vector<UInt32>     m_enteredAreaIds;       // 进入过的区域ID
	std::set<UInt32>		m_clearedAreaIds;       // 已经清掉的房间
    std::vector<UInt32>     m_taskList;             // 关卡相关的任务列表
    std::set<UInt32>        m_killedMonster;        // 已经杀过的怪
    UInt32                  m_monsterExp;           // 杀怪获得的经验
	UInt32					m_dailyCount;			// 今日次数
	UInt32					m_totalCount;			// 总次数
	UInt16					m_reviveCount;			// 复活次数
	bool					m_freeOpenGoldChest;	// 免费打开黄金宝箱
	UInt32					m_goldDropNum;			// 获得的金币掉落数量
	UInt32					m_bindGoldDropNum;		// 绑定金币掉落数量
	LevelScore				m_Score;				// 战斗分数
	bool					m_IsAssist;				// 是否助战
	bool					m_IsRacing;				// 是否已经进入战斗
	bool					m_IsGenGuaranteeDrop;	// 是否已经计算过保底掉落

    // 关卡记录
    UInt32                  m_usedTime;             // 使用时间
    UInt32                  m_remainHp;             // Hp
    UInt32                  m_remainMp;             // Mp

	VPlayerDropItem			m_bossDrop;				// boss掉落

	bool					m_cheat;				// 是否已经作弊

	UInt32					m_addedDropNum;			// 额外掉落次数

	// 活动掉落翻倍信息
	std::map<UInt32, ActivityMonsterDropAddition> m_activityDoubleDropInfo;

	// 掉落次数用完的怪物
	std::vector<UInt32>		m_dropOverMonsters;

	// 是否是回归玩家
	bool					m_isVeteranReturn;

	// buffer药使用次数
	std::map<UInt32, UInt32>	m_BufferItemUsedNum;
};

typedef HashMap<ObjID_t, DungeonPlayer*> MapDungeonPlayer;

#endif
