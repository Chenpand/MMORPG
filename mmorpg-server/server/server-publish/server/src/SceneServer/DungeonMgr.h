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
    *@brief ���û�ȡӵ����
    */
    void SetOwner(Player* owner) { m_pOwner = owner; }
    Player* GetOwner() const { return m_pOwner; }

    /**
    *@brief ͬ�����ͻ���
    */
    void OnOnline();

    /**
    *@brief ͬ�����ŵĵ��³��б�
    */
    void SyncDungeonList(bool syncAll = false);

    /**
    *@brief ����ʼ���³ǹؿ�
            isContinue�����Ƿ������һ��δ��ɵģ�Ŀǰֻ֧������֮��
            isHell �����ǲ�����Ԩģʽ
    */
	void OnStartDungeonReq(UInt32 dungeonId, std::vector<UInt32> bufDrugs, const SceneNpc& cityMonster);

	void OnWorldStartDungeonRes(UInt32 dungeonId, bool isHell, bool isContinue, bool isAssist, UInt32 result);

    /**
    *@brief �ܷ�ʼ���³�
    */
	UInt32 CanPlayThisMap(UInt32 dungeonId, bool isCheckTicket, bool checkStoryTask = true, bool checkHard = true, bool checkFatigue = true, bool checkTimes = true);

	/**
	*@brief ������û���㹻�Ŀռ�
	*/
	bool BagHasEnoughSpace();
    
	/**
	*@brief ��ȡ���³������Ϣ
	*/
	DungeonRacePlayerInfo GetDungeonPlayerInfo(UInt32 dungeonId) const;

	/**
	*@brief ˢ�¿��������ط��Ʒ���
	*/
	void RefreshGuildTerrDungeonTimes() const;

    /**
    *@brief �ܷ������õ��³�
    */
    bool CanContinuePlayThisMap(UInt32 dungeonId);

    /**
    *@brief ���³ǽ���
    */
	void OnDungeonRaceEnd(UInt32 dungeonId, LevelScore score, UInt16 beHitCount, UInt16 enteredAreaCount, UInt32 usedTime, UInt16 reviveCount, UInt32 endAreaId, bool isAssist, std::vector<ObjID_t> teammateIds);

    /**
    *@brief ͨ�����³�ID��ȡ���³���Ϣ
    */
    Dungeon* FindDungeonByDataId(UInt32 dataId) const;

    /**
    *@brief �Ƿ�ͨ��ĳһ���³�
    */
    bool IsDungeonPassed(UInt32 dungeonId);

	bool IsDungeonPassedSpecScore(UInt32 dungeonId, LevelScore score = LS_C);

    /**
    *@brief �Ƿ�ͨ�ص��³ǵ������Ѷ�
    */
    bool IsDungeonAnyHardPassed(UInt32 dungeonId);

	/**
	*@brief �Ƿ�ͨ���½ڵ����е��³�
	*/
	bool IsChapterAnyHardPassedSpecScore(UInt32 chapter, LevelScore score = LS_C);

	bool IsDungeonAnyHardPassedSpecScore(UInt32 dungeonId, LevelScore score = LS_C);

    /**
    *@brief ���ص��³���Ϣ
    */
    bool LoadDungeon(Dungeon* dungeon);

	/**
	*@brief ��յ��³��Ѷ���Ϣ
	*/
	void ClearDungeonHardInfo();

    /**
    *@brief ���ص��³��Ѷ���Ϣ
    */
    bool LoadDungeonHardInfo(DungeonHardInfo* hard);

    /**
    *@brief ͨ�����³�ID��ȡ���³��Ѷ���Ϣ
    */
    DungeonHardInfo* FindDungeonHardInfoById(UInt32 dungeonId);

	/**
	*@brief ��ȡ����ڼ��δ����ͼ(��1��ʼ)
	*/
	UInt32 GetDailyCount(UInt32 dungeonId) const;

	/**
	*@brief ��ȡ�����Ѿ����˼������ͼ(��0��ʼ)
	*/
	UInt32 GetDailyCountBySubtype(DungeonSubType subType) const;

	/**
	*@brief ��ȡ�ڼ��δ����ͼ
	*/
	UInt32 GetTotalCount(UInt32 dungeonId) const;

	/**
	*@brief ��ȡ�ڼ��δ����ͼ
	*/
	UInt32 GetRemainTimes(UInt32 dungeonId) const;

	/**
	*@brief �����Ҫ��¼ÿ�մ�Ĵ����ͼ�¼���������ǵڼ���
	*/
	UInt32 TryRecordDailyCount(UInt32 dungeonId);

	/**
	*@brief ͬ��ʣ�������world
	*/
	void SyncRemainTimes2World(UInt32 dungeonId) const;

	/**
	*@brief �����Ҫ��¼ÿ�ܴ�Ĵ����ͼ�¼���������ǵڼ���
	*/
	UInt32 TryRecordWeekCount(UInt32 dungeonId);

	/**
	*@brief ��鼤���̳���ʱ�������
	*/
	void CheckMallGiftActivateCondition(UInt32 canPlayMapRet, DungeonSubType subType);

	/**
	*@brief �������³�id
	*/
	UInt32 AdjustDungeonId(UInt32 dungeonId);

	/**
	*@brief ��ȡ���³���óɼ�
	*/
	UInt32 GetDungeonBestScore(UInt32 dungeonId);

	/**
	*@brief ��ȡ���˺Ž���ڼ��δ����ͼ(��1��ʼ)
	*/
	UInt32 GetAccountDailyCount(UInt32 dungeonId) const;
	UInt32 AddAccountDailyCount(UInt32 dungeonId);
	UInt32 GetAccountDailyRemainTimes(UInt32 dungeonId) const;

