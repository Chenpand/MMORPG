#ifndef __DUNGEON_MGR_H__
#define __DUNGEON_MGR_H__

#include "Package.h"
#include "CLObjectIndex.h"
#include <AvalonSimpleTimer.h>
#include "Dungeon.h"
#include "DungeonHardInfo.h"
#include <CLDungeonDefine.h>
#include <CLMatchDefine.h>

class Player;

class DungeonMgr : public CLComplexProperty
{
    typedef HashMap<UInt32, Dungeon*> Map2Dungeon;
    typedef HashMap<UInt32, DungeonHardInfo*> Map2DungeonHardInfo;

	static const UInt32 EnterDungeonNeedBagSpace = 5;

public:
    DungeonMgr();
    ~DungeonMgr();

public:
    /**
    *@brief 设置获取拥有者
    */
    void SetOwner(Player* owner) { m_pOwner = owner; }
    Player* GetOwner() const { return m_pOwner; }

    /**
    *@brief 同步到客户端
    */
    void OnOnline();

    /**
    *@brief 同步开放的地下城列表
    */
    void SyncDungeonList(bool syncAll = false);

    /**
    *@brief 请求开始地下城关卡
            isContinue代表是否继续上一次未完成的，目前只支持死亡之塔
            isHell 代表是不是深渊模式
    */
	void OnStartDungeonReq(UInt32 dungeonId, std::vector<UInt32> bufDrugs, const SceneNpc& cityMonster);

	void OnWorldStartDungeonRes(UInt32 dungeonId, bool isHell, bool isContinue, bool isAssist, UInt32 result);

    /**
    *@brief 能否开始地下城
    */
	UInt32 CanPlayThisMap(UInt32 dungeonId, bool isCheckTicket, bool checkStoryTask = true, bool checkHard = true, bool checkFatigue = true, bool checkTimes = true);

	/**
	*@brief 背包有没有足够的空间
	*/
	bool BagHasEnoughSpace();
    
	/**
	*@brief 获取地下城玩家信息
	*/
	DungeonRacePlayerInfo GetDungeonPlayerInfo(UInt32 dungeonId) const;

	/**
	*@brief 刷新跨服工会领地翻牌翻倍
	*/
	void RefreshGuildTerrDungeonTimes() const;

    /**
    *@brief 能否继续打该地下城
    */
    bool CanContinuePlayThisMap(UInt32 dungeonId);

    /**
    *@brief 地下城结算
    */
	void OnDungeonRaceEnd(UInt32 dungeonId, LevelScore score, UInt16 beHitCount, UInt16 enteredAreaCount, UInt32 usedTime, UInt16 reviveCount, UInt32 endAreaId, bool isAssist, std::vector<ObjID_t> teammateIds);

    /**
    *@brief 通过地下城ID获取地下城信息
    */
    Dungeon* FindDungeonByDataId(UInt32 dataId) const;

    /**
    *@brief 是否通关某一地下城
    */
    bool IsDungeonPassed(UInt32 dungeonId);

	bool IsDungeonPassedSpecScore(UInt32 dungeonId, LevelScore score = LS_C);

    /**
    *@brief 是否通关地下城的任意难度
    */
    bool IsDungeonAnyHardPassed(UInt32 dungeonId);

	/**
	*@brief 是否通关章节的所有地下城
	*/
	bool IsChapterAnyHardPassedSpecScore(UInt32 chapter, LevelScore score = LS_C);

	bool IsDungeonAnyHardPassedSpecScore(UInt32 dungeonId, LevelScore score = LS_C);

    /**
    *@brief 加载地下城信息
    */
    bool LoadDungeon(Dungeon* dungeon);

	/**
	*@brief 清空地下城难度信息
	*/
	void ClearDungeonHardInfo();

    /**
    *@brief 加载地下城难度信息
    */
    bool LoadDungeonHardInfo(DungeonHardInfo* hard);

    /**
    *@brief 通过地下城ID获取地下城难度信息
    */
    DungeonHardInfo* FindDungeonHardInfoById(UInt32 dungeonId);

	/**
	*@brief 获取今天第几次打这个图(从1开始)
	*/
	UInt32 GetDailyCount(UInt32 dungeonId) const;

	/**
	*@brief 获取今天已经打了几次这个图(从0开始)
	*/
	UInt32 GetDailyCountBySubtype(DungeonSubType subType) const;

