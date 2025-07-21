#ifndef __DUNGEON_PLAYER_H__
#define __DUNGEON_PLAYER_H__

#include <CLDefine.h>
#include <CLObject.h>
#include <AvalonObjectPool.h>
#include <CLDropItemDefine.h>
#include <CLMatchDefine.h>
#include <CLDungeonDefine.h>
#include <set>

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

class DungeonPlayer : public CLObject
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

	inline void SetVipLvl(UInt8 viplvl) { m_vipLvl = viplvl; }
	inline UInt8 GetVipLvl() { return m_vipLvl; }

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

	//inline UInt8 GetExpAddition() const { return m_addition.expRate; }
	//inline UInt8 GetGoldAddition() const { return m_addition.goldRate; }
	//inline UInt8 GetVipGoldAddition() const { return m_addition.vipGoldRate; }
	//inline UInt8 GetGuildSkillAddition() const { return m_addition.guildSkillExpRate; }
	//inline UInt8 GetVipExpAddition() const { return m_addition.vipExpRate; }
	//inline UInt8 GetBuffExpAddition() const { return m_addition.buffExpRate; }
	//inline UInt8 GetMasterExpAddition() const { return m_addition.masterExpRate; }
	//inline UInt8 GetTaskDropNumAddition() const { return m_addition.taskDropNumRate; }
	//inline UInt16 GetPinkDropAddition() const { return m_addition.pinkDropRate; }

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
	inline void SetSpeedCheat() { m_speedCheat = true; }
	inline bool IsSpeedCheat() const { return m_speedCheat; }

	void AddActivityDoubleDropInfo(UInt32 activityMonsterId, std::set<UInt32>& dropItems, UInt32 doubleRate);
	UInt32 GetActivityDoubleDropRate(UInt32 activityMonsterId, UInt32 dropItem);

	bool HasGoldTitle() const;

	void AddDropOverMonster(UInt32 id) { m_dropOverMonsters.push_back(id); }
	const std::vector<UInt32>& GetDropOverMonsters() const { return m_dropOverMonsters; }

	void AddDropNumMonster(UInt32 id) { m_AddedDropNumMonsters.insert(id); }
	bool IsAddedDropNumMonster(UInt32 id) { return m_AddedDropNumMonsters.find(id) != m_AddedDropNumMonsters.end(); }

	inline bool IsVeteranReturn() const { return m_isVeteranReturn; }
	inline void SetVeteranReturn() { m_isVeteranReturn = true; }

	inline bool GetBoMonthCard() { return m_BoMonthCard; }
	inline void SetBoMonthCard() { m_BoMonthCard = true; }

	inline bool GetBoGiftRightCard() { return m_BoGiftRightCard; }
	inline void SetBoGiftRightCard() { m_BoGiftRightCard = true; }

	UInt32 GetExtra();

	void IncBufferItemUsedNum(UInt32 itemid);
	UInt32 GetBufferItemUsedNum(UInt32 itemid);

	bool IsRecordDailyDropNum() const { return m_isRecordDailyDropNum; }
	void SetRecordDailyDropNum() { m_isRecordDailyDropNum = true; }

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

private:    
	RacePlayerInfo			m_raceInfo;				// ս����Ҫ������
    UInt8                   m_occu;                 // ְҵID
    UInt16                  m_level;                // �ȼ�
	UInt8					m_vipLvl;				// vip�ȼ�
	UInt32					m_accId;
	UInt32					m_zoneId;
    bool                    m_isLeave;
    bool                    m_isDisconnect;
    bool                    m_hasRaceEnd;
	bool					m_hasGotRaceEndDrop;
    UInt32                  m_curAreaId;
	UInt32					m_enterAreaTime;
    DungeonAddition         m_addition;
    std::vector<UInt32>     m_gotRewardAreaIds;  // �Ѿ���ȡ�˾��������ID
    std::vector<UInt32>     m_enteredAreaIds;       // �����������ID
	std::set<UInt32>		m_clearedAreaIds;       // �Ѿ�����ķ���
    std::vector<UInt32>     m_taskList;             // �ؿ���ص������б�
    std::set<UInt32>        m_killedMonster;        // �Ѿ�ɱ���Ĺ�
    UInt32                  m_monsterExp;           // ɱ�ֻ�õľ���
	UInt32					m_dailyCount;			// ���մ���
	UInt32					m_totalCount;			// �ܴ���
	UInt16					m_reviveCount;			// �������
	bool					m_freeOpenGoldChest;	// ��Ѵ򿪻ƽ���
	UInt32					m_goldDropNum;			// ��õĽ�ҵ�������
	UInt32					m_bindGoldDropNum;		// �󶨽�ҵ�������
	LevelScore				m_Score;				// ս������
	bool					m_IsAssist;				// �Ƿ���ս
	bool					m_IsRacing;				// �Ƿ��Ѿ�����ս��
	bool					m_IsGenGuaranteeDrop;	// �Ƿ��Ѿ���������׵���

    // �ؿ���¼
    UInt32                  m_usedTime;             // ʹ��ʱ��
    UInt32                  m_remainHp;             // Hp
    UInt32                  m_remainMp;             // Mp

	VPlayerDropItem			m_bossDrop;				// boss����

	bool					m_cheat;				// �Ƿ��Ѿ�����

	UInt32					m_addedDropNum;			// ����������

	// ����䷭����Ϣ
	std::map<UInt32, ActivityMonsterDropAddition> m_activityDoubleDropInfo;
	bool					m_speedCheat;			// �Ƿ���ٹ�

	// �����������Ĺ���
	std::vector<UInt32>		m_dropOverMonsters;
	// �Ѿ���ӹ���������Ĺ���
	std::set<UInt32>		m_AddedDropNumMonsters;

	// �Ƿ��ǻع����
	bool					m_isVeteranReturn;

	// bufferҩʹ�ô���
	std::map<UInt32, UInt32>	m_BufferItemUsedNum;

	// �Ƿ��Ѿ���¼��ÿ�ձص�����
	bool					m_isRecordDailyDropNum;
	// �Ƿ����¿��û�
	bool                    m_BoMonthCard;
	// �Ƿ���������Ȩ���û�
	bool                    m_BoGiftRightCard;
};

typedef HashMap<ObjID_t, DungeonPlayer*> MapDungeonPlayer;

#endif