private:
    void RecordDungeonInfo(UInt32 dungeonId, LevelScore score, UInt32 usedTime);
    void RecordDungeonHardInfo(UInt32 dungeonId, DungeonHardType type);

    /**
    *@brief ���Խ�����һ�Ѷ�
    */
	void TryToUnlockNextHard(UInt32 dungeonId, LevelScore score, UInt16 beHitCount, UInt16 reviveCount);

    /**
    *@brief �ܷ����ĳһ�Ѷ�
    */
	bool CanUnlockThisHard(DungeonHardType type, LevelScore score, UInt16 beHitCount, UInt16 reviveCount);

	/**
	*@brief �Ƿ�����ؿ���������
	*/
	bool IsMatchDungeonOpenCond(UInt32 dungeonId, bool checkStoryTask = true);

    /**
    *@brief �Ƿ��Ѿ�����
    */
    bool IsDungeonOpened(UInt32 dungeonId);

    /**
    *@brief �Ƿ��Ѿ�������Ԩģʽ
    */
    bool IsDungeonHellModeOpened(UInt32 dungeonId);

    /**
    *@brief �Ƿ��Ѿ�����
    */
    bool IsDungeonOpenHellMode(UInt32 dungeonId);

	/**
	*@brief �����̳�ƣ�����
	*/
	void ActivateFatigue();

	/**
	*@brief �����̳���Ʊ���
	*/
	void ActivateTicket(DungeonSubType subType);

public:
    /**
    *@brief ͨ�����³�ID��ȡ��ͨ�ĵ��³�ID(���վ���ˢ���Ǹ����³�)
    */
    static UInt32 GetNormalDungeonID(UInt32 dungeonId);

    /**
    *@brief ͨ�����³�ID��ȡ��ͨ�Ѷȵĵ��³�ID
    */
    static UInt32 GetNormalHardDungeonID(UInt32 dungeonId);

    /**
    *@brief ͨ�����³�ID��ȡǰ�þ���ؿ�ID
    */
    static UInt32 GetPrevStoryDungeonID(UInt32 dungeonId);

    /**
    *@brief ͨ����ͨ�Ѷȵ��³�ID��ȡ��ͬ�Ѷȵĵ��³�ID
    */
    static UInt32 GetDungeonIDByHard(UInt32 normalHardDungeonId, DungeonHardType type);

public:
    /**
    *@brief ������������ͬ�����³����ݵı��롢����
    */
    void Input(Avalon::NetInputStream &stream);
    void Output(Avalon::NetOutputStream &stream);

    /**
    *@brief �����¿��ź͹رյĵ��³��б�
    */
    void _GenNewOpenDungeonList(std::vector<DungeonOpenInfo>& newOpenList, std::vector<UInt32>& newCloseList);

	/**
	*@breif �����½�ͨ��gm����
	*/
	void SetChapterPass(UInt8 chapter);

	void PassAllChapterDungeon();
private:
    //ӵ����
    Player*		                m_pOwner;

    Map2Dungeon                 m_dungeons;
    Map2DungeonHardInfo         m_dungeonHardInfoes;

    // ���ŵĵ��³��б�
    std::map<UInt32, DungeonOpenInfo>   m_openedDungeonList;
};

#endif