	/**
	*@brief 获取第几次打这个图
	*/
	UInt32 GetTotalCount(UInt32 dungeonId) const;

	/**
	*@brief 获取第几次打这个图
	*/
	UInt32 GetRemainTimes(UInt32 dungeonId) const;

	/**
	*@brief 如果需要记录每日打的次数就记录，返回这是第几次
	*/
	UInt32 TryRecordDailyCount(UInt32 dungeonId);

	/**
	*@brief 同步剩余次数到world
	*/
	void SyncRemainTimes2World(UInt32 dungeonId) const;

	/**
	*@brief 如果需要记录每周打的次数就记录，返回这是第几次
	*/
	UInt32 TryRecordWeekCount(UInt32 dungeonId);

	/**
	*@brief 检查激活商城限时礼包条件
	*/
	void CheckMallGiftActivateCondition(UInt32 canPlayMapRet, DungeonSubType subType);

	/**
	*@brief 修正地下城id
	*/
	UInt32 AdjustDungeonId(UInt32 dungeonId);

	/**
	*@brief 获取地下城最好成绩
	*/
	UInt32 GetDungeonBestScore(UInt32 dungeonId);

	/**
	*@brief 获取该账号今天第几次打这个图(从1开始)
	*/
	UInt32 GetAccountDailyCount(UInt32 dungeonId) const;
	UInt32 AddAccountDailyCount(UInt32 dungeonId);
	UInt32 GetAccountDailyRemainTimes(UInt32 dungeonId) const;

private:
    void RecordDungeonInfo(UInt32 dungeonId, LevelScore score, UInt32 usedTime);
    void RecordDungeonHardInfo(UInt32 dungeonId, DungeonHardType type);

    /**
    *@brief 尝试解锁下一难度
    */
	void TryToUnlockNextHard(UInt32 dungeonId, LevelScore score, UInt16 beHitCount, UInt16 reviveCount);

    /**
    *@brief 能否解锁某一难度
    */
	bool CanUnlockThisHard(DungeonHardType type, LevelScore score, UInt16 beHitCount, UInt16 reviveCount);

	/**
	*@brief 是否满足关卡开启条件
	*/
	bool IsMatchDungeonOpenCond(UInt32 dungeonId, bool checkStoryTask = true);

    /**
    *@brief 是否已经开放
    */
    bool IsDungeonOpened(UInt32 dungeonId);

    /**
    *@brief 是否已经开放深渊模式
    */
    bool IsDungeonHellModeOpened(UInt32 dungeonId);

    /**
    *@brief 是否已经开放
    */
    bool IsDungeonOpenHellMode(UInt32 dungeonId);

	/**
	*@brief 激活商城疲劳礼包
	*/
	void ActivateFatigue();

	/**
	*@brief 激活商城门票礼包
	*/
	void ActivateTicket(DungeonSubType subType);

public:
    /**
    *@brief 通过地下城ID获取普通的地下城ID(最终经常刷的那个地下城)
    */
    static UInt32 GetNormalDungeonID(UInt32 dungeonId);

    /**
    *@brief 通过地下城ID获取普通难度的地下城ID
    */
    static UInt32 GetNormalHardDungeonID(UInt32 dungeonId);

    /**
    *@brief 通过地下城ID获取前置剧情关卡ID
    */
    static UInt32 GetPrevStoryDungeonID(UInt32 dungeonId);

    /**
    *@brief 通过普通难度地下城ID获取不同难度的地下城ID
    */
    static UInt32 GetDungeonIDByHard(UInt32 normalHardDungeonId, DungeonHardType type);

public:
    /**
    *@brief 场景服务器间同步地下城数据的编码、解码
    */
    void Input(Avalon::NetInputStream &stream);
    void Output(Avalon::NetOutputStream &stream);

    /**
    *@brief 计算新开放和关闭的地下城列表
    */
    void _GenNewOpenDungeonList(std::vector<DungeonOpenInfo>& newOpenList, std::vector<UInt32>& newCloseList);

	/**
	*@breif 设置章节通关gm命令
	*/
	void SetChapterPass(UInt8 chapter);

	void PassAllChapterDungeon();
private:
    //拥有者
    Player*		                m_pOwner;

    Map2Dungeon                 m_dungeons;
    Map2DungeonHardInfo         m_dungeonHardInfoes;

    // 开放的地下城列表
    std::map<UInt32, DungeonOpenInfo>   m_openedDungeonList;
};

#endif

